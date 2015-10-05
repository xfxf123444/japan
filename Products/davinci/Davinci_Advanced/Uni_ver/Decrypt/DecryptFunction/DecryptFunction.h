#ifndef YG_DECRYPT_FUNCTION_H
#define YG_DECRYPT_FUNCTION_H

#include "..\..\EncryptToolStruct\EncryptToolStruct.h"
#include "..\..\..\..\Davinci_tech\lib\Crypto\Cur_ver\Export\Crypto.h"
#include "..\..\..\..\Davinci_tech\ZLib\Cur_ver\Export\ZLib.h"

BOOL DecryptSelectionFile(DECRYPT_INFO DecryptInfo);

BOOL GetDecryptTailFile(LPCTSTR szTargetFile, CString &strTailFile);

BOOL GetOneSeperateFile(HANDLE hFirstFile, HANDLE hSecondFile, LARGE_INTEGER FilePointer, unsigned __int64 qwFileSize);

BOOL GetSeperateFile(LPCTSTR szImageFile,LPCTSTR szPassword);

BOOL GetDecryptFileHandle(LPCTSTR szImageFile,DECRYPT_FILE_HANDLE &FileHandle);

BOOL CloseDecryptFileHandle(DECRYPT_FILE_HANDLE FileHandle);

BOOL RestoreOnePath(HANDLE hImageFile, ARRAY_DATA ArrayData,DECRYPT_INFO DecryptInfo,BOOL &bCancelRestore);

BOOL RestoreOneFile(HANDLE hImageFile, DECRYPT_INFO DecryptInfo,ARRAY_DATA ArrayData);

// 2004.08.24 added begin
BOOL RestoreOneDir(DECRYPT_INFO DecryptInfo,ARRAY_DATA ArrayData);

BOOL PrepareOnePath(LPCTSTR szOnePath);

BOOL CreateOneDir(LPCTSTR szOneDir);

BOOL GetLeftPart(LPCTSTR szOnePath, WCHAR * szLeftPart);

BOOL bPathExist(LPCTSTR OnePath);

// 2004.08.24 added end

// 2004.08.26 added begin

BOOL GetTempDir(WCHAR *szTempDir);

BOOL GetLongMainName(LPCTSTR szImageFile,WCHAR *szLongMainName);

BOOL bIsFileSelected(LPCTSTR szOnePath, DECRYPT_INFO DecryptInfo);

BOOL AdjustOnePathString(ARRAY_DATA &ArrayData, DECRYPT_INFO DecryptInfo);

// 2004.08.26 added end

// 2004.08.27 added begin

void SetDecryptCancel();

void GetWorkState(DA_WORK_STATE &WorkState);

BOOL GetDecryptFileSize(DECRYPT_INFO DecryptInfo);

// 2004.08.27 added end

// 2004.09.20 added begin

BOOL ReadSelectionArray(LPCTSTR szImageFile,LPCTSTR szPassword,CStringArray &SelectionArray);

// 2004.09.20 added end

// 2004.09.21 added begin

BOOL GetCommonLeftDir(const CStringArray &SelectionArray,CString &strCommonLeftDir);

BOOL CheckPassword(WCHAR *pEcyPassword,LPCTSTR szPassword);

BOOL GetRestoreFileHandle(LPCTSTR szSourceFile,LPCTSTR szTargetFile,HANDLE &hSourceFile,HANDLE &hTargetFile);

// 2004.09.15 added begin
BOOL IncreaseImageErrorLimit(DECRYPT_INFO *pDecryInfo);
BOOL GetImageInfo(DECRYPT_INFO *pDecryInfo,DWORD &dwFileCount,CString &strOneFileName);
BOOL YGNSMAddMonitorFile(CONVERT_BUF *pDecryptFileInfo);
BOOL YGNSMRemoveMonitorFile(CONVERT_BUF *pDecryptFileInfo);
BOOL YGNSMGetAutoDecryptInfo(CONVERT_BUF *pDecryptFileInfo);
BOOL DecryptOneDirSeparately(LPCTSTR szSourceDirectory,LPCTSTR szPassword,BOOL bDeleteSource);
BOOL FixupDecryptInfo(DECRYPT_INFO *pDecryptInfo);

HANDLE GetDataInfoFileHandle(BOOL bCreateAlways);
BOOL GetDataInfoFilePath(CString& strPath);
BOOL GetEncryptInfoFilePath(CString& strPath);
BOOL GetTempInfoFilePath(CString& strPath);
BOOL RecordDataInfo(WCHAR* szPath, HANDLE hFile);
FILETIME GetLastModifyTime(HANDLE hFile);

BOOL CheckIsSelfExtractingFile( const CString& path, LARGE_INTEGER& address, LARGE_INTEGER& size, BOOL& isValid );
BOOL WriteImageFileData(HANDLE target, LARGE_INTEGER& targetAddress, HANDLE source, LARGE_INTEGER& sourceAddress, LARGE_INTEGER& size);
#endif