; 该脚本使用 HM VNISEdit 脚本编辑器向导产生

; 安装程序初始定义常量
!define PRODUCT_NAME "FileBackup"
!define PRODUCT_VERSION "6.00"
!define PRODUCT_PUBLISHER "FrontLine"
!define PRODUCT_WEB_SITE "http://www.mycompany.com"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\AppMainExe.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

SetCompressor lzma

; ------ MUI 现代界面定义 (1.67 版本以上兼容) ------

!include "MUI.nsh"
!define REG_CODE "sample"

!include "LogicLib.nsh"
!include "..\SunfJapanese\sunfJapanese.nsh"
!include "..\SunfJapanese\JapaneseCheck.nsh"

; MUI 预定义常量
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; 欢迎页面
!insertmacro MUI_PAGE_WELCOME
; 许可协议页面
!insertmacro MUI_PAGE_LICENSE "bmp txt\Licence.txt"
; 安装目录选择页面
Page custom ShowRegistration CheckSerial
!insertmacro MUI_PAGE_DIRECTORY
; 开始菜单设置页面
var ICONS_GROUP
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "FileBackup6"
;!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
;!define MUI_STARTMENUPAGE_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
;!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${PRODUCT_STARTMENU_REGVAL}"
!insertmacro MUI_PAGE_STARTMENU Application $ICONS_GROUP

; 安装过程页面
!insertmacro MUI_PAGE_INSTFILES
; readme页面
Page custom showreadme ReadMe
;pa   Untitled01.ini
; 安装完成页面
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\ReadMe.txt"
!define MUI_FINISHPAGE_SHOWREADME_FUNCTION .REBOOT
!insertmacro root
!insertmacro MUI_PAGE_FINISH


; 安装卸载过程页面
!insertmacro MUI_UNPAGE_INSTFILES

; 安装界面包含的语言设置
!insertmacro MUI_LANGUAGE "Japanese"

; 安装预释放文件
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS
; ------ MUI 现代界面定义结束 ------
;Var INI_VALUE
ReserveFile "Japaneseregistration.ini"
ReserveFile "JapaneseSpecial.ini"
ReserveFile "${NSISDIR}\Plugins\splash.dll"
ReserveFile "bmp txt\SETUP.BMP"


Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
BGFont "MS UI Gothic"
BGGradient  0F0ACF 0F0ACF FFFFFF ;[(0F0ACF 0F0ACF [FFFFFF|notext])]


InstallColors 00FF00 000000

OutFile "sunfSetup.exe"
InstallDir "$PROGRAMFILES\FileBackup"
InstallDirRegKey HKLM "${PRODUCT_UNINST_KEY}" "UninstallString"
ShowInstDetails show
ShowUnInstDetails show
BrandingText "    FrontLine"
RequestExecutionLevel admin
Section "FileBackup" SEC01
SetShellVarContext all
 SetOutPath "$INSTDIR"
  SetOverwrite on
  
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateDirectory "$SMPROGRAMS\$ICONS_GROUP"
  CreateDirectory "$SMPROGRAMS\$ICONS_GROUP\僼傽僀儖僶僢僋傾僢僾"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\僼傽僀儖僶僢僋傾僢僾\僼傽僀儖僶僢僋傾僢僾.lnk" "$INSTDIR\FileBackup.exe"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\僼傽僀儖僶僢僋傾僢僾\僼傽僀儖暅尦.lnk" "$INSTDIR\FileRestore.exe"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\ReadMe.lnk" "$INSTDIR\ReadMe.txt"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Manual.lnk" "$INSTDIR\Manual.pdf"
  !insertmacro MUI_STARTMENU_WRITE_END
  File /r /x 2000 "..\Rel\Japanese\*.*"

  File "..\Rel\SharedFile\2000\ParInfo.dll"
  File "..\Rel\SharedFile\2000\ParMan.dll"
  File "..\Rel\SharedFile\Zlib.dll"
  File "..\Rel\SharedFile\Install.dll"
  File "..\Rel\SharedFile\KERNEL.SYS"
;  File "..\Rel\SharedFile\ErDisk.BIN"
;  File "..\Rel\SharedFile\Boot.DAT"
;  File "..\Rel\SharedFile\ASPIKIT.EXE"
  File "..\Rel\SharedFile\RM.exe"
  SetOutPath "$WINDIR\system32"
  File "..\Rel\SharedFile\2000\WNASPINT.DLL"
  SetOutPath $TEMP\eInspect
  SetOverwrite ifnewer
SectionEnd

Section -AdditionalIcons
SetShellVarContext all
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  !insertmacro MUI_STARTMENU_WRITE_END
  
SectionEnd

Section -Post
SetShellVarContext all
  WriteUninstaller "$INSTDIR\uninst.exe"
;  StrCmp $R6 "6" 0 vista
;  WriteRegStr HKCU "Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers" "$INSTDIR\uninst.exe" "RUNASADMIN"
;  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
;  vista:
  WriteRegStr HKLM "Software\FrontLine\FileBackup\6.00" "UserName" "$2"
  WriteRegStr HKLM "Software\FrontLine\FileBackup\6.00" "Company" "$1"
  WriteRegStr HKLM "Software\FrontLine\FileBackup\6.00" "Password" "$3"
  WriteRegStr HKLM "Software\FrontLine\FileBackup\6.00" "InstallPath" "$INSTDIR"
  WriteRegStr  HKLM "software\Microsoft\Windows\CurrentVersion\Uninstall\FileBackup" "DisplayIcon" "FileBackup"
  WriteRegStr  HKLM "software\Microsoft\Windows\CurrentVersion\Run" "FileBackup" "$INSTDIR\FileBackup.exe"
  
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\AppMainExe.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\AppMainExe.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

#-- 根据 NSIS 脚本编辑规则，所有 Function 区段必须放置在 Section 区段之后编写，以避免安装程序出现未可预知的问题。--#
Function ReadMe

Var /GLOBAL VALUE
    !insertmacro MUI_INSTALLOPTIONS_READ $VALUE "JapaneseSpecial.ini" "Field 2" "State"
    ${If} $VALUE = 1
       ExecShell "open" "$INSTDIR\readme.txt" 
    ${Else}
    ${EndIf}
FunctionEnd

Function .REBOOT
  !insertmacro rootnow
    ;MessageBox MB_YESNO|MB_ICONQUESTION "Are you want to restart my company now." IDNO +2
    Reboot
FunctionEnd
Function .onInit
System::Call "Kernel32::GetVersion(v) i .s"
      Pop $0
      IntOp $R6 $0 & 0xFF
	  ;MessageBox MB_OK "$R6"
      IntOp $R7 $0 & 0xFFFF
      IntOp $R7 $R7 >> 8
      ;StrCmp $R6 "6" 0 +2
      ;Goto +7
      ;MessageBox MB_OK "$R7"
      IntCmpU $0 0x80000000 0 nt
      StrCmp $R7 "0" 0 +2
      !insertmacro ossystem
      ;quit
      ;DetailPrint "Windows 95/98/Me"
        StrCpy $R8 "0"


	  Goto me
       nt:
       StrCpy $R8 "1"
       me:


  InitPluginsDir
  File "/oname=$PLUGINSDIR\Splash_SETUP.BMP" "bmp txt\SETUP.BMP"
  ; 使用闪屏插件显示闪屏
  advsplash::show 1000 600 400 -1 "$PLUGINSDIR\Splash_SETUP"
  ;Pop $0 ; $0 返回 '1' 表示用户提前关闭闪屏, 返回 '0' 表示闪屏正常结束, 返回 '-1' 表示闪屏显示出错
  !insertmacro MUI_INSTALLOPTIONS_EXTRACT "Japaneseregistration.ini"
 
  !insertmacro MUI_INSTALLOPTIONS_EXTRACT "JapaneseSpecial.ini"
  ReadRegStr $R3 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "RegisteredOwner"
  ReadRegStr $R4 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "RegisteredOrganization"
  !insertmacro MUI_INSTALLOPTIONS_WRITE "Japaneseregistration.ini" "Field 5" "State" $R4
  !insertmacro MUI_INSTALLOPTIONS_WRITE "Japaneseregistration.ini" "Field 1" "State" $R3
  SetShellVarContext All
FunctionEnd

/******************************
 *  以下是安装程序的卸载部分  *
 ******************************/

Section Uninstall
SetShellVarContext All
  SetOutPath $TEMP\eInspect
  SetOverwrite ifnewer
  
;  File KillProcDLL.dll
;  KillProcDLL::KillProc "FileBackup.exe"
  
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\Example.file"
  Delete "$INSTDIR\AppMainExe.exe"

  Delete "$SMPROGRAMS\FileBackup6\Uninstall.lnk"
  Delete "$SMPROGRAMS\FileBackup6\Website.lnk"
  
  DeleteRegValue HKCU "Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers" "$INSTDIR\uninst.exe"

  
  Delete "$WINDIR\system32\WNASPINT.DLL"
  Strcpy $R9 $WINDIR 2
  Strcpy $R9 "$R9\ProgramData\Microsoft\Windows\Start Menu\Programs\FileBackup6"
  RMDir /r "$R9"
  RMDir /r "$SMPROGRAMS\FileBackup6"

  RMDir /r "$INSTDIR"
  DeleteRegValue HKLM "software\Microsoft\Windows\CurrentVersion\Run" "FileBackup"
  
  DeleteRegKey HKLM "software\Microsoft\Windows\CurrentVersion\Uninstall\FileBackup"
  DeleteRegKey HKLM "Software\FrontLine\FileBackup"
  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
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
