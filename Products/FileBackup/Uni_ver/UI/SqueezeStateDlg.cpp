// SqueezeStateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "am01.h"
#include "SqueezeStateDlg.h"
#include "../AM01Expt/AM01Expt.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static BOOL g_bSqueezeThreadFinished;

DWORD WINAPI ThreadSqueezeImage(LPVOID pIn);

/////////////////////////////////////////////////////////////////////////////
// CSqueezeStateDlg dialog


CSqueezeStateDlg::CSqueezeStateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSqueezeStateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSqueezeStateDlg)
	m_strCurrentFile = _T("");
	//}}AFX_DATA_INIT
}


void CSqueezeStateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSqueezeStateDlg)
	DDX_Control(pDX, IDC_ANIMATE_SQUEEZE_FILE, m_SqueezeAnimate);
	DDX_Control(pDX, IDC_SQUEEZE_PROGRESS, m_SqueezeProgress);
	DDX_Text(pDX, IDC_CURRENT_FILE, m_strCurrentFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSqueezeStateDlg, CDialog)
	//{{AFX_MSG_MAP(CSqueezeStateDlg)
	ON_BN_CLICKED(IDC_BUTTON_CANCLE, OnButtonCancle)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSqueezeStateDlg message handlers

void CSqueezeStateDlg::OnButtonCancle() 
{
	// TODO: Add your control notification handler code here
	SetSqueezeCancel();
}

BOOL CSqueezeStateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	DWORD dwThreadID;
	m_SqueezeThreadParam.hParentWnd = m_hWnd;

	// A call to SetWindowPos forces the window to re-read its style
	m_SqueezeProgress.SetRange(0,100);
	m_SqueezeProgress.SetStep(1);
	m_SqueezeProgress.SetPos(0);

	m_SqueezeAnimate.Open(IDR_FILE_COPY);
	m_SqueezeAnimate.Play(0,0xFFFF,1);

	m_nTimer = 101;
	SetTimer(m_nTimer,200,NULL);

	g_bSqueezeThreadFinished = FALSE;
	
	m_hThreadHandle = CreateThread(NULL,0,ThreadSqueezeImage,&m_SqueezeThreadParam,0,&dwThreadID);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CSqueezeStateDlg::SetSqueezeParameter(SQUEEZE_PARAMETER SqueezeParameter)
{
	m_SqueezeThreadParam.SqueezeParameter = SqueezeParameter;
}


DWORD WINAPI ThreadSqueezeImage(LPVOID pIn)
{
	SQUEEZE_THREAD_PARAM *pPara;
	pPara= (SQUEEZE_THREAD_PARAM *)pIn;

	if( FALSE == SqueezeImage( ((*pPara).SqueezeParameter).szSourceImage,((*pPara).SqueezeParameter).szTargetImage) )
	{
		TRACE(L"\nSqueezeImage error in ThreadSqueezeImage");
	}

	PostMessage(pPara->hParentWnd,WM_CLOSE,0,0);
	
	g_bSqueezeThreadFinished = TRUE;
	
	return 0;
}


void CSqueezeStateDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

	SQUEEZE_STATE SqueezeState;
	
	GetSqueezeState(&SqueezeState);
	
	m_strCurrentFile=SqueezeState.szCurrentFile;
	m_SqueezeProgress.SetPos(SqueezeState.dwPercent);

	UpdateData(FALSE);
	
	CDialog::OnTimer(nIDEvent);
}

void CSqueezeStateDlg::OnDestroy() 
{
	
    KillTimer(m_nTimer);

	m_SqueezeAnimate.Stop();
	m_SqueezeAnimate.Close();

	if(FALSE == g_bSqueezeThreadFinished)
	{
	   SetSqueezeCancel();
	}

	while(FALSE == g_bSqueezeThreadFinished )
	{
		// wait for SqueezeThreadFinised.
		Sleep(100);
	}
	
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	
}


