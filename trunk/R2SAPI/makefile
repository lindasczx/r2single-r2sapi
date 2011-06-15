CC=gcc
RC=windres
LINK=dllwrap
LIB=lib
CFLAGS= -c -O3
CXXFLAGS= -c -O3
SBFLAGS= -Wl,--add-stdcall-alias
IMPLIB=libcrypto.a libz.a c_g18030.lib
IMPOBJ=stdc.o compress.o uncompr.o crc32.o
LINKOBJ=fileio.o lzssdec.o mxmlparser.o r2sapi.o utf.o r2sapi.res
BIN=r2sapi.dll


all:$(BIN)

clean:
	del $(BIN) $(LINKOBJ)

cleanobj:
	del $(LINKOBJ)

$(BIN):$(LINKOBJ)
	$(LINK) -s -o $(BIN) $(LINKOBJ) $(IMPOBJ) $(IMPLIB) --def r2sapi.def
	$(LIB) /nologo /def:r2sapi.def $(LINKOBJ) $(IMPOBJ) $(IMPLIB) /out:r2sapi.lib

fileio.o:../r2pak/fileio.cpp
	$(CC) ../r2pak/fileio.cpp $(CXXFLAGS) -D_USRDLL

lzssdec.o:../r2pak/lzssdec.cpp
	$(CC) ../r2pak/lzssdec.cpp $(CXXFLAGS) -D_USRDLL

mxmlparser.o:../../../mxmlparser.cpp
	$(CC) ../../../mxmlparser.cpp $(CXXFLAGS)

utf.o:utf.c
	$(CC) utf.c $(CFLAGS)

r2sapi.o:r2sapi.cpp
	$(CC) r2sapi.cpp $(CXXFLAGS)

r2sapi.res:r2sapi.rc
	$(RC) r2sapi.rc -o r2sapi.res -O coff

