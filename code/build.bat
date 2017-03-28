@echo off

mkdir ..\build
pushd ..\build
cl -DHANDMADE_INTERNAL=1 -DHANDMADE_SLOW=1 -DHANDMADE_WIN32=1 -FC -Zi ..\code\handmadeProj\win32_handmade.cpp /link user32.lib gdi32.lib
popd

