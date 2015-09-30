# Microsoft Developer Studio Project File - Name="DeleteMaster" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=DeleteMaster - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DeleteMaster.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DeleteMaster.mak" CFG="DeleteMaster - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DeleteMaster - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DeleteMaster - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DeleteMaster - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "WIN_9X" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ..\..\..\..\..\..\Tech\Parinfo\Cur_ver\export\9X\ParInfo.lib ..\..\..\..\..\..\Tech\Parman\Cur_Ver\export\9X\parman.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "DeleteMaster - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "WIN_9X" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\..\..\..\..\..\Tech\Parinfo\Cur_ver\export\9X\ParInfo.lib ..\..\..\..\..\..\Tech\Parman\Cur_Ver\export\9X\parman.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "DeleteMaster - Win32 Release"
# Name "DeleteMaster - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\Confirm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\DeleteMaster.cpp
# End Source File
# Begin Source File

SOURCE=.\DeleteMaster.rc
# End Source File
# Begin Source File

SOURCE=..\..\DelOptions.cpp
# End Source File
# Begin Source File

SOURCE=..\..\DelPar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\DelProc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\DiskInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\DiskView.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Fun.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Main.cpp
# End Source File
# Begin Source File

SOURCE=..\..\PropSht.cpp
# End Source File
# Begin Source File

SOURCE=..\..\SecInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\SectorView.cpp
# End Source File
# Begin Source File

SOURCE=..\..\SetSecNum.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\WipeFree.cpp
# End Source File
# Begin Source File

SOURCE=..\..\WipeProc.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\Confirm.h
# End Source File
# Begin Source File

SOURCE=..\..\DeleteMaster.h
# End Source File
# Begin Source File

SOURCE=..\..\deloptions.h
# End Source File
# Begin Source File

SOURCE=..\..\DelPar.h
# End Source File
# Begin Source File

SOURCE=..\..\DelProc.h
# End Source File
# Begin Source File

SOURCE=..\..\DiskInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\DiskView.h
# End Source File
# Begin Source File

SOURCE=..\..\Fun.h
# End Source File
# Begin Source File

SOURCE=..\..\Main.h
# End Source File
# Begin Source File

SOURCE=..\..\PropSht.h
# End Source File
# Begin Source File

SOURCE=..\..\resource.h
# End Source File
# Begin Source File

SOURCE=..\..\SecInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\SectorView.h
# End Source File
# Begin Source File

SOURCE=..\..\SetSecNum.h
# End Source File
# Begin Source File

SOURCE=..\..\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\..\WipeFree.h
# End Source File
# Begin Source File

SOURCE=..\..\WipeProc.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\DeleteMaster.ico
# End Source File
# Begin Source File

SOURCE=.\res\DeleteMaster.rc2
# End Source File
# Begin Source File

SOURCE=.\res\DeleteMasterDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\list.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Tab.bmp
# End Source File
# Begin Source File

SOURCE=.\res\volum.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
