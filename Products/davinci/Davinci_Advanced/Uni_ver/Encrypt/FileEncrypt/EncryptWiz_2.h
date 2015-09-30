#if !defined(AFX_ENCRYPTWIZ_2_H__8455CA24_8796_4502_81BD_4E73E038B881__INCLUDED_)
#define AFX_ENCRYPTWIZ_2_H__8455CA24_8796_4502_81BD_4E73E038B881__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EncryptWiz_2.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEncryptWiz_2 dialog

class CEncryptWiz_2 : public CPropertyPage
{
	DECLARE_DYNCREATE(CEncryptWiz_2)

// Construction
public:
	CEncryptWiz_2();
	~CEncryptWiz_2();

// Dialog Data
	//{{AFX_DATA(CEncryptWiz_2)
	enum { IDD = IDD_ENCRYPT_WIZ_2 };
	CString	m_strTarget;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CEncryptWiz_2)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CEncryptWiz_2)
	afx_msg void OnRefer();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENCRYPTWIZ_2_H__8455CA24_8796_4502_81BD_4E73E038B881__INCLUDED_)
