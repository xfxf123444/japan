#if !defined(AFX_INPUTPASSWORDDLG_H__0F69DF6F_40BF_456C_B6DE_222F13809478__INCLUDED_)
#define AFX_INPUTPASSWORDDLG_H__0F69DF6F_40BF_456C_B6DE_222F13809478__INCLUDED_

#include "..\AM01Struct\AM01Struct.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InputPasswordDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInputPasswordDlg dialog

class CInputPasswordDlg : public CDialog
{
// Construction
public:
	void SetPasswordValue(WCHAR *pchPassword);
	BOOL TestPasswordResult();
	CInputPasswordDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInputPasswordDlg)
	enum { IDD = IDD_INPUT_PASSWORD };
	CString	m_strPassword;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputPasswordDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInputPasswordDlg)
	afx_msg void OnButtonOk();
	afx_msg void OnButtonCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	WCHAR m_pchPassword[PASSWORD_SIZE];
	BOOL m_bPasswordCorrect;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPUTPASSWORDDLG_H__0F69DF6F_40BF_456C_B6DE_222F13809478__INCLUDED_)
