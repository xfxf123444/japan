// VIRPARTI.H -- Virtual Partition header file
// Copyright (C) 1999 by YG Corp.
// All rights reserved
// Author:  ZL
// Date:    24-Nov-1999

#ifndef VIRPARTI_H
#define VIRPARTI_H

#include "..\..\..\..\..\VPORTDRV\Cur_Ver\Export\9x\vportdrv.h"
#include "..\..\..\..\..\parinfo\Cur_Ver\Export\9x\parinfo.h"


#define ALPHA		    0
#define VIRPARTI_Major	    ALPHA
#define VIRPARTI_Minor	    1

#define PARTBL2BOOTSEC_OFFSET			0x3F
#define MAX_CAPACITY_SUPPORTED_BY_OLD_INT13	0xFE * 0x3FF * 0x3F
						// Head * Cylinder * Sector
#define PARTITABLEOFFSET	0x1be

#define HARD_DISK_LIMIT_8G	0xFC0000

#define RESIZE_PRIMARY_PARTITION    1
#define RESIZE_NORMAL_1ST_LOGIC_PARTITION_NO_NULL_PARTITION 2
#define RESIZE_NORMAL_1ST_LOGIC_PARTITION_CREATE_NULL_PARTITION 3
#define RESIZE_SPECIAL_1ST_LOGIC_PARTITION_REMOVE_NULL_PARTITION    4
#define RESIZE_SPECIAL_1ST_LOGIC_PARTITION_RESERVE_NULL_PARTITION   5
#define RESIZE_NON_1ST_LOGIC_PARTITION	6

#define CREATE_VDISK_WAIT_TIME	60000000    // 1 min


#define PRIMARY_PAR	1
#define LOGICAL_PAR	3


#define MIRROR_DRIVE	1
#define MOVER_DRIVE	2


#define PARTITABLEOFFSET	0x1be

/*
typedef struct
{
    WORD    wInfoSize;
    WORD    wFlags;
    DWORD   dwCylinders;
    DWORD   dwHeads;
    DWORD   dwSecPerTrack;
    __int64 dwSectors;
    WORD    SectorsSize;
} BIOS_DRIVE_PARAM, *PBIOS_DRIVE_PARAM;

*/



typedef struct	tagResizeInfo
    {
	DWORD	Type	:   4;	//  Primary , Logical
	DWORD	Index	:   4;
	DWORD	SysID	:   8;	//  FAT16 , FAT32 , NTFS , ext2 etc.
	DWORD	ExtParBase;	//  Extended partition's start sector number
	DWORD	PrevParStart;
	DWORD	PrevParEnd;	//  not used
	DWORD	StartOfVP;	//  the starting sector number of partition virtualized
	DWORD	EndOfVP;	//  the ending sector number of partition virtualized
	DWORD	NextParStart;
	DWORD	NextParEnd;
	DWORD	MirrorType    :   4;  //  Primary , Logical
	DWORD	MirrorSysID   :   8;  //  FAT16 , FAT32 , NTFS , ext2 etc.
	DWORD	StartOfMP;	//  the starting sector number of mirror partition
	DWORD	EndOfMP;	//  the ending sector number of mirror partition
	DWORD	SharkType     :   4;  //  Primary , Logical
	DWORD	SharkSysID    :   8;  //  FAT16 , FAT32 , NTFS , ext2 etc.
	DWORD	StartOfSP;	//  the starting sector number of mirror partition
	DWORD	EndOfSP;	//  the ending sector number of mirror partition
    } RESIZE_INFO,*PRESIZE_INFO;



typedef struct	tagCallBackProcess
    {
	ULONG	ulPhysicalDCB;
	ULONG	ulOriginalDCB;
	ULONG	ulCallDownPtr;
	ULONG	ulStartSector;
    } CallBackProcess,*PCallBackProcess;


#define DCB_DATA_SIGN	0x58585858

typedef struct	tagDCB_DATA
    {
	PDCB		dcb;
	DWORD		dwSignature;	//  XXXX
	DWORD		dwExpanLen;
	pDCB_cd_entry	pChain;
	DWORD		dwDemandFlags;
    } DCB_DATA,*PDCB_DATA;

typedef struct
{
    BYTE   Jump[3];
    char   OEMName[8];
    WORD   BytesPerSector ;
    BYTE   SectorsPerCluster ;
    WORD   ReserveSectors    ;
    BYTE   NumOfFATs	     ;
    WORD   RootEntries	     ; // Ignored
    WORD   SectorsSmall      ;
    BYTE   MediaType	     ;
    WORD   SectorsPerFAT_16  ; //Always be Zero
    WORD   SectorsPerTrack   ;
    WORD   NumOfHeads	     ;
    DWORD  HiddenSectors     ;
    DWORD  SectorsLarger     ;
    // 28 bytes 
    DWORD  SectorsPerFAT_32  ;
    WORD   ExtFlags	     ; //  default value 0 ???
    WORD   Fs_Version	     ;
    DWORD  RootStartCluster  ;
    WORD   FSInfoSectors     ; //  default value 1 ???
    WORD   BackupSectors     ; //  default value 6 ???
    WORD   Reserved[6]	     ;
    BYTE    Int13Unit_32;
    BYTE    Reserved1;
    BYTE    ExtBootSig;
    DWORD   VolumeID;
    BYTE    VolumeLabel[0x0B];
    char    FS_Type[0x08];
    BYTE    StartCode[SECTOR_SIZE*2+0x400-0x005A];
}   BOOT32_SEC,*PBOOT32_SEC ;



#define MAX_PRIMARY_NUM     4
#define MAX_LOGICAL_NUM     4

#define MIRROR_SIGN	    1
#define MOVER_SIGN	    2

#define PARTITION_SIGN_OFF  0x1FE
#define PARTITION_SIGN	    0xAA55


/*

typedef struct
{
    BYTE    BootFlag;
    BYTE    StartOfPartition[3];
    BYTE    SystemFlag;
    BYTE    EndOfPartition[3];
    DWORD   StartSector;
    DWORD   SectorsInPartition;
} PARTITION_ENTRY,*PPARTITION_ENTRY;

*/

typedef struct	tagVIRTUAL_SECTOR
    {
	DWORD	dwSector;
	PARTITION_ENTRY par[4];
    } VIRTUAL_SECTOR,*PVIRTUAL_SECTOR;



/*
 *  following structure copy from LuoYunChuan's header file
 *
 */

/*

typedef struct tagLogicParInfoEx
{
    DWORD	    dwPreStart;
    DWORD	    dwNextStart;
    DWORD	    dwCurOffset;
    PARTITION_ENTRY peCurParInfo;
} LOGICPARINFOEX,*PLOGICPARINFOEX;



typedef struct	tagParInfoOnHardDiskEx
{
    WORD	    wNumOfPri;			// the number of the primary partition
    WORD	    wNumOfLogic;		// the number of the logical partition
    PARTITION_ENTRY pePriParInfo[4];		// primary partition table entry
    LOGICPARINFOEX  peLogParInfo[MAX_PAR_NUM];	// logical partition table entry
} PARINFOONHARDDISKEX,*PPARINFOONHARDDISKEX;

*/

#define PrimaryPartition    0
#define ExtendedPartition   1
#define LogicPartition	    2

#define TransformPartition  1
#define EnlargePartition    2
#define ShrinkPartition     3
#define MovePartition	    4


#define MAX_LOGIC_PAR_NUM   26

BOOL	OnSysDynamicDeviceInit(VOID);
BOOL	OnSysDynamicDeviceExit(VOID);
VOID	OnAsyncRequest(PAEP);
USHORT	DefaultHandler(PAEP aep);
USHORT	OnConfigDcb(PAEP_dcb_config);
USHORT	OnUnconfigDcb(PAEP_dcb_unconfig);
void	InsertCallBack(PIOP,VOID (*callback)(PIOP),ULONG);
VOID	OnRequest_OnMirrorDisk(PIOP);
VOID	OnRequest_OnMoverDisk(PIOP);
VOID	OnRequest_OnRealDisk(PIOP);
VOID	OnRequest_OnSharkDisk(PIOP iop);
VOID	MirrorProcess(PIOP iop);
VOID	MoverProcess(PIOP iop);
VOID	SharkProcess(PIOP iop);
DWORD	Logic2Phys(DWORD);
BOOL	IsOurVirtualDCB(PDCB);
BOOL	IsOurMoverDCB(PDCB);
BOOL	IsOurMirrorDCB(PDCB);
BOOL	IsOurSharkDCB(PDCB  dcb);
PDCB	DriveLetter2PhysDcb(BYTE    btDriveLetter);
pDCB_cd_entry	GetCallDownChain(PDCB  dcb);
VOID	AssignTCH(PPARTITION_ENTRY,DWORD,DWORD);
BYTE	Real2MirrorDriveLttr(BYTE btDrive);
BYTE	Real2SharkDriveLttr(BYTE btDrive);
BYTE	MatchVirtualDriveLttr(BYTE btSysFlag,DWORD dwStart);
BYTE	RetrieveDriveLttr(BYTE DriveNum,DWORD flag,DWORD dwParStart);
PDCB	GetMonitorDCB(VOID);
PDCB	GetSharkDCB(VOID);
BOOL	SetMonitorDCB(DWORD dwDrive);
BOOL	SetSharkDCB(DWORD dwDrive);
VOID	CalcTick(QWORD	tick_1, QWORD  tick_2, PQWORD ticks);
void	GetCurTime(PQWORD);


#ifdef	DEBUG
BOOL	DebugQueryHandler(VOID);
CHAR	GetDebugChar(VOID);
VOID	ListPhysDcb(VOID);
VOID	ListLogicDriveDCB(VOID);
#endif

typedef struct DIOCParams   *LPDIOC;
DWORD	_stdcall VIRPARTI_W32_DeviceIOControl(DWORD,DWORD,DWORD,LPDIOC);
DWORD	_stdcall VIRPARTI_GetDriveletterOfMover(DWORD dwDDB,DWORD  hDevice,LPDIOC lpDIOCParms);
DWORD	_stdcall VIRPARTI_CleanUp(void);
DWORD	_stdcall VIRPARTI_Get_Version(DWORD,DWORD,LPDIOC);
DWORD	_stdcall VIRPARTI_Set_Params(DWORD,DWORD,LPDIOC);
DWORD	_stdcall VIRPARTI_Real2MirrorDriveLttr(DWORD,DWORD,LPDIOC);
DWORD	_stdcall VIRPARTI_RetrieveDriveLttr(DWORD  dwDDB,DWORD,LPDIOC);
DWORD	_stdcall VIRPARTI_DriveMapInfo(DWORD  dwDDB,DWORD,LPDIOC);
DWORD	_stdcall VIRPARTI_Real2SharkDriveLttr(DWORD  dwDDB,
					     DWORD  hDevice,
					     LPDIOC lpDIOCParms);
DWORD	_stdcall VIRPARTI_EnableOrDisable(DWORD  dwDDB,
					  DWORD  hDevice,
					  LPDIOC lpDIOCParms);
void SimulatePartition(DWORD dwStartSector, DWORD dwSectors, PBYTE pMemBuf,
		       DWORD dwType, BYTE btSysID, DWORD dwParStart, DWORD dwParEnd);
void SimulateMoverPartition(DWORD dwStartSector, DWORD dwSectors, PBYTE pMemBuf,
			    DWORD dwType, BYTE btSysID, DWORD dwParStart, DWORD dwParEnd);

#endif	//  VIRPARTI_H
