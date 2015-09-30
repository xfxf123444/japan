// ShutdownDlg.cpp : implementation file
//

#include "stdafx.h"
#include "am01.h"
#include "ShutdownDlg.h"
#include "../AM01Struct/AM01Struct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CShutdownDlg dialog


CShutdownDlg::CShutdownDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShutdownDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShutdownDlg)
	m_strShutdownMessage = _T("");
	m_strLeftSecond = _T("");
	//}}AFX_DATA_INIT
	m_nShutdownSecond=0;
}


void CShutdownDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShutdownDlg)
	DDX_Control(pDX, IDC_PROGRESS_TIME, m_TimeProgress);
	DDX_Text(pDX, IDC_STATIC_LEFT_SECOND, m_strLeftSecond);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShutdownDlg, CDialog)
	//{{AFX_MSG_MAP(CShutdownDlg)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnButtonCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShutdownDlg message handlers

BOOL CShutdownDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	CenterWindow();
	CenterWindow(GetDesktopWindow());

    ::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_SHOWWINDOW|SWP_NOSIZE );

	m_nLeftSecond=m_nShutdownSecond;

	SetTimer(1,1000,NULL);
	m_strLeftSecond.Format(L"%d",m_nLeftSecond);
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CShutdownDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	m_nLeftSecond --;

	if( m_nLeftSecond <= 0)
	{
		m_bShutdown = TRUE;
		PostMessage(WM_CLOSE,0,0);
	}

    m_strLeftSecond.Format(L"%d",m_nLeftSecond);
	m_TimeProgress.SetPos( (m_nShutdownSecond-m_nLeftSecond)* 100 / m_nShutdownSecond );

	UpdateData(FALSE);

	CDialog::OnTimer(nIDEvent);
}


void CShutdownDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	KillTimer(1);
	CDialog::OnClose();
}

void CShutdownDlg::OnButtonCancel() 
{
	// TODO: Add your control notification handler code here
	OnCancel(); // DoModal return FALSE
}

void CShutdownDlg::SetShutdownSecond(int nShutdownSecond)
{
	m_nShutdownSecond=nShutdownSecond;
}
