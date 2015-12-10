#include "type.h"
#include "utility.h"
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

#if defined(_MSC_VER) && defined(DEBUG)
int main(void)
{
	int argc = 2;
	char* argv[] = { "fc", "test.hex" };
#else
int main(int argc, char* argv[])
{
#endif
	if (argc < 2) {
        usage();
        return -1;
    }

    if (false == load_mem(argv[1])) {
        return -1;
    }
	cpu_boot(&cpu6502);
    cpu6502_run(&cpu6502);
    print_cpu(&cpu6502);
    return 0;
}
