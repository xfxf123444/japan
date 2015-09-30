@SET OLDTOOLS=%TOOLS%
@SET OLDTOOLKIT=%TOOLKIT%
@SET OLDPATH=%PATH%
@SET OLDTMP=%TMP%
@SET OLDINCLUDE=%INCLUDE%
@SET OLDLIB=%LIB%

@call env32.bat
@SET CFG=zlibvc - Win32 Release
@nmake ZLibVc.mak
@copy .\Release\*.lib ..\..\Export\windows
@copy .\Release\*.DLL ..\..\Export\windows

@set TOOLS=%OLDTOOLS%
@set TOOLKIT=%OLDTOOLKIT%
@set PATH=%OLDPATH%
@set TMP=%OLDTMP%
@set INCLUDE=%OLDINCLUDE%
@set LIB=%OLDLIB%
