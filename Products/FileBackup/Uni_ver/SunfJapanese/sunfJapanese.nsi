; �ýű�ʹ�� HM VNISEdit �ű��༭���򵼲���

; ��װ�����ʼ���峣��
!define PRODUCT_NAME "FileBackup"
!define PRODUCT_VERSION "6.00"
!define PRODUCT_PUBLISHER "FrontLine"
!define PRODUCT_WEB_SITE "http://www.mycompany.com"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\AppMainExe.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

SetCompressor lzma

; ------ MUI �ִ����涨�� (1.67 �汾���ϼ���) ------

!include "MUI.nsh"
!define REG_CODE "sample"

!include "LogicLib.nsh"
!include "..\SunfJapanese\sunfJapanese.nsh"
!include "..\SunfJapanese\JapaneseCheck.nsh"

; MUI Ԥ���峣��
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; ��ӭҳ��
!insertmacro MUI_PAGE_WELCOME
; ���Э��ҳ��
!insertmacro MUI_PAGE_LICENSE "bmp txt\Licence.txt"
; ��װĿ¼ѡ��ҳ��
Page custom ShowRegistration CheckSerial
!insertmacro MUI_PAGE_DIRECTORY
; ��ʼ�˵�����ҳ��
var ICONS_GROUP
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "FileBackup6"
;!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
;!define MUI_STARTMENUPAGE_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
;!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${PRODUCT_STARTMENU_REGVAL}"
!insertmacro MUI_PAGE_STARTMENU Application $ICONS_GROUP

; ��װ����ҳ��
!insertmacro MUI_PAGE_INSTFILES
; readmeҳ��
Page custom showreadme ReadMe
;pa   Untitled01.ini
; ��װ���ҳ��
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\ReadMe.txt"
!define MUI_FINISHPAGE_SHOWREADME_FUNCTION .REBOOT
!insertmacro root
!insertmacro MUI_PAGE_FINISH


; ��װж�ع���ҳ��
!insertmacro MUI_UNPAGE_INSTFILES

; ��װ�����������������
!insertmacro MUI_LANGUAGE "Japanese"

; ��װԤ�ͷ��ļ�
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS
; ------ MUI �ִ����涨����� ------
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
  CreateDirectory "$SMPROGRAMS\$ICONS_GROUP\�t�@�C���o�b�N�A�b�v"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\�t�@�C���o�b�N�A�b�v\�t�@�C���o�b�N�A�b�v.lnk" "$INSTDIR\FileBackup.exe"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\�t�@�C���o�b�N�A�b�v\�t�@�C������.lnk" "$INSTDIR\FileRestore.exe"
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

#-- ���� NSIS �ű��༭�������� Function ���α�������� Section ����֮���д���Ա��ⰲװ�������δ��Ԥ֪�����⡣--#
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
  ; ʹ�����������ʾ����
  advsplash::show 1000 600 400 -1 "$PLUGINSDIR\Splash_SETUP"
  ;Pop $0 ; $0 ���� '1' ��ʾ�û���ǰ�ر�����, ���� '0' ��ʾ������������, ���� '-1' ��ʾ������ʾ����
  !insertmacro MUI_INSTALLOPTIONS_EXTRACT "Japaneseregistration.ini"
 
  !insertmacro MUI_INSTALLOPTIONS_EXTRACT "JapaneseSpecial.ini"
  ReadRegStr $R3 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "RegisteredOwner"
  ReadRegStr $R4 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "RegisteredOrganization"
  !insertmacro MUI_INSTALLOPTIONS_WRITE "Japaneseregistration.ini" "Field 5" "State" $R4
  !insertmacro MUI_INSTALLOPTIONS_WRITE "Japaneseregistration.ini" "Field 1" "State" $R3
  SetShellVarContext All
FunctionEnd

/******************************
 *  �����ǰ�װ�����ж�ز���  *
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

#-- ���� NSIS �ű��༭�������� Function ���α�������� Section ����֮���д���Ա��ⰲװ�������δ��Ԥ֪�����⡣--#

Function un.onInit
!insertmacro beforun

  Abort
FunctionEnd

Function un.onUninstSuccess
  HideWindow
!insertmacro afterun
FunctionEnd
