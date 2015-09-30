#if !defined(AFX_PROTECT_H__1E352681_B0B3_11D2_A305_52544CBDF83A__INCLUDED_)
#define AFX_PROTECT_H__1E352681_B0B3_11D2_A305_52544CBDF83A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Protect.h : header file
//
#define	ICONWIDTH	32

#include "resource.h"
 
/////////////////////////////////////////////////////////////////////////////
// CProtect dialog

class CProtect : public CPropertyPage
{
	DECLARE_DYNCREATE(CProtect)

// Construction
public:
	// CStringArray m_ProtectedPathArray;
	CProtect();
	~CProtect();

	//
	CImageList  m_ImageList;
// Dialog Data
	//{{AFX_DATA(CProtect)
	enum { IDD = IDD_YGFSMON_SETING };
	CListCtrl	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CProtect)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CProtect)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROTECT_H__1E352681_B0B3_11D2_A305_52544CBDF83A__INCLUDED_)
