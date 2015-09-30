#if !defined(AFX_WIPEPROC_H__1D42058F_C257_4992_86FF_5DA081BBC1F6__INCLUDED_)
#define AFX_WIPEPROC_H__1D42058F_C257_4992_86FF_5DA081BBC1F6__INCLUDED_

#include "StdAfx.h"

/////////////////////////////////////////////////////////////////////////////
// CWipeProc dialog

class CWipeProc : public CDialog
{
// Construction
public:
	CWipeProc(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWipeProc)
	enum { IDD = IDD_WIPE_PROGRESS };
	CButton	m_CancelButton;
	CStatic	m_WipePresent;
	CButton	m_SaveButton;
	CButton	m_NoSaveButton;
	CProgressCtrl	m_WipeProgress;
	CString	m_szTar;
	CString	m_WipeMothed;
	CString	m_szPercent;
	CString	m_csElasped;
	CString	m_csRemain;
	//}}AFX_DATA


	TCHAR		m_DriveLetter;
	int			m_nPresent;
	int			m_nFileNum;
	DWORD		m_dwElaspedTime;
	BOOL		m_bCancelProc; 
	__int64		m_nTotalFileSize;
	__int64		m_nTotalWipeSize;
	__int64		m_nWipedSize;
	struct tm	m_StartTime;
	struct tm	m_EndTime;

	HANDLE		m_Thread;

	BOOL WipeFiles(TCHAR szDriveLetter,int nFileNum,BYTE *pBuf);
	BOOL					WipePartition(int nFileNum);
	BOOL					WipeContent();
	BOOL					SaveReport(LPCTSTR lpFileName);
	static	DWORD WINAPI	ThreadProc(CWipeProc* dlg);
	virtual DWORD			ProgressProc();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWipeProc)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWipeProc)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnNoSave();
	afx_msg void OnSave();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIPEPROC_H__1D42058F_C257_4992_86FF_5DA081BBC1F6__INCLUDED_)
