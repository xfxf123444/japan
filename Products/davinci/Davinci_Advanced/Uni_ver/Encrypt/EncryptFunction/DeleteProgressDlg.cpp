// DeleteProgressDlg.cpp : implementation file
//

#ifdef _FILE_ENCRYPT
#include "..\FileEncrypt\stdafx.h"
#include "..\FileEncrypt\fileencrypt.h"
#endif

#ifdef _SHELL_ENCRYPT
#include "..\ShellEncrypt\stdafx.h"
#include "..\ShellEncrypt\fileencrypt.h"
#endif

#ifdef _YG_FS_MON
#include "..\..\YGFSMon\stdafx.h"
#include "..\..\YGFSMon\YGFSMon.h"
#endif

#include "DeleteProgressDlg.h"
#include "..\EncryptFunction\SecureDelete.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeleteProgressDlg dialog

static CSecureDelete g_SecureDelete;
static BOOL g_bThreadFinished;
static const int DELETE_PROGRESS_TIMER_ID = 1;

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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeleteProgressDlg message handlers

void CDeleteProgressDlg::OnCancelDelete() 
{
	// TODO: Add your control notification handler code here
	m_bCancel = TRUE;
	g_SecureDelete.SetCancelDelete();
}

UINT CDeleteProgressDlg::SecureDeleteDirThread(LPVOID pParam)
{
	DELETE_THREAD_PARAMETER *pDeleteThreadParameter;
	pDeleteThreadParameter = (DELETE_THREAD_PARAMETER *)pParam;

	g_SecureDelete.DoDeleteMasterDel(pDeleteThreadParameter->szDirName,pDeleteThreadParameter->nDelMethod);

	g_bThreadFinished = TRUE;

	::PostMessage(pDeleteThreadParameter->hParentWnd,WM_CLOSE,0,0);

	return 0;
}

BOOL CDeleteProgressDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	// TODO: Add extra initialization here
	m_DeleteProgress.SetPos(0);
	m_bCancel = FALSE;
	g_bThreadFinished = FALSE;
	
	// read the progress every 0.1 second
	SetTimer(DELETE_PROGRESS_TIMER_ID,100,NULL);

	m_DeleteThreadParameter.hParentWnd = m_hWnd;
	strncpy(m_DeleteThreadParameter.szDirName,(LPCTSTR)m_strDeleteDir,MAX_PATH-1);
	m_DeleteThreadParameter.nDelMethod = m_nDeleteMethod;

    AfxBeginThread((AFX_THREADPROC)SecureDeleteDirThread,&m_DeleteThreadParameter);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDeleteProgressDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	int nPercent;

	nPercent = g_SecureDelete.GetDeletePercent();

	m_DeleteProgress.SetPos(nPercent);
	
	CDialog::OnTimer(nIDEvent);
}

void CDeleteProgressDlg::SetDeleteParameter(CString strDeleteDir, int nDeleteMethod)
{
	m_strDeleteDir = strDeleteDir;
	m_nDeleteMethod = nDeleteMethod;
}
