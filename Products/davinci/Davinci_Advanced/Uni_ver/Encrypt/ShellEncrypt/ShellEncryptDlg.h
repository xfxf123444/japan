#if !defined(AFX_SHELLENCRYPTDLG_H__AC35F184_342C_43DD_8E13_8371890476B5__INCLUDED_)
#define AFX_SHELLENCRYPTDLG_H__AC35F184_342C_43DD_8E13_8371890476B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShellEncryptDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShellEncryptDlg dialog

class CShellEncryptDlg : public CDialog
{
// Construction
public:
	void OnOK();
	CString m_strSource;
	CShellEncryptDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CShellEncryptDlg)
	enum { IDD = IDD_SHELL_ENCRYPT };
	CString	m_strPassword;
	CString	m_strConfirmPassword;
	BOOL	m_bDeleteSource;
	int		m_CompressLevel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShellEncryptDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CShellEncryptDlg)
	afx_msg void OnEncrypt();
	afx_msg void OnButtonCancel();
	afx_msg void OnOption();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHELLENCRYPTDLG_H__AC35F184_342C_43DD_8E13_8371890476B5__INCLUDED_)
