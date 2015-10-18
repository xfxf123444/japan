
// SelfEntractionConvertingToolDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"


// CSelfEntractionConvertingToolDlg ダイアログ
class CSelfEntractionConvertingToolDlg : public CDialogEx
{
// コンストラクション
public:
	CSelfEntractionConvertingToolDlg(CWnd* pParent = NULL);	// 標準コンストラクター

// ダイアログ データ
	enum { IDD = IDD_SELFENTRACTIONCONVERTINGTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonBrowse();
	CEdit m_wndFilePath;
	CStatic m_labelStatus;
	CString m_strTarget;
	afx_msg void OnBnClickedButtonConvert();
	BOOL Convert(const CString& sourcePath);
};
