#if !defined(AFX_ADDPATH_H__500522F0_64AB_40B1_9ACA_F67317FC3A9C__INCLUDED_)
#define AFX_ADDPATH_H__500522F0_64AB_40B1_9ACA_F67317FC3A9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddPath.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddPath dialog

class CAddPath : public CDialog
{
// Construction
public:
	CAddPath(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddPath)
	enum { IDD = IDD_ADDPATH_DLG };
	CString	m_Path;
	CString	m_strPassword;
	CString	m_strConfirmPassword;
	int		m_CompressLevel;
	BOOL	m_bCheckPasswordAlways;
	BOOL	m_bErrorLimition;
	int		m_nMaxInputNumber;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddPath)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddPath)
	afx_msg void OnButtonOk();
	afx_msg void OnButtonRefer();
	virtual BOOL OnInitDialog();
	afx_msg void OnErrorLimition();
	afx_msg void OnCheckPasswordAlways();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDPATH_H__500522F0_64AB_40B1_9ACA_F67317FC3A9C__INCLUDED_)
