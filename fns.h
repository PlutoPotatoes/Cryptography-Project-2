/*
 * Project 2 of COMP389: Cryptography
 * Function: Header File of fns.c
 * File Name: fns.h
 */

#ifndef FNS_H
#define FNS_H

/*
 * A simple stream cipher utilizing MD5 Message-Digest Algorithm
 * Parameters:
 * p: pointer to locate the pass phrase
 * l: number of bytes to output
 * fp: file pointer pointing to the specified temporary file or stdout
 *
 * Return value:
 * on success, return 0; else return -1
 */
int stream(char *p, int l, FILE *fp);

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
int encrypt(char *p, char *out, FILE *fp);

// writes encrypted PBM data to a file
int write_pbm(const char *filename, int width, int height, int *pixels);
/*
 * Merge two input encrypted PBM files
 * Parameters:
 * fp1: file pointer pointing to the specified input file #1
 * fp2: file pointer pointing to the specified input file #2
 *
 * Return value:
 * on success, return 0; else return -1
 */
int merge(FILE *fp1, FILE *fp2);

/*
 * Decrypt either stdin or a specified merged PBM input file
 * Parameters:
 * fp: file pointer pointing to either the specified input file or stdin
 *
 * Return value:
 * on success, return 0; else return -1
 */


int decrypt(FILE *fp) {
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

    // Flush last byte if needed
    // if (bit_pos != 7) putchar(out_byte);

    // free(row_buf);
    // return 0;
}




#endif
