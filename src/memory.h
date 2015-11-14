#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "type.h"
#include <stdio.h>
#define ADDR_W  (16)
#define MEM_SIZE    (1 << ADDR_W)
#define MEM_MASK    (MEM_SIZE - 1)
#define RESET_VECTOR_ADDR   (0xFFFE)
#define MASK(type)   ( (1 << (sizeof(type) * 8)) - 1)
#define H(addr)     ((word_t)(((addr) & 0xFF00) >> 8))
#define L(addr)     ((addr) & 0x00FF)

#if ADDR_W == 8
typedef uint8_t addr_t;

#elif ADDR_W == 16
typedef uint16_t addr_t;

#elif ADDR_W == 32
typedef uint32_t addr_t;

#elif ADDR_W == 64
typedef uint64_t addr_t;

#endif

typedef byte mem_t;

extern mem_t mem[MEM_SIZE];

static inline 
uint16_t read_2byte(addr_t addr)
{
    uint16_t data;
    data = mem[addr & MEM_MASK];
    //little endian
    data |= (uint16_t)mem[(addr+1) & MEM_MASK] << 8;
    return data;
}

static inline 
byte read_byte(addr_t addr)
{
    return mem[addr & MEM_MASK];
}

static inline
void write_byte(addr_t addr, byte data)
{
	//printf("MEM_MASK : %x \n", MEM_MASK);
	//printf("addr & MEM_MASK : %x \n", addr & MEM_MASK);
    mem[addr & MEM_MASK] = data;
	//printf("mem[addr & MEM_MASK] : %x \n", mem[addr & MEM_MASK]);
}

int load_mem(addr_t start_addr, const char* filename);


#endif
