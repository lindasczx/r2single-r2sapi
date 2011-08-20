CC=		gcc
RC=		windres
LINK=		dllwrap
LIB=		lib
CFLAGS=		-c -O3
CXXFLAGS=	-c -O3
SBFLAGS=	-Wl,--add-stdcall-alias
IMPLIB=		libcrypto.a libz.a c_g18030.lib
IMPOBJ=		stdc.o compress.o uncompr.o crc32.o
LINKOBJ=	fileio.o lzssdec.o mxmlparser.o r2sapi.o utf.o r2sapi.res
RES=		r2sapi.res
DEFFILE=	r2sapi.def
OUTLIB=		r2sapi.lib
BIN=		r2sapi.dll

all:$(BIN)

clean:
	del $(BIN) $(LINKOBJ)

cleanobj:
	del $(LINKOBJ)

$(BIN): $(LINKOBJ)
	$(LINK) -s -o $(BIN) $(LINKOBJ) $(IMPOBJ) $(IMPLIB) --def $(DEFFILE)
	$(LIB) /nologo /def:$(DEFFILE) $(LINKOBJ) $(IMPOBJ) $(IMPLIB) /out:$(OUTLIB)

$(RES): r2sapi.rc
	$(RC) $? -o $(RES) -O coff

r2sapi.o: r2sapi.cpp
	$(CC) $? $(CXXFLAGS)

fileio.o: ../r2pak/fileio.cpp
	$(CC) $? $(CXXFLAGS) -D_USRDLL

lzssdec.o: ../r2pak/lzssdec.cpp
	$(CC) $? $(CXXFLAGS) -D_USRDLL

mxmlparser.o: ../../../mxmlparser.cpp
	$(CC) $? $(CXXFLAGS)

utf.o: utf.c
	$(CC) $? $(CFLAGS)

