#if !defined(AFX_DECRYPTWIZ_3_H__4E5A0961_8C80_4046_BC8A_B3D5164761F1__INCLUDED_)
#define AFX_DECRYPTWIZ_3_H__4E5A0961_8C80_4046_BC8A_B3D5164761F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DecryptWiz_3.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDecryptWiz_3 dialog

class CDecryptWiz_3 : public CPropertyPage
{
	DECLARE_DYNCREATE(CDecryptWiz_3)

// Construction
public:
	CDecryptWiz_3();
	~CDecryptWiz_3();

// Dialog Data
	//{{AFX_DATA(CDecryptWiz_3)
	enum { IDD = IDD_DECRYPT_WIZ_3 };
	int		m_nRestoreType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDecryptWiz_3)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDecryptWiz_3)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DECRYPTWIZ_3_H__4E5A0961_8C80_4046_BC8A_B3D5164761F1__INCLUDED_)
