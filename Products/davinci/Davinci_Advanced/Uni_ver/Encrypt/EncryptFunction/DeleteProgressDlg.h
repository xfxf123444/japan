#if !defined(AFX_DELETEPROGRESSDLG_H__B88CF744_09D5_4E21_8572_929D9AB51348__INCLUDED_)
#define AFX_DELETEPROGRESSDLG_H__B88CF744_09D5_4E21_8572_929D9AB51348__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DeleteProgressDlg.h : header file
//

#include "..\..\EncryptToolStruct\EncryptToolStruct.h"

/////////////////////////////////////////////////////////////////////////////
// CDeleteProgressDlg dialog

class CDeleteProgressDlg : public CDialog
{
// Construction
public:
	void SetDeleteParameter(CString strDeleteDir, int nDeleteMethod);
	static UINT SecureDeleteDirThread(LPVOID pParam);
	CDeleteProgressDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL m_bCancel;

// Dialog Data
	//{{AFX_DATA(CDeleteProgressDlg)
	enum { IDD = IDD_DELETE_PROGRESS };
	CProgressCtrl	m_DeleteProgress;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeleteProgressDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDeleteProgressDlg)
	afx_msg void OnCancelDelete();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString m_strDeleteDir;
    DELETE_THREAD_PARAMETER m_DeleteThreadParameter;
	int  m_nDeleteMethod;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DELETEPROGRESSDLG_H__B88CF744_09D5_4E21_8572_929D9AB51348__INCLUDED_)
