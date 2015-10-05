#include "BaseLib.h"
#include <ShlObj.h>
#include <cstdio>
#include <string>
#include <iphlpapi.h>
#include <WinIoCtl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

bool CBaseLib::GetFilePath( const TCHAR* path, 
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

bool CBaseLib::__GetFilePath( const TCHAR* path, 
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
		suffixLength = _tcslen(pFileSuffix);
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

bool CBaseLib::GetUserSid(PSID *ppSid)
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

void CBaseLib::GetSidString(PSID pSid, LPTSTR szBuffer)
{
	SID_IDENTIFIER_AUTHORITY *psia = ::GetSidIdentifierAuthority( pSid );
	DWORD dwTopAuthority = psia->Value[5];
#ifdef UNICODE
	wsprintf(szBuffer, _T("S-1-%lu"), dwTopAuthority);
#else
	sprintf(szBuffer, ("S-1-%lu"), dwTopAuthority);
#endif

	TCHAR szTemp[32];
	int iSubAuthorityCount = *(GetSidSubAuthorityCount(pSid));
	for (int i = 0; i<iSubAuthorityCount; i++) 
	{
		DWORD dwSubAuthority = *(GetSidSubAuthority(pSid, i));
#ifdef UNICODE
		wsprintf(szTemp, _T("%lu"), dwSubAuthority);
#else
		sprintf(szTemp, ("%lu"), dwSubAuthority);
#endif
		_tcscat(szBuffer, _T("-"));
		_tcscat(szBuffer, szTemp);
	}
}

#define MAX_NAME 256

bool CBaseLib::SearchTokenGroupsForSID () 
{
	DWORD i, dwSize = 0, dwResult = 0;
	HANDLE hToken;
	PTOKEN_GROUPS pGroupInfo;
	SID_NAME_USE SidType;
	TCHAR lpName[MAX_NAME];
	TCHAR lpDomain[MAX_NAME];
	BYTE sidBuffer[100];
	PSID pSID = (PSID)&sidBuffer;
	SID_IDENTIFIER_AUTHORITY SIDAuth = SECURITY_NT_AUTHORITY;

	// Open a handle to the access token for the calling process.

	if (!OpenProcessToken( GetCurrentProcess(), TOKEN_QUERY, &hToken )) {
		printf( "OpenProcessToken Error %u\n", GetLastError() );
		return FALSE;
	}

	// Call GetTokenInformation to get the buffer size.

	if(!GetTokenInformation(hToken, TokenGroups, NULL, dwSize, &dwSize)) {
		dwResult = GetLastError();
		if( dwResult != ERROR_INSUFFICIENT_BUFFER ) {
			printf( "GetTokenInformation Error %u\n", dwResult );
			return FALSE;
		}
	}

	// Allocate the buffer.

	pGroupInfo = (PTOKEN_GROUPS) GlobalAlloc( GPTR, dwSize );

	// Call GetTokenInformation again to get the group information.

	if(! GetTokenInformation(hToken, TokenGroups, pGroupInfo, 
		dwSize, &dwSize ) ) {
			printf( "GetTokenInformation Error %u\n", GetLastError() );
			return FALSE;
	}

	// Create a SID for the BUILTIN\Administrators group.

	if(! AllocateAndInitializeSid( &SIDAuth, 2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&pSID) ) {
			printf( "AllocateAndInitializeSid Error %u\n", GetLastError() );
			return FALSE;
	}

	// Loop through the group SIDs looking for the administrator SID.

	for(i=0; i<pGroupInfo->GroupCount; i++) {
		if ( EqualSid(pSID, pGroupInfo->Groups[i].Sid) ) {

			// Lookup the account name and print it.

			dwSize = MAX_NAME;
			if( !LookupAccountSid( NULL, pGroupInfo->Groups[i].Sid,
				lpName, &dwSize, lpDomain, 
				&dwSize, &SidType ) ) {
					dwResult = GetLastError();
					if( dwResult == ERROR_NONE_MAPPED )
						_tcscpy_s(lpName, dwSize, _T("NONE_MAPPED") );
					else {
						printf("LookupAccountSid Error %u\n", GetLastError());
						return FALSE;
					}
			}
			printf( "Current user is a member of the %s\\%s group\n", 
				lpDomain, lpName );

			// Find out whether the SID is enabled in the token.
			if (pGroupInfo->Groups[i].Attributes & SE_GROUP_ENABLED)
				printf("The group SID is enabled.\n");
			else if (pGroupInfo->Groups[i].Attributes & 
				SE_GROUP_USE_FOR_DENY_ONLY)
				printf("The group SID is a deny-only SID.\n");
			else 
				printf("The group SID is not enabled.\n");
		}
	}

	if (pSID)
		FreeSid(pSID);
	if ( pGroupInfo )
		GlobalFree( pGroupInfo );
	return TRUE;
}

bool CBaseLib::IsUserAdmin() 
{
	typedef
		BOOL (WINAPI * PCheckTokenMembership)(
		HANDLE TokenHandle,  // handle to access token
		PSID SidToCheck,     // SID
		PBOOL IsMember       // result
		);

	BOOL						b;
	PSID						AdministratorsGroup; 
	SID_IDENTIFIER_AUTHORITY	NtAuthority = SECURITY_NT_AUTHORITY;
	HMODULE						hModule;
	PCheckTokenMembership		func;

	b = AllocateAndInitializeSid(&NtAuthority,
		2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&AdministratorsGroup); 

	hModule = GetModuleHandle(_T("advapi32.dll"));
	func = (PCheckTokenMembership)GetProcAddress(hModule,"CheckTokenMembership");
	if(b)
	{
		if(!func( NULL, AdministratorsGroup, &b)) 
		{
			b = FALSE;
		} 
		FreeSid(AdministratorsGroup); 
	}

	if (b)
	{
		return true;
	}
	return false;
}

const TCHAR* CBaseLib::GetRootPath()
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

const TCHAR* CBaseLib::GetWindowsPath()
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

const TCHAR* CBaseLib::GetProgramPath()
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

const TCHAR* CBaseLib::GetProgramX86Path()
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

const TCHAR* CBaseLib::GetAppDataPath()
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

const TCHAR* CBaseLib::GetLocalAppDataPath()
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

const TCHAR* CBaseLib::GetDesktopPath()
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

const TCHAR* CBaseLib::GetPersonalPath()
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

const TCHAR* CBaseLib::GetDocumentPath()
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

const TCHAR* CBaseLib::GetModulePath()
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

const TCHAR* CBaseLib::GetTempPath()
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

bool CBaseLib::IsWindows7Like()
{
	static bool bInit = true;
	static bool ret = false;
	if (bInit)
	{
		OSVERSIONINFO osvi;
		BOOL bOsVersionEx = FALSE;

		ZeroMemory(&osvi,sizeof(OSVERSIONINFO));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx((OSVERSIONINFO *)&osvi);

		ret = (osvi.dwMajorVersion == 6);
	}
	return ret;
}

bool CBaseLib::IsWindowsXPLike()
{
	static bool bInit = true;
	static bool ret = false;
	if (bInit)
	{
		OSVERSIONINFO osvi;
		BOOL bOsVersionEx = FALSE;

		ZeroMemory(&osvi,sizeof(OSVERSIONINFO));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx((OSVERSIONINFO *)&osvi);

		ret = (osvi.dwMajorVersion == 5);
	}
	return ret;
}

bool CBaseLib::GetMacAddress(TCHAR* macBuf, DWORD size)  
{  
	if (size < 32)
	{
		return false;
	}
	ZeroMemory(macBuf, sizeof(TCHAR) * size);
	PIP_ADAPTER_INFO pAdapterInfo;  
	DWORD AdapterInfoSize;
	DWORD Err;    
	AdapterInfoSize   =   0;  
	Err   =   GetAdaptersInfo(NULL,   &AdapterInfoSize);  
	if((Err   !=   0)   &&   (Err   !=   ERROR_BUFFER_OVERFLOW)){   
		return   false;  
	}  
	//   分配网卡信息内存  
	pAdapterInfo   =   (PIP_ADAPTER_INFO)   GlobalAlloc(GPTR,   AdapterInfoSize);  
	if(pAdapterInfo   ==   NULL){   
		return   false;  
	}    
	if(GetAdaptersInfo(pAdapterInfo,   &AdapterInfoSize)   !=   0){  
		GlobalFree(pAdapterInfo);  
		return   false;  
	}

	PIP_ADAPTER_INFO pFirstAdapter = pAdapterInfo;
	PIP_ADAPTER_INFO pPrimaryAdapter = 0;
	DWORD index = DWORD_MAX;
	while (pAdapterInfo)
	{
		if (pAdapterInfo->Index < index)
		{
			index = pAdapterInfo->Index;
			pPrimaryAdapter = pAdapterInfo;
		}
		pAdapterInfo = pAdapterInfo->Next;
	}

	swprintf(macBuf, L"%02X-%02X-%02X-%02X-%02X-%02X", pPrimaryAdapter->Address[0],  
		pPrimaryAdapter->Address[1],  
		pPrimaryAdapter->Address[2],  
		pPrimaryAdapter->Address[3],  
		pPrimaryAdapter->Address[4],  
		pPrimaryAdapter->Address[5]);

	GlobalFree(pFirstAdapter);  
	return   true;  
}

bool CBaseLib::IsNormalPath( const TCHAR* path )
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

bool CBaseLib::CheckEqualOrSubDir(LPCTSTR szChildDir,LPCTSTR szParentDir,bool &bEqualDir, bool &bSubDir)
{
	bEqualDir = false;
	bSubDir = false;

	TCHAR szFirstLongDir[MAX_PATH];
	TCHAR szSecondLongDir[MAX_PATH];

	memset(szFirstLongDir,0,sizeof(szFirstLongDir));
	memset(szSecondLongDir,0,sizeof(szSecondLongDir));

	GetLongPathName(szChildDir,szFirstLongDir,MAX_PATH);
	GetLongPathName(szParentDir,szSecondLongDir,MAX_PATH);
	DWORD firstLength = _tcsclen(szFirstLongDir);
	DWORD secondLength = _tcsclen(szSecondLongDir);
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
		if( _tcsicmp(temp, szSecondLongDir) == 0
			&& szFirstLongDir[secondLength] == _T('\\'))
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
	return bEqualDir || bSubDir;
}

bool CBaseLib::CheckOverlapDir( LPCTSTR szFirstDir,LPCTSTR szSecondDir )
{
	bool temp1, temp2;
	return CheckEqualOrSubDir(szFirstDir, szSecondDir, temp1, temp2) || CheckEqualOrSubDir(szSecondDir, szFirstDir, temp1, temp2);
}

bool CBaseLib::SelectFolder( HWND hWnd,TCHAR *ach )
{
	bool ret = false;
	IMalloc *pm;
	BROWSEINFO bi;
	LPITEMIDLIST pidl;

	if (!FAILED(CoInitialize(NULL)))
	{
		bi.hwndOwner      = hWnd;
		bi.pidlRoot       = NULL;
		bi.pszDisplayName = ach;
		bi.lpszTitle      = NULL;
		bi.ulFlags        = BIF_STATUSTEXT | BIF_USENEWUI | BIF_RETURNONLYFSDIRS;
		bi.lpfn           = NULL;
		bi.lParam         = 0;
		bi.iImage         = 0;

		pidl = SHBrowseForFolder(&bi);

		if (pidl)
		{
			SHGetPathFromIDList(pidl, ach);

			// TrimRightChar(ach,'\\',MAX_PATH);

			SHGetMalloc(&pm);
			if (pm)
			{
				pm->Free(pidl);
				pm->Release();
			}
			ret = TRUE;
		}
		CoUninitialize();
	}
	return ret;
}

bool CBaseLib::SelectFiles( std::vector<CString>& pathSet )
{
	CFileDialog dlg(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT);
	CString fileName;
	const int c_cMaxFiles = 1024;
	const int c_cbBuffSize = (c_cMaxFiles * (MAX_PATH + 1)) + 1;
	dlg.GetOFN().lpstrFile = fileName.GetBuffer(c_cbBuffSize);
	dlg.GetOFN().nMaxFile = c_cbBuffSize;

	if(dlg.DoModal() == IDOK) 
	{ 
		int   nCount   =   0; 
		POSITION pos = dlg.GetStartPosition(); 
		while   (pos != NULL) 
		{ 
			pathSet.push_back(dlg.GetNextPathName(pos)); 
		} 
	} 
	fileName.ReleaseBuffer();
	return true;
}

CString CBaseLib::GetDeviceName()
{
	static CString name;
	if (name.GetLength() == 0)
	{
		char buf[100];
		::gethostname(buf, sizeof(buf));
		TCHAR tbuf[100];
		ZeroMemory(tbuf, sizeof(tbuf));
		mbstowcs(tbuf, buf, 100);
		name = tbuf;
	}
	return name;
}

void CBaseLib::GetDriverLetters( vector<TCHAR>& driverLetterSet )
{
	DWORD uiMask = GetLogicalDrives();
	for (int i = 0; i < 32; ++i)
	{
		if (uiMask & (1 << i))
		{
			driverLetterSet.push_back(_T('a') + i);
		}
	}
}

bool CBaseLib::IsUsbDisk( CString& driverLetter )
{
	if (driverLetter.IsEmpty())
	{
		return false;
	}
	CString driver;
	driver.Format(_T("%c:\\"), driverLetter.GetAt(0));
	switch ( GetDriveType( driver ) )  
	{
	case DRIVE_REMOVABLE:	// The drive can be removed from the drive.
		return true;
	case DRIVE_FIXED:		// The disk cannot be removed from the drive.
		{
			driver.Format(_T("\\\\?\\%c:"), driverLetter.GetAt(0));
			HANDLE hDevice = CreateFile(driver, GENERIC_READ,
				FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
			if (hDevice != INVALID_HANDLE_VALUE)
			{
				bool ret = false;
				PSTORAGE_DEVICE_DESCRIPTOR pDevDesc = (PSTORAGE_DEVICE_DESCRIPTOR)new BYTE[sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1];
				pDevDesc->Size = sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1;
				STORAGE_PROPERTY_QUERY	Query;	// input param for query
				DWORD dwOutBytes;				// IOCTL output length
				// specify the query type
				Query.PropertyId = StorageDeviceProperty;
				Query.QueryType = PropertyStandardQuery;

				// Query using IOCTL_STORAGE_QUERY_PROPERTY 
				if(::DeviceIoControl(hDevice,			// device handle
					IOCTL_STORAGE_QUERY_PROPERTY,			// info of device property
					&Query, sizeof(STORAGE_PROPERTY_QUERY),	// input data buffer
					pDevDesc, pDevDesc->Size,				// output data buffer
					&dwOutBytes,							// out's length
					(LPOVERLAPPED)NULL)
					&& pDevDesc->BusType == BusTypeUsb)
				{
					ret = true;
				}
				delete pDevDesc;
				CloseHandle(hDevice);
				return ret;
			}
			else
			{
				return false;
			}
		}
	default:
		return false;
	}
}

const TCHAR* CBaseLib::GetIECachePath()
{
	static bool bInit = true;
	static TCHAR s_path[MAX_PATH];
	if (bInit)
	{
		bInit = false;
		memset(s_path,0,sizeof(s_path));
		SHGetSpecialFolderPath(NULL,s_path,CSIDL_INTERNET_CACHE,FALSE);
		GetLongPathName(s_path, s_path, MAX_PATH);
	}
	return s_path;
}

const TCHAR* CBaseLib::GetCookieCachePath()
{
	static bool bInit = true;
	static TCHAR s_path[MAX_PATH];
	if (bInit)
	{
		bInit = false;
		memset(s_path,0,sizeof(s_path));
		SHGetSpecialFolderPath(NULL,s_path,CSIDL_COOKIES,FALSE);
		GetLongPathName(s_path, s_path, MAX_PATH);
	}
	return s_path;
}

COleDateTime CBaseLib::LocalTimeToUTCTime( COleDateTime& localTime )
{
	SYSTEMTIME time;
	localTime.GetAsSystemTime(time);
	TIME_ZONE_INFORMATION zinfo;
	GetTimeZoneInformation(&zinfo);
	SYSTEMTIME convertTime;
	TzSpecificLocalTimeToSystemTime(&zinfo,&time, &convertTime);
	return COleDateTime(convertTime);
}

COleDateTime CBaseLib::UTCTimeToLocalTime( COleDateTime& utcTime )
{
	SYSTEMTIME time;
	utcTime.GetAsSystemTime(time);
	TIME_ZONE_INFORMATION zinfo;
	GetTimeZoneInformation(&zinfo);
	SYSTEMTIME convertTime;
	SystemTimeToTzSpecificLocalTime(&zinfo,&time, &convertTime);
	return COleDateTime(convertTime);
}

const TCHAR* CBaseLib::GetIEPath()
{
	static bool bInit = true;
	static TCHAR s_path[MAX_PATH];
	if (bInit)
	{
		bInit = false;
		memset(s_path,0,sizeof(s_path));
		CString strIEPath;
		HKEY hKey = NULL;  
		long lErr = RegOpenKeyEx( HKEY_LOCAL_MACHINE, TEXT("SOFTWARE//Microsoft//Windows//CurrentVersion//App Paths//IEXPLORE.EXE"),  
			0, KEY_QUERY_VALUE, &hKey );  
		if ( lErr == ERROR_SUCCESS )  
		{  
			TCHAR szIEPath[80];  
			DWORD dwLen = sizeof(szIEPath);  
			lErr = RegQueryValueEx( hKey, NULL, 0, NULL, (LPBYTE)szIEPath, &dwLen );  
			RegCloseKey( hKey );  
			if ( lErr == ERROR_SUCCESS )  
			{
				strIEPath.Format( TEXT("%s"), szIEPath );  
				strIEPath.Replace( TCHAR('//'), TCHAR('/') );  
				_tcscpy(s_path, strIEPath);
			}
		}  
	}
	return s_path;
}

std::string CBaseLib::CStringToString( const CString& value )
{
	char buf[MAX_PATH * 2];
	ZeroMemory(buf, sizeof(buf));
	WideCharToMultiByte(CP_ACP, 0, value, value.GetLength(), buf, sizeof(buf), 0, 0);
	return string(buf);
}

CString CBaseLib::StringToCString( const std::string& value )
{
	TCHAR buf[MAX_PATH];
	ZeroMemory(buf, sizeof(buf));
	MultiByteToWideChar(CP_ACP, 0, value.c_str(), value.length(), buf, MAX_PATH);
	return CString(buf);
}

std::string CBaseLib::UnicodeStrToUTF8Str( const CString& value )
{
	// 将unicode字符串转换为utf8字符串, 英文不转
	vector<pair<bool, CString> > ss;
	CutString(value, ss);
	string ret;
	char tempBuf[MAX_PATH];
	char codeBuf[MAX_PATH * 4];
	for (vector<pair<bool, CString> >::iterator it = ss.begin(); it != ss.end(); ++it)
	{
		if (it->first)
		{
			ZeroMemory(tempBuf, sizeof(tempBuf));
			WideCharToMultiByte(CP_UTF8, 0, it->second, it->second.GetLength(), tempBuf, sizeof(tempBuf), 0, 0);
			int length = strlen(tempBuf);
			ZeroMemory(codeBuf, sizeof(codeBuf));
			for (int i = 0; i < length; ++i)
			{
				sprintf(codeBuf + i * 3, "%%%X", (BYTE)tempBuf[i]);
			}
			ret += codeBuf;
		}
		else
		{
			ret += CStringToString(it->second);
		}
	}
	return ret;
}

std::string CBaseLib::UnicodeStrToGBKStr( const CString& value )
{
	// 将unicode字符串转换为gbk字符串, 英文不转
	vector<pair<bool, CString> > ss;
	CutString(value, ss);
	string ret;
	char tempBuf[MAX_PATH];
	char codeBuf[MAX_PATH * 4];
	for (vector<pair<bool, CString> >::iterator it = ss.begin(); it != ss.end(); ++it)
	{
		if (it->first)
		{
			ZeroMemory(tempBuf, sizeof(tempBuf));
			WideCharToMultiByte(CP_ACP, 0, it->second, it->second.GetLength(), tempBuf, sizeof(tempBuf), 0, 0);
			int length = strlen(tempBuf);
			ZeroMemory(codeBuf, sizeof(codeBuf));
			for (int i = 0; i < length; ++i)
			{
				sprintf(codeBuf + i * 3, "%%%X", (BYTE)tempBuf[i]);
			}
			ret += codeBuf;
		}
		else
		{
			ret += CStringToString(it->second);
		}
	}
	return ret;
}

void CBaseLib::CutString( const CString& value, vector<pair<bool, CString> >& ss )
{
	int alphaNumIndex = -1;
	int otherIndex = -1;
	ss.clear();
	int i = 0;
	for (; i < value.GetLength(); ++i)
	{
		if ((_T('A') <= value[i] && value[i] <= _T('Z'))
			|| (_T('a') <= value[i] && value[i] <= _T('z'))
			|| (_T('0') <= value[i] && value[i] <= _T('9')))
		{
			if (alphaNumIndex < 0)
			{
				alphaNumIndex = i;
			}
			if (otherIndex >= 0)
			{
				ss.push_back(make_pair(true, value.Mid(otherIndex, i - otherIndex)));
				otherIndex = -1;
			}
		}
		else
		{
			if (alphaNumIndex >= 0)
			{
				ss.push_back(make_pair(false, value.Mid(alphaNumIndex, i - alphaNumIndex)));
				alphaNumIndex = -1;
			}
			if (otherIndex < 0)
			{
				otherIndex = i;
			}
		}
	}
	if (alphaNumIndex >= 0)
	{
		ss.push_back(make_pair(false, value.Mid(alphaNumIndex, i - alphaNumIndex)));
	}
	else if (otherIndex >= 0)
	{
		ss.push_back(make_pair(true, value.Mid(otherIndex, i - otherIndex)));
	}
}
