!include "LogicLib.nsh"
!include "x64.nsh"
; 安装程序初始定义常量
!define PRODUCT_NAME "デサイファー201"
!define PRODUCT_VERSION "2.00"
!define PRODUCT_PUBLISHER "FrontLine"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\FileDecrypt.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

SetCompressor lzma

; ------ MUI 现代界面定义 (1.67 版本以上兼容) ------
!include "MUI.nsh"
!include "UsefulLib.nsh"
${Replace}
!include "LogicLib.nsh"
!include ".\LANGUAGE.nsh"
!include ".\Check.nsh"

; MUI 预定义常量
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; 欢迎页面
!insertmacro MUI_PAGE_WELCOME
; 许可协议页面
!insertmacro MUI_PAGE_LICENSE "..\Rel\Japanese\license.txt"
Page custom showreadme
Page custom CheckSerial
; 安装目录选择页面
!insertmacro MUI_PAGE_DIRECTORY
; 安装过程页面
!insertmacro MUI_PAGE_INSTFILES
; 安装完成页面
;!define MUI_FINISHPAGE_RUN "$INSTDIR\FileDecrypt.exe"
!insertmacro MUI_PAGE_FINISH

; 安装卸载过程页面
!insertmacro MUI_UNPAGE_INSTFILES

; 安装界面包含的语言设置
!insertmacro MUI_LANGUAGE "Japanese"

; 安装预释放文件
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS
; ------ MUI 现代界面定义结束 ------

ReserveFile "${NSISDIR}\Plugins\advsplash.dll"
ReserveFile ".\Special.ini"
ReserveFile "..\Rel\Japanese\SETUP.BMP"

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
BGFont "MS UI Gothic"
BGGradient  0F0ACF 0F0ACF FFFFFF ;[(0F0ACF 0F0ACF [FFFFFF|notext])]


InstallColors 00FF00 000000
OutFile "Setup.exe"
InstallDir "$PROGRAMFILES\FRONTLINE\デサイファー201"
InstallDirRegKey HKLM "${PRODUCT_UNINST_KEY}" "UninstallString"
ShowInstDetails show
ShowUnInstDetails show
BrandingText "FrontLine"

; 激活安装日志记录，该日志文件将会作为卸载文件的依据(注意，本区段必须放置在所有区段之前)
;Section "-LogSetOn"
;  LogSet on
;SectionEnd
RequestExecutionLevel admin
Section "MainSection" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  File "..\Rel\zlib.dll"
  CreateDirectory "$SMPROGRAMS\デサイファー201"
  CreateShortCut "$SMPROGRAMS\デサイファー201\デサイファー201.lnk" "$INSTDIR\FileDecrypt.exe"
  File "..\Rel\Japanese\FileDecrypt.exe"
SectionEnd

Section -Post
  WriteRegStr HKLM "Software\FrontLine\Decipher201\2.00" "UserName" ""
  WriteRegStr HKLM "Software\FrontLine\Decipher201\2.00" "Company" ""
  WriteRegStr HKLM "Software\FrontLine\Decipher201\2.00" "Password" ""
  WriteRegStr HKLM "Software\FrontLine\Decipher201\2.00" "InstallPath" "$INSTDIR"
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\FileDecrypt.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\FileDecrypt.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

#-- 根据 NSIS 脚本编辑规则，所有 Function 区段必须放置在 Section 区段之后编写，以避免安装程序出现未可预知的问题。--#

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
 
  File /oname=$PLUGINSDIR\Special.ini "Special.ini"
  File "/oname=$PLUGINSDIR\win.bmp" "..\Rel\Japanese\win.bmp"
  File "/oname=$PLUGINSDIR\ReadMe.txt" "..\Rel\Japanese\ReadMe.txt"
 !insertmacro MUI_INSTALLOPTIONS_EXTRACT "Special.ini"



  InitPluginsDir
  File "/oname=$PLUGINSDIR\Splash_SETUP.BMP" "..\Rel\Japanese\SETUP.BMP"
  ; ﾊｹﾓﾃﾉﾁﾆﾁｲ蠑ﾏﾔﾊｾﾉﾁﾆﾁ
  advsplash::show 1000 600 400 -1 "$PLUGINSDIR\Splash_SETUP"
  Pop $0 ; $0 返回 '1' 表示用户提前关闭闪屏, 返回 '0' 表示闪屏正常结束, 返回 '-1' 表示闪屏显示出错
   
       
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
    SetOutPath $TEMP\eInspect
     SetOverwrite ifnewer
;     File "KillProcDLL.dll"
;     KillProcDLL::KillProc "FileDecrypt.exe"
  RMDir "$SMPROGRAMS\デサイファー201"

  RMDir "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  DeleteRegKey HKLM "Software\FrontLine\Decipher201\2.00"
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
