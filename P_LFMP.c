#include "P_LFMP.h"

// Global frequency table
unsigned int freq_tab[WCONT][WLEN+1] = {0};

int int_pow(int b, int e)
{
    int r = 1;
    for(int i=0; i<e; i++)
        r*=b;
    return r;
}

void fill_frequency_table(unsigned char *ref, int ref_size)
{
    unsigned char alphabet[] = {'A','C','G','T'};
    
    #if WLEN == 8
    unsigned int multi[] = {16384,4096,1024,256,64,16,4,1};
    #elif WLEN == 4
    unsigned int multi[] = {256,64,16,4,1};
    #endif

    for(int i=0; i<WLEN; i++)
    {
        for(int j=0; j<WCONT; j++)
        {
            freq_tab[j][i] = alphabet[(j/multi[i])%4];
        }
    }

    // Iterate words
    for(int i = 0; i < ref_size - WLEN; i++)
    {
        int row = 0;
        // Iterate characters
        for(int j=0; j < WLEN; j++)
        {
            switch(ref[i+j])
            {
                case 'A':
                    break;
                case 'C':
                    row += 1 * int_pow(4,WLEN-j-1);
                    break;
                case 'G':
                    row += 2 * int_pow(4,WLEN-j-1);
                    break;
                case 'T':
                    row += 3 * int_pow(4,WLEN-j-1);
                    break;
            }
        }
        freq_tab[row][WLEN]++;
    }
}

int preprocessing(unsigned char *pattern, unsigned int pattern_size)
{
    int min_value = INT32_MAX;
    int min_index = -1;

    for(int i=0; i < (pattern_size - WLEN); i++)
    {
        int row = 0;
        for(int j=0; j < WLEN; j++)
        {
            switch (pattern[i+j])
            {
                case 'A':
                    break;
                case 'C':
                    row += 1 * int_pow(4, WLEN -j -1);
                    break;
                case 'G':
                    row += 2 * int_pow(4, WLEN -j -1);
                    break;
                case 'T':
                    row += 3 * int_pow(4, WLEN -j -1);
                    break;
            }
        }
        
        if(freq_tab[row][WLEN] < min_value)
        {
            min_value = freq_tab[row][WLEN];
            min_index = i;
        }
    }
    
    return min_index;
}

void windowing(int min_index, unsigned char *text, unsigned int text_size, unsigned char *pattern, unsigned int pattern_size, struct index_tracker *windows)
{
    for(int i=min_index; i<text_size-(pattern_size-min_index); i+=1)
    {
        int equal = 1;
        for(int j=0; j<WLEN; j++)
        {
            if(text[i+j]!=pattern[min_index+j])
            {
                equal = 0;
                break;
            }
        }
        if(equal)
        {
            windows->idx[windows->count] = i - min_index;
            windows->count++;
        }
    }
}

void matching(unsigned char *text, unsigned char *pattern, unsigned int pattern_size, struct index_tracker *windows, struct index_tracker *matches)
{
    int num_match = 0;
    int k = pattern_size % WLEN;

    for(int i=0; i<windows->count; i++)
    {
        int s = windows->idx[i];
        int w = WLEN;

        int c = 0;
        for(c = 0; c<pattern_size; c+=w)
        {
            if(c > pattern_size - WLEN)
            {
                w = k;
            }
            
            int nequal = 0;
            for(int j=c; j<w; j++)
            {
                if(pattern[c] != text[s+c])
                    nequal = 1;
            }

            if(nequal)
                break;
        }

        if(c==pattern_size)
        {
            matches->idx[matches->count] = s;
            matches->count++;
        }
    }
}

void print_freq_tab()
{
    for(int i=0; i<WCONT; i++)
    {
        for(int j=0; j<WLEN; j++)
        {   
            if(freq_tab[i][WLEN]>0)
                printf("%c",freq_tab[i][j]);
        }
        if(freq_tab[i][WLEN]>0)
        {
            printf(" %d ---",freq_tab[i][WLEN]);
            printf("\n");
        }
    }
}

void p_windowing(int min_index, unsigned char *text, unsigned int text_size, unsigned char *pattern, unsigned int pattern_size, struct index_tracker *windows)
{
    #pragma omp parallel for
    for(int i=min_index; i<text_size-(pattern_size-min_index); i++)
    {
        int equal = 1;
        for(int j=0; j<WLEN; j++)
        {
            if(text[i+j]!=pattern[min_index+j])
            {
                equal = 0;
                break;
            }
        }
        if(equal)
        {
            #pragma omp critical
            {
                windows->idx[windows->count] = i - min_index;
                windows->count++;
            }
        }
    }
}