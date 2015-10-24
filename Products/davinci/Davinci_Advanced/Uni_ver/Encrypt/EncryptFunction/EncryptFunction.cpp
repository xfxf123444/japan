#ifdef _FILE_ENCRYPT
#include "..\FileEncrypt\stdafx.h"
#include "..\FileEncrypt\resource.h"
#endif

#ifdef _SHELL_ENCRYPT
#include "..\ShellEncrypt\stdafx.h"
#include "..\ShellEncrypt\resource.h"
#endif

#ifdef _YG_FS_MON
#include "..\..\YGFSMon\stdafx.h"
#include "..\..\YGFSMon\resource.h"
#endif

#ifdef _YG_COMMAND_LINE
#include "..\..\FED\stdafx.h"
//#include <IOSTREAM.H>
#include "..\..\FED\resource.h"
#endif

#include "..\EncryptFunction\EncryptFunction.h"
#include "..\..\DavinciFunction\DavinciFunction.h"
#include "FileStatisticInfo.h"
#include "EncryptInfo.h"


BOOL g_bEncryptCancel;
BOOL g_bEncryptUserCancel;

extern DA_WORK_STATE g_WorkState;
extern BYTE g_byBuffer[ENCRYPT_BUFFER_SIZE+ENCRYPT_BUFFER_SIZE/5];
extern BYTE g_byBufferUnCompress[ENCRYPT_BUFFER_SIZE];

BOOL EncryptSelection(CEncryptInfo &EncryptInfo)
{
	CString strText,strTitle;
	g_bEncryptCancel = FALSE;
	g_bEncryptUserCancel = FALSE;

	memset(&g_WorkState,0,sizeof(g_WorkState));

	CFileStatisticInfo FileStatisticInfo;
	FileStatisticInfo.CountTotalFileSize(EncryptInfo.m_strSourceArray);

	g_WorkState.qwTotalFileSize = FileStatisticInfo.m_qwTotalFileSize;
	g_WorkState.dwTotalFileCount = FileStatisticInfo.m_dwTotalFileCount;
	g_WorkState.qwCurrentWriteSize = 0;

	if( 0 == EncryptInfo.m_strSourceArray.GetSize() )
	{
		TRACE(L"\nNo item to be encrypted.");
		return FALSE;
	}

	ENCRYPT_FILE_HANDLE FileHandle;
	
	memset(&FileHandle,0,sizeof(FileHandle));

	if( FALSE == GetFileHandle((LPCTSTR)(EncryptInfo.m_strTarget),FileHandle ) )
	{
		strText.LoadString(IDS_CREATE_IMAGE_ERROR);
#ifndef _YG_COMMAND_LINE
		strTitle.LoadString(IDS_APP_NAME);
	    MessageBox(GetFocus(),strText,strTitle,MB_OK | MB_ICONWARNING);
#else
		wcout << (LPCTSTR)strText << endl;
#endif
		return FALSE;
	}

	if( FALSE == WriteTargetHead(FileHandle.hImageFile,(LPCTSTR)EncryptInfo.m_strPassword,EncryptInfo.m_dwCompressLevel,&EncryptInfo.m_EncryptOption))
	{
		TRACE(L"\nWriteTargetHead error in EncryptSelection.");
		return FALSE;
	}

	// 2004.09.20 added begin
	if( FALSE == RecordSelectionArray(FileHandle.hImageFile,(LPCTSTR)(EncryptInfo.m_strPassword),EncryptInfo.m_strSourceArray) )
	{
		TRACE(L"\nRecordSelectionArray error in EncryptSelection.");
	}
	// 2004.09.20 added end

	// here to start backup file

	DWORD dwPathID;
	dwPathID = 0; // while traveling the directory,
	              // we should add the dwPathID


	// rem ****************************************
	// in the future we should StoreExtraDirInfo

	// to help the restore program show the tree
	if( FALSE == StoreExtraDirInfo(EncryptInfo.m_strSourceArray,FileHandle.hTailFile,(LPCTSTR)(EncryptInfo.m_strPassword),dwPathID) )
	{
		TRACE(L"\nStoreExtraDirInfo error in EncryptSelection.");
	}

    // rem ****************************************

	long nArrayIndex;
	long nArraySize;
	nArraySize = EncryptInfo.m_strSourceArray.GetSize();

	for(nArrayIndex=0; nArrayIndex<nArraySize; nArrayIndex++)
	{
		if(g_bEncryptCancel || g_bEncryptUserCancel)
		{
			TRACE(L"\ng_bEncryptCancel or g_bEncryptUserCancel in EncryptSelection, in the for loop.");
			break;
		}

		CString OnePathString;
		OnePathString = (LPCTSTR)EncryptInfo.m_strSourceArray[nArrayIndex];

		// TRACE("\nEncrypt %s",(LPCTSTR)OnePathString);
		
		if( -1 == GetFileAttributes((LPCTSTR)OnePathString) )
		{
			TRACE(L"\n source %s not valid",(LPCTSTR)OnePathString);
			continue;
		}

		if( IsOneDirectory((LPCTSTR)OnePathString ) )
		{
			// if OnePathString is a directory
			if( FALSE == EncryptDirectory((LPCTSTR)OnePathString,FileHandle,(LPCTSTR)(EncryptInfo.m_strPassword),dwPathID,EncryptInfo.m_dwCompressLevel) )
			{
				TRACE(L"\nEncryptDirectory error in EncryptSelection.");
				g_bEncryptCancel=TRUE;
				break;
			}
		}
		else
		{
			// if OnePathString is a file
			if( FALSE == EncryptFile((LPCTSTR)OnePathString,FileHandle,(LPCTSTR)(EncryptInfo.m_strPassword),dwPathID,EncryptInfo.m_dwCompressLevel) )
			{
				CString strEncryptFileError;
				strEncryptFileError.Format(IDS_ENCRYPT_FILE_ERROR,(LPCTSTR)OnePathString);

				if( FALSE == g_bEncryptUserCancel )
				{
#ifndef _YG_COMMAND_LINE
					strTitle.LoadString(IDS_APP_NAME);
				    MessageBox(GetFocus(),strEncryptFileError,strTitle,MB_OK | MB_ICONWARNING);
#else
					wcout << (LPCTSTR)strEncryptFileError << endl;
#endif
				}
				else
				{
					// if the user cancels, we do not report the previous message
					TRACE(L"\nThe user cancel while encrypting %s",(LPCTSTR)OnePathString);
				}

				g_bEncryptCancel=TRUE; // return FALSE means cancel backup
				break;
			}
		} // end else
	} // end for

	CString strTailFile;
	GetTailFileName((LPCTSTR)EncryptInfo.m_strTarget,strTailFile);

	if( g_bEncryptCancel || g_bEncryptUserCancel )
	{
		TRACE(L"\ng_bEncryptCancel or g_bEncryptUserCancel in EncryptSelection.");
        CloseFileHandle(FileHandle);
		SetFileAttributes(strTailFile,FILE_ATTRIBUTE_NORMAL);
	    DeleteFile((LPCTSTR)strTailFile);
		SetFileAttributes(EncryptInfo.m_strTarget,FILE_ATTRIBUTE_NORMAL);
		DeleteFile((LPCTSTR)EncryptInfo.m_strTarget);
		return FALSE;
	}	 

    if( FALSE == CombineFileToOneImg(FileHandle,dwPathID,FileStatisticInfo.m_dwTotalFileCount,(LPCTSTR)EncryptInfo.m_strPassword) )
	{
		TRACE(L"\nCombineFileToOneImg error in EncryptSelection.");
		// we may delete the original file
		// after we finished encryption, so if there is something wrong
		// we SHOULD return false.
		// then the source will not be deleted.
		CloseFileHandle(FileHandle);
		SetFileAttributes(strTailFile,FILE_ATTRIBUTE_NORMAL);
		DeleteFile((LPCTSTR)strTailFile);
		return FALSE;
	}

    if( FALSE == CloseFileHandle(FileHandle) )
	{
		// just for notice
		TRACE(L"\nCloseFileHandle error in EncryptSelection.");
		SetFileAttributes(strTailFile,FILE_ATTRIBUTE_NORMAL);
		DeleteFile((LPCTSTR)strTailFile);
		return FALSE;
	}

	SetFileAttributes(strTailFile,FILE_ATTRIBUTE_NORMAL);
	if( FALSE == DeleteFile((LPCTSTR)strTailFile) )
	{
		TRACE(L"\nDelete tail file error.");
		return FALSE;
	}
	return TRUE;
}



BOOL GetFileHandle(LPCTSTR szImageFile, ENCRYPT_FILE_HANDLE &FileHandle)
{
	CString strText,strTitle;
	DWORD dwFileAttribute;

	dwFileAttribute = GetFileAttributes(szImageFile);

	if( -1 != dwFileAttribute)
	{
		if( 0 == ( dwFileAttribute & FILE_ATTRIBUTE_DIRECTORY ) )
		{
		   // if this is a file
		    CString strAskForOverwrite;
		    strAskForOverwrite.Format(IDS_ASK_FOR_OVERWRITE,szImageFile);
#ifndef _YG_COMMAND_LINE
			strTitle.LoadString(IDS_APP_NAME);
		    if( IDYES != MessageBox(GetFocus(),strAskForOverwrite,strTitle,MB_YESNO | MB_ICONQUESTION) )
			{
			    TRACE(L"\nThe user choose not to overwrite the target file.");
			    return FALSE;
			}
#else
CMDAskForChoice1:
		    wcout << (LPCTSTR)strAskForOverwrite << endl;
			wcin.getline(strText.GetBuffer (100), 100); 
			strText.ReleaseBuffer();
			if (strText.CompareNoCase(L"Y"))
			{
				if (!strText.CompareNoCase(L"N"))
				{
					TRACE(L"\nThe user choose not to overwrite the target file.");
					return FALSE;
				}
				else 
				{
					goto CMDAskForChoice1;
				}
			}
#endif
		}
		else
		{
			// if this is a directory
			// it is impossible to overwrite a directory by a file
			TRACE(L"\nCan not overwrite a directory.");
			return FALSE;
		}
	}

	FileHandle.hImageFile = CreateFile(szImageFile,
		                               GENERIC_READ|GENERIC_WRITE,
									   FILE_SHARE_READ | FILE_SHARE_WRITE,
									   NULL,
									   CREATE_ALWAYS,
									   FILE_ATTRIBUTE_NORMAL,
									   NULL);

	if( INVALID_HANDLE_VALUE == FileHandle.hImageFile)
	{
		TRACE(L"\nCan not open target image file.");
		return FALSE;
	}

	// get the tail file name, the tail file is a temp file

	CString strTailFile;
	strTailFile = szImageFile;

	GetTailFileName(szImageFile,strTailFile);
	
	dwFileAttribute = GetFileAttributes((LPCTSTR)strTailFile);
	if( -1 != dwFileAttribute )
	{
		if( 0 == ( dwFileAttribute & FILE_ATTRIBUTE_DIRECTORY ) )
		{
		   // if this is a file
		   CString strAskForOverwrite;
		   strAskForOverwrite.Format(IDS_ASK_FOR_OVERWRITE,(LPCTSTR)strTailFile);
#ifndef _YG_COMMAND_LINE
			strTitle.LoadString(IDS_APP_NAME);
		   if( IDYES != MessageBox(GetFocus(),strAskForOverwrite,strTitle,MB_YESNO | MB_ICONQUESTION) )
		   {
			   TRACE(L"\nThe user choose not to overwrite the target file.");
			   return FALSE;
		   }
#else
CMDAskForChoice2:
		    wcout << (LPCTSTR)strAskForOverwrite << endl;
			wcin.getline(strText.GetBuffer (100), 100); 
			strText.ReleaseBuffer();
			if (strText.CompareNoCase(L"Y"))
			{
				if (!strText.CompareNoCase(L"N"))
				{
					TRACE(L"\nThe user choose not to overwrite the target file.");
					return FALSE;
				}
				else
				{
					goto CMDAskForChoice2;
				}
			}
#endif
		}
		else
		{
			// if this is a directory
			// it is impossible to overwrite a directory by a file
			TRACE(L"\nCan not overwrite a directory.");
			return FALSE;
		}
	}

	FileHandle.hTailFile = CreateFile( (LPCTSTR)strTailFile,
		                                GENERIC_READ|GENERIC_WRITE,
										FILE_SHARE_READ | FILE_SHARE_WRITE,
										NULL,
										CREATE_ALWAYS,
										FILE_ATTRIBUTE_NORMAL,
										NULL);

	if( INVALID_HANDLE_VALUE == FileHandle.hTailFile )
	{
		TRACE(L"\nCan not open tail file.");
		CloseHandle(FileHandle.hImageFile);
		FileHandle.hImageFile = NULL;
		return FALSE;
	}

	return TRUE;
}

BOOL CloseFileHandle(ENCRYPT_FILE_HANDLE &FileHandle)
{
	BOOL bReturnValue;
	bReturnValue = TRUE;
	
	if( FALSE == CloseHandle(FileHandle.hImageFile) )
	{
		bReturnValue = FALSE;
	}
	
	if( FALSE == CloseHandle(FileHandle.hTailFile) )
	{
		bReturnValue = FALSE;
	}

	FileHandle.hImageFile = NULL;
	FileHandle.hTailFile = NULL;

	return bReturnValue;
}


BOOL WriteTargetHead(HANDLE hTargetFile,LPCTSTR szPassword,DWORD dwCompressLevel,PENCRYPTOPTION pEncryptOption)
{
	FILE_ENCRYPT_HEAD FileEncryptHead;
	memset(&FileEncryptHead,0,sizeof(FileEncryptHead));
	wcsncpy(FileEncryptHead.szImageIdentity,IMAGE_IDENTITY,IMAGE_IDENTITY_SIZE-1);
	FileEncryptHead.dwCompressLevel = dwCompressLevel;
	FileEncryptHead.dwFileVer = CURRENT_FILE_VERSION;
	FileEncryptHead.dwStructSize = sizeof(FILE_ENCRYPT_HEAD);
	memcpy(&FileEncryptHead.OptionInfo,pEncryptOption,sizeof(ENCRYPTOPTION));

	WCHAR chStoreKey[PASSWORD_SIZE];

	int nStoreSize;
	nStoreSize = wcslen(szPassword);

	if(nStoreSize > PASSWORD_SIZE )
	{
		nStoreSize = PASSWORD_SIZE;
	}

	memset(chStoreKey,0,sizeof(chStoreKey));
	char chPassword[PASSWORD_SIZE];
	ZeroMemory(chPassword, PASSWORD_SIZE);
	WideCharToMultiByte(CP_ACP, 0, szPassword, PASSWORD_SIZE, chPassword, PASSWORD_SIZE, 0, 0);
	// Encrypt key with key itself
	// may be this is single direction
	if( FALSE == YGAESEncryptData((unsigned char*)chPassword,strlen(chPassword),(unsigned char*)szPassword,(unsigned char*)chStoreKey,nStoreSize * sizeof(WCHAR)))
	{
		TRACE("\nYGAESEncryptData error in WriteTargetHead.");
		return FALSE;
	}

	int nKeySize;
	nKeySize = wcslen(szPassword);

	if(nKeySize>PASSWORD_SIZE)
	{
		nKeySize = PASSWORD_SIZE;
	}

	memcpy(FileEncryptHead.szPassword,chStoreKey,nKeySize*sizeof(WCHAR));
	
	return SetFileEncryptHead(hTargetFile,szPassword,FileEncryptHead,FALSE);
}

BOOL GetTailFileName(LPCTSTR szTargetFile, CString &strTailFile)
{
	CString strResult;
	strResult = szTargetFile;

	int nIndex;
	nIndex = strResult.ReverseFind(L'.');

	if(nIndex > 0 )
	{
	  // found
	  strTailFile = strResult.Left(nIndex) + L".ary";
	}
	else
	{
	  TRACE(L"\nAttention: source file name is not correct.");
	  strTailFile = strResult + L".ary";
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


BOOL EncryptDirectory(LPCTSTR szSourceDir,ENCRYPT_FILE_HANDLE FileHandle,LPCTSTR szPassword,DWORD &dwPathID,ULONG ulCompressLevel)
{

	TRACE(L"\nEncrypt Dir:%s",szSourceDir);

	// wait for proegress begin
	if( g_bEncryptCancel || g_bEncryptUserCancel )
	{
		return FALSE;
	}

	DWORD dwFileAttribute;
	dwFileAttribute = GetFileAttributes(szSourceDir);
	if( -1 == dwFileAttribute )
	{
		return FALSE;
	}
	if(  0 == (dwFileAttribute & FILE_ATTRIBUTE_DIRECTORY) )
	{
		// if it is a file.
		return FALSE;
	}

	if( FALSE == StoreDirectoryInfo(szSourceDir,FileHandle.hTailFile,szPassword,dwPathID) )
	{
		TRACE("\nStoreDirectoryInfo error in EncryptDirectory.");
		return FALSE;
	}

	// travel the directory

	CString strFind;
	strFind=strFind + szSourceDir + L"\\*.*";

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
		strSubPath=strSubPath + szSourceDir +L"\\"+ FindFileData.cFileName;

		if( 0 == ( FILE_ATTRIBUTE_DIRECTORY & FindFileData.dwFileAttributes )  && !CheckSpeicalFile(strSubPath) )
		{
			// this is the output
			// TRACE("\nFile Name:%s",(LPCTSTR)strSubPath);
			if( FALSE == EncryptFile( (LPCTSTR)strSubPath,FileHandle,szPassword,dwPathID,ulCompressLevel) )
			{
				TRACE(L"\nEncryptFile error in EcnryptDirectory.");
				return FALSE;
			}
		}
		else
		{
		    if( FALSE == EncryptDirectory((LPCTSTR)strSubPath,FileHandle,szPassword,dwPathID,ulCompressLevel) )
			{
				TRACE(L"\nEncrypt SubPath error in EncryptDirectory.");
				return FALSE;
			}
		}
	}while(FindNextFile(hFind,&FindFileData));

	FindClose(hFind);
	
	// wait for proegress end
	return TRUE;
}


BOOL EncryptFile(LPCTSTR szSourceFile,ENCRYPT_FILE_HANDLE FileHandle,LPCTSTR szPassword,DWORD &dwPathID,ULONG ulCompressLevel)
{
	TRACE(L"\nEncrypt File:%s",szSourceFile);
#ifdef _YG_COMMAND_LINE
	wcout << szSourceFile << endl;
#endif
	memset(g_WorkState.szCurrentFile,0,sizeof(g_WorkState.szCurrentFile));
	wcsncpy(g_WorkState.szCurrentFile,szSourceFile,MAX_PATH-1);

	// wait for proegress begin
	if( g_bEncryptCancel || g_bEncryptUserCancel )
	{
		return FALSE;
	}

	// check for valid source
	HANDLE hSourceFile;
	hSourceFile=CreateFile( szSourceFile,
                            GENERIC_READ,
                            FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,OPEN_EXISTING,
     					    FILE_ATTRIBUTE_NORMAL,0 );
	
	if(INVALID_HANDLE_VALUE == hSourceFile)
	{
		TRACE(L"\nCan not open source file: %s in EncryptFile",szSourceFile);
		return FALSE;
	}

	ULARGE_INTEGER qwTargetSize;
	qwTargetSize.LowPart=GetFileSize(FileHandle.hImageFile,&(qwTargetSize.HighPart));

    // Get Current DataPointer, we will record it in array File.
	
	LARGE_INTEGER DataFilePointer;

	if( FALSE == YGSetFilePointer(FileHandle.hImageFile,0,FILE_CURRENT,DataFilePointer.QuadPart) )
	{
		TRACE(L"\nYGSetFilePointer error in EncryptFile.");
		CloseHandle(hSourceFile);
		return FALSE;
	}

	// encrypt file
	DWORD dwReadBytes;
	dwReadBytes = 0;

	DWORD dwWrittenBytes;
	dwWrittenBytes = 0;

	if( FALSE == ReadFile(hSourceFile,g_byBufferUnCompress,sizeof(g_byBufferUnCompress),&dwReadBytes,NULL) )
	{
		TRACE(L"\nRead source file error in EncryptFile.");
		CloseHandle(hSourceFile);
		return FALSE;
	}

	while(dwReadBytes>0)
	{
		if(g_bEncryptCancel || g_bEncryptUserCancel)
		{
			TRACE(L"\ng_bEncryptCancel or g_bEncryptUserCancel in EncryptFile.");
		    CloseHandle(hSourceFile);
			return FALSE;
		}

		DATA_NODE DataNode;
		memset(&DataNode,0,sizeof(DataNode));
		DataNode.dwCompressLevel = ulCompressLevel;
		DataNode.dwOrigSize = dwReadBytes;
		memset(DataNode.btReserved,0,sizeof(DataNode.btReserved));

		switch (ulCompressLevel)
		{
		case LEVEL_FAST_COMPRESS:
			DataNode.dwPacketSize = ENCRYPT_BUFFER_SIZE+ENCRYPT_BUFFER_SIZE/5;
			if ( Z_OK != compress(g_byBuffer,&DataNode.dwPacketSize,
	         			g_byBufferUnCompress,dwReadBytes,4))
			{
			    CloseHandle(hSourceFile);
				return FALSE ; 
			}
			break;
		case LEVEL_HIGH_COMPRESS:
			DataNode.dwPacketSize = ENCRYPT_BUFFER_SIZE+ENCRYPT_BUFFER_SIZE/5;
			if ( Z_OK != compress(g_byBuffer,&DataNode.dwPacketSize,
	         			g_byBufferUnCompress,dwReadBytes,8))
			{
			    CloseHandle(hSourceFile);
				return FALSE ; 
			}
			break;
		case LEVEL_NO_COMPRESS:
		default:
			DataNode.dwPacketSize = dwReadBytes;
			DataNode.dwCompressLevel = LEVEL_NO_COMPRESS;
			memcpy(g_byBuffer,g_byBufferUnCompress,dwReadBytes);
			break;
		}

		char chPassword[PASSWORD_SIZE];
		ZeroMemory(chPassword, PASSWORD_SIZE);
		WideCharToMultiByte(CP_ACP, 0, szPassword, PASSWORD_SIZE, chPassword, PASSWORD_SIZE, 0, 0);
        if( FALSE == YGAESEncryptData((unsigned char*)chPassword,strlen(chPassword),g_byBuffer,g_byBuffer,DataNode.dwPacketSize) )
		{
			TRACE(L"\nYGAESEncryptData error in EncryptFile.");
		    CloseHandle(hSourceFile);
			return FALSE;
		}

		// write data node

		if( FALSE == WriteFile(FileHandle.hImageFile,&DataNode,sizeof(DataNode),&dwWrittenBytes,NULL) )
		{
			TRACE(L"\nWrite target file error in EncryptFile.");
		    CloseHandle(hSourceFile);
			return FALSE;
		}

		if( FALSE == WriteFile(FileHandle.hImageFile,g_byBuffer,DataNode.dwPacketSize,&dwWrittenBytes,NULL) )
		{
			TRACE(L"\nWrite target file error in EncryptFile.");
		    CloseHandle(hSourceFile);
			return FALSE;
		}

		g_WorkState.qwCurrentWriteSize = g_WorkState.qwCurrentWriteSize + dwReadBytes;

		if( g_WorkState.qwTotalFileSize > 0 )
		{
		   g_WorkState.nPercent = (int ) ( ( g_WorkState.qwCurrentWriteSize * 100 ) / ( g_WorkState.qwTotalFileSize ) );
		}
		else
		{
			TRACE(L"\ng_WorkState.qwTotalFileSize error in EncryptFile.");
		}

		if( FALSE == ReadFile(hSourceFile,g_byBufferUnCompress,sizeof(g_byBufferUnCompress),&dwReadBytes,NULL) )
		{
			TRACE(L"\nRead source file error in EncryptFile.");
		    CloseHandle(hSourceFile);
			return FALSE;
		}
	}

	// wait for proegress end
    CloseHandle(hSourceFile);

	// record the array information
	WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile(szSourceFile,&findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		TRACE(L"\nFindFirstFile error in EncryptFile.");
	    return FALSE;
	}
      
	VERIFY(FindClose(hFind));

	dwPathID = dwPathID + 1;

	ARRAY_DATA ArrayData;
	memset(&ArrayData,0,sizeof(ArrayData));
	ArrayData.dwFileAttribute = findFileData.dwFileAttributes;
	ArrayData.dwPathID = dwPathID;
	ArrayData.ftCreationTime = findFileData.ftCreationTime;
	ArrayData.ftLastAccessTime = findFileData.ftLastAccessTime;
	ArrayData.ftLastWriteTime = findFileData.ftLastWriteTime;
	ArrayData.qwFileOffset.QuadPart = DataFilePointer.QuadPart;
	ArrayData.qwFileSize.LowPart = findFileData.nFileSizeLow;
	ArrayData.qwFileSize.HighPart = findFileData.nFileSizeHigh;
	wcsncpy(ArrayData.szFileName,szSourceFile,MAX_PATH-1);

	// for debug temp comment begin
	// encrypt array data begin
	char chPassword[PASSWORD_SIZE];
	ZeroMemory(chPassword, PASSWORD_SIZE);
	WideCharToMultiByte(CP_ACP, 0, szPassword, PASSWORD_SIZE, chPassword, PASSWORD_SIZE, 0, 0);
    if( FALSE == YGAESEncryptData((unsigned char*)chPassword,strlen(chPassword),(unsigned char*)(&ArrayData),(unsigned char*)(&ArrayData),sizeof(ArrayData)) )
	{
		  TRACE(L"\nYGAESEncryptData error in WriteTargetTail."); 
		  return FALSE;
	}
	// encrypt array data end
	// for debug temp comment end

	if( FALSE == WriteFile(FileHandle.hTailFile,&ArrayData,sizeof(ArrayData),&dwWrittenBytes,NULL) )
	{
		TRACE(L"\nWriteFile error in EncryptFile (Write tail file error). ");
		return FALSE;
	}

	return TRUE;
}


BOOL CombineFileToOneImg(ENCRYPT_FILE_HANDLE &FileHandle,DWORD dwTotalPath, DWORD dwTotalFileCount,LPCTSTR szPassword)
{
	__int64 linTargetFilePointer;

	// init it with an invalid value
	linTargetFilePointer = -1;

	if( FALSE == YGSetFilePointer(FileHandle.hTailFile,0,FILE_BEGIN,linTargetFilePointer) )
	{
		TRACE(L"\n1. YGSetFilePointer error in CombineFileToOneImg.");
		return FALSE;
	}

	FILE_ENCRYPT_HEAD FileEncryptHead;
	if (GetFileEncryptHead(FileHandle.hImageFile,szPassword,FileEncryptHead) != CURRENT_FILE_VERSION)
		return FALSE;

	LARGE_INTEGER ArrayFilePointer;
	ArrayFilePointer.QuadPart=0;

    if( FALSE == YGSetFilePointer(FileHandle.hImageFile,0,FILE_END,ArrayFilePointer.QuadPart) ) 
	{
		TRACE(L"\n3. YGSetFilePointer error in CombineFileToOneImg.");
		return FALSE;
	}

	// adjust the file pointer part in the image head
	FileEncryptHead.FileInfoPointer.QuadPart = ArrayFilePointer.QuadPart;
	if (!SetFileEncryptHead(FileHandle.hImageFile,szPassword,FileEncryptHead,FALSE))
		return FALSE;


	// and the arary node
	ARRAY_NODE ArrayNode;
	memset(&ArrayNode,0,sizeof(ArrayNode));

	ArrayNode.dwPathCount = dwTotalPath;
	ArrayNode.dwFileCount = dwTotalFileCount;

	if( FALSE == YGSetFilePointer(FileHandle.hImageFile,0,FILE_END,linTargetFilePointer) )
	{
		TRACE(L"\n5. YGSetFilePointer error in CombineFileToOneImg.");
		return FALSE;
	}
	
	DWORD dwWrittenBytes;
	if( FALSE == WriteFile(FileHandle.hImageFile,&ArrayNode,sizeof(ArrayNode),&dwWrittenBytes,NULL) )
	{
		TRACE(L"\nWrite ArrayNode error in CombineFileToOneImg.");
		return FALSE;
	}

	// add the array data
	if( FALSE == CombineTwoFile(FileHandle.hImageFile,FileHandle.hTailFile) )
	{
		TRACE(L"\nCombineTwoFile error in CombineFileToOneImg.");
		return FALSE;
	}

	return TRUE;
}

BOOL CombineTwoFile(HANDLE hFirstFile, HANDLE hSecondFile)
{
	CString strText,strTitle;
	  DWORD dwWrittenBytes;

	  __int64 linTargetFilePointer;
	  
	  if( FALSE == YGSetFilePointer(hFirstFile,0,FILE_END,linTargetFilePointer) )
	  {
		  TRACE("\n1. YGSetFilePointer error in CombineTwoFile.");
		  return FALSE;
	  }

	  if( FALSE == YGSetFilePointer(hSecondFile,0,FILE_BEGIN,linTargetFilePointer) )
	  {
		  TRACE("\n2. YGSetFilePointer error in CombineTwoFile.");
		  return FALSE;
	  }

	  DWORD dwReadBytes;

	  char *pCopyBuffer;
		  
	  pCopyBuffer = (char *)malloc(AM_BUFFER_SIZE);

	  if( NULL == pCopyBuffer )
	  {
		  strText.LoadString(IDS_NOT_ENOUGH_MEMORY);
#ifndef _YG_COMMAND_LINE
		  strTitle.LoadString(IDS_APP_NAME);
		  MessageBox(GetFocus(),strText,strTitle,MB_OK | MB_ICONWARNING);
#else
		  wcout << (LPCTSTR)strText << endl;
#endif
	  }
      else
	  {
		  if( FALSE == ReadFile(hSecondFile,pCopyBuffer,
		                        AM_BUFFER_SIZE,&dwReadBytes,NULL) )
		  {
			  free(pCopyBuffer);
			  TRACE("\nRead second file error in CombienTwoFile.");
			  return FALSE;
		  }

		  while( dwReadBytes>0 )
		  {
		      if( FALSE == WriteFile(hFirstFile,pCopyBuffer,dwReadBytes,&dwWrittenBytes,NULL) )
			  {
				  free(pCopyBuffer);
				  return FALSE;
			  }
 	   		  if( FALSE == ReadFile(hSecondFile,pCopyBuffer,AM_BUFFER_SIZE,&dwReadBytes,NULL) )
			  {
				  free(pCopyBuffer);
				  return FALSE;
			  }
		  }

		  free(pCopyBuffer);
		  pCopyBuffer=NULL;
	  }
	  return TRUE;
}

// 2004.08.24 huapage added begin

BOOL StoreDirectoryInfo(LPCTSTR szDirName,HANDLE hTailFile,LPCTSTR szPassword,DWORD &dwPathID)
{
	// TRACE("\nStore directory information:%s",szDirName);
	
	// wait for proegress begin
	if( g_bEncryptCancel || g_bEncryptUserCancel )
	{
		return FALSE;
	}

	// record the array information
	WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile(szDirName,&findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		TRACE("\nFindFirstFile error in EncryptFile.");
	    return FALSE;
	}
      
	VERIFY(FindClose(hFind));

	ARRAY_DATA ArrayData;
	memset(&ArrayData,0,sizeof(ArrayData));
	ArrayData.dwFileAttribute = findFileData.dwFileAttributes;
	ArrayData.dwPathID = dwPathID;
	ArrayData.ftCreationTime = findFileData.ftCreationTime;
	ArrayData.ftLastAccessTime = findFileData.ftLastAccessTime;
	ArrayData.ftLastWriteTime = findFileData.ftLastWriteTime;
	
	// for a directory, qwFileOffset is not needed
	ArrayData.qwFileOffset.QuadPart = 0;
	
	ArrayData.qwFileSize.LowPart = findFileData.nFileSizeLow;
	ArrayData.qwFileSize.HighPart = findFileData.nFileSizeHigh;
	wcsncpy(ArrayData.szFileName,szDirName,MAX_PATH-1);

	// for debug temp comment begin
	// encrypt array data begin
	char chPassword[PASSWORD_SIZE];
	ZeroMemory(chPassword, PASSWORD_SIZE);
	WideCharToMultiByte(CP_ACP, 0, szPassword, PASSWORD_SIZE, chPassword, PASSWORD_SIZE, 0, 0);
    if( FALSE == YGAESEncryptData((unsigned char*)chPassword,strlen(chPassword),(unsigned char*)(&ArrayData),(unsigned char*)(&ArrayData),sizeof(ArrayData)) )
	{
		  TRACE(L"\nYGAESEncryptData error in WriteTargetTail."); 
		  return FALSE;
	}
	// encrypt array data end
	// for debug temp comment end

	DWORD dwWrittenBytes;

	if( FALSE == WriteFile(hTailFile,&ArrayData,sizeof(ArrayData),&dwWrittenBytes,NULL) )
	{
		TRACE(L"\nWriteFile error in EncryptFile (Write tail file error). ");
		return FALSE;
	}

	dwPathID = dwPathID + 1;

	return TRUE;
}


BOOL StoreExtraDirInfo(CStringArray &SelectionArray,HANDLE hTailFile,LPCTSTR szPassword,DWORD &dwPathID)
{
  CStringArray ExtraDirArray;

  BOOL bError;
  bError = FALSE;

  long i,StartPoint,ArraySize,LeftPartIndex;

  CString OneString,OneSubString;

  ExtraDirArray.RemoveAll();

  ArraySize=SelectionArray.GetSize();

  for(i=0;i<ArraySize;i++)
  {

	  OneString=SelectionArray.ElementAt(i);

	  OneString.TrimRight(L'\\');
	  
	  OneString=OneString.Left(OneString.ReverseFind(L'\\')+1);

	  StartPoint=3;
	  
	  while( 
		     ( StartPoint < OneString.GetLength() - 1 ) && 
		     ( OneString.Find(L'\\',StartPoint) != -1    )
		   )
	  {
         LeftPartIndex=OneString.Find(L'\\',StartPoint);

		 OneSubString=OneString.Left(LeftPartIndex);
		 
		 if( bIsFoundInMorePath((LPCTSTR)OneSubString,ExtraDirArray) == FALSE )
		 {
			ExtraDirArray.Add(OneSubString); 

			if( FALSE == StoreDirectoryInfo(LPCTSTR(OneSubString),hTailFile,szPassword,dwPathID) )
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

// 2004.08.24 huapage added end

// 2004.08.27 huapage added begin

void SetEncryptCancel()
{
	g_bEncryptUserCancel = TRUE;
}

// for progress bar

// 2004.09.16 added begin
// this fucntion is used for watch folder
BOOL EncryptOneFile(LPCTSTR szSourceFile,LPCTSTR szTargetFile,LPCTSTR szPassword,BOOL bDeleteSource,DWORD dwCompressLevel,BOOL bErrorLimit,ULONG ulMaxInputNumber)
{
	DWORD dwFileAttribute;
	dwFileAttribute = GetFileAttributes(szSourceFile);
	if( -1 == dwFileAttribute )
	{
		TRACE(L"\nEncryptOneFile %s error, can not find the source",szSourceFile);
		return FALSE;
	}

	if( FILE_ATTRIBUTE_DIRECTORY & dwFileAttribute )
	{
		TRACE(L"\nEncryptOneFile %s error, do not encrypt a directory",szSourceFile);
		return FALSE;
	}

	if(wcslen(szPassword) > PASSWORD_SIZE )
	{
		TRACE(L"\nPassword size error in EncryptOneFile, Password too long.");
		return FALSE;
	}
	
	CString strSourceFile = szSourceFile;

	CEncryptInfo EncryptInfo;

	EncryptInfo.m_dwCompressLevel = dwCompressLevel;
	EncryptInfo.m_EncryptOption.bOptionOn = TRUE;
	EncryptInfo.m_EncryptOption.bLimitCount = FALSE;
	EncryptInfo.m_EncryptOption.bLimitTime = FALSE;
	EncryptInfo.m_EncryptOption.bOptionOn = bErrorLimit;
	EncryptInfo.m_EncryptOption.bErrorLimit = bErrorLimit;
	EncryptInfo.m_EncryptOption.ulErrorLimit = ulMaxInputNumber;
	EncryptInfo.m_strSourceArray.Add(strSourceFile);
	
	// 2004.09.14 for debug begin
	// now only monitor one folder
	EncryptInfo.m_strPassword = szPassword;
	
	// 2004.09.14 for debuge end
	
	EncryptInfo.m_strTarget = szTargetFile;
	SetFileAttributes(szTargetFile,FILE_ATTRIBUTE_NORMAL);
	DeleteFile(szTargetFile);
	if( FALSE == EncryptSelection(EncryptInfo) )
	{
		TRACE(L"\nEncryptSelection error in ThreadEncryptFile");
		return FALSE;
	}
	else
	{
		TRACE(L"\nEncrypt %s finished.",(LPCTSTR)(EncryptInfo.m_strSourceArray[0]) );
		
		// EncryptSelection finished, delete the original file.
		if( bDeleteSource )
		{
			SetFileAttributes(szSourceFile,FILE_ATTRIBUTE_NORMAL);
		    if(!DeleteFile(szSourceFile))
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}


BOOL EncryptOneDirSeparately(LPCTSTR szSourceDirectory,LPCTSTR szPassword,BOOL bDeleteSource,DWORD dwCompressLevel,BOOL bErrorLimit,ULONG ulMaxErrLimit)
{
	BOOL bEncryptSucceed;

	bEncryptSucceed = TRUE;

	if( FALSE == IsOneDirectory(szSourceDirectory))
	{
		TRACE(L"\nIsOneDirectory return FALSE in GetDirectoryInfo");
		return FALSE;
	}
	
	CString strFind;
	strFind = strFind + szSourceDirectory + L"\\*.*";
	
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
			CString strSourceFile;
			strSourceFile = strSubPath;

			CString strFileExtension;
			strFileExtension = strFileExtension + L"." + FILE_ENCRYPT_EXTENSION;

			if( 0 != strSourceFile.Right(4).CompareNoCase(strFileExtension) && !CheckSpeicalFile(strSourceFile) )
			{
				CString strTargetFile;
				strTargetFile = strSourceFile + L"." + FILE_ENCRYPT_EXTENSION;
				if( FALSE == EncryptOneFile((LPCTSTR)strSourceFile,(LPCTSTR)strTargetFile,szPassword,bDeleteSource,dwCompressLevel,bErrorLimit,ulMaxErrLimit) )
				{
					bEncryptSucceed = FALSE;
					TRACE(L"\nEncryptOneFile %s error in EncryptOneDirSeparately.",(LPCTSTR)strSubPath);
				}
			}
		}
		else
		{
			if( FALSE == EncryptOneDirSeparately((LPCTSTR)strSubPath,szPassword,bDeleteSource,dwCompressLevel,bErrorLimit,ulMaxErrLimit) )
			{
				bEncryptSucceed = FALSE;
				TRACE(L"\nEncryptOneDirSeperately %s error in EncryptOneDirSeparately.",(LPCTSTR)strSubPath);
			}
		}
	}while(FindNextFile(hFind,&FindFileData));
	
	FindClose(hFind);
	
	return bEncryptSucceed;
}

// 2004.09.16 added end


// 2004.09.20 added begin

BOOL RecordSelectionArray(HANDLE hImageFile,LPCTSTR szPassword,const CStringArray &SelectionArray)
{ 
	__int64 linTargetFilePointer;

	if( FALSE == YGSetFilePointer(hImageFile,0,FILE_END,linTargetFilePointer) )
	{
		TRACE(L"\nYGSetFilePointer error in RecoredSettingArray.");
		// needn't CloseHandle
		// we will CloseHandle in the main function
		return FALSE;
	}

    if( sizeof(FILE_ENCRYPT_HEAD) != linTargetFilePointer )
	{
		TRACE(L"\nThe file pointer of SelectionArray is not correct.");
		return FALSE;
	}

	SELECTION_ARRAY_NODE SelectionArrayNode;
	memset(&SelectionArrayNode,0,sizeof(SelectionArrayNode));
	SelectionArrayNode.dwSourceCount = SelectionArray.GetSize();

	DWORD dwWrittenBytes;
	if( FALSE == WriteFile(hImageFile,&SelectionArrayNode,sizeof(SelectionArrayNode),&dwWrittenBytes,NULL) )
	{
		TRACE(L"\nWriteFile error in RecordSelectionArray, write SelectionArrayNode.");
		return FALSE;
	}

	int nItemIndex;
	int nItemCount;
	nItemCount = SelectionArray.GetSize();

	WCHAR szSourceFile[MAX_PATH];
	memset(szSourceFile,0,sizeof(szSourceFile));

	for(nItemIndex=0; nItemIndex < nItemCount; nItemIndex++)
	{
		wcsncpy(szSourceFile,(LPCTSTR)SelectionArray[nItemIndex],MAX_PATH);

		// encrypt selection file begin
		char chPassword[PASSWORD_SIZE];
		ZeroMemory(chPassword, PASSWORD_SIZE);
		WideCharToMultiByte(CP_ACP, 0, szPassword, PASSWORD_SIZE, chPassword, PASSWORD_SIZE, 0, 0);
		if( FALSE == YGAESEncryptData((unsigned char*)chPassword,strlen(chPassword),(unsigned char*)szSourceFile,(unsigned char*)szSourceFile,sizeof(szSourceFile)) )
		{
			  TRACE("\nYGAESEncryptData error in RecordSelectionArray"); 
			  return FALSE;
		}
		// encrypt selection file end

		if( FALSE == WriteFile(hImageFile,szSourceFile,sizeof(szSourceFile),&dwWrittenBytes,NULL) )
		{
			TRACE(L"\nWriteFile error in RecoredSelectionArray, write szSourceFile.");
			return FALSE;
		}
	}
	return TRUE;
}

// 2004.09.20 added end


BOOL GetTargetFileName(LPCTSTR szSourceFile,CString &strTargetFile)
{
	// get target file name

	CString strSourceFile;
	strSourceFile = szSourceFile;

	if(L"." == strSourceFile.Right(4).Left(1) )
	{
		// normal suffix, replace the suffix with ".chy"
		strTargetFile = strSourceFile.Left(strSourceFile.GetLength()-4) + L"." + FILE_ENCRYPT_EXTENSION;
	}
	else
	{
		// not normal suffix, just append ".chy" to the original file name
		strTargetFile = strSourceFile + L"." + FILE_ENCRYPT_EXTENSION;
	}
	return TRUE;
}

BOOL GetBackupFileHandle(LPCTSTR szSourceFile,LPCTSTR szTargetFile,HANDLE &hSourceFile,HANDLE &hTargetFile)
{
	CString strTitle;
	SetFileAttributes(szTargetFile,FILE_ATTRIBUTE_NORMAL);
	
	hSourceFile = CreateFile(szSourceFile,GENERIC_READ,FILE_SHARE_READ | FILE_SHARE_WRITE,
		                     NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if( INVALID_HANDLE_VALUE == hSourceFile )
	{
		CString strOpenSourceFailed;
		strOpenSourceFailed.Format(IDS_OPEN_SOURCE_FAILED,szSourceFile);
#ifndef _YG_COMMAND_LINE
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(GetFocus(),strOpenSourceFailed,strTitle,MB_OK | MB_ICONWARNING);
#else
		wcout << (LPCTSTR)strOpenSourceFailed << endl;
#endif
		return FALSE;
	}


	hTargetFile = CreateFile(szTargetFile,
		                     GENERIC_READ|GENERIC_WRITE,
							 FILE_SHARE_READ | FILE_SHARE_WRITE,
							 NULL,
							 CREATE_ALWAYS,
							 FILE_ATTRIBUTE_NORMAL,
							 NULL);

	if( INVALID_HANDLE_VALUE == hTargetFile )
	{
		CloseHandle(hSourceFile);
		CString strOpenTargetFailed;
		strOpenTargetFailed.Format(IDS_OPEN_TARGET_FAILED,szTargetFile);
#ifndef _YG_COMMAND_LINE
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(GetFocus(),strOpenTargetFailed,strTitle,MB_OK | MB_ICONWARNING);
#else
		wcout << (LPCTSTR)strOpenTargetFailed << endl;
#endif
		return FALSE;
	}
	return TRUE;
}

BOOL CheckPlacement(LPCTSTR szSourceDir, LPCTSTR szTargetImage)
{
	CString strSourceDir;
	CString strTargetImage;
	strSourceDir = szSourceDir;
	strTargetImage = szTargetImage;

	strSourceDir.TrimRight(L"\\");
	strSourceDir = strSourceDir + L"\\";
	
	if( 0 == strTargetImage.Left(strSourceDir.GetLength()).CompareNoCase((LPCTSTR)strSourceDir) )
	{
		return FALSE;
	}
	return TRUE;
}


void AddDelMethod(CComboBoxEx* pBox)
{
	COMBOBOXEXITEM		cbItem;
	BYTE				i;
	CString				csMethod;

	cbItem.mask			  = CBEIF_TEXT;
	cbItem.iItem		  = 0;
	cbItem.cchTextMax	  = MAX_PATH;
	cbItem.iImage		  = 0;
	cbItem.iSelectedImage = 0;

	for(i = 0; i < 8 ; i ++)
	{
		switch(i)
		{
		case 0:
			csMethod.LoadString (IDS_00_METHOD);
			break;
		case 1:
			csMethod.LoadString (IDS_FF_METHOD);
			break;
		case 2:
			csMethod.LoadString (IDS_RANDOM_METHOD);
			break;
		case 3:
			csMethod.LoadString (IDS_NSA_METHOD);
			break;
		case 4:
			csMethod.LoadString (IDS_OLD_NSA_METHOD);
			break;
		case 5:
			csMethod.LoadString (IDS_DOD_STD_METHOD);
			break;
		case 6:
			csMethod.LoadString (IDS_NATO_METHOD);
			break;
		case 7:
			csMethod.LoadString (IDS_GUTMANN_METHOD);
			break;
		}

		WCHAR szMethod[MAX_PATH];
		memset(szMethod,0,sizeof(szMethod));
		wcsncpy(szMethod,(LPCTSTR)csMethod,MAX_PATH-1);
		cbItem.pszText = szMethod;
		cbItem.iItem  = i;
		pBox->InsertItem(&cbItem);
		pBox->SetItemData(cbItem.iItem,i);
	}
	pBox->SetCurSel(3);
}


BOOL CheckSpeicalFile(LPCTSTR p) {
	BOOL skip = FALSE;
	CString path = p;
	path.MakeLower();
	CString fileName = path.Mid(path.ReverseFind(L'\\') + 1);
	if (!fileName.IsEmpty()) {
		CString suffix = L"";
		int index = fileName.ReverseFind(L'.');
		if (index != 0 && index != -1) {
			suffix = fileName.Mid(index + 1);
		}
		if (fileName.GetAt(0) == L'.') {
			skip = TRUE;
		}
		else if (path.Find(L"onedrive")) {
			if (suffix == L"url" || fileName == L"desktop.ini") {
				skip = TRUE;
			}
		}
		else if (path.Find(L"dropbox")) {
			if (fileName == L"desktop.ini") {
				skip = TRUE;
			}
		}
	}
	return skip;
}

