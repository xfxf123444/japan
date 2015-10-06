#ifndef DAVINCI_FUNCTION_H
#define DAVINCI_FUNCTION_H

#include "..\EncryptToolStruct\EncryptToolStruct.h"

ULONG GetFileEncryptHead(HANDLE hImageFile,LPCTSTR szPassword,FILE_ENCRYPT_HEAD &FileEncryptHead);
BOOL  SetFileEncryptHead(HANDLE hImageFile,LPCTSTR szPassword,FILE_ENCRYPT_HEAD &FileEncryptHead,BOOL bOptionOnly);

// 2005.01.07 added begin
BOOL YGSetFilePointer(HANDLE hFile, __int64 linDistance, DWORD dwMoveMethod, __int64 &linTargetFilePointer);
// 2005.01.07 added end
BOOL SelectFile(LPCTSTR szFileExt,LPCTSTR szFileType,TCHAR *szFile);
void GetWorkState(DA_WORK_STATE &WorkState);
BOOL SelectFolder(HWND hWnd,TCHAR *ach);

BOOL SelectFilePro(LPCTSTR szFileExt,LPCTSTR szFileType,TCHAR *szFile);
ULONG YGNSMCheckSum(UCHAR *pBuffer,ULONG ulSize);
BOOL UpdateDecryptOption(HANDLE hImageFile,LPCTSTR szPassword,ENCRYPTOPTION *pEncryOption);
BOOL CheckDecryptOption(PENCRYPTOPTION pOption);

#endif
