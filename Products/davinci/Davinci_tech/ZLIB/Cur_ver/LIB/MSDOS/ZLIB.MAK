# Microsoft Visual C++ generated build script - Do not modify

PROJ = ZLIB
DEBUG = 0
PROGTYPE = 4
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = Y:\TECH\ZLIB\CUR_VER\LIB\MSDOS\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = DEFLATE.C   
FIRSTCPP =             
RC = rc
CFLAGS_D_LIB = /nologo /G2 /W3 /Z7 /AM /Od /D "_DEBUG" /FR /GA 
CFLAGS_R_LIB = /nologo /f- /G2 /Zp1 /W3 /AL /Ox /D "NDEBUG" /FR /GA 
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_LIB)
LFLAGS = 
LIBS = 
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_LIB)
LFLAGS = 
LIBS = 
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = INFBLOCK.SBR \
		INFCODES.SBR \
		INFFAST.SBR \
		INFLATE.SBR \
		INFTREES.SBR \
		INFUTIL.SBR \
		TREES.SBR \
		ZUTIL.SBR \
		DEFLATE.SBR


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


DEFLATE_DEP = y:\tech\zlib\cur_ver\lib\deflate.h \
	y:\tech\zlib\cur_ver\lib\zutil.h \
	y:\tech\zlib\cur_ver\export\zlib.h


all:	$(PROJ).LIB $(PROJ).BSC

INFBLOCK.OBJ:	..\INFBLOCK.C $(INFBLOCK_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\INFBLOCK.C

INFCODES.OBJ:	..\INFCODES.C $(INFCODES_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\INFCODES.C

INFFAST.OBJ:	..\INFFAST.C $(INFFAST_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\INFFAST.C

INFLATE.OBJ:	..\INFLATE.C $(INFLATE_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\INFLATE.C

INFTREES.OBJ:	..\INFTREES.C $(INFTREES_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\INFTREES.C

INFUTIL.OBJ:	..\INFUTIL.C $(INFUTIL_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\INFUTIL.C

TREES.OBJ:	..\TREES.C $(TREES_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\TREES.C

ZUTIL.OBJ:	..\ZUTIL.C $(ZUTIL_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\ZUTIL.C

DEFLATE.OBJ:	..\DEFLATE.C $(DEFLATE_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c ..\DEFLATE.C

$(PROJ).LIB::	INFBLOCK.OBJ INFCODES.OBJ INFFAST.OBJ INFLATE.OBJ INFTREES.OBJ INFUTIL.OBJ \
	TREES.OBJ ZUTIL.OBJ DEFLATE.OBJ $(OBJS_EXT)
	echo >NUL @<<$(PROJ).CRF
$@ /PAGESIZE:64
y
+INFBLOCK.OBJ &
+INFCODES.OBJ &
+INFFAST.OBJ &
+INFLATE.OBJ &
+INFTREES.OBJ &
+INFUTIL.OBJ &
+TREES.OBJ &
+ZUTIL.OBJ &
+DEFLATE.OBJ &
;
<<
	if exist $@ del $@
	lib @$(PROJ).CRF

$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
