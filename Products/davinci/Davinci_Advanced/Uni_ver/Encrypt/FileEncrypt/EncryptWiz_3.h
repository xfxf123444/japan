#if !defined(AFX_ENCRYPTWIZ_3_H__B690AE0E_3D71_4C92_A5B7_AFD3F6A08EA9__INCLUDED_)
#define AFX_ENCRYPTWIZ_3_H__B690AE0E_3D71_4C92_A5B7_AFD3F6A08EA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EncryptWiz_3.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEncryptWiz_3 dialog

class CEncryptWiz_3 : public CPropertyPage
{
	DECLARE_DYNCREATE(CEncryptWiz_3)

// Construction
public:
	CString m_strTextCancel;
	CEncryptWiz_3();
	~CEncryptWiz_3();

// Dialog Data
	//{{AFX_DATA(CEncryptWiz_3)
	enum { IDD = IDD_ENCRYPT_WIZ_3 };
	CString	m_strPassword;
	CString	m_strConfirmPassword;
	BOOL	m_bDeleteSource;
	int		m_CompressLevel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CEncryptWiz_3)
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
	//{{AFX_MSG(CEncryptWiz_3)
	afx_msg void OnOption();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENCRYPTWIZ_3_H__B690AE0E_3D71_4C92_A5B7_AFD3F6A08EA9__INCLUDED_)
