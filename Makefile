#
#  Project 2 of COMP389: Cryptography
#  Function: Makefile
#  File Name: Makefile
#

CC = gcc
CFLAGS = -Wall
RM = rm -f
TAR = gtar czvf proj2.tar.gz
SOURCE = proj2.c fns.c fns.h
SUPPORT = Makefile README
OBJS=proj2.o fns.o

default: all

all: proj2

proj2: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -lm -o proj2 -L/c/OpenSSL/lib -lssl -lcrypto

fns.o: fns.h
	$(CC) -c fns.c $(CFLAGS) -I/c/OpenSSL/include

clean: 
	$(RM) *.o proj2 *~

tar: 
	$(TAR) $(SOURCE) $(SUPPORT)

