@echo off
set>save.txt
X:\ENV\LINEADD "@SET " save.txt save.bat
del/f save.txt

set MODULE_NAME=Crypto
set MODULE_EXT=lib

if (%1)==(debug) goto Debug
if (%1)==(DEBUG) goto Debug

echo ------------ 9X Release Build %MODULE_NAME%.%MODULE_EXT% --------------
call x:\env\98DDKAF.bat
goto Build

:Debug
echo ------------ 9x Debug Build %MODULE_NAME%.%MODULE_EXT% ----------------
call x:\env\98DDKAC.bat

:Build
%MSVC42%\bin\nmake %2 %3 %4 %5 %6 %7 %8 %9 >log
copy /y obj\%MODULE_NAME%.lib ..\..\export\9x\

:end
set MODULE_NAME=
set MODULE_EXT=
call x:\env\98DDKD.bat
call save.bat
del/f save.bat