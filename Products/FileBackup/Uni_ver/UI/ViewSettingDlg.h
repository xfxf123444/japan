#if !defined(AFX_VIEWSETTINGDLG_H__286EB737_ABEB_4B98_9A17_B86613A97509__INCLUDED_)
#define AFX_VIEWSETTINGDLG_H__286EB737_ABEB_4B98_9A17_B86613A97509__INCLUDED_

#include "stdafx.h"

/////////////////////////////////////////////////////////////////////////////
// CViewSettingDlg dialog

class CViewSettingDlg : public CDialog
{
// Construction
public:
	BOOL InitDialogControl();
	CViewSettingDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CViewSettingDlg)
	enum { IDD = IDD_VIEW_SETTING };
	CListCtrl	m_SourceList;
	CButton	m_btChangeName;
	CString	m_strTarget;
	CString	m_strSettingName;
	CString	m_strEncrypt;
	BOOL	m_bLogFail;
	DWORD	m_dwMinutes;
	BOOL	m_bRetry;
	BOOL	m_bAutoSpawn;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewSettingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CViewSettingDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonExit();
	afx_msg void OnButtonChangeName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWSETTINGDLG_H__286EB737_ABEB_4B98_9A17_B86613A97509__INCLUDED_)
