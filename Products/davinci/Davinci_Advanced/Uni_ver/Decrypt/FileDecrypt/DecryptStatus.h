#if !defined(AFX_DECRYPTSTATUS_H__93B1EF1E_5740_42D4_B3BE_129DB98EAE86__INCLUDED_)
#define AFX_DECRYPTSTATUS_H__93B1EF1E_5740_42D4_B3BE_129DB98EAE86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DecryptStatus.h : header file
//

#include "..\..\EncryptToolStruct\EncryptToolStruct.h"

typedef struct
{
	HWND hParentWnd;
	DECRYPT_INFO DecryptInfo;
	BOOL bDecryptSelectedFolderOnly;
}THREAD_PARAM;

/////////////////////////////////////////////////////////////////////////////
// CDecryptStatus dialog

class CDecryptStatus : public CDialog
{
// Construction
public:
	void OnOK();
	CDecryptStatus(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDecryptStatus)
	enum { IDD = IDD_DECRYPT_STATUS };
	CProgressCtrl	m_Progress;
	CAnimateCtrl	m_AnimateCtrl;
	CString	m_strCurrentFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDecryptStatus)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDecryptStatus)
	afx_msg void OnButtonCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	THREAD_PARAM m_ThreadParam;
	HANDLE m_hThreadHandle;
	int    m_nTimer;
public:
	DECRYPT_INFO m_DecryptInfo;
	BOOL m_bDecryptSelectedFolderOnly;
	BOOL         m_bConcisePrompt; // if decrypt to temp dir, show concise prompt

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DECRYPTSTATUS_H__93B1EF1E_5740_42D4_B3BE_129DB98EAE86__INCLUDED_)
