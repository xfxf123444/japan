#if !defined(AFX_ENCRYPTOPTION_H__0C8F3EEF_C12C_41C2_A7AF_54B5A479FFB6__INCLUDED_)
#define AFX_ENCRYPTOPTION_H__0C8F3EEF_C12C_41C2_A7AF_54B5A479FFB6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EncryptOption.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEncryptOption dialog

class CEncryptOption : public CDialog
{
// Construction
public:
	void InitCtrlData();
	CEncryptOption(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEncryptOption)
	enum { IDD = IDD_ENCRYPT_OPTION };
	CEdit	m_LimitCount;
	CSpinButtonCtrl	m_SpinCount;
	BOOL	m_bCheckCount;
	BOOL	m_bCheckTime;
	CTime	m_LimitTime;
	DWORD	m_dwLimitCount;
	BOOL	m_bOptionOn;
	BOOL	m_bErrorLimition;
	int		m_nMaxInputNumber;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEncryptOption)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEncryptOption)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckOption();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENCRYPTOPTION_H__0C8F3EEF_C12C_41C2_A7AF_54B5A479FFB6__INCLUDED_)
