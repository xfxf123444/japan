#include "stdafx.h"
#include "DelFile.h"
#include "Fun.h"
#include "SelMethod.h"
#include "DelProc.h"
#include "Folder.h"
#include "DelFileDlg.h"
#include <string>
using namespace std;
extern WCHAR		g_szDir[MAX_PATH][MAX_PATH];
extern int			g_nSelItem;
extern __int64		g_nTotalSize;
extern __int64		g_nDelSize;
extern int			g_nDelMethod;
extern DWORD		g_dwFileNumb;
extern BOOL			g_bSave;
extern BOOL         g_bBeepOff;
extern CListCtrl *g_pListView;

BOOL GetTotalFileSize()
{
	int		i;
	//char	szTotalSize[64];

	g_nTotalSize = 0;
	for(i=0;i<g_nSelItem;i++)
	{
		GetDirFileSize(g_szDir[i]);
	}
	//sprintf(szTotalSize,"Total file size: %d",g_nTotalSize);
	//::MessageBox(NULL,szTotalSize,"DeleteMaster",MB_OK);
	return TRUE;
}

BOOL GetDirFileSize(WCHAR  *pszDir)
{
    WIN32_FIND_DATAW    FindData;
	HANDLE              hFind   ;
	int					nDirEnd;
	WCHAR				szDirOrFileName[MAX_PATH];

	hFind = FindFirstFileW(pszDir,&FindData);
	if (INVALID_HANDLE_VALUE == hFind)		return FALSE;
	if ( !(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
	{
		g_nTotalSize += FindData.nFileSizeLow;
		FindClose(hFind);
		return TRUE;
	}
	FindClose(hFind);

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
			wcscmp(FindData.cFileName,L".."))
		{
			wcscpy( &szDirOrFileName[nDirEnd] ,FindData.cFileName);
			if ( !(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) 
			{
				g_nTotalSize += FindData.nFileSizeLow;
			}
			if ( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) 
			{
				GetDirFileSize(szDirOrFileName);
			}
		}
	} while( FindNextFileW( hFind ,&FindData));
    
	FindClose( hFind);
    return  TRUE;
}

void AddDelMethod(CComboBoxEx* pBox,int nCurMethod)
{
	COMBOBOXEXITEM		cbItem;
	int                 nCurSel = -1,i;
	CString				csMethod;

	cbItem.mask			  = CBEIF_TEXT;
	cbItem.iItem		  = 0;
	cbItem.cchTextMax	  = MAX_PATH;
	cbItem.iImage		  = 0;
	cbItem.iSelectedImage = 0;

	for(i = 0; i < 8 ; i ++)
	{
		if (nCurMethod == i) nCurSel = i;

		switch(i)
		{
		case 0:
			csMethod.LoadString (IDS_00_METHOD);
			break;
		case 1:
			csMethod.LoadString (IDS_FF_METHOD);
			break;
		case 2:
			csMethod.LoadString (IDS_RANDOM_METHOD);
			break;
		case 3:
			csMethod.LoadString (IDS_NSA_METHOD);
			break;
		case 4:
			csMethod.LoadString (IDS_OLD_NSA_METHOD);
			break;
		case 5:
			csMethod.LoadString (IDS_DOD_STD_METHOD);
			break;
		case 6:
			csMethod.LoadString (IDS_NATO_METHOD);
			break;
		case 7:
			csMethod.LoadString (IDS_GUTMANN_METHOD);
			break;
		}
		cbItem.pszText		  = csMethod.GetBuffer(0);
		cbItem.iItem  = i;
		pBox->InsertItem(&cbItem);
		pBox->SetItemData(cbItem.iItem,i);
	}
	if (nCurSel != -1)
	{
		pBox->SetCurSel(nCurSel);
		g_nDelMethod = pBox->GetItemData(nCurSel);
	}
}

BOOL InitDelBuf(BYTE *pBuf,BYTE btFir,BYTE btSec,BYTE btThr,BOOL bRand,BOOL bOneByte)
{
	BYTE *pOneBuf;
	int	 i,nRemain;

	if(bRand)
	{
		srand( (unsigned)time( NULL ) );
   		btFir = (BYTE)rand();
	}
	if(bOneByte)
	{
		memset(pBuf,btFir,DM_DEL_SIZE);
	}
	else
	{
		pOneBuf = (BYTE*)malloc(BYTEINSEC);
		if(!pOneBuf) return FALSE;
		for(i=0;i<BYTEINSEC;i++)
		{
			nRemain = i % 3;
			switch(nRemain)
			{
			case 0:
				*(pOneBuf+i) = btFir;
				break;
			case 1:
				*(pOneBuf+i) = btSec;
				break;
			case 2:
				*(pOneBuf+i) = btThr;
				break;
			}
		}
		for(i=0;i<128;i++)
		{
			memcpy(pBuf+BYTEINSEC*i,pOneBuf,BYTEINSEC);
		}
		free(pOneBuf);
	}
	return TRUE;
}

BOOL WriteLog(WCHAR *szFile,CTime StartTime,CTime EndTime,BOOL bSucc)
{
	CString strText,strResult;
	WCHAR    szUserName[MAX_PATH],szLogFile[MAX_PATH];
	HANDLE  hFile;
	WORD    wUnicodeSign = 0;
	ULONG	ulEnter = 0x000a000d;
	ULONG	ulLen = MAX_PATH,dwWrtited;

	GetUserName(szUserName,&ulLen);

	strResult.LoadString(bSucc?IDS_RESULT_SUCC:IDS_RESULT_FAIL);
	strText.Format(IDS_LOG_FORMAT,szFile,szUserName,
					StartTime.GetYear(),StartTime.GetMonth(),StartTime.GetDay(),
					StartTime.GetHour(),StartTime.GetMinute(),StartTime.GetSecond(),
					EndTime.GetYear(),EndTime.GetMonth(),EndTime.GetDay(),
					EndTime.GetHour(),EndTime.GetMinute(),EndTime.GetSecond(),
					g_nDelMethod, strResult);

	////GetSystemDirectory(szLogFile,MAX_PATH);
	//ZeroMemory(szLogFile, MAX_PATH * sizeof(WCHAR));
	//WCHAR chCurrentProcessName[MAX_PATH];
	//WCHAR *pDest = NULL;
	//GetModuleFileName(NULL,chCurrentProcessName,MAX_PATH);

	//wcscpy(szLogFile,chCurrentProcessName);
	//pDest = wcsstr(szLogFile,L"\\DelFile.exe");
	//*pDest = L'\0';		
	CString path;
	GetLocalDir(path);
	path += L"\\dm.log";
	//wcscat(szLogFile,L"\\dm.log");
	hFile = CreateFile(path,GENERIC_READ|GENERIC_WRITE,
					 FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
					 OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,
					 NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		strText.Format(IDS_LOG_FAIL,path);
		AfxMessageBox(strText);
		return FALSE;
	}

	ReadFile(hFile,&wUnicodeSign,sizeof(WORD),&dwWrtited,NULL);
	if (wUnicodeSign != 0xfeff)
	{
		wUnicodeSign = 0xfeff;
		SetFilePointer(hFile,0,NULL,FILE_BEGIN);
		WriteFile(hFile,&wUnicodeSign,sizeof(WORD),&dwWrtited,NULL);
	}
	else SetFilePointer(hFile,0,NULL,FILE_END);
	WriteFile(hFile,strText,strText.GetLength()*sizeof(WCHAR),&dwWrtited,NULL);
	WriteFile(hFile,&ulEnter,sizeof(ULONG),&dwWrtited,NULL);
	SetEndOfFile(hFile);
	CloseHandle(hFile);
	return TRUE;
}

int GetLetterType(LPCTSTR szLetter)
{
	UINT	DriveType;
	int		nType;

	DriveType = GetDriveType(szLetter);

	switch(DriveType)
	{
	case DRIVE_REMOVABLE:
		nType = 5;
		break;
	case DRIVE_FIXED:
		nType = 0;
		break;
	case DRIVE_CDROM:
		nType = 7;
		break;
	case DRIVE_REMOTE:
		nType = 6;
		break;
	default:
		nType = 0;
		break;
	}
	return nType;
}

BOOL SecDeleteFile()
{
	CSelMethod		SelMethodDlg;

	if (!g_nSelItem) 
	{
		AfxMessageBox(IDS_INVALID_PATH,MB_OK | MB_ICONINFORMATION);
		return TRUE;
	}

	GetTotalFileSize();

	if(SelMethodDlg.DoModal () == IDOK)
	{
		g_bSave = FALSE;
		CDelFileDlg dlg;
		int nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
			CDelProc	DelProcDlg;
			if (!g_bBeepOff) Beep(1000,100);
			DelProcDlg.DoModal ();
			return TRUE;
		}
	}
	return FALSE;
}

BOOL SecDeleteFileInRecycled()
{
	CSelMethod		SelMethodDlg;
	if(SelMethodDlg.DoModal () == IDOK)
	{
		g_bSave = FALSE;
		CDelProc	DelProcDlg;
		if (!g_bBeepOff) Beep(1000,100);
		DelProcDlg.DoModal ();
		return TRUE;
	}
	return FALSE;
}

BOOL ParseCmdLine(WCHAR *pszCommand)
{
	int				i = 0,nLen;
	WCHAR			*p;

	g_nSelItem	   = 0;	
	
	memset(g_szDir,0,MAX_PATH*MAX_PATH);

	if(pszCommand[0] == L'*') return TRUE;

	do
	{
		nLen = wcslen(pszCommand);
		if (pszCommand[nLen - 1] == L'"')
		{
			pszCommand[nLen - 1] = L'\0';
			p = wcsrchr(pszCommand,L'"');
			if (p)
			{
				*p = L'\0';
				p--;
				if (*p == L' ') *p = L'\0';
				wcscpy(g_szDir[i],&p[2]);//p[0]==0 p[1]==0
				i++;
			}
		}
		else
		{
			p = wcsrchr(pszCommand,L' ');
			if(p)
			{
				*p = L'\0';
				p++;
				wcscpy(g_szDir[i],p);
				i++;
			}
		}
	}while (p);

	g_nSelItem = i;
	SecDeleteFile();
	return TRUE;
}

int CALLBACK FileListCompare(LPARAM lParam1,LPARAM lParam2,LPARAM lParamSort)
{
	ULONG ulItemData,ulItemData1,ulItemData2;
	int  nCompResult = 0,nTotal,i,nItem1 = -1,nItem2 = -1;
	CString strName1,strName2;

	nTotal = g_pListView->GetItemCount();
	for (i = 0;(i < nTotal) && ((nItem1 == -1) || (nItem2 == -1));i++)
	{
		ulItemData = g_pListView->GetItemData(i);
		if (ulItemData == (ULONG)lParam1) 
		{
			nItem1 = i;
			ulItemData1 = ulItemData;
			strName1 = g_pListView->GetItemText(nItem1,0);
		}
		if (ulItemData == (ULONG)lParam2) 
		{
			nItem2 = i;
			ulItemData2 = ulItemData;
			strName2 = g_pListView->GetItemText(nItem2,0);
		}
	}

	BOOL bAsc = (BOOL)lParamSort;
	if (bAsc)
	{
		nCompResult = (int)(ulItemData1/0x1000 - ulItemData2/0x1000);
		if (!nCompResult)
			nCompResult = strName1.CompareNoCase(strName2);
	}
	else
	{
		nCompResult = (int)(ulItemData2/0x1000 - ulItemData1/0x1000);
		if (!nCompResult)
			nCompResult = strName1.CompareNoCase(strName2);
	}
	return nCompResult;
}

BOOL RecordSetting(CSCHEDULE_ITEM_ARRAY &ScheduleItemArray)
{
	CString strSettingFileName;

	if( FALSE == GetSettingFileName(strSettingFileName) )
	{
		TRACE(L"\nGetSettingFileName error in RecordSetting.");
		return FALSE;
	}

	if( FALSE == RecordSettingHead((LPCTSTR)strSettingFileName) )
	{
		TRACE(L"\nRecordSettingHead error in RecordSetting.");
		return FALSE;
	}

	HANDLE hSettingFile;
	hSettingFile = CreateFile( 
		                       (LPCTSTR)strSettingFileName,
							   GENERIC_READ|GENERIC_WRITE,
							   FILE_SHARE_READ,
							   NULL,
							   OPEN_EXISTING,
							   FILE_ATTRIBUTE_NORMAL,
							   NULL
							);

	if( INVALID_HANDLE_VALUE == hSettingFile )
	{
		TRACE(L"\nOpen setting file error in RecordSetting.");
		return FALSE;
	}

	DWORD dwReadBytes;

	SETTING_FILE_HEAD SettingFileHead;
	
	if( FALSE == ReadFile(hSettingFile,&SettingFileHead,sizeof(SettingFileHead),&dwReadBytes,NULL) )
	{
		TRACE(L"\nRead Setting file head in ReadSetting.");
		CloseHandle(hSettingFile);
		return FALSE;
	}

	SetFilePointer(hSettingFile,0,NULL,FILE_BEGIN);

	SettingFileHead.dwSettingCount = ScheduleItemArray.GetSize();

	DWORD dwWrittenBytes;
	if( FALSE == WriteFile(hSettingFile,&SettingFileHead,sizeof(SettingFileHead),&dwWrittenBytes,NULL) )
	{
		TRACE(L"\nWriteFile error in RecordingSetting.");
		CloseHandle(hSettingFile);
		return FALSE;
	}

	SCHEDULE_ITEM ScheduleItem;

	int nItemIndex;
	for(nItemIndex=0; nItemIndex<ScheduleItemArray.GetSize(); nItemIndex++)
	{
		
		ScheduleItem = ScheduleItemArray[nItemIndex];
		
		if( FALSE == WriteFile(hSettingFile,&ScheduleItem,sizeof(ScheduleItem),&dwWrittenBytes,NULL) )
		{
			TRACE(L"\nWrite ScheduleItem error in RecordSetting.");
			CloseHandle(hSettingFile);
			return FALSE;
		}
	}

	CloseHandle(hSettingFile);
	return TRUE;
}

BOOL ReadSetting(CSCHEDULE_ITEM_ARRAY &ScheduleItemArray)
{
	ScheduleItemArray.RemoveAll();

	CString strSettingFileName;

	if( FALSE == GetSettingFileName(strSettingFileName) )
	{
		TRACE(L"\nGetSettingFileName error in RecordSetting.");
		return FALSE;
	}

    if( FALSE == IsFileScheduleSettingFile( (LPCTSTR)strSettingFileName) )
	{
		TRACE(L"\nThis is not a Davinci setting file.");
		return FALSE;
	}

	HANDLE hSettingFile;
	hSettingFile = CreateFile( 
		                       (LPCTSTR)strSettingFileName,
							   GENERIC_READ,
							   FILE_SHARE_READ,
							   NULL,
							   OPEN_EXISTING,
							   FILE_ATTRIBUTE_NORMAL,
							   NULL
							  );

	if( INVALID_HANDLE_VALUE == hSettingFile )
	{
		TRACE(L"\nOpen setting file error in ReadSetting.");
		return FALSE;
	}

	DWORD dwReadBytes;

	SETTING_FILE_HEAD SettingFileHead;
	
	if( FALSE == ReadFile(hSettingFile,&SettingFileHead,sizeof(SettingFileHead),&dwReadBytes,NULL) )
	{
		TRACE(L"\nRead Setting file head in ReadSetting.");
		CloseHandle(hSettingFile);
		return FALSE;
	}

	SCHEDULE_ITEM ScheduleItem;

	DWORD dwItemIndex;
	for(dwItemIndex=0; dwItemIndex<SettingFileHead.dwSettingCount; dwItemIndex++)
	{
		if( FALSE == ReadFile(hSettingFile,&ScheduleItem,sizeof(ScheduleItem),&dwReadBytes,NULL))
		{
			TRACE(L"\nRead ScheduleItem error in ReadSetting.");
			CloseHandle(hSettingFile);
			return FALSE;
		}
		ScheduleItemArray.Add(ScheduleItem);
	}

	CloseHandle(hSettingFile);
	return TRUE;
}

BOOL UpdateScheduleSetting()
{
	CString strSettingFileName;

	if( FALSE == GetSettingFileName(strSettingFileName) )
	{
		TRACE(L"\nGetSettingFileName error in RecordSetting.");
		return FALSE;
	}

    if( FALSE == IsFileScheduleSettingFile( (LPCTSTR)strSettingFileName) )
	{
		TRACE(L"\nThis is not a Davinci setting file.");
		return FALSE;
	}

	HANDLE hSettingFile;
	hSettingFile = CreateFile( 
		                       (LPCTSTR)strSettingFileName,
							   GENERIC_READ,
							   FILE_SHARE_READ,
							   NULL,
							   OPEN_EXISTING,
							   FILE_ATTRIBUTE_NORMAL,
							   NULL
							  );

	if( INVALID_HANDLE_VALUE == hSettingFile )
	{
		TRACE(L"\nOpen setting file error in ReadSetting.");
		return FALSE;
	}

	DWORD dwReadBytes;

	SETTING_FILE_HEAD SettingFileHead;
	
	if( FALSE == ReadFile(hSettingFile,&SettingFileHead,sizeof(SettingFileHead),&dwReadBytes,NULL) )
	{
		TRACE(L"\nRead Setting file head in ReadSetting.");
		CloseHandle(hSettingFile);
		return FALSE;
	}

	if (SettingFileHead.ulFileVer != CURRENT_FILE_VERSION)
	{
		SettingFileHead.ulFileVer = CURRENT_FILE_VERSION;
		SetFilePointer(hSettingFile,0,NULL,FILE_BEGIN);
		WriteFile(hSettingFile,&SettingFileHead,sizeof(SettingFileHead),&dwReadBytes,NULL);

		SCHEDULE_ITEM ScheduleItem;

		DWORD dwItemIndex;
		for(dwItemIndex=0; dwItemIndex<SettingFileHead.dwSettingCount; dwItemIndex++)
		{
			if( FALSE == ReadFile(hSettingFile,&ScheduleItem,sizeof(ScheduleItem),&dwReadBytes,NULL))
			{
				TRACE(L"\nRead ScheduleItem error in ReadSetting.");
				CloseHandle(hSettingFile);
				return FALSE;
			}
			GetLocalTime(&ScheduleItem.LastExcuteTime);
			SetFilePointer(hSettingFile,sizeof(SettingFileHead)+dwItemIndex*sizeof(ScheduleItem),NULL,FILE_BEGIN);
			WriteFile(hSettingFile,&ScheduleItem,sizeof(ScheduleItem),&dwReadBytes,NULL);
		}
	}
	CloseHandle(hSettingFile);
	return TRUE;
}

BOOL GetSettingFileName(CString &strProgSettingDataPath)
{

	CString strProgSettingDir;

	//if( FALSE == GetProgDir(strProgSettingDir ) )
	//{
	//	TRACE(L"\nGetProgDir error in GetSettingFileName.");
	//	return FALSE;
	//}
	if( FALSE == GetLocalDir(strProgSettingDir ) )
	{
		TRACE(L"\nGetProgDir error in GetSettingFileName.");
		return FALSE;
	}

	strProgSettingDataPath=strProgSettingDir+L"\\"+SETTING_DATA_FILE;

	return TRUE;
}


BOOL GetProgDir(CString &strProgDir)
{
	strProgDir.Empty();
	WCHAR szProgDir[MAX_PATH];
	memset(szProgDir,0,sizeof(szProgDir));
	if( FALSE == GetModuleFileName(NULL,szProgDir,MAX_PATH) )
	{
		return FALSE;
	}

	CString strWorkingDir;
	strWorkingDir = szProgDir;
	strWorkingDir = strWorkingDir.Left(strWorkingDir.ReverseFind(L'\\'));
	
	DWORD dwFileAttribute;
	dwFileAttribute = GetFileAttributes((LPCTSTR)strWorkingDir);
	if(  ( -1 != dwFileAttribute ) && ( FILE_ATTRIBUTE_DIRECTORY & dwFileAttribute ) )
	{
		strProgDir = strWorkingDir;
		return TRUE;
	}
	else
	{
		TRACE(L"\nin GetProgDir, the result directory is not a valid directory");
		return FALSE;
	}
}


BOOL IsFileScheduleSettingFile(LPCTSTR szSettingFileName)
{
	// the setting file should be exist

	DWORD dwSettingDataAttribute;

	dwSettingDataAttribute = GetFileAttributes( LPCTSTR(szSettingFileName ) );

	if( -1 == dwSettingDataAttribute )
	{
		return FALSE;
	}

	HANDLE hSettingFile;
	hSettingFile = CreateFile( szSettingFileName,GENERIC_READ,FILE_SHARE_READ,NULL,
		                       OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if( INVALID_HANDLE_VALUE == hSettingFile )
	{
		TRACE(L"\nCreateFile error in IsUSBBackupSettingFile.");
		return FALSE;
	}

	SETTING_FILE_HEAD SettingFileHead;
	DWORD dwReadBytes;
	if( FALSE == ReadFile(hSettingFile,&SettingFileHead,sizeof(SettingFileHead),&dwReadBytes,NULL) )
	{
		TRACE(L"\nReadFile error in IsFileBackupSettingFile.");
		CloseHandle(hSettingFile);
		return FALSE;
	}

	if( 0 != wcscmp(SettingFileHead.szSettingIdentity,SETTING_FILE_IDENTITY) )
	{
		TRACE(L"\nThis is not an FileBackup setting file.");
		CloseHandle(hSettingFile);
		return FALSE;
	}
	else
	{
		TRACE(L"\nThis is an File Schedule setting file.");
	}

	CloseHandle(hSettingFile);

	return TRUE;
}

BOOL RecordSettingHead(LPCTSTR szSettingFile)
{
	HANDLE hSettingFile;
	hSettingFile = CreateFile(szSettingFile,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,
		                       NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

	if( INVALID_HANDLE_VALUE == hSettingFile )
	{
		TRACE(L"\nCreateFile error in RecordSettingHead.");
		return FALSE;
	}

	SETTING_FILE_HEAD SettingFileHead;
	memset(&SettingFileHead,0,sizeof(SettingFileHead));

	SettingFileHead.ulFileVer = CURRENT_FILE_VERSION;
	SettingFileHead.ulStructSize = sizeof(SETTING_FILE_HEAD);
	SettingFileHead.dwSettingCount = 0;
	wcscpy(SettingFileHead.szSettingIdentity,SETTING_FILE_IDENTITY);

	DWORD dwWrittenBytes;
	if( FALSE == WriteFile(hSettingFile,&SettingFileHead,sizeof(SettingFileHead),&dwWrittenBytes,NULL) )
	{
	   TRACE(L"\nWriteFile error in RecordSettingHead.");
	   CloseHandle(hSettingFile);
	   return FALSE;
	}
	CloseHandle(hSettingFile);
	return TRUE;
}

// 2004.09.14 added end

// 2004.09.21 added begin

BOOL CheckExcludeFolder(LPCTSTR szAddPath,BOOL &bExcludeFolder)
{
	bExcludeFolder = FALSE;
	WCHAR szProtectDir[MAX_PATH];
	WCHAR *p;
	BOOL bEqualDir = FALSE;
	BOOL bSubDir = FALSE;
	
	GetWindowsDirectory(szProtectDir,MAX_PATH);

	if(wcslen(szAddPath) <= wcslen(L"C:\\"))
	{
		if (szAddPath[0] == szProtectDir[0])
		{
			bExcludeFolder = TRUE;
			return TRUE;
		}
	}

	CheckEqualOrSubDir(szAddPath,szProtectDir,bEqualDir,bSubDir);

	if( bEqualDir || bSubDir )
	{
		bExcludeFolder = TRUE;
		return TRUE;
	}

	GetModuleFileName(NULL,szProtectDir,MAX_PATH);
	p = wcsrchr(szProtectDir,L'\\');
	if (p) *p = L'\0';

	bEqualDir = FALSE;
	bSubDir = FALSE;
	
	CheckEqualOrSubDir(szAddPath,szProtectDir,bEqualDir,bSubDir);

	if(bEqualDir)
	{
		bExcludeFolder = TRUE;
		return TRUE;
	}

	return TRUE;
}

BOOL CheckEqualOrSubDir(LPCTSTR szFirstDir,LPCTSTR szSecondDir,BOOL &bEqualDir, BOOL &bSubDir)
{
	bEqualDir = FALSE;
	bSubDir = FALSE;

	WCHAR szFirstShortDir[MAX_PATH];
	WCHAR szSecondShortDir[MAX_PATH];

	memset(szFirstShortDir,0,sizeof(szFirstShortDir));
	memset(szSecondShortDir,0,sizeof(szSecondShortDir));

	if( FALSE == GetShortPathName(szFirstDir,szFirstShortDir,sizeof(szFirstShortDir)) )
	{
		TRACE(L"\nGetShortPathName %s error in CheckEqualOrSubDir",szFirstDir);
	}

	if( FALSE == GetShortPathName(szSecondDir,szSecondShortDir,sizeof(szSecondShortDir)) )
	{
		TRACE(L"\nGetShortPathName %s error in CheckEqualOrSubDir",szSecondDir);
	}

	CString strFirstDir;
	strFirstDir = szFirstShortDir;
	
	CString strSecondDir;
	strSecondDir = szSecondShortDir;

	strFirstDir.TrimRight(L'\\');
	strFirstDir = strFirstDir + L"\\";

	strSecondDir.TrimRight(L'\\');
	strSecondDir = strSecondDir + L"\\";

	if(strFirstDir.GetLength() < strSecondDir.GetLength() )
	{
	    bEqualDir = FALSE;
	    bSubDir = FALSE;
		return TRUE;
	}
	else if(strFirstDir.GetLength() == strSecondDir.GetLength() )
	{
		if( 0 == strFirstDir.CompareNoCase(strSecondDir) )
		{
			bEqualDir = TRUE;
			bSubDir = FALSE;
			return TRUE;
		}
		else
		{
			bEqualDir = FALSE;
			bSubDir = FALSE;
			return TRUE;
		}
	}
	else if(strFirstDir.GetLength() > strSecondDir.GetLength())
	{
		if( 0 == strFirstDir.Left(strSecondDir.GetLength()).CompareNoCase(strSecondDir) )
		{
			bEqualDir = FALSE;
			bSubDir = TRUE;
			return TRUE;
		}
		else
		{
			bEqualDir = FALSE;
			bSubDir = FALSE;
			return TRUE;
		}
	}
	return TRUE;
}

BOOL SavePassword( char *szUserName,char *szPassword,BOOL bAdmin )
{
	WCHAR   szFile[MAX_PATH],*p;
	USERPASSSTRUCT   UserInfo[2] = {0};
	DWORD  dwWrited;
	HANDLE hFile;
	BOOL bResult = FALSE,bDefCharUsed;

	//GetModuleFileName(NULL,szFile,MAX_PATH);
	//p = wcsrchr(szFile,L'\\');
	//if (!p) return FALSE;
	//wcscpy(p,L"\\AdminData.dat");
	CString path;
	GetLocalDir(path);
	path += L"\\AdminData.dat";

	hFile = CreateFile(path,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,
						NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

	if( INVALID_HANDLE_VALUE == hFile ) return FALSE;
	ReadFile(hFile,&UserInfo,sizeof(USERPASSSTRUCT)*2,&dwWrited,NULL);

	USERPASSSTRUCT* pTempUser = 0;
	if (bAdmin)
	{
		pTempUser = &UserInfo[0];
	}
	else
	{
		pTempUser = &UserInfo[1];
	}
	strncpy(pTempUser->szUserPass, szPassword, 32);
	strncpy(pTempUser->szUserName, szUserName, 32);
	YGAESEncryptData((UCHAR *)pTempUser->szUserPass,strlen(pTempUser->szUserPass),(UCHAR *)pTempUser->szUserName,(UCHAR *)pTempUser->szUserName,32);
	pTempUser->ulStructVer = CURRENT_FILE_VERSION;	
	SetFilePointer(hFile,0,NULL,FILE_BEGIN);
	WriteFile(hFile,&UserInfo,sizeof(USERPASSSTRUCT)*2,&dwWrited,NULL);
	SetEndOfFile(hFile);
	CloseHandle(hFile);
	return TRUE;
}

BOOL CheckPassword( char *szUserName,char *szPassword )
{
	WCHAR   szFile[MAX_PATH],*p;
	USERPASSSTRUCT   UserInfo[2] = {0};
	DWORD  dwWrited;
	HANDLE hFile;
	SYSTEMTIME  CurTime;
	CTimeSpan   TimeSpan;
	BOOL bResult = FALSE;

	if (stricmp(szUserName,"administrator") &&
		stricmp(szUserName,"comptroller")) return FALSE;

	//GetModuleFileName(NULL,szFile,MAX_PATH);
	//p = wcsrchr(szFile,L'\\');
	//if (!p) return FALSE;
	//wcscpy(p,L"\\AdminData.dat");
	CString path;
	GetLocalDir(path);
	path += L"\\AdminData.dat";

	hFile = CreateFile(path,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,
						NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if( INVALID_HANDLE_VALUE == hFile) 
	{
		SavePassword("administrator","administrator",TRUE);
		SavePassword("comptroller","comptroller",FALSE);
		if (!stricmp(szUserName,"administrator"))
		{
			bResult = (!stricmp(szPassword,"administrator"));
		}
		else
		{
			bResult = (!stricmp(szPassword,"comptroller"));
		}
		return bResult;
	}

	ReadFile(hFile,&UserInfo,sizeof(USERPASSSTRUCT)*2,&dwWrited,NULL);
	BOOL bAdmin = !stricmp(szUserName,"administrator");
	USERPASSSTRUCT* pTempUser = 0;
	if (bAdmin)
	{
		pTempUser = &UserInfo[0];
	}
	else
	{
		pTempUser = &UserInfo[1];
	}
	if (pTempUser->ulStructVer != CURRENT_FILE_VERSION)
	{
		CloseHandle(hFile);
		if (bAdmin)
		{
			SavePassword("administrator","administrator",TRUE);
			bResult = (!stricmp(szPassword,"administrator"));
		}
		else
		{
			SavePassword("comptroller","comptroller",FALSE);
			bResult = (!stricmp(szPassword,"comptroller"));
		}
		return bResult;
	}
	else
	{
		if (pTempUser->ulTryTimes >= 3)
		{
			GetLocalTime(&CurTime);
			if (CTime(CurTime) > CTime(pTempUser->LastTryTime))
			{
				TimeSpan = CTime(CurTime) - CTime(pTempUser->LastTryTime);
				if (TimeSpan.GetTotalMinutes() >= 10)
					pTempUser->ulTryTimes = 0;
			}
		}

		if (pTempUser->ulTryTimes >= 3)
		{
			AfxMessageBox(IDS_TRY_LIMIT);
			bResult = FALSE;
		}
		else
		{
			GetLocalTime(&pTempUser->LastTryTime);

			YGAESDecryptData((UCHAR *)pTempUser->szUserPass,strlen(pTempUser->szUserPass),\
				(UCHAR *)pTempUser->szUserName,(UCHAR *)pTempUser->szUserName,32);

			bResult = (!stricmp(szUserName,pTempUser->szUserName) && 
						!stricmp(szPassword,pTempUser->szUserPass));
			if (bResult)
			{
				pTempUser->ulLastTryStatus = 0;
				pTempUser->ulTryTimes = 0;
			}
			else
			{
				pTempUser->ulLastTryStatus = -1;
				pTempUser->ulTryTimes ++;
			}
			YGAESEncryptData((UCHAR *)pTempUser->szUserPass,strlen(pTempUser->szUserPass),(UCHAR *)pTempUser->szUserName,(UCHAR *)pTempUser->szUserName,32);
			SetFilePointer(hFile,0,NULL,FILE_BEGIN);
			WriteFile(hFile,&UserInfo,sizeof(USERPASSSTRUCT)*2,&dwWrited,NULL);
		}
	}
	
	CloseHandle(hFile);
	return bResult;
}

BOOL GetUserSid(PSID *ppSid)
{
	HANDLE hToken;
	BOOL bRes;
	DWORD cbBuffer, cbRequired;
	PTOKEN_USER pUserInfo;

	// The User's SID can be obtained from the process token
	bRes = OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken);
	if (FALSE == bRes)
	{
		return FALSE;
	}

	// Set buffer size to 0 for first call to determine
	// the size of buffer we need.
	cbBuffer = 0;
	bRes = GetTokenInformation(hToken, TokenUser, NULL, cbBuffer, &cbRequired);
	if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
	{
		return FALSE;
	}

	// Allocate a buffer for our token user data
	cbBuffer = cbRequired;
	pUserInfo = (PTOKEN_USER) HeapAlloc(GetProcessHeap(), 0, cbBuffer);
	if (NULL == pUserInfo)
	{
		return FALSE;
	}

	// Make the "real" call
	bRes = GetTokenInformation(hToken, TokenUser, pUserInfo, cbBuffer, &cbRequired);
	if (FALSE == bRes) 
	{
		return FALSE;
	}

	// Make another copy of the SID for the return value
	cbBuffer = GetLengthSid(pUserInfo->User.Sid);

	*ppSid = (PSID) HeapAlloc(GetProcessHeap(), 0, cbBuffer);
	if (NULL == *ppSid)
	{
		return FALSE;
	}

	bRes = CopySid(cbBuffer, *ppSid, pUserInfo->User.Sid);
	if (FALSE == bRes)
	{
	    HeapFree(GetProcessHeap(), 0, *ppSid);
		return FALSE;
	}

	bRes = HeapFree(GetProcessHeap(), 0, pUserInfo);

	return TRUE;
}

void GetSidString(PSID pSid, LPTSTR szBuffer)
{
	SID_IDENTIFIER_AUTHORITY *psia = ::GetSidIdentifierAuthority( pSid );
	DWORD dwTopAuthority = psia->Value[5];
	wsprintf(szBuffer, _T("S-1-%lu"), dwTopAuthority);

	TCHAR szTemp[32];
	int iSubAuthorityCount = *(GetSidSubAuthorityCount(pSid));
	for (int i = 0; i<iSubAuthorityCount; i++) 
	{
		DWORD dwSubAuthority = *(GetSidSubAuthority(pSid, i));
		wsprintf(szTemp, _T("%lu"), dwSubAuthority);
		wcscat(szBuffer, _T("-"));
		wcscat(szBuffer, szTemp);
	}
}

BOOL GetLocalDir( CString &strPath )
{
	static bool bInit = true;
	static TCHAR s_path[MAX_PATH];
	static CString path;
	if (bInit)
	{
		bInit = false;
		memset(s_path,0,sizeof(s_path));
		SHGetSpecialFolderPath(NULL,s_path,CSIDL_LOCAL_APPDATA ,FALSE);
		GetLongPathName(s_path, s_path, MAX_PATH);
		path = s_path;
		path += _T("\\FrontLine");
		CreateDirectory(path, 0);
		path += _T("\\DeleteMaster");
		CreateDirectory(path, 0);
	}
	strPath = path;
	return TRUE;
}
