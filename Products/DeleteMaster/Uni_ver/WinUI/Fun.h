#ifndef   DM_FUN_H_INCLUDED    // if have been included then skip all
#define   DM_FUN_H_INCLUDED 

#include "StdAfx.h"
//#include <WinIoCtl.h>

#include "..\..\..\..\Tech\ParInfo\Cur_Ver\export\2000\Parinfo.h"
//#include "..\..\..\..\Tech\Parman\Cur_Ver\export\2000\Parman.h"

#define MAX_FILE_SIZE				0x80000000
#define MIN_FILE_SECTORS			0x2000
#define TEMP_DATA_FILE_NAME			_T("ygtempgarbage")
#define STRING_END_CHAR             _T('\0')
#define SECTORSIZE                  512 

#define VWIN32_DIOC_DOS_INT25		2
#define VWIN32_DIOC_DOS_INT26		3
#define CARRY_FLAG					1
#define VWIN32_DIOC_DOS_DRIVEINFO	6

#define X_FAT32			0x0B
#define X_FAT32_OUT		0x0c		

#define X_HIDE_FAT32			0x1B
#define X_HIDE_FAT32_OUT		0x1c		

#define X_FAT16			0x06
#define X_FAT16_1		0x01
#define X_FAT16_4		0x04
#define X_FAT16_E		0x0e

#define X_HIDE_FAT16	0x16
#define X_HIDE_FAT16_1	0x11
#define X_HIDE_FAT16_4	0x14
#define X_HIDE_FAT16_E	0x1e

#define X_NTFS			0x07
#define X_NTFS_HIDE		0x17

#define X_LINX_SWAP		0x82
#define X_LINX_EXT		0x83	

#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)
#define IOCTL_STORAGE_GET_MEDIA_TYPES  CTL_CODE(IOCTL_STORAGE_BASE, 0x0300, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#define IOCTL_DISK_GET_MEDIA_TYPES CTL_CODE(IOCTL_DISK_BASE, 0x0300, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define VWIN32_DIOC_DOS_IOCTL	1
#define FILE_ANY_ACCESS                 0
#define METHOD_BUFFERED                 0
#define FILE_DEVICE_DISK                0x00000007
#define IOCTL_DISK_BASE                 FILE_DEVICE_DISK
#define FILE_DEVICE_MASS_STORAGE        0x0000002d
#define IOCTL_STORAGE_BASE				FILE_DEVICE_MASS_STORAGE

typedef BOOL (WINAPI *P_GDFSE)(LPCTSTR,
							   PULARGE_INTEGER,
                               PULARGE_INTEGER,
							   PULARGE_INTEGER);

struct YG_PARTITION_INFO  {
	DWORD PartitionStyle;

	// for mbr
    BYTE     btDiskNum     ; // 0 mean first hard disk	
    TCHAR     DriveLetter   ; // 0 mean HAVE_NO_DRIVE_LETTER
	BOOL     bLogic        ; // Primary or logic?  
	DWORD    dwSystemFlag  ; // File system flag ,0b mean FAT32 ,etc.
	DWORD    dwStartSector ; // From MBR
    DWORD    dwPartSize    ; // Unit :sector
    DWORD    dwUsedSize    ; //
	char	 szOsLabel[MAX_PATH];// OS label
	DWORD	 dwPhyUsedSize ; // File used + system used sectors.
	char	 szLabel[MAX_PATH] ; // Partition Label;
	DWORD    nLevel        ; // Compressed level 
	BYTE     BootFlag      ; // 80 mean bootable

	// for gpt
	GUID_PARTITION_TYPE GUIDType;

    struct YG_PARTITION_INFO *pNext;
};
typedef  YG_PARTITION_INFO  *PYG_PARTITION_INFO  ;

typedef struct tagDISKVIEWINFO{ 
	DWORD			dwMinSec;
	DWORD			dwMaxSec;
	int				nSelDisk;
	DWORD			dwSelSec;
}DiskViewInfo;

typedef struct tagDEVICEPARAMS
   {
   BYTE  bSpecFunc;        // Special functions
   BYTE  bDevType;         // Device type
   WORD  wDevAttr;         // Device attributes
   WORD  wCylinders;       // Number of cylinders
   BYTE  bMediaType;       // Media type
                        // Beginning of BIOS parameter block (BPB)
   WORD  wBytesPerSec;     // Bytes per sector
   BYTE  bSecPerClust;     // Sectors per cluster
   WORD  wResSectors;      // Number of reserved sectors
   BYTE  bFATs;            // Number of FATs
   WORD  wRootDirEnts;     // Number of root-directory entries
   WORD  wSectors;         // Total number of sectors
   BYTE  bMedia;           // Media descriptor
   WORD  wFATsecs;         // Number of sectors per FAT
   WORD  wSecPerTrack;     // Number of sectors per track
   WORD  wHeads;           // Number of heads
   DWORD dwHiddenSecs;     // Number of hidden sectors
   DWORD dwHugeSectors;    // Number of sectors if wSectors == 0
                        // End of BIOS parameter block (BPB)
   } DEVICEPARAMS, FAR * LPDEVICEPARAMS;

	   typedef struct _DIOC_REGISTERS {
    DWORD reg_EBX;
    DWORD reg_EDX;
    DWORD reg_ECX;
    DWORD reg_EAX;
    DWORD reg_EDI;
    DWORD reg_ESI;
    DWORD reg_Flags;
	} DIOC_REGISTERS, *PDIOC_REGISTERS;

#pragma pack(1)
   typedef struct _DISKIO {
      DWORD  dwStartSector;   // starting logical sector number
      WORD   wSectors;        // number of sectors
      DWORD  dwBuffer;        // address of read/write buffer
   } DISKIO, * PDISKIO;

	   typedef struct _DOSDPB {
   BYTE    specialFunc;    // 
   BYTE    devType;        // 
   WORD    devAttr;        // 
   WORD    cCyl;           // number of cylinders
   BYTE    mediaType;      // 
   WORD    cbSec;          // Bytes per sector
   BYTE    secPerClus;     // Sectors per cluster
   WORD    cSecRes;        // Reserved sectors
   BYTE    cFAT;           // FATs
   WORD    cDir;           // Root Directory Entries
   WORD    cSec;           // Total number of sectors in image
   BYTE    bMedia;         // Media descriptor
   WORD    secPerFAT;      // Sectors per FAT
   WORD    secPerTrack;    // Sectors per track
   WORD    cHead;          // Heads
   DWORD   cSecHidden;     // Hidden sectors
   DWORD   cTotalSectors;  // Total sectors, if cbSec is zero
   BYTE    reserved[6];    // 
} DOSDPB, *PDOSDPB;
 #pragma pack()

//typedef struct _DISK_GEOMETRY_EX {
//        DISK_GEOMETRY Geometry;
//        LARGE_INTEGER DiskSize;
//        UCHAR Data[1];
//} DISK_GEOMETRY_EX, *PDISK_GEOMETRY_EX;
//
#pragma pack(1) 
typedef struct { 
 BYTE bVersion;  // Binary driver version. 
 BYTE bRevision;  // Binary driver revision. 
 BYTE bReserved;  // Not used. 
 BYTE bIDEDeviceMap; // Bit map of IDE devices. 
 DWORD fCapabilities; // Bit mask of driver capabilities. 
 DWORD dwReserved[4]; // For future use. 
} YG_GETVERSIONINPARAMS; 
//
//typedef struct _IDEREGS { 
// BYTE bFeaturesReg;  // Used for specifying SMART "commands". 
// BYTE bSectorCountReg; // IDE sector count register 
// BYTE bSectorNumberReg; // IDE sector number register 
// BYTE bCylLowReg;   // IDE low order cylinder value 
// BYTE bCylHighReg;  // IDE high order cylinder value 
// BYTE bDriveHeadReg;  // IDE drive/head register 
// BYTE bCommandReg;  // Actual IDE command. 
// BYTE bReserved;   // reserved for future use.  Must be zero. 
//} IDEREGS, *PIDEREGS, *LPIDEREGS; 
//
typedef struct { 
 DWORD cBufferSize;  // Buffer size in bytes 
 IDEREGS irDriveRegs;  // Structure with drive register values. 
 BYTE bDriveNumber;  // Physical drive number to send 
        // command to (0,1,2,3). 
 BYTE bReserved[3];  // Reserved for future expansion. 
 DWORD dwReserved[4];  // For future use. 
 BYTE  bBuffer[1];   // Input buffer. 
} YG_SENDCMDINPARAMS; 
//
//typedef struct _DRIVERSTATUS { 
// BYTE bDriverError;  // Error code from driver, 
//        // or 0 if no error. 
// BYTE bIDEStatus;   // Contents of IDE Error register. 
//        // Only valid when bDriverError 
//        // is SMART_IDE_ERROR. 
// BYTE bReserved[2];  // Reserved for future expansion. 
// DWORD dwReserved[2];  // Reserved for future expansion. 
//} DRIVERSTATUS, *PDRIVERSTATUS, *LPDRIVERSTATUS; 
//
typedef struct { 
 DWORD    cBufferSize;  // Size of bBuffer in bytes 
 DRIVERSTATUS DriverStatus;  // Driver status structure. 
 BYTE   bBuffer[512];   // Buffer of arbitrary length 
          // in which to store the data read from the drive. 
} YG_SENDCMDOUTPARAMS; 
//
typedef struct _IDSECTOR { 
 USHORT wGenConfig; 
 USHORT wNumCyls; 
 USHORT wReserved; 
 USHORT wNumHeads; 
 USHORT wBytesPerTrack; 
 USHORT wBytesPerSector; 
 USHORT wSectorsPerTrack; 
 USHORT wVendorUnique[3]; 
 CHAR sSerialNumber[20]; 
 USHORT wBufferType; 
 USHORT wBufferSize; 
 USHORT wECCSize; 
 CHAR sFirmwareRev[8]; 
 CHAR sModelNumber[40]; 
 USHORT wMoreVendorUnique; 
 USHORT wDoubleWordIO; 
 USHORT wCapabilities; 
 USHORT wReserved1; 
 USHORT wPIOTiming; 
 USHORT wDMATiming; 
 USHORT wBS; 
 USHORT wNumCurrentCyls; 
 USHORT wNumCurrentHeads; 
 USHORT wNumCurrentSectorsPerTrack; 
 ULONG ulCurrentSectorCapacity; 
 USHORT wMultSectorStuff; 
 ULONG ulTotalAddressableSectors; 
 USHORT wSingleWordDMA; 
 USHORT wMultiWordDMA; 
 BYTE bReserved[128]; 
} IDSECTOR, *PIDSECTOR; 
#pragma pack()

void	DeleteTempFile(TCHAR szDriveLetter,int nFileNum);
BOOL	CreateTmpFileInSou( TCHAR szDriveLetter,int *nFileNum,BOOL *bHaveFile,__int64 *nTotalFileSize);
BOOL GetLogfontFromResources(LPTSTR resourceName, LOGFONT *logfont);
void AddFixDisk(CComboBoxEx* pBox,BOOL bAddFloppy);
void	AddDelMothed(CComboBoxEx* pBox);
void	InitList(HWND hWnd, CRect rCreate);
void	ResizeListColumn(HWND hWnd);
void	SortParInfo(PARINFOONHARDDISKEX *ParInfo);
BOOL	GetFixDiskInfo(int nDisk);
BOOL GetVolumeSpace(TCHAR chDrive,DWORD *pdwCount,DWORD *pdwUsed);
BOOL	FreePartList( PYG_PARTITION_INFO  pPartInfo);
void	AddList(CListCtrl* pList, int nSelDisk);
BOOL	IsSystemDisk();
BOOL	ShowSectorInfo(CWnd* pWnd);
BOOL	InitDelBuf(BYTE *pBuf,BYTE btFir,BYTE btSec,BYTE btThr,BOOL bRand,BOOL bOneByte);
DWORD	GetFloppyFormFactor(int iDrive);
BOOL	GetFDParam(BYTE btFDLetter,BIOS_DRIVE_PARAM *pParam);
BOOL	WriteFDSector(DWORD dwStartSec,WORD wSectors,PBYTE pBuf,BYTE btUnit,PBIOS_DRIVE_PARAM pDriveParam);
BOOL	ReadFDSector(DWORD dwStartSec,WORD wSectors,PBYTE pBuf,BYTE btUnit,PBIOS_DRIVE_PARAM pDriveParam);
BOOL	HasMedia(TCHAR szDrive);
TCHAR * strrchrpro(TCHAR* szSource,TCHAR chChar);
CString GetUserPath();

BOOL	GetRemovableDiskSize(int nDisk, LPTSTR pszDrv,DWORD *pdwSize);
VOID	ChangeByteOrder(LPTSTR szString, USHORT uscStrSize) ;
VOID	HDInfont(BYTE btDisk,HWND hHandle);
VOID	HDInfo9x(BYTE btDisk,HWND hHandle);

#endif;
