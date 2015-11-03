// Main.cpp : implementation file
//

#include "stdafx.h"
#include "DeleteMaster.h"
#include "Main.h"
#include "Fun.h"

HWND				g_hMain;
extern YG_PARTITION_INFO*	g_pFixDiskInfo;
//extern CFont				g_Font;

/////////////////////////////////////////////////////////////////////////////
// CMain

CMain::CMain()
{
	CString		csCaption;
	
	csCaption.LoadString(IDS_PRODUCT_NAME);

	rExit=CRect(MAIN_CLIENT_WIDTH-SB_WIDTH-10,MAIN_CLIENT_HEIGHT-SB_HEIGHT-10,MAIN_CLIENT_WIDTH-10,MAIN_CLIENT_HEIGHT-10);
	rExe=rExit+CRect(0,SB_HEIGHT+5,0,- SB_HEIGHT - 5);

	//rWindow.left   = (GetSystemMetrics(SM_CXSCREEN) - MAIN_CLIENT_WIDTH)/2-GetSystemMetrics(SM_CXFRAME)-GetSystemMetrics(SM_CXBORDER);
	//rWindow.top    = (GetSystemMetrics(SM_CYSCREEN)- MAIN_CLIENT_HEIGHT)/2-GetSystemMetrics(SM_CYFRAME)-GetSystemMetrics(SM_CYSIZE)-GetSystemMetrics(SM_CYBORDER) ;
	//rWindow.right  = rWindow.left+MAIN_CLIENT_WIDTH+GetSystemMetrics(SM_CXFRAME)+GetSystemMetrics(SM_CXBORDER);
	//rWindow.bottom = rWindow.top+MAIN_CLIENT_HEIGHT+GetSystemMetrics(SM_CYFRAME)+GetSystemMetrics(SM_CYSIZE)+GetSystemMetrics(SM_CYBORDER);

	rWindow.left   = GetSystemMetrics(SM_CXSCREEN) / 4;
	rWindow.top    = GetSystemMetrics(SM_CYSCREEN) / 4;
	rWindow.right  = rWindow.left+GetSystemMetrics(SM_CXSCREEN) / 2;
	rWindow.bottom = rWindow.top+GetSystemMetrics(SM_CYSCREEN) / 2;

	CreateEx(WS_EX_STATICEDGE ,DMMainWinClass,csCaption,WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME,
			 rWindow.left,rWindow.top,rWindow.Width(),rWindow.Height(),
			 NULL,NULL);
}

CMain::~CMain()
{
}


BEGIN_MESSAGE_MAP(CMain, CWnd)
	//{{AFX_MSG_MAP(CMain)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMain message handlers

int CMain::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	CRect			rWork,rClient;
	int				nBottom;
	WINDOWPLACEMENT wp;

	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	g_hMain = this->m_hWnd;
	GetClientRect(&rClient);
	m_pPropSht		= new CPropSht(_T("DM_PropSht"),this);
	m_pDelete		= new CDelPar();
	m_pDiskView		= new CDiskView();
	m_pWipeFree		= new CWipeFree();
	
#ifndef WIPE_ONLY
	m_pPropSht->AddPage(m_pDelete);
	m_pPropSht->AddPage(m_pDiskView);
#endif
	m_pPropSht->AddPage(m_pWipeFree);
	m_pPropSht->Create(this,WS_CHILD|WS_VISIBLE );
	m_pPropSht->GetClientRect(&rWork);
	nBottom = rClient.bottom - MAIN_QUIT_AREA ;
	m_pPropSht->ResizeAll(rClient.right-rWork.right,nBottom-rWork.bottom );
	m_pPropSht->GetClientRect(&m_rcPageClient);

	GetClientRect(&rWork);
	m_pPropSht->MoveWindow (rWork);

	ZeroMemory(&wp, sizeof(WINDOWPLACEMENT));
	wp.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(&wp);
	SetWindowPlacement(&wp);
	return 0;
}

void CMain::CloseAllPages()
{
	if(m_pPropSht)
	{
		delete(m_pPropSht);
		delete(m_pDelete);
		delete(m_pDiskView);
		delete(m_pWipeFree);
	}
	FreePartList(g_pFixDiskInfo);
}

void CMain::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	CloseAllPages();
	
	CWnd::OnClose();
}

void CMain::OnSize(UINT nType, int cx, int cy) 
{
	CRect		rect;
	int			nBottom;

	if(nType  != SIZE_MINIMIZED )
	{
		m_pPropSht->MoveWindow (m_rcPageClient);
	
		m_pPropSht->GetClientRect(&rect);
		nBottom = cy - MAIN_QUIT_AREA;
		m_pPropSht->ResizeAll(cx-rect.right,nBottom-rect.bottom );
		
		m_pPropSht->GetClientRect(&m_rcPageClient);

		GetClientRect(&rect);
		m_pPropSht->MoveWindow (rect);
	}
	
	CWnd::OnSize(nType, cx, cy);
	
}

void CMain::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	m_ptMinTrackSize.x = rWindow.Width();
	m_ptMinTrackSize.y = rWindow.Height();
	lpMMI->ptMinTrackSize = m_ptMinTrackSize;
	CWnd::OnGetMinMaxInfo(lpMMI);
}

