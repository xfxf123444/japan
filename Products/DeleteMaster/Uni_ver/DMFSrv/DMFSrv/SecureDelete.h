// SecureDelete.h: interface for the CSecureDelete class.
//
//////////////////////////////////////////////////////////////////////

#define TIME_DAY_OF_WEEK   1
#define TIME_DAY_OF_MONTH  2

#define	IMG_NORMAL	2
#define	IMG_FILE	10

#define	CURRENT_FILE_VERSION       0x102
#define SETTING_FILE_IDENTITY_SIZE MAX_PATH
#define SETTING_DATA_FILE         L"DMSche.dat"
#define SETTING_FILE_IDENTITY     L"DeleteMaster Schedule File!"
#define IDS_LOG_FORMAT			  L"Delete %s by Delete File Services, User:%s,Start time:%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d,End time:%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d,Result:%s"

#define DELETE_OBJECT_NORMAL	0
#define DELETE_OBJECT_RECYCLER  1

#pragma pack(1)

#include <atlstr.h>

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
	ULONG ulItemNum;
	BYTE  btReserved[100];
}SCHEDULE_ITEM,*PSCHEDULE_ITEM;

#pragma pack()

#define RENAME_STRING	L"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$"
#define DM_DEL_SIZE		64*1024
#define BYTEINSEC		512

BOOL DoDeleteMasterDel(LPCWSTR szDirName,int nDelMethod);
BOOL InitDelBuf(BYTE *pBuf,BYTE btFir,BYTE btSec,BYTE btThr,BOOL bRand,BOOL bOneByte);
BOOL DelDirAndFiles(LPCWSTR  pszDir ,BYTE *pBuf, BOOL bDel);
BOOL DoDelFile(LPCWSTR  pszDir,BYTE *pBuf,BOOL bDel);
BOOL DoDeleteMasterDelFileInRecycled(int nDelMethod);
BOOL DelAllInRecycled(BYTE *pBuf,BOOL bDel);
BOOL DeleteFileInRecycled(LPCWSTR pszDir, BYTE *pBuf,BOOL bDel);
BOOL DoDeleteFileInRecycled(LPCWSTR pszDir,BYTE *pBuf, BOOL bDel);
BOOL GetLocalDir(CString &strPath);
ULONG ReadSetting(PSCHEDULE_ITEM *pScheduleItemArray);
BOOL GetSettingFileName(CString& strProgSettingDataPath);
BOOL IsFileScheduleSettingFile(CString& szSettingFileName);
BOOL ExcuteSchedule(SYSTEMTIME *pSystemTime);
BOOL WriteLog(LPCWSTR szFile,SYSTEMTIME *pStartTime,SYSTEMTIME *pEndTime,BOOL bSucc);
WCHAR *GetFileNameFromPath(WCHAR *szPath);
BOOL ExcuteLostSchedule(SYSTEMTIME *pSystemTime);

