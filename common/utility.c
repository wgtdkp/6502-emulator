#include "utility.h"
#include <stdarg.h>

int error(const char* fmt, ...)
{
  int cnt = 0;
  va_list arg;
  va_start(arg, fmt);
  cnt += fprintf(stdout, "errror: ");
  cnt += vfprintf(stdout, fmt, arg);
  va_end(arg);
  return cnt;
}
