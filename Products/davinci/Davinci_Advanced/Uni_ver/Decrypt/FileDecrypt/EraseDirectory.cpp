// from disk cleaner

// EraseDirectory.cpp: implementation of the CEraseDirectory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EraseDirectory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEraseDirectory::CEraseDirectory()
{
}

CEraseDirectory::~CEraseDirectory()
{
}


BOOL CEraseDirectory::EraseLeaf(LPCTSTR szDirName)
{

	// TODO: Add your control notification handler code here

	if(bIsDirectory(szDirName)==FALSE)
	{
		return FALSE;
	}

	CString strDir;
	strDir=szDirName;

	CString strFind;
	strFind=strDir + L"\\*.*";

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
		strSubPath=strSubPath + strDir +L"\\"+ FindFileData.cFileName;

		if( 0 == ( FILE_ATTRIBUTE_DIRECTORY & FindFileData.dwFileAttributes ) )
		{
	        SetFileAttributes((LPCTSTR)strSubPath,FILE_ATTRIBUTE_NORMAL);
			if( FALSE == DeleteFile((LPCTSTR)strSubPath ) )
			{
				TRACE(L"\nDeleteFile %s error in EraseLeaf.",(LPCTSTR)strSubPath );
			}
		}
		else
		{
	        SetFileAttributes((LPCTSTR)strSubPath,FILE_ATTRIBUTE_NORMAL);
			if(FALSE == EraseLeaf((LPCTSTR)strSubPath) )
			{
				  TRACE(L"\nEraseLeaf %s error in EraseLeaf.",(LPCTSTR)strSubPath);
			}
		}
	}while(FindNextFile(hFind,&FindFileData));

	FindClose(hFind);

	// to here, the sub dir of szDirName has been travelled
	SetFileAttributes(szDirName,FILE_ATTRIBUTE_NORMAL);
	if( FALSE == RemoveDirectory(szDirName) )
	{
		TRACE(L"\nRemoveDirector %s error in EraseLeaf.",szDirName);
		return FALSE;
	}
	else
	{
		// TRACE(L"\nRemoveDirecotry %s succeed",szDirName);
	}

    return TRUE;
}

BOOL CEraseDirectory::EraseDirectory(LPCTSTR szDirName)
{
	if(bIsDirectory(szDirName) == FALSE)
	{
		return FALSE;
	}

	EraseLeaf(szDirName);

	return TRUE;
}


BOOL CEraseDirectory::bIsDirectory(LPCTSTR szDirName)
{
	DWORD dwFileAttribute;
	dwFileAttribute = GetFileAttributes(szDirName);

	if( -1 == dwFileAttribute )
	{
		return FALSE;
	}

	if( dwFileAttribute & FILE_ATTRIBUTE_DIRECTORY )
	{
		return TRUE;
	}

	return FALSE;
}
