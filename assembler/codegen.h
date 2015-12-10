#ifndef _CODEGEN_H_
#define _CODEGEN_H_
#include "utility.h"
#include "type.h"
#include "token.h"

uint8_t gen_code(byte code[3], const struct token_inst* inst);

#endif
