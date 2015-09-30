// Details.cpp : implementation file
//
#include "stdafx.h"
#include <afxpriv.h>
#include "AM01.h"
#include "Setup.h" 
#include "resource.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef _WIN32
#define IDHELP	ID_HELP
#endif

UINT	gSetupActivePage;
/////////////////////////////////////////////////////////////////////////////
// CDetails

IMPLEMENT_DYNAMIC(CSetup, CPropertySheet)

CSetup::CSetup(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CSetup::CSetup(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CSetup::~CSetup()
{
}


BEGIN_MESSAGE_MAP(CSetup, CPropertySheet)
	//{{AFX_MSG_MAP(CSetup)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDetails message handlers
BOOL CSetup::OnInitDialog() 
{
	CRect	rWinRect,
			rButtonRect;
	CString sTemp;
	int		yposn;
	CPropertySheet::OnInitDialog();
	GetWindowRect(&rWinRect);
	GetDlgItem(IDOK)->ShowWindow(SW_SHOW);
	GetDlgItem(IDOK)->EnableWindow(TRUE);
	GetDlgItem(IDCANCEL)->ShowWindow(SW_SHOW);
	GetDlgItem(IDCANCEL)->EnableWindow(TRUE);
	GetDlgItem(ID_APPLY_NOW)->ShowWindow(SW_HIDE);
	GetDlgItem(ID_APPLY_NOW)->EnableWindow(FALSE);
	GetDlgItem(IDHELP)->ShowWindow(SW_HIDE);
	GetDlgItem(IDHELP)->EnableWindow(FALSE);

	// IDOK has y posn of button
	GetDlgItem(IDOK)->GetClientRect(&rButtonRect);
	GetDlgItem(IDOK)->ClientToScreen(&rButtonRect);
	ScreenToClient(&rButtonRect);
	yposn = rButtonRect.top;
	//GetDlgItem(IDHELP)->SetWindowPos(NULL,
	//	(rWinRect.Width() - 3*rButtonRect.Width())/3,
	//	yposn,0, 0,	SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	GetDlgItem(IDOK)->SetWindowPos(NULL,
		rWinRect.Width()/2 - 3*rButtonRect.Width()/2,
		yposn,0, 0,	SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	GetDlgItem(IDCANCEL)->SetWindowPos(NULL,
		rWinRect.Width()/2 + rButtonRect.Width()/2,
		yposn,0, 0,	SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	return TRUE;
}


int CSetup::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}
