CC:=		gcc
CXX:=		g++
RC:=		windres
LINK:=		dllwrap
LIB:=		lib
RM:=		rm

CFLAGS:=	-O3 -I. -D_USRDLL
CXXFLAGS:=	-O3 -I. -D_USRDLL
SBFLAGS:=	-Wl,--add-stdcall-alias
IMPLIB:=	libcrypto.a libz.a c_g18030.lib
IMPOBJ:=	compress.o uncompr.o crc32.o

CSRC:=		stdc.c
CSRC+=		utf.c
CSRC+=		md6_compress.c
CSRC+=		md6_mode.c
CSRC+=		md6.c
CXXSRC:=	r2sapi.cpp
CXXSRC+=	../../../mxmlparser.cpp
CXXSRC+=	../r2pak/fileio.cpp
CXXSRC+=	../r2pak/lzssdec.cpp

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
	$(RM) $(BIN) $(LINKOBJ) $(TYPELIB)

cleanobj:
	$(RM) $(LINKOBJ)

$(BIN): $(LINKOBJ)
	$(LINK) -s -o $(BIN) $(LINKOBJ) $(IMPOBJ) $(IMPLIB) --def $(DEFFILE)
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

