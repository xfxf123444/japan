// BLKMOVER.H
// Copyright (C) 2001 by YG Corp.
// All rights reserved
// Author:  ZhuZhengNan
// Date:    10-Oct-2001

#ifndef BLKMOVER_EXPORT_H
#define BLKMOVER_EXPORT_H

#define YGBLK_READ_WRITE		1
#define YGSHARK_READ_WRITE		2
#define YGSUNFISH_READ_WRITE	3

// This structure provides a linkage betwen the current location
// and the original location of the sector. This constitutes a doubly linked-list.
typedef struct	tagSecMap
{
    DWORD   dwCurAddr  : 31;
    DWORD   bDataSec   :  1;
    DWORD   dwOrigAddr;
} SECMAP,*PSECMAP;

// This structure maps a target sector to its source.
// If the bFlag == 1, the sector has been moved to its final location
typedef struct	tagT2SMAP
{
    DWORD   dwSrcAddr  : 31;
    DWORD   bFlag      :  1;
} T2SMAP,*PT2SMAP;

typedef struct	tagSecGroup
{
    DWORD   dwStartSec;
    DWORD   dwSectors;
} SEC_GROUP,*PSEC_GROUP;

typedef struct tagRMSecGroup
{
	DWORD	dwSStartSec;
	DWORD	dwLength;
	DWORD	dwTStartSec;
}RM_SEC_GROUP,*PRM_SEC_GROUP;

#pragma	pack(1)
typedef struct tagPMSecGroup
{
	DWORD	dwSStartSec;
	DWORD	dwSize : 31;
	DWORD	bFront : 1;
	DWORD	dwTStartSec;
	DWORD	dwOriginal;
}PM_SEC_GROUP,*PPM_SEC_GROUP;

typedef struct tagPMRelocateGroup
{
	DWORD	dwOriginal;
	DWORD	dwSize;
	DWORD	dwFinal;
	//DWORD	dwReserved;
}PM_RELOCATE_GROUP,*PPM_RELOCATE_GROUP;
#pragma	pack()

typedef struct tagPMProtectGroup
{
	DWORD						dwSStart;
	DWORD						dwSize;
	DWORD						dwFStart;
	struct tagPMProtectGroup	*pNext;
	struct tagPMProtectGroup	*pPrev;
}PM_PROTECT_GROUP,*PPM_PROTECT_GROUP;

typedef struct tagPMSecGroupBuf
{
	DWORD				dwStartPage;
	DWORD				dwGroupNum;
	PPM_SEC_GROUP		pGroupsBuff;
}PM_SEC_GROUP_BUF,*PPM_SEC_GROUP_BUF;

typedef struct tagPMFreeSpaceGroup
{
	DWORD				dwStart;
	DWORD				dwSize;
	struct tagPMFreeSpaceGroup   *pNext;
}PM_FREE_SPACE_GROUP,*PPM_FREE_SPACE_GROUP;

typedef struct RM_SEC_GROUP_TABLE  
{
	struct RM_SEC_GROUP_TABLE   *pNext;
 	DWORD						dwStartSec;
	DWORD						dwRecNum;
	DWORD						dwDataRegionStart;
	DWORD						dwDataRegionEnd;
	PRM_SEC_GROUP				pGroupsBuff;
} RM_SEC_GROUP_TABLE , *PRM_SEC_GROUP_TABLE;

typedef struct RM_LOG_INFO  
{
 	DWORD						dwStartSec;
	DWORD						dwLength;
	BOOL						bRead;
	DWORD						dwTStart;
} RM_LOG_INFO , *PRM_LOG_INFO;

#define BLKMOV_SIGN_STRING  "FILE_INIT_SECTOR"	//  16 bytes
#define BLKMOV_SIGN_LEN		16

#define		SECTORS_PER_BYTE			0X2000L
#define		SEC_GROUP_BUFFER_SIZE		0xC0L
#define		MAX_SEC_GROUP_BUFFER_SIZE	0x3C0L
#define		MAX_DATA_REGION_LOADED		0x05L
#define		SECTORS_PER_PAGE			0X08L

//#define		MOVE_BUFFER_SIZE			0x80L
#define		ARRY_SIZE					0x400L
#define		DATA_PAGE_SIZE				0X20L
#define		BUFFER_OF_PAGES				0X10L

#define     PROTECT_GROUP_BUFFER_PAGE	64
typedef struct tagReadWriteBuffer
{
	DWORD	dwStartSec;
	DWORD	dwLength;
	BOOL	bSys;
	DWORD	dwMovedRecNum;
	PVOID	pBuffer;
} READ_WRITE_BUFFER,*PREAD_WRITE_BUFFER;

typedef struct	tagBlkMoverPara
{
    BYTE       btDrive;
    BYTE       btSharkDrive;
    DWORD	   dwDataStartSec;
    DWORD	   dwDataRecNum;
    DWORD	   dwDataRecPages;
    DWORD	   dwSysStartSec;
    DWORD	   dwSysRecNum;
    DWORD	   dwSysRecPages;
    DWORD      dwOrig1st;
    DWORD      dwOrigLast;
    DWORD	   dwFreeStart;
    DWORD	   dwFreeSize;
	DWORD	   dwAreaStart;
	DWORD	   dwAreaSize;
#ifndef WIN_9x
	DWORD	   dwTotalPhyMem;
#endif

} BLK_MOVER_PARAM,*PBLK_MOVER_PARAM;

#pragma	pack(1)

typedef struct	tagMovingGroup
{
	BOOL		bFront;
	DWORD		dwSStart;
	DWORD		dwTStart;
	DWORD		dwSize;
	DWORD		dwMovedSize;
} MOVING_GROUP,*PMOVING_GROUP;

typedef struct  tagMoveHeadSec
{
	BYTE    signature[24];
	DWORD   dwDataStart;
	DWORD	dwDataRecNum;
	DWORD	dwCurDataRecNum;//36 Bytes
	BYTE	bSouLogical;//Info of Source partition
	BYTE	btSouSysFlag;
	DWORD	dwSouStartSec;
	DWORD	dwSouSize;
	BYTE	bTarLogical;//Info of target partition
	BYTE	btTarSysFlag;
	DWORD	dwTarStartSec;
	DWORD	dwTarSize;
	BYTE	bActive;
	DWORD	dwTotolMovSize;
	DWORD	dwMovedSize;
	DWORD	dwCurBlkMovedSize;
	DWORD	dwSysSize;

	DWORD   dwSysDataStart;
	DWORD	dwSysRecNum;
	DWORD	dwDataPages;
	DWORD	dwSysPages;

	BYTE	BootFlag;   
	BYTE	SystemFlag;
	int		nPartitionType;
	BYTE	bBiosMove;
	BYTE    btReserve[416];
} MOVEHEADSEC,FAR *LPMOVEHEADSEC;
#pragma	pack()

#ifdef WIN_9X

#define BLKMOVER_RM_Init				7
#define BLKMOVER_RM_SetGroupTable		8
#define BLKMOVER_RM_EanbleOrDisRelocate	9
#define BLKMOVER_RM_WriteSectors		10
#define BLKMOVER_RM_MoveGroup			11
#define BLKMOVER_PM_ReadSectors			12
#define BLKMOVER_PM_WriteSectors		13
#define BLKMOVER_MovingCurrentGroup		14
#define BLKMOVER_EnableWritePotect		15

#else

#define IOCTL_YG_BLOCK_MOVER \
	CTL_CODE(FILE_DEVICE_DISK, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_YG_BLOCK_MOVER_SET_PARAM \
	CTL_CODE(FILE_DEVICE_DISK, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_YG_BLOCK_MOVER_ENABLE_OR_DISABLE \
	CTL_CODE(FILE_DEVICE_DISK, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_YG_BLOCK_MOVER_PERFACE \
	CTL_CODE(FILE_DEVICE_DISK, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_YG_BLOCK_MOVER_POST_SCRIPT \
	CTL_CODE(FILE_DEVICE_DISK, 0x804, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_YG_BLOCK_MOVER_SET_DATA \
	CTL_CODE(FILE_DEVICE_DISK, 0x805, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_YG_BLOCK_MOVER_EXIST \
	CTL_CODE(FILE_DEVICE_DISK, 0x806, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_YG_BLOCK_LOW_SET_DRIVE_LAYOUT \
	CTL_CODE(FILE_DEVICE_DISK, 0x807, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_SET_RM_SEC_GROUP_TABLE \
	CTL_CODE(FILE_DEVICE_DISK, 0x808, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_RM_ENABLE_DISABLE_RELOCATE \
	CTL_CODE(FILE_DEVICE_DISK, 0x809, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_YG_WRITE \
	CTL_CODE(FILE_DEVICE_DISK, 0x80A, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_RM_BLKMOV_INIT \
	CTL_CODE(FILE_DEVICE_DISK, 0x80B, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_YG_MOVE_GROUP \
	CTL_CODE(FILE_DEVICE_DISK, 0x80C, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_YG_GET_MOVED_SECTORS \
	CTL_CODE(FILE_DEVICE_DISK, 0x80D, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_YG_MOVE_SYS_BLOCK \
	CTL_CODE(FILE_DEVICE_DISK, 0x80E, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_ENABLE_OR_DISABLE_FILTE_INIT_SECTOR \
	CTL_CODE(FILE_DEVICE_DISK, 0x80F, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_YG_READ \
	CTL_CODE(FILE_DEVICE_DISK, 0x810, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_YG_BLOCK_MOVER_FORCE_REBOOT \
	CTL_CODE(FILE_DEVICE_DISK, 0x811, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_YG_BLOCK_MOVER_FLUSH_BUFFER \
	CTL_CODE(FILE_DEVICE_DISK, 0x812, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_YG_BLOCK_MOVER_ENABLE_WRITE_PROTECT \
	CTL_CODE(FILE_DEVICE_DISK, 0x813, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_YG_BLOCK_MOVING_GROUP_CURRENT \
	CTL_CODE(FILE_DEVICE_DISK, 0x814, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_YG_BLOCK_SET_FREE_SPACE_GROUP \
	CTL_CODE(FILE_DEVICE_DISK, 0x815, METHOD_BUFFERED, FILE_ANY_ACCESS)
#endif //ndefine WIN_9X

#endif	//  BLKMOVER_EXPORT_H
