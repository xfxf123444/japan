#if !defined(AFX_SELMETHOD_H__4A3AF076_20DE_4012_9C2E_AAF039223431__INCLUDED_)
#define AFX_SELMETHOD_H__4A3AF076_20DE_4012_9C2E_AAF039223431__INCLUDED_

#include "StdAfx.h"

/////////////////////////////////////////////////////////////////////////////
// CSelMethod dialog

class CSelMethod : public CDialog
{
// Construction
public:
	CSelMethod(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelMethod)
	enum { IDD = IDD_SELECT_METHOD };
	CComboBoxEx	m_SelMethod;
	CString	m_csDelString;
	BOOL	m_bBeepOff;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelMethod)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelMethod)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELMETHOD_H__4A3AF076_20DE_4012_9C2E_AAF039223431__INCLUDED_)
