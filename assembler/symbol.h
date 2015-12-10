#ifndef _SYMBOL_H_
#define _SYMBOL_H_

#include "type.h"

struct symb_node {
    uint16_t data;
	size_t len;
    const char* liter;
    struct symb_node* left;
    struct symb_node* right;
};

void symb_insert(const char* symb, size_t len, uint16_t data);
struct symb_node* symb_find(const char* symb);
void destroy_symb_tb(void);

#endif
