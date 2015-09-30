# Microsoft Developer Studio Generated NMAKE File, Based on ParMan.dsp
!IF "$(CFG)" == ""
CFG=ParMan - Win32 Debug
!MESSAGE No configuration specified. Defaulting to ParMan - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ParMan - Win32 Release" && "$(CFG)" != "ParMan - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ParMan.mak" CFG="ParMan - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ParMan - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ParMan - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "ParMan - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\ParMan.dll"


CLEAN :
	-@erase "$(INTDIR)\DiskMan.obj"
	-@erase "$(INTDIR)\ParMan.obj"
	-@erase "$(INTDIR)\ParManPrivate.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\ParMan.dll"
	-@erase "$(OUTDIR)\ParMan.exp"
	-@erase "$(OUTDIR)\ParMan.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PARMAN_EXPORTS" /Fp"$(INTDIR)\ParMan.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ParMan.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=..\..\..\..\ParInfo\Cur_ver\export\2000\ParInfo.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ..\..\..\..\ParInfo\cur_ver\export\2000\ParInfo.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\ParMan.pdb" /machine:I386 /out:"$(OUTDIR)\ParMan.dll" /implib:"$(OUTDIR)\ParMan.lib" 
LINK32_OBJS= \
	"$(INTDIR)\DiskMan.obj" \
	"$(INTDIR)\ParMan.obj" \
	"$(INTDIR)\ParManPrivate.obj"

"$(OUTDIR)\ParMan.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ParMan - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\ParMan.dll" "$(OUTDIR)\ParMan.bsc"


CLEAN :
	-@erase "$(INTDIR)\DiskMan.obj"
	-@erase "$(INTDIR)\DiskMan.sbr"
	-@erase "$(INTDIR)\ParMan.obj"
	-@erase "$(INTDIR)\ParMan.sbr"
	-@erase "$(INTDIR)\ParManPrivate.obj"
	-@erase "$(INTDIR)\ParManPrivate.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ParMan.bsc"
	-@erase "$(OUTDIR)\ParMan.dll"
	-@erase "$(OUTDIR)\ParMan.exp"
	-@erase "$(OUTDIR)\ParMan.ilk"
	-@erase "$(OUTDIR)\ParMan.lib"
	-@erase "$(OUTDIR)\ParMan.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PARMAN_EXPORTS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ParMan.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\DiskMan.sbr" \
	"$(INTDIR)\ParMan.sbr" \
	"$(INTDIR)\ParManPrivate.sbr"

"$(OUTDIR)\ParMan.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ..\..\..\..\ParInfo\cur_ver\export\2000\ParInfo.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\ParMan.pdb" /debug /machine:I386 /out:"$(OUTDIR)\ParMan.dll" /implib:"$(OUTDIR)\ParMan.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\DiskMan.obj" \
	"$(INTDIR)\ParMan.obj" \
	"$(INTDIR)\ParManPrivate.obj"

"$(OUTDIR)\ParMan.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("ParMan.dep")
!INCLUDE "ParMan.dep"
!ELSE 
!MESSAGE Warning: cannot find "ParMan.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ParMan - Win32 Release" || "$(CFG)" == "ParMan - Win32 Debug"
SOURCE=.\DiskMan.cpp

!IF  "$(CFG)" == "ParMan - Win32 Release"


"$(INTDIR)\DiskMan.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ParMan - Win32 Debug"


"$(INTDIR)\DiskMan.obj"	"$(INTDIR)\DiskMan.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ParMan.cpp

!IF  "$(CFG)" == "ParMan - Win32 Release"


"$(INTDIR)\ParMan.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ParMan - Win32 Debug"


"$(INTDIR)\ParMan.obj"	"$(INTDIR)\ParMan.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ParManPrivate.cpp

!IF  "$(CFG)" == "ParMan - Win32 Release"


"$(INTDIR)\ParManPrivate.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ParMan - Win32 Debug"


"$(INTDIR)\ParManPrivate.obj"	"$(INTDIR)\ParManPrivate.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 


!ENDIF 

