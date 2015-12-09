#include "main.h"
#include "parser.h"
#include "utility.h"

#if defined(_MSC_VER) && defined(DEBUG)
int main(void)
{
	int argc = 2;
	char* argv[] = {"asmer", "test.asm"};
#else
int main(int argc, char* argv[])
{
#endif
	if (argc < 2)
		return -1;
	int err = parse("test.hex", argv[1]);
    return err;
}