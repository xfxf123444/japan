// DelFileDlg.h : header file
//

#if !defined(AFX_DELFILEDLG_H__D1BC09ED_244B_4B43_8D67_2615441C2747__INCLUDED_)
#define AFX_DELFILEDLG_H__D1BC09ED_244B_4B43_8D67_2615441C2747__INCLUDED_

#include "StdAfx.h"

/////////////////////////////////////////////////////////////////////////////
// CDelFileDlg dialog

class CDelFileDlg : public CDialog
{
// Construction
public:
	CDelFileDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDelFileDlg)
	enum { IDD = IDD_DELFILE_DIALOG };
	CString	m_csSelInfo;
	CString m_csPathName;
	//}}AFX_DATA

	BOOL IsInCludePath(WCHAR  *pszDir);
	BOOL IsParentPath(WCHAR  *pszDir);
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDelFileDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDelFileDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBackupDelte();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DELFILEDLG_H__D1BC09ED_244B_4B43_8D67_2615441C2747__INCLUDED_)
