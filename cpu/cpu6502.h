#ifndef _CPU6502_H_
#define _CPU6502_H_
#include "type.h"
#include "memory.h"
#include "instruction.h"

extern struct cpu cpu6502;

struct cpu {
    word_t psw; //Processor Status Word
    word_t acc; //Accumulator
    word_t xr;  //X Register
    word_t yr;  //Y Register
    word_t sp;  //Stack Pointer, one page size
    addr_t pc;  //Program Counter
    const struct inst* is;  //instruction set
    uint16_t cycle_count;   //cycles
};

void cpu_boot(struct cpu* cpu);
int cpu6502_run(struct cpu* cpu, mem_t* mem);

static inline uint16_t le16_to_cpu(uint16_t le16)
{
	return (((le16 & 0x00FF) << 8) | ((le16 & 0xFF00) >> 8));
}

static inline uint16_t cpu_to_le16(uint16_t cpu16)
{
	return le16_to_cpu(cpu16);
}

static inline uint8_t BCD(uint8_t byte)
{
	return ((byte & 0x0F) + (byte >> 4) * 10);
}

static inline bool BIT(uint16_t data, uint8_t bit)
{
	return (((0x01 << bit) & (data)) >> bit);
}

static inline void SET_BIT(uint8_t* byte, uint8_t bit, bool val)
{
	*byte = val ? *byte | (0x01 << bit) : *byte & ~(0x01 << bit);
}


#endif
