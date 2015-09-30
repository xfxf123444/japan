#if !defined(AFX_MAINWND_H__313F6B63_3E97_11D3_A638_5254AB10413A__INCLUDED_)
#define AFX_MAINWND_H__313F6B63_3E97_11D3_A638_5254AB10413A__INCLUDED_

#include "TrayNot.h"
#include "Setup.h"
#include "Protect.h"

#include "..\Encrypt\EncryptFunction\EncryptInfo.h"

#ifdef NO_DRIVER_MODE
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <string>
using namespace std;
#endif

typedef struct
{
	CEncryptInfo  EncryptInfo; 
}THREAD_PARAM;

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MainWnd.h : header file
//

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
    CString     m_ToolTip;
	// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	// CStringArray m_ProtectedPathArray;
	int m_nTimer;
#ifdef NO_DRIVER_MODE
	int m_nTimerForUpdate;
#endif
	virtual ~CMainWnd();
	void OnMyParseFile();
	void OnUpdateFileInfo();
	CSetup      *m_pControl;
	// Generated message map functions
protected:
	//{{AFX_MSG(CMainWnd)
	afx_msg LRESULT OnMyNotify(UINT,LONG);
	afx_msg void OnOpenMainControl();
	afx_msg void OnCloseWindow();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnQueryEndSession();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	THREAD_PARAM m_ThreadParam;
	HANDLE m_hThreadHandle;
#ifdef NO_DRIVER_MODE
	THREAD_PARAM m_ThreadParamForUpdate;
	HANDLE m_hThreadHandleForUpdate;
#endif

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINWND_H__313F6B63_3E97_11D3_A638_5254AB10413A__INCLUDED_)
