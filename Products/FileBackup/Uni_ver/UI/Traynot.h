#ifndef TRAY_NOTIFY_HEAD_FILE_H
#define TRAY_NOTIFY_HEAD_FILE_H
/////////////////////////////////////////////////////////////////////////////
// TrayNot.h : header file
//
// This is a part of the Webster HTTP Server application
// for Microsoft Systems Journal
//

/////////////////////////////////////////////////////////////////////////////
// CTrayNot window
#include "stdafx.h"
class CTrayNot : public CObject
{
// Construction
public:
	CTrayNot ( CWnd* pWnd, UINT uCallbackMessage, LPCTSTR szTip) ;

// Attributes
public:
	NOTIFYICONDATA	m_tnd ;

// Operations
public:
	void SetState(LPCTSTR szTip) ;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrayNot)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTrayNot();

};

/////////////////////////////////////////////////////////////////////////////
#endif