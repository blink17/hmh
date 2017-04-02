@echo off

set CommonCompilerFlags=-MT -nologo -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -DHANDMADE_INTERNAL=1 -DHANDMADE_SLOW=1 -DHANDMADE_WIN32=1 -FC -Z7
set CommonLinkerFlags= -incremental:no -opt:ref user32.lib gdi32.lib winmm.lib

REM TODO - can we just build noth with one exe?

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

REM 32-bit build
REM cl %CommonCompilerFlags% ..\codewin32_handmade.cpp /link -subsystem:windows,5.1 %CommonLinkerFlags%

REM 64-bit build
cl %CommonCompilerFlags% ..\code\handmade.cpp -Fmhandmade.map /LD /link /EXPORT:GameGetSoundSamples /EXPORT:GameUpdateAndRender
cl %CommonCompilerFlags% ..\code\win32_handmade.cpp -Fmwin32_handmade.map /link %CommonLinkerFlags%
popd


REM goto 구문으로 코멘트를 처리하는 신박한 방법 
goto comment
[C/C++]
-Z7 일반 > 디버그 정보 형식 : 컴파일러에서 생성되는 디버깅 정보 형식을 지정한다. 이때 호환 가능한 링커 설정이 필요하다.
-W4 일반 > 경고 수준 : 컴파일러가 코드 오류를 처리하는 수준. 현재 4레벨
-WX 일반 > 경고를 오류로 처리
-Od 최적화 > 최적화 : 코드 최적화 사용 안함
-Oi 최적화 > 내장 함수 사용 : 내장 함수를 사용하면 더 빠르지만 더 큰 코드가 생성된다.
-Gm- 코드 생성 > 최소 다시 빌드 가능 : 변경된 C++ 클래스 정의(헤더(.h) 파일에 저장됨)를 포함하는 C++ 소스 파일을 다시 컴파일해야 하는지 여부를 결정하는 최소 다시 빌드를 사용한다. -> 아니요
-EHa- 코드 생성 > C++ 예외 처리 가능 : 컴파일러가 사용하는 예외 처리 모델을 지정. SEH 예외 있음.
-MT 코드 생성 > 런타임 라이브러리 : 다중 스레드
-GR- 언어 > 런타임 형식 정보 사용 : 런타임에 C++ 개체의 형식(런타임 형식 정보)를 검사하는 코드를 추가한다. -> 아니요
-wd4201 -wd4100 -wd4189 고급 > 특정 경고 사용안함
-FC 진단 메시지에 전체 경로를 사용


[링커]
-nologo 일반 > 시작 배너 표시 안 함 : 저작권 메시지와 버전 번호가 표시되지 않음.
-Fm 디버깅 > 맵 파일 생성, 맵 파일 이름 : 맵 파일을 만들도록 링커에 지시할 수 있다.
-subsystem:windows 시스템 > 하위 시스템 : 운영체제에서 .exe 파일을 실행하는 방법을 지시할 수 있다.
-opt:ref 최적화 > 참조 : 참조되지 않은 함수 및/또는 데이터가 제거
-incremental:no 일반 > 증분 링크 사용 : 증분 링크를 사용하지 않음
:comment