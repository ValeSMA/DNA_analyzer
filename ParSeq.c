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
    
    fill_frequency_table(reference_seq,refsize);

    char filename[255] = "\0";
    open_connection();

    while(1)
    {
        request(filename);
        printf("%s\n",filename);
        seqs = read_seq_file(filename,&nseqs);
        printf("%d\n",nseqs);

        if(filename) {


        clock_t g;
        g = clock();

        struct index_tracker windows = {NULL,0};
        struct index_tracker matches = {NULL,0};

        int total_matches = 0;

        long long total = 0;
        long long ref_total = refsize;
        int no_matches = 0;

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
            p_windowing(min_index,reference_seq,refsize,pattern,pattern_size,&windows);
            //t = clock()-t;
            //printf("Windowing time: %f\n",(double)t/(CLOCKS_PER_SEC / 1000));

            //t = clock();
            matching(reference_seq,pattern,refsize,pattern_size,&windows,&matches);
            //t = clock()-t;
            //printf("Matching time: %f\n",(double)t/(CLOCKS_PER_SEC / 1000));

            for(int j=0; j<matches.count; j++)
            {
                /*for(int k=0; k<pattern_size; k++)
                    printf("%c", reference_seq[matches.idx[j] + k]);*/
                printf("Sequence[%d]: Found a match at index %d\n", i, matches.idx[j]);
                total += pattern_size+1;
            }

            if(matches.count>0)
            {
                total_matches++;
            }
            else 
            {
                no_matches++;
            }
            
            free(windows.idx);
            free(matches.idx);

            windows.count=0;
            matches.count=0;
        }
        
        g = clock() - g;
        printf("Total time: %f miliseconds\n",(double)g/(CLOCKS_PER_SEC / 1000));
        printf("%d mapped sequences\n",total_matches);
        printf("%d not mapped sequences\n",no_matches);
        printf("%lld Total characters matched\n", total);
        printf("%lld percentage of reference covered\n", (total * 100 /ref_total));
        for(int i=0; i<nseqs; i++)
            free(seqs[i].seq);
        free(seqs);
        }
    }
    free(reference_seq);
}