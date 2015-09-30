// LowLoc.H
// Copyright (C) 1999 by YG Corp.
// All rights reserved
// Author:  ZL
// Date:    22-Nov-1999

#ifndef LOWLOC_H
#define LOWLOC_H

#include <basedef.h>
#include <vmm.h>
#include <debug.h>
#include "iosdcls.h"

#ifndef READ_OPERATION
#define READ_OPERATION		    TRUE
#endif

#ifndef WRITE_OPERATION
#define WRITE_OPERATION 	    FALSE
#endif

#ifdef	 DEBUG
    #define ASSERT(e)	if(!(e)){Debug_Printf("Assertion failure in "__FILE__",line %d:" #e "\r\n",__LINE__);\
			_asm	int 1	\
			}
#else
    #define ASSERT(e)
#endif

#ifndef MAKELONG
    #define MAKELONG(low, high) ((LONG)(((WORD)(low)) | (((DWORD)((WORD)(high))) << 16)))
#endif

#pragma warning(disable:4035)

#ifndef NAKED
    #define NAKED __declspec(naked)
#endif

#ifndef arraysize
    #define arraysize(p) (sizeof(p)/sizeof((p)[0]))
#endif

#define SECTOR_SIZE	    512


typedef struct	tagMonRdWt
    {
	WORD	wSignature;
	WORD	wCount;
	DWORD	dwLogicSecNum;
	QWORD	qwPhysSector;
    } MonRdWt,*PMonRdWt;

typedef struct	tagDskList
    {
	PDCB	pPhysDcb;
	VMMLIST lLocater;
    } DskList,*PDskList;



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

#define MonRdWt_Sig	    0x4C5A	//  "ZL"

#ifdef	 DEBUG
    #define ASSERT_MonRdWt(p)	if(!((((PMonRdWt)(p))->wSignature) == MonRdWt_Sig)) \
				    { \
					Debug_Printf("Assertion failure in "__FILE__",line %d:" #p "\r\n",__LINE__);\
					_asm	int 1	\
				    }
#else
    #define ASSERT_MonRdWt(p)
#endif


typedef struct DIOCParams   *LPDIOC;
typedef VMMLIST *PVMMLIST;
typedef QWORD	*PQWORD;


VOID	    OnRequest(PIOP iop);
VOID	    OnRequestComplete(PIOP iop);
VOID	    DoCallDown(PIOP iop);
VOID	    DoCallBack(PIOP iop);
VOID	    InsertCallBack(PIOP iop, VOID (*callback)(PIOP), ULONG refdata);
USHORT	    OnInitialize(PAEP_bi_init aep);
USHORT	    OnUninitialize(PAEP_bi_uninit aep);
USHORT	    OnBootComplete(PAEP_boot_done aep);
USHORT	    OnConfigDcb(PAEP_dcb_config aep);
USHORT	    OnUnconfigDcb(PAEP_dcb_unconfig aep);
PMonRdWt    SearchSector(VMMLIST,ULONG,ULONG);
BOOL	    SynReadWrite(BOOL bRDWT,
			 DWORD dwStartingSector,
			 DWORD dwSectors,
			 PVOID pvBuffer,
			 PDCB  dcb);

VMMLIST     CreateOrOpenList(PDCB);
VMMLIST     SearchList(PDCB);

//PIOP	  GetIOP(PDCB dcb);
//BOOL	  RetIOP(PIOP iop,PDCB dcb);
BOOL	ReadSector(
		    DWORD dwSectorLo,
		    DWORD dwSectorHi,
		    DWORD dwSectors,
		    PVOID pvBuf,
		    PIOP  iop,
		    PDCB  dcb
		  );

BOOL	WriteSector(
		    DWORD dwSectorLo,
		    DWORD dwSectorHi,
		    DWORD dwSectors,
		    PVOID pvBuf,
		    PIOP  iop,
		    PDCB  dcb
		   );
BOOL	IsLocatingSector(PLOC_INFO  pLoc);
BOOL	SynReadWrite(BOOL bRDWT,
		     DWORD dwStartingSector,
		     DWORD dwSectors,
		     PVOID pvBuffer,
		     PDCB  dcb);


#ifdef	DEBUG
VOID	ListSectorNum(VOID);
BOOL	DebugQueryHandler(VOID);
CHAR	GetDebugChar(VOID);
#endif





#endif	//  LOWLOC_H
