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
ASM:=		as
RC:=		windres
LINK:=		dllwrap
LIB:=		lib
MIDL:=		midl
RM:=		rm

GCCFLAGS:=	-O3 -I. -march=i686 -D_USRDLL -DL_ENDIAN -DDSO_WIN32 -DMD5_ASM -DRMD160_ASM -DSHA1_ASM -DSHA256_ASM -DSHA512_ASM -DWHIRLPOOL_ASM -DZLIB_WINAPI -DASMV -D_LARGEFILE64_SOURCE -D_LFS64_LARGEFILE -Wall -Wno-unknown-pragmas -Wno-parentheses -Wno-comments
CFLAGS:=	$(GCCFLAGS)
CXXFLAGS:=	$(GCCFLAGS)
LINKFLAGS:=	-s -Wl,--subsystem,console:5.1 --image-base 0x65000000 -Wl,--insert-timestamp -Wl,--enable-stdcall-fixup
MIDLFLAGS:=	
ifneq ($(NO_MSVC_SDK), FALSE)
MIDLFLAGS:=	-I include
endif
#SBFLAGS:=	-Wl,--add-stdcall-alias
IMPLIB:=	-loleaut32 c_g18030.lib
IMPOBJ:=	
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
CSRC+=		nettle/memxor.c
CSRC+=		nettle/sha3.c
CSRC+=		nettle/sha3-224.c
CSRC+=		nettle/sha3-256.c
CSRC+=		nettle/sha3-384.c
CSRC+=		nettle/sha3-512.c
CSRC+=		nettle/sha3-permute.c
CSRC+=		nettle/shake128.c
CSRC+=		nettle/shake256.c
CSRC+=		nettle/write-le64.c
CSRC+=		openssl/md4_dgst.c
CSRC+=		openssl/md4_one.c
CSRC+=		openssl/md5_dgst.c
CSRC+=		openssl/md5_one.c
CSRC+=		openssl/mdc2dgst.c
CSRC+=		openssl/mdc2_one.c
CSRC+=		openssl/mem_clr.c
CSRC+=		openssl/rmd_dgst.c
CSRC+=		openssl/rmd_one.c
CSRC+=		openssl/set_key.c
CSRC+=		openssl/sha1dgst.c
CSRC+=		openssl/sha1_one.c
CSRC+=		openssl/sha256.c
CSRC+=		openssl/sha512.c
CSRC+=		openssl/wp_block.c
CSRC+=		openssl/wp_dgst.c
CSRC+=		sha3.c
CSRC+=		sha512-t.c
CSRC+=		sm3.c
CSRC+=		stdc.c
CSRC+=		utf.c
CSRC+=		utf7.c
CSRC+=		zlib/adler32.c
CSRC+=		zlib/compress.c
CSRC+=		zlib/deflate.c
CSRC+=		zlib/inffast.c
CSRC+=		zlib/inflate.c
CSRC+=		zlib/inftrees.c
CSRC+=		zlib/trees.c
CSRC+=		zlib/uncompr.c
CSRC+=		zlib/zutil.c
CSRCSSE:=	blake2s_sse.c
CXXSRC:=	
CXXSRCSSE:=	
ASMSRC:=	openssl/des-586.s
ASMSRC+=	openssl/md5-586.s
ASMSRC+=	openssl/rmd-586.s
ASMSRC+=	openssl/sha1-586.s
ASMSRC+=	openssl/sha256-586.s
ASMSRC+=	openssl/sha512-586.s
ASMSRC+=	openssl/wp-mmx.s
ASMSRC+=	zlib/match.S
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
ASMOBJ:=	$(ASMSRC:%.s=%.o)
ASMOBJ:=	$(ASMOBJ:%.S=%.o)

RES:=		r2sapi.res
LINKOBJ:=	$(COBJ) $(COBJSSE) $(CXXOBJ) $(CXXOBJSSE) $(ASMOBJ) $(RES)
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

%.o:%.s
	$(CC) -o $@ -c $<

%.o:%.S
	$(CC) -o $@ -c $<

