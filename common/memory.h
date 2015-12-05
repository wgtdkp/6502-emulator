#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "type.h"
#include <stdio.h>
#define ADDR_WIDTH  (16)
#define MEM_SIZE    (1 << ADDR_WIDTH)
#define PAGE_SIZE	(0x0100)

#define MEM_MASK    (MEM_SIZE - 1)
#define MASK(type)   ( (1 << (sizeof(type) * 8)) - 1)
#define H(addr)     ((word_t)(((addr) & 0xFF00) >> 8))
#define L(addr)     ((addr) & 0x00FF)

/******memory map******/
#define SP_BEGIN	(0x0100)
#define SP_END      (RAM_BEGIN - 1)

#define RAM_BEGIN	(0x0200)
#define RAM_END     (MMIO_BEGIN - 1)

#define MMIO_BEGIN	(0x4000)
#define GPIO_A	(0x4000)
#define GPIO_B	(0x4001)
#define GPIO_C	(0x4002)
#define GPIO_D	(0x4003)
#define GPIO_E	(0x4004)
#define GPIO_F	(0x4005)

#define UART0	(0x4010)
#define UART1	(0x4020)
#define UART2	(0x4030)
#define UART3	(0x4040)

#define TIMER0	(0x4050)
#define TIMER1	(0x4060)

#define NMI		(0x4070)

#define MMIO_END    (ROW_BEGIN - 1)

#define ROW_BEGIN	(0x8000)
#define ROW_END     (0xFFF9)

#define NMI_VECTOR	(0xFFFA)
#define RESET_VECTOR	(0xFFFC)
#define IRQ_VECTOR	(0xFFFE)
#define BRK_VECTOR	IRQ_VECTOR


#if ADDR_WIDTH == 8
	typedef uint8_t addr_t;
#elif ADDR_WIDTH == 16
	typedef uint16_t addr_t;
#elif ADDR_WIDTH == 32
	typedef uint32_t addr_t;
#elif ADDR_WIDTH == 64
	typedef uint64_t addr_t;
#endif

typedef byte mem_t;
extern mem_t mem[MEM_SIZE];

static inline uint16_t read_2bytes(addr_t addr)
{
    uint16_t data;
    data = mem[addr & MEM_MASK];
    //little endian
    data |= (uint16_t)mem[(addr+1) & MEM_MASK] << 8;
    return data;
}

static inline byte read_byte(addr_t addr)
{
    return mem[addr & MEM_MASK];
}

static inline void write_byte(addr_t addr, byte data)
{
	//printf("MEM_MASK : %x \n", MEM_MASK);
	//printf("addr & MEM_MASK : %x \n", addr & MEM_MASK);
    mem[addr & MEM_MASK] = data;
	//printf("mem[addr & MEM_MASK] : %x \n", mem[addr & MEM_MASK]);
}

static inline void write_2bytes(addr_t addr, uint16_t data)
{
	write_byte(addr, L(data));
	write_byte(addr + 1, H(data));
}

static inline addr_t addr(byte high, byte low)
{
	return ((addr_t)high << 8) | low;
}

bool load_mem(const char* filename);

#endif
