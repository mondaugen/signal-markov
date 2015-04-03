#include "stattree.h"

int main (void)
{
    StatTreeNode *top = NULL;
    StatTreeNode *a, *b, *c;
    a = StatTreeNode_new(4);
    char str[4];
    memcpy(str,"bbbb",4);
    memcpy(a->data,str,4);
    top = a;
    b = StatTreeNode_new(4);
    memcpy(str,"aaaa",4);
    memcpy(b->data,str,4);
    StatTreeNode_insert(a,b);
    c = StatTreeNode_new(4);
    memcpy(str,"xxxx",4);
    memcpy(c->data,str,4);
    StatTreeNode_insert(a,c);
    StatTreeNode_print(a);
}

    
