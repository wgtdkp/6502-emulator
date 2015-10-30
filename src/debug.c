#include "type.h"
#include "cpu6502.h"
#include "memory.h"
#include <stdio.h>
//#include <stdlib.h>

void usage(void)
{
    
}

void print_cpu(struct cpu* cpu)
{
    printf("PC: %2x\n", cpu->pc);
    printf("ACC: %2x\n", cpu->acc);
    printf("XR: %2x\n", cpu->xr);
    printf("YR: %2x\n", cpu->yr);
    printf("PSW: %2x\n", cpu->psw);
    printf("SP: %2x\n", cpu->sp);
    printf("N\tV\t-\tB\tD\tI\tZ\tC\t\n");
    printf("%d\t", BIT(cpu->psw, 7));
    printf("%d\t", BIT(cpu->psw, 6));
    printf("%d\t", BIT(cpu->psw, 5));
    printf("%d\t", BIT(cpu->psw, 4));
    printf("%d\t", BIT(cpu->psw, 3));
    printf("%d\t", BIT(cpu->psw, 2));
    printf("%d\t", BIT(cpu->psw, 1));
    printf("%d\t", BIT(cpu->psw, 0));
    printf("\n");
}


//int main(int argc, char* argv[])
int main(void)
{
	int argc = 2;
	char* argv[2] = {"fc", "test.hex"};
    char *filename;
    const addr_t start_addr = 0x8000;
    addr_t addr;
    FILE* file;
    if (argc < 2) {
        usage();
        return -1;
    }

    filename = argv[1];
    file = fopen(filename, "rb");
    for (addr = start_addr; ; addr++) {
        uint32_t buffer;
        if(1 != fscanf(file, "%x", &buffer))
            break;
        write_byte(addr, buffer & MASK(word_t));
        printf("addr: %4x,  %2x\n", addr, read_byte(addr));
    }
    write_byte(RESET_VECTOR_ADDR, L(start_addr));
    write_byte(RESET_VECTOR_ADDR + 1, H(start_addr));

    //print_cpu(&cpu6502);
    cpu6502_run(&cpu6502, mem);
    print_cpu(&cpu6502);
    return 0;
}
