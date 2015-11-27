#ifndef _UTILITY_H_
#define _UTILITY_H_
#include <stdio.h>

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))


int error(const char* fmt, ...);

#endif
