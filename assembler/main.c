#include "main.h"
#include "parser.h"

int main(int args, char* argv[])
{
	if (args < 2)
		return -1;
	int err = parse("test.hex", argv[1]);
    return err;
}