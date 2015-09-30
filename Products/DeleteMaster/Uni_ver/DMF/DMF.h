
#if !defined(AFX_SED_H__51E1BE53_7476_47C9_BC2C_E14955ADFDA0__INCLUDED_)
#define AFX_SED_H__51E1BE53_7476_47C9_BC2C_E14955ADFDA0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

void PrintUsage();
BOOL ParseCmdLine(int argc, TCHAR* argv[]);
BOOL ParseSettingFile(WCHAR *szFile);
BOOL WriteLog(WCHAR *szFile,CTime StartTime,CTime EndTime,BOOL bSucc);
void GetSidString(PSID pSid, WCHAR *szBuffer);
BOOL GetUserSid(PSID *ppSid);
WCHAR *GetFileNameFromPath(WCHAR *szPath);

#endif // !defined(AFX_SED_H__51E1BE53_7476_47C9_BC2C_E14955ADFDA0__INCLUDED_)
