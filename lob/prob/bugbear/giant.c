/*
        The Lord of the BOF : The Fellowship of the BOF
        - giant
        - RTL2
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

main(int argc, char *argv[])
{
	char buffer[40];
	FILE *fp;
	char *lib_addr, *execve_offset, *execve_addr;
	char *ret;

	if(argc < 2){
		printf("argv error\n");
		exit(0);
	}

	// gain address of execve
	fp = popen("/usr/bin/ldd /home/giant/assassin | /bin/grep libc | /bin/awk '{print $4}'", "r");
	fgets(buffer, 255, fp);
	sscanf(buffer, "(%x)", &lib_addr);
	fclose(fp);

	fp = popen("/usr/bin/nm /lib/libc.so.6 | /bin/grep __execve | /bin/awk '{print $1}'", "r");
	fgets(buffer, 255, fp);
	sscanf(buffer, "%x", &execve_offset);
	fclose(fp);

	execve_addr = lib_addr + (int)execve_offset;
	// end

	memcpy(&ret, &(argv[1][44]), 4);
	if(ret != execve_addr)
	{
		printf("You must use execve!\n");
		exit(0);
	}

	strcpy(buffer, argv[1]); 
	printf("%s\n", buffer);
}
