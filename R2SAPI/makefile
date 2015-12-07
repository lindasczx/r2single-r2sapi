# 编译要求：	Windows
#		GCC
#		GNU Binutils 2.24以上 (除了2.25.0以外均可)
#		GNU Make
#		MSVC 7.0以上 (cl + lib + link，可选，工具包另有提供)
#		Windows SDK (midl + midlc + 头文件，可选，工具包另有提供)
#		如果忽略可选项，请自己改makefile
#
# 测试平台：	Windows 7 SP1
#		MinGW-W64套件 (i686-5.2.0-release-win32-sjlj-rt_v4-rev1)
#		MSVC 2010 SP1 (cl + lib + link 10.0.40219.1)
#		WinSDK 7.1 (midl + midlc 7.0.555.1)
#
# 运行平台：	Windows XP以上 (Windows 2000需要安装GB18030支持和msvcrt.dll 7.0版)
#
# 若编译出错，顾名思义把下面两个变量改成TRUE即可
COMPILE_FROM_PUBLIC:=FALSE
NO_MSVC_SDK:=	FALSE

ifneq ($(NO_MSVC_SDK), FALSE)
export PATH:=	tool;$(PATH)
endif

CC:=		gcc
CXX:=		g++
RC:=		windres
LINK:=		dllwrap
LIB:=		lib
MIDL:=		midl
RM:=		rm

CFLAGS:=	-O3 -I. -D_USRDLL
CXXFLAGS:=	-O3 -I. -D_USRDLL
LINKFLAGS:=	-s -Wl,--subsystem,console:5.1 --image-base 0x65000000 -Wl,--insert-timestamp
MIDLFLAGS:=	
ifneq ($(NO_MSVC_SDK), FALSE)
MIDLFLAGS:=	-I include
endif
#SBFLAGS:=	-Wl,--add-stdcall-alias
IMPLIB:=	-loleaut32 libcrypto.a libz.a c_g18030.lib
IMPOBJ:=	compress.o uncompr.o
ifneq ($(COMPILE_FROM_PUBLIC), FALSE)
IMPOBJ:=	$(IMPOBJ) private/xmlparser.o private/fileio.o private/lzssdec.o private/lzssenc.o
endif

CSRC:=		r2sapi.c
CSRC+=		blake2.c
CSRC+=		blake2s.c
CSRC+=		blake2sp.c
CSRC+=		crc32.c
CSRC+=		crc64.c
CSRC+=		getcpuid.c
CSRC+=		i4helper.c
CSRC+=		i8conv.c
CSRC+=		i8helper.c
CSRC+=		md6.c
CSRC+=		md6_compress.c
CSRC+=		md6_mode.c
CSRC+=		sha512-t.c
CSRC+=		sm3.c
CSRC+=		stdc.c
CSRC+=		utf.c
CSRC+=		utf7.c
CSRCSSE:=	blake2s_sse.c
CXXSRC:=	
CXXSRCSSE:=	
ifeq ($(COMPILE_FROM_PUBLIC), FALSE)
CSRC+=		../../../xmlparser.c
CSRC+=		../r2pak/lzssdec.c
CSRC+=		../r2pak/lzssenc.c
CXXSRC+=	../r2pak/fileio.cpp
endif

COBJ:=		$(CSRC:%.c=%.o)
COBJSSE:=	$(CSRCSSE:%.c=%.o)
CXXOBJ:=	$(CXXSRC:%.cpp=%.o)
CXXOBJSSE:=	$(CXXSRCSSE:%.cpp=%.o)

RES:=		r2sapi.res
LINKOBJ:=	$(COBJ) $(COBJSSE) $(CXXOBJ) $(CXXOBJSSE) $(RES)
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
	$(MIDL) $(MIDLFLAGS) $< /tlb $@

$(COBJ): $(CSRC)

$(COBJSSE): $(CSRCSSE)
$(COBJSSE): CFLAGS += -msse2

$(CXXOBJ): $(CXXSRC)

$(CXXOBJSSE): $(CXXSRCSSE)
$(CXXOBJSSE): CXXFLAGS += -msse2

%.o:%.c
	$(CC) -o $@ $(CFLAGS) -c $<

%.o:%.cpp
	$(CXX) -o $@ $(CXXFLAGS) -c $<

