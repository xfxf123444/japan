#if !defined(AFX_SECINFO_H__CB56D70C_2B99_43E4_820A_99DA809135D3__INCLUDED_)
#define AFX_SECINFO_H__CB56D70C_2B99_43E4_820A_99DA809135D3__INCLUDED_

#include "StdAfx.h"

/////////////////////////////////////////////////////////////////////////////
// CSecInfo window

class CSecInfo : public CWnd
{
// Construction
public:
	CSecInfo(CWnd	*pSecViewWnd,CRect rect);

// Attributes
public:
	int		m_nCurLine;
	int		m_nPage;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSecInfo)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSecInfo();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSecInfo)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SECINFO_H__CB56D70C_2B99_43E4_820A_99DA809135D3__INCLUDED_)
