// NormalDelete.cpp: implementation of the CNormalDelete class.
//
//////////////////////////////////////////////////////////////////////

#include "ygfsmon.h"
#include "NormalDelete.h"
#include "Function.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNormalDelete::CNormalDelete()
{

}

CNormalDelete::~CNormalDelete()
{

}

BOOL CNormalDelete::DeleteDirectory(LPCTSTR szDirectoryName)
{
	if(m_bCancelDelete)
	{
		return FALSE;
	}

	if( FALSE == IsDirectory(szDirectoryName ) )
	{
		return FALSE;
	}

	CString strFind;
	strFind=strFind+szDirectoryName+"\\*.*";
	
	WIN32_FIND_DATA FindData;
	HANDLE hFind;
	
	hFind = ::FindFirstFile((LPCTSTR)strFind,&FindData);

	if( INVALID_HANDLE_VALUE == hFind )
	{
		return FALSE;
	}

	do
	{
		if('.' == FindData.cFileName[0])
		{
			continue;
		}

		CString strFilePath;
		strFilePath = strFilePath + szDirectoryName + "\\" + FindData.cFileName;

		if( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			// if this is a directory
			TRACE("\nDeleteDirectory: "+strFilePath);
			DeleteDirectory((LPCTSTR)strFilePath);
		}
		else
		{
			// if this is a file
			SetFileAttributes((LPCTSTR)strFilePath,FILE_ATTRIBUTE_NORMAL);
			TRACE("\nDelete File: "+strFilePath);
			DeleteFile((LPCTSTR)strFilePath);
			m_dwFinishedFileCount++;
		}
	}while( FindNextFile(hFind,&FindData) );

	FindClose(hFind);

	// do not delete the original Directory
	if( 0 != m_strOriginalDir.Compare(szDirectoryName) )
	{
		if( FALSE == RemoveDirectory(szDirectoryName) )
		{
			TRACE("\nRemove directory: %s error, error number: %d",szDirectoryName,GetLastError());
		}
	}

	m_dwFinishedFileCount++;
	
	return TRUE;

}

BOOL CNormalDelete::GetCurrentPercent(int *pnCurrentPercent)
{
	*pnCurrentPercent = 0;

	if(0 == m_dwTotalFileCount )
	{
		return FALSE;;
	}

	*pnCurrentPercent = m_dwFinishedFileCount * 100 / m_dwTotalFileCount;

	return TRUE;

}

BOOL CNormalDelete::GetFileCount(LPCTSTR szDirectoryName)
{
	if( FALSE == IsDirectory(szDirectoryName ) )  //Verifies that a path is a valid directory. 
	{
		return FALSE;
	}

	m_dwTotalFileCount++ ;

	CString strFind;
	strFind=strFind+szDirectoryName+"\\*.*";
	
	WIN32_FIND_DATA FindData;  //The WIN32_FIND_DATA structure describes a file found by the FindFirstFile, FindFirstFileEx, or FindNextFile function.

	HANDLE hFind;
	
	hFind = ::FindFirstFile((LPCTSTR)strFind,&FindData);  //Searches a directory for a file whose name matches the specified file name on the destination site identified by this object. It examines subdirectory names as well as file names.

	if( INVALID_HANDLE_VALUE == hFind )
	{
		return FALSE;
	}

	do
	{
		if('.' == FindData.cFileName[0])   
		{
			continue;
		}

		CString strFilePath;
		strFilePath = strFilePath + szDirectoryName + "\\" + FindData.cFileName;

		if( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			// if this is a directory
			GetFileCount((LPCTSTR)strFilePath);
		}
		else
		{
			// if this is a file
			m_dwTotalFileCount++;
		}
	}while( FindNextFile(hFind,&FindData) );

	FindClose(hFind);

	return TRUE;

}

BOOL CNormalDelete::InitNormalDeleteDirectory(LPCTSTR szDirectory)
{
	m_strOriginalDir = szDirectory;
	m_bCancelDelete = FALSE;
	m_dwFinishedFileCount = 0;
	m_dwTotalFileCount = 0;

	if( FALSE == GetFileCount(szDirectory) )
	{
		TRACE("\nGetFileCount error in InitNormalDelete.");
		return FALSE;
	}

	return TRUE;

}

BOOL CNormalDelete::NormalDeleteDirectory(LPCTSTR szDirectory)
{
	InitNormalDeleteDirectory(szDirectory);

	if( FALSE == DeleteDirectory(szDirectory) )
	{
		TRACE("\nDeleteDirectory error in NormalDeleteDirectory.");
		return FALSE;
	}
	return TRUE;

}

BOOL CNormalDelete::SetCancelDelete()
{
	SetCancelDelete();

}
