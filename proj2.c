/*
 * Project 2 of COMP389: Cryptography
 * Function: commandline parsing and file reading
 * File Name: proj2.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fns.h"

void usage(){
  printf("The commandline syntax for proj 2 is as follows:\n");
  printf("    proj stream -p=pphrase -l=len\n");
  printf("    proj encrypt -p=pphrase -out=name [pbmfile]\n");
  printf("    proj merge pbmfile1 pbmfile2\n");
  printf("    proj decrypt [pbmfile]\n");
  exit(1);
}


int main(int argc, char *argv[]) {
  if (argc < 2) usage();

  if (strcmp(argv[1], "stream") == 0) {
      char *p = NULL;
      int l = -1;
      for (int i = 2; i < argc; ++i) {
          if (strncmp(argv[i], "-p=", 3) == 0) p = argv[i] + 3;
          else if (strncmp(argv[i], "-l=", 3) == 0) l = atoi(argv[i] + 3);
      }
      if (!p || l <= 0) usage();
      if (stream(p, l, stdout) != 0) {
          fprintf(stderr, "Stream generation failed.\n");
          return 1;
      }
  }

  else if (strcmp(argv[1], "encrypt") == 0) {
      char *p = NULL, *out = NULL;
      FILE *input_fp = stdin;
      for (int i = 2; i < argc; ++i) {
          if (strncmp(argv[i], "-p=", 3) == 0) p = argv[i] + 3;
          else if (strncmp(argv[i], "-out=", 5) == 0) out = argv[i] + 5;
          else {
              input_fp = fopen(argv[i], "rb");
              if (!input_fp) { perror("Failed to open input file"); return 1; }
          }
      }
      if (!p || !out) usage();
      if (encrypt(p, out, input_fp) != 0) {
          fprintf(stderr, "Encryption failed.\n");
          return 1;
      }
      if (input_fp != stdin) fclose(input_fp);
  }

  else if (strcmp(argv[1], "merge") == 0) {
      if (argc < 5) usage();
      FILE *fp1 = fopen(argv[2], "rb");
      FILE *fp2 = fopen(argv[3], "rb");
      char *outName = argv[4];
      if (!fp1 || !fp2) { perror("Failed to open input files"); return 1; }
      if (merge(fp1, fp2, outName) != 0) {
          fprintf(stderr, "Merge failed.\n");
          return 1;
      }
      fclose(fp1);
      fclose(fp2);
  }

  else if (strcmp(argv[1], "decrypt") == 0) {
      FILE *input_fp = stdin;
      if (argc >= 3) {
          input_fp = fopen(argv[2], "rb");
          if (!input_fp) { perror("Failed to open input file"); return 1; }
      }
      if (decrypt(input_fp) != 0) {
          fprintf(stderr, "Decryption failed.\n");
          return 1;
      }
      if (input_fp != stdin) fclose(input_fp);
  }

  else usage();

  return 0;
}