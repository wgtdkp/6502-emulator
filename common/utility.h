#ifndef _UTILITY_H_
#define _UTILITY_H_

#include "type.h"
#include <stdio.h>

#if defined(DEBUG)
#   if defined(_MSC_VER)
#       // "(void)0," is for suppressing C4127 warning in "assert(false)", "assert(true)" and the like
#       define ASSERT(x)    if ( !((void)0,(x))) { __debugbreak(); }
#   else
#       include <assert.h>
#       define ASSERT       assert
#   endif
#else
#   define ASSERT( x )               {}
#endif

#if defined(_MSC_VER)
#   define PACKED(declaration) __pragma( pack(push, 1) ) declaration __pragma( pack(pop) )
#else
#   define PACKED(declaration) declaration __attribute__ ((packed));
#endif


#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

static inline uint16_t le16_to_cpu(uint16_t le16)
{
	return (((le16 & 0x00FF) << 8) | ((le16 & 0xFF00) >> 8));
}

static inline uint16_t cpu_to_le16(uint16_t cpu16)
{
	return le16_to_cpu(cpu16);
}

int error(const char* fmt, ...);

#endif
