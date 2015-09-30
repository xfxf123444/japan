; �ýű�ʹ�� HM VNISEdit �ű��༭���򵼲���

!include "LogicLib.nsh"
!include "x64.nsh"
; ��װ�����ʼ���峣��
!define PRODUCT_NAME "DELETEMASTER"
!define PRODUCT_VERSION "4.00"
!define PRODUCT_PUBLISHER "FrontLine"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\AppMainExe.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
;var bootware
;var readme

SetCompressor lzma

; ------ MUI �ִ����涨�� (1.67 �汾���ϼ���) ------
;!include "WinMessages.nsh"

!include "MUI.nsh"
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
;!define MUI_PAGE_CUSTOMFUNCTION_SHOW ChangeFont
!insertmacro MUI_PAGE_WELCOME
; ���Э��ҳ��
!insertmacro MUI_PAGE_LICENSE "bmptxt\License.txt"
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
ReserveFile "registration.ini"
ReserveFile "finisha.ini"
;ReserveFile "finishb.ini"
ReserveFile "Special.ini"
ReserveFile "${NSISDIR}\Plugins\advsplash.dll"
ReserveFile "bmptxt\SETUP.BMP"

Name "DELETEMASTER 4.00"                    
BGFont "MS UI Gothic"
BGGradient  0F0ACF 0F0ACF FFFFFF ;[(0F0ACF 0F0ACF [FFFFFF|notext])]


InstallColors 00FF00 000000

OutFile "DeleSetup.exe"
SetDateSave on
SetDatablockOptimize on

InstallDir "$PROGRAMFILES\FrontLine\���S�����}�X�^�["
;InstallDirRegKey HKLM "${PRODUCT_UNINST_KEY}" "UninstallString"
ShowInstDetails nevershow
      ;show
ShowUnInstDetails show
BrandingText "    FrontLine"

; ���װ��־��¼������־�ļ�������Ϊж���ļ�������(ע�⣬�����α����������������֮ǰ)
;Section "-LogSetOn"
;  LogSet on
;SectionEnd
RequestExecutionLevel admin

Section "DELETEMASTER" SEC01
 SetShellVarContext all
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  File "..\Rel\Japanese\DMF.exe"

  File "..\Rel\Japanese\DelFile.exe"
  File "..\Rel\Japanese\DMFSrv.exe"
  File "..\Rel\Japanese\DeleteMaster.exe"
  Rename $SYSDIR\DmExt.dll $SYSDIR\DDmExt.dll
  Delete /REBOOTOK "$SYSDIR\DDmExt.dll"
  SetOutPath "$SYSDIR"
  SetOverwrite ifnewer
  File "..\Rel\Japanese\DmExt.dll"
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  File "..\Rel\Shared\ParMan.dll"
  File "..\Rel\Shared\ParInfo.dll"
  WriteRegStr HKCU "Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers" "$INSTDIR\DMFSrv.exe" "RUNASADMIN"
  WriteRegStr HKCU "Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers" "$SYSDIR\sc.exe" "RUNASADMIN"
  ExecWait '"$INSTDIR\DMFSrv.exe"  -remove'
  
  ExecWait '"$INSTDIR\DMFSrv.exe"  -install'
  ExecWait '"sc" start "Delete File Service"'
  DeleteRegValue HKCU "Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers" "$SYSDIR\sc.exe"
  CreateDirectory "$SMPROGRAMS\���S�����}�X�^�["
  CreateShortCut "$SMPROGRAMS\���S�����}�X�^�[\���S�����}�X�^�[.lnk" "$INSTDIR\DeleteMaster.exe"
  CreateShortCut "$SMPROGRAMS\���S�����}�X�^�[\�t�@�C������.lnk" "$INSTDIR\DelFile.exe"
  CreateShortCut "$SMPROGRAMS\���S�����}�X�^�[\Uninstall.lnk" "$INSTDIR\uninst.exe"
  CreateShortCut "$DESKTOP\�t�@�C������.lnk" "$INSTDIR\DelFile.exe"
  CreateShortCut "$DESKTOP\���S�����}�X�^�[.lnk" "$INSTDIR\DeleteMaster.exe"
	WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKCU "Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers" "$INSTDIR\DeleteMaster.exe" "RUNASADMIN"
  ;WriteRegStr HKCU "Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers" "$INSTDIR\DelFile.exe" "RUNASADMIN"
  WriteRegStr HKCU "Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers" "$INSTDIR\uninst.exe" "RUNASADMIN"
  WriteRegStr HKCR "*\shell" "���S����" "���S����"
  WriteRegStr HKCR "*\shell\���S����\command" "" "$\"$INSTDIR\DelFile.exe$\" $\"%1$\""
  WriteRegStr HKCR "Directory\shell" "���S����" "���S����"
  WriteRegStr HKCR "Directory\shell\���S����\command" "" "$\"$INSTDIR\DelFile.exe$\" $\"%1$\""
;  WriteRegStr HKCR "CLSID\{645FF040-5081-101B-9F08-00AA002F954E}\shell" "���S����" "���S����"
;  WriteRegStr HKCR "CLSID\{645FF040-5081-101B-9F08-00AA002F954E}\shell\���S����\command" "" "$\"$INSTDIR\DelFile.exe$\" $\"DeleteFileInRecycled$\""
;    WriteRegStr HKCR "*\ShellFolder" "���S����" "���S����"
;  WriteRegStr HKCR "*\ShellFolder\���S����\command" "" "$\"$INSTDIR\DelFile.exe$\" $\"%1$\""
;  WriteRegStr HKCR "Directory\ShellFolder" "���S����" "���S����"
;  WriteRegStr HKCR "Directory\ShellFolder\���S����\command" "" "$\"$INSTDIR\DelFile.exe$\" $\"%1$\""
;  WriteRegStr HKCR "CLSID\{645FF040-5081-101B-9F08-00AA002F954E}\ShellFolder" "���S����" "���S����"
;  WriteRegStr HKCR "CLSID\{645FF040-5081-101B-9F08-00AA002F954E}\ShellFolder\���S����\command" "" "$\"$INSTDIR\DelFile.exe$\" $\"DeleteFileInRecycled$\""
  WriteRegStr HKCR "*\shell" "���ݔ��̊��S����" "���ݔ��̊��S����"
  WriteRegStr HKCR "*\shell\���ݔ��̊��S����\command" "" "$\"$INSTDIR\DelFile.exe$\" $\"DeleteFileInRecycled$\""
  WriteRegStr HKCR "Directory\shell" "���ݔ��̊��S����" "���ݔ��̊��S����"
  WriteRegStr HKCR "Directory\shell\���ݔ��̊��S����\command" "" "$\"$INSTDIR\DelFile.exe$\" $\"DeleteFileInRecycled$\""
  WriteRegStr HKLM "Software\FrontLine\���S�����}�X�^�[\4.00" "UserName" "$2"
  WriteRegStr HKLM "Software\FrontLine\���S�����}�X�^�[\4.00" "Company" "$1"
  WriteRegStr HKLM "Software\FrontLine\���S�����}�X�^�[\4.00" "Password" "$3"
  WriteRegStr HKLM "Software\FrontLine" "DFInstallPath" "$INSTDIR"

;  WriteRegStr HKCR "*\shellex\ContextMenuHandlers\DMFile" "" "{510202d2-ba05-4e09-8911-31e019995af0}"
;  WriteRegStr HKCR "CLSID\{510202d2-ba05-4e09-8911-31e019995af0}" "" "DMFile"
;  WriteRegStr HKCR "CLSID\{510202d2-ba05-4e09-8911-31e019995af0}\InProcServer32" "" "DmExt.dll"
;  WriteRegStr HKCR "CLSID\{510202d2-ba05-4e09-8911-31e019995af0}\InProcServer32" "ThreadingModel" "Apartment"
;  WriteRegStr HKCR "Directory\shellex\ContextMenuHandlers\DMFile" "" "{510202d2-ba05-4e09-8911-31e019995af0}"
;  WriteRegStr HKCR "CLSID\{645FF040-5081-101B-9F08-00AA002F954E}\shellex\ContextMenuHandlers\DMFile" "" "{510202d2-ba05-4e09-8911-31e019995af0}"





  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\DMF.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\DMF.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

Function REBOOT
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
  File "/oname=$PLUGINSDIR\win.bmp" "bmptxt\win.bmp"
  File "/oname=$PLUGINSDIR\ReadMe.txt" "bmptxt\ReadMe.txt"
 !insertmacro MUI_INSTALLOPTIONS_EXTRACT "Special.ini"
 !insertmacro MUI_INSTALLOPTIONS_EXTRACT "finisha.ini"
 ;!insertmacro MUI_INSTALLOPTIONS_EXTRACT "ioSpecial.ini"
!insertmacro MUI_INSTALLOPTIONS_EXTRACT "registration.ini"
 WriteINIStr "$PLUGINSDIR\finisha.ini" "Field 1" "text" "$PLUGINSDIR\win.bmp"
  InitPluginsDir
  File "/oname=$PLUGINSDIR\Splash_SETUP.BMP" "bmptxt\SETUP.BMP"
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
SetShellVarContext all
  ; ���ú�ֻ���ݰ�װ��־ж�ذ�װ�����Լ���װ�����ļ�
  !insertmacro DelFileByLog "$INSTDIR\install.log"

  ; �����װ���򴴽�������ж��ʱ����Ϊ�յ���Ŀ¼�����ڵݹ���ӵ��ļ�Ŀ¼���������ڲ����Ŀ¼��ʼ���(ע�⣬��Ҫ�� /r �����������ʧȥ DelFileByLog ������)
  ExecWait '"$INSTDIR\DMFSrv.exe" -remove'
  Delete "$DESKTOP\�t�@�C������.lnk"
	SetShellVarContext All
  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
  Delete "$SYSDIR\Iosubsys\VirParti.vxd"
  Strcpy $R9 $WINDIR 2
  Strcpy $R9 "$R9\ProgramData\Microsoft\Windows\Start Menu\Programs\���S�����}�X�^�["
  RMDir /r "$R9"
  RMDir /r "$SMPROGRAMS\���S�����}�X�^�["
  
  Delete "$DESKTOP\�t�@�C������.lnk"
  Delete "$DESKTOP\���S�����}�X�^�[.lnk"
  Delete "$SMPROGRAMS\���S�����}�X�^�[\���S�����}�X�^�[.lnk"
  Delete "$SMPROGRAMS\���S�����}�X�^�[\�t�@�C������.lnk"
  Delete "$SMPROGRAMS\���S�����}�X�^�[\Uninstall.lnk"
  RMDir /r "$SMPROGRAMS\���S�����}�X�^�["
  RMDir /r /REBOOTOK "$SMPROGRAMS\���S�����}�X�^�["
  RMDir /r "$INSTDIR"
  Delete /REBOOTOK "$SYSDIR\DmExt.dll"
  RMDir "$INSTDIR"

  DeleteRegValue HKCR "CLSID\{645FF040-5081-101B-9F08-00AA002F954E}\ShellFolder" "���S����"
  DeleteRegKey HKCR "CLSID\{645FF040-5081-101B-9F08-00AA002F954E}\ShellFolder\���S����"
  DeleteRegValue HKCR "*\ShellFolder" "���S����"
  DeleteRegKey HKCR "*\ShellFolder\���S����"
    DeleteRegValue HKCR "Directory\ShellFolder" "���S����"
  DeleteRegKey HKCR "Directory\ShellFolder\���S����"
    DeleteRegValue HKCR "CLSID\{645FF040-5081-101B-9F08-00AA002F954E}\shell" "���S����"
  DeleteRegKey HKCR "CLSID\{645FF040-5081-101B-9F08-00AA002F954E}\shell\���S����"
  DeleteRegValue HKCR "*\shell" "���S����"
  DeleteRegKey HKCR "*\shell\���S����"
    DeleteRegValue HKCR "Directory\shell" "���S����"
  DeleteRegKey HKCR "Directory\shell\���S����"
  DeleteRegValue HKCR "*\shell" "���ݔ��̊��S����"
  DeleteRegKey HKCR "*\shell\���ݔ��̊��S����"
    DeleteRegValue HKCR "Directory\shell" "���ݔ��̊��S����"
  DeleteRegKey HKCR "Directory\shell\���ݔ��̊��S����"
  DeleteRegKey HKLM "Software\FrontLine\���S�����}�X�^�["
  DeleteRegValue HKLM "Software\FrontLine" "DFInstallPath"
  DeleteRegValue HKCU "Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers" "$INSTDIR\DeleteMaster.exe" 
  DeleteRegValue HKCU "Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers" "$INSTDIR\DelFile.exe" 
  DeleteRegValue HKCU "Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers" "$INSTDIR\uninst.exe" 
  DeleteRegValue HKCU "Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers" "$INSTDIR\DMFSrv.exe"
  DeleteRegValue HKCR "*\shellex\ContextMenuHandlers\DMFile" "" 
  DeleteRegValue HKCR "CLSID\{510202d2-ba05-4e09-8911-31e019995af0}" "" 
  DeleteRegValue HKCR "CLSID\{510202d2-ba05-4e09-8911-31e019995af0}\InProcServer32" "" 
  DeleteRegValue HKCR "CLSID\{510202d2-ba05-4e09-8911-31e019995af0}\InProcServer32" "ThreadingModel" 
  DeleteRegValue HKCR "Directory\shellex\ContextMenuHandlers\DMFile" "" 


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
