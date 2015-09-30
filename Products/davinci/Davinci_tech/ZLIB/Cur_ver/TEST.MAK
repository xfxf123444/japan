# Microsoft Visual C++ generated build script - Do not modify

PROJ = TEST
DEBUG = 1
PROGTYPE = 6
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = Y:\TECH\ZLIB\CUR_VER\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = DEFLATE.C   
FIRSTCPP = MAIN.CPP    
RC = rc
CFLAGS_D_DEXE = /nologo /G2 /Zp1 /W3 /Zi /AL /Od /D "_DEBUG" /D "_DOS" /FR /Fd"TEST.PDB"
CFLAGS_R_DEXE = /nologo /Gs /G2 /W3 /AM /Ox /D "NDEBUG" /D "_DOS" /FR 
LFLAGS_D_DEXE = /NOLOGO /ONERROR:NOEXE /NOI /CO /STACK:5120
LFLAGS_R_DEXE = /NOLOGO /ONERROR:NOEXE /NOI /STACK:5120
LIBS_D_DEXE = oldnames llibce
LIBS_R_DEXE = oldnames mlibce
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_DEXE)
LFLAGS = $(LFLAGS_D_DEXE)
LIBS = $(LIBS_D_DEXE)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_DEXE)
LFLAGS = $(LFLAGS_R_DEXE)
LIBS = $(LIBS_R_DEXE)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = DEFLATE.SBR \
		INFBLOCK.SBR \
		INFCODES.SBR \
		INFFAST.SBR \
		INFLATE.SBR \
		INFTREES.SBR \
		INFUTIL.SBR \
		TREES.SBR \
		ZUTIL.SBR \
		MAIN.SBR


DEFLATE_DEP = y:\tech\zlib\cur_ver\lib\deflate.h \
	y:\tech\zlib\cur_ver\lib\zutil.h \
	y:\tech\zlib\cur_ver\export\zlib.h


INFBLOCK_DEP = y:\tech\zlib\cur_ver\lib\zutil.h \
	y:\tech\zlib\cur_ver\export\zlib.h \
	y:\tech\zlib\cur_ver\lib\infblock.h \
	y:\tech\zlib\cur_ver\lib\inftrees.h \
	y:\tech\zlib\cur_ver\lib\infcodes.h \
	y:\tech\zlib\cur_ver\lib\infutil.h \
	y:\tech\zlib\cur_ver\lib\inffixed.h


INFCODES_DEP = y:\tech\zlib\cur_ver\lib\zutil.h \
	y:\tech\zlib\cur_ver\export\zlib.h \
	y:\tech\zlib\cur_ver\lib\inftrees.h \
	y:\tech\zlib\cur_ver\lib\infblock.h \
	y:\tech\zlib\cur_ver\lib\infcodes.h \
	y:\tech\zlib\cur_ver\lib\infutil.h \
	y:\tech\zlib\cur_ver\lib\inffast.h


INFFAST_DEP = y:\tech\zlib\cur_ver\lib\zutil.h \
	y:\tech\zlib\cur_ver\export\zlib.h \
	y:\tech\zlib\cur_ver\lib\inftrees.h \
	y:\tech\zlib\cur_ver\lib\infblock.h \
	y:\tech\zlib\cur_ver\lib\infcodes.h \
	y:\tech\zlib\cur_ver\lib\infutil.h \
	y:\tech\zlib\cur_ver\lib\inffast.h


INFLATE_DEP = y:\tech\zlib\cur_ver\lib\zutil.h \
	y:\tech\zlib\cur_ver\export\zlib.h \
	y:\tech\zlib\cur_ver\lib\infblock.h


INFTREES_DEP = y:\tech\zlib\cur_ver\lib\zutil.h \
	y:\tech\zlib\cur_ver\export\zlib.h \
	y:\tech\zlib\cur_ver\lib\inftrees.h


INFUTIL_DEP = y:\tech\zlib\cur_ver\lib\zutil.h \
	y:\tech\zlib\cur_ver\export\zlib.h \
	y:\tech\zlib\cur_ver\lib\infblock.h \
	y:\tech\zlib\cur_ver\lib\inftrees.h \
	y:\tech\zlib\cur_ver\lib\infcodes.h \
	y:\tech\zlib\cur_ver\lib\infutil.h


TREES_DEP = y:\tech\zlib\cur_ver\lib\deflate.h \
	y:\tech\zlib\cur_ver\lib\zutil.h \
	y:\tech\zlib\cur_ver\export\zlib.h \
	y:\tech\zlib\cur_ver\lib\trees.h


ZUTIL_DEP = y:\tech\zlib\cur_ver\lib\zutil.h \
	y:\tech\zlib\cur_ver\export\zlib.h \
	y:\tech\zlib\cur_ver\lib\deflate.h \
	y:\tech\zlib\cur_ver\lib\infblock.h


MAIN_DEP = y:\tech\zlib\cur_ver\main.h


all:	$(PROJ).EXE $(PROJ).BSC

DEFLATE.OBJ:	LIB\DEFLATE.C $(DEFLATE_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c LIB\DEFLATE.C

INFBLOCK.OBJ:	LIB\INFBLOCK.C $(INFBLOCK_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c LIB\INFBLOCK.C

INFCODES.OBJ:	LIB\INFCODES.C $(INFCODES_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c LIB\INFCODES.C

INFFAST.OBJ:	LIB\INFFAST.C $(INFFAST_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c LIB\INFFAST.C

INFLATE.OBJ:	LIB\INFLATE.C $(INFLATE_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c LIB\INFLATE.C

INFTREES.OBJ:	LIB\INFTREES.C $(INFTREES_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c LIB\INFTREES.C

INFUTIL.OBJ:	LIB\INFUTIL.C $(INFUTIL_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c LIB\INFUTIL.C

TREES.OBJ:	LIB\TREES.C $(TREES_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c LIB\TREES.C

ZUTIL.OBJ:	LIB\ZUTIL.C $(ZUTIL_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c LIB\ZUTIL.C

MAIN.OBJ:	MAIN.CPP $(MAIN_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c MAIN.CPP

$(PROJ).EXE::	DEFLATE.OBJ INFBLOCK.OBJ INFCODES.OBJ INFFAST.OBJ INFLATE.OBJ INFTREES.OBJ \
	INFUTIL.OBJ TREES.OBJ ZUTIL.OBJ MAIN.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
DEFLATE.OBJ +
INFBLOCK.OBJ +
INFCODES.OBJ +
INFFAST.OBJ +
INFLATE.OBJ +
INFTREES.OBJ +
INFUTIL.OBJ +
TREES.OBJ +
ZUTIL.OBJ +
MAIN.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
x:\msvc\lib\+
x:\msvc\mfc\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
