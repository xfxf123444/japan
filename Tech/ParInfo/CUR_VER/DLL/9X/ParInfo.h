

#ifndef	ParInfo_H
#define	ParInfo_H

#define PRIMARY					1		//the partition is primary
#define EXTEND					2       //the Partition is extended
#define LOGICAL					3       //the Partition is logical
#define FAT32ENTRY_PER_SECTOR   128
#define END_OF_FAT32_CLAIN      0x0fffffff
#define	FAT16					1
#define FAT32					2
#define	NTFS					3
#define	UNIX					4
#define MAX_RW_SECS		    	7
#define MAX_OSLABEL				10      //it's operate system name
#define	MAX_PARTITIONS	        0x04		// max 4 partitions per drive
#define	VWIN32_DIOC_DOS_IOCTL	1
#define YG_DISK_W32_INT13       1
#define YG_GetDriveNum			4
#define MAX_COMMENT		        30		//size of comment
#define MAX_PAR_NUM				26		//max of logical drive number
#define SECTOR_SIZE				0x200	//size of per sector
#define MAX_LABELNAME			11      //size of the drive volume label
#define ERRDRIVEPARAM			0xffffffff
#define	STARTPARTITION			0
#define	ENDPARTITION			1

#pragma	pack(1)	//	align to byte 

// DIOCRegs
// Structure with i386 registers for making DOS_IOCTLS
// vwin32 DIOC handler interprets lpvInBuffer , lpvOutBuffer to be this struc.
// and does the int 21
// reg_flags is valid only for lpvOutBuffer->reg_Flags

/*
typedef struct DIOCRegs	
{
	DWORD	reg_EBX;
	DWORD	reg_EDX;
	DWORD	reg_ECX;
	DWORD	reg_EAX;
	DWORD	reg_EDI;
	DWORD	reg_ESI;
	DWORD	reg_Flags;		
} DIOC_REGISTERS,*PDIOC_REGISTERS;

*/

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
	BYTE	StartCode[0x400-0x005A];
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
	BYTE	StartCode[SECTOR_SIZE-0x003E];
} BOOT_SEC16, *PBOOT_SEC16;


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


typedef struct
{
	DWORD dwRootStart;//root directory abs position in the harddisk
	DWORD dwFatStart;
	DWORD dwRoot;
	DWORD dwSectorsPerCluster;
	DWORD dwSectorsPerFAT;
}FATBOOTSECT,*PFATBOOTSECT;


typedef struct
{
    BYTE	dmiAllocationLength;
    BYTE	dmiInfoLength;
    BYTE	dmiFlags;
    BYTE	dmiInt13Unit;
    DWORD	dmiAssociatedDriveMap;
    DWORD	dmiPartitionStartRBA[2];
} DRIVE_MAP_INFO,*PDRIVE_MAP_INFO;

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


typedef struct  
{
	BYTE	btPaketSize;
	BYTE	btReserved;
	WORD	wCount;
	PBYTE	pbtBuffer;
	__int64 qwStartSector;
}DISK_ADDR_PKT,*PDISK_ADDR_PKT;

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
	PARTITION_ENTRY  peLogParInfo[MAX_PAR_NUM]; //logical's MBR 
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
extern	"C"
{
	BOOL EXPORT Init_PartitionInfo(VOID);
	BOOL EXPORT Free_PartitionInfo(VOID);
	BOOL EXPORT GetPartitionInfo(BYTE btHardDrive,PPARINFOONHARDDISK pParHard);
	BOOL EXPORT WriteSector(DWORD dwStartSec,WORD wSectors,
							PBYTE pBuf,BYTE btUnit,PBIOS_DRIVE_PARAM pDriveParam);
	BOOL EXPORT ReadSector(DWORD dwStartSec, WORD wSectors,
							PBYTE pBuf,BYTE btUnit,PBIOS_DRIVE_PARAM pDriveParam);
	BOOL EXPORT OSExist(BYTE btHardDisk,BYTE btSystemFlag,DWORD dwStartSec,PBIOS_DRIVE_PARAM DriveParam);
	DWORD  LogicToPhy(DWORD dwSectors,PBIOS_DRIVE_PARAM pDriveParam);
	int  FindPartition(PDRIVE_MAP_INFO pDriveMapInfo, PPARTITION_SEC pPartitionSec);
	BOOL  FindLogicalPartition(PDRIVE_MAP_INFO pDriveMapInfo, PPARTITION_SEC pPartitionSec,DWORD dwStart);
	BOOL EXPORT PartitionInfoOfDriveLetter(BYTE btLetter,PPARTITION_INFO pPartitionInfo);
	DWORD EXPORT LocatePartition(DWORD dwSectors,BYTE btHardDisk,BYTE btStartOrEnd);
	BOOL EXPORT GetPartitionInfoEx(BYTE btHardDrive,PPARINFOONHARDDISKEX pParHard);
	BOOL EXPORT	GetDriveParam(BYTE btHardDrive,PBIOS_DRIVE_PARAM pDriveParam);
	DWORD  EXPORT GetHardDiskNum();
	DWORD	Phy2Logic(BYTE btHead,WORD wTrack,BYTE btSector, PBIOS_DRIVE_PARAM pbios);
	BOOL SearchSysFile(PFATBOOTSECT pBootSec,BYTE btHardDisk,PBIOS_DRIVE_PARAM DriveParam);
	DWORD GetRootClusters(DWORD  dwStaClu,DWORD dwFatStartSec,BYTE btHardDisk,PBIOS_DRIVE_PARAM DriveParam);
	DWORD GetRootNextCluster(DWORD dwPriClu,DWORD dwFatStartSec,BYTE btHardDisk,PBIOS_DRIVE_PARAM DriveParam);
	inline  DWORD  ClusterToSector(DWORD  dwCluster,DWORD dwDataStartSec,DWORD dwSectorsPerCluster);

}
#endif