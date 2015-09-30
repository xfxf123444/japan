// deleteprogressdlg.cpp : implementation file
//

#include "stdafx.h"
#include "ygfsmon.h"
#include "deleteprogressdlg.h"
#include "NormalDelete.h"
#include "Function.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeleteProgressDlg dialog


static CNormalDelete g_NormalDelete;
static BOOL g_bThreadFinished;

CDeleteProgressDlg::CDeleteProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDeleteProgressDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDeleteProgressDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDeleteProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeleteProgressDlg)
	DDX_Control(pDX, IDC_DELETE_PROGRESS, m_DeleteProgress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDeleteProgressDlg, CDialog)
	//{{AFX_MSG_MAP(CDeleteProgressDlg)
	ON_BN_CLICKED(IDC_CANCEL_DELETE, OnCancelDelete)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeleteProgressDlg message handlers

void CDeleteProgressDlg::CancelDelete()
{
	m_bCancel = TRUE;
	g_NormalDelete.SetCancelDelete();

}

UINT CDeleteProgressDlg::NormalDeleteDirThread()
{
	LPVOID pParam;
	DELETE_THREAD_PARAMETER *pDeleteThreadParameter;
	pDeleteThreadParameter = (DELETE_THREAD_PARAMETER *)pParam;  

	g_NormalDelete.NormalDeleteDirectory(pDeleteThreadParameter->szDirName);

	g_bThreadFinished = TRUE;

	::PostMessage(pDeleteThreadParameter->hParentWnd,WM_CLOSE,0,0);

	return 0;

}

void CDeleteProgressDlg::SetDeleteParameter(CString strDeleteDir)
{
	m_strDeleteDir = strDeleteDir;

}

void CDeleteProgressDlg::OnCancelDelete() 
{
	// TODO: Add your control notification handler code here

	CancelDelete();
	
}

BOOL CDeleteProgressDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	m_DeleteProgress.SetPos(0);
	m_bCancel = FALSE;
	g_bThreadFinished = FALSE;
	
	// read the progress every 0.1 second
	SetTimer(DELETE_PROGRESS_TIMER_ID,100,NULL);

	m_DeleteThreadParameter.hParentWnd = m_hWnd;
	strncpy(m_DeleteThreadParameter.szDirName,(LPCTSTR)m_strDeleteDir,MAX_PATH-1);

    NormalDeleteDirThread();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDeleteProgressDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

	int nPercent;

	g_NormalDelete.GetCurrentPercent(&nPercent);

	m_DeleteProgress.SetPos(nPercent);

	
	CDialog::OnTimer(nIDEvent);
}

void CDeleteProgressDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here

	// if the user press cancel
	if( m_bCancel)
	{
		// delete the current delete file
		// since the current delete file may be not integrate
		if(m_strCurrentDeleteFile.GetLength()>0)
		{
			if(IsFile((LPCTSTR)m_strCurrentDeleteFile) )
			{
				SetFileAttributes((LPCTSTR)m_strCurrentDeleteFile,FILE_ATTRIBUTE_NORMAL);
				TRACE("\nDelete %s when cancel delete.",(LPCTSTR)m_strCurrentDeleteFile);
				DeleteFile((LPCTSTR)m_strCurrentDeleteFile);
			}
		}
	}

	// if the user use ESC, or sysmenu to close the dialog

	if( FALSE == g_bThreadFinished )
	{
		CancelDelete();
	}

	while(FALSE == g_bThreadFinished)
	{
		Sleep(100);
		// wait for the Thread Finish.
	}
	
	KillTimer(DELETE_PROGRESS_TIMER_ID);	
	
}
