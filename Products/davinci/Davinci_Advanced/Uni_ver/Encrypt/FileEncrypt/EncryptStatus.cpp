// EncryptStatus.cpp : implementation file
//

#include "stdafx.h"
#include "fileencrypt.h"
#include <process.h>      // for _beginthreadex
#include "EncryptStatus.h"
#include "..\EncryptFunction\EncryptInfo.h"
#include "..\..\DavinciFunction\DavinciFunction.h"
#include "..\EncryptFunction\EncryptFunction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CEncryptStatus dialog

typedef unsigned (__stdcall *PTHREAD_START) (void *);

BOOL g_bThreadFinished;

BOOL g_bEncryptSucceed;

extern BOOL g_bCreateSelfExtractFile;
DWORD WINAPI ThreadEncryptFile(LPVOID pIn);

extern CEncryptInfo g_EncryptInfo;

CEncryptStatus::CEncryptStatus(CWnd* pParent /*=NULL*/)
	: CDialog(CEncryptStatus::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEncryptStatus)
	m_strCurrentFile = _T("");
	//}}AFX_DATA_INIT
}


void CEncryptStatus::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEncryptStatus)
	DDX_Control(pDX, IDC_ANIMATE_ENCRYPT_FILE, m_AnimateCtrl);
	DDX_Control(pDX, IDC_ENCRYPT_PROGRESS, m_Progress);
	DDX_Text(pDX, IDC_CURRENT_FILE, m_strCurrentFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEncryptStatus, CDialog)
	//{{AFX_MSG_MAP(CEncryptStatus)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnButtonCancel)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEncryptStatus message handlers

BOOL CEncryptStatus::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	DWORD dwThreadID;
	m_ThreadParam.hParentWnd = m_hWnd;

	// A call to SetWindowPos forces the window to re-read its style
	m_Progress.SetRange(0,100);
	m_Progress.SetStep(1);
	m_Progress.SetPos(0);

	m_AnimateCtrl.Open(IDR_FILE_COPY);
	m_AnimateCtrl.Play(0,0xFFFF,1);

	m_nTimer = 101;
	SetTimer(m_nTimer,200,NULL);

    g_bEncryptSucceed = FALSE;

	g_bThreadFinished = FALSE;
	
	m_hThreadHandle = (HANDLE)_beginthreadex(NULL,0,(PTHREAD_START)ThreadEncryptFile,&m_ThreadParam,0,(unsigned *)(&dwThreadID));

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEncryptStatus::OnButtonCancel() 
{
	// TODO: Add your control notification handler code here
	SetEncryptCancel();
}



void CEncryptStatus::OnDestroy() 
{
	// TODO: Add your message handler code here

	m_AnimateCtrl.Stop();
	m_AnimateCtrl.Close();

	KillTimer(m_nTimer);

	if(FALSE == g_bThreadFinished )
	{
	   SetEncryptCancel();
	}

	while(FALSE == g_bThreadFinished)
	{
		Sleep(100);
		// wait for the Thread Finish.
	}	

	CloseHandle(m_hThreadHandle);

	CDialog::OnDestroy();
}

void CEncryptStatus::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

	DA_WORK_STATE WorkState;
	
	GetWorkState(WorkState);
	
	m_strCurrentFile=WorkState.szCurrentFile;
	if (g_bCreateSelfExtractFile && WorkState.nPercent >= 100) {
		m_Progress.SetPos(99);
	}
	else {
		m_Progress.SetPos(WorkState.nPercent);
	}

	UpdateData(FALSE);

	CDialog::OnTimer(nIDEvent);
}

void CEncryptStatus::OnOK()
{
	// if the user press Enter, do not close the dialog
	// CDialog::OnOK();
}


DWORD WINAPI ThreadEncryptFile(LPVOID pIn)
{
   THREAD_PARAM *pThreadParam;

   g_bThreadFinished = FALSE;

   pThreadParam = (THREAD_PARAM *)pIn;

   if( FALSE == EncryptSelection(g_EncryptInfo) )
   {
	   TRACE(L"\nEncryptSelection error in ThreadEncryptFile.");
       PostMessage(pThreadParam->hParentWnd,WM_CLOSE,0,0);
       g_bThreadFinished = TRUE;
	   g_bEncryptSucceed = FALSE;
	   return FALSE;
   }
   else
   {
	   g_bEncryptSucceed = TRUE;

	   // copy file
	   TCHAR buf[MAX_PATH];
	   memset(buf,0,sizeof(buf));
	   GetModuleFileName(NULL,buf,MAX_PATH);
	   GetLongPathName(buf, buf, MAX_PATH);
	   CString modulePath = buf;
	   CString sourceExePath = modulePath.Left(modulePath.ReverseFind(L'\\')) + L"\\sefb.dll";
	   CString targetPath = g_EncryptInfo.m_strTarget;
	   CString tempExePath = targetPath + SELF_EXTRACTING_TEMP_EXTENSION;
	   BOOL operationResult = FALSE;
	   HANDLE hTempFile;
	   HANDLE hImage;
	   do {
		   if (!CopyFile(sourceExePath, tempExePath, FALSE)) {
			   break;
		   }
		   SetFileAttributes(tempExePath, FILE_ATTRIBUTE_HIDDEN);
		   hTempFile = CreateFile(tempExePath,GENERIC_WRITE, NULL, NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		   if (hTempFile == INVALID_HANDLE_VALUE) {
			   break;
		   }
		   hImage = CreateFile(targetPath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		   if (hImage == INVALID_HANDLE_VALUE) {
			   break;
		   }
		   LARGE_INTEGER address;
		   LARGE_INTEGER size;
		   DWORD dwTemp;
		   size.LowPart = GetFileSize(hImage, &dwTemp);
		   size.HighPart = dwTemp;
		   address.LowPart = GetFileSize(hTempFile, &dwTemp);
		   address.HighPart = dwTemp;

		   char chBuf[1024*1024];
		   ZeroMemory(chBuf, sizeof(chBuf));
		   DWORD dwRead = 0;
		   DWORD dwWrite = 0;
		   LARGE_INTEGER remainToRead;
		   remainToRead.QuadPart = size.QuadPart;
		   LARGE_INTEGER totalRead;
		   totalRead.QuadPart = 0;
		   DWORD dwToRead = 0;
		   if (remainToRead.QuadPart > sizeof(chBuf)) {
			   dwToRead = sizeof(chBuf);
		   }
		   else {
			   dwToRead = remainToRead.QuadPart;
		   } 
		   SetFilePointer(hImage, 0, 0, FILE_BEGIN);
		   SetFilePointer(hTempFile, 0, 0, FILE_END);
		   while (dwToRead != 0
			   && ReadFile(hImage, chBuf, dwToRead, &dwRead, 0)
			   && dwToRead == dwRead){
				   totalRead.QuadPart += dwRead;
				   remainToRead.QuadPart -= dwRead;
				   if (WriteFile(hTempFile, chBuf, dwToRead, &dwWrite, 0)
					   && dwWrite == dwToRead) {
						   if (remainToRead.QuadPart > sizeof(chBuf)) {
							   dwToRead = sizeof(chBuf);
						   }
						   else {
							   dwToRead = remainToRead.QuadPart;
						   } 
						   dwRead = dwWrite = 0;
						   ZeroMemory(chBuf, sizeof(chBuf));
				   }
				   else {
					   break;
				   }
		   }
		   if (dwToRead == 0) {
			   if (WriteFile(hTempFile, SELF_EXTRACTING_IDENTITY, IMAGE_IDENTITY_SIZE, &dwTemp, 0)
				   && WriteFile(hTempFile, &address.QuadPart, sizeof(LARGE_INTEGER), &dwTemp, 0)
				   && WriteFile(hTempFile, &size.QuadPart, sizeof(LARGE_INTEGER), &dwTemp, 0)
				   && WriteFile(hTempFile, SELF_EXTRACTING_IDENTITY, IMAGE_IDENTITY_SIZE, &dwTemp, 0)) {
					   operationResult = TRUE;
			   }
			   else {
				   operationResult = FALSE;
			   }
		   }
	   } while (0);
	   CloseHandle(hTempFile);
	   CloseHandle(hImage);

	   if (operationResult) {
		   DeleteFile(targetPath);
		   MoveFile(tempExePath, targetPath);
		   g_bEncryptSucceed = TRUE;
	   }
	   else {
		   DeleteFile(tempExePath);
		   DeleteFile(targetPath);
		   TRACE(L"\nEncryptSelection error in ThreadEncryptFile.");
		   PostMessage(pThreadParam->hParentWnd,WM_CLOSE,0,0);
		   g_bThreadFinished = TRUE;
		   g_bEncryptSucceed = FALSE;
		   return FALSE;
	   }
   }

    PostMessage(pThreadParam->hParentWnd,WM_CLOSE,0,0);
    g_bThreadFinished = TRUE;
    return TRUE;
}
