#ifndef AM_BAKFUNCTION_DLL_H
#define AM_BAKFUNCTION_DLL_H

#include "stdafx.h"
#include "..\AM01Struct\AM01Struct.h"

BOOL BackupSelectionFile(SETTING_DATA_INFO SettingDataInfo,CStringArray &SelectionArray);

BOOL BackupSelection(
					  CStringArray &SelectionArray,
					  SETTING_DATA_INFO SettingDataInfo,
					  CDATA_ARRAY &TargetArray
					);

BOOL DiffBackupSelectionFile(LPCTSTR ImageFilePath,PBACKUPOPTION pBackupOption);

BOOL NewGetFileHandlePro(AM_FILE_HANDLE *pFileHandle, LPCTSTR TargetDirectory, LPCTSTR TargetFileName,CDATA_ARRAY &TargetArray,IMAGE_HEAD *pImageHead, DWORD *pdwErrorCode);

BOOL DiffBackupSelection(CStringArray &SelectionArray, LPCTSTR TargetDirectory, LPCTSTR TargetFileName,CDATA_ARRAY &TargetArray,PBACKUPOPTION pBackupOption);

BOOL DiffStoreDirectoryInfo(
							  LPCTSTR lpstrDirectoryPath,
							  AM_FILE_HANDLE *pFileHandle,
							  DWORD *pdwPathID
							  // CStringArray &TotalFilePathArray
						   );

BOOL DiffBackupDirectory(LPCTSTR OnePathString,
						 AM_FILE_HANDLE *pFileHandle,
						 IMAGE_HEAD *pImageHead,
						 DWORD *pdwPathID,
						 CDATA_ARRAY &TargetArray,
						 PBACKUPOPTION   pBackupOption);

BOOL DiffBackupFile( LPCTSTR		 lpstrSourceFilePath,
					 AM_FILE_HANDLE	 *pFileHandle,
				     IMAGE_HEAD		 *pImageHead,
					 DWORD			 *pdwPathID,
					 CDATA_ARRAY	 &TargetArray,
					 PBACKUPOPTION   pBackupOption);

BOOL DiffStoreExtraDirInfo(
						    CStringArray &SelectionArray,
						    AM_FILE_HANDLE *pFileHandle,
						    DWORD *pdwPathID
						    // CStringArray &TotalFilePathArray
						  );

BOOL OpenNewFile(LPCTSTR szTargetDir, LPCTSTR szTargetName , CDATA_ARRAY &TargetArray,IMAGE_HEAD *pSourceHead, HANDLE *phCurrentTarget);


BOOL BackupFile(LPCTSTR lpstrSourceFilePath,
				AM_FILE_HANDLE *pFileHandle,
				IMAGE_HEAD *pImageHead,
				CDATA_ARRAY &TargetArray,
				DWORD *pdwPathID,
				DWORD *pdwErrorCode,
				PBACKUPOPTION pBackupOption);

BOOL BackupDirectory(LPCTSTR lpstrSourceDirectory,
					 AM_FILE_HANDLE *pFileHandle,
					 IMAGE_HEAD *pImageHead,
					 CDATA_ARRAY &TargetArray,
					 DWORD &dwPathID,
					 PBACKUPOPTION pBackupOption);

BOOL GetWorkState(AM01WORKSTATE *pWorkState);

BOOL SetBakCancel();

BOOL CheckSelectionArray(CStringArray &SelectionArray);

void GetTotalFileSize(CStringArray &SelectionArray);

BOOL GetDirectoryInfo(LPCTSTR szSourceDirectory);

BOOL GetFileInfo(LPCTSTR FilePath);


BOOL bPathInSegment(
					 LPCTSTR		SourceFilePath,
					 CPATH_ARRAY	&PathArray,
					 DWORD			dwBeginOffset,
					 DWORD			*pdwPathOffset
				   );

BOOL LoadFirstSegment(HANDLE hArrayFile);

BOOL ClearSignature(LPCTSTR szLastImage);

BOOL WriteFileHead(AM_FILE_HANDLE *pFileHandle,const SETTING_DATA_INFO SettingDataInfo,CDATA_ARRAY &TargetArray);

BOOL RecordStampInfo(HANDLE hStampFile);

BOOL RecordSelection(HANDLE hImageFile, CStringArray &SelectionArray);

BOOL StoreExtraDirInfo(CStringArray &SelectionArray,AM_FILE_HANDLE *pFileHandle,DWORD *pdwPathID);

BOOL IsOneDirectory(LPCTSTR pstr);

BOOL GetLastFileName(LPCTSTR szDirFileName,WCHAR *szLastImage);

BOOL SelectLastImage(WCHAR *szLastImage);

BOOL NewLoadSegment(
				     HANDLE	hArrayFile, 
				     CPATH_ARRAY &PathArray,
				     DWORD dwSegmentNumber,
				     DWORD dwSegmentSize
				   );

BOOL GetHandleForCombine(AM_FILE_HANDLE *pFileHandle, LPCTSTR TargetDirectory, LPCTSTR TargetFileName,DWORD *pdwErrorCode);

BOOL bExistingFileNew( LPCTSTR SourceFilePath, HANDLE  hArrayFile, DWORD *pdwPathIndex );

BOOL bIsFileChanged(CString SourceFilePath, HANDLE hTotalIndexFile, DWORD dwPathIndex);

BOOL GetImageSeperateIdentity(LPCTSTR szSourceFilePath, WCHAR*szImageSeperateIdentity);

BOOL bIsFoundInMorePath(LPCTSTR pOneSubString,CStringArray &ExtraDirArray);

BOOL GetNextFileNamePro(LPCTSTR szTargetDirectory, LPCTSTR szTargetFileName,WCHAR *szDataFilePath,DWORD dwFileNumber);

BOOL GetNextFileName(LPCTSTR szTargetFile, WCHAR *szDataFilePath,DWORD dwFileNumber);

BOOL StoreDirectoryInfo(LPCTSTR lpstrDirectoryPath,AM_FILE_HANDLE *pFileHandle, DWORD *pdwPathID, DWORD *pdwErrorCode);

BOOL CheckOpenFile(LPCTSTR szOriginalFileName,WCHAR *szResultFileName);

BOOL BackupFileData(HANDLE hFile,HANDLE &hData,LARGE_INTEGER *pDataFilePointer,
					LPCTSTR lpstrSourceFilePath,
					IMAGE_HEAD *pImageHead,
					CDATA_ARRAY &TargetArray,
					DWORD * pdwErrorCode,
					PBACKUPOPTION pBackupOption);
BOOL SeparateFileFromOneImg(WCHAR *szLastImage);

#endif
