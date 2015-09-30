#if !defined(AFX_AMBACKUP_H__322B4965_3F44_11D3_A638_5254AB10413A__INCLUDED_)
#define AFX_AMBACKUP_H__322B4965_3F44_11D3_A638_5254AB10413A__INCLUDED_

#include "stdafx.h"

/////////////////////////////////////////////////////////////////////////////
// CAMBackup dialog

class CAMBackup : public CPropertyPage
{
	DECLARE_DYNCREATE(CAMBackup)
// Construction
public:
	BOOL m_bExecuting;
	CString m_strPassword;
	BOOL CheckValidSetting();
	CImageList * m_pImageList;
	CAMBackup();   // standard constructor
    ~CAMBackup();
// Dialog Data
	//{{AFX_DATA(CAMBackup)
	enum { IDD = IDD_AMBACKUP };
	CListCtrl	m_SourceList;
	CString	m_SourcePath;
	CString	m_TargetPath;
	CString	m_Comment;
	BOOL	m_bLogFail;
	DWORD	m_dwMinutes;
	BOOL	m_bRetry;
	BOOL	m_bAutoSpawn;
	//}}AFX_DATA

	BOOL OnInitDialog();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAMBackup)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAMBackup)
	afx_msg void OnSaveSetting();
	afx_msg void OnReferTarget();
	afx_msg void OnReferSource();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnStart();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnRetry();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AMBACKUP_H__322B4965_3F44_11D3_A638_5254AB10413A__INCLUDED_)
