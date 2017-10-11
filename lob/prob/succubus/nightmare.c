/*
        The Lord of the BOF : The Fellowship of the BOF
        - nightmare
        - PLT
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dumpcode.h>

main(int argc, char *argv[])
{
	char buffer[40];
	char *addr;

	if(argc < 2){
		printf("argv error\n");
		exit(0);
	}

	// check address
	addr = (char *)&strcpy;
        if(memcmp(argv[1]+44, &addr, 4) != 0){
                printf("You must fall in love with strcpy()\n");
                exit(0);
        }

        // overflow!
        strcpy(buffer, argv[1]);
	printf("%s\n", buffer);

	// dangerous waterfall
	memset(buffer+40+8, 'A', 4);
}
