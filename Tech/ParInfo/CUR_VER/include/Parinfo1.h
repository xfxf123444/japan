/********************************************************************/
/*                                                                  */
/*			      share.h  - shared by all modules	    */
/*                                                                  */
/********************************************************************/

#ifndef	ParInfo_H
#define	ParInfo_H

#include	"..\dll\Private.h"

#define MAX_LABELNAME			11      //size of the drive volume label
#define MAX_COMMENT		        30		//size of comment
#define MAX_PAR_NUM				10		//max of logical drive number
#define MAX_OSLABEL				10      //it's operate system name
#define	MAX_PARTITIONS	        0x04		// max 4 partitions per drive
#define PRIMARY					1		//the partition is primary
#define EXTEND					2       //the Partition is extended
#define LOGICAL					3       //the Partition is logical
#define ERRDRIVEPARAM			0xffffffff
#define	STARTPARTITION			0
#define	ENDPARTITION			1

#pragma	pack(1)	//	align to byte 


typedef struct     
{
	BYTE	BootFlag;   
	BYTE	StartOfPartition[3];
	BYTE	SystemFlag;
	BYTE	EndOfPartition[3];
	DWORD	StartSector;
	DWORD	SectorsInPartition;
} PARTITION_ENTRY,*PPARTITION_ENTRY;

typedef struct
{
	BYTE	StartCode[0x01be];
	PARTITION_ENTRY    Partition[0x04];
	WORD	Signature;
} PARTITION_SEC,*PPARTITION_SEC;


typedef struct
{
	WORD	wInfoSize;
	WORD	wFlags;
	DWORD	dwCylinders;
	DWORD	dwHeads;
	DWORD	dwSecPerTrack;
	__int64	dwSectors;
	WORD	SectorsSize;		
} BIOS_DRIVE_PARAM, *PBIOS_DRIVE_PARAM;



typedef struct	tagPARTITION_INFO
{
	BYTE	btDrive;		//drive letter
	BYTE	btHardDisk;		//drive in which harddisk
	BYTE	btSystemFlag;
	BYTE	btReserved[1];	//	padding
	int     nPartition;		//if the partition is primary,it's position of MBR;if logical,it's the number of logical
	WORD	wStartCylinder;
	WORD  	wStartHead;
	WORD  	wStartSector;
	WORD 	wEndCylinder;
	WORD  	wEndHead;
	WORD	wEndSector;
	DWORD	dwStartLogicalSector;	//it's really line address
	DWORD	dwSectorsInPartition;   //it's size of the partition
	DWORD   dwPartitionType;		//it's partition's type /eg.priamry or entended 
	BYTE	szLabelName[MAX_LABELNAME];  //drive's volume label
	char	szComment[MAX_COMMENT];
	BYTE	btFileSystem;		//FAT32 or FAT16
	char	szOsLabel[MAX_OSLABEL];// Win95 or Win98
} PARTITION_INFO,*PPARTITION_INFO;


typedef struct tagLogicParInfo
{
	PARTITION_ENTRY	peCurParInfo; //it's current logical partition table
	PARTITION_ENTRY peNextParInfo;////it's point to the next logical partition table
}LOGICPARINFO,*PLOGICPARINFO;


typedef struct tagLogicParInfoEx
{
	DWORD			dwPreStart;
	DWORD			dwNextStart;
	DWORD			dwCurOffset;
	PARTITION_ENTRY	peCurParInfo; //it's current logical partition table
}LOGICPARINFOEX,*PLOGICPARINFOEX;

typedef struct	tagParInfoOnHardDisk
{
    WORD	    wNumOfPri; //the number of the primary
    WORD	    wNumOfLogic;//the number of the logical
	PARTITION_ENTRY	 pePriParInfo[4]; //MBR 
	LOGICPARINFO     peLogParInfo[MAX_PAR_NUM]; //logical's MBR 
} PARINFOONHARDDISK,*PPARINFOONHARDDISK;

typedef struct	tagParInfoOnHardDiskEx
{
    WORD	    wNumOfPri; //the number of the primary
    WORD	    wNumOfLogic;//the number of the logical
	PARTITION_ENTRY	 pePriParInfo[4]; //MBR 
	LOGICPARINFOEX     peLogParInfo[MAX_PAR_NUM]; //logical's MBR 
} PARINFOONHARDDISKEX,*PPARINFOONHARDDISKEX;




#pragma	pack()

#ifndef EXPORT
    #define EXPORT  __declspec(dllexport)
#endif

#ifndef _VMM_
extern	"C"
{

	BOOL EXPORT WriteSector(DWORD dwStartSec,WORD wSectors,
							PBYTE pBuf,BYTE btUnit,PBIOS_DRIVE_PARAM pDriveParam);
	BOOL EXPORT ReadSector(DWORD dwStartSec, WORD wSectors,
							PBYTE pBuf,BYTE btUnit,PBIOS_DRIVE_PARAM pDriveParam);

	//	DriveNum ( 80H , 81H , ... )
	//	flags ( PRIMARY_PAR , LOICAL_PAR )	
	//	dwStart( linear sector number )	
	//	return	is drive letter ( 1 based), if 0xFF, fail
	BYTE	EXPORT  RetrieveDriveLttr(BYTE	DriveNum,DWORD flags,DWORD dwStart);

	//	DriveLttr( drive letter , 1 based )
	//	pInfo structure ( define share.h header file )
	//	return	TRUE or FALSE
	BOOL	EXPORT  GetDriveMapInfo(BYTE	DriveLttr,PSIMULATE_DRIVEMAPINFO pInfo);

#endif

	BOOL EXPORT Init_PartitionInfo(VOID);
	BOOL EXPORT Free_PartitionInfo(VOID);
	BOOL EXPORT GetPartitionInfo(BYTE btHardDrive,PPARINFOONHARDDISK pParHard);
	BOOL EXPORT OSExist(BYTE btHardDisk,BYTE btSystemFlag,DWORD dwStartSec,PBIOS_DRIVE_PARAM DriveParam);
	BOOL EXPORT PartitionInfoOfDriveLetter(BYTE btLetter,PPARTITION_INFO pPartitionInfo);
	DWORD EXPORT LocatePartition(DWORD dwSectors,BYTE btHardDisk,BYTE btStartOrEnd);
	BOOL EXPORT GetPartitionInfoEx(BYTE btHardDrive,PPARINFOONHARDDISKEX pParHard);
	BOOL EXPORT	GetDriveParam(BYTE btHardDrive,PBIOS_DRIVE_PARAM pDriveParam);
	DWORD  EXPORT GetHardDiskNum();


#ifndef _VMM_
}
#endif	//  _VMM_

#endif
