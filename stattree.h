#ifndef STATTREE_H
#define STATTREE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h> 

typedef struct __StatTreeNode StatTreeNode;

struct __StatTreeNode {
    StatTreeNode *parent;
    StatTreeNode *left;
    StatTreeNode *right;
    int probs[256];
    int seen; /* total number of times this data has been seen */
    size_t len;
    char data[];
};

static inline StatTreeNode *StatTreeNode_new(size_t len)
{
    StatTreeNode *result = (StatTreeNode*)malloc(sizeof(StatTreeNode) + sizeof(char) * len); 
    memset(result,0,sizeof(StatTreeNode) + sizeof(char) * len);
    result->len = len;
    return result;
}

static inline int StatTreeNode_cmp(StatTreeNode *a, StatTreeNode *b)
{
    return (a->len < b->len) ? memcmp(a->data,b->data,a->len) :
            memcmp(a->data,b->data,b->len);
}

/* insert tree b into a */
static inline void StatTreeNode_insert(StatTreeNode *a, StatTreeNode *b)
{
    if ((!a) || (!b)) return;
    StatTreeNode *y, *x;
    x = a;
    while (x != NULL) {
        y = x;
        if (StatTreeNode_cmp(b,x) < 0) {
            x = x->left;
        } else {
            x = x->right;
        }
    }
    b->parent = y;
    if (StatTreeNode_cmp(b,y) < 0) {
        y->left = b;
    } else {
        y->right = b;
    }
}

static inline StatTreeNode *StatTreeNode_search(StatTreeNode *haystack, StatTreeNode *needle)
{
    while (haystack && (StatTreeNode_cmp(haystack, needle) != 0)) {
        if (StatTreeNode_cmp(needle,haystack) < 0) {
            haystack = haystack->left;
        } else {
            haystack = haystack->right;
        }
    }
    return haystack;
}

static inline void StatTreeNode_print(StatTreeNode *a)
{
    if (a) {
        StatTreeNode_print(a->left);
        size_t i;
        printf("0x");
        for (i = 0; i < a->len; i++) {
            printf("%x",a->data[i]);
        }
        printf(" ");
        StatTreeNode_print(a->right);
    }
}

static inline void StatTreeNode_longPrint(StatTreeNode *a)
{
    if (a) {
        StatTreeNode_longPrint(a->left);
        size_t i;
        printf("0x");
        for (i = 0; i < a->len; i++) {
            printf("%x",a->data[i]);
        }
        printf(" %d:", a->seen);
        for (i = 0; i < 256; i++) {
            printf(" ");
            printf("%f",(double)a->probs[i] / (double)a->seen);
//            printf("%d",a->probs[i]);
        }
        printf("\n");
        StatTreeNode_longPrint(a->right);
    }
}

static inline void StatTreeNode_longPrint_shallow(StatTreeNode *a)
{
    if (a) {
        size_t i;
        printf("0x");
        for (i = 0; i < a->len; i++) {
            printf("%x",a->data[i]);
        }
        printf(" %d:", a->seen);
        for (i = 0; i < 256; i++) {
            printf(" ");
            printf("%f",(double)a->probs[i] / (double)a->seen);
//            printf("%d",a->probs[i]);
        }
        printf("\n");
    }
}

/* Given a tree that's been initialized with StatTreeNode_accum, return a random
 * char */
static inline int StatTreeNode_weightedRandom(StatTreeNode *a)
{
    int num = random() % a->seen + 1;//a->probs[255];
    size_t i;
    for (i = 0; i < 256; i++) {
        if (a->probs[i] >= num) {
            return (int)i;
        }
    }
}

/* Converts number of occurences into a probability density function so that the
 * density can be used to drive a weighted random number generator by finding a
 * number between 0 and the value at the highest index and then iterating to the
 * lowest index whose value is greater than the currently drawn number. */
static inline void StatTreeNode_accum(StatTreeNode *a)
{
    size_t i;
    int accum = 0;
    for (i = 0; i < 256; i++) {
        accum += a->probs[i];
        a->probs[i] = accum;
    }
}

/* Apply a function to all nodes of tree */
static inline void StatTreeNode_apply(StatTreeNode *a, void (*func)(StatTreeNode*))
{
    if (a) {
        StatTreeNode_apply(a->left,func);
        func(a);
        StatTreeNode_apply(a->right,func);
    }
}

#endif /* STATTREE_H */
