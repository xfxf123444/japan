
//
//  HighLoc.H
//
//  AUTHOR:
//	ZHOU LONG   (ZL)
//
//  Date:
//	18-Aug-1999
//

#ifndef HIGHLOC_H
#define HIGHLOC_H

#define     HighLoc_Major_Ver	    1
#define     HighLoc_Minor_Ver	    0

#ifndef     SECTOR_SIZE
#define     SECTOR_SIZE 	    512
#endif

#ifndef     TRAP
#ifdef	    DEBUG
#define     TRAP    _asm    int 1
#else
#define     TRAP
#endif
#endif




/*  File Attribute  */

#define _A_NORMAL		    0x00
#define _A_RDONLY		    0x01
#define _A_HIDDEN		    0x02
#define _A_SYSTEM		    0x04
#define _A_ARCH 		    0x20

/*  Access Mode */

#define OPEN_ACCESS_READONLY	    0x00
#define OPEN_ACCESS_WRITEONLY	    0x01
#define OPEN_ACCESS_READWRITE	    0x02
#define OPEN_ACCESS_RO_NOMODLASTACCESS	0x04h

/*  Sharing Mode    */

#define OPEN_SHARE_COMPATIBLE	    0x00


#define SECTORS_OF_CACHE	    0x100


/*  Service Function No */
#define FILELOCH_GetVersion		    0x01
#define FILELOCH_OpenHandle		    0x02
#define FILELOCH_CloseHandle		    0x03
#define FILELOCH_LogicSector2PhysSector     0x04
#define FILELOCH_LogicSector2PhysSectorFast 0x05
#define FILELOCH_Flush			    0x06
#define FILELOCH_OpenBuffer		    0x07
#define FILELOCH_CloseBuffer		    0x08
#define FILELOCH_CopyData		    0x09
#define FILELOCH_EnableDisable		    0x0A
#define FILELOCH_SetPhysSectors 	    0x0B




typedef struct DIOCParams *LPDIOC;


typedef struct tagFILE_LOC_HANDLE
    {
	DWORD	dwhSys; 		//  System's file handle
	PSZ	pszFileName;		//  Point to filename space
	WORD	wFileNameLen;		//  Length of file name
	WORD	wAttr;			//  File's attribute
	DWORD	dwVol;			//  Logic drive
	DWORD	dwFileSize;		//  original file size
	VMMLIST lMap;
    } FILE_LOC_HANDLE,*PFILE_LOC_HANDLE;


typedef struct tagLogic2Phys
    {
	DWORD	dwLogicNum;
	DWORD	dwPhysNum;
    } LOGIC2PHYS,*PLOGIC2PHYS;


#define SIGN_LEN	496
#define SIGN_HEAD_LEN	SECTOR_SIZE - SIGN_LEN

/*
typedef struct tagLOC_INFO
    {
	QWORD	qwLinSectorNum;
	DWORD	dwLogicSecNum;
	DWORD	dwReserve;
	CHAR	LocSign[SIGN_LEN];
    } LOC_INFO,*PLOC_INFO;
*/

/*
typedef struct tagLOC_INFO
{
    union
    {
	struct
	{
	    PQWORD  pqwPhysSector;
	    DWORD   dwLogicSecNum;
	    DWORD   dwReserve;
	    CHAR    LocSign[SIGN_LEN];
	};
	BYTE	Dum[SECTOR_SIZE];
    };
} LOC_INFO,*PLOC_INFO;
*/

DWORD	_stdcall HighLoc_GetVersion(DWORD,DWORD,LPDIOC);
DWORD	_stdcall HighLoc_OpenHandle(DWORD,DWORD,LPDIOC);
DWORD	_stdcall HighLoc_CloseHandle(DWORD,DWORD,LPDIOC);
DWORD	_stdcall HighLoc_LogicSector2PhysSector(DWORD,DWORD,LPDIOC);
DWORD	_stdcall HighLoc_LogicSector2PhysSectorFast(DWORD,DWORD,LPDIOC);
DWORD	_stdcall HighLoc_Flush(DWORD dwDDB,DWORD hDevice,LPDIOC lpDIOCParms);
DWORD	_stdcall HighLoc_OpenBuffer(DWORD dwDDB,DWORD hDevice,LPDIOC lpDIOCParms);
DWORD	_stdcall HighLoc_CloseBuffer(DWORD dwDDB,DWORD hDevice,LPDIOC lpDIOCParms);
DWORD	_stdcall HighLoc_CopyData(DWORD dwDDB,DWORD hDevice,LPDIOC lpDIOCParms);
DWORD	_stdcall HighLoc_EnableDisable(DWORD dwDDB,DWORD hDevice,LPDIOC lpDIOCParms);
DWORD	_stdcall HighLoc_SetPhysSectors(DWORD dwDDB,DWORD hDevice,LPDIOC lpDIOCParms);

DWORD	GetVersion(VOID);
BOOL	OnSysDynamicDeviceInit(VOID);
BOOL	OnSysDynamicDeviceExit(VOID);
HANDLE	OpenHandle(PSZ);
BOOL	CloseHandle(HANDLE);
BOOL	LogicSector2PhysSectorEx(HANDLE hLocater,DWORD dwLogicSectorNum,
				 PQWORD pqwPhysSectorNum, int nSectors);
BOOL	LogicSector2PhysSectorFast(HANDLE hLocater,DWORD dwLogicSectorNum,
				 PQWORD pqwPhysSectorArr, DWORD dwArrIndex, int nSectors);

BOOL	DynaLoadVxD(char *);
BOOL	DynaUnloadVxD(char *);

DWORD	HighLoc_Cleanup(void);

BOOL	IsEmptyList(VMMLIST list);
PLOGIC2PHYS SearchNode(VMMLIST list,DWORD dwLogicNum);
BOOL	FreeCacheList(PFILE_LOC_HANDLE pHandle);
void	GetCurTime(QWORD *pTick);
VOID	CalcTick(QWORD	tick_1, QWORD  tick_2, PQWORD ticks);


#endif	/*  HIGHLOC_H	*/
