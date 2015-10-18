#include "DecryptFunction.h"
#include "..\..\DavinciFunction\DavinciFunction.h"
#include "..\..\EncryptToolStruct\EncryptToolStruct.h"

#ifdef FILE_DECRYPT
#include "..\FileDecrypt\stdafx.h"
#include "..\FileDecrypt\resource.h"
#endif

#ifdef	_YG_FS_MON
#include "..\..\YGFSMon\stdafx.h"
#include "..\..\YGFSMon\resource.h"
#endif

#ifdef SHELL_DECRYPT
#include "..\ShellDecrypt\stdafx.h"
#include "..\ShellDecrypt\resource.h"
#endif

#ifdef _YG_COMMAND_LINE
#include "..\..\FED\stdafx.h"
#include "..\..\FED\resource.h"
//#include "IOSTREAM.H"
#endif

BOOL g_bCancel;
BOOL g_bUserCancel;
extern DA_WORK_STATE g_WorkState;
extern BYTE g_byBuffer[ENCRYPT_BUFFER_SIZE+ENCRYPT_BUFFER_SIZE/5];
extern BYTE g_byBufferUnCompress[ENCRYPT_BUFFER_SIZE];

#include <vector>
using namespace std;
BOOL DecryptSelectionFile(DECRYPT_INFO DecryptInfo)
{
	g_bCancel = FALSE;
	g_bUserCancel = FALSE;

	memset(&g_WorkState,0,sizeof(g_WorkState));

	switch (DecryptInfo.ulImageVersion)
	{
	case 100:
	case 0x200:
	case CURRENT_FILE_VERSION:
		break;
	default:
		return FALSE;
	}

	if( FALSE == GetDecryptFileSize(DecryptInfo) )
	{
		// perhaps create tail file error.
		TRACE(L"\nGetDecryptFileSize error in DecryptSelectionFile.");
	}

	DECRYPT_FILE_HANDLE FileHandle;
	if( FALSE == GetDecryptFileHandle(DecryptInfo.szImageFile,FileHandle) )
	{
		TRACE(L"\nGetDecryptFileHandle error in DecryptSelectionFile.");
		return FALSE;
	}

	ARRAY_NODE ArrayNode;
	__int64 linTargetFilePointer;
	
	if( FALSE == YGSetFilePointer(FileHandle.hTailFile,0,FILE_BEGIN,linTargetFilePointer) )
	{
		TRACE(L"\nYGSetFilePointer error in DecryptSelectionFile.");
		CloseDecryptFileHandle(FileHandle);
		return FALSE;
	}
	
	DWORD dwReadBytes;
	if( FALSE == ReadFile(FileHandle.hTailFile,&ArrayNode,sizeof(ArrayNode),&dwReadBytes,NULL) )
	{
		TRACE(L"\nReadFile error in DecryptSelectionFile.");
		CloseDecryptFileHandle(FileHandle);
		return FALSE;
	}

	DWORD dwPathIndex;

	for(dwPathIndex=1; dwPathIndex<=ArrayNode.dwPathCount; dwPathIndex++)
	{
		if( g_bCancel || g_bUserCancel )
		{
			break;
		}
		ARRAY_DATA ArrayData;
		ReadFile(FileHandle.hTailFile,&ArrayData,sizeof(ArrayData),&dwReadBytes,NULL);

		// for debug temp comment begin
		// decrypt array data begin
		char chPassword[PASSWORD_SIZE];
		ZeroMemory(chPassword, PASSWORD_SIZE);
		WideCharToMultiByte(CP_ACP, 0, DecryptInfo.szPassword, PASSWORD_SIZE, chPassword, PASSWORD_SIZE, 0, 0);
		if( FALSE == YGAESDecryptData((unsigned char*)chPassword,strlen(chPassword),(unsigned char*)(&ArrayData),(unsigned char*)(&ArrayData),sizeof(ArrayData)) )
		{
			  TRACE(L"\nYGAESDecryptData error in DecryptSelectionFile."); 
			  return FALSE;
		}
		// decrypt array data end
		// for debug temp comment end

		BOOL bSelectedPath;
		bSelectedPath=bIsFileSelected(ArrayData.szFileName,DecryptInfo);

		if(bSelectedPath)
		{
			BOOL bCancelRestore;
			bCancelRestore = FALSE;

			// the following function will adjust OnePathString
			BOOL bValidPath;

			// depend on restore to old place or new palce
			// the target file name is not the same.

			// AdjustOnePathString will modify the 
			// ArrayData.szFileName 
			bValidPath=AdjustOnePathString(ArrayData,DecryptInfo);

			if( FALSE == RestoreOnePath(FileHandle.hImageFile,ArrayData,DecryptInfo,bCancelRestore) )
			{
				TRACE(L"\nRestoreOnePath error in DecryptSelectionFile.");
				// Maybe we should ask the user if we need to continue restoring
				// the next file
				break;
			}

			if( bCancelRestore )
			{
				g_bUserCancel = TRUE;
				break;
			}
		}
	}

	if (DecryptInfo.ulImageVersion != 100 )
	{
		if (DecryptInfo.EncryptOption.bOptionOn)
		{
			if (DecryptInfo.EncryptOption.bLimitCount)
				DecryptInfo.EncryptOption.ulCurrentCount ++;
			if (DecryptInfo.EncryptOption.bErrorLimit)
				DecryptInfo.EncryptOption.uCurrentErr = 0;
			UpdateDecryptOption(FileHandle.hImageFile,DecryptInfo.szPassword,&DecryptInfo.EncryptOption);
		}
	}

	if( FALSE == CloseDecryptFileHandle(FileHandle) )
	{
		TRACE(L"\nCloseDecryptFileHandle error in DecryptSelectionFile.");
		// needn't return FALSE
	}

	// if GetSerateFile succeed, here we can delete the file that we created by ourself
	CString strTailFile;
	GetDecryptTailFile(DecryptInfo.szImageFile,strTailFile);
	SetFileAttributes(strTailFile,FILE_ATTRIBUTE_NORMAL);
	DeleteFile((LPCTSTR)strTailFile);

	return TRUE;
}

BOOL GetSeperateFile(LPCTSTR szImageFile,LPCTSTR szPassword)
{
	CString strText,strTitle;
	HANDLE hImageFile;
	BOOL   bResult = FALSE;

	hImageFile = CreateFile(szImageFile,
							GENERIC_READ,
							FILE_SHARE_READ | FILE_SHARE_WRITE,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL);

	if( INVALID_HANDLE_VALUE == hImageFile )
	{
		TRACE(L"\nCan not open image file.");
		return FALSE;
	}

	FILE_ENCRYPT_HEAD FileEncryptHead;
	ULONG			  ulImageVersion;
	ulImageVersion = GetFileEncryptHead(hImageFile,szPassword,FileEncryptHead);
	if (!ulImageVersion)
	{
		CloseHandle(hImageFile);
		return FALSE;
	}

	CString strTailFile;
	GetDecryptTailFile(szImageFile,strTailFile);

	// to check if the tail file already exist
	// we create the tail file for temporary usage
	DWORD dwFileAttribute;
	dwFileAttribute = GetFileAttributes((LPCTSTR)strTailFile);

	if( -1 != dwFileAttribute)
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
			   CloseHandle(hImageFile);
			   return FALSE;
		   }
#else
CMDAskForChoice:
		    wcout << (LPCTSTR)strAskForOverwrite << endl;
			wcin.getline(strText.GetBuffer (100), 100); 
			strText.ReleaseBuffer();
			if (strText.CompareNoCase(L"Y"))
			{
				if (!strText.CompareNoCase(L"N"))
				{
					TRACE(L"\nThe user choose not to overwrite the target file.");
					CloseHandle(hImageFile);
					return FALSE;
				}
				else
				{
					goto CMDAskForChoice;
				}
			}
#endif
		}
		else
		{
			// if this is a directory
			// it is impossible to overwrite a directory by a file
			TRACE(L"\nCan not overwrite a directory.");
			CloseHandle(hImageFile);
			return FALSE;
		}
	}

	HANDLE hTailFile;
	hTailFile = CreateFile((LPCTSTR)strTailFile,
		                    GENERIC_READ|GENERIC_WRITE,
							FILE_SHARE_READ | FILE_SHARE_WRITE,
							NULL,
							CREATE_ALWAYS,
							FILE_ATTRIBUTE_NORMAL,
							NULL);

	if( INVALID_HANDLE_VALUE == hTailFile )
	{
		TRACE(L"\n Open tail file error in GetSeperateFile.");
	    CloseHandle(hImageFile);
		return FALSE;
	}

	ULARGE_INTEGER qwFileSize;
	qwFileSize.LowPart = GetFileSize(hImageFile,&(qwFileSize.HighPart));
	
	switch (ulImageVersion)
	{
	case 100:
		qwFileSize.QuadPart -= ((FILE_ENCRYPT_HEAD_100 *)&FileEncryptHead)->FileInfoPointer.QuadPart;
		bResult = GetOneSeperateFile(hImageFile,hTailFile,((FILE_ENCRYPT_HEAD_100 *)&FileEncryptHead)->FileInfoPointer,qwFileSize.QuadPart);
		break;
	case 0x200:
	case CURRENT_FILE_VERSION:
		qwFileSize.QuadPart -= FileEncryptHead.FileInfoPointer.QuadPart;
		bResult = GetOneSeperateFile(hImageFile,hTailFile,FileEncryptHead.FileInfoPointer,qwFileSize.QuadPart);
		bResult = TRUE;
		break;
	default:
		bResult = FALSE;
		break;
	}

	CloseHandle(hImageFile);
	CloseHandle(hTailFile);
	return bResult;
}

// create the tail file in the temp directory
// so we can show the content even if it is in a Read-only CD-ROM
BOOL GetDecryptTailFile(LPCTSTR szTargetFile, CString &strTailFile)
{
	WCHAR szTempDir[MAX_PATH];
	memset(szTempDir,0,sizeof(szTempDir));

	if( FALSE == GetTempDir(szTempDir) )
	{
		TRACE(L"\nGetTempDir error in GetDecryptTailFile.");
	}

	CString strTempDir;
	strTempDir = szTempDir;

	WCHAR szMainName[MAX_PATH];
	memset(szMainName,0,sizeof(szMainName));

	if( FALSE == GetLongMainName(szTargetFile,szMainName) )
	{
		TRACE(L"\nGetLongMainName error in GetDecryptTailFile.");
	}

	CString strLeafPath = szTargetFile;
	int index = strLeafPath.ReverseFind(L'\\');
	strLeafPath = strLeafPath.Left(index + 1);
	if (strLeafPath.GetLength() + strTempDir.GetLength() + wcslen(szMainName) + 5 < MAX_PATH - 1){
		strLeafPath.Replace(L'\\', L'#');
		strLeafPath.Replace(L':', L'#');
		strTailFile = strTempDir + L"\\" + strLeafPath + szMainName + L".ary";
	}
	else {
		strTailFile = strTempDir + L"\\" + szMainName + L".ary";
	}

    return TRUE;
}

// copy from AM01DLL
BOOL GetOneSeperateFile(HANDLE hFirstFile, HANDLE hSecondFile, LARGE_INTEGER FilePointer, unsigned __int64 qwFileSize)
{
	CString strText,strTitle;
	  __int64 linTargetFilePointer;

	  DWORD dwWrittenBytes;

	  if( FALSE == YGSetFilePointer(hFirstFile,FilePointer.QuadPart,FILE_BEGIN,linTargetFilePointer ) )
	  {
		  TRACE(L"\n1. YGSetFilePointer error in GetOneSeperateFile.");
	  }

	  if( FALSE == YGSetFilePointer(hSecondFile,0,FILE_BEGIN,linTargetFilePointer) ) 
	  {
		  TRACE(L"\n2. YGSetFilePointer error in GetOneSeperateFile.");
	  }
  
	  DWORD dwReadBytes;

	  char *CopyString;
		  
	  CopyString=(char *)malloc(AM_BUFFER_SIZE);

	  if( CopyString == NULL )
	  {
		  strText.LoadString(IDS_NOT_ENOUGH_MEMORY);
#ifndef _YG_COMMAND_LINE
		  strTitle.LoadString(IDS_APP_NAME);
		  MessageBox(GetFocus(),strText,strTitle,MB_OK | MB_ICONWARNING);
#else
		  wcout << (LPCTSTR)strText << endl;
#endif
		  return FALSE;
	  }
      else
	  {
		  unsigned __int64 qwBytesToCopy=qwFileSize;
		  for(;
		      qwBytesToCopy>=AM_BUFFER_SIZE;
			  qwBytesToCopy=qwBytesToCopy-AM_BUFFER_SIZE)
		  {

		     if( FALSE == ReadFile(hFirstFile,CopyString,AM_BUFFER_SIZE,&dwReadBytes,NULL) )
			 {
				 free(CopyString);
				 CopyString = NULL;
				 return FALSE;
			 }

		     if( FALSE == WriteFile(hSecondFile,CopyString,dwReadBytes,&dwWrittenBytes,NULL) )
			 {
				 free(CopyString);
				 CopyString = NULL;
				 return FALSE;
			 }
			 
			 // if disk is full, or if the file is in a CD-R
			 // then return FALSE;
			 if(dwWrittenBytes < dwReadBytes)
			 {
				 free(CopyString);
				 CopyString = NULL;
				 return FALSE;
			 }
		 }

		  DWORD dwBytesLeft;
		  dwBytesLeft=(DWORD)qwBytesToCopy;

 	   	  if( FALSE == ReadFile(hFirstFile,CopyString,dwBytesLeft,&dwReadBytes,NULL) )
		  {
			 free(CopyString);
			 CopyString = NULL;
			 return FALSE;
		  }
		  
		  if( FALSE == WriteFile(hSecondFile,CopyString,dwBytesLeft,&dwWrittenBytes,NULL) )
		  {
			 free(CopyString);
			 CopyString = NULL;
			 return FALSE;
		  }

		 // if disk is full, or if the file is in a CD-R
		 // then return FALSE;
		 if(dwWrittenBytes < dwBytesLeft)
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



BOOL GetDecryptFileHandle(LPCTSTR szImageFile,DECRYPT_FILE_HANDLE &FileHandle)
{
	CString strTailFile;
	GetDecryptTailFile(szImageFile,strTailFile);

	FileHandle.hImageFile = CreateFile(szImageFile,
		                      GENERIC_READ | GENERIC_WRITE,
							  FILE_SHARE_READ | FILE_SHARE_WRITE,
							  NULL,
							  OPEN_EXISTING,
							  FILE_ATTRIBUTE_NORMAL,
							  NULL);

	if( INVALID_HANDLE_VALUE == FileHandle.hImageFile )
	{
		TRACE(L"\nOpen image file error in GetDecryptFileHandle.");
		return FALSE;
	}

	FileHandle.hTailFile = CreateFile( (LPCTSTR)strTailFile,
										GENERIC_READ,
										FILE_SHARE_READ | FILE_SHARE_WRITE,
										NULL,
										OPEN_EXISTING,
										FILE_ATTRIBUTE_NORMAL,
										NULL);

	if( INVALID_HANDLE_VALUE == FileHandle.hTailFile )
	{
		TRACE(L"\nOpen tail file erro in GetDecryptFileHanlde.");
		CloseHandle(FileHandle.hImageFile);
		return FALSE;
	}

	return TRUE;
}

BOOL CloseDecryptFileHandle(DECRYPT_FILE_HANDLE FileHandle)
{
	BOOL bReturnValue;
	bReturnValue = TRUE;
	
	if( FALSE == CloseHandle(FileHandle.hImageFile ))
	{
		bReturnValue = FALSE;
	}

	if( FALSE == CloseHandle(FileHandle.hTailFile ) )
	{
		bReturnValue = FALSE;
	}

	return bReturnValue;
}

BOOL RestoreOnePath(HANDLE hImageFile, ARRAY_DATA ArrayData,DECRYPT_INFO DecryptInfo,BOOL &bCancelRestore)
{
	CString strText,strTitle;

	if( g_bCancel || g_bUserCancel)
	{
		return FALSE;
	}
	if( 0 == ( FILE_ATTRIBUTE_DIRECTORY & ArrayData.dwFileAttribute ) )
	{
		// if this is a file

		DWORD dwFileAttribute;
		dwFileAttribute = GetFileAttributes(ArrayData.szFileName);

		if( -1 != dwFileAttribute )
		{
			if( 0 != (dwFileAttribute & FILE_ATTRIBUTE_DIRECTORY ) )
			{
				TRACE(L"\nCan not overwrite directory");
				return FALSE;
			}
			else
			{
				CString strAskForOverwrite;
				strAskForOverwrite.Format(IDS_ASK_FOR_OVERWRITE,ArrayData.szFileName);
				
				int nReturnValue;
#ifndef _YG_COMMAND_LINE
				strTitle.LoadString(IDS_APP_NAME);
				nReturnValue = MessageBox(GetFocus(),strAskForOverwrite,strTitle,MB_YESNOCANCEL | MB_ICONQUESTION);
#else
CMDAskForChoice:
				wcout << (LPCTSTR)strAskForOverwrite << endl;
				wcin.getline(strText.GetBuffer (100), 100); 
				strText.ReleaseBuffer();
				if (!strText.CompareNoCase(L"Y")) 
				{
					nReturnValue = IDYES;
				}
				else
				{
					if (!strText.CompareNoCase(L"N"))
					{
						nReturnValue = IDNO;
					}
					else 
					{
						goto CMDAskForChoice;
					}
				}
#endif

				if( IDCANCEL == nReturnValue )
				{
					bCancelRestore = TRUE;
					return FALSE;
				}
				else if( IDNO == nReturnValue )
				{
					return TRUE;
				}
				else if( IDYES == nReturnValue )
				{
					// here to restore one file
				   if( FALSE == RestoreOneFile(hImageFile,DecryptInfo,ArrayData) )
				   {
					   TRACE(L"\nRestoreOneFile error in RestoreOnePath.");
					   return FALSE;
				   }
				}
			} // if the duplicate file name is a file, not a directory
		}
		else
		{  // if the file is not exist
		   if( FALSE == PrepareOnePath(ArrayData.szFileName) )
		   {
			   TRACE(L"\nPrepareOnePath error in RestoreOnePath.");
			   return FALSE;
		   }
		   if( FALSE == RestoreOneFile(hImageFile,DecryptInfo,ArrayData) )
		   {
			   TRACE(L"\nRestoreOneFile error in RestoreOnePath.");
			   return FALSE;
		   }
		}
	}
	else
	{
		// if this is a directory
		if( FALSE == RestoreOneDir(DecryptInfo,ArrayData) )
		{
			TRACE(L"\nRestoreOneDir error in RestoreOnePath.");
			return FALSE;
		}
	}
	return TRUE;
}


BOOL RestoreOneFile(HANDLE hImageFile, DECRYPT_INFO DecryptInfo,ARRAY_DATA ArrayData)
{

	if(g_bCancel || g_bUserCancel)
	{
		return FALSE;
	}

#ifdef _YG_COMMAND_LINE
	wcout << ArrayData.szFileName << endl;
#endif

	HANDLE hTargetFile;
	memset(g_WorkState.szCurrentFile,0,sizeof(g_WorkState.szCurrentFile));
	wcsncpy(g_WorkState.szCurrentFile,ArrayData.szFileName,MAX_PATH-1);
	
	hTargetFile = CreateFile(ArrayData.szFileName,
		                     GENERIC_READ|GENERIC_WRITE,
							 FILE_SHARE_READ | FILE_SHARE_WRITE,
							 NULL,
							 CREATE_ALWAYS,
							 FILE_ATTRIBUTE_NORMAL,
							 NULL);

	if( INVALID_HANDLE_VALUE == hTargetFile )
	{
		TRACE(L"\nOpen file error in RestoreOneFile.");
		return FALSE;
	}

    __int64 linTargetFilePointer;
	if( FALSE == YGSetFilePointer(hImageFile,ArrayData.qwFileOffset.QuadPart,FILE_BEGIN,linTargetFilePointer) )
	{
		TRACE(L"\nYGSetFilePointer error in RestoreOneFile.");
		CloseHandle(hTargetFile);
		return FALSE;
	}

	DATA_NODE DataNode;

	DWORD dwWrittenBytes;

	unsigned __int64 qwByteLeft;

	qwByteLeft = ArrayData.qwFileSize.QuadPart;

	while( qwByteLeft > 0 )
	{
		if(g_bCancel || g_bUserCancel)
		{
			break;
		}
		
		DWORD dwReadBytes;
		
		ReadFile(hImageFile,&DataNode,sizeof(DataNode),&dwReadBytes,NULL);
		
		if (DecryptInfo.ulImageVersion == 100)	DataNode.dwOrigSize = DataNode.dwPacketSize;
		if(DataNode.dwOrigSize > ENCRYPT_BUFFER_SIZE || DataNode.dwPacketSize > ENCRYPT_BUFFER_SIZE+ENCRYPT_BUFFER_SIZE/5)
		{
			TRACE(L"\nDataNode error.");
			CloseHandle(hTargetFile);
			return FALSE;
		}

		if( FALSE == ReadFile(hImageFile,g_byBuffer,DataNode.dwPacketSize,&dwReadBytes,NULL))
		{
			TRACE(L"\nReadFile error in RestoreOneFile.");
			CloseHandle(hTargetFile);
			return FALSE;
		}

		if(dwReadBytes != DataNode.dwPacketSize)
		{
			TRACE(L"\nReadFile error in RestoreOneFile.");
			CloseHandle(hTargetFile);
			return FALSE;
		}

		char chPassword[PASSWORD_SIZE];
		ZeroMemory(chPassword, PASSWORD_SIZE);
		WideCharToMultiByte(CP_ACP, 0, DecryptInfo.szPassword, PASSWORD_SIZE, chPassword, PASSWORD_SIZE, 0, 0);
		if( FALSE == YGAESDecryptData((unsigned char *)chPassword,strlen(chPassword),g_byBuffer,g_byBuffer,dwReadBytes) )
		{
			TRACE(L"\nYGAESDecryptData error in RestoreOneFile.");
			CloseHandle(hTargetFile);
			return FALSE;
		}

		if (DecryptInfo.ulImageVersion == 100)	DataNode.dwCompressLevel =  LEVEL_NO_COMPRESS;
		switch (DataNode.dwCompressLevel)
		{
		case LEVEL_FAST_COMPRESS:
		case LEVEL_HIGH_COMPRESS:
			dwReadBytes = ENCRYPT_BUFFER_SIZE;
			if (Z_OK != uncompress(g_byBufferUnCompress,&dwReadBytes,g_byBuffer,DataNode.dwPacketSize))
			{
				TRACE(L"\nYGAES uncompress Data error in RestoreOneFile.");
				CloseHandle(hTargetFile);
				return FALSE;
			}
			break;
		case LEVEL_NO_COMPRESS:
		default:
			DataNode.dwOrigSize = DataNode.dwPacketSize;
			memcpy(g_byBufferUnCompress,g_byBuffer,DataNode.dwPacketSize);
			break;
		}

		WriteFile(hTargetFile,g_byBufferUnCompress,dwReadBytes,&dwWrittenBytes,NULL);
		
		// for progress bar
		g_WorkState.qwCurrentWriteSize = g_WorkState.qwCurrentWriteSize + dwReadBytes;

		// 2004.08.27 wait for progress begin

		if( g_WorkState.qwTotalFileSize > 0 )
		{
		   g_WorkState.nPercent = (int ) (g_WorkState.qwCurrentWriteSize * 100 / g_WorkState.qwTotalFileSize);
		}
		else
		{
			TRACE(L"\ng_WorkState.qwTotalFileSize is not correct in RestoreOneFile.");
		}

		// 2004.08.27 wait for progress end

		qwByteLeft = qwByteLeft - DataNode.dwOrigSize;
	}

	SetFileTime(hTargetFile,
		        &(ArrayData.ftCreationTime),
				&(ArrayData.ftLastAccessTime),
				&(ArrayData.ftLastWriteTime));
#ifdef NO_DRIVER_MODE
	if (!g_bCancel && !g_bUserCancel)
	{
		RecordDataInfo(ArrayData.szFileName, hTargetFile);
	}
#endif
	CloseHandle(hTargetFile);

	SetFileAttributes(ArrayData.szFileName,ArrayData.dwFileAttribute);

	if(g_bCancel || g_bUserCancel)
	{
		TRACE(L"\nDelete the last file:%s, it may be an incomplete file.",ArrayData.szFileName);
	    SetFileAttributes(ArrayData.szFileName,FILE_ATTRIBUTE_NORMAL);
		DeleteFile(ArrayData.szFileName);
		return FALSE;
	}

	g_WorkState.dwCurrentFileCount ++ ; // the value currently is not used

	return TRUE;
}

BOOL RestoreOneDir(DECRYPT_INFO DecryptInfo,ARRAY_DATA ArrayData)
{
	if( g_bCancel || g_bUserCancel)
	{
		return FALSE;
	}

    // Get the required parent directory
	WCHAR szDirPath[MAX_PATH];
	memset(szDirPath,0,sizeof(szDirPath));
	wcsncpy(szDirPath,ArrayData.szFileName,MAX_PATH-1);

	if (FALSE == PrepareOnePath(szDirPath) )  // the szFileName here is a Dir name
	{
		TRACE(L"\nPrepareOnePath error in RestoreOneDir.");
		return FALSE;
	}
	
	DWORD dwFileAttribute;
	dwFileAttribute = GetFileAttributes(szDirPath);
	
	if ( -1 == dwFileAttribute )
	{
		// if the directory is not exist, we create the directory and set the time
		// if the directory has already exist, we do not touch it.
		
		if( FALSE == CreateDirectory(szDirPath,NULL) )
		{
			TRACE(L"CreateDirectory error in RestoreOneDir.");
			return FALSE;
		}
		
		SetFileAttributes(szDirPath,ArrayData.dwFileAttribute);
		
		HANDLE hDir;
		hDir = CreateFile (szDirPath, 
			GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
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
		else
		{
			if(
				FALSE == SetFileTime( 
									  hDir,
									  &(ArrayData.ftCreationTime),
									  &(ArrayData.ftLastAccessTime),
									  &(ArrayData.ftLastWriteTime)
									)
			  )
			{
				TRACE(L"\nSetFileTime error in RestoreOneDir.");
			}
			CloseHandle(hDir);
		} // if we get an valid handle for a directory
	}
	return TRUE;
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
		 SubPathArray.RemoveAll();

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

// for DLL we did not use CString &strLeftPart as output

BOOL GetLeftPart(LPCTSTR szOnePath, WCHAR * szLeftPart)
{

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


BOOL bPathExist(LPCTSTR OnePath)
{
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

  wcsncpy(szTempDir,(LPCTSTR)strTempDir,MAX_PATH-1);

  if( strTempDir.GetLength() > MAX_PATH-1 )
  {
	  TRACE(L"\nTempDir is too long.");
	  // cut off the extra characters
	  szTempDir[MAX_PATH-1]=L'\0';
  }

  return TRUE;
}

/*=============================================================
 *
 * Function name:  GetLongMainName()
 * Purpose:        get the main name from the total path 
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


BOOL bIsFileSelected(LPCTSTR szOnePath, DECRYPT_INFO DecryptInfo)
{
	// if it is restore entire, all path should be considered
	// as selected

	if(DECRYPT_ENTIRE == DecryptInfo.nRestoreType)
	{
		return TRUE;
	}

	CString SelectedPath;
	SelectedPath=DecryptInfo.szSelectedPath;

	CString OnePathString;
	OnePathString=szOnePath;

	// add "\\" for compare

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
	if( 0 == OnePathString.Compare(SelectedPath) )
	{
			return TRUE;
	}

	// if restore to original place
	// the parent dir should be restored too.

	if(DECRYPT_ORIGINAL_PLACE == DecryptInfo.nTargetType)
	{  
		SelectedPath=SelectedPath.Left( OnePathString.GetLength());
	    if( 0 == OnePathString.Compare(SelectedPath))
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL FixupDecryptInfo(DECRYPT_INFO *pDecryptInfo)
{
	DECRYPT_INFO DecryptInfoTemp;
	DWORD dwImageFileCount;
	CString strOneFileName;
	CStringArray SelectionArray;

	if (pDecryptInfo->nTargetType == DECRYPT_CURRENT_PLACE && pDecryptInfo->nRestoreType == DECRYPT_ENTIRE)
	{
		memcpy(&DecryptInfoTemp,pDecryptInfo,sizeof(DECRYPT_INFO));
		if( FALSE == GetImageInfo(&DecryptInfoTemp,dwImageFileCount,strOneFileName) )
		{
			return FALSE;
		}

		// 2004.09.15 modify begin

		if( 1 != dwImageFileCount) // more than one file, or no file, only directory
		{
			if( FALSE == ReadSelectionArray(DecryptInfoTemp.szImageFile,DecryptInfoTemp.szPassword,SelectionArray) )
			{
				TRACE(L"\nReadSelectionArray error.");
				return FALSE;
			}

			if( 1 == SelectionArray.GetSize() )
			{
				memset(pDecryptInfo->szSelectedPath,0,sizeof(pDecryptInfo->szSelectedPath));
				wcsncpy(pDecryptInfo->szSelectedPath,(LPCTSTR)(SelectionArray[0]),MAX_PATH-1);
			}
			else
			{
				// if selection is more than one item

				CString strCommonLeftDir;
				
				if( FALSE == GetCommonLeftDir(SelectionArray,strCommonLeftDir) )
				{
					TRACE(L"\nGetCommonLeftDir error in OnButtonDecrypt.");
				}
				else
				{
					TRACE(L"\nThe common left dir is:%s",(LPCTSTR)strCommonLeftDir);
				}

				if( 0 == strCommonLeftDir.GetLength() )
				{
					return FALSE;
				}
				else
				{
					// more than one item, but in the same disk
					strCommonLeftDir.TrimRight(L'\\');
					memset(pDecryptInfo->szSelectedPath,0,sizeof(pDecryptInfo->szSelectedPath));
					wcsncpy(pDecryptInfo->szSelectedPath,(LPCTSTR)(strCommonLeftDir),MAX_PATH-1);
				}
			}
		}
		else
		{
			// if it contains only one file
			// if this image file only contain one file
			// right click it will decrypt the file to the same directory as the original image file
			memset(pDecryptInfo->szSelectedPath,0,sizeof(pDecryptInfo->szSelectedPath));
			wcsncpy(pDecryptInfo->szSelectedPath,(LPCTSTR)strOneFileName,MAX_PATH-1);
		}
		pDecryptInfo->nRestoreType = DECRYPT_PART;
	}
	return TRUE;
}

BOOL AdjustOnePathString(ARRAY_DATA &ArrayData, DECRYPT_INFO DecryptInfo)
{
	CString strSelectedPath;
	CString OnePathString;
	OnePathString = ArrayData.szFileName;

	CString strTargetDir;
	strTargetDir=DecryptInfo.szTargetPath;
	strTargetDir.TrimRight(L"\\");

	switch (DecryptInfo.nTargetType)
	{
	case DECRYPT_ORIGINAL_PLACE:
		return TRUE;
	case DECRYPT_RELATIVE_PLACE:
		switch (DecryptInfo.nRestoreType)
		{
		case DECRYPT_ENTIRE:
			OnePathString=strTargetDir + L"\\"  +OnePathString.Mid(3);
			break;
		case DECRYPT_PART:
			strSelectedPath=DecryptInfo.szSelectedPath;

			// for compare, the selected path has a "\" in the end
			strSelectedPath.TrimRight(L"\\");

			if(strSelectedPath.GetLength()==2) // for example "k:"
			{
				OnePathString=strTargetDir + L"\\" 
							  +OnePathString.Mid(3);
			}
			else 
			{
				int nIndex;
				nIndex=strSelectedPath.ReverseFind(L'\\');
				OnePathString=strTargetDir+L"\\"
							  +OnePathString.Mid(nIndex+1);
			}
			break;
		default:
			return FALSE;
		}
		break;
	case DECRYPT_CURRENT_PLACE:
		switch (DecryptInfo.nRestoreType)
		{
		case DECRYPT_PART:
			strSelectedPath=DecryptInfo.szSelectedPath;

			// for compare, the selected path has a "\" in the end
			strSelectedPath.TrimRight(L"\\");

			if(strSelectedPath.GetLength()==2) // for example "k:"
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
			break;
		default:
			return FALSE;
		}
		break;
	case DECRYPT_NEW_PLACE:
		switch (DecryptInfo.nRestoreType)
		{
		case DECRYPT_ENTIRE:
			OnePathString=strTargetDir + L"\\" 
						  +L"Disk"+OnePathString.Left(1) + L"\\"
						  +OnePathString.Mid(3);
			break;
		case DECRYPT_PART:
			strSelectedPath=DecryptInfo.szSelectedPath;

			// for compare, the selected path has a "\" in the end
			strSelectedPath.TrimRight(L"\\");

			if(strSelectedPath.GetLength()==2) // for example "k:"
			{
				OnePathString=strTargetDir + L"\\" 
							  +L"Disk"+OnePathString.Left(1) + L"\\"
							  +OnePathString.Mid(3);
			}
			else if(strSelectedPath.GetLength()>2)
			{
				int nIndex;
				nIndex=strSelectedPath.ReverseFind(L'\\');

				BOOL ajustWithTempDir = FALSE;
				WCHAR szTemp[MAX_PATH];
				memset(szTemp,0,sizeof(szTemp));
				if( GetTempDir(szTemp) ) {
					GetLongPathName(szTemp, szTemp, MAX_PATH);
					WCHAR szTemp2[MAX_PATH];
					memset(szTemp2,0,sizeof(szTemp2));
					GetLongPathName(strTargetDir, szTemp2, MAX_PATH);
					ajustWithTempDir = (wcscmp(szTemp, szTemp2) == 0);
				}

				if (ajustWithTempDir) {
					CString strLeafPath = strSelectedPath;
					int index = strLeafPath.ReverseFind(L'\\');
					strLeafPath = strLeafPath.Left(index + 1);
					if (strLeafPath.GetLength() + strTargetDir.GetLength() + OnePathString.GetLength() + 5 < MAX_PATH - 1){
						strLeafPath.Replace(L'\\', L'#');
						strLeafPath.Replace(L':', L'#');
						OnePathString=strTargetDir+L"\\"
							+strLeafPath + OnePathString.Mid(nIndex+1);
					}
					else {
						OnePathString=strTargetDir+L"\\"
							+OnePathString.Mid(nIndex+1);
					}
				}
				else {
					OnePathString=strTargetDir+L"\\"
						+OnePathString.Mid(nIndex+1);
				}
			}
			break;
		default:
			return FALSE;
		}
		break;
	default:
		return FALSE;
	}

	wcsncpy(ArrayData.szFileName,(LPCTSTR)OnePathString,MAX_PATH-1);
	return	TRUE;
}

void SetDecryptCancel()
{
	g_bUserCancel = TRUE;
}

BOOL GetDecryptFileSize(DECRYPT_INFO DecryptInfo)
{
	g_WorkState.qwTotalFileSize = 0;

	// Create tail file
	if( FALSE == GetSeperateFile(DecryptInfo.szImageFile,DecryptInfo.szPassword) )
	{
		TRACE(L"\nGetSeperateFile error in DecryptSelectionFile.");
		return FALSE;
	}

	DECRYPT_FILE_HANDLE FileHandle;
	if( FALSE == GetDecryptFileHandle(DecryptInfo.szImageFile,FileHandle) )
	{
		TRACE(L"\nGetDecryptFileHandle error in DecryptSelectionFile.");
		return FALSE;
	}

	ARRAY_NODE ArrayNode;
	
	__int64 linTargetFilePointer;

	if( FALSE == YGSetFilePointer(FileHandle.hTailFile,0,FILE_BEGIN,linTargetFilePointer ) )
	{
		TRACE(L"\nYGSetFilePointer error in GetDecryptFileSize.");
	}
	
	DWORD dwReadBytes;
	if( FALSE == ReadFile(FileHandle.hTailFile,&ArrayNode,sizeof(ArrayNode),&dwReadBytes,NULL) )
	{
		TRACE(L"\nReadFile error in DecryptSelectionFile.");
	}

	DWORD dwPathIndex;
	for(dwPathIndex=1; dwPathIndex<=ArrayNode.dwPathCount; dwPathIndex++)
	{
		ARRAY_DATA ArrayData;
		ReadFile(FileHandle.hTailFile,&ArrayData,sizeof(ArrayData),&dwReadBytes,NULL);

		// for debug temp comment begin
		// decrypt array data begin
		char chPassword[PASSWORD_SIZE];
		ZeroMemory(chPassword, PASSWORD_SIZE);
		WideCharToMultiByte(CP_ACP, 0, DecryptInfo.szPassword, PASSWORD_SIZE, chPassword, PASSWORD_SIZE, 0, 0);
		if( FALSE == YGAESDecryptData((unsigned char*)chPassword,strlen(chPassword),(unsigned char*)(&ArrayData),(unsigned char*)(&ArrayData),sizeof(ArrayData)) )
		{
			  TRACE(L"\nYGAESDecryptData error in DecryptSelectionFile."); 
			  return FALSE;
		}
		// decrypt array data end
		// for debug temp comment end

		BOOL bSelectedPath;
		bSelectedPath=bIsFileSelected(ArrayData.szFileName,DecryptInfo);

		if(bSelectedPath)
		{
			if( 0 == (ArrayData.dwFileAttribute & FILE_ATTRIBUTE_DIRECTORY) )
			{
				// if this is a file
				g_WorkState.dwTotalFileCount++;
				g_WorkState.qwTotalFileSize = g_WorkState.qwTotalFileSize + ArrayData.qwFileSize.QuadPart;
			}
		}
	}

	if( FALSE == CloseDecryptFileHandle(FileHandle) )
	{
		TRACE(L"\nCloseDecryptFileHandle error in DecryptSelectionFile.");
		// needn't return FALSE
	}

	return TRUE;
}


// 2004.09.20 added begin
BOOL ReadSelectionArray(LPCTSTR szImageFile,LPCTSTR szPassword,CStringArray &SelectionArray)
{
	// 2004.09.17 temp comment
	SelectionArray.RemoveAll();

	HANDLE hImageFile;

	hImageFile = CreateFile(szImageFile,GENERIC_READ,FILE_SHARE_READ | FILE_SHARE_WRITE,
		                    NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if( INVALID_HANDLE_VALUE == hImageFile )
	{
		TRACE(L"\nOpen %s error.",szImageFile);
		return FALSE;
	}

	__int64 linTargetFilePointer;
	if( FALSE == YGSetFilePointer(hImageFile,sizeof(FILE_ENCRYPT_HEAD),FILE_BEGIN,linTargetFilePointer) )
	{
		TRACE(L"\nYGSetFilePointer error in ReadSelectionArray.");
		CloseHandle(hImageFile);
		return FALSE;
	}
	
	SELECTION_ARRAY_NODE SelectionArrayNode;

	DWORD dwReadBytes;

	if( FALSE == ReadFile(hImageFile,&SelectionArrayNode,sizeof(SelectionArrayNode),&dwReadBytes,NULL) )
	{
		TRACE(L"\nReadFile error in ReaddSelectionArray, read SelectionArrayNode.");
		CloseHandle(hImageFile);
		return FALSE;
	}

	int nItemIndex;
	int nItemCount;
	nItemCount = SelectionArrayNode.dwSourceCount;

	WCHAR szSourceFile[MAX_PATH];
	memset(szSourceFile,0,sizeof(szSourceFile));

	for(nItemIndex=0; nItemIndex < nItemCount; nItemIndex++)
	{
		if( FALSE == ReadFile(hImageFile,szSourceFile,sizeof(szSourceFile),&dwReadBytes,NULL) )
		{
			TRACE(L"\nReadFile error in RecoredSelectionArray, write szSourceFile.");
			CloseHandle(hImageFile);
			return FALSE;
		}

        // Decrypt selection file begin
		char chPassword[PASSWORD_SIZE];
		ZeroMemory(chPassword, PASSWORD_SIZE);
		WideCharToMultiByte(CP_ACP, 0, szPassword, PASSWORD_SIZE, chPassword, PASSWORD_SIZE, 0, 0);
		if( FALSE == YGAESDecryptData((unsigned char*)chPassword,strlen(chPassword),(unsigned char*)szSourceFile,(unsigned char*)szSourceFile,sizeof(szSourceFile)) )
		{
			  TRACE(L"\nYGAESEncryptData error in RecordSelectionArray"); 
			  return FALSE;
		}
		// Decrypt selection file end

		TRACE(L"\nSelection array:%s",szSourceFile);
		SelectionArray.Add(szSourceFile);
	}
	// 2004.09.17 temp comment end

	CloseHandle(hImageFile);

	return TRUE;
}

// 2004.09.20 added end

// 2004.09.21 added begin

BOOL GetCommonLeftDir(const CStringArray &SelectionArray,CString &strCommonLeftDir)
{
	strCommonLeftDir.Empty();

	int nArraySize;
	nArraySize = SelectionArray.GetSize();

	if( 0 == nArraySize )
	{
		// the array is empty, no commonm left dir
		return TRUE;
	}

	int nMinItemLength;
	nMinItemLength = SelectionArray[0].GetLength();

	int nArrayIndex;
	for(nArrayIndex=1; nArrayIndex<nArraySize; nArrayIndex++)
	{
		if( SelectionArray[nArrayIndex].GetLength() < nMinItemLength )
		{
			nMinItemLength = SelectionArray[nArrayIndex].GetLength();
		}
	}

	int nResultLeftLength;
	nResultLeftLength = 0;

	int nLeftLength;
	for( nLeftLength=1; nLeftLength<=nMinItemLength; nLeftLength++ )
	{
		CString strCommonDir;
		strCommonDir = SelectionArray[0].Left(nLeftLength);
		for( nArrayIndex=1; nArrayIndex<nArraySize; nArrayIndex++ )
		{
			if( 0 != strCommonDir.CompareNoCase(SelectionArray[nArrayIndex].Left(nLeftLength)) )
			{
				nResultLeftLength=nLeftLength-1;

				if(nResultLeftLength > 0)
				{
					strCommonLeftDir = SelectionArray[0].Left(nResultLeftLength);
					int nIndex;
					nIndex = strCommonLeftDir.ReverseFind(L'\\');
					strCommonLeftDir = strCommonLeftDir.Left(nIndex);
				}
				return TRUE;
			}
		}
	}
	return TRUE;
}

// 2004.09.21 added end

BOOL CheckPassword(WCHAR *pEcyPassword,LPCTSTR szPassword)
{
	WCHAR chStoreKey[PASSWORD_SIZE];
	memset(chStoreKey,0,sizeof(chStoreKey));

	int nStoreSize;
	nStoreSize = wcslen(szPassword);

	if(nStoreSize > PASSWORD_SIZE )
	{
		nStoreSize = PASSWORD_SIZE;
	}

	// Encrypt key with key itself
	// may be this is single direction
	char chPassword[PASSWORD_SIZE];
	ZeroMemory(chPassword, PASSWORD_SIZE);
	WideCharToMultiByte(CP_ACP, 0, szPassword, PASSWORD_SIZE, chPassword, PASSWORD_SIZE, 0, 0);
	if( FALSE == YGAESEncryptData((unsigned char*)chPassword,strlen(chPassword),(unsigned char*)szPassword,(unsigned char*)chStoreKey,nStoreSize * sizeof(WCHAR)))
	{
		TRACE(L"\nYGAESEncryptData error in WriteTargetHead.");
		return FALSE;
	}

	return (!memcmp(pEcyPassword,chStoreKey,PASSWORD_SIZE*sizeof(WCHAR)));
}

BOOL GetRestoreFileHandle(LPCTSTR szSourceFile,LPCTSTR szTargetFile,HANDLE &hSourceFile,HANDLE &hTargetFile)
{
	CString strTitle;
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

// 2004.09.15 added begin

// this function is used to get the FileCount
// and one file name in the image if the FileCount is 1

BOOL GetImageInfo(DECRYPT_INFO *pDecryInfo,DWORD &dwFileCount,CString &strOneFileName)
{
	CString strText;
	dwFileCount = 0;
	strOneFileName.Empty();

	HANDLE hImageFile;
	hImageFile = CreateFile(pDecryInfo->szImageFile,
							GENERIC_READ | GENERIC_WRITE,
							FILE_SHARE_READ | FILE_SHARE_WRITE,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL);

	if( INVALID_HANDLE_VALUE == hImageFile )
	{
		TRACE(L"\nCan not open image file.");
		return FALSE;
	}

	FILE_ENCRYPT_HEAD FileEncryptHead;

    pDecryInfo->ulImageVersion = GetFileEncryptHead(hImageFile,pDecryInfo->szPassword,FileEncryptHead);

	if (!CheckDecryptOption(&FileEncryptHead.OptionInfo))	return FALSE;

	__int64 linTargetFilePointer;
	switch (pDecryInfo->ulImageVersion)
	{
	case 100:
		if (!CheckPassword((WCHAR *)&((FILE_ENCRYPT_HEAD_100 *)&FileEncryptHead)->szPassword,pDecryInfo->szPassword))
		{
			strText.LoadString(IDS_INVALID_PASSWORD);
#ifndef _YG_COMMAND_LINE
			AfxMessageBox(strText,MB_OK | MB_ICONWARNING);
#else
			wcout << (LPCTSTR)strText << endl;
#endif
			CloseHandle(hImageFile);
			return FALSE;
		}
		if( FALSE == YGSetFilePointer(
										hImageFile,
										((FILE_ENCRYPT_HEAD_100 *)&FileEncryptHead)->FileInfoPointer.QuadPart, 
										FILE_BEGIN,
										linTargetFilePointer
									  ) )

		{
			TRACE(L"\nYGSetFilePointer error in GetImageInfo");
			CloseHandle(hImageFile);
			return FALSE;
		}
		break;
	case 0x200:
		if (!CheckPassword((WCHAR *)&FileEncryptHead.szPassword,pDecryInfo->szPassword))
		{
			strText.LoadString(IDS_INVALID_PASSWORD);
#ifndef _YG_COMMAND_LINE
			AfxMessageBox(strText,MB_OK | MB_ICONWARNING);
#else
			wcout << (LPCTSTR)strText << endl;
#endif
			CloseHandle(hImageFile);
			return FALSE;
		}

		if( FALSE == YGSetFilePointer(
										hImageFile,
										FileEncryptHead.FileInfoPointer.QuadPart, 
										FILE_BEGIN,
										linTargetFilePointer
									  ) )

		{
			TRACE(L"\nYGSetFilePointer error in GetImageInfo");
			CloseHandle(hImageFile);
			return FALSE;
		}
		memcpy(&pDecryInfo->EncryptOption,&FileEncryptHead.OptionInfo,sizeof(ENCRYPTOPTION));
		break;
	case CURRENT_FILE_VERSION:
		if (!CheckPassword((WCHAR *)&FileEncryptHead.szPassword,pDecryInfo->szPassword))
		{
			if (FileEncryptHead.OptionInfo.bOptionOn && FileEncryptHead.OptionInfo.bErrorLimit)
			{
				FileEncryptHead.OptionInfo.uCurrentErr ++;
				SetFileEncryptHead(hImageFile,pDecryInfo->szPassword,FileEncryptHead,TRUE);
			}
			strText.LoadString(IDS_INVALID_PASSWORD);
#ifndef _YG_COMMAND_LINE
			AfxMessageBox(strText,MB_OK | MB_ICONWARNING);
#else
			wcout << (LPCTSTR)strText << endl;
#endif
			CloseHandle(hImageFile);
			return FALSE;
		}
		if( FALSE == YGSetFilePointer(
										hImageFile,
										FileEncryptHead.FileInfoPointer.QuadPart, 
										FILE_BEGIN,
										linTargetFilePointer
									  ) )

		{
			TRACE(L"\nYGSetFilePointer error in GetImageInfo");
			CloseHandle(hImageFile);
			return FALSE;
		}
		memcpy(&pDecryInfo->EncryptOption,&FileEncryptHead.OptionInfo,sizeof(ENCRYPTOPTION));
		break;
	default:
		TRACE(L"\nYGSetFilePointer error in GetImageInfo");
		strText.LoadString(IDS_INVALID_PASSWORD);
#ifndef _YG_COMMAND_LINE
		AfxMessageBox(strText,MB_OK | MB_ICONWARNING);
#else
		wcout << (LPCTSTR)strText << endl;
#endif
		CloseHandle(hImageFile);
		return FALSE;
	}
	pDecryInfo->ulCompressLevel = FileEncryptHead.dwCompressLevel;

	ARRAY_NODE ArrayNode;
	DWORD		dwReadBytes;
	if( FALSE == ReadFile(hImageFile,&ArrayNode,sizeof(ArrayNode),&dwReadBytes,NULL) )
	{
		TRACE(L"\nRead ArrayNode error in GetImageInfo.");
		CloseHandle(hImageFile);
		return FALSE;
	}

	dwFileCount = ArrayNode.dwFileCount;

	if( ArrayNode.dwFileCount > ArrayNode.dwPathCount )
	{
		TRACE(L"\nError ArrayNode, dwFileCount should be less or equal dwPathCount.");
	}

	ARRAY_DATA ArrayData;
	DWORD dwItemIndex;
	for(dwItemIndex=1; dwItemIndex<=ArrayNode.dwPathCount; dwItemIndex++)
	{
		if( FALSE == ReadFile(hImageFile,&ArrayData,sizeof(ArrayData),&dwReadBytes,NULL) )
		{
			TRACE(L"\nRead ArrayData error in GetImageInfo.");
			CloseHandle(hImageFile);
			return FALSE;
		}

		char chPassword[PASSWORD_SIZE];
		ZeroMemory(chPassword, PASSWORD_SIZE);
		WideCharToMultiByte(CP_ACP, 0, pDecryInfo->szPassword, PASSWORD_SIZE, chPassword, PASSWORD_SIZE, 0, 0);
		if( FALSE == YGAESDecryptData((unsigned char*)chPassword,strlen(chPassword),(unsigned char*)(&ArrayData),(unsigned char*)(&ArrayData),sizeof(ArrayData)) )
		{
			  TRACE(L"\nYGAESEncryptData error in WriteTargetTail."); 
			  return FALSE;
		}

		if( 0 == (FILE_ATTRIBUTE_DIRECTORY & ArrayData.dwFileAttribute) )
		{
			// find the item
			strOneFileName = ArrayData.szFileName;
			CloseHandle(hImageFile);
			return TRUE;
		}
	}

	CloseHandle(hImageFile);
    return TRUE;
}

BOOL IncreaseImageErrorLimit(DECRYPT_INFO *pDecryInfo)
{
	FILE_ENCRYPT_HEAD FileEncryptHead;
	HANDLE hImageFile;

	hImageFile = CreateFile(pDecryInfo->szImageFile,
							GENERIC_READ | GENERIC_WRITE,
							FILE_SHARE_READ | FILE_SHARE_WRITE,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL);

	if( INVALID_HANDLE_VALUE == hImageFile )
	{
		TRACE(L"\nCan not open image file.");
		return FALSE;
	}

    pDecryInfo->ulImageVersion = GetFileEncryptHead(hImageFile,pDecryInfo->szPassword,FileEncryptHead);

	switch (pDecryInfo->ulImageVersion)
	{
	case CURRENT_FILE_VERSION:
		if (FileEncryptHead.OptionInfo.bOptionOn && FileEncryptHead.OptionInfo.bErrorLimit)
		{
			FileEncryptHead.OptionInfo.uCurrentErr ++;
			SetFileEncryptHead(hImageFile,pDecryInfo->szPassword,FileEncryptHead,TRUE);
		}
		break;
	default:
		CloseHandle(hImageFile);
		return FALSE;
	}
	CloseHandle(hImageFile);
    return TRUE;
}


BOOL YGNSMGetAutoDecryptInfo(CONVERT_BUF *pDecryptFileInfo)
{
#ifdef NO_DRIVER_MODE
	if (pDecryptFileInfo == 0)
	{
		return FALSE;
	}
	CString strPath;
	if (GetEncryptInfoFilePath(strPath))
	{
		HANDLE hFile = CreateFile( strPath,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,
			OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN,NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			return FALSE;
		}
		vector<CONVERT_BUF> temp;
		DWORD w;
		while (1)
		{
			CONVERT_BUF info;
			ZeroMemory(&info, sizeof(CONVERT_BUF));
			BOOL ret = ReadFile(hFile, &info, sizeof(CONVERT_BUF), &w, 0);
			if (ret && w == sizeof(CONVERT_BUF))
			{
				temp.push_back(info);
			}
			else
			{
				break;
			}
		}
		CloseHandle(hFile);

		WCHAR szMonitorFile[MAX_PATH];
		ZeroMemory(szMonitorFile, sizeof(szMonitorFile));
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pDecryptFileInfo->szMonitorFile, MAX_PATH, szMonitorFile, MAX_PATH);
		CString f = szMonitorFile;

		for (vector<CONVERT_BUF>::iterator it = temp.begin();
			it != temp.end();
			++it)
		{
			ZeroMemory(szMonitorFile, sizeof(szMonitorFile));
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, it->szMonitorFile, MAX_PATH, szMonitorFile, MAX_PATH);
			CString d = szMonitorFile;

			if (f.GetLength() >= d.GetLength() && f.Left(d.GetLength()) == d && (f.GetLength() == d.GetLength() || f.GetAt(d.GetLength()) == _T('\\')))
			{
				CHAR szO[MAXPATHLEN];
				ZeroMemory(szO, sizeof(szO));
				strncpy(szO, pDecryptFileInfo->szMonitorFile, strlen(pDecryptFileInfo->szMonitorFile));
				memcpy(pDecryptFileInfo, &*it, sizeof(CONVERT_BUF));
				ZeroMemory(pDecryptFileInfo->szMonitorFile, sizeof(pDecryptFileInfo->szMonitorFile));
				strncpy(pDecryptFileInfo->szMonitorFile, szO, strlen(szO));
				ZeroMemory(pDecryptFileInfo->szDestFile, sizeof(pDecryptFileInfo->szDestFile));
				strncpy(pDecryptFileInfo->szDestFile, pDecryptFileInfo->szMonitorFile, strlen(pDecryptFileInfo->szMonitorFile) - 4);
				return TRUE;
			}
		}
		
	}
	return FALSE;
#else
	HANDLE hYGFSMonDrive;
	DWORD  dwRead;
	BOOL	bResult = FALSE;

	hYGFSMonDrive = CreateFile(L"\\\\.\\YGFSMon",GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,OPEN_EXISTING,0,NULL);

	if( INVALID_HANDLE_VALUE == hYGFSMonDrive)
	{
		TRACE(L"\nCreateFile error in InitInstance.");
		return FALSE;
	}
	bResult = DeviceIoControl(hYGFSMonDrive,IOCTL_YGFSMON_GET_MONITOR_FILE_INFO_NT,
		pDecryptFileInfo,sizeof(CONVERT_BUF),pDecryptFileInfo,sizeof(CONVERT_BUF),
		&dwRead,NULL);

	CloseHandle(hYGFSMonDrive);
	return bResult;
#endif

}

BOOL YGNSMRemoveMonitorFile(CONVERT_BUF *pDecryptFileInfo)
{
#ifdef NO_DRIVER_MODE
	return true;
#else
	HANDLE hYGFSMonDrive;
	DWORD  dwRead;
	BOOL	bResult = FALSE;

	hYGFSMonDrive = CreateFile(L"\\\\.\\YGFSMon",GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,OPEN_EXISTING,0,NULL);

	if( INVALID_HANDLE_VALUE == hYGFSMonDrive)
	{
		TRACE(L"\nCreateFile error in InitInstance.");
		return FALSE;
	}
	bResult = DeviceIoControl(hYGFSMonDrive,IOCTL_YGFSMON_REMOVE_MONITOR_ITEM_NT,
		pDecryptFileInfo,sizeof(CONVERT_BUF),NULL,0,
		&dwRead,NULL);
	CloseHandle(hYGFSMonDrive);
	return bResult;
#endif

}

BOOL YGNSMAddMonitorFile(CONVERT_BUF *pDecryptFileInfo)
{
#ifdef NO_DRIVER_MODE
	return true;
#else
	HANDLE hYGFSMonDrive;
	DWORD  dwRead;
	BOOL	bResult = FALSE;

	hYGFSMonDrive = CreateFile(L"\\\\.\\YGFSMon",GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,OPEN_EXISTING,0,NULL);

	if( INVALID_HANDLE_VALUE == hYGFSMonDrive)
	{
		TRACE(L"\nCreateFile error in InitInstance.");
		return FALSE;
	}
	bResult = DeviceIoControl(hYGFSMonDrive,IOCTL_YGFSMON_ADD_MONITOR_ITEM_NT,
		pDecryptFileInfo,sizeof(CONVERT_BUF),NULL,0,
		&dwRead,NULL);
	CloseHandle(hYGFSMonDrive);
	return bResult;
#endif

}

BOOL DecryptOneDirSeparately(LPCTSTR szSourceDirectory,LPCTSTR szPassword,BOOL bDeleteSource)
{
	BOOL bDecryptSucceed = TRUE;

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

			if( 0 == strSourceFile.Right(4).CompareNoCase(strFileExtension) )
			{
				DECRYPT_INFO DecryptInfo;
				memset(DecryptInfo.szImageFile,0,sizeof(DecryptInfo.szImageFile));
				wcsncpy(DecryptInfo.szImageFile,strSubPath,MAX_PATH-1);
				wcsncpy(DecryptInfo.szPassword,(LPCTSTR)szPassword,PASSWORD_SIZE-1);
				memset(DecryptInfo.szTargetPath,0,sizeof(DecryptInfo.szTargetPath));
				wcsncpy(DecryptInfo.szTargetPath,(LPCTSTR)szSourceDirectory,MAX_PATH-1);

				DWORD dwImageFileCount;
				CString strOneFileName;

				if(GetImageInfo(&DecryptInfo,dwImageFileCount,strOneFileName) )
				{
					if (dwImageFileCount == 1)
					{
						DecryptInfo.nRestoreType = DECRYPT_PART;
						DecryptInfo.nTargetType = DECRYPT_RELATIVE_PLACE;
						memset(DecryptInfo.szSelectedPath,0,sizeof(DecryptInfo.szSelectedPath));
						wcsncpy(DecryptInfo.szSelectedPath,(LPCTSTR)strOneFileName,MAX_PATH-1);
						DecryptSelectionFile(DecryptInfo);
						if (bDeleteSource)
						{
							SetFileAttributes(DecryptInfo.szImageFile,FILE_ATTRIBUTE_NORMAL);
							DeleteFile(DecryptInfo.szImageFile);
						}
					}
				}
			}
		}
		else
		{
			if( FALSE == DecryptOneDirSeparately((LPCTSTR)strSubPath,szPassword,bDeleteSource) )
			{
				bDecryptSucceed = FALSE;
				TRACE(L"\nDecryptOneDirSeperately %s error in DecryptOneDirSeparately.",(LPCTSTR)strSubPath);
			}
		}
	}while(FindNextFile(hFind,&FindFileData));
	
	FindClose(hFind);
	
	return bDecryptSucceed;
}

HANDLE GetDataInfoFileHandle(BOOL bCreateAlways)
{
#ifdef NO_DRIVER_MODE
	CString strInfoPath;
	if (!GetDataInfoFilePath(strInfoPath))
	{
		return INVALID_HANDLE_VALUE;
	}

	if (!bCreateAlways && GetFileAttributes(strInfoPath) == -1)
	{
		return INVALID_HANDLE_VALUE;
	}

	int failCount = 0;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	do 
	{
		if (!bCreateAlways)
		{
			hFile = CreateFile( strInfoPath,GENERIC_READ | GENERIC_WRITE,NULL,NULL,
				OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN,NULL);
		}
		else
		{
			hFile = CreateFile( strInfoPath,GENERIC_READ | GENERIC_WRITE,NULL,NULL,
				CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN,NULL);
		}

		if (hFile == INVALID_HANDLE_VALUE)
		{
			Sleep(10);
			++failCount;
		}
	} while(failCount <= 10 && hFile == INVALID_HANDLE_VALUE);
	return hFile;
#else
	return INVALID_HANDLE_VALUE;
#endif
}

BOOL GetDataInfoFilePath( CString& strPath )
{
#ifdef NO_DRIVER_MODE
	static CString strInfoPath;
	if (strInfoPath.GetLength() == 0)
	{
		BOOL ret = FALSE;
		WCHAR szPath[MAX_PATH];
		ZeroMemory(szPath, MAX_PATH * sizeof(WCHAR));
		ret = GetTempPath(MAX_PATH, szPath);
		if (!ret)
		{
			return false;
		}
		GetLongPathName(szPath, szPath, MAX_PATH);
		strInfoPath = szPath;
		strInfoPath += TEMP_DATA_INFO_FILE_NAME;
	}
	strPath = strInfoPath;
	if (strPath.GetLength() == 0)
	{
		return false;
	}
	return true;
#else
	return false;
#endif
}

BOOL GetEncryptInfoFilePath( CString& strPath )
{
#ifdef NO_DRIVER_MODE
	static CString strInfoPath;
	if (strInfoPath.GetLength() == 0)
	{
		BOOL ret = FALSE;
		WCHAR szPath[MAX_PATH];
		ZeroMemory(szPath, MAX_PATH * sizeof(WCHAR));
		ret = GetTempPath(MAX_PATH, szPath);
		if (!ret)
		{
			return false;
		}
		GetLongPathName(szPath, szPath, MAX_PATH);
		strInfoPath = szPath;
		strInfoPath += TEMP_ENCRYPT_INFO_FILE_NAME;
	}
	strPath = strInfoPath;
	if (strPath.GetLength() == 0)
	{
		return false;
	}
	return true;
#else
	return false;
#endif
}

BOOL RecordDataInfo( WCHAR* szPath, HANDLE hFile )
{
#ifdef NO_DRIVER_MODE
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	HANDLE h = GetDataInfoFileHandle(false);
	if (h != INVALID_HANDLE_VALUE)
	{
		ExcludeFileInfo temp;
		wcsncpy(temp.strFilePath, szPath, MAX_PATH);
		temp.dwLastModifyTime = GetLastModifyTime(hFile);
		DWORD w = 0;
		SetFilePointer(h, 0, 0, SEEK_END);
		WriteFile(h, &temp, sizeof(ExcludeFileInfo), &w, 0);
		CloseHandle(h);
	}

	return TRUE;
#else
	return FALSE;
#endif
}

FILETIME GetLastModifyTime( HANDLE hFile )
{
	FILETIME ftCreate,ftModify,ftAccess,ftLocal;
#ifdef _DEBUG
	SYSTEMTIME sysTime;
#endif

	GetFileTime(hFile, &ftCreate, &ftAccess, &ftModify); //

#ifdef _DEBUG
	FileTimeToLocalFileTime(&ftModify, &ftLocal); //

	FileTimeToSystemTime(&ftLocal, &sysTime);
#endif
	return ftModify;
}

BOOL GetTempInfoFilePath( CString& strPath )
{
#ifdef NO_DRIVER_MODE
	static CString strInfoPath;
	if (strInfoPath.GetLength() == 0)
	{
		BOOL ret = FALSE;
		WCHAR szPath[MAX_PATH];
		ZeroMemory(szPath, MAX_PATH * sizeof(WCHAR));
		ret = GetTempPath(MAX_PATH, szPath);
		if (!ret)
		{
			return false;
		}
		GetLongPathName(szPath, szPath, MAX_PATH);
		strInfoPath = szPath;
		strInfoPath += TEMP_TEMP_FILE_INFO_FILE_NAME;
	}
	strPath = strInfoPath;
	if (strPath.GetLength() == 0)
	{
		return false;
	}
	return true;
#else
	return false;
#endif
}

BOOL CheckIsSelfExtractingFile( const CString& path, LARGE_INTEGER& address, LARGE_INTEGER& size, BOOL& isValid )
{
	CString temp;
	BOOL isSelfExtractingFile = FALSE;
	isValid = FALSE;
	ULARGE_INTEGER fileSize;
	fileSize.QuadPart = 0;
	HANDLE hFile = CreateFile(path,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		fileSize.LowPart = GetFileSize(hFile, &fileSize.HighPart);
		if (SetFilePointer(hFile, -(int(sizeof(SELF_EXTRACTING_IDENTITY)) * 2 + int(sizeof(LARGE_INTEGER)) * 2), 0, FILE_END) != INVALID_SET_FILE_POINTER) {
			LARGE_INTEGER p;
			p.QuadPart = 0;
			SetFilePointerEx(hFile, p, &p, FILE_CURRENT);
			WCHAR identityBuf[IMAGE_IDENTITY_SIZE];
			ZeroMemory(identityBuf, sizeof(identityBuf));
			DWORD dwRead = 0;
			if (ReadFile(hFile, identityBuf, sizeof(SELF_EXTRACTING_IDENTITY), &dwRead, 0) && dwRead == sizeof(SELF_EXTRACTING_IDENTITY)) {
				if (wcscmp(identityBuf, SELF_EXTRACTING_IDENTITY) == 0) {
					isSelfExtractingFile = TRUE;
					if (ReadFile(hFile, &address.QuadPart, sizeof(address), &dwRead, 0) 
						&& dwRead == sizeof(LARGE_INTEGER)
						&& ReadFile(hFile, &size.QuadPart, sizeof(size), &dwRead, 0) 
						&& dwRead == sizeof(LARGE_INTEGER)
						&& ReadFile(hFile, identityBuf, sizeof(SELF_EXTRACTING_IDENTITY), &dwRead, 0)
						&& dwRead == sizeof(SELF_EXTRACTING_IDENTITY)) {
							if (wcscmp(identityBuf, SELF_EXTRACTING_IDENTITY) == 0
								&& address.QuadPart + size.QuadPart <= fileSize.QuadPart){
								isValid = TRUE;
							}
					}
				}
			}
		}
		CloseHandle(hFile);
	}
	return isSelfExtractingFile;
}