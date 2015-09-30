#if !defined(AFX_SHELLDECRYPTDLG_H__1337AA86_7EEA_498F_89D0_B445359AC859__INCLUDED_)
#define AFX_SHELLDECRYPTDLG_H__1337AA86_7EEA_498F_89D0_B445359AC859__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShellDecryptDlg.h : header file
//
#include "..\DecryptFunction\DecryptFunction.h"

/////////////////////////////////////////////////////////////////////////////
// CShellDecryptDlg dialog

class CShellDecryptDlg : public CDialog
{
// Construction
public:
	BOOL m_bOpen;
	CString m_strImageFile;
	void OnOK();
	CShellDecryptDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CShellDecryptDlg)
	enum { IDD = IDD_DIALOG_SHELL_DECRYPT };
	CString	m_strPassword;
	BOOL	m_bSecureDeleteSource;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShellDecryptDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CShellDecryptDlg)
	afx_msg void OnButtonDecrypt();
	afx_msg void OnButtonCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHELLDECRYPTDLG_H__1337AA86_7EEA_498F_89D0_B445359AC859__INCLUDED_)
