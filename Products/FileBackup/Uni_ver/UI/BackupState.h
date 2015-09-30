#if !defined(AFX_BACKUPSTATE_H__65035B4A_32F7_4272_B47F_9D05078FD032__INCLUDED_)
#define AFX_BACKUPSTATE_H__65035B4A_32F7_4272_B47F_9D05078FD032__INCLUDED_

#include "stdafx.h"

typedef struct{
	HWND hParentWnd;
	AM01_PROG_SETTING *pSetting;
}THREAD_PARAM,*PTHREAD_PARAM;


/////////////////////////////////////////////////////////////////////////////
// CBackupState dialog

class CBackupState : public CDialog
{
// Construction
public:
	void OnOK();
	CBackupState(CWnd* pParent = NULL);   // standard constructor
	int m_nTimer;
	HANDLE m_hThreadHandle;
	THREAD_PARAM m_ThreadParam;
	AM01_PROG_SETTING m_Setting;
// Dialog Data
	//{{AFX_DATA(CBackupState)
	enum { IDD = IDD_BACKUP_STATE };
	CProgressCtrl	m_Progress;
	CAnimateCtrl	m_AnimateCtrl;
	CString	m_CurrentFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBackupState)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBackupState)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtonCancel();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BACKUPSTATE_H__65035B4A_32F7_4272_B47F_9D05078FD032__INCLUDED_)
