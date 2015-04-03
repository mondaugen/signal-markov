#include <stdlib.h> 
#include <stdio.h> 
#include <string.h> 
#include <stdint.h> 
#include "stattree.h" 

/* Number of uint8_ts to read into string */
#define STRING_LEN 13 


int main (int argc, char**argv)
{
    if (argc != 2) {
        fprintf(stderr,"Useage: %s num-samples-to-generate\n",argv[0]);
        exit(-1);
    }

    uint8_t buf[STRING_LEN];
    memset(buf,0,STRING_LEN);
    uint8_t look;
    StatTreeNode *top = NULL;
    while (fread(&look, sizeof(uint8_t), 1, stdin) == 1) {
        StatTreeNode *needle = StatTreeNode_new(STRING_LEN);
        memcpy(needle->data,buf,STRING_LEN);
        if (top == NULL) {
            needle->probs[look] += 1;
            needle->seen += 1;
            top = needle;
        } else {
            StatTreeNode *where = StatTreeNode_search(top,needle);
            if (where) {
                where->probs[look] += 1;
                where->seen += 1;
                free(needle);
            } else {
                needle->probs[look] += 1;
                needle->seen += 1;
                StatTreeNode_insert(top,needle);
            }
        }
        /* shift over contents of buf */
        size_t i;
        for (i = 0; i < (STRING_LEN - 1); i++) {
            buf[i] = buf[i+1];
        }
        buf[i] = look;
    }
    /* do accumulation to get probability densities */
    StatTreeNode_apply(top,StatTreeNode_accum);
    /* for now just choose top string, later choose random staring string */
    size_t i;
    size_t len = strtoul(argv[1],NULL,10);
    StatTreeNode *tmp = StatTreeNode_new(STRING_LEN);
    memset(buf,0,STRING_LEN);
    for (i = 0; i < len; i++) {
        StatTreeNode *current;
        memcpy(tmp->data,buf,STRING_LEN);
        int n;
        current = StatTreeNode_search(top,tmp);
        if (!current) {
            fprintf(stderr,"No probabilities for string %s. Aborting.\n", buf);
            exit(-1);
        }
        int out = StatTreeNode_weightedRandom(current);
        printf("%c",out);
        /* shift over contents of buf */
        int j;
        for (j = 0; j < (STRING_LEN - 1); j++) {
            buf[j] = buf[j+1];
        }
        buf[j] = out;
    }
//    char out = EOF;
//    fwrite(&out, sizeof(char), 1, stdout);

    exit(0);
}

