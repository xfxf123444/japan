// RestoreState.cpp : implementation file
//

#include "stdafx.h"
#include "AMRestor.h"
#include "RestoreState.h"

#include "..\AM01Struct\AM01Struct.h"
#include "..\AM01Expt\AM01Expt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//-- third time added begin

// extern int g_nPercent;
// extern char g_szCurrentFile[MAX_PATH];
// extern BOOL g_bFinishRestore;

extern RESTORE_STATE g_RestoreState;
extern AM_RESTORE_INFO g_RestoreInfo;
extern CDATA_ARRAY g_ImageArray;

//-- third time added end

/////////////////////////////////////////////////////////////////////////////
// CRestoreState dialog


CRestoreState::CRestoreState(CWnd* pParent /*=NULL*/)
	: CDialog(CRestoreState::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRestoreState)
	m_CurrentFile = _T("");
	//}}AFX_DATA_INIT
}


void CRestoreState::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRestoreState)
	DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
	DDX_Control(pDX, IDC_ANIMATE1, m_AnimateCtrl);
	DDX_Text(pDX, IDC_CURRENT_FILE, m_CurrentFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRestoreState, CDialog)
	//{{AFX_MSG_MAP(CRestoreState)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRestoreState message handlers

BOOL CRestoreState::OnInitDialog() 
{
    
	DWORD dwThreadID;

	CDialog::OnInitDialog();
	m_ThreadParam.hParentWnd = m_hWnd;

	// A call to SetWindowPos forces the window to re-read its style
	m_Progress.SetRange(0,100);
	
	// m_Progress.SetStep(1);
	
	m_Progress.SetPos(0);

	m_AnimateCtrl.Open(IDR_FILE_COPY);
	m_AnimateCtrl.Play(0,0xFFFF,1);

	m_nTimer = 101;
	SetTimer(m_nTimer,50,NULL);

	//-- third time for debug
	
	m_hThreadHandle = CreateThread(NULL,0,ThreadFun,&m_ThreadParam,0,&dwThreadID);
	
	//-- third time for debug end

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRestoreState::OnClose() 
{
	m_AnimateCtrl.Stop();
	m_AnimateCtrl.Close();
	KillTimer(m_nTimer);
	TerminateThread(m_hThreadHandle,0);
	CDialog::OnClose();
}

void CRestoreState::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

	RESTORE_STATE RestoreState;
	GetRestoreState(&RestoreState);
	m_Progress.SetPos(RestoreState.wPercent);
	m_CurrentFile=RestoreState.szCurrentFile;

	UpdateData(FALSE);

	CDialog::OnTimer(nIDEvent);
}


DWORD WINAPI ThreadFun(LPVOID pIn)
{
	PTHREAD_PARAM pPara = (PTHREAD_PARAM)pIn;
	RESTORE_STATE RestoreState;
	
	// here to restore file.

	RestoreFile(g_RestoreInfo,g_ImageArray);

	GetRestoreState(&RestoreState);
	if (!RestoreState.bRestoreCancel)
		PostMessage(pPara->hParentWnd,WM_CLOSE,0,0);
	return 0;
}

void CRestoreState::OnCancel() 
{
	// TODO: Add extra cleanup here
	RESTORE_STATE RestoreState;
	
	GetRestoreState(&RestoreState);
	RestoreState.bRestoreCancel = TRUE;
	SetRestoreState(RestoreState);

	CDialog::OnCancel();
}
