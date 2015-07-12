@echo off

mkdir ..\build
pushd ..\build
cl -Zi ..\code\win32_handmade.cpp /link user32.lib
popd

