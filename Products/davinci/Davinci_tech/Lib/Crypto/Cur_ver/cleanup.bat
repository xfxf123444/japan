set DIR_NAME=lib\
echo off
if exist %DIR_NAME%9x goto Cleanup9x
goto Skip9x
:Cleanup9x
cd %DIR_NAME%9x
call cleanup
cd..\..
:Skip9x
if exist %DIR_NAME%2000 goto Cleanup2000
goto Skip2000
:Cleanup2000
cd %DIR_NAME%2000
call cleanup
cd..\..
:Skip2000

if exist %DIR_NAME%bios goto CleanupBios
goto SkipBios
:CleanupBios
cd %DIR_NAME%bios
call cleanup
cd..\..
:SkipBios

if exist %DIR_NAME%dos goto CleanupDOS
goto SkipDOS
:CleanupDOS
cd %DIR_NAME%dos
call cleanup
cd..\..
:SkipDOS
