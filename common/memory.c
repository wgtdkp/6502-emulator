#include "memory.h"
#include "utility.h"
#include "hex_handler.h"

mem_t mem[MEM_SIZE];

bool load_mem(const char* filename)
{
    FILE* file = fopen(filename, "r");
    if (NULL == file) {
        error("open file '%s' failed!", filename);
        return false;
    }
    if (false == load(mem, file)) {
        error("loading program failed!\n");
        fclose(file);
        return false;
    }
    fclose(file);
    return true;
}
