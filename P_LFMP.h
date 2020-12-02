#ifndef __P_LFMP_H__
#define __P_LFMP_H__

// Paralell Least Frequency Pattern Matching 
/*****************************************************
    Paralell version of the LFMP algorithm originally 
    proposed by Neamatollahi, Hadi and Naghibzadeh in

    "Simple and efficient pattern matching algorithms
    for biological sequences"
    
    IEEE https://ieeexplore.ieee.org/document/8967097
******************************************************/

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <omp.h>

// 32/64 bit defs
#if UINTPTR_MAX == 0xffffffff
#define WLEN 4
#define WCONT 256
#elif UINTPTR_MAX == 0xffffffffffffffff
#define WLEN 8
#define WCONT 65536
#endif

// Support array
struct index_tracker {
    unsigned int *idx;
    unsigned int count;
};

// LFMP Funcs
/* Fills the global frequency table with the frequencies of each word in the reference string */
void fill_frequency_table(unsigned char *ref, int ref_size);

/* Finds the least frequent word in the pattern */
int preprocessing(unsigned char *pattern, unsigned int pattern_size);

/* Finds the windows of search in which matching will be performed */
void windowing(int min_index, unsigned char *text, unsigned int text_size, 
        unsigned char *pattern, unsigned int pattern_size, struct index_tracker *windows);

/* Prints the first indexes for all ocurrences of pattern in text */
void matching(unsigned char *text, unsigned char *pattern, unsigned int pattern_size,
         struct index_tracker *windows, struct index_tracker *matches);

/* Prints the global frequency table */
void print_freq_tab();

/* Performs paralell window construction operation */
void p_windowing(int min_index, unsigned char *text, unsigned int text_size, unsigned char *pattern, unsigned int pattern_size, struct index_tracker *windows);

#endif