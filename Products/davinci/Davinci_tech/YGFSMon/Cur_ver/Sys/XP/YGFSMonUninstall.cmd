@REM
@REM Runs the DefaultUninstall section of YGFSMon.inf
@REM

@echo off

rundll32.exe setupapi,InstallHinfSection DefaultUninstall 132 .\YGFSMon.inf

