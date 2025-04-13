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
  char md5_buf[MD5_DIGEST_LENGTH];
  int len = strlen(p) + 2 + MD5_DIGEST_LENGTH;
  char *s = malloc(len+1);
  int i=0;
  MD5(p, strlen(p), md5_buf);
  for (;;) {
    sprintf(&s[MD5_DIGEST_LENGTH],"%02d%s",i, p);
    memcpy(s, md5_buf, MD5_DIGEST_LENGTH);
    MD5(s, len, md5_buf);
    fwrite(md5_buf, 1, sizeof(md5_buf)>>1, stdout);
    if (++i == 100) i = 0;
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

