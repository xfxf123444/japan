#ifndef AM01_STRUCT_HEAD_FILE_H
#define AM01_STRUCT_HEAD_FILE_H

#include <afxtempl.h>

// const for FILE HEAD
const int  IMAGE_IDENTITY_SIZE=32;
const int  IMAGE_COMMENT_SIZE=80;
const int  IMAGE_SIGNATURE_SIZE=512;
const int  IMAGE_RESERVED_SIZE=76;
const WCHAR IMAGE_IDENTITY[16]=L"AM01IMAGE01";
const int  IMAGE_SERIALNUMBER_SIZE=70;

// in windows Registry, OE store root is stored under the key of L"Store Root"
const WCHAR OE_REG_STORE_ROOT[MAX_PATH]=L"Store Root";

// const int  RM_DIFF_BACKUP=2;

// these value will be used in the file.
// some of them will be write to file.
// for example MAIL_BACKUP, OE_MAIL_BACKUP

const int  FILE_BACKUP=0;
const int  MAIL_BACKUP=1;

const int  BACKUP_TYPE_DIFF    = 3;
const int  EASY_BACKUP         = 5;
const int  OE_MAIL_BACKUP      = 6;
const int  WIN_MAIL_BACKUP      = 7;

const int  IMAGE_STATUS_BEGIN=0;
const int  IMAGE_STATUS_COMPLETE=1;

const int  IMAGE_FILE_VERSION = 1;

const WCHAR AM_SCHEDULE_FILE[32]=L"AMTask.dat";
const WCHAR AM_SCHEDULE_IDENTITY[IMAGE_IDENTITY_SIZE]=L"AM01_SCHEDULE_FILE_001";

// const for Backup File

const int AM_BUFFER_SIZE=65536; // Buffer is 64 k

const int AM_COMPRESS_BUFFER=AM_BUFFER_SIZE + AM_BUFFER_SIZE / 5 + 12;

// const int AM_BUFFER_SIZE= 128;

//-- for debug
// const int AM_BUFFER_SIZE=16; // Buffer is 16, then a pakage is 16 bytes.
//-- end for debug

// when check free space add extra space to check.
const int AM_EXTRA_FREE_SPACE = 1024 ;

// now let the max_file_size useless.
// in general backup we needn't make the file size limited.

const unsigned long LAST_FILE_NUMBER=0xffffffff;

const int GUID_SIZE=100;

const int SEGMENT_SIZE=1000;

// File status at each stamp

const int FILE_NOT_EXIST=0;
const int FILE_NEW=1;
const int FILE_NO_CHANGE=2;
const int FILE_CHANGE=3;
const int DIRECTORY_NEW=6;
const int DIRECTORY_EXIST=7;
const int RM_FILE_NEW=8;

// const for diff backup file
#define READ_ONCE_ITEM 1000; // for compare, read 1000 Items at a time.

// const for error code
const int NOT_ENOUGH_MEMORY=12050; // in backup file
const int FILE_PATH_TOO_LONG=12060; // in Backup file

// Error: 3 ERROR_PATH_NOT_FOUND
// in store directory info
const int DIRECTORY_NOT_EXIST = 12070; //in StoreDirectoryInfo
const int NOT_LAST_IMAGE = 3;

// const for Restore
const int RESTORE_ENTIRE=0;
const int RESTORE_PART=1;
const int RESTORE_ORIGINAL_PLACE=0;
const int RESTORE_NEW_PLACE=1;   

// const for compress
// const int AM_COMPRESS_LEVEL = 4 ;
const int AM_COMPRESS_LEVEL = 3 ;

#define EXCUTE_TYPE_COMBINATION 0
#define EXCUTE_TYPE_FULL		1
#define EXCUTE_TYPE_INCREMENTAL	2

// if a file name is created by the program
// it has at least 18 characters.

// A_20030915_1745_e001.amg
const int LONG_NAME_SIZE=24;

const WCHAR DIR_FILE_IDENTITY[16]=L"AM01_DIRFILE";

const int SHUTDOWN_NOTICE_TIME = 30; // 30 seconds

const WCHAR AM_FILE_SUFFIX[6]          = L".amg";  // the suffix of AM01 file
const WCHAR AM_INDEX_SUFFIX[6]         = L".idx";
const WCHAR AM_CURRENT_INDEX_SUFFIX[6] = L".id0";
const WCHAR AM_ARRAY_SUFFIX[6]         = L".ary";
const WCHAR AM_STAMP_SUFFIX[6]         = L".stp";

const WCHAR SETTING_INDEX_FILE[] = L"AMO1Set_uni_ver.idx";
const WCHAR SETTING_DATA_FILE[] = L"AMO1Set_uni_ver.dat";

const WCHAR MIDDLE_FILE_SIGNATURE[3]=L"_f"; // if one image is divided to several files, the file name has the letter: "f"

// it seems that since we can not change the file name in the CD-R
// we just keep the file name as original filename.
// const char END_FILE_SIGNATURE[3]=L"_e"; // if one image is the last image, the file has the letter: "e"
const WCHAR END_FILE_SIGNATURE[3]=L"_f"; // if one image is the last image, the file has the letter: "e"

const int  NAME_TAIL_SIZE=9; // L"_e001.amg" have 9 letters.


// Backup type
// this is only used in UI.
const int EASY_MY_DOCUMENT = 0;
const int EASY_MY_MAIL = 1;
const int EASY_DOCUMENT_AND_MAIL = 2;

// Mail type
// this is only used in UI.
const int OUTLOOK_EXPRESS_MAIL = 0;
const int OUTLOOK_MAIL = 1 ;
const int BECKY_MAIL = 2;

// for nBackupPeriod
const int WEEK_PERIOD = 0;
const int MONTH_PERIOD = 1;


// the following is the schedule type
// infact if no schedule the schedule will not be recorded 
// in the schedule file.
// in the other case if have schedule and all the schedule data
// is empty,then it is equal to no schedule.

const int NO_SCHDULE = 0;
const int NORMAL_SCHEDULE = 1;
const int ADVANCED_SCHEDULE = 2;

const int DIALOG_YES = 1;
const int DIALOG_NO = 2;
const int DIALOG_CANCEL =3;

// 2004.05.19 added begin

const int  SETTING_FILE_IDENTITY_SIZE = MAX_PATH;
const WCHAR SETTING_FILE_IDENTITY[SETTING_FILE_IDENTITY_SIZE]=L"USB_BACKUP_SETTING_FILE";

const WCHAR BM_FAIL_RETRY_SCHEDULE_FILE[32] = L"BMFRSche.dat";
const WCHAR BM_FAIL_RETRY_SCHEDULE_IDENTITY[SETTING_FILE_IDENTITY_SIZE]=L"BackupMater Fail Retry Schedule File";

const int PASSWORD_SIZE = 60;
const int MAX_SETTING_NAME_LEN = 40;
const int MAX_COMMENT_LEN      = 50;

// 2004.05.19 added end


// 2004.05.20 for encrypt begin

const int  YG_INITIAL_VECTOR_SIZE = 16;
const char YG_INITIAL_VECTOR[YG_INITIAL_VECTOR_SIZE]= "YG_VECTOR\x0\x0\x0\x0\x0\x0";

const int  YG_ENCRYPT_KEY_SIZE = 16;
const char YG_PASSWORD_KEY[YG_ENCRYPT_KEY_SIZE]= "YG_PASSWORD_KEY";

// 2004.05.20 for encrypt end


typedef struct 
{
   DWORD       dwFileAttributes;
   FILETIME    ftCreationTime;
   FILETIME    ftLastAccessTime;
   FILETIME    ftLastWriteTime;
   DWORD       nFileSizeHigh;
   DWORD       nFileSizeLow;
   ULONGLONG   qwCheckSum;
   DWORD       dwReservedWord1;
}AM_FILE_INFO,*PAM_FILE_INFO;


typedef struct
{
   WCHAR 			szImageIdentity[IMAGE_IDENTITY_SIZE];
   DWORD 			dwFileVersion;
   WORD			    wBackupType; // FILE_BACKUP  or MAIL_BACKUP
   DWORD  			dwCompressLevel;
   DWORD 			dwCheckSum;
   WCHAR 			szComment[IMAGE_COMMENT_SIZE];
   LARGE_INTEGER    TotalIndexPointer;
   LARGE_INTEGER    ArrayPointer;
   LARGE_INTEGER    StampPointer;
   LARGE_INTEGER    FileTablePointer;
   WCHAR 			szSerialNumber[IMAGE_SERIALNUMBER_SIZE];
   WCHAR 			szLastFileName[MAX_PATH];
   ULARGE_INTEGER   qwLastFileSize;
   DWORD  		 	dwFileNumber; // if this is LAST_FILE_NUMBER (0xffffffff) ,means the last file.
   DWORD            dwFinishSignature; // after a whole backup, write the signature
   
   // 2004.05.19 added begin
   BOOL             bEncrypt;
   WCHAR             pchPassword[PASSWORD_SIZE];
   // 2004.05.19 added end
   
   DWORD			dwFileIndex;
   BYTE			    btReserved[IMAGE_RESERVED_SIZE*sizeof(WCHAR)-PASSWORD_SIZE*sizeof(WCHAR)-sizeof(BOOL)-sizeof(DWORD)]; // for BM2 76 bytes reserved, now 12 bytes reserved bytes left
} IMAGE_HEAD, *PIMAGE_HEAD;


typedef struct
{
	HANDLE hCurrentTarget;
	HANDLE hTotalIndexFile;
	HANDLE hArrayFile;
	HANDLE hCurrentIndexFile;
	HANDLE hStampFile;
} AM_FILE_HANDLE,*PAM_FILE_HANDLE;

typedef struct
{
	DWORD dwPathCount;
	BYTE btReserved[12];
}SELECTION_NODE,*PSELECTION_NODE;

typedef struct
{
    DWORD dwSelectionPathID;
	DWORD dwFileAttribute;
    WCHAR szFilePath[MAX_PATH];
	BYTE btReserved[12];
}SELECTION_DATA,*PSELECTION_DATA;

typedef struct SELECTION_LINK_TAG
{
	SELECTION_DATA SelectionData;
	struct SELECTION_LINK_TAG *pNextSelection;
}SELECTION_LINK;


// for data compress new version
// Backup Master version 2
typedef struct
{
	DWORD dwPacketSize; // the packet size is the packet size after compression
	WORD  wCompressLevel;
	DWORD dwOrigSize;//Orig size of the data packet
	BYTE  btReserved[6]; // this can be used to record check sum, size before compress, etc.
}DATA_NODE,*PDATA_NODE;


typedef struct
{
	DWORD dwTotalPathCount;
	DWORD dwTimeStampCount;
	BYTE btReserved[12];
}TOTAL_INDEX_NODE,*PTOTAL_INDEX_NODE;

typedef struct
{
	DWORD dwPathCount;
	BYTE btReserved[12];
}CURRENT_INDEX_NODE,*PCURRENT_INDEX_NODE;

typedef struct
{
	DWORD dwPathID;
	WORD  wStatusFlag;  //0 for not exist, 1 for not changed, 2 for changed, perhaps need a status of new?
	DWORD dwFileNumber; // if the image have multi file, this identify the multi file.
    LARGE_INTEGER DataPointer;
	AM_FILE_INFO FileInfo;
	BYTE btReserved[12]; 
}INDEX_DATA,*PINDEX_DATA;


typedef struct
{
    // DWORD dwReserved;
	BYTE btReserved[12];
}ARRAY_NODE,*PARRAY_NODE;

typedef struct
{
   DWORD dwPathID;
   DWORD dwFileAttribute;
   WCHAR szFilePath[MAX_PATH];
   BYTE	btReserved[12];
}ARRAY_DATA,*PARRAY_DATA;


typedef struct
{
	BYTE btReserved[12];
}STAMP_NODE, *PSTAMP_NODE;

typedef struct
{
    SYSTEMTIME  StampTime;
    BYTE  btReserved[80]; // if you want to add a comment.
}STAMP_DATA,*PSTAMP_DATA;

typedef struct
{
   DWORD dwSettingCount;
   BYTE btReserved[12];
}SETTING_NODE,*PSETTING_NODE;

typedef struct
{
  DWORD dwFileAttribute;
  WCHAR szSourcePath[MAX_PATH];
  BYTE btReserved[12];
}SETTING_SOURCE,*PSETTING_SOURCE;

// for restore progress

typedef struct
{
	WCHAR szCurrentFile[MAX_PATH];
	WORD wPercent;
	BOOL bRestoreCancel;
}RESTORE_STATE,*PRESTORE_STATE;

typedef struct
{
	WCHAR szImageFile[MAX_PATH];
	WCHAR szLastImageFile[MAX_PATH];
	WORD wImageType;
	int nRestoreType;
	WCHAR szSelectedPath[MAX_PATH]; // if nRestoreType=0, RESTORE_ENTIRE, this will be NULL;
	int nTargetType;  
	WCHAR szTargetDir[MAX_PATH];  // if nTargetType=0, this will be NULL;
	WCHAR szTempDir[MAX_PATH];
	WCHAR szImageComment[MAX_PATH]; // just use max path currently.
	DWORD dwTimeStamp;
}AM_RESTORE_INFO,*PAM_RESTORE_INFO;

typedef struct
{
    BOOL bOverwriteAll;
	BOOL bOverwriteNone;
}OVERWRITE_OPTION;

// for File Table (Multi File)
typedef struct
{
	DWORD dwFileCount;
	BYTE btReserved[12];
}TABLE_NODE;

typedef struct
{
	DWORD dwFileNumber; // the FileNumber of each file
	WCHAR szFileName[MAX_PATH]; // the filename of each file
	ULARGE_INTEGER qwFileSize; // the size of each file
	WCHAR szSerialNumber[IMAGE_SERIALNUMBER_SIZE]; // a SerialNumber for each File
	                         // it seems that the serial number is a bit short
	                         // infact this should be IMAGE_SERIAL_SIZE
	BYTE btSignature[512]; // the last 512 bytes of the file
	DWORD dwDiskNumber; // the Disk Number, in most cases it is the same as the file number.
	WCHAR szDiskLabel[32];  // the Disk Volume Label.
	BYTE btReserved[32];
}TABLE_DATA;

typedef CArray <TABLE_DATA,TABLE_DATA&> CDATA_ARRAY;

typedef struct
{
	HANDLE hCurrentImage;
	DWORD dwCurrentSource;
}CURRENT_RESTORE_INFO;
//#define  FEIX_TEST
#ifdef FEIX_TEST
const unsigned __int64 AM_MAX_FILE_SIZE = AM_BUFFER_SIZE * 2;
#else
const unsigned __int64 AM_MAX_FILE_SIZE = 0x0FFFFFFFE - AM_BUFFER_SIZE - sizeof(DATA_NODE); // for FAT32, limit the size to 4 G.
#endif

//const unsigned __int64 AM_MAX_FILE_SIZE = 1024*1024 - AM_BUFFER_SIZE - sizeof(DATA_NODE); // for FAT32, limit the size to 4 G.

// for DVD, limit the max file size to 1 G
const DWORD AM_MAX_CD_SIZE = 1000 * 1024 * 1024 ;

// for AM01 Backup

typedef struct
{
	WCHAR szCurrentFile[MAX_PATH];
	WORD wCurWork;
	WORD wPercent;
}AM01WORKSTATE,*PAM01WORKSTATE;

typedef CArray <ARRAY_DATA,ARRAY_DATA&> CPATH_ARRAY;

typedef struct
{
	DWORD dwCurrentSegment;
	DWORD dwCurrentOffset;
	CPATH_ARRAY PathArray;
}SEGMENT_INFO;

typedef struct
{
	WCHAR  chDirSignature[32];
	ULONGLONG qwCheckSum;
	char  chReserved[32];
}DIR_FILE_HEAD;

typedef struct
{
	int nBackupContent; // 0 for mydocumet, 1 for my mail, 2 for mydocument and my mail
	int nMailType;      // 0 for Outlook Express, 1 for Outlook, 2 for The becky!
    WCHAR szImageFileName[MAX_PATH]; // the is the image file to store 
}EASY_BACKUP_INFO;


typedef struct
{
	BOOL  bSchedule;
	CTime OneTaskTime;
	BOOL  bShutdown;
}ONE_DAY_TASK;

typedef struct
{
   WCHAR        szScheduleIdentity[IMAGE_IDENTITY_SIZE];
   ULONGLONG   qwCheckSum;
   DWORD       dwSettingNumber; // The total count of setting
   DWORD       dwDataSize;      // The size of one setting data
                            // This is important for the future upgrade 
                            // If the size of one setting grow, we needn't change the program
   DWORD       dwScheduleVersion; // Schedule way perhaps will be changed, one version number is needed
   BYTE	       btReserved[IMAGE_RESERVED_SIZE*sizeof(WCHAR)];
}SCHEDULE_HEAD;


typedef struct
{
	CTime OneTaskTime;
	BOOL  bShutdown;
}PERIOD_TASK;

typedef struct
{
	ULONG ulFileVer;
	ULONG ulStructSize;
	WCHAR  szIdentity[SETTING_FILE_IDENTITY_SIZE];
	DWORD dwScheduleCount;
	char  szReserved[56];
}FAIL_RETRY_SCHEDULE_FILE_HEAD,*PFAIL_RETRY_SCHEDULE_FILE_HEAD;

typedef struct
{
	WCHAR          szSettingName[MAX_SETTING_NAME_LEN];
	WCHAR          szImage[MAX_PATH];
	WCHAR          szFailFile[MAX_PATH];
	DWORD         dwTimeStamp;
	SYSTEMTIME    FailTime;
	SYSTEMTIME    LastTryTime;
	DWORD         dwTryTimes;
	DWORD         dwRetryMinutes;
	BYTE          byReserved[32];
}FAILRETRYSCHEDULE, *PFAILRETRYSCHEDULE;

typedef CArray <FAILRETRYSCHEDULE,FAILRETRYSCHEDULE&> CFAILRETRYSCHEDULE;

typedef struct
{
	WCHAR          szSettingName[MAX_SETTING_NAME_LEN];
	BYTE          btReserved[40];
	int           nScheduleType;  // 0 no schedule, 1 normal schedule, 2 advanced schedule
	ONE_DAY_TASK  DayTask[7];
	int           nBackupPeriod;  // 0 is per week,  1 is per month 
	int           nBaseWeekDay; // this value is for week
	int           nBaseMonthDay; // this value is for month
	PERIOD_TASK   PeriodTask;
	WORD          wExecuteType;//1 is Full backup first 2 is full backup only
	BYTE          byReserved[10];
}SCHEDULE_DATA;

typedef CArray <SCHEDULE_DATA,SCHEDULE_DATA&> CSCHEDULE_ARRAY;


typedef struct
{
	HANDLE hSourceImage;
	HANDLE hOldTotalIndex;
	HANDLE hOldArrayFile;
	HANDLE hReserved; // currently no use.
}SQUEEZE_FILE_HANDLE;

typedef struct
{
  CURRENT_RESTORE_INFO  RestoreInfo;
  unsigned __int64      qwFileSize; // do not know how many packages there are, then use qwFileSize to judge the package number
  unsigned __int64      qwCurrentargetSize;
}CURRENT_SQUEEZE_INFO;

typedef struct
{
	WCHAR  szCurrentFile[MAX_PATH];
	DWORD dwPercent;
	BOOL  bSqueezeCancel;
}SQUEEZE_STATE,*PSQUEEZE_STATE;

typedef struct
{
	WCHAR szSourceImage[MAX_PATH];
	WCHAR szTargetImage[MAX_PATH];
}SQUEEZE_PARAMETER;

typedef struct
{
	BOOL bAdjustMailPath;
	WCHAR szOldOEPath[MAX_PATH];
	WCHAR szNewOEPath[MAX_PATH];
}ADJUST_MAIL_INFO;

typedef struct
{
	BOOL bEncrypt;
	WCHAR pchPassword[PASSWORD_SIZE];
	BYTE  bLogFail;
	BYTE  bRetryFail;
	DWORD dwRetryMinutes;
	BYTE  bAutoSpawn;
	BYTE  bCmdLine;
	BYTE  bReserved[8];
}BACKUPOPTION,*PBACKUPOPTION;

// 2004.05.19 for setting begin

typedef struct
{
	WCHAR szSettingName[MAX_SETTING_NAME_LEN];
	WORD wBackupType;
	WCHAR szTargetImage[MAX_PATH];
	WCHAR szComment[MAX_COMMENT_LEN];
	BACKUPOPTION BackupOption;
}SETTING_DATA_INFO; // this struct will be used in DLL

typedef struct
{
	BOOL bEnable; // this value is used in the setting data file
	SETTING_DATA_INFO DataInfo;
    DWORD dwNumofSource;     // store to file, use this
}AM01_FILE_SETTING; // setting info written to the file

struct AM01_PROG_SETTING
{
	AM01_PROG_SETTING()
	{
		Clear();
	};

	void Clear()
	{
		ZeroMemory(&DataInfo, sizeof(SETTING_DATA_INFO));
		pcaSource.RemoveAll();
	}
	SETTING_DATA_INFO DataInfo;
	CStringArray pcaSource; // while the program is running, use this
}; // this information will be used in the file

typedef struct
{
	WCHAR  szSettingIdentity[SETTING_FILE_IDENTITY_SIZE];
	DWORD dwSettingCount;
	BYTE  btReserved[60];
	DWORD dwReserved;
}SETTING_FILE_HEAD;

// 2004.05.19 for setting end

#endif