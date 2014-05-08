# Makefile for MinGW32 GCC make/nmake
#

CC = cl
RC = windres
LINK=link
OUTDIR=Release/
RES  = $(OUTDIR)swrap.res
OBJ  = $(OUTDIR)swrap.obj $(OUTDIR)soundwrapper.obj $(RES)
LINKOBJ  = $(OUTDIR)swrap.obj $(OUTDIR)soundwrapper.obj $(RES)
LIBS = /dll /nologo /def:swrap.def kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib shlwapi.lib bass.lib mss32.lib msvcrt60.lib
INCS =
BIN  = $(OUTDIR)swrap.dll
CFLAGS = -c -DBUILDING_DLL=1 $(INCS) /Fo$(OUTDIR) /O2 /MD
CXXFLAGS = -c -DBUILDING_DLL=1 $(INCS) /Fo$(OUTDIR) /O2 /MD
RM = rm -f
DEFFILE=
STATICLIB=

.PHONY: clean all
all: $(BIN)
clean:
	$(RM) $(OBJ) $(BIN) $(DEFFILE) $(STATICLIB)
clean-obj:
	$(RM) $(OBJ)
$(BIN): $(LINKOBJ)
	$(LINK) /OUT:$(BIN) $(LIBS) $(LINKOBJ)
$(OUTDIR)swrap.obj: swrap.cpp
	$(CC) swrap.cpp $(CFLAGS)
$(OUTDIR)soundwrapper.obj: soundwrapper.cpp
	$(CC) soundwrapper.cpp $(CXXFLAGS)
$(OUTDIR)swrap.res: swrap.rc 
	$(RC) -i swrap.rc --input-format=rc -o $(OUTDIR)swrap.res -O coff 

