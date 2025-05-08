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


int main(int argc, char *argv[]){
  char *passphrase = "test123";
  int num_bytes = 64;  // Number of bytes to generate

  FILE *input = fopen(argv[1], "rb");
  FILE *cypher1 = fopen(argv[2], "rb+");
  FILE *cypher2 = fopen(argv[3], "rb+");
  FILE *fp = fopen("stream_output.bin", "wb");

  fclose(fp);
  printf("Stream output written to stream_output.bin\n");
  return 0;
//=============================
  // usage();
  // int 
  // return 0;
}