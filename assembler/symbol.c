#include "symbol.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct symb_node* symb_tb = NULL;

static struct symb_node* create_symb(uint16_t data, const char* liter)
{
    size_t len;
    struct symb_node* ret = NULL;
    ret = (struct symb_node*)malloc(sizeof(struct symb_node));
    if (NULL == ret)
        return NULL;
    
    len = strlen(liter);
    ret->liter = (char*)malloc(sizeof(char) * (len + 1));
    if (NULL == ret->liter) {
        free(ret);
        return NULL;
    }
    memcpy((void*)ret->liter, liter, sizeof(char) * len);
	ret->liter[len] = 0;
    ret->data = data;
    ret->left = NULL;
    ret->right = NULL;
    return ret;
}

struct symb_node* symb_find(struct symb_node* root, const char* symb)
{
    int cmp;
    while (NULL != root) {
        cmp = strcmp(symb, root->liter);
        if (0 == cmp)
            return root;
        if (0 > cmp)
            root = root->left;
        else
            root = root->right;
    }
    return NULL;
}

struct symb_node* symb_insert(struct symb_node* root, const char* symb, uint16_t data)
{
    int cmp;
    struct symb_node* ret = NULL;
    if (NULL == root) {
        ret = create_symb(data, symb);
        return ret;
    }
    cmp = strcmp(symb, root->liter);
    if (0 == cmp) {
        root->data = data;
    } else if (0 > cmp) {
        root->left = symb_insert(root->left, symb, data);
    } else {
        root->right = symb_insert(root->right, symb, data);
    }
    return root;
}

void destroy_symb_tb(struct symb_node* root)
{
    if (NULL == root)
        return;
    destroy_symb_tb(root->left);
    destroy_symb_tb(root->right);
    free(root->liter);
    free(root);
}

