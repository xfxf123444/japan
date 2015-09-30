#include "AMSqueeze.h"
#include "AMFunction.h"
#include "resource.h"
#include "..\..\..\davinci\Davinci_tech\zlib\Cur_ver\Export\zlib.h"

SQUEEZE_STATE g_SqueezeState;

/*======================================================================
 *
 * Function Name: CopyOneFile()
 *
 * Purpose:       While squeeze file, copy one file
 *
 * note:          not like restoring file, if the function fail
 *                we should not continue copying the left image data
 *                since the source image is not fine in this case
 *
 *======================================================================*/

BOOL CopyOneFile( CURRENT_SQUEEZE_INFO   *pCurrentSqueezeInfo,    // here contain current source number and current handle, current target is always the last element of TargetArray
				  AM_FILE_HANDLE		 *pFileHandle,
				  BYTE                   *CopyString,      // compressed buffer
				  const CDATA_ARRAY      &SourceArray,     // Source Array, read only
				  CDATA_ARRAY            &TargetArray,     // Target Array, may be changed
				  IMAGE_HEAD			 *pImageHead
				  )
{

   // the user may cancel squeeze when needed
   // but if he canceled squeeze we should clear the searil file
   // that he created.

   if(g_SqueezeState.bSqueezeCancel)
   {
	   TRACE(L"\ng_SqueezeState.bSqueezeCancel is true in CopyOneFile, return.");
	   return FALSE;
   }
   
   unsigned __int64 qwBytesLeft;
   
   qwBytesLeft=pCurrentSqueezeInfo->qwFileSize;

   DATA_NODE DataNode;

   memset(&DataNode,0,sizeof(DataNode));

   // qwBytesLeft value come from index data


	while(qwBytesLeft>0)
	{
		if( FALSE == ReadDataFile(&(pCurrentSqueezeInfo->RestoreInfo),SourceArray,&DataNode,sizeof(DataNode)) )
		{
			TRACE(L"\nReadDataFile error in CopyLastTimeStamp");
			return FALSE;
		}	

		DWORD dwRequestWrite;
	  
		dwRequestWrite = sizeof(DataNode);

		if( FALSE == WriteDataFile(pFileHandle->hCurrentTarget,&DataNode,sizeof(DataNode),pCurrentSqueezeInfo->qwCurrentargetSize,TargetArray,pImageHead,NULL) )
		{
			TRACE(L"\nWriteDataFile error in CopyOneFile.");
			return FALSE;
		}
	  
		if( DataNode.dwPacketSize > AM_COMPRESS_BUFFER)
		{
			TRACE(L"Data File Error, Packet Size too large.");
			return FALSE;
		}

		if( FALSE == ReadDataFile( &(pCurrentSqueezeInfo->RestoreInfo),SourceArray,CopyString,(DataNode.dwPacketSize)))
		{
			TRACE(L"\nReadDataFile error in CopyLastTimeStamp 2");
			return FALSE;
		}

		if( FALSE == WriteDataFile(pFileHandle->hCurrentTarget,CopyString,DataNode.dwPacketSize,
			                        pCurrentSqueezeInfo->qwCurrentargetSize,TargetArray,pImageHead,NULL))
		{
			TRACE(L"\nWriteDataFile error in CopyOneFile, for none compress file.");
			return FALSE;
		}

		qwBytesLeft = qwBytesLeft - DataNode.dwOrigSize;
		// in the while loop, allow the user to break;
		if( g_SqueezeState.bSqueezeCancel )
		{
			TRACE(L"\ng_SqueezeState.bSqueezeCancel is true while copy one file, reutrn");
			return FALSE;
		}
	} // end while ( BytesLeft > 0 )

	return TRUE;
}


/*============================================================
 *
 * Function Name: CopyLastTimeStamp()
 *
 * Purpose:       To squeeze original image, copy the last
 *                timestamp data to new image file
 *
 *===========================================================*/
BOOL CopyLastTimeStamp(LPCTSTR szSourceImage, LPCTSTR szTargetImage)
{
	__int64 lFilePointer;	
	// first check the source file is the last image of amg file.
	DWORD dwErrorCode;
	if( FALSE == CheckLastImage(szSourceImage,&dwErrorCode) )
	{
		TRACE(L"\nCheckLastImage error in CopyLastTimeStamp");
		return FALSE;
	}
    
	CDATA_ARRAY SourceArray;

	if( FALSE == GetImageArray(SourceArray,szSourceImage) )
	{
		TRACE(L"\nGetImageArray error in CopyLastTimeStamp");
		return FALSE;
	}

	CDATA_ARRAY TargetArray;

	AddOneTargetElement(TargetArray,szTargetImage);

	SQUEEZE_FILE_HANDLE SqueezeFileHandle;

	memset(&SqueezeFileHandle,0,sizeof(SqueezeFileHandle));

	// the source handle we get is the last file handle of image file.

	// if the source and the target have the same main name
	// we adjust it so that they will be different in temp dir
    
	WCHAR szSourceMainName[MAX_PATH];
	memset(szSourceMainName,0,sizeof(szSourceMainName));

	if( FALSE == AdjustMainName(szSourceImage,szTargetImage,szSourceMainName) )
	{
		TRACE(L"\nAdjustMainName error in CopyLastTimeStamp.");
		return FALSE;
	}

	// this will be used in the futrue
	// when clear the extra file
	WCHAR szTargetMainName[MAX_PATH];
	memset(szTargetMainName,0,sizeof(szTargetMainName));
	GetLongMainName(szTargetImage,szTargetMainName);

	if( FALSE == GetSqueezeFileHandle(szSourceImage,szSourceMainName,SqueezeFileHandle) )
	{
		TRACE(L"\nGetSequeezeFileHandle error in CopyLastTimeStamp.");
		return FALSE;
	}

	// AM_FILE_HANDLE is used by BackupSelection first.
	AM_FILE_HANDLE FileHandle;
	memset(&FileHandle,0,sizeof(FileHandle));
	
	if( FALSE == GetFileHandle(&FileHandle,szTargetImage,&dwErrorCode) )
	{
		TRACE(L"\nGetFileHandle Error in CopyLastTimeStamp.");
	}

	IMAGE_HEAD ImageHead;
	memset(&ImageHead,0,sizeof(ImageHead));
	
	DWORD dwReadBytes;
 	dwReadBytes = 0;
	
	DWORD dwWrittenBytes;
	dwWrittenBytes = 0;

	// record the image head
	ReadFile(SqueezeFileHandle.hSourceImage,&ImageHead,sizeof(ImageHead),&dwReadBytes,NULL);
	
	// wait for progress
	WriteSqueezeHead(&FileHandle,&ImageHead,TargetArray);

	DWORD dwStampCount;
	if( FALSE == GetStampCount(SqueezeFileHandle.hSourceImage,&dwStampCount) )
	{
		TRACE(L"\nGetStampCount error in CopyLastTimeStamp.");
	}

	// for debug
	// TRACE(L"\nStampcount is:%d",dwStampCount);

	STAMP_DATA StampData;
	if( FALSE == GetStampInfo(SqueezeFileHandle.hSourceImage,dwStampCount,StampData))
	{
		TRACE(L"\nGetSTampInfo error in CopyLastTimeStamp.");
	}

	WriteFile(FileHandle.hStampFile,&StampData,sizeof(StampData),&dwWrittenBytes,NULL);

	TOTAL_INDEX_NODE TotalIndexNode;
	memset(&TotalIndexNode,0,sizeof(TotalIndexNode));
	ReadFile(SqueezeFileHandle.hOldTotalIndex,&TotalIndexNode,sizeof(TotalIndexNode),
		     &dwReadBytes,NULL);

    // Fill the CurrentInfo information
	// CurrentInfo is used to help switching between different files.

	// CURRENT_RESTORE_INFO CurrentInfo;
	// memset(&CurrentInfo,0,sizeof(CurrentInfo));
	CURRENT_SQUEEZE_INFO CurrentSqueezeInfo;
	memset(&CurrentSqueezeInfo,0,sizeof(CurrentSqueezeInfo));
	
	// SourceArray have an empty data in index 0
	CurrentSqueezeInfo.RestoreInfo.dwCurrentSource  = SourceArray.GetUpperBound();
	CurrentSqueezeInfo.RestoreInfo.hCurrentImage = SqueezeFileHandle.hSourceImage ;
	
	GetOneFileSize(FileHandle.hCurrentTarget,&(CurrentSqueezeInfo.qwCurrentargetSize));

	DWORD dwCurrentPathID;

	BYTE * CopyString;
	CopyString = (BYTE *)malloc(AM_COMPRESS_BUFFER);

	if( CopyString == NULL )
	{
	  AfxMessageBox(IDS_NOT_ENOUGH_MEMORY,MB_OK|MB_ICONINFORMATION,NULL);
	  return FALSE;
	}

	BYTE *pDataBuffer;
	pDataBuffer = (BYTE *)malloc(AM_BUFFER_SIZE);

	if(NULL == pDataBuffer)
	{
	   AfxMessageBox(IDS_NOT_ENOUGH_MEMORY,MB_OK|MB_ICONINFORMATION,NULL);
	   free(CopyString);
	   CopyString = NULL;
	   return FALSE;
	}

	DWORD dwNewPathID;
	dwNewPathID = 0;

    ARRAY_DATA ArrayData;
	YGSetFilePointer(SqueezeFileHandle.hOldArrayFile,sizeof(ARRAY_NODE),FILE_BEGIN,lFilePointer);

	for(dwCurrentPathID=1; dwCurrentPathID<=TotalIndexNode.dwTotalPathCount ; dwCurrentPathID++ )
	{
	   // for progress bar
	   g_SqueezeState.dwPercent = dwCurrentPathID * 100 / (TotalIndexNode.dwTotalPathCount);

	   if(g_SqueezeState.bSqueezeCancel)
	   {
		   free(CopyString);
		   CopyString=NULL;

		   free(pDataBuffer);
		   pDataBuffer=NULL;

		   CancelSqueezeImage(SqueezeFileHandle,&FileHandle,TargetArray,szSourceMainName,szTargetMainName);
	   }

	   INDEX_DATA IndexData;
	   memset(&IndexData,0,sizeof(IndexData));

	   // read index file data
	   GetIndexData(SqueezeFileHandle.hOldTotalIndex,dwCurrentPathID,TotalIndexNode.dwTimeStampCount,IndexData);
	   
	   // read arry file data
	   DWORD dwReadBytes;
	   ReadFile(SqueezeFileHandle.hOldArrayFile,&ArrayData,sizeof(ArrayData),&dwReadBytes,NULL);

	   // for progress bar
	   wcsncpy(g_SqueezeState.szCurrentFile,ArrayData.szFilePath,MAX_PATH-1);

	   if( 0 == IndexData.wStatusFlag)
	   {
		   // if it is not exist in the last time stamp.
		   continue;
	   }
	   else
	   {
		   // if exist this path ID in the last timestamp
		   dwNewPathID ++ ;

		   if(
			   FILE_NEW == IndexData.wStatusFlag ||
			   FILE_CHANGE == IndexData.wStatusFlag ||
			   FILE_NO_CHANGE == IndexData.wStatusFlag
			 )
		   {
			   // if this is a file and the file exist in the last timestamp

			   GetDataFilePointer( 
								   SqueezeFileHandle.hOldTotalIndex,
								   TotalIndexNode.dwTotalPathCount,
								   dwCurrentPathID,
								   TotalIndexNode.dwTimeStampCount,
								   &IndexData );

			   if(IndexData.dwFileNumber  != CurrentSqueezeInfo.RestoreInfo.dwCurrentSource )
			   {
				   if(GetRequiredHandle(&(CurrentSqueezeInfo.RestoreInfo),IndexData.dwFileNumber ,SourceArray)==FALSE)
				   {
					   // then the file can not be resored
					   // we delete the partially restored file
					   TRACE(L"\nGetRequiredHandle error in CopyLastTimeStamp.");
					   free(CopyString);
					   CopyString=NULL;

					   free(pDataBuffer);
					   pDataBuffer=NULL;

					   CloseSqueezeFileHandle(SqueezeFileHandle);
					   return FALSE;
				   }

			   }

			   YGSetFilePointer( CurrentSqueezeInfo.RestoreInfo.hCurrentImage,
							   IndexData.DataPointer.QuadPart,
							   FILE_BEGIN,lFilePointer);

			   ULARGE_INTEGER qwFileLength;
			   qwFileLength.HighPart=IndexData.FileInfo.nFileSizeHigh;
			   qwFileLength.LowPart=IndexData.FileInfo.nFileSizeLow;

			   ULARGE_INTEGER qwOneFileSize;
			   qwOneFileSize.HighPart= IndexData.FileInfo.nFileSizeHigh;
			   qwOneFileSize.LowPart = IndexData.FileInfo.nFileSizeLow;
			   CurrentSqueezeInfo.qwFileSize = qwOneFileSize.QuadPart;

			   // note:
			   // CurrentSqueezeInfo.qwCurrentargetSize will be changed in CopyOneFile
			   // CurrentSqueezeInfo.qwFileSize is a const to judge copy how much data
			   // CurrentSqueezeInfo.RestoreInfo is used to avoid open and close source file every time

  			   // afte CopyOneFile the TagetArray size may be changed.
			   // IndexData.dwFileNumber recored the beginning file number
			   // of one file. and DataFilePointer is the beginning place


				YGSetFilePointer(FileHandle.hCurrentTarget,0,FILE_CURRENT,lFilePointer);

			   // modify the index data

			   IndexData.wStatusFlag = FILE_NEW;
			   IndexData.dwFileNumber = TargetArray.GetUpperBound();
			   IndexData.dwPathID = dwNewPathID;
			   IndexData.DataPointer.QuadPart = lFilePointer;

			   if( FALSE == CopyOneFile(&CurrentSqueezeInfo,&FileHandle,CopyString,SourceArray,TargetArray,&ImageHead) )
			   {
					TRACE(L"\nCopyOneFile error in CopyLastTimeStamp");
					free(CopyString);
					CopyString=NULL;

					free(pDataBuffer);
					pDataBuffer=NULL;

		            CancelSqueezeImage(SqueezeFileHandle,&FileHandle,TargetArray,szSourceMainName,szTargetMainName);

					return FALSE;
			   }


			   //modify the array data
			   ArrayData.dwPathID = dwNewPathID;
			   
			   DWORD dwWrittenBytes;
			   if( FALSE == WriteFile(FileHandle.hCurrentIndexFile,&IndexData,sizeof(IndexData),&dwWrittenBytes,NULL) )
			   {
				   TRACE(L"\nWriteFile error in CopyLastTimeStamp, write CurrentIndexFile.");
				   // here should return FALSE ( wait for consideration )
			   }

			   WriteFile(FileHandle.hArrayFile,&ArrayData,sizeof(ArrayData),&dwWrittenBytes,NULL);

		   }
		   else
		   {
			   // if this is a directory
			   IndexData.wStatusFlag = DIRECTORY_NEW;
			   IndexData.dwPathID = dwNewPathID;

			   //modify the array data
			   ArrayData.dwPathID = dwNewPathID;
			   
			   DWORD dwWrittenBytes;
			   if( FALSE == WriteFile(FileHandle.hCurrentIndexFile,&IndexData,sizeof(IndexData),&dwWrittenBytes,NULL) )
			   {
				   TRACE(L"\nWriteFile error in CopyLastTimeStamp, write CurrentIndexFile.");
				   // here should return FALSE ( wait for consideration )
			   }

			   WriteFile(FileHandle.hArrayFile,&ArrayData,sizeof(ArrayData),&dwWrittenBytes,NULL);
		   } 

	   } // end if the PathID exist in the last timestamp

	} // end for loop

    free(CopyString);
    CopyString=NULL;

    free(pDataBuffer);
    pDataBuffer=NULL;

	CloseSqueezeFileHandle(SqueezeFileHandle);

	if( FALSE == AdjustIndexHead(FileHandle.hCurrentIndexFile,dwNewPathID))
	{
		TRACE(L"\nAdjustIndexHead error in CopyLastTimeStamp.");
	}

	if( FALSE == CurrentIdxToTotalIdx(&FileHandle) )
	{
		TRACE(L"\nCurrentIdxToTotalIdx error in CopyLastTimeStamp.");
	}
	
    // CloseFileHandle will be place in CombineFileToOneImg
	CombineFileToOneImg(&FileHandle,TargetArray);
	CloseFileHandle(&FileHandle);

    RecordFileTable(TargetArray);
	//RecordLastImageInfo(&FileHandle,TargetArray);
    WriteSignature( TargetArray[TargetArray.GetUpperBound()].szFileName, TargetArray );

    if( CreateDirFilePro(TargetArray) == FALSE )
	{
	  TRACE(L"\n Create Dir File Error.");
	  // in the future we should add some function
	  // to handle Error situation
	}

    ClearExtraFile(szSourceMainName);

	ClearExtraFile(szTargetMainName);
  
    return TRUE;
}


/*==================================================================
 *
 * Function Name: SqueezeImage
 * Purpose:       Make the image that have multi time stamp to one 
 *                Timestamp ( the newest timestamp ).
 *
 *
 *==================================================================*/

BOOL SqueezeImage(LPCTSTR szSourceImage, LPCTSTR szTargetImage)
{

	// to avoid the user overwrite the source file.
	if( 0 == wcscmp(szSourceImage,szTargetImage))
	{
		TRACE(L"\nThe source and the target image can not be the same.");
		return FALSE;
	}

	// do some preparation
	// before copying file, first get temporarily file
    memset(&g_SqueezeState,0,sizeof(g_SqueezeState));
	g_SqueezeState.bSqueezeCancel = FALSE;

	WCHAR szTempDir[MAX_PATH];
	if( FALSE == GetTempDir(szTempDir) )
	{
		TRACE(L"\nGetTempDir error in SqueezeImage.");
		return FALSE;
	}

	WCHAR szSourceMainName[MAX_PATH];
	
	if( FALSE == AdjustMainName(szSourceImage,szTargetImage,szSourceMainName) )
	{
		TRACE(L"\nAdjustMainName error in SqueezeImage.");
		return FALSE;
	}

	if( FALSE == GetSeperateFile(szSourceImage,szSourceMainName,szTempDir) )
	{
		TRACE(L"\nGetSeperateFile error in SqueezeImage.");
		return FALSE;
	}

	if( FALSE == CopyLastTimeStamp(szSourceImage,szTargetImage) )
	{
		TRACE(L"\nCopyLastTimeStamp error in SqueezeImage.");
		return FALSE;
	}

	return TRUE;

}

void SetSqueezeCancel()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	g_SqueezeState.bSqueezeCancel=TRUE;
}

void GetSqueezeState(SQUEEZE_STATE *pSqueezeState)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	(*pSqueezeState) = g_SqueezeState;
}


/*===================================================================
 *
 *  Function Name: AdjustMainName()
 *
 *  Purpose:       when the source main name and the target main
 *                 name are the same, adjust source main name
 *                 then total index file and array file 
 *                 will be different in the temp dir.
 *
 *===================================================================*/
BOOL AdjustMainName(LPCTSTR szSourceImage,LPCTSTR szTargetImage,WCHAR *szSourceMainName)
{

	WCHAR szOldSourceMain[MAX_PATH];
	if( FALSE == GetLongMainName(szSourceImage,szOldSourceMain) )
	{
		TRACE(L"\nGetLongMainName error in AdjustMainname,get source main name.");
		return FALSE;
	}
	
	WCHAR szTargetMainName[MAX_PATH];
	if( FALSE == GetLongMainName(szTargetImage,szTargetMainName) )
	{
		TRACE(L"\nGetLongMainName error in AdjustMainName,get target main name.");
		return FALSE;
	}

	if(0 == wcscmp(szOldSourceMain,szTargetMainName))
	{
		wcscat(szOldSourceMain,L"_src");
	}
	
	wcsncpy(szSourceMainName,szOldSourceMain,MAX_PATH-1);
	return TRUE;
}


/*==================================================================
 *
 *  Function name: GetSqueezeFielHandle
 *  Purpose:       Get required handle to squeeze file
 *                 the value is return in SqueezeFileHandle 
 *                 ( pass by reference)
 *  note:          Total index file and stamp file should be 
 *                 prepared before invoking this function
 *
 *==================================================================*/

BOOL GetSqueezeFileHandle(LPCTSTR szSourceImage, LPCTSTR szSourceMainName, SQUEEZE_FILE_HANDLE &SqueezeFileHandle)
{

	memset(&SqueezeFileHandle,0,sizeof(SqueezeFileHandle));
	
	// if( FALSE == PrepareOnePath(szTargetImage) )
	// {
		// TRACE(L"\nPrepareOnePath error in GetSqueezeFileHandle.");
		// return FALSE;
	// }

	SqueezeFileHandle.hSourceImage = CreateFile(szSourceImage,GENERIC_READ,FILE_SHARE_READ,
		                                        NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if( INVALID_HANDLE_VALUE == SqueezeFileHandle.hSourceImage )
	{
		TRACE(L"\nUnable to open Source Image file.");
		SqueezeFileHandle.hSourceImage=NULL;
		return FALSE;
	}


	CString strTotalIndexFile;
	
	WCHAR szTempDir[MAX_PATH];
	GetTempDir(szTempDir);
	
	strTotalIndexFile = strTotalIndexFile + szTempDir + L"\\" + szSourceMainName + AM_INDEX_SUFFIX;

	SqueezeFileHandle.hOldTotalIndex = CreateFile( (LPCTSTR)strTotalIndexFile,GENERIC_READ,FILE_SHARE_READ,
		                                             NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if( INVALID_HANDLE_VALUE == SqueezeFileHandle.hOldTotalIndex )
	{
		TRACE(L"\nOpen total index file error.");
		
		CloseHandle(SqueezeFileHandle.hSourceImage);
		SqueezeFileHandle.hSourceImage=NULL;
		
		return FALSE;
	}


	CString strOldArrayFile;
	strOldArrayFile = strOldArrayFile + szTempDir + L"\\" + szSourceMainName + AM_ARRAY_SUFFIX;

	SqueezeFileHandle.hOldArrayFile = CreateFile( (LPCTSTR)strOldArrayFile,GENERIC_READ,FILE_SHARE_READ,
		                                             NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if( INVALID_HANDLE_VALUE == SqueezeFileHandle.hOldTotalIndex )
	{
		TRACE(L"\nOpen array file error.");
		
		CloseHandle(SqueezeFileHandle.hSourceImage);
		SqueezeFileHandle.hSourceImage=NULL;

		CloseHandle(SqueezeFileHandle.hOldTotalIndex);
		SqueezeFileHandle.hOldTotalIndex = NULL;
		
		return FALSE;
	}

	return TRUE;
}

BOOL CloseSqueezeFileHandle(SQUEEZE_FILE_HANDLE &SqueezeFileHandle)
{
	CloseHandle(SqueezeFileHandle.hSourceImage);
	SqueezeFileHandle.hSourceImage=NULL;

	CloseHandle(SqueezeFileHandle.hOldArrayFile);
    SqueezeFileHandle.hOldArrayFile  = NULL;
	
	CloseHandle(SqueezeFileHandle.hOldTotalIndex);
	SqueezeFileHandle.hOldTotalIndex = NULL;
	return TRUE;
}


// this function is come from writefilehead
BOOL WriteSqueezeHead(AM_FILE_HANDLE *pFileHandle, IMAGE_HEAD *pOriginalHead,CDATA_ARRAY &TargetArray)
{

  DWORD WrittenBytes;

  //-- write destination index file
  //-- path number

  // CurrentIndexNode.dwPathCount will be got after we
  // passed all the files.

  // write current index node
  CURRENT_INDEX_NODE CurrentIndexNode;
  memset(&CurrentIndexNode,0,sizeof(CurrentIndexNode));
  CurrentIndexNode.dwPathCount=0;
  
  if( FALSE == WriteFile(pFileHandle->hCurrentIndexFile,&CurrentIndexNode,sizeof(CurrentIndexNode),
               &WrittenBytes,NULL) )
  {
	  TRACE(L"\nWriteFile error in WriteSqueezeHead, write CurrentIndexFile.");
  }

  // write array node
  ARRAY_NODE ArrayNode;
  memset(&ArrayNode,0,sizeof(ArrayNode));
  WriteFile(pFileHandle->hArrayFile,&ArrayNode,sizeof(ArrayNode),
	        &WrittenBytes,NULL);

  // write stamp node
  STAMP_NODE StampNode;
  memset(&StampNode,0,sizeof(StampNode));
  WriteFile(pFileHandle->hStampFile,&StampNode,sizeof(StampNode),
	        &WrittenBytes,NULL);
  
  // write image head
  IMAGE_HEAD ImageHead;
  
  // first use zero to fill the head area
  memset(&ImageHead,0,sizeof(ImageHead));

  wcscpy(ImageHead.szImageIdentity,IMAGE_IDENTITY);

  ImageHead.dwFileVersion=pOriginalHead->dwFileVersion;
  ImageHead.wBackupType=pOriginalHead->wBackupType;

  // 2004.05.21 for encrypt begin
  ImageHead.bEncrypt = pOriginalHead->bEncrypt;
  if(pOriginalHead->bEncrypt)
  {
	  wcsncpy(ImageHead.pchPassword,pOriginalHead->pchPassword,PASSWORD_SIZE);
  }
  // 2004.05.21 for encrypt end

  ImageHead.dwCompressLevel=pOriginalHead->dwCompressLevel;

  ImageHead.dwCheckSum=0; // now have no check sum
  
  // Comment Max is IMAGE_COMMENT_SIZE
  wcsncpy(ImageHead.szComment,pOriginalHead->szComment, IMAGE_COMMENT_SIZE-1  );

  ImageHead.TotalIndexPointer.QuadPart=0;
 
  ImageHead.ArrayPointer.QuadPart=0;
 
  ImageHead.StampPointer.QuadPart=0;
 
  wcsncpy(ImageHead.szSerialNumber,
	      TargetArray[1].szSerialNumber,
		  IMAGE_SERIALNUMBER_SIZE-1);

  ImageHead.dwFileNumber=1;
  // if there is the last file, this will be -1;
  // ImageHead.dwFileNumber=LAST_FILE_NUMBER;

  WriteFile(pFileHandle->hCurrentTarget,&ImageHead,sizeof(ImageHead),&WrittenBytes,NULL);

  return TRUE;
}

/*==================================================================
 *
 * Function Name: GetStampCount()
 * Purpose:       Get toal stamp count
 * 
 *
 *==================================================================*/

BOOL GetStampCount(HANDLE hImageFile, DWORD *pdwStampCount)
{
	__int64 lFilePointer,OrigFilePointer;

	// record the old file pointer
	YGSetFilePointer(hImageFile,0,FILE_CURRENT,OrigFilePointer);

	YGSetFilePointer(hImageFile,0,FILE_BEGIN,lFilePointer);

	IMAGE_HEAD ImageHead;
	DWORD dwReadBytes;
	ReadFile(hImageFile,&ImageHead,sizeof(ImageHead),&dwReadBytes,NULL);

	TOTAL_INDEX_NODE TotalIndexNode;

	YGSetFilePointer(hImageFile,ImageHead.TotalIndexPointer.QuadPart,FILE_BEGIN,lFilePointer);
	
	if ( FALSE == ReadFile(hImageFile,&TotalIndexNode,sizeof(TotalIndexNode),&dwReadBytes,NULL) )
	{
		TRACE(L"\nReadFile error in GetStampCount.");
		(*pdwStampCount)=0;
		YGSetFilePointer(hImageFile,OrigFilePointer,FILE_BEGIN,lFilePointer);
		return FALSE;
	}

	(*pdwStampCount)=TotalIndexNode.dwTimeStampCount;
	YGSetFilePointer(hImageFile,OrigFilePointer,FILE_BEGIN,lFilePointer);

	return TRUE;
}

/*==================================================================
 *
 * Function Name: GetStampInfo()
 * Purpose:       give a StampNumber ,get stamp info
 * 
 *
 *==================================================================*/

BOOL GetStampInfo(HANDLE hImageFile,DWORD dwStampNumber,STAMP_DATA &StampData)
{
	__int64 lFilePointer,OrigFilePointer;

	// record the old file pointer
	YGSetFilePointer(hImageFile,0,FILE_CURRENT,OrigFilePointer);

	YGSetFilePointer(hImageFile,0,FILE_BEGIN,lFilePointer);

	IMAGE_HEAD ImageHead;
	DWORD dwReadBytes;
	ReadFile(hImageFile,&ImageHead,sizeof(ImageHead),
		     &dwReadBytes,NULL);

	TOTAL_INDEX_NODE TotalIndexNode;

	YGSetFilePointer(hImageFile,ImageHead.TotalIndexPointer.QuadPart,FILE_BEGIN,lFilePointer);
	ReadFile(hImageFile,&TotalIndexNode,sizeof(TotalIndexNode),&dwReadBytes,NULL);

	YGSetFilePointer(hImageFile,ImageHead.StampPointer.QuadPart,FILE_BEGIN,lFilePointer);

	DWORD dwStampOffset;
	dwStampOffset = sizeof(STAMP_NODE)+sizeof(StampData)*(dwStampNumber-1);
	
	YGSetFilePointer(hImageFile,dwStampOffset,FILE_CURRENT,lFilePointer);

    if( FALSE == ReadFile(hImageFile,&StampData,sizeof(StampData),&dwReadBytes,NULL) )
	{
		TRACE(L"\nReadFile error in GetStampInfo.");
		memset(&StampData,0,sizeof(StampData));
	    YGSetFilePointer(hImageFile,OrigFilePointer,FILE_BEGIN,lFilePointer);
		return FALSE;
	}

	// restore the old file pointer.
    YGSetFilePointer(hImageFile,OrigFilePointer,FILE_BEGIN,lFilePointer);

	return TRUE;
}


BOOL  CancelSqueezeImage( SQUEEZE_FILE_HANDLE  &SqueezeFileHandle,
						  AM_FILE_HANDLE       *pFileHandle,
						  const CDATA_ARRAY    &TargetArray,
						  LPCTSTR              szSourceMainName,
						  LPCTSTR              szTargetMainName)
{
    CloseHandle(SqueezeFileHandle.hOldArrayFile);
	CloseHandle(SqueezeFileHandle.hOldTotalIndex);
	CloseHandle(SqueezeFileHandle.hSourceImage);
	memset(&SqueezeFileHandle,0,sizeof(SqueezeFileHandle));

	CloseHandle(pFileHandle->hArrayFile);
	CloseHandle(pFileHandle->hCurrentIndexFile);
	CloseHandle(pFileHandle->hCurrentTarget);
	CloseHandle(pFileHandle->hStampFile);
	CloseHandle(pFileHandle->hTotalIndexFile);
	memset(pFileHandle,0,sizeof(AM_FILE_HANDLE));

    CString strDeleteFile;
	int nFileNumber;
	int nTotalFileNumber;
	nTotalFileNumber = TargetArray.GetUpperBound();
	
	for(nFileNumber = 1; nFileNumber <=nTotalFileNumber; nFileNumber ++)
	{
		strDeleteFile = TargetArray[nFileNumber].szFileName;
		// to avoid delete the wrong file
		if( (strDeleteFile.Right(4)).CompareNoCase(AM_FILE_SUFFIX) == 0 )
		{
			DeleteFile( (LPCTSTR)strDeleteFile );
		}
	}

	ClearExtraFile(szSourceMainName);
	ClearExtraFile(szTargetMainName);

	return TRUE;
}


BOOL GetIndexData(HANDLE hTotalIndexFile, DWORD dwPathID, DWORD dwTimeStampNumber, INDEX_DATA &IndexData)
{
	__int64 lFilePointer;
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // DWORD dwMaxPathID;
    DWORD dwReadBytes;
	TOTAL_INDEX_NODE TotalIndexNode;
	YGSetFilePointer(hTotalIndexFile,0,FILE_BEGIN,lFilePointer);
	ReadFile(hTotalIndexFile,&TotalIndexNode,sizeof(TotalIndexNode),&dwReadBytes,NULL);


    DWORD dwTotalIndexOffset;
  
	dwTotalIndexOffset=GetTotalIndexOffset(TotalIndexNode.dwTotalPathCount, dwTimeStampNumber, dwPathID);
	
	YGSetFilePointer(hTotalIndexFile,dwTotalIndexOffset,FILE_BEGIN,lFilePointer);

	ReadFile(hTotalIndexFile,&IndexData,sizeof(IndexData),&dwReadBytes,NULL);

    return TRUE;
}
