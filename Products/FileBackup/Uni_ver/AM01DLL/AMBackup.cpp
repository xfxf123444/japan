#include "AMBackup.h"
#include "resource.h"
#include "AMFunction.h"
#include <iostream>
#include "..\..\..\davinci\Davinci_tech\zlib\Cur_ver\Export\zlib.h"

using namespace std;
AM01WORKSTATE g_WorkState;
BOOL g_bCancel;
BOOL g_bUserCancel;
unsigned __int64 g_TotalWrittenBytes;
unsigned __int64 g_TotalReadBytes;
unsigned __int64 g_TotalFileSize;
CFAILRETRYSCHEDULE g_FailRetrySchedule;
DWORD g_TotalFileCount;
DWORD g_CurrentFileCount;
WORD g_ProgressPosition;
char g_szPath[MAX_PATH];
SEGMENT_INFO g_SegmentInfo;

BOOL BackupSelectionFile(SETTING_DATA_INFO SettingDataInfo,CStringArray &SelectionArray)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// 2004.05.20 added begin
	CString strText;
	WCHAR ImageFilePath[MAX_PATH];
	memset(ImageFilePath,0,sizeof(ImageFilePath));
	wcsncpy(ImageFilePath,SettingDataInfo.szTargetImage,MAX_PATH-1);

	WORD wBackupType;
	wBackupType = SettingDataInfo.wBackupType;

	WCHAR ImageComment[IMAGE_COMMENT_SIZE];
	memset(ImageComment,0,sizeof(ImageComment));
	wcsncpy(ImageComment,SettingDataInfo.szComment,IMAGE_COMMENT_SIZE-1);
	// 2005.05.20 added end

	// to init the parogress bar.
	g_WorkState.wPercent=0;
	wcscpy(g_WorkState.szCurrentFile,L"\0");

	// to init the Cancel Valible.
	g_bCancel=FALSE;
	g_bUserCancel=FALSE;

	if(GetFileAttributes(ImageFilePath)!=-1 )
	{
		DiffBackupSelectionFile(ImageFilePath,&SettingDataInfo.BackupOption);
	}
	else
	{
		if(CheckSelectionArray(SelectionArray)==FALSE)
		{
			strText.LoadString(IDS_EMPTY_SOURCE);
			if (SettingDataInfo.BackupOption.bCmdLine)
				wcout << (LPCTSTR)strText << endl;
			else AfxMessageBox(IDS_EMPTY_SOURCE,MB_OK|MB_ICONINFORMATION,NULL);
			return FALSE;
		}
  
		g_TotalWrittenBytes=0;

		// AM01 Version 2
		g_TotalReadBytes = 0;

		g_TotalFileSize=0;
    
		g_ProgressPosition=0;

		g_CurrentFileCount=0;
		g_TotalFileCount=0;
		memset(&g_WorkState,0,sizeof(AM01WORKSTATE));

		//-- for progress bar end
	
		GetTotalFileSize(SelectionArray);
    
		CDATA_ARRAY TargetArray;
	
		AddOneTargetElement(TargetArray,ImageFilePath);

		if( BackupSelection( SelectionArray,
			                 SettingDataInfo,
					         TargetArray
					       ) == FALSE )
		{
			//-- I think here we should delete the incomplete file.
			ClearSignature(TargetArray.GetAt(TargetArray.GetUpperBound()).szFileName ) ;
			int nTotalFileNumber;
			nTotalFileNumber = TargetArray.GetUpperBound();
			int nFileNumber;

			CString strDeleteFile;

			for(nFileNumber = 1; nFileNumber <=nTotalFileNumber; nFileNumber ++)
			{
				strDeleteFile = TargetArray.GetAt(nFileNumber).szFileName;

				// to avoid delete the wrong file

				if( (strDeleteFile.Right(4)).CompareNoCase(AM_FILE_SUFFIX) == 0 )
				{
					DeleteFile( (LPCTSTR)strDeleteFile );
				}

			}

			return FALSE;
		}

		if(g_bCancel != TRUE && g_bUserCancel != TRUE)
		{
			WriteSignature( TargetArray.GetAt(TargetArray.GetUpperBound()).szFileName, TargetArray );
		}

	}
	return TRUE;
}

BOOL BackupSelection( 
					  CStringArray &SelectionArray,
					  SETTING_DATA_INFO SettingDataInfo,
					  CDATA_ARRAY &TargetArray
					)
{
	__int64 lFilePointer;
	CString strText;

	if(SelectionArray.GetSize()==0)
	{
		return FALSE;
	}
	IMAGE_HEAD ImageHead;
	DWORD dwPathID;
	dwPathID=0;
  
	BOOL bGetHandleResult;
	DWORD dwErrorCode;

	if (g_FailRetrySchedule.GetSize()) g_FailRetrySchedule.RemoveAll();
	AM_FILE_HANDLE FileHandle;
	FileHandle.hArrayFile=NULL;
	FileHandle.hCurrentIndexFile=NULL;
	FileHandle.hCurrentTarget=NULL;
	FileHandle.hStampFile=NULL;
	// FileHandle.hTableFile=NULL;
	FileHandle.hTotalIndexFile=NULL;

	bGetHandleResult=GetFileHandle(&FileHandle,SettingDataInfo.szTargetImage,&dwErrorCode);

	if(bGetHandleResult==FALSE)
	{
		strText.LoadString(IDS_CREATE_IMAGE_ERROR);
		if (SettingDataInfo.BackupOption.bCmdLine)
			wcout << (LPCTSTR)strText << endl;
		else AfxMessageBox(IDS_CREATE_IMAGE_ERROR,MB_OK|MB_ICONINFORMATION,NULL);
		return FALSE;
	}

	WriteFileHead(&FileHandle,SettingDataInfo,TargetArray);
  
	DWORD dwReadBytes;
	YGSetFilePointer(FileHandle.hCurrentTarget,0,FILE_BEGIN,lFilePointer);
	ReadFile(FileHandle.hCurrentTarget,&ImageHead,sizeof(ImageHead),&dwReadBytes,NULL);
  
	YGSetFilePointer(FileHandle.hCurrentTarget,0,FILE_END,lFilePointer);

	// record the time that begin backup.
	RecordStampInfo(FileHandle.hStampFile);

	//-- for debug temp comment

	YGSetFilePointer(FileHandle.hCurrentTarget,0,FILE_END,lFilePointer);

	RecordSelection(FileHandle.hCurrentTarget,SelectionArray);

	YGSetFilePointer(FileHandle.hCurrentTarget,0,FILE_END,lFilePointer);

	StoreExtraDirInfo(SelectionArray,&FileHandle,&dwPathID);

	CString OnePathString;

	for(long ArrayIndex=0;
           ArrayIndex < SelectionArray.GetSize();
  		   ArrayIndex++)
	{

	    // in the future we will add some function
		// of close handle, delete file etc.

	    if(g_bCancel==TRUE || g_bUserCancel == TRUE)
		{
			break;
		}

        OnePathString=SelectionArray.ElementAt(ArrayIndex);
		
		if(OnePathString.GetLength()>=2)
		{ 
		   if(IsOneDirectory(OnePathString)) // if it is a directory
		   {
			  if(
			      BackupDirectory((LPCTSTR)OnePathString,
				     &FileHandle, &ImageHead, TargetArray,dwPathID,&SettingDataInfo.BackupOption) == FALSE
				)
			  {
				  TRACE(L"\nBackup " + OnePathString + L" Error.");
				  g_bCancel=TRUE;
				  break;
			  }

		   }
		   else 
		   {
			  DWORD dwErrorCode;
			  if(
				  BackupFile( (LPCTSTR)OnePathString,
							  &FileHandle, // the curent target may change
							  &ImageHead,
							  TargetArray,
							  &dwPathID,
							  &dwErrorCode,
							  &SettingDataInfo.BackupOption) == FALSE
				)
			  {
				  TRACE(L"\nBackup " + OnePathString + L" Error.");
				  g_bCancel=TRUE;
				  break;
			  }
		   }
		}
		else
		{
			TRACE(L"\nThe souce path is too short, it needed at least 2 characters.");
			g_bCancel=TRUE;
			break;
		}

	}
	WCHAR szLongMainName[MAX_PATH];
	if(FALSE == GetLongMainName(SettingDataInfo.szTargetImage,szLongMainName))
	{
		TRACE(L"\nGetLongMainName error in BackupSelection.");
	}

	if(g_bCancel==TRUE || g_bUserCancel==TRUE)
	{
		TRACE(L"\ng_bCancel or g_bUserCancel is true in BackupSelection.");
		CloseFileHandle(&FileHandle);
		ClearExtraFile(szLongMainName);
		return FALSE;
	}
  
	AdjustIndexHead(FileHandle.hCurrentIndexFile,dwPathID);
	CurrentIdxToTotalIdx(&FileHandle);
	CombineFileToOneImg(&FileHandle,TargetArray);
	CloseFileHandle(&FileHandle);
	RecordFileTable(TargetArray);
	//RecordLastImageInfo(&FileHandle,TargetArray);
	ClearExtraFile(szLongMainName);
	RecordFailRetrySchedule(TargetArray.GetAt(TargetArray.GetSize()-1).szFileName,1);
	return TRUE;
}


BOOL GetMoreSeperateFile(LPCTSTR szLastImage, LPCTSTR TargetDirectory, LPCTSTR TargetFileName,BOOL bResetImage)
{
	__int64 lFilePointer;
	CString strTargetDirectory;
	CString strTargetFileName;

	strTargetDirectory=TargetDirectory;
	strTargetFileName=TargetFileName;
		
	  // open data file
	HANDLE hDataFile;
	hDataFile=NULL;

	  // now we should open the szLastImage in the way of read only
  
	hDataFile=CreateFile(szLastImage,
						   GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,
						   OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);


	if(hDataFile == INVALID_HANDLE_VALUE)
	{
		  // open file error, then return.
		  return FALSE;
	}

	// open TotalIndexFile
	HANDLE hTotalIndexFile;
	hTotalIndexFile=NULL;

	CString strTempDir;
	GetTempDir(strTempDir.GetBuffer(MAX_PATH));
	strTempDir.ReleaseBuffer();
  
	CString strTotalIndexFilePath;
	strTotalIndexFilePath=strTempDir + L"\\" + strTargetFileName + AM_INDEX_SUFFIX;
	hTotalIndexFile=CreateFile((LPCTSTR)strTotalIndexFilePath,
					  GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,
					  CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

	if(hTotalIndexFile == INVALID_HANDLE_VALUE)
	{
		  // open file error, then return.
		CloseHandle(hDataFile);
		return FALSE;
	}


	// open ArrayFile
	HANDLE hArrayFile;
	hArrayFile=NULL;

	CString strArrayFilePath;
	strArrayFilePath=strTempDir + L"\\" + strTargetFileName + L".ary";
	hArrayFile=CreateFile((LPCTSTR)strArrayFilePath,
				 GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,
				 CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

	if(hArrayFile == INVALID_HANDLE_VALUE )
	{
		  // open file error, then return.
		CloseHandle(hDataFile);
		CloseHandle(hTotalIndexFile);
		return FALSE;
	}

	// open stamp file
	HANDLE hStampFile;
	hStampFile=NULL;

	CString strStampFilePath;
	strStampFilePath=strTempDir + L"\\" + strTargetFileName + L".stp";
	hStampFile=CreateFile((LPCTSTR)strStampFilePath,
							 GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,
							 CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

	if(hStampFile == INVALID_HANDLE_VALUE )
	{
		  // open file error, then return.
		CloseHandle(hDataFile);
		CloseHandle(hTotalIndexFile);
		CloseHandle(hStampFile);
		return FALSE;
	}
  

	DWORD dwReadBytes;

	IMAGE_HEAD ImageHead;
  
	ReadFile(hDataFile,&ImageHead,sizeof(ImageHead),&dwReadBytes,NULL);


	unsigned __int64 qwTotalIndexFileLength;
	qwTotalIndexFileLength=ImageHead.ArrayPointer.QuadPart - ImageHead.TotalIndexPointer.QuadPart;

	unsigned __int64 qwArrayFileLength;

	qwArrayFileLength=ImageHead.StampPointer.QuadPart-ImageHead.ArrayPointer.QuadPart;

	unsigned __int64 qwStampFileLength;

	qwStampFileLength=ImageHead.FileTablePointer.QuadPart-ImageHead.StampPointer.QuadPart;

	GetOneSeperateFile(hDataFile,hTotalIndexFile,
						 ImageHead.TotalIndexPointer,
						 qwTotalIndexFileLength);

	GetOneSeperateFile(hDataFile,hArrayFile,
						 ImageHead.ArrayPointer,
						 qwArrayFileLength);

	GetOneSeperateFile(hDataFile,hStampFile,
						 ImageHead.StampPointer,
						 qwStampFileLength);

	if (bResetImage)
	{
		YGSetFilePointer(hDataFile,ImageHead.TotalIndexPointer.QuadPart,FILE_BEGIN,lFilePointer);
		SetEndOfFile(hDataFile);
	}
	CloseHandle(hDataFile);
	CloseHandle(hTotalIndexFile);
	CloseHandle(hArrayFile);
	CloseHandle(hStampFile);

	hDataFile=NULL;
	hTotalIndexFile=NULL;
	hArrayFile=NULL;
	hStampFile=NULL;
  
	return TRUE;
}

/************************************************************
 *
 * Function: DiffBackupSelectionFile()
 * Purpose:  do differential backup based on one base image
 * Comment:  this function will call DiffBackupSelection()
 *  
 *************************************************************/

BOOL DiffBackupSelectionFile(LPCTSTR ImageFilePath,PBACKUPOPTION pBackupOption)
{
	CString strText;
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	g_WorkState.wPercent = 0;
	wcscpy(g_WorkState.szCurrentFile,L"\0");

	g_bCancel=FALSE;
	g_bUserCancel=FALSE;
	DWORD dwErrorCode;
	dwErrorCode=0;
  
	WCHAR szMainFileName[MAX_PATH];
	GetMainFileName(ImageFilePath,szMainFileName);

	// GetDirFileName
	WCHAR szDirFileName[MAX_PATH];
	wcsncpy(szDirFileName,szMainFileName,MAX_PATH-1);
	wcscat(szDirFileName,L".dir");

	WCHAR szLastImage[MAX_PATH];
  
	if( GetFileAttributes(szDirFileName) != -1 )
	{
		if( GetLastFileName(szDirFileName,szLastImage) == FALSE )
		{
			TRACE(L"\n get last file name error.");
			wcsncpy(szLastImage,ImageFilePath,MAX_PATH-1);
		}
	}
	else
	{
		wcsncpy(szLastImage,ImageFilePath,MAX_PATH-1);
	}

	if( CheckLastImage(szLastImage,&dwErrorCode) == FALSE )
	{
		if( dwErrorCode == NOT_LAST_IMAGE)
		{
			if (SearchLastImage(szLastImage, &dwErrorCode) == FALSE)
			{
				strText.LoadString(IDS_SELECT_LAST_IMAGE);
				if (pBackupOption->bCmdLine)
				{
					wcout << (LPCTSTR)strText << endl;
					return FALSE;
				}
				else AfxMessageBox(IDS_SELECT_LAST_IMAGE,MB_OK|MB_ICONINFORMATION,NULL);
				SelectLastImage(szLastImage);
			}
		}
		else
		{
			return FALSE;
		}
	}

	CStringArray SelectionArray;

	GetSelectionArrayPro(szLastImage,SelectionArray);

	if(CheckSelectionArray(SelectionArray)==FALSE)
	{
		strText.LoadString(IDS_EMPTY_SOURCE);
		if (pBackupOption->bCmdLine)
			wcout << (LPCTSTR)strText << endl;
		else AfxMessageBox(IDS_EMPTY_SOURCE,MB_OK|MB_ICONINFORMATION,NULL);
		return FALSE;
	}

	CDATA_ARRAY TargetArray;
	if( FALSE == GetImageArray(TargetArray,szLastImage) )
	{
		TRACE(L"\nThe second GetImageArray Error in DiffBackupSelectionFile.");
		return FALSE;
	}

	CString strImageFilePath;
	CString strTargetDirectory;
	CString strTargetFileName;

    g_TotalWrittenBytes=0;
    g_TotalFileSize=0;

    g_ProgressPosition=0;

	g_CurrentFileCount=0;
    g_TotalFileCount=0;
	memset(&g_WorkState,0,sizeof(AM01WORKSTATE));

	CString strTempDir;
 
	GetTempDir(strTempDir.GetBuffer(MAX_PATH));
	strTempDir.ReleaseBuffer();

	strImageFilePath=szLastImage;
  
	int nIndex=strImageFilePath.ReverseFind(L'\\');
  // perhaps we should check the directry exist or not.
	strTargetDirectory=strImageFilePath.Left(nIndex);
	strTargetFileName=strImageFilePath.Mid(nIndex+1);
	nIndex=strTargetFileName.ReverseFind(L'.');
	strTargetFileName=strTargetFileName.Left(nIndex);
 
	GetMoreSeperateFile(
						  (LPCTSTR)strImageFilePath,
						  (LPCTSTR)strTempDir,
						  (LPCTSTR)strTargetFileName,
						  FALSE
				     );

	GetTotalFileSize(SelectionArray);

	strTargetFileName=szLastImage;
	nIndex=strTargetFileName.ReverseFind(L'\\');
	strTargetFileName=strTargetFileName.Mid(nIndex+1);
	nIndex=strTargetFileName.ReverseFind(L'.');
	strTargetFileName=strTargetFileName.Left(nIndex);

	WCHAR szOriginalLastImage[MAX_PATH];
	memset(szOriginalLastImage,0,sizeof(szOriginalLastImage));

	DWORD dwOriginalLastFile;
	dwOriginalLastFile=TargetArray.GetUpperBound();
  
	wcsncpy(szOriginalLastImage,TargetArray.GetAt(dwOriginalLastFile).szFileName,MAX_PATH-1);

	BOOL bReturnValue;
	bReturnValue = DiffBackupSelection(SelectionArray,LPCTSTR(strTargetDirectory),LPCTSTR(strTargetFileName),TargetArray,pBackupOption);

	DWORD dwCurrentLastFile;
	dwCurrentLastFile=TargetArray.GetUpperBound();

	if( bReturnValue == FALSE )
	{
		if(dwCurrentLastFile > dwOriginalLastFile)
		{
			DWORD  dwFileNumber;
			CString strDeleteFile;

			for( dwFileNumber = dwOriginalLastFile + 1;
				   dwFileNumber <= dwCurrentLastFile;
				   dwFileNumber ++ )
			{
				strDeleteFile = TargetArray.GetAt(dwFileNumber).szFileName;

				if( (strDeleteFile.Right(4)).CompareNoCase(L".AMG") == 0 )
				{
					 DeleteFile( (LPCTSTR)strDeleteFile );
				}
			}
		}

		return FALSE;
	}

	WriteSignature(TargetArray.GetAt(TargetArray.GetUpperBound()).szFileName, TargetArray);

	return TRUE;
}


// Note: AM01 Version 2

/*=========================================================================
 * 
 * Function Name:  NewGetFileHandlePro
 * Purpose:        Get the required file handle for differential backup
 * Comment:        bCreateFile is added since NewGetFileHandlePro
 *                 will be invoke in different place for different purpose
 *
 *=========================================================================*/

BOOL NewGetFileHandlePro(AM_FILE_HANDLE *pFileHandle, LPCTSTR TargetDirectory, LPCTSTR TargetFileName,CDATA_ARRAY &TargetArray,IMAGE_HEAD *pImageHead,DWORD *pdwErrorCode)
{
	CString strTargetDirectory;
	CString strTargetFileName;
	CString strDataFilePath;

	strTargetDirectory=TargetDirectory;
	strTargetFileName=TargetFileName;

	// we need the original data file first.
	  
	strDataFilePath=strTargetDirectory + L"\\" + strTargetFileName + AM_FILE_SUFFIX;

	  // now we open the source file with the way of read only

	HANDLE hBaseFile;
	hBaseFile=CreateFile((LPCTSTR)strDataFilePath,
						   GENERIC_READ,FILE_SHARE_READ,NULL,
						   OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if(INVALID_HANDLE_VALUE == hBaseFile)
	{
		TRACE(L"\nOpen base file error.");
		return FALSE;
	}

	DWORD      dwReadBytes;
	ReadFile( hBaseFile,pImageHead,sizeof(IMAGE_HEAD),&dwReadBytes,NULL);
	 
	CloseHandle(hBaseFile);
	hBaseFile=NULL;
	 
	// here we get the handle of (*pFileHandle).hCurrentTarget
	 
	if( FALSE == OpenNewFile((LPCTSTR)strTargetDirectory,(LPCTSTR)strTargetFileName,TargetArray,pImageHead, &( (*pFileHandle).hCurrentTarget ) ) )
	{
		 return FALSE;
	}

	CString strTempDir;
	GetTempDir(strTempDir.GetBuffer(MAX_PATH));
	strTempDir.ReleaseBuffer();

	// open current index file
	CString strCurrentIndexFilePath;
	strCurrentIndexFilePath=strTempDir + L"\\" + strTargetFileName + L".id0";
	(*pFileHandle).hCurrentIndexFile=CreateFile((LPCTSTR)strCurrentIndexFilePath,
				GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,
				CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

	if( INVALID_HANDLE_VALUE== (*pFileHandle).hCurrentIndexFile)
	{
		(*pdwErrorCode)=GetLastError();
		return FALSE;
	}

	// open path array file
	CString strArrayFilePath;
	strArrayFilePath=strTempDir + L"\\" + strTargetFileName + L".ary";
	(*pFileHandle).hArrayFile=CreateFile((LPCTSTR)strArrayFilePath,
				GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,
				OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if(INVALID_HANDLE_VALUE == (*pFileHandle).hArrayFile)
	{
		(*pdwErrorCode)=GetLastError();
		return FALSE;
	}

	// open total index file
	CString strTotalIndexFilePath;
	strTotalIndexFilePath=strTempDir + L"\\" + strTargetFileName + AM_INDEX_SUFFIX;
	(*pFileHandle).hTotalIndexFile=CreateFile((LPCTSTR)strTotalIndexFilePath,
					GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,
					OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if(INVALID_HANDLE_VALUE == (*pFileHandle).hTotalIndexFile)
	{
		(*pdwErrorCode)=GetLastError();
		return FALSE;
	}

	// open stamp file
	CString strStampFilePath;
	strStampFilePath=strTempDir + L"\\" + strTargetFileName + L".stp";
	(*pFileHandle).hStampFile=CreateFile((LPCTSTR)strStampFilePath,
				  GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,
				  OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if(INVALID_HANDLE_VALUE == (*pFileHandle).hStampFile)
	{
	  (*pdwErrorCode)=GetLastError();
	  return FALSE;
	}

	(*pdwErrorCode)=0;
	return TRUE;
}

BOOL DiffBackupSelection(CStringArray &SelectionArray, LPCTSTR TargetDirectory, LPCTSTR TargetFileName,CDATA_ARRAY &TargetArray,PBACKUPOPTION pBackupOption)
{
  // should check the files that need is existing.
  // when find a new path, the dwPathID will increase one.
  // dwPathID begin with zero, and PathID begin with 1.
	__int64 lFilePointer;
	DWORD dwTimeStamp;
	BOOL bError;
	bError = FALSE;
  
	if (g_FailRetrySchedule.GetSize()) g_FailRetrySchedule.RemoveAll();
	AM_FILE_HANDLE FileHandle;
	FileHandle.hArrayFile=NULL;
	FileHandle.hCurrentIndexFile=NULL;
	FileHandle.hCurrentTarget=NULL;
	FileHandle.hStampFile=NULL;
	FileHandle.hTotalIndexFile=NULL;

	BOOL bGetFileHandle;

	CString LastFileName;
	LastFileName=TargetArray.GetAt(TargetArray.GetUpperBound()).szFileName;
	int nIndex;
	nIndex = LastFileName.ReverseFind(L'\\');
	LastFileName=LastFileName.Mid(nIndex+1);
	nIndex = LastFileName.ReverseFind(L'.');
	LastFileName=LastFileName.Left(nIndex);

	DWORD dwErrorCode;
  
	IMAGE_HEAD ImageHead;
  
	bGetFileHandle = NewGetFileHandlePro(&FileHandle,TargetDirectory,(LPCTSTR)LastFileName,TargetArray,&ImageHead,&dwErrorCode);

	if(bGetFileHandle == FALSE)
	{
		TRACE(L"\n GetFileHandle error while doing differential backup");
		return FALSE;
	}
  

	//-- for debug begin
	YGSetFilePointer(FileHandle.hArrayFile,0,FILE_END,lFilePointer);
	//-- for debug end

	//-- load the current segment begin

	g_SegmentInfo.dwCurrentSegment=0;
	g_SegmentInfo.dwCurrentOffset=0;

	DWORD dwSegmentSize;
	dwSegmentSize=SEGMENT_SIZE;

	// LoadSegment is a function that deal with
	// a certain mount of files one time.
	// if there are much files, this is need to avoid lack of memory

	NewLoadSegment( FileHandle.hArrayFile,
	              g_SegmentInfo.PathArray,
				  g_SegmentInfo.dwCurrentSegment,
				  dwSegmentSize );

	//-- load the current segment end
	// record stamp information
	RecordStampInfo(FileHandle.hStampFile);

	// get a image head for preparing for
	// a new image.

	DWORD dwReadBytes;
  
	YGSetFilePointer(FileHandle.hCurrentTarget,0,FILE_END,lFilePointer);

	CURRENT_INDEX_NODE CurrentIndexNode;
	memset(&CurrentIndexNode,0,sizeof(CurrentIndexNode));
	CurrentIndexNode.dwPathCount=0;

	DWORD WrittenBytes;

	WriteFile( FileHandle.hCurrentIndexFile,
	         &CurrentIndexNode,
	         sizeof(CurrentIndexNode),
             &WrittenBytes,
			 NULL );

	DWORD dwPathID;
  
	// Cet the begining of PathID;
	TOTAL_INDEX_NODE TotalIndexNode;

	YGSetFilePointer(FileHandle.hTotalIndexFile,0,FILE_BEGIN,lFilePointer);
	ReadFile(FileHandle.hTotalIndexFile,&TotalIndexNode,sizeof(TotalIndexNode),&dwReadBytes,NULL);

	// the current max dwPathID
	dwPathID=TotalIndexNode.dwTotalPathCount;

	CStringArray TotalFilePathArray;

	YGSetFilePointer(FileHandle.hArrayFile,0,FILE_END,lFilePointer);


	DiffStoreExtraDirInfo( SelectionArray,
							&FileHandle,
							&dwPathID
						);
  
  
	CString OnePathString;

	for( long ArrayIndex=0;
		ArrayIndex < SelectionArray.GetSize();
		ArrayIndex++ )
	{
        OnePathString=SelectionArray.ElementAt(ArrayIndex);

		if(OnePathString.GetLength()>=2)
		{ 
		   if(IsOneDirectory(OnePathString))
		   {
			  if(
				  DiffBackupDirectory(
				                       (LPCTSTR)OnePathString,
				                       &FileHandle,
									   &ImageHead,
									   &dwPathID,
									   TargetArray,
									   pBackupOption
									  ) == FALSE
				)
			  {
				  TRACE(L"\nDifferential backup directory" + OnePathString + L" failed.");
				  g_bCancel=TRUE;
				  break;
			  }
		   }
		   else
		   {
			  if( DiffBackupFile((LPCTSTR)OnePathString,
				                  &FileHandle,
							      &ImageHead,
				                  &dwPathID,
				                  TargetArray,
								  pBackupOption) == FALSE
				)
			  {
				  TRACE(L"\nDifferential backup file " + OnePathString + L" failed.");
				  g_bCancel=TRUE;
				  break;
			  }
		   }
		}
		else
		{
			TRACE(L"\n" + OnePathString + L" is too short, at least two characters needed.");
			g_bCancel=TRUE;
			break;
		}
	}

	if(g_bCancel == TRUE || g_bUserCancel == TRUE)
	{
		CloseFileHandle(&FileHandle);
		ClearExtraFile(TargetFileName);
		return FALSE;
	}

	AdjustIndexHead(FileHandle.hCurrentIndexFile,dwPathID);

	// here we close all the file handle.
	CloseFileHandle(&FileHandle);

	// here we should close the handle so that we can rename
	// the name of some file.

	if( FALSE == NewCurrentIdxToTotalIdx(TargetFileName,dwPathID,&dwTimeStamp))
	{
		TRACE(L"\nNewCurrentIdxToTotalIdx error in DiffBackupSelection.");
		bError = TRUE;
	}

	if( FALSE == GetHandleForCombine(&FileHandle,TargetDirectory,TargetFileName,&dwErrorCode) )
	{
		bError = TRUE;
	}

	//-- for debug begin
	CString strLastFile;
	strLastFile = TargetArray.GetAt(TargetArray.GetUpperBound()).szFileName;
	//-- for debug end

	FileHandle.hCurrentTarget=CreateFile(TargetArray.GetAt(TargetArray.GetUpperBound()).szFileName,
			            GENERIC_READ|GENERIC_WRITE,
						FILE_SHARE_READ,
						NULL,
						OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL,
						NULL);

	if(INVALID_HANDLE_VALUE == FileHandle.hCurrentTarget)
	{
		// here we should delete one file
		ClearExtraFile(TargetFileName);
		return FALSE;
	}
  
	if( FALSE == CombineFileToOneImg(&FileHandle,TargetArray) )
	{
		bError = TRUE;
	}
	CloseFileHandle(&FileHandle);

	if( FALSE == RecordFileTable(TargetArray) )
	{
		bError = TRUE;
	}
	//RecordLastImageInfo(&FileHandle,TargetArray);
	if( FALSE == CreateDirFilePro(TargetArray) )
	{
		bError = TRUE;
	}

	ClearExtraFile(TargetFileName);

	if(bError)
	{
		return FALSE;
	}

	RecordFailRetrySchedule(TargetArray.GetAt(TargetArray.GetSize()-1).szFileName,dwTimeStamp);
	return TRUE;
}


BOOL DiffStoreDirectoryInfo( LPCTSTR lpstrDirectoryPath,
							 AM_FILE_HANDLE *pFileHandle,
							 DWORD *pdwPathID
							)
{
	__int64 lFilePointer;
	CString SourceDirectoryPath;
	SourceDirectoryPath=lpstrDirectoryPath;

	DWORD WrittenBytes;

	WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile((LPCTSTR)SourceDirectoryPath,
		                            &findFileData);
	  
	if (hFind == INVALID_HANDLE_VALUE)
	{
		  return TRUE;
	}
      
	VERIFY(FindClose(hFind));

	DWORD dwPathIndex;
	BOOL bIsExistingFileResult;
	  
	bIsExistingFileResult = bExistingFileNew(SourceDirectoryPath,pFileHandle->hArrayFile,&dwPathIndex);

	INDEX_DATA IndexData;
	memset(&IndexData,0,sizeof(IndexData));

	ARRAY_DATA ArrayData;
	memset(&ArrayData,0,sizeof(ArrayData));
	 
	// write current index file.
	  
	if(bIsExistingFileResult==FALSE)
	{
		  // if the directory not exist

		(*pdwPathID)=(*pdwPathID)+1;
		  
		IndexData.dwPathID=(*pdwPathID);
		IndexData.wStatusFlag=DIRECTORY_NEW;

		// new path, add it to Array File.
		ArrayData.dwFileAttribute=findFileData.dwFileAttributes;
		ArrayData.dwPathID=(*pdwPathID);
		wcscpy(ArrayData.szFilePath,lpstrDirectoryPath);

		YGSetFilePointer(pFileHandle->hArrayFile,0,FILE_END,lFilePointer);
		WriteFile(pFileHandle->hArrayFile,&ArrayData,sizeof(ArrayData),
			       &WrittenBytes,NULL);
	}
	else 
	{
		// if the directory exist
	    DWORD dwSearchedPathID;
			 
		// dwPathIndex is 0 based, it should add 1 .
		dwSearchedPathID=dwPathIndex+1;
		IndexData.dwPathID=dwSearchedPathID;
		IndexData.wStatusFlag=DIRECTORY_EXIST;
	  
	} // end of judge if file exist.

	IndexData.DataPointer.QuadPart = 0;
	IndexData.FileInfo.dwFileAttributes=findFileData.dwFileAttributes;
	IndexData.FileInfo.nFileSizeHigh=0;
	IndexData.FileInfo.nFileSizeLow=0;
	  
	IndexData.FileInfo.ftLastWriteTime = findFileData.ftLastWriteTime;
	IndexData.FileInfo.ftCreationTime = findFileData.ftCreationTime ;
	IndexData.FileInfo.ftLastAccessTime = findFileData.ftLastAccessTime;

	IndexData.FileInfo.dwReservedWord1=0x49494949;
	  
	// write index file.
	if( FALSE == WriteFile(pFileHandle->hCurrentIndexFile,&IndexData,sizeof(IndexData),
		           &WrittenBytes,NULL) )
	{
		return FALSE;
	}
	  
	return TRUE;
}

BOOL DiffBackupDirectory(
						  LPCTSTR szSourceDirectory,
						  AM_FILE_HANDLE *pFileHandle,
						  IMAGE_HEAD *pImageHead,
						  DWORD *pdwPathID,
						  CDATA_ARRAY &TargetArray,
						  PBACKUPOPTION pBackupOption
						)
{

	if(g_bCancel==TRUE || g_bUserCancel==TRUE)
	{
		return FALSE;
	}

	DWORD dwFileAttribute;
	
	dwFileAttribute = GetFileAttributes(szSourceDirectory);

	if( -1 == dwFileAttribute )
	{
		return FALSE;
	}

	if(  0 == (dwFileAttribute & FILE_ATTRIBUTE_DIRECTORY) )
	{
		// if it is a file.
		return FALSE;
	}

    // first, store the infomation of itself
	
	if( wcslen(szSourceDirectory) > 3 )
	{
	  DiffStoreDirectoryInfo(
		                      szSourceDirectory,
		                      pFileHandle,
							  pdwPathID
							  // TotalFilePathArray
							);
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
		    DiffBackupFile(
						    (LPCTSTR)strSubPath,
						    pFileHandle,
						    pImageHead,
						    pdwPathID,
						    TargetArray,
							pBackupOption);
		}
		else
		{
		    DiffBackupDirectory( (LPCTSTR)strSubPath,
							     pFileHandle,
							     pImageHead,
							     pdwPathID,
							     TargetArray,
								 pBackupOption
							   );
		}
	}while(FindNextFile(hFind,&FindFileData));

	FindClose(hFind);
	
	return TRUE;

}


BOOL DiffBackupFile(
					 LPCTSTR lpstrSourceFilePath,
					 AM_FILE_HANDLE *pFileHandle,
				     IMAGE_HEAD *pImageHead,
					 DWORD *pdwPathID,
					 CDATA_ARRAY &TargetArray,
					 PBACKUPOPTION pBackupOption
				    )

{
	__int64 lFilePointer;
	if(g_bCancel==TRUE || g_bUserCancel==TRUE)
	{
		return FALSE;
	}

	wcsncpy(g_WorkState.szCurrentFile,lpstrSourceFilePath,MAX_PATH);

	WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile(lpstrSourceFilePath,
		                             &findFileData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		TRACE(L"\nSource file is not found in DiffBackupFile.");
		return FALSE;
	}
      
	VERIFY(FindClose(hFind));
	 
	// add a check for source file.
	// if the source file can not be opened, then do not do backup.

	HANDLE hFileSource;
	hFileSource=CreateFile( lpstrSourceFilePath,
                            GENERIC_READ,
                            FILE_SHARE_READ,NULL,OPEN_EXISTING,
     					    FILE_ATTRIBUTE_NORMAL,0);
	 
	if(hFileSource == INVALID_HANDLE_VALUE)
	{

		CString strOpenSourceError;
		strOpenSourceError.LoadString(IDS_OPEN_SOURCE_ERROR);
		
		CString strAskContinue;
		strAskContinue.LoadString(IDS_ASK_CONTINUE);

		int nReturnValue;
		if (pBackupOption->bLogFail || pBackupOption->bRetryFail)
		{
			if (pBackupOption->bLogFail) WriteLog((WCHAR *)lpstrSourceFilePath,pBackupOption->bRetryFail?pBackupOption->dwRetryMinutes:0);
			if (pBackupOption->bRetryFail)
			{
				FAILRETRYSCHEDULE ScheduleItem;
				wcsncpy(ScheduleItem.szImage,TargetArray.GetAt(1).szFileName,MAX_PATH);
				wcsncpy(ScheduleItem.szFailFile,lpstrSourceFilePath,MAX_PATH);
				GetLocalTime(&ScheduleItem.FailTime);
				GetLocalTime(&ScheduleItem.LastTryTime);
				ScheduleItem.dwTryTimes = 0;
				ScheduleItem.dwRetryMinutes = pBackupOption->dwRetryMinutes;
				g_FailRetrySchedule.Add(ScheduleItem);
			}
			nReturnValue = IDYES;
		}
		else
		{
			nReturnValue = AfxMessageBox(strOpenSourceError+L"\n\n"+lpstrSourceFilePath+L"\n\n"+strAskContinue,MB_YESNOCANCEL|MB_ICONINFORMATION,NULL);
		}

		if( IDYES != nReturnValue )
		{
			g_bUserCancel=TRUE; // return FALSE means cancel backup
			return FALSE;
		}

	}

	// get the current file number

	DWORD dwFileNumber;
    dwFileNumber=TargetArray.GetUpperBound();

	YGSetFilePointer(pFileHandle->hCurrentTarget,0,FILE_END,lFilePointer);
	  
	// the following is used to judge whether a file exist
	// or not
	  
	BOOL bIsExistingFileResult;
    BOOL bIsFileChangedResult;

    // the following value is for debug.

	bIsExistingFileResult=FALSE;
	bIsFileChangedResult=TRUE;

	// dwPathID is used to recored max PathID.
	// dwPathIndex will come from ArrayFile.

	DWORD dwPathIndex;

	bIsExistingFileResult = bExistingFileNew(lpstrSourceFilePath,pFileHandle->hArrayFile,&dwPathIndex);

	// use the following if to save some time.
	if(bIsExistingFileResult == FALSE)
	{
		bIsFileChangedResult=TRUE;
	}
	else
	{
		// now, we just compare with the near last time.
		// since we do not compare the file content at this point.
		// the function return dwPathIndex
	    bIsFileChangedResult=bIsFileChanged(lpstrSourceFilePath,pFileHandle->hTotalIndexFile,dwPathIndex);
	}

	DWORD		WrittenBytes;
    LARGE_INTEGER DataFilePointer;

	DataFilePointer.QuadPart = 0;
	if (hFileSource != INVALID_HANDLE_VALUE)
	{
		if	((!bIsExistingFileResult||bIsFileChangedResult)) // end the L"if" expression
		{
			DWORD dwErrorCode;
			BackupFileData(hFileSource,pFileHandle->hCurrentTarget,&DataFilePointer,
							lpstrSourceFilePath,
							pImageHead,
							TargetArray,
							&dwErrorCode,
							pBackupOption);
		} // end if the file need backup

		CloseHandle(hFileSource);
		hFileSource=NULL;
	}

	// the Current Index Data File.
	INDEX_DATA IndexData;
	memset(&IndexData,0,sizeof(IndexData));

    // if it is a new file, we should add array data
	ARRAY_DATA ArrayData;
	memset(&ArrayData,0,sizeof(ArrayData));

	if(bIsExistingFileResult==FALSE)
	{
		// if it is a new file, dwPathID should add 1.
		(*pdwPathID)=(*pdwPathID)+1;

		IndexData.wStatusFlag=FILE_NEW;
		IndexData.dwPathID=(*pdwPathID);

		// if it is a new file, it should be recorded
		// in Path Array File.
		ArrayData.dwFileAttribute=findFileData.dwFileAttributes;
		ArrayData.dwPathID=(*pdwPathID);
		wcscpy(ArrayData.szFilePath,lpstrSourceFilePath);

		YGSetFilePointer(pFileHandle->hArrayFile,0,FILE_END,lFilePointer);
		WriteFile(pFileHandle->hArrayFile,&ArrayData,sizeof(ArrayData),
			        &WrittenBytes,NULL);

	}
	else 
	{
		if(bIsFileChangedResult==TRUE)
		{
			 // if it is an old file, and changed.
			 // dwPathIndex is 0 based, so it should add 1.
			 // dwPathID is 1 based.
             IndexData.wStatusFlag=FILE_CHANGE;
		     IndexData.dwPathID=dwPathIndex+1;
		}
	    else
		{
		    // if file is not changed.
			IndexData.wStatusFlag=FILE_NO_CHANGE;
			IndexData.dwPathID=dwPathIndex+1;
		} // end if it is a new file.
	} // end if file exist or not.


	g_CurrentFileCount++;
	if(g_TotalFileCount>0)
	{
         g_WorkState.wPercent=(WORD)( (g_CurrentFileCount*100)/g_TotalFileCount );
	}


	IndexData.DataPointer.QuadPart = DataFilePointer.QuadPart;
	  
	IndexData.dwFileNumber=dwFileNumber;
	  
	IndexData.FileInfo.dwFileAttributes=findFileData.dwFileAttributes;
	  
	IndexData.FileInfo.nFileSizeHigh=findFileData.nFileSizeHigh;
	IndexData.FileInfo.nFileSizeLow=findFileData.nFileSizeLow;
	  
	IndexData.FileInfo.ftLastWriteTime = findFileData.ftLastWriteTime;
	IndexData.FileInfo.ftCreationTime = findFileData.ftCreationTime ;
	IndexData.FileInfo.ftLastAccessTime = findFileData.ftLastAccessTime ;

	IndexData.FileInfo.dwReservedWord1=0x49494949;

	WriteFile(pFileHandle->hCurrentIndexFile,&IndexData,sizeof(IndexData),&WrittenBytes,NULL);

	return TRUE;
}


// store parent dir information is for restore.
// with these information restore can show the tree quickly.

BOOL DiffStoreExtraDirInfo( 
						    CStringArray &SelectionArray,
						    AM_FILE_HANDLE *pFileHandle,
						    DWORD *pdwPathID
						  )
{
  
  CStringArray ExtraDirArray;

  BOOL bError;
  bError = FALSE;

  long i,StartPoint,ArraySize,LeftPartIndex;

  CString OneString,OneSubString;

  ArraySize=SelectionArray.GetSize();

  for(i=0;i<ArraySize;i++)
  {

	  OneString=SelectionArray.ElementAt(i);


	  OneString.TrimRight(L'\\');
	  

	  OneString=OneString.Left(OneString.ReverseFind(L'\\')+1);


	  StartPoint=3;
	  

	  while( (StartPoint<OneString.GetLength()-1) && 
		     OneString.Find(L'\\',StartPoint)!=-1)
	  {
	     

         LeftPartIndex=OneString.Find(L'\\',StartPoint);
	     

		 OneSubString=OneString.Left(LeftPartIndex);
		 

		 if( bIsFoundInMorePath((LPCTSTR)OneSubString,ExtraDirArray) == FALSE )
		 {
			ExtraDirArray.Add(OneSubString); 
			// DWORD dwErrorCode;
			
			// StoreDirectoryInfo(LPCTSTR(OneSubString),FileHandle,pdwPathID,&dwErrorCode);
            if(
				FALSE == DiffStoreDirectoryInfo(
				                                 (LPCTSTR)OneSubString,
				                                 pFileHandle,
									             pdwPathID
									             // TotalFilePathArray
								                )
			  )
			{
				bError = TRUE;
			}
		 }

		 StartPoint=LeftPartIndex+1;

	  }
  }

  if(bError)
  {
	  return FALSE;
  }

  return TRUE;
}



/*================================================================
 *
 * Function: OpenNewFile()
 * Purpose:  while doing differential backup
 *           Now we create one file for differential backup
 * Comment:  This function is similar to OpenNewTarget()
 *           Hope to integrate them to one in the future.
 *           I modify this function from OpenNewTarget()
 *
 *           The image comment should be added to each new image
 *
 *================================================================*/
BOOL OpenNewFile(LPCTSTR szTargetDir, LPCTSTR szTargetName , CDATA_ARRAY &TargetArray,IMAGE_HEAD *pSourceHead, HANDLE *phCurrentTarget)
{

	DWORD dwCurrentTargetNumber;
	dwCurrentTargetNumber=TargetArray.GetUpperBound();

	WCHAR szDataFilePath[MAX_PATH];

	if( GetNextFileNamePro(szTargetDir,szTargetName,szDataFilePath,dwCurrentTargetNumber+1) == FALSE)
	{
		  return FALSE;
	}
	else
	{
		// TRACE(L"\nGetNextFileName OK, the file name is:%s",szDataFilePath);
	}

	(*phCurrentTarget)=CreateFile(szDataFilePath,
					              GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,
					              CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

	if(INVALID_HANDLE_VALUE == (*phCurrentTarget) )
	{
		 TRACE(L"\n Get Current Target Error.");
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
	wcsncpy(TargetData.szSerialNumber,(LPCTSTR)strGuid,IMAGE_SERIALNUMBER_SIZE - 1);

	// CurrentTargetNumber increase
	dwCurrentTargetNumber++;

	TargetData.dwFileNumber=dwCurrentTargetNumber;
	
	// wcsncpy(TargetData.szFileName,szTargetName,MAX_PATH-1);
	wcsncpy(TargetData.szFileName,szDataFilePath,MAX_PATH-1);
	
	TargetArray.Add(TargetData);
	
	// in the future, we should judge the disk space should be larger than ImageHead

	// here we write the target head.

	WriteTargetHead( (*phCurrentTarget),pSourceHead,dwCurrentTargetNumber,TargetData);

	return TRUE;
}


BOOL bExistingFileNew( LPCTSTR SourceFilePath, HANDLE  hArrayFile, DWORD *pdwPathIndex )
{
	__int64  lFilePointer;
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//-- if the SourceFilePath is found in memory,
	BOOL bFound;
	bFound = FALSE;

	DWORD dwBeginOffset;
	DWORD dwPathOffset;

	dwBeginOffset = g_SegmentInfo.dwCurrentOffset;

	bFound = bPathInSegment( SourceFilePath,g_SegmentInfo.PathArray,
		                     dwBeginOffset,&dwPathOffset );

	if(bFound == TRUE)
	{
		g_SegmentInfo.dwCurrentOffset = dwPathOffset;
	    (*pdwPathIndex) = g_SegmentInfo.dwCurrentSegment * SEGMENT_SIZE + g_SegmentInfo.dwCurrentOffset;
	}

	//-- if did not find the element then load other segment

	if(bFound==FALSE)
	{
		DWORD dwArraySize;
		dwArraySize = GetFileSize(hArrayFile,NULL);

		DWORD dwArrayCount;
		dwArrayCount = ( dwArraySize - sizeof(ARRAY_NODE) ) / sizeof(ARRAY_DATA) ;

		DWORD dwSegmentCount;
		dwSegmentCount= dwArrayCount / SEGMENT_SIZE ;

		//-- store the file pointer begin
		
		LARGE_INTEGER FilePointer;
		YGSetFilePointer(hArrayFile,0,FILE_CURRENT,FilePointer.QuadPart);

		//-- store the file pointer end

		CString FilePathString;
		FilePathString=SourceFilePath;

		DWORD dwSegmentNumber;

		for ( dwSegmentNumber = g_SegmentInfo.dwCurrentSegment + 1 ;
		      dwSegmentNumber <= dwSegmentCount;
			  dwSegmentNumber++ )
		{
			NewLoadSegment(hArrayFile,g_SegmentInfo.PathArray,dwSegmentNumber,SEGMENT_SIZE);
			g_SegmentInfo.dwCurrentSegment=dwSegmentNumber;
			g_SegmentInfo.dwCurrentOffset=0;

			DWORD dwBeginOffset;
			dwBeginOffset=0;

			bFound = bPathInSegment(  (LPCTSTR)FilePathString,
									  g_SegmentInfo.PathArray,
									  dwBeginOffset,
									  &dwPathOffset );
			
			if ( bFound == TRUE )
			{
			   g_SegmentInfo.dwCurrentOffset = dwPathOffset;
		       (*pdwPathIndex) = g_SegmentInfo.dwCurrentSegment * SEGMENT_SIZE + g_SegmentInfo.dwCurrentOffset;
				break;
			}
		}


		if(bFound==FALSE)
		{

			for ( dwSegmentNumber = 0 ;
				  dwSegmentNumber < g_SegmentInfo.dwCurrentSegment;
				  dwSegmentNumber++ )
			{
				NewLoadSegment(hArrayFile,g_SegmentInfo.PathArray,dwSegmentNumber,SEGMENT_SIZE);
				g_SegmentInfo.dwCurrentSegment=dwSegmentNumber;
				g_SegmentInfo.dwCurrentOffset=0;

				DWORD dwBeginOffset;
				dwBeginOffset=0;

				bFound = bPathInSegment(  (LPCTSTR)FilePathString,
										  g_SegmentInfo.PathArray,
										  dwBeginOffset,
										  &dwPathOffset );
				
				if ( bFound == TRUE )
				{
					g_SegmentInfo.dwCurrentOffset = dwPathOffset;
		            (*pdwPathIndex) = g_SegmentInfo.dwCurrentSegment * SEGMENT_SIZE + g_SegmentInfo.dwCurrentOffset;
					break;
				}
			}
		}
		
		//-- restore the array file pointer begin

		YGSetFilePointer(hArrayFile,FilePointer.QuadPart,FILE_BEGIN,lFilePointer);

		//-- restore the array file pointer end
	}

	return bFound;
}


/*====================================================================================
 * 
 *  Function Name:  BackupFile()
 *
 *  Description:    Store one source file to image
 *
 *  Parameters:     lpstrSourceFilePath: the source file to copy
 *
 *                  FileHandle:          Contains some file handle,
 *                                       e.g Target Image file and some temp files.
 *                  
 *                  ImageHead:           will be used while creating a new target image
 *                                       if the target image is too large
 *										 or the target disk is full
 *
 *                  TargetArray:         Record the chain of target file.
 *			
 *		            pdwPathID:           Each file has an unique ID.
 *					
 *					pdwErrorCode:        the return error code
 *
 *
 *====================================================================================*/

BOOL BackupFile( LPCTSTR lpstrSourceFilePath,
				 AM_FILE_HANDLE *pFileHandle,
				 IMAGE_HEAD *pImageHead,
				 CDATA_ARRAY &TargetArray,
				 DWORD * pdwPathID,
				 DWORD * pdwErrorCode,
				 PBACKUPOPTION pBackupOption)
{
	__int64 lFilePointer;
	if(g_bCancel==TRUE || g_bUserCancel==TRUE)
	{
		return FALSE;
	}

	CString SourceFilePath;
	SourceFilePath=lpstrSourceFilePath;

	wcsncpy(g_WorkState.szCurrentFile,lpstrSourceFilePath,MAX_PATH);
	WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile((LPCTSTR)SourceFilePath,
		                            &findFileData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		VERIFY(FindClose(hFind));
		HANDLE hFileSource;
		hFileSource=CreateFile( lpstrSourceFilePath,
                            GENERIC_READ,
                            FILE_SHARE_READ,NULL,OPEN_EXISTING,
     					    FILE_ATTRIBUTE_NORMAL,0 );
	
		if(INVALID_HANDLE_VALUE == hFileSource)
		{
			CString strOpenSourceError;
			strOpenSourceError.LoadString(IDS_OPEN_SOURCE_ERROR);
		
			CString strAskContinue;
			strAskContinue.LoadString(IDS_ASK_CONTINUE);

			int nReturnValue;

			if (pBackupOption->bLogFail || pBackupOption->bRetryFail)
			{
				if (pBackupOption->bLogFail) WriteLog((WCHAR *)lpstrSourceFilePath,pBackupOption->bRetryFail?pBackupOption->dwRetryMinutes:0);
				if (pBackupOption->bRetryFail)
				{
					FAILRETRYSCHEDULE ScheduleItem;
					wcsncpy(ScheduleItem.szImage,TargetArray.GetAt(1).szFileName,MAX_PATH);
					wcsncpy(ScheduleItem.szFailFile,lpstrSourceFilePath,MAX_PATH);
					GetLocalTime(&ScheduleItem.FailTime);
					GetLocalTime(&ScheduleItem.LastTryTime);
					ScheduleItem.dwTryTimes = 0;
					ScheduleItem.dwRetryMinutes = pBackupOption->dwRetryMinutes;
					g_FailRetrySchedule.Add(ScheduleItem);
				}
				nReturnValue = IDYES;
			}
			else
			{
				nReturnValue = AfxMessageBox(strOpenSourceError+L"\n\n"+lpstrSourceFilePath+"\n\n"+strAskContinue,MB_YESNOCANCEL|MB_ICONINFORMATION,NULL);
			}


			if( nReturnValue != IDYES )
			{
				g_bUserCancel=TRUE; // return FALSE means cancel backup
				return FALSE;
			}
		}

		DWORD dwFileNumber;
		dwFileNumber=TargetArray.GetUpperBound();

		LARGE_INTEGER DataFilePointer;
		if (hFileSource != INVALID_HANDLE_VALUE)
		{
			BackupFileData(hFileSource,pFileHandle->hCurrentTarget,&DataFilePointer,
							lpstrSourceFilePath,
							pImageHead,
							TargetArray,
							pdwErrorCode,
							pBackupOption);
			CloseHandle(hFileSource);
			hFileSource=NULL;
		}
		else
		{
			DataFilePointer.QuadPart = 0;
		}

		//DWORD dwFileNumber;
	 //   dwFileNumber=TargetArray.GetUpperBound();
 
		if(g_bCancel == TRUE || g_bUserCancel == TRUE)
		{
			return FALSE;
		}

		(* pdwPathID)=( * pdwPathID)+1;
	  
		INDEX_DATA IndexData;
		memset(&IndexData,0,sizeof(IndexData));
	  
		IndexData.dwPathID=( * pdwPathID);

		// the first backup set is always new file.
		IndexData.wStatusFlag=FILE_NEW;
		IndexData.DataPointer.QuadPart = DataFilePointer.QuadPart;
		IndexData.dwFileNumber=dwFileNumber;
		IndexData.FileInfo.qwCheckSum = 0;
		IndexData.FileInfo.dwFileAttributes=findFileData.dwFileAttributes;
		IndexData.FileInfo.nFileSizeHigh = findFileData.nFileSizeHigh;
		IndexData.FileInfo.nFileSizeLow = findFileData.nFileSizeLow;

		IndexData.FileInfo.ftCreationTime  = findFileData.ftCreationTime;
		IndexData.FileInfo.ftLastAccessTime = findFileData.ftLastAccessTime;
		IndexData.FileInfo.ftLastWriteTime = findFileData.ftLastWriteTime;
	  
		IndexData.FileInfo.dwReservedWord1=0x49494949;

		// Write Current IndexFile
		DWORD WrittenBytes;
		WriteFile(pFileHandle->hCurrentIndexFile,&IndexData,sizeof(IndexData),&WrittenBytes,NULL);

		// write array data file

		ARRAY_DATA ArrayData;
		memset(&ArrayData,0,sizeof(ArrayData));
		ArrayData.dwFileAttribute=findFileData.dwFileAttributes;
		ArrayData.dwPathID=(* pdwPathID);
		wcsncpy(ArrayData.szFilePath,lpstrSourceFilePath,MAX_PATH-1);

		YGSetFilePointer(pFileHandle->hArrayFile,0,FILE_END,lFilePointer);
		WriteFile(pFileHandle->hArrayFile,&ArrayData,
		        sizeof(ArrayData),&WrittenBytes,NULL);

  		(*pdwErrorCode)=0;
	 
    } 
	return TRUE;
}

BOOL BackupFileData(HANDLE hFile,HANDLE &hData,LARGE_INTEGER *pDataFilePointer,
					LPCTSTR lpstrSourceFilePath,
					IMAGE_HEAD *pImageHead,
					CDATA_ARRAY &TargetArray,
					DWORD * pdwErrorCode,
					PBACKUPOPTION pBackupOption)
{
	CString strText;
	ULARGE_INTEGER qwTargetSize;
 	WCHAR szImageSeperateIdentity[MAX_PATH];
		  
	qwTargetSize.LowPart=GetFileSize(hData,&(qwTargetSize.HighPart));
	GetImageSeperateIdentity(lpstrSourceFilePath,szImageSeperateIdentity);

	if(!WriteDataFile(hData,szImageSeperateIdentity,MAX_PATH,qwTargetSize.QuadPart,TargetArray,pImageHead,pBackupOption)) return FALSE;

	YGSetFilePointer(hData,0,FILE_CURRENT,pDataFilePointer->QuadPart);


	BYTE *pDataBuffer;
	  
	pDataBuffer=(BYTE *)malloc(AM_BUFFER_SIZE);

	if( NULL == pDataBuffer)
	{
		strText.LoadString(IDS_NOT_ENOUGH_MEMORY);
		if (pBackupOption->bCmdLine)
			wcout << (LPCTSTR)strText << endl;
		else AfxMessageBox(IDS_NOT_ENOUGH_MEMORY,MB_OK|MB_ICONINFORMATION,NULL);
		(*pdwErrorCode) = NOT_ENOUGH_MEMORY;
		return FALSE;
	}


	BYTE *pCompressBuffer;
		   
	pCompressBuffer = (BYTE *)malloc(AM_COMPRESS_BUFFER);

	if( NULL == pCompressBuffer)
	{
		strText.LoadString(IDS_NOT_ENOUGH_MEMORY);
		if (pBackupOption->bCmdLine)
			wcout << (LPCTSTR)strText << endl;
		else AfxMessageBox(IDS_NOT_ENOUGH_MEMORY,MB_OK|MB_ICONINFORMATION,NULL);
		(*pdwErrorCode)=NOT_ENOUGH_MEMORY;

		free(pDataBuffer);
		pDataBuffer=NULL;

		return FALSE;
	}


    ULARGE_INTEGER  qwLeftBytes;

	qwLeftBytes.LowPart = GetFileSize(hFile,&(qwLeftBytes.HighPart));

	DWORD dwReadBytes;

	DWORD dwRequestRead;
	DWORD dwRequestWrite;
		  
	if(qwLeftBytes.QuadPart >AM_BUFFER_SIZE)
	{
		dwRequestRead=AM_BUFFER_SIZE;
	}
	else
	{
		dwRequestRead=(DWORD)qwLeftBytes.QuadPart ;
	}
	      
	DATA_NODE DataNode;

	// write in this way for easy extend
	memset(&DataNode,0,sizeof(DataNode));
		  
	ReadFile(hFile,pDataBuffer,dwRequestRead,&dwReadBytes,NULL);

	// DWORD dwErrorNumber;

	while( qwLeftBytes.QuadPart >0 
	        && dwReadBytes>0) // infact this condition is enough
				                  // if we only use dwReadBytes, the program will be simple.
	{
		DataNode.dwOrigSize = dwReadBytes;
			   // note: the unsigned number sub
		if(qwLeftBytes.QuadPart > dwRequestRead)
		{
			qwLeftBytes.QuadPart  = qwLeftBytes.QuadPart  - dwRequestRead;
		}
		else
		{
			qwLeftBytes.QuadPart=0;
		}

    	DWORD dwCompressedLen;
		dwCompressedLen = AM_COMPRESS_BUFFER; // the size of the buffer
		int nCompressLevel;
		nCompressLevel = AM_COMPRESS_LEVEL;

		// dwReadBytes is the bytes that need to compress
#ifdef FEIX_TEST
		if (pCompressBuffer && dwCompressedLen != 0)
		{
			free(pCompressBuffer);
			dwCompressedLen = 0;
			pCompressBuffer = (BYTE*)malloc(dwReadBytes);
			dwCompressedLen = dwReadBytes;
			memcpy(pCompressBuffer, pDataBuffer, dwReadBytes);
		}
#else
		if(Z_OK != compress( pCompressBuffer,&dwCompressedLen,pDataBuffer,dwReadBytes,nCompressLevel))
		{
			free(pDataBuffer);
			pDataBuffer=NULL;

			free(pCompressBuffer);
			pCompressBuffer=NULL;

			return FALSE;
		}
#endif


		dwRequestWrite=sizeof(DataNode);
		DataNode.wCompressLevel = AM_COMPRESS_LEVEL;

		// dwCompressedLen is the packet size that need to store.
		DataNode.dwPacketSize = dwCompressedLen;

		// Reserved Begin
		// qwTargetSize: pass by reference, may be changed in function WriteDataFile
		if( FALSE == WriteDataFile(hData,&DataNode,dwRequestWrite,qwTargetSize.QuadPart,TargetArray,pImageHead,pBackupOption) )
		{
			free(pDataBuffer);
			pDataBuffer=NULL;
			free(pCompressBuffer);
			pCompressBuffer=NULL;
			return FALSE;
		}

		if( pBackupOption->bEncrypt )
		{
			// 2004.05.26 added begin
			WCHAR szPassword[PASSWORD_SIZE];
			memcpy(szPassword,pBackupOption->pchPassword,PASSWORD_SIZE * sizeof(WCHAR));
			DecryptPassword(szPassword,PASSWORD_SIZE);
				// 2004.05.26 added end

			if( FALSE == YGEncryptData(szPassword,pCompressBuffer,dwCompressedLen) )
			{
				free(pDataBuffer);
				pDataBuffer=NULL;

				free(pCompressBuffer);
				pCompressBuffer=NULL;
				return FALSE;
			}
		}
			   
		// 2004.05.20 for encryption end

		dwRequestWrite = dwCompressedLen;

		if( FALSE == WriteDataFile(hData,pCompressBuffer, dwRequestWrite,qwTargetSize.QuadPart,TargetArray,pImageHead,pBackupOption) )
		{
			free(pDataBuffer);
			pDataBuffer=NULL;

			free(pCompressBuffer);
			pCompressBuffer=NULL;

			return FALSE;
		}
		// Reserved End

		// qwTargetSize: pass by reference, may be changed in function WriteDataFile

		//if( pBackupOption->bEncrypt )
		//{
		//	// 2004.05.26 added begin
		//	WCHAR szPassword[PASSWORD_SIZE];
		//	memcpy(szPassword,pBackupOption->pchPassword,PASSWORD_SIZE * sizeof(WCHAR));
		//	DecryptPassword(szPassword,PASSWORD_SIZE);
		//	// 2004.05.26 added end

		//	if( FALSE == YGEncryptData(szPassword,pCompressBuffer,dwCompressedLen) )
		//	{
		//		free(pDataBuffer);
		//		pDataBuffer=NULL;

		//		free(pCompressBuffer);
		//		pCompressBuffer=NULL;
		//		return FALSE;
		//	}
		//}

		//DWORD WriteLength = sizeof(DataNode) + dwCompressedLen;
		//BYTE* WriteBuffer = (BYTE*)malloc(WriteLength);
		//memcpy(WriteBuffer, &DataNode, sizeof(DataNode));
		//memcpy(WriteBuffer + sizeof(DataNode), pCompressBuffer, dwCompressedLen);

		//dwRequestWrite = dwCompressedLen;

		//if( FALSE == WriteDataFile(hData,WriteBuffer, WriteLength,qwTargetSize.QuadPart,TargetArray,pImageHead,pBackupOption) )
		//{
		//	free(WriteBuffer);
		//	WriteBuffer = NULL;

		//	free(pDataBuffer);
		//	pDataBuffer=NULL;

		//	free(pCompressBuffer);
		//	pCompressBuffer=NULL;

		//	return FALSE;
		//}
		//free(WriteBuffer);
		//WriteBuffer = NULL;

		// AM01 Version 2, for progress bar
		g_TotalReadBytes=g_TotalReadBytes+dwReadBytes;
		if(g_TotalFileSize > 0 )
		{
			g_ProgressPosition=(WORD)(g_TotalReadBytes*100/g_TotalFileSize);
		}
		else
		{
			TRACE(L"\nThe total file size is error in BackupFile.");
		}

		g_WorkState.wPercent=(WORD)g_ProgressPosition;

		if(qwLeftBytes.QuadPart >AM_BUFFER_SIZE)
		{
			dwRequestRead=AM_BUFFER_SIZE;
		}
		else
		{
			dwRequestRead=qwLeftBytes.LowPart;
		}

		if(g_bUserCancel==TRUE)
		{
			free(pDataBuffer);
			pDataBuffer=NULL;

			free(pCompressBuffer);
			pCompressBuffer=NULL;

			return FALSE;
		}

		ReadFile(hFile,pDataBuffer,dwRequestRead,&dwReadBytes,NULL);

	} // end while

	free(pDataBuffer);
	pDataBuffer=NULL;

	free(pCompressBuffer);
	pCompressBuffer=NULL;
	return TRUE;
}
/*====================================================================
 *
 * Function Name: BackupDirectory()
 * Purpose: BackupOneDir
 *
 * note:    you can not pass a file name to this function.
 *
 *====================================================================*/

BOOL BackupDirectory(LPCTSTR szSourceDirectory,
					 AM_FILE_HANDLE *pFileHandle,
					 IMAGE_HEAD *pImageHead,
					 CDATA_ARRAY &TargetArray,
					 DWORD &dwPathID,
					 PBACKUPOPTION pBackupOption
					 )
{
	if(g_bCancel==TRUE || g_bUserCancel == TRUE)
	{
		return FALSE;
	}


	DWORD dwFileAttribute;
	
	dwFileAttribute = GetFileAttributes(szSourceDirectory);

	if( -1 == dwFileAttribute )
	{
		return FALSE;
	}

	if(  0 == (dwFileAttribute & FILE_ATTRIBUTE_DIRECTORY) )
	{
		// if it is a file.
		return FALSE;
	}

	// TRACE(L"\nDir Name:%s",szSourceDirectory);

	DWORD dwErrorCode;

	StoreDirectoryInfo( szSourceDirectory,
		                pFileHandle,
						&dwPathID,
						&dwErrorCode);
	
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
			// TRACE(L"\nFile Name:%s",(LPCTSTR)strSubPath);
		    BackupFile((LPCTSTR)strSubPath,
			           pFileHandle,
					   pImageHead,
					   TargetArray,
					   &dwPathID,
					   &dwErrorCode,
					   pBackupOption);
		}
		else
		{
		    BackupDirectory((LPCTSTR)strSubPath,
							   pFileHandle,
							   pImageHead,
							   TargetArray,
							   dwPathID,
							   pBackupOption);
		}
	}while(FindNextFile(hFind,&FindFileData));

	FindClose(hFind);
	
	return TRUE;

}


BOOL GetWorkState(AM01WORKSTATE *pWorkState)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	(*pWorkState)=g_WorkState;
	return TRUE;
}

BOOL SetBakCancel()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	g_bUserCancel=TRUE;
	return TRUE;
}


BOOL CheckSelectionArray(CStringArray &SelectionArray)
{
  BOOL bOnePathValid;
  bOnePathValid=FALSE;

  for(long i=0; i<SelectionArray.GetSize(); i++)
  {
	  if( GetFileAttributes((LPCTSTR) (SelectionArray.ElementAt(i)) ) != -1)
	  {
		  bOnePathValid=TRUE;
		  break;
	  }
  }

  return bOnePathValid;
}


void GetTotalFileSize(CStringArray &SelectionArray)
{

	CString OnePathString;

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
		    //-- Length of OnePathString
			//-- shoud be great than or equal 2
			//-- Infact this shoud use an sentenct to quit
			//-- ASSERT(OnePathString.GetLength()>=2);
			ASSERT(FALSE);
		}

	}
}


BOOL GetDirectoryInfo(LPCTSTR szSourceDirectory)
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
			// TRACE(L"\nFile Name:%s",(LPCTSTR)strSubPath);
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

BOOL GetFileInfo(LPCTSTR FilePath)
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

	  g_TotalFileSize=g_TotalFileSize+qwOneFileSize.QuadPart;
	  g_TotalFileCount++;

	  return TRUE;
}


BOOL bPathInSegment(
					 LPCTSTR		SourceFilePath,
					 CPATH_ARRAY	&PathArray,
					 DWORD			dwBeginOffset,
					 DWORD			*pdwPathOffset
				   )
{

	CString FilePathString;
	FilePathString=SourceFilePath;

	// BOOL bReturnValue;

	// before compare, insure the path is ended with L"\\"
	// if(FilePathString.Right(1)!=L"\\")
	// {
	     // FilePathString=FilePathString+L"\\";
	// }

	BOOL bFound;
	bFound=FALSE;

	if(dwBeginOffset >= (DWORD)( PathArray.GetSize()) )
	{
		dwBeginOffset=0;
	}
	
	DWORD i;
	for(i=dwBeginOffset; i < (DWORD) (PathArray.GetSize()); i++)
	{
		if ( FilePathString.CompareNoCase( LPCTSTR (PathArray.GetAt(i).szFilePath ) ) == 0 )
		{
			bFound=TRUE;
			break;
		}
	}

	if(bFound==FALSE)
	{
		for(i=0 ; i< dwBeginOffset; i++)
		{

			if ( FilePathString.CompareNoCase( LPCTSTR (PathArray.GetAt(i).szFilePath ) ) == 0 )
			{
				bFound=TRUE;
				break;
			}
		}
	}


	if(bFound==TRUE)
	{
	    (*pdwPathOffset) = i;
    }
	else
	{
		(*pdwPathOffset) = 0;
	}

	
	return bFound;
}

BOOL LoadFirstSegment(HANDLE hArrayFile)
{

  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  //-- load the current segment begin
  
  g_SegmentInfo.dwCurrentSegment=0;
  g_SegmentInfo.dwCurrentOffset=0;

  DWORD dwSegmentSize;
  dwSegmentSize=SEGMENT_SIZE;

  NewLoadSegment( hArrayFile,
	              g_SegmentInfo.PathArray,
				  g_SegmentInfo.dwCurrentSegment,
				  dwSegmentSize );

  //-- load the current segment end
  
  return TRUE;
}


BOOL ClearSignature(LPCTSTR szLastImage)
{
	__int64 lFilePointer;
	HANDLE hLastImage;

	IMAGE_HEAD ImageHead;

	hLastImage=CreateFile(szLastImage,GENERIC_READ|GENERIC_WRITE,
		                  FILE_SHARE_READ,NULL,
						  OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,
						  NULL);

	if(hLastImage == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	YGSetFilePointer(hLastImage,0,FILE_BEGIN,lFilePointer);

	DWORD dwReadBytes;

	DWORD WrittenBytes;
	
	ReadFile(hLastImage,&ImageHead,sizeof(ImageHead),&dwReadBytes,NULL);

	ImageHead.dwFinishSignature = IMAGE_STATUS_BEGIN;

	YGSetFilePointer(hLastImage,0,FILE_BEGIN,lFilePointer);

	WriteFile(hLastImage,&ImageHead,sizeof(ImageHead),&WrittenBytes,NULL);

	CloseHandle(hLastImage);

	return TRUE;

}


BOOL WriteFileHead(AM_FILE_HANDLE *pFileHandle,const SETTING_DATA_INFO SettingDataInfo,CDATA_ARRAY &TargetArray)
{
	DWORD WrittenBytes;

	BOOL bError;
	bError = FALSE;

	//-- write destination index file
	//-- path number

	// write current index node
	CURRENT_INDEX_NODE CurrentIndexNode;
	memset(&CurrentIndexNode,0,sizeof(CurrentIndexNode));
	CurrentIndexNode.dwPathCount=0;
  
  
	if( FALSE == WriteFile(pFileHandle->hCurrentIndexFile,&CurrentIndexNode,sizeof(CurrentIndexNode),
                         &WrittenBytes,NULL) )
	{
		bError = TRUE;
	}

	// write array node
	ARRAY_NODE ArrayNode;
	memset(&ArrayNode,0,sizeof(ArrayNode));
  
	if( FALSE == WriteFile(pFileHandle->hArrayFile,&ArrayNode,sizeof(ArrayNode),
	                     &WrittenBytes,NULL) )
	{
		bError = TRUE;
	}

	// write stamp node
	STAMP_NODE StampNode;
	memset(&StampNode,0,sizeof(StampNode));
	if(FALSE == WriteFile(pFileHandle->hStampFile,&StampNode,sizeof(StampNode),
				        &WrittenBytes,NULL) )
	{
		bError = TRUE;
	}
  
	// write image head
	IMAGE_HEAD ImageHead;
  
	// first use zero to fill the head area
	memset(&ImageHead,0,sizeof(ImageHead));

	wcscpy(ImageHead.szImageIdentity,IMAGE_IDENTITY);
	wcsncpy(ImageHead.szComment,SettingDataInfo.szComment,IMAGE_COMMENT_SIZE-1);
  
	ImageHead.dwFileVersion=IMAGE_FILE_VERSION;
	ImageHead.dwCompressLevel=AM_COMPRESS_LEVEL;
	ImageHead.wBackupType=SettingDataInfo.wBackupType;
	ImageHead.dwCheckSum=0; // now have no check sum

	// 2004.05.20 for encrypt added begin
	ImageHead.bEncrypt = SettingDataInfo.BackupOption.bEncrypt;
	memcpy(ImageHead.pchPassword,SettingDataInfo.BackupOption.pchPassword,PASSWORD_SIZE * sizeof(WCHAR));

	// 2004.05.20 for encrypt added end

	ImageHead.TotalIndexPointer.QuadPart=0;
	ImageHead.ArrayPointer.QuadPart=0;
	ImageHead.StampPointer.QuadPart=0;
 
	wcsncpy(ImageHead.szSerialNumber,
	      TargetArray.GetAt(1).szSerialNumber,
		  IMAGE_SERIALNUMBER_SIZE-1);

	ImageHead.dwFileNumber=1;
	// if there is the last file, this will be -1;


	if( FALSE == WriteFile(pFileHandle->hCurrentTarget,&ImageHead,sizeof(ImageHead),&WrittenBytes,NULL) )
	{
		bError = TRUE;
	}

	if(bError)
	{
		return FALSE;
	}

	return TRUE;
}


BOOL RecordStampInfo(HANDLE hStampFile)
{
	__int64 lFilePointer;
	// this function will use FileHandle.hStampFile
	STAMP_DATA StampData;
	memset(&StampData,0,sizeof(StampData));

	GetLocalTime( &(StampData.StampTime) );

	YGSetFilePointer(hStampFile,0,FILE_END,lFilePointer);

    DWORD WrittenBytes;
	if( FALSE == WriteFile(hStampFile,&StampData,sizeof(StampData),&WrittenBytes,NULL) )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL RecordSelection(HANDLE hImageFile,CStringArray &SelectionArray)
{
	// this function will use
	// FileHandle.hDataFile

	BOOL bError;
	bError = FALSE;
    
	SELECTION_NODE SelectionNode;
	DWORD WrittenBytes;

	memset(&SelectionNode,0,sizeof(SelectionNode));
    SelectionNode.dwPathCount=SelectionArray.GetSize();

    if( FALSE == WriteFile(hImageFile,&SelectionNode,sizeof(SelectionNode),
	                       &WrittenBytes,NULL) )
	{
		bError = TRUE;
	}

    SELECTION_DATA SelectionData;
	WIN32_FIND_DATA findFileData;
	HANDLE hFind;
	hFind=NULL;

	for(int nPathNumber=0; nPathNumber<SelectionArray.GetSize(); nPathNumber++)
	{
      hFind=FindFirstFile( LPCTSTR( SelectionArray.ElementAt(nPathNumber) ),&findFileData );
	  
	  // if it is a driver letter than add it to selection
	  if (hFind != INVALID_HANDLE_VALUE) 
	  {	  
	    memset(&SelectionData,0,sizeof(SelectionData));
	    SelectionData.dwFileAttribute=findFileData.dwFileAttributes;
	    wcscpy(SelectionData.szFilePath,(LPCTSTR)(SelectionArray.ElementAt(nPathNumber)) );
        if( FALSE == WriteFile(hImageFile,&SelectionData,
	                           sizeof(SelectionData),&WrittenBytes,NULL) )
		{
			bError = TRUE;
		}
	  }
	  else if(SelectionArray.ElementAt(nPathNumber).GetLength()==2 )
	  {
	    memset(&SelectionData,0,sizeof(SelectionData));
	    wcscpy(SelectionData.szFilePath,(LPCTSTR)(SelectionArray.ElementAt(nPathNumber)) );
        if( FALSE == WriteFile(hImageFile,&SelectionData,
	                           sizeof(SelectionData),&WrittenBytes,NULL) )
		{
			bError = TRUE;
		}
	  }
	  FindClose(hFind); 
	}

	if(bError)
	{
		return FALSE;
	}
	return TRUE;
}


BOOL StoreExtraDirInfo(CStringArray &SelectionArray,AM_FILE_HANDLE *pFileHandle,DWORD *pdwPathID)
{
  CStringArray ExtraDirArray;

  BOOL bError;
  bError = FALSE;

  long i,StartPoint,ArraySize,LeftPartIndex;

  CString OneString,OneSubString;

  ArraySize=SelectionArray.GetSize();

  for(i=0;i<ArraySize;i++)
  {

	  OneString=SelectionArray.ElementAt(i);

	  OneString.TrimRight(L'\\');
	  

	  OneString=OneString.Left(OneString.ReverseFind(L'\\')+1);

	  StartPoint=3;
	  
	  while( (StartPoint<OneString.GetLength()-1) && 
		     OneString.Find(L'\\',StartPoint)!=-1)
	  {
	     

         LeftPartIndex=OneString.Find(L'\\',StartPoint);
	     

		 OneSubString=OneString.Left(LeftPartIndex);
		 

		 if( bIsFoundInMorePath((LPCTSTR)OneSubString,ExtraDirArray) == FALSE )
		 {
			ExtraDirArray.Add(OneSubString); 

			DWORD dwErrorCode;
			
			if( FALSE == StoreDirectoryInfo(LPCTSTR(OneSubString),pFileHandle,pdwPathID,&dwErrorCode) )
			{
				bError = TRUE;
			}
		 }

		 StartPoint=LeftPartIndex+1;
	  } // end while
  } // end for

  if(bError)
  {
	  return FALSE;
  }

  return TRUE;

}

BOOL IsOneDirectory(LPCTSTR szFileName)
{

	DWORD dwFileAttribute;
	dwFileAttribute = 0;
	dwFileAttribute = GetFileAttributes(szFileName);

	if(-1 == dwFileAttribute )
	{
		return FALSE;
	}
	else
	{
	  if( 0 != (dwFileAttribute & FILE_ATTRIBUTE_DIRECTORY) )
	  {
		  return TRUE;
	  }
	}

	return FALSE;
}


BOOL GetLastFileName(LPCTSTR szDirFileName,WCHAR *szLastImage)
{
	HANDLE hDirFile;
	hDirFile=CreateFile(szDirFileName,GENERIC_READ,
		                FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(INVALID_HANDLE_VALUE == hDirFile)
	{
		TRACE(L"\n can not open dir file");
		return FALSE;
	}

	DIR_FILE_HEAD DirFileHead;
	DWORD dwReadBytes;

	ReadFile(hDirFile,&DirFileHead,sizeof(DirFileHead),&dwReadBytes,NULL);

	if( wcscmp(DirFileHead.chDirSignature,DIR_FILE_IDENTITY) != 0)
	{
		TRACE(L"\n not AM01 dir file.");
		CloseHandle(hDirFile);
		return FALSE;
	}
	
	// to check the integrity of the image file
	ULONGLONG qwChecksum;
	GetChecksum(hDirFile,sizeof(DirFileHead),&qwChecksum);
	if( DirFileHead.qwCheckSum != qwChecksum )
	{
		TRACE(L"\n dir file corrupted.");
		CloseHandle(hDirFile);
		return FALSE;
	}

	TABLE_DATA TableData;
	__int64 lFilePointer;
	__int64 nMoveSize = 0;
	nMoveSize -= sizeof(TableData);
	YGSetFilePointer(hDirFile,nMoveSize,FILE_END,lFilePointer);
	ReadFile(hDirFile,&TableData,sizeof(TableData),&dwReadBytes,NULL);
	
	wcsncpy(szLastImage,TableData.szFileName,MAX_PATH-1);
	
	// after use dir file, the handle should be closed
	CloseHandle(hDirFile);
	hDirFile=NULL;
	
	return TRUE;
}

// if the user select an invalid last image
// then ask user to select again.
// if user select an valid image, then return TRUE;
// if user do not want to select, press cancel, return FALSE;
 
BOOL SelectLastImage(WCHAR *szLastImage)
{
	 szLastImage[0]=L'\0';

	 DWORD dwErrorCode;
	 WCHAR szFileName[MAX_PATH];

	 do
	 {
	   if (SelectFile(L"AMG",L"AM01 Image File",szFileName)==TRUE)
	   {
		  // if user do not press cancel
		  wcsncpy(szLastImage , szFileName, MAX_PATH-1 );
	   }
	   else
	   {
		  // if user press cancel
		  return FALSE; 
	   }
	 } while ( CheckLastImage(szLastImage,&dwErrorCode) == FALSE );
	 
	 return TRUE;
}


BOOL NewLoadSegment(
				     HANDLE	hArrayFile, 
				     CPATH_ARRAY &PathArray,
				     DWORD dwSegmentNumber,
				     DWORD dwSegmentSize
				   )
{
   // Certainly we should insure that 
   // need the RestoreInfo.szImageFile;

   //-- store the file pointer begin

   __int64 OrigFilePointer = 0,lFilePointer;
   YGSetFilePointer(hArrayFile,0,FILE_CURRENT,OrigFilePointer);

   //-- store the file pointer end
 	
   // this is needed, since we often refresh PathArray
   if (PathArray.GetSize()) PathArray.RemoveAll();

   if(hArrayFile == INVALID_HANDLE_VALUE)
   {
	   // if open data file error
	   // return
	   //-- for debug
	   TRACE(L"Open image file error.\n");
	   //-- end for debug
	   return FALSE;
   }
   
   DWORD dwReadBytes;

   __int64 lPointer = sizeof(ARRAY_NODE) + sizeof(ARRAY_DATA)*dwSegmentNumber*dwSegmentSize;
   // skip array node
   YGSetFilePointer(hArrayFile,lPointer,FILE_BEGIN,lFilePointer);

   ARRAY_DATA ArrayData;

   DWORD dwPathNumber;
   
   for( dwPathNumber=0; dwPathNumber<dwSegmentSize ; dwPathNumber++ )
   {
	   ReadFile(hArrayFile,&ArrayData,sizeof(ArrayData),&dwReadBytes,NULL);

	   if(dwReadBytes==0)
	   {
		   // in fact we needn't read to the end of the file
		   // we only need read the old array data.
		   break;
	   }

	   PathArray.Add(ArrayData);
   }

   //-- restore the array file pointer begin
   YGSetFilePointer(hArrayFile,OrigFilePointer,FILE_BEGIN,lFilePointer);
   //-- restore the array file pointer end

   return TRUE;
}


BOOL NewCurrentIdxToTotalIdx(LPCTSTR TargetFileName, DWORD dwMaxPathID,DWORD *pdwTimeStamp)
{
	__int64 lFilePointer;
	// open file
	// CString strTargetDirectory(TargetDirectory);

	BOOL bError;
	bError = FALSE;

	CString strTargetDirectory;

	GetTempDir(strTargetDirectory.GetBuffer(MAX_PATH));
	strTargetDirectory.ReleaseBuffer();
  
	CString strTargetFileName(TargetFileName);

	CString strCurrentIndexPath;
	CString strTotalIndexPath;
	CString strNewTotalIndexPath;
  
	strCurrentIndexPath=strTargetDirectory+L"\\"+strTargetFileName+L".id0";

	strTotalIndexPath=strTargetDirectory+L"\\"+strTargetFileName+AM_INDEX_SUFFIX;

	strNewTotalIndexPath=strTargetDirectory+L"\\"+strTargetFileName+L".id2";

	HANDLE hCurrentIndexFile;
	hCurrentIndexFile=NULL;
	hCurrentIndexFile=CreateFile((LPCTSTR)strCurrentIndexPath,
	              GENERIC_READ,FILE_SHARE_READ,
				  NULL,OPEN_EXISTING,
				  FILE_ATTRIBUTE_NORMAL,NULL);

	if(INVALID_HANDLE_VALUE == hCurrentIndexFile)
	{
		bError = TRUE;
		return FALSE;
	}


	HANDLE hTotalIndexFile;
	hTotalIndexFile=NULL;
	hTotalIndexFile=CreateFile((LPCTSTR)strTotalIndexPath,
								GENERIC_READ,FILE_SHARE_READ,NULL,
								OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if( INVALID_HANDLE_VALUE == hTotalIndexFile )
	{
		CloseHandle(hCurrentIndexFile);
		bError = TRUE;
		return FALSE;
	}

	HANDLE hNewTotalIndexFile;
	hNewTotalIndexFile=NULL;
	hNewTotalIndexFile=CreateFile((LPCTSTR)strNewTotalIndexPath,
				                 GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,
							     CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

	if( INVALID_HANDLE_VALUE == hNewTotalIndexFile )
	{
		bError = TRUE;
		CloseHandle(hCurrentIndexFile);
		CloseHandle(hTotalIndexFile);
		return FALSE;
	}

	DWORD dwTimeStampNumber;
	DWORD dwPathNumber;
	DWORD dwReadBytes;
	DWORD WrittenBytes;

	// get last TimeStamp and last PathID
	TOTAL_INDEX_NODE TotalIndexNode;
  
	if( FALSE == ReadFile(hTotalIndexFile,&TotalIndexNode,sizeof(TotalIndexNode),
		                    &dwReadBytes,NULL) )
	{
		bError = TRUE;
	}

	// first fill the new total index with zero.
	TOTAL_INDEX_NODE NewTotalIndexNode;
	INDEX_DATA IndexData;

	memset(&NewTotalIndexNode,0,sizeof(NewTotalIndexNode));
	memset(&IndexData,0,sizeof(IndexData));

	YGSetFilePointer(hNewTotalIndexFile,0,FILE_BEGIN,lFilePointer);
	WriteFile(hNewTotalIndexFile,&NewTotalIndexNode,sizeof(NewTotalIndexNode),
	        &WrittenBytes,NULL);

	for(dwTimeStampNumber=1; dwTimeStampNumber<=TotalIndexNode.dwTimeStampCount+1; dwTimeStampNumber++)
	{
  		for(dwPathNumber=1; dwPathNumber<=dwMaxPathID; dwPathNumber++)
		{
			WriteFile(hNewTotalIndexFile,&IndexData,sizeof(IndexData),
				        &WrittenBytes,NULL);
		}
	}

	YGSetFilePointer(hNewTotalIndexFile,0,FILE_BEGIN,lFilePointer);

	// locate old total index to new total index

	YGSetFilePointer(hTotalIndexFile,sizeof(TotalIndexNode),FILE_BEGIN,lFilePointer);
  
	for(dwTimeStampNumber=1; dwTimeStampNumber<=TotalIndexNode.dwTimeStampCount; dwTimeStampNumber++)
	{
		for(dwPathNumber=1; dwPathNumber<=TotalIndexNode.dwTotalPathCount; dwPathNumber++)
		{
			ReadFile(hTotalIndexFile,&IndexData,sizeof(IndexData),
				       &dwReadBytes,NULL);
			DWORD dwNewTotalIndexOffset;
		  
			// it seems that IndexData.dwPathID have some problem
			// because some data is blank, there is no dwPathID
			// so we should use count: dwPathNumber to locate.

			dwNewTotalIndexOffset=GetTotalIndexOffset(dwMaxPathID,dwTimeStampNumber,dwPathNumber);
	      
			YGSetFilePointer(hNewTotalIndexFile,dwNewTotalIndexOffset,FILE_BEGIN,lFilePointer);

			WriteFile(hNewTotalIndexFile,&IndexData,sizeof(IndexData),&WrittenBytes,NULL);

		}
	}

	CloseHandle(hTotalIndexFile);
	hTotalIndexFile=NULL;

	// now, locate the current index file to total index file.

	YGSetFilePointer(hCurrentIndexFile,sizeof(CURRENT_INDEX_NODE),FILE_BEGIN,lFilePointer);

	if( FALSE == ReadFile(hCurrentIndexFile,&IndexData,sizeof(IndexData),
		                    &dwReadBytes,NULL) )
	{
		bError = TRUE;
	}

	while (dwReadBytes == sizeof(IndexData))
	{
		DWORD dwNewTotalIndexOffset;

		// In Current Index to TotalIndex, then  the case of 
		// dwPathID is zero is not possible,so we use IndexData.dwPathID
		// to locate
		dwNewTotalIndexOffset=GetTotalIndexOffset(dwMaxPathID, TotalIndexNode.dwTimeStampCount + 1, IndexData.dwPathID );

		YGSetFilePointer(hNewTotalIndexFile,dwNewTotalIndexOffset,FILE_BEGIN,lFilePointer);

		WriteFile(hNewTotalIndexFile,&IndexData,sizeof(IndexData),
					&WrittenBytes,NULL);

		ReadFile(hCurrentIndexFile,&IndexData,sizeof(IndexData),
		       &dwReadBytes,NULL);
	}
	NewTotalIndexNode.dwTimeStampCount=TotalIndexNode.dwTimeStampCount+1;
	NewTotalIndexNode.dwTotalPathCount=dwMaxPathID;
	(*pdwTimeStamp) = NewTotalIndexNode.dwTimeStampCount;

	YGSetFilePointer(hNewTotalIndexFile,0,FILE_BEGIN,lFilePointer);

	if( FALSE == WriteFile(hNewTotalIndexFile,&NewTotalIndexNode,sizeof(NewTotalIndexNode),
		            &WrittenBytes,NULL) )
	{
		bError = TRUE;
	}

	CloseHandle(hCurrentIndexFile); 
	hCurrentIndexFile=NULL;

	CloseHandle(hNewTotalIndexFile);
	hNewTotalIndexFile=NULL;

	DeleteFile((LPCTSTR)strTotalIndexPath);
  
	// rename the file
  
	if(FALSE == MoveFile((LPCTSTR)strNewTotalIndexPath,(LPCTSTR)strTotalIndexPath) )
	{
		bError = TRUE;
	}

	if( bError )
	{
		return FALSE;
	}

	return TRUE;
}

/*===============================================================
 *  Function: GetHandleForCombine()
 *  Purpose:  While combining file, some file handle is needed
 *  
 *            
 *===============================================================*/
BOOL GetHandleForCombine(AM_FILE_HANDLE *pFileHandle, LPCTSTR TargetDirectory, LPCTSTR TargetFileName,DWORD *pdwErrorCode)
{
	CString strTargetDirectory;
	CString strTargetFileName;
	CString strDataFilePath;

	strTargetDirectory=TargetDirectory;
	strTargetFileName=TargetFileName;

	CString strTempDir;
	GetTempDir(strTempDir.GetBuffer(MAX_PATH));
	strTempDir.ReleaseBuffer();

	// open current index file
	CString strCurrentIndexFilePath;
	strCurrentIndexFilePath=strTempDir + L"\\" + strTargetFileName + L".id0";
	(*pFileHandle).hCurrentIndexFile=CreateFile((LPCTSTR)strCurrentIndexFilePath,
												GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,
												CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

	if( (*pFileHandle).hCurrentIndexFile== INVALID_HANDLE_VALUE)
	{
		(*pdwErrorCode)=GetLastError();
		return FALSE;
	}

	// open path array file
	CString strArrayFilePath;
	strArrayFilePath=strTempDir + L"\\" + strTargetFileName + L".ary";
	(*pFileHandle).hArrayFile=CreateFile((LPCTSTR)strArrayFilePath,
		GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	
	if( (*pFileHandle).hArrayFile == INVALID_HANDLE_VALUE)
	{
		(*pdwErrorCode)=GetLastError();
		return FALSE;
	}
	
	// open total index file
	CString strTotalIndexFilePath;
	strTotalIndexFilePath=strTempDir + L"\\" + strTargetFileName + AM_INDEX_SUFFIX;
	(*pFileHandle).hTotalIndexFile=CreateFile((LPCTSTR)strTotalIndexFilePath,
		GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	
	if( (*pFileHandle).hTotalIndexFile== INVALID_HANDLE_VALUE)
	{
		(*pdwErrorCode)=GetLastError();
		return FALSE;
	}
	
	// open stamp file
	CString strStampFilePath;
	strStampFilePath=strTempDir + L"\\" + strTargetFileName + L".stp";
	(*pFileHandle).hStampFile=CreateFile((LPCTSTR)strStampFilePath,
		GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	
	if( (*pFileHandle).hStampFile== INVALID_HANDLE_VALUE)
	{
		(*pdwErrorCode)=GetLastError();
		return FALSE;
	}
	
	(*pdwErrorCode)=0;
	return TRUE;
}


BOOL bIsFileChanged(CString SourceFilePath, HANDLE hTotalIndexFile, DWORD dwPathIndex)
{
	__int64 lFilePointer;
	DWORD dwReadBytes;
    
	YGSetFilePointer(hTotalIndexFile,0,FILE_BEGIN,lFilePointer);

	TOTAL_INDEX_NODE TotalIndexNode;
	ReadFile(hTotalIndexFile,&TotalIndexNode,sizeof(TotalIndexNode),
		     &dwReadBytes,NULL);

	DWORD dwTotalIndexOffset;
	DWORD dwOnePathID;
	
	dwOnePathID=dwPathIndex+1;
	
	dwTotalIndexOffset=GetTotalIndexOffset(TotalIndexNode.dwTotalPathCount,
		                                   TotalIndexNode.dwTimeStampCount,
										   dwOnePathID);
	
	YGSetFilePointer(hTotalIndexFile,dwTotalIndexOffset,FILE_BEGIN,lFilePointer);

	INDEX_DATA IndexData;
	ReadFile(hTotalIndexFile,&IndexData,sizeof(IndexData),
		     &dwReadBytes,NULL);

	// now we just compare the last stamp.
	// to judge it changed or not.

	WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile((LPCTSTR)SourceFilePath,
		                          &findFileData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
	  // return FALSE;
	}
      
	VERIFY(FindClose(hFind));

	
	if(
		(IndexData.FileInfo.nFileSizeHigh == findFileData.nFileSizeHigh) &&
	    (IndexData.FileInfo.nFileSizeLow == findFileData.nFileSizeLow) &&
	    (IndexData.FileInfo.ftLastWriteTime.dwHighDateTime == findFileData.ftLastWriteTime.dwHighDateTime) &&
		(IndexData.FileInfo.ftLastWriteTime.dwLowDateTime == findFileData.ftLastWriteTime.dwLowDateTime)
	  )
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}


/*=================================================================
 *
 * Function Name: GetImageSeperateIdentity()
 *
 * Purpose:       Form an identity, Write it to the data file.
 *                if a image is damaged.
 *                we can get some information from the identity.
 *                the identity is put in front of one file node.
 *
 *==================================================================*/

BOOL GetImageSeperateIdentity(LPCTSTR lpstrSourceFilePath,WCHAR * szImageSeperateIdentity)
{
	  for (size_t i = 0; i < MAX_PATH - 1; ++i)
	  {
		  szImageSeperateIdentity[i] = L'$';
	  }
	  szImageSeperateIdentity[MAX_PATH-1]=L'\0';
	  int nCopyStringBytes;
	  nCopyStringBytes=wcslen(lpstrSourceFilePath);
	  if(nCopyStringBytes >= MAX_PATH )
	  {
		  nCopyStringBytes = MAX_PATH-1;
	  }
	  wcsncpy(szImageSeperateIdentity,lpstrSourceFilePath,nCopyStringBytes);
	  return TRUE;
}


BOOL bIsFoundInMorePath(LPCTSTR pOneSubString,CStringArray &ExtraDirArray)
{
   long i,ArraySize;

   CString OneStringToFind(pOneSubString);

   CString OneStringInArray;

   BOOL ReturnValue;

   ReturnValue=FALSE;

   // ArraySize=StoreMorePathArray.GetSize();
   ArraySize=ExtraDirArray.GetSize();

   for(i=0;i<ArraySize;i++)
   {
	  OneStringInArray = ExtraDirArray.ElementAt(i);
	  
	  if(OneStringToFind.CompareNoCase(OneStringInArray) == 0)
	  {
		  ReturnValue=TRUE;
		  break;
	  }
   }
   return ReturnValue;
}

/*=========================================================
 * Function Name: GetNextFileNamePro
 *
 * Purpose: Each differential backup will create a new file
 *          this function generate a new file name. 
 *=========================================================*/

BOOL GetNextFileNamePro(LPCTSTR szTargetDirectory, LPCTSTR szTargetFileName,WCHAR *szDataFilePath,DWORD dwFileNumber)
{
	// if the use make several backup in one minute
	// then it needs an extra number.
	// the file name will begin with _002.amg
	
	CString strTargetDirectory;
	strTargetDirectory = szTargetDirectory;

	CString strTargetFileName;

	// Now we trim the extra date and time message from the strTargetFileName

	GetMainFileName(szTargetFileName,strTargetFileName.GetBuffer(MAX_PATH));
	// strTargetFileName = szTargetFileName;
	strTargetFileName.ReleaseBuffer();

    // add some time function
	// get the local time
	SYSTEMTIME OneSysTime;
	GetLocalTime(&OneSysTime);
	CString strShowTime;

	// strShowTime.Format(L"%.4d-%.2d-%.2d %.2d:%.2d:%.2d",OneSysTime.wYear,OneSysTime.wMonth,OneSysTime.wDay,OneSysTime.wHour,OneSysTime.wMinute,OneSysTime.wSecond);
	strShowTime.Format(L"%.4d%.2d%.2d_%.2d%.2d",OneSysTime.wYear,OneSysTime.wMonth,OneSysTime.wDay,OneSysTime.wHour,OneSysTime.wMinute);

	CString strFileNumber;
	strFileNumber.Format(L"f%.3d",dwFileNumber);

	// open target data file
	CString strDataFilePath;
  
	// strDataFilePath=strTargetDirectory + L"\\" + strTargetFileName + "_" + strShowTime + "_" + strFileNumber + AM_FILE_SUFFIX;
	strDataFilePath=strTargetDirectory + L"\\" + strTargetFileName + L"_" + strShowTime + L"_" + strFileNumber + AM_FILE_SUFFIX;

	if(strDataFilePath.GetLength()>=MAX_PATH)
	{
	    szDataFilePath[0]=L'\0';
		return FALSE;
	}
  
    // if this is an existing file
    // perhaps the user run backup twice in one minute

	// try to open the file.

	if(-1 == GetFileAttributes( LPCTSTR(strDataFilePath) ) )
	{
		// the file is not exist, but we still need to check
		// the file can be create or not.
		// if the file is not exist, but the program can not create the 
		// file, the original file may be in the CD-ROM
		// wcsncpy(szDataFilePath,(LPCTSTR)strDataFilePath,MAX_PATH-1);
		
		if( CheckOpenFile((LPCTSTR)strDataFilePath,szDataFilePath) )
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
	  // in this cases, this person do much snapshot backup
	  // in a minute

	  CString strTimeNumber;
	  int nTimeNumber;

	  for(nTimeNumber=2; nTimeNumber<99; nTimeNumber++)
	  {
	     // L"t" means this is division for time (in one minutes multi backup set)
		 strTimeNumber.Format(L"t%.3d",nTimeNumber);
   	  
	     // strDataFilePath=strTargetDirectory + L"\\" + strTargetFileName + "_" + strShowTime + "_" + strNumber + AM_FILE_SUFFIX;
		 strDataFilePath=strTargetDirectory + L"\\" + strTargetFileName + L"_" + strShowTime + L"_" + strFileNumber + L"_" + strTimeNumber + AM_FILE_SUFFIX;
		 
		 if(strDataFilePath.GetLength()>=MAX_PATH)
		 {
		 	 memset(szDataFilePath,0,1*sizeof(WCHAR));
			 TRACE(L"\nError in GetNextFileNamePro(), after combine the file name is too long.");
			 return FALSE;
		 }
	
	     if( -1 == GetFileAttributes((LPCTSTR)strDataFilePath) )
		 {
		    // found one avaiable file name.

			wcsncpy(szDataFilePath,(LPCTSTR)strDataFilePath,MAX_PATH-1);
			
			if( CheckOpenFile((LPCTSTR)strDataFilePath,szDataFilePath))
			{
			   return TRUE;
			}
			else
			{
				return FALSE;
			}
		    return TRUE;
		 }
	  }
	}

    // the string is empty.
	TRACE(L"\nCan not find one file name, do you make too much backup in one minutes ( around more than 100 )?");
	szDataFilePath[0]=L'\0';
	return FALSE;
}


/*=============================================================================
 *
 * Function Name: GetNextFileName()
 *
 * Purpose:       This function can get an avaiable file name
 *                based on the last file name.
 *                this function is used for snapshot backup
 *                which need a new file name,
 *                in the case of snapshot backup, the dwFileNubmer is 1
 *                It is a new total backup set, not a differential backup set.
 *                
 *=============================================================================*/

BOOL GetNextFileName(LPCTSTR szTargetFile, WCHAR *szDataFilePath,DWORD dwFileNumber)
{
	WCHAR szTargetDir[MAX_PATH];
	szTargetDir[0]=L'\0';
	GetLeftPart(szTargetFile,szTargetDir);

	WCHAR szMainFileName[MAX_PATH];
	szMainFileName[0]=L'\0';
	GetLongMainName(szTargetFile,szMainFileName);

	if( FALSE == GetNextFileNamePro(szTargetDir,szMainFileName,szDataFilePath,dwFileNumber) )
	{
		return FALSE;
	}

	return TRUE;
}


BOOL StoreDirectoryInfo(LPCTSTR lpstrDirectoryPath,
						AM_FILE_HANDLE *pFileHandle,
						DWORD *pdwPathID,
						DWORD *pdwErrorCode)
{
	__int64 lFilePointer;
	CString DirectoryPath;
	DirectoryPath=lpstrDirectoryPath;

	if(DirectoryPath.GetLength()==2)
	{
		  // if it is a Drive letter, do not store the information.
		(*pdwErrorCode)=0;
		return TRUE;
	}

	if(DirectoryPath.GetLength() > MAX_PATH-1)
	{
		(*pdwErrorCode)=FILE_PATH_TOO_LONG;
		return FALSE;
	}

    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile((LPCTSTR)DirectoryPath,
		                            &findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		  // if we can not found the directory, 
		  // then we do not store the directory information.
		  // since first find file, then backup directroy,
		  // usually it can find the floder and will not return here.
		(*pdwErrorCode)=DIRECTORY_NOT_EXIST;
		  //-- for debug
		TRACE(L"Store Dir information error, dir not found.");
		  //-- end for debug
		return FALSE;
	}
      
	VERIFY(FindClose(hFind));
	  
	(*pdwPathID)++;

	DWORD WrittenBytes;
      
	// write current index file
	  
	INDEX_DATA CurrentIndexData;
	CurrentIndexData.dwPathID=(*pdwPathID); // dwPathID is started with 1

	// the first backup is a NEW version

	memset(&CurrentIndexData,0,sizeof(CurrentIndexData));

	CurrentIndexData.dwPathID=(*pdwPathID);
	CurrentIndexData.DataPointer.QuadPart=0;
    CurrentIndexData.wStatusFlag=DIRECTORY_NEW;
	  
	CurrentIndexData.FileInfo.ftLastWriteTime = findFileData.ftLastWriteTime;
	CurrentIndexData.FileInfo.ftCreationTime = findFileData.ftCreationTime;
	CurrentIndexData.FileInfo.ftLastAccessTime = findFileData.ftLastAccessTime ;
	  
	CurrentIndexData.FileInfo.dwFileAttributes = findFileData.dwFileAttributes & ~FILE_ATTRIBUTE_NORMAL;
	CurrentIndexData.FileInfo.dwReservedWord1=0x49494949;

    WriteFile(pFileHandle->hCurrentIndexFile,&CurrentIndexData,
	        sizeof(CurrentIndexData),&WrittenBytes,NULL);

	DWORD dwErrorNumber;
	dwErrorNumber=GetLastError();

	// it seems that have error code doesn't necessary mean it has error here.
	if( ( sizeof(CurrentIndexData) != WrittenBytes) && (dwErrorNumber!=0) )
	{
		(*pdwErrorCode)=dwErrorNumber;
        //-- for debug
        TRACE(L"Error When Store Directory Info: " + DirectoryPath);
		TRACE(L"  Error code: %d\n",dwErrorNumber);
        //-- end for debug

		//-- for debug temp comment
		// return FALSE;
		//-- end for debug temp comment
	}

	  // write array data file

	ARRAY_DATA ArrayData;
	memset(&ArrayData,0,sizeof(ArrayData));
	ArrayData.dwFileAttribute=findFileData.dwFileAttributes;
	ArrayData.dwPathID=(* pdwPathID);
	wcscpy(ArrayData.szFilePath,LPCTSTR(DirectoryPath));

	YGSetFilePointer(pFileHandle->hArrayFile,0,FILE_END,lFilePointer);
	WriteFile(pFileHandle->hArrayFile,&ArrayData,
		        sizeof(ArrayData),&WrittenBytes,NULL);

	(*pdwErrorCode)=0;
	return TRUE;
}


						/*========================================================================
 *
 * Function Name:  CheckOpenFile()
 *
 * Purpose:        When we do differential backup  
 *                 we need a new target file name.
 *                 The program will create one automatically
 *                 if the program can not create one automatically
 *                 the program will ask the user to choose one file name
 *
 * Note:           before the function, we have ensure the file name is
 *                 not exist so we needn't ask for overwrite now.
 *
 *=======================================================================*/

BOOL CheckOpenFile(LPCTSTR szOriginalFileName,WCHAR *szResultFileName)
{
	///*
	// this fucntion is called in GetNextFileNamePro.
	// this function return a file name that original not exist
	TRACE(L"\nEnter function CheckOpenFile.");

	szResultFileName[0]=L'\0';

	if( -1 != GetFileAttributes( szOriginalFileName ) )
	{
		TRACE(L"\nThe target file already exists, CheckOpenFile error.");
		return FALSE;
	}
	else
	{
		TRACE(L"\nThe originale file name is:%s",szOriginalFileName);
	}

	HANDLE hTargetFile;
	
	hTargetFile=CreateFile( szOriginalFileName,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,
		                    NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	
	if( INVALID_HANDLE_VALUE != hTargetFile )
	{
		// if the file can be created.
		CloseHandle(hTargetFile);
		DeleteFile(szOriginalFileName);
		TRACE(L"\nDelete new open file:%s",szOriginalFileName);
		wcsncpy(szResultFileName,szOriginalFileName,MAX_PATH-1);
		return TRUE;
	}
	else // if ( INVALID_HANDLE_VALUE == hTargetFile )
	{
		// now open the differential backup image manually

		AfxMessageBox(IDS_OPEN_DIFF_IMAGE,MB_OK|MB_ICONINFORMATION,NULL);
		WCHAR szFileName[MAX_PATH];

		BOOL bContinueSelect;
		bContinueSelect=TRUE;
		
		while(bContinueSelect)
		{
			// if (SelectFile(L"AMG","AM01 Image File",szFileName)==TRUE)
			BOOL bOpenFile;
			bOpenFile = FALSE; // L"save as" dialog
			if(SelectFilePro(L"AMG",L"AM01 Image File",szOriginalFileName,szFileName,bOpenFile) )
			{
				// if user do not press cancel
				if( -1 != GetFileAttributes(szFileName) )
				{
					AfxMessageBox(IDS_FILE_ALREADY_EXIST,MB_OK|MB_ICONINFORMATION,NULL);
					continue;
				}

				hTargetFile=CreateFile( szFileName,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,
										NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
				
				if( INVALID_HANDLE_VALUE != hTargetFile )
				{
					// if the file can be created.
					CloseHandle(hTargetFile);
					DeleteFile(szFileName);
					TRACE(L"\nDelete new open file:%s", szFileName);
					wcsncpy(szResultFileName,szFileName,MAX_PATH -1);
					bContinueSelect=FALSE;
					return TRUE;
				}
				// contiue to loop.
			}
			else
			{
				// if user press cancel
				bContinueSelect=FALSE;
				return FALSE; 
			}
		}

		return FALSE;
	}
	// */

	// for debug
	return TRUE;
}

// test@263.net
