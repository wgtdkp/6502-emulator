#include "memory.h"
#include "utility.h"
#include "hex_handler.h"

typedef byte mem_t;

static mem_t mem[MEM_SIZE];

bool load_mem(const char* filename)
{
    FILE* file = fopen(filename, "r");
    if (NULL == file) {
        error("open file '%s' failed!", filename);
        return false;
    }
    if (false == load(file)) {
        error("loading program failed!\n");
        fclose(file);
        return false;
    }
    fclose(file);
    return true;
}

uint16_t read_2bytes(addr_t addr)
{
    uint16_t data;
    data = mem[addr & MEM_MASK];
    //little endian
    data |= (uint16_t)mem[(addr+1) & MEM_MASK] << 8;
    return data;
}

byte read_byte(addr_t addr)
{
    return mem[addr & MEM_MASK];
}

void write_byte(addr_t addr, byte data)
{
    //printf("MEM_MASK : %x \n", MEM_MASK);
    //printf("addr & MEM_MASK : %x \n", addr & MEM_MASK);
    mem[addr & MEM_MASK] = data;
    //printf("mem[addr & MEM_MASK] : %x \n", mem[addr & MEM_MASK]);
}

void write_2bytes(addr_t addr, uint16_t data)
{
    write_byte(addr, L(data));
    write_byte(addr + 1, H(data));
}
