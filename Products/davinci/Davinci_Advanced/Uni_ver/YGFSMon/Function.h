#ifndef YG_FS_MON_FUNCTION_H
#define YG_FS_MON_FUNCTION_H

#include "stdafx.h"
#include "..\EncryptToolStruct\EncryptToolStruct.h"
#include "YGFSMonConst.h"
#include "..\DavinciFunction\DeleteProgressDlg.h"

BOOL SelectFolder(HWND hWnd,WCHAR *ach);

// 2004.09.14 added begin
// for setting file

BOOL GetSettingFileName(CString &strProgSettingDataPath);

BOOL GetProgDir(CString &strProgDir);

BOOL RecordSetting(CMONITOR_ITEM_ARRAY &MonitorItemArray);

BOOL ReadSetting(CMONITOR_ITEM_ARRAY &MonitorItemArray);

BOOL IsFileMonitorSettingFile(LPCTSTR szSettingFileName);

BOOL RecordSettingHead(LPCTSTR szSettingFile);

BOOL EncryptPassword(TCHAR *pchPassword,int nPasswordSize);

BOOL DecryptPassword(TCHAR *pchPassword,int nPasswordSize);

// 2004.09.14 added end

// 2004.09.21 added begin

BOOL CheckExcludeFolder(LPCTSTR szAddPath,BOOL &bExcludeFolder);

BOOL CheckEqualOrSubDir(LPCTSTR szFirstDir,LPCTSTR szSecondDir,BOOL &bEqualDir,BOOL &bSubDir);

// 2004.09.21 added end

BOOL UpgradeSettingFile();

//

BOOL GetShellFolderInfo(SHELL_FOLDER_INFO &ShellFolder);

BOOL QueryRegCurrentUser(LPCTSTR szRegPath,LPCTSTR szRegName,DWORD dwDataType,CString &strRegValue);

BOOL IsDirectory(LPCTSTR szDirectoryName);

BOOL IsFile(LPCTSTR szFileName);

UINT NormalDeleteDirThread();

BOOL GetApplicationDataPath(WCHAR *szPath);
BOOL GetLocalDir(CString &strPath);
#endif