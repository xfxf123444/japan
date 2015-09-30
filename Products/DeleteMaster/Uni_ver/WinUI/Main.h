#if !defined(AFX_MAIN_H__A46A9322_5C5C_46F6_B738_B41F2F37C7FE__INCLUDED_)
#define AFX_MAIN_H__A46A9322_5C5C_46F6_B738_B41F2F37C7FE__INCLUDED_

#include "StdAfx.h"
#include "PropSht.h"
#include "DelPar.h"
#include "DiskView.h"
#include "WipeFree.h"

/////////////////////////////////////////////////////////////////////////////
// CMain window

class CMain : public CWnd
{
// Construction
public:
	CMain();

// Attributes
public:
	CRect			rWindow,rExit,rExe,m_rcGripRect,m_rcPageClient;
	CPropSht*		m_pPropSht;
	CDelPar*		m_pDelete;
	CDiskView*		m_pDiskView;
	CWipeFree*		m_pWipeFree;
	POINT			m_ptMinTrackSize;
// Operations
public:
	void	CloseAllPages();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMain)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMain();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMain)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAIN_H__A46A9322_5C5C_46F6_B738_B41F2F37C7FE__INCLUDED_)
