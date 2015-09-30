@REM
@REM Runs the DefaultInstall section of YGFSMon.inf
@REM

@echo off

rundll32.exe setupapi,InstallHinfSection DefaultInstall 132 .\YGFSMon.inf

