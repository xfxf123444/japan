// DMF.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "BMF.h"
#include "..\AM01Expt\AM01Expt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

AM01_PROG_SETTING g_Setting;
BOOL    g_bThreadFinished;
WCHAR	g_szSettingName[MAX_SETTING_NAME_LEN];
ULONG	g_ulBackupType = 0;

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	CString strText,strTemp1,strTemp2;
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		wcerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		if (!ParseCmdLine(argc-1,&argv[1]))
			PrintUsage();
		else
		{
			strText.Format(IDS_START_BACKUP,g_szSettingName);
			wcout << (LPCTSTR)strText << endl;
			if (!BackupSetting(g_szSettingName,g_ulBackupType))
			{
				strText.Format(IDS_BACKUP_FAIL,g_szSettingName);
				cout << (LPCTSTR)strText << endl;
			}
			else
			{
				strText.Format(IDS_BACKUP_SUCC,g_szSettingName);
				wcout << (LPCTSTR)strText << endl;
			}
		}
	}
	return nRetCode;
}

BOOL ParseCmdLine(int argc, TCHAR* argv[])
{
	int i;
	CString strText;
	BOOL bInitSetting = FALSE,bInitType = FALSE;
	
	if (argc != 2) return FALSE;

	for (i = 0;i < argc;i ++)
	{
		switch (argv[i][0])
		{
		case L'-':
		case L'/':
			switch (argv[i][1])
			{
			case L'S':
			case L's':
				if (bInitSetting) return FALSE;
				wcsncpy(g_szSettingName,&argv[i][2],MAX_SETTING_NAME_LEN);
				bInitSetting = TRUE;
				break;
			case L'T':
			case L't':
				if (bInitType) return FALSE;
				switch (argv[i][2])
				{
				case L'O':
				case L'o':
					g_ulBackupType = BACKUP_TYPE_ORIGNIAL;
					bInitType = TRUE;
					break;
				case L'S':
				case L's':
					g_ulBackupType = BACKUP_TYPE_SNAPSHOT;
					bInitType = TRUE;
					break;
				}
				break;
			default:
				return FALSE;
			}
			break;
		default:
			return FALSE;
		}
	}

	return bInitType && bInitSetting;
}

void PrintUsage()
{
	CString strText;

	strText.LoadString(IDS_USAGE);
	wcout << (LPCTSTR)strText << endl;

	strText.LoadString(IDS_IMPORTANT);
	wcout << (LPCTSTR)strText << endl;

	strText.LoadString(IDS_EXPLAIN);
	wcout << (LPCTSTR)strText << endl;
	
}

BOOL BackupSetting(WCHAR *szSettingName,ULONG ulBackupType)
{
	BOOL bResult = FALSE;
	switch (ulBackupType)
	{
	case BACKUP_TYPE_ORIGNIAL:
		bResult = ExecuteOneSetting(g_szSettingName,FALSE,NULL);
		break;
	case BACKUP_TYPE_SNAPSHOT:
		bResult = ExecuteSettingNewBase(g_szSettingName);
		break;
	}
	return bResult;
}

BOOL ExecuteOneSetting(LPCTSTR szSettingName,BOOL bNewTarget, LPCTSTR szNewTargetName)
{
	AM01WORKSTATE WorkState;
	WCHAR  szCurrentFile[MAX_PATH];
	DWORD dwThreadID;

	g_Setting.Clear();

	if (!GetSettingInfo(szSettingName,&g_Setting))
	{
		if (g_Setting.pcaSource.GetSize()) g_Setting.pcaSource.RemoveAll();
		return FALSE;
	}

	if(bNewTarget)
	{
		wcsncpy(g_Setting.DataInfo.szTargetImage,szNewTargetName,MAX_PATH-1);
	}

	g_Setting.DataInfo.BackupOption.bAutoSpawn = TRUE;

	g_bThreadFinished = FALSE;
	CreateThread(NULL,0,ThreadBackupFile,NULL,0,&dwThreadID);
	
	while (!g_bThreadFinished)
	{
		Sleep(200);
		GetWorkState(&WorkState);
		if (wcscmp(szCurrentFile,WorkState.szCurrentFile))
		{
			wcsncpy(szCurrentFile,WorkState.szCurrentFile,MAX_PATH);
			wcout << (LPCTSTR)szCurrentFile << endl;
		}
	}
	if (g_Setting.pcaSource.GetSize()) g_Setting.pcaSource.RemoveAll();
	return TRUE;
}

DWORD WINAPI ThreadBackupFile(LPVOID pIn)
{
	CString strText;
	g_Setting.DataInfo.BackupOption.bCmdLine = TRUE;
	switch(g_Setting.DataInfo.wBackupType)
	{
	case MAIL_BACKUP:
		// here need fall through
	case WIN_MAIL_BACKUP:
	case OE_MAIL_BACKUP:
		if(CheckMailDataReady(g_Setting.pcaSource) == FALSE)
		{
			strText.LoadString(IDS_MAIL_DATA_NOT_READY);
			wcout << (LPCTSTR)strText << endl;
			break;
		}
	case FILE_BACKUP:
		BackupSelectionFile(g_Setting.DataInfo,g_Setting.pcaSource);
	    break;
	case BACKUP_TYPE_DIFF:
		DiffBackupSelectionFile(g_Setting.DataInfo.szTargetImage,&g_Setting.DataInfo.BackupOption);
		break;
	case EASY_BACKUP:
		BackupSelectionFile(g_Setting.DataInfo,g_Setting.pcaSource);
		break;
	}

	g_bThreadFinished = TRUE;

	return 0;
}

BOOL ExecuteSettingNewBase(WCHAR *szSettingName)
{
	AM01_PROG_SETTING SettingInfo;

	//memset(&SettingInfo,0,sizeof(SettingInfo));

	if (GetSettingInfo(szSettingName,&SettingInfo))
	{	
		BOOL bNewTarget;
		bNewTarget = TRUE;

		WCHAR szNewTargetName[MAX_PATH];
		szNewTargetName[0]=L'\0';
	
		if( FALSE == GetNextFileName(SettingInfo.DataInfo.szTargetImage,szNewTargetName,1) )
		{
			TRACE(L"\nGetNextFileName error in ExecuteSettingNewBase.");
			return FALSE;
		}

		if( FALSE == ExecuteOneSetting(szSettingName,bNewTarget,szNewTargetName) )
		{
			TRACE(L"\nExecuteOneSettingPro error in ExecuteSettingNewBase.");
			return FALSE;
		}
	}
	return TRUE;
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

	// TRACE("\nDir Name:%s",szDirName);
	
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
			// TRACE("\nFile Name:%s",(LPCTSTR)strSubPath);
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

BOOL GetSettingInfo(LPCTSTR szSettingName, AM01_PROG_SETTING *pProgSetting)
{
	__int64 lFilePointer;
	if (pProgSetting->pcaSource.GetSize()) pProgSetting->pcaSource.RemoveAll();	

	CString strProgSettingDataPath;
	
	if( FALSE == GetSettingFileName(strProgSettingDataPath) )
	{
		TRACE(L"\nGetSettingFileName error in GetSettingArray.");
	}

	if( FALSE == IsFileBackupSettingFile((LPCTSTR)strProgSettingDataPath) )
	{
		TRACE(L"\n%s is not a USBBackup setting file.",(LPCTSTR)strProgSettingDataPath );
		return FALSE;
	}

	HANDLE hSettingDataFile;
	hSettingDataFile = CreateFile( (LPCTSTR)strProgSettingDataPath,GENERIC_READ,FILE_SHARE_READ,
		                            NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if( INVALID_HANDLE_VALUE == hSettingDataFile )
	{
		TRACE(L"\nOpen setting data file error in GetSettingArray.");
		return FALSE;
	}

	SETTING_FILE_HEAD SettingFileHead;
	DWORD dwReadBytes;

	// we check the return code in the first ReadFile
	if( FALSE == ReadFile(hSettingDataFile,&SettingFileHead,sizeof(SettingFileHead),&dwReadBytes,NULL) )
	{
		TRACE(L"\nReadFile error in GetSettingArray.");
		CloseHandle(hSettingDataFile);
		return FALSE;
	}

	DWORD dwSettingIndex;

	AM01_FILE_SETTING OneSettingDataInfo;

	WCHAR szOneSourceItem[MAX_PATH];
	memset(szOneSourceItem,0,sizeof(szOneSourceItem));

	int  nSourceIndex;

	for( dwSettingIndex=0; dwSettingIndex < SettingFileHead.dwSettingCount ; dwSettingIndex++ )
	{
		ReadFile(hSettingDataFile,&OneSettingDataInfo,sizeof(OneSettingDataInfo),&dwReadBytes,NULL);
		// after read one setting, decrypt it

		if(OneSettingDataInfo.bEnable)
		{
			if( 0 == wcscmp(OneSettingDataInfo.DataInfo.szSettingName,szSettingName) )
			{
				// find one
				pProgSetting->DataInfo = OneSettingDataInfo.DataInfo;
				for(nSourceIndex=0; nSourceIndex<(int)(OneSettingDataInfo.dwNumofSource); nSourceIndex++ )
				{
					ReadFile(hSettingDataFile,szOneSourceItem,sizeof(szOneSourceItem),&dwReadBytes,NULL);
					pProgSetting->pcaSource.Add(szOneSourceItem);
				}
				
				YGSetFilePointer(hSettingDataFile,-((int)(OneSettingDataInfo.dwNumofSource))*sizeof(szOneSourceItem),FILE_CURRENT,lFilePointer);
				
				break;
			}
		}
		YGSetFilePointer(hSettingDataFile,(OneSettingDataInfo.dwNumofSource )*sizeof(szOneSourceItem),FILE_CURRENT,lFilePointer);
	}

	CloseHandle(hSettingDataFile);
	hSettingDataFile=NULL;

	return TRUE;
}

BOOL GetSettingFileName(CString &strProgSettingDataPath)
{
	WCHAR    szSettingFile[MAX_PATH];

	if( FALSE == GetApplicationDataPath(szSettingFile) )
	{
		TRACE(L"\nGet Setting Dir error in GetSettingFileName.");
		return FALSE;
	}
	
	wcscat(szSettingFile,L"\\BackupMaster");
	CreateDirectory(szSettingFile,NULL);
	
	if (GetFileAttributes(szSettingFile) == -1) 
	{
		TRACE(L"\nGet Setting Dir error in GetSettingFileName.");
		return FALSE;
	}

	wcscat(szSettingFile,L"\\");
	wcscat(szSettingFile,SETTING_DATA_FILE);
	strProgSettingDataPath = szSettingFile;

	return TRUE;
}

BOOL IsFileBackupSettingFile(LPCTSTR szSettingFileName)
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
		TRACE(L"\nThis is an USBBackup setting file.");
	}

	CloseHandle(hSettingFile);

	return TRUE;
}
