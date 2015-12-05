#ifndef _HEX_HANDLER_H_
#define _HEX_HANDLER_H_

#include "type.h"
#include "utility.h"
#include <stdio.h>

void dump_end(FILE* code_file);
void dump_code(FILE* code_file, uint8_t code[3], uint8_t code_len, addr_t addr);
bool load(FILE* code_file);

#endif
