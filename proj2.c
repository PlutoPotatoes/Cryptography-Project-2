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
      return 0;
  }

  else if (strcmp(argv[1], "encrypt") == 0) {
      // placeholder: you will implement this
  }

  else if (strcmp(argv[1], "merge") == 0) {
      // placeholder: you will implement this
  }

  else if (strcmp(argv[1], "decrypt") == 0) {
      // placeholder: you will implement this
  }

  else {
      usage();
  }

  return 0;
}