# Microsoft Developer Studio Project File - Name="AM01" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=AM01 - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AM01.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AM01.mak" CFG="AM01 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AM01 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "AM01 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AM01 - Win32 Release"

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
# ADD CPP /nologo /Zp1 /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_NO_SPLASH" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ..\..\AM01dll\japanese\release\AM01DLL.lib ..\..\..\..\..\Tech\cdrlib\cur_ver\lib\32bit\i386\microsft\FRSMA32.LIB /nologo /subsystem:windows /machine:I386 /out:"Release/FileBackup.exe"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "AM01 - Win32 Debug"

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
# ADD CPP /nologo /Zp1 /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_NO_SPLASH" /Fr /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\..\AM01dll\japanese\release\AM01DLL.lib ..\..\..\..\..\Tech\cdrlib\cur_ver\lib\32bit\i386\microsft\FRSMA32.LIB /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/BackupMaster.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "AM01 - Win32 Release"
# Name "AM01 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp,c"
# Begin Source File

SOURCE=..\AM01.cpp
# End Source File
# Begin Source File

SOURCE=..\AMAbout.cpp
# End Source File
# Begin Source File

SOURCE=..\AMBackup.cpp
# End Source File
# Begin Source File

SOURCE=..\AMMakeCDR.cpp
# End Source File
# Begin Source File

SOURCE=..\AMSetting.cpp
# End Source File
# Begin Source File

SOURCE=..\AMTask.cpp
# End Source File
# Begin Source File

SOURCE=..\BackupState.cpp
# End Source File
# Begin Source File

SOURCE=..\EasyPage1.cpp
# End Source File
# Begin Source File

SOURCE=..\EasyPage2.cpp
# End Source File
# Begin Source File

SOURCE=..\EasyPage3.cpp
# End Source File
# Begin Source File

SOURCE=..\EasyWizard.cpp
# End Source File
# Begin Source File

SOURCE=..\Function.cpp
# End Source File
# Begin Source File

SOURCE=..\MainWnd.cpp
# End Source File
# Begin Source File

SOURCE=..\SaveSetting.cpp
# End Source File
# Begin Source File

SOURCE=..\ScheduleEdit.cpp
# End Source File
# Begin Source File

SOURCE=..\SchedulePage1.cpp
# End Source File
# Begin Source File

SOURCE=..\SchedulePage2.cpp
# End Source File
# Begin Source File

SOURCE=..\SchedulePage3.cpp
# End Source File
# Begin Source File

SOURCE=..\ScheduleResult.cpp
# End Source File
# Begin Source File

SOURCE=..\ScheduleWizard.cpp
# End Source File
# Begin Source File

SOURCE=..\SetPasswordDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Setup.cpp
# End Source File
# Begin Source File

SOURCE=..\ShutdownDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Splash.cpp
# End Source File
# Begin Source File

SOURCE=..\SqueezeImage.cpp
# End Source File
# Begin Source File

SOURCE=..\SqueezeStateDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Traynot.cpp
# End Source File
# Begin Source File

SOURCE=..\ViewSettingDlg.cpp
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

SOURCE=..\AM01.h
# End Source File
# Begin Source File

SOURCE=..\..\AM01Expt\AM01Expt.h
# End Source File
# Begin Source File

SOURCE=..\..\Am01struct\AM01Struct.h
# End Source File
# Begin Source File

SOURCE=..\AMAbout.h
# End Source File
# Begin Source File

SOURCE=..\AMBackup.h
# End Source File
# Begin Source File

SOURCE=..\AMMakeCDR.h
# End Source File
# Begin Source File

SOURCE=..\AMSetting.h
# End Source File
# Begin Source File

SOURCE=..\AMTask.h
# End Source File
# Begin Source File

SOURCE=..\BackupState.h
# End Source File
# Begin Source File

SOURCE=..\EasyPage1.h
# End Source File
# Begin Source File

SOURCE=..\EasyPage2.h
# End Source File
# Begin Source File

SOURCE=..\EasyPage3.h
# End Source File
# Begin Source File

SOURCE=..\EasyWizard.h
# End Source File
# Begin Source File

SOURCE=..\Function.h
# End Source File
# Begin Source File

SOURCE=..\MainWnd.h
# End Source File
# Begin Source File

SOURCE=..\resource.h
# End Source File
# Begin Source File

SOURCE=..\SaveSetting.h
# End Source File
# Begin Source File

SOURCE=..\ScheduleEdit.h
# End Source File
# Begin Source File

SOURCE=..\SchedulePage1.h
# End Source File
# Begin Source File

SOURCE=..\SchedulePage2.h
# End Source File
# Begin Source File

SOURCE=..\SchedulePage3.h
# End Source File
# Begin Source File

SOURCE=..\ScheduleResult.h
# End Source File
# Begin Source File

SOURCE=..\ScheduleWizard.h
# End Source File
# Begin Source File

SOURCE=..\SetPasswordDlg.h
# End Source File
# Begin Source File

SOURCE=..\Setup.h
# End Source File
# Begin Source File

SOURCE=..\ShutdownDlg.h
# End Source File
# Begin Source File

SOURCE=..\Splash.h
# End Source File
# Begin Source File

SOURCE=..\SqueezeImage.h
# End Source File
# Begin Source File

SOURCE=..\SqueezeStateDlg.h
# End Source File
# Begin Source File

SOURCE=..\Stdafx.h
# End Source File
# Begin Source File

SOURCE=..\Traynot.h
# End Source File
# Begin Source File

SOURCE=..\ViewSettingDlg.h
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

SOURCE=.\AM01.rc
# End Source File
# Begin Source File

SOURCE=..\Res\baner.bmp
# End Source File
# Begin Source File

SOURCE=..\Res\Eagle.bmp
# End Source File
# Begin Source File

SOURCE=..\Res\splash.bmp
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
