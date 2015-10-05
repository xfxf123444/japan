#pragma once

#include "stdafx.h"
#include <vector>
#include <string>
#include <map>

class CBaseLib
{
public:
	// ��ȡ�ļ��л��ļ��Ĵ�С
	static bool GetFilePath( const TCHAR* path, 
		std::vector<CString>& pathSet, 
		TCHAR* pFileSuffix = 0,
		bool bIterate = true, 
		ULARGE_INTEGER* pSize = 0,
		bool bIncludingSubDir = false);

	// ��ѯWindows��Ϣ
	static bool IsWindows7Like();
	static bool IsWindowsXPLike();

	// ��ѯ�˻���Ϣ
	static bool IsUserAdmin();
	static bool GetUserSid(PSID *ppSid);
	static void GetSidString(PSID pSid, LPTSTR szBuffer);
	static bool SearchTokenGroupsForSID ();

	// ��ȡ�����ļ���·��
	static const TCHAR* GetRootPath();
	static const TCHAR* GetWindowsPath();
	static const TCHAR* GetProgramPath();
	static const TCHAR* GetProgramX86Path();
	static const TCHAR* GetAppDataPath();
	static const TCHAR* GetLocalAppDataPath();
	static const TCHAR* GetDesktopPath();
	static const TCHAR* GetPersonalPath();
	static const TCHAR* GetDocumentPath();
	static const TCHAR* GetModulePath();
	static const TCHAR* GetTempPath();
	static const TCHAR* GetIECachePath();
	static const TCHAR* GetCookieCachePath();
	static const TCHAR* GetIEPath();
	static bool IsNormalPath(const TCHAR* path);
	static bool CheckEqualOrSubDir(LPCTSTR szFirstDir,LPCTSTR szSecondDir,bool &bEqualDir, bool &bSubDir);
	static bool CheckOverlapDir(LPCTSTR szFirstDir,LPCTSTR szSecondDir);

	// ��ѯMAC
	static bool GetMacAddress(TCHAR* macBuf, DWORD size);

	// ��ѯ�豸��Ϣ
	static CString GetDeviceName();

	// �ļ����ļ���ѡ��
	static bool SelectFolder( HWND hWnd,TCHAR *ach );
	static bool SelectFiles(std::vector<CString>& pathSet);

	// Ӳ�����
	static void GetDriverLetters(std::vector<TCHAR>& driverLetterSet);
	static bool IsUsbDisk(CString& driverLetter);

	// ʱ�����
	static COleDateTime LocalTimeToUTCTime(COleDateTime& localTime);
	static COleDateTime UTCTimeToLocalTime(COleDateTime& utcTime);

	// ���ֲ���
	static std::string CStringToString(const CString& value);
	static CString StringToCString(const std::string& value);
	static std::string UnicodeStrToUTF8Str(const CString& value);
	static std::string UnicodeStrToGBKStr(const CString& value);

private:
	static bool __GetFilePath(const TCHAR* path, 
		std::vector<CString>& pathSet, 
		TCHAR* pFileSuffix,
		bool bIterate, 
		ULARGE_INTEGER* pSize,
		bool bIncludingSubDir);
	static void CutString(const CString& value, std::vector<std::pair<bool, CString> >& ss);
};
