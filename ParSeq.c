#include "Reader.h"
#include "P_LFMP.h"
#include "client.h"
#include <time.h>

extern unsigned int freq_tab[WCONT][WLEN+1];

void main()
{
    int refsize;
    char *reference_seq = read_reference(&refsize);
    struct sequence *seqs;
    int nseqs = 0;

    char filename[255];

    open_connection();
    request(filename);
    printf("%s",filename);
    
    seqs = read_seq_file(filename,&nseqs);
    fill_frequency_table(reference_seq,refsize);

    clock_t g;
    g = clock();

    struct index_tracker windows = {NULL,0};
    struct index_tracker matches = {NULL,0};

    int total_matches = 0;
    for(int i=0; i<nseqs; i++)
    {
        clock_t t;    
        windows.idx = (int*)malloc(sizeof(int)*WCONT);
        matches.idx = (int*)malloc(sizeof(int)*WCONT);

        unsigned char *pattern = seqs[i].seq;
        unsigned int pattern_size = seqs[i].size;
        unsigned int min_index;

        //t = clock();
        min_index = preprocessing(pattern, pattern_size);
        //t = clock()-t;
        //printf("Preprocessing time: %f miliseconds\n",(double)t/(CLOCKS_PER_SEC / 1000));

        //printf("min index %d\n",min_index);
        
        //t = clock();
        windowing(min_index,reference_seq,refsize,pattern,pattern_size,&windows);
        //t = clock()-t;
        //printf("Windowing time: %f\n",(double)t/(CLOCKS_PER_SEC / 1000));

        //t = clock();
        matching(reference_seq,pattern,pattern_size,&windows,&matches);
        //t = clock()-t;
        //printf("Matching time: %f\n",(double)t/(CLOCKS_PER_SEC / 1000));

        for(int i=0; i<matches.count; i++)
            printf("Found a match at index %d\n", matches.idx[i]);

        if(matches.count>0)
        {
            total_matches++;
        }
        
        free(windows.idx);
        free(matches.idx);

        windows.count=0;
        matches.count=0;
    }
    
    g = clock() - g;
    printf("Total time: %f miliseconds\n",(double)g/(CLOCKS_PER_SEC / 1000));
    printf("%d mapped sequences\n",total_matches);

    for(int i=0; i<nseqs; i++)
        free(seqs[i].seq);
    free(seqs);
    free(reference_seq);
}