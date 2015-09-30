// SystemTray.cpp: implementation of the CSystemTray class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "antitype.h"
#include "SystemTray.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern CAntiTypeApp theApp;

CSystemTray::CSystemTray(CWnd* pWnd, UINT uCallbackMessage,	LPCTSTR szTip)
{
	m_tnd.cbSize = sizeof(NOTIFYICONDATA) ;
	m_tnd.hWnd = pWnd->GetSafeHwnd() ;
	m_tnd.uID = uCallbackMessage+1;
	m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	m_tnd.uCallbackMessage = uCallbackMessage ;
	strcpy ( m_tnd.szTip, szTip ) ;

	m_tnd.hIcon = theApp.LoadIcon(IDR_MAINFRAME);
	Shell_NotifyIcon ( NIM_ADD, &m_tnd );
}

CSystemTray::~CSystemTray()
{
	Shell_NotifyIcon ( NIM_DELETE, &m_tnd );
}

void CSystemTray::SetState(LPCTSTR szTip)
{
	strcpy ( m_tnd.szTip, szTip ) ;
	Shell_NotifyIcon ( NIM_MODIFY, &m_tnd );
}

void CSystemTray::Close()
{
	Shell_NotifyIcon ( NIM_DELETE, &m_tnd );
}
