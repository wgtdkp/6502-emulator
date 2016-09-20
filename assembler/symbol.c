#include "symbol.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct symb_node* symb_tb = NULL;

static struct symb_node* create_symb(uint16_t data, const char* liter, size_t len)
{
  struct symb_node* ret = NULL;
  ret = (struct symb_node*)malloc(sizeof(struct symb_node));
  if (NULL == ret) {
    error("memory running out!\n");
    return NULL;
  }
  ret->liter = liter;
  ret->len = len;
  ret->data = data;
  ret->left = NULL;
  ret->right = NULL;
  return ret;
}

static struct symb_node* __symb_find(struct symb_node* root, const char* symb, size_t len)
{
  int cmp;
  while (NULL != root) {
    cmp = strncmp(symb, root->liter, len);
    if (0 == cmp)
      return root;
    if (0 > cmp)
      root = root->left;
    else
      root = root->right;
  }
  return NULL;
}

struct symb_node* symb_find(const char* symb, size_t len)
{
  return __symb_find(symb_tb, symb, len);
}

static struct symb_node* __symb_insert(struct symb_node* root, const char* symb, size_t len, uint16_t data)
{
  int cmp;
  struct symb_node* ret = NULL;
  if (NULL == root) {
    ret = create_symb(data, symb, len);
    return ret;
  }
  cmp = strncmp(symb, root->liter, root->len);
  if (0 == cmp) {
    root->data = data;
  } else if (0 > cmp) {
    root->left = __symb_insert(root->left, symb, len, data);
  } else {
    root->right = __symb_insert(root->right, symb, len, data);
  }
  return root;
}

void symb_insert(const char* symb, size_t len, uint16_t data)
{
  symb_tb = __symb_insert(symb_tb, symb, len, data);
}

static void __destroy_symb_tb(struct symb_node* root)
{
  if (NULL == root) return;
  __destroy_symb_tb(root->left);
  __destroy_symb_tb(root->right);
  //free(root->liter);
  free(root);
}

void destroy_symb_tb(void)
{
  __destroy_symb_tb(symb_tb);
  symb_tb = NULL;
}

static void print_symb(const struct symb_node* symb)
{
  size_t i;
  for (i = 0; i < symb->len; i++)
    printf("%c", symb->liter[i]);
  printf("\t%#04X", symb->data);
}

static void __print_symb_tb(const struct symb_node* root)
{
  if (NULL == root) return;
  __print_symb_tb(root->left);
  __print_symb_tb(root->right);
  print_symb(root);
  printf("\n");
}

void print_symb_tb(void)
{
  __print_symb_tb(symb_tb);
}



