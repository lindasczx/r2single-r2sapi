# 编译要求：	Windows
#		GCC
#		GNU Binutils
#		GNU Make
#
# 测试平台：	Windows XP SP3
#		MinGW 4.0.0 (GCC 4.8.1 + Binutils 2.24 + Make 3.82)
#

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

OUTDIR:=	Release/
CFLAGS:=	-O3 -I.
CXXFLAGS:=	-O3 -I.
LINKFLAGS:=	-s -Wl,--subsystem,console:4.0 --image-base 0x62e40000 -Wl,--insert-timestamp
#SBFLAGS:=	-Wl,--add-stdcall-alias
IMPLIB:=	bass.lib mss32.lib
IMPOBJ:=	

CSRC:=		
CSRCSSE:=	
CXXSRC:=	swrap.cpp
CXXSRC+=	soundwrapper.cpp
CXXSRCSSE:=	

COBJ:=		$(CSRC:%.c=%.o)
COBJSSE:=	$(CSRCSSE:%.c=%.o)
CXXOBJ:=	$(CXXSRC:%.cpp=%.o)
CXXOBJSSE:=	$(CXXSRCSSE:%.cpp=%.o)

RES:=		$(OUTDIR)swrap.res
LINKOBJ:=	$(COBJ) $(COBJSSE) $(CXXOBJ) $(CXXOBJSSE) $(RES)
DEFFILE:=	swrap.def
#OUTLIB:=	$(OUTDIR)swrap.lib
BIN:=		$(OUTDIR)swrap.dll
DEBUGBIN:=	$(OUTDIR)swrapd.dll

.PHONY: all clean cleanobj

all:	$(BIN)

debug:	$(DEBUGBIN)
debug:	CFLAGS += -DDEBUG
debug:	CXXFLAGS += -DDEBUG

clean:
	$(RM) $(BIN) $(DEBUGBIN) $(LINKOBJ) *.o

cleanobj:
	$(RM) $(LINKOBJ) *.o

$(BIN): $(LINKOBJ)
	$(LINK) -o $(BIN) $(LINKOBJ) $(IMPOBJ) $(IMPLIB) --def $(DEFFILE) $(LINKFLAGS)
#	$(LIB) /nologo /def:$(DEFFILE) $(LINKOBJ) $(IMPOBJ) $(IMPLIB) /out:$(OUTLIB)

$(DEBUGBIN): $(LINKOBJ)
	$(LINK) -o $(DEBUGBIN) $(LINKOBJ) $(IMPOBJ) $(IMPLIB) --def $(DEFFILE) $(LINKFLAGS)

$(RES): swrap.rc
	$(RC) $< -o $(RES) -O coff

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


