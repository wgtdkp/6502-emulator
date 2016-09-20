#ifndef _TYPE_H_
#define _TYPE_H_

//#include <linux/types.h>
#include <stdint.h>

#define _DEBUG_

#ifdef WIN32
#define inline __inline
#endif

typedef enum {
  true = 1,
  false = !true,
} bool;

typedef unsigned char byte;

#ifndef __int8_t_defined
//typedef char int8_t;
//typedef short int16_t;
//typedef int int32_t;
//typedef long long int64_t;
#endif

/*
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
*/

#define WORD_SIZE   (8)

#if WORD_SIZE == 8
typedef uint8_t word_t;
#elif WORD_SIZE == 16
typedef uint16_t word_t;
#elif WORD_SIZE == 32
typedef uint32_t word_t;
#elif WORD_SIZE == 64
typedef uint64_t word_t;
#endif

#define ADDR_WIDTH  (16)

#if ADDR_WIDTH == 8
typedef uint8_t addr_t;
#elif ADDR_WIDTH == 16
typedef uint16_t addr_t;
#elif ADDR_WIDTH == 32
typedef uint32_t addr_t;
#elif ADDR_WIDTH == 64
typedef uint64_t addr_t;
#endif

#endif