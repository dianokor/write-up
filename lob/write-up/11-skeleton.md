# Summary

이전 문제와 동일한 내용은 생략하고 추가된 내용만 다루었다.**

> 문제의 소스코드

```c
[skeleton@localhost skeleton]$ cat golem.c
/*
        The Lord of the BOF : The Fellowship of the BOF
        - golem
        - stack destroyer
*/

#include <stdio.h>
#include <stdlib.h>

extern char **environ;

main(int argc, char *argv[])
{
	char buffer[40];
	int i;

	if(argc < 2){
		printf("argv error\n");
		exit(0);
	}

	if(argv[1][47] != '\xbf')
	{
		printf("stack is still your friend.\n");
		exit(0);
	}

	strcpy(buffer, argv[1]);
	printf("%s\n", buffer);

        // stack destroyer!
        memset(buffer, 0, 44);
	memset(buffer+48, 0, 0xbfffffff - (int)(buffer+48));
}
```

문제의 소스코드에서 추가된 내용을 살펴보자

- buffer 시작 지점에서부터 유저 영역 끝까지 0으로 초기화



# Analysis



# Exploit

