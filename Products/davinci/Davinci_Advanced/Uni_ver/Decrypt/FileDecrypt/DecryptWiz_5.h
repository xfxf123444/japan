#if !defined(AFX_DECRYPTWIZ_5_H__E3900EC6_57D4_4E22_9355_3514B544A144__INCLUDED_)
#define AFX_DECRYPTWIZ_5_H__E3900EC6_57D4_4E22_9355_3514B544A144__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DecryptWiz_5.h : header file
//

#include "NewRestoreTreeCtrl.h"


/////////////////////////////////////////////////////////////////////////////
// CDecryptWiz_5 dialog

class CDecryptWiz_5 : public CPropertyPage
{
	DECLARE_DYNCREATE(CDecryptWiz_5)

// Construction
public:
	CString GetItemPath();
	BOOL GetUIInfo();
	void InitDialogItem();
	CDecryptWiz_5();
	~CDecryptWiz_5();

	CString m_strTextCancel;
	CImageList m_ImageList;

// Dialog Data
	//{{AFX_DATA(CDecryptWiz_5)
	enum { IDD = IDD_DECRYPT_WIZ_5 };
	CNewRestoreTreeCtrl	m_DirTree;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDecryptWiz_5)
	public:
	virtual BOOL OnWizardFinish();
	virtual LRESULT OnWizardBack();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDecryptWiz_5)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DECRYPTWIZ_5_H__E3900EC6_57D4_4E22_9355_3514B544A144__INCLUDED_)
