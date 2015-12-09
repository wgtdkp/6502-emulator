#include "codegen.h"
#include "parser.h"
#include "token.h"

uint8_t gen_code(byte code[3], const struct tk_inst* inst)
{
    uint8_t code_len;
    if (NULL == code)
        return 0;
    if (PRAGMA_BYTE == inst->op_code) {
        code_len = 1;
    } else if (PRAGMA_WORD == inst->op_code) {
        code_len = 2;
    } else {
        code_len = is[inst->op_code].len;
    }
    
    if (PRAGMA_BYTE == inst->op_code) {
        code_len = 1;
        code[0] = inst->lo;
    }
    else if (PRAGMA_WORD == inst->op_code) {
        code_len = 2;
        code[0] = inst->lo;
        code[1] = inst->hi;
    } else {
        code_len = is[inst->op_code].len;
        code[0] = inst->op_code;
        if (2 <= code_len)
            code[1] = inst->lo;
        if (3 <= code_len)
            code[2] = inst->hi;
    }
    return code_len;
}
