#if !defined(AFX_SAVESETTING_H__DDB7107D_20AA_40FB_B423_20F4A8C0C457__INCLUDED_)
#define AFX_SAVESETTING_H__DDB7107D_20AA_40FB_B423_20F4A8C0C457__INCLUDED_

#include "stdafx.h"

/////////////////////////////////////////////////////////////////////////////
// CSaveSetting dialog

class CSaveSetting : public CDialog
{
// Construction
public:
	CSaveSetting(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSaveSetting)
	enum { IDD = IDD_SAVE_SETTING };
	CString	m_Comment;
	CString	m_SettingName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSaveSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSaveSetting)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAVESETTING_H__DDB7107D_20AA_40FB_B423_20F4A8C0C457__INCLUDED_)
