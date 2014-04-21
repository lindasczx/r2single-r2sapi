# 编译要求：	Windows
#		GCC
#		GNU Binutils
#		GNU Make
#		MSVC (lib.exe + link.exe 用于生成静态导入库文件，可选)
#		Windows SDK 2001年以后的版本 (midl.exe + midlc.exe 5.3以上，用于生成类型库，可选)
#		如果忽略可选项，请自己改makefile和资源文件
#
# 测试平台：	Windows XP SP3
#		MinGW 4.0.0 (GCC 4.8.1 + Binutils 2.24 + Make 3.82)
#		MSVC 2010 SP1 (lib.exe + link.exe 10.0.40219.1)
#		WinSDK 7.1 (midl.exe + midlc.exe 7.0.555.1)
#
# 若编译出错，把下面这个变量改成TRUE即可
COMPILE_FROM_PUBLIC:=FALSE

CC:=		gcc
CXX:=		g++
RC:=		windres
LINK:=		dllwrap
LIB:=		lib
MIDL:=		midl
RM:=		rm

CFLAGS:=	-O3 -I. -D_USRDLL
CXXFLAGS:=	-O3 -I. -D_USRDLL
LINKFLAGS:=	-s -lstdc++ -Wl,--subsystem,console:4.0 --image-base 0x65000000
#SBFLAGS:=	-Wl,--add-stdcall-alias
IMPLIB:=	libcrypto.a libz.a c_g18030.lib
IMPOBJ:=	compress.o uncompr.o crc32.o
ifneq ($(COMPILE_FROM_PUBLIC), FALSE)
IMPOBJ:=	$(IMPOBJ) private/xmlparser.o private/fileio.o private/lzssdec.o private/lzssenc.o
endif

CSRC:=		stdc.c
CSRC+=		sha512-t.c
CSRC+=		utf.c
CSRC+=		utf7.c
CSRC+=		md6_compress.c
CSRC+=		md6_mode.c
CSRC+=		md6.c
CSRC+=		sm3.c
ifeq ($(COMPILE_FROM_PUBLIC), FALSE)
CSRC+=		../../../xmlparser.c
CSRC+=		../r2pak/lzssdec.c
CSRC+=		../r2pak/lzssenc.c
endif
CXXSRC:=	r2sapi.cpp
CXXSRC+=	blake2.cpp
CXXSRC+=	blake2s.cpp
ifeq ($(COMPILE_FROM_PUBLIC), FALSE)
CXXSRC+=	../r2pak/fileio.cpp
endif

COBJ:=		$(CSRC:%.c=%.o)
CXXOBJ:=	$(CXXSRC:%.cpp=%.o)

RES:=		r2sapi.res
LINKOBJ:=	$(COBJ) $(CXXOBJ) $(RES)
TYPELIB:=	r2sapilib.tlb
DEFFILE:=	r2sapi.def
OUTLIB:=	bin/r2sapi.lib
BIN:=		bin/r2sapi.dll

.PHONY: all clean cleanobj

all:	$(BIN)

clean:
	$(RM) $(BIN) $(LINKOBJ) $(TYPELIB) *.o

cleanobj:
	$(RM) $(LINKOBJ) $(TYPELIB) *.o

$(BIN): $(LINKOBJ)
	$(LINK) -o $(BIN) $(LINKOBJ) $(IMPOBJ) $(IMPLIB) --def $(DEFFILE) $(LINKFLAGS)
	$(LIB) /nologo /def:$(DEFFILE) $(LINKOBJ) $(IMPOBJ) $(IMPLIB) /out:$(OUTLIB)

$(RES): r2sapi.rc $(TYPELIB)
	$(RC) $< -o $(RES) -O coff

$(TYPELIB): r2sapilib.idl
	$(MIDL) $< /tlb $@

$(COBJ): $(CSRC)

$(CXXOBJ): $(CXXSRC)

%.o:%.c
	$(CC) -o $@ $(CFLAGS) -c $<

%.o:%.cpp
	$(CXX) -o $@ $(CXXFLAGS) -c $<

