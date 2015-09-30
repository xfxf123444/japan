#if !defined(AFX_DECRYPTWIZ_2_H__1035B520_306E_4AF5_AD28_119192DBF0DE__INCLUDED_)
#define AFX_DECRYPTWIZ_2_H__1035B520_306E_4AF5_AD28_119192DBF0DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DecryptWiz_2.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDecryptWiz_2 dialog


class CDecryptWiz_2 : public CPropertyPage
{
	DECLARE_DYNCREATE(CDecryptWiz_2)

// Construction
public:
	CDecryptWiz_2();
	~CDecryptWiz_2();

// Dialog Data
	//{{AFX_DATA(CDecryptWiz_2)
	enum { IDD = IDD_DECRYPT_WIZ_2 };
	CString	m_strPassword;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDecryptWiz_2)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDecryptWiz_2)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DECRYPTWIZ_2_H__1035B520_306E_4AF5_AD28_119192DBF0DE__INCLUDED_)
