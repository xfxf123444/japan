#if !defined(AFX_SETPASSWORDDLG_H__DCA00E89_6C26_493F_B521_1DB22514F495__INCLUDED_)
#define AFX_SETPASSWORDDLG_H__DCA00E89_6C26_493F_B521_1DB22514F495__INCLUDED_

#include "stdafx.h"

/////////////////////////////////////////////////////////////////////////////
// CSetPasswordDlg dialog

class CSetPasswordDlg : public CDialog
{
// Construction
public:
	CSetPasswordDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetPasswordDlg)
	enum { IDD = IDD_SET_PASSWORD };
	CString	m_strPassword;
	CString	m_strConfirmPassword;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetPasswordDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetPasswordDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
private:
	void ClearPassword();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETPASSWORDDLG_H__DCA00E89_6C26_493F_B521_1DB22514F495__INCLUDED_)
