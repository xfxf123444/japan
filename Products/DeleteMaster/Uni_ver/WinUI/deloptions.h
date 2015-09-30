#if !defined(AFX_DELOPTIONS_H__0852E342_7F9D_4673_9F42_615C53EA7505__INCLUDED_)
#define AFX_DELOPTIONS_H__0852E342_7F9D_4673_9F42_615C53EA7505__INCLUDED_

#include "StdAfx.h"

/////////////////////////////////////////////////////////////////////////////
// CDelOptions dialog

class CDelOptions : public CDialog
{
// Construction
public:
	CDelOptions(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDelOptions)
	enum { IDD = IDD_DELETE_OPTIONS };
	BOOL	m_bVerify;
	BOOL	m_bFormat;
	DWORD	m_MaxTry;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDelOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDelOptions)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DELOPTIONS_H__0852E342_7F9D_4673_9F42_615C53EA7505__INCLUDED_)
