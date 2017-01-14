# 编译要求：	Windows
#		GCC
#		GNU Binutils 2.24以上 (除了2.25.0以外均可)
#		GNU Make
#		MSVC 7.0以上 (cl + lib + link，可选，工具包另有提供)
#		Windows SDK (midl + midlc + 头文件，可选，工具包另有提供)
#		如果忽略可选项，请自己改makefile
#
# 测试平台：	Windows 7 SP1
#		MinGW-W64 32位套件 (i686-6.2.0-release-win32-sjlj-rt_v5-rev1)
#		MinGW-W64 64位套件 (x86_64-6.2.0-release-win32-seh-rt_v5-rev1)
#		MSVC 2010 SP1 (cl + lib + link 10.0.40219.1)
#		WinSDK 7.1 (midl + midlc 7.0.555.1)
#
# 运行平台：	32位版：Windows XP以上 (Windows 2000需要安装GB18030支持和msvcrt.dll 7.0版)
#		x64版：所有基于x64的Windows系统
#
# 若编译出错，顾名思义把下面两个变量改成TRUE即可。
# 编译64位版本时，请确保下面的各种带64的程序能够运行，否则请修改相应变量。
COMPILE_FROM_PUBLIC:=FALSE
NO_MSVC_SDK:=	FALSE

ifneq ($(NO_MSVC_SDK), FALSE)
ifneq ($(BUILDAMD64), 1)
export PATH:=	tool;tool/x86;$(PATH)
else
export PATH:=	tool;tool/amd64;$(PATH)
endif
endif

ifneq ($(BUILDAMD64), 1)
CC:=		gcc
CXX:=		g++
ASM:=		as
RC:=		windres
LINK:=		ld
LIB:=		lib
MIDL:=		midl
else
CC:=		gcc64
CXX:=		g++64
ASM:=		as64
RC:=		windres64
LINK:=		ld64
LIB:=		lib64
MIDL:=		midl
endif
RM:=		rm
CP:=		cp

GCCFLAGS:=	-O3 -I. -municode -D_USRDLL -DL_ENDIAN -DDSO_WIN32 -DMD5_ASM -DSHA1_ASM -DSHA256_ASM -DSHA512_ASM -DWHIRLPOOL_ASM -DZLIB_WINAPI -DASMV -D_LARGEFILE64_SOURCE -D_LFS64_LARGEFILE -Wall -Wno-unknown-pragmas -Wno-parentheses -Wno-comments
X86FLAGS:=	-m32 -march=i686 -DRMD160_ASM
AMD64FLAGS:=	-m64
ifneq ($(BUILDAMD64), 1)
CFLAGS:=	$(GCCFLAGS) $(X86FLAGS)
CXXFLAGS:=	$(GCCFLAGS) $(X86FLAGS)
ASMFLAGS:=	
else
CFLAGS:=	$(GCCFLAGS) $(AMD64FLAGS)
CXXFLAGS:=	$(GCCFLAGS) $(AMD64FLAGS)
ASMFLAGS:=	-DNO_UNDERLINE
endif
LINKFLAGS:=	-s -static -Wl,--subsystem,console:5.1 -Wl,--image-base,0x65000000 -Wl,--insert-timestamp -Wl,--enable-stdcall-fixup
MIDLFLAGS:=	
ifneq ($(NO_MSVC_SDK), FALSE)
MIDLFLAGS:=	-I include
endif

ifneq ($(BUILDAMD64), 1)
IMPLIB:=	-loleaut32 x86/c_g18030.lib
else
IMPLIB:=	-loleaut32 amd64/c_g18030.lib
endif

IMPOBJ:=	
ifneq ($(COMPILE_FROM_PUBLIC), FALSE)
ifneq ($(BUILDAMD64), 1)
IMPOBJ:=	$(IMPOBJ) x86/private/xmlparser.o x86/private/fileio.o x86/private/lzssdec.o x86/private/lzssenc.o
else
IMPOBJ:=	$(IMPOBJ) amd64/private/xmlparser.o amd64/private/fileio.o amd64/private/lzssdec.o amd64/private/lzssenc.o
endif
endif

CSRC:=		r2sapi.c
CSRCSSE2:=
CSRCSSE3:=
CXXSRC:=	
CXXSRCSSE:=	
ASMSRC:=
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
CSRC+=		openssl/wp_dgst.c
CSRC+=		sha3.c
CSRC+=		sha512-t.c
CSRC+=		sm3.c
CSRC+=		sort.c
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
ifneq ($(BUILDAMD64), 1)
CSRCSSE2+=	blake2s_sse.c
CSRC+=		nettle/sha3-permute.c
ASMSRC+=	openssl/des-586.s
ASMSRC+=	openssl/md5-586.s
ASMSRC+=	openssl/rmd-586.s
ASMSRC+=	openssl/sha1-586.s
ASMSRC+=	openssl/sha256-586.s
ASMSRC+=	openssl/sha512-586.s
CSRC+=		openssl/wp_block.c
ASMSRC+=	openssl/wp-mmx.s
ASMSRC+=	zlib/match.S
else
CSRCSSE3+=	blake2s_sse.c
ASMSRC+=	nettle/sha3-permute-amd64.s
CSRC+=		openssl/des_enc.c
ASMSRC+=	openssl/md5-x86_64.s
ASMSRC+=	openssl/sha1-x86_64.s
ASMSRC+=	openssl/sha256-x86_64.s
ASMSRC+=	openssl/sha512-x86_64.s
ASMSRC+=	openssl/wp-x86_64.s
ASMSRC+=	zlib/amd64-match.S
endif
ifeq ($(COMPILE_FROM_PUBLIC), FALSE)
CSRC+=		../../../xmlparser.c
CSRC+=		../r2pak/lzssdec.c
CSRC+=		../r2pak/lzssdecxor.c
CSRC+=		../r2pak/lzssenc.c
CSRC+=		../r2pak/lzssencxor.c
CXXSRC+=	../r2pak/fileio.cpp
endif

COBJ:=		$(CSRC:%.c=%.o)
COBJSSE2:=	$(CSRCSSE2:%.c=%.o)
COBJSSE3:=	$(CSRCSSE3:%.c=%.o)
CXXOBJ:=	$(CXXSRC:%.cpp=%.o)
CXXOBJSSE:=	$(CXXSRCSSE:%.cpp=%.o)
ASMOBJ:=	$(ASMSRC:%.s=%.o)
ASMOBJ:=	$(ASMOBJ:%.S=%.o)

RES:=		r2sapi.res
LINKOBJ:=	$(COBJ) $(COBJSSE2) $(COBJSSE3) $(CXXOBJ) $(CXXOBJSSE) $(ASMOBJ) $(RES)
TYPELIB:=	r2sapilib.tlb
ifneq ($(BUILDAMD64), 1)
DEFFILE:=	r2sapi.def
OUTLIB:=	r2sapi.lib
BIN:=		r2sapi.dll
else
DEFFILE:=	r2sapi64.def
OUTLIB:=	r2sapi64.lib
BIN:=		r2sapi64.dll
endif
DESTDIR:=	bin

.PHONY: all clean cleanobj install

all:	$(BIN)

clean:
	-$(RM) $(BIN) $(OUTLIB) $(LINKOBJ) $(TYPELIB) *.o

cleanobj:
	-$(RM) $(LINKOBJ) $(TYPELIB) *.o

install:
	$(CP) $(BIN) $(OUTLIB) $(DESTDIR)

$(BIN): $(LINKOBJ)
	$(CC) -shared -o $(BIN) $(LINKOBJ) $(IMPOBJ) $(IMPLIB) $(DEFFILE) $(LINKFLAGS)
	$(LIB) /nologo /def:$(DEFFILE) $(LINKOBJ) $(IMPOBJ) $(IMPLIB) /out:$(OUTLIB) /name:$(BIN)

$(RES): r2sapi.rc $(TYPELIB)
	$(RC) $< -o $(RES) -O coff

$(TYPELIB): r2sapilib.idl
	$(MIDL) $(MIDLFLAGS) $< /tlb $@

$(COBJ): $(CSRC)

$(COBJSSE2): $(CSRCSSE2)
$(COBJSSE2): CFLAGS += -msse2

$(COBJSSE3): $(CSRCSSE3)
$(COBJSSE3): CFLAGS += -mssse3

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
	$(CC) -o $@ $(ASMFLAGS) -c $<

