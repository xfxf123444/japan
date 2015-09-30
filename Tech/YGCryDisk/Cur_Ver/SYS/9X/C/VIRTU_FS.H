// VIRTU_FS.H -- Virtual File System header file
// Copyright (C) 1999 by YG Corp.
// All rights reserved
// Author:  ZL
// Date:    25-Nov-1999

#ifndef VIRTU_FS_H
#define VIRTU_FS_H

#ifndef MAX
#define MAX(a,b)    (((a)>(b))	? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b)    (((a)<(b))	? (a) : (b))
#endif


#define VIRTU_FS_Major	    1
#define VIRTU_FS_Minor	    0


#ifndef     SECTOR_SIZE
#define     SECTOR_SIZE 	512
#endif

#ifndef     PARTBL2BOOTSEC_OFFSET
#define     PARTBL2BOOTSEC_OFFSET   0x3f
#endif

#define PRIMARY     1
#define LOGICAL     3


#define     VFS_ID	"System\\CurrentControlSet\\Service\\VxD\\Virtu_FS"
#define     VIRTU_INFO	"VFS_Info"

#ifndef GARBAGE
#define GARBAGE     0xcc
#endif

#define     TOKENCOUNT		1


typedef struct	tagParaForMoverDsk
{
    DWORD   dwLinSectorNumLo;
    DWORD   dwLinSectorNumHi;
    BYTE    btPartitionType;
    CHAR    BootSectorData[SECTOR_SIZE];
} PARA_FOR_MOVER_DSK,*PPARA_FOR_MOVER_DSK;

typedef struct	tagSectorRec
{
    CHAR	cDataOfSector[SECTOR_SIZE];
} SECTOR_REC,*PSECTOR_REC;

/*
typedef struct	tagSEC_PTR
    {
	DWORD	    dwSector;
	PSECTOR_REC pSr;
    } SEC_PTR,*PSEC_PTR;
*/

#ifdef	VXD_H

typedef struct
{
    BYTE    Jump[3];		//  0
    char    szOEMName[8];	//  3
    WORD    BytesPerSector;	//  11
    BYTE    SectorsPerCluster;	//  13
    WORD    BootSectors;	//  14
    BYTE    NumOfFATs;		//  16
    WORD    NumOfRootEntries_16;//  17
    WORD    NumOfSectors;	//  19
    BYTE    MDB;		//  21
    WORD    SectorsPerFAT_16;	//  22
    WORD    SectorsPerTrack;	//  24
    WORD    NumOfHeads; 	//  26
    DWORD   DriveStart; 	//  28
    DWORD   NumOfSectorsInDrive;//  32
    DWORD   SectorsPerFAT_32;	//  36
    WORD    FS_Version; 	//  40
    WORD    FS_Info_Sec;	//  42
    DWORD   RootCluster_Start;	//  44
    WORD    FirstSectorNum;	//  48
    WORD    Backup_Boot_Sec;	//  50
    BYTE    Reserved[0x0C];	//  52
    BYTE    Int13Unit_32;	//  64
    BYTE    Reserved1;		//  65
    BYTE    ExtBootSig; 	//  66
    DWORD   VolumeID;		//  67
    BYTE    VolumeLabel[0x0B];	//  71
    char    FS_Type[0x08];	//  82
    BYTE    StartCode[0x400-0x005A];	//  90
} BOOT_SEC32, *PBOOT_SEC32;


typedef struct
{
    BYTE    Jump[3];		    //	0
    char    szOEMName[8];	    //	3
    WORD    BytesPerSector;	    //	11
    BYTE    SectorsPerCluster;	    //	13
    WORD    BootSectors;	    //	14
    BYTE    NumOfFATs;		    //	16
    WORD    NumOfRootEntries_16;    //	17
    WORD    NumOfSectors;	    //	19
    BYTE    MDB;		    //	21
    WORD    SectorsPerFAT_16;	    //	22
    WORD    SectorsPerTrack;	    //	24
    WORD    NumOfHeads; 	    //	26
    DWORD   DriveStart; 	    //	28
    DWORD   NumOfSectorsInDrive;    //	32
    WORD    PhiscalDrive;	    //	36
    BYTE    ExtBootSig; 	    //	38
    DWORD   VolumeID;		    //	39
    BYTE    VolumeLabel[0x0B];	    //	43
    char    FS_Type[0x08];	    //	54
    BYTE    StartCode[SECTOR_SIZE-0x003E];  //	62
} BOOT_SEC16, *PBOOT_SEC16;


int	FastUpdateFSData(PDWORD pBitMap, DWORD dwBase, DWORD dwCeiling);
BOOL	OnSysDynamicDeviceInit();
BOOL	OnSysDynamicDeviceExit();
VOID	OnAsyncRequest(PAEP aep);
USHORT	OnConfigDcb(PAEP_dcb_config aep);
void	InsertCallBack(PIOP iop, VOID (*callback)(PIOP), ULONG refdata);
VOID	OnRequest(PIOP iop);
BOOL	IsOurMoverDCB(PDCB  dcb);
BOOL	IsOurVirtualDCB(PDCB  dcb);
BOOL	ReadSector(DWORD dwLinSectorNum, PSECTOR_REC pBuf);
BOOL	WriteSector(DWORD dwLinSectorNum, PSECTOR_REC pBuf);
BOOL	GetParameterForMoverDsk(PPARA_FOR_MOVER_DSK  p);
USHORT	DefaultHandler(PAEP aep);

PBYTE	GetFstNxtFsData(PBYTE pSector, PDWORD pdwIndex, PDWORD pdwSectorNum);
int	GetSectorGroup(PBYTE pSector, PDWORD pdwSectorNum, int nMaxSectors);
BOOL	FreeFsData(VOID);
USHORT	OnUnConfigDcb(PAEP_dcb_unconfig aep);
BOOL	WriteEMBR(DWORD dwSecAddr, PBYTE pEMBR);

#ifdef	DEBUG
BOOL	DebugQueryHandler(VOID);
CHAR	GetDebugChar(VOID);
VOID	ListSectorsInMoverDisk(VOID);
#endif

extern	  BOOL	 g_bWorking;

#endif	//  VXD_H

#endif	//  VIRTU_FS_H
