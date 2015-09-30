#include "stdafx.h"
#include "DeleteLibraryHelper.h"
#include <ShlObj.h>
#include <string>

using namespace std;

bool DeleteLibraryHelper::GetFilePath( const TCHAR* path, 
	std::vector<CString>& pathSet, 
	TCHAR* pFileSuffix /*= 0*/, 
	bool bIterate /*= true*/, 
	ULARGE_INTEGER* pSize /*= 0*/,
	bool bIncludingSubDir)
{
	if (pSize)
	{
		pSize->QuadPart = 0;
	}
	pathSet.clear();
	return __GetFilePath(path, pathSet, pFileSuffix, bIterate, pSize, bIncludingSubDir);
}

bool DeleteLibraryHelper::__GetFilePath( const TCHAR* path, 
	std::vector<CString>& pathSet,
	TCHAR* pFileSuffix,
	bool bIterate, 
	ULARGE_INTEGER* pSize,
	bool bIncludingSubDir)
{
	WIN32_FIND_DATA     FindData;
	HANDLE              hFind   ;
	size_t				nDirEnd;
	TCHAR				szDirOrFileName[MAX_PATH];

	hFind = FindFirstFile(path,&FindData);
	if (INVALID_HANDLE_VALUE == hFind)		
	{
		return false;
	}
	ULARGE_INTEGER temp;
	if ( !(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
	{
		if (pSize)
		{
			temp.HighPart = FindData.nFileSizeHigh;
			temp.LowPart = FindData.nFileSizeLow;
			pSize->QuadPart += temp.QuadPart;
		}
		if (pFileSuffix)
		{
			CString tempPath = path;
			if (tempPath.Mid(tempPath.ReverseFind(_T('.'))).CompareNoCase(pFileSuffix) == 0)
			{
				pathSet.push_back(path);
			}
		}
		else
		{
			pathSet.push_back(path);
		}
		FindClose(hFind);
		return true;
	}
	FindClose(hFind);

	_tcscpy( szDirOrFileName , path) ;
	_tcscat( szDirOrFileName , _T("\\")) ;
	nDirEnd = _tcslen( szDirOrFileName);
	_tcscpy( &szDirOrFileName[nDirEnd] , _T("*.*"));

	hFind = FindFirstFile(szDirOrFileName ,&FindData);

	if ( INVALID_HANDLE_VALUE == hFind )
	{
		return  false;
	}
	DWORD suffixLength = 0;
	if (pFileSuffix)
	{
		suffixLength = static_cast<DWORD>(_tcslen(pFileSuffix));
	}
	do
	{
		if (_tcscmp(FindData.cFileName,_T(".")) && 
			_tcscmp(FindData.cFileName,_T("..")))
		{
			_tcscpy( &szDirOrFileName[nDirEnd] ,FindData.cFileName);
			if ( !(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) 
			{
				if (pFileSuffix)
				{
					if (_tcslen(FindData.cFileName) <= suffixLength)
					{ 
						continue;
					}
					CString tempFileName = FindData.cFileName;
					tempFileName = tempFileName.Right(suffixLength);
					if (tempFileName.CompareNoCase(pFileSuffix) != 0)
					{
						continue;
					}
				}
				pathSet.push_back(szDirOrFileName);
				if (pSize)
				{
					temp.HighPart = FindData.nFileSizeHigh;
					temp.LowPart = FindData.nFileSizeLow;
					pSize->QuadPart += temp.QuadPart;
				}
			}
			if ( bIterate && (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
			{
				if (bIncludingSubDir)
				{
					pathSet.push_back(szDirOrFileName);
				}
				__GetFilePath(szDirOrFileName, pathSet, pFileSuffix, bIterate, pSize, bIncludingSubDir);
			}
		}
	} while( FindNextFile( hFind ,&FindData));

	FindClose( hFind);
	return  true;
}

const TCHAR* DeleteLibraryHelper::GetRootPath()
{
	static bool bInit = true;
	static TCHAR s_path[MAX_PATH];
	if (bInit)
	{
		bInit = false;
		memset(s_path,0,sizeof(s_path));
		SHGetSpecialFolderPath(NULL,s_path,CSIDL_WINDOWS,FALSE);
		memset(s_path + 3, 0, sizeof(s_path) - 3 * sizeof(TCHAR));
	}
	return s_path;
}

const TCHAR* DeleteLibraryHelper::GetWindowsPath()
{
	static bool bInit = true;
	static TCHAR s_path[MAX_PATH];
	if (bInit)
	{
		bInit = false;
		memset(s_path,0,sizeof(s_path));
		SHGetSpecialFolderPath(NULL,s_path,CSIDL_WINDOWS,FALSE);
		GetLongPathName(s_path, s_path, MAX_PATH);
	}
	return s_path;
}

const TCHAR* DeleteLibraryHelper::GetProgramPath()
{
	static bool bInit = true;
	static TCHAR s_path[MAX_PATH];
	if (bInit)
	{
		bInit = false;
		memset(s_path,0,sizeof(s_path));
		SHGetSpecialFolderPath(NULL,s_path,CSIDL_PROGRAM_FILES,FALSE);
		GetLongPathName(s_path, s_path, MAX_PATH);
	}
	return s_path;
}

const TCHAR* DeleteLibraryHelper::GetProgramX86Path()
{
	static bool bInit = true;
	static TCHAR s_path[MAX_PATH];
	if (bInit)
	{
		bInit = false;
		memset(s_path,0,sizeof(s_path));
		SHGetSpecialFolderPath(NULL,s_path,CSIDL_PROGRAM_FILESX86,FALSE);
		GetLongPathName(s_path, s_path, MAX_PATH);
	}
	return s_path;
}

const TCHAR* DeleteLibraryHelper::GetAppDataPath()
{
	static bool bInit = true;
	static TCHAR s_path[MAX_PATH];
	if (bInit)
	{
		bInit = false;
		memset(s_path,0,sizeof(s_path));
		SHGetSpecialFolderPath(NULL,s_path,CSIDL_APPDATA ,FALSE);
		GetLongPathName(s_path, s_path, MAX_PATH);
	}
	return s_path;
}

const TCHAR* DeleteLibraryHelper::GetLocalAppDataPath()
{
	static bool bInit = true;
	static TCHAR s_path[MAX_PATH];
	if (bInit)
	{
		bInit = false;
		memset(s_path,0,sizeof(s_path));
		SHGetSpecialFolderPath(NULL,s_path,CSIDL_LOCAL_APPDATA ,FALSE);
		GetLongPathName(s_path, s_path, MAX_PATH);
	}
	return s_path;
}

const TCHAR* DeleteLibraryHelper::GetDesktopPath()
{
	static bool bInit = true;
	static TCHAR s_path[MAX_PATH];
	if (bInit)
	{
		bInit = false;
		memset(s_path,0,sizeof(s_path));
		SHGetSpecialFolderPath(NULL,s_path,CSIDL_DESKTOPDIRECTORY,FALSE);
		GetLongPathName(s_path, s_path, MAX_PATH);
	}
	return s_path;
}

const TCHAR* DeleteLibraryHelper::GetPersonalPath()
{
	static bool bInit = true;
	static TCHAR s_path[MAX_PATH];
	if (bInit)
	{
		bInit = false;
		memset(s_path,0,sizeof(s_path));
		SHGetSpecialFolderPath(NULL,s_path,CSIDL_PERSONAL,FALSE);
		GetLongPathName(s_path, s_path, MAX_PATH);
	}
	return s_path;
}

const TCHAR* DeleteLibraryHelper::GetDocumentPath()
{
	static bool bInit = true;
	static TCHAR s_path[MAX_PATH];
	if (bInit)
	{
		bInit = false;
		memset(s_path,0,sizeof(s_path));
		SHGetSpecialFolderPath(NULL,s_path,CSIDL_COMMON_DOCUMENTS,FALSE);
		GetLongPathName(s_path, s_path, MAX_PATH);
#ifdef UNICODE
		wstring strWorkingDir = s_path;
		strWorkingDir = strWorkingDir.substr(0, strWorkingDir.find(L'\\', 3));
#else
		string strWorkingDir = s_path;
		strWorkingDir = strWorkingDir.substr(0, strWorkingDir.find_last_of('\\', 3));
#endif
		memset(s_path,0,sizeof(s_path));
		_tcscpy(s_path, strWorkingDir.c_str());
	}
	return s_path;
}

const TCHAR* DeleteLibraryHelper::GetModulePath()
{
	static bool bInit = true;
	static TCHAR s_path[MAX_PATH];
	if (bInit)
	{
		bInit = false;
		memset(s_path,0,sizeof(s_path));
		GetModuleFileName(NULL,s_path,MAX_PATH);
		GetLongPathName(s_path, s_path, MAX_PATH);
#ifdef UNICODE
		wstring strWorkingDir = s_path;
		strWorkingDir = strWorkingDir.substr(0, strWorkingDir.find_last_of(L'\\'));
#else
		string strWorkingDir = s_path;
		strWorkingDir = strWorkingDir.substr(0, strWorkingDir.find_last_of('\\'));
#endif
		memset(s_path,0,sizeof(s_path));
		_tcscpy(s_path, strWorkingDir.c_str());
	}
	return s_path;
}

const TCHAR* DeleteLibraryHelper::GetTempPath()
{
	static bool bInit = true;
	static TCHAR s_path[MAX_PATH];
	if (bInit)
	{
		bInit = false;
		memset(s_path,0,sizeof(s_path));
		::GetTempPath(MAX_PATH, s_path);
		GetLongPathName(s_path, s_path, MAX_PATH);
	}
	return s_path;
}

bool DeleteLibraryHelper::IsNormalPath( const TCHAR* path )
{
	// the root directory can not be a monitor folder
	if(_tcsicmp(path, GetRootPath()) == 0
		|| _tcsclen(path) < _tcsclen(GetRootPath()))
	{
		return false;
	}

	// the windows dir and it's sub directory can not be monitor folder
	bool bEqualDir;
	bool bSubDir;

	bEqualDir = false;
	bSubDir = false;
	CheckEqualOrSubDir(path, GetWindowsPath(),bEqualDir,bSubDir);

	if( bEqualDir || bSubDir )
	{
		return false;
	}

	// the "program files" directory will not be added as monitor folder
	bEqualDir = false;
	bSubDir = false;

	CheckEqualOrSubDir(path,GetProgramX86Path(),bEqualDir,bSubDir);

	if(bEqualDir || bSubDir )
	{
		return false;
	}

	bEqualDir = false;
	bSubDir = false;

	CheckEqualOrSubDir(path,GetProgramPath(),bEqualDir,bSubDir);

	if(bEqualDir || bSubDir )
	{
		return false;
	}

	// the "module path" directory will not be added as monitor folder
	bEqualDir = false;
	bSubDir = false;

	CheckEqualOrSubDir(path,GetModulePath(),bEqualDir,bSubDir);

	if(bEqualDir || bSubDir )
	{
		return false;
	}

	CheckEqualOrSubDir(GetModulePath(),path,bEqualDir,bSubDir);

	if(bEqualDir || bSubDir )
	{
		return false;
	}



	// "Desktop" and "My Document" are allowed to be the monitor folder
	// their sub folder can be used as monitor too.
	bEqualDir = false;
	bSubDir = false;
	CheckEqualOrSubDir(path,GetDesktopPath(),bEqualDir,bSubDir);

	if(bEqualDir || bSubDir )
	{
		return true;
	}

	bEqualDir = false;
	bSubDir = false;
	CheckEqualOrSubDir(path,GetPersonalPath(),bEqualDir,bSubDir);

	if(bEqualDir || bSubDir)
	{
		return true;
	}

	// the "Documents and settings" directory and it's subdirectory will not be added as monitor folder

	bEqualDir = false;
	bSubDir = false;
	CheckEqualOrSubDir(path,GetDocumentPath(),bEqualDir,bSubDir);

	if(bEqualDir || bSubDir)
	{
		return false;
	}

	return true;
}

bool DeleteLibraryHelper::CheckEqualOrSubDir(LPCTSTR szChildDir,LPCTSTR szParentDir,bool &bEqualDir, bool &bSubDir)
{
	bEqualDir = false;
	bSubDir = false;

	TCHAR szFirstLongDir[MAX_PATH];
	TCHAR szSecondLongDir[MAX_PATH];

	memset(szFirstLongDir,0,sizeof(szFirstLongDir));
	memset(szSecondLongDir,0,sizeof(szSecondLongDir));

	GetLongPathName(szChildDir,szFirstLongDir,MAX_PATH);
	GetLongPathName(szParentDir,szSecondLongDir,MAX_PATH);
	DWORD firstLength = static_cast<DWORD>(_tcsclen(szFirstLongDir));
	DWORD secondLength =static_cast<DWORD>( _tcsclen(szSecondLongDir));

	DWORD dwAttrFirst = GetFileAttributes(szFirstLongDir);
	DWORD dwAttrSecond = GetFileAttributes(szSecondLongDir);
	if (!(dwAttrFirst & FILE_ATTRIBUTE_DIRECTORY) && !(dwAttrFirst & FILE_ATTRIBUTE_DIRECTORY))
	{
		bSubDir = false;
		if (firstLength == secondLength && _tcsicmp(szFirstLongDir, szSecondLongDir) == 0)
		{
			bEqualDir = true;
		}
		else
		{
			bEqualDir = false;
		}
	}
	else
	{
		if(firstLength < secondLength )
		{
			bEqualDir = false;
			bSubDir = false;
		}
		else if(firstLength == secondLength )
		{
			if( _tcsicmp(szFirstLongDir, szSecondLongDir) == 0)
			{
				bEqualDir = true;
				bSubDir = false;
			}
			else
			{
				bEqualDir = false;
				bSubDir = false;
			}
		}
		else if(firstLength > secondLength)
		{
			TCHAR temp[MAX_PATH];
			ZeroMemory(temp, sizeof(temp));
			_tcscpy(temp, szFirstLongDir);
			temp[secondLength] = 0;
			if( _tcsicmp(temp, szSecondLongDir) == 0)
			{
				bEqualDir = false;
				bSubDir = true;
			}
			else
			{
				bEqualDir = false;
				bSubDir = false;
			}
		}
	}

	return bEqualDir || bSubDir;
}

bool DeleteLibraryHelper::GetFileSizeInRecycled(ULARGE_INTEGER* pSize)
{
	if (pSize == 0)
	{
		return false;
	}
	pSize->QuadPart = 0;
	TCHAR driverLetters[MAX_PATH];
	TCHAR diskLetter[4] = _T("C:\\");
	TCHAR szRecyclePath[MAX_PATH];
	int n;
	int i,j,k=0;
	int nCurrentUserNum = 0;
	TCHAR  FileSystemNameBuffer[10];
	TCHAR  chAllSIDName[MAX_PATH][MAX_PATH];
	TCHAR  chTempPath[MAX_PATH];
	DWORD MaximumConponentLenght;
	DWORD FileSystemFlags;
	OSVERSIONINFO osvi;
	BOOL bOsVersionInfoEx = FALSE;
	BOOL bCurrentOsVersionVista;
	TCHAR chSubKeyName[MAX_PATH];
	UINT nIndex = 0;

	HKEY hKey;
	DWORD dwSubLen;
	DWORD dwType = REG_SZ;
	DWORD dwSize = MAX_PATH;
	LONG lRet;
	FILETIME ftLastWriteTime;

	TCHAR     achClass[MAX_PATH] = _T("");  // buffer for class name 
	DWORD    cchClassName = MAX_PATH;  // size of class string 
	DWORD    cSubKeys=0;               // number of subkeys 
	DWORD    cbMaxSubKey;              // longest subkey size 
	DWORD    cchMaxClass;              // longest class string 
	DWORD    cValues;              // number of values for key 
	DWORD    cchMaxValue;          // longest value name 
	DWORD    cbMaxValueData;       // longest value data 
	DWORD    cbSecurityDescriptor; // size of security descriptor 

	GetLogicalDriveStrings(MAX_PATH,driverLetters);
	n = sizeof(driverLetters);
	ZeroMemory(&osvi,sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if ( !(bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO *) &osvi)) )
	{
		return false;
	}

	if (osvi.dwMajorVersion >= 6)
	{
		bCurrentOsVersionVista = TRUE;
	}

	if (osvi.dwMajorVersion == 5)
	{
		bCurrentOsVersionVista = FALSE;
	}

	lRet = RegOpenKeyEx(HKEY_USERS,_T(""),0,KEY_READ,&hKey);

	if (lRet != ERROR_SUCCESS)
	{
		return false;
	}

	lRet = RegQueryInfoKey(
		hKey,                    // key handle 
		achClass,                // buffer for class name 
		&cchClassName,           // size of class string 
		NULL,                    // reserved 
		&cSubKeys,               // number of subkeys 
		&cbMaxSubKey,            // longest subkey size 
		&cchMaxClass,            // longest class string 
		&cValues,                // number of values for this key 
		&cchMaxValue,            // longest value name 
		&cbMaxValueData,         // longest value data 
		&cbSecurityDescriptor,   // security descriptor 
		&ftLastWriteTime);       // last write time

	if (lRet != ERROR_SUCCESS)
	{
		return false;
	}

	for (nIndex = 0;nIndex < cSubKeys;nIndex++)
	{
		dwSubLen = MAX_PATH;
		lRet = RegEnumKeyEx(hKey,nIndex,
			chSubKeyName,
			&dwSubLen,
			NULL,
			NULL,
			NULL,
			&ftLastWriteTime);
		if (lRet != ERROR_SUCCESS)
		{
			return false;
		}
		else
		{
			if (dwSubLen > 8 && (_tcsstr(chSubKeyName,_T("Classes")) == NULL))
			{
				_tcscpy(chAllSIDName[k],chSubKeyName);
				k++;
				nCurrentUserNum++;
			}
		}
	}

	RegCloseKey(hKey);

	for (i = 0;i < n;i += 4)
	{
		diskLetter[0] = driverLetters[i];

		if (GetDriveType(&driverLetters[i]) == DRIVE_FIXED)
		{
			GetVolumeInformation(&driverLetters[i],NULL,0,NULL,
				&MaximumConponentLenght,&FileSystemFlags,
				FileSystemNameBuffer,10);
			_tcscpy(szRecyclePath,diskLetter);

			if (bCurrentOsVersionVista)
			{
				if (!_tcscmp(FileSystemNameBuffer,_T("NTFS")))
				{
					_tcscat(szRecyclePath,_T("$Recycle.Bin\\"));

					for (j = 0;j < nCurrentUserNum;j++)
					{
						_tcscpy(chTempPath,szRecyclePath);
						_tcscat(chTempPath,chAllSIDName[j]);
						__GetFileSizeInRecycled(chTempPath, pSize);
						ZeroMemory(chTempPath,MAX_PATH*sizeof(TCHAR));
					}
				}
				else
				{
					_tcscat(szRecyclePath,_T("$Recycle.Bin"));
					__GetFileSizeInRecycled(szRecyclePath, pSize);
				}
			}
			else
			{
				if (!_tcscmp(FileSystemNameBuffer,_T("NTFS")))
				{
					_tcscat(szRecyclePath,_T("RECYCLER\\"));

					for (j = 0;j < nCurrentUserNum;j++)
					{
						_tcscpy(chTempPath,szRecyclePath);
						_tcscat(chTempPath,chAllSIDName[j]);
						__GetFileSizeInRecycled(chTempPath, pSize);
						ZeroMemory(chTempPath,MAX_PATH*sizeof(TCHAR));
					}
				}
				else
				{
					_tcscat(szRecyclePath,_T("Recycled"));
					__GetFileSizeInRecycled(szRecyclePath, pSize);
				}
			}
		}

	}

	return true;
}

bool DeleteLibraryHelper::__GetFileSizeInRecycled(TCHAR *pszDir, ULARGE_INTEGER* pSize)
{
	WIN32_FIND_DATA FindData;
	HANDLE hFind;
	TCHAR recycledFilePath[MAX_PATH];
	TCHAR findFileName[MAX_PATH];
	TCHAR szTempDirectory[MAX_PATH];

#ifdef UNICODE
	swprintf(recycledFilePath,L"%s\\*.*",pszDir);
#else
	sprintf(recycledFilePath,"%s\\*.*",pszDir);
#endif

	hFind = FindFirstFile(recycledFilePath,&FindData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	do
	{
		if ((_tcscmp(FindData.cFileName,_T(".")) != 0) && (_tcscmp(FindData.cFileName,_T(".."))) != 0)
		{
			_tcscpy(findFileName,FindData.cFileName);

			if (_tcscmp(findFileName,_T("desktop.ini")) && _tcscmp(findFileName,_T("INFO2")))
			{
				if (!(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					ULARGE_INTEGER temp;
					temp.HighPart = FindData.nFileSizeHigh;
					temp.LowPart = FindData.nFileSizeLow;
					pSize->QuadPart += temp.QuadPart;
				}
				else
				{
#ifdef UNICODE
					swprintf(szTempDirectory,L"%s\\%s",pszDir,findFileName);
#else
					sprintf(szTempDirectory,"%s\\%s",pszDir,findFileName);
#endif
					__GetFileSizeInRecycled(szTempDirectory, pSize);
				}
			}
		}
	}while(FindNextFile(hFind,&FindData));

	FindClose(hFind);
	return true;
}
