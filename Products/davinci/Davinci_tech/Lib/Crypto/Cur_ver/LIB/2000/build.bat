echo off
set MODULE_NAME=Crypto
set MODULE_EXT=lib
if (%1)==(debug) goto DEBUG
if (%1)==(DEBUG) goto DEBUG
echo --------------Win2000 Release Build '%MODULE_NAME%.%MODULE_EXT%'---------------
call x:\buildenv\2000\setenv x:\2000ddk free
goto Build

:DEBUG
echo --------------Win2000 Debug Build '%MODULE_NAME%.%MODULE_EXT%'-----------------
call x:\buildenv\2000\setenv x:\2000ddk checked
:Build
call x:\2000ddk\bin\build
call x:\buildenv\2000\cleanupenv
if (%1)==(debug) goto CopyModuleDebug
if (%1)==(DEBUG) goto CopyModuleDebug
:CopyModuleRelease
copy objfre\i386\%MODULE_NAME%.%MODULE_EXT% ..\..\export\2000\*.*/y
goto end
:CopyModuleDebug
copy objchk\i386\%MODULE_NAME%.%MODULE_EXT% ..\..\export\2000\*.*/y
:end