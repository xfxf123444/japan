#include "stdafx.h"
#include "test.h"
#include "SecureDelete.h"
//#include "Tlhelp32.h"

#define MAX_KEY_LENGTH 255

BOOL g_bCancelDelete;
char chSID[MAX_PATH];

BOOL DoDelFile(LPCWSTR pszDir, BYTE *pBuf,BOOL bDel)
{
	HANDLE		hFile;
	DWORD		dwRWSize,dwWrites;
	DWORD		dwRW,i;
	WCHAR		szFileName[MAX_PATH];
	WCHAR		*p;
	DWORD       dwFileSizeLo,dwFileSizeHigh;

	if(g_bCancelDelete)	return FALSE;

	hFile = CreateFileW( pszDir, 
						GENERIC_READ|GENERIC_WRITE, 
						FILE_SHARE_READ | FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING, 
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if (hFile == INVALID_HANDLE_VALUE) 
		return FALSE;
	dwFileSizeLo = GetFileSize(hFile,&dwFileSizeHigh);

	for(dwRWSize = 0;dwRWSize < dwFileSizeLo;dwRWSize += dwWrites)
	{
		if (dwFileSizeLo - dwRWSize < DM_DEL_SIZE) dwWrites = dwFileSizeLo - dwRWSize;
		else dwWrites = DM_DEL_SIZE;
		WriteFile(hFile,pBuf,dwWrites,&dwRW,NULL);
		
		if(g_bCancelDelete)
		{
			FlushFileBuffers(hFile);
			CloseHandle(hFile);
			hFile = INVALID_HANDLE_VALUE;
			DeleteFileW(pszDir);
			return FALSE;
		}
	}
	FlushFileBuffers(hFile);
	CloseHandle(hFile);
	if(bDel) 
	{
		wcscpy(szFileName,pszDir);
		p = GetFileNameFromPath(szFileName);
		p++;
		wcscpy(p,RENAME_STRING);
		MoveFileW(pszDir,szFileName);
		DeleteFileW(szFileName);
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
			DeleteFileW(szFileName);
		}
		p++;
		for(i=97;i<=122;i++)
		{
			*p = (WCHAR)i;
			DeleteFileW(szFileName);
		}
	}
	return TRUE;
}

BOOL DoDeleteMasterDel(LPCWSTR szDirName, int nDelMethod)
{
	int			i;
	BOOL        bResult = FALSE;
	BYTE *pBuf;

	g_bCancelDelete = FALSE;
	
	pBuf = (BYTE *)malloc(DM_DEL_SIZE);
	
	if(!pBuf) return FALSE;

	switch(nDelMethod)
	{
	case 0:
		InitDelBuf(pBuf,0,0,0,FALSE,TRUE);
		bResult = DelDirAndFiles(szDirName,pBuf,TRUE);
		break;
	case 1:
		InitDelBuf(pBuf,0XFF,0,0,FALSE,TRUE);
		bResult = DelDirAndFiles(szDirName,pBuf,TRUE);
		break;
	case 2:
		InitDelBuf(pBuf,0,0,0,TRUE,TRUE);
		bResult = DelDirAndFiles(szDirName,pBuf,TRUE);
		break;
	case 3:
		
		// now we do not consider the progress bar
		for(i=0;i<3;i++)
		{
			switch(i)
			{
			case 0:
			case 1:
				InitDelBuf(pBuf,0,0,0,TRUE,TRUE);
				break;
			case 2:
				InitDelBuf(pBuf,0,0,0,FALSE,TRUE);
				break;
			}
			if(i==2)
			{
				if(!DelDirAndFiles(szDirName,pBuf,TRUE))
					break;
			}
			else
			{
				if(!DelDirAndFiles(szDirName,pBuf,FALSE))
					break;
			}
		}
		bResult = TRUE;
		break;
	case 4:
		// do not consider progress bar tempory
		for(i=0;i<4;i++)
		{
			switch(i)
			{
			case 0:
			case 2:
				InitDelBuf(pBuf,0,0,0,FALSE,TRUE);
				break;
			case 1:
			case 3:
				InitDelBuf(pBuf,0xFF,0,0,FALSE,TRUE);
				break;
			}
			if(i==3)
			{
				if(!DelDirAndFiles(szDirName,pBuf,TRUE))
					break;
			}
			else
			{
				if(!DelDirAndFiles(szDirName,pBuf,FALSE))
					break;
			}
		}
		bResult = TRUE;
		break;
	case 5:
		// do not consider progress bar temporary
		for(i=0;i<3;i++)
		{
			switch(i)
			{
			case 0:
				InitDelBuf(pBuf,0,0,0,FALSE,TRUE);
				break;
			case 1:
				InitDelBuf(pBuf,0XFF,0,0,FALSE,TRUE);
				break;
			case 2:
				InitDelBuf(pBuf,0X35,0XCA,0X97,FALSE,FALSE);
				break;
			}
			if(i==2)
			{
				if(!DelDirAndFiles(szDirName,pBuf,TRUE)) break;
			}
			else
			{
				if(!DelDirAndFiles(szDirName,pBuf,FALSE)) break;
			}
		}
		bResult = TRUE;
		break;
	case 6:
		
		// now do not consider progress bar temporary
		for(i=0;i<7;i++)
		{
			switch(i)
			{
			case 0:
			case 2:
			case 4:
				InitDelBuf(pBuf,0,0,0,FALSE,TRUE);
				break;
			case 1:
			case 3:
			case 5:
				InitDelBuf(pBuf,0XFF,0,0,FALSE,TRUE);
				break;
			case 6:
				InitDelBuf(pBuf,0X35,0XCA,0X97,FALSE,FALSE);
				break;
			}
			if(i==6)
			{
				if(!DelDirAndFiles(szDirName,pBuf,TRUE)) break;
			}
			else
			{
				if(!DelDirAndFiles(szDirName,pBuf,FALSE)) break;
			}
		}
		bResult = TRUE;
		break;
	case 7:
		
		// now do not conside progress bar temporary
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
				InitDelBuf(pBuf,0,0,0,TRUE,TRUE);
				break;
			case 4:
				InitDelBuf(pBuf,0X55,0,0,FALSE,TRUE);
				break;
			case 5:
				InitDelBuf(pBuf,0XAA,0,0,FALSE,TRUE);
				break;
			case 6:
			case 25:
				InitDelBuf(pBuf,0X92,0X49,0X24,FALSE,FALSE);
				break;
			case 7:
			case 26:
				InitDelBuf(pBuf,0X49,0X24,0X92,FALSE,FALSE);
				break;
			case 8:
			case 27:
				InitDelBuf(pBuf,0X24,0X92,0X49,FALSE,FALSE);
				break;
			case 9:
				InitDelBuf(pBuf,0,0,0,FALSE,TRUE);
				break;
			case 10:
				InitDelBuf(pBuf,0X11,0,0,FALSE,TRUE);
				break;
			case 11:
				InitDelBuf(pBuf,0X22,0,0,FALSE,TRUE);
				break;
			case 12:
				InitDelBuf(pBuf,0X33,0,0,FALSE,TRUE);
				break;
			case 13:
				InitDelBuf(pBuf,0X44,0,0,FALSE,TRUE);
				break;
			case 14:
				InitDelBuf(pBuf,0X55,0,0,FALSE,TRUE);
				break;
			case 15:
				InitDelBuf(pBuf,0X66,0,0,FALSE,TRUE);
				break;
			case 16:
				InitDelBuf(pBuf,0x77,0,0,FALSE,TRUE);
				break;
			case 17:
				InitDelBuf(pBuf,0X88,0,0,FALSE,TRUE);
				break;
			case 18:
				InitDelBuf(pBuf,0X99,0,0,FALSE,TRUE);
				break;
			case 19:
				InitDelBuf(pBuf,0XAA,0,0,FALSE,TRUE);
				break;
			case 20:
				InitDelBuf(pBuf,0XBB,0,0,FALSE,TRUE);
				break;
			case 21:
				InitDelBuf(pBuf,0XCC,0,0,FALSE,TRUE);
				break;
			case 22:
				InitDelBuf(pBuf,0XDD,0,0,FALSE,TRUE);
				break;
			case 23:
				InitDelBuf(pBuf,0XEE,0,0,FALSE,TRUE);
				break;
			case 24:
				InitDelBuf(pBuf,0XFF,0,0,FALSE,TRUE);
				break;
			case 28:
				InitDelBuf(pBuf,0X6D,0XB6,0XDB,FALSE,FALSE);
				break;
			case 29:
				InitDelBuf(pBuf,0XB6,0XDB,0X6D,FALSE,FALSE);
				break;
			case 30:
				InitDelBuf(pBuf,0XDB,0X6D,0XB6,FALSE,FALSE);
				break;
			}
			if(i==34)
			{
				if(!DelDirAndFiles(szDirName,pBuf,TRUE)) break;
			}
			else
			{
				if(!DelDirAndFiles(szDirName,pBuf,FALSE)) break;
			}
		}
		bResult = TRUE;
		break;
	}
	free(pBuf);
	pBuf = NULL;
	
	return bResult;
}

BOOL DelDirAndFiles(LPCWSTR pszDir,
								   BYTE *pBuf,
								   BOOL bDel)
{
    WIN32_FIND_DATAW     FindData;
	HANDLE              hFind   ;
	DWORD				dwAttrs;
	int					i;
	WCHAR				*p;
	int					nDirEnd;
	WCHAR				szDirOrFileName[MAX_PATH];
	WCHAR				szDirName[MAX_PATH];

	if(g_bCancelDelete)	return FALSE;

	hFind = FindFirstFileW(pszDir,&FindData);
	if (INVALID_HANDLE_VALUE == hFind)
		return FALSE;
  	
	if ( !(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
	{
		FindClose(hFind);
		dwAttrs = GetFileAttributesW(pszDir);
		SetFileAttributesW(pszDir,FILE_ATTRIBUTE_NORMAL);
		if(!DoDelFile(pszDir,pBuf,bDel))
		{
			SetFileAttributesW(pszDir,dwAttrs);
			if (bDel)
			{
				SYSTEMTIME CurrentTime;
				GetLocalTime(&CurrentTime);
				WriteLog(pszDir,&CurrentTime,&CurrentTime,FALSE);
			}
			return FALSE;
		}
		return TRUE;
	}
	FindClose(hFind);

  	wcscpy( szDirOrFileName , pszDir) ;
    wcscat( szDirOrFileName , L"\\" ) ;
    nDirEnd = wcslen( szDirOrFileName);
    wcscpy( &szDirOrFileName[nDirEnd], L"*.*");
	
	hFind = FindFirstFileW(szDirOrFileName ,&FindData);
	
	if ( INVALID_HANDLE_VALUE == hFind )
		return  FALSE ;
	
	do
    {
		if (wcscmp(FindData.cFileName,L".") &&
			wcscmp(FindData.cFileName,L".."))
		{
			wcscpy(&szDirOrFileName[nDirEnd] ,FindData.cFileName);

			if ( !(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) 
			{
				dwAttrs = GetFileAttributesW(szDirOrFileName);
				SetFileAttributesW(szDirOrFileName,FILE_ATTRIBUTE_NORMAL);
				if(!DoDelFile(szDirOrFileName,pBuf,bDel))
				{
					SetFileAttributesW(szDirOrFileName,dwAttrs);
					if (bDel)
					{
						SYSTEMTIME CurrentTime;
						GetLocalTime(&CurrentTime);
						WriteLog(szDirOrFileName,&CurrentTime,&CurrentTime,FALSE);
					}
				}
			}
			else
			{
				if (!DelDirAndFiles(szDirOrFileName,pBuf,bDel))
				{
					  FindClose( hFind );
					  return FALSE ;
				}
				if(bDel)
				{
					RemoveDirectoryW(szDirOrFileName);

					wcscpy(szDirName,szDirOrFileName);
					p = wcsrchr(szDirName,'\\');
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
			}
		}
	} while( FindNextFileW( hFind ,&FindData));
	FindClose( hFind);
    return  TRUE;
}

BOOL InitDelBuf(BYTE *pBuf, BYTE btFir, BYTE btSec, BYTE btThr, BOOL bRand, BOOL bOneByte)
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

BOOL DoDeleteMasterDelFileInRecycled(int nDelMethod)
{
	int			i;
	BOOL        bResult = FALSE;
	BYTE *pBuf;

	g_bCancelDelete = FALSE;
	
	pBuf = (BYTE *)malloc(DM_DEL_SIZE);
	
	if(!pBuf) return FALSE;

	switch(nDelMethod)
	{
	case 0:
		InitDelBuf(pBuf,0,0,0,FALSE,TRUE);
		bResult = DelAllInRecycled(pBuf,TRUE);
		break;
	case 1:
		InitDelBuf(pBuf,0XFF,0,0,FALSE,TRUE);
		bResult = DelAllInRecycled(pBuf,TRUE);
		break;
	case 2:
		InitDelBuf(pBuf,0,0,0,TRUE,TRUE);
		bResult = DelAllInRecycled(pBuf,TRUE);
		break;
	case 3:
		
		// now we do not consider the progress bar
		for(i=0;i<3;i++)
		{
			switch(i)
			{
			case 0:
			case 1:
				InitDelBuf(pBuf,0,0,0,TRUE,TRUE);
				break;
			case 2:
				InitDelBuf(pBuf,0,0,0,FALSE,TRUE);
				break;
			}
			if(i==2)
			{
				if(!DelAllInRecycled(pBuf,TRUE))
					break;
			}
			else
			{
				if(!DelAllInRecycled(pBuf,FALSE))
					break;
			}
		}
		bResult = TRUE;
		break;
	case 4:
		// do not consider progress bar tempory
		for(i=0;i<4;i++)
		{
			switch(i)
			{
			case 0:
			case 2:
				InitDelBuf(pBuf,0,0,0,FALSE,TRUE);
				break;
			case 1:
			case 3:
				InitDelBuf(pBuf,0xFF,0,0,FALSE,TRUE);
				break;
			}
			if(i==3)
			{
				if(!DelAllInRecycled(pBuf,TRUE))
					break;
			}
			else
			{
				if(!DelAllInRecycled(pBuf,FALSE))
					break;
			}
		}
		bResult = TRUE;
		break;
	case 5:
		// do not consider progress bar temporary
		for(i=0;i<3;i++)
		{
			switch(i)
			{
			case 0:
				InitDelBuf(pBuf,0,0,0,FALSE,TRUE);
				break;
			case 1:
				InitDelBuf(pBuf,0XFF,0,0,FALSE,TRUE);
				break;
			case 2:
				InitDelBuf(pBuf,0X35,0XCA,0X97,FALSE,FALSE);
				break;
			}
			if(i==2)
			{
				if(!DelAllInRecycled(pBuf,TRUE)) break;
			}
			else
			{
				if(!DelAllInRecycled(pBuf,FALSE)) break;
			}
		}
		bResult = TRUE;
		break;
	case 6:
		
		// now do not consider progress bar temporary
		for(i=0;i<7;i++)
		{
			switch(i)
			{
			case 0:
			case 2:
			case 4:
				InitDelBuf(pBuf,0,0,0,FALSE,TRUE);
				break;
			case 1:
			case 3:
			case 5:
				InitDelBuf(pBuf,0XFF,0,0,FALSE,TRUE);
				break;
			case 6:
				InitDelBuf(pBuf,0X35,0XCA,0X97,FALSE,FALSE);
				break;
			}
			if(i==6)
			{
				if(!DelAllInRecycled(pBuf,TRUE)) break;
			}
			else
			{
				if(!DelAllInRecycled(pBuf,FALSE)) break;
			}
		}
		bResult = TRUE;
		break;
	case 7:
		
		// now do not conside progress bar temporary
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
				InitDelBuf(pBuf,0,0,0,TRUE,TRUE);
				break;
			case 4:
				InitDelBuf(pBuf,0X55,0,0,FALSE,TRUE);
				break;
			case 5:
				InitDelBuf(pBuf,0XAA,0,0,FALSE,TRUE);
				break;
			case 6:
			case 25:
				InitDelBuf(pBuf,0X92,0X49,0X24,FALSE,FALSE);
				break;
			case 7:
			case 26:
				InitDelBuf(pBuf,0X49,0X24,0X92,FALSE,FALSE);
				break;
			case 8:
			case 27:
				InitDelBuf(pBuf,0X24,0X92,0X49,FALSE,FALSE);
				break;
			case 9:
				InitDelBuf(pBuf,0,0,0,FALSE,TRUE);
				break;
			case 10:
				InitDelBuf(pBuf,0X11,0,0,FALSE,TRUE);
				break;
			case 11:
				InitDelBuf(pBuf,0X22,0,0,FALSE,TRUE);
				break;
			case 12:
				InitDelBuf(pBuf,0X33,0,0,FALSE,TRUE);
				break;
			case 13:
				InitDelBuf(pBuf,0X44,0,0,FALSE,TRUE);
				break;
			case 14:
				InitDelBuf(pBuf,0X55,0,0,FALSE,TRUE);
				break;
			case 15:
				InitDelBuf(pBuf,0X66,0,0,FALSE,TRUE);
				break;
			case 16:
				InitDelBuf(pBuf,0x77,0,0,FALSE,TRUE);
				break;
			case 17:
				InitDelBuf(pBuf,0X88,0,0,FALSE,TRUE);
				break;
			case 18:
				InitDelBuf(pBuf,0X99,0,0,FALSE,TRUE);
				break;
			case 19:
				InitDelBuf(pBuf,0XAA,0,0,FALSE,TRUE);
				break;
			case 20:
				InitDelBuf(pBuf,0XBB,0,0,FALSE,TRUE);
				break;
			case 21:
				InitDelBuf(pBuf,0XCC,0,0,FALSE,TRUE);
				break;
			case 22:
				InitDelBuf(pBuf,0XDD,0,0,FALSE,TRUE);
				break;
			case 23:
				InitDelBuf(pBuf,0XEE,0,0,FALSE,TRUE);
				break;
			case 24:
				InitDelBuf(pBuf,0XFF,0,0,FALSE,TRUE);
				break;
			case 28:
				InitDelBuf(pBuf,0X6D,0XB6,0XDB,FALSE,FALSE);
				break;
			case 29:
				InitDelBuf(pBuf,0XB6,0XDB,0X6D,FALSE,FALSE);
				break;
			case 30:
				InitDelBuf(pBuf,0XDB,0X6D,0XB6,FALSE,FALSE);
				break;
			}
			if(i==34)
			{
				if(!DelAllInRecycled(pBuf,TRUE)) break;
			}
			else
			{
				if(!DelAllInRecycled(pBuf,FALSE)) break;
			}
		}
		bResult = TRUE;
		break;
	}
	free(pBuf);
	pBuf = NULL;
	
	return bResult;
}

BOOL DelAllInRecycled(BYTE *pBuf,BOOL bDel)
{
	WCHAR driverLetters[MAX_PATH];
	WCHAR diskLetter[4] = L"C:\\";
	WCHAR szRecyclePath[MAX_PATH];
	int n;
	int i,j;
	static int nCurrentUserNum = 0;
	WCHAR  FileSystemNameBuffer[10];
	WCHAR  chAllSIDName[MAX_PATH][MAX_PATH];
	WCHAR  chTempPath[MAX_PATH];
	DWORD MaximumConponentLenght;
	DWORD FileSystemFlags;
	DWORD dwFileAttri;
	OSVERSIONINFO osvi;
	BOOL bOsVersionInfoEx = FALSE;
	BOOL bCurrentOsVersionVista;
	UINT nIndex = 0;
	WCHAR chSubKeyName[MAX_KEY_LENGTH];

	HKEY hKey;
	DWORD dwSubLen;
	DWORD dwType = REG_SZ;
	DWORD dwSize = MAX_PATH;
	LONG lRet;
	FILETIME ftLastWriteTime;

	WCHAR    achClass[MAX_PATH] = L"";  // buffer for class name 
    DWORD    cchClassName = MAX_PATH;  // size of class string 
    DWORD    cSubKeys=0;               // number of subkeys 
    DWORD    cbMaxSubKey;              // longest subkey size 
    DWORD    cchMaxClass;              // longest class string 
    DWORD    cValues;              // number of values for key 
    DWORD    cchMaxValue;          // longest value name 
    DWORD    cbMaxValueData;       // longest value data 
    DWORD    cbSecurityDescriptor; // size of security descriptor 

	GetLogicalDriveStringsW(MAX_PATH,driverLetters);
	n = sizeof(driverLetters);
	ZeroMemory(&osvi,sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if ( !(bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO *) &osvi)) )
	{
		return FALSE;
	}

	if (osvi.dwMajorVersion == 6)
	{
		bCurrentOsVersionVista = TRUE;
	}

	if (osvi.dwMajorVersion == 5)
	{
		bCurrentOsVersionVista = FALSE;
	}

	lRet = RegOpenKeyExW(HKEY_USERS,L"",0,KEY_READ,&hKey);
	
	if (lRet != ERROR_SUCCESS)
	{
		return FALSE;
	}

	lRet = RegQueryInfoKeyW(
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
		return FALSE;
	}

	for (nIndex = 0;nIndex < cSubKeys;nIndex++)
	{
		dwSubLen = MAX_KEY_LENGTH;
		lRet = RegEnumKeyExW(hKey,nIndex,
			chSubKeyName,
			&dwSubLen,
			NULL,
			NULL,
			NULL,
			&ftLastWriteTime);
		if (lRet != ERROR_SUCCESS)
		{
			return FALSE;
		}
		else
		{
			if (dwSubLen > 16 && (wcsstr(chSubKeyName,L"Classes") == NULL))
			{
				wcscpy(chAllSIDName[nCurrentUserNum],chSubKeyName);
				nCurrentUserNum++;
			}
		}
	}

	RegCloseKey(hKey);

	for (i = 0;i < n;i += 4)
	{
		diskLetter[0] = driverLetters[i];
		
		if (GetDriveTypeW(&driverLetters[i]) == DRIVE_FIXED)
		{
			GetVolumeInformationW(&driverLetters[i],NULL,0,NULL,
				&MaximumConponentLenght,&FileSystemFlags,
				FileSystemNameBuffer,10);
			wcscpy(szRecyclePath,diskLetter);
			
			if (bCurrentOsVersionVista)
			{
				if (!wcscmp(FileSystemNameBuffer,L"NTFS"))
				{
					wcscat(szRecyclePath,L"$Recycle.Bin\\");
					
					for (j = 0;j < nCurrentUserNum;j++)
					{
						wcscpy(chTempPath,szRecyclePath);
						wcscat(chTempPath,chAllSIDName[j]);
						dwFileAttri = GetFileAttributesW(chTempPath);
						DeleteFileInRecycled(chTempPath,pBuf,bDel);
						SetFileAttributesW(chTempPath,dwFileAttri);
						dwFileAttri = GetFileAttributesW(szRecyclePath);
						SetFileAttributesW(szRecyclePath,dwFileAttri);
						ZeroMemory(chTempPath,MAX_PATH*sizeof(WCHAR));
					}
				}
				else
				{
					wcscat(szRecyclePath,L"$Recycle.Bin");
					dwFileAttri = GetFileAttributesW(szRecyclePath);
					DeleteFileInRecycled(szRecyclePath,pBuf,bDel);
					SetFileAttributesW(szRecyclePath,dwFileAttri);
				}
			}
			else
			{
				if (!wcscmp(FileSystemNameBuffer,L"NTFS"))
				{
					wcscat(szRecyclePath,L"RECYCLER\\");

					for (j = 0;j < nCurrentUserNum;j++)
					{
						wcscpy(chTempPath,szRecyclePath);
						wcscat(chTempPath,chAllSIDName[j]);
						dwFileAttri = GetFileAttributesW(chTempPath);
						DeleteFileInRecycled(chTempPath,pBuf,bDel);
						SetFileAttributesW(chTempPath,dwFileAttri);
						ZeroMemory(chTempPath,MAX_PATH*sizeof(WCHAR));
					}
				}
				else
				{
					wcscat(szRecyclePath,L"Recycled");
					dwFileAttri = GetFileAttributesW(szRecyclePath);
					DeleteFileInRecycled(szRecyclePath,pBuf,bDel);
					SetFileAttributesW(szRecyclePath,dwFileAttri);
				}
			}
		}
						
	}

	return TRUE;
}

BOOL DeleteFileInRecycled(LPCWSTR pszDir,BYTE *pBuf,BOOL bDel)
{
	WIN32_FIND_DATAW FindData;
	HANDLE hFind;
	DWORD dwAttrs;
	WCHAR recycledFilePath[MAX_PATH];
	WCHAR findFileName[MAX_PATH];
	WCHAR szDirName[MAX_PATH];
	WCHAR szTempDirectory[MAX_PATH];
	WCHAR *p;
	int i;

	if (g_bCancelDelete) return FALSE;
	wcscpy(recycledFilePath,pszDir);
	wcscat(recycledFilePath,L"\\*.*");
	hFind = FindFirstFileW(recycledFilePath,&FindData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	
	do
	{
		if ((wcscmp(FindData.cFileName,L".") != 0) && (wcscmp(FindData.cFileName,L"..")) != 0)
		{
			wcscpy(findFileName,FindData.cFileName);

			if (wcscmp(findFileName,L"desktop.ini") && wcscmp(findFileName,L"INFO2"))
			{
				wcscpy(szTempDirectory,pszDir);
				wcscat(szTempDirectory,L"\\");
				wcscat(szTempDirectory,findFileName);
				if (!(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					dwAttrs = GetFileAttributesW(szTempDirectory);
					SetFileAttributesW(szTempDirectory,dwAttrs);
					if (!DoDeleteFileInRecycled(szTempDirectory,pBuf,bDel))
					{
						SetFileAttributesW(pszDir,dwAttrs);
						if (bDel)
						{
							SYSTEMTIME CurrentTime;
							GetLocalTime(&CurrentTime);
							WriteLog(pszDir,&CurrentTime,&CurrentTime,FALSE);
						}
					}
				}
				else
				{
					DeleteFileInRecycled(szTempDirectory,pBuf,bDel);
				}
			}
		}
	}while(FindNextFileW(hFind,&FindData));

	FindClose(hFind);

	SetFileAttributesW(pszDir,FILE_ATTRIBUTE_NORMAL);
	
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

	return TRUE;
}

BOOL DoDeleteFileInRecycled(LPCWSTR pszDir,BYTE *pBuf, BOOL bDel)
{
	HANDLE		hFile;
	DWORD		dwRWSize,dwWrites;
	DWORD		dwRW,i;
	WCHAR		szFileName[MAX_PATH];
	WCHAR		*p;
	DWORD       dwFileSizeLo,dwFileSizeHigh;

	if(g_bCancelDelete)	return FALSE;

	hFile = CreateFileW( pszDir, 
						GENERIC_READ|GENERIC_WRITE, 
						FILE_SHARE_READ | FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING, 
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if (hFile == INVALID_HANDLE_VALUE) 
		return FALSE;
	dwFileSizeLo = GetFileSize(hFile,&dwFileSizeHigh);

	for(dwRWSize = 0;dwRWSize < dwFileSizeLo;dwRWSize += dwWrites)
	{
		if (dwFileSizeLo - dwRWSize < DM_DEL_SIZE) dwWrites = dwFileSizeLo - dwRWSize;
		else dwWrites = DM_DEL_SIZE;
		WriteFile(hFile,pBuf,dwWrites,&dwRW,NULL);
		
		if(g_bCancelDelete)
		{
			FlushFileBuffers(hFile);
			CloseHandle(hFile);
			hFile = INVALID_HANDLE_VALUE;
			DeleteFileW(pszDir);
			return FALSE;
		}
	}
	FlushFileBuffers(hFile);
	CloseHandle(hFile);
	if(bDel) 
	{
		wcscpy(szFileName,pszDir);
		p = GetFileNameFromPath(szFileName);
		p++;
		wcscpy(p,RENAME_STRING);
		MoveFileW(pszDir,szFileName);
		DeleteFileW(szFileName);
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
			DeleteFileW(szFileName);
		}
		p++;
		for(i=97;i<=122;i++)
		{
			*p = (WCHAR)i;
			DeleteFileW(szFileName);
		}
	}
	return TRUE;
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
	char szTemp[32];
	int i;
	int iSubAuthorityCount;
	SID_IDENTIFIER_AUTHORITY *psia = GetSidIdentifierAuthority( pSid );
	DWORD dwTopAuthority = psia->Value[5];
	sprintf(szBuffer, _T("S-1-%lu"), dwTopAuthority);

	iSubAuthorityCount = *(GetSidSubAuthorityCount(pSid));
	for (i = 0; i<iSubAuthorityCount; i++) 
	{
		DWORD dwSubAuthority = *(GetSidSubAuthority(pSid, i));
		sprintf(szTemp, _T("%lu"), dwSubAuthority);
		strcat(szBuffer, _T("-"));
		strcat(szBuffer, szTemp);
	}
}

BOOL RecordSetting(PSCHEDULE_ITEM pScheduleItemArray)
{
	HANDLE hSettingFile;
	DWORD dwWrittenBytes;
	SETTING_FILE_HEAD SettingFileHead;
	DWORD nItemIndex;
	char  strSettingFileName[MAX_PATH];

	if(!GetSettingFileName(strSettingFileName) ) return FALSE;

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
		return FALSE;
	}

	if( FALSE == ReadFile(hSettingFile,&SettingFileHead,sizeof(SettingFileHead),&dwWrittenBytes,NULL) )
	{
		CloseHandle(hSettingFile);
		return FALSE;
	}

	for(nItemIndex=0; nItemIndex<SettingFileHead.dwSettingCount; nItemIndex++)
	{
		
		if( FALSE == WriteFile(hSettingFile,&pScheduleItemArray[nItemIndex],sizeof(SCHEDULE_ITEM),&dwWrittenBytes,NULL) )
		{
			CloseHandle(hSettingFile);
			return FALSE;
		}
	}

	CloseHandle(hSettingFile);
	return TRUE;
}

ULONG ReadSetting(PSCHEDULE_ITEM *pScheduleItemArray)
{
	HANDLE hSettingFile;
	ULONG ulScheduleNum = 0;
	DWORD dwReadBytes;
	SETTING_FILE_HEAD SettingFileHead;
	char  strSettingFileName[MAX_PATH];

	if(!GetSettingFileName(strSettingFileName)) return ulScheduleNum;

    if(!IsFileScheduleSettingFile( (LPCTSTR)strSettingFileName) )
		return ulScheduleNum;

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
		return ulScheduleNum;

	if(!ReadFile(hSettingFile,&SettingFileHead,sizeof(SETTING_FILE_HEAD),&dwReadBytes,NULL) )
	{
		CloseHandle(hSettingFile);
		return ulScheduleNum;
	}

	if (SettingFileHead.dwSettingCount)
	{
		*pScheduleItemArray = (PSCHEDULE_ITEM)malloc(SettingFileHead.dwSettingCount * sizeof(SCHEDULE_ITEM));
		if (*pScheduleItemArray)
		{
			DWORD dwItemIndex;
			for(dwItemIndex=0; dwItemIndex<SettingFileHead.dwSettingCount; dwItemIndex++)
			{
				if(!ReadFile(hSettingFile,&(*pScheduleItemArray)[dwItemIndex],sizeof(SCHEDULE_ITEM),&dwReadBytes,NULL))
				{
					CloseHandle(hSettingFile);
					return ulScheduleNum;
				}
				ulScheduleNum ++;
			}
		}
	}

	CloseHandle(hSettingFile);
	return ulScheduleNum;
}

BOOL GetSettingFileName(char *strProgSettingDataPath)
{
	BOOL bDefCharUsed;
	WCHAR szModuleName[MAX_PATH];
	WCHAR *p;

	GetModuleFileNameW(NULL,szModuleName,MAX_PATH);

	p = GetFileNameFromPath(szModuleName);
	if (p) *p = '\0';

	WideCharToMultiByte(CP_OEMCP,0,szModuleName,-1,strProgSettingDataPath, MAX_PATH, NULL, &bDefCharUsed);

	strcat(strProgSettingDataPath,"\\");
	strcat(strProgSettingDataPath,SETTING_DATA_FILE);

	return TRUE;
}

BOOL IsFileScheduleSettingFile(LPCTSTR szSettingFileName)
{
	// the setting file should be exist

	HANDLE hSettingFile;
	SETTING_FILE_HEAD SettingFileHead;
	DWORD dwReadBytes;

	if (GetFileAttributes(szSettingFileName) == -1)
		return FALSE;

	hSettingFile = CreateFile( szSettingFileName,GENERIC_READ,FILE_SHARE_READ,NULL,
		                       OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if( INVALID_HANDLE_VALUE == hSettingFile )
		return FALSE;

	if(!ReadFile(hSettingFile,&SettingFileHead,sizeof(SETTING_FILE_HEAD),&dwReadBytes,NULL) )
	{
		CloseHandle(hSettingFile);
		return FALSE;
	}

	if(wcscmp(SettingFileHead.szSettingIdentity,SETTING_FILE_IDENTITY) )
	{
		CloseHandle(hSettingFile);
		return FALSE;
	}

	CloseHandle(hSettingFile);

	return TRUE;
}

void  FileTimeToTime_t(  FILETIME  ft,  time_t  *t  )  
{  
     LONGLONG  ll;  
 
     ULARGE_INTEGER            ui;  
     ui.LowPart            =  ft.dwLowDateTime;  
     ui.HighPart            =  ft.dwHighDateTime;  

     ll =  (ft.dwHighDateTime  <<  32)  +  ft.dwLowDateTime;  
     *t =  (LONG)((LONGLONG)(ui.QuadPart  -  116444736000000000)  /  10000000);  
}

LONG  GetDiffDays(  SYSTEMTIME  *pt1,  SYSTEMTIME  *pt2  )  
{  
	FILETIME            fTime1;  
	FILETIME            fTime2;  

	time_t            tt1;  
	time_t            tt2;  
	time_t            difftime;

	SystemTimeToFileTime(  pt1,  &fTime1  );  
	SystemTimeToFileTime(  pt2,  &fTime2  );  

	FileTimeToTime_t(  fTime1,  &tt1  );  
	FileTimeToTime_t(  fTime2,  &tt2  );  

	difftime            =  tt2  -  tt1;  

	return  difftime  /  (24*3600L);//  除以每天24小时3600秒  
}  

time_t GetLastExcuteTime(SYSTEMTIME *pSystemTime,PSCHEDULE_ITEM  pSchedule)
{
	int nHour,nMin;
	FILETIME            fTime1;  
	time_t            tt1;  
	LONG				lDays,lMonth,lYear;
	BOOL				bMatchTime;
	SYSTEMTIME ExcuteTime;

	nHour = (pSchedule->ExecuteTime & 0xff0000)/0x10000;
	nMin = (pSchedule->ExecuteTime & 0xff00)/0x100;
	ExcuteTime = *pSystemTime;

	ExcuteTime.wHour = nHour;
	ExcuteTime.wMinute = nMin;
	SystemTimeToFileTime(  &ExcuteTime,  &fTime1  );  
	FileTimeToTime_t(  fTime1,  &tt1  );  

	bMatchTime = (pSystemTime->wHour > nHour) || ((pSystemTime->wHour == nHour) && (pSystemTime->wMinute > nMin));

	switch (pSchedule->ulDateType)
	{
	case TIME_DAY_OF_WEEK:
		if (pSchedule->ulDate == pSystemTime->wDayOfWeek)//today
		{
			if (bMatchTime)	lDays = 0;
			else lDays = 7;
		}
		else
		{
			lDays = pSystemTime->wDayOfWeek - pSchedule->ulDate;
			if (lDays < 0) lDays += 7;
		}
		break;
	case TIME_DAY_OF_MONTH:
		lMonth = pSystemTime->wMonth - 1;
		lYear = pSystemTime->wYear;
		if (!lMonth) 
		{
			lMonth = 12;
			lYear --;
		}

		switch (lMonth)
		{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			lMonth = 31;
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			lMonth = 31;
			break;
		case 2:
			lMonth = 28;
			if (!(lYear % 4))
			{
				lMonth = 29;
				if (!(lYear % 100) && (lYear % 400))
					lMonth = 28;
			}
			break;
		}

		if (pSchedule->ulDate == pSystemTime->wDay)//today
		{
			if (bMatchTime)	lDays = 0;
			else lDays = lMonth;
		}
		else
		{
			lDays = pSystemTime->wDay - pSchedule->ulDate;
			if (lDays < 0) lDays += lMonth;
		}
		break;
	default:
		return 0;
	}

	tt1 = tt1 - lDays * (24*3600L);

	return tt1;
}

BOOL ExcuteLostSchedule(SYSTEMTIME *pSystemTime)
{
	SYSTEMTIME      StartTime,EndTime;
	PSCHEDULE_ITEM  pSchedule = NULL;
	ULONG			ulTotalSchedule,i;
	int				bSucc = 0;
	FILETIME            fTime1;  
	time_t            tt1,tt2;  

	ulTotalSchedule = ReadSetting(&pSchedule);

	for (i = 0;i < ulTotalSchedule;i ++)
	{
		tt2 = GetLastExcuteTime(pSystemTime,&pSchedule[i]);

		SystemTimeToFileTime(  &pSchedule[i].LastExcuteTime,  &fTime1  );  
		FileTimeToTime_t(  fTime1,  &tt1  );  

		if (tt1 < tt2)
		{
			GetLocalTime(&StartTime);
			switch (pSchedule[i].ulDeleteObjectType)
			{
			case DELETE_OBJECT_NORMAL:
				bSucc = DoDeleteMasterDel(pSchedule[i].szPathFile,(int)pSchedule[i].ulDeleteMethod);
				break;
			case DELETE_OBJECT_RECYCLER:
				bSucc = DoDeleteMasterDelFileInRecycled((int)pSchedule[i].ulDeleteMethod);
				break;
			}
			GetLocalTime(&EndTime);
			if (pSchedule[i].bRecordLog)
			{
				WriteLog(pSchedule[i].szPathFile,&StartTime,&EndTime,bSucc);
			}
			GetLocalTime(&pSchedule[i].LastExcuteTime);
			RecordSetting(pSchedule);
		}
	}

	if (pSchedule) free(pSchedule);
	return TRUE;
}

BOOL ExcuteSchedule(SYSTEMTIME *pSystemTime)
{
	SYSTEMTIME      StartTime,EndTime;
	PSCHEDULE_ITEM  pSchedule = NULL;
	ULONG			ulTotalSchedule,i;
	BOOL            bMatchDate = FALSE,bSucc;

	ulTotalSchedule = ReadSetting(&pSchedule);

	for (i = 0;i < ulTotalSchedule;i ++)
	{
		switch (pSchedule[i].ulDateType)
		{
		case TIME_DAY_OF_WEEK:
			bMatchDate = ((ULONG)pSystemTime->wDayOfWeek == pSchedule[i].ulDate);
			break;
		case TIME_DAY_OF_MONTH:
			bMatchDate = ((ULONG)pSystemTime->wDay == pSchedule[i].ulDate);
			break;
		}
		if (bMatchDate)
		{
			if (pSystemTime->wHour == ((pSchedule[i].ExecuteTime & 0xff0000)/0x10000) && 
				pSystemTime->wMinute == ((pSchedule[i].ExecuteTime & 0xff00)/0x100))
			{
				GetLocalTime(&StartTime);
				switch (pSchedule[i].ulDeleteObjectType)
				{
				case DELETE_OBJECT_NORMAL:
					bSucc = DoDeleteMasterDel(pSchedule[i].szPathFile,(int)pSchedule[i].ulDeleteMethod);
					break;
				case DELETE_OBJECT_RECYCLER:
					bSucc = DoDeleteMasterDelFileInRecycled((int)pSchedule[i].ulDeleteMethod);
					break;
				}
				
				GetLocalTime(&EndTime);
				if (pSchedule[i].bRecordLog)
				{
					WriteLog(pSchedule[i].szPathFile,&StartTime,&EndTime,bSucc);
				}
				GetLocalTime(&pSchedule[i].LastExcuteTime);
				RecordSetting(pSchedule);
			}
		}
	}

	if (pSchedule) free(pSchedule);
	return TRUE;
}

BOOL WriteLog(LPCWSTR szFile,SYSTEMTIME *pStartTime,SYSTEMTIME *pEndTime,BOOL bSucc)
{
	WCHAR    strText[MAX_PATH*2];
	WCHAR    szUserName[MAX_PATH],szLogFile[MAX_PATH];
	HANDLE  hFile;
	WORD    wUnicodeSign = 0;
	ULONG	ulEnter = 0x000a000d;
	ULONG	ulLen = MAX_PATH,dwWrtited;

	GetUserNameW(szUserName,&ulLen);

	if (bSucc)
	{
		swprintf(strText,IDS_LOG_FORMAT,szFile,szUserName,
					pStartTime->wYear,pStartTime->wMonth,pStartTime->wDay,
					pStartTime->wHour,pStartTime->wMinute,pStartTime->wSecond,
					pEndTime->wYear,pEndTime->wMonth,pEndTime->wDay,
					pEndTime->wHour,pEndTime->wMinute,pEndTime->wSecond,
					L"Success");
	}
	else
	{
		swprintf(strText,IDS_LOG_FORMAT,szFile,szUserName,
					pStartTime->wYear,pStartTime->wMonth,pStartTime->wDay,
					pStartTime->wHour,pStartTime->wMinute,pStartTime->wSecond,
					pEndTime->wYear,pEndTime->wMonth,pEndTime->wDay,
					pEndTime->wHour,pEndTime->wMinute,pEndTime->wSecond,
					L"Fail");
	}

	GetSystemDirectoryW(szLogFile,MAX_PATH);
	wcscat(szLogFile,L"\\dm.log");

	hFile = CreateFileW(szLogFile,GENERIC_READ|GENERIC_WRITE,
					 FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
					 OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,
					 NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	ReadFile(hFile,&wUnicodeSign,sizeof(WORD),&dwWrtited,NULL);
	if (wUnicodeSign != 0xfeff)
	{
		wUnicodeSign = 0xfeff;
		SetFilePointer(hFile,0,NULL,FILE_BEGIN);
		WriteFile(hFile,&wUnicodeSign,sizeof(WORD),&dwWrtited,NULL);
	}
	else SetFilePointer(hFile,0,NULL,FILE_END);
	WriteFile(hFile,strText,wcslen(strText)*sizeof(WCHAR),&dwWrtited,NULL);
	WriteFile(hFile,&ulEnter,sizeof(DWORD),&dwWrtited,NULL);
	SetEndOfFile(hFile);
	CloseHandle(hFile);
	return TRUE;
}

WCHAR *GetFileNameFromPath(WCHAR *szPath)
{
	DWORD dwFileAttr;
	WCHAR *p,szBkPath[MAX_PATH];
	wcscpy(szBkPath,szPath);
	do
	{
		p = wcsrchr(szBkPath,'\\');
		if (!p) return NULL;
		else
		{
			*p = '\0';
			dwFileAttr = GetFileAttributesW(szBkPath);
			if (dwFileAttr != -1 && dwFileAttr & FILE_ATTRIBUTE_DIRECTORY)
			{
				return &szPath[wcslen(szBkPath)];
			}
		}
	}while (TRUE);
	return NULL;
}

BOOL GetTextualSid(
    PSID pSid,            // binary SID
    LPTSTR TextualSid,    // buffer for Textual representation of SID
    LPDWORD lpdwBufferLen // required/provided TextualSid buffersize
    )
{
    PSID_IDENTIFIER_AUTHORITY psia;
    DWORD dwSubAuthorities;
    DWORD dwSidRev=SID_REVISION;
    DWORD dwCounter;
    DWORD dwSidSize;

    // Validate the binary SID.

    if(!IsValidSid(pSid)) return FALSE;

    // Get the identifier authority value from the SID.

    psia = GetSidIdentifierAuthority(pSid);

    // Get the number of subauthorities in the SID.

    dwSubAuthorities = *GetSidSubAuthorityCount(pSid);

    // Compute the buffer length.
    // S-SID_REVISION- + IdentifierAuthority- + subauthorities- + NULL

    dwSidSize=(15 + 12 + (12 * dwSubAuthorities) + 1) * sizeof(TCHAR);

    // Check input buffer length.
    // If too small, indicate the proper size and set the last error.

    if (*lpdwBufferLen < dwSidSize)
    {
        *lpdwBufferLen = dwSidSize;
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return FALSE;
    }

    // Add 'S' prefix and revision number to the string.

    dwSidSize=sprintf(TextualSid, TEXT("S-%lu-"), dwSidRev );

    // Add a SID identifier authority to the string.

    if ( (psia->Value[0] != 0) || (psia->Value[1] != 0) )
    {
        dwSidSize+=sprintf(TextualSid + lstrlen(TextualSid),
                    TEXT("0x%02hx%02hx%02hx%02hx%02hx%02hx"),
                    (USHORT)psia->Value[0],
                    (USHORT)psia->Value[1],
                    (USHORT)psia->Value[2],
                    (USHORT)psia->Value[3],
                    (USHORT)psia->Value[4],
                    (USHORT)psia->Value[5]);
    }
    else
    {
        dwSidSize+=sprintf(TextualSid + lstrlen(TextualSid),
                    TEXT("%lu"),
                    (ULONG)(psia->Value[5]      )   +
                    (ULONG)(psia->Value[4] <<  8)   +
                    (ULONG)(psia->Value[3] << 16)   +
                    (ULONG)(psia->Value[2] << 24)   );
    }

    // Add SID subauthorities to the string.
    //
    for (dwCounter=0 ; dwCounter < dwSubAuthorities ; dwCounter++)
    {
        dwSidSize+=sprintf(TextualSid + dwSidSize, TEXT("-%lu"),
                    *GetSidSubAuthority(pSid, dwCounter) );
    }

    return TRUE;
}

BOOL GetCurrentUserForSID (VOID) 
{
   DWORD dwSize = 0, dwResult = 0;
   HANDLE hToken;
   PTOKEN_USER pUserInfo;
   DWORD dwBufferSize = MAX_PATH;
 
   LPTSTR  StringSid = (LPTSTR)malloc(MAX_PATH);

   if (!OpenProcessToken( GetCurrentProcess(), TOKEN_QUERY, &hToken )) 
       return FALSE;

   if(!GetTokenInformation(hToken, TokenUser, NULL, dwSize, &dwSize)) 
   {
       dwResult = GetLastError();
       if( dwResult != ERROR_INSUFFICIENT_BUFFER )
           return FALSE;
   }

   pUserInfo = (PTOKEN_USER) GlobalAlloc( GPTR, dwSize );

   if(! GetTokenInformation(hToken, TokenUser, pUserInfo, 
       dwSize, &dwSize ) ) 
   {
       return FALSE;
   }

//   ConvertSidToStringSid(pUserInfo->User.Sid, &StringSid);

   GetTextualSid(pUserInfo->User.Sid,StringSid,&dwBufferSize);
   strcpy(chSID,StringSid);
   free(StringSid);

   if ( pUserInfo )
       GlobalFree( pUserInfo );
   return TRUE;
}
   
