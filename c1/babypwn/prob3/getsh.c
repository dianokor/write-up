#include <stdio.h>

int main()
{
	printf("env addr: 0x%x \n", getenv("shell"));
	return 0;
}
