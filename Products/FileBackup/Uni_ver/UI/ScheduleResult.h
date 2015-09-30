#if !defined(AFX_SCHEDULERESULT_H__87F32EE7_3F36_4118_B323_BE320E1B72B1__INCLUDED_)
#define AFX_SCHEDULERESULT_H__87F32EE7_3F36_4118_B323_BE320E1B72B1__INCLUDED_

#include "stdafx.h"

/////////////////////////////////////////////////////////////////////////////
// CScheduleResult dialog

class CScheduleResult : public CDialog
{
// Construction
public:
	BOOL GetShutdownString(BOOL bShutdown, CString &strShutdown);
	BOOL GetTimeString(CTime OneTime, CString &strTimeString);
	BOOL GetDiffBackupString(CTime OneTime,CString &strDiffBackup);
	BOOL GetBaseBackupString(SCHEDULE_DATA ScheduleData,CString &strBaseBackup);
	CScheduleResult(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CScheduleResult)
	enum { IDD = IDD_SCHEDULE_RESULT };
	CString	m_strBaseBackup;
	CString	m_strDiffBackup;
	CString	m_strShutdown;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScheduleResult)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScheduleResult)
	afx_msg void OnButtonYes();
	afx_msg void OnButtonNo();
	afx_msg void OnButtonCancelSchedule();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCHEDULERESULT_H__87F32EE7_3F36_4118_B323_BE320E1B72B1__INCLUDED_)
