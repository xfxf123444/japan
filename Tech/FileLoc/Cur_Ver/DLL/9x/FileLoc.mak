# Microsoft Developer Studio Generated NMAKE File, Based on FileLoc.dsp
!IF "$(CFG)" == ""
CFG=FileLoc - Win32 Debug
!MESSAGE No configuration specified. Defaulting to FileLoc - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "FileLoc - Win32 Release" && "$(CFG)" != "FileLoc - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FileLoc.mak" CFG="FileLoc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FileLoc - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "FileLoc - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "FileLoc - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\FileLoc.dll"


CLEAN :
	-@erase "$(INTDIR)\CVdskLdr.obj"
	-@erase "$(INTDIR)\CVirPar.obj"
	-@erase "$(INTDIR)\CVxD.obj"
	-@erase "$(INTDIR)\Fileloc.obj"
	-@erase "$(INTDIR)\Misc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\Version.res"
	-@erase "$(OUTDIR)\FileLoc.dll"
	-@erase "$(OUTDIR)\FileLoc.exp"
	-@erase "$(OUTDIR)\FileLoc.lib"
	-@erase "$(OUTDIR)\FileLoc.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /Zp1 /MT /W3 /GX /Od /D "WIN_9X" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FILELOC_EXPORTS" /Fp"$(INTDIR)\FileLoc.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Version.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\FileLoc.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=..\..\..\..\..\tech\ParInfo\Cur_ver\dll\9x\release\ParInfo.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\FileLoc.pdb" /map:"$(INTDIR)\FileLoc.map" /machine:I386 /out:"$(OUTDIR)\FileLoc.dll" /implib:"$(OUTDIR)\FileLoc.lib" 
LINK32_OBJS= \
	"$(INTDIR)\CVdskLdr.obj" \
	"$(INTDIR)\CVirPar.obj" \
	"$(INTDIR)\Fileloc.obj" \
	"$(INTDIR)\Misc.obj" \
	"$(INTDIR)\Version.res" \
	"$(INTDIR)\CVxD.obj"

"$(OUTDIR)\FileLoc.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "FileLoc - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\FileLoc.dll"


CLEAN :
	-@erase "$(INTDIR)\CVdskLdr.obj"
	-@erase "$(INTDIR)\CVirPar.obj"
	-@erase "$(INTDIR)\CVxD.obj"
	-@erase "$(INTDIR)\Fileloc.obj"
	-@erase "$(INTDIR)\Misc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\Version.res"
	-@erase "$(OUTDIR)\FileLoc.dll"
	-@erase "$(OUTDIR)\FileLoc.exp"
	-@erase "$(OUTDIR)\FileLoc.ilk"
	-@erase "$(OUTDIR)\FileLoc.lib"
	-@erase "$(OUTDIR)\FileLoc.map"
	-@erase "$(OUTDIR)\FileLoc.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /Zp1 /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FILELOC_EXPORTS" /Fp"$(INTDIR)\FileLoc.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Version.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\FileLoc.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=..\..\..\..\tech\ParInfo\Cur_ver\dll\release\ParInfo.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\FileLoc.pdb" /map:"$(INTDIR)\FileLoc.map" /debug /machine:I386 /out:"$(OUTDIR)\FileLoc.dll" /implib:"$(OUTDIR)\FileLoc.lib" /pdbtype:sept /Zp1 
LINK32_OBJS= \
	"$(INTDIR)\CVdskLdr.obj" \
	"$(INTDIR)\CVirPar.obj" \
	"$(INTDIR)\Fileloc.obj" \
	"$(INTDIR)\Misc.obj" \
	"$(INTDIR)\Version.res" \
	"$(INTDIR)\CVxD.obj"

"$(OUTDIR)\FileLoc.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("FileLoc.dep")
!INCLUDE "FileLoc.dep"
!ELSE 
!MESSAGE Warning: cannot find "FileLoc.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "FileLoc - Win32 Release" || "$(CFG)" == "FileLoc - Win32 Debug"
SOURCE=..\..\..\..\Shark\Cur_Ver\Dll\9x\CVdskLdr.cpp

"$(INTDIR)\CVdskLdr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\Shark\Cur_Ver\Dll\9x\CVirPar.cpp

"$(INTDIR)\CVirPar.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\Shark\Cur_Ver\DLL\CVxD.cpp

"$(INTDIR)\CVxD.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Fileloc.cpp

"$(INTDIR)\Fileloc.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\..\..\..\Shark\Cur_Ver\Dll\9x\Misc.cpp

"$(INTDIR)\Misc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Version.rc

"$(INTDIR)\Version.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

