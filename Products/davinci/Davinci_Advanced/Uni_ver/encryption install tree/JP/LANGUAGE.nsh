
!macro MUI_ShowRegistration_SUBTITLE
  !insertmacro MUI_HEADER_TEXT "ご利用になる方の、お名前、所属と、製品のシリアル番号を入力してください。" " "
!macroend

!macro MUI_showreadme_SUBTITLE
  !insertmacro MUI_HEADER_TEXT "お読みください" ""
!macroend

!macro ossystem
  messagebox  MB_ICONINFORMATION|MB_OK "本製品は、このバージョンのオペレーティングシステムをサポートしていません。$\r $\n バージョンアップについては、発売元までお問い合わせください。"
!macroend

!macro admin
  messageBox MB_OK "この製品をインストールするには、このコンピュータの管理者ユーザである必要があります。管理者ユーザとしてログオンして、改めて、インストールを行ってください。"
!macroend

!macro serial
  MessageBox MB_ICONSTOP|MB_YESNO "無効なシリアル番号が入力されました。処理を続けるためには正しいシリアル番号を入力してください。正しいシリアル番号を$\r入力せずに処理を続けることはできません。$\r $\nシリアル番号を入力し直しますか。." IDno +2
!macroend

!macro shd2
   MessageBox MB_ICONSTOP|MB_YESNO|MB_ICONEXCLAMATION "このアプリケーションは、既にインストールされています.$\r $\n[はい]をクリックすると、古いバージョンをアンインストールして、新規にインストールします。.$\r $\n[いいえ]をクリックすると、上書きでインストールします。." IDYES tr2 IDNO fa2
!macroend

!macro shd1
   MessageBox MB_ICONSTOP|MB_YESNO|MB_ICONEXCLAMATION "このアプリケーションは、既にインストールされています.$\r $\n[はい]をクリックすると、古いバージョンをアンインストールして、新規にインストールします。.$\r $\n[いいえ]をクリックすると、上書きでインストールします。." IDYES tr1 IDNO fa1
!macroend

!macro shd
   MessageBox MB_ICONSTOP|MB_YESNO|MB_ICONEXCLAMATION "このアプリケーションは、既にインストールされています.$\r $\n[はい]をクリックすると、古いバージョンをアンインストールして、新規にインストールします。.$\r $\n[いいえ]をクリックすると、上書きでインストールします。." IDYES tr IDNO fa
!macroend                                              

!macro root
  !define MUI_FINISHPAGE_SHOWREADME_TEXT RebootNow
!macroend

!macro rootnow
 MessageBox MB_YESNO|MB_ICONQUESTION "コンピュータを再起動してもよろしいですか" IDNO +2
!macroend

!macro beforun
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "本当に、 $(^Name) と、その関連のコンポーネントを完全に削除しますか?" IDYES +2
!macroend

!macro afterun
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) はシステムから取り除かれました。アンインストール処理の際に使用中だったファイルを確実に削除するために、マシンを再起動してください。"
!macroend





