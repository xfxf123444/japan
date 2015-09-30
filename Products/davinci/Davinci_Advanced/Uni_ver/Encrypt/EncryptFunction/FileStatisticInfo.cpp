// FileStatisticInfo.cpp: implementation of the CFileStatisticInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "FileStatisticInfo.h"
#include "EncryptFunction.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileStatisticInfo::CFileStatisticInfo()
{
	m_dwTotalFileCount = 0;
	m_qwTotalFileSize = 0;
}

CFileStatisticInfo::~CFileStatisticInfo()
{

}

void CFileStatisticInfo::CountTotalFileSize(CStringArray &SelectionArray)
{
	CString OnePathString;

	m_dwTotalFileCount = 0;
	m_qwTotalFileSize = 0;

	for(long i=0; i<SelectionArray.GetSize(); i++)
	{
        OnePathString=SelectionArray.ElementAt(i);
		
		if(OnePathString.GetLength()>=2)
		{ 
		   if(IsOneDirectory(OnePathString))
		   {
			  GetDirectoryInfo((LPCTSTR)OnePathString);
		   }
		   else
		   {
			  GetFileInfo((LPCTSTR)OnePathString);
		   }
		}
		else
		{
			TRACE(L"\nThe size of OnePathString is not correct in GetTotalSize.");
		}
	}
}


BOOL CFileStatisticInfo::GetDirectoryInfo(LPCTSTR szSourceDirectory)
{
	if( FALSE == IsOneDirectory(szSourceDirectory))
	{
		TRACE(L"\nIsOneDirectory return FALSE in GetDirectoryInfo");
		return FALSE;
	}

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
		    GetFileInfo((LPCTSTR)strSubPath);
		}
		else
		{
            // PureTravelDir((LPCTSTR)strSubPath);
		    GetDirectoryInfo((LPCTSTR)strSubPath);
		}
	}while(FindNextFile(hFind,&FindFileData));

	FindClose(hFind);
	
	return TRUE;
}


BOOL CFileStatisticInfo::GetFileInfo(LPCTSTR FilePath)
{
	  
	  CString SourceFilePath;

	  SourceFilePath=FilePath;

	  WIN32_FIND_DATA findFileData;

	  HANDLE hFind = FindFirstFile((LPCTSTR)SourceFilePath,
									&findFileData);

	  if (hFind == INVALID_HANDLE_VALUE)
	  {
		  return FALSE;
	  }
  
	  VERIFY(FindClose(hFind));
  
	  LARGE_INTEGER qwOneFileSize;
	  qwOneFileSize.HighPart=findFileData.nFileSizeHigh;
	  qwOneFileSize.LowPart=findFileData.nFileSizeLow;

	  m_qwTotalFileSize=m_qwTotalFileSize+qwOneFileSize.QuadPart;
	  m_dwTotalFileCount++;

	  return TRUE;
}
