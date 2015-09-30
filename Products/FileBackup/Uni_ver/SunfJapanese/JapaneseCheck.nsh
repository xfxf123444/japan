; Registration Custom Page functions
; Include this after you have set up the installer options
;
; If you are using the unregistered version of registration
; you should define REG_CODE before including this file.
;
; e.g. !define REG_CODE "My Secret Code"
;      !include "registration.nsh"
 ;ReserveFile "${NSISDIR}\Plugins\System.dll"
Function ShowRegistration
  !insertmacro MUI_ShowRegistration_SUBTITLE 
  !insertmacro MUI_INSTALLOPTIONS_DISPLAY "Japaneseregistration.ini"
FunctionEnd
Function showreadme
!insertmacro MUI_showreadme_SUBTITLE
!insertmacro MUI_INSTALLOPTIONS_DISPLAY "JapaneseSpecial.ini"
FunctionEnd
Function CheckSerial

  System::Call "Kernel32::GetVersion(v) i .s"
      Pop $0
      IntOp $R6 $0 & 0xFF
      IntOp $R7 $0 & 0xFFFF
      IntOp $R7 $R7 >> 8
      IntCmpU $0 0x80000000 0 nt
      ;DetailPrint "Windows 95/98/Me"
      
       ;wend:
       !insertmacro ossystem          
        quit

      ;IntCmpU $0 0x80000000 0 nt
      ;DetailPrint "Windows 95/98/Me"
       nt:
        ;DetailPrint "Windows NT/2000/XP"
  SetOutPath $TEMP\eInspect
  SetOverwrite ifnewer
  File userInfo.dll
  userInfo::getAccountType
    pop $0
    strCmp $0 "Admin" +3
    !insertmacro admin    
    return
    ;messageBox MB_OK "is admin"

  Strcpy $R9 $WINDIR 2

  !insertmacro MUI_INSTALLOPTIONS_READ $1 "Japaneseregistration.ini" "Field 5" "State"
  !insertmacro MUI_INSTALLOPTIONS_READ $2 "Japaneseregistration.ini" "Field 1" "State"
  !insertmacro MUI_INSTALLOPTIONS_READ $3 "Japaneseregistration.ini" "Field 2" "State"
  StrCpy $4 $3 4
  StrCpy $5 $3 3 5
  StrCpy $6 $3 6 9
  StrCpy $7 $3 6 16
  StrCmp $4 "flam" 0 n
  StrCmp $5 "006" +5 
  StrCmp $5 "009" +4
  StrCmp $5 "011" +3
  StrCmp $5 "013" +2
  StrCmp $5 "025" +1 n
  StrCpy $9 "$6" 1
  StrCmp $9 "0" y 
  StrCmp $9 "1" y
  StrCmp $9 "2" y
  StrCmp $9 "3" y
  StrCmp $9 "4" y
  StrCmp $9 "5" y
  StrCmp $9 "6" y
  StrCmp $9 "7" y
  StrCmp $9 "8" y
  StrCmp $9 "9" y n
    n:
     !insertmacro serial
     
     Abort
     quit
    y:
  SetOutPath $TEMP\eInspect
  SetOverwrite try
  File SerCheck.dll
  System::Call 'SerCheck::YGSCCheckSerial(t,t,t,t) i(r4,r5,r6,r7).r8'
  IntCmpU $8 1 goodreg
   !insertmacro serial
    Abort
    quit
goodreg:
  ReadRegStr $R0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString"
  StrCpy $R1 $R0
  StrCmp $R0 "" YES NO
  NO:
    !insertmacro shd

    tr:
    SetOutPath $TEMP\eInspect
    SetOverwrite try
;    File KillProcDLL.dll

;     KillProcDLL::KillProc "FileBackup.exe"
     ExecWait "$R1"

    fa:
    SetOutPath $TEMP\eInspect
    SetOverwrite ifnewer
;    File KillProcDLL.dll
;     KillProcDLL::KillProc "FileBackup.exe"
  YES:
FunctionEnd
