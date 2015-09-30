# Microsoft Developer Studio Project File - Name="FileDecrypt" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=FileDecrypt - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FileDecrypt.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FileDecrypt.mak" CFG="FileDecrypt - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FileDecrypt - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "FileDecrypt - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FileDecrypt - Win32 Release"

# PROP BASE Use_MFC 5
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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "ZLIB_DLL" /D "SHELL_DECRYPT" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ..\..\..\..\..\Davinci_tech\lib\Crypto\Cur_ver\Export\User\Crypto.lib ..\..\..\..\..\Davinci_tech\ZLib\Cur_ver\Export\Windows\ZLib.lib /nologo /subsystem:windows /machine:I386 /out:"Release/ShellDecrypt.exe"

!ELSEIF  "$(CFG)" == "FileDecrypt - Win32 Debug"

# PROP BASE Use_MFC 5
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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Zp1 /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "ZLIB_DLL" /D "SHELL_DECRYPT" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\..\..\..\..\Davinci_tech\lib\Crypto\Cur_ver\Export\User\Crypto.lib ..\..\..\..\..\Davinci_tech\ZLib\Cur_ver\Export\Windows\ZLib.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/ShellDecrypt.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "FileDecrypt - Win32 Release"
# Name "FileDecrypt - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\DavinciFunction\DavinciFunction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\DecryptFunction\DecryptFunction.cpp
# End Source File
# Begin Source File

SOURCE=..\DecryptStatus.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\DavinciFunction\deletemethoddlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\DavinciFunction\DeleteProgressDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\FileDecrypt.cpp
# End Source File
# Begin Source File

SOURCE=.\FileDecrypt.rc
# End Source File
# Begin Source File

SOURCE=..\..\..\DavinciFunction\SecureDelete.cpp
# End Source File
# Begin Source File

SOURCE=..\ShellDecryptDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\StdAfx.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\Davinci_tech\Lib\Crypto\Cur_ver\Export\Crypto.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DavinciFunction\DavinciFunction.h
# End Source File
# Begin Source File

SOURCE=..\..\DecryptFunction\DecryptFunction.h
# End Source File
# Begin Source File

SOURCE=..\DecryptStatus.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DavinciFunction\deletemethoddlg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DavinciFunction\DeleteProgressDlg.h
# End Source File
# Begin Source File

SOURCE=..\..\EncryptToolStruct\EncryptToolStruct.h
# End Source File
# Begin Source File

SOURCE=..\FileDecrypt.h
# End Source File
# Begin Source File

SOURCE=..\resource.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DavinciFunction\SecureDelete.h
# End Source File
# Begin Source File

SOURCE=..\ShellDecryptDlg.h
# End Source File
# Begin Source File

SOURCE=..\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Drives.bmp
# End Source File
# Begin Source File

SOURCE=.\res\File.bmp
# End Source File
# Begin Source File

SOURCE=.\res\FileDecrypt.ico
# End Source File
# Begin Source File

SOURCE=.\res\FileDecrypt.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\filecopy.avi
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
