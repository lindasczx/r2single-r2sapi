# 若编译出错，把下面这个变量改成TRUE即可
COMPILE_FROM_PUBLIC:=FALSE

CC:=		gcc
CXX:=		g++
RC:=		windres
LINK:=		dllwrap
LIB:=		lib
RM:=		rm

CFLAGS:=	-O3 -I. -D_USRDLL
CXXFLAGS:=	-O3 -I. -D_USRDLL
LINKFLAGS:=	-s -lstdc++
#SBFLAGS:=	-Wl,--add-stdcall-alias
IMPLIB:=	libcrypto.a libz.a c_g18030.lib
IMPOBJ:=	compress.o uncompr.o crc32.o
ifneq ($(COMPILE_FROM_PUBLIC), FALSE)
IMPOBJ:=	$(IMPOBJ) private/xmlparser.o private/fileio.o private/lzssdec.o
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
endif
CXXSRC:=	r2sapi.cpp
CXXSRC+=	blake2.cpp
CXXSRC+=	blake2s.cpp
ifeq ($(COMPILE_FROM_PUBLIC), FALSE)
CXXSRC+=	../r2pak/fileio.cpp
CXXSRC+=	../r2pak/lzssdec.cpp
endif

COBJ:=		$(CSRC:%.c=%.o)
CXXOBJ:=	$(CXXSRC:%.cpp=%.o)

RES:=		r2sapi.res
LINKOBJ:=	$(COBJ) $(CXXOBJ) $(RES)
TYPELIB:=	r2sapilib.tlb
DEFFILE:=	r2sapi.def
OUTLIB:=	r2sapi.lib
BIN:=		r2sapi.dll

.PHONY: all clean cleanobj

all:$(BIN)

clean:
	$(RM) $(BIN) $(LINKOBJ) $(TYPELIB) *.o

cleanobj:
	$(RM) $(LINKOBJ) *.o

$(BIN): $(LINKOBJ)
	$(LINK) -o $(BIN) $(LINKOBJ) $(IMPOBJ) $(IMPLIB) --def $(DEFFILE) $(LINKFLAGS)
	$(LIB) /nologo /def:$(DEFFILE) $(LINKOBJ) $(IMPOBJ) $(IMPLIB) /out:$(OUTLIB)

$(RES): r2sapi.rc $(TYPELIB)
	$(RC) $< -o $(RES) -O coff

$(TYPELIB): r2sapilib.idl
	midl $< /tlb $@

$(COBJ): $(CSRC)

$(CXXOBJ): $(CXXSRC)

%.o:%.c
	$(CC) -o $@ $(CFLAGS) -c $<

%.o:%.cpp
	$(CXX) -o $@ $(CXXFLAGS) -c $<

