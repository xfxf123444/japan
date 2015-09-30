# Microsoft Developer Studio Generated NMAKE File, Based on ParInfo.dsp
!IF "$(CFG)" == ""
CFG=ParInfo - Win32 Debug
!MESSAGE No configuration specified. Defaulting to ParInfo - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ParInfo - Win32 Release" && "$(CFG)" != "ParInfo - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ParInfo.mak" CFG="ParInfo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ParInfo - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ParInfo - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ParInfo - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\ParInfo.dll"


CLEAN :
	-@erase "$(INTDIR)\CVirPar.obj"
	-@erase "$(INTDIR)\CVxD.obj"
	-@erase "$(INTDIR)\ParInfo.obj"
	-@erase "$(INTDIR)\parinfo.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\ParInfo.dll"
	-@erase "$(OUTDIR)\ParInfo.exp"
	-@erase "$(OUTDIR)\ParInfo.lib"
	-@erase "$(OUTDIR)\ParInfo.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /Zp1 /MT /W3 /GX /O2 /D "WIN_9X" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PARINFO_EXPORTS" /Fp"$(INTDIR)\ParInfo.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\parinfo.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ParInfo.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\ParInfo.pdb" /map:"$(INTDIR)\ParInfo.map" /machine:I386 /out:"$(OUTDIR)\ParInfo.dll" /implib:"$(OUTDIR)\ParInfo.lib" 
LINK32_OBJS= \
	"$(INTDIR)\CVirPar.obj" \
	"$(INTDIR)\CVxD.obj" \
	"$(INTDIR)\ParInfo.obj" \
	"$(INTDIR)\parinfo.res"

"$(OUTDIR)\ParInfo.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ParInfo - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\ParInfo.dll"


CLEAN :
	-@erase "$(INTDIR)\CVirPar.obj"
	-@erase "$(INTDIR)\CVxD.obj"
	-@erase "$(INTDIR)\ParInfo.obj"
	-@erase "$(INTDIR)\parinfo.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ParInfo.dll"
	-@erase "$(OUTDIR)\ParInfo.exp"
	-@erase "$(OUTDIR)\ParInfo.ilk"
	-@erase "$(OUTDIR)\ParInfo.lib"
	-@erase "$(OUTDIR)\ParInfo.map"
	-@erase "$(OUTDIR)\ParInfo.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /Zp1 /MTd /W3 /Gm /GX /ZI /Od /D "WIN_9X" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PARINFO_EXPORTS" /Fp"$(INTDIR)\ParInfo.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\parinfo.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ParInfo.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\ParInfo.pdb" /map:"$(INTDIR)\ParInfo.map" /debug /machine:I386 /out:"$(OUTDIR)\ParInfo.dll" /implib:"$(OUTDIR)\ParInfo.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\CVirPar.obj" \
	"$(INTDIR)\CVxD.obj" \
	"$(INTDIR)\ParInfo.obj" \
	"$(INTDIR)\parinfo.res"

"$(OUTDIR)\ParInfo.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("ParInfo.dep")
!INCLUDE "ParInfo.dep"
!ELSE 
!MESSAGE Warning: cannot find "ParInfo.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ParInfo - Win32 Release" || "$(CFG)" == "ParInfo - Win32 Debug"
SOURCE=..\..\..\..\shark\Cur_Ver\dll\9x\CVirPar.cpp

"$(INTDIR)\CVirPar.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\Shark\Cur_Ver\DLL\CVxD.cpp

"$(INTDIR)\CVxD.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\ParInfo.cpp

"$(INTDIR)\ParInfo.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\parinfo.rc

"$(INTDIR)\parinfo.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

