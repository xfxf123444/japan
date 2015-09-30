#if !defined(AFX_SCHEDULEPAGE2_H__81D51B5A_492A_42A2_B1CF_7459165CA895__INCLUDED_)
#define AFX_SCHEDULEPAGE2_H__81D51B5A_492A_42A2_B1CF_7459165CA895__INCLUDED_

#include "stdafx.h"

/////////////////////////////////////////////////////////////////////////////
// CSchedulePage2 dialog

class CSchedulePage2 : public CPropertyPage
{
	DECLARE_DYNCREATE(CSchedulePage2)

// Construction
public:
	CSchedulePage2();
	~CSchedulePage2();

// Dialog Data
	//{{AFX_DATA(CSchedulePage2)
	enum { IDD = IDD_SCHEDULE_PAGE_2 };
	CComboBox	m_DateCtrl;
	CString	m_strDateTitle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSchedulePage2)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSchedulePage2)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCHEDULEPAGE2_H__81D51B5A_492A_42A2_B1CF_7459165CA895__INCLUDED_)
