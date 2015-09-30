// BLKMOVER.H
// Copyright (C) 1999 by YG Corp.
// All rights reserved
// Author:  ZL
// Date:    25-Nov-1999


#ifndef BLKMOVER_H
#define BLKMOVER_H

#define BLKMOVER_Major	    1
#define BLKMOVER_Minor	    0


#ifndef	SECTOR_SIZE
#define SECTOR_SIZE		    512
#endif

#define XFACTOR 		    10

#ifndef READ_OPERATION
#define READ_OPERATION		    TRUE
#endif

#ifndef WRITE_OPERATION
#define WRITE_OPERATION 	    FALSE
#endif

#ifndef MAX
#define MAX(a,b)    (((a)>(b))	? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b)    (((a)<(b))	? (a) : (b))
#endif

#define _A_NORMAL		    0

#ifndef GARBAGE
#define GARBAGE 		    0xcc
#endif

#define MULTI_IO		    0x01


#define SECTORS_PER_MOVE	    128
#define INVALID_INDEX		    -1
#define MOVE_SLICE		    0x2000

#define CACHE_WRITE_SECTORS	    0x100
#define MIN_CACHE_WRITE_SECTORS     0x10
#define SECTORS_PER_IO		    0x100
#define SYSTEM_DATA_SECTOR_BUF_SIZE 0x400

#define MAX_ASYNC_SECTORS	    0x100
#define MAX_NUM_OF_LISTS	    0x100

#define BLKMOVER_NOT_READY	    1
#define BLKMOVER_MEMORY_STARVE	    2
#define BLKMOVER_CONTINUE	    3
#define BLKMOVER_DB_ERROR	    4
#define BLKMOVER_SUCCESS	    5

//#define MOVE_BUFFER_SIZE		0x100L

#ifndef MAX_PATH
#define MAX_PATH    260
#endif
#define MAX_SYNC_SEM		    8

typedef QWORD	*PQWORD;

typedef struct	tagBlkMover_Data
{
	PDCB		dcb;
	DWORD		dwSignature;	    //	XXXX
	BOOL		bWorking;
} BLKMOVER_DATA,*PBLKMOVER_DATA;

#ifdef	BLKMOVER_VXD

#define BLKMOVER_SIG	0x58585858


typedef struct tagRM_Handle
{
	RM_SEC_GROUP_TABLE			*pCurNode;
	DWORD						dwCurGroupNum;
}RM_HANDLE,*PRM_HANDLE;

typedef struct tagSECTOR_GROUP
{
    DWORD   dwStartSector;
    DWORD   dwSectors;
    DWORD   dwOfs;
} SECTOR_GROUP, *PSECTOR_GROUP;

// Describe a continuous sector block either on disk.
typedef struct tagDiskRange
{
    DWORD	dwSecAddr;	// the start of the sector range
    DWORD	dwSectors;	// Number of sectors in this range
} DISK_RANGE, *PDISK_RANGE;

// Describe a continuous sector block either in memory.
typedef struct tagMemRange
{
    DWORD	dwSecOfs;	    // Memory offset in relative to the global disk cache buffer
    DWORD	dwSectors;	    // size of the memory range in bytes
} MEM_RANGE, *PMEM_RANGE;


// A cache object.
typedef struct tagCacheObject
{
    DISK_RANGE	    DiskRange;	   // the disk ranges the object occupies
    MEM_RANGE	    MemRange;	   // Memory ranges of the object
    DWORD	    dwUseCnt;
    BOOL	    bLoaded;
    BOOL	    bDirty;
    struct tagCacheObject   *pPrev;	     // LRU list
    struct tagCacheObject   *pNext;	     // LRU list
} CACHE_OBJECT, *PCACHE_OBJECT;

// A data object.
typedef struct tagBMObject
{
    DWORD	    dwOfs;
    DWORD	    dwObjSize;
    DWORD	    dwNumObjs;
    DWORD	    dwSectors;	    // Memory ranges of the object
    PCACHE_OBJECT   pCacheObj;	   // corresponding cache object
} BM_OBJECT, *PBM_OBJECT;

// One of such object for each disk
// If Shark drive == Mover Drive then we only have one such object
typedef struct tagDiskCache
{
    PBYTE			pCache; 		// Cache Buffer
    DWORD			dwCacheSize;	// Size of the cache buffer
    PYG_LIST		pObjList;		// Describes all cached sector blocks
    PCACHE_OBJECT	pLRUHead;
    PCACHE_OBJECT	pLRUTail;
} DISK_CACHE, *PDISK_CACHE;


// A Sector Block represents block of sectors and their corresponding maping
typedef struct tagSectorBlock
{
    BM_OBJECT	Content;
    BM_OBJECT	Map;
} SECTOR_BLOCK, *PSECTOR_BLOCK;

// This structure describes a block of original sectors and their corresponding
// content.
// This structure is used for virtualization of the original space
typedef struct tagOrigSectorBlock
{
    PYG_LIST	 pOrigMap;
    PYG_LIST	 pCurMap;
    PYG_LIST	 pSectors;
} ORIG_SECTOR_BLOCK, *PORIG_SECTOR_BLOCK;


typedef struct	tagSDD	/*  SDD = Sector Discriptor Directory	*/
    {
	DWORD	sector_SDT  :	31;
	DWORD	Invalidate  :	1;  //	TRUE = invalid, FALSE = validate
    } SDD,*PSDD;


typedef struct	tagIndexForSwap
    {
	DWORD	dwSrcIndex;
	DWORD	dwDstIndex;
    } INDEX_FOR_SWAP,*PINDEX_FOR_SWAP;


typedef struct	tagSectorBuf
    {
	CHAR	Buf[SECTOR_SIZE];
    } SECTOR_BUF,*PSECTOR_BUF;

#define PHYSMEM_16M   16 * 0x100000
#define PHYSMEM_32M   32 * 0x100000
#define PHYSMEM_48M   48 * 0x100000
#define PHYSMEM_64M   64 * 0x100000
#define PHYSMEM_128M  128* 0x100000

#define CACHE_LEN_512 512
#define CACHE_LEN_1K  1024  //	1M cache sector for 0.5M physical memory
#define CACHE_LEN_2K  2048  //	2M cache sector for 1M physical memory
#define CACHE_LEN_4K  4096  //	4M cache sector for 2M physical memory
#define CACHE_LEN_8K  8192  //	8M cache sector for 4M physical memory
#define CACHE_LEN_16K 16384 //	16M cache sector for 8M physical memory
#define CACHE_LEN_32K 32768 //	32M cache sector for 16M physical memory
#define NUM_SECMAP_ENTRIES    0x100*SECTOR_SIZE/sizeof(SECMAP)
#define MAP_CACHE_READ_SIZE	0x100


#define IO_NOT_CACHE	1   //	Read or Write through , not be cached
#define IO_CACHE	2
#define MAX_MOVING_SECTORS  2	// There could be at max 2 sectors invalid during move



typedef struct tagGET_SEC_GROUP
{
    DWORD	dwCurLogicalSecAddr;
    PSEC_GROUP	pCurGroup;
} GET_SEC_GROUP,*PGET_SEC_GROUP;

typedef struct	tagVXD_STATUS
    {
	BYTE	EnableCache :	1;
	BYTE	Done	    :	1;
    } VXD_STATUS,*PVXD_STATUS;


typedef struct	tagCACHE_STATUS
    {
	DWORD	 Dirty	 :   1;
	DWORD	 Access  :   1;
	DWORD	 res	 :   30;
    } CACHE_STATUS,*PCACHE_STATUS;

__int64 lmul(DWORD a, DWORD b);
__int64 ldiv(__int64 a, DWORD b);

USHORT	OnTimeOut(PAEP_iop_timeout_occurred aep);
BOOL	OnSysDynamicDeviceInit(VOID);
BOOL	OnSysDynamicDeviceExit(VOID);
VOID	OnAsyncRequest(PAEP);
USHORT	DefaultHandler(PAEP aep);
USHORT	OnConfigDcb(PAEP_dcb_config);
USHORT	OnUnconfigDcb(PAEP_dcb_unconfig);
BOOL	IsOurVirtualDCB(PDCB  dcb);
void	InsertCallBack(PIOP iop, VOID (*callback)(PIOP), ULONG refdata);
VOID	OnRequest(PIOP);
BOOL	SetMoverPara(PBLK_MOVER_PARAM PBlkMoverPara);
/**************Using for RecoverMaster *****************/ 
BOOL	RmInit(BOOL bEnable,BYTE btDrive);
BOOL	SetReDirPointer(DWORD dwStart,DWORD dwSize);
BOOL	SetGroupTable(DWORD dwStartSec,DWORD dwRecNum);
BOOL	EnabelOrDisRelocate(BOOL bRelocate,DWORD dwMinSec,DWORD dwMaxSec);
BOOL    WriteSectors(DWORD dwStartSec,DWORD dwSecs,PVOID pBuffer);
BOOL    ReadSectors(DWORD dwStartSec,DWORD dwSecs,PVOID pBuffer);
BOOL	MoveGroup(DWORD dwSStart,DWORD dwSize,DWORD dwTStart);
BOOL	SetWriteResult(BOOL bSys,DWORD dwStartSec,DWORD dwSize);
BOOL	EnableWriteProtect();
BOOL	SetMoingGroupCur(BOOL bFront,DWORD dwSStart,DWORD dwTStart,DWORD dwSize,DWORD dwRecNum);


BOOL	DoReLocate(DWORD dwIoMode,DWORD dwStart,DWORD dwSectors,PVOID pvBuffer,PBLKMOVER_DATA pBlkMoverData);
BOOL	SearchGroups(DWORD dwIoMode,DWORD dwStart,DWORD dwSectors,PVOID pvBuffer,PBLKMOVER_DATA pBlkMoverData);
PRM_SEC_GROUP GetFirstSecsGroup(PBLKMOVER_DATA pBlkMoverData,RM_HANDLE *Handle, DWORD dwStartSec,DWORD dwSecs,BOOL *bStatus);
PRM_SEC_GROUP GetNextSecsGroup(PBLKMOVER_DATA pBlkMoverData,RM_HANDLE *Handle, DWORD dwStartSec,DWORD dwSecs,BOOL *bStatus);
/******************************************************/
BOOL	Preface(VOID);
BOOL	PostScript(BOOL bReboot);
BOOL	EnableOrDisable(BOOL bEnableOrDisable);
BOOL	ReadWriteOnMover(BOOL,DWORD,DWORD,PBYTE,PBLKMOVER_DATA,PIOR);
BOOL	SecureReadWriteSector(BOOL,DWORD,DWORD,PVOID,PBLKMOVER_DATA);
BOOL	SynReadWrite(BOOL,DWORD,DWORD,PVOID,PBLKMOVER_DATA);
BOOL	ReadSector(DWORD,DWORD,DWORD,PVOID,PBLKMOVER_DATA);
BOOL	WriteSector(DWORD,DWORD,DWORD,PVOID,PBLKMOVER_DATA);
BOOL	DoLinearIo(DWORD dwIoMode,DWORD dwStart,DWORD dwSectors,PVOID pvBuffer,PBLKMOVER_DATA pBlkMoverData);
void	SetWorkingSector(DWORD dwSector, PDWORD pWorkingSector, DWORD dwFlags);
void	WaitOnWorkingSector(DWORD dwSector, PDWORD pWorkingSectors, int nNumWorkingSectors, DWORD dwFlag);
void	YG_IlbInternalRequest(PIOP iop, PDCB dcb);
VOID	InternalProcess(PIOP iop);
BOOL	FlushPortDriver(PBLKMOVER_DATA pBlkMoverData);
VOID	Reboot();
BOOL	DoSectorTransfer(BOOL bRDWT, PIOP iop, PDCB dcb, PDCB pPhysDcb, pDCB_cd_entry pCallDown,
		      DWORD dwStartingSector, DWORD dwSectors,
		      PBYTE pBuf);

BOOL	 LoadSource(PYG_LIST pOrigSrcMapObjList, PYG_LIST *pSrcMapObjList, PYG_LIST *pSrcSecObjList);
PYG_LIST AllocMapEntryObj(PYG_LIST pObjList, DWORD dwSecPos, DWORD dwSectors);
PYG_LIST AllocSectorObj(PYG_LIST pObjList, DWORD dwSecPos, DWORD dwSectors);
BOOL	 AllocateObjectList(DWORD dwSecPos, DWORD dwSectors,
			    PDISK_CACHE pCache, PBLKMOVER_DATA pBlkMoverData,
			    DWORD dwObjSize, PYG_LIST pBMObjList);
BOOL	 CreateObjUnAllocatedRegion(PDISK_CACHE pCache, PBLKMOVER_DATA pBlkMoverData, PCACHE_OBJECT *pCacheObj,
				    PDWORD pdwLeft, DWORD dwRight, DWORD dwObjSize, PYG_LIST pBMObjList);
BOOL	 ProcessAllocatedRegionLeft(PDISK_CACHE pCache, PCACHE_OBJECT pObj, DWORD dwSectors);
BOOL	 ProcessAllocatedRegionRight(PDISK_CACHE pCache, PCACHE_OBJECT pObj, DWORD dwSectors);
BOOL	 ProcessOverlappedRegion(PDISK_CACHE pCache, PCACHE_OBJECT pObj, PDWORD pdwLeft);
BOOL	 AddToFreeList(PDISK_CACHE pCache, PCACHE_OBJECT pCacheObj);
BOOL	 AllocateCache(PDISK_CACHE pCache, DWORD dwSectors, PBLKMOVER_DATA pBlkMoverData, PYG_LIST pFreeCacheList);
BOOL	 FreeOrigSecBlock(PORIG_SECTOR_BLOCK pOrigSecBlock);
BOOL	 ReadObject(PBM_OBJECT pObj, PDISK_CACHE pCache, PBLKMOVER_DATA pBlkMoverData);
BOOL	 WriteObject(PBM_OBJECT pObj, PDISK_CACHE pCache, PBLKMOVER_DATA pBlkMoverData);
BOOL	 ReadObjectList(PYG_LIST pObjList, PDISK_CACHE pCache, PBLKMOVER_DATA pBlkMoverData);
BOOL	 WriteObjectList(PYG_LIST pObjList, PDISK_CACHE pCache, PBLKMOVER_DATA pBlkMoverData);
BOOL	 ReadMapObjectList(PYG_LIST pObjList, PDISK_CACHE pCache, PBLKMOVER_DATA pBlkMoverData);
BOOL	 WriteMapObjectList(PYG_LIST pObjList, PDISK_CACHE pCache, PBLKMOVER_DATA pBlkMoverData);
BOOL	 FreeObject(PBM_OBJECT pObj, PDISK_CACHE pCache);
BOOL	 FreeObjectList(PYG_LIST pObjList, PDISK_CACHE pCache);
BOOL	 GetTranslatedReadRanges(PYG_LIST pCacheList, PYG_LIST pObjList, PYG_LIST pReadList);
BOOL	 GetTranslatedWriteRanges(PYG_LIST pCacheList, PYG_LIST pObjList, PYG_LIST pList);
PVOID	 GetFirstMemObject(PDISK_CACHE pCache, PYG_LIST pObjList, PBM_OBJECT *pObject);
PVOID	 GetNextMemObject(PDISK_CACHE pCache, PYG_LIST pObjList, PVOID pData, PBM_OBJECT *pObject);
BOOL	 AddToObjectListInOrder(PYG_LIST pObjList, PCACHE_OBJECT pNewObj);
BOOL	 AsyncReadWriteSectors(BOOL bRDWT, DWORD dwStartingSector,
			       DWORD dwSectors,
			       PBYTE pBuffer,
			       PBLKMOVER_DATA pBlkMoverData);
BOOL	 AddToRangeList(PYG_LIST pRangeList, PCACHE_OBJECT pNewObj);
BOOL	 ReadRanges(PYG_LIST pList, PDISK_CACHE pCache, PBLKMOVER_DATA pBlkMoverData);
BOOL	 WriteRanges(PYG_LIST pList, PDISK_CACHE pCache, PBLKMOVER_DATA pBlkMoverData);
BOOL	 GetReadRanges(PYG_LIST pCacheList, PYG_LIST pObjList, PYG_LIST pList);
BOOL	 GetWriteRanges(PYG_LIST pCacheList, PYG_LIST pObjList, PYG_LIST pList);
BOOL	 CreateObjOverlappedRegion(PDISK_CACHE pCache, PCACHE_OBJECT pObj, PDWORD pdwLeft, DWORD dwObjSize,
				   PYG_LIST pBMObjList);
BOOL	 InsertToLRUList(PDISK_CACHE pCache, PCACHE_OBJECT pNewObj, PCACHE_OBJECT pObj);
BOOL	 MoveToLRUTail(PDISK_CACHE pCache, PCACHE_OBJECT pObj);
BOOL	 DeleteFromLRUList(PDISK_CACHE pCache, PCACHE_OBJECT pObj);
BOOL	 SetDirtyFlag(PYG_LIST pObjList, BOOL bDirty, PDISK_CACHE pCache);
BOOL	 SetDirtyFlagObject(PBM_OBJECT pObj, BOOL bDirty, PDISK_CACHE pCache);
BOOL	 FlushMoverCache(PDISK_CACHE pCache, PBLKMOVER_DATA pBlkMoverData);
BOOL	 FlushSharkCache(PDISK_CACHE pCache, PBLKMOVER_DATA pBlkMoverData);
BOOL	 FlushMergeCacheObjs(PDISK_CACHE pCache, PBLKMOVER_DATA pBlkMoverData, DWORD dwSectors, PYG_LIST pFreeCacheList);
BOOL	 MergeCacheObjects(PDISK_CACHE pCache, PCACHE_OBJECT pObj);
DWORD	 GetFinalPos(DWORD dwPos, PSDD pdwOrigMapDir, PSDD pdwFinalMapDir, DWORD dwSectors);
BOOL	 FreeMoverBuffer(void);
BOOL	 DeleteFromRangeList(PYG_LIST pRangeList, PCACHE_OBJECT pObj);
DWORD	 Get_Cur_Thread_Priority(PTCB dwThreadHandle);
BOOL	 Set_Cur_Thread_Priority(PTCB dwThreadHandle, DWORD dwPri);
BOOL	 GetImgSectorGroup(PGET_SEC_GROUP pGetGroup, DWORD dwLogicalSec, DWORD dwSectors, PSEC_GROUP pGroup);
BOOL	 CopyRangeFromCache(PYG_LIST pObjList, PCACHE_OBJECT pObj, PDISK_CACHE pCache, PDISK_CACHE pFromCache);
BOOL	 ReadRangesFromCache(PYG_LIST pList, PDISK_CACHE pCache, PDISK_CACHE pFromCache, PBLKMOVER_DATA pBlkMoverData);
BOOL	 WriteRangesToCache(PYG_LIST pList, PDISK_CACHE pCache, PDISK_CACHE pToCache, PBLKMOVER_DATA pBlkMoverData);
BOOL	 TranslateRange(PCACHE_OBJECT pCacheObj, PYG_LIST pList);

BOOL	 SearchFromSysGroup(DWORD dwIoMode,	DWORD dwStart,
							DWORD dwSectors,PVOID pvBuffer,
							PBLKMOVER_DATA pBlkMoverData);
BOOL	 AddToFinalGroup(PM_RELOCATE_GROUP FinalGroup,int nCurPos);
BOOL	 AddToProtectGroup(DWORD dwStart,DWORD dwSize,PVOID pvBuffer,PBLKMOVER_DATA pBlkMoverData);
BOOL	 SearchFromGroup(DWORD dwIoMode,DWORD dwStart,DWORD dwSectors,PVOID pvBuffer,PBLKMOVER_DATA pBlkMoverData);
BOOL	 ReadFromGroup(DWORD dwIoMode,
						DWORD dwStart,
					    DWORD dwSectors,
						PVOID pvBuffer,
						BOOL  bRelocate,
						PBLKMOVER_DATA pBlkMoverData);
BOOL	 ReadWriteFromMovedGroup(DWORD dwIoMode,
					   DWORD dwRWStart,
					   DWORD dwSize,
					   PVOID pvBuffer,
					   PBLKMOVER_DATA pBlkMoverData);
PPM_PROTECT_GROUP GetProtectFreeGroup();


// Global variables
extern		    DRP theDRP;  // device registration packet
extern		    VMMLIST		g_lDCB;
extern		    PDCB		g_MoverDCB;
extern		    PDCB		g_SharkDCB;
extern		    DWORD		g_dwSharkDrive;
extern		    DWORD		g_dwMoverDrive;

extern		    VMM_SEMAPHORE	g_hSemSyncIO;
extern		    VMM_SEMAPHORE	g_hSemInternalReq;
extern		    DWORD		g_dwOrig1st;
extern		    DWORD		g_dwOrigLast;

extern		    DISK_CACHE		g_SharkCache;
extern		    DISK_CACHE		g_MoverCache;
extern		    PDISK_CACHE 	g_pSharkCache;
extern		    PDISK_CACHE 	g_pMoverCache;
extern		    PYG_LIST		g_pCacheObjNodePool;
extern		    PYG_LIST		g_pBMObjNodePool;
extern		    PYG_LIST		g_pListPool;
extern		    PYG_LIST		g_pImgSecAddrList;

#define LINSECTORNUM2INDEX_O(a)   ((a) - g_dwOrig1st)

#endif	//  BLKMOVER_VXD
extern		    PBLKMOVER_DATA	g_pMoverData;
extern		    PBLKMOVER_DATA	g_pSharkData;

#endif	//  BLKMOVER_H
