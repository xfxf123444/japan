
// TestDeleteLibraryDlg.h : header file
//

#pragma once
#include "afxeditbrowsectrl.h"

class DeleteLibrary;

// CTestDeleteLibraryDlg dialog
class CTestDeleteLibraryDlg : public CDialogEx
{
// Construction
public:
	CTestDeleteLibraryDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TESTDELETELIBRARY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonCheckExcludedPath();
	afx_msg void OnBnClickedButtonSecureDeleteRecycleBin();
	const TCHAR* GetModulePath();

	enum DeleteType
	{
		DT_File,
		DT_Folder
	};
	DeleteType m_type;
	bool m_isExecuting;
	bool m_deleteRecycledBin;
	CString m_deletePath;
	CString m_logPath;
	DeleteLibrary* m_pDeleteLibrary;
	afx_msg void OnBnClickedButtonSecureDelete();
	afx_msg void OnBnClickedRadioFolder();
	afx_msg void OnBnClickedRadioFile();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CMFCEditBrowseCtrl m_editBrowseFolder;
	CMFCEditBrowseCtrl m_editBrowseFile;
};
