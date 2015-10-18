
// SelfEntractionConvertingTool.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CSelfEntractionConvertingToolApp:
// このクラスの実装については、SelfEntractionConvertingTool.cpp を参照してください。
//

class CSelfEntractionConvertingToolApp : public CWinApp
{
public:
	CSelfEntractionConvertingToolApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CSelfEntractionConvertingToolApp theApp;