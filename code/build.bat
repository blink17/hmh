@echo off

set CommonCompilerFlags=-MTd -nologo -fp:fast -Gm- -GR- -EHa- -Zo -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -wd4127 -DHANDMADE_INTERNAL=1 -DHANDMADE_SLOW=1 -DHANDMADE_WIN32=1 -FC -Z7
set CommonLinkerFlags= -incremental:no -opt:ref user32.lib gdi32.lib winmm.lib

REM TODO - can we just build noth with one exe?

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

REM 32-bit build
REM cl %CommonCompilerFlags% ..\codewin32_handmade.cpp /link -subsystem:windows,5.1 %CommonLinkerFlags%

REM 64-bit build
del *.pdb > NUL 2> NUL
REM Optimization switches /O2
echo WAITING FOR PDB > lock.tmp
cl %CommonCompilerFlags% ..\code\handmade.cpp -Fmhandmade.map -LD /link -incremental:no -opt:ref -PDB:handmade_%random%.pdb -EXPORT:GameGetSoundSamples -EXPORT:GameUpdateAndRender
del lock.tmp
cl %CommonCompilerFlags% ..\code\win32_handmade.cpp -Fmwin32_handmade.map /link %CommonLinkerFlags%
popd


REM goto 구문으로 코멘트를 처리하는 신박한 방법 
goto comment
[C/C++]
-Z7 일반 > 디버그 정보 형식 : 컴파일러에서 생성되는 디버깅 정보 형식을 지정한다. 이때 호환 가능한 링커 설정이 필요하다.
-W4 일반 > 경고 수준 : 컴파일러가 코드 오류를 처리하는 수준. 현재 4레벨
-WX 일반 > 경고를 오류로 처리
-Od 최적화 > 최적화 : 코드 최적화 사용 안함
-O1 최적화 > 최적화 : 크기 최적화
-O2 최적화 > 최적화 : 속도 최적화
-Ox 최적화 > 최적화 : 최대 최적화
-Oi 최적화 > 내장 함수 사용 : 내장 함수를 사용하면 더 빠르지만 더 큰 코드가 생성된다.
/Zo 커맨드라인 인라인에 추가 : 최적화 코드에 대해서 향상된 디버깅 정보를 추출한다. 최적화 코드는 로컬 변수를 위한 레지스터 사용, 코드 재배치, 루프의 벡터라이즈,
그리고 인라인 펑션 콜 등이 있다. 이러한 최적화는 소스코드와 컴파일된 오브젝트 코드 사이를 가릴 수 있다. 이 옵션을 주면 컴파일러에게 로컬 변수와 인라인 함수에 대한
추가적인 정보를 생성하게 한다. 이것을 사용하면 Autos, Locals, Watch 윈도우에서 비주얼 스튜디오 디버거에서 최적화된 코드를 조사할때 변수들을 볼 수 있다.
또한 인라인 함수에 대한 스택트레이스도 볼 수 있다. /Od가 켜져있으면(최적화 사용안함) /Zo는 뭐 추가적인 작업을 할 필요없다. 다시말해 필요없는 옵션이다.
-Gm- 코드 생성 > 최소 다시 빌드 가능 : 변경된 C++ 클래스 정의(헤더(.h) 파일에 저장됨)를 포함하는 C++ 소스 파일을 다시 컴파일해야 하는지 여부를 결정하는 최소 다시 빌드를 사용한다. -> 아니요
-EHa- 코드 생성 > C++ 예외 처리 가능 : 컴파일러가 사용하는 예외 처리 모델을 지정. SEH 예외 있음.
-MT 코드 생성 > 런타임 라이브러리 : 다중 스레드 정적 CRT
-GR- 언어 > 런타임 형식 정보 사용 : 런타임에 C++ 개체의 형식(런타임 형식 정보)를 검사하는 코드를 추가한다. -> 아니요
-wd4201 -wd4100 -wd4189 고급 > 특정 경고 사용안함
-FC 진단 메시지에 전체 경로를 사용
-fp:fast 코드생성 > 부동 소수점 모델 : 부동 소수점 모델을 설정한다.


[링커]
-nologo 일반 > 시작 배너 표시 안 함 : 저작권 메시지와 버전 번호가 표시되지 않음.
-Fm 디버깅 > 맵 파일 생성, 맵 파일 이름 : 맵 파일을 만들도록 링커에 지시할 수 있다.
-subsystem:windows 시스템 > 하위 시스템 : 운영체제에서 .exe 파일을 실행하는 방법을 지시할 수 있다.
-opt:ref 최적화 > 참조 : 참조되지 않은 함수 및/또는 데이터가 제거
-incremental:no 일반 > 증분 링크 사용 : 증분 링크를 사용하지 않음
%date:~-4,4%%date:~-10,2%%date:~-7,2%_%time:~0,2%%time:~3,2%%time:~6,2%
%(date:~-4,4)%%(date:~-10,2)%%(date:~-7,2)%_%(time:~0,2)%%(time:~3,2)%%(time:~6,2)%
:comment