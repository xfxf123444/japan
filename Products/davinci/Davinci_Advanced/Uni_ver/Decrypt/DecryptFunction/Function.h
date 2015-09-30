#ifndef FILE_ENCRYPT_FUNCTION_H
#define FILE_ENCRYPT_FUNCTION_H

#include "..\FileDecrypt\stdafx.h"
#include "..\..\EncryptToolStruct\EncryptToolStruct.h"
#include "..\..\DavinciFunction\DavinciFunction.h"

BOOL SelectFile(LPCTSTR szFileExt,LPCTSTR szFileType,char *szFile);

BOOL CheckPassword(UCHAR *pEcyPassword,LPCTSTR szPassword);

BOOL GetRestoreFileHandle(LPCTSTR szSourceFile,LPCTSTR szTargetFile,HANDLE &hSourceFile,HANDLE &hTargetFile);

ULONG CheckValidSource(LPCTSTR szSourceFile,LPCTSTR szPassword);

BOOL SelectFolder(HWND hWnd,char *ach);

// 2004.09.15 added begin

BOOL GetImageInfo(DECRYPT_INFO *pDecryInfo,DWORD &dwFileCount,CString &strOneFileName);

// 2004.09.15 added end

#endif