#if !defined(AFX_DECRYPTWIZ_4_H__51535A85_0344_4AE6_B764_05367C591277__INCLUDED_)
#define AFX_DECRYPTWIZ_4_H__51535A85_0344_4AE6_B764_05367C591277__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DecryptWiz_4.h : header file
//

#include "NewRestoreTreeCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CDecryptWiz_4 dialog

class CDecryptWiz_4 : public CPropertyPage
{
	DECLARE_DYNCREATE(CDecryptWiz_4)

// Construction
public:
	void InitControl();
	CDecryptWiz_4();
	~CDecryptWiz_4();

	CString m_strStorePath;

// Dialog Data
	//{{AFX_DATA(CDecryptWiz_4)
	enum { IDD = IDD_DECRYPT_WIZ_4 };
	CString	m_strTargetPath;
	int		m_nTargetType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDecryptWiz_4)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDecryptWiz_4)
	virtual BOOL OnInitDialog();
	afx_msg void OnOriginalPlace();
	afx_msg void OnRelativePlace();
	afx_msg void OnCurrentPlace();
	afx_msg void OnNewPlace();
	afx_msg void OnRefer();
//	afx_msg void OnRelativePlace();
//	afx_msg void OnCurrentPlace();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DECRYPTWIZ_4_H__51535A85_0344_4AE6_B764_05367C591277__INCLUDED_)
