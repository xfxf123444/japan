#if !defined(AFX_SHUTDOWNDLG_H__5073B31A_E890_46F6_9B28_6629B3C50884__INCLUDED_)
#define AFX_SHUTDOWNDLG_H__5073B31A_E890_46F6_9B28_6629B3C50884__INCLUDED_

#include "stdafx.h"

/////////////////////////////////////////////////////////////////////////////
// CShutdownDlg dialog

class CShutdownDlg : public CDialog
{
// Construction
public:
	BOOL m_bShutdown;
	void SetShutdownSecond(int nShutdownSecond);
	CShutdownDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CShutdownDlg)
	enum { IDD = IDD_DIALOG_SHUTDOWN };
	CProgressCtrl	m_TimeProgress;
	CString	m_strShutdownMessage;
	CString	m_strLeftSecond;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShutdownDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CShutdownDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnButtonCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int m_nLeftSecond;
	int m_nShutdownSecond;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHUTDOWNDLG_H__5073B31A_E890_46F6_9B28_6629B3C50884__INCLUDED_)
