#if !defined(AFX_CHANGEPASSWORDDIALOG_H__7582F6C3_8360_432E_BB87_AD2E6DB8E88F__INCLUDED_)
#define AFX_CHANGEPASSWORDDIALOG_H__7582F6C3_8360_432E_BB87_AD2E6DB8E88F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChangePasswordDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChangePasswordDialog dialog

extern BOOL g_bAbort;

class CChangePasswordDialog : public CDialog
{
// Construction
public:
	void UndoChangePassword(PBYTE pOriginalPassword,PBYTE pNewPassword,int originalPasswordLen,
		int newPasswordLen,PBYTE pBuf,int originalEncryptType,int newEncryptType,DWORD alreadyEncryptSectors);
	CChangePasswordDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CChangePasswordDialog)
	enum { IDD = IDD_CHANGE_PASSWORD_DIALOG };
	CComboBox	m_changeEncryptAlgorithm;
	CProgressCtrl	m_changePasswordProgress;
	CString	m_originalPassword;
	CString	m_newPassword;
	CString	m_confirmChangedPassword;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChangePasswordDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChangePasswordDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonStartChange();
	afx_msg void OnButtonAbortChangePassword();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGEPASSWORDDIALOG_H__7582F6C3_8360_432E_BB87_AD2E6DB8E88F__INCLUDED_)
