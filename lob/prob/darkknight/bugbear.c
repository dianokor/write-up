/*
        The Lord of the BOF : The Fellowship of the BOF
        - bugbear
        - RTL1
*/

#include <stdio.h>
#include <stdlib.h>

main(int argc, char *argv[])
{
	char buffer[40];
	int i;

	if(argc < 2){
		printf("argv error\n");
		exit(0);
	}

	if(argv[1][47] == '\xbf')
	{
		printf("stack betrayed you!!\n");
		exit(0);
	}

	strcpy(buffer, argv[1]); 
	printf("%s\n", buffer);
}
