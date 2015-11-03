// PropSht.cpp : implementation file
//

#include "stdafx.h"
#include "DeleteMaster.h"
#include "PropSht.h"

//extern CFont				g_Font;
/////////////////////////////////////////////////////////////////////////////
// CPropSht

IMPLEMENT_DYNAMIC(CPropSht, CPropertySheet)

CPropSht::CPropSht(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CPropSht::CPropSht(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;    // Lose the Apply Now button
//	m_psh.dwFlags &= ~PSH_HASHELP;  // Lose the Help button
}

CPropSht::~CPropSht()
{
}


BEGIN_MESSAGE_MAP(CPropSht, CPropertySheet)
	//{{AFX_MSG_MAP(CPropSht)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropSht message handlers

BOOL CPropSht::OnInitDialog() 
{
	int				nPage;
	TCITEM			tcItem;
	CWnd*			pWndQuit;
	CString			csText;
	CRect			rect;
	BOOL bResult = CPropertySheet::OnInitDialog();
	
	csText.LoadString (IDS_MAIN_BUTTON_EXIT);
	pWndQuit = GetDlgItem(IDOK);
	pWndQuit->ShowWindow(SW_SHOW);
	pWndQuit->EnableWindow(TRUE);
	pWndQuit->SetWindowText(csText);

	m_pTab  = GetTabControl();
    m_pPage = GetActivePage ();
	m_imageTab.Create(IDB_TAB_IMAGE_LIST,TAB_IMAGE_WEDITH,0,RGB(0,255,255));
#ifndef WIPE_ONLY
	m_pTab->SetImageList(&m_imageTab);
#endif
	tcItem.mask = TCIF_IMAGE;

	for(nPage=0;nPage<PAGE_COUNT;nPage++)
	{
		m_pTab->GetItem(nPage, &tcItem);
		tcItem.iImage = nPage;
		m_pTab->SetItem(nPage, &tcItem);
	}
	m_pTab->GetItemRect (0,&rect);
    m_pPage->GetWindowRect (&rect);
    ScreenToClient (&rect);
	rect.top += 38;
    m_pPage->MoveWindow (&rect);

	UpdateData(FALSE);
	return bResult;
}

void CPropSht::ResizeAll(int cx,int cy)
{
	CWnd			*pWnd;
	CRect			rc;
	int				nQuitRight;
	int				nQuitWidth;

	pWnd = GetParent();

    GetWindowRect (&rc);
    pWnd->ScreenToClient (&rc);
    rc.right += cx;
    rc.bottom += cy;
    MoveWindow (&rc);

    m_pTab = GetTabControl ();
    m_pTab->GetWindowRect (&rc);
    ScreenToClient (&rc);
    rc.right += cx;
    rc.bottom += cy;
    m_pTab->MoveWindow (&rc);

    m_pPage = GetActivePage ();
    m_pPage->GetWindowRect (&rc);
    ScreenToClient (&rc);
    rc.right += cx;
    rc.bottom += cy;
    m_pPage->MoveWindow (&rc);

	nQuitRight = rc.right;
    GetDlgItem(IDOK)->GetWindowRect(&rc);
    ScreenToClient(&rc);
    rc.bottom += cy;
    rc.top += cy;
	nQuitWidth = rc.right - rc.left ;

    rc.right  = nQuitRight;
    rc.left   = rc.right - nQuitWidth;
    GetDlgItem(IDOK)->MoveWindow(&rc);
}

BOOL CPropSht::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	if (pMsg->message == WM_SYSCHAR)
	{
		ActivePage(pMsg->wParam);
		return TRUE;
	}
	return CPropertySheet::PreTranslateMessage(pMsg);
}

void CPropSht::ActivePage(WORD wParam)
{
	switch(wParam)
	{
	case 'D':
	case 'd':
		SetActivePage(0);
		break;
	case 'V':
	case 'v':
		SetActivePage(1);
		break;
	default:
		break;
	}
}