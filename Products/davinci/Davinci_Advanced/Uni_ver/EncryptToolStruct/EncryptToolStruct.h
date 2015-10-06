#ifndef ENCRYPT_TOOL_STRUCT_H
#define ENCRYPT_TOOL_STRUCT_H

#include <afxtempl.h>
#include <windows.h>
#include <winioctl.h>
#include "..\..\..\Davinci_tech\YGFSMon\cur_ver\export\ygfsmon.h"
#include "..\..\..\Davinci_tech\Lib\Crypto\Cur_ver\Export\Crypto.h"
#include "..\..\..\Davinci_tech\ZLib\Cur_ver\Export\ZLib.h"

#define NO_DRIVER_MODE

#define CURRENT_FILE_VERSION  0x300

const  WCHAR  SELF_EXTRACTING_FILE_EXTENSION[12]= L"exe";
const  WCHAR  FILE_ENCRYPT_EXTENSION[12]= L"chy";
const  WCHAR  SELF_EXTRACTING_TEMP_EXTENSION[12]= L".xtfemp";

const  int   IMAGE_IDENTITY_SIZE=32;
const  int   IMAGE_RESERVED_SIZE=128;

const  WCHAR  IMAGE_IDENTITY[IMAGE_IDENTITY_SIZE]= L"FILE_ENCRYPT_IMAGE_FILE";
const WCHAR SELF_EXTRACTING_IDENTITY[IMAGE_IDENTITY_SIZE] = L"SELF_EXTRACTING_FILE_XF";

const  int   PASSWORD_SIZE = 60; // it only record the character of the password

const  int   ENCRYPT_BUFFER_SIZE = 51200; // current it is in stack, do not make it too large 

const int    AM_BUFFER_SIZE=65536; // Copy Buffer is 64 k

const int    SEGMENT_SIZE = 1000;

const int  SETTING_FILE_IDENTITY_SIZE = MAX_PATH;
const WCHAR SETTING_FILE_IDENTITY[SETTING_FILE_IDENTITY_SIZE] = L"YG_FILE_MONITOR_SETTING_FILE";

#ifdef NO_DRIVER_MODE
const WCHAR TEMP_DATA_INFO_FILE_NAME[] = L"DIDI";
const WCHAR TEMP_ENCRYPT_INFO_FILE_NAME[] = L"EIEI"; 
const WCHAR TEMP_TEMP_FILE_INFO_FILE_NAME[] = L"TMTE";
struct ExcludeFileInfo 
{
	FILETIME dwLastModifyTime;
	BOOL bOpenedByOtherProcess;
	WCHAR strFilePath[MAX_PATH];
	ExcludeFileInfo()
	{
		ZeroMemory(strFilePath, MAX_PATH * sizeof(WCHAR));
		ZeroMemory(&dwLastModifyTime, sizeof(FILETIME));
		bOpenedByOtherProcess = false;
	}
};
#endif


// const for Restore
const int DECRYPT_ENTIRE=0;
const int DECRYPT_PART=1;
const int DECRYPT_ORIGINAL_PLACE=0;
const int DECRYPT_RELATIVE_PLACE=1;
const int DECRYPT_CURRENT_PLACE=2; 
const int DECRYPT_NEW_PLACE=3; 

#define LEVEL_NO_COMPRESS    0
#define LEVEL_FAST_COMPRESS	 1
#define LEVEL_HIGH_COMPRESS	 2

typedef struct
{
	BYTE bOptionOn;
	BYTE bLimitCount;
	ULONG ulCurrentCount;
	ULONG ulMaxCount;
	BYTE  bLimitTime;
	CTime BaseTime;
	CTime LimitTime;
	BYTE  bErrorLimit;
	ULONG ulErrorLimit;
	ULONG uCurrentErr;
}ENCRYPTOPTION,*PENCRYPTOPTION;

// note: the element of bSingleSource
// if the image contains only one source, we remember the source
// this will be used when the user decrypts the file by context menu
typedef struct
{
    WCHAR  szImageIdentity[IMAGE_IDENTITY_SIZE];
	WCHAR  szPassword[PASSWORD_SIZE];
	LARGE_INTEGER FileInfoPointer;
	BYTE  btReserved[IMAGE_RESERVED_SIZE];
}FILE_ENCRYPT_HEAD_100;

typedef struct
{
	DWORD dwFileVer;
	DWORD dwStructSize;
    WCHAR  szImageIdentity[IMAGE_IDENTITY_SIZE];
	WCHAR  szPassword[PASSWORD_SIZE];
	LARGE_INTEGER FileInfoPointer;
	DWORD dwCompressLevel;
	ENCRYPTOPTION OptionInfo;
	BYTE  btReserved[IMAGE_RESERVED_SIZE-sizeof(DWORD)*4-sizeof(ENCRYPTOPTION)];
	ULONG ulCheckSum;
}FILE_ENCRYPT_HEAD,*PFILE_ENCRYPT_HEAD;

typedef struct
{
	DWORD dwSourceCount;
	BYTE  btReserved[12];
}SELECTION_ARRAY_NODE;

// for decrypt parameter
typedef struct
{
	WCHAR szImageFile[MAX_PATH];
	WCHAR szPassword[PASSWORD_SIZE];
	int  nRestoreType;
	WCHAR szSelectedPath[MAX_PATH];
	int  nTargetType;
	ULONG ulImageVersion;
	WCHAR szTargetPath[MAX_PATH];
	ULONG ulCompressLevel;
	ENCRYPTOPTION EncryptOption;
}DECRYPT_INFO;

// file handle for encrypt
typedef struct
{
	HANDLE hImageFile;
	HANDLE hTailFile;
}ENCRYPT_FILE_HANDLE;

// file handle for encrypt
typedef struct
{
	HANDLE hImageFile;
	HANDLE hTailFile;
}DECRYPT_FILE_HANDLE;

// the struct for image tail

typedef struct
{
	DWORD dwPathCount; // the total path count
	DWORD dwFileCount; // the file count, if it is 1, this is a special kind of image
	BYTE  btReserved[8];
}ARRAY_NODE;

typedef struct
{
   DWORD            dwPathID;
   WCHAR			szFileName[MAX_PATH];
   DWORD            dwFileAttribute;
   FILETIME         ftCreationTime;
   FILETIME         ftLastAccessTime;
   FILETIME         ftLastWriteTime;
   ULARGE_INTEGER   qwFileSize;
   LARGE_INTEGER    qwFileOffset; // file pointer
   LARGE_INTEGER    qwReserved;
}ARRAY_DATA;

typedef struct
{
	DWORD dwPacketSize; // the packet size is the packet size after compression
	DWORD dwCompressLevel;//compress level
	DWORD dwOrigSize;//Orig size of the data packet
	BYTE  btReserved[4]; // this can be used to record check sum, size before compress, etc.
}DATA_NODE;

typedef CArray <ARRAY_DATA,ARRAY_DATA&> CPATH_ARRAY;

typedef struct
{
	DWORD dwCurrentSegment;
	DWORD dwCurrentOffset;
	CPATH_ARRAY PathArray;
}SEGMENT_INFO;

typedef struct
{
	WCHAR szCurrentFile[MAX_PATH];  
	unsigned __int64 qwCurrentWriteSize; // for encrypt
	unsigned __int64 qwTotalFileSize; // for encrypt
	DWORD  dwTotalFileCount; // for decrypt
	DWORD  dwCurrentFileCount;  // for decrypt
	int  nPercent;
}DA_WORK_STATE;

// 2004.09.03 added begin
typedef struct
{
	HWND hParentWnd;
	WCHAR szDirName[MAX_PATH];
	int  nDelMethod;
}DELETE_THREAD_PARAMETER;
// 2004.09.03 added end

typedef struct
{
	WCHAR szMonitorPath[MAX_PATH-sizeof(ULONG)*2-sizeof(BOOLEAN)*2];
	ULONG ulCompressLevel;
	WCHAR szPassword[PASSWORD_SIZE];
	BOOLEAN			bCheckPasswordAlways;
	BOOLEAN			bErrorLimit;
	ULONG			ulMaxErrLimit;
}MONITOR_ITEM;

typedef CArray <MONITOR_ITEM,MONITOR_ITEM> CMONITOR_ITEM_ARRAY;
typedef CArray <CONVERT_BUF,CONVERT_BUF> CCONVERT_BUF_ARRAY;

typedef struct
{
	WCHAR  szSettingIdentity[SETTING_FILE_IDENTITY_SIZE];
	DWORD dwSettingCount;
	BYTE  szReserved[64];
}SETTING_FILE_HEAD_100;

typedef struct
{
	ULONG ulFileVer;
	ULONG ulStructSize;
	WCHAR  szSettingIdentity[SETTING_FILE_IDENTITY_SIZE];
	DWORD dwSettingCount;
	BYTE  szReserved[56];
}SETTING_FILE_HEAD;

#endif