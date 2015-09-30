#if !defined(AFX_SCHEDULEPAGE3_H__5032CDC3_E9C8_46F8_9246_EF887252DDDA__INCLUDED_)
#define AFX_SCHEDULEPAGE3_H__5032CDC3_E9C8_46F8_9246_EF887252DDDA__INCLUDED_

#include "stdafx.h"

/////////////////////////////////////////////////////////////////////////////
// CSchedulePage3 dialog

class CSchedulePage3 : public CPropertyPage
{
	DECLARE_DYNCREATE(CSchedulePage3)

// Construction
public:
	CSchedulePage3();
	~CSchedulePage3();

// Dialog Data
	//{{AFX_DATA(CSchedulePage3)
	enum { IDD = IDD_SCHEDULE_PAGE_3 };
	CDateTimeCtrl	m_BackupTime;
	BOOL	m_bShutdown;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSchedulePage3)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSchedulePage3)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCHEDULEPAGE3_H__5032CDC3_E9C8_46F8_9246_EF887252DDDA__INCLUDED_)
