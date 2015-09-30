#if !defined(AFX_SETSECNUM_H__884C8CD3_646D_4E55_9683_63041737B6C8__INCLUDED_)
#define AFX_SETSECNUM_H__884C8CD3_646D_4E55_9683_63041737B6C8__INCLUDED_

#include "StdAfx.h"

/////////////////////////////////////////////////////////////////////////////
// CSetSecNum dialog

class CSetSecNum : public CDialog
{
// Construction
public:
	CSetSecNum(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetSecNum)
	enum { IDD = IDD_SET_SERCTOR_NUMBER };
	CString	m_szSecNum;
	CString	m_szRange;
	//}}AFX_DATA
	int		m_nSecMin;
	int		m_nSecMax;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetSecNum)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetSecNum)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETSECNUM_H__884C8CD3_646D_4E55_9683_63041737B6C8__INCLUDED_)
