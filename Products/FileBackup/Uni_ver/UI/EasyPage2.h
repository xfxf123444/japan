#if !defined(AFX_EASYPAGE2_H__8CADD47C_3269_4C31_B0AB_F01542913DC3__INCLUDED_)
#define AFX_EASYPAGE2_H__8CADD47C_3269_4C31_B0AB_F01542913DC3__INCLUDED_

#include "stdafx.h"

/////////////////////////////////////////////////////////////////////////////
// CEasyPage2 dialog

class CEasyPage2 : public CPropertyPage
{
	DECLARE_DYNCREATE(CEasyPage2)

// Construction
public:
	CEasyPage2();
	~CEasyPage2();

// Dialog Data
	//{{AFX_DATA(CEasyPage2)
	enum { IDD = IDD_EASY_WIZARD_2 };
	int		m_nMailType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CEasyPage2)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CEasyPage2)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EASYPAGE2_H__8CADD47C_3269_4C31_B0AB_F01542913DC3__INCLUDED_)
