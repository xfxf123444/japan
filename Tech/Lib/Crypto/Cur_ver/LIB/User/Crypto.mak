# Microsoft Developer Studio Generated NMAKE File, Based on Crypto.dsp
!IF "$(CFG)" == ""
CFG=Crypto - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Crypto - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Crypto - Win32 Release" && "$(CFG)" != "Crypto - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Crypto.mak" CFG="Crypto - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Crypto - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Crypto - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "Crypto - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\Crypto.lib"


CLEAN :
	-@erase "$(INTDIR)\Crypto.obj"
	-@erase "$(INTDIR)\des.obj"
	-@erase "$(INTDIR)\md5c.obj"
	-@erase "$(INTDIR)\misc.obj"
	-@erase "$(INTDIR)\nn.obj"
	-@erase "$(INTDIR)\rsa.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\Crypto.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "USER_LIB" /Fp"$(INTDIR)\Crypto.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Crypto.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\Crypto.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Crypto.obj" \
	"$(INTDIR)\des.obj" \
	"$(INTDIR)\md5c.obj" \
	"$(INTDIR)\misc.obj" \
	"$(INTDIR)\nn.obj" \
	"$(INTDIR)\rsa.obj"

"$(OUTDIR)\Crypto.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Crypto - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\Crypto.lib"


CLEAN :
	-@erase "$(INTDIR)\Crypto.obj"
	-@erase "$(INTDIR)\des.obj"
	-@erase "$(INTDIR)\md5c.obj"
	-@erase "$(INTDIR)\misc.obj"
	-@erase "$(INTDIR)\nn.obj"
	-@erase "$(INTDIR)\rsa.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Crypto.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "USER_LIB" /Fp"$(INTDIR)\Crypto.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Crypto.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\Crypto.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Crypto.obj" \
	"$(INTDIR)\des.obj" \
	"$(INTDIR)\md5c.obj" \
	"$(INTDIR)\misc.obj" \
	"$(INTDIR)\nn.obj" \
	"$(INTDIR)\rsa.obj"

"$(OUTDIR)\Crypto.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Crypto.dep")
!INCLUDE "Crypto.dep"
!ELSE 
!MESSAGE Warning: cannot find "Crypto.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Crypto - Win32 Release" || "$(CFG)" == "Crypto - Win32 Debug"
SOURCE=..\Crypto.cpp

"$(INTDIR)\Crypto.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\des.cpp

"$(INTDIR)\des.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\md5c.cpp

"$(INTDIR)\md5c.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\misc.cpp

"$(INTDIR)\misc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\nn.cpp

"$(INTDIR)\nn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\rsa.cpp

"$(INTDIR)\rsa.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

