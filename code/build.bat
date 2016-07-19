@echo off

mkdir ..\build
pushd ..\build
cl -F4194304 -FC -Zi ..\code\win32_handmade.cpp /link user32.lib gdi32.lib
popd

