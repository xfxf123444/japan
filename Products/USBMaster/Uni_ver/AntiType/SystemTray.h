// SystemTray.h: interface for the CSystemTray class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYSTEMTRAY_H__7D9B01E8_B29A_4DF2_A9D9_0DF09B8DC0E6__INCLUDED_)
#define AFX_SYSTEMTRAY_H__7D9B01E8_B29A_4DF2_A9D9_0DF09B8DC0E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSystemTray  
{
public:
	CSystemTray(CWnd* pWnd, UINT uCallbackMessage, LPCTSTR szTip);
	virtual ~CSystemTray();

public:
	NOTIFYICONDATA	m_tnd;

public:
	void SetState(LPCTSTR szTip) ;
	void Close();

};

#endif // !defined(AFX_SYSTEMTRAY_H__7D9B01E8_B29A_4DF2_A9D9_0DF09B8DC0E6__INCLUDED_)
