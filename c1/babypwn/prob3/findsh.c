#include <stdio.h>

int main(int arc, char* argc[]){
	long sh;
	sh = 0xf7e3eda0;
	while(memcmp((void*)sh,"/bin/sh",8)) sh++;
	printf("addr : 0x%x\n",sh);
}
