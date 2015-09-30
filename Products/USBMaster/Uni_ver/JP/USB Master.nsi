; �ýű�ʹ�� HM VNISEdit �ű��༭���򵼲���

; ��װ�����ʼ���峣��
!define PRODUCT_NAME "USB Master 2"
!define PRODUCT_VERSION "2.00"
!define PRODUCT_PUBLISHER "FrontLine"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\USBMaster.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

SetCompressor lzma

; ------ MUI �ִ����涨�� (1.67 �汾���ϼ���) ------

!include "MUI.nsh"
!include "LogicLib.nsh"
!include "..\JP\LANGUAGE.nsh"
!include "..\JP\Check.nsh"
!include "..\JP\finish.nsh"
!include "x64.nsh"

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

; ��װж�ع���ҳ��
!insertmacro MUI_UNPAGE_INSTFILES

; ��װ�����������������
!insertmacro MUI_LANGUAGE "Japanese"

; ��װԤ�ͷ��ļ�
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS
; ------ MUI �ִ����涨����� ------
ReserveFile "${NSISDIR}\Plugins\advsplash.dll"
ReserveFile "registration.ini"
ReserveFile "Special.ini"
Name "USB Master 2"
BGFont "MS UI Gothic"
BGGradient  0F0ACF 0F0ACF FFFFFF ;[(0F0ACF 0F0ACF [FFFFFF|notext])]
InstallColors 00FF00 000000
BrandingText "    FrontLine"


OutFile "Setup.exe"
InstallDir "$PROGRAMFILES\USB Master 2"
InstallDirRegKey HKLM "${PRODUCT_UNINST_KEY}" "UninstallString"
ShowInstDetails show
ShowUnInstDetails show

; ���װ��־��¼������־�ļ�������Ϊж���ļ�������(ע�⣬�����α����������������֮ǰ)
;Section "-LogSetOn"
;  LogSet on
;SectionEnd

Section "USB Master 2" SEC01


  CreateDirectory "$SMPROGRAMS\USB Master 2"
  CreateShortCut "$SMPROGRAMS\USB Master 2\USBMaster.lnk" "$INSTDIR\USBMaster.exe"
  CreateShortCut "$SMPROGRAMS\USB Master 2\MainCarrier.lnk" "$INSTDIR\MainCarrier.exe"
  CreateShortCut "$SMPROGRAMS\USB Master 2\SimpleVault.lnk" "$INSTDIR\SimpleVault.exe"
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  
  File "..\rel\Japanese\MainCarrier.exe"
  File "..\rel\Japanese\SubCarrier.exe"
  File "..\rel\user_shell_folder.dat"
  File "..\Rel\Japanese\SimpleVault.exe"
  File "..\Rel\Japanese\USBAM01DLL.dll"
  File "..\Rel\USBDelFile.exe"

   SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  File "..\Rel\Japanese\USBMaster.exe"
  File "..\Rel\Japanese\ParMan.dll"
  File "..\Rel\Japanese\FileLoc.dll"
  File "..\Rel\Japanese\ParInfo.dll"
  ${if} ${RunningX64}
  ${else}
  SetOutPath "$SYSDIR\drivers"
  SetOverwrite ifnewer
  File "..\Rel\Japanese\YGCrydisk.sys"
  WriteRegDWORD HKLM "SYSTEM\CurrentControlSet\Services\YGCrydisk" "ErrorControl" 0x00000001
  WriteRegDWORD HKLM "SYSTEM\CurrentControlSet\Services\YGCrydisk" "Start" 0x00000001
  WriteRegDWORD HKLM "SYSTEM\CurrentControlSet\Services\YGCrydisk" "Type" 0x00000001
  ${endif}

  WriteRegStr HKCU "Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers" "$INSTDIR\USBMaster.exe" "RUNASADMIN"
  WriteRegStr HKCU "Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers" "$INSTDIR\SimpleVault.exe" "RUNASADMIN"
  WriteRegStr HKCU "Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers" "$INSTDIR\uninst.exe" "RUNASADMIN"
  

  SetOutPath $TEMP\eInspect
  SetOverwrite ifnewer
  File "..\Rel\Japanese\ParInfo.dll"
  WriteRegStr HKLM "Software\YuGuang\USB Master 2\2.00" "UserName" "$2"
  WriteRegStr HKLM "Software\YuGuang\USB Master 2\2.00" "Company" "$1"
  WriteRegStr HKLM "Software\YuGuang\USB Master 2\2.00" "Password" "$3"
  WriteRegStr HKLM "Software\YuGuang\USB Master 2\2.00" "DFInstallPath" "$INSTDIR"
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\MainCarrier.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\USBMaster.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
  
  
  
SectionEnd
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
   File "/oname=$PLUGINSDIR\win.bmp" "..\Rel\win.bmp"
  ;File "/oname=$PLUGINSDIR\ReadMe.txt" "bmptxt\ReadMe.txt"

 !insertmacro MUI_INSTALLOPTIONS_EXTRACT "finisha.ini"
 ;!insertmacro MUI_INSTALLOPTIONS_EXTRACT "ioSpecial.ini"
!insertmacro MUI_INSTALLOPTIONS_EXTRACT "registration.ini"
 WriteINIStr "$PLUGINSDIR\finisha.ini" "Field 1" "text" "$PLUGINSDIR\win.bmp"

  File /oname=$PLUGINSDIR\Special.ini "Special.ini"
   File "/oname=$PLUGINSDIR\ReadMe.txt" "..\Rel\Japanese\ReadMe.txt"
 !insertmacro MUI_INSTALLOPTIONS_EXTRACT "Special.ini"

 ;!insertmacro MUI_INSTALLOPTIONS_EXTRACT "ioSpecial.ini"
!insertmacro MUI_INSTALLOPTIONS_EXTRACT "registration.ini"
InitPluginsDir
  File "/oname=$PLUGINSDIR\Splash_SETUP.BMP" "..\Rel\SETUP.BMP"
  ; ʹ�����������ʾ����
  advsplash::show 1000 600 400 -1 "$PLUGINSDIR\Splash_SETUP"
  ;Pop $0 ; $0 ���� '1' ��ʾ�û���ǰ�ر�����, ���� '0' ��ʾ������������, ���� '-1' ��ʾ������ʾ����
  ;!insertmacro MUI_LANGDLL_DISPLAY
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

  ${if} ${RunningX64}
  ${else}
  Delete "$SYSDIR\drivers\YGCrydisk.sys"
 	DeleteRegKey HKLM "SYSTEM\CurrentControlSet\Services\YGCrydisk"
  ${endif}
  
  DeleteRegValue HKCU "Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers" "$INSTDIR\USBMaster.exe"
  DeleteRegValue HKCU "Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers" "$INSTDIR\SimpleVault.exe"
  DeleteRegValue HKCU "Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers" "$INSTDIR\uninst.exe"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true

  RMDir /r "$INSTDIR"
  Delete "$SMPROGRAMS\USB Master 2\MainCarrier.lnk"
  Delete "$SMPROGRAMS\USB Master 2\SimpleVault.lnk"
  Delete "$SMPROGRAMS\USB Master 2\USBMaster.lnk"
  RMDir /r /REBOOTOK "$SMPROGRAMS\USB Master 2"
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
