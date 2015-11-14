#ifndef _TOKEN_H_
#define _TOKEN_H_

#include "type.h"
#include "memory.h"

#define TOKEN_SYMB  (0)
#define TOKEN_COMM  (1)
#define TOKEN_INST  (2)
#define TOKEN_COMMA (3)
#define TOKEN_IMME  (4)


struct token {
    int type;
    char* literal;
};

struct tk_symb {
    addr_t addr;
};

struct tk_inst {
    byte op_code;
    byte op_data[2];
    //addr_t addr;
};



#endif
