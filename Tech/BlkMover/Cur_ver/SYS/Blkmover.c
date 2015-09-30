// BLKMOVER.C
// Copyright (C) 2001 by YG Corp.
// All rights reserved
// This module contains functions managing low level sector buffers and virtualize
// the final (target) space. These functions provide strong support for both
// sector moving and original to current location lookup.
// Author:  ZhengNan Zhu
// Date:    12-Dec-2001

#ifdef		WIN_9X
#include    ".\9x\iosdcls.h"    // VMM and IOS headers
//#include    <debug.h>
#include    <vmm.h>
#include    <ifs.h>
#include    <vwin32.h>
#include    <vmmreg.h>
#include    <winerror.h>
#include    <vxdwraps.h>
#pragma hdrstop

#define     BLKMOVER_VXD

#include    ".\9x\ifsmgr.h"
#include    ".\9x\share.h"
#include    ".\9x\partype.h"
#include    "..\..\..\vportdrv\cur_ver\Export\9x\vportdrv.h"
#include    "..\..\..\VFS\cur_ver\Export\9x\virtu_fs.h"
#include    ".\9x\ERRORNO.H"
#include    "..\..\..\lib\hashsrch\cur_ver\export\list.h"
#include    "..\export\blkmover.h"
#include    ".\9x\blkmover.h"

#pragma VxD_LOCKED_DATA_SEG
#else
#include "ntddk.h"
#include "ntdddisk.h"
#include "stdarg.h"
#include "stdio.h"
#include <ntddvol.h>
#include <mountdev.h>
#include "wmistr.h"
#include "wmidata.h"
#include "wmiguid.h"
#include "wmilib.h"

#include ".\2000\iomon.h"
#include ".\2000\blkmover.h"
#endif //WIN_9X;

PBLKMOVER_DATA	    g_pMoverData = NULL;
PBLKMOVER_DATA	    g_pSharkData = NULL;
DWORD				g_dwSharkDrive	= 0;
DWORD				g_dwMoverDrive	= 0;


#ifdef WIN_9X
VMMLIST 			g_lDCB				= 0;
PDCB				g_MoverDCB			= (PDCB)NULL;
PDCB				g_SharkDCB			= (PDCB)NULL;
VMM_SEMAPHORE	    g_hSemSyncIO		= 0;
VMM_SEMAPHORE	    g_hSemInternalReq	= 0;
#else
PDRIVE_LAYOUT_INFORMATION	g_partitionInfo = NULL;
BOOL						g_bBlkMovSucc;
BOOL						g_bPartitionSet;
KSPIN_LOCK					g_IrpSpinLock;
#endif

MOVING_GROUP	g_MovingGroup;
DWORD			g_dwMinSec,g_dwMaxSec;
DWORD			g_dwRePointer;
DWORD			g_dwReadEnd;
BOOL			g_bReLocate	= FALSE;

DWORD		    g_dwOldPri;
DWORD		    g_dwOrig1st 		= 0;
DWORD		    g_dwOrigLast		= 0;
PYG_LIST	    g_pImgSecAddrList	= NULL;

DISK_CACHE	    g_SharkCache;
DISK_CACHE	    g_MoverCache;
PDISK_CACHE	    g_pSharkCache			= NULL;
PDISK_CACHE	    g_pMoverCache			= NULL;
PYG_LIST	    g_pCacheObjNodePool		= NULL;
PYG_LIST	    g_pBMObjNodePool		= NULL;
PYG_LIST	    g_pListPool 			= NULL;
PYG_LIST		g_pDataRegionLRUList	= NULL;
PRM_SEC_GROUP_TABLE		g_pDataRegionListHead =NULL;
PRM_SEC_GROUP_TABLE		g_pDataRegionListTail =NULL;
DWORD					g_dwNumDataRegionLoaded = 0;
PM_RELOCATE_GROUP		g_FinalGroup[32];
PPM_PROTECT_GROUP		g_pProtectFreeListOld = NULL;
PPM_PROTECT_GROUP		g_pProtectFreeList = NULL;
PPM_PROTECT_GROUP		g_pProtectList = NULL;
DWORD					g_dwFreeStart   = 0;
DWORD					g_dwFreeSize	= 0;
PPM_FREE_SPACE_GROUP	g_FreeSpaceList = NULL;
// The following variables are used for syncronizations

DWORD				g_dwDataStart,g_dwDataRecNum,g_dwDataRecPages;
DWORD				g_dwSysStart,g_dwSysRecNum,g_dwSysRecPages;
DWORD				g_dwMovedSecs;

PM_SEC_GROUP_BUF	g_SecGroupBuf;
DWORD				g_dwMovedRecNum;
PBYTE				g_pCompInitBuf;

BOOL				g_bEnableProtect = FALSE;
DWORD				g_dwDataStart;


#ifdef WIN_9X
#pragma VxD_LOCKED_CODE_SEG
BOOL	SetMoverPara(PBLK_MOVER_PARAM  pPara)
#else
NTSTATUS	SetMoverPara(PBLK_MOVER_PARAM  pPara)
#endif
{
	int i;
#ifdef WIN_9X
    PDCB			dcb;
    PBLKMOVER_DATA  pMoverData;
#endif 

    g_dwMoverDrive		= (DWORD)pPara->btDrive;
	g_dwSharkDrive		= (DWORD)pPara->btSharkDrive;
    g_dwDataStart		= pPara->dwDataStartSec;
    g_dwDataRecNum		= pPara->dwDataRecNum;
    g_dwDataRecPages	= pPara->dwDataRecPages;
    g_dwSysStart		= pPara->dwSysStartSec;
    g_dwSysRecNum		= pPara->dwSysRecNum;
    g_dwSysRecPages		= pPara->dwSysRecPages;
    g_dwOrig1st 		= pPara->dwOrig1st;
    g_dwOrigLast		= pPara->dwOrigLast;
    g_dwFreeStart		= pPara->dwFreeStart;
    g_dwFreeSize		= pPara->dwFreeSize;
    
	g_pProtectList		= NULL;
	g_pProtectFreeList	= NULL;
	g_pProtectFreeListOld = NULL;
    g_dwMovedSecs		= 0;
	g_dwMovedRecNum 	= 0;
	g_bEnableProtect	= FALSE;
    memset(&g_SecGroupBuf,0,sizeof(PM_SEC_GROUP_BUF));

#ifdef WIN_9X
    g_MoverDCB = NULL;
    g_SharkDCB = NULL;
    for(
	dcb = IspGetFirstNextDcb(NULL,DCB_type_disk);
	dcb && (!g_MoverDCB || !g_SharkDCB);
	dcb = IspGetFirstNextDcb(dcb,DCB_type_disk)
       )
    {
		if(dcb->DCB_cmn.DCB_device_flags & DCB_DEV_LOGICAL) //	Is logical drive ?
		{
		    if(dcb->DCB_cmn.DCB_drive_lttr_equiv == pPara->btDrive)
				g_MoverDCB = (PDCB)dcb->DCB_cmn.DCB_physical_dcb;
		    if(dcb->DCB_cmn.DCB_drive_lttr_equiv == pPara->btSharkDrive)
				g_SharkDCB = (PDCB)dcb->DCB_cmn.DCB_physical_dcb;
		}
    }
    ASSERT(g_SharkDCB && g_MoverDCB);

    g_pMoverData = NULL;
    g_pSharkData = NULL;
    for(pMoverData = (PBLKMOVER_DATA)List_Get_First(g_lDCB);
	pMoverData && (!g_pMoverData || !g_pSharkData);
	pMoverData =  (PBLKMOVER_DATA)List_Get_Next(g_lDCB,pMoverData)
       )
    {
		if (pMoverData->dcb == g_MoverDCB)
		    g_pMoverData = pMoverData;
		if (pMoverData->dcb == g_SharkDCB)
		    g_pSharkData = pMoverData;
	}

    ASSERT(g_pSharkData && g_pMoverData);
    return  TRUE;
#else
	g_pProtectFreeList	= (PPM_PROTECT_GROUP)ExAllocatePool(NonPagedPool,sizeof(PM_PROTECT_GROUP)*PROTECT_GROUP_BUFFER_PAGE*SECTORS_PER_PAGE*SECTOR_SIZE);
	if(!g_pProtectFreeList) return FALSE;
	g_pProtectFreeListOld = g_pProtectFreeList;
	memset(g_pProtectFreeList,0,sizeof(PM_PROTECT_GROUP)*PROTECT_GROUP_BUFFER_PAGE*SECTORS_PER_PAGE*SECTOR_SIZE);
	for(i=0;i<PROTECT_GROUP_BUFFER_PAGE*SECTORS_PER_PAGE*SECTOR_SIZE;i++)
	{
		if(i != PROTECT_GROUP_BUFFER_PAGE*SECTORS_PER_PAGE*SECTOR_SIZE-1)
			(g_pProtectFreeList + i)->pNext = g_pProtectFreeList + i + 1;
		if(i)
			(g_pProtectFreeList + i)->pPrev = g_pProtectFreeList + i - 1;
	}

	g_SecGroupBuf.pGroupsBuff = (PPM_SEC_GROUP)ExAllocatePool(NonPagedPool,DATA_PAGE_SIZE*SECTOR_SIZE*BUFFER_OF_PAGES );
	return STATUS_SUCCESS;
#endif
}   //	SetMoverPara

#ifdef WIN_9X
BOOL	EnableOrDisable(BOOL bEnableOrDisable)
{
	g_pMoverData->bWorking =  bEnableOrDisable;
	if(!bEnableOrDisable)
	{
		if(g_pMoverData)_PageFree(g_pMoverData,0);
		if(g_pProtectFreeListOld)_PageFree(g_pProtectFreeListOld,0);
		if(g_SecGroupBuf.pGroupsBuff)_PageFree(g_SecGroupBuf.pGroupsBuff,0);
		g_pMoverData = NULL;
		g_pProtectFreeListOld = NULL;
		g_SecGroupBuf.pGroupsBuff = NULL;
	}
	return TRUE;
}

#else
NTSTATUS	EnableOrDisable(BOOLEAN bEnableOrDisable,PDEVICE_OBJECT pDeviceObject)
{
	if(bEnableOrDisable)
	{
		g_dwOrigLast = 0;
		g_dwOrig1st	 = 0;
		g_pMoverData = (PBLKMOVER_DATA)ExAllocatePool(NonPagedPool,sizeof(BLKMOVER_DATA));
		if(g_pMoverData) 
		{
			RtlZeroMemory(g_pMoverData, sizeof(BLKMOVER_DATA));
			g_pMoverData->DeviceObject = pDeviceObject;
		}
	}
	else
	{
		g_pMoverData->bWorking = FALSE;
		g_dwMovedRecNum		   = 0;
		g_bEnableProtect	   = FALSE;
		ExFreePool(g_pMoverData);
		if (g_pProtectFreeListOld)
			ExFreePool(g_pProtectFreeListOld);
		if (g_SecGroupBuf.pGroupsBuff)
			ExFreePool(g_SecGroupBuf.pGroupsBuff);
		DelFreeSpaceList();
		g_pMoverData = NULL;
		g_pProtectFreeListOld = NULL;
		g_SecGroupBuf.pGroupsBuff = NULL;
	}
	return STATUS_SUCCESS;
}   //	EnableOrDisable
#endif

#ifdef WIN_9X
BOOL	EnableWriteProtect()
{
	Wait_Semaphore(g_hSemSyncIO,BLOCK_THREAD_IDLE);
	g_bEnableProtect = TRUE;
	Signal_Semaphore(g_hSemSyncIO);
	return TRUE;
}

BOOL	SetMoingGroupCur(BOOL bFront,DWORD dwSStart,DWORD dwTStart,DWORD dwSize,DWORD dwRecNum)
{
	Wait_Semaphore(g_hSemSyncIO,BLOCK_THREAD_IDLE);
	g_MovingGroup.bFront		= bFront;
	g_MovingGroup.dwSStart		= dwSStart;
	g_MovingGroup.dwTStart		= dwTStart;
	g_MovingGroup.dwSize		= dwSize;
	g_MovingGroup.dwMovedSize	= 0;
	g_dwMovedRecNum				= dwRecNum;
	g_pMoverData->bWorking		= TRUE;
	Signal_Semaphore(g_hSemSyncIO);
	return TRUE;
}

BOOL	SetWriteResult(BOOL bSys,DWORD dwStartSec,DWORD dwSize)
{
	Wait_Semaphore(g_hSemSyncIO,BLOCK_THREAD_IDLE);
	if(!bSys && g_dwMovedRecNum)
	{
		g_MovingGroup.dwMovedSize += dwSize;
	}
	if(g_bReLocate && bSys) 
		g_dwRePointer = dwStartSec + dwSize;
	Signal_Semaphore(g_hSemSyncIO);		
	return TRUE;
}

BOOL	Preface(VOID)
{
	int i;
	g_hSemInternalReq = Create_Semaphore(8);

    if(!g_hSemInternalReq)
    {
		return	FALSE;
    }

	g_SecGroupBuf.pGroupsBuff = (PPM_SEC_GROUP)_PageAllocate((DATA_PAGE_SIZE*BUFFER_OF_PAGES)/8,PG_SYS,NULL,0,0,0,NULL,PAGEFIXED);
	if(!g_SecGroupBuf.pGroupsBuff)
		return FALSE;
	else
		return TRUE;
}   //	Preface
#endif

#ifdef WIN_9X
BOOL	PostScript(BOOL bReboot)
#else
NTSTATUS	PostScript(BOOL bReboot)
#endif
{
    PBYTE	pSector;
    DWORD	dwHashIndex;
    DWORD	dwSectorNum;
    PBYTE	pSectors;
    BOOL	bSucc;
    BOOL	bRetVal = TRUE;
    int 	nTotalSectors;
    int 	n;

#ifdef WIN_9X
    if (bReboot)
      Reboot();
    Wait_Semaphore(g_hSemSyncIO,BLOCK_THREAD_IDLE);
    EnableOrDisable(FALSE);
    if (g_hSemInternalReq)
		Destroy_Semaphore(g_hSemInternalReq);
#else
    if (g_MoverCache.pCache)
		ExFreePool(g_MoverCache.pCache);
	
	if(g_pMoverData)
	{
		ExFreePool(g_pMoverData);
		g_pMoverData = NULL;
	}
	if(g_pSharkData)
	{
		ExFreePool(g_pSharkData);
		g_pSharkData	= NULL;
	}
	if(g_partitionInfo)
	{
		ExFreePool(g_partitionInfo);
		g_partitionInfo = NULL;
	}
	g_bBlkMovSucc	 = FALSE;
	g_bPartitionSet  = FALSE;
#endif

#ifdef WIN_9X
    Signal_Semaphore(g_hSemSyncIO);
    return  bRetVal;
#else
	return STATUS_SUCCESS;
#endif
}   //	PostScript


BOOL FreeMoverBuffer(void)
{
    DWORD   dwMoverCacheSize;
    DWORD   dwSharkCacheSize;
    DWORD   dwCacheSectors;
    DWORD   dwCachePages;
    PBYTE   pCacheBuf;

    if (g_MoverCache.pCache)
#ifdef WIN_9X
	_PageFree(g_MoverCache.pCache,0);
#else
	ExFreePool(g_MoverCache.pCache);
#endif

    dwMoverCacheSize = MAX_ASYNC_SECTORS*SECTOR_SIZE;
    dwSharkCacheSize = MAX_ASYNC_SECTORS*sizeof(SECMAP);

    dwCacheSectors = (dwSharkCacheSize+dwMoverCacheSize+SECTOR_SIZE-1)/SECTOR_SIZE;
    dwCachePages = (dwCacheSectors+7)/8;
#ifdef WIN_9X
    pCacheBuf = (PBYTE)_PageAllocate(dwCachePages,PG_SYS,NULL,0,0,0,NULL,PAGEFIXED);
#else
	pCacheBuf = (PBYTE)ExAllocatePool(NonPagedPool,dwCachePages*SECTOR_SIZE*8);
#endif

    g_pMoverCache = &g_MoverCache;
    g_pSharkCache = &g_SharkCache;
    return TRUE;
}

VOID ForceReboot()
{
	//BYTE	Sector[SECTOR_SIZE];
	_asm int 8
	//Reboot();
}

#ifndef WIN_9X
NTSTATUS   SearchGroups(UCHAR MajorFunction,DWORD dwStart,DWORD dwSectors,PVOID pvBuffer,PDEVICE_OBJECT DeviceObject)
{
    DWORD					dwStartSec;
    DWORD					dwEndSec;
    DWORD					dwCurPos;
    DWORD					dwStop;
    DWORD					dwSecs;
	NTSTATUS				status;
	PRM_SEC_GROUP			pGroup;
	RM_HANDLE				hHandle;
	DWORD					dwSize;
//	RM_LOG_INFO				LogInfo;

//	RtlZeroMemory(&LogInfo,sizeof(RM_LOG_INFO));

	pGroup = GetFirstSecsGroup(DeviceObject,&hHandle,dwStart,dwSectors,&status);
	if(status != STATUS_SUCCESS)
		return status;
	if (pGroup)
	{
	    dwStartSec = dwStart;
	    dwEndSec   = dwStart+dwSectors;
	    dwCurPos   = dwStartSec;

		while(pGroup && dwCurPos < dwEndSec && status == STATUS_SUCCESS)
		{
		    if (dwCurPos < pGroup->dwSStartSec)
			{
				// Do the first part
				status = STATUS_SUCCESS;
				dwStop = MIN(dwEndSec,pGroup->dwSStartSec);
				dwSecs = dwStop-dwCurPos;
				if(dwSecs)
				{
					status = SyncReadWriteSec(DeviceObject,dwCurPos,dwSecs,pvBuffer,MajorFunction);
					ASSERT(status == STATUS_SUCCESS);
					(PBYTE)pvBuffer += dwSecs*SECTOR_SIZE;
					dwCurPos += dwSecs;
				}
			}

		    if (dwCurPos >= pGroup->dwSStartSec && dwCurPos < pGroup->dwSStartSec+pGroup->dwLength)
			{
				// Do the second part
				status = STATUS_SUCCESS;
				dwStop = MIN(dwEndSec,pGroup->dwSStartSec+pGroup->dwLength);
				dwSecs = dwStop-dwCurPos;
				if (dwSecs)
				{
					status = SyncReadWriteSec(DeviceObject,pGroup->dwTStartSec+(dwCurPos-pGroup->dwSStartSec),
								dwSecs,pvBuffer,MajorFunction);
					ASSERT(status == STATUS_SUCCESS);
					if(status != STATUS_SUCCESS)
						return status;
					(PBYTE)pvBuffer += dwSecs*SECTOR_SIZE;
					dwCurPos += dwSecs;
				}
			}
			if (dwCurPos < dwEndSec)
				pGroup = GetNextSecsGroup(DeviceObject,&hHandle,dwCurPos,dwEndSec-dwCurPos,&status);
			ASSERT(status == STATUS_SUCCESS);
		}
		if (dwCurPos < dwEndSec)
		{
			// Do the final part
			status = STATUS_SUCCESS;
			dwStop = dwEndSec;
			dwSecs = dwStop-dwCurPos;

			if(dwSecs)
				status = SyncReadWriteSec(DeviceObject,dwCurPos,dwSecs,pvBuffer,MajorFunction);
		}
	}
	ASSERT(status == STATUS_SUCCESS);
    return  status;
}
PRM_SEC_GROUP GetFirstSecsGroup(PDEVICE_OBJECT DeviceObject,RM_HANDLE *Handle, 
								DWORD dwStartSec,DWORD dwSecs,NTSTATUS *status)
{
	PRM_SEC_GROUP_TABLE		pDataRegion;
	PRM_SEC_GROUP_TABLE		pReUse;

	*status = STATUS_SUCCESS;

	pDataRegion = g_pDataRegionListHead;
	while(pDataRegion)
	{
		if(dwStartSec >= pDataRegion->dwDataRegionEnd)
		{
			pDataRegion = pDataRegion->pNext;
		}
		else
		{
			if(pDataRegion->pGroupsBuff == NULL)
			{
				pReUse = (PRM_SEC_GROUP_TABLE)YG_List_Get_First(g_pDataRegionLRUList);

				pDataRegion->pGroupsBuff = pReUse->pGroupsBuff;
				*status = SyncReadWriteSec(DeviceObject,pDataRegion->dwStartSec,
								  SEC_GROUP_BUFFER_SIZE,pDataRegion->pGroupsBuff,IRP_MJ_READ);
				pReUse->pGroupsBuff = NULL;
				YG_List_Remove(g_pDataRegionLRUList,pReUse);
				YG_List_Attach_Tail(g_pDataRegionLRUList,pDataRegion);
			}
			Handle->pCurNode	  = pDataRegion;
			Handle->dwCurGroupNum = 0;
			return pDataRegion->pGroupsBuff;
		}
	}
	return NULL;
}

PRM_SEC_GROUP GetNextSecsGroup(PDEVICE_OBJECT DeviceObject,RM_HANDLE *Handle, 
								DWORD dwStartSec,DWORD dwSecs,NTSTATUS *status)
{
	PRM_SEC_GROUP_TABLE		pDataRegion;
	PRM_SEC_GROUP_TABLE		pReUse;
	DWORD					dwCurRecNum;
	PRM_SEC_GROUP			pCurGroup;

	*status = STATUS_SUCCESS;

	dwCurRecNum = Handle->dwCurGroupNum+1;
	if(dwCurRecNum < Handle->pCurNode->dwRecNum)
	{
		pCurGroup = (PRM_SEC_GROUP)(Handle->pCurNode->pGroupsBuff+dwCurRecNum);
		Handle->dwCurGroupNum = dwCurRecNum;
		return pCurGroup;
	}

	if(!Handle->pCurNode->pNext)
		return NULL;

	pDataRegion = Handle->pCurNode->pNext;
	while(pDataRegion)
	{
		if(dwStartSec >= pDataRegion->dwDataRegionEnd)
		{
			pDataRegion = pDataRegion->pNext;
		}
		else
		{
			if(pDataRegion->pGroupsBuff == NULL)
			{
				pReUse = (PRM_SEC_GROUP_TABLE)YG_List_Get_First(g_pDataRegionLRUList);

				pDataRegion->pGroupsBuff = pReUse->pGroupsBuff;
				*status = SyncReadWriteSec(DeviceObject,pDataRegion->dwStartSec,
								  SEC_GROUP_BUFFER_SIZE,pDataRegion->pGroupsBuff,IRP_MJ_READ);
				pReUse->pGroupsBuff = NULL;
				YG_List_Remove(g_pDataRegionLRUList,pReUse);
				YG_List_Attach_Tail(g_pDataRegionLRUList,pDataRegion);
			}
			Handle->pCurNode	  = pDataRegion;
			Handle->dwCurGroupNum = 0;
			return pDataRegion->pGroupsBuff;
		}
	}
	return NULL;
}

BOOL  RMSetGroup(DWORD dwStart,DWORD dwRecNums,PDEVICE_OBJECT DeviceObject)
{
	NTSTATUS			status;
	PRM_SEC_GROUP_TABLE pDataRegion;
	PRM_SEC_GROUP		pGroupBuff,pGroup;

	pDataRegion = (PRM_SEC_GROUP_TABLE)YG_List_Allocate(g_pDataRegionLRUList);
	if(pDataRegion)
	{
		pGroupBuff	  = (PRM_SEC_GROUP)ExAllocatePool(NonPagedPool,SEC_GROUP_BUFFER_SIZE*SECTOR_SIZE);
		if(pGroupBuff)
		{
			status = SyncReadWriteSec(DeviceObject,dwStart,
						SEC_GROUP_BUFFER_SIZE,pGroupBuff,IRP_MJ_READ);
			if(status != STATUS_SUCCESS) return FALSE;
		}
		memset(pDataRegion,0,sizeof(RM_SEC_GROUP_TABLE));
		pDataRegion->dwStartSec = dwStart;
		pDataRegion->dwRecNum   = dwRecNums;
		pDataRegion->pGroupsBuff= pGroupBuff;
		pDataRegion->dwDataRegionStart = pGroupBuff->dwSStartSec;
		pGroup = (PRM_SEC_GROUP)(pGroupBuff+dwRecNums-1);
		pDataRegion->dwDataRegionEnd = pGroup->dwSStartSec + pGroup->dwLength;
		if(g_dwNumDataRegionLoaded >= MAX_DATA_REGION_LOADED)
		{
			ExFreePool(pGroupBuff);
			pDataRegion->pGroupsBuff = NULL;
		}
		if(g_pDataRegionListTail)
			g_pDataRegionListTail->pNext = pDataRegion;
						
		g_pDataRegionListTail = pDataRegion;
		if(g_pDataRegionListHead == NULL)
		g_pDataRegionListHead = pDataRegion;

		if(pDataRegion->pGroupsBuff)
			YG_List_Attach_Tail(g_pDataRegionLRUList,pDataRegion);
		g_dwNumDataRegionLoaded ++;
	}
	else
		return FALSE;

	return TRUE;
}

NTSTATUS   DoReLocate(UCHAR MajorFunction,DWORD dwStart,
					  DWORD dwSectors,PVOID pvBuffer,
					  PDEVICE_OBJECT DeviceObject)
{
    DWORD				dwStartSec;
    DWORD				dwEndSec;
	DWORD				dwCurPos;
	DWORD				dwStop,dwSecs;
	NTSTATUS			status;

    dwStartSec = dwStart;
    dwEndSec   = dwStart+dwSectors;
	status	   = STATUS_SUCCESS;

	ASSERT(dwSectors);
	if(dwSectors)
	{
		if(dwEndSec <= g_dwMinSec || dwStartSec >= g_dwMaxSec)
			status = SyncReadWriteSec(DeviceObject,dwStartSec,dwSectors,pvBuffer,MajorFunction);
		else
		{
			if(g_dwRePointer == g_dwMaxSec)
			{
				status = SearchGroups(MajorFunction,dwStartSec,dwSectors,pvBuffer,DeviceObject);
			}
			else
			{
				status = SyncReadWriteSec(DeviceObject,dwStartSec,dwSectors,pvBuffer,MajorFunction);
				if(MajorFunction == IRP_MJ_WRITE)
					status = SearchGroups(MajorFunction,dwStartSec,dwSectors,pvBuffer,DeviceObject);
			}
		}
	}
    return  status;
}

NTSTATUS DoLinearIo(UCHAR MajorFunction,
		DWORD dwStart,
		DWORD dwSectors,
		PVOID pvBuffer,
		PBLKMOVER_DATA pBlkMoverData)
{
	DWORD	i;
    DWORD   dwStartSec;
    DWORD   dwEndSec;
    DWORD   dwSecs;
    BOOL    bSucc;
	BOOL	bReadSys;
	NTSTATUS status = STATUS_SUCCESS;

	status = SearchFromSysGroup(MajorFunction,dwStart,dwSectors,pvBuffer,pBlkMoverData);
	ASSERT(status == STATUS_SUCCESS);
    return  status;
}   //	DoLinear

NTSTATUS SearchFromSysGroup(UCHAR MajorFunction,
						DWORD dwStart,
						DWORD dwSectors,
						PVOID pvBuffer,
						PBLKMOVER_DATA pBlkMoverData)
						//BOOL  *pbReadSys)
{
    DWORD					dwStartSec;
    DWORD					dwEndSec,dwCurPos;
    DWORD					dwSecs,dwStop;
	DWORD					i,k;
	DWORD					dwRecStart;
	PPM_SEC_GROUP			pGroup = NULL;
	NTSTATUS 				status = STATUS_SUCCESS;
	
	dwStartSec 				= dwStart;
	dwEndSec 				= dwStart+dwSectors;
	dwCurPos 				= dwStart;
	
	//*pbReadSys = FALSE;
	for(i=0;i<g_dwSysRecPages;i+=BUFFER_OF_PAGES)
	{
		status = SyncReadWriteSec(pBlkMoverData->DeviceObject,
									g_dwSysStart+i*DATA_PAGE_SIZE,
									DATA_PAGE_SIZE*BUFFER_OF_PAGES,
									g_SecGroupBuf.pGroupsBuff,
									IRP_MJ_READ);
		if(status != STATUS_SUCCESS) break;
		
		dwRecStart = i* ARRY_SIZE*BUFFER_OF_PAGES;
		for(k=0;k<ARRY_SIZE*BUFFER_OF_PAGES;k++)
		{
			if((k + dwRecStart) < g_dwSysRecNum)
			{
				pGroup = g_SecGroupBuf.pGroupsBuff + k;
				if(pGroup->dwSStartSec > dwCurPos)
				{
					dwStop  = MIN(dwEndSec,pGroup->dwSStartSec);
					dwSecs  = dwStop-dwCurPos;
					status  = SearchFromGroup(MajorFunction,
											dwCurPos,dwEndSec-dwCurPos,
											pvBuffer,pBlkMoverData);
					ASSERT(status == STATUS_SUCCESS);
					if(status != STATUS_SUCCESS)
						return status;
					//Restore Group Buffer;
					status = SyncReadWriteSec(pBlkMoverData->DeviceObject,
												g_dwSysStart+i*DATA_PAGE_SIZE,
												DATA_PAGE_SIZE*BUFFER_OF_PAGES,
												g_SecGroupBuf.pGroupsBuff,
												IRP_MJ_READ);
					if(status != STATUS_SUCCESS)
						return status;
						
					(PBYTE)pvBuffer += dwSecs*SECTOR_SIZE;
					dwCurPos 		+= dwSecs;
					if(dwCurPos == dwEndSec) break;
				}

				if (dwCurPos >= pGroup->dwSStartSec 
					&& dwCurPos < pGroup->dwSStartSec+pGroup->dwSize)
				{
					dwStop = MIN(dwEndSec,pGroup->dwSStartSec+pGroup->dwSize);
					dwSecs = dwStop-dwCurPos;
					status = SyncReadWriteSec(pBlkMoverData->DeviceObject,
											   dwCurPos-pGroup->dwSStartSec+pGroup->dwTStartSec,
											   dwSecs,
											   pvBuffer,
											   MajorFunction);
					ASSERT(status == STATUS_SUCCESS);
					if(status != STATUS_SUCCESS) 
						return status;
					(PBYTE)pvBuffer += dwSecs*SECTOR_SIZE;
					dwCurPos 		+= dwSecs;
					//*pbReadSys 		= TRUE;
					if(dwCurPos == dwEndSec)
						break;
				}
			}
			else
				break;
		}
	}
	if(dwCurPos < dwEndSec)
	{
		status = SearchFromGroup(MajorFunction,dwCurPos,dwEndSec-dwCurPos,pvBuffer,pBlkMoverData);
		ASSERT(status == STATUS_SUCCESS);
	}
	return status;
}

NTSTATUS SearchFromGroup(UCHAR MajorFunction,
					DWORD dwStart,
					DWORD dwSectors,
					PVOID pvBuffer,
					PBLKMOVER_DATA pBlkMoverData)
{
	BOOL					bBreak = FALSE;
	int						nCurPos = 0;
	DWORD					i,k;
	DWORD					dwRecStart,dwSecs,dwStop;
	DWORD					dwStartSec,dwEndSec,dwCurPos;
	PPM_SEC_GROUP			pGroup = NULL;
	PM_RELOCATE_GROUP		FinalGroup;
	NTSTATUS 				status = STATUS_SUCCESS;
	
	ASSERT(dwStart >= g_dwDataStart+g_dwDataRecPages*DATA_PAGE_SIZE*BUFFER_OF_PAGES ||
			dwStart+dwSectors <= g_dwDataStart);

	for(i=0;i<g_dwDataRecPages;i+=BUFFER_OF_PAGES)
	{
		status = SyncReadWriteSec(pBlkMoverData->DeviceObject,
								   g_dwDataStart+i*DATA_PAGE_SIZE,
								   DATA_PAGE_SIZE*BUFFER_OF_PAGES,
								   g_SecGroupBuf.pGroupsBuff,
								   IRP_MJ_READ);
		if(status != STATUS_SUCCESS) break;
		
		dwRecStart = i* ARRY_SIZE*BUFFER_OF_PAGES;
		for(k=0;k<ARRY_SIZE*BUFFER_OF_PAGES;k++)
		{
			if((k + dwRecStart) < g_dwMovedRecNum)
			{
				pGroup = g_SecGroupBuf.pGroupsBuff + k;
				if(!(dwStart >= pGroup->dwOriginal+pGroup->dwSize ||
					dwStart+dwSectors <= pGroup->dwOriginal))
				{
					FinalGroup.dwSize 		= pGroup->dwSize;
					FinalGroup.dwFinal		= pGroup->dwTStartSec;
					FinalGroup.dwOriginal 	= pGroup->dwOriginal;
					if(!AddToFinalGroup(FinalGroup,nCurPos))
						return STATUS_PENDING;

					nCurPos ++;
				}
			}
			else
			{
				bBreak = TRUE;
				break;
			}
		}
		if(bBreak) break;
	}

    dwStartSec 	= dwStart;
    dwEndSec 	= dwStart+dwSectors;
    dwCurPos 	= dwStartSec;
    
	for(i=0;i<(DWORD)nCurPos;i++)
	{
		if (dwCurPos < g_FinalGroup[i].dwOriginal)
		{
			// Do the first part
			dwStop = MIN(dwEndSec,g_FinalGroup[i].dwOriginal);
			dwSecs = dwStop-dwCurPos;
			status = ReadWriteFromMovedGroup(MajorFunction,dwCurPos,dwSecs,pvBuffer,pBlkMoverData);
			ASSERT(status == STATUS_SUCCESS);
			if(status != STATUS_SUCCESS) 
				return status;
			(PBYTE)pvBuffer += dwSecs*SECTOR_SIZE;
			dwCurPos 		+= dwSecs;
		}

		if (dwCurPos >= g_FinalGroup[i].dwOriginal 
			&& dwCurPos < g_FinalGroup[i].dwOriginal + g_FinalGroup[i].dwSize)
		{
			// Do the second part
			dwStop = MIN(dwEndSec,g_FinalGroup[i].dwOriginal + g_FinalGroup[i].dwSize);
			dwSecs = dwStop-dwCurPos;
			status = ReadWriteFromMovedGroup(MajorFunction,
						dwCurPos - g_FinalGroup[i].dwOriginal + g_FinalGroup[i].dwFinal,
						dwSecs,pvBuffer,pBlkMoverData);
			ASSERT(status == STATUS_SUCCESS);
			if(status != STATUS_SUCCESS) 
			return status;
			(PBYTE)pvBuffer += dwSecs*SECTOR_SIZE;
			dwCurPos 		+= dwSecs;
		}
	}

	if(dwCurPos < dwEndSec)
	{
		dwSecs = dwEndSec-dwCurPos;
		status = ReadWriteFromMovedGroup(MajorFunction,dwCurPos,dwSecs,pvBuffer,pBlkMoverData);
		ASSERT(status == STATUS_SUCCESS);
	}
	return status;
}

NTSTATUS ReadWriteFromMovedGroup(UCHAR MajorFunction,
					   DWORD dwRWStart,
					   DWORD dwSize,
					   PVOID pvBuffer,
					   PBLKMOVER_DATA pBlkMoverData)
{
	NTSTATUS			status = STATUS_SUCCESS;
	DWORD				dwStart,dwCur,dwEnd,dwStop,dwSecs;
	DWORD				dwMovedStart,dwMovedSize;

	if(g_dwMovedRecNum)
	{
		dwStart		= dwRWStart;
		dwCur		= dwRWStart;
		dwEnd		= dwRWStart+dwSize;
		dwMovedSize = g_MovingGroup.dwSize;

		if(!(dwRWStart >= g_MovingGroup.dwTStart + g_MovingGroup.dwSize || 
			dwRWStart + dwSize <= g_MovingGroup.dwTStart))
		{
			if(dwRWStart >= g_MovingGroup.dwTStart && dwRWStart+dwSize <= g_MovingGroup.dwTStart + g_MovingGroup.dwSize)
			{
				g_MovingGroup.dwTStart = g_MovingGroup.dwTStart;
			}
			else
			{
				g_MovingGroup.dwTStart = g_MovingGroup.dwTStart;
			}

			if(g_MovingGroup.bFront)
			{
				dwMovedStart = g_MovingGroup.dwTStart;
				//dwFinalStart = g_MovingGroup.dwTStart;
			}
			else
			{
				dwMovedStart = g_MovingGroup.dwTStart + g_MovingGroup.dwSize - g_MovingGroup.dwMovedSize;
				//dwFinalStart = g_MovingGroup.dwTStart + g_MovingGroup.dwSize - g_MovingGroup.dwMovedSize;
			}

			if(dwCur < dwMovedStart)
			{
				dwStop = MIN(dwEnd , dwMovedStart);
				dwSecs = dwStop - dwCur;
				status = SyncReadWriteSec(pBlkMoverData->DeviceObject,dwCur-g_MovingGroup.dwTStart+g_MovingGroup.dwSStart ,
											dwSecs,pvBuffer,MajorFunction);
				(PBYTE)pvBuffer += dwSecs*SECTOR_SIZE;
				dwCur	 		+= dwSecs;
				if(dwCur == dwEnd) return status;
			}

			if (dwCur >= dwMovedStart
				&& dwCur < dwMovedStart + dwMovedSize)
			{
				dwStop = MIN(dwEnd,dwMovedStart + dwMovedSize);
				dwSecs = dwStop-dwCur;
				status = SyncReadWriteSec(pBlkMoverData->DeviceObject,
											dwCur ,
											dwSecs,pvBuffer,MajorFunction);
				(PBYTE)pvBuffer += dwSecs*SECTOR_SIZE;
				dwCur		+= dwSecs;
				if(dwCur == dwEnd) return status;
			}

			if(dwCur < dwEnd)
			{
				dwSecs = dwEnd-dwCur;
				status = SyncReadWriteSec(pBlkMoverData->DeviceObject,dwCur-g_MovingGroup.dwTStart+g_MovingGroup.dwSStart ,
											dwSecs,pvBuffer,MajorFunction);
			}
		}
		else
			status = SyncReadWriteSec(pBlkMoverData->DeviceObject,dwRWStart ,dwSize,pvBuffer,MajorFunction);
	}
	else
		status = SyncReadWriteSec(pBlkMoverData->DeviceObject,dwRWStart ,dwSize,pvBuffer,MajorFunction);
	return status;
}

NTSTATUS ReadWriteFromGroup(UCHAR MajorFunction,
					   DWORD dwStart,
					   DWORD dwSize,
					   PVOID pvBuffer,
					   BOOL  bRelocate,
					   PBLKMOVER_DATA pBlkMoverData)
{
	NTSTATUS			status = STATUS_SUCCESS;
    DWORD				dwStartSec;
    DWORD				dwEndSec,dwCurPos;
    DWORD				dwSecs,dwStop;
	PPM_PROTECT_GROUP	pProtectGroup;
	
	dwStartSec			= dwStart;
	dwEndSec 			= dwStart+dwSize;
	dwCurPos 			= dwStart;

	pProtectGroup = g_pProtectList;

	while(pProtectGroup)
	{
		if(pProtectGroup->dwSStart > dwCurPos)
		{
			dwStop = MIN(dwEndSec,pProtectGroup->dwSStart);
			dwSecs = dwStop-dwCurPos;
			if(bRelocate)
				status = DoLinearIo(MajorFunction,dwCurPos,dwSecs,pvBuffer,pBlkMoverData);
			else
				status = SyncReadWriteSec(pBlkMoverData->DeviceObject,dwCurPos ,dwSecs,pvBuffer,MajorFunction);
			(PBYTE)pvBuffer += dwSecs*SECTOR_SIZE;
			dwCurPos 		+= dwSecs;
			if(dwCurPos == dwEndSec) break;
		}
		
		if (dwCurPos >= pProtectGroup->dwSStart 
			&& dwCurPos < pProtectGroup->dwSStart + pProtectGroup->dwSize)
		{
			// Do the second part
			dwStop = MIN(dwEndSec,pProtectGroup->dwSStart + pProtectGroup->dwSize);
			dwSecs = dwStop-dwCurPos;
			status = SyncReadWriteSec(pBlkMoverData->DeviceObject,
						  dwCurPos - pProtectGroup->dwSStart + pProtectGroup->dwFStart,
						  dwSecs,
						  pvBuffer,
						  MajorFunction);

			(PBYTE)pvBuffer += dwSecs*SECTOR_SIZE;
			dwCurPos 		+= dwSecs;
			if(dwCurPos == dwEndSec) break;
		}
		if(dwCurPos == dwEndSec) break;
		pProtectGroup = pProtectGroup->pNext;
	}

	if(dwCurPos < dwEndSec)
	{
		dwSecs = dwEndSec-dwCurPos;
		if(bRelocate)
			status = DoLinearIo(MajorFunction,dwCurPos,dwSecs,pvBuffer,pBlkMoverData);
		else
			status = SyncReadWriteSec(pBlkMoverData->DeviceObject,dwCurPos ,dwSecs,pvBuffer,MajorFunction);
	}
	return status;
}

PPM_PROTECT_GROUP GetProtectFreeGroup()
{
	PPM_PROTECT_GROUP	pProtectFreeGroup;

	pProtectFreeGroup	= g_pProtectFreeList;
	g_pProtectFreeList	= g_pProtectFreeList->pNext;
	if(g_pProtectFreeList)
	{
		g_pProtectFreeList->pPrev = NULL;
	}
	ASSERT(pProtectFreeGroup);
	pProtectFreeGroup->pNext = NULL;
	return pProtectFreeGroup;
}

NTSTATUS AddToProtectGroup(DWORD dwStart,
					   DWORD dwSize,
					   PVOID pvBuffer,
					   PBLKMOVER_DATA pBlkMoverData)
{
	NTSTATUS			status = STATUS_SUCCESS;
	PPM_PROTECT_GROUP	pProtectGroup;
	PPM_PROTECT_GROUP	pProtectGroupTail;
	PPM_PROTECT_GROUP	pProtectFreeGroup;

    DWORD					dwStartSec;
    DWORD					dwEndSec,dwCurPos;
    DWORD					dwSecs,dwStop;
	
	dwStartSec 				= dwStart;
	dwEndSec 				= dwStart+dwSize;
	dwCurPos 				= dwStart;

	pProtectGroup = g_pProtectList;
	pProtectGroupTail = g_pProtectList;

	while(pProtectGroup)
	{
		//if(!(dwCurPos >= pProtectGroup->dwSStart+pProtectGroup->wSize ||
		//	dwEndSec <= pProtectGroup->dwSStart))
		//{
		if(pProtectGroup->dwSStart > dwCurPos)
		{
			dwStop = MIN(dwEndSec,pProtectGroup->dwSStart);
			dwSecs = dwStop-dwCurPos;
			pProtectFreeGroup = GetProtectFreeGroup();
			if(!pProtectFreeGroup)
				return status;

			if(pProtectGroup == g_pProtectList)
			{
				g_pProtectList = pProtectFreeGroup;
				pProtectFreeGroup->pPrev = NULL;
			}
			else
			{
				pProtectFreeGroup->pPrev = pProtectGroup->pPrev;
				pProtectGroup->pPrev->pNext = pProtectFreeGroup;
			}
			pProtectFreeGroup->pNext = pProtectGroup;
			pProtectGroup->pPrev	 = pProtectFreeGroup;

			pProtectFreeGroup->dwSStart = dwCurPos;
			pProtectFreeGroup->dwFStart = g_dwFreeStart;
			pProtectFreeGroup->dwSize	= dwSecs;
			ASSERT(g_dwFreeSize > dwSecs);
			g_dwFreeStart			+= dwSecs;
			g_dwFreeSize			-= dwSecs;

			status = SyncReadWriteSec(pBlkMoverData->DeviceObject,
						  pProtectFreeGroup->dwFStart ,
						  dwSecs,
						  pvBuffer,
						  IRP_MJ_WRITE);
			(PBYTE)pvBuffer += dwSecs*SECTOR_SIZE;
			dwCurPos 		+= dwSecs;
			if(dwCurPos == dwEndSec) break;
		}

		if (dwCurPos >= pProtectGroup->dwSStart 
			&& dwCurPos < pProtectGroup->dwSStart + pProtectGroup->dwSize)
		{
			// Do the second part
			dwStop = MIN(dwEndSec,pProtectGroup->dwSStart + pProtectGroup->dwSize);
			dwSecs = dwStop-dwCurPos;
			status = SyncReadWriteSec(pBlkMoverData->DeviceObject,
						  dwCurPos - pProtectGroup->dwSStart + pProtectGroup->dwFStart,
						  dwSecs,
						  pvBuffer,
						  IRP_MJ_WRITE);

			(PBYTE)pvBuffer += dwSecs*SECTOR_SIZE;
			dwCurPos 		+= dwSecs;
			if(dwCurPos == dwEndSec) break;
		}
		if(dwCurPos == dwEndSec) break;
		if(pProtectGroup->pNext) pProtectGroupTail = pProtectGroup->pNext;
		pProtectGroup = pProtectGroup->pNext;
	}

	if(dwCurPos < dwEndSec)
	{
		pProtectFreeGroup = GetProtectFreeGroup();
		if(!pProtectFreeGroup)
			return status;

		if(pProtectGroupTail)
		{
			pProtectGroupTail->pNext = pProtectFreeGroup;
			pProtectFreeGroup->pPrev = pProtectGroupTail;
		}
		else
		{
			g_pProtectList = pProtectFreeGroup;
			pProtectFreeGroup->pPrev = NULL;
		}
		pProtectFreeGroup->pNext = NULL;

		dwSecs = dwEndSec-dwCurPos;
		pProtectFreeGroup->dwSStart = dwCurPos;
		pProtectFreeGroup->dwFStart = g_dwFreeStart;
		pProtectFreeGroup->dwSize	= dwSecs;
		ASSERT(g_dwFreeSize > dwSize);
		g_dwFreeSize			-= dwSecs;
		g_dwFreeStart			+= dwSecs;

		status = SyncReadWriteSec(pBlkMoverData->DeviceObject,
									pProtectFreeGroup->dwFStart ,
									dwSecs,
									pvBuffer,
									IRP_MJ_WRITE);
	}
	return status;
}

BOOL AddToFinalGroup(PM_RELOCATE_GROUP FinalGroup,int nCurPos)
{
	int	i,j;

	for(i=0;i<nCurPos;i++)
	{
		if(g_FinalGroup[i].dwOriginal == FinalGroup.dwOriginal)
		{
			g_FinalGroup[i].dwFinal = FinalGroup.dwFinal;
			break;
		}
		if(FinalGroup.dwOriginal < g_FinalGroup[i].dwOriginal)
		{
			for(j=nCurPos-1;j>=i;j--)
			{
				RtlCopyMemory(&g_FinalGroup[j+1],&g_FinalGroup[j],sizeof(PM_RELOCATE_GROUP));
			}
			//RtlMoveMemory(&g_FinalGroup[i+1],&g_FinalGroup[i],(nCurPos-i)*sizeof(PM_RELOCATE_GROUP));
			RtlCopyMemory(&g_FinalGroup[i],&FinalGroup,sizeof(PM_RELOCATE_GROUP));
			break;
		}
	}
	if(i == nCurPos)
		RtlCopyMemory(&g_FinalGroup[i],&FinalGroup,sizeof(PM_RELOCATE_GROUP));
	return TRUE;
}

BOOL DelFreeSpaceList()
{
	PPM_FREE_SPACE_GROUP			pCurFree,pDelGroup;

	pCurFree = g_FreeSpaceList;

	while(pCurFree)
	{
		pDelGroup= pCurFree;
		pCurFree = pCurFree->pNext;
		ExFreePool(pDelGroup);
	}
	g_FreeSpaceList = NULL;
	return TRUE;
}

BOOL IsRWToFreeSpace(DWORD dwStart,DWORD dwSize)
{
	PPM_FREE_SPACE_GROUP			pCurFree;

	pCurFree = g_FreeSpaceList;

	while(pCurFree)
	{
		if(!(dwStart >= pCurFree->dwStart+pCurFree->dwSize || dwStart + dwSize <= pCurFree->dwStart))
		{
			return TRUE;
		}
		pCurFree = pCurFree->pNext;
	}
	return FALSE;
}

BOOL SetFreeSpaceList(DWORD dwStart,DWORD dwSize)
{
	PPM_FREE_SPACE_GROUP			pCurFreeGroup;
	PPM_FREE_SPACE_GROUP			pNewGroup,pPrevGroup;

	pCurFreeGroup = g_FreeSpaceList;

	pNewGroup = (PPM_FREE_SPACE_GROUP)ExAllocatePool(NonPagedPool,sizeof(PM_FREE_SPACE_GROUP));
	if(!pNewGroup) return FALSE;
	pNewGroup->dwStart = dwStart;
	pNewGroup->dwSize  = dwSize;
	pNewGroup->pNext   = NULL;

	pPrevGroup = NULL;
	if(pCurFreeGroup)
	{
		while(pCurFreeGroup)
		{
			if(pCurFreeGroup->dwStart > pNewGroup->dwStart)
			{
				pNewGroup ->pNext = pCurFreeGroup;
				if(pPrevGroup)
				{
					pPrevGroup->pNext = pNewGroup;
				}
				else
				{
					g_FreeSpaceList = pNewGroup;
				}
				break;
			}
			if(pCurFreeGroup->dwStart < pNewGroup->dwStart && !pCurFreeGroup->pNext)
			{
				pCurFreeGroup->pNext = pNewGroup;
				//pNewGroup ->pNext = pCurFreeGroup;
				break;
			}
			pPrevGroup		= pCurFreeGroup;
			pCurFreeGroup	= pCurFreeGroup->pNext ;
		}
	}
	else
	{
		g_FreeSpaceList = pNewGroup;
	}

	return TRUE;
}
#endif

__int64 lmul(DWORD a, DWORD b)
{
	__int64	lResult;
	_asm
	{
		mov	eax, DWORD PTR [a]
		mul DWORD PTR [b]
		mov DWORD PTR [lResult+0],eax
		mov DWORD PTR [lResult+4],edx
	}
	return lResult;
}
__int64 ldiv(__int64 a, DWORD b)
{
	DWORD	dwResult;
	DWORD   dwLo = ((PDWORD)&a)[0];
	DWORD   dwHi = ((PDWORD)&a)[1];
	_asm
	{
		mov edx, DWORD PTR[dwHi]
		mov eax, DWORD PTR[dwLo]
		div DWORD PTR [b]
		mov DWORD PTR [dwResult],eax 
	}
	return (__int64)dwResult;
}

#ifdef WIN_9X
//Using for recovermaster(create list and find move disk)
BOOL RmInit(BOOL bEnable,BYTE btDrive)
{
    PDCB			dcb;
    PBLKMOVER_DATA  pMoverData;

	g_pDataRegionLRUList = YG_List_Create(0,sizeof(RM_SEC_GROUP_TABLE));
	if(!g_pDataRegionLRUList)
		return FALSE;

    g_MoverDCB = NULL;
    for(dcb = IspGetFirstNextDcb(NULL,DCB_type_disk);
		dcb && (!g_MoverDCB);
		dcb = IspGetFirstNextDcb(dcb,DCB_type_disk))
    {
		if(dcb->DCB_cmn.DCB_device_flags & DCB_DEV_LOGICAL) //	Is logical drive ?
		{
		    if(dcb->DCB_cmn.DCB_drive_lttr_equiv == btDrive)
				g_MoverDCB = (PDCB)dcb->DCB_cmn.DCB_physical_dcb;
		}
    }
    ASSERT(g_MoverDCB);

    g_pMoverData = NULL;
    for(pMoverData = (PBLKMOVER_DATA)List_Get_First(g_lDCB);
		pMoverData && (!g_pMoverData);
		pMoverData =  (PBLKMOVER_DATA)List_Get_Next(g_lDCB,pMoverData))
    {
		if (pMoverData->dcb == g_MoverDCB)
		    g_pMoverData = pMoverData;
	}

	return TRUE;
}

BOOL SetGroupTable(DWORD dwStartSec,DWORD dwRecNum)
{
	BOOL				bResult = FALSE;
	PRM_SEC_GROUP		pGroupBuff,pGroup;
	PRM_SEC_GROUP_TABLE pDataRegion;

//	PTCB			hThread = NULL;
//	DWORD		    dwOldPri;

//    hThread	 = Get_Cur_Thread_Handle();
//    dwOldPri = Get_Cur_Thread_Priority(hThread);

	Out_Debug_String("BLOCKMOVER: Set group table\n\r");
	pDataRegion = (PRM_SEC_GROUP_TABLE)YG_List_Allocate(g_pDataRegionLRUList);
	if(pDataRegion)
	{
		pGroupBuff	  = (PRM_SEC_GROUP)_PageAllocate(SEC_GROUP_BUFFER_SIZE/SECTORS_PER_PAGE,PG_SYS,NULL,0,0,0,NULL,PAGEFIXED);
		if(pGroupBuff)
		{
//			if(dwOldPri < 12)
//			{
//				Set_Cur_Thread_Priority(hThread,12);
//			}
			 Wait_Semaphore(g_hSemSyncIO,BLOCK_THREAD_IDLE);
			 bResult = SecureReadWriteSector(READ_OPERATION,dwStartSec, SEC_GROUP_BUFFER_SIZE,
									pGroupBuff,g_pMoverData);
			 Signal_Semaphore(g_hSemSyncIO);
//			if(dwOldPri < 12)
//			{
//				Set_Cur_Thread_Priority(hThread,dwOldPri);
//			}
		}
		
		memset(pDataRegion,0,sizeof(RM_SEC_GROUP_TABLE));
		pDataRegion->dwStartSec = dwStartSec;
		pDataRegion->dwRecNum   = dwRecNum;
		pDataRegion->pGroupsBuff= pGroupBuff;
		pDataRegion->dwDataRegionStart = pGroupBuff->dwSStartSec;
		pGroup = (PRM_SEC_GROUP)(pGroupBuff+dwRecNum-1);
		pDataRegion->dwDataRegionEnd = pGroup->dwSStartSec + pGroup->dwLength;

		if(g_dwNumDataRegionLoaded >= MAX_DATA_REGION_LOADED && !bResult)
		{
			_PageFree(pGroupBuff,0);
			pDataRegion->pGroupsBuff = NULL;
		}
		if(g_pDataRegionListTail)
			g_pDataRegionListTail->pNext = pDataRegion;
						
		g_pDataRegionListTail = pDataRegion;
		if(g_pDataRegionListHead == NULL)
			g_pDataRegionListHead = pDataRegion;

		if(pDataRegion->pGroupsBuff)
			YG_List_Attach_Tail(g_pDataRegionLRUList,pDataRegion);
		g_dwNumDataRegionLoaded ++;
	}
	return bResult;
}

BOOL	EnabelOrDisRelocate(BOOL bRelocate,DWORD dwMinSec,DWORD dwMaxSec)
{
    g_hSemInternalReq = Create_Semaphore(8);
    if(!g_hSemInternalReq)
    {
		return	FALSE;
    }

	g_dwMinSec = dwMinSec;
	g_dwMaxSec = dwMaxSec;
	g_bReLocate= bRelocate;
	g_dwRePointer = 0;
	g_dwReadEnd	  = 0;
	g_pMoverData->bWorking = bRelocate;

	return TRUE;
}

BOOL MoveGroup(DWORD dwSStart,DWORD dwSize,DWORD dwTStart)
{
//	DWORD		k,i,j;
	BOOL		bResult;
	DWORD		dwPages;
	PVOID		pBuf = NULL;

	dwPages	= (dwSize+7)/8;
	pBuf	= (PVOID)_PageAllocate(dwPages,PG_SYS,NULL,0,0,0,NULL,PAGEFIXED);

	Wait_Semaphore(g_hSemSyncIO,BLOCK_THREAD_IDLE);
	if(pBuf)
	{
		bResult = SecureReadWriteSector(READ_OPERATION,dwSStart,dwSize,pBuf,g_pMoverData);
		if(bResult)
		{
			bResult = SecureReadWriteSector(WRITE_OPERATION,dwTStart,dwSize,pBuf,g_pMoverData);
		}
	}
	Signal_Semaphore(g_hSemSyncIO);

	if(pBuf)
	{
		g_dwRePointer = dwSStart + dwSize;
		_PageFree(pBuf,0);
	}
	return bResult;
}

BOOL     WriteSectors(DWORD dwStartSec,DWORD dwSecs,PVOID pBuffer)
{
	BOOL		bResult = FALSE;

    Wait_Semaphore(g_hSemSyncIO,BLOCK_THREAD_IDLE);

	bResult = SecureReadWriteSector(WRITE_OPERATION,dwStartSec,dwSecs,pBuffer,g_pMoverData);

    Signal_Semaphore(g_hSemSyncIO);
	return bResult;
}

BOOL    ReadSectors(DWORD dwStartSec,DWORD dwSecs,PVOID pBuffer)
{
	BOOL		bResult = FALSE;

    Wait_Semaphore(g_hSemSyncIO,BLOCK_THREAD_IDLE);

	bResult = SecureReadWriteSector(READ_OPERATION,dwStartSec,dwSecs,pBuffer,g_pMoverData);

    Signal_Semaphore(g_hSemSyncIO);
	return bResult;
}

BOOL SearchGroups(DWORD dwIoMode,DWORD dwStart,DWORD dwSectors,
				  PVOID pvBuffer,PBLKMOVER_DATA pBlkMoverData)
{
    DWORD					dwStartSec;
    DWORD					dwEndSec;
    DWORD					dwCurPos;
    DWORD					dwStop;
    DWORD					dwSecs;
	BOOL					bStatus = FALSE;
	PRM_SEC_GROUP			pGroup;
	RM_HANDLE				hHandle;

	pGroup = GetFirstSecsGroup(pBlkMoverData,&hHandle,dwStart,dwSectors,&bStatus);

	if(bStatus)
	{
		if (pGroup)
		{
		    dwStartSec = dwStart;
		    dwEndSec   = dwStart+dwSectors;
		    dwCurPos   = dwStartSec;

			while(pGroup && dwCurPos < dwEndSec )
			{
			    if (dwCurPos < pGroup->dwSStartSec)
				{
					// Do the first part
					dwStop = MIN(dwEndSec,pGroup->dwSStartSec);
					dwSecs = dwStop-dwCurPos;
					ASSERT(dwSecs);
					if(dwSecs)
					{
					  	bStatus = SecureReadWriteSector(dwIoMode,dwCurPos,dwSecs,pvBuffer,pBlkMoverData);
						if(!bStatus) return bStatus;
						(PBYTE)pvBuffer += dwSecs*SECTOR_SIZE;
						dwCurPos += dwSecs;
					}
				}

			    if (dwCurPos >= pGroup->dwSStartSec && dwCurPos < pGroup->dwSStartSec+pGroup->dwLength)
				{
					// Do the second part
					dwStop = MIN(dwEndSec,pGroup->dwSStartSec+pGroup->dwLength);
					dwSecs = dwStop-dwCurPos;
					ASSERT(dwSecs);
					if (dwSecs)
					{
						bStatus = SecureReadWriteSector(dwIoMode,pGroup->dwTStartSec+(dwCurPos-pGroup->dwSStartSec),
											dwSecs,pvBuffer,pBlkMoverData);
						if(!bStatus) return bStatus;
						(PBYTE)pvBuffer += dwSecs*SECTOR_SIZE;
						dwCurPos += dwSecs;
					}
				}
				if (dwCurPos < dwEndSec)
					pGroup = GetNextSecsGroup(pBlkMoverData,&hHandle,dwCurPos,dwEndSec-dwCurPos,&bStatus);
				if(!bStatus) return bStatus;
			}
			if (dwCurPos < dwEndSec)
			{
				// Do the final part
				dwStop = dwEndSec;
				dwSecs = dwStop-dwCurPos;

				ASSERT(dwSecs);
				if(dwSecs)
				{
					bStatus = SecureReadWriteSector(dwIoMode,dwCurPos,dwSecs,pvBuffer,pBlkMoverData);
				}
				if(!bStatus) return bStatus;
			}
		}
	}
    return  bStatus;
}

PRM_SEC_GROUP GetFirstSecsGroup(PBLKMOVER_DATA pBlkMoverData,RM_HANDLE *Handle, 
								DWORD dwStartSec,DWORD dwSecs,BOOL *bStatus)
{
	PRM_SEC_GROUP_TABLE		pDataRegion;
	PRM_SEC_GROUP_TABLE		pReUse;

	pDataRegion = g_pDataRegionListHead;
	while(pDataRegion)
	{
		if(dwStartSec >= pDataRegion->dwDataRegionEnd)
		{
			pDataRegion = pDataRegion->pNext;
		}
		else
		{
			if(pDataRegion->pGroupsBuff == NULL)
			{
				pReUse = (PRM_SEC_GROUP_TABLE)YG_List_Get_First(g_pDataRegionLRUList);

				pDataRegion->pGroupsBuff = pReUse->pGroupsBuff;
				*bStatus = SecureReadWriteSector(READ_OPERATION,pDataRegion->dwStartSec,
								  SEC_GROUP_BUFFER_SIZE,pDataRegion->pGroupsBuff,pBlkMoverData);
				pReUse->pGroupsBuff = NULL;
				YG_List_Remove(g_pDataRegionLRUList,pReUse);
				YG_List_Attach_Tail(g_pDataRegionLRUList,pDataRegion);
			}
			else
				*bStatus = TRUE;
			Handle->pCurNode	  = pDataRegion;
			Handle->dwCurGroupNum = 0;
			return pDataRegion->pGroupsBuff;
		}
	}
	*bStatus = FALSE;
	return NULL;
}

PRM_SEC_GROUP GetNextSecsGroup(PBLKMOVER_DATA pBlkMoverData,RM_HANDLE *Handle, 
								DWORD dwStartSec,DWORD dwSecs,BOOL *bStatus)
{
	PRM_SEC_GROUP_TABLE		pDataRegion;
	PRM_SEC_GROUP_TABLE		pReUse;
	DWORD					dwCurRecNum;
	PRM_SEC_GROUP			pCurGroup;

	dwCurRecNum = Handle->dwCurGroupNum+1;
	if(dwCurRecNum < Handle->pCurNode->dwRecNum)
	{
		pCurGroup = (PRM_SEC_GROUP)(Handle->pCurNode->pGroupsBuff+dwCurRecNum);
		Handle->dwCurGroupNum = dwCurRecNum;
		*bStatus = TRUE;
		return pCurGroup;
	}

	if(!Handle->pCurNode->pNext)
	{
		*bStatus = FALSE;
		return NULL;
	}

	pDataRegion = Handle->pCurNode->pNext;
	while(pDataRegion)
	{
		if(dwStartSec >= pDataRegion->dwDataRegionEnd)
		{
			pDataRegion = pDataRegion->pNext;
		}
		else
		{
			if(pDataRegion->pGroupsBuff == NULL)
			{
				pReUse = (PRM_SEC_GROUP_TABLE)YG_List_Get_First(g_pDataRegionLRUList);

				pDataRegion->pGroupsBuff = pReUse->pGroupsBuff;
				*bStatus = SecureReadWriteSector(READ_OPERATION,pDataRegion->dwStartSec,
								  SEC_GROUP_BUFFER_SIZE,pDataRegion->pGroupsBuff,pBlkMoverData);
				pReUse->pGroupsBuff = NULL;
				YG_List_Remove(g_pDataRegionLRUList,pReUse);
				YG_List_Attach_Tail(g_pDataRegionLRUList,pDataRegion);
			}
			else
				*bStatus = TRUE;
			Handle->pCurNode	  = pDataRegion;
			Handle->dwCurGroupNum = 0;
			return pDataRegion->pGroupsBuff;
		}
	}
	*bStatus = FALSE;
	return NULL;
}

BOOL DoReLocate(DWORD dwIoMode,DWORD dwStart,DWORD dwSectors,
				PVOID pvBuffer,PBLKMOVER_DATA pBlkMoverData)
{
    DWORD			dwStartSec;
    DWORD			dwEndSec;
	BOOL			bResult = FALSE;

    dwStartSec = dwStart;
    dwEndSec   = dwStart+dwSectors;

	ASSERT(dwSectors);
	if(dwSectors)
	{
		if(dwEndSec <= g_dwMinSec || dwStartSec >= g_dwMaxSec)
			bResult = SecureReadWriteSector(dwIoMode,dwStartSec,dwSectors,pvBuffer,pBlkMoverData);
		else
		{
			if(g_dwRePointer == g_dwMaxSec)
			{
				bResult = SearchGroups(dwIoMode,dwStartSec,dwSectors,pvBuffer,pBlkMoverData);
			}
			else
			{
				bResult = SecureReadWriteSector(dwIoMode,dwStartSec,dwSectors,pvBuffer,pBlkMoverData);
				if(dwIoMode == WRITE_OPERATION)
					bResult = SearchGroups(dwIoMode,dwStartSec,dwSectors,pvBuffer,pBlkMoverData);
			}
		}
	}
    return  bResult;
}

#endif
