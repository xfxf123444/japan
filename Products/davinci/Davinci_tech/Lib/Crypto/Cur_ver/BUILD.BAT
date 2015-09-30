set DIR_NAME=lib\
echo off
if exist %DIR_NAME%9x goto Build9x
goto Skip9x
:Build9x
cd %DIR_NAME%9x
call Build %1
cd..\..
:Skip9x
if exist %DIR_NAME%2000 goto Build2000
goto Skip2000
:Build2000
cd %DIR_NAME%2000
call Build %1
cd..\..
:Skip2000

if exist %DIR_NAME%bios goto BuildBios
goto SkipBios
:BuildBios
cd %DIR_NAME%bios
call Build %1
cd..\..
:SkipBios

if exist %DIR_NAME%dos goto BuildDOS
goto SkipDOS
:BuildDOS
cd %DIR_NAME%dos
call Build %1
cd..\..
:SkipDOS
