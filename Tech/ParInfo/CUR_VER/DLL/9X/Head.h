#ifndef	ParInfo1_H
#define	ParInfo1_H


#define MAX_RW_SECS		    	7
#define	VWIN32_DIOC_DOS_IOCTL	1
#define YG_DISK_W32_INT13       1
#define YG_GetDriveNum			4
#define FAT32ENTRY_PER_SECTOR   128
#define END_OF_FAT32_CLAIN      0x0fffffff

#ifndef	SECTOR_SIZE
#define	SECTOR_SIZE		0x200
#endif


#pragma	pack(1)	//	align to byte 
// DIOCRegs
// Structure with i386 registers for making DOS_IOCTLS
// vwin32 DIOC handler interprets lpvInBuffer , lpvOutBuffer to be this struc.
// and does the int 21
// reg_flags is valid only for lpvOutBuffer->reg_Flags
#ifndef	DIOC_REG
#define	DIOC_REG
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
#endif

typedef struct
{
	DWORD dwRootStart;//root directory abs position in the harddisk
	DWORD dwFatStart;
	DWORD dwRoot;
	DWORD dwSectorsPerCluster;
	DWORD dwSectorsPerFAT;
	BOOL  blIsME;
	BOOL  blIs98;
	BOOL  blIs98Se;
	BOOL  blIs95;
	BOOL  blIs97;
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

typedef struct  
{
	BYTE	btPaketSize;
	BYTE	btReserved;
	WORD	wCount;
	PBYTE	pbtBuffer;
	__int64 qwStartSector;
}DISK_ADDR_PKT,*PDISK_ADDR_PKT;

#pragma	pack()

extern	"C"
{
	BOOL IsSupportExt13(HANDLE hYGDev,BYTE btHardDrive);
	DWORD  LogicToPhy(DWORD dwSectors,PBIOS_DRIVE_PARAM pDriveParam);
	DWORD	Phy2Logic(BYTE btHead,WORD wTrack,BYTE btSector, PBIOS_DRIVE_PARAM pbios);
	BOOL FatSet(BYTE btHardDisk,BYTE btSystemFlag,DWORD dwStartSec,PBIOS_DRIVE_PARAM DriveParam,PFATBOOTSECT pSec);
	BOOL SearchSysFile(PFATBOOTSECT pBootSec,BYTE btHardDisk,PBIOS_DRIVE_PARAM DriveParam);
	DWORD GetRootClusters(DWORD  dwStaClu,DWORD dwFatStartSec,BYTE btHardDisk,PBIOS_DRIVE_PARAM DriveParam);
	DWORD GetRootNextCluster(DWORD dwPriClu,DWORD dwFatStartSec,BYTE btHardDisk,PBIOS_DRIVE_PARAM DriveParam);
	inline  DWORD  ClusterToSector(DWORD  dwCluster,DWORD dwDataStartSec,DWORD dwSectorsPerCluster);
	BOOL OSExist1(BYTE btHardDisk,BYTE btSystemFlag,DWORD dwStartSec,PBIOS_DRIVE_PARAM DriveParam,int);
	BOOL SearchSysFile1(PFATBOOTSECT pBootSec,BYTE btHardDisk,PBIOS_DRIVE_PARAM DriveParam,int);
	void WhichOS(FATBOOTSECT* p,int nDrive2);
	BOOL SearchSysFile2(PFATBOOTSECT pBootSec,BYTE btHardDisk,PBIOS_DRIVE_PARAM DriveParam);
}
#endif