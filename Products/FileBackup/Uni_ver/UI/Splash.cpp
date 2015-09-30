// Splash.cpp : implementation file
//

#include "stdafx.h"
#include "Am01.h"
#include "Splash.h"

#define SPLASH_WIDTH    500
#define SPLASH_HEIGHT   315

extern      CAM01App    theApp;

CSplash     *g_pSplash;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSplash

CSplash::CSplash()
{
	m_bClickButton = FALSE;
	m_bValidTimer  = TRUE;
	m_dwTickCount  = GetTickCount();

	m_pSplash = new CBitmap;
	m_pSplash->LoadBitmap(MAKEINTRESOURCE(IDB_SPLASH));

 	m_Rect.left = (GetSystemMetrics(SM_CXSCREEN) - SPLASH_WIDTH)/2;
	m_Rect.top = (GetSystemMetrics(SM_CYSCREEN) - SPLASH_HEIGHT)/2;
	m_Rect.right = m_Rect.left+SPLASH_WIDTH;
	m_Rect.bottom = m_Rect.top+SPLASH_HEIGHT;

//	CreateEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST ,::AfxRegisterWndClass(CS_BYTEALIGNWINDOW,0,0,0),"Splash",WS_POPUP,
	CreateEx(WS_EX_TOOLWINDOW,::AfxRegisterWndClass(CS_BYTEALIGNWINDOW,0,0,0),L"Splash",WS_POPUP,
		     m_Rect.left,m_Rect.top,m_Rect.Width(),m_Rect.Height(),NULL,0,NULL); 
}

CSplash::~CSplash()
{
    KillTimer(m_Timer);	
	if (m_pSplash)
	{
		delete m_pSplash; 
		m_pSplash=NULL;
	}
	DestroyWindow();
}


BEGIN_MESSAGE_MAP(CSplash, CWnd)
	//{{AFX_MSG_MAP(CSplash)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSplash message handlers

void CSplash::OnPaint() 
{
	CDC		MemDC;
	CPaintDC dc(this); // device context for painting
    MemDC.CreateCompatibleDC(&dc);

    VERIFY(MemDC.SelectObject(m_pSplash));
    dc.BitBlt(0,0,SPLASH_WIDTH,SPLASH_HEIGHT,&MemDC,0,0,SRCCOPY);
}

int CSplash::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
    m_Timer = SetTimer(1, 100, NULL);
	
	return 0;
}

void CSplash::OnTimer(UINT nIDEvent) 
{
	if(m_bValidTimer)
	{
   		if (GetTickCount() - m_dwTickCount > 3000 || m_bClickButton)
		{
			theApp.CloseSplash();
			m_bValidTimer = FALSE;
		}
		else	CWnd::OnTimer(nIDEvent);
	}
}

void CSplash::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_bClickButton = TRUE;
	CWnd::OnLButtonDown(nFlags, point);
}
