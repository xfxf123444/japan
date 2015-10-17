!include "LogicLib.nsh"
!include "x64.nsh"
; 安装程序初始定义常量
!define PRODUCT_NAME "暗号化マスター2"
!define PRODUCT_VERSION "2.00"
!define PRODUCT_PUBLISHER "FRONTLINE"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\ShellEncrypt.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

SetCompressor lzma

; ------ MUI 现代界面定义 (1.67 版本以上兼容) ------
!include "MUI.nsh"
!include "UsefulLib.nsh"
${Replace}
;!include "LogicLib.nsh"
!include "..\JP\LANGUAGE.nsh"
!include "..\JP\Check.nsh"
!include "..\JP\finish.nsh"
;!include "Sections.nsh"

; MUI 预定义常量
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; 欢迎页面
!insertmacro MUI_PAGE_WELCOME
; 许可协议页面
!insertmacro MUI_PAGE_LICENSE "..\Rel\Japanese\license.txt"
Page custom showreadme
Page custom ShowRegistration CheckSerial
; 安装目录选择页面
!insertmacro MUI_PAGE_DIRECTORY
; 安装过程页面
!insertmacro MUI_PAGE_INSTFILES
; 安装完成页面
Page custom finish REBOOT
;!define MUI_FINISHPAGE_RUN "$INSTDIR\ShellEncrypt.exe"
;!insertmacro MUI_PAGE_FINISH

; 安装卸载过程页面
!insertmacro MUI_UNPAGE_INSTFILES

; 安装界面包含的语言设置
!insertmacro MUI_LANGUAGE "Japanese"

; 安装预释放文件
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS
; ------ MUI 现代界面定义结束 ------
ReserveFile "registration.ini"
ReserveFile "finisha.ini"
ReserveFile "Special.ini"
ReserveFile "..\Rel\Japanese\SETUP.BMP"

Name "暗号化マスター2"
BGFont "MS UI Gothic"
BGGradient  0F0ACF 0F0ACF FFFFFF ;[(0F0ACF 0F0ACF [FFFFFF|notext])]


InstallColors 00FF00 000000

SetDateSave on
SetDatablockOptimize on
ReserveFile "${NSISDIR}\Plugins\advsplash.dll"
ReserveFile "..\Rel\Japanese\SETUP.BMP"

;Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "Setup.exe"
InstallDir "$PROGRAMFILES\FRONTLINE\暗号化マスター2"
InstallDirRegKey HKLM "${PRODUCT_UNINST_KEY}" "UninstallString"
ShowInstDetails show
ShowUnInstDetails show
BrandingText "FrontLine"

; 激活安装日志记录，该日志文件将会作为卸载文件的依据(注意，本区段必须放置在所有区段之前)
;Section "-LogSetOn"
 ; LogSet on
;SectionEnd
RequestExecutionLevel admin
Section "MainSection" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  File "..\Rel\zlib.dll"
  CreateShortCut "$SMSTARTUP\帺摦埫崋壔僼僅儖僟.lnk" "$INSTDIR\YGFSMon.exe"
  CreateDirectory "$SMPROGRAMS\暗号化マスター2"
  CreateShortCut "$SMPROGRAMS\暗号化マスター2\暗号化マスター2.lnk" "$INSTDIR\FileEncrypt.exe"
  CreateShortCut "$SMPROGRAMS\暗号化マスター2\僨僒僀僼傽乕201.lnk" "$INSTDIR\FileDecrypt.exe"
  CreateShortCut "$SMPROGRAMS\暗号化マスター2\帺摦埫崋壔僼僅儖僟.lnk" "$INSTDIR\YGFSMon.exe"
  CreateShortCut "$SMPROGRAMS\暗号化マスター2\Manual.lnk" "$INSTDIR\Manual.pdf"
  File "..\Rel\Japanese\Manual.pdf"
  File "..\Rel\Japanese\ShellEncrypt.exe"
  File "..\Rel\Japanese\ShellDecrypt.exe"
  File "..\Rel\Japanese\FileEncrypt.exe"
  File "..\Rel\Japanese\FileDecrypt.exe"
  File "..\Rel\Japanese\FED.exe"
  File "..\Rel\Japanese\DelFile.exe"
  File "..\Rel\Japanese\YGFSMon.exe"
  Rename $SYSDIR\DavinciMenu.dll $SYSDIR\DDavinciMenu.dll
  Delete /REBOOTOK "$SYSDIR\DDavinciMenu.dll"
  SetOutPath "$SYSDIR"
  File "..\Rel\Japanese\DavinciMenu.dll"
  File "..\Rel\Japanese\DaExt.dll"
  WriteRegStr  HKLM "Software\Microsoft\Windows\CurrentVersion\Run" "${PRODUCT_NAME}" "$INSTDIR\YGFSMon.exe"
  WriteRegStr  HKCU "Software\Microsoft\Windows\CurrentVersion\Run" "${PRODUCT_NAME}" "$INSTDIR\YGFSMon.exe"
  ;SetOutPath "$SYSDIR\drivers"
  ;File "..\Rel\YGFSMon.sys"
SectionEnd

Section -AdditionalIcons
  SetOutPath $INSTDIR
  ;CreateShortCut "$SMPROGRAMS\暗号化マスター2\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\ShellEncrypt.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\ShellEncrypt.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
  WriteRegStr HKLM "Software\FrontLine\AngoukaMaster\2.00" "UserName" "$2"
  WriteRegStr HKLM "Software\FrontLine\AngoukaMaster\2.00" "Company" "$1"
  WriteRegStr HKLM "Software\FrontLine\AngoukaMaster\2.00" "Password" "$3"
  WriteRegStr HKLM "Software\FrontLine\AngoukaMaster\2.00" "InstallPath" "$INSTDIR"
  ;WriteRegDWORD HKLM "SYSTEM\CurrentControlSet\Services\ygfsmon" "Start" "0"
  ;WriteRegStr HKLM "SYSTEM\CurrentControlSet\Services\ygfsmon" "ImagePath" "system32\DRIVERS\ygfsmon.sys"
  ;WriteRegDWORD HKLM "SYSTEM\CurrentControlSet\Services\ygfsmon" "Type" "1"
  ;WriteRegDWORD HKLM "SYSTEM\CurrentControlSet\Services\ygfsmon" "ErrorControl" "1"
	;WriteRegStr HKCR "*\shellex\ContextMenuHandlers\DeleteMaster" "" "{D1D0C6B8-ABD2-4ec8-9AEF-BA3E0E516126}"
	;WriteRegStr HKCR "*\shellex\ContextMenuHandlers\Davinci" "" "{6EB9CFA7-B6C9-49f5-A3F0-986255E60F27}"
	;WriteRegStr HKCR "CLSID\{6EB9CFA7-B6C9-49f5-A3F0-986255E60F27}" "" "Davinci"
	;WriteRegStr HKCR "CLSID\{6EB9CFA7-B6C9-49f5-A3F0-986255E60F27}\InProcServer32" "" "DavinciMenu.dll"
	;WriteRegStr HKCR "CLSID\{6EB9CFA7-B6C9-49f5-A3F0-986255E60F27}\InProcServer32" "ThreadingModel" "Apartment"
	;WriteRegStr HKCR "CLSID\{D1D0C6B8-ABD2-4ec8-9AEF-BA3E0E516126}" "" "DeleteMaster"
	;WriteRegStr HKCR "CLSID\{D1D0C6B8-ABD2-4ec8-9AEF-BA3E0E516126}\InProcServer32" "" "DaExt.dll"
	;WriteRegStr HKCR "CLSID\{D1D0C6B8-ABD2-4ec8-9AEF-BA3E0E516126}\InProcServer32" "ThreadingModel" "Apartment"
	;WriteRegStr HKCR "Directory\shellex\ContextMenuHandlers\USBBackup" "" "{6EB9CFA7-B6C9-49f5-A3F0-986255E60F27}"
	;WriteRegStr HKCR "Directory\shellex\ContextMenuHandlers\DeleteMaster" "" "{D1D0C6B8-ABD2-4ec8-9AEF-BA3E0E516126}"
	;WriteRegStr HKCR ".chy" "" "chy_auto_file"
	;WriteRegStr HKCR "chy_auto_file" "" "Auto Decrypt"
	;WriteRegStr HKCR "chy_auto_file\shell\open\command" "" '"$INSTDIR\ShellDecrypt.exe" "%1"'

	;begin
	;WriteRegStr HKCR ".chy" "" "chy_auto_file"
	;WriteRegStr HKCR "chy_auto_file" "" "Auto Decrypt"
	;WriteRegStr HKCR "chy_auto_file\shell\open\command" "" "$INSTDIR\ShellDecrypt.exe *%1"
	WriteRegStr HKCR "*\shell" "埫崋壔" "埫崋壔"
  WriteRegStr HKCR "*\shell\埫崋壔\command" "" "$INSTDIR\ShellEncrypt.exe *%1"
  WriteRegStr HKCR "Directory\shell" "埫崋壔" "埫崋壔"
  WriteRegStr HKCR "Directory\shell\埫崋壔\command" "" "$INSTDIR\ShellEncrypt.exe *%1"
  WriteRegStr HKCR ".chy\shell" "暅崋壔" "暅崋壔"
  WriteRegStr HKCR ".chy\shell\暅崋壔\command" "" "$INSTDIR\ShellDecrypt.exe *%1"
  WriteRegStr HKCR ".chy\shell\open\command" "" "$INSTDIR\ShellDecrypt.exe ?%1"
    WriteRegStr HKCR "*\shell" "姰慡徚嫀" "姰慡徚嫀"
  WriteRegStr HKCR "*\shell\姰慡徚嫀\command" "" "$\"$INSTDIR\DelFile.exe$\" $\"%1$\""
  WriteRegStr HKCR "Directory\shell" "姰慡徚嫀" "姰慡徚嫀"
  WriteRegStr HKCR "Directory\shell\姰慡徚嫀\command" "" "$\"$INSTDIR\DelFile.exe$\" $\"%1$\""
	;end
SectionEnd

#-- 根据 NSIS 脚本编辑规则，所有 Function 区段必须放置在 Section 区段之后编写，以避免安装程序出现未可预知的问题。--#
Function REBOOT
;SetOutPath $INSTDIR
Var /GLOBAL INI_VALUE
    !insertmacro MUI_INSTALLOPTIONS_READ $INI_VALUE "finisha.ini" "Field 4" "State"
    ${If} $INI_VALUE = 1
    ;MessageBox MB_YESNO|MB_ICONQUESTION "$(^NameDA) 偺僀儞僗僩乕儖傪姰椆偡傞偵偼丄偙偺僐儞僺儏乕僞傪嵞婲摦偡傞昁梫偑偁傝傑偡丅崱偡偖嵞婲摦偟傑偡偐丠." IDNO +2
      Reboot

    ${EndIf}
    !insertmacro MUI_INSTALLOPTIONS_READ $INI_VALUE "finisha.ini" "Field 5" "State"
    ${If} $INI_VALUE = 1

    ${EndIf}

FunctionEnd
Function .onInit
SetOutPath $TEMP\eInspect
  SetOverwrite ifnewer
  File userInfo.dll
  userInfo::getAccountType
    pop $0
    strCmp $0 "Admin" +3
    !insertmacro admin
    quit

System::Call "Kernel32::GetVersion(v) i .s"
      Pop $0
      IntOp $R6 $0 & 0xFF
	  ;MessageBox MB_OK "$R6"
      IntOp $R7 $0 & 0xFFFF
      IntOp $R7 $R7 >> 8
      ;MessageBox MB_OK "$R7"
      IntCmpU $0 0x80000000 0 nt
      StrCmp $R7 "0" 0 +2
      !insertmacro ossystem
      ;DetailPrint "Windows 95/98/Me"
        StrCpy $R8 "0"


	  Goto me
       nt:
       StrCpy $R8 "1"
       me:

InitPluginsDir
  File /oname=$PLUGINSDIR\finisha.ini "finisha.ini"
  File /oname=$PLUGINSDIR\Special.ini "Special.ini"
  File "/oname=$PLUGINSDIR\win.bmp" "..\Rel\Japanese\win.bmp"
  File "/oname=$PLUGINSDIR\ReadMe.txt" "..\Rel\Japanese\ReadMe.txt"
 !insertmacro MUI_INSTALLOPTIONS_EXTRACT "Special.ini"
 !insertmacro MUI_INSTALLOPTIONS_EXTRACT "finisha.ini"
 ;!insertmacro MUI_INSTALLOPTIONS_EXTRACT "ioSpecial.ini"
!insertmacro MUI_INSTALLOPTIONS_EXTRACT "registration.ini"
 WriteINIStr "$PLUGINSDIR\finisha.ini" "Field 1" "text" "$PLUGINSDIR\win.bmp"
  InitPluginsDir
  File "/oname=$PLUGINSDIR\Splash_SETUP.BMP" "..\Rel\Japanese\SETUP.BMP"
  ; 使用闪屏插件显示闪屏
  ;advsplash::show 1000 600 400 -1 "$PLUGINSDIR\Splash_SETUP"
  Pop $0 ; $0 返回 '1' 表示用户提前关闭闪屏, 返回 '0' 表示闪屏正常结束, 返回 '-1' 表示闪屏显示出错
   StrCmp $R8 "0" new old
       new:
       ReadRegStr $R3 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion" "RegisteredOwner"
        ReadRegStr $R4 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion" "RegisteredOrganization"
        !insertmacro MUI_INSTALLOPTIONS_WRITE "registration.ini" "Field 5" "State" $R4
        !insertmacro MUI_INSTALLOPTIONS_WRITE "registration.ini" "Field 1" "State" $R3
 goto nn
  old:
        ReadRegStr $R3 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "RegisteredOwner"
        ReadRegStr $R4 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "RegisteredOrganization"
        !insertmacro MUI_INSTALLOPTIONS_WRITE "registration.ini" "Field 5" "State" $R4
        !insertmacro MUI_INSTALLOPTIONS_WRITE "registration.ini" "Field 1" "State" $R3
  nn:
  SetShellVarContext All
FunctionEnd

/******************************
 *  以下是安装程序的卸载部分  *
 ******************************/

; 根据安装日志卸载文件的调用宏
!macro DelFileByLog LogFile
  ifFileExists `${LogFile}` 0 +4
    Push `${LogFile}`
    Call un.DelFileByLog
    Delete `${LogFile}`
!macroend

Section Uninstall

  ; 调用宏只根据安装日志卸载安装程序自己安装过的文件
  !insertmacro DelFileByLog "$INSTDIR\install.log"

  ; 清除安装程序创建的且在卸载时可能为空的子目录，对于递归添加的文件目录，请由最内层的子目录开始清除(注意，不要带 /r 参数，否则会失去 DelFileByLog 的意义)
  SetShellVarContext All
  RMDir /r "$SMPROGRAMS\暗号化マスター2"
  Delete "$SMSTARTUP\帺摦埫崋壔僼僅儖僟.lnk"
  Strcpy $R9 $WINDIR 2
  Strcpy $R9 "$R9\ProgramData\Microsoft\Windows\Start Menu\Programs\暗号化マスター2"
  RMDir /r "$R9"
  SetOutPath $TEMP\eInspect
  SetOverwrite ifnewer
  File KillProcDLL.dll
  ;KillProcDLL::KillProc "YGFSMon.exe"
  Delete /REBOOTOK "$SYSDIR\DavinciMenu.dll"
  Delete /REBOOTOK "$SYSDIR\DaExt.dll"
  ;Delete  "$SYSDIR\Drivers\YGFSMon.sys"
  
  RMDir /r "$INSTDIR"
  DeleteRegKey HKCR "*\shellex\ContextMenuHandlers\Davinci" 
  DeleteRegKey HKCR "*\shellex\ContextMenuHandlers\DeleteMaster" 
  DeleteRegKey HKCR "*\shellex\ContextMenuHandlers\Davinci" 
  DeleteRegKey HKCR "CLSID\{6EB9CFA7-B6C9-49f5-A3F0-986255E60F27}" 
  DeleteRegKey HKCR "CLSID\{6EB9CFA7-B6C9-49f5-A3F0-986255E60F27}\InProcServer32" 
  DeleteRegKey HKCR "CLSID\{6EB9CFA7-B6C9-49f5-A3F0-986255E60F27}\InProcServer32" 
  DeleteRegKey HKCR "CLSID\{D1D0C6B8-ABD2-4ec8-9AEF-BA3E0E516126}" 
  DeleteRegKey HKCR "CLSID\{D1D0C6B8-ABD2-4ec8-9AEF-BA3E0E516126}\InProcServer32" 
  DeleteRegKey HKCR "CLSID\{D1D0C6B8-ABD2-4ec8-9AEF-BA3E0E516126}\InProcServer32" 
  DeleteRegKey HKCR "Directory\shellex\ContextMenuHandlers\USBBackup" 
  DeleteRegKey HKCR "Directory\shellex\ContextMenuHandlers\DeleteMaster" 
  ;DeleteRegKey HKCR ".chy"
  ;DeleteRegKey HKCR "chy_auto_file"
  ;DeleteRegKey HKCR "chy_auto_file\shell\open\command"
  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  ;DeleteRegKey HKLM "SYSTEM\CurrentControlSet\Services\ygfsmon"
  DeleteRegKey HKLM "Software\FrontLine\AngoukaMaster\2.00"
  
  ;begin
  ;DeleteRegKey HKCR ".chy"
  ;DeleteRegKey HKCR "chy_auto_file"
  ;DeleteRegKey HKCR "chy_auto_file\shell\open\command"
    DeleteRegValue HKCR "*\shell" "埫崋壔"
  DeleteRegKey HKCR "*\shell\埫崋壔"
    DeleteRegValue HKCR "Directory\shell" "埫崋壔"
  DeleteRegKey HKCR "Directory\shell\埫崋壔"
		DeleteRegKey HKCR ".chy"
		  DeleteRegValue HKCR "*\shell" "姰慡徚嫀"
  DeleteRegKey HKCR "*\shell\姰慡徚嫀"
    DeleteRegValue HKCR "Directory\shell" "姰慡徚嫀"
  DeleteRegKey HKCR "Directory\shell\姰慡徚嫀"
  ;end
  SetAutoClose true
SectionEnd

#-- 根据 NSIS 脚本编辑规则，所有 Function 区段必须放置在 Section 区段之后编写，以避免安装程序出现未可预知的问题。--#

Function un.onInit
  !insertmacro beforun
  Abort
FunctionEnd

Function un.onUninstSuccess
  HideWindow
  !insertmacro afterun
FunctionEnd

; 以下是卸载程序通过安装日志卸载文件的专用函数，请不要随意修改
Function un.DelFileByLog
  Exch $R0
  Push $R1
  Push $R2
  Push $R3
  FileOpen $R0 $R0 r
  ${Do}
    FileRead $R0 $R1
    ${IfThen} $R1 == `` ${|} ${ExitDo} ${|}
    StrCpy $R1 $R1 -2
    StrCpy $R2 $R1 11
    StrCpy $R3 $R1 20
    ${If} $R2 == "File: wrote"
    ${OrIf} $R2 == "File: skipp"
    ${OrIf} $R3 == "CreateShortCut: out:"
    ${OrIf} $R3 == "created uninstaller:"
      Push $R1
      Push `"`
      Call un.DelFileByLog.StrLoc
      Pop $R2
      ${If} $R2 != ""
        IntOp $R2 $R2 + 1
        StrCpy $R3 $R1 "" $R2
        Push $R3
        Push `"`
        Call un.DelFileByLog.StrLoc
        Pop $R2
        ${If} $R2 != ""
          StrCpy $R3 $R3 $R2
          Delete /REBOOTOK $R3
        ${EndIf}
      ${EndIf}
    ${EndIf}
    StrCpy $R2 $R1 7
    ${If} $R2 == "Rename:"
      Push $R1
      Push "->"
      Call un.DelFileByLog.StrLoc
      Pop $R2
      ${If} $R2 != ""
        IntOp $R2 $R2 + 2
        StrCpy $R3 $R1 "" $R2
        Delete /REBOOTOK $R3
      ${EndIf}
    ${EndIf}
  ${Loop}
  FileClose $R0
  Pop $R3
  Pop $R2
  Pop $R1
  Pop $R0
FunctionEnd

Function un.DelFileByLog.StrLoc
  Exch $R0
  Exch
  Exch $R1
  Push $R2
  Push $R3
  Push $R4
  Push $R5
  StrLen $R2 $R0
  StrLen $R3 $R1
  StrCpy $R4 0
  ${Do}
    StrCpy $R5 $R1 $R2 $R4
    ${If} $R5 == $R0
    ${OrIf} $R4 = $R3
      ${ExitDo}
    ${EndIf}
    IntOp $R4 $R4 + 1
  ${Loop}
  ${If} $R4 = $R3
    StrCpy $R0 ""
  ${Else}
    StrCpy $R0 $R4
  ${EndIf}
  Pop $R5
  Pop $R4
  Pop $R3
  Pop $R2
  Pop $R1
  Exch $R0
FunctionEnd
