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

#define MMIO_END    (ROM_BEGIN - 1)

#define ROM_BEGIN	(0x8000)
#define ROM_END     (0xFFF9)

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

uint16_t read_2bytes(addr_t addr);
byte read_byte(addr_t addr);
void write_byte(addr_t addr, byte data);
void write_2bytes(addr_t addr, uint16_t data);

static inline addr_t addr(byte high, byte low)
{
  return ((addr_t)high << 8) | low;
}

bool load_mem(const char* filename);

#endif
