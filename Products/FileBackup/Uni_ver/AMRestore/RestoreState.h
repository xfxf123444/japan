#if !defined(AFX_RESTORESTATE_H__65035B4A_32F7_4272_B47F_9D05078FD032__INCLUDED_)
#define AFX_RESTORESTATE_H__65035B4A_32F7_4272_B47F_9D05078FD032__INCLUDED_

#include "stdafx.h"
typedef struct{
	HWND hParentWnd;
	DWORD dwReserved[10];
}THREAD_PARAM,*PTHREAD_PARAM;

DWORD WINAPI ThreadFun(LPVOID pIn);

/////////////////////////////////////////////////////////////////////////////
// CRestoreState dialog

class CRestoreState : public CDialog
{
// Construction
public:
	CRestoreState(CWnd* pParent = NULL);   // standard constructor
	int m_nTimer;
	HANDLE m_hThreadHandle;
	THREAD_PARAM m_ThreadParam;
// Dialog Data
	//{{AFX_DATA(CRestoreState)
	enum { IDD = IDD_RESTORE_STATE };
	CProgressCtrl	m_Progress;
	CAnimateCtrl	m_AnimateCtrl;
	CString	m_CurrentFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRestoreState)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRestoreState)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESTORESTATE_H__65035B4A_32F7_4272_B47F_9D05078FD032__INCLUDED_)
