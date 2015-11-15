#ifndef _TYPE_H_
#define _TYPE_H_

#define _DEBUG_

#ifdef WIN32
#define inline __inline
#endif

#define true    (1)
#define false   (0)

typedef unsigned char bool;
typedef unsigned char byte;

typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;


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


#endif