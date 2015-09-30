#if !defined(AFX_DRAGLISTCTRL_H__56ED7209_ADA9_4AF0_B4CC_E00ABC89ADBE__INCLUDED_)
#define AFX_DRAGLISTCTRL_H__56ED7209_ADA9_4AF0_B4CC_E00ABC89ADBE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DragListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDragListCtrl window

class CDragListCtrl : public CListCtrl
{
// Construction
public:
	CDragListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDragListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDragListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDragListCtrl)
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRAGLISTCTRL_H__56ED7209_ADA9_4AF0_B4CC_E00ABC89ADBE__INCLUDED_)
