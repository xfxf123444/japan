#include "stdafx.h"
#include "TrayNot.h"
#include "resource.h"
#include "AM01.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CAM01App theApp;
/////////////////////////////////////////////////////////////////////////////
// CTrayNot

CTrayNot::CTrayNot ( CWnd* pWnd, UINT uCallbackMessage,	LPCTSTR szTip)
{
	// this is only for Windows 95 (or higher)
	// load up the NOTIFYICONDATA structure
	m_tnd.cbSize = sizeof(NOTIFYICONDATA) ;
	m_tnd.hWnd = pWnd->GetSafeHwnd() ;
	m_tnd.uID = uCallbackMessage+1;
	m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	m_tnd.uCallbackMessage = uCallbackMessage ;
	wcscpy ( m_tnd.szTip, szTip ) ;

	// save the pointer to the icon list and set the initial
	// default icon.
    m_tnd.hIcon = theApp.LoadIcon(IDI_ICON_MAIN);
	Shell_NotifyIcon ( NIM_ADD, &m_tnd );
}

CTrayNot::~CTrayNot()
{
	Shell_NotifyIcon ( NIM_DELETE, &m_tnd );
}

void CTrayNot::SetState(LPCTSTR szTip)
{
	wcscpy ( m_tnd.szTip, szTip ) ;
	Shell_NotifyIcon ( NIM_MODIFY, &m_tnd );
}

