/********************************************************************/
/*                                                                  */
/*			      share.h  - shared by all modules	    */
/*                                                                  */
/********************************************************************/
#ifndef  PARINFO_H_INCLUDED
#define  PARINFO_H_INCLUDED

#ifndef BOOL
	typedef  int               BOOL  ;
#endif
#ifndef PVOID 
	typedef  void *            PVOID ;
#endif
#ifndef INT 
	typedef  int               INT   ;
#endif
#define MAX_LABELNAME			11      //size of the drive volume label
#define MAX_COMMENT		        30		//size of comment
#define MAX_PAR_NUM				26		//max of logical drive number
#define MAX_OSLABEL				10      //it's operate system name
#define	MAX_PARTITIONS	        0x04		// max 4 partitions per drive
#define PRIMARY					1		//the partition is primary
#define EXTEND					2       //the Partition is extended
#define LOGICAL					3       //the Partition is logical
#define ERRDRIVEPARAM			0xffffffff
#define	STARTPARTITION			0
#define	ENDPARTITION			1
#ifndef  MAX_RW_SECTORS
#define MAX_RW_SECTORS	7
#endif
#ifndef   SECTOR_SIZE
#define SECTOR_SIZE 512
#endif
#define MAX_RW_SIZE (MAX_RW_SECTORS*SECTOR_SIZE)
#define ERR_PARMAN_OPENVXD	101
#define ERR_PARMAN_GETDRIVEPARAM 102
#define ERR_PARMAN_READSECTOR 103
#define ERR_PARMAN_WRITESECTOR 104
#define ERR_PARMAN_NUMBEROFSECTORS 105
#define ERR_PARMAN_GETPARTITIONINFO 106
#define ERR_PARMAN_EXTENDPARTITIONSTARTSECTOR 107
#define ERR_PARMAN_MBRPARTITIONFULL 108
#define ERR_PARMAN_PARTITIONINPARTITION 109
#define ERR_PARMAN_FORMATDRIVE 110
#define ERR_PARMAN_PARAM 111
#define ERR_PARMAN_RESIZEEXTENDPARTITION 112
#define ERR_PARMAN_NOFREESPACE 113 
#define ERR_PARMAN_CREATEPARTITION 114
#define FirstHardDisk 0x80
//#define CYLINDER1024 0xfc0000
#define MBRSIGNATURE 0xaa55

struct   PARTITION_ENTRY  
{
	BYTE	BootFlag;   
	BYTE	StartOfPartition[3];
	BYTE	SystemFlag;
	BYTE	EndOfPartition[3];
	DWORD	StartSector;
	DWORD	SectorsInPartition;
};

struct	PARTITION_INFO
{
	BYTE	btDrive;		//drive letter
	BYTE	btHardDisk;		//drive in which harddisk
	BYTE	btSystemFlag;
	BYTE	btReserved;	//	padding
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
} ;

struct   PARTITION_SEC
{
	BYTE	StartCode[0x01be];
	PARTITION_ENTRY    Partition[0x04];
	WORD	Signature;
} ;

struct BIOS_DRIVE_PARAM
{
	WORD	wInfoSize;
	WORD	wFlags;
	DWORD	dwCylinders;
	DWORD	dwHeads;
	DWORD	dwSecPerTrack;
	DWORD	dwSectorL;
	DWORD	dwSectorH;
	WORD	SectorsSize;		
} ;

typedef struct
{
	DWORD  dwLogicStart;
	PARTITION_ENTRY  LogParInfo; 
}LOGICPARINFO;

struct LOGICPARINFOEX
{
	DWORD			dwPreStart;
	DWORD			dwNextStart;
	DWORD			dwCurOffset;
	PARTITION_ENTRY	peCurParInfo; //it's current logical partition table
};

struct  DRIVE_MAP_INFO
{
    BYTE	dmiAllocationLength;
    BYTE	dmiInfoLength;
    BYTE	dmiFlags;
    BYTE	dmiInt13Unit;
    DWORD	dmiAssociatedDriveMap;
    DWORD	dmiPartitionStartRBA[2];
};

struct	PARINFOONHARDDISK
{
    WORD	    wNumOfPri; //the number of the primary
    WORD	    wNumOfLogic;//the number of the logical
	PARTITION_ENTRY	 pePriParInfo[4]; //MBR 
	LOGICPARINFO     peLogParInfo[MAX_PAR_NUM]; //logical's MBR 
} ;

struct	PARINFOONHARDDISKEX
{
    WORD	    wNumOfPri; //the number of the primary
    WORD	    wNumOfLogic;//the number of the logical
	PARTITION_ENTRY	 pePriParInfo[4]; //MBR 
	LOGICPARINFOEX     peLogParInfo[MAX_PAR_NUM]; //logical's MBR 
} ;

struct  DISK_ADDR_PKT
{
	BYTE	btPaketSize;
	BYTE	btReserved;
	WORD	wCount;
	DWORD	pbtBuffer;
	DWORD   dwStartSectorL;
	DWORD	dwStartSectorH;
};

struct	CREATE_PAR_FLAG
{
	DWORD		Extended:1	;
	DWORD		Active:1	;
};

struct MyDiskIo
{
      DWORD  dwStartSector;   // starting logical sector number
      WORD    wSectors;        // number of sectors
      DWORD  dwBuffer;        // address of read/write buffer
};

struct MID
 {
    WORD  midInfoLevel;
    DWORD midSerialNum;
    BYTE  midVolLabel[11];
    BYTE  midFileSysType[8];
};

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

struct  ExtGetDskFreSpcStruc
{
    WORD ExtFree_Size;//                      DW    ?
    WORD ExtFree_Level ;//                    DW    ?
    DWORD ExtFree_SectorsPerCluster;//         DD    ?
    DWORD ExtFree_BytesPerSector       ;//     DD    ?
    DWORD ExtFree_AvailableClusters       ;//  DD    ?
    DWORD ExtFree_TotalClusters             ;//DD    ?
    DWORD ExtFree_AvailablePhysSectors  ;//;    DD    ?
    DWORD ExtFree_TotalPhysSectors  ;//        DD    ?
    DWORD ExtFree_AvailableAllocationUnits ;// DD    ?
    DWORD ExtFree_TotalAllocationUnits;      //DD    ?
    DWORD ExtFree_Rsvd1;//                      DD    2 DUP (?)
	DWORD ExtFree_Rsvd2;//
};


extern "C"
{
BOOL  	ReadSector(DWORD dwStartSec, WORD wSectors,PVOID pBuf,BYTE btUnit,BIOS_DRIVE_PARAM*);
DWORD 	LogicToPhy(DWORD dwSectors,BIOS_DRIVE_PARAM*);
BOOL 	GetPartitionInfo(BYTE btHardDrive,PARINFOONHARDDISK * pParHard);
BOOL 	GetMID(int nDrive,MID *pMid);
BOOL 	GetPartitionInfoEx(BYTE btHardDrive,PARINFOONHARDDISKEX *pParHard);
int 	GetHardDiskNum();
BOOL 	PartitionInfoOfDriveLetter(BYTE btLetter,PARTITION_INFO  *pPartitionInfo);
BOOL 	GetDriveMapInfo(int nDrive, DRIVE_MAP_INFO   *pDriveMapInfo);
BOOL 	WriteSector(DWORD dwStaSec,WORD wReadSecs,PVOID pBuf,BYTE btHardDisk,BIOS_DRIVE_PARAM*);
BOOL 	HidePartition(DWORD dwStartSec,BYTE btPriOrLog,BYTE btHardDrive,BYTE blHideOrShow);
BOOL 	ActivePartition(DWORD dwStartSec,BYTE btHardDrive);
BOOL 	DoDeletePartition(DWORD dwStartSector,BYTE btHardDisk,CREATE_PAR_FLAG flags,INT*);
BOOL 	DoResizeExtendPartition(BYTE btHardDisk,PARTITION_ENTRY  *ParEntry,INT*);
BOOL 	SetActivePartition(int i,PARTITION_SEC * ps,BYTE btHardDisk) ;
BOOL 	CaculateStartAndEnd(PARTITION_ENTRY  *pEntry,BIOS_DRIVE_PARAM DriveParam,BYTE btHardDisk,DWORD dwFlag);
DWORD 	LocatePartition(DWORD dwSectors,BYTE btHardDisk,BYTE btStartOrEnd);
BOOL 	VerifyLogicalPartition(PARINFOONHARDDISKEX  *ph, PARTITION_ENTRY  *pe);//0 is success,1 is error
//BOOL MakePartition(DWORD dwSourceStart,DWORD dwSourceSize,DWORD dwSystemFlag,DWORD dwTargetStart,DWORD dwTargetSize,DWORD dwHardDisk);
int 	VerifyPrimaryPartition(PARINFOONHARDDISKEX *ph, PARTITION_ENTRY *pe);
BOOL 	DoCreatePartition(PARTITION_ENTRY *peParEntry,BYTE btHardDisk,DWORD dwFlag,BOOL blIsFormat,BYTE *pLabel,DWORD hWnd,INT* nCreateErrorCode);
BOOL 	GetStartSector(int nDrive,DWORD* dwStartSector,int *Drive);
BOOL 	DoCreatePriPar(PARTITION_ENTRY *peParEntry,BYTE btHardDisk,	BIOS_DRIVE_PARAM *DriveParam);
BOOL 	IsFloppyDisk(int nDrive);
BOOL 	IsCDRomDrive (int nDrive);
DWORD	 GetDriveFreeSpace(char *Drive);
void 	RestartComputer();
BOOL 	IsSupportExt13(BYTE btHardDrive);
BOOL   	GetDriveParam(BYTE btDisk ,BIOS_DRIVE_PARAM *pDiskParam );
//BOOL 	OpenIMGFile(char *FileName);
}
#endif

