
#if !defined(AFX_SED_H__51E1BE53_7476_47C9_BC2C_E14955ADFDA0__INCLUDED_)
#define AFX_SED_H__51E1BE53_7476_47C9_BC2C_E14955ADFDA0__INCLUDED_

#include "StdAfx.h"
#include "resource.h"
#include "..\AM01Expt\AM01Expt.h"

#define BACKUP_TYPE_ORIGNIAL	0
#define BACKUP_TYPE_SNAPSHOT	1

void PrintUsage();
BOOL ParseCmdLine(int argc, TCHAR* argv[]);
BOOL BackupSetting(WCHAR *szSetting,ULONG ulBackupType);
DWORD WINAPI ThreadBackupFile(LPVOID pIn);
BOOL CheckMailDataReady(CStringArray &SourceArray);
BOOL CheckDirectoryInfo(LPCTSTR lpstrSourceDirectory);
BOOL ExecuteOneSetting(LPCTSTR szSettingName,BOOL bNewTarget, LPCTSTR szNewTargetName);
BOOL ExecuteSettingNewBase(WCHAR *szSettingName);
BOOL GetSettingInfo(LPCTSTR szSettingName, AM01_PROG_SETTING *pProgSetting);
BOOL IsFileBackupSettingFile(LPCTSTR szSettingFileName);
BOOL GetSettingFileName(CString &strProgSettingDataPath);

#endif // !defined(AFX_SED_H__51E1BE53_7476_47C9_BC2C_E14955ADFDA0__INCLUDED_)
