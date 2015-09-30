
typedef struct
{
	HANDLE  hFile;
	char    szFile[MAX_PATH];
	WORD    wFixEnd;
	BYTE    btFileEnd[2];
	WIN32_FILE_ATTRIBUTE_DATA FileAttrib;
}FILE_LOC_HANDLE, *PFILE_LOC_HANDLE;

typedef struct tagOPEN_LOC_FILE
{
	PFILE_LOC_HANDLE LocFileHandle;
	tagOPEN_LOC_FILE *pNextNode;
	tagOPEN_LOC_FILE *pPrevNode;
}OPEN_FILE_LIST,*POPEN_FILE_LIST;

typedef struct
{
	LONGLONG llPartitionStart;
	DWORD dwVolumeType;
	DWORD dwDriverNum;
	HANDLE hVolume;
	BOOL   bInited;
	POPEN_FILE_LIST pFileListHead;
	POPEN_FILE_LIST pFileListTail;
	DWORD  dwLockSign;
	BYTE   btOrginal;
}VOLUMEINFORMATION,PVOLUMEINFORMATION;

BOOL  RemoveFLHandle(PFILE_LOC_HANDLE pHandle);
BOOL  SaveFLHandle(PFILE_LOC_HANDLE pHandle);
int   GetAvailableVParDrive(DWORD dwVParType);
int   GetAvailableCryDrive(DWORD dwVParType);
BOOL  GetAvailableDriveLetter(char *szDrive);
BOOL  MaskAvailableVParDrive(int nDrive,DWORD dwVParType,BOOL bMask);
HRESULT BroadcastVolumeDeviceChange(WPARAM notification, WCHAR DriveLetter);
