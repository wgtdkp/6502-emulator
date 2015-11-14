#include "parser.h"
#include "utility.h"

#define MAX_LINE_LEN    (255)

static addr_t pc = 0;


/*
 * @return: the length of new string.
 */
int trim(char* str)
{
    char ch;
    size_t begin, end;
    int i, j;
    for (i = 0; 0 != str[i] && IS_BLANK(str[i]);) ++i;
    begin = i;
    for (; 0 != str[i];) ++i;
    for (--i; i >= 0 && IS_BLANK(str[i]);) ++i;
    end = i;
    for (i = 0, j = begin; j <= end; i++, j++)
        str[i] = str[j];
    str[end + 1] = 0;

    return end - begin + 1;
}

int parse(const char* output, const char* input)
{
    size_t line_cnt, inst_cnt;
    char line[MAX_LINE_LEN + 1];
    FILE* asm_file = fopen(input, "r");
    FILE* code_file = fopen(output, "wb");
    if (NULL == asm_file || NULL == code_file) {
        error("open file %s failed!\n", 
            NULL == asm_file ? asm_file : code_file);
        return -1;
    }

    line_num = 0;
    while (NULL != fgets(line, MAX_LINE_LEN + 1, asm_file)) {
        size_t len;
        len = trim(line);

    }
    return inst_cnt;
}

int parse_inst(struct inst* inst, const char* line, size_t len)
{
    int err = 0;

}

