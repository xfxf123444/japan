Function finish

;StrCmp $R8 "0" me 0
;StrCmp $R6 "6" 0 nt
;me:
Var /GLOBAL INI
Var /GLOBAL ZT

InstallOptions::initDialog /NOUNLOAD "$PLUGINSDIR\finisha.ini"
  Pop $0
      ReadINIStr $INI "$PLUGINSDIR\finisha.ini" "Field 3" "HWND"
      SetCtlColors $INI 0x000000 0xFFFFFF
      CreateFont $ZT "Tahoma" "11" "700"
      SendMessage $INI ${WM_SETTEXT} 0 "STR:    $(^NameDA) セットアップ $\nウィザードは完了しました。"
      SendMessage $INI ${WM_SETFONT} $ZT 0
      
      ReadINIStr $INI "$PLUGINSDIR\finisha.ini" "Field 2" "HWND"
      SetCtlColors $INI 0x000000 0xFFFFFF
      SendMessage $INI ${WM_SETTEXT} 0 "STR:    $(^NameDA)は、このコンピュータ$\nにインストールされました。$\r$\n$\r$\nウィザードを閉じるには [完了] を押してください。"
      
      ReadINIStr $INI "$PLUGINSDIR\finisha.ini" "Field 4" "HWND"
      SendMessage $INI ${WM_SETTEXT} 0 "STR:今すぐ再起動する"
      SetCtlColors $INI 0x000000 0xFFFFFF
      ReadINIStr $INI "$PLUGINSDIR\finisha.ini" "Field 5" "HWND"
      SendMessage $INI ${WM_SETTEXT} 0 "STR:後で手動で再起動する"
      SetCtlColors $INI 0x000000 0xFFFFFF
      ReadINIStr $INI "$PLUGINSDIR\finisha.ini" "Field 6" "HWND"
      SetCtlColors $INI 0x000000 0xFFFFFF
      ReadINIStr $INI "$PLUGINSDIR\finisha.ini" "Field 7" "HWND"
      SetCtlColors $INI 0x000000 0xFFFFFF
  
  
  InstallOptions::show
  ; Finally fetch the InstallOptions status value (we don't care what it is though)
  Pop $0
;goto mm
      ;nt:
      
      !insertmacro MUI_INSTALLOPTIONS_WRITE "finisha.ini" "Settings" "NumFields" "3"
      !insertmacro MUI_INSTALLOPTIONS_WRITE "finisha.ini" "Field 2" "Bottom" "200"

FunctionEnd


