# Microsoft Developer Studio Project File - Name="AntiType" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=AntiType - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AntiType.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AntiType.mak" CFG="AntiType - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AntiType - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "AntiType - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AntiType - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /fo"Release/USBMaster.res" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 shlwapi.lib ..\..\..\..\..\Tech\ParInfo\CUR_VER\EXPORT\2000\parInfo.lib ..\..\..\..\..\Tech\FileLoc\CUR_VER\EXPORT\2000\FileLoc.lib ..\..\..\..\..\Tech\Lib\Crypto\CUR_VER\EXPORT\User\Crypto.lib /nologo /subsystem:windows /machine:I386 /out:"Release/USBMaster.exe"

!ELSEIF  "$(CFG)" == "AntiType - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /fo"Debug/USBMaster.res" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 shlwapi.lib ..\..\..\..\..\Tech\ParInfo\CUR_VER\EXPORT\2000\parInfo.lib ..\..\..\..\..\Tech\FileLoc\Cur_Ver\EXPORT\2000\FileLoc.lib ..\..\..\..\..\Tech\Lib\Crypto\CUR_VER\EXPORT\User\Crypto.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/USBMaster.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "AntiType - Win32 Release"
# Name "AntiType - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\AntiType.cpp
# End Source File
# Begin Source File

SOURCE=.\AntiType.rc
# End Source File
# Begin Source File

SOURCE=..\AntiTypeDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\AvailableDriverDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\ChangePasswordDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\DecryptDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\EncryptDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=..\SystemTray.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\AntiType.h
# End Source File
# Begin Source File

SOURCE=..\AntiTypeDlg.h
# End Source File
# Begin Source File

SOURCE=..\AvailableDriverDialog.h
# End Source File
# Begin Source File

SOURCE=..\ChangePasswordDialog.h
# End Source File
# Begin Source File

SOURCE=..\DecryptDialog.h
# End Source File
# Begin Source File

SOURCE=..\EncryptDialog.h
# End Source File
# Begin Source File

SOURCE=..\Resource.h
# End Source File
# Begin Source File

SOURCE=..\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\SystemTray.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\AntiType.ico
# End Source File
# Begin Source File

SOURCE=.\res\AntiType.rc2
# End Source File
# Begin Source File

SOURCE=.\res\encrypted.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
