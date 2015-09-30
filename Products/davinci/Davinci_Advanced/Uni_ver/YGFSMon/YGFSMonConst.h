#ifndef YG_FS_MON_CONST_H
#define YG_FS_MON_CONST_H
#include "..\EncryptToolStruct\EncryptToolStruct.h"

// a key used to encrypt the password which is recoreded in the setting file
const WCHAR YG_DAVINCI_PASSWORD_KEY[PASSWORD_SIZE]=L"YG_DAVINCI_PASSWORD_KEY_CONTENT";

const WCHAR SETTING_DATA_FILE[MAX_PATH]=L"MonitorSetting.dat";

const WCHAR YG_FS_MON_PROCESS[6][MAX_PATH] = {
											 L"YGFSMon.exe",
											 L"FileEncrypt.exe",
											 L"ShellEncrypt.exe",
											 L"FileDecrypt.exe",
											 L"ShellDecrypt.exe",
											 L"DelFile.exe"
											};

typedef struct
{
	WCHAR szHistoryDir[MAX_PATH];
	WCHAR szCacheDir[MAX_PATH];
	WCHAR szCookieDir[MAX_PATH];
	WCHAR szFavoriteDir[MAX_PATH];
	WCHAR szTempDir[MAX_PATH];
}SHELL_FOLDER_INFO;

typedef struct
{
	HWND hParentWnd;
	WCHAR szDirName[MAX_PATH];
}DELETE_THREAD_PARAMETER1;

const int DELETE_PROGRESS_TIMER_ID = 106;

#endif