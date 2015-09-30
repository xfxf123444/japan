#if !defined(AFX_SCHEDULEPAGE1_H__DA46D6AE_CBBC_4246_88ED_22973589C448__INCLUDED_)
#define AFX_SCHEDULEPAGE1_H__DA46D6AE_CBBC_4246_88ED_22973589C448__INCLUDED_

#include "stdafx.h"

/////////////////////////////////////////////////////////////////////////////
// CSchedulePage1 dialog

class CSchedulePage1 : public CPropertyPage
{
	DECLARE_DYNCREATE(CSchedulePage1)

// Construction
public:
	CSchedulePage1();
	~CSchedulePage1();

// Dialog Data
	//{{AFX_DATA(CSchedulePage1)
	enum { IDD = IDD_SCHEDULE_PAGE_1 };
	int		m_nBackupPeriod;
	int		m_nExecuteType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSchedulePage1)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSchedulePage1)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCHEDULEPAGE1_H__DA46D6AE_CBBC_4246_88ED_22973589C448__INCLUDED_)
