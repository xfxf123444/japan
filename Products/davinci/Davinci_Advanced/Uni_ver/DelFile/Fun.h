#ifndef   DEL_FILE_FUN_H_INCLUDED    
#define   DEL_FILE_FUN_H_INCLUDED 

#include "StdAfx.h"
#include <winioctl.h>

//#include "..\..\..\..\Tech\Lib\Crypto\Cur_ver\Export\Crypto.h"

#define TIME_DAY_OF_WEEK   1
#define TIME_DAY_OF_MONTH  2

#define	IMG_NORMAL	2
#define	IMG_FILE	10

#define	CURRENT_FILE_VERSION       0x101
#define SETTING_FILE_IDENTITY_SIZE MAX_PATH
#define SETTING_DATA_FILE         L"DMSche.dat"
#define SETTING_FILE_IDENTITY     L"DeleteMaster Schedule File!"

#define DELETE_OBJECT_NORMAL	0
#define DELETE_OBJECT_RECYCLER  1

#pragma pack(1)

typedef struct
{
	ULONG ulTryTimes;
	ULONG ulLastTryStatus;
	SYSTEMTIME LastTryTime;
	char szUserName[32];
	char szUserPass[32];
}USERPASSSTRUCT,*PUSERPASSSTRUCT;

typedef struct
{
	ULONG ulFileVer;
	ULONG ulStructSize;
	WCHAR  szSettingIdentity[SETTING_FILE_IDENTITY_SIZE];
	DWORD dwSettingCount;
	WCHAR  szReserved[56];
}SETTING_FILE_HEAD,*PSETTING_FILE_HEAD;


typedef struct
{
	WCHAR szPathFile[MAX_PATH];
	ULONG ulFileattribute;
	ULONG ulDateType;
	ULONG ulDate;//1--7 or 1--31
	ULONG ExecuteTime;//XX-HH-MM-XX
	ULONG bRecordLog;
	ULONG ulDeleteMethod;
	SYSTEMTIME LastExcuteTime;
	ULONG ulDeleteObjectType;
	BYTE  btReserved[100-sizeof(SYSTEMTIME)-sizeof(ULONG)];
}SCHEDULE_ITEM,*PSCHEDULE_ITEM;

#pragma pack()

typedef CArray <SCHEDULE_ITEM,SCHEDULE_ITEM> CSCHEDULE_ITEM_ARRAY;

BOOL GetTotalFileSize();
BOOL GetDirFileSize(WCHAR  *pszDir);
void AddDelMethod(CComboBoxEx* pBox,int nCurMethod);
BOOL InitDelBuf(BYTE *pBuf,BYTE btFir,BYTE btSec,BYTE btThr,BOOL bRand,BOOL bOneByte);
BOOL WriteLog(WCHAR *szFile,CTime StartTime,CTime EndTime,BOOL bSucc);
int  GetLetterType(LPCTSTR szLetter);
BOOL SecDeleteFile();
BOOL SecDeleteFileInRecycled();
BOOL ParseCmdLine(WCHAR *pszCommand);
int CALLBACK FileListCompare(LPARAM lParam1,LPARAM lParam2,LPARAM lParamSort);

BOOL GetSettingFileName(CString &strProgSettingDataPath);
BOOL GetProgDir(CString &strProgDir);
BOOL GetLocalDir(CString &strPath);
BOOL RecordSetting(CSCHEDULE_ITEM_ARRAY &ScheduleItemArray);
BOOL ReadSetting(CSCHEDULE_ITEM_ARRAY &ScheduleItemArray);
BOOL IsFileScheduleSettingFile(LPCTSTR szSettingFileName);
BOOL RecordSettingHead(LPCTSTR szSettingFile);
BOOL CheckExcludeFolder(LPCTSTR szAddPath,BOOL &bExcludeFolder);
BOOL CheckEqualOrSubDir(LPCTSTR szFirstDir,LPCTSTR szSecondDir,BOOL &bEqualDir,BOOL &bSubDir);
BOOL UpdateScheduleSetting();
BOOL SavePassword(char *szUserName,char *szPassword,BOOL bAdmin);
BOOL CheckPassword(char *szUserName,char *szPassword);

BOOL GetUserSid(PSID *ppSid);
void GetSidString(PSID pSid, LPTSTR szBuffer);

#endif;
