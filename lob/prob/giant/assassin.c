/*
        The Lord of the BOF : The Fellowship of the BOF
        - assassin
        - no stack, no RTL
*/

#include <stdio.h>
#include <stdlib.h>

main(int argc, char *argv[])
{
	char buffer[40];

	if(argc < 2){
		printf("argv error\n");
		exit(0);
	}

	if(argv[1][47] == '\xbf')
	{
		printf("stack retbayed you!\n");
		exit(0);
	}

        if(argv[1][47] == '\x40')
        {
                printf("library retbayed you, too!!\n");
                exit(0);
        }

	strcpy(buffer, argv[1]); 
	printf("%s\n", buffer);

        // buffer+sfp hunter
        memset(buffer, 0, 44);
}
