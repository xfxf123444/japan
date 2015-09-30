#if !defined(AFX_ENCRYPTSTATUS_H__22FBCA4B_E37E_4D61_9FBC_6DEDCAA9773D__INCLUDED_)
#define AFX_ENCRYPTSTATUS_H__22FBCA4B_E37E_4D61_9FBC_6DEDCAA9773D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EncryptStatus.h : header file
//

#include "..\EncryptFunction\EncryptInfo.h"

typedef struct
{
	HWND hParentWnd;
	CEncryptInfo  EncryptInfo; 
}THREAD_PARAM;

/////////////////////////////////////////////////////////////////////////////
// CEncryptStatus dialog

class CEncryptStatus : public CDialog
{
// Construction
public:
	void OnOK();
	int m_nTimer;
	CEncryptStatus(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEncryptStatus)
	enum { IDD = IDD_ENCRYPT_STATUS };
	CAnimateCtrl	m_AnimateCtrl;
	CProgressCtrl	m_Progress;
	CString	m_strCurrentFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEncryptStatus)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEncryptStatus)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonCancel();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	THREAD_PARAM m_ThreadParam;
	HANDLE m_hThreadHandle;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENCRYPTSTATUS_H__22FBCA4B_E37E_4D61_9FBC_6DEDCAA9773D__INCLUDED_)
