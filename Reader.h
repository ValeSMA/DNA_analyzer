#ifndef __READER_H__
#define __READER_H__

#include <stdio.h>
#include <stdlib.h>

#define REFERENCE_FILE "./sequences/cerevisiae_processed.txt"
#define TEST_SEQ "texto"

/* Returns a pointer to a buffer with the reference sequence */
char* read_reference();

struct sequence {
    char *seq;
    int size;
};

struct sequence * read_seq_file(char *filename, int *nlines);

#endif