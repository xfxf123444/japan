#if !defined(AFX_PASSWORDDLG_H__503A9C4D_1A44_442D_8EB7_1DE67C58BB88__INCLUDED_)
#define AFX_PASSWORDDLG_H__503A9C4D_1A44_442D_8EB7_1DE67C58BB88__INCLUDED_

#include "StdAfx.h"

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg dialog

class CPasswordDlg : public CDialog
{
// Construction
public:
	BOOL m_bSetPassword;
	CPasswordDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPasswordDlg)
	enum { IDD = IDD_DIALOG_LOGIN };
	CString	m_Password;
	CString	m_PasswordConfirm;
	CString	m_OldPassword;
	CString	m_EnterPass;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPasswordDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPasswordDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PASSWORDDLG_H__503A9C4D_1A44_442D_8EB7_1DE67C58BB88__INCLUDED_)
