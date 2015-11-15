#ifndef _SYMBOL_H_
#define _SYMBOL_H_

#include "type.h"

struct symb_node {
    uint16_t data;
    char* liter;
    struct symb_node* left;
    struct symb_node* right;
};

extern struct symb_node* symb_tb;

struct symb_node* symb_insert(struct symb_node* root, const char* symb, uint16_t data);
struct symb_node* symb_find(struct symb_node* root, const char* symb);
void destroy_symb_tb(struct symb_node* root);

#endif
