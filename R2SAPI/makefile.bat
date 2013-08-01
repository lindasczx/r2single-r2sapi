@echo off
for /f "tokens=2,3" %%i in (build.h) do (
	if "%%i"=="APIVER" (
		set ver=%%j
		goto gotver
	)
)
:gotver
set /a ver+=1
echo #define APIVER %ver% >build.h

make clean
ar -x libz.a
make
pause
make cleanobj
