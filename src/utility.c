#include "utility.h"

int error(const char* fmt, ..)
{
    int cnt = 0;
    va_list arg;
    va_start(arg, fmt);
    cnt += fprintf(stderr, "errror: ");
    cnt += vfprintf(stderr, fmt, arg);
    va_end(arg);
    return cnt;
}
