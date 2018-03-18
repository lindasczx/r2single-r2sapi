@echo off
set PATH=tool;%PATH%
set BUILDAMD64=1
mingw32-make clean
mingw32-make 2>&1 | tee build.log
pause
mingw32-make cleanobj
set BUILDAMD64=0
