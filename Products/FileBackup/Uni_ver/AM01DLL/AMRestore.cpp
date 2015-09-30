#include "AMRestore.h"
#include "resource.h"
#include "AMFunction.h"
#include "MailSetting.h"
#include "..\AM01Expt\AM01Expt.h"
#include "..\..\..\davinci\Davinci_tech\zlib\Cur_ver\Export\zlib.h"

AM_RESTORE_INFO g_RestoreInfo;

// for progress bar
int g_nTotalFileCount;
int g_nCurrentFileCount;

//-- restore function begin
RESTORE_STATE g_RestoreState;

// for overwrite option
OVERWRITE_OPTION g_OverwriteOption;

BOOL bIsFileSelected(LPCTSTR szOnePath, AM_RESTORE_INFO RestoreInfo)
{
	// if it is restore entire, all path should be considered
	// as selected

	if(RestoreInfo.nRestoreType == RESTORE_ENTIRE)
	{
		return TRUE;
	}

	CString SelectedPath;
	SelectedPath=RestoreInfo.szSelectedPath;

	CString OnePathString;
	OnePathString=szOnePath;

	// add L"\\" for compare

	if(SelectedPath.Right(1)!=L"\\")
	{
	   SelectedPath=SelectedPath+L"\\";
	}

	if(OnePathString.Right(1)!=L"\\")
	{
		OnePathString=OnePathString+L"\\";
	}

	// if a string is longer then SelectedPath, and the left is the same
	// if should be restored.

	OnePathString = OnePathString.Left( SelectedPath.GetLength() );
	if(OnePathString == SelectedPath)
	{
			return TRUE;
	}

	// if restore to original place
	// the parent dir should be restored too.

	if(RestoreInfo.nTargetType==RESTORE_ORIGINAL_PLACE)
	{  
		SelectedPath=SelectedPath.Left( OnePathString.GetLength());
	    if(OnePathString == SelectedPath)
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL GetRestoreFileCount(AM_RESTORE_INFO RestoreInfo)
{
	__int64 lFilePointer;
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(RestoreInfo.dwTimeStamp<0)
	{
		AfxMessageBox(IDS_NOSELECT_TIMESTAMP,MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}

	g_nTotalFileCount=0;

    CString TotalIndexFilePath;
	CString TempDir;
	CString ImageFileName;

	TempDir=RestoreInfo.szTempDir;

	GetLongMainName(RestoreInfo.szImageFile,ImageFileName.GetBuffer(MAX_PATH));
	ImageFileName.ReleaseBuffer();

	TotalIndexFilePath=TempDir+L"\\"+ ImageFileName + AM_INDEX_SUFFIX ;

    // open TotalIndex file
    HANDLE hTotalIndexFile;
    hTotalIndexFile=CreateFile( LPCTSTR(TotalIndexFilePath), GENERIC_READ,
                                FILE_SHARE_READ,NULL,
			        		    OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,
					            NULL);

	if(hTotalIndexFile==INVALID_HANDLE_VALUE)
	{
		TRACE(L"Open Total Index File Error in Get Restore File Count. \n");
		return FALSE;
	}

    // open Array file

	CString ArrayFilePath;
	ArrayFilePath=TempDir+L"\\"+ ImageFileName + L".ary" ;
    HANDLE hArrayFile;
    hArrayFile=CreateFile(  LPCTSTR(ArrayFilePath), GENERIC_READ,
							FILE_SHARE_READ,NULL,
			        		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,
							NULL);

	if(hArrayFile==INVALID_HANDLE_VALUE)
	{
		TRACE(L"Open Array File Error in Get Restore File Count. \n");
		return FALSE;
	}


	TOTAL_INDEX_NODE TotalIndexNode;
	DWORD dwReadBytes;
	ReadFile(hTotalIndexFile,&TotalIndexNode,sizeof(TotalIndexNode),
		     &dwReadBytes,NULL);

	INDEX_DATA IndexData;
	ARRAY_DATA ArrayData;

    DWORD dwTimeStampNumber;

	dwTimeStampNumber=RestoreInfo.dwTimeStamp;

	if(RestoreInfo.nRestoreType == RESTORE_ENTIRE)
	{
		DWORD dwTotalIndexOffset;

		dwTotalIndexOffset=GetTotalIndexOffset(TotalIndexNode.dwTotalPathCount,RestoreInfo.dwTimeStamp ,1);

		YGSetFilePointer(hTotalIndexFile,dwTotalIndexOffset,FILE_BEGIN,lFilePointer);

		for(DWORD i=0; i<TotalIndexNode.dwTotalPathCount; i++)
	    {
			ReadFile(hTotalIndexFile,&IndexData,sizeof(IndexData),
				     &dwReadBytes,NULL);

		
			if(IndexData.dwPathID > 0) 
			{
				if( (IndexData.FileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)!= 0)
				{
					// g_nDirectoryCount++;
				}
				else
				{
					g_nTotalFileCount++;
				}

			} // end if dwPathID > 0;
		} // end for loop (loop every PathID)
	}
	else if(RestoreInfo.nRestoreType == RESTORE_PART)
	{
		// MessageBox(L"Part Restore");

        BOOL bSelectedPath=FALSE;

		DWORD dwTotalIndexOffset;

		// GetSelectedItem return the index, it is zero based
		// so it should add 1.

		dwTotalIndexOffset=GetTotalIndexOffset(TotalIndexNode.dwTotalPathCount,RestoreInfo.dwTimeStamp ,1);

		YGSetFilePointer(hTotalIndexFile,dwTotalIndexOffset,FILE_BEGIN,lFilePointer);

		YGSetFilePointer(hArrayFile,sizeof(ARRAY_NODE),FILE_BEGIN,lFilePointer);

		for(DWORD i=0; i<TotalIndexNode.dwTotalPathCount; i++)
	    {
			ReadFile(hTotalIndexFile,&IndexData,sizeof(IndexData),
				     &dwReadBytes,NULL);

			ReadFile(hArrayFile,&ArrayData,sizeof(ArrayData),&dwReadBytes,NULL);
				
			// IndexData.dwPathID ==0 means no this file at this time stamp
			if(IndexData.dwPathID > 0) 
			{
				CString OnePathString;
		          
				// OnePathString=TotalStringArray.ElementAt(i);
				OnePathString=ArrayData.szFilePath;

		        OnePathString.TrimRight(L"\\");

				bSelectedPath=bIsFileSelected((LPCTSTR)OnePathString,RestoreInfo);

		        if(bSelectedPath)
				{
					if( (IndexData.FileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
					{
						// g_nDirectoryCount++;
					}
					else
					{
						g_nTotalFileCount++;
					}

				} // end if Selected path;
			} // end if dwPathID > 0;
		} // end for loop (loop every PathID)
	} // end if restore entire or part

	CloseHandle(hTotalIndexFile);
	
	CloseHandle(hArrayFile);

	return TRUE;
}


BOOL RestoreFile(AM_RESTORE_INFO RestoreInfo,CDATA_ARRAY &ImageArray)
{
	__int64 lFilePointer;
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(ImageArray.GetSize()<2)
	{
		TRACE(L"did not fill the Image array before new restore file\n");
		return FALSE;
	}

	ADJUST_MAIL_INFO AdjustMailInfo;
	memset(&AdjustMailInfo,0,sizeof(AdjustMailInfo));

	if( OE_MAIL_BACKUP == g_RestoreInfo.wImageType ||
		WIN_MAIL_BACKUP == g_RestoreInfo.wImageType)
	{
		// if OutlookExpressDir is not exist, we try to restore the 
		// mail data to the current user
		if( FALSE == CheckOutlookExpressDir(RestoreInfo,AdjustMailInfo) )
		{
			TRACE(L"\nCheckOutlookExpressDir error in RestoreFile.");
			return FALSE;
		}
	}

	g_RestoreState.bRestoreCancel = FALSE;

	// TODO: Add your control notification handler code here

	// the following is for progress bar
	// it is a seperate function

	g_nCurrentFileCount=0;
	g_RestoreState.wPercent=0;

	BOOL bContinueRestore;
	bContinueRestore=FALSE;

	// for overwrite option
	g_OverwriteOption.bOverwriteAll = FALSE;
	g_OverwriteOption.bOverwriteNone = FALSE;

	//-- in the case of OEMail adjust restore we will not ask the following question
	//-- we have asked the user before.

	if(
		!( (OE_MAIL_BACKUP == RestoreInfo.wImageType ||
			WIN_MAIL_BACKUP == g_RestoreInfo.wImageType)
		&& TRUE == AdjustMailInfo.bAdjustMailPath )
	  )
	{
		int nReturnValue;
		if(RestoreInfo.nTargetType == RESTORE_ORIGINAL_PLACE)
		{
			nReturnValue=AfxMessageBox(IDS_OVERWRITE_ORIGINAL,MB_YESNOCANCEL|MB_ICONINFORMATION,NULL);
			
			if( IDYES != nReturnValue )
			{
				return FALSE;
			}
			
		}
	}

    BOOL bSelectedPath=FALSE;

	// get the temp TotalIndex file path
	CString strTotalIndexFilePath;
	CString strImageFileName;
	
	GetLongMainName(RestoreInfo.szImageFile, strImageFileName.GetBuffer(MAX_PATH));
	strImageFileName.ReleaseBuffer();

	CString strTempDir;
	strTempDir=RestoreInfo.szTempDir;
	
	strTotalIndexFilePath= strTempDir +L"\\"+ strImageFileName + AM_INDEX_SUFFIX;
	CString strArrayFilePath;
	strArrayFilePath=strTempDir + L"\\" + strImageFileName + L".ary";

	IMAGE_HEAD ImageHead;
	if( FALSE == GetImageHead(RestoreInfo.szLastImageFile,&ImageHead) )
	{
		TRACE(L"\nGetImageHead error in RestoreFile.");
		return FALSE;
	}

	CURRENT_RESTORE_INFO CurrentInfo;
	memset(&CurrentInfo,0,sizeof(CurrentInfo));

	CurrentInfo.dwCurrentSource=ImageArray.GetUpperBound();
    CurrentInfo.hCurrentImage = CreateFile(RestoreInfo.szLastImageFile,
		                  GENERIC_READ,
						  FILE_SHARE_READ,
						  NULL,
					      OPEN_EXISTING,
					      FILE_ATTRIBUTE_NORMAL,
					      NULL);

	if(INVALID_HANDLE_VALUE == CurrentInfo.hCurrentImage )
	{
		TRACE(L"Can not open Last Image file.\n");
		return FALSE;
	}

    // open total index file (for temp use)
    HANDLE hTotalIndexFile;
    hTotalIndexFile=CreateFile( LPCTSTR(strTotalIndexFilePath), GENERIC_READ,
	                            FILE_SHARE_READ,NULL,
							    OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,
							    NULL);

	if( INVALID_HANDLE_VALUE == hTotalIndexFile)
	{
		TRACE(L"Can not open temp total Index file.\n");
		return FALSE;
	}

	TOTAL_INDEX_NODE TotalIndexNode;
	DWORD dwReadBytes;
	ReadFile(hTotalIndexFile,&TotalIndexNode,sizeof(TotalIndexNode),
		     &dwReadBytes,NULL);
	
	// reset the file pointer to file begin
	YGSetFilePointer(hTotalIndexFile,0,FILE_BEGIN,lFilePointer);

	// open array file (for temp use)
    HANDLE hArrayFile;
    hArrayFile=CreateFile( LPCTSTR(strArrayFilePath), GENERIC_READ,
	                       FILE_SHARE_READ,NULL,
					       OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,
						   NULL);

	if( INVALID_HANDLE_VALUE == hArrayFile )
	{
		TRACE(L"Can not open temp array file.\n");
		return FALSE;
	}

	YGSetFilePointer(hArrayFile,sizeof(ARRAY_NODE),FILE_BEGIN,lFilePointer);
	ARRAY_DATA ArrayData;

	DWORD dwCurrentPathID;

	for(dwCurrentPathID=1; dwCurrentPathID<=TotalIndexNode.dwTotalPathCount ; dwCurrentPathID++)
	{
		ReadFile(hArrayFile,&ArrayData,sizeof(ArrayData),&dwReadBytes,NULL);

		CString OnePathString;
		OnePathString=ArrayData.szFilePath;

		OnePathString.TrimRight(L"\\");

		// dwPathID=i+1; // this is dwPawthID
    
		bSelectedPath=bIsFileSelected((LPCTSTR)OnePathString,RestoreInfo);
	
		if(bSelectedPath)
		{
			//-- second added begin
			if(g_RestoreInfo.nTargetType==RESTORE_NEW_PLACE)
			{
				if( wcslen(g_RestoreInfo.szTargetDir) < 2)
				{
					AfxMessageBox(IDS_INVALID_TARGET_DIR,MB_OK|MB_ICONINFORMATION,NULL);
					return FALSE;
				}
				else if( bPathExist(g_RestoreInfo.szTargetDir)==FALSE )  //bPathExist can not judge j:\ exist or not, now think it exist
				{
					AfxMessageBox(IDS_INVALID_TARGET_DIR,MB_OK|MB_ICONINFORMATION,NULL);
					return FALSE;
				}
			}

			BOOL bValidPath;

			bValidPath=AdjustOnePathString(OnePathString,RestoreInfo,AdjustMailInfo);

			if(FALSE == bValidPath)
			{
				CString strInvalidTargetPath;
				strInvalidTargetPath.LoadString(IDS_INVALID_TARGET_PATH);
				AfxMessageBox(strInvalidTargetPath + L"  " + OnePathString,MB_OK|MB_ICONINFORMATION,NULL);
				return FALSE;
			}


			BOOL bNewPlace;
			  
		    if(RestoreInfo.nTargetType==RESTORE_ORIGINAL_PLACE)
			{			  
			     bNewPlace=FALSE;
			}
			else
			{
				 bNewPlace=TRUE;
			}

			BOOL bReturnValue;

			bReturnValue=FALSE;

			bReturnValue = RestoreOnePath( 
				                           (LPCTSTR)OnePathString,
										   &ImageHead,
				                           dwCurrentPathID,
				                           RestoreInfo.dwTimeStamp,
										   ImageArray,
				                           &CurrentInfo,
										   hTotalIndexFile,
										   bNewPlace
										  );
			
			if( FALSE == bReturnValue )
			{
				TRACE(L"\nRestoreOnePath error in RestorFile.");
				CloseHandle(CurrentInfo.hCurrentImage);
	            CloseHandle(hTotalIndexFile);
	            CloseHandle(hArrayFile);
				return FALSE;
				// break;
			}
	
		}
	}

	CloseHandle(CurrentInfo.hCurrentImage);
	CloseHandle(hTotalIndexFile);
	CloseHandle(hArrayFile);

	return TRUE;
}


BOOL SetRestoreInfo(AM_RESTORE_INFO RestoreInfo)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	g_RestoreInfo=RestoreInfo;
    return TRUE;
}


BOOL RestoreOnePath( LPCTSTR OnePathString,
					 IMAGE_HEAD *pImageHead, // for encrypt
					 DWORD dwPathID,
					 DWORD dwTimeStampNumber,
					 CDATA_ARRAY  &ImageArray,
					 CURRENT_RESTORE_INFO *pCurrentInfo,
					 HANDLE hTotalIndexFile,
					 BOOL bNewPlace )
{

	__int64 lFilePointer;
	if(g_RestoreState.bRestoreCancel == TRUE)
	{
		TRACE(L"\nbRestoreCancel is TRUE in RestoreOnePath, restore quit.");
		return FALSE;
	}

	CString strOnePathString;
	strOnePathString=OnePathString;

	if( strOnePathString.GetLength() >= MAX_PATH )
	{
		TRACE(L"\nstrOnePathString is too long in RestoreOnePath");
		return FALSE;
	}

	// ImageArray have an empty data
	// so it has at least two elements.
	if(ImageArray.GetSize()<2)
	{
		TRACE(L"\nImageArray size too small in RestoreOnePath.");
		return FALSE;
	}
	
    DWORD dwReadBytes;
	TOTAL_INDEX_NODE TotalIndexNode;
	YGSetFilePointer(hTotalIndexFile,0,FILE_BEGIN,lFilePointer);
	ReadFile(hTotalIndexFile,&TotalIndexNode,sizeof(TotalIndexNode),&dwReadBytes,NULL);

    DWORD dwTotalIndexOffset;
  
	dwTotalIndexOffset=GetTotalIndexOffset(TotalIndexNode.dwTotalPathCount, dwTimeStampNumber, dwPathID);

	YGSetFilePointer(hTotalIndexFile,dwTotalIndexOffset,FILE_BEGIN,lFilePointer);

    // ReadFile(hTotalIndexFile,&dwPathID,sizeof(dwPathID),&dwReadBytes,NULL);
	INDEX_DATA IndexData;
	ReadFile(hTotalIndexFile,&IndexData,sizeof(IndexData),&dwReadBytes,NULL);
	
	if(IndexData.dwPathID>0 && IndexData.dwPathID <= TotalIndexNode.dwTotalPathCount ) // if IndexData.dwPathID is zero, then the file is not exist in this timestamp
	{
		HANDLE hFind;
		WIN32_FIND_DATA findFileData;
		BOOL bFound;

		if( (IndexData.FileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 )
		{
  			if( FALSE == RestoreOneDir( (LPCTSTR)OnePathString,IndexData) )
			{
				// RestoreOneDir error is not so important
				// so we needn't break the restore progress
				// we will not return FALSE here.
				TRACE(L"\nRestoreOneDir error in RestoreOnePath");
			}
		}
		else
		{

			// if it is a file

			hFind = FindFirstFile((LPCTSTR)OnePathString,&findFileData);

			if(hFind == INVALID_HANDLE_VALUE)
			{
				bFound=FALSE;
			}	
			else
			{
				bFound=TRUE;
			}

			FindClose(hFind);

			BOOL bCreateNewFile;

			bCreateNewFile=FALSE;

			if(FALSE == bNewPlace)
			{
				// if restore to the old place, always replace the original file.
				bCreateNewFile=TRUE;
			}
			else
			{
  				// if restore to a new place, replace the original file or not depends on others.

				if(FALSE == bFound)
				{
					bCreateNewFile=TRUE;
				}
				else
				{
		
					// if the target file exist, ask the user to make decision
			  
					// an init value
					bCreateNewFile=FALSE;

					if(g_OverwriteOption.bOverwriteAll == FALSE
						&& g_OverwriteOption.bOverwriteNone == FALSE)
					{
						int nReturnValueOne;
						int nReturnValueTwo;

						//-- ID_STRING_BEGIN
						CString strFileAlreadyExist;
						strFileAlreadyExist.LoadString(IDS_FILE_ALREADY_EXIST);
						nReturnValueOne = AfxMessageBox(strOnePathString + L" " + strFileAlreadyExist,MB_YESNOCANCEL|MB_ICONINFORMATION,NULL);
						//-- ID_STRING_END

						if( nReturnValueOne ==IDYES )
						{
							bCreateNewFile=TRUE;

							nReturnValueTwo=AfxMessageBox(IDS_OVERWRITE_ALL,MB_YESNOCANCEL|MB_ICONINFORMATION,NULL);
   			   
							if(nReturnValueTwo == IDYES )
							{
								g_OverwriteOption.bOverwriteAll=TRUE;
							}
							else if (nReturnValueTwo == IDNO )
							{
								g_OverwriteOption.bOverwriteAll=FALSE;
							}
						}
						else if(nReturnValueOne== IDNO)
						{
							bCreateNewFile=FALSE;
							nReturnValueTwo = AfxMessageBox(IDS_SKIP_ALL,MB_YESNOCANCEL|MB_ICONINFORMATION,NULL);
 		      
 							if( nReturnValueTwo == IDYES )
							{
								g_OverwriteOption.bOverwriteNone=TRUE;
							}
							else if(nReturnValueTwo == IDNO)
							{
								g_OverwriteOption.bOverwriteNone=FALSE;
							}
						} 
					}
					else if(g_OverwriteOption.bOverwriteAll==TRUE)
					{
						bCreateNewFile=TRUE;
					}
					else if(g_OverwriteOption.bOverwriteNone==TRUE)
					{
						bCreateNewFile=FALSE;
					}

				}
			}

			//-- now we always overwrite the old file.

			if(bCreateNewFile==TRUE)
			{
				if(
					FALSE == RestoreOneFile(OnePathString,
											pImageHead,
											hTotalIndexFile,
											TotalIndexNode.dwTotalPathCount, 
											dwPathID,  
											dwTimeStampNumber, 
											pCurrentInfo,
											ImageArray
											)
					)
				{
					// restore one path error, but we needn't break the 
					// restore process
					TRACE(L"\nRestoreOneFile Error in RestoreOnePath.");
					CString strMessage;

					// waiting for IDS_STRING begin
					strMessage = strMessage + L"Restore " + OnePathString + L" error, continue ?";
					if( IDOK != AfxMessageBox((LPCTSTR)strMessage,MB_YESNOCANCEL|MB_ICONINFORMATION,NULL) )
					{
						// return FALSE will break the total restore progress
						return FALSE;
					}
					// waiting for IDS_STRING end
				}
			}

			// if this is a file.
			//-- third time added begin
			//-- for progress bar

			wcsncpy(g_RestoreState.szCurrentFile,(LPCTSTR)OnePathString,MAX_PATH-1);
			g_nCurrentFileCount=g_nCurrentFileCount+1;

			if(g_nTotalFileCount>0)
			{
				g_RestoreState.wPercent=(WORD)(g_nCurrentFileCount * 100 / g_nTotalFileCount);
			}

			//-- third time added end

		}
	}
	else if( IndexData.dwPathID > TotalIndexNode.dwTotalPathCount )
	{
		TRACE(L"\nIndexData error in RestoreOnepath, dwPathID is greater than TotalIndexNode.dwTotalPathCount");
		return FALSE;
	}
  
	return TRUE;
}


/*==========================================================
 *
 * Fucntion Name: RestoreFindFile()
 * Purpose:       this function is used by find function
 *
 *
 *==========================================================*/

BOOL RestoreFindFile( LPCTSTR szImgFile,
					    LPCTSTR szTempDir,
				        DWORD dwPathID,
				        DWORD dwTimeStampNumber,
				        LPCTSTR szTargetFile )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CDATA_ARRAY SourceArray;

	IMAGE_HEAD ImageHead;
	if( FALSE == GetImageHead(szImgFile,&ImageHead) )
	{
		TRACE(L"\nGetImageHead error in RestoreFindFile.");
		return FALSE;
	}

	if(GetSourceArray(szImgFile,SourceArray)==FALSE)
	{
		return FALSE;
	}

	// get the temp TotalIndex file path
	CString strTotalIndexFilePath;
	CString strImageFileName;
	
	// here szImgFile is the last Image File.
	GetLongMainName(szImgFile,strImageFileName.GetBuffer(MAX_PATH));
	strImageFileName.ReleaseBuffer();

	CString strTempDir;
	// strTempDir=g_szTempDir;
	strTempDir=szTempDir;
	strTotalIndexFilePath= strTempDir +L"\\"+ strImageFileName + AM_INDEX_SUFFIX;

    // open total index file (for temp use)
    HANDLE hTotalIndexFile;
    hTotalIndexFile=CreateFile( LPCTSTR(strTotalIndexFilePath), GENERIC_READ,
	                            FILE_SHARE_READ,NULL,
							    OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,
							    NULL);

	if(hTotalIndexFile==INVALID_HANDLE_VALUE)
	{
		TRACE(L"Can not open temp total Index file.\n");
		return FALSE;
	}

	CURRENT_RESTORE_INFO CurrentInfo;
	CurrentInfo.dwCurrentSource=0;
	CurrentInfo.hCurrentImage=NULL;

	// if(GetRequiredHandle(&CurrentInfo,1,SourceArray)==FALSE)
	// {
		// return FALSE;
	// }

	// bNewPlace is FALSE, then it always
	// overwrite the original file.
	// since we have judge replace the file in the UI.

	RestoreOnePath(szTargetFile,&ImageHead,dwPathID,dwTimeStampNumber,
		SourceArray,&CurrentInfo,hTotalIndexFile,FALSE);
  
    CloseHandle(hTotalIndexFile);
    CloseHandle(CurrentInfo.hCurrentImage);

    return TRUE;
}


BOOL CheckOutlookExpressDir(AM_RESTORE_INFO RestoreInfo, ADJUST_MAIL_INFO &AdjustMailInfo)
{
	CStringArray SelectionArray;
	CString strOutlookExpressPath;

	int nArrayIndex;
	nArrayIndex=0;
	
	memset(&AdjustMailInfo,0,sizeof(AdjustMailInfo));

	if( OE_MAIL_BACKUP == RestoreInfo.wImageType ||
		WIN_MAIL_BACKUP == RestoreInfo.wImageType)
	{
		if( RESTORE_ENTIRE == RestoreInfo.nRestoreType )
		{
			if( FALSE == GetSelectionArrayPro(RestoreInfo.szLastImageFile,SelectionArray) )
			{
				TRACE(L"\nGetSelectionArrayPro error in CheckOutlookExpressDir.");
				return FALSE;
			}

			// now this may be an Outlook Express Mail Backup
			strOutlookExpressPath=SelectionArray[0];

			wcsncpy(AdjustMailInfo.szOldOEPath,(LPCTSTR)strOutlookExpressPath,MAX_PATH-1);
			
			AdjustMailInfo.bAdjustMailPath = FALSE;
			
			if( -1 != GetFileAttributes(AdjustMailInfo.szOldOEPath) )
			{
				// if the directory exist, needn't adjust
				AdjustMailInfo.bAdjustMailPath = FALSE;
				return TRUE;
			}
			else
			{
				// if the directory not exist
				if( IDYES == AfxMessageBox(IDS_OE_DIR_NOT_EXIST,MB_YESNOCANCEL|MB_ICONINFORMATION,NULL) )
				{
					if( IDYES == AfxMessageBox(IDS_ASK_OVERWRITE_OE,MB_YESNOCANCEL,NULL))
					{
						WCHAR szDataPath[MAX_PATH];
						WCHAR szAddressPath[MAX_PATH];
						if( FALSE == NewGetOutlookExpressDataPath(szDataPath,szAddressPath) )
						{
							AfxMessageBox(IDS_OE_DIR_NOT_FOUND,MB_OK|MB_ICONINFORMATION,NULL);
							TRACE(L"\nNewGetOutlookExpressDataPath error in CheckOutlookExpressDir.");
							return FALSE;
						}
						if (GetOsVersion())
						{
//							wcscat(szDataPath,L"\\Local Folders");
							wcscat(szAddressPath,L"\\Contacts");
						}

						AdjustMailInfo.bAdjustMailPath = TRUE;
						wcsncpy(AdjustMailInfo.szNewOEPath,szDataPath,MAX_PATH-1);
						return TRUE;
					}
				}
			}
		}
		else
		{
			TRACE(L"\nMail Restore require restore total but not one part.");
			return FALSE;
		}
	}

	return TRUE;
}

BOOL GetRestoreState(RESTORE_STATE *pRestoreState)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pRestoreState=g_RestoreState;
	return TRUE;
}

BOOL SetRestoreState(RESTORE_STATE RestoreState)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	g_RestoreState=RestoreState;
	return TRUE;
}

/*==================================================================
 *
 * Function Name: RestoreOneFile()
 * Purpose:       Restore one file to the target path
 * note:          This function will overwrite the target file
 *                Asking user for overwrite should be done
 *                before invoke this function
 *
 *==================================================================*/

BOOL RestoreOneFile(
					 LPCTSTR               szOnePathString,
					 IMAGE_HEAD			   *pImageHead, // for encrypt
				     HANDLE	               hTotalIndexFile,
				     DWORD                 dwTotalPathCount, 
				     DWORD                 dwPathID, 
				     DWORD                 dwTimeStampNumber, 
				     CURRENT_RESTORE_INFO  *pCurrentInfo,
				     const CDATA_ARRAY     &ImageArray
			       )
{
	__int64    lFilePointer;
	INDEX_DATA IndexData;
	memset(&IndexData,0,sizeof(IndexData));

	// now the data file pointer is the total file pointer
	GetDataFilePointer( hTotalIndexFile,
			               dwTotalPathCount, 
			               dwPathID,  
			               dwTimeStampNumber, 
						   &IndexData );

	if(IndexData.wStatusFlag == FILE_NEW || IndexData.wStatusFlag == FILE_CHANGE )
	{
		PrepareOnePath(szOnePathString);

		if(IndexData.dwFileNumber  != (*pCurrentInfo).dwCurrentSource )
		{
			if(GetRequiredHandle(pCurrentInfo,IndexData.dwFileNumber ,ImageArray)==FALSE)
			{
				// then the file can not be resored
				// we delete the partially restored file
				TRACE(L"\nGetRequiredHandle error in RestoreOneFile.");
				return FALSE;
			}
		}

		YGSetFilePointer( (*pCurrentInfo).hCurrentImage ,IndexData.DataPointer.QuadPart,FILE_BEGIN,lFilePointer);

		//-- new one added end

		// to overwrite read only files.
		SetFileAttributes(szOnePathString,FILE_ATTRIBUTE_NORMAL);

		HANDLE hFileDestination;
	    hFileDestination=CreateFile( szOnePathString,
			                            GENERIC_READ|GENERIC_WRITE,
		                                FILE_SHARE_READ,NULL,
		                                CREATE_ALWAYS,
		                                FILE_ATTRIBUTE_NORMAL,NULL);

		if( INVALID_HANDLE_VALUE == hFileDestination )
		{
			TRACE(L"\nError to open target file.");
			return FALSE;
		}
			   
		// AM01 Version 2 modify
		BYTE * CopyString;
		CopyString = (BYTE *)malloc(AM_COMPRESS_BUFFER);

 	    if( CopyString == NULL )
		{
			AfxMessageBox(IDS_NOT_ENOUGH_MEMORY,MB_OK|MB_ICONINFORMATION,NULL);
			CloseHandle(hFileDestination);
			hFileDestination=NULL;
		    return FALSE;
		}

		// AM01 Version 2 modify
		BYTE *pDataBuffer;
		  
		pDataBuffer = (BYTE *)malloc(AM_BUFFER_SIZE);
			
		if(NULL == pDataBuffer)
		{
			AfxMessageBox(IDS_NOT_ENOUGH_MEMORY,MB_OK|MB_ICONINFORMATION,NULL);
			free(CopyString);
			CopyString = NULL;
			CloseHandle(hFileDestination);
			hFileDestination=NULL;
			return FALSE;
		}

			
		ULARGE_INTEGER qwFileLength;
		qwFileLength.HighPart=IndexData.FileInfo.nFileSizeHigh;
		qwFileLength.LowPart=IndexData.FileInfo.nFileSizeLow;
		DATA_NODE DataNode;

		DWORD dwWrittenBytes;

        unsigned __int64 BytesLeft;
           
		BytesLeft=qwFileLength.QuadPart;

		while(BytesLeft>0)
		{
			if(ReadDataFile(pCurrentInfo,ImageArray,&DataNode,sizeof(DataNode))==FALSE)
			{
				free(CopyString);
				CopyString=NULL;
				   
				free(pDataBuffer);
				pDataBuffer=NULL;
				   
				CloseHandle(hFileDestination);
				hFileDestination=NULL;

				return FALSE;
			}
			if( DataNode.dwPacketSize > AM_COMPRESS_BUFFER)
			{
			    //-- for debug
			    TRACE(L"Data File Error, Packet Size too large.");
			    //-- end for debug

				free(CopyString);
				CopyString=NULL;
				   
				free(pDataBuffer);
				pDataBuffer=NULL;
				   
				CloseHandle(hFileDestination);
				hFileDestination=NULL;
			       
				return FALSE;

			}

			if(ReadDataFile(pCurrentInfo,ImageArray,CopyString,(DataNode.dwPacketSize))==FALSE)
			{
	  			free(CopyString);
				CopyString=NULL;
				   
				free(pDataBuffer);
				pDataBuffer=NULL;
				   
				CloseHandle(hFileDestination);
				hFileDestination=NULL;

				return FALSE;
			}

			if( AM_COMPRESS_LEVEL == DataNode.wCompressLevel )
			{
				
				// 2004.05.20 for encrypt begin
				if(pImageHead->bEncrypt)
				{
				    // 2004.05.26 added begin
				    WCHAR szPassword[PASSWORD_SIZE];
				    wcsncpy(szPassword,pImageHead->pchPassword,PASSWORD_SIZE);
				    DecryptPassword(szPassword,PASSWORD_SIZE);
				    // 2004.05.26 added end

                    if( FALSE == YGDecryptData(szPassword,CopyString,DataNode.dwPacketSize) )
					{
						TRACE(L"\nYGDecryptData error in RestoreOneFile.");

						free(CopyString);
						CopyString=NULL;
						   
						free(pDataBuffer);
						pDataBuffer=NULL;
						   
						CloseHandle(hFileDestination);
						hFileDestination=NULL;

						return FALSE;
					}
				}
				// 2004.05.20 for encrypt end

				// AM01 version 2, the data is compressed
				DWORD dwDataLen;
				
				// the BufferSize of the uncompressed data
				dwDataLen = AM_BUFFER_SIZE;

#ifdef FEIX_TEST
				if (pDataBuffer && dwDataLen != 0)
				{
					free(pDataBuffer);
					dwDataLen = 0;
					pDataBuffer = (BYTE*)malloc(DataNode.dwPacketSize);
					dwDataLen = DataNode.dwPacketSize;
					memcpy(pDataBuffer, CopyString, DataNode.dwPacketSize);
				}
#else
				if( Z_OK != uncompress(pDataBuffer, &dwDataLen, CopyString,DataNode.dwPacketSize) )
				{
					TRACE(L"\n uncompress data error.");
					free(CopyString);
					CopyString=NULL;

					free(pDataBuffer);
					pDataBuffer=NULL;

					CloseHandle(hFileDestination);
					hFileDestination=NULL;
					return FALSE;
				}
#endif

				
				WriteFile(hFileDestination,pDataBuffer,dwDataLen,
			              &dwWrittenBytes,NULL);

				BytesLeft=BytesLeft-dwDataLen;
			}
			else if( 0 == DataNode.wCompressLevel )
			{
				 // AM01 version 1, the data is not compressed
				WriteFile(hFileDestination,CopyString,(DataNode.dwPacketSize),
			          &dwWrittenBytes,NULL);
				BytesLeft=BytesLeft-DataNode.dwPacketSize;
			}
			else
			{
				// ReadImage File Error.
				TRACE(L"\nRead Image File Error, Compress Level error.");

				free(CopyString);
				CopyString=NULL;
					   
				free(pDataBuffer);
				pDataBuffer=NULL;

			    CloseHandle(hFileDestination);
				hFileDestination=NULL;
				  
				DeleteFile( szOnePathString );

				return FALSE;
			}

			if(g_RestoreState.bRestoreCancel == TRUE)
			{
 				free(CopyString);
				CopyString=NULL;
					   
				free(pDataBuffer);
				pDataBuffer=NULL;

			    CloseHandle(hFileDestination);
				hFileDestination=NULL;
				  
				DeleteFile( szOnePathString );

				return FALSE;
			}
		} // end while ( BytesLeft > 0 )

		free(CopyString);
		CopyString = NULL;

		free(pDataBuffer);
		pDataBuffer = NULL;

		SetFileTime(hFileDestination,
		           &(IndexData.FileInfo.ftCreationTime),
		           &(IndexData.FileInfo.ftLastAccessTime),
				   &(IndexData.FileInfo.ftLastWriteTime));


	    FlushFileBuffers(hFileDestination);

		CloseHandle(hFileDestination);

		SetFileAttributes(szOnePathString,IndexData.FileInfo.dwFileAttributes);
	}
	else if( DIRECTORY_NEW == IndexData.wStatusFlag || DIRECTORY_NEW == IndexData.wStatusFlag )
	{
		TRACE(L"\nThe PathID is a directory.");
	}
   	else
	{
		TRACE(L"\nGetDataFilePointer did not get a valid IndexData, the file status is not new or changed.");
  		return FALSE;
	}
	return TRUE;
}

BOOL bPathExist(LPCTSTR OnePath)
{
	 AFX_MANAGE_STATE(AfxGetStaticModuleState());

	 // if( 2 == wcslen(OnePath) ) // if it is a disk driver
	 // {
		  // return TRUE;
	 // }
	 
	 DWORD  dwFileAttribute;
	 dwFileAttribute = GetFileAttributes(OnePath);

	 if( 
		 ( -1 !=  dwFileAttribute ) &&
		 ( 0  != (FILE_ATTRIBUTE_DIRECTORY & dwFileAttribute ) )
	   )
	 {
		 return TRUE;
	 }

	 return FALSE;

}

BOOL CreateOneDir(LPCTSTR szOneDir)
{
	
	CString strOnePath;
	strOnePath=szOneDir;

	WCHAR szLeftPart[MAX_PATH];
	memset(szLeftPart,0,sizeof(szLeftPart));

	if(strOnePath.GetLength()>=2) // the size of one dir is at lease two characters, e.g k:
	{
	 if( bPathExist(LPCTSTR(strOnePath)) == TRUE )
	 {
		 return TRUE;
	 }
	 else
	 {

		 CStringArray SubPathArray;

		 while(
				(strOnePath.GetLength()>2)  && 
				( bPathExist( (LPCTSTR) strOnePath )== FALSE)
			  )
		 {
			 SubPathArray.Add(strOnePath);
			 GetLeftPart(strOnePath,szLeftPart);
			 strOnePath=szLeftPart;
		 }

		 for(int i=SubPathArray.GetSize()-1; i>=0; i--)
		 {
			if( FALSE == CreateDirectory((LPCTSTR)(SubPathArray.ElementAt(i)),NULL) )
			{
				TRACE(L"\nCreateDirectory error in PrepareOnePath");
				return FALSE;
			}
		 }
	 }
	 return TRUE;
	}
	else
	{
	   return FALSE;
	}
}

BOOL PrepareOnePath(LPCTSTR szOnePath)
{

	 CString strOnePath;
	 
	 strOnePath=szOnePath;

     CString strStoreOnePath;

	 WCHAR szLeftPart[MAX_PATH];
     GetLeftPart( (LPCTSTR) strOnePath,szLeftPart);

	 return(CreateOneDir(szLeftPart));
}

BOOL RestoreOneDir( LPCTSTR	szDirPath, INDEX_DATA IndexData)
{

    // Get the required parent directory
	if (FALSE == PrepareOnePath(szDirPath) )
	{
		TRACE(L"\nPrepareOnePath error in RestoreOneDir.");
		return FALSE;
	}

	BOOL bFound;

	WIN32_FIND_DATA findFileData;
	
	memset(&findFileData,0,sizeof(findFileData));

	HANDLE hFind;
         
	hFind = FindFirstFile(szDirPath,&findFileData);

    if(hFind == INVALID_HANDLE_VALUE)
	{
	   bFound=FALSE;
	}
	else
	{
	   bFound=TRUE;
	}

	 FindClose(hFind);

	 if (bFound==FALSE)
	 {
		// if the directory is not exist, we create the directory and set the time
		// if the directory has already exist, we do not touch it.

	    if( FALSE == CreateDirectory(szDirPath,NULL) )
		{
			TRACE(L"CreateDirectory error in RestoreOneDir.");
			return FALSE;
		}

		SetFileAttributes(szDirPath,IndexData.FileInfo.dwFileAttributes);

		HANDLE hDir;
		hDir = CreateFile (szDirPath, 
			              GENERIC_READ|GENERIC_WRITE,
						  FILE_SHARE_READ,
						  NULL,
						  OPEN_EXISTING,
					      FILE_FLAG_BACKUP_SEMANTICS,
						  NULL);

		if( INVALID_HANDLE_VALUE == hDir)
		{
			// CreateDir is important, if set file time error
			// we only trace a message, do not return false.
			TRACE(L"\nOpen hDir error in function RestoreOneDir.");
		}
		

		if( FALSE == SetFileTime(hDir,
			                     &(IndexData.FileInfo.ftCreationTime),
			                     &(IndexData.FileInfo.ftLastAccessTime),
			                     &(IndexData.FileInfo.ftLastWriteTime) ) )
		{
			TRACE(L"\nSetFileTime error in RestoreOneDir.");
		}

		if( INVALID_HANDLE_VALUE != hDir )
		{
		   CloseHandle(hDir);
		}
	 }

	 return TRUE;
}


BOOL AdjustOnePathString(CString &OnePathString, AM_RESTORE_INFO RestoreInfo, ADJUST_MAIL_INFO AdjustMailInfo )
{
	if(OE_MAIL_BACKUP == RestoreInfo.wImageType ||
		WIN_MAIL_BACKUP == RestoreInfo.wImageType)
	{
		if(AdjustMailInfo.bAdjustMailPath)
		{
			if(
				( 0 == OnePathString.Find(AdjustMailInfo.szOldOEPath) ) &&
				( 0 != OnePathString.CompareNoCase(AdjustMailInfo.szOldOEPath) )// the directory name is not need to restore
		  	  )
			{
				// find the path and adjust it.
				WCHAR szRightPart[MAX_PATH];
				if( FALSE == GetRightPart((LPCTSTR)OnePathString,szRightPart) )
				{
					TRACE(L"\nGetRightPart error in AdjustOnePathString.");
					return FALSE;
				}
				OnePathString.Empty();
				OnePathString=OnePathString+AdjustMailInfo.szNewOEPath;
				OnePathString.TrimRight(L"\\");
				OnePathString=OnePathString+L"\\"+szRightPart;
				return TRUE;
			}
			else if ( 0 == OnePathString.CompareNoCase(AdjustMailInfo.szOldOEPath) ) // the directory name is not need to restore
			{
				OnePathString=AdjustMailInfo.szNewOEPath;
			}
			else
			{
				// perhaps this is a address path.
				// we need not adjust it.
			}
		}
	}
	
	// if restore to original place , needn't adjust
	if(RestoreInfo.nTargetType==RESTORE_ORIGINAL_PLACE)
	{
		return TRUE;
	}

	// if restore to new place
	CString strTargetDir;
	
	// L"k:\" have "\\" in the end
	// L"k:\try" have not "\\" in the end
	// we just TrimRight L"\\" to deal with it.

	strTargetDir=RestoreInfo.szTargetDir;

	strTargetDir.TrimRight(L"\\");

	if(RestoreInfo.nRestoreType==RESTORE_ENTIRE)
	{
		OnePathString=strTargetDir + L"\\" 
			          +L"Disk"+OnePathString.Left(1) + L"\\"
					  +OnePathString.Mid(3);
	}
	else if(RestoreInfo.nRestoreType==RESTORE_PART)
	{
		CString strSelectedPath;
		strSelectedPath=RestoreInfo.szSelectedPath;
		strSelectedPath.TrimRight(L"\\");

		if(strSelectedPath.GetLength()==2) // for example L"k:"
		{
		    OnePathString=strTargetDir + L"\\" 
			              +L"Disk"+OnePathString.Left(1) + L"\\"
					      +OnePathString.Mid(3);
		}
		else if(strSelectedPath.GetLength()>2)
		{
			int nIndex;
			nIndex=strSelectedPath.ReverseFind(L'\\');
			OnePathString=strTargetDir+L"\\"
				          +OnePathString.Mid(nIndex+1);
		}
	}
	
	if(OnePathString.GetLength() > (MAX_PATH-1) || 
	   OnePathString.GetLength() <= 3 ) // k:\ have 3 letters
	{
		TRACE(L"\nOnePathString error in AdjustOnePathString.");
		return FALSE;
	}
	
	return TRUE;
}
