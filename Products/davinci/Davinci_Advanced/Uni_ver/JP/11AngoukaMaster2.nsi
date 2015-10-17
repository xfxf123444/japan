!include "LogicLib.nsh"
!include "x64.nsh"
; ��װ�����ʼ���峣��
!define PRODUCT_NAME "���Ż��ޥ����`2"
!define PRODUCT_VERSION "2.00"
!define PRODUCT_PUBLISHER "FRONTLINE"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\ShellEncrypt.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

SetCompressor lzma

; ------ MUI �ִ����涨�� (1.67 �汾���ϼ���) ------
!include "MUI.nsh"
!include "UsefulLib.nsh"
${Replace}
;!include "LogicLib.nsh"
!include "..\JP\LANGUAGE.nsh"
!include "..\JP\Check.nsh"
!include "..\JP\finish.nsh"
;!include "Sections.nsh"

; MUI Ԥ���峣��
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; ��ӭҳ��
!insertmacro MUI_PAGE_WELCOME
; ���Э��ҳ��
!insertmacro MUI_PAGE_LICENSE "..\Rel\Japanese\license.txt"
Page custom showreadme
Page custom ShowRegistration CheckSerial
; ��װĿ¼ѡ��ҳ��
!insertmacro MUI_PAGE_DIRECTORY
; ��װ����ҳ��
!insertmacro MUI_PAGE_INSTFILES
; ��װ���ҳ��
Page custom finish REBOOT
;!define MUI_FINISHPAGE_RUN "$INSTDIR\ShellEncrypt.exe"
;!insertmacro MUI_PAGE_FINISH

; ��װж�ع���ҳ��
!insertmacro MUI_UNPAGE_INSTFILES

; ��װ�����������������
!insertmacro MUI_LANGUAGE "Japanese"

; ��װԤ�ͷ��ļ�
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS
; ------ MUI �ִ����涨����� ------
ReserveFile "registration.ini"
ReserveFile "finisha.ini"
ReserveFile "Special.ini"
ReserveFile "..\Rel\Japanese\SETUP.BMP"

Name "���Ż��ޥ����`2"
BGFont "MS UI Gothic"
BGGradient  0F0ACF 0F0ACF FFFFFF ;[(0F0ACF 0F0ACF [FFFFFF|notext])]


InstallColors 00FF00 000000

SetDateSave on
SetDatablockOptimize on
ReserveFile "${NSISDIR}\Plugins\advsplash.dll"
ReserveFile "..\Rel\Japanese\SETUP.BMP"

;Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "Setup.exe"
InstallDir "$PROGRAMFILES\FRONTLINE\���Ż��ޥ����`2"
InstallDirRegKey HKLM "${PRODUCT_UNINST_KEY}" "UninstallString"
ShowInstDetails show
ShowUnInstDetails show
BrandingText "FrontLine"

; ���װ��־��¼������־�ļ�������Ϊж���ļ�������(ע�⣬�����α����������������֮ǰ)
;Section "-LogSetOn"
 ; LogSet on
;SectionEnd
RequestExecutionLevel admin
Section "MainSection" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  File "..\Rel\zlib.dll"
  CreateShortCut "$SMSTARTUP\�����Í����t�H���_.lnk" "$INSTDIR\YGFSMon.exe"
  CreateDirectory "$SMPROGRAMS\���Ż��ޥ����`2"
  CreateShortCut "$SMPROGRAMS\���Ż��ޥ����`2\���Ż��ޥ����`2.lnk" "$INSTDIR\FileEncrypt.exe"
  CreateShortCut "$SMPROGRAMS\���Ż��ޥ����`2\�f�T�C�t�@�[201.lnk" "$INSTDIR\FileDecrypt.exe"
  CreateShortCut "$SMPROGRAMS\���Ż��ޥ����`2\�����Í����t�H���_.lnk" "$INSTDIR\YGFSMon.exe"
  CreateShortCut "$SMPROGRAMS\���Ż��ޥ����`2\Manual.lnk" "$INSTDIR\Manual.pdf"
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
  ;CreateShortCut "$SMPROGRAMS\���Ż��ޥ����`2\Uninstall.lnk" "$INSTDIR\uninst.exe"
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
	WriteRegStr HKCR "*\shell" "�Í���" "�Í���"
  WriteRegStr HKCR "*\shell\�Í���\command" "" "$INSTDIR\ShellEncrypt.exe *%1"
  WriteRegStr HKCR "Directory\shell" "�Í���" "�Í���"
  WriteRegStr HKCR "Directory\shell\�Í���\command" "" "$INSTDIR\ShellEncrypt.exe *%1"
  WriteRegStr HKCR ".chy\shell" "������" "������"
  WriteRegStr HKCR ".chy\shell\������\command" "" "$INSTDIR\ShellDecrypt.exe *%1"
  WriteRegStr HKCR ".chy\shell\open\command" "" "$INSTDIR\ShellDecrypt.exe ?%1"
    WriteRegStr HKCR "*\shell" "���S����" "���S����"
  WriteRegStr HKCR "*\shell\���S����\command" "" "$\"$INSTDIR\DelFile.exe$\" $\"%1$\""
  WriteRegStr HKCR "Directory\shell" "���S����" "���S����"
  WriteRegStr HKCR "Directory\shell\���S����\command" "" "$\"$INSTDIR\DelFile.exe$\" $\"%1$\""
	;end
SectionEnd

#-- ���� NSIS �ű��༭�������� Function ���α�������� Section ����֮���д���Ա��ⰲװ�������δ��Ԥ֪�����⡣--#
Function REBOOT
;SetOutPath $INSTDIR
Var /GLOBAL INI_VALUE
    !insertmacro MUI_INSTALLOPTIONS_READ $INI_VALUE "finisha.ini" "Field 4" "State"
    ${If} $INI_VALUE = 1
    ;MessageBox MB_YESNO|MB_ICONQUESTION "$(^NameDA) �̃C���X�g�[������������ɂ́A���̃R���s���[�^���ċN������K�v������܂��B�������ċN�����܂����H." IDNO +2
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
  ; ʹ�����������ʾ����
  ;advsplash::show 1000 600 400 -1 "$PLUGINSDIR\Splash_SETUP"
  Pop $0 ; $0 ���� '1' ��ʾ�û���ǰ�ر�����, ���� '0' ��ʾ������������, ���� '-1' ��ʾ������ʾ����
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
 *  �����ǰ�װ�����ж�ز���  *
 ******************************/

; ���ݰ�װ��־ж���ļ��ĵ��ú�
!macro DelFileByLog LogFile
  ifFileExists `${LogFile}` 0 +4
    Push `${LogFile}`
    Call un.DelFileByLog
    Delete `${LogFile}`
!macroend

Section Uninstall

  ; ���ú�ֻ���ݰ�װ��־ж�ذ�װ�����Լ���װ�����ļ�
  !insertmacro DelFileByLog "$INSTDIR\install.log"

  ; �����װ���򴴽�������ж��ʱ����Ϊ�յ���Ŀ¼�����ڵݹ���ӵ��ļ�Ŀ¼���������ڲ����Ŀ¼��ʼ���(ע�⣬��Ҫ�� /r �����������ʧȥ DelFileByLog ������)
  SetShellVarContext All
  RMDir /r "$SMPROGRAMS\���Ż��ޥ����`2"
  Delete "$SMSTARTUP\�����Í����t�H���_.lnk"
  Strcpy $R9 $WINDIR 2
  Strcpy $R9 "$R9\ProgramData\Microsoft\Windows\Start Menu\Programs\���Ż��ޥ����`2"
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
    DeleteRegValue HKCR "*\shell" "�Í���"
  DeleteRegKey HKCR "*\shell\�Í���"
    DeleteRegValue HKCR "Directory\shell" "�Í���"
  DeleteRegKey HKCR "Directory\shell\�Í���"
		DeleteRegKey HKCR ".chy"
		  DeleteRegValue HKCR "*\shell" "���S����"
  DeleteRegKey HKCR "*\shell\���S����"
    DeleteRegValue HKCR "Directory\shell" "���S����"
  DeleteRegKey HKCR "Directory\shell\���S����"
  ;end
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

; ������ж�س���ͨ����װ��־ж���ļ���ר�ú������벻Ҫ�����޸�
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
