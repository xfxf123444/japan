# Microsoft Developer Studio Generated NMAKE File, Based on YGDesc.dsp
!IF "$(CFG)" == ""
CFG=YGDesc - Win32 Release
!MESSAGE No configuration specified. Defaulting to YGDesc - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "YGDesc - Win32 Release" && "$(CFG)" != "YGDesc - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f"YGDesc.mak" CFG="YGDesc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "YGDesc - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "YGDesc - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "YGDesc - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\YGDesc.lib"


CLEAN :
	-@erase "$(INTDIR)\*.obj"
	-@erase "$(OUTDIR)\YGDesc.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D OS_PLATFORM=WIN2000 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\YGDesc.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\YGDesc.bsc" 
BSC32_SBRS= \
	
LIB32=$(MSVC42)\bin\lib.exe
LIB32_FLAGS=/nologo /machine:IX86 /out:"$(OUTDIR)\YGDesc.lib" 
LIB32_OBJS= \
	"$(INTDIR)\d3des.obj" \
	"$(INTDIR)\ygdesc.obj"

"$(OUTDIR)\YGDesc.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
	$(LIB32_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "YGDesc - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\YGDesc.lib"


CLEAN :
	-@erase "$(INTDIR)\*.obj"
	-@erase "$(OUTDIR)\YGDesc.lib"
	-@erase "$(OUTDIR)\YGDesc.ilk"
	-@erase "$(OUTDIR)\YGDesc.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D OS_PLATFORM=WIN2000 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\YGDesc.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\YGDesc.bsc" 
BSC32_SBRS= \
	
LIB32=$(MSVC42)\bin\lib.exe
LIB32_FLAGS=/nologo /machine:IX86 /out:"$(OUTDIR)\YGDesc.lib"
LIB32_OBJS= \
	"$(INTDIR)\d3des.obj" \
	"$(INTDIR)\YGDesc.obj"

"$(OUTDIR)\YGDesc.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
	$(LIB32_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("YGDesc.dep")
!INCLUDE "YGDesc.dep"
!ELSE 
!MESSAGE Warning: cannot find "YGDesc.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "YGDesc - Win32 Release" || "$(CFG)" == "YGDesc - Win32 Debug"
SOURCE=..\d3des.c

"$(INTDIR)\d3des.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\YGDesc.c

"$(INTDIR)\YGDesc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

