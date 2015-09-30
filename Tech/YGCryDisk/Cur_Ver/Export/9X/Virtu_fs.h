#ifndef EXPORT_VFS
#define EXPORT_VFS

#define     VFS_ID	"System\\CurrentControlSet\\Service\\VxD\\Virtu_FS"
#define     VIRTU_INFO	"VFS_Info"

#ifndef GARBAGE
#define GARBAGE     0xcc
#endif


#ifndef     SECTOR_SIZE
#define     SECTOR_SIZE 	512
#endif

typedef struct	tagParaForMoverDsk
    {
	DWORD	dwLinSectorNumLo;
	DWORD	dwLinSectorNumHi;
	BYTE	btPartitionType;
	CHAR	BootSectorData[SECTOR_SIZE];
    } PARA_FOR_MOVER_DSK,*PPARA_FOR_MOVER_DSK;


typedef struct	tagSectorRec
    {
	DWORD	    dwLogicSecNo;
	CHAR	    cDataOfSector[SECTOR_SIZE];
    } SECTOR_REC,*PSECTOR_REC;



typedef struct	tagSEC_PTR
    {
	DWORD	    dwSector;
	PSECTOR_REC pSr;
    } SEC_PTR,*PSEC_PTR;

//
//  VIRTU_FS.VxD export service
//

#define VIRTU_FS_Get_Version		    1
#define VIRTU_FS_EnableOrDisable	    2
#define VIRTU_FS_UpdateFSData		    3
#define VIRTU_FS_FreeFsData		    4
#define VIRTU_FS_FstNxtFSData		    5
#define VIRTU_FS_WriteEMBR		    6
#define VIRTU_FS_GetSectorGroup 	    7
#define VIRTU_FS_GetNumOfSysSectors	    8

#endif	//  EXPORT_VFS
