#include "MakeCDR.h"
#include "AMFunction.h"
#include "resource.h"

BOOL GetLastTargetNumber(LPCTSTR szLastImage,
						 unsigned __int64 qwMaxFileSize,
						 DWORD *pdwLastTarget)
{

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CDATA_ARRAY SourceArray;

	__int64 DataSize;

	if( GetSourceArray(szLastImage,SourceArray) == FALSE)
	{
		(*pdwLastTarget) = 0;
		return FALSE;
	}
	
	if( GetDataSize(SourceArray,&DataSize) == FALSE )
	{
		(*pdwLastTarget) = 0;
		return FALSE;
	}

	(*pdwLastTarget) = (DWORD) ( ( DataSize - sizeof(IMAGE_HEAD) ) / ( qwMaxFileSize - sizeof(IMAGE_HEAD) ) +1 );

	return TRUE;

}


BOOL GetDataSize(CDATA_ARRAY &SourceArray, __int64 *pDataSize)
{
	HANDLE hLastImage;
	hLastImage=CreateFile( SourceArray[SourceArray.GetUpperBound()].szFileName,
		                   GENERIC_READ,
						   FILE_SHARE_READ,
						   NULL,
						   OPEN_EXISTING,
						   FILE_ATTRIBUTE_NORMAL,
						   NULL);

	if(hLastImage == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	IMAGE_HEAD ImageHead;
	DWORD dwReadBytes;
	ReadFile(hLastImage,&ImageHead,sizeof(ImageHead),
		     &dwReadBytes,NULL);

	CloseHandle(hLastImage);
              
	GetTotalOffset(SourceArray,
		           SourceArray.GetUpperBound(),
				   ImageHead.TotalIndexPointer.QuadPart,
				   pDataSize);

	return TRUE;
}

BOOL  GetTotalOffset( CDATA_ARRAY &SourceArray,
					  DWORD dwSourceNumber,
					  __int64 SourceOffset,
					  __int64 *pTotalOffset)
{
	if(dwSourceNumber> (DWORD)(SourceArray.GetUpperBound()) )
	{
		(*pTotalOffset)=0;
		return FALSE;
	}

	(*pTotalOffset)=0;

	if(dwSourceNumber==1)
	{
		(*pTotalOffset)=(*pTotalOffset)+SourceOffset;
	}
	else if(dwSourceNumber>1)
	{
	    (*pTotalOffset)=(*pTotalOffset)
		            +SourceArray[1].qwFileSize.QuadPart;

		for(DWORD dwFileNumber=2; dwFileNumber < dwSourceNumber; dwFileNumber++)
		{
		    (*pTotalOffset)=  (*pTotalOffset)
				            + SourceArray[dwFileNumber].qwFileSize.QuadPart
						    - sizeof(IMAGE_HEAD);
		}

		(*pTotalOffset)=(*pTotalOffset) + SourceOffset- sizeof(IMAGE_HEAD);

	}

	return TRUE;
}


/*=============================================================
 *
 *  Function Name: GetOneFile()
 * 
 *  Purpose: while recording CD-R get one file
 *
 *
 *=============================================================*/

BOOL  GetOneFile ( LPCTSTR		     szLastImage,
				   LPCTSTR			 szBufferDir,
				   unsigned __int64	 qwMaxFileSize,
				   DWORD			 dwTargetNumber,
				   TABLE_DATA		 *pTargetData
				 )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	__int64 lFilePointer;
    CDATA_ARRAY SourceArray;

	GetSourceArray(szLastImage,SourceArray);
	 
	ASSERT( SourceArray.GetSize()>=2 );

	if(SourceArray.GetSize()<2)
	{
		TRACE("\n there must be at least one source file");
		return FALSE;
	}

	// first, get the Source Head Value.
	// we use the first file for the stand source head.

	DWORD dwReadBytes;
	DWORD WrittenBytes;

	IMAGE_HEAD SourceHead;

	//-- now we read the last file head.
	//-- for the sake of get the last bytes to be copied 

    DWORD dwLastFileNumber;
	dwLastFileNumber=(DWORD)(SourceArray.GetUpperBound());

	char szLastSource[MAX_PATH];
	strcpy(szLastSource,SourceArray[dwLastFileNumber].szFileName);
		
	HANDLE hLastSource;
	hLastSource=NULL;

	hLastSource=CreateFile(szLastSource,GENERIC_READ,
		                   FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if(hLastSource == INVALID_HANDLE_VALUE)
	{
		if( GetRequiredImage(szLastSource,dwLastFileNumber,SourceArray) == FALSE )
		{
			return FALSE;
		}

		strncpy(SourceArray[dwLastFileNumber].szFileName , 
			   szLastSource,MAX_PATH-1);

		hLastSource=CreateFile(szLastSource,GENERIC_READ,
		                FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	}

	IMAGE_HEAD LastHead;
	ReadFile(hLastSource,&LastHead,sizeof(LastHead), &dwReadBytes,NULL);
	
	// use the last head as the stand head.

	SourceHead=LastHead;

	CloseHandle(hLastSource);

    char szTargetName[MAX_PATH];

    char *pBuffer;
	pBuffer=NULL;
    
	// here we prepare for the first file

	CString csLastFileName;
	HANDLE hCurrentSource;

    HANDLE hCurrentTarget;
	hCurrentTarget=NULL;

	csLastFileName=SourceArray[SourceArray.GetUpperBound()].szFileName;

	CString csBufferDir;
	csBufferDir=szBufferDir;

    if( NewGetNewTargetHandle( hCurrentTarget,
							   (LPCTSTR)csBufferDir,
							   (LPCTSTR)csLastFileName,
							   dwTargetNumber,
							   szTargetName) == FALSE )
    {
	    return FALSE;
    }

    // create a serial no
    GUID OneGuid;
    ::CoCreateGuid(&OneGuid);
    CString strGuid;
    GetGuidString(strGuid.GetBuffer(GUID_SIZE),OneGuid);
	strGuid.ReleaseBuffer();

	// the TargetData is only for get the size
	TABLE_DATA TargetData;
	memset(&TargetData,0,sizeof(TargetData));
	memset(pTargetData,0,sizeof(TargetData));
    
	// record the serial number.
	strncpy( (*pTargetData).szSerialNumber,(LPCTSTR)strGuid,sizeof(TargetData.szSerialNumber)-1);

	strncpy( (*pTargetData).szFileName,szTargetName,MAX_PATH-1);

	WriteTargetHead(hCurrentTarget,&SourceHead,dwTargetNumber,(*pTargetData) );

	 // here we should get the file pointer of a certain file number

	__int64 qwTotalOffset;
	 
	qwTotalOffset = sizeof(IMAGE_HEAD) + (dwTargetNumber - 1) * ( qwMaxFileSize-sizeof(IMAGE_HEAD) );
	 
	LARGE_INTEGER SubFilePointer;

	DWORD dwBeginFileNumber;
	 
	if( GetSubFilePointer(SourceArray,qwTotalOffset,&dwBeginFileNumber,&SubFilePointer) == FALSE )
	{
		CloseHandle(hCurrentTarget);
		hCurrentTarget=NULL;
		return FALSE;
	}


	//-- get the data size of the image file

	__int64 DataSize;

	GetTotalOffset(SourceArray,SourceArray.GetUpperBound(),
	                LastHead.TotalIndexPointer.QuadPart,&DataSize);


	BOOL bMaxSizeReach;
	bMaxSizeReach=FALSE;

	BOOL bCancel;
	bCancel=FALSE;
	 
	for( 
		  DWORD dwFileNumber=dwBeginFileNumber;
	      dwFileNumber <= (DWORD)(SourceArray.GetUpperBound()); 
		  dwFileNumber++
		)
	{
	   hCurrentSource=CreateFile(SourceArray[dwFileNumber].szFileName ,GENERIC_READ,
								  FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	   if(hCurrentSource==INVALID_HANDLE_VALUE)
	   {

		   char szImageFile[MAX_PATH];

		   if(GetRequiredImage(szImageFile,dwFileNumber,SourceArray) == FALSE)
		   {
		       CloseHandle(hCurrentTarget);
			   hCurrentTarget=NULL;
		       return FALSE;
		   }
	       
		   hCurrentSource=CreateFile(szImageFile,GENERIC_READ,
		                   FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	   }

   	   if(dwFileNumber == dwBeginFileNumber)
	   {
	      YGSetFilePointer(hCurrentSource,SubFilePointer.QuadPart,FILE_BEGIN,lFilePointer);
	   }
	   else if(dwFileNumber > dwBeginFileNumber)
	   {
	      YGSetFilePointer(hCurrentSource,sizeof(IMAGE_HEAD),FILE_BEGIN,lFilePointer);
	   }

	   pBuffer=(char *)malloc(AM_BUFFER_SIZE);
	
	   if(pBuffer==NULL)
	   {
		   AfxMessageBox(IDS_NOT_ENOUGH_MEMORY,MB_OK|MB_ICONINFORMATION,NULL);
		   CloseHandle(hCurrentTarget);
		   hCurrentTarget=NULL;
		   return FALSE;
	   }

	   // infact we should open the current target here

	   ULARGE_INTEGER qwTargetSize;
	   qwTargetSize.LowPart=GetFileSize(hCurrentTarget,&(qwTargetSize.HighPart));

	   DWORD dwRequestRead;
	   DWORD dwRequestWrite;

	   if ( dwFileNumber < (DWORD)(SourceArray.GetUpperBound() ) )
	   {
		   // in this case, read till the end of the file
		   do
		   {
				 ReadFile(hCurrentSource,pBuffer,AM_BUFFER_SIZE,&dwReadBytes,NULL);

				 dwRequestWrite=dwReadBytes;
				
				 // if(qwTargetSize.QuadPart < qwMaxFileSize - AM_BUFFER_SIZE)
				 if(qwTargetSize.QuadPart <= qwMaxFileSize - dwRequestWrite)
				 {
					WriteFile(hCurrentTarget,pBuffer,dwRequestWrite,
								 &WrittenBytes,NULL);

					qwTargetSize.QuadPart  = qwTargetSize.QuadPart  + WrittenBytes;

					if(WrittenBytes<dwRequestWrite)
					{
						// if target disk is full, open a new file to write.
						// in this case, this thing will not happen
						// we will check the buffer size first.
						CloseHandle(hCurrentTarget);
						hCurrentTarget=NULL;
						bCancel=TRUE;
						break;
						// return FALSE;
					} // end if the target disk full
					
				}// end if max target file size is not reached
				else
				{
					dwRequestWrite = (DWORD)(qwMaxFileSize - qwTargetSize.QuadPart);

					WriteFile(hCurrentTarget,pBuffer,dwRequestWrite,
								 &WrittenBytes,NULL);

					qwTargetSize.QuadPart  = qwTargetSize.QuadPart  + WrittenBytes;

					// if the target file size is reached
					bMaxSizeReach=TRUE;

					break;

				} // end if max file size is reached.
		   } while(dwReadBytes>0);
	   }
	   else if ( dwFileNumber == (DWORD)(SourceArray.GetUpperBound() ) )
	   {
            __int64 LeftBytes;
		    LeftBytes=0;
			// LeftBytes = LastHead.TotalIndexPointer.QuadPart - sizeof(LastHead) ;

			if(dwFileNumber == dwBeginFileNumber)
			{
			   // this means this divide is made in the last image
			   LeftBytes = LastHead.TotalIndexPointer.QuadPart - SubFilePointer.LowPart;
			}
			else if(dwFileNumber > dwBeginFileNumber)
			{
			   // this means this divide cross one or more files
				LeftBytes = LastHead.TotalIndexPointer.QuadPart - sizeof(LastHead);
			}

			if(LeftBytes>0)
			{
				do
				{
					 if(LeftBytes>AM_BUFFER_SIZE)
					 {
						dwRequestRead=AM_BUFFER_SIZE;
					 }
					 else
					 {
						dwRequestRead=(DWORD)LeftBytes;
					 }

					 ReadFile(hCurrentSource,pBuffer,dwRequestRead,&dwReadBytes,NULL);
				
					 // LeftBytes is count by read bytes.
					 // in fact, here dwRequestBytes should equal to dwReadBytes

					 LeftBytes = LeftBytes - dwRequestRead;
					
					 dwRequestWrite = dwReadBytes;

					 if(qwTargetSize.QuadPart <= qwMaxFileSize - dwRequestWrite)
					 {
						WriteFile(hCurrentTarget,pBuffer,dwRequestWrite,
									 &WrittenBytes,NULL);

						qwTargetSize.QuadPart  = qwTargetSize.QuadPart  + WrittenBytes;

						if(WrittenBytes<dwRequestWrite)
						{
							// if target disk is full, open a new file to write.
							// in this case, this thing will not happen
							// we will check the buffer size first.
							CloseHandle(hCurrentTarget);
							hCurrentTarget=NULL;
							bCancel=TRUE;
							break;
							// return FALSE;
						} // end if the target disk full
					} // end if max target file size is not reached
					else
					{
						dwRequestWrite = (DWORD)(qwMaxFileSize - qwTargetSize.QuadPart);

						WriteFile(hCurrentTarget,pBuffer,dwRequestWrite,
									 &WrittenBytes,NULL);

						qwTargetSize.QuadPart  = qwTargetSize.QuadPart  + WrittenBytes;

						// if the target file size is reached
						bMaxSizeReach=TRUE;

						break;

					} // end if max file size is reached.

				} while ( LeftBytes >0 && dwReadBytes>0);
			}
	   }

       CloseHandle(hCurrentSource);

	   if(bMaxSizeReach==TRUE || bCancel==TRUE)
	   {
		   break;
	   }

    } // end the for loop

	free(pBuffer);

	pBuffer=NULL;

	// get the file size.
	ULARGE_INTEGER qwFileSize;
	
	qwFileSize.LowPart=GetFileSize( hCurrentTarget,&(qwFileSize.HighPart));
	
	(*pTargetData).qwFileSize.QuadPart = qwFileSize.QuadPart;

	CloseHandle(hCurrentTarget);
	hCurrentTarget=NULL;

	// here to write signature
	WriteSignature(szTargetName);

	return TRUE;
}


BOOL GetRequiredImage(char *szImageFile, DWORD dwFileNumber, CDATA_ARRAY &ImageArray)
{

	ASSERT(ImageArray.GetUpperBound()>=1);

	if( dwFileNumber ==0 ||  dwFileNumber> (DWORD)( ImageArray.GetUpperBound()) )
	{
		return FALSE;
	}

	strcpy(szImageFile,ImageArray[dwFileNumber].szFileName);

	BOOL bValidImage;
	bValidImage=FALSE;

	BOOL bContinueSelect;
	bContinueSelect=TRUE;

	bValidImage=CheckImage(szImageFile,dwFileNumber,ImageArray);

	while(bValidImage==FALSE)
	{
	    CString strInfo;
		CString strFileName;

		strFileName=ImageArray[dwFileNumber].szFileName;
		int nIndex;
		nIndex=strFileName.ReverseFind('\\');
		strFileName=strFileName.Mid(nIndex+1);

	    //-- display the information begin
		// strInfo.Format(" Please select the file \n File Number is: %d  Original File Name is: %s",dwFileNumber,(LPCTSTR)strFileName);

		CString strSelectFile;
		strSelectFile.LoadString(IDS_SELECT_FILE);

		CString strFileNumber;
		strFileNumber.LoadString(IDS_FILE_NUMBER);

		CString strOriginalFileName;
		strOriginalFileName.LoadString(IDS_ORIGINAL_FILE_NAME);

		CString strFormat;
		strFormat=strSelectFile + "\n" + strFileNumber + "  %d\n" + strOriginalFileName + "  %s  ";

		strInfo.Format(strFormat,dwFileNumber,(LPCTSTR)strFileName);
		
		AfxMessageBox((LPCTSTR)strInfo,MB_OK|MB_ICONINFORMATION,NULL);
		//-- display the information end

		BOOL bReturnValue;

		bReturnValue=SelectFile("AMG","AM01 Image File",szImageFile);

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
		   return FALSE;
		}
	}

	// record it in the ImageArray
	if(bValidImage==TRUE)
	{
	   strcpy(ImageArray[dwFileNumber].szFileName,szImageFile);
	   return TRUE;
    }
	else
	{
	   return FALSE;
	}
}


/*==========================================================
 *
 * Function Name: NewGetNewTargetHandle()
 * Purpose:       This function is used while recording CD 
 *
 *
 *
 *==========================================================*/

BOOL NewGetNewTargetHandle(HANDLE &hCurrentTarget,
						   LPCTSTR szBufferDir,
						   LPCTSTR szLastFileName,
						   DWORD dwFileNumber,
						   char *szTargetName)
{

	CString csBufferDir;
	csBufferDir=szBufferDir;

	CString csLastFileName;
	csLastFileName=szLastFileName;

	// AfxMessageBox("Please input a Target File Name",MB_OK|MB_ICONINFORMATION,NULL);
	int nIndex;
	nIndex=csLastFileName.ReverseFind('\\');
	csLastFileName=csLastFileName.Mid(nIndex+1);
	nIndex=csLastFileName.ReverseFind('.');
	csLastFileName=csLastFileName.Left(nIndex);

	CString csNumber;
	csNumber.Format("f%.3d",dwFileNumber);
    
	CString strPathName;
	// strPathName=csBufferDir + "\\" + csLastFileName + "_" + csNumber+AM_FILE_SUFFIX;
	strPathName=csBufferDir + "\\" + csLastFileName + "_" + csNumber+ AM_FILE_SUFFIX;

	//-- for debug begin
	// AfxMessageBox(  (LPCTSTR)("NewFile: "+strPathName),MB_OK|MB_ICONINFORMATION,NULL);
	//-- for debug end

    hCurrentTarget=CreateFile((LPCTSTR)strPathName,GENERIC_READ|GENERIC_WRITE,
		              FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

	if(hCurrentTarget == INVALID_HANDLE_VALUE)
	{
		TRACE("\n Create Buffer File Error.");
		return FALSE;
	}

	strncpy(szTargetName,(LPCTSTR)strPathName,MAX_PATH-1);
	
	return TRUE;
}


BOOL GetSubFilePointer(CDATA_ARRAY &FileArray, __int64 TotalOffset, DWORD *pdwFileNumber, LARGE_INTEGER *pSubFilePointer)
{
	// can not be empty FileArray
	ASSERT(FileArray.GetSize()>=2);
	ASSERT(TotalOffset>=0);


	__int64 MaxTotalOffset;

	GetMaxTotalOffset(FileArray, &MaxTotalOffset);

	if(TotalOffset > MaxTotalOffset)
	{
        (*pdwFileNumber)=FileArray.GetUpperBound();
        (*pSubFilePointer).QuadPart =sizeof(IMAGE_HEAD);
		return FALSE;
	}


	DWORD dwFileNumber=1;

	// total offset is __int64
	// qwFileSize is unsigned __int64

	TotalOffset=TotalOffset-sizeof(IMAGE_HEAD);

	//-- for debug
	unsigned __int64 qwCurrentSize;
	qwCurrentSize=FileArray[dwFileNumber].qwFileSize.QuadPart;

	//-- end for debug

	while( ( TotalOffset >= (__int64)(qwCurrentSize-sizeof(IMAGE_HEAD) ) )
		   && 
		   ( dwFileNumber <  ( (DWORD) FileArray.GetUpperBound() ) ) 
		 )
	{
		TotalOffset=TotalOffset-(qwCurrentSize-sizeof(IMAGE_HEAD) );
		dwFileNumber++;
		qwCurrentSize=FileArray[dwFileNumber].qwFileSize.QuadPart;
	}


	TotalOffset=TotalOffset+sizeof(IMAGE_HEAD);
    
	// this is return value

	(*pdwFileNumber)=dwFileNumber;
	(*pSubFilePointer).QuadPart = TotalOffset;

	
	return TRUE;
}


BOOL GetMaxTotalOffset(CDATA_ARRAY &SourceArray, __int64 *pMaxTotalOffset)
{

	GetTotalOffset( SourceArray,
		            SourceArray.GetUpperBound(),
				    SourceArray[SourceArray.GetUpperBound()].qwFileSize.QuadPart -1,
				    pMaxTotalOffset);

	return TRUE;
}

/*===========================================================
 *
 * Function Name: NewRecordTail
 *
 * Purpose:  This function is used in AMMakeCDR.cpp
 *           The function NewMakeMultiCD will call it
 *
 *==========================================================*/
BOOL NewRecordTail(
				    LPCTSTR        szLastImage,
				    LPCTSTR        szBufferDir,
					CDATA_ARRAY    &TargetArray,
				    unsigned		__int64 qwMaxFileSize,
				    BOOL			*pbNewFile,
					char		    *szNewTarget
				  )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

	__int64 lFilePointer;

    (*pbNewFile)=FALSE;
	memset(szNewTarget,0,MAX_PATH);
	
	CDATA_ARRAY NewTargetArray;

	for(int i=0; i<TargetArray.GetSize(); i++)
	{
		NewTargetArray.Add(TargetArray[i]);
	}

	CDATA_ARRAY SourceArray;
	GetSourceArray(szLastImage,SourceArray);

	HANDLE hLastSource;
	hLastSource=CreateFile( SourceArray[SourceArray.GetUpperBound()].szFileName,GENERIC_READ,FILE_SHARE_READ,
		                    NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	__int64 LeftTailSize;
	ULARGE_INTEGER qwFileSize;

	IMAGE_HEAD ImageHead;
	IMAGE_HEAD SourceHead;

	DWORD dwReadBytes;
	ReadFile(hLastSource,&ImageHead,sizeof(ImageHead),&dwReadBytes,NULL);

	SourceHead = ImageHead;

	qwFileSize.LowPart=GetFileSize(hLastSource,&(qwFileSize.HighPart));

	// TailSize=qwFileSize.QuadPart - ImageHead.TotalIndexPointer.QuadPart;
	__int64 ArraySize;
	ArraySize=ImageHead.StampPointer.QuadPart - ImageHead.ArrayPointer.QuadPart;

	__int64 StampSize;
	StampSize=ImageHead.FileTablePointer.QuadPart - ImageHead.StampPointer.QuadPart;

	LeftTailSize=   ArraySize
				  + StampSize
				  + sizeof(TABLE_NODE)
				  + sizeof(TABLE_DATA) * NewTargetArray.GetUpperBound();

	// last target file will come from NewTargetArray
	HANDLE hLastTarget;
	hLastTarget=CreateFile( NewTargetArray[NewTargetArray.GetUpperBound()].szFileName,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,
		                    NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	
	ULARGE_INTEGER qwTargetSize;
	qwTargetSize.LowPart = GetFileSize(hLastTarget,&(qwTargetSize.HighPart));

	LARGE_INTEGER TotalIndexOffset;
	LARGE_INTEGER ArrayOffset;
	LARGE_INTEGER StampOffset;
	
	if(qwTargetSize.QuadPart  + LeftTailSize < qwMaxFileSize)
	{
	    YGSetFilePointer(hLastTarget,0,FILE_END,TotalIndexOffset.QuadPart);

		CopyIndexFile(hLastSource,
			          hLastTarget,
					  ImageHead.TotalIndexPointer,
					  SourceArray,
					  NewTargetArray);		// copy index data, needn't length, use loop to control

		YGSetFilePointer(hLastTarget,0,FILE_END,ArrayOffset.QuadPart);

		CopyLeftTail(hLastSource,hLastTarget,ImageHead.ArrayPointer,ArraySize); // copy the left tail, perhaps needn't length, but have a length is still good

		YGSetFilePointer(hLastTarget,0,FILE_END,StampOffset.QuadPart);

		CopyLeftTail(hLastSource,hLastTarget,ImageHead.StampPointer,StampSize); // copy the left tail, perhaps needn't length, but have a length is still good
	}
	else
	{
		// open a new file

		// then CopyIndexFile
	    ULARGE_INTEGER qwPreLastFileSize;

		qwPreLastFileSize.LowPart = GetFileSize(hLastTarget,&(qwPreLastFileSize.HighPart));
		
		NewTargetArray[NewTargetArray.GetUpperBound()].qwFileSize.QuadPart = qwPreLastFileSize.QuadPart;

		CloseHandle(hLastTarget);

		//-- here to make cd copy begin

		CString csFile;
		csFile=NewTargetArray[NewTargetArray.GetUpperBound()].szFileName;

		// here to write finish signature
		WriteSignature( (LPCTSTR)csFile );

		char szTargetName[MAX_PATH];
        
		DWORD dwFileNumber;
		dwFileNumber=NewTargetArray.GetUpperBound()+1;

		//-- new time added begin

		CString csLastFileName;
		GetLongMainName(SourceArray[SourceArray.GetUpperBound()].szFileName,csLastFileName.GetBuffer(MAX_PATH));
		csLastFileName.ReleaseBuffer();
		// csLastFileName=csLastFileName+AM_FILE_SUFFIX;
		csLastFileName=csLastFileName+AM_FILE_SUFFIX;

		//-- new time added end

		if( NewGetNewTargetHandle(hLastTarget,szBufferDir,(LPCTSTR)csLastFileName,
			                      dwFileNumber,szTargetName) == FALSE)
		{
			return FALSE;
		}

		//-- return some value to main function begin

		(*pbNewFile)=TRUE;
		strncpy(szNewTarget,szTargetName,MAX_PATH-1);

		//-- return some value to main function end

		// create a serial no
	    GUID OneGuid;
        ::CoCreateGuid(&OneGuid);
        CString strGuid;
        GetGuidString(strGuid.GetBuffer(GUID_SIZE),OneGuid);
		strGuid.ReleaseBuffer();

	    TABLE_DATA TargetData;
	    memset(&TargetData,0,sizeof(TargetData));
    
	    // record the serial number.
	    strncpy(TargetData.szSerialNumber,(LPCTSTR)strGuid,sizeof(TargetData.szSerialNumber)-1);

	    TargetData.dwFileNumber=NewTargetArray.GetUpperBound()+1;
	
	    strncpy(TargetData.szFileName,szTargetName,MAX_PATH-1);
	
	    NewTargetArray.Add(TargetData);
	
	    // in the future, we should judge the disk space should be larger than
	    // IMAGE_HEAD
	    
		WriteTargetHead( hLastTarget,&SourceHead,TargetData.dwFileNumber ,TargetData);

	    YGSetFilePointer(hLastTarget,0,FILE_END,TotalIndexOffset.QuadPart);

		CopyIndexFile(hLastSource,
			          hLastTarget,
					  ImageHead.TotalIndexPointer,
					  SourceArray,
					  NewTargetArray);		// copy index data, needn't length, use loop to control

		YGSetFilePointer(hLastTarget,0,FILE_END,ArrayOffset.QuadPart);

		CopyLeftTail(hLastSource,hLastTarget,ImageHead.ArrayPointer,ArraySize); // copy the left tail, perhaps needn't length, but have a length is still good

		YGSetFilePointer(hLastTarget,0,FILE_END,StampOffset.QuadPart);

		CopyLeftTail(hLastSource,hLastTarget,ImageHead.StampPointer,StampSize); // copy the left tail, perhaps needn't length, but have a length is still good
	}

	// when open a new file
	// record file name, serial number, file number
    // when close a file, record file size

	ULARGE_INTEGER qwLastFileSize;
	qwLastFileSize.LowPart=GetFileSize(hLastTarget,&(qwLastFileSize.HighPart));
	NewTargetArray[NewTargetArray.GetUpperBound()].qwFileSize.QuadPart = qwLastFileSize.QuadPart ;

	// adjust the file pointer in the file head

	YGSetFilePointer(hLastTarget,0,FILE_BEGIN,lFilePointer);
	ReadFile(hLastTarget,&ImageHead,sizeof(ImageHead),&dwReadBytes,NULL);

	// here to add a last file number signature
	// ImageHead.dwFileNumber = LAST_FILE_NUMBER;

	ImageHead.TotalIndexPointer.QuadPart=TotalIndexOffset.QuadPart;
	ImageHead.ArrayPointer.QuadPart = ArrayOffset.QuadPart;
	ImageHead.StampPointer.QuadPart = StampOffset.QuadPart;
	
	YGSetFilePointer(hLastTarget,0,FILE_BEGIN,lFilePointer);

	DWORD WrittenBytes;

	// before write the ImageHead to disk, encrypt the password
	WriteFile(hLastTarget,&ImageHead,sizeof(ImageHead),&WrittenBytes,NULL);

	CloseHandle(hLastSource);
	CloseHandle(hLastTarget);

	// here to do some extra thing
	RecordFileTable(NewTargetArray);
	
	AdjustFileHead(NewTargetArray);

	CString strLastFile;
	strLastFile=NewTargetArray[NewTargetArray.GetUpperBound()].szFileName;

	// here to write signature
	WriteSignature( (LPCTSTR) strLastFile );

	return TRUE;
}


BOOL CopyIndexFile(HANDLE hLastSource, HANDLE hLastTarget, LARGE_INTEGER TotalIndexPointer,CDATA_ARRAY &SourceArray, CDATA_ARRAY &TargetArray)
{
	__int64 lFilePointer;

	YGSetFilePointer(hLastSource,TotalIndexPointer.QuadPart,FILE_BEGIN,lFilePointer);
	
	// set file pointer to end for append

	YGSetFilePointer(hLastTarget,0,FILE_END,lFilePointer);

	TOTAL_INDEX_NODE TotalIndexNode;
	INDEX_DATA IndexData;

	DWORD dwReadBytes;
	DWORD WrittenBytes;

	ReadFile(hLastSource,&TotalIndexNode,sizeof(TotalIndexNode),
		     &dwReadBytes,NULL);

	WriteFile(hLastTarget,&TotalIndexNode,sizeof(TotalIndexNode),
		     &WrittenBytes,NULL);


	DWORD dwStampNumber;
	DWORD dwPathNumber;

	LARGE_INTEGER SourceOffset;
	DWORD dwSourceNumber;

	LARGE_INTEGER TargetOffset;
	DWORD dwTargetNumber;

	__int64 TotalOffset;

	// LARGE_INTEGER SubPointer;

	for(dwStampNumber=1; dwStampNumber<=TotalIndexNode.dwTimeStampCount ; dwStampNumber++)
	{
		for(dwPathNumber=1; dwPathNumber<=TotalIndexNode.dwTotalPathCount ; dwPathNumber++)
		{

			ReadFile(hLastSource,&IndexData,sizeof(IndexData),&dwReadBytes,NULL);

			if(IndexData.wStatusFlag == FILE_NEW
			   || IndexData.wStatusFlag == FILE_CHANGE )
			{
			  SourceOffset.QuadPart=IndexData.DataPointer.QuadPart;
			  dwSourceNumber=IndexData.dwFileNumber;

			  GetTotalOffset( SourceArray,
				              IndexData.dwFileNumber,
							  SourceOffset.QuadPart,
							  &TotalOffset );

			  GetSubFilePointer( TargetArray,
				                 TotalOffset,
								 &dwTargetNumber,
								 &TargetOffset );


			  IndexData.dwFileNumber=dwTargetNumber;
			  IndexData.DataPointer.QuadPart=TargetOffset.QuadPart;
			  
			}
		   
			WriteFile(hLastTarget,&IndexData,sizeof(IndexData),&WrittenBytes,NULL);
			
			if( WrittenBytes != sizeof(IndexData) )
			{
				// perhaps target disk full
				return FALSE;
			}
		}
	}

	return TRUE;
}


BOOL CopyLeftTail(HANDLE hLastSource, HANDLE hLastTarget, LARGE_INTEGER ArrayPointer, unsigned __int64 qwTailSize)
{
	__int64 lFilePointer;

	YGSetFilePointer(hLastSource,ArrayPointer.QuadPart,FILE_BEGIN,lFilePointer);

	// set file pointer to end for append
	YGSetFilePointer(hLastTarget,0,FILE_END,lFilePointer);

	DWORD dwReadBytes;
	DWORD WrittenBytes;

	char *pBuffer;
	
	pBuffer=(char *) malloc(AM_BUFFER_SIZE);

	if(pBuffer == NULL)
	{
		AfxMessageBox(IDS_NOT_ENOUGH_MEMORY,MB_OK|MB_ICONINFORMATION,NULL);
		return FALSE;
	}


	// copy the left of hLastSource

	unsigned __int64 qwLeftBytes;

	qwLeftBytes=qwTailSize;

	DWORD dwRequestRead;

	do 
	{
	  if(qwLeftBytes > AM_BUFFER_SIZE)
	  {
		  dwRequestRead = AM_BUFFER_SIZE;
	  }
	  else
	  {
		  dwRequestRead = (DWORD)qwLeftBytes;
	  }

	  ReadFile(hLastSource,pBuffer,dwRequestRead,&dwReadBytes,NULL);
	  qwLeftBytes=qwLeftBytes - dwReadBytes;

      WriteFile(hLastTarget,pBuffer,dwReadBytes,&WrittenBytes,NULL);

	  if(WrittenBytes != dwReadBytes)
	  {
		  // perhaps target disk full

		  return FALSE;
	  }
	
	}while(dwReadBytes>0 && qwLeftBytes>0);

	return TRUE;
}

// in the function of AdjustFileHead SourceHead is not needed
BOOL AdjustFileHead(const CDATA_ARRAY &TargetArray)
{
	__int64 lFilePointer;
	ASSERT(TargetArray.GetSize()>=2);
	DWORD dwFileNumber;
	DWORD dwLastFileNumber;

	IMAGE_HEAD TargetHead;
	DWORD dwReadBytes;
	DWORD WrittenBytes;

	dwLastFileNumber=(DWORD)(TargetArray.GetSize())-1;

	HANDLE hCurrentTarget;

	// only adjust the last file head.
	// in the future, perhaps we will adjust the file header of every file.
    
	for( dwFileNumber=(DWORD)(TargetArray.GetSize()-1);
	     dwFileNumber<(DWORD)(TargetArray.GetSize());
		 dwFileNumber++)
	{
		hCurrentTarget=CreateFile(TargetArray[dwFileNumber].szFileName,
			GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,
			NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

		if(INVALID_HANDLE_VALUE == hCurrentTarget)
		{
			return FALSE;
		}

        if(dwFileNumber==dwLastFileNumber)
		{
			YGSetFilePointer(hCurrentTarget,0,FILE_BEGIN,lFilePointer);

			ReadFile(hCurrentTarget,&TargetHead,sizeof(TargetHead),
				     &dwReadBytes,NULL);


			TargetHead.dwFileNumber=LAST_FILE_NUMBER;
			
			YGSetFilePointer(hCurrentTarget,0,FILE_BEGIN,lFilePointer);
			WriteFile(hCurrentTarget,&TargetHead,sizeof(TargetHead),
				     &WrittenBytes,NULL);
		}

		CloseHandle(hCurrentTarget);

	}

	return TRUE;
}


BOOL bMultiFile(LPCTSTR szLastImage)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CDATA_ARRAY SourceArray;
	GetSourceArray(szLastImage,SourceArray);
	if(SourceArray.GetUpperBound()==1)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}
