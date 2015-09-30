// DelProc.cpp : implementation file
//

#include "stdafx.h"
#include "DelFile.h"
#include "DelProc.h"
#include "Fun.h"
#include "atlbase.h"
#include "windows.h"

#define MAX_KEY_LENGTH 255

extern WCHAR		g_szDir[MAX_PATH][MAX_PATH];
extern int			g_nSelItem;
extern __int64		g_nTotalSize;
extern __int64		g_nDelSize;
extern int			g_nDelMethod;
extern BOOL			g_bSave;
extern WCHAR		g_szSaveDir[MAX_PATH];
extern BOOL         g_bBeepOff;
extern BOOL         g_bDeleteObjMark;
/////////////////////////////////////////////////////////////////////////////
// CDelProc dialog


CDelProc::CDelProc(CWnd* pParent /*=NULL*/)
	: CDialog(CDelProc::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDelProc)
	//}}AFX_DATA_INIT
}


void CDelProc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDelProc)
	DDX_Control(pDX, IDC_DEL_STATUS, m_Status);
	DDX_Control(pDX, IDC_BACK_FILE_NAME, m_BackupFile);
	DDX_Control(pDX, IDCANCEL, m_CancelBut);
	DDX_Control(pDX, IDOK, m_OkBut);
	DDX_Control(pDX, IDC_DEL_PROGRESS, m_Progress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDelProc, CDialog)
	//{{AFX_MSG_MAP(CDelProc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDelProc message handlers

BOOL CDelProc::OnInitDialog() 
{
	DWORD		threadID;
	CDialog::OnInitDialog();
	
	m_Progress.SetRange (0,100);
	m_Progress.SetPos(0);
	
	m_OkBut.ShowWindow (SW_HIDE);
	UpdateData(FALSE);

	m_bDeleteObjMark = g_bDeleteObjMark;
	m_Thread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)CDelProc::ThreadProc ,
							(LPVOID)this,0,&threadID);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

DWORD WINAPI CDelProc::ThreadProc(CDelProc *pDlg)
{
	if (pDlg->m_bDeleteObjMark)
	{
		pDlg->DoDeleteMasterDelFileInRecycled();
	}
	else
	{
		pDlg->DoDeleteMasterDel();
	}
	return 0;
}


BOOL CDelProc::DoDeleteMasterDel()
{
	CTime StartTime,EndTime;
	StartTime = CTime::GetCurrentTime();
	
	int			i;
	BOOL        bResult = FALSE;
	CString		csText,csCaption;

	g_nDelSize = 0;

	csCaption.LoadString (IDS_PRODUCT_NAME);
	m_pBuf = (BYTE*)malloc(DM_DEL_SIZE);
	if(!m_pBuf) 
	{
		csText.LoadString (IDS_NO_ENOUGH_MEM);
		MessageBox(csText,csCaption,MB_OK|MB_ICONSTOP);

		EndTime = CTime::GetCurrentTime();
		for (int i = 0; i < g_nSelItem;i ++)
		{
			WriteLog(g_szDir[i],StartTime,EndTime,FALSE);
		}
		return FALSE;
	}

	m_CancelBut.ShowWindow (SW_HIDE);
	csText.LoadString(IDS_DELETE_STATUS);
	m_Status.SetWindowText(csText);

	g_nDelSize = 0;
	switch(g_nDelMethod)
	{
	case 0:
		InitDelBuf(m_pBuf,0,0,0,FALSE,TRUE);
		bResult = DelAll(TRUE);
		break;
	case 1:
		InitDelBuf(m_pBuf,0XFF,0,0,FALSE,TRUE);
		bResult = DelAll(TRUE);
		break;
	case 2:
		InitDelBuf(m_pBuf,0,0,0,TRUE,TRUE);
		bResult = DelAll(TRUE);
		break;
	case 3:
		g_nTotalSize = g_nTotalSize *3;
		for(i=0;i<3;i++)
		{
			switch(i)
			{
			case 0:
			case 1:
				InitDelBuf(m_pBuf,0,0,0,TRUE,TRUE);
				break;
			case 2:
				InitDelBuf(m_pBuf,0,0,0,FALSE,TRUE);
				break;
			}
			if(i==2)
			{
				if(!DelAll(TRUE))
				{
					break;
				}
			}
			else
			{
				if(!DelAll(FALSE))
					break;
			}
		}
		bResult = TRUE;
		break;
	case 4:
		g_nTotalSize = g_nTotalSize *4;
		for(i=0;i<4;i++)
		{
			switch(i)
			{
			case 0:
			case 2:
				InitDelBuf(m_pBuf,0,0,0,FALSE,TRUE);
				break;
			case 1:
			case 3:
				InitDelBuf(m_pBuf,0xFF,0,0,FALSE,TRUE);
				break;
			}
			if(i==3)
			{
				if(!DelAll(TRUE))
					break;
			}
			else
			{
				if(!DelAll(FALSE))
					break;
			}
		}
		bResult = TRUE;
		break;
	case 5:
		g_nTotalSize = g_nTotalSize *3;
		for(i=0;i<3;i++)
		{
			switch(i)
			{
			case 0:
				InitDelBuf(m_pBuf,0,0,0,FALSE,TRUE);
				break;
			case 1:
				InitDelBuf(m_pBuf,0XFF,0,0,FALSE,TRUE);
				break;
			case 2:
				InitDelBuf(m_pBuf,0X35,0XCA,0X97,FALSE,FALSE);
				break;
			}
			if(i==2)
			{
				if(!DelAll(TRUE)) break;
			}
			else
			{
				if(!DelAll(FALSE)) break;
			}
		}
		bResult = TRUE;
		break;
	case 6:
		g_nTotalSize = g_nTotalSize *7;
		for(i=0;i<7;i++)
		{
			switch(i)
			{
			case 0:
			case 2:
			case 4:
				InitDelBuf(m_pBuf,0,0,0,FALSE,TRUE);
				break;
			case 1:
			case 3:
			case 5:
				InitDelBuf(m_pBuf,0XFF,0,0,FALSE,TRUE);
				break;
			case 6:
				InitDelBuf(m_pBuf,0X35,0XCA,0X97,FALSE,FALSE);
				break;
			}
			if(i==6)
			{
				if(!DelAll(TRUE)) break;
			}
			else
			{
				if(!DelAll(FALSE)) break;
			}
		}
		bResult = TRUE;
		break;
	case 7:
		g_nTotalSize = g_nTotalSize *35;
		for(i=0;i<35;i++)
		{
			switch(i)
			{
			case 0:
			case 1:
			case 2:
			case 3:
			case 31:
			case 32:
			case 33:
			case 34:
				InitDelBuf(m_pBuf,0,0,0,TRUE,TRUE);
				break;
			case 4:
				InitDelBuf(m_pBuf,0X55,0,0,FALSE,TRUE);
				break;
			case 5:
				InitDelBuf(m_pBuf,0XAA,0,0,FALSE,TRUE);
				break;
			case 6:
			case 25:
				InitDelBuf(m_pBuf,0X92,0X49,0X24,FALSE,FALSE);
				break;
			case 7:
			case 26:
				InitDelBuf(m_pBuf,0X49,0X24,0X92,FALSE,FALSE);
				break;
			case 8:
			case 27:
				InitDelBuf(m_pBuf,0X24,0X92,0X49,FALSE,FALSE);
				break;
			case 9:
				InitDelBuf(m_pBuf,0,0,0,FALSE,TRUE);
				break;
			case 10:
				InitDelBuf(m_pBuf,0X11,0,0,FALSE,TRUE);
				break;
			case 11:
				InitDelBuf(m_pBuf,0X22,0,0,FALSE,TRUE);
				break;
			case 12:
				InitDelBuf(m_pBuf,0X33,0,0,FALSE,TRUE);
				break;
			case 13:
				InitDelBuf(m_pBuf,0X44,0,0,FALSE,TRUE);
				break;
			case 14:
				InitDelBuf(m_pBuf,0X55,0,0,FALSE,TRUE);
				break;
			case 15:
				InitDelBuf(m_pBuf,0X66,0,0,FALSE,TRUE);
				break;
			case 16:
				InitDelBuf(m_pBuf,0x77,0,0,FALSE,TRUE);
				break;
			case 17:
				InitDelBuf(m_pBuf,0X88,0,0,FALSE,TRUE);
				break;
			case 18:
				InitDelBuf(m_pBuf,0X99,0,0,FALSE,TRUE);
				break;
			case 19:
				InitDelBuf(m_pBuf,0XAA,0,0,FALSE,TRUE);
				break;
			case 20:
				InitDelBuf(m_pBuf,0XBB,0,0,FALSE,TRUE);
				break;
			case 21:
				InitDelBuf(m_pBuf,0XCC,0,0,FALSE,TRUE);
				break;
			case 22:
				InitDelBuf(m_pBuf,0XDD,0,0,FALSE,TRUE);
				break;
			case 23:
				InitDelBuf(m_pBuf,0XEE,0,0,FALSE,TRUE);
				break;
			case 24:
				InitDelBuf(m_pBuf,0XFF,0,0,FALSE,TRUE);
				break;
			case 28:
				InitDelBuf(m_pBuf,0X6D,0XB6,0XDB,FALSE,FALSE);
				break;
			case 29:
				InitDelBuf(m_pBuf,0XB6,0XDB,0X6D,FALSE,FALSE);
				break;
			case 30:
				InitDelBuf(m_pBuf,0XDB,0X6D,0XB6,FALSE,FALSE);
				break;
			}
			if(i==34)
			{
				if(!DelAll(TRUE)) break;
			}
			else
			{
				if(!DelAll(FALSE)) break;
			}
		}
		bResult = TRUE;
		break;
	}
	free(m_pBuf);

	csText.LoadString(IDS_DELETE_SUCCESS);
	m_Status.SetWindowText(csText);
	m_CancelBut.ShowWindow (SW_HIDE);
	m_OkBut.ShowWindow (SW_SHOW);
	csCaption.LoadString (IDS_DEL_FINISH_CAPTION);
	SetWindowText(csCaption);

	EndTime = CTime::GetCurrentTime();
	for (int i = 0; i < g_nSelItem;i ++)
	{
		WriteLog(g_szDir[i],StartTime,EndTime,bResult);
	}
	if (!g_bBeepOff)
	{
		Beep(2000,500);
		Beep(2000,500);
		Beep(2000,500);
	}

	return bResult;
}

BOOL CDelProc::DelAll(BOOL bDel)
{
	int			i;
	for(i=0;i<g_nSelItem;i++)
	{
		if(!DelDirAndFiles(g_szDir[i],bDel,g_szSaveDir,g_bSave))
		{
			if (!g_bBeepOff)
			{
				Beep(1500,500);
				Beep(1500,500);
				Beep(1500,500);
				Beep(1500,500);
				Beep(1500,500);
			}
			return FALSE;
		}
	}
	m_BackupFile.SetWindowText(L"");
	g_bSave = FALSE;
	return TRUE;
}

BOOL CDelProc::DelDirAndFiles(WCHAR  *pszDir , BOOL bDel,WCHAR  *pszSaveDir,BOOL bSave)
{
    WIN32_FIND_DATAW    FindData;
	HANDLE              hFind   ;
	DWORD				dwAttrs;
	int					i;
	WCHAR				*p;
	int					nDirEnd,nSaveDirEnd;
	WCHAR				szDirOrFileName[MAX_PATH];
	WCHAR				szSaveDir[MAX_PATH];
	WCHAR				szSaveDirOrFileName[MAX_PATH];
	WCHAR				szDirName[MAX_PATH];
	CString             strText;

	hFind = FindFirstFileW(pszDir,&FindData);
	if (INVALID_HANDLE_VALUE == hFind)	return FALSE;
  	
	if(bSave)
	{
		wcscpy( szSaveDirOrFileName , pszSaveDir) ;
		wcscat( szSaveDirOrFileName , L"\\" ) ;
		nSaveDirEnd = wcslen( szSaveDirOrFileName);
		p = wcsrchr(pszDir,L'\\');
		p++;
		wcscpy( &szSaveDirOrFileName[nSaveDirEnd] ,p);
	}

	if ( !(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
	{
		FindClose(hFind);
		dwAttrs = GetFileAttributesW(pszDir);
		if(bSave)
		{
			strText.Format(IDS_BACKUP_FILE_NAME,p);
			m_BackupFile.SetWindowText(strText);
			CopyFileW(pszDir,szSaveDirOrFileName,FALSE);
			SetFileAttributesW(szSaveDirOrFileName,dwAttrs);
		}
		SetFileAttributesW(pszDir,FILE_ATTRIBUTE_NORMAL);
		if(!DoDelFile(pszDir,bDel))
		{
			g_nDelSize += FindData.nFileSizeLow;
			g_nDelSize += FindData.nFileSizeHigh * 0x100000000;
			SetFileAttributesW(pszDir,dwAttrs);
			if (bDel)
			{
				CTime  CurrentTime;
				CurrentTime = CTime::GetCurrentTime();
				WriteLog(pszDir,CurrentTime,CurrentTime,FALSE);
			}
			return FALSE;
		}
		return TRUE;
	}
	FindClose(hFind);

	if(bSave)
	{
		wcscat( szSaveDirOrFileName , L"\\" ) ;
		nSaveDirEnd = wcslen( szSaveDirOrFileName);
		CreateDirectoryW(szSaveDirOrFileName,NULL);
	}

  	wcscpy( szDirOrFileName , pszDir) ;
    wcscat( szDirOrFileName , L"\\" ) ;
    nDirEnd = wcslen( szDirOrFileName);
    wcscpy( &szDirOrFileName[nDirEnd] , L"*.*");
	
	hFind = FindFirstFileW(szDirOrFileName ,&FindData);
	
	if ( INVALID_HANDLE_VALUE == hFind )
		return  FALSE ;
	
	do
    {
		if (wcscmp(FindData.cFileName,L".") && 
			wcscmp(FindData.cFileName,L".." ))
		{
			wcscpy( &szDirOrFileName[nDirEnd] ,FindData.cFileName);
			//if(bSave)

			if ( !(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) 
			{
				dwAttrs = GetFileAttributesW(szDirOrFileName);
				if(bSave)
				{
					strText.Format(IDS_BACKUP_FILE_NAME,FindData.cFileName);
					m_BackupFile.SetWindowText(strText);
					wcscpy( &szSaveDirOrFileName[nSaveDirEnd] ,FindData.cFileName);
					CopyFileW(szDirOrFileName,szSaveDirOrFileName,FALSE);
					SetFileAttributesW(szSaveDirOrFileName,dwAttrs);
				}
				SetFileAttributesW(szDirOrFileName,FILE_ATTRIBUTE_NORMAL);
				if(!DoDelFile(szDirOrFileName,bDel))
				{
					g_nDelSize += FindData.nFileSizeLow;
					g_nDelSize += FindData.nFileSizeHigh * 0x100000000;
					SetFileAttributesW(szDirOrFileName,dwAttrs);
					if (bDel)
					{
						CTime  CurrentTime;
						CurrentTime = CTime::GetCurrentTime();
						WriteLog(szDirOrFileName,CurrentTime,CurrentTime,FALSE);
					}
				}
			}
			else
			{
				if(bSave)
				{
					wcscpy(szSaveDir,szSaveDirOrFileName);
					szSaveDir[nSaveDirEnd-1] = 0;
				}
				if (!DelDirAndFiles(szDirOrFileName,bDel,szSaveDir,bSave))
				{
					  FindClose( hFind );
					  return FALSE ;
				}
			}
		}
	} while( FindNextFile( hFind ,&FindData));
	FindClose( hFind);
	SetFileAttributes(pszDir,FILE_ATTRIBUTE_NORMAL);
	if(bDel)
	{
		RemoveDirectoryW(pszDir);

		wcscpy(szDirName,pszDir);
		p = wcsrchr(szDirName,L'\\');
		p++;
		wcscpy(p,RENAME_STRING);
		for(i=65;i<=90;i++)
		{
			*p = (WCHAR)i;
			CreateDirectoryW(szDirName,NULL);
		}
		p++;
		for(i=97;i<=122;i++)
		{
			*p = (WCHAR)i;
			CreateDirectoryW(szDirName,NULL);
		}
		p --;
		wcscpy(p,RENAME_STRING);
		for(i=65;i<=90;i++)
		{
			*p = (WCHAR)i;
			RemoveDirectoryW(szDirName);
		}
		p++;
		for(i=97;i<=122;i++)
		{
			*p = (WCHAR)i;
			RemoveDirectoryW(szDirName);
		}
	}
    return  TRUE;
}

BOOL CDelProc::DoDelFile(WCHAR  *pszDir,BOOL bDel)
{
	HANDLE		hFile;
	DWORD		i,j;
	DWORD		dwRWTimes;
	DWORD		dwRW;
	int			nPresent;
	WCHAR		szFileName[MAX_PATH];
	WCHAR		*p;
	DWORD       dwFileSizeLo,dwFileSizeHigh;

	hFile = CreateFileW(pszDir, 
						GENERIC_READ|GENERIC_WRITE, 
						FILE_SHARE_READ | FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING, 
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if (hFile == INVALID_HANDLE_VALUE) 
		return FALSE;

	dwFileSizeLo = GetFileSize(hFile,&dwFileSizeHigh);

	i = dwFileSizeLo / DM_DEL_SIZE + dwFileSizeHigh * 0x4000;
	j = dwFileSizeLo % DM_DEL_SIZE; 

	for(dwRWTimes=0;dwRWTimes<i;dwRWTimes++)
	{
		WriteFile(hFile,m_pBuf,DM_DEL_SIZE,&dwRW,NULL);
		g_nDelSize += DM_DEL_SIZE;
		if (!g_nTotalSize) nPresent = 100;
		else nPresent = (int)(g_nDelSize*100/g_nTotalSize);
		m_Progress.SetPos(nPresent);
	}
	if(j)
	{
		WriteFile(hFile,m_pBuf,j,&dwRW,NULL);
		g_nDelSize += j;
		if (!g_nTotalSize) nPresent = 100;
		else nPresent = (int)(g_nDelSize*100/g_nTotalSize);
		m_Progress.SetPos(nPresent);
	}
	FlushFileBuffers(hFile);
	CloseHandle(hFile);
	if(bDel) 
	{
		wcscpy(szFileName,pszDir);
		p = wcsrchr(szFileName,L'\\');
		p++;
		wcscpy(p,RENAME_STRING);
		_trename(pszDir,szFileName);
		DeleteFile(szFileName);
		for(i=65;i<=90;i++)
		{
			*p = (WCHAR)i;
			hFile = CreateFileW(szFileName, 
						GENERIC_READ|GENERIC_WRITE, 
						FILE_SHARE_READ |FILE_SHARE_WRITE,
						NULL,
						OPEN_ALWAYS, 
						FILE_ATTRIBUTE_NORMAL,
						NULL);
			CloseHandle(hFile);
		}
		p++;
		for(i=97;i<=122;i++)
		{
			*p = (WCHAR)i;
			hFile = CreateFileW(szFileName, 
						GENERIC_READ|GENERIC_WRITE, 
						FILE_SHARE_READ |FILE_SHARE_WRITE,
						NULL,
						OPEN_ALWAYS, 
						FILE_ATTRIBUTE_NORMAL,
						NULL);
			CloseHandle(hFile);
		}
		p--;
		wcscpy(p,RENAME_STRING);
		for(i=65;i<=90;i++)
		{
			*p = (WCHAR)i;
			DeleteFile(szFileName);
		}
		p++;
		for(i=97;i<=122;i++)
		{
			*p = (WCHAR)i;
			DeleteFile(szFileName);
		}
	}

	return TRUE;
}

BOOL CDelProc::DoDeleteMasterDelFileInRecycled()
{
	int			i;
	BOOL        bResult = FALSE;
	CString		csText,csCaption;

	g_nDelSize = 0;

	csCaption.LoadString (IDS_PRODUCT_NAME);
	m_pBuf = (BYTE*)malloc(DM_DEL_SIZE);
	if(!m_pBuf) 
	{
		csText.LoadString (IDS_NO_ENOUGH_MEM);
		MessageBox(csText,csCaption,MB_OK|MB_ICONSTOP);
		return FALSE;
	}

	m_CancelBut.ShowWindow (SW_HIDE);
	csText.LoadString(IDS_DELETE_STATUS);
	m_Status.SetWindowText(csText);

	g_nDelSize = 0;
	
	switch(g_nDelMethod)
	{
	case 0:
		InitDelBuf(m_pBuf,0,0,0,FALSE,TRUE);
		bResult = DelAllInRecycled(TRUE);
		break;
	case 1:
		InitDelBuf(m_pBuf,0XFF,0,0,FALSE,TRUE);
		bResult = DelAllInRecycled(TRUE);
		break;
	case 2:
		InitDelBuf(m_pBuf,0,0,0,TRUE,TRUE);
		bResult = DelAllInRecycled(TRUE);
		break;
	case 3:
		g_nTotalSize = g_nTotalSize *3;
		for(i=0;i<3;i++)
		{
			switch(i)
			{
			case 0:
			case 1:
				InitDelBuf(m_pBuf,0,0,0,TRUE,TRUE);
				break;
			case 2:
				InitDelBuf(m_pBuf,0,0,0,FALSE,TRUE);
				break;
			}
			if(i==2)
			{
				if(!DelAllInRecycled(TRUE))
				{
					break;
				}
			}
			else
			{
				if(!DelAllInRecycled(FALSE))
					break;
			}
		}
		bResult = TRUE;
		break;
	case 4:
		g_nTotalSize = g_nTotalSize *4;
		for(i=0;i<4;i++)
		{
			switch(i)
			{
			case 0:
			case 2:
				InitDelBuf(m_pBuf,0,0,0,FALSE,TRUE);
				break;
			case 1:
			case 3:
				InitDelBuf(m_pBuf,0xFF,0,0,FALSE,TRUE);
				break;
			}
			if(i==3)
			{
				if(!DelAllInRecycled(TRUE))
					break;
			}
			else
			{
				if(!DelAllInRecycled(FALSE))
					break;
			}
		}
		bResult = TRUE;
		break;
	case 5:
		g_nTotalSize = g_nTotalSize *3;
		for(i=0;i<3;i++)
		{
			switch(i)
			{
			case 0:
				InitDelBuf(m_pBuf,0,0,0,FALSE,TRUE);
				break;
			case 1:
				InitDelBuf(m_pBuf,0XFF,0,0,FALSE,TRUE);
				break;
			case 2:
				InitDelBuf(m_pBuf,0X35,0XCA,0X97,FALSE,FALSE);
				break;
			}
			if(i==2)
			{
				if(!DelAllInRecycled(TRUE)) break;
			}
			else
			{
				if(!DelAllInRecycled(FALSE)) break;
			}
		}
		bResult = TRUE;
		break;
	case 6:
		g_nTotalSize = g_nTotalSize *7;
		for(i=0;i<7;i++)
		{
			switch(i)
			{
			case 0:
			case 2:
			case 4:
				InitDelBuf(m_pBuf,0,0,0,FALSE,TRUE);
				break;
			case 1:
			case 3:
			case 5:
				InitDelBuf(m_pBuf,0XFF,0,0,FALSE,TRUE);
				break;
			case 6:
				InitDelBuf(m_pBuf,0X35,0XCA,0X97,FALSE,FALSE);
				break;
			}
			if(i==6)
			{
				if(!DelAllInRecycled(TRUE)) break;
			}
			else
			{
				if(!DelAllInRecycled(FALSE)) break;
			}
		}
		bResult = TRUE;
		break;
	case 7:
		g_nTotalSize = g_nTotalSize *35;
		for(i=0;i<35;i++)
		{
			switch(i)
			{
			case 0:
			case 1:
			case 2:
			case 3:
			case 31:
			case 32:
			case 33:
			case 34:
				InitDelBuf(m_pBuf,0,0,0,TRUE,TRUE);
				break;
			case 4:
				InitDelBuf(m_pBuf,0X55,0,0,FALSE,TRUE);
				break;
			case 5:
				InitDelBuf(m_pBuf,0XAA,0,0,FALSE,TRUE);
				break;
			case 6:
			case 25:
				InitDelBuf(m_pBuf,0X92,0X49,0X24,FALSE,FALSE);
				break;
			case 7:
			case 26:
				InitDelBuf(m_pBuf,0X49,0X24,0X92,FALSE,FALSE);
				break;
			case 8:
			case 27:
				InitDelBuf(m_pBuf,0X24,0X92,0X49,FALSE,FALSE);
				break;
			case 9:
				InitDelBuf(m_pBuf,0,0,0,FALSE,TRUE);
				break;
			case 10:
				InitDelBuf(m_pBuf,0X11,0,0,FALSE,TRUE);
				break;
			case 11:
				InitDelBuf(m_pBuf,0X22,0,0,FALSE,TRUE);
				break;
			case 12:
				InitDelBuf(m_pBuf,0X33,0,0,FALSE,TRUE);
				break;
			case 13:
				InitDelBuf(m_pBuf,0X44,0,0,FALSE,TRUE);
				break;
			case 14:
				InitDelBuf(m_pBuf,0X55,0,0,FALSE,TRUE);
				break;
			case 15:
				InitDelBuf(m_pBuf,0X66,0,0,FALSE,TRUE);
				break;
			case 16:
				InitDelBuf(m_pBuf,0x77,0,0,FALSE,TRUE);
				break;
			case 17:
				InitDelBuf(m_pBuf,0X88,0,0,FALSE,TRUE);
				break;
			case 18:
				InitDelBuf(m_pBuf,0X99,0,0,FALSE,TRUE);
				break;
			case 19:
				InitDelBuf(m_pBuf,0XAA,0,0,FALSE,TRUE);
				break;
			case 20:
				InitDelBuf(m_pBuf,0XBB,0,0,FALSE,TRUE);
				break;
			case 21:
				InitDelBuf(m_pBuf,0XCC,0,0,FALSE,TRUE);
				break;
			case 22:
				InitDelBuf(m_pBuf,0XDD,0,0,FALSE,TRUE);
				break;
			case 23:
				InitDelBuf(m_pBuf,0XEE,0,0,FALSE,TRUE);
				break;
			case 24:
				InitDelBuf(m_pBuf,0XFF,0,0,FALSE,TRUE);
				break;
			case 28:
				InitDelBuf(m_pBuf,0X6D,0XB6,0XDB,FALSE,FALSE);
				break;
			case 29:
				InitDelBuf(m_pBuf,0XB6,0XDB,0X6D,FALSE,FALSE);
				break;
			case 30:
				InitDelBuf(m_pBuf,0XDB,0X6D,0XB6,FALSE,FALSE);
				break;
			}
			if(i==34)
			{
				if(!DelAllInRecycled(TRUE)) break;
			}
			else
			{
				if(!DelAllInRecycled(FALSE)) break;
			}
		}
		bResult = TRUE;
		break;
	}
	free(m_pBuf);

	csText.LoadString(IDS_DELETE_SUCCESS);
	m_Status.SetWindowText(csText);
	m_CancelBut.ShowWindow (SW_HIDE);
	m_OkBut.ShowWindow (SW_SHOW);
	csCaption.LoadString (IDS_DEL_FINISH_CAPTION);
	SetWindowText(csCaption);
	if (!g_bBeepOff)
	{
		Beep(2000,500);
		Beep(2000,500);
		Beep(2000,500);
	}
	return bResult;
}

BOOL CDelProc::DelAllInRecycled(BOOL bDel)
{
	WCHAR driverLetters[MAX_PATH];
	WCHAR diskLetter[4] = L"C:\\";
	WCHAR szRecyclePath[MAX_PATH];
	int n;
	PSID pSID = NULL;
	TCHAR szSID[MAX_PATH];
	WCHAR  FileSystemNameBuffer[10];
	DWORD MaximumConponentLenght;
	DWORD FileSystemFlags;

	GetLogicalDriveStrings(MAX_PATH,driverLetters);
	n = sizeof(driverLetters);

	if (GetUserSid(&pSID))
	{
		//get the hidden key name
		GetSidString(pSID, szSID);
	}

	for (int i = 0;i < n;i += 4)
	{
		if (driverLetters[i] == NULL)
		{
			break;
		}

		diskLetter[0] = driverLetters[i];
		
		if (GetDriveType(&driverLetters[i]) == DRIVE_FIXED)
		{
			GetVolumeInformation(&driverLetters[i],NULL,0,NULL,
				&MaximumConponentLenght,&FileSystemFlags,
				FileSystemNameBuffer,10);
			wcscpy(szRecyclePath,diskLetter);

				if (!wcscmp(FileSystemNameBuffer,L"NTFS"))
			{
				wcscat(szRecyclePath,L"RECYCLER\\");
				wcscat(szRecyclePath,szSID);
			}
			else
			{
				wcscat(szRecyclePath,L"Recycled");
			}

			DeleteFileInRecycled(szRecyclePath,bDel);
		}
	}

	return TRUE;
}

BOOL CDelProc::DeleteFileInRecycled(LPTSTR pszDir, BOOL bDel)
{
	WCHAR tempFilePath[MAX_PATH];
	WCHAR findFileName[MAX_PATH];
	WCHAR szDirName[MAX_PATH];
	WCHAR *p;
	int i;
	BOOL bIsFound;
	CFileFind finder;

	swprintf(tempFilePath,L"%s\\*.*",pszDir);
	bIsFound = (BOOL)finder.FindFile(tempFilePath);

	while (bIsFound)
	{
		bIsFound = (BOOL)finder.FindNextFile();

		if (!finder.IsDots())
		{
			wcscpy(findFileName,finder.GetFileName().GetBuffer(0));

			if (wcscmp(findFileName,L"desktop.ini") && wcscmp(findFileName,L"INFO2"))
			{
				if (finder.IsDirectory())
				{
					WCHAR tempDirectory[MAX_PATH];
					swprintf(tempDirectory,L"%s\\%s",pszDir,findFileName);
					DeleteFileInRecycled(tempDirectory,bDel);
				}
				else
				{
					WCHAR tempFileName[MAX_PATH];
					swprintf(tempFileName,L"%s\\%s",pszDir,findFileName);
					DoDeleteFileInRecycled(tempFileName,bDel);
				}
			}
		}
	}

	finder.Close();

	SetFileAttributes(pszDir,FILE_ATTRIBUTE_NORMAL);
	
	if(bDel)
	{
		RemoveDirectory(pszDir);

		wcscpy(szDirName,pszDir);
		p = wcsrchr(szDirName,L'\\');
		p++;
		wcscpy(p,RENAME_STRING);
		for(i=65;i<=90;i++)
		{
			*p = (WCHAR)i;
			CreateDirectory(szDirName,NULL);
		}
		p++;
		for(i=97;i<=122;i++)
		{
			*p = (WCHAR)i;
			CreateDirectory(szDirName,NULL);
		}
		p --;
		wcscpy(p,RENAME_STRING);
		for(i=65;i<=90;i++)
		{
			*p = (WCHAR)i;
			RemoveDirectory(szDirName);
		}
		p++;
		for(i=97;i<=122;i++)
		{
			*p = (WCHAR)i;
			RemoveDirectory(szDirName);
		}
	}

	return TRUE;
}

BOOL CDelProc::DoDeleteFileInRecycled(LPTSTR pszDir, BOOL bDel)
{
	HANDLE hFile;
	WCHAR *p;
	int nPresent;
	DWORD i,j;
	DWORD dwRWTimes;
	DWORD dwRW;
	DWORD dwFileSizeLow,dwFileSizeHigh;
	WCHAR szFileName[MAX_PATH];

	hFile = CreateFile(pszDir,
		               GENERIC_READ|GENERIC_WRITE,
					   FILE_SHARE_READ|FILE_SHARE_WRITE,
					   NULL,
					   OPEN_EXISTING,
					   FILE_ATTRIBUTE_NORMAL,
					   NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	dwFileSizeLow = GetFileSize(hFile,&dwFileSizeHigh);
	GetDirFileSize(pszDir);
	i = dwFileSizeLow / DM_DEL_SIZE + dwFileSizeHigh * 0x4000;
	j = dwFileSizeLow % DM_DEL_SIZE;
	
	for(dwRWTimes=0;dwRWTimes<i;dwRWTimes++)
	{
		WriteFile(hFile,m_pBuf,DM_DEL_SIZE,&dwRW,NULL);
		g_nDelSize += DM_DEL_SIZE;
		if (!g_nTotalSize) nPresent = 100;
		else nPresent = (int)(g_nDelSize*100/g_nTotalSize);
		m_Progress.SetPos(nPresent);
	}
	if(j)
	{
		WriteFile(hFile,m_pBuf,j,&dwRW,NULL);
		g_nDelSize += j;
		if (!g_nTotalSize) nPresent = 100;
		else nPresent = (int)(g_nDelSize*100/g_nTotalSize);
		m_Progress.SetPos(nPresent);
	}
	FlushFileBuffers(hFile);
	CloseHandle(hFile);

	if(bDel) 
	{
		wcscpy(szFileName,pszDir);
		p = wcsrchr(szFileName,L'\\');
		p++;
		wcscpy(p,RENAME_STRING);
		_trename(pszDir,szFileName);
		DeleteFile(szFileName);
		for(i=65;i<=90;i++)
		{
			*p = (WCHAR)i;
			hFile = CreateFile(szFileName, 
						GENERIC_READ|GENERIC_WRITE, 
						FILE_SHARE_READ |FILE_SHARE_WRITE,
						NULL,
						OPEN_ALWAYS, 
						FILE_ATTRIBUTE_NORMAL,
						NULL);
			CloseHandle(hFile);
		}
		p++;
		for(i=97;i<=122;i++)
		{
			*p = (WCHAR)i;
			hFile = CreateFile(szFileName, 
						GENERIC_READ|GENERIC_WRITE, 
						FILE_SHARE_READ |FILE_SHARE_WRITE,
						NULL,
						OPEN_ALWAYS, 
						FILE_ATTRIBUTE_NORMAL,
						NULL);
			CloseHandle(hFile);
		}
		p--;
		wcscpy(p,RENAME_STRING);
		for(i=65;i<=90;i++)
		{
			*p = (WCHAR)i;
			DeleteFile(szFileName);
		}
		p++;
		for(i=97;i<=122;i++)
		{
			*p = (WCHAR)i;
			DeleteFile(szFileName);
		}
	}

	return TRUE;
}


