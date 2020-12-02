#include "Reader.h"
#include <string.h>

char * read_reference(int *rsize)
{
    char *reference = NULL;
    
    FILE *fp;
    long lsize;

    fp = fopen(REFERENCE_FILE,"r");

    if(!fp)
    { 
        perror(REFERENCE_FILE);
        exit(1);
    }

    fseek(fp, 0L, SEEK_END);
    lsize = ftell(fp);
    rewind(fp);

    reference = (char*)malloc(sizeof(char)*(lsize+1));
    *rsize = lsize;

    if(!reference)
    {
        fclose(fp);
        fputs("Reference memory allocaction fails", stderr);
        exit(1);
    }

    if(fread(reference, lsize, 1, fp)!=1)
    {
        fclose(fp);
        free(reference);
        fputs("Entire reference read fails", stderr);
        exit(1);
    }

    fclose(fp);
    return reference;
}

struct sequence * read_seq_file(char *filename, int *nlines)
{
    FILE *fp;
    
    int count = 0;
    char c;

    char prefixes[255]="./sequences/";

    fp = fopen(strcat(prefixes,filename), "r");

    if(!fp)
    {
        perror(filename);
        exit(1);
    }

    for(c=getc(fp); c!= EOF; c=getc(fp))
    {
        
        if(c=='\n')
            count++;
    }

    
    rewind(fp);
    fseek(fp,0,SEEK_SET);

    struct sequence *seqs = (struct sequence *)malloc(sizeof(struct sequence) * count+1);
    char *line=NULL;

    
    int i=0;
    seqs[i].seq=(char*)malloc(sizeof(char)*20000);
    while(fgets(seqs[i].seq, 20000, fp))
    {
        seqs[i].size=strlen(seqs[i].seq)-2;
        i++;
        seqs[i].seq=(char*)malloc(sizeof(char)*20000);
    }

    fclose(fp);
    *nlines = count;
    return seqs;
}