================================================================================
MICROSOFT FOUNDATION CLASS ライブラリ: SelfEntractionConvertingTool プロジェクトの概要
===============================================================================

アプリケーション ウィザードが作成した SelfEntractionConvertingTool には Microsoft 
Foundation Class の基本的な使い方が示されています。アプリケーション作成のひな型
としてお使いください。

このファイルには SelfEntractionConvertingTool アプリケーションを構成している各ファイ
ルの概要説明が含まれます。

SelfEntractionConvertingTool.vcxproj
   これはアプリケーション ウィザードで生成された VC++ プロジェクトのメイン プ
   ロジェクト ファイルです。ファイルが生成された Visual C++ のバージョン情報が
   含まれています。また、アプリケーション ウィザードで選択したプラットフォー
   ム、構成およびプロジェクト機能に関する情報も含まれています。

SelfEntractionConvertingTool.vcxproj.filters
   これは、アプリケーション ウィザードで生成された VC++ プロジェクトの
   フィルター ファイルです。
   このファイルには、プロジェクト内のファイルとフィルターとの間の関連付けに関する
   情報が含まれています。 この関連付けは、特定のノードで同様の拡張子を持つファイルの
   グループ化を示すために IDE で使用されます (たとえば、".cpp" ファイルは "ソース 
   ファイル" フィルターに関連付けられています)。

SelfEntractionConvertingTool.h
   これはアプリケーションのメイン ヘッダー ファイルです。このファイルには、
   Resource.h を含む、その他のプロジェクト固有のヘッダーが含まれていて、
   CSelfEntractionConvertingToolApp アプリケーション クラスを宣言します。

SelfEntractionConvertingTool.cpp
   これは、アプリケーション クラス CSelfEntractionConvertingToolApp を含むメイン アプリケー
   ション ソース ファイルです。

SelfEntractionConvertingTool.rc
   これは、プログラムが使用する Microsoft Windows のリソースの一覧ファイルで
   す。このファイルには RES サブディレクトリに保存されているアイコン、ビットマ
   ップ、カーソルが含まれています。このファイルは、Microsoft Visual C++ で直接
   編集することができます。プロジェクト リソースは 1041 にあります。

res\SelfEntractionConvertingTool.ico
   これは、アプリケーションのアイコンとして使用されるアイコンファイルです。この
   アイコンはメイン リソース ファイル SelfEntractionConvertingTool.rc に含まれていま
   す。

res\SelfEntractionConvertingTool.rc2
   このファイルは Microsoft Visual C++ を使用しないで編集されたリソースを含んで
   います。リソース エディタで編集できないリソースはすべてこのファイルに入れて
   ください。


/////////////////////////////////////////////////////////////////////////////

アプリケーション ウィザードは 1 つのダイアログ クラスを作成します:

SelfEntractionConvertingToolDlg.h, SelfEntractionConvertingToolDlg.cpp - ダイアログ
   これらのファイルは CSelfEntractionConvertingToolDlg クラスを含みます。このクラスはアプ
   リケーションのメイン ダイアログの動作を定義します。ダイアログ テンプレートは
   Microsoft Visual C++ で編集可能な SelfEntractionConvertingTool.rc に含まれます。


/////////////////////////////////////////////////////////////////////////////

その他の機能:

ActiveX コントロール
   アプリケーションは ActiveX コントロールの使用に関するサポートを含みます。

/////////////////////////////////////////////////////////////////////////////

その他の標準ファイル:

StdAfx.h, StdAfx.cpp
   これらのファイルは、既にコンパイルされたヘッダー ファイル (PCH) 
   SelfEntractionConvertingTool.pch や既にコンパイルされた型のファイル StdAfx.obj を
   ビルドするために使用されます。

Resource.h
   これは新規リソース ID を定義する標準ヘッダー ファイルです。Microsoft 
   Visual C++ はこのファイルの読み取りと更新を行います。

SelfEntractionConvertingTool.manifest
   アプリケーション マニフェスト ファイルは Windows XP で、Side-by-Side アセン
   ブリの特定のバージョンに関するアプリケーションの依存関係を説明するために使用
   されます。ローダーはこの情報を使用して、アセンブリ キャッシュから適切なアセ
   ンブリを、またはアプリケーションからプライベート アセンブリを読み込みます。
   アプリケーション マニフェストは再頒布用に、実行可能アプリケーションと同じフ
   ォルダにインストールされる外部 .manifest ファイルとして含まれているか、また
   はリソースのフォーム内の実行可能ファイルに含まれています。
/////////////////////////////////////////////////////////////////////////////

その他の注意:

アプリケーション ウィザードは "TODO:" で始まるコメントを使用して、追加したりカ
スタマイズする必要があるソース コードの部分を示します。

アプリケーションが共有 DLL 内で MFC を使用する場合は、MFC DLL を再頒布する必要
があります。また、アプリケーションがオペレーティング システムのロケール以外の言
語を使用している場合も、対応するローカライズされたリソース MFC100XXX.DLL を再頒
布する必要があります。これらのトピックの詳細については、MSDN ドキュメントの 
Visual C++ アプリケーションの再頒布に関するセクションを参照してください。

/////////////////////////////////////////////////////////////////////////////
