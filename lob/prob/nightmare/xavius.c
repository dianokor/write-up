/*
        The Lord of the BOF : The Fellowship of the BOF
        - xavius
        - arg
*/

#include <stdio.h>
#include <stdlib.h>
#include <dumpcode.h>

main()
{
	char buffer[40];
	char *ret_addr;

	// overflow!
	fgets(buffer, 256, stdin);
	printf("%s\n", buffer);

	if(*(buffer+47) == '\xbf')
	{
		printf("stack retbayed you!\n");
		exit(0);
	}

	if(*(buffer+47) == '\x08')
        {
                printf("binary image retbayed you, too!!\n");
                exit(0);
        }

	// check if the ret_addr is library function or not
	memcpy(&ret_addr, buffer+44, 4);
	while(memcmp(ret_addr, "\x90\x90", 2) != 0)	// end point of function
	{
		if(*ret_addr == '\xc9'){		// leave
			if(*(ret_addr+1) == '\xc3'){	// ret
				printf("You cannot use library function!\n");
				exit(0);
			}
		}
		ret_addr++; 
	}

        // stack destroyer
        memset(buffer, 0, 44);
	memset(buffer+48, 0, 0xbfffffff - (int)(buffer+48));

	// LD_* eraser
	// 40 : extra space for memset function
	memset(buffer-3000, 0, 3000-40);
}
