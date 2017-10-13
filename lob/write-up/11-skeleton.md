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

<br>

이번 문제는 버퍼의 모든 유저 영역을 0으로 초기화 하므로 쉘 코드를 삽입할 마땅한 곳을 찾을 수 없다.

이전 문제에서 사용했던 방식처럼 버퍼의 마지막 영역도 사용이 불가능하다.

그럼 이 문제는 어떻게 접근해야 하는가..

<br>

이 문제는 **[리눅스 공유 라이브러리][ld]**에 대한 사전 지식을 필요로 한다.

시스템은 `argv`와 같은 인자 뿐만 아니라 **공유 라이브러리**를 메모리에 로드한다.

프로그램을 실행했을 때 `loader`는 해당 프로그램 구동에 필요한 공유 라이브러리를 찾는다.

<br>

이 문제에서 사용할 공유 라이브러리는 대표적으로 후킹(Hooking)에서 많이 사용되는

**LD_PRELOAD**라는 환경변수이다.

해당 변수가 설정되어 있는 경우 변수에 지정된 라이브러리를 최우선으로 메모리에 로드하게 된다.

이 환경변수 안에 쉘코드를 삽입해 리턴 값을 변경하여 문제를 해결할 수 있을 것이다.

<br>

# Analysis

먼저 바이너리가 동적 링크(dynamically linked) 인지 확인이 필요하다.

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
[skeleton@localhost skeleton]$
```



# Exploit



[ld]: https://www.lesstif.com/pages/viewpage.action?pageId=12943542#id-동적라이브러리(sharedlibrary)와Linker/Loader이해하기-공유라이브러리(sharedlibrary)