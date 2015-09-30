#include <windows.h>

#define  FORMAT_FAT_32        1      
#define  FORMAT_FAT_16        2
#define  FORMAT_FAT_12        3 

// wParam  mean  percent have been done
#define  WM_FORMAT_FLUSH_PROGRESS  (WM_USER+100)

/*=============================================================
 *  format information about a drive 
 *===========================================================*/
typedef struct
{
	BYTE    btDriveNum  ;   // O mean A: etc.
	DWORD   dwDriveSize ;   // size  of the drive (unit:sector)
	DWORD   dwType      ;   // See top 
	BYTE    Label[11]   ;
	HWND    hWnd        ;	
	BYTE	*pBootSec	;   // Maybe null ,so we must use standard boot sector
	DWORD	dwStartSec;   // == hiddern sectors 
}FORMAT_INFO ,*PFORMAT_INFO ;

/*=============================================================
 *  format information about a drive 
 *===========================================================*/
typedef struct
{
	BYTE    btDiskNum    ;   // O mean first hard disk  : etc.
	DWORD   dwStartSector;   // start sector of the partition 
	DWORD   dwSectors    ;   // size of the partition 
	DWORD   dwType       ;   // See top 
	BYTE    Label[11]    ;
	HWND    hWnd         ;
}FORMAT_INFO_EX ,*PFORMAT_INFO_EX ;

extern "C"
{
BOOL  DoFormat(FORMAT_INFO &);
BOOL  DoFormatEx(FORMAT_INFO_EX &);
BOOL  LogicAbsRead( BYTE btDrive ,DWORD  dwStartSector,WORD wReadSectors,PVOID pBuffer );
BOOL  LogicAbsWrite( BYTE btDrive ,DWORD  dwStartSector,WORD wWriteSectors,PVOID pBuffer );
}


BOOL  DoFormatFat16( BYTE btDiskOrDriveNum,  // Disk or drive number 
					 DWORD dwStartSector,    // start position (from MBR)
					 DWORD dwDriveSecs,      // Size of the drive(Unit:Sector)
					 DWORD dwSecsPerClu,     // 
					 DWORD dwSectorsPerFat16,//
					 BOOL bDisk,             // int13 or int26?
					 PBYTE  Label ,          // Label of the drive  
					 HWND hWnd ,             
					 PBYTE pBootSec,         // Boot Sectors of drive
					 BOOL  bFat16 = TRUE     // Fat16 or Fat12?  
					);

BOOL  DoFormatFat32( BYTE btDiskOrDriveNum,
					 DWORD dwStartSector,
					 DWORD dwDriveSecs,
					 DWORD dwSecsPerClu,
					 DWORD dwSectorsPerFat32,
					 BOOL bDisk,
					 PBYTE  Label,
					 HWND hWnd,
					 PBYTE pBootSec
					 );

