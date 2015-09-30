#ifndef YG_FILE_ENCRYPT_FUNCTION_H
#define YG_FILE_ENCRYPT_FUNCTION_H

#ifdef _FILE_ENCRYPT
#include "..\FileEncrypt\stdafx.h"
#endif

#ifdef _SHELL_ENCRYPT
#include "..\ShellEncrypt\stdafx.h"
#endif

#ifdef _YG_COMMAND_LINE
#include "..\..\FED\stdafx.h"
#endif

#include "EncryptInfo.h"
#include "..\..\EncryptToolStruct\EncryptToolStruct.h"

BOOL SelectFile(LPCTSTR szFileExt,LPCTSTR szFileType,char *szFile);

BOOL GetTargetFileName(LPCTSTR szSourceFile,CString &strTargetFile);

BOOL GetBackupFileHandle(LPCTSTR szSourceFile,LPCTSTR szTargetFile,HANDLE &hSourceFile,HANDLE &hTargetFile);

BOOL WriteTargetTail(HANDLE hTargetFile,LPCTSTR szSourceFile,LPCTSTR szPassword);

// 2004.08.17 added begin

BOOL SelectFolder(HWND hWnd,char *ach);

BOOL SelectFilePro(LPCTSTR szFileExt,LPCTSTR szFileType,char *szFile);

BOOL CheckPlacement(LPCTSTR szSourceDir, LPCTSTR szTargetImage);

// 2004.08.17 added end

// 2004.09.03 added begin
void AddDelMethod(CComboBoxEx* pBox);
// 2004.09.03 added end



#endif