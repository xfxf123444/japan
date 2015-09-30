// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__9AC5B9E8_F084_4A2D_A1FC_99CAF16313A1__INCLUDED_)
#define AFX_STDAFX_H__9AC5B9E8_F084_4A2D_A1FC_99CAF16313A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)
#define IOCTL_DISK_GET_MEDIA_TYPES CTL_CODE(IOCTL_DISK_BASE, 0x0300, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define	RW_SIZE							81920
#define RW_TIMES						18
#define FILE_ANY_ACCESS                 0
#define METHOD_BUFFERED                 0
#define FILE_DEVICE_DISK                0x00000007
#define IOCTL_DISK_BASE                 FILE_DEVICE_DISK

typedef enum _MEDIA_TYPE {
    Unknown,                // Format is unknown
    F5_1Pt2_512,            // 5.25", 1.2MB,  512 bytes/sector
    F3_1Pt44_512,           // 3.5",  1.44MB, 512 bytes/sector
    F3_2Pt88_512,           // 3.5",  2.88MB, 512 bytes/sector
    F3_20Pt8_512,           // 3.5",  20.8MB, 512 bytes/sector
    F3_720_512,             // 3.5",  720KB,  512 bytes/sector
    F5_360_512,             // 5.25", 360KB,  512 bytes/sector
    F5_320_512,             // 5.25", 320KB,  512 bytes/sector
    F5_320_1024,            // 5.25", 320KB,  1024 bytes/sector
    F5_180_512,             // 5.25", 180KB,  512 bytes/sector
    F5_160_512,             // 5.25", 160KB,  512 bytes/sector
    RemovableMedia,         // Removable media other than floppy
    FixedMedia,             // Fixed hard disk media
    F3_120M_512,            // 3.5", 120M Floppy
    F3_640_512,             // 3.5" ,  640KB,  512 bytes/sector
    F5_640_512,             // 5.25",  640KB,  512 bytes/sector
    F5_720_512,             // 5.25",  720KB,  512 bytes/sector
    F3_1Pt2_512,            // 3.5" ,  1.2Mb,  512 bytes/sector
    F3_1Pt23_1024,          // 3.5" ,  1.23Mb, 1024 bytes/sector
    F5_1Pt23_1024,          // 5.25",  1.23MB, 1024 bytes/sector
    F3_128Mb_512,           // 3.5" MO 128Mb   512 bytes/sector
    F3_230Mb_512,           // 3.5" MO 230Mb   512 bytes/sector
    F8_256_128,             // 8",     256KB,  128 bytes/sector
    F3_200Mb_512,           // 3.5",   200M Floppy (HiFD)
    F3_240M_512,            // 3.5",   240Mb Floppy (HiFD)
    F3_32M_512              // 3.5",   32Mb Floppy
} MEDIA_TYPE, *PMEDIA_TYPE;


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

typedef struct _DISK_GEOMETRY { 
  LARGE_INTEGER  Cylinders; 
  MEDIA_TYPE  MediaType; 
  DWORD  TracksPerCylinder; 
  DWORD  SectorsPerTrack; 
  DWORD  BytesPerSector; 
} DISK_GEOMETRY ; 

   #define VWIN32_DIOC_DOS_IOCTL 1
#ifdef WIN_9X
#include "..\..\..\..\Tech\ParInfo\Cur_Ver\export\9x\Parinfo.h"
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__9AC5B9E8_F084_4A2D_A1FC_99CAF16313A1__INCLUDED_)
