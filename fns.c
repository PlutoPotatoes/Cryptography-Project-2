/*
 * Project 2 of COMP389: Cryptography
 * Function: stream, encrypt, merge, and decrypt
 * File Name: fns.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <openssl/md5.h> 



int write_pbm(const char *filename, int width, int height, int *pixels) {
    FILE *fp = fopen(filename, "wb");  // binary mode!
    if (!fp) {
        perror("Failed to open output file");
        return -1;
    }

    fprintf(fp, "P4\n%d %d\n", width, height);

    int row_bytes = (width + 7) / 8;
    unsigned char byte = 0;

    for (int y = 0; y < height; y++) {
        int bit_index = 0;
        byte = 0;
        for (int x = 0; x < width; x++) {
            byte = (byte << 1) | (pixels[y * width + x] ? 0 : 1);  // 1=black, 0=white (P4)
            bit_index++;

            if (bit_index == 8) {
                fputc(byte, fp);
                byte = 0;
                bit_index = 0;
            }
        }
        if (bit_index > 0) {  // pad remaining bits
            byte <<= (8 - bit_index);
            fputc(byte, fp);
        }
    }

    fclose(fp);
    return 0;
}



int stream(char *p, int l, FILE *fp){
  if (!p || !fp || l < 0) return -1; // basic safety check

  unsigned char md5_buf[MD5_DIGEST_LENGTH]; // holds current MD5 hash (16 bytes)

  // Initialize the MD5 state from the passphrase
  MD5((unsigned char *)p, strlen(p), md5_buf);

  int counter = 0;
  int written = 0;

  // Compute the size of the temporary buffer
  int passphrase_len = strlen(p);
  int temp_len = MD5_DIGEST_LENGTH + 2 + passphrase_len; // md5 + "00" + passphrase
  char *s = (char *)malloc(temp_len + 1); // +1 for null terminator
  if (!s) return -1;

  while (written < l) {
      // Build string: [md5_buf][2-digit counter][passphrase]
      sprintf(&s[MD5_DIGEST_LENGTH], "%02d%s", counter, p); // write to s[16..]
      memcpy(s, md5_buf, MD5_DIGEST_LENGTH);                // write md5_buf to s[0..15]

      // Hash the whole buffer
      MD5((unsigned char *)s, temp_len, md5_buf); // md5_buf is updated

      // Write first 8 bytes of new md5 to output
      int bytes_to_write = (l - written < 8) ? (l - written) : 8;
      if (fwrite(md5_buf, 1, bytes_to_write, fp) != bytes_to_write) {
          free(s);
          return -1;
      }

      written += bytes_to_write;
      counter = (counter + 1) % 100;
  }

  free(s);
  return 0; // FIXME
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>

int encrypt(char *p, char *out, FILE *fp) {
    char magic[3];
    int width, height;

    // Read and check PBM magic number
    if (fscanf(fp, "%2s", magic) != 1 || strcmp(magic, "P4") != 0) {
        fprintf(stderr, "Invalid PBM magic\n");
        return -1;
    }

    // Read width and height
    if (fscanf(fp, "%d %d", &width, &height) != 2) {
        fprintf(stderr, "Failed to read PBM width and height\n");
        return -1;
    }
    fgetc(fp); // skip newline after header

    int row_bytes = (width + 7) / 8;
    unsigned char *row_buf = malloc(row_bytes);
    if (!row_buf) return -1;

    int expW = width * 2, expH = height * 2;
    int exp_row_bytes = (expW + 7) / 8;

    unsigned char *packed1 = calloc(expH * exp_row_bytes, 1);
    unsigned char *packed2 = calloc(expH * exp_row_bytes, 1);
    if (!packed1 || !packed2) return -1;

    FILE *key_fp = tmpfile();
    if (!key_fp) return -1;
    stream(p, width * height, key_fp);
    rewind(key_fp);

    unsigned char key_byte = 0;
    int bits_left = 0;

    for (int r = 0; r < height; ++r) {
        if (fread(row_buf, 1, row_bytes, fp) != row_bytes) {
            fprintf(stderr, "Failed to read row\n");
            free(row_buf); free(packed1); free(packed2);
            fclose(key_fp);
            return -1;
        }

        for (int c = 0; c < width; ++c) {
            int byte_idx = c / 8;
            int bit_idx = 7 - (c % 8);
            int pixel = (row_buf[byte_idx] >> bit_idx) & 1;

            if (bits_left == 0) {
              if (fread(&key_byte, 1, 1, key_fp) != 1) {
                fprintf(stderr, "Failed to read key byte\n");
                
            }
            bits_left = 8;
            
                bits_left = 8;
            }
            int key_bit = (key_byte >> 7) & 1;
            key_byte <<= 1;
            bits_left--;

            // Calculate expanded positions
            int er = r * 2, ec = c * 2;

            // Define patterns
            unsigned char patA[2][2] = {{1,0},{0,1}};
            unsigned char patB[2][2] = {{0,1},{1,0}};

            unsigned char (*pat1)[2], (*pat2)[2];
            if (pixel == 0) { // white
                pat1 = pat2 = (key_bit) ? patA : patB;
            } else { // black
                pat1 = (key_bit) ? patA : patB;
                pat2 = (key_bit) ? patB : patA;
            }

            // Set bits in packed1 and packed2
            for (int dr = 0; dr < 2; ++dr) {
                for (int dc = 0; dc < 2; ++dc) {
                    if (pat1[dr][dc]) 
                        packed1[(er + dr) * exp_row_bytes + (ec + dc)/8] |= (1 << (7 - (ec + dc)%8));
                    if (pat2[dr][dc])
                        packed2[(er + dr) * exp_row_bytes + (ec + dc)/8] |= (1 << (7 - (ec + dc)%8));
                }
            }
        }
    }

    // Write to output files
    char out1[256], out2[256];
    snprintf(out1, sizeof(out1), "%s.1.pbm", out);
    snprintf(out2, sizeof(out2), "%s.2.pbm", out);

    FILE *fp1 = fopen(out1, "wb");
    FILE *fp2 = fopen(out2, "wb");
    if (!fp1 || !fp2) return -1;

    fprintf(fp1, "P4\n%d %d\n", expW, expH);
    fprintf(fp2, "P4\n%d %d\n", expW, expH);

    fwrite(packed1, 1, exp_row_bytes * expH, fp1);
    fwrite(packed2, 1, exp_row_bytes * expH, fp2);

    fclose(fp1);
    fclose(fp2);
    fclose(key_fp);
    free(row_buf);
    free(packed1);
    free(packed2);

    return 0;
}






/*
 * Merge two input encrypted PBM files
 * Parameters:
 * fp1: file pointer pointing to the specified input file #1
 * fp2: file pointer pointing to the specified input file #2
 *
 * Return value:
 * on success, return 0; else return -1
 */
int merge(FILE *fp1, FILE *fp2, char *outName){
  int width1, height1;
  if (fscanf(fp1, "P4\n%d %d\n", &width1, &height1) != 2) {
      fprintf(stderr, "Invalid PBM header\n");
      return -1;
  }
  int width2, height2;
  if (fscanf(fp2, "P4\n%d %d\n", &width2, &height2) != 2) {
      fprintf(stderr, "Invalid PBM header\n");
      return -1;
  }
  if(width1 != width2 || height1 != height2){
    printf(stderr, "Input PBM files are not the same size");
  }
  
  FILE *out = fopen(outName, "w");
  if (!out) {
      perror("Failed to open output file");
      return -1;
  }
  fprintf(out, "P4\n%d %d\n", width1, height1);
  for(int i = 1; i<=width1*height1; i++){
    int pix1;
    int pix2;
    fscanf(fp1, "%1d", &pix1);
    fscanf(fp2, "%1d", &pix2);
    if(pix1 == pix2){
      fprintf(out, "%d", 0);
    }else{
      fprintf(out, "%d", 1);
    }
  }

  fclose(out);
  return 0;


}

/*
 * Decrypt either stdin or a specified merged PBM input file
 * Parameters:
 * fp: file pointer pointing to either the specified input file or stdin
 *
 * Return value:
 * on success, return 0; else return -1
 */
int decrypt(FILE *fp){
  char magic[3];
  int width, height;

  // Read magic number
  if (fscanf(fp, "%2s", magic) != 1 || strcmp(magic, "P4") != 0) {
      fprintf(stderr, "Invalid PBM file.\n");
      return -1;
  }

  // Read width and height
  if (fscanf(fp, "%d %d", &width, &height) != 2) {
      fprintf(stderr, "Failed to read width and height.\n");
      return -1;
  }
  fgetc(fp); // skip newline after header

  int row_bytes = (width + 7) / 8;
  unsigned char *row_buf = malloc(row_bytes);
  if (!row_buf) return -1;

  // Prepare output PBM header (half size)
  printf("P4\n%d %d\n", width / 2, height / 2);

  // Process rows two at a time
  for (int row = 0; row < height; row += 2) {
      unsigned char *row_buf2 = malloc(row_bytes);
      if (!row_buf2) return -1;

      if (fread(row_buf, 1, row_bytes, fp) != row_bytes) break;
      if (fread(row_buf2, 1, row_bytes, fp) != row_bytes) break;

      for (int col = 0; col < width; col += 2) {
          int byte_index1 = col / 8;
          int bit_index1 = 7 - (col % 8);
          int byte_index2 = (col + 1) / 8;
          int bit_index2 = 7 - ((col + 1) % 8);

          int top_left = (row_buf[byte_index1] >> bit_index1) & 1;
          int top_right = (row_buf[byte_index2] >> bit_index2) & 1;
          int bottom_left = (row_buf2[byte_index1] >> bit_index1) & 1;
          int bottom_right = (row_buf2[byte_index2] >> bit_index2) & 1;

          int black_count = top_left + top_right + bottom_left + bottom_right;

          int pixel_value = (black_count >= 3) ? 1 : 0; // majority black → black pixel

          // Write to stdout (1 pixel at a time → pack 8 bits into a byte)
          static int bit_pos = 7;
          static unsigned char out_byte = 0;
          if (pixel_value) out_byte |= (1 << bit_pos);
          bit_pos--;
          if (bit_pos < 0) {
              putchar(out_byte);
              out_byte = 0;
              bit_pos = 7;
          }
      }

      free(row_buf2);
  }

}

