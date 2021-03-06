// DecryptStatus.cpp : implementation file
//

#include "stdafx.h"
#include "filedecrypt.h"
#include "DecryptStatus.h"
#include "..\DecryptFunction\DecryptFunction.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef unsigned (__stdcall *PTHREAD_START) (void *);

static BOOL g_bThreadFinished = FALSE;
extern DECRYPT_INFO g_DecryptInfo;
extern BOOL g_bDecryptSelectedFolderOnly;
BOOL g_bDecryptSucceed;

DWORD WINAPI ThreadDecryptFile(LPVOID pIn);

/////////////////////////////////////////////////////////////////////////////
// CDecryptStatus dialog


CDecryptStatus::CDecryptStatus(CWnd* pParent /*=NULL*/)
	: CDialog(CDecryptStatus::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDecryptStatus)
	m_strCurrentFile = _T("");
	//}}AFX_DATA_INIT
	m_bConcisePrompt = FALSE;
	m_bDecryptSelectedFolderOnly = FALSE;
}

void CDecryptStatus::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDecryptStatus)
	DDX_Control(pDX, IDC_DECRYPT_PROGRESS, m_Progress);
	DDX_Control(pDX, IDC_ANIMATE_ENCRYPT_FILE, m_AnimateCtrl);
	DDX_Text(pDX, IDC_CURRENT_FILE, m_strCurrentFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDecryptStatus, CDialog)
	//{{AFX_MSG_MAP(CDecryptStatus)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnButtonCancel)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDecryptStatus message handlers

void CDecryptStatus::OnButtonCancel() 
{
	// TODO: Add your control notification handler code here
	SetDecryptCancel();
}

BOOL CDecryptStatus::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	DWORD dwThreadID;
	m_ThreadParam.hParentWnd = m_hWnd;
	m_ThreadParam.DecryptInfo = m_DecryptInfo;
	m_ThreadParam.bDecryptSelectedFolderOnly = m_bDecryptSelectedFolderOnly;

	// A call to SetWindowPos forces the window to re-read its style
	m_Progress.SetRange(0,100);
	m_Progress.SetStep(1);
	m_Progress.SetPos(0);

	m_AnimateCtrl.Open(IDR_FILE_COPY);
	m_AnimateCtrl.Play(0,0xFFFF,1);

	m_nTimer = 101;
	SetTimer(m_nTimer,200,NULL);

	g_bThreadFinished = FALSE;

	m_hThreadHandle = (HANDLE)_beginthreadex(NULL,0,(PTHREAD_START)ThreadDecryptFile,&m_ThreadParam,0,(unsigned *)(&dwThreadID));
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

DWORD WINAPI ThreadDecryptFile(LPVOID pIn)
{
   THREAD_PARAM *pThreadParam;

   g_bThreadFinished = FALSE;

   pThreadParam = (THREAD_PARAM *)pIn;

   if( FALSE == DecryptSelectionFile(pThreadParam->DecryptInfo, pThreadParam->bDecryptSelectedFolderOnly) )
   {
	   TRACE(L"\nDecryptSelection error in ThreadDecryptFile.");
       PostMessage(pThreadParam->hParentWnd,WM_CLOSE,0,0);
       g_bThreadFinished = TRUE;
	   return FALSE;
   }
   else
   {
	   g_bDecryptSucceed = TRUE;
   }

   PostMessage(pThreadParam->hParentWnd,WM_CLOSE,0,0);
   g_bThreadFinished = TRUE;
   return TRUE;
}

void CDecryptStatus::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
	DA_WORK_STATE WorkState;
	
	GetWorkState(WorkState);
	
	if( FALSE == m_bConcisePrompt )
	{
	  m_strCurrentFile=WorkState.szCurrentFile;
	}
	else
	{
	   CString strCurrentFile;
	   strCurrentFile=WorkState.szCurrentFile;
	   int nIndex;
	   nIndex = strCurrentFile.ReverseFind(L'\\');
	   m_strCurrentFile = strCurrentFile.Mid(nIndex+1);
	}
	
	m_Progress.SetPos(WorkState.nPercent);

	UpdateData(FALSE);

	CDialog::OnTimer(nIDEvent);
}

void CDecryptStatus::OnDestroy() 
{
	
	// TODO: Add your message handler code here
	m_AnimateCtrl.Stop();
	m_AnimateCtrl.Close();

	KillTimer(m_nTimer);

	if(FALSE == g_bThreadFinished )
	{
	   SetDecryptCancel();
	}

	while(FALSE == g_bThreadFinished)
	{
		Sleep(100);
		// wait for the Thread Finish.
	}	

	// at most wait 2 seconds for the thread to quit
	// WaitForSingleObject(m_hThreadHandle,2000);

	CloseHandle(m_hThreadHandle);	

	CDialog::OnDestroy();
}

void CDecryptStatus::OnOK()
{
	// if the user press Enter, do not close the dialog
	// CDialog::OnOK();
}
