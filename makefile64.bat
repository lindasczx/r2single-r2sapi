@echo off
set BUILDAMD64=1
mingw32-make clean
mingw32-make
pause
mingw32-make cleanobj
set BUILDAMD64=0
