// SecureDelete.cpp: implementation of the CSecureDelete class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "SecureDelete.h"
#include "resource.h"
#include "DMF.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern BOOL    g_bBeepOff;

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSecureDelete::CSecureDelete():DM_DEL_SIZE(64*1024),BYTEINSEC(512)
{
	m_nPercent=0;
	m_qwDelSize=0;
	m_qwTotalSize=0;
	m_bCancelDelete = FALSE;
	wcscpy(RENAME_STRING,L"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
}

CSecureDelete::~CSecureDelete()
{

}

BOOL CSecureDelete::DoDelFile(WCHAR *pszDir, BYTE *pBuf,BOOL bDel)
{
	HANDLE		hFile;
	DWORD		i,j;
	DWORD		dwRWTimes;
	DWORD		dwRW;
	int         nPercent;
	DWORD       dwFileSizeLo,dwFileSizeHigh;

	if(m_bCancelDelete)	return FALSE;
	
	WCHAR		szFileName[MAX_PATH];
	WCHAR		*p;

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

	i = dwFileSizeLo / DM_DEL_SIZE + dwFileSizeHigh * 0x4000;
	j = dwFileSizeLo % DM_DEL_SIZE; 

	m_strCurrentFile = pszDir;

	for(dwRWTimes=0;dwRWTimes<i;dwRWTimes++)
	{
		// WriteFile(hFile,m_pBuf,DM_DEL_SIZE,&dwRW,NULL);
		WriteFile(hFile,pBuf,DM_DEL_SIZE,&dwRW,NULL);
		
		// now we do not consider progress bar temporary
		m_qwDelSize += DM_DEL_SIZE;
		
		// if m_qwTotalSize is initialized
		if(m_qwTotalSize>0)
		{
		    nPercent = (int)(m_qwDelSize*100/m_qwTotalSize);
			if (m_nPercent != nPercent)
			{
				if (nPercent - m_nPercent > 2)
				{
					for (;m_nPercent < nPercent;m_nPercent += 2)
					{
						cout << '>';
						cout.flush();
					}
				}
			}
			if (nPercent == 100)	cout << endl;
		}
		else
		{
		    m_nPercent = 0;
		}

		if(m_bCancelDelete)
		{
			FlushFileBuffers(hFile);
			CloseHandle(hFile);
			hFile = INVALID_HANDLE_VALUE;
			
			// delete the current deleting file
			DeleteFileW(pszDir);
			return TRUE;
		}
		
		// m_Progress.SetPos(nPresent);
	}
	if(j)
	{
		WriteFile(hFile,pBuf,j,&dwRW,NULL);
		
		// now we do not consider progress bar temporary
		m_qwDelSize += j;
		
		if(m_qwTotalSize>0)
		{
		    nPercent = (int)(m_qwDelSize*100/m_qwTotalSize);
			if (m_nPercent != nPercent)
			{
				if (nPercent - m_nPercent > 2)
				{
					for (;m_nPercent < nPercent;m_nPercent += 2)
					{
						cout << '>';
						cout.flush();
					}
				}
			}
			if (nPercent == 100)	cout << endl;
		}
		else
		{
		    m_nPercent = 0;
		}

		if(m_bCancelDelete)
		{
			FlushFileBuffers(hFile);
			CloseHandle(hFile);
			hFile = INVALID_HANDLE_VALUE;
			
			DeleteFileW(pszDir);
			return TRUE;
		}

		// m_Progress.SetPos(nPresent);
	}
	FlushFileBuffers(hFile);
	CloseHandle(hFile);
	if(bDel) 
	{
		wcscpy(szFileName,pszDir);
		p = wcsrchr(szFileName,'\\');
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
			*p = (BYTE)i;
			DeleteFileW(szFileName);
		}
	}
	return TRUE;
}

BOOL CSecureDelete::DoDeleteMasterDel(WCHAR *szDirName, int nDelMethod)
{
	int			i;
	BOOL        bResult = FALSE;
	CString		csText,csCaption;

	InitDeleteMasterDel(szDirName);
	if (!wcsicmp(szDirName,L"Recycler"))
		return DoDeleteMasterDelFileInRecycled(nDelMethod);

	csCaption.LoadString (IDS_PRODUCT_NAME);

	BYTE *pBuf;
	
	pBuf = (BYTE *)malloc(DM_DEL_SIZE);
	
	if(!pBuf) 
	{
		csText.LoadString (IDS_NOT_ENOUGH_MEMORY);
		wcout << (LPCTSTR)csText << endl;
		return FALSE;
	}

	switch(nDelMethod)
	{
	case 0:
		InitDelBuf(pBuf,0,0,0,FALSE,TRUE);
		bResult = DelAll(szDirName,pBuf,TRUE);
		break;
	case 1:
		InitDelBuf(pBuf,0XFF,0,0,FALSE,TRUE);
		bResult = DelAll(szDirName,pBuf,TRUE);
		break;
	case 2:
		InitDelBuf(pBuf,0,0,0,TRUE,TRUE);
		bResult = DelAll(szDirName,pBuf,TRUE);
		break;
	case 3:
		m_qwTotalSize = m_qwTotalSize *3;
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
				if(!DelAll(szDirName,pBuf,TRUE))
					break;
			}
			else
			{
				if(!DelAll(szDirName,pBuf,FALSE))
					break;
			}
		}
		bResult = TRUE;
		break;
	case 4:
		// do not consider progress bar tempory
		m_qwTotalSize = m_qwTotalSize *4;
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
				if(!DelAll(szDirName,pBuf,TRUE))
					break;
			}
			else
			{
				if(!DelAll(szDirName,pBuf,FALSE))
					break;
			}
		}
		bResult = TRUE;
		break;
	case 5:
		// do not consider progress bar temporary
		m_qwTotalSize = m_qwTotalSize *3;
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
				if(!DelAll(szDirName,pBuf,TRUE)) break;
			}
			else
			{
				if(!DelAll(szDirName,pBuf,FALSE)) break;
			}
		}
		bResult = TRUE;
		break;
	case 6:
		
		// now do not consider progress bar temporary
		m_qwTotalSize = m_qwTotalSize *7;
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
				if(!DelAll(szDirName,pBuf,TRUE)) break;
			}
			else
			{
				if(!DelAll(szDirName,pBuf,FALSE)) break;
			}
		}
		bResult = TRUE;
		break;
	case 7:
		
		// now do not conside progress bar temporary
		m_qwTotalSize = m_qwTotalSize *35;
		
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
				if(!DelAll(szDirName,pBuf,TRUE)) break;
			}
			else
			{
				if(!DelAll(szDirName,pBuf,FALSE)) break;
			}
		}
		bResult = TRUE;
		break;
	}
	free(pBuf);
	pBuf = NULL;
	
	return bResult;
}

BOOL CSecureDelete::DelDirAndFiles(WCHAR *pszDir,
								   BYTE *pBuf,
								   BOOL bDel,
								   WCHAR *pszSaveDir,
								   BOOL bSave)
{
    WIN32_FIND_DATAW     FindData;
	HANDLE              hFind   ;
	DWORD				dwAttrs;
	int					i;
	WCHAR				*p;
	int					nDirEnd,nSaveDirEnd;
	WCHAR				szDirOrFileName[MAX_PATH];
	WCHAR				szSaveDir[MAX_PATH];
	WCHAR				szSaveDirOrFileName[MAX_PATH];
	WCHAR				szDirName[MAX_PATH];

	if(m_bCancelDelete)	return TRUE;

	if ( NULL == pszDir)
	{
		return FALSE ;
	}
	hFind = FindFirstFileW(pszDir,&FindData);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		return FALSE;
	}
  	
	if(bSave)
	{
		wcscpy( szSaveDirOrFileName , pszSaveDir) ;
		wcscat( szSaveDirOrFileName , L"\\" ) ;
		nSaveDirEnd = wcslen( szSaveDirOrFileName);
		p = wcsrchr(pszDir,'\\');
		p++;
		wcscpy( &szSaveDirOrFileName[nSaveDirEnd] ,p);
	}

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
	{
		return  FALSE ;
	}
	
	do
    {
		if (wcscmp(FindData.cFileName,L".") && 
			wcscmp(FindData.cFileName,L".."))
		{
			wcscpy( &szDirOrFileName[nDirEnd] ,FindData.cFileName);

			if ( !(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) 
			{
				dwAttrs = GetFileAttributesW(szDirOrFileName);
				SetFileAttributesW(szDirOrFileName,FILE_ATTRIBUTE_NORMAL);
				if(!DoDelFile(szDirOrFileName,pBuf,bDel))
				{
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
				if (!DelDirAndFiles(szDirOrFileName,pBuf,bDel,szSaveDir,bSave))
				{
					  FindClose( hFind );
					  return FALSE ;
				}
			}
		}
	} while( FindNextFileW( hFind ,&FindData));
	FindClose( hFind);
	if(bDel)
	{
		RemoveDirectoryW(pszDir);

		wcscpy(szDirName,pszDir);
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
    return  TRUE;
}

BOOL CSecureDelete::InitDelBuf(BYTE *pBuf, BYTE btFir, BYTE btSec, BYTE btThr, BOOL bRand, BOOL bOneByte)
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

BOOL CSecureDelete::DelAll(WCHAR *szDirName, BYTE *pBuf, BOOL bDel)
{
	if(m_bCancelDelete)	return FALSE;

	WCHAR szTempDir[MAX_PATH];
	GetTempPathW(MAX_PATH,szTempDir);
	DelDirAndFiles(szDirName,pBuf,bDel,szTempDir,FALSE);
	return TRUE;
}

BOOL CSecureDelete::GetDirFileSize(WCHAR *pszDir)
{
    WIN32_FIND_DATAW     FindData;
	HANDLE              hFind   ;
	int					nDirEnd;
	WCHAR				szDirOrFileName[MAX_PATH];

	if ( NULL == pszDir)
	{
		return FALSE ;
	}
	hFind = FindFirstFileW(pszDir,&FindData);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		return FALSE;
	}
	if ( !(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
	{
		m_qwTotalSize += FindData.nFileSizeLow;
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
	{
		return  FALSE ;
	}
	
	do
    {
		if (wcscmp(FindData.cFileName,L".") && 
			wcscmp(FindData.cFileName,L".."))
		{
			wcscpy( &szDirOrFileName[nDirEnd] ,FindData.cFileName);
			if ( !(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) 
			{
				m_qwTotalSize += FindData.nFileSizeLow;
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

int CSecureDelete::GetDeletePercent()
{
	return m_nPercent;
}

void CSecureDelete::SetCancelDelete()
{
	m_bCancelDelete = TRUE;
}

BOOL CSecureDelete::InitDeleteMasterDel(WCHAR *szDirName)
{
	m_bCancelDelete = FALSE;
	m_nPercent = 0;

	m_qwDelSize = 0;
	m_qwTotalSize = 0;

	m_strCurrentFile.Empty();
	m_strOriginalDir = szDirName;

	if( FALSE == GetDirFileSize(szDirName) )
	{
		TRACE(L"\nGetDirFileSize error in InitDelteMasterDel.");
		return FALSE;
	}

	return TRUE;

}

void CSecureDelete::GetCurrentDeleteFile(CString &strCurrentDeleteFile)
{
	strCurrentDeleteFile = m_strCurrentFile;
}

BOOL CSecureDelete::IsDirectory(LPCTSTR szDirectoryName)
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


BOOL CSecureDelete::DoDeleteMasterDelFileInRecycled(int nDelMethod)
{
	int			i;
	BOOL        bResult = FALSE;
	BYTE *pBuf;

	m_bCancelDelete = FALSE;
	
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

BOOL CSecureDelete::DelAllInRecycled(BYTE *pBuf,BOOL bDel)
{
	WCHAR driverLetters[MAX_PATH];
	WCHAR diskLetter[4] = L"C:\\";
	WCHAR szRecyclePath[MAX_PATH];
	int n;
	int i;
	PSID pSID = NULL;
	WCHAR szSID[MAX_PATH];
	WCHAR  FileSystemNameBuffer[10];
	DWORD MaximumConponentLenght;
	DWORD FileSystemFlags;

	::GetLogicalDriveStrings(MAX_PATH,driverLetters);
	n = sizeof(driverLetters);

	if (GetUserSid(&pSID))
	{
		//get the hidden key name
		GetSidString(pSID, szSID);
	}

	for (i = 0;i < n;i += 4)
	{
		diskLetter[0] = driverLetters[i];
		
		if (GetDriveType(&driverLetters[i]) == DRIVE_FIXED)
		{
			GetVolumeInformation(&driverLetters[i],NULL,0,NULL,
				&MaximumConponentLenght,&FileSystemFlags,
				FileSystemNameBuffer,10);
			wcscpy(szRecyclePath,diskLetter);

			if (!wcsicmp(FileSystemNameBuffer,L"NTFS"))
			{
				wcscat(szRecyclePath,L"RECYCLER\\");
				wcscat(szRecyclePath,szSID);
			}
			else
			{
				wcscat(szRecyclePath,L"Recycled");
			}

			DeleteFileInRecycled(szRecyclePath,pBuf,bDel);
		}
	}

	return TRUE;
}

BOOL CSecureDelete::DeleteFileInRecycled(WCHAR *pszDir,BYTE *pBuf,BOOL bDel)
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

	if (m_bCancelDelete) return FALSE;
	wcscpy(recycledFilePath,pszDir);
	wcscat(recycledFilePath,L"\\*.*");
	hFind = FindFirstFileW(recycledFilePath,&FindData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		return TRUE;
	}
	
	do
	{
		if ((wcscmp(FindData.cFileName,L".") != 0) && (wcscmp(FindData.cFileName,L"..")) != 0)
		{
			wcscpy(findFileName,FindData.cFileName);

			if (wcscmp(findFileName,L"desktop.ini") && wcscmp(findFileName,L"INFO2"))
			{
				if (!(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					swprintf(szTempDirectory,L"%s\\%s",pszDir,findFileName);
					dwAttrs = GetFileAttributesW(szTempDirectory);
					SetFileAttributesW(szTempDirectory,dwAttrs);
					if (!DoDeleteFileInRecycled(szTempDirectory,pBuf,bDel))
					{
						SetFileAttributesW(pszDir,dwAttrs);
						if (bDel)
						{
							CTime  CurrentTime;
							CurrentTime = CTime::GetCurrentTime();
							WriteLog(pszDir,CurrentTime,CurrentTime,FALSE);
						}
					}
				}
				else
				{
					swprintf(szTempDirectory,L"%s\\%s",pszDir,findFileName);
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

	return TRUE;
}

BOOL CSecureDelete::DoDeleteFileInRecycled(WCHAR *pszDir,BYTE *pBuf, BOOL bDel)
{
	HANDLE		hFile;
	DWORD		dwRWSize,dwWrites;
	DWORD		dwRW,i;
	WCHAR		szFileName[MAX_PATH];
	WCHAR		*p;
	DWORD       dwFileSizeLo,dwFileSizeHigh;

	if(m_bCancelDelete)	return FALSE;

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
		
		if(m_bCancelDelete)
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

