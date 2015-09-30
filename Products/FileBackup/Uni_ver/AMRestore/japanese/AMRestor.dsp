# Microsoft Developer Studio Project File - Name="AMRestor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=AMRestor - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AMRestor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AMRestor.mak" CFG="AMRestor - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AMRestor - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "AMRestor - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AMRestor - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Zp1 /MT /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fr /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ..\..\AM01Dll\japanese\release\AM01DLL.lib /nologo /subsystem:windows /machine:I386 /out:"Release/FileRestore.exe"
# SUBTRACT LINK32 /incremental:yes /debug

!ELSEIF  "$(CFG)" == "AMRestor - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Zp1 /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\..\AM01Dll\japanese\release\AM01DLL.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none /incremental:no

!ENDIF 

# Begin Target

# Name "AMRestor - Win32 Release"
# Name "AMRestor - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp,c"
# Begin Source File

SOURCE=..\AMRestor.cpp
# End Source File
# Begin Source File

SOURCE=..\NewRestoreTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\RestoreState.cpp
# End Source File
# Begin Source File

SOURCE=..\wizsheet.cpp
# End Source File
# Begin Source File

SOURCE=..\WZStep1.cpp
# End Source File
# Begin Source File

SOURCE=..\WZStep2.cpp
# End Source File
# Begin Source File

SOURCE=..\WZStep3.cpp
# End Source File
# Begin Source File

SOURCE=..\WZStep4.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=..\..\Am01struct\AM01Struct.h
# End Source File
# Begin Source File

SOURCE=..\AMRestor.h
# End Source File
# Begin Source File

SOURCE=..\NewRestoreTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=..\resource.h
# End Source File
# Begin Source File

SOURCE=..\RestoreState.h
# End Source File
# Begin Source File

SOURCE=..\Stdafx.h
# End Source File
# Begin Source File

SOURCE=..\wizsheet.h
# End Source File
# Begin Source File

SOURCE=..\WZStep1.h
# End Source File
# Begin Source File

SOURCE=..\WZStep2.h
# End Source File
# Begin Source File

SOURCE=..\WZStep3.h
# End Source File
# Begin Source File

SOURCE=..\WZStep4.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "rc,bmp"
# Begin Source File

SOURCE=.\AMRestor.rc
# End Source File
# Begin Source File

SOURCE=..\Res\baner.bmp
# End Source File
# Begin Source File

SOURCE=..\Res\Drives.bmp
# End Source File
# Begin Source File

SOURCE=..\Res\Eagle.bmp
# End Source File
# Begin Source File

SOURCE=..\Res\File.bmp
# End Source File
# Begin Source File

SOURCE=..\Res\stateico.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=..\Res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=..\Res\filecopy.avi
# End Source File
# Begin Source File

SOURCE=..\Res\LOGO3D.ico
# End Source File
# End Target
# End Project
