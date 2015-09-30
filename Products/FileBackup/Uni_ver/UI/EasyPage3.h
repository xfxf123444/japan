#if !defined(AFX_EASYPAGE3_H__2A71DF17_C53F_4944_96E1_ADE39B102D9C__INCLUDED_)
#define AFX_EASYPAGE3_H__2A71DF17_C53F_4944_96E1_ADE39B102D9C__INCLUDED_

#include "stdafx.h"

/////////////////////////////////////////////////////////////////////////////
// CEasyPage3 dialog

class CEasyPage3 : public CPropertyPage
{
	DECLARE_DYNCREATE(CEasyPage3)

// Construction
public:
	BOOL m_bExecuting;
	BOOL GetMailSourceArray(CStringArray &MailSourceArray, int nMailType);
	CEasyPage3();
	~CEasyPage3();

// Dialog Data
	//{{AFX_DATA(CEasyPage3)
	enum { IDD = IDD_EASY_WIZARD_3 };
	CString	m_strTargetFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CEasyPage3)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CEasyPage3)
	afx_msg void OnButtonRefer();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL GetEasySourceArray(CStringArray &EasySourceArray);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EASYPAGE3_H__2A71DF17_C53F_4944_96E1_ADE39B102D9C__INCLUDED_)
