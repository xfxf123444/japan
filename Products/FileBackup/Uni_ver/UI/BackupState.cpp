// BackupState.cpp : implementation file
//

#include "stdafx.h"
#include "AM01.h"
#include "BackupState.h"

// #include "..\AM01Struct\AM01Struct.h"
#include "..\AM01Expt\AM01Expt.h"

static BOOL g_bThreadFinished;

DWORD WINAPI ThreadBackupFile(LPVOID pIn);

BOOL CheckMailDataReady(CStringArray &SourceArray);
BOOL CheckDirectoryInfo(LPCTSTR lpstrSourceDirectory);

// #include "Function.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// AM01WORKSTATE g_WorkState;
// extern BOOL g_bUserCancel;

/////////////////////////////////////////////////////////////////////////////
// CBackupState dialog


CBackupState::CBackupState(CWnd* pParent /*=NULL*/)
	: CDialog(CBackupState::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBackupState)
	m_CurrentFile = _T("");
	//}}AFX_DATA_INIT
}


void CBackupState::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBackupState)
	DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
	DDX_Control(pDX, IDC_ANIMATE1, m_AnimateCtrl);
	DDX_Text(pDX, IDC_CURRENT_FILE, m_CurrentFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBackupState, CDialog)
	//{{AFX_MSG_MAP(CBackupState)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnButtonCancel)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBackupState message handlers

BOOL CBackupState::OnInitDialog() 
{
	CDialog::OnInitDialog();

	DWORD dwThreadID;
	m_ThreadParam.hParentWnd = m_hWnd;
	m_ThreadParam.pSetting = &m_Setting;

	// A call to SetWindowPos forces the window to re-read its style
	m_Progress.SetRange(0,100);
	m_Progress.SetStep(1);
	m_Progress.SetPos(0);

	m_AnimateCtrl.Open(IDR_FILE_COPY);
	m_AnimateCtrl.Play(0,0xFFFF,1);

	m_nTimer = 101;
	SetTimer(m_nTimer,200,NULL);

	g_bThreadFinished = FALSE;
	
	m_hThreadHandle = CreateThread(NULL,0,ThreadBackupFile,&m_ThreadParam,0,&dwThreadID);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CBackupState::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

	AM01WORKSTATE WorkState;
	
	GetWorkState(&WorkState);
	
	m_CurrentFile=WorkState.szCurrentFile;
	m_Progress.SetPos(WorkState.wPercent);

	UpdateData(FALSE);
	CDialog::OnTimer(nIDEvent);
}

DWORD WINAPI ThreadBackupFile(LPVOID pIn)
{
	PTHREAD_PARAM pPara = (PTHREAD_PARAM)pIn;
	switch(pPara->pSetting->DataInfo.wBackupType)
	{
	case MAIL_BACKUP:
		// here need fall through
	case WIN_MAIL_BACKUP:
	case OE_MAIL_BACKUP:
		if (!pPara->pSetting->DataInfo.BackupOption.bLogFail && 
			!pPara->pSetting->DataInfo.BackupOption.bRetryFail)
		{
			if(CheckMailDataReady(pPara->pSetting->pcaSource) == FALSE)
			{
				AfxMessageBox(IDS_MAIL_DATA_NOT_READY,MB_OK|MB_ICONINFORMATION,NULL);
				break;
			}
		}
	case FILE_BACKUP:
		BackupSelectionFile(pPara->pSetting->DataInfo,pPara->pSetting->pcaSource);
	    break;
	case BACKUP_TYPE_DIFF:
		DiffBackupSelectionFile(pPara->pSetting->DataInfo.szTargetImage,&pPara->pSetting->DataInfo.BackupOption);
		break;
	case EASY_BACKUP:
		BackupSelectionFile(pPara->pSetting->DataInfo,pPara->pSetting->pcaSource);
		break;
	}

	PostMessage(pPara->hParentWnd,WM_CLOSE,0,0);

	g_bThreadFinished = TRUE;

	return 0;
}

void CBackupState::OnButtonCancel() 
{
	// TODO: Add your control notification handler code here
	SetBakCancel();
}


BOOL CheckMailDataReady(CStringArray &SelectionArray)
{
	
	CString OnePathString;

	for(long i=0; i<SelectionArray.GetSize(); i++)
	{
        OnePathString=SelectionArray.ElementAt(i);
		if( FALSE == IsOneDirectory(OnePathString) )
		{
			  // Then OnePathString is not a directory

			  HANDLE hFileSource;

			  hFileSource=CreateFile((LPCTSTR)OnePathString,
									GENERIC_READ,
									FILE_SHARE_READ,NULL,OPEN_EXISTING,
     								FILE_ATTRIBUTE_NORMAL,0);

			  if(hFileSource == INVALID_HANDLE_VALUE)
			  {
				  return FALSE;
			  }

		      CloseHandle(hFileSource);
		}
		else
		{
			if( CheckDirectoryInfo((LPCTSTR)OnePathString) == FALSE )
			{
			    return FALSE;
			}
		}
	}

	return TRUE;
}


BOOL CheckDirectoryInfo(LPCTSTR szSourceDirectory)
{

	DWORD dwFileAttribute;
	
	dwFileAttribute = GetFileAttributes(szSourceDirectory);

	if( -1 == dwFileAttribute )
	{
		return FALSE;
	}

	if(  0 == (dwFileAttribute & FILE_ATTRIBUTE_DIRECTORY) )
	{
		// if it is a file.
		return FALSE;
	}

	// TRACE(L"\nDir Name:%s",szDirName);
	
	CString strFind;
	strFind=strFind + szSourceDirectory + L"\\*.*";

	HANDLE hFind;
	WIN32_FIND_DATA FindFileData;

	hFind = FindFirstFile((LPCTSTR)strFind,&FindFileData);

	if(INVALID_HANDLE_VALUE == hFind)
	{
		return FALSE;
	}

	do
	{
		if(
			wcscmp(FindFileData.cFileName,L".")==0 ||
			wcscmp(FindFileData.cFileName,L"..")==0
		  )
		{
			continue;
		}

		CString strSubPath;
		strSubPath=strSubPath + szSourceDirectory +L"\\"+ FindFileData.cFileName;

		if( 0 == ( FILE_ATTRIBUTE_DIRECTORY & FindFileData.dwFileAttributes ) )
		{
			// this is the output
			// TRACE(L"\nFile Name:%s",(LPCTSTR)strSubPath);
			HANDLE hFileSource;
			
			hFileSource=CreateFile((LPCTSTR)strSubPath,
									GENERIC_READ,
									FILE_SHARE_READ,NULL,OPEN_EXISTING,
									FILE_ATTRIBUTE_NORMAL,0);
			
			if(hFileSource == INVALID_HANDLE_VALUE)
			{
				return FALSE;
			}
			
			CloseHandle(hFileSource);
			
		}
		else
		{
			return( CheckDirectoryInfo((LPCTSTR)strSubPath) );
            // PureTravelDir((LPCTSTR)strSubPath);
		}
	}while(FindNextFile(hFind,&FindFileData));

	FindClose(hFind);
	
	return TRUE;
}

void CBackupState::OnOK()
{
	// if the user press Enter, do not close the dialog
	// CDialog::OnOK();
}


void CBackupState::OnDestroy() 
{

	m_AnimateCtrl.Stop();
	m_AnimateCtrl.Close();

	KillTimer(m_nTimer);

	if(FALSE == g_bThreadFinished )
	{
	   SetBakCancel();
	}

	while(FALSE == g_bThreadFinished)
	{
		Sleep(100);
		// wait for the Thread Finish.
	}

	CDialog::OnDestroy();

	// TODO: Add your message handler code here
	
}
