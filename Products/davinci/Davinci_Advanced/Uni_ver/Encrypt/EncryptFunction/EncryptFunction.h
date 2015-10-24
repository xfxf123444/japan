#ifndef YG_ENCRYPT_FUNCTION_H
#define YG_ENCRYPT_FUNCTION_H

// 2004.10.10 modify begin
// #include "EncryptInfo.h"
// 2004.10.10 modify end
#include "..\..\EncryptToolStruct\EncryptToolStruct.h"
#include "..\..\..\..\Davinci_tech\lib\Crypto\Cur_ver\Export\Crypto.h"
#include "..\..\..\..\Davinci_tech\ZLib\Cur_ver\Export\ZLib.h"

// 2004.08.19 added begin

// not correct
// for test begin
class CEncryptInfo;
// for test end

BOOL EncryptSelection(CEncryptInfo &EncryptInfo);

BOOL GetFileHandle(LPCTSTR szImageFile, ENCRYPT_FILE_HANDLE &FileHandle);

BOOL CloseFileHandle(ENCRYPT_FILE_HANDLE &FileHandle);


// 2004.08.19 added end

// 2004.08.20 added begin

BOOL WriteTargetHead(HANDLE hTargetFile,LPCTSTR szPassword,DWORD m_dwCompressLevel,PENCRYPTOPTION pEncryptOption);

BOOL GetTailFileName(LPCTSTR szTargetFile, CString &strTailFile);

// 2004.08.20 added end

// 2004.08.23 added begin

BOOL IsOneDirectory(LPCTSTR szFileName);

BOOL EncryptDirectory(LPCTSTR szSourceDir,ENCRYPT_FILE_HANDLE FileHandle,LPCTSTR szPassword,DWORD &dwPathID,ULONG ulCompressLevel);

BOOL EncryptFile(LPCTSTR szSourceFile,ENCRYPT_FILE_HANDLE FileHandle,LPCTSTR szPassword,DWORD &dwPathID,ULONG ulCompressLevel);

BOOL CombineTwoFile(HANDLE hFirstFile, HANDLE hSecondFile);

BOOL CombineFileToOneImg(ENCRYPT_FILE_HANDLE &FileHandle,DWORD dwTotalPath,DWORD dwTotalFileCount,LPCTSTR szPassword);

// 2004.08.23 added end

// 2004.08.24 added begin

BOOL StoreDirectoryInfo(LPCTSTR szDirName,HANDLE hTailFile,LPCTSTR szPassword,DWORD &dwPathID);

BOOL StoreExtraDirInfo(CStringArray &SelectionArray,HANDLE hTailFile,LPCTSTR szPassword,DWORD &dwPathID);

BOOL bIsFoundInMorePath(LPCTSTR pOneSubString,CStringArray &ExtraDirArray);

// 2004.08.24 added end

// 2004.08.27 added begin

void SetEncryptCancel();

// 2004.08.27 added end

// 2004.09.16 added begin

BOOL EncryptOneFile(LPCTSTR szSourceFile,LPCTSTR szTargetFile,LPCTSTR szPassword,BOOL bDeleteSource,DWORD dwCompressLevel,BOOL bErrorLimit,ULONG ulMaxInputNumber);

BOOL EncryptOneDirSeparately(LPCTSTR szSourceDirectory,LPCTSTR szPassword,BOOL bDeleteSource,DWORD dwCompressLevel,BOOL bErrorLimit,ULONG ulMaxErrLimit);

// 2004.09.16 added end

// 2004.09.20 added begin

BOOL RecordSelectionArray(HANDLE hImageFile,LPCTSTR szPassword,const CStringArray &SelectionArray);

BOOL GetTargetFileName(LPCTSTR szSourceFile,CString &strTargetFile);

BOOL GetBackupFileHandle(LPCTSTR szSourceFile,LPCTSTR szTargetFile,HANDLE &hSourceFile,HANDLE &hTargetFile);

BOOL WriteTargetTail(HANDLE hTargetFile,LPCTSTR szSourceFile,LPCTSTR szPassword);

// 2004.08.17 added begin

BOOL CheckPlacement(LPCTSTR szSourceDir, LPCTSTR szTargetImage);

// 2004.08.17 added end

// 2004.09.03 added begin
void AddDelMethod(CComboBoxEx* pBox);
// 2004.09.03 added end
BOOL CheckSpeicalFile(LPCTSTR p);
#endif