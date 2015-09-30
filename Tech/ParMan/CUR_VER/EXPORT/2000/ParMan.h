/*=========================================================================
ParMan.h
WQ
=========================================================================*/
#ifndef PARMAN_H
#define PARMAN_H

#include "..\..\..\..\ParInfo\Cur_Ver\export\2000\ParInfo.h"

#ifndef DLL_EXPORT
    #define DLL_EXPORT  __declspec(dllexport)
#endif

#define ERR_PARMAN_OPENVXD						101
#define ERR_PARMAN_GETDRIVEPARAM				102
#define ERR_PARMAN_READSECTOR					103
#define ERR_PARMAN_WRITESECTOR					104
#define ERR_PARMAN_NUMBEROFSECTORS				105
#define ERR_PARMAN_GETPARTITIONINFO				106
#define ERR_PARMAN_EXTENDPARTITIONSTARTSECTOR	107
#define ERR_PARMAN_MBRPARTITIONFULL				108
#define ERR_PARMAN_PARTITIONINPARTITION			109
#define ERR_PARMAN_FORMATDRIVE					110
#define ERR_PARMAN_PARAM						111
#define ERR_PARMAN_RESIZEEXTENDPARTITION		112
#define ERR_PARMAN_NOFREESPACE					113
#define ERR_PARMAN_CREATEPARTITION				114
//FORMAT 
#define	FORMAT_FAT_32			1      
#define	FORMAT_FAT_16			2
//#define	FORMAT_FAT_12			3
#define	FORMAT_NTFS				4

//wParam  mean  percent have been done
#define  WM_FORMAT_FLUSH_PROGRESS  (WM_USER+100)
//format information about a drive 
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

typedef struct
{
	BOOL		bProtectReadOne;
	BOOL		bProtectReadTwo;
	BOOL		bProtectWriteOne;
	BOOL		bProtectWriteTwo;
	DWORD		dwProtectReadOne;
	DWORD		dwProtectReadTwo;
	DWORD		dwProtectWriteOne;
	DWORD		dwProtectWriteTwo;
}PROTECT_SECTOR,*PPROTECT_SECTOR;

extern "C"
{

BOOL DLL_EXPORT	DeletePartition(DWORD					dwStartSector,
								BYTE					btHardDisk,
								CREATE_PAR_FLAG			flags,
								PINT					pnError);

BOOL DLL_EXPORT CreatePartition(PPARTITION_ENTRY		ppeParEntry,
								BYTE					btHardDisk,
								DWORD					dwFlag,
								BOOL					blIsFormat,
								PBYTE					pLabel,
								HWND					hWnd,
								PINT					pnError);

BOOL DLL_EXPORT ResizeExtendPartition(BYTE						btHardDisk,
									  PPARTITION_ENTRY			ppeParEntry,
										PINT					pnError);

BOOL DLL_EXPORT Log2Pri(DWORD		dwStartSec,
						BYTE		btHardDisk,
						HWND		hWnd);

BOOL DLL_EXPORT DoFormat(FORMAT_INFO & FormatInfo);

BOOL DLL_EXPORT	FormatDrive(BYTE	DriveLetter, 
							LPSTR	szLabel, 
							DWORD	dwFileSystem,
							HWND	hWnd);

BOOL DLL_EXPORT	ChangeDriveLayout(BYTE					btHardDisk,
								  DWORD					dwOldStart,
								  CREATE_PAR_FLAG		flags,//indicate is logical or primary
								  DWORD					dwFlag,//create flags
								  PPARTITION_ENTRY		ppeParEntry,
								  int					*pnErrorNum);

BOOL DLL_EXPORT	GetProtectSectors(BYTE					btHardDisk,
								  DWORD					dwStartSector,
								  CREATE_PAR_FLAG		flags,//indicate is logical or primary
								  DWORD					dwFlag,//create flags
								  PPARTITION_ENTRY		ppeParEntry,
								  PPROTECT_SECTOR		pProtect,//[out]
								  BOOL					bWriteToDisk,									
								  int					*pnError);

}//extern "C"

#endif //PARMAN_H