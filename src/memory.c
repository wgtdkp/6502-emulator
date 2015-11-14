#include "memory.h"
#include "utility.h"
mem_t mem[MEM_SIZE];

int load_mem(addr_t start_addr, const char* filename)
{
    addr_t addr;
    FILE* file = fopen(filename, "rb");
    if (NULL == file) {
        error("open file '%s' failed!", filename);
        return 0;
    }

    for (addr = start_addr; ; addr++) {
        uint32_t buffer;
        if(1 != fscanf(file, "%x", &buffer))
            break;
        write_byte(addr, buffer & MASK(word_t));
        printf("addr: %4x,  %2x\n", addr, read_byte(addr));
    }
    write_byte(RESET_VECTOR_ADDR, L(start_addr));
    write_byte(RESET_VECTOR_ADDR + 1, H(start_addr));
    return (addr - start_addr);
}
