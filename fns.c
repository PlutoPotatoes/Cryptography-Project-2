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
  return 0; // FIXME
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

