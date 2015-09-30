#include "stdafx.h"
#include "AMFunction.h"
#include "AMRestore.h"
#include "AMBackup.h"
#include "resource.h"
#include <iostream>
using namespace std;
// 2004.05.20 for encrypt begin
#include "..\..\..\..\tech\crypto51\md5.h"
#include "..\..\..\..\tech\Crypto51\modes.h"
#include "..\..\..\..\tech\Crypto51\aes.h"
using namespace CryptoPP;
// 2004.05.20 for encrypt end

#include "InputPasswordDlg.h"

extern CFAILRETRYSCHEDULE g_FailRetrySchedule;

BOOL GetOsVersion()
{
	OSVERSIONINFO osvi;
	BOOL bOsVersionEx = FALSE;

	ZeroMemory(&osvi,sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx((OSVERSIONINFO *)&osvi);

	if (osvi.dwMajorVersion == 6)
		return TRUE;
	else
		return FALSE;
}


//-- function for restore begin
BOOL TrimRightChar(WCHAR *szOneString, const WCHAR chOneChar, const int nMaxCharCount)
{
	CString strOneString;
	strOneString=szOneString;
	strOneString.TrimRight(chOneChar);
	wcsncpy(szOneString,(LPCTSTR)strOneString,nMaxCharCount-1);
	return TRUE;
}

WCHAR * strrchrpro(WCHAR* szSource,int chFindChar)
{
	CString strSource;
	strSource=szSource;
	int nIndex;
	nIndex = strSource.ReverseFind((WCHAR)chFindChar);
	if ( -1 == nIndex )
	{
		// not found;
		return NULL;
	}
	// return a pointer
	return (szSource+nIndex);
}

/*=============================================================
 *
 * Function name:  GetLongMainName()
 * Purpose:        get the main name from the total path 
 *                 compare with GetMainFileName, it do not erase the 
 *                 date and time information
 *=============================================================*/
BOOL GetLongMainName(LPCTSTR szImageFile,WCHAR *szLongMainName)
{
  // get the main file name from szImageFile

	szLongMainName[0]=L'\0';

	CString strImageFile;
	CString strMainFileName;

	strImageFile = szImageFile;
  
	int nIndex=strImageFile.ReverseFind(L'\\');
  
	strMainFileName=strImageFile.Mid(nIndex+1);

	nIndex=strMainFileName.ReverseFind(L'.');
	strMainFileName=strMainFileName.Left(nIndex);

	if( 0 == strMainFileName.GetLength() )
	{
		return FALSE;
	}

	wcsncpy(szLongMainName,(LPCTSTR)strMainFileName,MAX_PATH-1);
	return TRUE;
}

DWORD GetTotalIndexOffset(DWORD dwMaxPathID, DWORD dwTimeStampCount, DWORD dwOnePathID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ASSERT (dwOnePathID > 0);
    DWORD dwBaseOffset;
	dwBaseOffset=sizeof(TOTAL_INDEX_NODE);

    DWORD dwOneItemSize;
	dwOneItemSize=sizeof(INDEX_DATA);

    DWORD dwFileOffset;

	// dwPath is start with 
	dwFileOffset=dwBaseOffset + 
				   dwMaxPathID*(dwTimeStampCount-1)*dwOneItemSize +
				   (dwOnePathID-1)*dwOneItemSize;

	return dwFileOffset;
}

BOOL FreeAMStringArray(CStringArray &StringArray)
{
	if (StringArray.GetSize()) StringArray.RemoveAll();
	return TRUE;
}

BOOL FreeAMDataArray(CDATA_ARRAY &DataArray)
{
	if (DataArray.GetSize()) DataArray.RemoveAll();
	return TRUE;
}

BOOL GetSourceArray(LPCTSTR szLastImageFile, CDATA_ARRAY &SourceArray)
{
	__int64 lFilePointer;
	HANDLE hLastImage;
	hLastImage=CreateFile(szLastImageFile,GENERIC_READ,
		                     FILE_SHARE_READ,NULL,
							 OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,
							 NULL);
 
	if(hLastImage==INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(IDS_OPEN_IMAGE_FAIL,MB_OK|MB_ICONINFORMATION,NULL);
		return FALSE;
	}

	IMAGE_HEAD ImageHead;
	DWORD dwReadBytes;

	ReadFile(hLastImage,&ImageHead,sizeof(ImageHead),&dwReadBytes,NULL);
	YGSetFilePointer(hLastImage,ImageHead.FileTablePointer.QuadPart,FILE_BEGIN,lFilePointer);

	TABLE_NODE TableNode;
	ReadFile(hLastImage,&TableNode,sizeof(TableNode),&dwReadBytes,NULL);
	
	DWORD dwFileNumber;

	if (SourceArray.GetSize()) SourceArray.RemoveAll();
	TABLE_DATA EmptyData;
	memset(&EmptyData,0,sizeof(EmptyData));
	SourceArray.Add(EmptyData);

	TABLE_DATA TableData;

	for(dwFileNumber=1; dwFileNumber<=TableNode.dwFileCount; dwFileNumber++)
	{
		ReadFile(hLastImage,&TableData,sizeof(TableData),&dwReadBytes,NULL);
		SourceArray.Add(TableData);
	}

    CloseHandle(hLastImage);

	return TRUE;
}


/*==========================================================================
 *
 * Function Name: GetSelectionArrayPro()
 * Purpose:       give the last image name, find the selection array
 * Note:          first find the first file name, then read selection array               
 *
 *                for compatible,this function will call GetSelectionLink
 *                which can reduce the duplicate code.
 *
 *==========================================================================*/

BOOL GetSelectionArrayPro(LPCTSTR szLastImage,CStringArray &SelectionArray)
{
    if (SelectionArray.GetSize()) SelectionArray.RemoveAll();

	SELECTION_LINK *pSelectionHead;
	pSelectionHead=NULL;
	
	SELECTION_LINK *pSelectionLink;
	pSelectionLink=NULL;

	if( FALSE == GetSelectionLink(szLastImage,&pSelectionHead) )
	{
		TRACE(L"\nGetSelectionLink error in GetSelectionArrayPro.");
		return FALSE;
	}
	else
	{
		pSelectionLink = pSelectionHead;
		while(NULL != pSelectionLink)
		{
			SelectionArray.Add(CString(pSelectionLink->SelectionData.szFilePath));
			pSelectionLink=pSelectionLink->pNextSelection;
		}
		FreeSelectionLink(pSelectionHead);
	}
	return TRUE;
}


/*============================================================
 *
 * Function Name: GetSelectionLink()
 *
 * Purpose:       Get a selection link from the first image
 *                the first image is found by the last image
 *
 *=============================================================*/

BOOL GetSelectionLink(LPCTSTR szLastImage,SELECTION_LINK **ppSelectionHead)
{
	__int64 lFilePointer;  
	*ppSelectionHead=NULL;

	DWORD dwErrorNumber;
  
	if( FALSE == CheckLastImage(szLastImage,&dwErrorNumber) ) 
	{
		TRACE(L"\nCheckLastImage error in GetSelectionArrayPro.");
		return FALSE;
	}

	CDATA_ARRAY ImageArray;
  
	if ( FALSE == GetImageArray(ImageArray,szLastImage) )
	{
		TRACE(L"\nGetImageArray error in  GetSelectionArrayPro.");
		return FALSE;
	}

	CURRENT_RESTORE_INFO CurrentInfo;
	CurrentInfo.dwCurrentSource = 0;
	CurrentInfo.hCurrentImage = NULL;

	if( GetRequiredHandle(&CurrentInfo,1,ImageArray) == FALSE )
	{
		TRACE(L"\nGetRequiredHandle 1 error in GetSelectionArrayPro.");
		return FALSE;
	}

	// now CurrentInfo.hCurrentImage is the first image

	YGSetFilePointer(CurrentInfo.hCurrentImage,sizeof(IMAGE_HEAD),FILE_BEGIN,lFilePointer);
	SELECTION_NODE SelectionNode;

	DWORD dwReadBytes;

	if( FALSE == ReadFile(CurrentInfo.hCurrentImage,&SelectionNode,sizeof(SelectionNode),
							&dwReadBytes,NULL) )
	{
		TRACE(L"\nReadFile error in GetSelectionArrayPro,read SelectionNode.");
		CloseHandle(CurrentInfo.hCurrentImage);
		CurrentInfo.hCurrentImage = NULL;
		return FALSE;
	}

	SELECTION_DATA SelectionData;
	// CString OnePathString;

	for(DWORD dwPathNumber=0;dwPathNumber<SelectionNode.dwPathCount;dwPathNumber++)
	{
		ReadFile(CurrentInfo.hCurrentImage,&SelectionData,sizeof(SelectionData),&dwReadBytes,NULL);
		// OnePathString=SelectionData.szFilePath;
		if( FALSE == InsertSelectionNode(ppSelectionHead,SelectionData) )
		{
			FreeSelectionLink(*ppSelectionHead);
			CloseHandle(CurrentInfo.hCurrentImage);
			CurrentInfo.hCurrentImage = NULL;
			return FALSE;
		}
	}

	CloseHandle(CurrentInfo.hCurrentImage);
	CurrentInfo.hCurrentImage = NULL;

	return TRUE;
}


/*===========================================================
 *
 * Function Name: FreeSelectionLink()
 * Purpose:       Free the memeory that selection link occupy
 *
 *===========================================================*/
BOOL FreeSelectionLink(SELECTION_LINK *pSelectionHead)
{
	SELECTION_LINK *pNextSelection;
	pNextSelection = NULL;

	if( NULL == pSelectionHead )
	{
		return TRUE;
	}

	do
	{
		pNextSelection = pSelectionHead->pNextSelection;
		
		free(pSelectionHead);
		pSelectionHead=NULL;
		
		pSelectionHead = pNextSelection;
	}while(NULL != pNextSelection);

	return TRUE;
}


BOOL CheckLastImage(LPCTSTR szLastImage, DWORD *pdwErrorCode)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pdwErrorCode=0;

	CString ImageIdentity;
	CString ImageFilePath;
	ImageFilePath=szLastImage;

	HANDLE hImageFile;
	hImageFile=CreateFile((LPCTSTR)ImageFilePath,
		                   GENERIC_READ,FILE_SHARE_READ,
						   NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,
						   NULL);

	if(hImageFile==INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(IDS_OPEN_IMAGE_FAIL,MB_OK|MB_ICONINFORMATION,NULL);
		(*pdwErrorCode)=0;
		return FALSE;
	}
    
	IMAGE_HEAD ImageHead;
	DWORD dwReadBytes;
	
	ReadFile(hImageFile,&ImageHead,sizeof(ImageHead),&dwReadBytes,NULL);

	CloseHandle(hImageFile);

	if( wcscmp(ImageHead.szImageIdentity,IMAGE_IDENTITY) != 0 )
	{
		AfxMessageBox(IDS_NOT_AM01_IMAGE,MB_OK|MB_ICONINFORMATION,NULL);
		(*pdwErrorCode)=1;
		return FALSE;
	}

	if(ImageHead.dwFinishSignature != IMAGE_STATUS_COMPLETE)
	{
		AfxMessageBox(IDS_NOT_INTEGRATE_IMAGE,MB_OK|MB_ICONINFORMATION,NULL);
		(*pdwErrorCode)=2;
		return FALSE;
	}

	if(ImageHead.dwFileNumber != LAST_FILE_NUMBER)
	{
		( * pdwErrorCode) = NOT_LAST_IMAGE;
		return FALSE;
	}

	return TRUE;
}

BOOL SearchLastImage( LPTSTR szImageFile, DWORD *pdwErrorCode )
{
	HANDLE hImageFile;
	hImageFile=CreateFile(szImageFile,
		GENERIC_READ,FILE_SHARE_READ,
		NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,
		NULL);

	if(hImageFile==INVALID_HANDLE_VALUE)
	{
		*pdwErrorCode = -1;
		return FALSE;
	}

	IMAGE_HEAD ImageHead;
	DWORD dwReadBytes;

	ReadFile(hImageFile,&ImageHead,sizeof(ImageHead),&dwReadBytes,NULL);

	CloseHandle(hImageFile);
	CString szUserName = ImageHead.szLastFileName;
	if (szUserName.GetLength() == 0)
	{
		*pdwErrorCode = -1;
		return FALSE;
	}

	CString path(szImageFile);
	int index = path.ReverseFind(L'\\');
	path = path.Left(index + 1);

	CFileFind finder;

	// build a string with wildcards
	CString strWildcard(path);
	strWildcard += _T("*.amg");

	// start working for files
	BOOL bWorking = finder.FindFile(strWildcard);
	BOOL bFound = FALSE;
	*pdwErrorCode = -1;
	CString szFilePath;
	DWORD dwFileIndex = 0;
	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		// skip . and .. files, directory; otherwise, we'd
		// recur infinitely!

		if (finder.IsDots() || finder.IsDirectory())
			continue;

		CString szFileTitle = finder.GetFileTitle();
		if (szFileTitle.Find(szUserName) == 0)
		{
			BOOL bVerify = FALSE;
			if (szFileTitle.CompareNoCase(szUserName) == 0)
			{
				bVerify = TRUE;
			}
			else
			{
				if (szFileTitle.GetLength() > szUserName.GetLength())
				{
					int index = szFileTitle.Find(L'_', szUserName.GetLength());
					int count = 0;
					while (index != -1)
					{
						int p1 = index;
						++count;
						index = szFileTitle.Find(L'_', index + 1);
						int p2 = index;
						if (count == 1)
						{
							if ((p2 - p1 - 1) != 8)
							{
								bVerify = FALSE;
								break;
							}
						}

						if (count == 2)
						{
							if ((p2 - p1 - 1) != 4)
							{
								bVerify = FALSE;
								break;
							}
						}
					}

					if (count == 3)
					{
						bVerify = TRUE;
					}
				}
			}
			if (bVerify)
			{
				bVerify = CheckLastImage(LPCTSTR(finder.GetFilePath()), pdwErrorCode);
				if (bVerify)
				{
					hImageFile=CreateFile(finder.GetFilePath(),
						GENERIC_READ,FILE_SHARE_READ,
						NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,
						NULL);

					if(hImageFile!=INVALID_HANDLE_VALUE)
					{
						ReadFile(hImageFile,&ImageHead,sizeof(ImageHead),&dwReadBytes,NULL);

						CloseHandle(hImageFile);

						if (ImageHead.dwFileIndex >= dwFileIndex)
						{
							dwFileIndex = ImageHead.dwFileIndex;
							szFilePath = finder.GetFilePath();
							bFound = TRUE;
							*pdwErrorCode = 0;
						}
					}
				}
			}
		}
	}

	finder.Close();
	if (bFound && szFilePath.GetLength() > 0)
	{
		wcscpy(szImageFile, (LPCTSTR)szFilePath);
		*pdwErrorCode = 0;
	}
	return bFound;
}
/*================================================================
 *
 * Function Name:  GetImageArray()
 *
 * Purpose:        read the image array information from 
 *                 the last file.
 *
 * Note:           the start of the ImageArray is an empty data
 *
 *===============================================================*/
BOOL GetImageArray(CDATA_ARRAY &TargetArray,LPCTSTR szLastFileName)
{
	// TODO: Add your control notification handler code here
	__int64 lFilePointer;
	HANDLE hLastImage;
	hLastImage=CreateFile(szLastFileName,GENERIC_READ,
		                  FILE_SHARE_READ,NULL,
						  OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,
						  NULL);

	if(hLastImage==INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(IDS_OPEN_IMAGE_FAIL,MB_OK|MB_ICONINFORMATION,NULL);
		return FALSE;
	}

	IMAGE_HEAD ImageHead;
	DWORD dwReadBytes;

	ReadFile(hLastImage,&ImageHead,sizeof(ImageHead),&dwReadBytes,NULL);
	YGSetFilePointer(hLastImage,ImageHead.FileTablePointer.QuadPart,FILE_BEGIN,lFilePointer);

	TABLE_NODE TableNode;
	ReadFile(hLastImage,&TableNode,sizeof(TableNode),&dwReadBytes,NULL);
	if (dwReadBytes != sizeof(TableNode)) 
	{
		AfxMessageBox(IDS_OPEN_IMAGE_FAIL,MB_OK|MB_ICONINFORMATION,NULL);
		return FALSE;
	}
	
	DWORD dwFileNumber;

	if (TargetArray.GetSize()) TargetArray.RemoveAll();

	TABLE_DATA EmptyData;
	memset(&EmptyData,0,sizeof(EmptyData));
	TargetArray.Add(EmptyData);

	TABLE_DATA TableData;

	for(dwFileNumber=1; dwFileNumber<=TableNode.dwFileCount; dwFileNumber++)
	{
		ReadFile(hLastImage,&TableData,sizeof(TableData),&dwReadBytes,NULL);
		if (dwReadBytes != sizeof(TableData)) 
		{
			AfxMessageBox(IDS_OPEN_IMAGE_FAIL,MB_OK|MB_ICONINFORMATION,NULL);
			return FALSE;
		}
		TargetArray.Add(TableData);
	}
 
	// some times the last image file name is not in the original place
	// we just record the last image name to target array
	// to avoid the user made one more selection

	wcsncpy(TargetArray[TargetArray.GetUpperBound()].szFileName,szLastFileName,MAX_PATH-1);

    CloseHandle(hLastImage);
	return TRUE;
}


BOOL GetRequiredHandle(CURRENT_RESTORE_INFO *pCurrentInfo,DWORD dwFileNumber, const CDATA_ARRAY &ImageArray)
{

	ASSERT(ImageArray.GetUpperBound()>=1);

	if( (*pCurrentInfo).dwCurrentSource > (DWORD)( ImageArray.GetUpperBound()) )
	{
		 TRACE(L"\n dwCurrentSource is larger than upperbound of the array");
		 return FALSE;
	}


	if( dwFileNumber> (DWORD)( ImageArray.GetUpperBound()) )
	{
		TRACE(L"\n dwFileNumber is larger than upperbound of the image array.");
		return FALSE;
	}

	WCHAR szImageFile[MAX_PATH];

	wcsncpy(szImageFile,ImageArray.GetAt(dwFileNumber).szFileName,MAX_PATH-1);

	BOOL bValidImage;
	bValidImage=FALSE;

	BOOL bContinueSelect;
	bContinueSelect=TRUE;

	bValidImage=CheckImage(szImageFile,dwFileNumber,ImageArray);

	while(bValidImage==FALSE)
	{
	    CString strInfo;
		CString strFileName;

		strFileName=ImageArray.GetAt(dwFileNumber).szFileName;
		int nIndex;
		nIndex=strFileName.ReverseFind(L'\\');
		strFileName=strFileName.Mid(nIndex+1);

	    
		//-- display information begin
		
		// strInfo.Format(L" Please select the file \n File Number is: %d  Original File Name is: %s",dwFileNumber,(LPCTSTR)strFileName);

		CString strSelectFile;
		strSelectFile.LoadString(IDS_SELECT_FILE);

		CString strFileNumber;
		strFileNumber.LoadString(IDS_FILE_NUMBER);

		CString strOriginalFileName;
		strOriginalFileName.LoadString(IDS_ORIGINAL_FILE_NAME);

		CString strFormat;
		strFormat=strSelectFile + L"\n" + strFileNumber + L"  %d\n" + strOriginalFileName + L"  %s  ";

		strInfo.Format(strFormat,dwFileNumber,(LPCTSTR)strFileName);

		AfxMessageBox((LPCTSTR)strInfo,MB_OK|MB_ICONINFORMATION,NULL);

		//-- display information end

		BOOL bReturnValue;

		bReturnValue=SelectFile(L"AMG",L"AM01 Image File",szImageFile);

	    if(bReturnValue==TRUE)
		{
	       bValidImage=CheckImage(szImageFile,dwFileNumber,ImageArray);
		   if(bValidImage==TRUE)
		   {
			   break;
		   }
		}
		else
		{
		   TRACE(L"\n SelectFile Error.");
		   return FALSE;
		}
	}

	// record it in the ImageArray
	if(bValidImage==TRUE)
	{
	   wcsncpy(const_cast<WCHAR*>(ImageArray[dwFileNumber].szFileName),szImageFile,MAX_PATH-1);

	   CloseHandle( (*pCurrentInfo).hCurrentImage );

	   (*pCurrentInfo).dwCurrentSource =dwFileNumber ;

	   (*pCurrentInfo).hCurrentImage= CreateFile(szImageFile,
				           GENERIC_READ,FILE_SHARE_READ,
					       NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	   return TRUE;
    }
	else
	{
	   TRACE(L"\n The image is not valid");
	   return FALSE;
	}
}


/*=============================================================
 *
 * Function Name: InsertSelectionNode()
 * Purpose:       Add one selection node the the link
 * Note:          this function is invoked by GetSelectionLink
 * 
 *============================================================*/
BOOL InsertSelectionNode(SELECTION_LINK **ppSelectionHead,SELECTION_DATA SelectionData)
{
	SELECTION_LINK *pInsertNode;

	pInsertNode = *ppSelectionHead;

	SELECTION_LINK *pNewSelectionData;
	pNewSelectionData = NULL;

	pNewSelectionData = (SELECTION_LINK *)malloc(sizeof(SELECTION_LINK));

	if( NULL == pNewSelectionData )
	{
		TRACE(L"\nmalloc error in InsertSelectionNode.");
		return FALSE;
	}

	pNewSelectionData->SelectionData = SelectionData;
	pNewSelectionData->pNextSelection = NULL;

	if( NULL != *ppSelectionHead )
	{
		pInsertNode = *ppSelectionHead;

		while( NULL != pInsertNode->pNextSelection )
		{
			pInsertNode = pInsertNode->pNextSelection;
		}

        pInsertNode->pNextSelection = pNewSelectionData;
	}
	else
	{
		*ppSelectionHead = pNewSelectionData;

	}
	return TRUE;
}


BOOL CheckImage(LPCTSTR szImageName, DWORD dwFileNumber, const CDATA_ARRAY &ImageArray)
{

	HANDLE hImageFile;
	hImageFile = CreateFile(szImageName,
				           GENERIC_READ,FILE_SHARE_READ,
					       NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if(hImageFile==INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	IMAGE_HEAD ImageHead;
	DWORD dwReadBytes;

    ULARGE_INTEGER qwFileSize;

	qwFileSize.LowPart=GetFileSize(hImageFile,&(qwFileSize.HighPart));

	ReadFile(hImageFile,&ImageHead,sizeof(ImageHead),&dwReadBytes,NULL);

	CloseHandle(hImageFile);

	if(	wcscmp(ImageHead.szSerialNumber,ImageArray.GetAt(dwFileNumber).szSerialNumber) == 0
	    && ImageArray.GetAt(dwFileNumber).qwFileSize.QuadPart == qwFileSize.QuadPart)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


BOOL SelectFile(WCHAR *szFileExt,WCHAR *szFileType,WCHAR *szFile)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	BOOL bResult = FALSE;
	CString strFilter;
	strFilter.Format(L"%s (*.%s)|*.%s||",szFileType,szFileExt,szFileExt);
	CFileDialog dlg (TRUE, szFileExt, NULL, OFN_HIDEREADONLY, strFilter);

	// if user selected a file then open it as a database
	if (IDOK == dlg.DoModal())
	{
		wcsncpy(szFile,dlg.GetPathName(),MAX_PATH-1);
		bResult = TRUE;
	}
	return bResult;
}


BOOL GetDataFilePointer(HANDLE hTotalIndexFile,
					    DWORD dwMaxPathID,
						DWORD dwPathID, 
						DWORD dwTimeStampNumber, 
						INDEX_DATA *pIndexData)

{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	__int64 lFilePointer;
	DWORD dwMoveTimeStampNumber;
	DWORD dwTotalIndexOffset;
	DWORD dwReadBytes;

	dwMoveTimeStampNumber = dwTimeStampNumber;

	INDEX_DATA IndexData;
	   
	do
	{
		dwTotalIndexOffset=GetTotalIndexOffset(dwMaxPathID,dwMoveTimeStampNumber,dwPathID);

		YGSetFilePointer(hTotalIndexFile,dwTotalIndexOffset,FILE_BEGIN,lFilePointer);

		ReadFile(hTotalIndexFile,&IndexData,sizeof(IndexData),&dwReadBytes,NULL);

		dwMoveTimeStampNumber--;

	}while(dwMoveTimeStampNumber > 0 
		   && IndexData.wStatusFlag != FILE_NEW
		   && IndexData.wStatusFlag != FILE_CHANGE);

	dwMoveTimeStampNumber++;
   
	dwTotalIndexOffset=GetTotalIndexOffset(dwMaxPathID,dwMoveTimeStampNumber,dwPathID);

    YGSetFilePointer(hTotalIndexFile,dwTotalIndexOffset,FILE_BEGIN,lFilePointer);

	if( FALSE == ReadFile(hTotalIndexFile,pIndexData,sizeof(IndexData),&dwReadBytes,NULL) )
	{
		return FALSE;
	}

	return TRUE;
}


BOOL ReadDataFile(CURRENT_RESTORE_INFO *pCurrentInfo,const CDATA_ARRAY &ImageArray, void *pBuffer, DWORD dwRequestBytes)
{
	ASSERT(ImageArray.GetSize()>=2);
	__int64 lFilePointer;
	DWORD dwReadBytes;
	
	if( FALSE == ReadFile( (*pCurrentInfo).hCurrentImage,pBuffer,dwRequestBytes,&dwReadBytes,NULL) )
	{
		// for debug
		DWORD dwErrorNumber;
		dwErrorNumber=GetLastError();
	}
	
	while(dwReadBytes < dwRequestBytes)
	{
	   // use while in case one buffer is too much
	   // and the buffer need several files to fill

	   // GetNextHandle(pCurrentInfo,ImageArray);
		
	   if(GetRequiredHandle(pCurrentInfo,(*pCurrentInfo).dwCurrentSource + 1,ImageArray)==FALSE)
	   {
		   TRACE(L"\n Get reuqired handle error, exit ReadDataFile");
		   return FALSE;
	   }

	   dwRequestBytes=dwRequestBytes-dwReadBytes;
	   pBuffer=(BYTE *)pBuffer+dwReadBytes;

	   YGSetFilePointer( (*pCurrentInfo).hCurrentImage ,sizeof(IMAGE_HEAD),FILE_BEGIN,lFilePointer);

	   ReadFile((*pCurrentInfo).hCurrentImage,pBuffer,dwRequestBytes,&dwReadBytes,NULL);
	}

	return TRUE;
}


// get the left part of a path
// with the right L"\"

BOOL GetLeftPart(LPCTSTR szOnePath, WCHAR * szLeftPart)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString OnePath;
	CString LeftPart;

	OnePath=szOnePath;

	int i=0;

	i=OnePath.ReverseFind(L'\\');

	if(-1 == i)
	{
		LeftPart.Empty();
	}
	else
	{
        LeftPart=OnePath.Left(i);
	}

	wcsncpy(szLeftPart, (LPCTSTR)LeftPart, MAX_PATH-1);

	return TRUE;

}

BOOL GetRightPart(LPCTSTR szOnePath,WCHAR *szRightPart)
{
	CString strOnePathString;
	strOnePathString=szOnePath;

	szRightPart[0]=L'\0';
	
	int nIndex;
	nIndex=strOnePathString.ReverseFind(L'\\');
	if( -1 == nIndex )
	{
		return FALSE;
	}
	
	wcsncpy(szRightPart,LPCTSTR(strOnePathString.Mid(nIndex+1)),MAX_PATH-1);
	
	return TRUE;
}

//-- function for restore end


//-- function for squeeze begin

/*==================================================================
 *
 *
 * Function Name:  WriteDataFile();
 *
 * Purpose:        Write a buffer to disk, if needed, ask the user
 *                 to open a new file for writing
 *
 * return value:   if the function return FALSE, it means some error
 *                 happen, or user canceled the backup 
 *
 *=================================================================*/

BOOL WriteDataFile(
					   HANDLE             &hCurrentTarget,
					   void               *pDataBuffer,
					   DWORD              dwDataSize,
					   unsigned __int64   &qwTargetSize,
					   CDATA_ARRAY        &TargetArray,
					   IMAGE_HEAD         *pImageHead, // this is used to open the new image
					   PBACKUPOPTION pBackupOption
				  )
{

  DWORD WrittenBytes;

  DWORD dwRequestWrite;

  dwRequestWrite = dwDataSize;

  // for get the file size
  ULARGE_INTEGER ulTargetSize;

  // AM01 Version 2, the max next writting is AM_COMPRESS_BUFFER + sizeof(DataNode)
  if( qwTargetSize + dwDataSize < AM_MAX_FILE_SIZE)
  {

	  // if the target file is not larger than the max file size
  
	  if( WriteFile( hCurrentTarget,pDataBuffer,dwRequestWrite,&WrittenBytes,NULL) )
	  {
	     qwTargetSize = qwTargetSize + WrittenBytes;
	  }
	  else
	  {
		 DWORD dwReturnValue;
		 
		 dwReturnValue=GetLastError();

		 if( ERROR_DISK_FULL != dwReturnValue)
		 {
			 // if there is another reason
			 // the we just return
			 TRACE(L"\nWriteFile error in WriteDataFile, not because disk full.");
			 return FALSE;
		 }
		 else
		 {
			 AfxMessageBox(IDS_DISK_FULL,MB_OK|MB_ICONINFORMATION,NULL);
		 
			 if(
				 OpenNewTarget( hCurrentTarget,TargetArray,(BYTE *)pDataBuffer, 
								dwRequestWrite,WrittenBytes,qwTargetSize ,pImageHead,pBackupOption,TRUE) != TRUE 
			   )
			 {
				 TRACE(L"\nOpenNewTarget error in WriteDataFile");
				 return FALSE;
			 }

			 if( FALSE == GetOneFileSize(hCurrentTarget,&qwTargetSize) )
			 {
				 TRACE(L"\nGetOneFileSize error in WriteDataFile.");
				 return FALSE;
			 }

		 } // end if the target disk is full

	  }// end if write target file error

  }
  else
  {

	   WrittenBytes=0;

	   if(
		   FALSE == OpenNewTarget(
		                           hCurrentTarget,TargetArray,
			                       (BYTE *)pDataBuffer,dwDataSize,WrittenBytes,
			                       qwTargetSize,pImageHead,pBackupOption,FALSE
								  )
         )
	   {
		   return FALSE;
	   }

	   // infact the file size can be calculate out
	   // it is the size of image head and dwDataSize;
	   
	   ulTargetSize.LowPart=GetFileSize(hCurrentTarget,&(ulTargetSize.HighPart));

	   qwTargetSize=ulTargetSize.QuadPart;
  }
  return TRUE;
}


BOOL OpenNewTarget(HANDLE &hCurrentTarget, CDATA_ARRAY &TargetArray, BYTE *pBuffer, DWORD dwRequestBytes, DWORD WrittenBytes, unsigned __int64 &qwTargetSize, IMAGE_HEAD *pSourceHead,PBACKUPOPTION pBackupOption,BOOL bDiskFull)
{
	// Max File Size reached open a new file to store data
	// or 
	// target disk full, open a new file to store data
	ULARGE_INTEGER qwFileSize;
				
	// get the file size.
	qwFileSize.LowPart=GetFileSize( hCurrentTarget,&(qwFileSize.HighPart));

	DWORD dwCurrentTargetNumber;
	dwCurrentTargetNumber=TargetArray.GetUpperBound();

	// adjust the last element of TargetArray;
	// ULARGE_INTEGER to LARGE_INTEGER perhaps can transfer

	(TargetArray.ElementAt(dwCurrentTargetNumber)).qwFileSize.QuadPart=qwFileSize.QuadPart;

    CloseHandle( hCurrentTarget);
	hCurrentTarget=NULL;

	WriteSignature(TargetArray.GetAt(TargetArray.GetUpperBound()).szFileName, TargetArray);
	
	WCHAR szTargetName[MAX_PATH];

	// here we get a new file name and a new handle .
	if(GetNewTargetHandle(hCurrentTarget,szTargetName,TargetArray,pBackupOption,bDiskFull)!=TRUE)
	{
		return FALSE;
	}

    // create a serial number
	GUID OneGuid;
    ::CoCreateGuid(&OneGuid);
    CString strGuid;
    GetGuidString(strGuid.GetBuffer(GUID_SIZE),OneGuid);
	strGuid.ReleaseBuffer();

	TABLE_DATA TargetData;
	memset(&TargetData,0,sizeof(TargetData));
    
	// record the serial number.
	wcsncpy(TargetData.szSerialNumber,(LPCTSTR)strGuid,IMAGE_SERIALNUMBER_SIZE-1);

	// CurrentTargetNumber increase
	dwCurrentTargetNumber++;

	TargetData.dwFileNumber=dwCurrentTargetNumber;
	wcsncpy(TargetData.szFileName,szTargetName,MAX_PATH-1);
	
	TargetArray.Add(TargetData);
	
	// in the future, we should judge the disk space should be larger than
	// IMAGE_HEAD
	WriteTargetHead( hCurrentTarget,pSourceHead,dwCurrentTargetNumber,TargetData);
	qwTargetSize=sizeof(IMAGE_HEAD);

	// write the left buffer
	// now I just assume the disk space is enough for writting the left bytes.
	WriteFile( hCurrentTarget,pBuffer+WrittenBytes,dwRequestBytes-WrittenBytes,&WrittenBytes,NULL);
	qwTargetSize=qwTargetSize+WrittenBytes;

	return TRUE;
}


BOOL GetOneFileSize(HANDLE hFile, unsigned __int64 *pqwFileSize)
{
	(*pqwFileSize) = 0;
	ULARGE_INTEGER OneFileSize;
	OneFileSize.QuadPart=0;

	OneFileSize.LowPart = GetFileSize(hFile,&(OneFileSize.HighPart));
	
	if(
		( -1 == OneFileSize.LowPart)
		&&
		( GetLastError() != NO_ERROR ) 
	  )
	{
		return FALSE;
	}
	else
	{
	  (*pqwFileSize) = OneFileSize.QuadPart;
	  return TRUE;
	}

	// not reached
	return FALSE;
}


BOOL WriteSignature(LPCTSTR szImageFile, CDATA_ARRAY &TargetArray)
{
	__int64 lFilePointer;
	HANDLE hImageFile;

	IMAGE_HEAD ImageHead;

	hImageFile=CreateFile(szImageFile,GENERIC_READ|GENERIC_WRITE,
		                  FILE_SHARE_READ,NULL,
						  OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,
						  NULL);

	if(hImageFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	YGSetFilePointer(hImageFile,0,FILE_BEGIN,lFilePointer);

	DWORD dwReadBytes;

	DWORD WrittenBytes;
	
	ReadFile(hImageFile,&ImageHead,sizeof(ImageHead),&dwReadBytes,NULL);

	ImageHead.dwFinishSignature = IMAGE_STATUS_COMPLETE;
	CString szFileName;
	CString szFileTitle;
	DWORD dwFileIndex = 0;
	BOOL bFound = FALSE;
	for (int i = 1; i < TargetArray.GetSize(); ++i)
	{
		szFileName = TargetArray[i].szFileName;
		if (GetFileAttributes(szFileName) == -1)
		{
			continue;
		}
		if (wcscmp(szImageFile, (LPCTSTR)szFileName) == 0)
		{
			continue;
		}
		HANDLE f=CreateFile(szFileName,GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ,NULL,
			OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,
			NULL);

		if(f == INVALID_HANDLE_VALUE)
		{
			continue;
		}
		IMAGE_HEAD temp;
		ZeroMemory(&temp, sizeof(IMAGE_HEAD));
		DWORD rt;
		ReadFile(f,&temp,sizeof(temp),&rt,NULL);
		CloseHandle(f);
		szFileTitle = temp.szLastFileName;
		bFound = true;
		if (temp.dwFileIndex > dwFileIndex)
		{
			dwFileIndex = temp.dwFileIndex;
		}
	}

	if (!bFound || szFileTitle.GetLength() == 0 || dwFileIndex == 0)
	{
		szFileTitle = szImageFile;
		szFileTitle = szFileTitle.Mid(szFileTitle.ReverseFind(L'\\') + 1);
		szFileTitle = szFileTitle.Left(szFileTitle.Find(L'.'));
		dwFileIndex = 0;
	}
	
	ImageHead.dwFileIndex = dwFileIndex + 1;
	ZeroMemory(ImageHead.szLastFileName, MAX_PATH * sizeof(WCHAR));
	wcsncpy(ImageHead.szLastFileName, (LPCTSTR)(szFileTitle), szFileTitle.GetLength());

	YGSetFilePointer(hImageFile,0,FILE_BEGIN,lFilePointer);

	WriteFile(hImageFile,&ImageHead,sizeof(ImageHead),&WrittenBytes,NULL);

	CloseHandle(hImageFile);

	return TRUE;

}


/*=============================================================
 *
 *  Function Name: GetNewTargetHandle()
 *  Purpose:       when the disk is full, a new file name is needed.
 *                 we generate a default file name, the user may 
 *                 change the default file name.
 *
 *=============================================================*/

BOOL GetNewTargetHandle(HANDLE &hCurrentTarget,WCHAR *szTargetName,	CDATA_ARRAY &TargetArray,PBACKUPOPTION pBackupOption,BOOL bDiskFull)
{
	WCHAR strPathName[MAX_PATH];
	ZeroMemory(strPathName, MAX_PATH * sizeof(WCHAR));
	BOOL bContinueSelect = TRUE;
    CString strDefaultName;
	CString strImageName;

	GetLongMainName(TargetArray[1].szFileName,strImageName.GetBuffer(MAX_PATH));
	strImageName.ReleaseBuffer();

	CString strNumber;
	
	strNumber.Format(L"f%.3d",TargetArray.GetUpperBound()+1);

	SYSTEMTIME OneSysTime;
	GetLocalTime(&OneSysTime);
	CString strShowTime;

	strShowTime.Format(L"%.4d%.2d%.2d_%.2d%.2d",OneSysTime.wYear,OneSysTime.wMonth,OneSysTime.wDay,OneSysTime.wHour,OneSysTime.wMinute);

	strDefaultName=strImageName + L"_" + strShowTime + L"_" + strNumber + AM_FILE_SUFFIX;
	
	while(bContinueSelect)
	{
		WCHAR szDefaultFile[MAX_PATH];
		memset(szDefaultFile,0,sizeof(szDefaultFile));
		wcsncpy(szDefaultFile,(LPCTSTR)strDefaultName,MAX_PATH-1);

		WCHAR szNewFileName[MAX_PATH];
		memset(szNewFileName,0,sizeof(szNewFileName));

		if (!bDiskFull && pBackupOption && pBackupOption->bAutoSpawn && CheckFreeSpace((LPCTSTR)TargetArray.GetAt(1).szFileName))
		{
			WCHAR *p;
			wcsncpy(szNewFileName,(LPCTSTR)TargetArray.GetAt(1).szFileName,MAX_PATH-1);
			p = strrchrpro(szNewFileName,L'\\');
			if (p)
			{
				p++;
				wcsncpy(p,(LPCTSTR)strDefaultName,strDefaultName.GetLength());
			}
		}
		else
		{
			if( SelectFilePro(L"AMG",L"AM01 Image File",szDefaultFile,szNewFileName,TRUE) !=TRUE )
			{
				return FALSE;
			}
		}
		
		// strPathName=szNewFileName;
		wcsncpy(strPathName, szNewFileName, MAX_PATH);

		if (CheckFreeSpace((LPCTSTR)strPathName))
		{
			hCurrentTarget=CreateFile((LPCTSTR)strPathName,GENERIC_READ|GENERIC_WRITE,
							FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
			bContinueSelect=FALSE;
			break;
		}
		else
		{
			AfxMessageBox(IDS_DISK_FULL,MB_OK|MB_ICONINFORMATION);
		}
	}

	wcsncpy(szTargetName,(LPCTSTR)strPathName,MAX_PATH - 1);

	return TRUE;
}


void GetGuidString(WCHAR *szGuid,GUID OneGuid)
{
	CString strGuid;
	
	// load appropriate formatting string
	CString strFormat;
	
	// strFormat=L"{%08lX-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}";
	
	//-- GUID format modify begin
	// still less one char, but I think 31 characters is also OK.
	strFormat=L"%08lX%04X%04x%02X%02X%02X%02X%02X%02X%02X%02X";
	//-- GUID format modify end

	// then format into destination
	strGuid.Format(strFormat,
		// first copy...
		OneGuid.Data1, OneGuid.Data2, OneGuid.Data3,
		OneGuid.Data4[0], OneGuid.Data4[1], OneGuid.Data4[2], OneGuid.Data4[3],
		OneGuid.Data4[4], OneGuid.Data4[5], OneGuid.Data4[6], OneGuid.Data4[7]
		);

	memset(szGuid,0,GUID_SIZE*sizeof(WCHAR));
	wcsncpy(szGuid,(LPCTSTR)strGuid,GUID_SIZE-1);
}

/*============================================================
 *
 * Function Name: WriteTargetHead()
 * Purpose:       when creating a new image, a target head
 *                should be added
 * comment:       for BM2, the image head has more items
 *                ( dwCompressLevel is placed in IndexData )
 *
 *                It seems that we often call this function
 *                when we add one TableData
 *
 *============================================================*/

BOOL WriteTargetHead(HANDLE hCurrentTarget, IMAGE_HEAD *pSourceHead, DWORD dwCurrentTargetNumber, TABLE_DATA TargetData)
{
	// write target file head
	IMAGE_HEAD TargetHead;
	__int64 lFilePointer;
	DWORD WrittenBytes;

	memset(&TargetHead,0,sizeof(TargetHead));

	TargetHead.dwFileNumber = dwCurrentTargetNumber;
	TargetHead.dwFileVersion=pSourceHead->dwFileVersion;
	TargetHead.wBackupType=pSourceHead->wBackupType;
	
	// in fact dwCompressLevel is placed in IndexData
	// so the data stored in TargetHead is not so important
	
	TargetHead.dwCompressLevel=pSourceHead->dwCompressLevel;

	// 2004.05.21 for encryption begin
	TargetHead.bEncrypt = pSourceHead->bEncrypt;
	if(pSourceHead->bEncrypt)
	{
	  wcsncpy(TargetHead.pchPassword,pSourceHead->pchPassword,PASSWORD_SIZE);
	}
	// 2004.05.21 for encryption end

	// this is identity
	wcsncpy(TargetHead.szImageIdentity,pSourceHead->szImageIdentity,IMAGE_IDENTITY_SIZE );

    // serial number
	// and something others will be write in the future

	// now we copy the serail number
	// the serial number is com from the taregt data
	// and other thing is come from SourceHead
	wcsncpy(TargetHead.szSerialNumber,TargetData.szSerialNumber ,IMAGE_SERIALNUMBER_SIZE-1);

	wcsncpy(TargetHead.szComment,pSourceHead->szComment,IMAGE_COMMENT_SIZE-1);

	YGSetFilePointer(hCurrentTarget,0,FILE_BEGIN,lFilePointer);

	if( FALSE == WriteFile(hCurrentTarget,&TargetHead,sizeof(TargetHead),
		                   &WrittenBytes,NULL) )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL SelectFilePro(LPCTSTR szFileExt,LPCTSTR szFileType,LPCTSTR szDefaultFile, WCHAR *szFileName, BOOL bOpenFile)
{

	BOOL bResult = FALSE;
	CString strFilter;
	strFilter.Format(L"%s (*.%s)|*.%s||",szFileType,szFileExt,szFileExt);

	CFileDialog dlg (bOpenFile, szFileExt, szDefaultFile, OFN_HIDEREADONLY, strFilter);

	// if user selected a file then open it as a database
	if (IDOK == dlg.DoModal())
	{
		wcscpy(szFileName,dlg.GetPathName());
		bResult = TRUE;
	}
	return bResult;
}

BOOL CheckFreeSpace( LPCTSTR DriveLetter)
{
	DWORD dwSectorsPerCluster;
    DWORD dwBytesPerSector;
	DWORD dwNumberOfFreeClusters;
	DWORD dwTotalNumberOfClusters;
	unsigned __int64 qwFreeSpace = 0;
	unsigned __int64 qwMinFreeSpace;

	qwMinFreeSpace = sizeof(IMAGE_HEAD) + sizeof(DATA_NODE) + AM_COMPRESS_BUFFER;


	CString csRootPath;
	csRootPath=DriveLetter;

	// csRootPath=csRootPath.Left(1)+ L":" + "\\" ;

	// 2004.03.23 huapage added begin
	// for share folder

	// L"\\" should be written as "\\\\"
	if( 0 == csRootPath.Left(2).Compare(L"\\\\") ) 
	{
	    int nLeftSize;
		nLeftSize = csRootPath.ReverseFind(L'\\');
		csRootPath = csRootPath.Left(nLeftSize+1);
	}
	else
	{
	    csRootPath=csRootPath.Left(1)+ L":" + L"\\" ;
	}

	// 2004.03.23 huagpae added end
	
	if( FALSE == GetDiskFreeSpace((LPCTSTR)csRootPath,
									&dwSectorsPerCluster,
									&dwBytesPerSector,
									&dwNumberOfFreeClusters,
									&dwTotalNumberOfClusters)
	                              )
	{
		TRACE(L"\nGetDiskFreeSpace error in CheckFreeSpace.");
	}

	int nErrorCode;
	nErrorCode=GetLastError();
	if(nErrorCode!=0)
	{
		return FALSE;
	}

	qwFreeSpace = dwNumberOfFreeClusters;
	qwFreeSpace = qwFreeSpace * dwSectorsPerCluster * dwBytesPerSector;

	return (qwMinFreeSpace < qwFreeSpace);
}


/*======================================================================
 *
 * Function Name: AddOneTargetElement
 * Purpose:       add the first element to the target array
 *
 *======================================================================*/

BOOL AddOneTargetElement(CDATA_ARRAY &TargetArray,LPCTSTR szTargetImage)
{
    if (TargetArray.GetSize()) TargetArray.RemoveAll();

    // The first element of TargetArray is EmptyData
	TABLE_DATA EmptyData;
    memset(&EmptyData,0,sizeof(EmptyData));
    TargetArray.Add(EmptyData);

    // create a serial number
    GUID OneGuid;
    ::CoCreateGuid(&OneGuid);
    CString strGuid;
    GetGuidString(strGuid.GetBuffer(GUID_SIZE),OneGuid);
    strGuid.ReleaseBuffer();

    TABLE_DATA FirstData;
    memset(&FirstData,0,sizeof(FirstData));
    
    // record the serial number.
    wcsncpy(FirstData.szSerialNumber,(LPCTSTR)strGuid,IMAGE_SERIALNUMBER_SIZE-1);

    FirstData.dwFileNumber=1;

    wcsncpy(FirstData.szFileName,szTargetImage,MAX_PATH-1);
	
    TargetArray.Add(FirstData);

	return TRUE;
}


/*=================================================================
 *
 * Function Name:   GetFileHandle
 * Purpose:         Get the required file handle for first backup
 *
 *=================================================================*/

BOOL GetFileHandle(AM_FILE_HANDLE *pFileHandle,
				   LPCTSTR szTargetImage,
				   DWORD *pdwErrorCode)

{

  CString strTargetFileName;
  WCHAR szLongMainName[MAX_PATH];
  if( FALSE == GetLongMainName(szTargetImage,szLongMainName) )
  {
	  TRACE(L"\nGetLongMainName error in GetFileHandle.");
  }
  strTargetFileName = szLongMainName;
  
  // open target data file
  (*pFileHandle).hCurrentTarget=CreateFile(szTargetImage,
                       GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,
                       CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

  if( INVALID_HANDLE_VALUE == (*pFileHandle).hCurrentTarget)
  {
	  (*pdwErrorCode)=GetLastError();
	  //-- for debug
	  TRACE(L"Create Data File Error.");
	  //-- end for debug
	  return FALSE;
  }

  
  CString strTempDir;
  GetTempDir(strTempDir.GetBuffer(MAX_PATH));
  strTempDir.ReleaseBuffer();

  // open total index file
  CString strTotalIndexFilePath;

  strTotalIndexFilePath=strTempDir + L"\\" + strTargetFileName + AM_INDEX_SUFFIX;
  (*pFileHandle).hTotalIndexFile=CreateFile((LPCTSTR)strTotalIndexFilePath,
	              GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,
			      CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

  if( INVALID_HANDLE_VALUE == (*pFileHandle).hTotalIndexFile )
  {
	  (*pdwErrorCode)=GetLastError();
	  //-- for debug
	  TRACE(L"Create Total Index File Error.");
	  //-- end for debug
	  return FALSE;
  }

  // open path array file
  CString strArrayFilePath;
  strArrayFilePath=strTempDir + L"\\" + strTargetFileName + L".ary";
  (*pFileHandle).hArrayFile=CreateFile((LPCTSTR)strArrayFilePath,
             GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,
		     CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

  if( INVALID_HANDLE_VALUE == (*pFileHandle).hArrayFile)
  {
	  (*pdwErrorCode)=GetLastError();
	  //-- for debug
	  TRACE(L"Create Array File Error.");
	  //-- end for debug
	  return FALSE;
  }

  // open current index file
  CString strCurrentIndexFilePath;
  strCurrentIndexFilePath=strTempDir + L"\\" + strTargetFileName + L".id0";
  (*pFileHandle).hCurrentIndexFile=CreateFile((LPCTSTR)strCurrentIndexFilePath,
	         GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,
			 CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);


  if( INVALID_HANDLE_VALUE == (*pFileHandle).hCurrentIndexFile)
  {
	  (*pdwErrorCode)=GetLastError();
	  //-- for debug
	  TRACE(L"Create Current Index File Error.");
	  //-- end for debug
	  return FALSE;
  }
  
  // open stamp file
  CString strStampFilePath;
  strStampFilePath=strTempDir + L"\\" + strTargetFileName + L".stp";
  (*pFileHandle).hStampFile=CreateFile((LPCTSTR)strStampFilePath,
	         GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,
			 CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

  if( INVALID_HANDLE_VALUE == (*pFileHandle).hStampFile)
  {
	  (*pdwErrorCode)=GetLastError();
	  //-- for debug
	  TRACE(L"Create Stamp File Error.");
	  //-- end for debug

	  return FALSE;
  }

  // open table file

  (*pdwErrorCode)=0;
  return TRUE;
}

/*=================================================================
 *
 * Function Name:   OpenFileHandle
 * Purpose:         Get the required file handle for first backup
 *
 *=================================================================*/

BOOL OpenFileHandle(AM_FILE_HANDLE *pFileHandle,
				   LPCTSTR szTargetImage,
				   DWORD *pdwErrorCode)

{

	CString strTargetFileName;
	WCHAR szLongMainName[MAX_PATH];
	if( FALSE == GetLongMainName(szTargetImage,szLongMainName) )
	{
		TRACE(L"\nGetLongMainName error in GetFileHandle.");
	}
	strTargetFileName = szLongMainName;
  
	// open target data file
	(*pFileHandle).hCurrentTarget=CreateFile(szTargetImage,
                       GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,
                       OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if( INVALID_HANDLE_VALUE == (*pFileHandle).hCurrentTarget)
	{
		(*pdwErrorCode)=GetLastError();
		return FALSE;
	}

  
	CString strTempDir;
	GetTempDir(strTempDir.GetBuffer(MAX_PATH));
	strTempDir.ReleaseBuffer();

	// open total index file
	CString strTotalIndexFilePath;

	strTotalIndexFilePath=strTempDir + L"\\" + strTargetFileName + AM_INDEX_SUFFIX;
	(*pFileHandle).hTotalIndexFile=CreateFile((LPCTSTR)strTotalIndexFilePath,
	              GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,
			      OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if( INVALID_HANDLE_VALUE == (*pFileHandle).hTotalIndexFile )
	{
		(*pdwErrorCode)=GetLastError();
		return FALSE;
	}

	// open path array file
	CString strArrayFilePath;
	strArrayFilePath=strTempDir + L"\\" + strTargetFileName + L".ary";
	(*pFileHandle).hArrayFile=CreateFile((LPCTSTR)strArrayFilePath,
             GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,
		     OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if( INVALID_HANDLE_VALUE == (*pFileHandle).hArrayFile)
	{
		(*pdwErrorCode)=GetLastError();
		return FALSE;
	}

	// open stamp file
	CString strStampFilePath;
	strStampFilePath=strTempDir + L"\\" + strTargetFileName + L".stp";
	(*pFileHandle).hStampFile=CreateFile((LPCTSTR)strStampFilePath,
				GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,
				OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if( INVALID_HANDLE_VALUE == (*pFileHandle).hStampFile)
	{
		(*pdwErrorCode)=GetLastError();
		return FALSE;
	}

	// open table file

	(*pdwErrorCode)=0;
	return TRUE;
}

BOOL GetTempDir(WCHAR *szTempDir)
{
  CString strTempDir;
  
  if(FALSE == GetTempPath(MAX_PATH-1,szTempDir) )
  {
	  TRACE(L"\nGetTempPath error in function GetTempDir.");
	  return FALSE;
  }

  strTempDir=szTempDir;
  strTempDir.TrimRight(L"\\");
  memset(szTempDir,0,MAX_PATH*sizeof(WCHAR));
  wcsncpy(szTempDir,(LPCTSTR)strTempDir,MAX_PATH-1);
  return TRUE;
}


BOOL AdjustIndexHead(HANDLE hCurrentIndexFile,DWORD dwPathID)
{
	__int64 lFilePointer;
	DWORD WrittenBytes;
	
	// adjust Current Index file.
	YGSetFilePointer(hCurrentIndexFile,0,FILE_BEGIN,lFilePointer);
	CURRENT_INDEX_NODE CurrentIndexNode;
	
	// use this way to make extend easy
	memset(&CurrentIndexNode,0,sizeof(CurrentIndexNode));
	CurrentIndexNode.dwPathCount=dwPathID;

	if( FALSE == WriteFile(hCurrentIndexFile,&CurrentIndexNode,sizeof(CurrentIndexNode),&WrittenBytes,NULL) )
	{
		TRACE(L"\nWriteFile error in AdjustIndexHead,write current index file.");
		return FALSE;
	}

	return TRUE;
}

BOOL CurrentIdxToTotalIdx(AM_FILE_HANDLE *pFileHandle)
{
	// this function is for new backup.
	__int64 lFilePointer;	
    DWORD dwReadBytes;
    DWORD dwWrittenBytes;

	YGSetFilePointer(pFileHandle->hCurrentIndexFile,0,FILE_BEGIN,lFilePointer);

	YGSetFilePointer(pFileHandle->hTotalIndexFile,0,FILE_BEGIN,lFilePointer);

	// read current index node
	CURRENT_INDEX_NODE CurrentIndexNode;
    
	if(
		FALSE == ReadFile(pFileHandle->hCurrentIndexFile,
		                 &CurrentIndexNode,
						 sizeof(CurrentIndexNode),
						 &dwReadBytes,
						 NULL)
	  )
	  {
		  TRACE(L"\nReadFile error in CurrentIdxToTotalIdx, read hCurrentIndexFile.");
		  return FALSE;
	  }

	// write total index node
	TOTAL_INDEX_NODE TotalIndexNode;

	// write in this way to make extend easy
	memset(&TotalIndexNode,0,sizeof(TotalIndexNode));
      
	// for the first backup, TimeStampCount is 1.
    TotalIndexNode.dwTimeStampCount=1;
	TotalIndexNode.dwTotalPathCount=CurrentIndexNode.dwPathCount;

	if(
		FALSE == WriteFile( pFileHandle->hTotalIndexFile,&TotalIndexNode,
						    sizeof(TotalIndexNode),
						    &dwWrittenBytes,
						    NULL)
      )
	  {
		  TRACE(L"\nWriteFile error in CurrentIdxToTotalIdx, write hTotalIndexFile.");
		  return FALSE;
	  }

	INDEX_DATA IndexData;

    // write total index data
	for(DWORD dwPathCount=0;dwPathCount<CurrentIndexNode.dwPathCount ;dwPathCount++)
	{

	    ReadFile(pFileHandle->hCurrentIndexFile,&IndexData,sizeof(IndexData),
		         &dwReadBytes,NULL);

	    WriteFile(pFileHandle->hTotalIndexFile,&IndexData,sizeof(IndexData),
		          &dwWrittenBytes,NULL);
	}

	return TRUE;

}

BOOL CombineFileToOneImg(AM_FILE_HANDLE *pFileHandle, CDATA_ARRAY &TargetArray)
{
	__int64 lFilePointer;
	DWORD dwReadBytes;
	DWORD WrittenBytes;

	YGSetFilePointer(pFileHandle->hTotalIndexFile,0,FILE_BEGIN,lFilePointer);
	YGSetFilePointer(pFileHandle->hArrayFile,0,FILE_BEGIN,lFilePointer);
	YGSetFilePointer(pFileHandle->hStampFile,0,FILE_BEGIN,lFilePointer);
	YGSetFilePointer(pFileHandle->hCurrentTarget,0,FILE_BEGIN,lFilePointer);

	// read the file head
	IMAGE_HEAD ImageHead;
  
	if( FALSE	== ReadFile(pFileHandle->hCurrentTarget,&ImageHead,sizeof(ImageHead),&dwReadBytes,NULL) )
	{
		DWORD dwErrorNumber;
		dwErrorNumber=GetLastError();
		TRACE(L"\nReadFile error in CombineFileToOneImg, read hCurrentTarget, error number: %d.",dwErrorNumber);
		return FALSE;
	}

	YGSetFilePointer(pFileHandle->hCurrentTarget,0,FILE_END,lFilePointer);
	// read the pointer to the TotalIndexFile
	YGSetFilePointer(pFileHandle->hCurrentTarget,0,FILE_END,lFilePointer);

	ImageHead.TotalIndexPointer.QuadPart=lFilePointer;

	CombineTwoFile(pFileHandle->hCurrentTarget,pFileHandle->hTotalIndexFile);
  
	YGSetFilePointer(pFileHandle->hCurrentTarget,0,FILE_END,lFilePointer);

	ImageHead.ArrayPointer.QuadPart=lFilePointer;

	CombineTwoFile(pFileHandle->hCurrentTarget,pFileHandle->hArrayFile);

	// read the pointer to the stamp
	YGSetFilePointer(pFileHandle->hCurrentTarget,0,FILE_END,lFilePointer);
  
	ImageHead.StampPointer.QuadPart=lFilePointer;

	CombineTwoFile(pFileHandle->hCurrentTarget,pFileHandle->hStampFile);

	// write the new file head
	YGSetFilePointer(pFileHandle->hCurrentTarget,0,FILE_BEGIN,lFilePointer);

	WriteFile(pFileHandle->hCurrentTarget,&ImageHead,sizeof(ImageHead),&WrittenBytes,NULL);

	ULARGE_INTEGER qwLastFileSize;
	qwLastFileSize.LowPart=GetFileSize(pFileHandle->hCurrentTarget,&(qwLastFileSize.HighPart));
  
	// in fact TargetArray.GetSize() should be greater than 0

	if(TargetArray.GetSize()>0)
	{
		TargetArray[TargetArray.GetUpperBound()].qwFileSize.QuadPart=qwLastFileSize.QuadPart;
	}
    
	return TRUE;
}


void CloseFileHandle(AM_FILE_HANDLE *pFileHandle)
{
	if( NULL != pFileHandle->hCurrentTarget)
	{
	   if(FALSE == CloseHandle(pFileHandle->hCurrentTarget) )
	   {
		   TRACE(L"\nClose handle hCurrentTarget error in CloseFileHandle.");
	   }
	}
	
	if( NULL != pFileHandle->hCurrentIndexFile)
	{
	   if( FALSE == CloseHandle(pFileHandle->hCurrentIndexFile) )
	   {
		   TRACE(L"\nClose handle hCurrentIndexFile error in CloseFileHandle.");
	   }
	}

	if( NULL != pFileHandle->hArrayFile)
	{
	   if( FALSE == CloseHandle(pFileHandle->hArrayFile) )
	   {
		   TRACE(L"\nClose handle hArrayFile error in CloseFileHandle.");
	   }
	}
	
	if( NULL != pFileHandle->hTotalIndexFile)
	{
	   if( FALSE == CloseHandle(pFileHandle->hTotalIndexFile) )
	   {
		   TRACE(L"\nClose handle hTotalIndexFile error in CloseFileHandle.");
	   }
	}
	
	if( NULL != pFileHandle->hStampFile)
	{
	   if( FALSE == CloseHandle(pFileHandle->hStampFile) )
	   {
		   TRACE(L"\nClose handle hStampFile error in CloseFileHandle.");
	   }
	}
	
	pFileHandle->hCurrentTarget=NULL;
	pFileHandle->hCurrentIndexFile=NULL;
	pFileHandle->hArrayFile=NULL;
	pFileHandle->hTotalIndexFile=NULL;
	pFileHandle->hStampFile=NULL;
}

BOOL CombineTwoFile(HANDLE hFirstFile, HANDLE hSecondFile)
{
	__int64 lFilePointer;
	DWORD WrittenBytes;
	  
	YGSetFilePointer(hFirstFile,0,FILE_END,lFilePointer);

	YGSetFilePointer(hSecondFile,0,FILE_BEGIN,lFilePointer);

	DWORD ReadBytes;

	BYTE *CopyString;
		  
	CopyString=(BYTE *)malloc(AM_BUFFER_SIZE);

	if( CopyString == NULL )
	{
		AfxMessageBox(IDS_NOT_ENOUGH_MEMORY);
		exit(1);
	}
    else
	{

		BOOL ReadResults=FALSE;
		  
		//-- end debug

		ReadResults=ReadFile(hSecondFile,CopyString,
		              AM_BUFFER_SIZE,&ReadBytes,NULL);

		//-- for debug

		DWORD ErrorCode=GetLastError();

		//-- end for debug
			  
		while(ReadBytes==AM_BUFFER_SIZE)
		{
			WriteFile(hFirstFile,CopyString,
		  	            ReadBytes,&WrittenBytes,NULL);
		      
			//-- for debug
			ErrorCode=GetLastError();
			//-- end for debug

 	   		ReadFile(hSecondFile,CopyString,AM_BUFFER_SIZE,&ReadBytes,NULL);

			//-- for debug
			ErrorCode=GetLastError();
			//-- end for debug

		}

		WriteFile(hFirstFile,CopyString,ReadBytes,&WrittenBytes,NULL);

		free(CopyString);
			  
		CopyString=NULL;
	}

	return TRUE;
}


/*=====================================================================
 *
 * Function Name:  RecordFileTable()
 * Purpose:        Record the array of target file name
 *                 to the end of the file. (Original as Stamp file)
 *                 adjust the filepointer in the image head
 *
 *=====================================================================*/

BOOL RecordFileTable(CDATA_ARRAY &TargetArray)
{
	ASSERT(TargetArray.GetSize()>=2);
    DWORD dwFileNumber;
	__int64 lFilePointer;
	DWORD dwLastFileNumber;

	DWORD dwReadBytes;
	DWORD WrittenBytes;

	dwLastFileNumber=(DWORD)(TargetArray.GetSize())-1;

	HANDLE hLastTarget;
	hLastTarget=CreateFile(TargetArray.GetAt(dwLastFileNumber).szFileName,
			GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,
			NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	LARGE_INTEGER TablePointer;

	YGSetFilePointer(hLastTarget,0,FILE_END,TablePointer.QuadPart);

	// recored table node
	TABLE_NODE TableNode;
	memset(&TableNode,0,sizeof(TableNode));

    TableNode.dwFileCount=dwLastFileNumber;

	WriteFile(hLastTarget,&TableNode,sizeof(TableNode),
		         &WrittenBytes,NULL);

	// record table data
	TABLE_DATA TableData;
    for(dwFileNumber=1; dwFileNumber<dwLastFileNumber; dwFileNumber++)
	{
		TableData=TargetArray.GetAt(dwFileNumber);
		WriteFile(hLastTarget,&TableData,sizeof(TableData),
			      &WrittenBytes,NULL);
	}

	// record the last file data
	TableData=TargetArray.GetAt(dwLastFileNumber);
	TableData.qwFileSize.QuadPart = TableData.qwFileSize.QuadPart
		                            + sizeof(TABLE_NODE)
									+ sizeof(TABLE_DATA) * dwLastFileNumber;

	WriteFile(hLastTarget,&TableData,sizeof(TableData),
			      &WrittenBytes,NULL);

	// perhaps we should check the return value here
	// since we just calculate the size and write it.

	// record table pointer
	IMAGE_HEAD ImageHead;

	YGSetFilePointer(hLastTarget,0,FILE_BEGIN,lFilePointer);
	
	ReadFile(hLastTarget,&ImageHead,sizeof(ImageHead),&dwReadBytes,NULL);
	
	ImageHead.FileTablePointer.QuadPart=TablePointer.QuadPart;

	// to mark the file as the last file.
	ImageHead.dwFileNumber = LAST_FILE_NUMBER;

	YGSetFilePointer(hLastTarget,0,FILE_BEGIN,lFilePointer);
	
	if( FALSE == WriteFile(hLastTarget,&ImageHead,sizeof(ImageHead),&WrittenBytes,NULL) )
	{
		CloseHandle(hLastTarget);
		return FALSE;
	}
	
	CloseHandle(hLastTarget);

	// AdjustLastFileName should adjust TargetArray too.
	// since TargetArray will be used by CreateDir() function.
	if( FALSE == AdjustLastFileName(TargetArray) )
	{
		return FALSE;
	}

	return TRUE;
}


/*==============================================================
 *
 *  Function Name: AdjustLastFileName()
 *  Purpose:       Adjust the last file name to make thing clear
 *                 the last file will begin with e003, etc
 *                 the middle file will begin with f005, etc
 *                 the serial number is the place that the file in
 *                 the file array.
 *
 *==============================================================*/

BOOL AdjustLastFileName(CDATA_ARRAY &TargetArray)
{
	HANDLE hLastFile;
	__int64 lFilePointer;
	TABLE_DATA OneTableData;

	// get the new file name
	CString strLastFileName;
	CString strNewFileName;
	
	// strLastFileName=szLastFileName;

	strLastFileName = TargetArray.GetAt(TargetArray.GetUpperBound()).szFileName;

	
	// the following numbers: NAME_TAIL_SIZE depends one the file name format that
	// we selected
	
	if(
		( ( (strLastFileName.Right(NAME_TAIL_SIZE)).Left(2) ).CompareNoCase(MIDDLE_FILE_SIGNATURE) == 0 )
	  )
	{
		strNewFileName=strLastFileName.Left(strLastFileName.GetLength()-NAME_TAIL_SIZE) + END_FILE_SIGNATURE + strLastFileName.Right(NAME_TAIL_SIZE-2);
	}
	else
	{
		TRACE(L"\nThe file name need not be changed.\n");
		return TRUE;
	}

	// read the last table data
	hLastFile=CreateFile((LPCTSTR)strLastFileName,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,
		                 OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	
	if( INVALID_HANDLE_VALUE == hLastFile )
	{
		TRACE(L"\nOpen last file error while adjusting last file name");
		return FALSE;
	}

	__int64 nFileOffset = 0;
	nFileOffset -= sizeof(OneTableData);

	YGSetFilePointer(hLastFile,nFileOffset,FILE_END,lFilePointer);
	
	DWORD dwReadBytes;
	ReadFile(hLastFile,&OneTableData,sizeof(OneTableData),&dwReadBytes,NULL);
	if(dwReadBytes != sizeof(OneTableData))
	{
		TRACE(L"\nRead last file error.");
		CloseHandle(hLastFile);
		return FALSE;
	}

	wcsncpy(OneTableData.szFileName,(LPCTSTR)strNewFileName,MAX_PATH-1);
	
    YGSetFilePointer(hLastFile,nFileOffset,FILE_END,lFilePointer);

	DWORD WrittenBytes;

	WriteFile(hLastFile,&OneTableData,sizeof(OneTableData),&WrittenBytes,NULL);

	if(sizeof(OneTableData) != WrittenBytes)
	{
		TRACE(L"\nWrite Table data error.");
		CloseHandle(hLastFile);
		return FALSE;
	}

	CloseHandle(hLastFile);

	// change TargetArray element.
	TargetArray[TargetArray.GetUpperBound()]=OneTableData;

	MoveFile( (LPCTSTR)strLastFileName,(LPCTSTR)strNewFileName);

	return TRUE;
}


/*==============================================================
 *
 *  Function Name: CreateDirFilePro()
 *  Purpose:       Create a dir file to record all the Target
 *                 file. for the convience to Locate the last
 *				   file in backup & restore via setting name.
 *  Note:          This function is similar to RecordFileTable()
 *
 *                 in the future the dir file will be place
 *                 in the directory that Backup Master installed
 * 
 *==============================================================*/
BOOL CreateDirFilePro(CDATA_ARRAY &TargetArray)
{
	ASSERT(TargetArray.GetSize()>=2);
    DWORD dwFileNumber;
	__int64 lFilePointer;
	DWORD dwLastFileNumber;

	// DWORD dwReadBytes;
	DWORD WrittenBytes;

	dwLastFileNumber=(DWORD)(TargetArray.GetSize())-1;

	WCHAR szDirFileName[MAX_PATH];
	
	// we will put the dir file
	// at the same place as the last file

	GetMainFileName(TargetArray[dwLastFileNumber].szFileName,szDirFileName);

	// the wcslen of szDirFileName should not be very long
	// since it is come from GetMainFileName

	// if the dir file exists, create a bak file.
	WCHAR szDirBakFileName[MAX_PATH];
	wcsncpy(szDirBakFileName,szDirFileName,MAX_PATH-1);

	wcscat(szDirBakFileName,L"_dir.bak");
	wcscat(szDirFileName,L".dir");
	
	if(GetFileAttributes(szDirFileName) != -1 )
	{
		// if the file Exists

		SetFileAttributes(szDirFileName,FILE_ATTRIBUTE_NORMAL);

		if( MoveFile(szDirFileName,szDirBakFileName) == FALSE)
		{
			// here show one message
			// if in the future the dir file can not be opened
			// will return false.
			TRACE(L"\n Can not make backup dir file.");
		}
	}

	HANDLE hDirFile;
	
	// always create a dir file.
	// now we didn't form the bak dir file.

	hDirFile=CreateFile(szDirFileName,
			 GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,
			 NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

	if( INVALID_HANDLE_VALUE == hDirFile )
	{
		TRACE(L"\n Failed to open dir file.");
		MoveFile(szDirBakFileName,szDirFileName);
		return FALSE;
	}

	DIR_FILE_HEAD DirFileHead;
	memset(&DirFileHead,0,sizeof(DirFileHead));
	wcsncpy(DirFileHead.chDirSignature,DIR_FILE_IDENTITY,wcslen(DIR_FILE_IDENTITY));
	
	// write dir file head
	WriteFile(hDirFile,&DirFileHead,sizeof(DirFileHead),&WrittenBytes,NULL);

	// record table node
	TABLE_NODE TableNode;
	memset(&TableNode,0,sizeof(TableNode));

    TableNode.dwFileCount=dwLastFileNumber;
	WriteFile(hDirFile,&TableNode,sizeof(TableNode),
		      &WrittenBytes,NULL);

	// record table data
	TABLE_DATA TableData;
	
	// now we needn't adjust the file size of the last file
	// so we just use one loop to solve this problem

    // for(dwFileNumber=1; dwFileNumber<dwLastFileNumber; dwFileNumber++)
	for(dwFileNumber=1; dwFileNumber<=dwLastFileNumber; dwFileNumber++)
	{
		TableData=TargetArray.GetAt(dwFileNumber);
		WriteFile(hDirFile,&TableData,sizeof(TableData),
			      &WrittenBytes,NULL);
		if(sizeof(TableData) != WrittenBytes)
		{
			TRACE(L"\n Error while recording TableData");
			CloseHandle(hDirFile);
			MoveFile(szDirBakFileName,szDirFileName);
			return FALSE;
		}

	}

	GetChecksum(hDirFile,sizeof(DirFileHead),&(DirFileHead.qwCheckSum));
	
	YGSetFilePointer(hDirFile,0,FILE_BEGIN,lFilePointer);
	
	WriteFile(hDirFile,&DirFileHead,sizeof(DirFileHead),&WrittenBytes,NULL);
	
	CloseHandle(hDirFile);
	DeleteFile(szDirBakFileName);
	return TRUE;
}


/*=======================================================
 *
 *  Function Name: GetMainFileName()
 *  Purpose:       get the main file name from the name 
 *                 which have data and minutes
 *  
 *=======================================================*/

BOOL GetMainFileName(LPCTSTR szSourceFileName,WCHAR *szMainFileName)
{
  if( wcslen(szSourceFileName) >= MAX_PATH )
  {
	  return FALSE;
  }

  // here the szSourceFileName Maybe the full path name.
  // and it may be only the file name.
  // the program can deal with two cases

  // here only consider the input parameter is only the file name.
  if( wcslen(szSourceFileName)<=LONG_NAME_SIZE)
  {
	  // directory copy szSourceFileName to szMainFileName
	  wcsncpy(szMainFileName,szSourceFileName,MAX_PATH-1);
  }

  CString strSourceFileName;
  strSourceFileName=szSourceFileName;

  if( (strSourceFileName.Right(4)).CompareNoCase(AM_FILE_SUFFIX) == 0 )
  {
	  // cut the AM_FILE_SUFFIX
	  strSourceFileName= strSourceFileName.Left(strSourceFileName.GetLength()-4);
  }
  
  // cut the additional serial number
  if(
	  ( (strSourceFileName.Right(5)).Left(2) == L"_f") ||
	  ( (strSourceFileName.Right(5)).Left(2) == L"_F") 
	)
  {
	  // for example: vcpp32_2_20030915_1745_f002.amg
	  strSourceFileName=strSourceFileName.Left(strSourceFileName.GetLength()-5);
  }

  //-- in some cases when burning to CD-R there are two _f002
  //-- we just added this part to filter it 
  
  if(
	  ( (strSourceFileName.Right(5)).Left(2) == L"_f") ||
	  ( (strSourceFileName.Right(5)).Left(2) == L"_F") 
	)
  {
	  // for example: vcpp32_2_20030915_1745_f002.amg
	  strSourceFileName=strSourceFileName.Left(strSourceFileName.GetLength()-5);
  }

  // now is :vcpp32_2_20030915_1745
  // cut minute

  if(  
	   ( (strSourceFileName.Right(5)).Left(1) == L"_" ) &&
	   ( (strSourceFileName.Right(14)).Left(1) == L"_")
    )
  {
	  strSourceFileName=strSourceFileName.Left(strSourceFileName.GetLength()-14);
  }

  // if strSourceFileName does not match the regular that stated
  // we simply copy strSourceFileName to szMainFileName
  wcsncpy(szMainFileName,(LPCTSTR)strSourceFileName,MAX_PATH-1);

  return TRUE;
   
}

BOOL GetChecksum(HANDLE hFile,DWORD dwOffset,ULONGLONG *pqwCheckSum)
{
	__int64 lFilePointer;

	YGSetFilePointer(hFile,dwOffset,FILE_BEGIN,lFilePointer);
	
	(*pqwCheckSum)=0;

	BYTE *pBuffer;
	
	pBuffer=(BYTE *)malloc(AM_BUFFER_SIZE);

	if(NULL == pBuffer)
	{
		TRACE(L"\nNot Enough memory");
		return FALSE;
	}

	DWORD dwReadBytes;
	dwReadBytes=0;

	do
	{
		ReadFile(hFile,pBuffer,AM_BUFFER_SIZE,&dwReadBytes,NULL);

		DWORD dwBufferIndex;
		
		for(dwBufferIndex=0;dwBufferIndex<dwReadBytes;dwBufferIndex++)
		{
			(*pqwCheckSum)=(*pqwCheckSum)+pBuffer[dwBufferIndex];
		}

	}while(dwReadBytes>0);
		
	free(pBuffer);
	pBuffer=NULL;

    return TRUE;

}


void ClearExtraFile(LPCTSTR TargetFileName)
{
	// CString strDir(TargetDirectory);

	CString strDir;
	GetTempDir(strDir.GetBuffer(MAX_PATH));
	strDir.ReleaseBuffer();
	CString strName(TargetFileName);

	CString strCurrentIdx;
	CString strTotalIdx;
	CString strArrayFile;
	CString strStampFile;
	CString strTableFile;

	strCurrentIdx=strDir+L"\\"+strName+L".id0";
	strTotalIdx=strDir+L"\\"+strName+AM_INDEX_SUFFIX;
	strArrayFile=strDir+L"\\"+strName+L".ary";
	strStampFile=strDir+L"\\"+strName+L".stp";
	// strTableFile=strDir+L"\\"+strName+L".tab";

	DeleteFile((LPCTSTR)strCurrentIdx);
	DeleteFile((LPCTSTR)strTotalIdx);
	DeleteFile((LPCTSTR)strArrayFile);
	DeleteFile((LPCTSTR)strStampFile);
	// DeleteFile((LPCTSTR)strTableFile);

}


/*===============================================================
 *
 * Function Name: GetSeperateFile()
 *
 * Purpose:       Based on szImageFile, create index and array
 *                file in the target directory( usually temp dir)
 *
 * note:          This function is used while restore and squeeze
 *                image
 *
 *==============================================================*/

BOOL GetSeperateFile(LPCTSTR szImageFile,LPCTSTR szMainName,LPCTSTR TargetDirectory)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());
  
  CString strTargetDirectory;
  CString strMainFileName;
  CString strImageFile;

  strImageFile=szImageFile;

  strTargetDirectory=TargetDirectory;
  strMainFileName = szMainName;
	
  // open data file
  HANDLE hDataFile;
  hDataFile=CreateFile(szImageFile,
                       GENERIC_READ,FILE_SHARE_READ,NULL,
                       OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

  if ( INVALID_HANDLE_VALUE == hDataFile)
  {
	  // open file error, then return.
	  return FALSE;
  }

  // open TotalIndexFile
  HANDLE hTotalIndexFile;
  CString strTotalIndexFilePath;
  strTotalIndexFilePath=strTargetDirectory + L"\\" + strMainFileName + AM_INDEX_SUFFIX;

  hTotalIndexFile=CreateFile((LPCTSTR)strTotalIndexFilePath,
	              GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,
			      CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

  if(INVALID_HANDLE_VALUE == hTotalIndexFile )
  {
	  return FALSE;
  }
	  

  // open ArrayFile
  HANDLE hArrayFile;
  CString strArrayFilePath;
  strArrayFilePath=strTargetDirectory + L"\\" + strMainFileName + L".ary";
  hArrayFile=CreateFile((LPCTSTR)strArrayFilePath,
             GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,
		     CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

  if(INVALID_HANDLE_VALUE == hArrayFile )
  {
	  return FALSE;
  }

  DWORD dwReadBytes;

  IMAGE_HEAD ImageHead;
  ReadFile(hDataFile,&ImageHead,sizeof(ImageHead),&dwReadBytes,NULL);

  unsigned __int64 qwTotalIndexFileLength;
  qwTotalIndexFileLength=ImageHead.ArrayPointer.QuadPart - ImageHead.TotalIndexPointer.QuadPart;

  unsigned __int64 qwArrayFileLength;
  qwArrayFileLength=ImageHead.StampPointer.QuadPart-ImageHead.ArrayPointer.QuadPart;

  if( GetOneSeperateFile(hDataFile,
	                     hTotalIndexFile,
	                     ImageHead.TotalIndexPointer,
					     qwTotalIndexFileLength) == FALSE )
  {
	  CloseHandle(hDataFile);
	  CloseHandle(hTotalIndexFile);
	  CloseHandle(hArrayFile);
	  TRACE(L"\nGetOneSepreateFile error in function GetSeperate file. a index file.");
	  return FALSE;
  }

  if( GetOneSeperateFile(hDataFile,
	                     hArrayFile,
	                     ImageHead.ArrayPointer,
					     qwArrayFileLength) == FALSE )
  {
      CloseHandle(hDataFile);
      CloseHandle(hTotalIndexFile);
      CloseHandle(hArrayFile);
	  TRACE(L"\nGetOneSepreateFile error in function GetSeperate file. a data file.");
	  return FALSE;
  }

  CloseHandle(hDataFile);
  CloseHandle(hTotalIndexFile);
  CloseHandle(hArrayFile);

  return TRUE;
}

BOOL GetOneSeperateFile(HANDLE hFirstFile, HANDLE hSecondFile, LARGE_INTEGER FilePointer, unsigned __int64 qwFileSize)
{
	__int64 lFilePointer;
	DWORD WrittenBytes;

	YGSetFilePointer(hFirstFile,FilePointer.QuadPart,FILE_BEGIN,lFilePointer);

	YGSetFilePointer(hSecondFile,0,FILE_BEGIN,lFilePointer);

	DWORD ReadBytes;

	char *CopyString;
		  
	CopyString=(char *)malloc(AM_BUFFER_SIZE);

	if( CopyString == NULL )
	{
		  AfxMessageBox(IDS_NOT_ENOUGH_MEMORY,MB_OK|MB_ICONINFORMATION,NULL);
		  return FALSE;
	}
    else
	{
		unsigned __int64 qwBytesToCopy;
		for(qwBytesToCopy=qwFileSize;
		      qwBytesToCopy>=AM_BUFFER_SIZE;
			  qwBytesToCopy=qwBytesToCopy-AM_BUFFER_SIZE)
		{

		     ReadFile(hFirstFile,CopyString,
		              AM_BUFFER_SIZE,&ReadBytes,NULL);

		     WriteFile(hSecondFile,CopyString,
		  	            ReadBytes,&WrittenBytes,NULL);
			 
			 // if disk is full, or if the file is in a CD-R
			 // then return FALSE;
			 if(WrittenBytes < ReadBytes)
			 {
				 free(CopyString);
				 CopyString = NULL;
				 return FALSE;
			 }
				 
		}

		DWORD dwBytesLeft;
		dwBytesLeft=(DWORD)qwBytesToCopy;

 	   	ReadFile(hFirstFile,CopyString,
			       dwBytesLeft,&ReadBytes,NULL);
		  
		WriteFile(hSecondFile,CopyString,dwBytesLeft,
			        &WrittenBytes,NULL);

		 // if disk is full, or if the file is in a CD-R
		 // then return FALSE;
		if(WrittenBytes < dwBytesLeft)
		{
			 free(CopyString);
			 CopyString = NULL;
			 return FALSE;
		}

		free(CopyString);
			  
		CopyString=NULL;
	}
	return TRUE;
}

//-- function for squeeze end

//-- function for UI begin

BOOL CreateDirFile(LPCTSTR szLastImage)
{
	CDATA_ARRAY TargetArray;

	DWORD dwErrorCode;

	if(FALSE ==  CheckLastImage(szLastImage,&dwErrorCode) )
	{
		TRACE(L"\nCheckLastImage error in CreateDirFile.");
		return FALSE;
	}
	
	if( FALSE == GetImageArray(TargetArray,szLastImage) )
	{
		TRACE(L"\nGetImageArray Error in CreateDirFile.");
		return FALSE;
	}
	
	if( FALSE == CreateDirFilePro(TargetArray) )
	{
		TRACE(L"\nCreatDirFilePro error in CreateDirFile.");
		return FALSE;
	}

	return TRUE;
}

//-- function for UI end


//-- general function begin

BOOL SelectFolder(HWND hWnd,WCHAR *ach)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

	BOOL    bResult = FALSE;
    IMalloc *pm;
    BROWSEINFO bi;
    LPITEMIDLIST pidl;

    if (!FAILED(CoInitialize(NULL)))
	{
		bi.hwndOwner      = hWnd;
		bi.pidlRoot       = NULL;
		bi.pszDisplayName = ach;
		bi.lpszTitle      = NULL;
		bi.ulFlags        = BIF_RETURNONLYFSDIRS;
		bi.lpfn           = NULL;
		bi.lParam         = 0;
		bi.iImage         = 0;

		pidl = SHBrowseForFolder(&bi);

		if (pidl)
		{
			SHGetPathFromIDList(pidl, ach);

			TrimRightChar(ach,L'\\',MAX_PATH);

			SHGetMalloc(&pm);
			if (pm)
			{
				 pm->Free(pidl);
				 pm->Release();
			}
			bResult = TRUE;
		}
		CoUninitialize();
	}
	return bResult;
}

//-- general function end

// 2004.05.20 encrypt function begin

void EncryptPassword(WCHAR *pchPassword,int nPasswordSize)
{

	if( PASSWORD_SIZE != nPasswordSize )
	{
		TRACE(L"\nThe password size is not correct.");
	}

	unsigned char key[AES::DEFAULT_KEYLENGTH];
	
	// then the password should not more than 16 characters
	memcpy(key,YG_PASSWORD_KEY,sizeof(key));

	CFB_Mode<AES>::Encryption cfbEncryption(key, AES::DEFAULT_KEYLENGTH,(const BYTE*)YG_INITIAL_VECTOR); 
	
	unsigned char chEncryptedPassword[PASSWORD_SIZE];
	memset(chEncryptedPassword,0,sizeof(chEncryptedPassword));
	
	char buf[PASSWORD_SIZE];
	WideCharToMultiByte(CP_ACP, 0, pchPassword, PASSWORD_SIZE, buf, PASSWORD_SIZE, 0, 0);
	// cfbEncryption.ProcessData(chEncryptedPassword,pchPassword,PASSWORD_SIZE);
	cfbEncryption.ProcessData(chEncryptedPassword,(const BYTE*)buf,PASSWORD_SIZE);

	// copy the encrypted data to pDataBuffer
	// memcpy(pchPassword,chEncryptedPassword,nPasswordSize);
	MultiByteToWideChar(CP_ACP, 0, (char*)chEncryptedPassword, PASSWORD_SIZE, pchPassword, PASSWORD_SIZE);
}

void DecryptPassword(WCHAR *pchPassword,int nPasswordSize)
{
	if( PASSWORD_SIZE != nPasswordSize )
	{
		TRACE(L"\nThe password size is not correct.");
	}

	unsigned char key[AES::DEFAULT_KEYLENGTH];
	
	memcpy(key,YG_PASSWORD_KEY,sizeof(key));
	
	CFB_Mode<AES>::Decryption cfbDecryption(key, AES::DEFAULT_KEYLENGTH,(const BYTE*)YG_INITIAL_VECTOR); 

	unsigned char chPlainPassword[PASSWORD_SIZE];
	memset(chPlainPassword,0,sizeof(chPlainPassword));

	char buf[PASSWORD_SIZE];
	WideCharToMultiByte(CP_ACP, 0, pchPassword, PASSWORD_SIZE, buf, PASSWORD_SIZE, 0, 0);
	cfbDecryption.ProcessData(chPlainPassword,(const BYTE*)buf,PASSWORD_SIZE);
	
	// copy the encrypted data to pDataBuffer
	// memcpy(pchPassword,chPlainPassword,nPasswordSize);
	MultiByteToWideChar(CP_ACP, 0, (char*)chPlainPassword, PASSWORD_SIZE, pchPassword, PASSWORD_SIZE);
}


BOOL YGEncryptData(WCHAR *pchPassword,unsigned char *pDataBuffer, DWORD dwDataSize)
{
	unsigned char *pEncryptBuffer;
	
	// a middle buffer
	pEncryptBuffer = (unsigned char *)malloc(AM_COMPRESS_BUFFER);
	
	if( NULL == pEncryptBuffer)
	{
	   TRACE(L"\nmalloc error in BackupFile.");
	   return FALSE;
	}
	
	if(dwDataSize>AM_COMPRESS_BUFFER)
	{
	   TRACE(L"\ndwDataSize can not be larger than AM_COMPRESS_BUFFER.");
	   return FALSE;
	}
	
	unsigned char key[AES::DEFAULT_KEYLENGTH];
	
	// then the password should not more than 16 characters
	// memcpy(key,pchPassword,sizeof(key));
	ZeroMemory(key, AES::DEFAULT_KEYLENGTH);
	WideCharToMultiByte(CP_ACP, 0, pchPassword, AES::DEFAULT_KEYLENGTH, (char*)key, AES::DEFAULT_KEYLENGTH, 0, 0);
	CFB_Mode<AES>::Encryption cfbEncryption(key, AES::DEFAULT_KEYLENGTH,(const BYTE*)YG_INITIAL_VECTOR); 
	cfbEncryption.ProcessData(pEncryptBuffer,pDataBuffer,dwDataSize);
	
	// copy the encrypted data to pDataBuffer
	memcpy(pDataBuffer,pEncryptBuffer,dwDataSize);
	free(pEncryptBuffer);

	return TRUE;
}

BOOL YGDecryptData(WCHAR *pchPassword,unsigned char *pDataBuffer, DWORD dwDataSize)
{
	unsigned char *pDecryptBuffer;
	
	// a middle buffer
	pDecryptBuffer = (unsigned char *)malloc(AM_COMPRESS_BUFFER);
	
	if( NULL == pDecryptBuffer)
	{
	   TRACE(L"\nmalloc error in BackupFile.");
	   return FALSE;
	}
	
	if(dwDataSize>AM_COMPRESS_BUFFER)
	{
	   TRACE(L"\ndwDataSize can not be larger than AM_COMPRESS_BUFFER.");
	   return FALSE;
	}
	
	unsigned char key[AES::DEFAULT_KEYLENGTH];
	
	// then the password should not more than 16 characters
	//memcpy(key,pchPassword,sizeof(key));
	ZeroMemory(key, AES::DEFAULT_KEYLENGTH);
	WideCharToMultiByte(CP_ACP, 0, pchPassword, AES::DEFAULT_KEYLENGTH, (LPSTR)key, AES::DEFAULT_KEYLENGTH, 0, 0);
	CFB_Mode<AES>::Decryption cfbDecryption(key, AES::DEFAULT_KEYLENGTH,(const BYTE*)YG_INITIAL_VECTOR); 
	cfbDecryption.ProcessData(pDecryptBuffer,pDataBuffer,dwDataSize);
	
	// copy the encrypted data to pDataBuffer
	memcpy(pDataBuffer,pDecryptBuffer,dwDataSize);
	free(pDecryptBuffer);

	return TRUE;
}

// 2004.05.20 encrypt function end 


// 2004.05.20 added begin

BOOL GetImageHead(const WCHAR *szLastImage,IMAGE_HEAD *pImageHead)
{
	memset(pImageHead,0,sizeof(IMAGE_HEAD));

	HANDLE hLastImage;
    hLastImage = CreateFile(szLastImage,
							GENERIC_READ,
							FILE_SHARE_READ,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL);

	if(INVALID_HANDLE_VALUE == hLastImage )
	{
		TRACE(L"Can not open Last Image file in GetImageHead.\n");
		return FALSE;
	}

	DWORD dwReadBytes;
    if( FALSE == ReadFile(hLastImage,pImageHead,sizeof(IMAGE_HEAD),&dwReadBytes,NULL) )
	{
		TRACE(L"\nReadFile error in GetImageHead.");
		CloseHandle(hLastImage);
		return FALSE;
	}

	if( wcscmp(pImageHead->szImageIdentity,IMAGE_IDENTITY) != 0 )
	{
		TRACE(L"\nNot USBBackup Image in GetImageHead.");
	    CloseHandle(hLastImage);
		return FALSE;
	}

	if(pImageHead->dwFinishSignature != IMAGE_STATUS_COMPLETE)
	{
		TRACE(L"\nImageStatusNotComplete in GetImageHead.");
	    CloseHandle(hLastImage);
		return FALSE;
	}

	CloseHandle(hLastImage);
	return TRUE;
}

// 2004.05.20 added end

// 2004.05.21 added begin

BOOL CheckImagePassword(LPCTSTR szImageFile)
{
	IMAGE_HEAD ImageHead;
	if( FALSE == GetImageHead(szImageFile,&ImageHead) )
	{
		TRACE(L"\nGetImageHead error in CheckImagePassword.");
		return FALSE;
	}

	if(ImageHead.bEncrypt)
	{
		CInputPasswordDlg InputPasswordDlg;
		
		InputPasswordDlg.SetPasswordValue(ImageHead.pchPassword);
		InputPasswordDlg.DoModal();
		if( TRUE == InputPasswordDlg.TestPasswordResult() )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return TRUE;
	}
}

BOOL WriteLog(WCHAR *szFile,ULONG ulRetryMinutes)
{
	char szFileA[MAX_PATH];
	ZeroMemory(szFileA, MAX_PATH);
	WideCharToMultiByte(CP_ACP, 0, szFile, wcslen(szFile), szFileA, MAX_PATH, 0, 0);
	CStringA strText,strRetry;
	__int64 lFilePointer;
	CTime   CurTime;
	char    szUserName[MAX_PATH],szLogFileA[MAX_PATH];
	ZeroMemory(szUserName, MAX_PATH);
	ZeroMemory(szLogFileA, MAX_PATH);
	WCHAR szLogFile[MAX_PATH];
	ZeroMemory(szLogFile, MAX_PATH * sizeof(WCHAR));
	HANDLE  hFile;
	DWORD    dwEnter = 0x0a0d;
	ULONG	ulLen = MAX_PATH,dwWrtited;
	
	CurTime = CTime::GetCurrentTime();
	GetUserNameA(szUserName,&ulLen);

	if (ulRetryMinutes)
		strRetry.Format(IDS_RETRY_TIME,ulRetryMinutes);
	else strRetry = "";
	strText.Format(IDS_LOG_FORMAT,szFileA,szUserName,
					CurTime.GetYear(),CurTime.GetMonth(),CurTime.GetDay(),
					CurTime.GetHour(),CurTime.GetMinute(),CurTime.GetSecond(),
					strRetry);

	if( FALSE == GetApplicationDataPath(szLogFile) )
	{
		return FALSE;
	}

	wcscat(szLogFile,L"\\BackupMaster");
	CreateDirectory(szLogFile,NULL);

	if (GetFileAttributes(szLogFile) == -1) 
	{
		return FALSE;
	}

	wcscat(szLogFile,L"\\BM6.log");
	//GetSystemDirectory(szLogFile,MAX_PATH);
	//wcscat(szLogFile,L"\\BM6.log");
	WideCharToMultiByte(CP_ACP, 0, szLogFile, wcslen(szLogFile), szLogFileA, MAX_PATH, 0, 0);
	hFile = CreateFileA(szLogFileA,GENERIC_READ|GENERIC_WRITE,
					 FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
					 OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,
					 NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	YGSetFilePointer(hFile,0,FILE_END,lFilePointer);
	WriteFile(hFile,strText.GetBuffer(0),strText.GetLength(),&dwWrtited,NULL);
	WriteFile(hFile,"\r\n",strlen("\r\n"),&dwWrtited,NULL);
	CloseHandle(hFile);
	return TRUE;
}

BOOL RecordFailRetrySchedule(WCHAR *szLastImage,DWORD dwTimeStamp)
{
	__int64 lFilePointer;
	WCHAR szScheduleFile[MAX_PATH];

	if (!g_FailRetrySchedule.GetSize()) return TRUE;
	if( FALSE == GetFailRetryScheduleFileName(szScheduleFile) )
	{
		TRACE(L"\nGetSettingFileName error in RecordSetting.");
		return FALSE;
	}

	if (!IsFailRetryScheduleFile(szScheduleFile))
	{
		if( FALSE == RecordFailRetryScheduleHead((LPCTSTR)szScheduleFile) )
		{
			return FALSE;
		}
	}

	HANDLE hScheduleFile;
	hScheduleFile = CreateFile( 
							   (LPCTSTR)szScheduleFile,
							   GENERIC_READ|GENERIC_WRITE,
							   FILE_SHARE_READ | FILE_SHARE_WRITE,
							   NULL,
							   OPEN_EXISTING,
							   FILE_ATTRIBUTE_NORMAL,
							   NULL
							);

	if( INVALID_HANDLE_VALUE == hScheduleFile )
	{
		TRACE(L"\nOpen setting file error in RecordSetting.");
		return FALSE;
	}

	DWORD dwReadBytes;
	FAIL_RETRY_SCHEDULE_FILE_HEAD ScheduleFileHead;
		
	if( FALSE == ReadFile(hScheduleFile,&ScheduleFileHead,sizeof(ScheduleFileHead),&dwReadBytes,NULL) )
	{
		TRACE(L"\nRead Setting file head in ReadSetting.");
		CloseHandle(hScheduleFile);
		return FALSE;
	}

	DWORD dwWrittenBytes;

	for (int nIndex = 0;nIndex < g_FailRetrySchedule.GetSize();nIndex++)
	{
		wcsncpy(g_FailRetrySchedule[nIndex].szImage,szLastImage,MAX_PATH);
		g_FailRetrySchedule[nIndex].dwTimeStamp = dwTimeStamp;
		YGSetFilePointer(hScheduleFile,ScheduleFileHead.dwScheduleCount*sizeof(FAILRETRYSCHEDULE) + sizeof(FAIL_RETRY_SCHEDULE_FILE_HEAD),FILE_BEGIN,lFilePointer);
		if(WriteFile(hScheduleFile,&g_FailRetrySchedule.GetAt(nIndex),sizeof(FAILRETRYSCHEDULE),&dwWrittenBytes,NULL) )
			ScheduleFileHead.dwScheduleCount ++;
	}

	YGSetFilePointer(hScheduleFile,0,FILE_BEGIN,lFilePointer);
	WriteFile(hScheduleFile,&ScheduleFileHead,sizeof(ScheduleFileHead),&dwWrittenBytes,NULL);
	CloseHandle(hScheduleFile);
	if (g_FailRetrySchedule.GetSize()) g_FailRetrySchedule.RemoveAll();
	return TRUE;
}

BOOL FreeFailRetrySchedule(CFAILRETRYSCHEDULE &ScheduleItemArray)
{
	if (ScheduleItemArray.GetSize()) ScheduleItemArray.RemoveAll();

	return TRUE;
}
BOOL ReadFailRetrySchedule(CFAILRETRYSCHEDULE &ScheduleItemArray)
{
	WCHAR szScheduleFile[MAX_PATH];

	if (ScheduleItemArray.GetSize()) ScheduleItemArray.RemoveAll();

	if( FALSE == GetFailRetryScheduleFileName(szScheduleFile) )
	{
		TRACE(L"\nGetSettingFileName error in RecordSetting.");
		return FALSE;
	}

	if (!IsFailRetryScheduleFile(szScheduleFile))
	{
		return FALSE;
	}

	HANDLE hScheduleFile;
	hScheduleFile = CreateFile( 
		                       (LPCTSTR)szScheduleFile,
							   GENERIC_READ,
							   FILE_SHARE_READ,
							   NULL,
							   OPEN_EXISTING,
							   FILE_ATTRIBUTE_NORMAL,
							   NULL
							  );

	if( INVALID_HANDLE_VALUE == hScheduleFile )
	{
		TRACE(L"\nOpen setting file error in ReadSetting.");
		return FALSE;
	}

	DWORD dwReadBytes;

	FAIL_RETRY_SCHEDULE_FILE_HEAD ScheduleFileHead;
	
	if( FALSE == ReadFile(hScheduleFile,&ScheduleFileHead,sizeof(ScheduleFileHead),&dwReadBytes,NULL) )
	{
		TRACE(L"\nRead Setting file head in ReadSetting.");
		CloseHandle(hScheduleFile);
		return FALSE;
	}

	FAILRETRYSCHEDULE ScheduleItem;

	DWORD dwItemIndex;
	for(dwItemIndex=0; dwItemIndex<ScheduleFileHead.dwScheduleCount; dwItemIndex++)
	{
		if( FALSE == ReadFile(hScheduleFile,&ScheduleItem,sizeof(ScheduleItem),&dwReadBytes,NULL))
		{
			TRACE(L"\nRead ScheduleItem error in ReadSetting.");
			CloseHandle(hScheduleFile);
			return FALSE;
		}
		ScheduleItemArray.Add(ScheduleItem);
	}

	CloseHandle(hScheduleFile);
	return TRUE;
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


BOOL GetFailRetryScheduleFileName(WCHAR *szScheduleFile)
{
	if( FALSE == GetApplicationDataPath(szScheduleFile) )
	{
		TRACE(L"\nGet Setting Dir error in GetSettingFileName.");
		return FALSE;
	}
	
	wcscat(szScheduleFile,L"\\BackupMaster");
	CreateDirectory(szScheduleFile,NULL);
	
	if (GetFileAttributes(szScheduleFile) == -1) 
	{
		TRACE(L"\nGet Setting Dir error in GetSettingFileName.");
		return FALSE;
	}

	wcscat(szScheduleFile,L"\\");
	wcscat(szScheduleFile,BM_FAIL_RETRY_SCHEDULE_FILE);

	return TRUE;
}

BOOL IsFailRetryScheduleFile(LPCTSTR szScheduleFileName)
{
	DWORD dwAttribute;

	dwAttribute = GetFileAttributes( LPCTSTR(szScheduleFileName ) );

	if( -1 == dwAttribute  || dwAttribute & FILE_ATTRIBUTE_DIRECTORY)
	{
		return FALSE;
	}

	HANDLE hScheduleFile;
	hScheduleFile = CreateFile( szScheduleFileName,GENERIC_READ,FILE_SHARE_READ,NULL,
		                       OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if( INVALID_HANDLE_VALUE == hScheduleFile )
	{
		TRACE(L"\nCreateFile error in IsUSBBackupSettingFile.");
		return FALSE;
	}

	FAIL_RETRY_SCHEDULE_FILE_HEAD ScheduleFileHead;
	DWORD dwReadBytes;
	if( FALSE == ReadFile(hScheduleFile,&ScheduleFileHead,sizeof(ScheduleFileHead),&dwReadBytes,NULL) )
	{
		TRACE(L"\nReadFile error in IsFileBackupSettingFile.");
		CloseHandle(hScheduleFile);
		return FALSE;
	}

	if( 0 != wcscmp(ScheduleFileHead.szIdentity,BM_FAIL_RETRY_SCHEDULE_IDENTITY) )
	{
		TRACE(L"\nThis is not an FileBackup setting file.");
		CloseHandle(hScheduleFile);
		return FALSE;
	}
	else
	{
		TRACE(L"\nThis is an File Schedule setting file.");
	}

	CloseHandle(hScheduleFile);

	return TRUE;
}

BOOL RecordFailRetryScheduleHead(LPCTSTR szScheduleFile)
{
	HANDLE hScheduleFile;
	hScheduleFile = CreateFile(szScheduleFile,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,
		                       NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

	if( INVALID_HANDLE_VALUE == hScheduleFile )
	{
		TRACE(L"\nCreateFile error in RecordFailRetryScheduleHead.");
		return FALSE;
	}

	FAIL_RETRY_SCHEDULE_FILE_HEAD ScheduleFileHead;
	memset(&ScheduleFileHead,0,sizeof(FAIL_RETRY_SCHEDULE_FILE_HEAD));

	ScheduleFileHead.ulFileVer = 0x100;
	ScheduleFileHead.ulStructSize = sizeof(FAIL_RETRY_SCHEDULE_FILE_HEAD);
	ScheduleFileHead.dwScheduleCount = 0;
	wcsncpy(ScheduleFileHead.szIdentity,BM_FAIL_RETRY_SCHEDULE_IDENTITY,SETTING_FILE_IDENTITY_SIZE);

	DWORD dwWrittenBytes;
	if( FALSE == WriteFile(hScheduleFile,&ScheduleFileHead,sizeof(ScheduleFileHead),&dwWrittenBytes,NULL) )
	{
	   TRACE(L"\nWriteFile error in RecordScheduleHead.");
	   CloseHandle(hScheduleFile);
	   return FALSE;
	}
	SetEndOfFile(hScheduleFile);
	CloseHandle(hScheduleFile);
	return TRUE;
}

BOOL RemoveFailRetrySchedule(PFAILRETRYSCHEDULE pScheduleItem)
{
	__int64 lFilePointer;
	WCHAR szScheduleFile[MAX_PATH];

	if( FALSE == GetFailRetryScheduleFileName(szScheduleFile) )
	{
		TRACE(L"\nGetSettingFileName error in RecordSetting.");
		return FALSE;
	}

	if (!IsFailRetryScheduleFile(szScheduleFile))
	{
		return FALSE;
	}

	HANDLE hScheduleFile;
	hScheduleFile = CreateFile( 
		                       (LPCTSTR)szScheduleFile,
							   GENERIC_READ | GENERIC_WRITE,
							   FILE_SHARE_READ | FILE_SHARE_WRITE,
							   NULL,
							   OPEN_EXISTING,
							   FILE_ATTRIBUTE_NORMAL,
							   NULL
							  );

	if( INVALID_HANDLE_VALUE == hScheduleFile )
	{
		TRACE(L"\nOpen setting file error in ReadSetting.");
		return FALSE;
	}

	DWORD dwReadBytes;

	FAIL_RETRY_SCHEDULE_FILE_HEAD ScheduleFileHead;
	
	if( FALSE == ReadFile(hScheduleFile,&ScheduleFileHead,sizeof(ScheduleFileHead),&dwReadBytes,NULL) )
	{
		TRACE(L"\nRead Setting file head in ReadSetting.");
		CloseHandle(hScheduleFile);
		return FALSE;
	}

	PFAILRETRYSCHEDULE pSchedule;

	if (ScheduleFileHead.dwScheduleCount)
	{
		pSchedule = (PFAILRETRYSCHEDULE)malloc(ScheduleFileHead.dwScheduleCount * sizeof(FAILRETRYSCHEDULE));
		if (pSchedule)
		{
			if(ReadFile(hScheduleFile,pSchedule,ScheduleFileHead.dwScheduleCount * sizeof(FAILRETRYSCHEDULE),&dwReadBytes,NULL))
			{
				DWORD dwItemIndex;
				for(dwItemIndex=0; dwItemIndex<ScheduleFileHead.dwScheduleCount; dwItemIndex++)
				{
					if (!wcsicmp(pSchedule[dwItemIndex].szImage,pScheduleItem->szImage) &&
						!wcsicmp(pSchedule[dwItemIndex].szFailFile,pScheduleItem->szFailFile) &&
						!memcmp(&pSchedule[dwItemIndex].FailTime,&pScheduleItem->FailTime,sizeof(SYSTEMTIME)))
					{
						ScheduleFileHead.dwScheduleCount --;
						YGSetFilePointer(hScheduleFile,0,FILE_BEGIN,lFilePointer);
						WriteFile(hScheduleFile,&ScheduleFileHead,sizeof(ScheduleFileHead),&dwReadBytes,NULL);
						if (ScheduleFileHead.dwScheduleCount)
						{
							memcpy(&pSchedule[dwItemIndex],&pSchedule[dwItemIndex+1],(ScheduleFileHead.dwScheduleCount-dwItemIndex)*sizeof(FAILRETRYSCHEDULE));
							WriteFile(hScheduleFile,pSchedule,ScheduleFileHead.dwScheduleCount * sizeof(FAILRETRYSCHEDULE),&dwReadBytes,NULL);
						}
						SetEndOfFile(hScheduleFile);
						break;
					}
				}
			}
			free(pSchedule);
		}
	}
	CloseHandle(hScheduleFile);
	return TRUE;
}

BOOL ModifyFailRetrySchedule(PFAILRETRYSCHEDULE pScheduleItem)
{
	__int64 lFilePointer;
	WCHAR szScheduleFile[MAX_PATH];
	if( FALSE == GetFailRetryScheduleFileName(szScheduleFile) )
	{
		TRACE(L"\nGetSettingFileName error in RecordSetting.");
		return FALSE;
	}

	if (!IsFailRetryScheduleFile(szScheduleFile))
	{
		return FALSE;
	}

	HANDLE hScheduleFile;
	hScheduleFile = CreateFile( 
		                       (LPCTSTR)szScheduleFile,
							   GENERIC_READ | GENERIC_WRITE,
							   FILE_SHARE_READ | FILE_SHARE_WRITE,
							   NULL,
							   OPEN_EXISTING,
							   FILE_ATTRIBUTE_NORMAL,
							   NULL
							  );

	if( INVALID_HANDLE_VALUE == hScheduleFile )
	{
		TRACE(L"\nOpen setting file error in ReadSetting.");
		return FALSE;
	}

	DWORD dwReadBytes;

	FAIL_RETRY_SCHEDULE_FILE_HEAD ScheduleFileHead;
	
	if( FALSE == ReadFile(hScheduleFile,&ScheduleFileHead,sizeof(ScheduleFileHead),&dwReadBytes,NULL) )
	{
		TRACE(L"\nRead Setting file head in ReadSetting.");
		CloseHandle(hScheduleFile);
		return FALSE;
	}

	PFAILRETRYSCHEDULE pSchedule;

	if (ScheduleFileHead.dwScheduleCount)
	{
		pSchedule = (PFAILRETRYSCHEDULE)malloc(ScheduleFileHead.dwScheduleCount * sizeof(FAILRETRYSCHEDULE));
		if (pSchedule)
		{
			if(ReadFile(hScheduleFile,pSchedule,ScheduleFileHead.dwScheduleCount * sizeof(FAILRETRYSCHEDULE),&dwReadBytes,NULL))
			{
				DWORD dwItemIndex;
				for(dwItemIndex=0; dwItemIndex<ScheduleFileHead.dwScheduleCount; dwItemIndex++)
				{
					if (!wcsicmp(pSchedule[dwItemIndex].szImage,pScheduleItem->szImage) &&
						!wcsicmp(pSchedule[dwItemIndex].szFailFile,pScheduleItem->szFailFile) &&
						!memcmp(&pSchedule[dwItemIndex].FailTime,&pScheduleItem->FailTime,sizeof(SYSTEMTIME)))
					{
						memcpy(&pSchedule[dwItemIndex],pScheduleItem,sizeof(FAILRETRYSCHEDULE));
						YGSetFilePointer(hScheduleFile,0,FILE_BEGIN,lFilePointer);
						WriteFile(hScheduleFile,&ScheduleFileHead,sizeof(ScheduleFileHead),&dwReadBytes,NULL);
						WriteFile(hScheduleFile,pSchedule,ScheduleFileHead.dwScheduleCount * sizeof(FAILRETRYSCHEDULE),&dwReadBytes,NULL);
						SetEndOfFile(hScheduleFile);
						break;
					}
				}
			}
			free(pSchedule);
		}
	}
	CloseHandle(hScheduleFile);
	return TRUE;
}

BOOL ExcuteFailRetrySchedule(PFAILRETRYSCHEDULE pScheduleItem)
{
	IMAGE_HEAD	   ImageHead;
	AM_FILE_HANDLE FileHandle = {0};
	DWORD          dwErrorCode;
	CDATA_ARRAY		TargetArray;

	GetLocalTime(&pScheduleItem->LastTryTime);
	pScheduleItem->dwTryTimes ++;

	WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile((LPCTSTR)pScheduleItem->szFailFile,
		                            &findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		RemoveFailRetrySchedule(pScheduleItem);
		return FALSE;
	}
	FindClose(hFind);

	HANDLE hFileSource;
	hFileSource=CreateFile( pScheduleItem->szFailFile,
                            GENERIC_READ,
                            FILE_SHARE_READ,NULL,OPEN_EXISTING,
     					    FILE_ATTRIBUTE_NORMAL,0 );
	if (hFileSource == INVALID_HANDLE_VALUE)
	{
		ModifyFailRetrySchedule(pScheduleItem);
		return FALSE;
	}

	if(!CheckLastImage(pScheduleItem->szImage,&dwErrorCode))
	{
		RemoveFailRetrySchedule(pScheduleItem);
		CloseHandle(hFileSource);
		return FALSE;
	}

	if(!GetImageArray(TargetArray,pScheduleItem->szImage))
	{
		RemoveFailRetrySchedule(pScheduleItem);
		CloseHandle(hFileSource);
		return FALSE;
	}

	if(!GetImageHead(pScheduleItem->szImage,&ImageHead))
	{
		ModifyFailRetrySchedule(pScheduleItem);
		CloseHandle(hFileSource);
		return FALSE;
	}

	if (SeparateFileFromOneImg(pScheduleItem->szImage))
	{
		if (!OpenFileHandle(&FileHandle,pScheduleItem->szImage,&dwErrorCode))
		{
			RemoveFailRetrySchedule(pScheduleItem);
			CloseHandle(hFileSource);
			return FALSE;
		}
	
		RemoveFailRetrySchedule(pScheduleItem);
		LARGE_INTEGER DataFilePointer;
		BACKUPOPTION BackupOption;
		BackupOption.bAutoSpawn = TRUE;
		BackupOption.bCmdLine = FALSE;
		BackupOption.bLogFail = FALSE;
		BackupOption.bRetryFail = FALSE;
		BackupOption.bEncrypt = ImageHead.bEncrypt;
		wcsncpy(BackupOption.pchPassword,ImageHead.pchPassword,PASSWORD_SIZE);

		YGSetFilePointer(FileHandle.hCurrentTarget,0,FILE_END,DataFilePointer.QuadPart);
		BackupFileData(hFileSource,FileHandle.hCurrentTarget,&DataFilePointer,
						pScheduleItem->szFailFile,
						&ImageHead,
						TargetArray,
						&dwErrorCode,
						&BackupOption);

		CloseHandle(hFileSource);

		UpdateIndexAndArrayFile(&FileHandle,pScheduleItem->szFailFile,&findFileData,&DataFilePointer,pScheduleItem->dwTimeStamp);
		CombineFileToOneImg(&FileHandle,TargetArray);
		CloseFileHandle(&FileHandle);
		RecordFileTable(TargetArray);
		//RecordLastImageInfo(&FileHandle,TargetArray);
		return TRUE;
	}
	CloseHandle(hFileSource);
	return FALSE;
}

BOOL SeparateFileFromOneImg(WCHAR *szLastImage)
{
	CString strTempDir;
	CString strImageFilePath;
	CString strTargetDirectory,strTargetFileName;
 
	GetTempDir(strTempDir.GetBuffer(MAX_PATH));
	strTempDir.ReleaseBuffer();

	strImageFilePath=szLastImage;
  
	int nIndex=strImageFilePath.ReverseFind(L'\\');
	strTargetDirectory=strImageFilePath.Left(nIndex);
	strTargetFileName=strImageFilePath.Mid(nIndex+1);
	nIndex=strTargetFileName.ReverseFind(L'.');
	strTargetFileName=strTargetFileName.Left(nIndex);
 
	GetMoreSeperateFile(
						  (LPCTSTR)strImageFilePath,
						  (LPCTSTR)strTempDir,
						  (LPCTSTR)strTargetFileName,
						  TRUE
				     );
	return TRUE;
}

BOOL UpdateIndexAndArrayFile(AM_FILE_HANDLE *pHandle,WCHAR *szFile,WIN32_FIND_DATA *pFindData,LARGE_INTEGER *pFilePointer,DWORD dwTimeStamp)
{
	BOOL bResult = FALSE;
	__int64    lFilePointer;
	INDEX_DATA IndexData;
    DWORD dwTotalIndexOffset;
	TOTAL_INDEX_NODE TotalIndexNode;
	DWORD dwReadBytes;
	ARRAY_DATA ArrayData[100];
	DWORD      dwCurrent,i;

	ReadFile(pHandle->hTotalIndexFile,&TotalIndexNode,sizeof(TOTAL_INDEX_NODE),&dwReadBytes,NULL);
	YGSetFilePointer(pHandle->hArrayFile,sizeof(ARRAY_NODE),FILE_BEGIN,lFilePointer);

	for(DWORD dwCurrentPathID=1; dwCurrentPathID<=TotalIndexNode.dwTotalPathCount && !bResult; dwCurrentPathID+=dwCurrent)
	{
		if (dwCurrentPathID + 100 > TotalIndexNode.dwTotalPathCount) 
			dwCurrent = TotalIndexNode.dwTotalPathCount - dwCurrentPathID + 1;
		else dwCurrent = 100;
		ReadFile(pHandle->hArrayFile,&ArrayData[0],sizeof(ARRAY_DATA)*dwCurrent,&dwReadBytes,NULL);
		for (i = 0;i < dwCurrent;i++)
		{
			if (!wcsicmp(ArrayData[i].szFilePath,szFile))
			{	
				dwTotalIndexOffset=GetTotalIndexOffset(TotalIndexNode.dwTotalPathCount, dwTimeStamp,ArrayData[i].dwPathID);
				YGSetFilePointer(pHandle->hTotalIndexFile,dwTotalIndexOffset,FILE_BEGIN,lFilePointer);
				ReadFile(pHandle->hTotalIndexFile,&IndexData,sizeof(IndexData),&dwReadBytes,NULL);
				IndexData.DataPointer.QuadPart=pFilePointer->QuadPart;
				IndexData.FileInfo.dwFileAttributes=pFindData->dwFileAttributes;
				IndexData.FileInfo.nFileSizeHigh = pFindData->nFileSizeHigh;
				IndexData.FileInfo.nFileSizeLow = pFindData->nFileSizeLow;
				IndexData.FileInfo.ftCreationTime  = pFindData->ftCreationTime;
				IndexData.FileInfo.ftLastAccessTime = pFindData->ftLastAccessTime;
				IndexData.FileInfo.ftLastWriteTime = pFindData->ftLastWriteTime;
				YGSetFilePointer(pHandle->hTotalIndexFile,dwTotalIndexOffset,FILE_BEGIN,lFilePointer);
				WriteFile(pHandle->hTotalIndexFile,&IndexData,sizeof(IndexData),&dwReadBytes,NULL);
				bResult = TRUE;
				break;
			}
		}
	}
  
	return bResult;
}

BOOL YGSetFilePointer(HANDLE hFile, __int64 linDistance, DWORD dwMoveMethod, __int64 &linTargetFilePointer)
{
   LARGE_INTEGER liFilePointer;

   liFilePointer.QuadPart = linDistance;

   liFilePointer.LowPart = SetFilePointer (hFile, liFilePointer.LowPart, &liFilePointer.HighPart, dwMoveMethod);

   if (liFilePointer.LowPart == -1 && GetLastError() != NO_ERROR)
   {
	  linTargetFilePointer = -1;
	  return FALSE;
   }
   else
   {
	   linTargetFilePointer = liFilePointer.QuadPart;
	   return TRUE;
   }
}

WCHAR * strrchrcut(WCHAR szSource[MAX_PATH])
{
	int nIndex=0;
	int count=0;

//	char szTarget[MAX_PATH];
//	memset(szTarget,0,MAX_PATH);

	for(int i=0;i<MAX_PATH;i++)
	{
		if(szSource[i]==L'\\')
			count++;
		if(count==3)
		{
			nIndex=i;
			szSource[nIndex]=L'\0';
			break;
		}	
		if(szSource[i]==L'\0')
			break;
	}
//	szTarget[nIndex]=szSource[nIndex];
	return szSource;

}

// temp
BOOL bMultiFile(LPCTSTR szLastImage)
{
	return FALSE;
}
BOOL GetOneFile ( LPCTSTR		    szLastImage,
						LPCTSTR			szBufferDir,
						unsigned __int64	qwMaxFileSize,
						DWORD			dwTargetNumber,
						TABLE_DATA		*pTargetData
						)
{
	return FALSE;
}
BOOL NewRecordTail (  LPCTSTR           szLastImage,
						   LPCTSTR           szBufferDir,
						   CDATA_ARRAY       &TargetArray,
						   unsigned __int64  qwMaxFileSize ,
						   BOOL	           *pbNewFile,
						   WCHAR              *szNewTarget
						   )
{
	return FALSE;
}
BOOL GetLastTargetNumber( LPCTSTR szLastImage,
								unsigned __int64 qwMaxFileSize,
								DWORD *pdwLastTarget)
{
	return FALSE;
}

BOOL RecordLastImageInfo( AM_FILE_HANDLE *pFileHandle,CDATA_ARRAY &TargetArray )
{
	CString strLastFileName;
	strLastFileName = TargetArray.GetAt(TargetArray.GetUpperBound()).szFileName;
	int nIndex=strLastFileName.ReverseFind(L'\\');
	strLastFileName=strLastFileName.Mid(nIndex+1);

	for (int i = 1; i < TargetArray.GetSize(); ++i)
	{
		CString ImageFilePath = TargetArray[i].szFileName;
		HANDLE hImageFile;
		hImageFile=CreateFile((LPCTSTR)ImageFilePath,
			GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,
			OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

		if(hImageFile==INVALID_HANDLE_VALUE)
		{
			continue;
		}

		IMAGE_HEAD ImageHead;
		DWORD dwReadBytes;
		DWORD dwWrittenBytes;
		ReadFile(hImageFile,&ImageHead,sizeof(ImageHead),&dwReadBytes,NULL);
		wcscpy(ImageHead.szLastFileName, strLastFileName);
		__int64 lFilePointer;
		YGSetFilePointer(hImageFile,0,FILE_BEGIN,lFilePointer);
		WriteFile(hImageFile,&ImageHead,
			sizeof(ImageHead),&dwWrittenBytes,NULL);
		CloseHandle(hImageFile);
	}

	return TRUE;
}

