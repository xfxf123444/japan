#include "Stdafx.h"
#include "Resource.h"
#include "Function.h"
#include "YGFSMonConst.h"
#include "..\..\..\Davinci_tech\lib\Crypto\Cur_ver\Export\Crypto.h"
#include "..\..\..\Davinci_tech\ZLib\Cur_ver\Export\ZLib.h"
#include "..\DavinciFunction\DavinciFunction.h"

BOOL RecordSetting(CMONITOR_ITEM_ARRAY &MonitorItemArray)
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

	__int64 linTargetFilePointer;

	if( FALSE == YGSetFilePointer(hSettingFile,0,FILE_BEGIN,linTargetFilePointer) )
	{
		TRACE(L"\nSetFilePointer error in RecordSelectionArray.");
		CloseHandle(hSettingFile);
		return FALSE;
	}

	SettingFileHead.dwSettingCount = MonitorItemArray.GetSize();

	DWORD dwWrittenBytes;
	if( FALSE == WriteFile(hSettingFile,&SettingFileHead,sizeof(SettingFileHead),&dwWrittenBytes,NULL) )
	{
		TRACE(L"\nWriteFile error in RecordingSetting.");
		CloseHandle(hSettingFile);
		return FALSE;
	}

	MONITOR_ITEM MonitorItem;

	int nItemIndex;
	for(nItemIndex=0; nItemIndex<MonitorItemArray.GetSize(); nItemIndex++)
	{
		
		MonitorItem = MonitorItemArray[nItemIndex];
		
		if( FALSE == EncryptPassword(MonitorItem.szPassword,sizeof(MonitorItem.szPassword)) )
		{
			TRACE(L"\nEncryptPassword error in RecordSetting.");
		}

		if( FALSE == WriteFile(hSettingFile,&MonitorItem,sizeof(MonitorItem),&dwWrittenBytes,NULL) )
		{
			TRACE(L"\nWrite MonitorItem error in RecordSetting.");
			CloseHandle(hSettingFile);
			return FALSE;
		}
	}

	CloseHandle(hSettingFile);
	return TRUE;
}

BOOL ReadSetting(CMONITOR_ITEM_ARRAY &MonitorItemArray)
{
	MonitorItemArray.RemoveAll();

	CString strSettingFileName;

	if( FALSE == GetSettingFileName(strSettingFileName) )
	{
		TRACE(L"\nGetSettingFileName error in RecordSetting.");
		return FALSE;
	}

    if( FALSE == IsFileMonitorSettingFile( (LPCTSTR)strSettingFileName) )
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

	MONITOR_ITEM MonitorItem;

	DWORD dwItemIndex;
	for(dwItemIndex=0; dwItemIndex<SettingFileHead.dwSettingCount; dwItemIndex++)
	{
		if( FALSE == ReadFile(hSettingFile,&MonitorItem,sizeof(MonitorItem),&dwReadBytes,NULL))
		{
			TRACE(L"\nRead MonitorItem error in ReadSetting.");
			CloseHandle(hSettingFile);
			return FALSE;
		}
		if( FALSE == DecryptPassword(MonitorItem.szPassword,sizeof(MonitorItem.szPassword) ) )
		{
			TRACE(L"\nDecryptPassword error in ReadSetting.");
		}
		MonitorItemArray.Add(MonitorItem);
	}

	CloseHandle(hSettingFile);
	return TRUE;
}


BOOL GetSettingFileName(CString &strProgSettingDataPath)
{
	strProgSettingDataPath.Empty();
	WCHAR szAppDir[MAX_PATH];
	memset(szAppDir,0,sizeof(szAppDir));
	if( FALSE == GetApplicationDataPath(szAppDir ) )
	{
		TRACE(L"\nGetAppDir error in GetSettingFileName.");
		return FALSE;
	}
	strProgSettingDataPath = szAppDir;
	strProgSettingDataPath += L"\\AutoEncryption";
	CreateDirectory(strProgSettingDataPath,NULL);
	
	if (GetFileAttributes(strProgSettingDataPath) == -1) 
	{
		TRACE(L"\nGetAppDir error in GetSettingFileName.");
		return FALSE;
	}
	strProgSettingDataPath=strProgSettingDataPath+"\\"+SETTING_DATA_FILE;

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
	GetLongPathName(szProgDir, szProgDir, MAX_PATH);
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


BOOL IsFileMonitorSettingFile(LPCTSTR szSettingFileName)
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
		TRACE(L"\nThis is an File Monitor setting file.");
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
	wcsncpy(SettingFileHead.szSettingIdentity,SETTING_FILE_IDENTITY,SETTING_FILE_IDENTITY_SIZE);

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

BOOL EncryptPassword(WCHAR *pchPassword,int nPasswordSize)
{
	return true;

	if( PASSWORD_SIZE * sizeof(WCHAR) != nPasswordSize )
	{
		TRACE(L"\nThe password size is not correct.");
	}

	char chPasswordKey[PASSWORD_SIZE];
	ZeroMemory(chPasswordKey, sizeof(chPasswordKey));
	WideCharToMultiByte(CP_ACP, 0, pchPassword, nPasswordSize, chPasswordKey, PASSWORD_SIZE, 0, 0);

	if(
		FALSE == YGAESEncryptData(
		                           (unsigned char*)chPasswordKey, // key
		                           sizeof(chPasswordKey), // key size
								   (unsigned char*)pchPassword, // in bufffer
								   (unsigned char*)pchPassword, // out buffer
								   PASSWORD_SIZE * sizeof(WCHAR)// data size
								  ) 
	 )
	{
		TRACE(L"\nYGAESEncryptData error in EncryptPassword");
		return FALSE;
	}

	return TRUE;
}

BOOL DecryptPassword(WCHAR *pchPassword,int nPasswordSize)
{
	return true;

	if( PASSWORD_SIZE * sizeof(WCHAR) != nPasswordSize )
	{
		TRACE(L"\nThe password size is not correct.");
	}

	char chPasswordKey[PASSWORD_SIZE];
	ZeroMemory(chPasswordKey, sizeof(chPasswordKey));
	WideCharToMultiByte(CP_ACP, 0, pchPassword, nPasswordSize, chPasswordKey, PASSWORD_SIZE, 0, 0);

	if(
		FALSE == YGAESDecryptData(
		                           (unsigned char*)chPasswordKey, // key
		                           sizeof(chPasswordKey), // key size
								   (unsigned char*)pchPassword, // in bufffer
								   (unsigned char*)pchPassword, // out buffer
								   PASSWORD_SIZE * sizeof(WCHAR)// data size
								  ) 
	 )
	{
		TRACE(L"\nYGAESDecryptData error in DecryptPassword");
		return FALSE;
	}

	return TRUE;
}

// 2004.09.14 added end

// 2004.09.21 added begin

BOOL CheckExcludeFolder(LPCTSTR szAddPath,BOOL &bExcludeFolder)
{
	// the root directory can not be a monitor folder
	bExcludeFolder = FALSE;

	CString strAddPath;
	strAddPath = szAddPath;
 	if(strAddPath.GetLength() <= (int)( wcslen(L"C:\\") ) )
	{
		TRACE(L"\nCan not add root dir as monitor folder.");
		bExcludeFolder = TRUE;
		return TRUE;
	}

	// the windows dir and it's sub directory can not be monitor folder
	WCHAR szWinDir[MAX_PATH];
	memset(szWinDir,0,sizeof(szWinDir));
	GetWindowsDirectory(szWinDir,sizeof(szWinDir));

	BOOL bEqualDir;
	BOOL bSubDir;
	
	bEqualDir = FALSE;
	bSubDir = FALSE;
	CheckEqualOrSubDir(szAddPath,szWinDir,bEqualDir,bSubDir);

	if( bEqualDir || bSubDir )
	{
		TRACE(L"\nCan not add windows directory or its subdirectories: %s as monitor folder",(LPCTSTR)strAddPath);
		bExcludeFolder = TRUE;
		return TRUE;
	}

	// the "program files" directory will not be added as monitor folder
	WCHAR szProgPath[MAX_PATH];
	SHGetSpecialFolderPath(NULL,szProgPath,0x26,FALSE);

	bEqualDir = FALSE;
	bSubDir = FALSE;
	
	CheckEqualOrSubDir(szAddPath,szProgPath,bEqualDir,bSubDir);

	if(bEqualDir || bSubDir )
	{
		TRACE(L"\nCan not add program directory or its sub directory:%s as monitor folder",szAddPath);
		bExcludeFolder = TRUE;
		return TRUE;
	}

	// "Desktop" and "My Document" are allowed to be the monitor folder
	// their sub folder can be used as monitor too.

	WCHAR szDesktopPath[MAX_PATH];
	memset(szDesktopPath,0,sizeof(szDesktopPath));
	SHGetSpecialFolderPath(NULL,szDesktopPath,CSIDL_DESKTOPDIRECTORY,FALSE);

	bEqualDir = FALSE;
	bSubDir = FALSE;
	CheckEqualOrSubDir(szAddPath,szDesktopPath,bEqualDir,bSubDir);

	if(bEqualDir || bSubDir )
	{
		TRACE(L"\nDesktop folder or its sub folder %s can be used as monitor folder",szAddPath);
		bExcludeFolder = FALSE;
		return TRUE;
	}

	WCHAR szMyDocumentPath[MAX_PATH];
	memset(szMyDocumentPath,0,sizeof(szMyDocumentPath));
	SHGetSpecialFolderPath(NULL,szMyDocumentPath,CSIDL_PERSONAL,FALSE);
	
	bEqualDir=FALSE;
	bSubDir = FALSE;
	CheckEqualOrSubDir(szAddPath,szMyDocumentPath,bEqualDir,bSubDir);

	if(bEqualDir || bSubDir)
	{
		TRACE(L"\nMy document folder and its sub folder %s can be added as monitor folder.",szAddPath);
		bExcludeFolder = FALSE;
		return TRUE;
	}

	// the "Documents and settings" directory and it's subdirectory can be added as monitor folder

	WCHAR szDocPath[MAX_PATH];
	memset(szDocPath,0,sizeof(szDocPath));

	WCHAR szMyDocPath[MAX_PATH];
	memset(szMyDocPath,0,sizeof(szMyDocPath));

	// C:\Documents and Settings\username\My Documents

	SHGetSpecialFolderPath(NULL,szMyDocPath,CSIDL_PERSONAL,FALSE);

	CString strDocPath;
	strDocPath = szMyDocPath;

	int nIndex;
	nIndex = strDocPath.Find('\\',3);
	strDocPath = strDocPath.Left(nIndex);
	wcsncpy(szDocPath,(LPCTSTR)strDocPath,MAX_PATH-1);

	bEqualDir = FALSE;
	bSubDir = FALSE;
	CheckEqualOrSubDir(szAddPath,szDocPath,bEqualDir,bSubDir);

	if(bEqualDir || bSubDir)
	{
		bExcludeFolder = FALSE;
		TRACE(L"\ndocument and setting directory:%s can be set as monitor folder",szAddPath);
		return TRUE;
	}

	return TRUE;
}


BOOL CheckEqualOrSubDir(LPCTSTR szFirstDir,LPCTSTR szSecondDir,BOOL &bEqualDir, BOOL &bSubDir)
{
	bEqualDir = FALSE;
	bSubDir = FALSE;

	WCHAR szFirstLongDir[MAX_PATH];
	WCHAR szSecondLongDir[MAX_PATH];

	memset(szFirstLongDir,0,sizeof(szFirstLongDir));
	memset(szSecondLongDir,0,sizeof(szSecondLongDir));

	if( FALSE == GetLongPathName(szFirstDir,szFirstLongDir,MAX_PATH) )
	{
		TRACE(L"\nGetLongPathName %s error in CheckEqualOrSubDir",szFirstDir);
	}

	if( FALSE == GetLongPathName(szSecondDir,szSecondLongDir,MAX_PATH) )
	{
		TRACE(L"\nGetLongPathName %s error in CheckEqualOrSubDir",szSecondDir);
	}

	CString strFirstDir;
	strFirstDir = szFirstLongDir;
	
	CString strSecondDir;
	strSecondDir = szSecondLongDir;

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

BOOL UpgradeSettingFile()
{
	CString strSettingFileName;

	if( FALSE == GetSettingFileName(strSettingFileName) )
	{
		TRACE(L"\nGetSettingFileName error in UpgradeSetting.");
		return FALSE;
	}

	if (GetFileAttributes(strSettingFileName) == -1) return TRUE;

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
		TRACE(L"\nOpen setting file error in UpgradeSetting.");
		return FALSE;
	}

	DWORD dwReadBytes;

	SETTING_FILE_HEAD_100 SettingFileHeadOld;
	
	if( FALSE == ReadFile(hSettingFile,&SettingFileHeadOld,sizeof(SETTING_FILE_HEAD_100),&dwReadBytes,NULL) )
	{
		TRACE(L"\nRead Setting file head in UpgradeSetting.");
		CloseHandle(hSettingFile);
		return FALSE;
	}

	if (!memcmp(&SettingFileHeadOld.szSettingIdentity,SETTING_FILE_IDENTITY,sizeof(SETTING_FILE_IDENTITY)))
	{
		SETTING_FILE_HEAD SettingFileHead;
		DWORD			dwWrited;
		memset(&SettingFileHead,0,sizeof(SETTING_FILE_HEAD));
		SettingFileHead.ulStructSize = sizeof(SETTING_FILE_HEAD);
		SettingFileHead.ulFileVer = CURRENT_FILE_VERSION;
		SettingFileHead.dwSettingCount = SettingFileHeadOld.dwSettingCount;
		wcscpy(SettingFileHead.szSettingIdentity,SETTING_FILE_IDENTITY);
		__int64 linTargetFilePointer;

		if( FALSE == YGSetFilePointer(hSettingFile,0,FILE_BEGIN,linTargetFilePointer) )
		{
			TRACE(L"\nSetFilePointer error in UpgradeSetting.");
			CloseHandle(hSettingFile);
			return FALSE;
		}
		WriteFile(hSettingFile,&SettingFileHead,sizeof(SETTING_FILE_HEAD),&dwWrited,NULL);

		MONITOR_ITEM MonitorItem;

		DWORD dwItemIndex;
		for(dwItemIndex=0; dwItemIndex<SettingFileHead.dwSettingCount; dwItemIndex++)
		{
			if( FALSE == ReadFile(hSettingFile,&MonitorItem,sizeof(MonitorItem),&dwReadBytes,NULL))
			{
				TRACE(L"\nRead MonitorItem error in UpgradeSetting.");
				CloseHandle(hSettingFile);
				return FALSE;
			}
			MonitorItem.ulCompressLevel = LEVEL_NO_COMPRESS;
			if( FALSE == YGSetFilePointer(hSettingFile,0-sizeof(MonitorItem),FILE_CURRENT,linTargetFilePointer) )
			{
				TRACE(L"\nSetFilePointer error in UpgradeSetting.");
				CloseHandle(hSettingFile);
				return FALSE;
			}
			if( FALSE == WriteFile(hSettingFile,&MonitorItem,sizeof(MonitorItem),&dwWrited,NULL))
			{
				TRACE(L"\nWrite MonitorItem error in UpgradeSetting.");
				CloseHandle(hSettingFile);
				return FALSE;
			}
		}
	}
	CloseHandle(hSettingFile);
	return TRUE;
}
// 2004.09.21 added end

//


BOOL GetShellFolderInfo(SHELL_FOLDER_INFO &ShellFolder)
{
	CString strPath;
	CString strName;

	BOOL bNeedUpdate;
	bNeedUpdate = FALSE;

	memset(&ShellFolder,0,sizeof(ShellFolder));
	
	strPath = L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders";
	
	strName = L"History";

	CString strHistoryDir;
	if( QueryRegCurrentUser((LPCTSTR)strPath,(LPCTSTR)strName,REG_SZ,strHistoryDir) )
	{
		wcsncpy(ShellFolder.szHistoryDir,(LPCTSTR)strHistoryDir,MAX_PATH-1);
	}

	strName = L"Cache";
	CString strCacheDir;
    
	if( QueryRegCurrentUser((LPCTSTR)strPath,(LPCTSTR)strName,REG_SZ,strCacheDir) )
	{
		wcsncpy(ShellFolder.szCacheDir,(LPCTSTR)strCacheDir,MAX_PATH-1);
	}

	strName = L"Cookies";
	CString strCookieDir;
    
	if( QueryRegCurrentUser((LPCTSTR)strPath,(LPCTSTR)strName,REG_SZ,strCookieDir) )
	{
		wcsncpy(ShellFolder.szCookieDir,(LPCTSTR)strCookieDir,MAX_PATH-1);
	}

	strName = L"Favorites";
	CString strFavoriteDir;
    
	if( QueryRegCurrentUser((LPCTSTR)strPath,(LPCTSTR)strName,REG_SZ,strFavoriteDir) )
	{
		wcsncpy(ShellFolder.szFavoriteDir,(LPCTSTR)strFavoriteDir,MAX_PATH-1);
	}

	strPath = L"Environment" ;
	strName = L"Temp" ;
	CString strTempDir;

	if( QueryRegCurrentUser((LPCTSTR)strPath,(LPCTSTR)strName,REG_EXPAND_SZ,strTempDir) )
	{
		wcsncpy(ShellFolder.szTempDir,(LPCTSTR)strTempDir,MAX_PATH-1);
	}
	
	return TRUE;
}


BOOL IsFile(LPCTSTR szFileName)
{
	DWORD dwFileAttribute;
	dwFileAttribute = GetFileAttributes(szFileName);
	if ( -1 != dwFileAttribute ) 
	{
		if( 0 == ( dwFileAttribute & FILE_ATTRIBUTE_DIRECTORY ) )
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL QueryRegCurrentUser(LPCTSTR szRegPath,LPCTSTR szRegName,DWORD dwDataType,CString &strRegValue)
{
	strRegValue.Empty();

	if(
		(REG_SZ != dwDataType) &&
		(REG_EXPAND_SZ != dwDataType )
	  )
	{
		return FALSE;
	}

	HKEY hKEY;

	long ret0;

	ret0=(::RegOpenKeyEx(HKEY_CURRENT_USER,szRegPath,0,KEY_READ,&hKEY));   
	
	if(ret0!=ERROR_SUCCESS)
	{
		return FALSE;
	}

	WCHAR szRegValue[MAX_PATH];
	szRegValue[MAX_PATH-1]=L'\0';
	
	// DWORD type_2=REG_SZ;

	DWORD type_2=dwDataType;
	
	DWORD cbData_2=MAX_PATH*sizeof(WCHAR);

	long ret2=::RegQueryValueEx(hKEY,szRegName,NULL,&type_2,(LPBYTE)szRegValue,&cbData_2); 
	
	if(ret2!=ERROR_SUCCESS)
	{
		return FALSE;
	}

	if(REG_SZ == dwDataType)
	{
		strRegValue = szRegValue;
	}
	else if(REG_EXPAND_SZ == dwDataType )
	{
		WCHAR szExpandRegValue[MAX_PATH];
		ExpandEnvironmentStrings( (WCHAR *)szRegValue, szExpandRegValue, MAX_PATH-1 );//expands environment-variable strings and replaces them with their defined values.
		strRegValue = szExpandRegValue;
	}
	else
	{
		TRACE(L"\nDataType error.");
		return FALSE;
	}
	
	return TRUE;
}

BOOL IsDirectory(LPCTSTR szDirectoryName)
{
	DWORD dwFileAttribute;
	dwFileAttribute = GetFileAttributes(szDirectoryName);
	if ( -1 != dwFileAttribute ) 
	{
		if( 0 != ( dwFileAttribute & FILE_ATTRIBUTE_DIRECTORY ) )
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL GetApplicationDataPath(WCHAR *szPath)
{
	BOOL    bResult = FALSE;
    IMalloc *pm;
    LPITEMIDLIST pidl;

    if (!FAILED(CoInitialize(NULL)))
	{
		if (SUCCEEDED(SHGetSpecialFolderLocation(NULL,CSIDL_APPDATA,&pidl)))
		{
			SHGetPathFromIDList(pidl,szPath);
			SHGetMalloc(&pm);
			if (pm)
			{
				 pm->Free(pidl);
				 pm->Release();
			}
			bResult = (GetFileAttributes(szPath) != -1);
		}
		CoUninitialize();
	}
	return bResult;
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
		path += _T("\\AngoukaMaster");
		CreateDirectory(path, 0);
	}
	strPath = path;
	return TRUE;
}
