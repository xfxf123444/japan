#if !defined(AFX_SPLASH_H__0BA7F1E2_F352_11D3_AD37_52544CBE0147__INCLUDED_)
#define AFX_SPLASH_H__0BA7F1E2_F352_11D3_AD37_52544CBE0147__INCLUDED_

#include "stdafx.h"

/////////////////////////////////////////////////////////////////////////////
// CSplash window

class CSplash : public CWnd
{
// Construction
public:
	CSplash();

// Attributes
public:
	CBitmap     *m_pSplash;
	CRect		m_Rect;
	UINT	    m_Timer;
	BOOL	    m_bClickButton;
	BOOL		m_bValidTimer;
	DWORD		m_dwTickCount;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplash)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSplash();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSplash)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPLASH_H__0BA7F1E2_F352_11D3_AD37_52544CBE0147__INCLUDED_)
