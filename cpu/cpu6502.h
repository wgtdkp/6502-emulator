#ifndef _CPU6502_H_
#define _CPU6502_H_
#include "type.h"
#include "memory.h"
#include "instruction.h"

#define le16_to_cpu(x)  ((((x) & 0x00FF) << 8) | (((x) & 0xFF00) >> 8))
#define cpu_to_le16(x)  le16_to_cpu(x)    

#define BCD(x)      (((x) & 0x0F) + ((x) >> 4) * 10)
#define BIT(x, i)   (((0x01 << (i)) & (x)) >> (i))
#define SET_BIT(x, i, b)    ((x) = ((b) ? (x) | (0x01 << (i)) : (x) & ~(0x01 << (i))))

/**
static inline SET_BIT(word_t* x, word_t i, word_t b)
{
    if(b)

}
*/

#define PUSH(cpu, x)    (write_byte(cpu->sp--, (x)))
#define POP(cpu)        (read_byte(++cpu->sp))

extern struct cpu cpu6502;

struct cpu {
    word_t psw; //Processor Status Word
    word_t acc; //Accumulator
    word_t xr;  //X Register
    word_t yr;  //Y Register
    addr_t sp;  //Stack Pointer
    addr_t pc;  //Program Counter
    const struct inst* is;  //instruction set
    uint16_t cycle_count;   //cycles
};


int cpu6502_run(struct cpu* cpu, mem_t* mem);

#endif
