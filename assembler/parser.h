#ifndef _PARSER_H_
#define _PARSER_H_
#include "instruction.h"
#include "memory.h"

/*
 *this two instruction code is not used by the instruction set
 *so i use it to represent .BYTE and .WORD pragma.
 */ 
#define PRAGMA_BYTE (0x02)
#define PRAGMA_WORD (0x03)
#define PSEUDO_SPC  (0x04)

//#define TO_LITER_CODE(str)  ((str[0] << 16) + (str[1] << 8) + str[2])
#define TO_LITER_CODE(str)	(str)

#define ADDR_ACC        (0)
 #define ADDR_IMM       (1)
 #define ADDR_ZERO      (2)
#define ADDR_ZERO_X     (3)
#define ADDR_ZERO_Y     (4)
 #define ADDR_ABS       (5)
#define ADDR_ABS_X      (6)
#define ADDR_ABS_Y      (7)
#define ADDR_IMP        (8)
#define ADDR_REL        (9)
#define ADDR_IND_X      (10)
#define ADDR_IND_Y      (11)
#define ADDR_IND        (12)

int parse(const char* output, const char* input);
int find_inst(const char* liter);
static inline int is_inst(const char* liter)
{
	return -1 != find_inst(liter);
}
#endif
