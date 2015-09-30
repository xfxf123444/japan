; Registration Custom Page functions
; Include this after you have set up the installer options
;
; If you are using the unregistered version of registration
; you should define REG_CODE before including this file.
;
; e.g. !define REG_CODE "My Secret Code"
;      !include "registration.nsh"
 ;ReserveFile "${NSISDIR}\Plugins\System.dll"
     ;DetailPrint "Windows NT/2000/XP"
     

Function ShowRegistration
  !insertmacro MUI_ShowRegistration_SUBTITLE
  !insertmacro MUI_INSTALLOPTIONS_DISPLAY "registration.ini"
FunctionEnd

Function showreadme
;SetOutPath $TEMP\eInspect
  ;SetOverwrite ifnewer
  ;File userInfo.dll
;var /GLOBAL READ
;var /GLOBAL READ1
var /GLOBAL INIR
!insertmacro MUI_showreadme_SUBTITLE

InstallOptions::initDialog /NOUNLOAD "$PLUGINSDIR\Special.ini"
  Pop $0
  /*SetOutPath $TEMP\eInspect
  SetOverwrite ifnewer
  File bmptxt\ReadMe.txt
FileOpen $READ $TEMP\eInspect\ReadMe.txt r
FileRead $READ $READ1
FileClose "$READ"*/
      ReadINIStr $INIR "$PLUGINSDIR\Special.ini" "Field 1" "HWND"
      SetCtlColors $INIR 0x000000 0xFFFFFF
      Push "$PLUGINSDIR\ReadMe.txt" # 在堆栈中压入 大文本文件
      Push $INIR       # 在堆栈中压入 控件句柄
      Call ShowText   # 调用过程，在控件中加入大文本

      ;nsExStr::SetWindowText $$INIR "$PLUGINSDIR\ReadMe.txt"
      ;SendMessage $INIR ${WM_SETTEXT} 0 "STR:$READ1"

InstallOptions::show
  ; Finally fetch the InstallOptions status value (we don't care what it is though)
  Pop $0
!insertmacro MUI_showreadme_SUBTITLE
;!insertmacro MUI_INSTALLOPTIONS_DISPLAY "$PLUGINSDIR\Special.ini"
FunctionEnd

Function ShowText
  Exch $R0 ;控件句柄
  Exch
  Exch $R1 ;文件
  Push $R2
  Push $R3
  Push $R4
  Push $R5

  ClearErrors
  FileOpen $R2 $R1 r ;$R2 = 文件句柄
  ${Unless} ${Errors} ;确保打开文件没有发生错误
    System::Call /NOUNLOAD "Kernel32::GetFileSize(i, i) i (R2, 0) .R3" ;$R3 = 文件大小
    IntOp $R3 $R3 + 1
    System::Alloc /NOUNLOAD $R3 ;分配内存
    Pop $R4 ;内存地址
      ${If} $R4 U> 0 ;确保分配了内存
          System::Call /NOUNLOAD "*(i 0) i .R5"
          System::Call /NOUNLOAD `Kernel32::ReadFile(i, i, i, i, i) i (R2, R4R4, R3, R5R5, 0)`
          System::Call /NOUNLOAD "*$R5(i .R1)"
          ${If} $R1 > 0
            System::Call /NOUNLOAD "User32::SendMessage(i, i, i, i) i (R0, ${WM_SETTEXT}, 0, R4)"
          ${EndIf}
          System::Free /NOUNLOAD $R5
        System::Free $R4 ;释放内存
    ${EndIf}
    FileClose $R2
  ${EndUnless}

    Pop $R5
    Pop $R4
    Pop $R3
    Pop $R2
    Pop $R1
  Pop $R0
FunctionEnd



Function CheckSerial

/*
  System::Call "Kernel32::GetVersion(v) i .s"
      Pop $0
      IntOp $R6 $0 & 0xFF
      IntOp $R7 $0 & 0xFFFF
      IntOp $R7 $R7 >> 8
      IntCmpU $0 0x80000000 0 nt
      ;DetailPrint "Windows 95/98/Me"
        StrCpy $R8 "0"

       !insertmacro ossystem          
        
       nt:
       StrCpy $R8 "1"
*/

        
  /*SetOutPath $TEMP\eInspect
  SetOverwrite ifnewer
  File userInfo.dll
  userInfo::getAccountType
    pop $0
    strCmp $0 "Admin" +3
    !insertmacro admin    
    return
    ;messageBox MB_OK "is admin"*/


;!macro ini

;ReadRegStr $R3 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "RegisteredOwner"
;ReadRegStr $R4 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "RegisteredOrganization"
;WriteINIStr "registration.ini" "Field 5" "State" $R3
;messageBox MB_OK "$R3"
;!macroend
  !insertmacro MUI_INSTALLOPTIONS_READ $1 "registration.ini" "Field 5" "State"
  !insertmacro MUI_INSTALLOPTIONS_READ $2 "registration.ini" "Field 1" "State"
  !insertmacro MUI_INSTALLOPTIONS_READ $3 "registration.ini" "Field 2" "State"
  StrCpy $4 $3 4
  StrCpy $5 $3 3 5
  StrCpy $6 $3 6 9
  StrCpy $7 $3 6 16
  
  StrCmp $4 "flam" 0 oneno
  StrCmp $5 "021" oneyes
  StrCmp $5 "022" oneyes
  StrCmp $5 "023" oneyes
  StrCmp $5 "013" oneyes 
  oneno:
  StrCmp $4 "FLMS" 0 n
  StrCmp $5 "005" oneyes 
  StrCmp $5 "009" oneyes
  StrCmp $5 "010" oneyes n
  oneyes:
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
  SetOverwrite ifnewer
  File SerCheck.dll
  System::Call 'SerCheck::YGSCCheckSerial(t,t,t,t) i(r4,r5,r6,r7).r8'
  IntCmpU $8 1 goodreg
   !insertmacro serial
    Abort
    quit
goodreg:
  SetOutPath $TEMP\eInspect
  SetOverwrite ifnewer
  File KillProcDLL.dll
  ReadRegStr $R2 ${PRODUCT_UNINST_ROOT_KEY} "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\MovingMaster" "UninstallString"
  ReadRegStr $R1 ${PRODUCT_UNINST_ROOT_KEY} "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\SimpleVault" "UninstallString"
  ReadRegStr $R0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString"
  ;StrCpy $R1 $R0
  StrCmp $R2 "" YES NO
  NO:
    !insertmacro shd

    tr:
    

    ; KillProcDLL::KillProc "MainCarrier.exe"
     ExecWait "$R2"

    fa:
    quit
    
  YES:
  StrCmp $R1 "" YES1 NO1
  NO1:
    !insertmacro shd1

    tr1:
     ;KillProcDLL::KillProc "SimpleVault.exe"
     ExecWait "$R1"

    fa1:
    quit
  YES1:
  StrCmp $R0 "" YES2 NO2
  NO2:
    !insertmacro shd2

    tr2:
     ExecWait "$R0"

    fa2:
  YES2:
FunctionEnd











