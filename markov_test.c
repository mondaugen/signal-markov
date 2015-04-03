#include <stdlib.h> 
#include <stdio.h> 
#include <string.h> 
#include <stdint.h> 
#include "stattree.h" 

/* Number of uint8_ts to read into string */
#define STRING_LEN 5
#define OUTPUT_LEN 10 


int main ()
{
    char input[]  = "abcdefghijklmnopqrstuvwxyz";
    char *cptr = input;
    uint8_t buf[STRING_LEN];
    memset(buf,0,STRING_LEN);
    uint8_t look;
    StatTreeNode *top = NULL;
    while ((look = *(cptr++)) != '\0') {
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
    size_t len = OUTPUT_LEN;
    char output[OUTPUT_LEN+1];
    output[OUTPUT_LEN] = '\0';
    cptr = output;
    StatTreeNode *tmp = StatTreeNode_new(STRING_LEN);
    memset(buf,0,STRING_LEN);
    size_t i;
    for (i = 0; i < len; i++) {
        StatTreeNode *current;
        memcpy(tmp->data,buf,STRING_LEN);
        current = StatTreeNode_search(top,tmp);
        if (!current) {
            fprintf(stderr,"Current NULL, WTF?\n");
            exit(-1);
        }
        int out = StatTreeNode_weightedRandom(current);
        *(cptr++) = out;
        /* shift over contents of buf */
        int j;
        for (j = 0; j < (STRING_LEN - 1); j++) {
            buf[j] = buf[j+1];
        }
        buf[j] = out;
    }
    exit(0);
}

