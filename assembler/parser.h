#ifndef _PARSER_H_
#define _PARSER_H_
#include "instruction.h"
#include "memory.h"

#define IS_BLANK(ch)    (' ' == (ch) || '\t' == (ch) || '\n' == (ch))


int parse(const char* output, const char* input)
int trim(char* str);
int parse_inst(struct inst* inst, const char* line);

#endif
