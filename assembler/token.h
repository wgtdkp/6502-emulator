#ifndef _TOKEN_H_
#define _TOKEN_H_

#include "type.h"
#include "memory.h"

#define TOKEN_NULL  (0)
#define TOKEN_LABEL  (128)
#define TOKEN_SYMB_A (129)
#define TOKEN_SYMB_X (130)
#define TOKEN_SYMB_Y (131)
#define TOKEN_INST  (132)
#define TOKEN_NUMBER   (133)
//#define TOKEN_IMM   (134)
#define TOKEN_PRAGMA_END    (135)
#define TOKEN_PRAGMA_BYTE   (136)
#define TOKEN_PRAGMA_WORD   (137)

struct token_symb {
    addr_t addr;
};

struct token_inst {
    byte op_code;
    union {
        uint16_t operand;
        struct {
			byte lo;
            byte hi;
        };
        addr_t addr;
    };
};

struct token_node {
	struct {
		int type;
		size_t len;
		const char* liter;
		uint16_t data;	//only used by the number token
	};
    struct token_node* next;
};

struct token_list {
    struct token_node* tail;
    struct token_node* head;
};

int trim(char* str);
struct token_node* create_token(int type, const char* liter, size_t len);
void destroy_token(struct token_node** head);
void token_append(struct token_list* tok_list, struct token_node* tok_node);
int tokenize(struct token_list* tk_list, char* file_buffer);
void str_toupper(char* str);
const struct token_node* token_offset(const struct token_node* head, size_t n);
void print_token(const struct token_node* tk);

#endif
