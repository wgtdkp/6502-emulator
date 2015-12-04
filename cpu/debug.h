#ifndef _DEBUG_H_
#define _DEBUG_H_
#include "cpu6502.h"
#include <stdio.h>

#define DEBUG

#ifdef DEBUG
#	define ASSERT(x)           if ( !((void)0,(x))) { __debugbreak(); }
#endif

void print_cpu(struct cpu* cpu);

#endif
