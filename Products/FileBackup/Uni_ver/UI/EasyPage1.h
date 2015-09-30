#if !defined(AFX_EASYPAGE1_H__BB97F1B5_D3AD_4012_BDE9_2BF93490466B__INCLUDED_)
#define AFX_EASYPAGE1_H__BB97F1B5_D3AD_4012_BDE9_2BF93490466B__INCLUDED_

#include "stdafx.h"

/////////////////////////////////////////////////////////////////////////////
// CEasyPage1 dialog

class CEasyPage1 : public CPropertyPage
{
	DECLARE_DYNCREATE(CEasyPage1)

// Construction
public:
	CEasyPage1();
	~CEasyPage1();

// Dialog Data
	//{{AFX_DATA(CEasyPage1)
	enum { IDD = IDD_EASY_WIZARD_1 };
	int		m_nBackupContent;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CEasyPage1)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CEasyPage1)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EASYPAGE1_H__BB97F1B5_D3AD_4012_BDE9_2BF93490466B__INCLUDED_)
