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

  if(strcmp(argv[1], "stream") == 0){
    FILE *key = fopen(argv[4], "wb");
    char *password = argv[2];
    int len = (int)argv[3];
    stream(password, len, key);
    fclose(key);
  }else if (strcmp(argv[1], "encrypt") == 0)
  {
    FILE *input = fopen(argv[1], "rb");
    char *password = argv[2];
    FILE *cypher1 = fopen(argv[2], "rb+");
    FILE *cypher2 = fopen(argv[3], "rb+");
    encrypt(password, cypher1, input);
    /* code */
  }else if (strcmp(argv[1], "merge") == 0)
  {
    FILE *cypher1 = fopen(argv[2], "rb+");
    FILE *cypher2 = fopen(argv[3], "rb+");
    char *outName = argv[4];
    merge(cypher1, cypher2, outName);
    /* code */
  }else if (strcmp(argv[1], "decrypt") ==0)
  {
    /* code */
  }
  

  return 0;
//=============================
  // usage();
  // int 
  // return 0;
}