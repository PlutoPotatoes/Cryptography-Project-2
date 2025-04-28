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


/*
 * a simple stream cipher utilizing MD5 Message-Digest Algorithm
 * Parameters:
 * p: pointer to locate the pass phrase
 * l: number of bytes to output
 * fp: file pointer pointing to the specified temporary file or stdout
 *
 * Return value:
 * on success, return 0; else return -1
 */
// int stream(char *p, int l, FILE *fp){
//   if (!p || !fp || l < 0) return -1; // basic safety check

//   unsigned char md5_buf[MD5_DIGEST_LENGTH]; // holds current MD5 hash (16 bytes)

//   // Initialize the MD5 state from the passphrase
//   MD5((unsigned char *)p, strlen(p), md5_buf);

//   int counter = 0;
//   int written = 0;

//   // Compute the size of the temporary buffer
//   int passphrase_len = strlen(p);
//   int temp_len = MD5_DIGEST_LENGTH + 2 + passphrase_len; // md5 + "00" + passphrase
//   char *s = (char *)malloc(temp_len + 1); // +1 for null terminator
//   if (!s) return -1;

//   while (written < l) {
//       // Build string: [md5_buf][2-digit counter][passphrase]
//       sprintf(&s[MD5_DIGEST_LENGTH], "%02d%s", counter, p); // write to s[16..]
//       memcpy(s, md5_buf, MD5_DIGEST_LENGTH);                // write md5_buf to s[0..15]

//       // Hash the whole buffer
//       MD5((unsigned char *)s, temp_len, md5_buf); // md5_buf is updated

//       // Write first 8 bytes of new md5 to output
//       int bytes_to_write = (l - written < 8) ? (l - written) : 8;
//       if (fwrite(md5_buf, 1, bytes_to_write, fp) != bytes_to_write) {
//           free(s);
//           return -1;
//       }

//       written += bytes_to_write;
//       counter = (counter + 1) % 100;
//   }

//   free(s);
//   return 0; // FIXME
// }

int write_pbm(const char *filename, int width, int height, int *pixels) {
  FILE *fp = fopen(filename, "w");
  if (!fp) {
      perror("Failed to open output file");
      return -1;
  }
  fprintf(fp, "P1\n%d %d\n", width, height);
  for (int i = 0; i < width * height; i++) {
      fprintf(fp, "%d ", pixels[i]);
      if ((i + 1) % width == 0) {
          fprintf(fp, "\n");
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

/*
 * Encrypts an input PBM file with the simple stream cipher based on 4x data expension visual cryptography by Naor and Shamir
 * Parameters:
 * p: pointer to locate the pass phrase
 * out: pointer to locate the output filename string
 * fp: file pointer pointing to the specified input file or stdin
 *
 * Return value:
 * on success, return 0; else return -1
 */
int encrypt(char *p, char *out, FILE *fp){
  /*
  scan file header for width and height:
    P4
    WidthInt HeightInt
  */
  int width, height;
  if (fscanf(fp, "P4\n%d %d\n", &width, &height) != 2) {
      fprintf(stderr, "Invalid PBM header\n");
      return -1;
  }

  //create array to hold pixels
  int *pixels = malloc(width * height * sizeof(int));

  //get each pixel from the PBM file 
  for (int i = 0; i < width * height; i++) {
      if (fscanf(fp, "%1d", &pixels[i]) != 1) {
          fprintf(stderr, "Why did you pass me invalid pixels?? cmon man\n");
          free(pixels);
          return -1;
      }
  }

  // create each half of the key
  // size*4 because of the 4x expansion factor
  int *share1 = malloc(4 * width * height * sizeof(int));
  int *share2 = malloc(4 * width * height * sizeof(int));
  int expandedWidth = width*2;


  for (int i = 0; i < width * height; i++) {
      int shareIndex = i*2;
      switch(pixels[i]){
        //white pixel, check key and write to shares
        case 0:
          if(p[i]==1){
            //write share1 and share 2 to the same values
            //XOR will reveal a white pixel
            share1[shareIndex], share1[shareIndex+expandedWidth+1] = 1;
            share1[shareIndex+1], share1[shareIndex+expandedWidth] = 0;
            share2[shareIndex], share2[shareIndex+expandedWidth+1] = 1;
            share2[shareIndex+1], share2[shareIndex+expandedWidth] = 0;
          }else{
            //write share1 and share 2 to the same values
            //XOR will reveal a white pixel
            share1[shareIndex], share1[shareIndex+expandedWidth+1] = 0;
            share1[shareIndex+1], share1[shareIndex+expandedWidth] = 1;
            share2[shareIndex], share2[shareIndex+expandedWidth+1] = 0;
            share2[shareIndex+1], share2[shareIndex+expandedWidth] = 1;
          }
          break;
        //black pixel, check key and write to shares
        case 1:
          if(p[i]==1){
            //write share1 and share 2 to opposite values
            //XOR will reveal a black pixel
            share1[shareIndex], share1[shareIndex+expandedWidth+1] = 1;
            share1[shareIndex+1], share1[shareIndex+expandedWidth] = 0;
            share2[shareIndex], share2[shareIndex+expandedWidth+1] = 0;
            share2[shareIndex+1], share2[shareIndex+expandedWidth] = 1;
          }else{
            //write share1 and share 2 to opposite values
            //XOR will reveal a black pixel
            share1[shareIndex], share1[shareIndex+expandedWidth+1] = 0;
            share1[shareIndex+1], share1[shareIndex+expandedWidth] = 1;
            share2[shareIndex], share2[shareIndex+expandedWidth+1] = 1;
            share2[shareIndex+1], share2[shareIndex+expandedWidth] = 0;
          }
          break;

      }
  }

  // Write the shares to output files
  // FIXME DO i need 2 output files??
  if (write_pbm(out, width * 2, height * 2, share1) == -1 ||
      write_pbm(out, width * 2, height * 2, share2) == -1) {
      free(pixels);
      free(share1);
      free(share2);
      fprintf(stderr, "Write to PBM Failed");
      return -1;
  }

  free(pixels);
  free(share1);
  free(share2);
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
int merge(FILE *fp1, FILE *fp2){
  return 0; // FIXME
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
   return 0; // FIXME
}

