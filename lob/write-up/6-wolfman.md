
# Summary

문제의 소스코드를 우선 살펴보자.

```c
[wolfman@localhost wolfman]$ cat darkelf.c
/*
        The Lord of the BOF : The Fellowship of the BOF
        - darkelf
        - egghunter + buffer hunter + check length of argv[1]
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

	strcpy(buffer, argv[1]);
	printf("%s\n", buffer);

        // buffer hunter
        memset(buffer, 0, 40);

}
```

문제의 핵심은 다음과 같다.

- 버퍼를 `\x00`으로 초기화
- 환경 변수를 `\x00`으로 초기화
- 인자 argv[1]의 길이 값 체크

`argv[1]`의 길이 값 체크 부분이 추가되었기 때문에 해당 인자에 쉘코드를 삽입해 실행할 수 없다<br>
그렇다면 `argv[2]`를 이용해 쉘코드를 삽입할 수 있을것 같다.<br>
먼저 tmp 폴더에 문제 파일을 그대로 복사해 실행권한을 얻고 gdb로 분석해보자.
```
[wolfman@localhost wolfman]$ mkdir tmp; cp darkelf tmp
[wolfman@localhost wolfman]$ ls
darkelf  darkelf.c  tmp
[wolfman@localhost wolfman]$ cd tmp
[wolfman@localhost tmp]$ ls
darkelf
[wolfman@localhost tmp]$ gdb -q ./darkelf
(gdb) set disassembly-flavor intel
(gdb) disas main
Dump of assembler code for function main:
... 생략 ...
0x80485d7 <main+215>:	call   0x8048420 <exit>
0x80485dc <main+220>:	add    %esp,4
0x80485df <main+223>:	nop
0x80485e0 <main+224>:	mov    %eax,DWORD PTR [%ebp+12]
0x80485e3 <main+227>:	add    %eax,4
0x80485e6 <main+230>:	mov    %edx,DWORD PTR [%eax]
0x80485e8 <main+232>:	push   %edx
0x80485e9 <main+233>:	lea    %eax,[%ebp-40]
0x80485ec <main+236>:	push   %eax
0x80485ed <main+237>:	call   0x8048440 <strcpy>
0x80485f2 <main+242>:	add    %esp,8
0x80485f5 <main+245>:	lea    %eax,[%ebp-40]
0x80485f8 <main+248>:	push   %eax
0x80485f9 <main+249>:	push   0x80486b0
0x80485fe <main+254>:	call   0x8048410 <printf>
0x8048603 <main+259>:	add    %esp,8
0x8048606 <main+262>:	push   40
0x8048608 <main+264>:	push   0
0x804860a <main+266>:	lea    %eax,[%ebp-40]
0x804860d <main+269>:	push   %eax
0x804860e <main+270>:	call   0x8048430 <memset>
0x8048613 <main+275>:	add    %esp,12
0x8048616 <main+278>:	leave
0x8048617 <main+279>:	ret
End of assembler dump.
(gdb) b *main+242
Breakpoint 1 at 0x80485f2
```

먼저 main 함수를 디스어셈블하여 strcpy 함수 실행 직후 부분`main+242`에 브레이크 포인트를 걸어준다.<br>
