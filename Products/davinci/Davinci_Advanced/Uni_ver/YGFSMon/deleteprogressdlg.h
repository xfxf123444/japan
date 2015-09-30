#if !defined(AFX_DELETEPROGRESSDLG_H__3F61921B_D89F_4598_B564_F183C4FB6D7F__INCLUDED_)
#define AFX_DELETEPROGRESSDLG_H__3F61921B_D89F_4598_B564_F183C4FB6D7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// deleteprogressdlg.h : header file
//
#include "resource.h"
#include "YGFSMonConst.h"
/////////////////////////////////////////////////////////////////////////////
// CDeleteProgressDlg dialog

class CDeleteProgressDlg : public CDialog
{
// Construction
public:
	void SetDeleteParameter(CString strDeleteDir);
	UINT NormalDeleteDirThread();
	void CancelDelete();
	CDeleteProgressDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDeleteProgressDlg)
	enum { IDD = IDD_DELETE_PROCESS };
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
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bCancel;
	CString m_strCurrentDeleteFile;
	DELETE_THREAD_PARAMETER1 m_DeleteThreadParameter;
	CString m_strDeleteDir;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DELETEPROGRESSDLG_H__3F61921B_D89F_4598_B564_F183C4FB6D7F__INCLUDED_)
