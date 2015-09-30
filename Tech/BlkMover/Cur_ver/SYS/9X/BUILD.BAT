@echo off
set MODULE_NAME=BlkMover
set MODULE_EXT=VXD

if (%1)==(debug) goto Debug
if (%1)==(DEBUG) goto Debug

echo ------------ 9X Release Build %MODULE_NAME%.%MODULE_EXT% --------------
call D:\env\98DDKAF.bat
goto Build

:Debug
echo ------------ 9x Debug Build %MODULE_NAME%.%MODULE_EXT% ----------------
call D:\env\98DDKAC.bat

:Build
%MSVC42%\bin\nmake %2 %3 %4 %5 %6 %7 %8 %9 >log
copy /y obj\%MODULE_NAME%.%MODULE_EXT% ..\..\export\9x\
copy /y obj\%MODULE_NAME%.SYM ..\..\export\9x\

:end
set MODULE_NAME=
set MODULE_EXT=
call D:\env\98DDKD.bat
