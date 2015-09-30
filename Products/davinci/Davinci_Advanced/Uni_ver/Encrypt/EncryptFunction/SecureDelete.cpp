// SecureDelete.cpp: implementation of the CSecureDelete class.
//
//////////////////////////////////////////////////////////////////////

#include "SecureDelete.h"

#ifdef _FILE_ENCRYPT
#include "..\FileEncrypt\resource.h"
#endif

#ifdef _SHELL_ENCRYPT
#include "..\ShellEncrypt\resource.h"
#endif

#ifdef _YG_COMMAND_LINE
#include "..\..\FED\resource.h"
#include "IOSTREAM.H"
#endif

#ifdef _YG_FS_MON
#include "..\..\YGFSMon\resource.h"
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSecureDelete::CSecureDelete():DM_DEL_SIZE(64*1024),BYTEINSEC(512)
{
	m_nPercent=0;
	m_qwDelSize=0;
	m_qwTotalSize=0;
	m_bCancelDelete = FALSE;
	strncpy(RENAME_STRING,"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$",sizeof(RENAME_STRING)-1);
}

CSecureDelete::~CSecureDelete()
{

}

BOOL CSecureDelete::DoDelFile(LPTSTR pszDir, BYTE *pBuf, DWORD dwFileSize, BOOL bDel)
{
	HANDLE		hFile;
	DWORD		i,j;
	DWORD		dwRWTimes;
	DWORD		dwRW;
	int         nPercent;

	if(m_bCancelDelete)
	{
		// if cancel here, delete the curerent file
		// since the current file may be not integrate
		if( FALSE == IsDirectory(pszDir) )
		{
			SetFileAttributes(pszDir,FILE_ATTRIBUTE_NORMAL);
			if( FALSE == DeleteFile(pszDir) )
			{
				TRACE("\nDelete File %s error in DelAll",pszDir);
			}
		}
		TRACE("\nCancel in DoDelFile start.");
		return FALSE;
	}
	
	char		szFileName[MAX_PATH];
	char		*p;

	hFile = CreateFile( pszDir, 
						GENERIC_READ|GENERIC_WRITE, 
						FILE_SHARE_READ |FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING, 
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if (hFile == INVALID_HANDLE_VALUE) 
		return FALSE;

	i = dwFileSize / DM_DEL_SIZE;
	j = dwFileSize % DM_DEL_SIZE; 

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
#ifdef _YG_COMMAND_LINE
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
#else
			m_nPercent = nPercent;
#endif
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
			SetFileAttributes(pszDir,FILE_ATTRIBUTE_NORMAL);
			if( FALSE == DeleteFile(pszDir) )
			{
				TRACE("\nDeleteFile %serror in DoDelFile.",pszDir);
			}
			TRACE("\nCancel in DoDelFile middle.");
			return FALSE;
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
#ifdef _YG_COMMAND_LINE
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
#else
			m_nPercent = nPercent;
#endif
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
			SetFileAttributes(pszDir,FILE_ATTRIBUTE_NORMAL);
			if( FALSE == DeleteFile(pszDir) )
			{
				TRACE("\nDeleteFile %serror in DoDelFile.",pszDir);
			}
			TRACE("\nCancel in DoDelFile middle 2.");
			return FALSE;
		}

		// m_Progress.SetPos(nPresent);
	}
	FlushFileBuffers(hFile);
	CloseHandle(hFile);
	if(bDel) 
	{
		strcpy(szFileName,pszDir);
		p = strrchr(szFileName,'\\');
		p++;
		strcpy(p,RENAME_STRING);
		_trename(pszDir,szFileName);
		DeleteFile(szFileName);
		for(i=65;i<=90;i++)
		{
			*p = (BYTE)i;
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
			*p = (BYTE)i;
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
		strcpy(p,RENAME_STRING);
		for(i=65;i<=90;i++)
		{
			*p = (BYTE)i;
			DeleteFile(szFileName);
		}
		p++;
		for(i=97;i<=122;i++)
		{
			*p = (BYTE)i;
			DeleteFile(szFileName);
		}
	}
	return TRUE;
}

BOOL CSecureDelete::DoDeleteMasterDel(char *szDirName, int nDelMethod)
{
	int			i;
	CString		csText,csCaption;

#ifdef _YG_COMMAND_LINE
	csText.Format(IDS_START_SECDEL,szDirName);
	cout << (LPCTSTR)csText << endl;
#endif
	InitDeleteMasterDel(szDirName);

	csCaption.LoadString (IDS_PRODUCT_NAME);

	BYTE *pBuf;
	
	pBuf = (BYTE *)malloc(DM_DEL_SIZE);
	
	if(!pBuf) 
	{
		csText.LoadString (IDS_NOT_ENOUGH_MEMORY);
#ifndef _YG_COMMAND_LINE
		::MessageBox(GetFocus(),csText,csCaption,MB_OK | MB_ICONSTOP);
#else
		cout << (LPCTSTR)csText << endl;
#endif
		return FALSE;
	}

	switch(nDelMethod)
	{
	case 0:
		InitDelBuf(pBuf,0,0,0,FALSE,TRUE);
		DelAll(szDirName,pBuf,TRUE);
		break;
	case 1:
		InitDelBuf(pBuf,0XFF,0,0,FALSE,TRUE);
		DelAll(szDirName,pBuf,TRUE);
		break;
	case 2:
		InitDelBuf(pBuf,0,0,0,TRUE,TRUE);
		DelAll(szDirName,pBuf,TRUE);
		break;
	case 3:
		
		// now we do not consider the progress bar
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
		break;
	}
	free(pBuf);
	pBuf = NULL;
	
	return TRUE;
}

BOOL CSecureDelete::DelDirAndFiles(LPTSTR pszDir,
								   BYTE *pBuf,
								   BOOL bDel,
								   LPTSTR pszSaveDir,
								   BOOL bSave)
{
    WIN32_FIND_DATA     FindData;
	HANDLE              hFind   ;
	DWORD				dwAttrs;
	int					i;
	char				*p;
	int					nDirEnd,nSaveDirEnd;
	char				szDirOrFileName[MAX_PATH];
	char				szSaveDir[MAX_PATH];
	char				szSaveDirOrFileName[MAX_PATH];
	char				szDirName[MAX_PATH];

	if(m_bCancelDelete)
	{
		if( FALSE == IsDirectory(pszDir) )
		{
			SetFileAttributes(pszDir,FILE_ATTRIBUTE_NORMAL);
			if( FALSE == DeleteFile(pszDir) )
			{
				TRACE("\nDelete File %s error in DelAll",szDirName);
			}
		}
		TRACE("\nCancel in DelDirAndFiles.");
		return FALSE;
	}

	if ( NULL == pszDir)
	{
		return FALSE ;
	}
	hFind = FindFirstFile(pszDir,&FindData);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		return FALSE;
	}
  	
	if(bSave)
	{
		strcpy( szSaveDirOrFileName , pszSaveDir) ;
		strcat( szSaveDirOrFileName , "\\" ) ;
		nSaveDirEnd = strlen( szSaveDirOrFileName);
		p = strrchr(pszDir,'\\');
		p++;
		strcpy( szSaveDirOrFileName + nSaveDirEnd ,p);
	}

	if ( !(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
	{
		dwAttrs = GetFileAttributes(pszDir);
		SetFileAttributes(pszDir,FILE_ATTRIBUTE_NORMAL);
		if(!DoDelFile(pszDir,pBuf,FindData.nFileSizeLow,bDel))
			SetFileAttributes(pszDir,dwAttrs);
		FindClose(hFind);
		return TRUE;
	}
	FindClose(hFind);

	if(bSave)
	{
		strcat( szSaveDirOrFileName , "\\" ) ;
		nSaveDirEnd = strlen( szSaveDirOrFileName);
		CreateDirectory(szSaveDirOrFileName,NULL);
	}

  	strcpy( szDirOrFileName , pszDir) ;
    strcat( szDirOrFileName , "\\" ) ;
    nDirEnd = strlen( szDirOrFileName);
    strcpy( szDirOrFileName + nDirEnd , "*.*");
	
	hFind = FindFirstFile(szDirOrFileName ,&FindData);
	
	if ( INVALID_HANDLE_VALUE == hFind )
	{
		return  FALSE ;
	}
	
	do
    {
		if ( '.' == FindData.cFileName[0]) 
	          continue;

	    strcpy( szDirOrFileName + nDirEnd ,FindData.cFileName);
		//if(bSave)

        if ( !(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) 
		{
			dwAttrs = GetFileAttributes(szDirOrFileName);
			SetFileAttributes(szDirOrFileName,FILE_ATTRIBUTE_NORMAL);
			if(!DoDelFile(szDirOrFileName,pBuf,FindData.nFileSizeLow,bDel))
				SetFileAttributes(szDirOrFileName,dwAttrs);
		}
        if ( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) 
		{
			if(bSave)
			{
				strcpy(szSaveDir,szSaveDirOrFileName);
				szSaveDir[nSaveDirEnd-1] = 0;
			}
			if (!DelDirAndFiles(szDirOrFileName,pBuf,bDel,szSaveDir,bSave))
			{
				  FindClose( hFind );
				  return FALSE ;
			}
		}

	} while( FindNextFile( hFind ,&FindData));
	FindClose( hFind);
	SetFileAttributes(pszDir,FILE_ATTRIBUTE_NORMAL);
	if(bDel)
	{
		// do not remove the original dir

		if(!RemoveDirectory(pszDir))
		{
			DWORD dwError = GetLastError();
			return FALSE;
		}

		strcpy(szDirName,pszDir);
		p = strrchr(szDirName,'\\');
		p++;
		strcpy(p,RENAME_STRING);
		for(i=65;i<=90;i++)
		{
			*p = (BYTE)i;
			CreateDirectory(szDirName,NULL);
		}
		p++;
		for(i=97;i<=122;i++)
		{
			*p = (BYTE)i;
			CreateDirectory(szDirName,NULL);
		}
		p --;
		strcpy(p,RENAME_STRING);
		for(i=65;i<=90;i++)
		{
			*p = (BYTE)i;
			RemoveDirectory(szDirName);
		}
		p++;
		for(i=97;i<=122;i++)
		{
			*p = (BYTE)i;
			RemoveDirectory(szDirName);
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

BOOL CSecureDelete::DelAll(char *szDirName, BYTE *pBuf, BOOL bDel)
{
	if(m_bCancelDelete)
	{
		if( FALSE == IsDirectory(szDirName) )
		{
			SetFileAttributes(szDirName,FILE_ATTRIBUTE_NORMAL);
			if( FALSE == DeleteFile(szDirName) )
			{
				TRACE("\nDelete File %s error in DelAll",szDirName);
			}
		}
		TRACE("\nCancel in DelAll.");
		return FALSE;
	}

	char szTempDir[MAX_PATH];
	GetTempPath(MAX_PATH,szTempDir);
	DelDirAndFiles(szDirName,pBuf,bDel,szTempDir,FALSE);
	return TRUE;
}

BOOL CSecureDelete::GetDirFileSize(LPCTSTR pszDir)
{
    WIN32_FIND_DATA     FindData;
	HANDLE              hFind   ;
	int					nDirEnd;
	char				szDirOrFileName[MAX_PATH];

	if ( NULL == pszDir)
	{
		return FALSE ;
	}
	hFind = FindFirstFile(pszDir,&FindData);
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

  	strcpy( szDirOrFileName , pszDir) ;
    strcat( szDirOrFileName , "\\" ) ;
    nDirEnd = strlen( szDirOrFileName);
    strcpy( szDirOrFileName + nDirEnd , "*.*");
	
	hFind = FindFirstFile(szDirOrFileName ,&FindData);
	
	if ( INVALID_HANDLE_VALUE == hFind )
	{
		return  FALSE ;
	}
	
	do
    {
		if ( '.' == FindData.cFileName[0]) 
		{
	       continue;
		}

	    strcpy( szDirOrFileName + nDirEnd ,FindData.cFileName);
        if ( !(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) 
		{
			m_qwTotalSize += FindData.nFileSizeLow;
		}
        if ( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) 
		{
			GetDirFileSize(szDirOrFileName);
		}

	} while( FindNextFile( hFind ,&FindData));
    
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

BOOL CSecureDelete::InitDeleteMasterDel(LPCTSTR szDirName)
{
	m_bCancelDelete = FALSE;
	m_nPercent = 0;

	m_qwDelSize = 0;
	m_qwTotalSize = 0;

	m_strCurrentFile.Empty();
	m_strOriginalDir = szDirName;

	if( FALSE == GetDirFileSize(szDirName) )
	{
		TRACE("\nGetDirFileSize error in InitDelteMasterDel.");
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


