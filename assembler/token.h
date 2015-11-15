#ifndef _TOKEN_H_
#define _TOKEN_H_

#include "type.h"
#include "memory.h"

#define EQUAL_NOCASE(x, y) ((x) == (y) || (x) - 'a' == (y) - 'A' || (x) - 'A' == (y) - 'a')

#define IS_NUM_PREFIX(ch) ('<' == (ch) \
                    || '>' == (ch) \
                    || 'O' == (ch) \
                    || '$' == (ch) \
                    || '%' == (ch) \
                    || IS_NUM(ch))

#define IS_NUM(ch)      ('0' <= (ch) && '9' >= (ch))
#define IS_LETTER(ch)   ('A' <= (ch) && 'Z' >= (ch) || 'a' <= (ch) && 'z' >= (ch) || '_' == (ch))
#define IS_BLANK(ch)    (' ' == (ch) || '\t' == (ch) || '\n' == (ch))

#define TOKEN_NULL  (0)
#define TOKEN_SYMB  (128)
#define TOKEN_SYMB_A (129)
#define TOKEN_SYMB_X (130)
#define TOKEN_SYMB_Y (131)
#define TOKEN_INST  (132)
#define TOKEN_NUM   (133)
//#define TOKEN_IMM   (134)
#define TOKEN_PRAGMA_END    (135)
#define TOKEN_PRAGMA_BYTE   (136)
#define TOKEN_PRAGMA_WORD   (137)




struct tk_symb {
    addr_t addr;
};

struct tk_inst {
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
		char* liter;
	};
    struct token_node* next;
};

struct token_list {
    size_t size;
    struct token_node* tail;
    struct token_node* head;
};

int trim(char* str);
struct token_node* create_token(int type, const char* liter, size_t len);
void destroy_token(struct token_node** head);
void token_append(struct token_list* tok_list, struct token_node* tok_node);
int tokenlize(struct token_list* tok_list, char* line, int line_num);
void toupper(char* str);
struct token_node* token_offset(struct token_node* head, size_t n);

#endif
