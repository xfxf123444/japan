#if !defined(AFX_DECRYPTWIZ_1_H__E3F0E10A_CBB1_4451_AABA_E94F54A87460__INCLUDED_)
#define AFX_DECRYPTWIZ_1_H__E3F0E10A_CBB1_4451_AABA_E94F54A87460__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DecryptWiz_1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDecryptWiz_1 dialog

class CDecryptWiz_1 : public CPropertyPage
{
	DECLARE_DYNCREATE(CDecryptWiz_1)

// Construction
public:
	CDecryptWiz_1();
	~CDecryptWiz_1();

// Dialog Data
	//{{AFX_DATA(CDecryptWiz_1)
	enum { IDD = IDD_DECRYPT_WIZ_1 };
	CString	m_strImageFile;
	BOOL	m_bSecureDeleteSource;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDecryptWiz_1)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDecryptWiz_1)
	afx_msg void OnRefer();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DECRYPTWIZ_1_H__E3F0E10A_CBB1_4451_AABA_E94F54A87460__INCLUDED_)
