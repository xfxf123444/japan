# Microsoft Developer Studio Project File - Name="YGFSMon" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=YGFSMon - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "YGFSMon.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "YGFSMon.mak" CFG="YGFSMon - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "YGFSMon - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "YGFSMon - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "YGFSMon - Win32 Release"

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
# ADD CPP /nologo /Zp1 /MT /W3 /GX /O2 /I "I:\w2kprov2_prog\Microsoft Platform SDK\Include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_YG_FS_MON" /D "ZLIB_DLL" /Fr /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ..\..\..\..\Davinci_tech\Lib\Crypto\Cur_ver\Export\User\Crypto.lib ..\..\..\..\Davinci_tech\ZLib\Cur_ver\Export\Windows\ZLib.lib /nologo /subsystem:windows /machine:I386 /libpath:"l:\w2kprov2_prog\Microsoft Platform SDK\Lib"
# SUBTRACT LINK32 /incremental:yes /debug /nodefaultlib

!ELSEIF  "$(CFG)" == "YGFSMon - Win32 Debug"

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
# ADD CPP /nologo /Zp1 /MTd /W3 /Gm /GX /ZI /Od /I "M:\w2kprov2_prog\Microsoft Platform SDK\Include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_YG_FS_MON" /D "ZLIB_DLL" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\..\..\..\Davinci_tech\Lib\Crypto\Cur_ver\Export\User\Crypto.lib ..\..\..\..\Davinci_tech\ZLib\Cur_ver\Export\Windows\ZLib.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"M:\w2kprov2_prog\Microsoft Platform SDK\Lib"

!ENDIF 

# Begin Target

# Name "YGFSMon - Win32 Release"
# Name "YGFSMon - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp,c"
# Begin Source File

SOURCE=..\AddPath.cpp
# ADD CPP /I "Chinese"
# End Source File
# Begin Source File

SOURCE=..\..\DavinciFunction\DavinciFunction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Decrypt\DecryptFunction\DecryptFunction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\DavinciFunction\DeleteProgressDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Encrypt\EncryptFunction\EncryptFunction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Encrypt\EncryptFunction\EncryptInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Encrypt\EncryptFunction\FileStatisticInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\Function.cpp
# End Source File
# Begin Source File

SOURCE=..\MainWnd.cpp
# End Source File
# Begin Source File

SOURCE=..\Protect.cpp
# End Source File
# Begin Source File

SOURCE=..\RemoveFolder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\DavinciFunction\SecureDelete.cpp
# End Source File
# Begin Source File

SOURCE=..\Setup.cpp
# End Source File
# Begin Source File

SOURCE=..\Traynot.cpp
# End Source File
# Begin Source File

SOURCE=..\YGFSMon.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=..\AddPath.h
# End Source File
# Begin Source File

SOURCE=..\..\DavinciFunction\DavinciFunction.h
# End Source File
# Begin Source File

SOURCE=..\..\Decrypt\DecryptFunction\DecryptFunction.h
# End Source File
# Begin Source File

SOURCE=..\..\DavinciFunction\DeleteProgressDlg.h
# End Source File
# Begin Source File

SOURCE=..\..\Encrypt\EncryptFunction\EncryptFunction.h
# End Source File
# Begin Source File

SOURCE=..\..\Encrypt\EncryptFunction\EncryptInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\EncryptToolStruct\EncryptToolStruct.h
# End Source File
# Begin Source File

SOURCE=..\..\Encrypt\EncryptFunction\FileStatisticInfo.h
# End Source File
# Begin Source File

SOURCE=..\Function.h
# End Source File
# Begin Source File

SOURCE=..\MainWnd.h
# End Source File
# Begin Source File

SOURCE=..\Protect.h
# End Source File
# Begin Source File

SOURCE=..\RemoveFolder.h
# End Source File
# Begin Source File

SOURCE=..\resource.h
# End Source File
# Begin Source File

SOURCE=..\..\DavinciFunction\SecureDelete.h
# End Source File
# Begin Source File

SOURCE=..\Setup.h
# End Source File
# Begin Source File

SOURCE=..\Stdafx.h
# End Source File
# Begin Source File

SOURCE=..\TRACEWIN.H
# End Source File
# Begin Source File

SOURCE=..\Traynot.h
# End Source File
# Begin Source File

SOURCE=..\YGFSMon.h
# End Source File
# Begin Source File

SOURCE=..\YGFSMonConst.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "rc,bmp"
# Begin Source File

SOURCE=..\Res\baner.bmp
# End Source File
# Begin Source File

SOURCE=..\Res\Eagle.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\stateico.bmp
# End Source File
# Begin Source File

SOURCE=..\Res\stateico.bmp
# End Source File
# Begin Source File

SOURCE=.\YGFSMon.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\Res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=..\Res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\Res\LOGO3D.ico
# End Source File
# Begin Source File

SOURCE=..\Res\LOGO3D.ico
# End Source File
# End Target
# End Project
