@echo off
set BUILDAMD64=1
make clean
make
pause
make cleanobj
set BUILDAMD64=0
