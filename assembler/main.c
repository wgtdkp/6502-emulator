#include "main.h"
#include "parser.h"
#include "utility.h"

int main(int argc, char* argv[])
{
  if (argc < 2)
    return -1;
  return parse("test.hex", argv[1]);
}
