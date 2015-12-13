//////////////////////
// ParInfo.h
// export
// wq
//////////////////////
 
#ifndef	ParInfo_H
#define	ParInfo_H

#pragma once

#include <windows.h>
#include <winioctl.h>
#define MAX_LABELNAME			MAX_PATH      //size of the drive volume label
#define MAX_COMMENT		        MAX_PATH		//size of comment
#define MAX_PAR_NUM				26		//max of logical drive number
#define MAX_OSLABEL				MAX_PATH      //it's operate system name
#define	MAX_PARTITIONS	        0x04		// max 4 partitions per drive
#define PRIMARY					1		//the partition is primary
#define EXTEND					2       //the Partition is extended
#define LOGICAL					3       //the Partition is logical
#define ERRDRIVEPARAM			0xffffffff
//align flags
#define	ALIGN_FOREWARD			0
#define	ALIGN_BACKWARD			1
#define	ALIGN_PRIMARY			PRIMARY
#define	ALIGN_EXTEND			EXTEND
#define	ALIGN_LOGICAL			LOGICAL

#define MAX_PHYSICALDISK_NUM	128

extern DWORD SECTOR_SIZE;

enum GUID_PARTITION_TYPE{
	PARTITION_UNDEFINED = 0,
	PARTITION_BASIC_DATA_GUID,
	PARTITION_ENTRY_UNUSED_GUID,
	PARTITION_SYSTEM_GUID,
	PARTITION_MSFT_RESERVED_GUID,
	PARTITION_LDM_METADATA_GUID,
	PARTITION_LDM_DATA_GUID,
	PARTITION_MSFT_RECOVERY_GUID
};

#pragma	pack(1)	//	align to byte 

typedef struct
{
	BYTE	Jump[3];
	char	szOEMName[8];
	WORD	BytesPerSector;
	BYTE	SectorsPerCluster;
	WORD	BootSectors;
	BYTE	Reserved[5];
	BYTE	MDB;
	WORD	Reserved1;
	WORD	SectorsPerTrack;
	WORD	NumOfHeads;
	DWORD	DriveStart;
	DWORD	Reserved2;
	DWORD	Reserved3;//always be 0x800080
	DWORD	dwTotalSectorsLow;
	DWORD	dwTotalSectorsHigh;
	DWORD	dwStartMFTLow;
	DWORD	dwStartMFTHigh;
	DWORD	dwStartMFTMirrLow;
	DWORD	dwStartMFTMirrHigh;
	DWORD   ClustPerMFT;
	DWORD   ClustPerIndex;
	DWORD	dwSerialNum;
	BYTE	StartCode[0x200-0x04C];
} BOOT_SEC_NTFS, *PBOOT_SEC_NTFS;

typedef struct
{
	BYTE	Jump[3];
	char	szOEMName[8];
	WORD	BytesPerSector;
	BYTE	SectorsPerCluster;
	WORD	BootSectors;
	BYTE	NumOfFATs;
	WORD	NumOfRootEntries_16;
	WORD	NumOfSectors;
	BYTE	MDB;
	WORD	SectorsPerFAT_16;
	WORD	SectorsPerTrack;
	WORD	NumOfHeads;
	DWORD	DriveStart;
	DWORD	NumOfSectorsInDrive;
	DWORD	SectorsPerFAT_32;
	WORD	FS_Version;
	WORD	FS_Info_Sec;
	DWORD	RootCluster_Start;
	WORD	FirstSectorNum;
	WORD	Backup_Boot_Sec;
	BYTE	Reserved[0x0C];
	BYTE	Int13Unit_32;
	BYTE	Reserved1;
	BYTE	ExtBootSig;
	DWORD	VolumeID;
	BYTE	VolumeLabel[0x0B];
	char	FS_Type[0x08];
	BYTE	StartCode[0x200-0x05A];
} BOOT_SEC32, *PBOOT_SEC32;

typedef struct
{
	BYTE	Jump[3];
	char	szOEMName[8];
	WORD	BytesPerSector;
	BYTE	SectorsPerCluster;
	WORD	BootSectors;
	BYTE	NumOfFATs;
	WORD	NumOfRootEntries_16;
	WORD	NumOfSectors;
	BYTE	MDB;
	WORD	SectorsPerFAT_16;
	WORD	SectorsPerTrack;
	WORD	NumOfHeads;
	DWORD	DriveStart;
	DWORD	NumOfSectorsInDrive;
	WORD	PhiscalDrive;
	BYTE	ExtBootSig;
	DWORD	VolumeID;
	BYTE	VolumeLabel[0x0B];
	char	FS_Type[0x08];
	BYTE	StartCode[0x200-0x03E];
} BOOT_SEC16, *PBOOT_SEC16;

typedef struct     
{
	DWORD PartitionStyle;
	// mbr
	BYTE	BootFlag;   
	BYTE	SystemFlag;
	// gpt
	GUID_PARTITION_TYPE GUIDType;

	WORD PartitionStartSector;
	WORD PartitionStartCylinder;
	WORD PartitionStartHeader;
	WORD PartitionEndSector;
	WORD PartitionEndCylinder;
	WORD PartitionEndHeader;

	DWORD	StartSector;
	DWORD	SectorsInPartition;
} PARTITION_ENTRY,*PPARTITION_ENTRY;

typedef struct
{
	BYTE				StartCode[0x01be];
	PARTITION_ENTRY		Partition[0x04];
	WORD				Signature;
} PARTITION_SEC,*PPARTITION_SEC;

typedef struct
{
	WORD		wInfoSize;
	WORD		wFlags;
	DWORD		dwCylinders;
	DWORD		dwHeads;
	DWORD		dwSecPerTrack;
	__int64		dwSectors;
	WORD		SectorsSize;		
} BIOS_DRIVE_PARAM, *PBIOS_DRIVE_PARAM;

typedef	struct	tagCREATE_PAR_FLAG
{
	DWORD		Extended	:	1;
	DWORD		Active		:	1;
} CREATE_PAR_FLAG,*PCREATE_PAR_FLAG;

typedef struct	tagPARTITION_INFO
{
	BYTE	btBootFlag;
	BYTE	btDrive;		//drive letter
	BYTE	btHardDisk;		//drive in which harddisk
	BYTE	btSystemFlag;
	BYTE	btReserved;	//	padding
	int     nPartition;	//if the partition is primary,
						//it's position of MBR;
						//if logical,it's the number of logical
	DWORD	dwStartLogicalSector;	//it's really line address
	DWORD	dwSectorsInPartition;   //it's size of the partition
	DWORD   dwPartitionType;		//it's partition's type /eg.priamry or entended 
	BYTE	szLabelName[MAX_LABELNAME];  //drive's volume label
	char	szComment[MAX_COMMENT];
	BYTE	btFileSystem;		//FAT32 or FAT16
	char	szOsLabel[MAX_OSLABEL];// Win95 or Win98
} PARTITION_INFO,*PPARTITION_INFO;

typedef struct tagLogicParInfoEx
{
	DWORD				dwPreStart;
	DWORD				dwNextStart;
	DWORD				dwCurOffset;
	PARTITION_ENTRY		peCurParInfo; //it's current logical partition table
}LOGICPARINFOEX,*PLOGICPARINFOEX;

typedef struct
{
	DWORD					dwLogicStart;
	PARTITION_ENTRY			LogParInfo; 
}LOGICPARINFO,*PLOGICPARINFO;

typedef struct	tagParInfoOnHardDisk
{
    WORD					wNumOfPri; //the number of the primary
    WORD					wNumOfLogic;//the number of the logical
	PARTITION_ENTRY			pePriParInfo[MAX_PAR_NUM]; // primary partition
	LOGICPARINFO			peLogParInfo[MAX_PAR_NUM]; //logical partition
} PARINFOONHARDDISK,*PPARINFOONHARDDISK;


typedef struct	tagParInfoOnHardDiskEx
{
    WORD					wNumOfPri; //the number of the primary
    WORD					wNumOfLogic;//the number of the logical
	PARTITION_ENTRY			pePriParInfo[4]; //MBR 
	LOGICPARINFOEX			peLogParInfo[MAX_PAR_NUM]; //logical's MBR 
} PARINFOONHARDDISKEX,*PPARINFOONHARDDISKEX;

typedef struct
{
	int						nHardDiskNum;
	int						nPartitionType;
	PARTITION_INFORMATION	pi;
}PARTITION_INFO_2000,*PPARTITION_INFO_2000;

#pragma	pack()

//#define DLL_PARINFO_API __declspec(dllexport)
#define DLL_PARINFO_API

typedef struct	tagSimulateDriveMapInfo
{
	BYTE		DriveNum;	//	80H , 81H
	BYTE		ParType;	//	04H , 06H 
	WORD		wReserve;	
	DWORD		dwStart;	//	linear sector number
} SIMULATE_DRIVEMAPINFO,*PSIMULATE_DRIVEMAPINFO;	

extern	"C"
{
BOOL DLL_PARINFO_API WriteSector(DWORD					dwStartSec,
								 WORD					wSectors,
								 PBYTE					pBuf,
								 BYTE					btUnit,
								 PBIOS_DRIVE_PARAM		pDriveParam);
BOOL DLL_PARINFO_API ReadSector(DWORD					dwStartSec,
								WORD					wSectors,
								PBYTE					pBuf,
								BYTE					btUnit,
								PBIOS_DRIVE_PARAM		pDriveParam);

BYTE DLL_PARINFO_API RetrieveDriveLttr(BYTE			DriveNum,
									   DWORD		flags,
									   DWORD		dwStart);
//	DriveLttr( drive letter , 1 based )
//	pInfo structure ( define share.h header file )
//	return	TRUE or FALSE
BOOL DLL_PARINFO_API GetDriveMapInfo(BYTE						DriveLttr,
									 PSIMULATE_DRIVEMAPINFO		pInfo);

BOOL DLL_PARINFO_API Init_PartitionInfo();
BOOL DLL_PARINFO_API Free_PartitionInfo();

DWORD DLL_PARINFO_API GetHardDiskNum();

BOOL DLL_PARINFO_API
GetPartitionInfo(BYTE								btHardDrive,
				 PPARINFOONHARDDISK					pParHard);

BOOL DLL_PARINFO_API
PartitionInfoOfDriveLetter(BYTE						btLetter,
						   PPARTITION_INFO			pParInfo);

BOOL DLL_PARINFO_API
GetPartitionInfoEx(BYTE								btHardDrive,
				   PPARINFOONHARDDISKEX				pParHard);

BOOL DLL_PARINFO_API GetDriveParam(BYTE						btHardDrive,
								   PBIOS_DRIVE_PARAM		pDriveParam);

BOOL DLL_PARINFO_API 
PartitionInfoByDriveLetter(BYTE						btDriveLetter,
						   PPARTITION_INFO_2000		pPi2000);

void GUIDToStr(const GUID& guid, char* strGUID);
GUID_PARTITION_TYPE GetGUIDPartitionType(const GUID& id);
//for windows 2000

void DLL_PARINFO_API InitOperationSystemType();
}

#endif // ParInfo_H
