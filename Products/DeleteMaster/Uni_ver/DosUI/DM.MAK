# Microsoft Visual C++ generated build script - Do not modify

PROJ = DM
DEBUG = 0
PROGTYPE = 6
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = E:\PRODUCTS\DELETE~1\CUR_VER\DOSUI\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC =             
FIRSTCPP = MAIN.CPP    
RC = rc
CFLAGS_D_DEXE = /nologo /G3 /Zp1 /W3 /Zi /AL /Od /D "_DEBUG" /D "_DOS" /FR /Fd"DM.PDB"
CFLAGS_R_DEXE = /nologo /Gs /G3 /Zp1 /W3 /AL /Ox /D "NDEBUG" /D "_DOS" /FR 
LFLAGS_D_DEXE = /NOLOGO /NOI /STACK:5120 /ONERROR:NOEXE /CO 
LFLAGS_R_DEXE = /NOLOGO /NOI /STACK:5120 /ONERROR:NOEXE 
LIBS_D_DEXE = oldnames llibce 
LIBS_R_DEXE = oldnames llibce 
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
OBJS_EXT = 
LIBS_EXT = ..\..\..\..\TECH\PARINFO\CUR_VER\MSDOS\PARINFO.LIB 
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
SBRS = CONFIRM.SBR \
		DELDISK.SBR \
		DELPAR.SBR \
		DELPROC.SBR \
		DISKFUN.SBR \
		DISKVIEW.SBR \
		FINISH.SBR \
		IDENTIFY.SBR \
		INTRRPT.SBR \
		MAIN.SBR \
		MAINWIN.SBR \
		MOTHED.SBR \
		SELSEC.SBR \
		WIN.SBR \
		WRITELOG.SBR


PARINFO_DEP = 

CONFIRM_DEP = e:\products\delete~1\cur_ver\dosui\comm.h \
	e:\products\delete~1\cur_ver\dosui\main.h \
	e:\tech\parinfo\cur_ver\msdos\parinfo.h


DELDISK_DEP = e:\products\delete~1\cur_ver\dosui\comm.h \
	e:\products\delete~1\cur_ver\dosui\main.h \
	e:\tech\parinfo\cur_ver\msdos\parinfo.h


DELPAR_DEP = e:\products\delete~1\cur_ver\dosui\comm.h \
	e:\products\delete~1\cur_ver\dosui\main.h \
	e:\tech\parinfo\cur_ver\msdos\parinfo.h


DELPROC_DEP = e:\products\delete~1\cur_ver\dosui\comm.h \
	e:\products\delete~1\cur_ver\dosui\main.h \
	e:\tech\parinfo\cur_ver\msdos\parinfo.h


DISKFUN_DEP = e:\products\delete~1\cur_ver\dosui\comm.h \
	e:\products\delete~1\cur_ver\dosui\main.h \
	e:\tech\parinfo\cur_ver\msdos\parinfo.h \
	e:\products\delete~1\cur_ver\dosui\intrrpt.h


DISKVIEW_DEP = e:\products\delete~1\cur_ver\dosui\comm.h \
	e:\products\delete~1\cur_ver\dosui\main.h \
	e:\tech\parinfo\cur_ver\msdos\parinfo.h


FINISH_DEP = e:\products\delete~1\cur_ver\dosui\comm.h \
	e:\products\delete~1\cur_ver\dosui\main.h \
	e:\tech\parinfo\cur_ver\msdos\parinfo.h


IDENTIFY_DEP = e:\products\delete~1\cur_ver\dosui\comm.h \
	e:\products\delete~1\cur_ver\dosui\main.h \
	e:\tech\parinfo\cur_ver\msdos\parinfo.h \
	e:\products\delete~1\cur_ver\dosui\intrrpt.h


INTRRPT_DEP = e:\products\delete~1\cur_ver\dosui\intrrpt.h \
	e:\products\delete~1\cur_ver\dosui\comm.h


MAIN_DEP = e:\products\delete~1\cur_ver\dosui\comm.h \
	e:\products\delete~1\cur_ver\dosui\main.h \
	e:\tech\parinfo\cur_ver\msdos\parinfo.h \
	e:\products\delete~1\cur_ver\dosui\intrrpt.h


MAINWIN_DEP = e:\products\delete~1\cur_ver\dosui\comm.h \
	e:\products\delete~1\cur_ver\dosui\main.h \
	e:\tech\parinfo\cur_ver\msdos\parinfo.h


MOTHED_DEP = e:\products\delete~1\cur_ver\dosui\comm.h \
	e:\products\delete~1\cur_ver\dosui\main.h \
	e:\tech\parinfo\cur_ver\msdos\parinfo.h


SELSEC_DEP = e:\products\delete~1\cur_ver\dosui\comm.h \
	e:\products\delete~1\cur_ver\dosui\main.h \
	e:\tech\parinfo\cur_ver\msdos\parinfo.h


WIN_DEP = e:\products\delete~1\cur_ver\dosui\comm.h \
	e:\products\delete~1\cur_ver\dosui\main.h \
	e:\tech\parinfo\cur_ver\msdos\parinfo.h


WRITELOG_DEP = e:\products\delete~1\cur_ver\dosui\comm.h \
	e:\products\delete~1\cur_ver\dosui\main.h \
	e:\tech\parinfo\cur_ver\msdos\parinfo.h


all:	$(PROJ).EXE $(PROJ).BSC

CONFIRM.OBJ:	CONFIRM.CPP $(CONFIRM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c CONFIRM.CPP

DELDISK.OBJ:	DELDISK.CPP $(DELDISK_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c DELDISK.CPP

DELPAR.OBJ:	DELPAR.CPP $(DELPAR_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c DELPAR.CPP

DELPROC.OBJ:	DELPROC.CPP $(DELPROC_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c DELPROC.CPP

DISKFUN.OBJ:	DISKFUN.CPP $(DISKFUN_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c DISKFUN.CPP

DISKVIEW.OBJ:	DISKVIEW.CPP $(DISKVIEW_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c DISKVIEW.CPP

FINISH.OBJ:	FINISH.CPP $(FINISH_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c FINISH.CPP

IDENTIFY.OBJ:	IDENTIFY.CPP $(IDENTIFY_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c IDENTIFY.CPP

INTRRPT.OBJ:	INTRRPT.CPP $(INTRRPT_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c INTRRPT.CPP

MAIN.OBJ:	MAIN.CPP $(MAIN_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c MAIN.CPP

MAINWIN.OBJ:	MAINWIN.CPP $(MAINWIN_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MAINWIN.CPP

MOTHED.OBJ:	MOTHED.CPP $(MOTHED_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MOTHED.CPP

SELSEC.OBJ:	SELSEC.CPP $(SELSEC_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SELSEC.CPP

WIN.OBJ:	WIN.CPP $(WIN_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c WIN.CPP

WRITELOG.OBJ:	WRITELOG.CPP $(WRITELOG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c WRITELOG.CPP

$(PROJ).EXE::	CONFIRM.OBJ DELDISK.OBJ DELPAR.OBJ DELPROC.OBJ DISKFUN.OBJ DISKVIEW.OBJ \
	FINISH.OBJ IDENTIFY.OBJ INTRRPT.OBJ MAIN.OBJ MAINWIN.OBJ MOTHED.OBJ SELSEC.OBJ WIN.OBJ \
	WRITELOG.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
CONFIRM.OBJ +
DELDISK.OBJ +
DELPAR.OBJ +
DELPROC.OBJ +
DISKFUN.OBJ +
DISKVIEW.OBJ +
FINISH.OBJ +
IDENTIFY.OBJ +
INTRRPT.OBJ +
MAIN.OBJ +
MAINWIN.OBJ +
MOTHED.OBJ +
SELSEC.OBJ +
WIN.OBJ +
WRITELOG.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
c:\msvc\lib\+
c:\msvc\mfc\lib\+
..\..\..\..\TECH\PARINFO\CUR_VER\MSDOS\PARINFO.LIB+
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
