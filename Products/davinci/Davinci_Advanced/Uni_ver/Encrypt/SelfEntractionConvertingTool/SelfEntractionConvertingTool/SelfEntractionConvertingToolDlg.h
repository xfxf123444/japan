
// SelfEntractionConvertingToolDlg.h : �إå��` �ե�����
//

#pragma once
#include "afxwin.h"


// CSelfEntractionConvertingToolDlg ��������
class CSelfEntractionConvertingToolDlg : public CDialogEx
{
// ���󥹥ȥ饯�����
public:
	CSelfEntractionConvertingToolDlg(CWnd* pParent = NULL);	// �˜ʥ��󥹥ȥ饯���`

// �������� �ǩ`��
	enum { IDD = IDD_SELFENTRACTIONCONVERTINGTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ���ݩ`��


// �gװ
protected:
	HICON m_hIcon;

	// ���ɤ��줿����å��`����굱���v��
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
