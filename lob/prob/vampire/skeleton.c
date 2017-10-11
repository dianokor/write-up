/*
        The Lord of the BOF : The Fellowship of the BOF
        - skeleton
        - argv hunter
*/

#include <stdio.h>
#include <stdlib.h>

extern char **environ;

main(int argc, char *argv[])
{
	char buffer[40];
	int i, saved_argc;

	if(argc < 2){
		printf("argv error\n");
		exit(0);
	}

	// egghunter 
	for(i=0; environ[i]; i++)
		memset(environ[i], 0, strlen(environ[i]));

	if(argv[1][47] != '\xbf')
	{
		printf("stack is still your friend.\n");
		exit(0);
	}

	// check the length of argument
	if(strlen(argv[1]) > 48){
		printf("argument is too long!\n");
		exit(0);
	}

	// argc saver
	saved_argc = argc;

	strcpy(buffer, argv[1]); 
	printf("%s\n", buffer);

        // buffer hunter
        memset(buffer, 0, 40);

	// ultra argv hunter!
	for(i=0; i<saved_argc; i++)
		memset(argv[i], 0, strlen(argv[i]));
}
