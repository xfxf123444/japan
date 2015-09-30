#if !defined(AFX_PROPSHT_H__DD103394_A81C_4669_9A84_5A1A729C70FF__INCLUDED_)
#define AFX_PROPSHT_H__DD103394_A81C_4669_9A84_5A1A729C70FF__INCLUDED_

#include "StdAfx.h"

/////////////////////////////////////////////////////////////////////////////
// CPropSht

class CPropSht : public CPropertySheet
{
	DECLARE_DYNAMIC(CPropSht)

// Construction
public:
	CPropSht(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CPropSht(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:
	CImageList		m_imageTab;
	CTabCtrl*		m_pTab;
	CPropertyPage*	m_pPage;

// Operations
public:
	void ResizeAll(int cx,int cy);
	void ActivePage(WORD wParam);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropSht)
	public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPropSht();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropSht)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPSHT_H__DD103394_A81C_4669_9A84_5A1A729C70FF__INCLUDED_)
