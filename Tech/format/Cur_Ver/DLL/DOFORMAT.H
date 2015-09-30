#include <windows.h>

#define  FORMAT_FAT_32        1      
#define  FORMAT_FAT_16        2

// wParam  mean  percent have been done
#define  WM_FORMAT_FLUSH_PROGRESS  (WM_USER+100)

/*=============================================================
 *  format information about a drive 
 *===========================================================*/
typedef struct
{
	BYTE    btDriveNum  ;   // O mean A: etc.
	DWORD   dwDriveSize ;   // size  of the drive (unit:sector)
	DWORD   dwType      ;   // See below 
	BYTE    Label[11]   ;
	HWND    hWnd        ;	
	PBYTE	pBootSec;
}FORMAT_INFO ,*PFORMAT_INFO ;

/*=============================================================
 *  format information about a drive 
 *===========================================================*/
typedef struct
{
	BYTE    btDiskNum    ;   // O mean first hard disk  : etc.
	DWORD   dwStartSector;   // start sector of the partition 
	DWORD   dwSectors    ;   // size of the partition 
	DWORD   dwType       ;   // See below 
	BYTE    Label[11]    ;
	HWND    hWnd         ;
}FORMAT_INFO_EX ,*PFORMAT_INFO_EX ;

extern "C"
{
BOOL  DoFormat(FORMAT_INFO &);

BOOL  DoFormatEx(FORMAT_INFO_EX &);
}
