#ifndef AM_FUNCTION_DLL_H
#define AM_FUNCTION_DLL_H

#include "stdafx.h"
#include "..\AM01Struct\AM01Struct.h"
BOOL GetOsVersion();

// -- function for restore begin
WCHAR * strrchrcut(WCHAR* szSource);
BOOL TrimRightChar(WCHAR *szOneString, const WCHAR chOneChar, const int nMaxCharCount);
WCHAR * strrchrpro(WCHAR* szSource,int chFindChar);
BOOL GetLongMainName(LPCTSTR szImageFile,WCHAR *szLongMainName);
DWORD GetTotalIndexOffset(DWORD dwMaxPathID, DWORD dwTimeStampCount, DWORD dwOnePathID);
BOOL GetSourceArray(LPCTSTR szLastFileName, CDATA_ARRAY &SourceArray);
BOOL FreeSourceArray(CDATA_ARRAY &SourceArray);
BOOL GetSelectionArrayPro(LPCTSTR szLastImage,CStringArray &SelectionArray);
BOOL GetSelectionLink(LPCTSTR szLastImage,SELECTION_LINK **pSelectionHead);
BOOL FreeSelectionLink(SELECTION_LINK *pSelectionHead);
BOOL CheckLastImage(LPCTSTR szImageFile, DWORD *pdwErrorCode);
BOOL GetImageArray(CDATA_ARRAY &ImageArray,LPCTSTR szLastFileName);
BOOL GetRequiredHandle(CURRENT_RESTORE_INFO *pCurrentInfo,DWORD dwFileNumber, const CDATA_ARRAY &ImageArray);
BOOL InsertSelectionNode(SELECTION_LINK **ppSelectionHead,SELECTION_DATA SelectionData);
BOOL CheckImage(LPCTSTR szImageName, DWORD dwFileNumber, const CDATA_ARRAY &ImageArray);
BOOL SelectFile(WCHAR *szFileExt,WCHAR *szFileType,WCHAR *szFile);
BOOL GetDataFilePointer(HANDLE hTotalIndexFile,
					    DWORD dwMaxPathID,
						DWORD dwPathID, 
						DWORD dwTimeStampNumber, 
						INDEX_DATA *pIndexData);
BOOL ReadDataFile(CURRENT_RESTORE_INFO  *pCurrentInfo,
				  const         CDATA_ARRAY &ImageArray, 
				  void          *pBuffer, 
				  DWORD         dwRequestBytes);
BOOL GetLeftPart(LPCTSTR OnePath, WCHAR * szLeftPart);
BOOL GetRightPart(LPCTSTR szOnePath,WCHAR *szRightPart);

//-- function for restore end

//-- function for squeeze begin
BOOL WriteDataFile(
					   HANDLE             &hCurrentTarget,
					   void               *pDataBuffer,
					   DWORD              dwDataSize,
					   unsigned __int64   &qwTargetSize,
					   CDATA_ARRAY        &TargetArray,
					   IMAGE_HEAD         *pImageHead, // this is used to open the new image
					   PBACKUPOPTION	  pBackupOption
				  );

BOOL OpenNewTarget(HANDLE &phCurrentTarget, CDATA_ARRAY &TargetArray, BYTE *pBuffer, DWORD dwRequestBytes, DWORD WrittenBytes, unsigned __int64 &qwTargetSize, IMAGE_HEAD *pSourceHead,PBACKUPOPTION pBackupOption,BOOL bDiskFull);
BOOL GetOneFileSize(HANDLE hFile, unsigned __int64 *pqwFileSize);
BOOL WriteSignature(LPCTSTR szImageFile, CDATA_ARRAY &TargetArray);
BOOL GetNewTargetHandle(HANDLE &hCurrentTarget, WCHAR *szTargetName, CDATA_ARRAY &TargetArray,PBACKUPOPTION pBackupOption,BOOL bDiskFull);
void GetGuidString(WCHAR *szGuid,GUID OneGuid);
BOOL WriteTargetHead(HANDLE hCurrentTarget, IMAGE_HEAD *pSourceHead, DWORD dwCurrentTargetNumber, TABLE_DATA TargetData);
BOOL SelectFilePro(LPCTSTR szFileExt,LPCTSTR szFileType,LPCTSTR szDefaultFile, WCHAR *szFileName,BOOL bOpenFile);
BOOL CheckFreeSpace(LPCTSTR DriveLetter);
BOOL AddOneTargetElement(CDATA_ARRAY &TargetArray,LPCTSTR szTargetImage);
BOOL GetFileHandle(AM_FILE_HANDLE *FileHandle,LPCTSTR szTargetImage,DWORD *pdwErrorCode);
BOOL OpenFileHandle(AM_FILE_HANDLE *pFileHandle,
				   LPCTSTR szTargetImage,
				   DWORD *pdwErrorCode);
BOOL GetTempDir(WCHAR *szTempDir);
BOOL AdjustIndexHead(HANDLE hCurrentIndexFile, DWORD dwPathID);
BOOL CurrentIdxToTotalIdx(AM_FILE_HANDLE *pFileHandle);
BOOL CombineFileToOneImg(AM_FILE_HANDLE *pFileHandle,CDATA_ARRAY &TargetArray);
BOOL RecordLastImageInfo(AM_FILE_HANDLE *pFileHandle,CDATA_ARRAY &TargetArray);
void CloseFileHandle(AM_FILE_HANDLE *pFileHandle);
BOOL CombineTwoFile(HANDLE hFirstFile, HANDLE hSecondFile);
BOOL RecordFileTable(CDATA_ARRAY &TargetArray);
BOOL AdjustLastFileName(CDATA_ARRAY &TargetArray);
BOOL CreateDirFilePro(CDATA_ARRAY &TargetArray);
BOOL GetMainFileName(LPCTSTR szSourceFileName,WCHAR *szMainFileName);
BOOL GetChecksum(HANDLE hFile,DWORD dwOffset,ULONGLONG *pqwChecksum);
VOID ClearExtraFile(LPCTSTR TargetFileName);
BOOL GetSeperateFile(LPCTSTR szImageFile,LPCTSTR szMainName,LPCTSTR TargetDirectory);
BOOL GetOneSeperateFile(HANDLE hFirstFile, HANDLE hSecondFile, LARGE_INTEGER FilePointer, unsigned __int64 qwFileSize);
BOOL NewCurrentIdxToTotalIdx(LPCTSTR TargetFileName, DWORD dwMaxPathID,DWORD *pdwTimeStamp);

//-- function for squeeze end

//-- function for UI begin

BOOL CreateDirFile(LPCTSTR szLastImage);

// -- function for UI end

BOOL SelectFolder(HWND hWnd,WCHAR *ach);

// 2004.05.20 added begin

void EncryptPassword(WCHAR *pchPassword,int nPasswordSize);
void DecryptPassword(WCHAR *pchPassword,int nPasswordSize);
BOOL YGEncryptData(WCHAR *pchPassword,unsigned char *pDataBuffer, DWORD dwDataSize);
BOOL YGDecryptData(WCHAR *pchPassword,unsigned char *pDataBuffer, DWORD dwDataSize);
BOOL GetImageHead(const WCHAR *szLastImage,IMAGE_HEAD *pImageHead);
BOOL WriteLog(WCHAR *szFile,ULONG RetryMinutes);

BOOL GetFailRetryScheduleFileName(WCHAR *szScheduleFile);
BOOL RecordFailRetrySchedule(WCHAR *szLastImage,DWORD dwTimeStamp);
BOOL RemoveFailRetrySchedule(PFAILRETRYSCHEDULE pScheduleItem);
BOOL ReadFailRetrySchedule(CFAILRETRYSCHEDULE &ScheduleItemArray);
BOOL IsFailRetryScheduleFile(LPCTSTR szScheduleFileName);
BOOL RecordFailRetryScheduleHead(LPCTSTR szScheduleFile);
BOOL GetApplicationDataPath(WCHAR *szPath);
BOOL UpdateIndexAndArrayFile(AM_FILE_HANDLE *pHandle,WCHAR *szFile,WIN32_FIND_DATA *pFindData,LARGE_INTEGER *pFilePointer,DWORD dwTimeStamp);

BOOL YGSetFilePointer(HANDLE hFile, __int64 linDistance, DWORD dwMoveMethod, __int64 &linTargetFilePointer);


BOOL SearchLastImage(LPTSTR szImageFile, DWORD *pdwErrorCode);

#endif