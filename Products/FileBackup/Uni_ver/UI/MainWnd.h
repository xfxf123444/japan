#if !defined(AFX_MAINWND_H__313F6B63_3E97_11D3_A638_5254AB10413A__INCLUDED_)
#define AFX_MAINWND_H__313F6B63_3E97_11D3_A638_5254AB10413A__INCLUDED_

#include "stdafx.h"
#include "TrayNot.h"
#include "Setup.h"
#include "AMBackup.h"
#include "AMSetting.h"
#include "AMTask.h"
#include "AMAbout.h"
#include "..\AM01STRUCT\AM01Struct.h"	// Added by ClassView


/////////////////////////////////////////////////////////////////////////////
// CMainWnd window

class CMainWnd : public CWnd
{
// Construction
public:
	CMainWnd();

// Attributes
public:
    CTrayNot    *m_pTray;
	int         m_nTimer;
    CString     m_ToolTip;
	// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainWnd)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL m_bExecuting;
	
	BOOL CheckSettingSchedule(SCHEDULE_DATA OneScheduleData,CTime CurTime, BOOL *pbStartRun,BOOL *pbNewBase, BOOL *pbShutdown);
	
	void InitSubMenu(CMenu *pMenu);
	void OnOpenMainControl();
	void OnSearch();
	void OnCloseApp();
	void OnSetCurrent();
	void OnNewBackup();
	void OnRestore();
	void OnBackupShutdown();
	virtual ~CMainWnd();
	CSetup      *m_pControl;
	// Generated message map functions
protected:
	//{{AFX_MSG(CMainWnd)
	afx_msg LRESULT OnMyNotify(UINT,LONG);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bIsWorking;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINWND_H__313F6B63_3E97_11D3_A638_5254AB10413A__INCLUDED_)
