// OBJECT.C
// Copyright (C) 2000 by YG Corp.
// All rights reserved
// This module contains functions managing low level sector buffers and virtualize
// the final (target) space. These functions provide strong support for both
// sector moving and original to current location lookup.
// Author:  ZhengNan Zhu
// Date:    12-Dec-2001
#ifdef WIN_9X
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
#pragma VxD_LOCKED_CODE_SEG
#else
#include	"ntddk.h"
#include	".\2000\Iomon.h"
#include    ".\2000\blkmover.h"
#endif


// This function loads up a sector mapping object
PYG_LIST AllocMapEntryObj(PYG_LIST pObjList, DWORD dwSecPos, DWORD dwSectors)
{
    DWORD	   dwIndex;
    DWORD	   dw1stSecPos;
    DWORD	   dwLastSecPos;
    PBM_OBJECT	   pObj;
    PBM_OBJECT	   pLastObj;
    DWORD	   dwBegin;
    DWORD	   dwBeginOfs;
    DWORD	   dwEndOfs;
    DWORD	   i;
    BOOL	   bSucc;

    // This function allocates space in the g_pSharkCache buffer. The sectors
    // address corresponding to each Cache object is a logical address relative
    // to the start of the image file. The actual disk address is resolved when
    // ReadMapObjectList or WriteMapObjectList is called.
    bSucc = TRUE;
    dwIndex = LINSECTORNUM2INDEX_O(dwSecPos);
    dw1stSecPos = dwIndex*sizeof(SECMAP)/SECTOR_SIZE;
    dwBeginOfs = (dwIndex*sizeof(SECMAP))%SECTOR_SIZE;
    dwLastSecPos = ((dwIndex+dwSectors)*sizeof(SECMAP)+SECTOR_SIZE-1)/SECTOR_SIZE;
    dwEndOfs = ((dwIndex+dwSectors)*sizeof(SECMAP))%SECTOR_SIZE;
    if (dwEndOfs > 0)
    {
	dwEndOfs = SECTOR_SIZE-dwEndOfs;
    }
    if (!pObjList)
	pObjList = YG_List_Create_From_Pool(g_pListPool,sizeof(BM_OBJECT));
    if (!pObjList)
	bSucc = FALSE;
    if (bSucc)
	pLastObj = YG_List_Get_Last(pObjList);
    if (!AllocateObjectList(dw1stSecPos,dwLastSecPos-dw1stSecPos,g_pSharkCache,g_pSharkData,sizeof(SECMAP),pObjList))
	bSucc = FALSE;
    if (bSucc)
    {
	if (pLastObj)
	    pObj = (PBM_OBJECT)YG_List_Get_Next(pObjList,pLastObj);
	else
	    pObj = (PBM_OBJECT)YG_List_Get_First(pObjList);
	if (pObj)
	{
	    pObj->dwOfs = dwBeginOfs;
	    pObj->dwNumObjs -= dwBeginOfs/sizeof(SECMAP);
	}
	pObj = (PBM_OBJECT)YG_List_Get_Last(pObjList);
	if (pObj)
	{
	    pObj->dwNumObjs -= dwEndOfs/sizeof(SECMAP);
	}
    }
    else
    {
	pObjList = NULL;
    }
    return pObjList;
}

// This function loads up a data sector object
PYG_LIST AllocSectorObj(PYG_LIST pObjList, DWORD dwSecPos, DWORD dwSectors)
{
    BOOL	bSucc;


    bSucc = TRUE;
    if (!pObjList)
	pObjList = YG_List_Create_From_Pool(g_pListPool,sizeof(BM_OBJECT));
    if (!pObjList)
	bSucc = FALSE;
    if (bSucc)
    {
	// Allocate the object and reserve cache space
	if (!AllocateObjectList(dwSecPos,dwSectors,g_pMoverCache,g_pMoverData,SECTOR_SIZE,pObjList))
	    bSucc = FALSE;
    }
    if (!bSucc)
    {
	pObjList = NULL;
    }
    return pObjList;
}


// This function allocates a BM Object and assign a space in the global
// cache buffer.
// A BM Object can be any block of data. It has to be sector aligned
BOOL AllocateObjectList(DWORD dwSecPos, DWORD dwSectors,
			PDISK_CACHE pCache, PBLKMOVER_DATA pBlkMoverData,
			DWORD dwObjSize, PYG_LIST pBMObjList)
{
    PYG_LIST		    pCacheObjList;
    PCACHE_OBJECT	    pFirstObj;
    PCACHE_OBJECT	    pObj;
    PCACHE_OBJECT	    pCacheObj;
    DWORD		    dwRdLeft;
    DWORD		    dwRdRight;
    DWORD		    dwObjLeft;
    DWORD		    dwObjRight;
    DWORD		    dwEnd;
    BOOL		    bSucc;

    bSucc = TRUE;
    pFirstObj = NULL;
    pCacheObjList = pCache->pObjList;
    ASSERT(pCacheObjList);
    ASSERT(dwSectors > 0);
    dwRdLeft  = dwSecPos;
    dwRdRight = dwSecPos+dwSectors;
    // loop through the allocated objects to process all overlapping regions
    for (pCacheObj = (PCACHE_OBJECT)YG_List_Get_First(pCacheObjList);
	 pCacheObj && dwRdLeft < dwRdRight && bSucc;
	 pCacheObj = (PCACHE_OBJECT)YG_List_Get_Next(pCacheObjList,(PVOID)pCacheObj))
    {
	dwObjLeft  = pCacheObj->DiskRange.dwSecAddr;
	dwObjRight = pCacheObj->DiskRange.dwSecAddr+pCacheObj->DiskRange.dwSectors;
	// Skip NULL objects.
	if (dwObjRight > dwObjLeft)
	{
	    // Regions overlap
	    if (dwRdLeft < dwObjLeft && bSucc)
	    {
		dwRdLeft = MIN(dwRdRight,dwObjLeft);
	    }
	    if (dwRdLeft == dwObjLeft && dwRdLeft < dwRdRight && bSucc)
	    {
		if (dwRdRight <  dwObjRight && bSucc)
		{
		    // Split the obj
		    ProcessAllocatedRegionRight(pCache,pCacheObj,dwObjRight-dwRdRight);
		    dwObjRight = pCacheObj->DiskRange.dwSecAddr+pCacheObj->DiskRange.dwSectors;
		}
		if (dwRdRight >= dwObjRight && bSucc)
		{
		    // increase the use cnt and adjust the LRU list
		    ProcessOverlappedRegion(pCache,pCacheObj,&dwRdLeft);
		}
	    }
	    if (dwRdLeft >  dwObjLeft && dwRdLeft < dwObjRight && dwRdLeft < dwRdRight && bSucc)
	    {
		// Split object
		ProcessAllocatedRegionLeft(pCache,pCacheObj,dwRdLeft-dwObjLeft);
	    }
	}
    }

    // loop through the allocated objects to allocate unoverlapped regions
    dwRdLeft  = dwSecPos;
    dwRdRight = dwSecPos+dwSectors;
    for (pCacheObj = (PCACHE_OBJECT)YG_List_Get_First(pCacheObjList);
	 pCacheObj && dwRdLeft < dwRdRight && bSucc;
	 pCacheObj = (PCACHE_OBJECT)YG_List_Get_Next(pCacheObjList,(PVOID)pCacheObj))
    {
	dwObjLeft  = pCacheObj->DiskRange.dwSecAddr;
	dwObjRight = pCacheObj->DiskRange.dwSecAddr+pCacheObj->DiskRange.dwSectors;
	// Skip NULL objects.
	if (dwObjRight > dwObjLeft)
	{
	    // Regions overlap
	    if (dwRdLeft < dwObjLeft && bSucc)
	    {
		// At least part of the region is not in the cache must create a new object
		if (!CreateObjUnAllocatedRegion(pCache,pBlkMoverData,&pCacheObj,&dwRdLeft,dwRdRight,dwObjSize,pBMObjList))
		{
		    FreeObjectList(pBMObjList,pCache);
		    return FALSE;
		}
		if (pCacheObj)
		{
		    dwObjLeft  = pCacheObj->DiskRange.dwSecAddr;
		    dwObjRight = pCacheObj->DiskRange.dwSecAddr+pCacheObj->DiskRange.dwSectors;
		}
		else
		    break;
	    }
	    // Skip object
	    if (dwRdLeft == dwObjLeft && dwRdRight > dwRdLeft && bSucc)
	    {
		CreateObjOverlappedRegion(pCache,pCacheObj,&dwRdLeft,dwObjSize,pBMObjList);
	    }
	}
    }
    if (dwRdLeft < dwRdRight)
    {
	// un processed region found. Create a new object and add it to the tail
	if (!CreateObjUnAllocatedRegion(pCache,pBlkMoverData,&pCacheObj,&dwRdLeft,dwRdRight,dwObjSize,pBMObjList))
	{
	    FreeObjectList(pBMObjList,pCache);
	    return FALSE;
	}
    }
    return bSucc;
}

BOOL AddToRangeList(PYG_LIST pRangeList, PCACHE_OBJECT pObj)
{
    DWORD   dwMemLeft;
    DWORD   dwMemRight;
    DWORD   dwDiskLeft;
    DWORD   dwDiskRight;
    PCACHE_OBJECT   pCacheObj;
    PCACHE_OBJECT   pNextObj;
    PCACHE_OBJECT   pNewObj;

    dwMemLeft = pObj->MemRange.dwSecOfs;
    dwMemRight = dwMemLeft+pObj->MemRange.dwSectors;
    dwDiskLeft = pObj->DiskRange.dwSecAddr;
    dwDiskRight = dwDiskLeft+pObj->DiskRange.dwSectors;

    for (pCacheObj = (PCACHE_OBJECT)YG_List_Get_First(pRangeList);
	 pCacheObj;
	 pCacheObj = (PCACHE_OBJECT)YG_List_Get_Next(pRangeList,(PVOID)pCacheObj))
    {
	ASSERT(dwDiskLeft != pCacheObj->DiskRange.dwSecAddr);
	if (dwDiskLeft < pCacheObj->DiskRange.dwSecAddr)
	{
	    if (dwDiskRight == pCacheObj->DiskRange.dwSecAddr &&
		dwMemRight == pCacheObj->MemRange.dwSecOfs)
	    {
		pCacheObj->DiskRange.dwSecAddr -= pObj->DiskRange.dwSectors;
		pCacheObj->DiskRange.dwSectors += pObj->DiskRange.dwSectors;
		pCacheObj->MemRange.dwSecOfs  -= pObj->MemRange.dwSectors;
		pCacheObj->MemRange.dwSectors += pObj->MemRange.dwSectors;
		return TRUE;
	    }
	    ASSERT(dwDiskRight <= pCacheObj->DiskRange.dwSecAddr);
	    pNewObj = YG_List_Allocate_From_Pool(pRangeList,g_pCacheObjNodePool);
	    if (pNewObj)
	    {
		*pNewObj = *pObj;
		YG_List_Insert_Before(pRangeList,pNewObj,pCacheObj);
	    }
	    return TRUE;
	}
	else
	{
	    ASSERT(pCacheObj->DiskRange.dwSecAddr+pCacheObj->DiskRange.dwSectors <= dwDiskRight);
	    if (dwDiskLeft == pCacheObj->DiskRange.dwSecAddr+pCacheObj->DiskRange.dwSectors &&
		dwMemLeft == pCacheObj->MemRange.dwSecOfs+pCacheObj->MemRange.dwSectors)
	    {
		pCacheObj->DiskRange.dwSectors += pObj->DiskRange.dwSectors;
		pCacheObj->MemRange.dwSectors += pObj->MemRange.dwSectors;
		pNextObj = (PCACHE_OBJECT)YG_List_Get_Next(pRangeList,pCacheObj);
		while(pNextObj)
		{
		    if (pNextObj->MemRange.dwSecOfs == pCacheObj->MemRange.dwSecOfs+pCacheObj->MemRange.dwSectors &&
			pNextObj->DiskRange.dwSecAddr == pCacheObj->DiskRange.dwSecAddr+pCacheObj->DiskRange.dwSectors)
		    {
			pCacheObj->MemRange.dwSectors += pNextObj->MemRange.dwSectors;
			pCacheObj->DiskRange.dwSectors += pNextObj->DiskRange.dwSectors;
			YG_List_Remove(pRangeList,pNextObj);
			YG_List_Deallocate_To_Pool(pRangeList,pNextObj,g_pCacheObjNodePool);
			pNextObj = (PCACHE_OBJECT)YG_List_Get_Next(pRangeList,pCacheObj);
		    }
		    else
			return TRUE;
		}
		return TRUE;
	    }
	}
    }
    pNewObj = YG_List_Allocate_From_Pool(pRangeList,g_pCacheObjNodePool);
    if (pNewObj)
    {
	*pNewObj = *pObj;
	YG_List_Attach_Tail(pRangeList,pNewObj);
    }
    return TRUE;
}
BOOL DeleteFromRangeList(PYG_LIST pRangeList, PCACHE_OBJECT pObj)
{
    DWORD   dwMemLeft;
    DWORD   dwMemRight;
    DWORD   dwCommonLeft;
    DWORD   dwCommonRight;
    DWORD   dwCommonSectors;
    DWORD   dwSecAddr;
    DWORD   dwMemOfs;
    DWORD   dwDiskLeft;
    DWORD   dwDiskRight;
    PCACHE_OBJECT   pCacheObj;
    PCACHE_OBJECT   pNextCacheObj;
    PCACHE_OBJECT   pNewCacheObj;

    dwMemLeft = pObj->MemRange.dwSecOfs;
    dwMemRight = dwMemLeft+pObj->MemRange.dwSectors;
    dwDiskLeft = pObj->DiskRange.dwSecAddr;
    dwDiskRight = dwDiskLeft+pObj->DiskRange.dwSectors;

    for (pCacheObj = (PCACHE_OBJECT)YG_List_Get_First(pRangeList);
	 pCacheObj;
	 pCacheObj = pNextCacheObj)
    {
	pNextCacheObj = (PCACHE_OBJECT)YG_List_Get_Next(pRangeList,(PVOID)pCacheObj);
	dwCommonLeft = MAX(dwDiskLeft,pCacheObj->DiskRange.dwSecAddr);
	dwCommonRight = MIN(dwDiskRight,pCacheObj->DiskRange.dwSecAddr+pCacheObj->DiskRange.dwSectors);
	if (dwCommonRight > dwCommonLeft)
	{
	    dwCommonSectors = dwCommonRight-dwCommonLeft;
	    // exclude the common region from pCacheObj
	    dwSecAddr = pCacheObj->DiskRange.dwSecAddr;
	    dwMemOfs  = pCacheObj->MemRange.dwSecOfs;

	    if (dwSecAddr < dwCommonLeft)
	    {
		pNewCacheObj = YG_List_Allocate_From_Pool(pRangeList,g_pCacheObjNodePool);
		YG_List_Insert_Before(pRangeList,pNewCacheObj,pCacheObj);
		pNewCacheObj->DiskRange.dwSecAddr = dwSecAddr;
		pNewCacheObj->DiskRange.dwSectors = dwCommonLeft-dwSecAddr;
		pNewCacheObj->MemRange.dwSecOfs   = dwMemOfs;
		pNewCacheObj->MemRange.dwSectors  = dwCommonLeft-dwSecAddr;
		dwSecAddr += dwCommonLeft-dwSecAddr;
		dwMemOfs += dwCommonLeft-dwSecAddr;
	    }
	    if (dwSecAddr == dwCommonLeft)
	    {
		dwSecAddr += dwCommonSectors;
		dwMemOfs  += dwCommonSectors;
	    }
	    if (dwSecAddr < pCacheObj->DiskRange.dwSecAddr+pCacheObj->DiskRange.dwSectors)
	    {
		pNewCacheObj = YG_List_Allocate_From_Pool(pRangeList,g_pCacheObjNodePool);
		YG_List_Insert_Before(pRangeList,pNewCacheObj,pCacheObj);
		pNewCacheObj->DiskRange.dwSecAddr = dwSecAddr;
		pNewCacheObj->DiskRange.dwSectors = pCacheObj->DiskRange.dwSecAddr+pCacheObj->DiskRange.dwSectors-dwSecAddr;
		pNewCacheObj->MemRange.dwSecOfs   = dwMemOfs;
		pNewCacheObj->MemRange.dwSectors  = pCacheObj->DiskRange.dwSecAddr+pCacheObj->DiskRange.dwSectors-dwSecAddr;
	    }
	    YG_List_Remove(pRangeList,pCacheObj);
	    YG_List_Deallocate_To_Pool(pRangeList,pCacheObj,g_pCacheObjNodePool);
	}
    }
    return TRUE;
}
BOOL GetReadRanges(PYG_LIST pCacheList, PYG_LIST pObjList, PYG_LIST pReadList)
{
    PBM_OBJECT		    pObj;
    PCACHE_OBJECT	    pCacheObj;
    DWORD		    dwFirstSec;

    for (pObj = (PBM_OBJECT)YG_List_Get_First(pObjList);
	 pObj;
	 pObj = (PBM_OBJECT)YG_List_Get_Next(pObjList,(PVOID)pObj))
    {
	dwFirstSec = pObj->pCacheObj->DiskRange.dwSecAddr;
	for (pCacheObj = pObj->pCacheObj;
	     pCacheObj &&
	     pCacheObj->DiskRange.dwSecAddr < dwFirstSec+pObj->dwSectors;
	     pCacheObj = (PCACHE_OBJECT)YG_List_Get_Next(pCacheList,(PVOID)pCacheObj))
	{
	    if (pCacheObj->DiskRange.dwSectors > 0)
	    {
		ASSERT(pCacheObj->dwUseCnt > 0);
		if (!pCacheObj->bLoaded)
		{
		    AddToRangeList(pReadList,pCacheObj);
		    pCacheObj->bLoaded = TRUE;
		}
	    }
	}
    }
    return TRUE;
}
BOOL GetTranslatedReadRanges(PYG_LIST pCacheList, PYG_LIST pObjList, PYG_LIST pReadList)
{
    PBM_OBJECT		    pObj;
    PCACHE_OBJECT	    pCacheObj;
    DWORD		    dwFirstSec;
    DWORD		    dwSecOfs;
    GET_SEC_GROUP	    GetGroup;
    SEC_GROUP		    Group;

    for (pObj = (PBM_OBJECT)YG_List_Get_First(pObjList);
	 pObj;
	 pObj = (PBM_OBJECT)YG_List_Get_Next(pObjList,(PVOID)pObj))
    {
	dwFirstSec = pObj->pCacheObj->DiskRange.dwSecAddr;
	for (pCacheObj = pObj->pCacheObj;
	     pCacheObj && pCacheObj->DiskRange.dwSecAddr < dwFirstSec+pObj->dwSectors;
	     pCacheObj = (PCACHE_OBJECT)YG_List_Get_Next(pCacheList,(PVOID)pCacheObj))
	{
	    if (pCacheObj->DiskRange.dwSectors > 0)
	    {
		ASSERT(pCacheObj->dwUseCnt > 0);
		if (!pCacheObj->bLoaded)
		{
		    TranslateRange(pCacheObj,pReadList);
		    /*
		    dwSecOfs = pCacheObj->MemRange.dwSecOfs;
		    GetGroup.pCurGroup = NULL;
		    GetGroup.dwCurLogicalSecAddr = 0;
		    while (GetImgSectorGroup(&GetGroup,pCacheObj->DiskRange.dwSecAddr,pCacheObj->DiskRange.dwSectors,&Group))
		    {
			CacheObj.MemRange.dwSecOfs = dwSecOfs;
			CacheObj.MemRange.dwSectors = Group.dwSectors;
			CacheObj.DiskRange.dwSecAddr = Group.dwStartSec;
			CacheObj.DiskRange.dwSectors = Group.dwSectors;
			AddToRangeList(pReadList,&CacheObj);
			dwSecOfs += Group.dwSectors;
		    }
		    */
		    pCacheObj->bLoaded = TRUE;
		}
	    }
	}
    }
    return TRUE;
}


BOOL GetWriteRanges(PYG_LIST pCacheList, PYG_LIST pObjList, PYG_LIST pList)
{
    PBM_OBJECT		    pObj;
    PCACHE_OBJECT	    pCacheObj;
    DWORD		    dwFirstSec;

    for (pObj = (PBM_OBJECT)YG_List_Get_First(pObjList);
	 pObj;
	 pObj = (PBM_OBJECT)YG_List_Get_Next(pObjList,(PVOID)pObj))
    {
	dwFirstSec = pObj->pCacheObj->DiskRange.dwSecAddr;
	for (pCacheObj = pObj->pCacheObj;
	     pCacheObj &&
	     pCacheObj->DiskRange.dwSecAddr < dwFirstSec+pObj->dwSectors;
	     pCacheObj = (PCACHE_OBJECT)YG_List_Get_Next(pCacheList,(PVOID)pCacheObj))
	{
	    if (pCacheObj->DiskRange.dwSectors > 0)
	    {
		ASSERT(pCacheObj->dwUseCnt > 0);
		if (pCacheObj->bDirty)
		{
		    AddToRangeList(pList,pCacheObj);
		    pCacheObj->bDirty = FALSE;
		}
	    }
	}
    }
    return TRUE;
}

BOOL TranslateRange(PCACHE_OBJECT pCacheObj, PYG_LIST pList)
{
    CACHE_OBJECT	    CacheObj;
    DWORD		    dwSecOfs;
    GET_SEC_GROUP	    GetGroup;
    SEC_GROUP		    Group;

    dwSecOfs = pCacheObj->MemRange.dwSecOfs;
    GetGroup.pCurGroup = NULL;
    GetGroup.dwCurLogicalSecAddr = 0;
    while (GetImgSectorGroup(&GetGroup,pCacheObj->DiskRange.dwSecAddr,pCacheObj->DiskRange.dwSectors,&Group))
    {
	CacheObj.MemRange.dwSecOfs = dwSecOfs;
	CacheObj.MemRange.dwSectors = Group.dwSectors;
	CacheObj.DiskRange.dwSecAddr = Group.dwStartSec;
	CacheObj.DiskRange.dwSectors = Group.dwSectors;
	AddToRangeList(pList,&CacheObj);
	dwSecOfs += Group.dwSectors;
    }
    return TRUE;
}

BOOL GetTranslatedWriteRanges(PYG_LIST pCacheList, PYG_LIST pObjList, PYG_LIST pList)
{
    PBM_OBJECT		    pObj;
    PCACHE_OBJECT	    pCacheObj;
    DWORD		    dwFirstSec;
    DWORD		    dwSecOfs;
    GET_SEC_GROUP	    GetGroup;
    SEC_GROUP		    Group;

    for (pObj = (PBM_OBJECT)YG_List_Get_First(pObjList);
	 pObj;
	 pObj = (PBM_OBJECT)YG_List_Get_Next(pObjList,(PVOID)pObj))
    {
	dwFirstSec = pObj->pCacheObj->DiskRange.dwSecAddr;
	for (pCacheObj = pObj->pCacheObj;
	     pCacheObj &&
	     pCacheObj->DiskRange.dwSecAddr < dwFirstSec+pObj->dwSectors;
	     pCacheObj = (PCACHE_OBJECT)YG_List_Get_Next(pCacheList,(PVOID)pCacheObj))
	{
	    if (pCacheObj->DiskRange.dwSectors > 0)
	    {
		ASSERT(pCacheObj->dwUseCnt > 0);
		if (pCacheObj->bDirty)
		{
		    TranslateRange(pCacheObj,pList);
		    /*
		    dwSecOfs = pCacheObj->MemRange.dwSecOfs;
		    GetGroup.pCurGroup = NULL;
		    GetGroup.dwCurLogicalSecAddr = 0;
		    while (GetImgSectorGroup(&GetGroup,pCacheObj->DiskRange.dwSecAddr,pCacheObj->DiskRange.dwSectors,&Group))
		    {
			CacheObj.MemRange.dwSecOfs = dwSecOfs;
			CacheObj.MemRange.dwSectors = Group.dwSectors;
			CacheObj.DiskRange.dwSecAddr = Group.dwStartSec;
			CacheObj.DiskRange.dwSectors = Group.dwSectors;
			AddToRangeList(pList,&CacheObj);
			dwSecOfs += Group.dwSectors;
		    }
		    */
		    pCacheObj->bDirty = FALSE;
		}
	    }
	}
    }
    return TRUE;
}
BOOL ReadRanges(PYG_LIST pList, PDISK_CACHE pCache, PBLKMOVER_DATA pBlkMoverData)
{
    PCACHE_OBJECT   pObj;

    for (pObj = (PCACHE_OBJECT)YG_List_Get_First(pList);
	 pObj;
	 pObj = (PCACHE_OBJECT)YG_List_Get_Next(pList,(PVOID)pObj))
    {
	 ASSERT(pObj->MemRange.dwSecOfs+pObj->MemRange.dwSectors <= pCache->dwCacheSize);
#ifdef WIN_9X
	 SecureReadWriteSector(READ_OPERATION,
			       pObj->DiskRange.dwSecAddr,
			       pObj->DiskRange.dwSectors,
			       &pCache->pCache[pObj->MemRange.dwSecOfs*SECTOR_SIZE],
			       pBlkMoverData);
#else
	 DbgPrint("BLKMOVER:\tReadRanges\n");
	 SyncReadWriteSec(pBlkMoverData->DeviceObject,
			       pObj->DiskRange.dwSecAddr,
			       pObj->DiskRange.dwSectors,
			       &pCache->pCache[pObj->MemRange.dwSecOfs*SECTOR_SIZE],
				   IRP_MJ_READ);
#endif
    }
    return TRUE;
}
BOOL ReadRangesFromCache(PYG_LIST pList, PDISK_CACHE pCache, PDISK_CACHE pFromCache, PBLKMOVER_DATA pBlkMoverData)
{
    PCACHE_OBJECT   pObj;

    for (pObj = (PCACHE_OBJECT)YG_List_Get_First(pList);
	 pObj;
	 pObj = (PCACHE_OBJECT)YG_List_Get_Next(pList,(PVOID)pObj))
    {
	 CopyRangeFromCache(pList,pObj,pCache,pFromCache);
	 if (pObj->DiskRange.dwSectors > 0)
	 {
#ifdef WIN_9X
	     SecureReadWriteSector(READ_OPERATION,
				   pObj->DiskRange.dwSecAddr,
				   pObj->DiskRange.dwSectors,
				   &pCache->pCache[pObj->MemRange.dwSecOfs*SECTOR_SIZE],
				   pBlkMoverData);
#else
		DbgPrint("BLKMOVER:\tReadRangesFromCache\n");
		SyncReadWriteSec(pBlkMoverData->DeviceObject,
			       pObj->DiskRange.dwSecAddr,
			       pObj->DiskRange.dwSectors,
			       &pCache->pCache[pObj->MemRange.dwSecOfs*SECTOR_SIZE],
				   IRP_MJ_READ);
#endif
	}
    }
    return TRUE;
}

BOOL WriteRanges(PYG_LIST pList, PDISK_CACHE pCache, PBLKMOVER_DATA pBlkMoverData)
{
    PCACHE_OBJECT   pObj;

    for (pObj = (PCACHE_OBJECT)YG_List_Get_First(pList);
	 pObj;
	 pObj = (PCACHE_OBJECT)YG_List_Get_Next(pList,(PVOID)pObj))
    {
	 ASSERT(pObj->MemRange.dwSecOfs+pObj->MemRange.dwSectors <= pCache->dwCacheSize);
#ifdef WIN_9X
	 SecureReadWriteSector(WRITE_OPERATION,
			       pObj->DiskRange.dwSecAddr,
			       pObj->DiskRange.dwSectors,
			       &pCache->pCache[pObj->MemRange.dwSecOfs*SECTOR_SIZE],
			       pBlkMoverData);
#else
	 SyncReadWriteSec(pBlkMoverData->DeviceObject,
			       pObj->DiskRange.dwSecAddr,
			       pObj->DiskRange.dwSectors,
			       &pCache->pCache[pObj->MemRange.dwSecOfs*SECTOR_SIZE],
				   IRP_MJ_WRITE);
#endif
    }
    return TRUE;
}
BOOL WriteRangesToCache(PYG_LIST pList, PDISK_CACHE pCache, PDISK_CACHE pToCache, PBLKMOVER_DATA pBlkMoverData)
{
    PCACHE_OBJECT   pObj;

    for (pObj = (PCACHE_OBJECT)YG_List_Get_First(pList);
	 pObj;
	 pObj = (PCACHE_OBJECT)YG_List_Get_Next(pList,(PVOID)pObj))
    {
	CopyRangeToCache(pList,pObj,pCache,pToCache);
	if (pObj->DiskRange.dwSectors > 0)
	{
#ifdef WIN_9X
	    SecureReadWriteSector(WRITE_OPERATION,
				  pObj->DiskRange.dwSecAddr,
				  pObj->DiskRange.dwSectors,
				  &pCache->pCache[pObj->MemRange.dwSecOfs*SECTOR_SIZE],
				  pBlkMoverData);
#else
	 SyncReadWriteSec(pBlkMoverData->DeviceObject,
			       pObj->DiskRange.dwSecAddr,
			       pObj->DiskRange.dwSectors,
			       &pCache->pCache[pObj->MemRange.dwSecOfs*SECTOR_SIZE],
				   IRP_MJ_WRITE);
#endif
	}
    }
    return TRUE;
}
// This function loads up a BM Object.
BOOL ReadObjectList(PYG_LIST pObjList, PDISK_CACHE pCache, PBLKMOVER_DATA pBlkMoverData)
{
    PYG_LIST		    pReadList;

    ASSERT(pObjList);
    pReadList = YG_List_Create_From_Pool(g_pListPool,sizeof(CACHE_OBJECT));
    if (pReadList)
    {
	if (GetReadRanges(pCache->pObjList,pObjList,pReadList))
	    ReadRanges(pReadList,pCache,pBlkMoverData);
	YG_List_Destroy_To_Pool(pReadList,g_pListPool,g_pCacheObjNodePool);
	return TRUE;
    }
    return FALSE;
}
// This function writes a BM Object.
BOOL WriteObjectList(PYG_LIST pObjList, PDISK_CACHE pCache, PBLKMOVER_DATA pBlkMoverData)
{
    PYG_LIST		    pList;

    ASSERT(pObjList);
    pList = YG_List_Create_From_Pool(g_pListPool,sizeof(CACHE_OBJECT));
    if (pList)
    {
	if (GetWriteRanges(pCache->pObjList,pObjList,pList))
	    WriteRanges(pList,pCache,pBlkMoverData);
	YG_List_Destroy_To_Pool(pList,g_pListPool,g_pCacheObjNodePool);
	return TRUE;
    }
    return FALSE;
}
// This function loads up a Map Object List.
BOOL ReadMapObjectList(PYG_LIST pObjList, PDISK_CACHE pCache, PBLKMOVER_DATA pBlkMoverData)
{
    PYG_LIST		    pReadList;

    ASSERT(pObjList);
    pReadList = YG_List_Create_From_Pool(g_pListPool,sizeof(CACHE_OBJECT));
    if (pReadList)
    {
	if (GetTranslatedReadRanges(pCache->pObjList,pObjList,pReadList))
	{
	    if (g_dwMoverDrive == g_dwSharkDrive)
		ReadRangesFromCache(pReadList,pCache,g_pMoverCache,pBlkMoverData);
	    else
		ReadRanges(pReadList,pCache,pBlkMoverData);
	}
	YG_List_Destroy_To_Pool(pReadList,g_pListPool,g_pCacheObjNodePool);
	return TRUE;
    }
    return FALSE;
}
// This function writes a Map Object List to disk.
BOOL WriteMapObjectList(PYG_LIST pObjList, PDISK_CACHE pCache, PBLKMOVER_DATA pBlkMoverData)
{
    PYG_LIST		    pList;

    ASSERT(pObjList);
    pList = YG_List_Create_From_Pool(g_pListPool,sizeof(CACHE_OBJECT));
    if (pList)
    {
	if (GetTranslatedWriteRanges(pCache->pObjList,pObjList,pList))
	{
	    if (g_dwMoverDrive == g_dwSharkDrive)
		WriteRangesToCache(pList,pCache,g_pMoverCache,pBlkMoverData);
	    else
		WriteRanges(pList,pCache,pBlkMoverData);
	}
	YG_List_Destroy_To_Pool(pList,g_pListPool,g_pCacheObjNodePool);
	return TRUE;
    }
    return FALSE;
}
// This function flushes the cache.
BOOL FlushMoverCache(PDISK_CACHE pCache, PBLKMOVER_DATA pBlkMoverData)
{
    PYG_LIST			pList;
    PCACHE_OBJECT		pObj;
    BOOL			bSucc;

    bSucc = TRUE;
    pList = YG_List_Create_From_Pool(g_pListPool,sizeof(CACHE_OBJECT));
    if (pList)
    {
	for (pObj = (PCACHE_OBJECT)YG_List_Get_First(pCache->pObjList);
	     pObj && bSucc;
	     pObj = (PCACHE_OBJECT)YG_List_Get_Next(pCache->pObjList,pObj))
	{
	    if (pObj->bDirty)
	    {
		AddToRangeList(pList,pObj);
		pObj->bDirty = FALSE;
	    }
	}
	WriteRanges(pList,pCache,pBlkMoverData);
	YG_List_Destroy_To_Pool(pList,g_pListPool,g_pCacheObjNodePool);
    }
    if (bSucc)
	if (pCache->pObjList)
	    YG_List_Destroy_To_Pool(pCache->pObjList,g_pListPool,g_pCacheObjNodePool);
    if (bSucc)
	pCache->pObjList = YG_List_Create_From_Pool(g_pListPool,sizeof(CACHE_OBJECT));
    if (!pCache->pObjList)
	bSucc = FALSE;
    // Create a new object to represent the entire region
    if (bSucc)
	pObj = (PCACHE_OBJECT)YG_List_Allocate_From_Pool(pCache->pObjList,g_pCacheObjNodePool);
    if (!pObj)
	bSucc = FALSE;
    if (bSucc)
    {
	pObj->DiskRange.dwSecAddr  = 0;
	pObj->DiskRange.dwSectors = 0;
	pObj->MemRange.dwSecOfs  = 0;
	pObj->MemRange.dwSectors = pCache->dwCacheSize;
	pObj->dwUseCnt = 0;
	pObj->bLoaded = FALSE;
	pObj->bDirty  = FALSE;
	pObj->pPrev = NULL;
	pObj->pNext = NULL;
	YG_List_Attach_Tail(pCache->pObjList,pObj);
    }
    pCache->pLRUHead = pObj;
    pCache->pLRUTail = pObj;
    return bSucc;
}
// This function flushes the cache.
BOOL FlushSharkCache(PDISK_CACHE pCache, PBLKMOVER_DATA pBlkMoverData)
{
    PYG_LIST			pList;
    PCACHE_OBJECT		pObj;
    CACHE_OBJECT		CacheObj;
    DWORD			dwSecOfs;
    GET_SEC_GROUP		GetGroup;
    SEC_GROUP			Group;
    BOOL			bSucc;

    bSucc = TRUE;
    pList = YG_List_Create_From_Pool(g_pListPool,sizeof(CACHE_OBJECT));
    if (pList)
    {
	for (pObj = (PCACHE_OBJECT)YG_List_Get_First(pCache->pObjList);
	     pObj && bSucc;
	     pObj = (PCACHE_OBJECT)YG_List_Get_Next(pCache->pObjList,pObj))
	{
	    if (pObj->bDirty)
	    {
		TranslateRange(pObj,pList);
		/*
		dwSecOfs = pObj->MemRange.dwSecOfs;
		GetGroup.pCurGroup = NULL;
		GetGroup.dwCurLogicalSecAddr = 0;
		while (GetImgSectorGroup(&GetGroup,pObj->DiskRange.dwSecAddr,pObj->DiskRange.dwSectors,&Group))
		{
		    CacheObj.MemRange.dwSecOfs = dwSecOfs;
		    CacheObj.MemRange.dwSectors = Group.dwSectors;
		    CacheObj.DiskRange.dwSecAddr = Group.dwStartSec;
		    CacheObj.DiskRange.dwSectors = Group.dwSectors;
		    AddToRangeList(pList,&CacheObj);
		    dwSecOfs += Group.dwSectors;
		}
		*/
		pObj->bDirty = FALSE;
	    }
	}
	if (g_dwMoverDrive == g_dwSharkDrive)
	    WriteRangesToCache(pList,pCache,g_pMoverCache,pBlkMoverData);
	else
	    WriteRanges(pList,pCache,pBlkMoverData);
	YG_List_Destroy_To_Pool(pList,g_pListPool,g_pCacheObjNodePool);
    }
    if (bSucc)
	if (pCache->pObjList)
	    YG_List_Destroy_To_Pool(pCache->pObjList,g_pListPool,g_pCacheObjNodePool);
    if (bSucc)
	pCache->pObjList = YG_List_Create_From_Pool(g_pListPool,sizeof(CACHE_OBJECT));
    if (!pCache->pObjList)
	bSucc = FALSE;
    // Create a new object to represent the entire region
    if (bSucc)
	pObj = (PCACHE_OBJECT)YG_List_Allocate_From_Pool(pCache->pObjList,g_pCacheObjNodePool);
    if (!pObj)
	bSucc = FALSE;
    if (bSucc)
    {
	pObj->DiskRange.dwSecAddr  = 0;
	pObj->DiskRange.dwSectors = 0;
	pObj->MemRange.dwSecOfs  = 0;
	pObj->MemRange.dwSectors = pCache->dwCacheSize;
	pObj->dwUseCnt = 0;
	pObj->bLoaded = FALSE;
	pObj->bDirty  = FALSE;
	pObj->pPrev = NULL;
	pObj->pNext = NULL;
	YG_List_Attach_Tail(pCache->pObjList,pObj);
    }
    pCache->pLRUHead = pObj;
    pCache->pLRUTail = pObj;
    return bSucc;
}
BOOL FlushMergeCacheObjs(PDISK_CACHE pCache, PBLKMOVER_DATA pBlkMoverData, DWORD dwSectors, PYG_LIST pFreeCacheList)
{
    PCACHE_OBJECT   pObj;
    PCACHE_OBJECT   pNewObj;
    PCACHE_OBJECT   pNextObj;
    PYG_LIST	    pMemRangeList;
    PYG_LIST	    pDiskRangeList;
    DWORD	    dwSectorsLeft;
    GET_SEC_GROUP   GetGroup;
    SEC_GROUP	    Group;
    DWORD	    dwSecOfs;
    PYG_LIST	    pList;

    dwSectorsLeft = dwSectors;
    for (pObj = pCache->pLRUHead; pObj && dwSectorsLeft > 0; pObj = pNextObj)
    {
	pNextObj = pObj->pNext;
	if (pObj->dwUseCnt == 0)
	{
	    if (pObj->bDirty)
	    {
		if (pObj->DiskRange.dwSecAddr > 0 && pObj->DiskRange.dwSectors > 0)
		{
		    pList = YG_List_Create_From_Pool(g_pListPool,sizeof(CACHE_OBJECT));
		    if (pList)
		    {
			if (pCache == &g_SharkCache)
			{
			    TranslateRange(pObj,pList);
			}
			else
			    AddToRangeList(pList,pObj);
			if (g_dwMoverDrive == g_dwSharkDrive && pCache == &g_SharkCache)
			    WriteRangesToCache(pList,pCache,g_pMoverCache,pBlkMoverData);
			else
			    WriteRanges(pList,pCache,pBlkMoverData);
			YG_List_Destroy_To_Pool(pList,g_pListPool,g_pCacheObjNodePool);
		    }
		}
		pObj->bDirty = FALSE;
		// Must restart from the begining
		pNextObj = pCache->pLRUHead;
	    }
	    else
	    {
		while (MergeCacheObjects(pCache,pObj));
		pNextObj = pObj->pNext;
		if (pObj->MemRange.dwSectors <= dwSectorsLeft)
		{
		    DeleteFromLRUList(pCache,pObj);
		    YG_List_Remove(pCache->pObjList,pObj);
		    YG_List_Attach_Tail(pFreeCacheList,pObj);
		    dwSectorsLeft -= pObj->MemRange.dwSectors;
		}
		else
		{
		    pNewObj = (PCACHE_OBJECT)YG_List_Allocate_From_Pool(pFreeCacheList,g_pCacheObjNodePool);
		    pNewObj->MemRange.dwSecOfs	= pObj->MemRange.dwSecOfs;
		    pNewObj->MemRange.dwSectors = dwSectorsLeft;
		    pObj->MemRange.dwSecOfs  += dwSectorsLeft;
		    pObj->MemRange.dwSectors -= dwSectorsLeft;
		    if (pObj->DiskRange.dwSecAddr > 0 && pObj->DiskRange.dwSectors > 0)
		    {
			pObj->DiskRange.dwSecAddr += dwSectorsLeft;
			pObj->DiskRange.dwSectors -= dwSectorsLeft;
		    }
		    YG_List_Attach_Tail(pFreeCacheList,pNewObj);
		    dwSectorsLeft = 0;
		}
	    }
	}
    }
    return dwSectorsLeft == 0;
}

BOOL MergeCacheObjects(PDISK_CACHE pCache, PCACHE_OBJECT pObj)
{
    PCACHE_OBJECT   pCacheObj;
    PCACHE_OBJECT   pNextCacheObj;

    for (pCacheObj = pCache->pLRUHead; pCacheObj; pCacheObj = pNextCacheObj)
    {
	pNextCacheObj = pCacheObj->pNext;
	if (pCacheObj->dwUseCnt == 0 && pCacheObj != pObj)
	{
	    if (!pCacheObj->bDirty)
	    {
		if (pObj->MemRange.dwSecOfs+pObj->MemRange.dwSectors == pCacheObj->MemRange.dwSecOfs)
		{
		    pObj->MemRange.dwSectors += pCacheObj->MemRange.dwSectors;
		    // Nullify the current object
		    pObj->DiskRange.dwSecAddr = 0;
		    pObj->DiskRange.dwSectors = 0;
		    DeleteFromLRUList(pCache,pCacheObj);
		    YG_List_Remove(pCache->pObjList,pCacheObj);
		    YG_List_Deallocate_To_Pool(pCache->pObjList,pCacheObj,g_pCacheObjNodePool);
		    return TRUE;
		}
		else
		{
		    if (pCacheObj->MemRange.dwSecOfs+pCacheObj->MemRange.dwSectors == pObj->MemRange.dwSecOfs)
		    {
			pObj->MemRange.dwSecOfs  -= pCacheObj->MemRange.dwSectors;
			pObj->MemRange.dwSectors += pCacheObj->MemRange.dwSectors;
			// Nullify the current object
			pObj->DiskRange.dwSecAddr = 0;
			pObj->DiskRange.dwSectors = 0;
			DeleteFromLRUList(pCache,pCacheObj);
			YG_List_Remove(pCache->pObjList,pCacheObj);
			YG_List_Deallocate_To_Pool(pCache->pObjList,pCacheObj,g_pCacheObjNodePool);
			return TRUE;
		    }
		}
	    }
	}
    }
    return FALSE;
}
BOOL CreateObjUnAllocatedRegion(PDISK_CACHE pCache, PBLKMOVER_DATA pBlkMoverData, PCACHE_OBJECT *pCacheObj,
				PDWORD pdwLeft, DWORD dwRight, DWORD dwObjSize, PYG_LIST pBMObjList)
{
    PCACHE_OBJECT   pNewObj;
    PCACHE_OBJECT   pObj;
    PBM_OBJECT	    pBMObj;
    PCACHE_OBJECT   pFirstCacheObj;
    PCACHE_OBJECT   pFreeCacheObj;
    PCACHE_OBJECT   pNextFreeCacheObj;
    PYG_LIST	    pFreeCacheList;
    DWORD	    dwSectors;
    DWORD	    dwSecOfs;
    DWORD	    dwEnd;

    // This function process the unallocated region and returns a pointer to
    // the cache object that aligns with *pdwLeft

    if (pCacheObj)
	pObj = *pCacheObj;
    else
	pObj = NULL;
    if (pObj)
	dwEnd = MIN(dwRight,pObj->DiskRange.dwSecAddr);
    else
	dwEnd = dwRight;
    dwSectors = dwEnd-*pdwLeft;
    // Create a new object to represent the region
    pNewObj = YG_List_Allocate_From_Pool(pCache->pObjList,g_pCacheObjNodePool);
    ASSERT(pNewObj);
    pNewObj->MemRange.dwSectors = dwSectors;
    pNewObj->DiskRange.dwSecAddr = *pdwLeft;
    pNewObj->DiskRange.dwSectors = dwSectors;
    pNewObj->dwUseCnt = 1;
    pNewObj->bLoaded  = FALSE;
    pNewObj->bDirty  = FALSE;
    if (pObj)
	YG_List_Insert_Before(pCache->pObjList,pNewObj,pObj);
    else
	YG_List_Attach_Tail(pCache->pObjList,pNewObj);
    // Add to tail of LRU list
    InsertToLRUList(pCache,pNewObj,NULL);

    pFreeCacheList = YG_List_Create_From_Pool(g_pListPool,sizeof(CACHE_OBJECT));
    ASSERT(pFreeCacheList);
    if (!AllocateCache(pCache,dwSectors,pBlkMoverData,pFreeCacheList))
    {
	DeleteFromLRUList(pCache,pNewObj);
	YG_List_Remove(pCache->pObjList,pNewObj);
	YG_List_Deallocate_To_Pool(pCache->pObjList,pNewObj,g_pCacheObjNodePool);
	return FALSE;
    }
    pFirstCacheObj = YG_List_Get_First(pFreeCacheList);
    for (pFreeCacheObj = pFirstCacheObj;
	 pFreeCacheObj && pNewObj->DiskRange.dwSectors > 0;
	 pFreeCacheObj = pNextFreeCacheObj)
    {
	ASSERT(pFreeCacheObj->MemRange.dwSecOfs+pFreeCacheObj->MemRange.dwSectors <= pCache->dwCacheSize);
	pNextFreeCacheObj = YG_List_Get_Next(pFreeCacheList,pFreeCacheObj);
	pFreeCacheObj->DiskRange.dwSecAddr = pNewObj->DiskRange.dwSecAddr;
	pFreeCacheObj->DiskRange.dwSectors = pFreeCacheObj->MemRange.dwSectors;
	pNewObj->DiskRange.dwSecAddr += pFreeCacheObj->MemRange.dwSectors;
	pNewObj->DiskRange.dwSectors -= pFreeCacheObj->MemRange.dwSectors;
	pFreeCacheObj->dwUseCnt = 1;
	pFreeCacheObj->bLoaded	= FALSE;
	pFreeCacheObj->bDirty  = FALSE;
	YG_List_Remove(pFreeCacheList,pFreeCacheObj);
	YG_List_Insert_Before(pCache->pObjList,pFreeCacheObj,pNewObj);
	InsertToLRUList(pCache,pFreeCacheObj,NULL);
	// Create a BM object to match with the cache object
	pBMObj = (PBM_OBJECT)YG_List_Allocate_From_Pool(pBMObjList,g_pBMObjNodePool);
	ASSERT(pBMObj);
	if (pBMObj)
	{
	    pBMObj->dwSectors = pFreeCacheObj->DiskRange.dwSectors;
	    pBMObj->pCacheObj = pFreeCacheObj;
	    pBMObj->dwOfs     = 0;
	    pBMObj->dwObjSize = dwObjSize;
	    pBMObj->dwNumObjs = (pBMObj->dwSectors*SECTOR_SIZE)/dwObjSize;
	    ASSERT(SECTOR_SIZE%dwObjSize == 0);
	    YG_List_Attach_Tail(pBMObjList,pBMObj);
	}
    }
    ASSERT(!pFreeCacheObj && pNewObj->DiskRange.dwSectors == 0);
    YG_List_Destroy_To_Pool(pFreeCacheList,g_pListPool,g_pCacheObjNodePool);
    // Move left boundary to the right
    *pdwLeft = dwEnd;
    if (pCacheObj)
	*pCacheObj = YG_List_Get_Prev(pCache->pObjList,pNewObj);
    DeleteFromLRUList(pCache,pNewObj);
    YG_List_Remove(pCache->pObjList,pNewObj);
    YG_List_Deallocate_To_Pool(pCache->pObjList,pNewObj,g_pCacheObjNodePool);

    return TRUE;
}
BOOL ProcessAllocatedRegionLeft(PDISK_CACHE pCache, PCACHE_OBJECT pObj, DWORD dwSectors)
{
    DWORD	   dwEnd;
    PCACHE_OBJECT  pNewObj;

    // Split the region in half and create another object to represent the right half
    pNewObj = (PCACHE_OBJECT)YG_List_Allocate_From_Pool(pCache->pObjList,g_pCacheObjNodePool);
    if (pNewObj)
    {
	pNewObj->MemRange.dwSecOfs   = pObj->MemRange.dwSecOfs+dwSectors;
	pNewObj->MemRange.dwSectors  = pObj->MemRange.dwSectors-dwSectors;
	pNewObj->DiskRange.dwSecAddr = pObj->DiskRange.dwSecAddr+dwSectors;
	pNewObj->DiskRange.dwSectors = pObj->DiskRange.dwSectors-dwSectors;
	pNewObj->dwUseCnt = pObj->dwUseCnt;
	pNewObj->bLoaded  = pObj->bLoaded;
	pNewObj->bDirty   = pObj->bDirty;
	InsertToLRUList(pCache,pNewObj,pObj);
	// adjust the current object to exclude the right half
	pObj->MemRange.dwSectors  = dwSectors;
	pObj->DiskRange.dwSectors = dwSectors;
	YG_List_Insert(pCache->pObjList,pNewObj,pObj);
    }
    return TRUE;
}

BOOL ProcessAllocatedRegionRight(PDISK_CACHE pCache, PCACHE_OBJECT pObj, DWORD dwSectors)
{
    DWORD	   dwEnd;
    PCACHE_OBJECT  pNewObj;

    // Split the right portion of the pObj off to create another object
    // Create a new object to represent the new region on the right
    pNewObj = (PCACHE_OBJECT)YG_List_Allocate_From_Pool(pCache->pObjList,g_pCacheObjNodePool);
    if (pNewObj)
    {
	pNewObj->MemRange.dwSecOfs = pObj->MemRange.dwSecOfs+pObj->MemRange.dwSectors-dwSectors;
	pNewObj->MemRange.dwSectors = dwSectors;
	pNewObj->DiskRange.dwSecAddr = pObj->DiskRange.dwSecAddr+pObj->DiskRange.dwSectors-dwSectors;
	pNewObj->DiskRange.dwSectors = dwSectors;
	pNewObj->dwUseCnt = pObj->dwUseCnt;
	pNewObj->bLoaded = pObj->bLoaded;
	pNewObj->bDirty = pObj->bDirty;
	InsertToLRUList(pCache,pNewObj,pObj);
	// Add to list
	YG_List_Insert(pCache->pObjList,pNewObj,pObj);

	// adjust the current object to exclude this region
	pObj->MemRange.dwSectors  -= dwSectors;
	pObj->DiskRange.dwSectors -= dwSectors;
    }
    return TRUE;
}

BOOL ProcessOverlappedRegion(PDISK_CACHE pCache, PCACHE_OBJECT pObj, PDWORD pdwLeft)
{
    DWORD	dwEnd;
    DWORD	dwSectors;
    PBM_OBJECT	pBMObj;

    // We are aligned on the left boundary.
    ASSERT(*pdwLeft == pObj->DiskRange.dwSecAddr);
    pObj->dwUseCnt++;
    MoveToLRUTail(pCache,pObj);
    *pdwLeft = pObj->DiskRange.dwSecAddr+pObj->DiskRange.dwSectors;
    return TRUE;
}

BOOL CreateObjOverlappedRegion(PDISK_CACHE pCache, PCACHE_OBJECT pObj, PDWORD pdwLeft, DWORD dwObjSize,
			       PYG_LIST pBMObjList)
{
    DWORD	dwEnd;
    DWORD	dwSectors;
    PBM_OBJECT	pBMObj;

    // We are aligned on the left boundary.
    ASSERT(*pdwLeft == pObj->DiskRange.dwSecAddr);
    // Allocate a new BMObject to represent this range
    pBMObj = (PBM_OBJECT)YG_List_Allocate_From_Pool(pBMObjList,g_pBMObjNodePool);
    ASSERT(pBMObj);
    if (pBMObj)
    {
	pBMObj->dwSectors = pObj->MemRange.dwSectors;
	pBMObj->pCacheObj = pObj;
	pBMObj->dwOfs	  = 0;
	pBMObj->dwObjSize = dwObjSize;
	pBMObj->dwNumObjs = (pBMObj->dwSectors*SECTOR_SIZE)/dwObjSize;
	ASSERT(SECTOR_SIZE%dwObjSize == 0);
	YG_List_Attach_Tail(pBMObjList,pBMObj);
    }
    *pdwLeft = pObj->DiskRange.dwSecAddr+pObj->DiskRange.dwSectors;
    return TRUE;
}

BOOL AllocateCache(PDISK_CACHE pCache, DWORD dwSectors, PBLKMOVER_DATA pBlkMoverData, PYG_LIST pFreeCacheList)
{
    return FlushMergeCacheObjs(pCache,pBlkMoverData,dwSectors,pFreeCacheList);
}
// This function sets the dirty flag for the Object list
BOOL SetDirtyFlag(PYG_LIST pObjList, BOOL bDirty, PDISK_CACHE pCache)
{
    PBM_OBJECT		       pObj;
    BOOL		       bSucc;

    bSucc = TRUE;
    ASSERT(pObjList);
    for (pObj = (PBM_OBJECT)YG_List_Get_First(pObjList);
	 pObj && bSucc;
	 pObj = (PBM_OBJECT)YG_List_Get_Next(pObjList,(PVOID)pObj))
    {
	bSucc = SetDirtyFlagObject(pObj,bDirty,pCache);
    }
    return bSucc;
}
// This function frees a BM Object.
BOOL SetDirtyFlagObject(PBM_OBJECT pObj, BOOL bDirty, PDISK_CACHE pCache)
{
    PYG_LIST		    pCacheObjList;
    PCACHE_OBJECT	    pCacheObj;
    PCACHE_OBJECT	    pNextCacheObj;
    PCACHE_OBJECT	    pPrev;
    PCACHE_OBJECT	    pNext;
    BOOL			    bSucc;
    DWORD			    dwFirstSec;
    DWORD			    dwTotalSectors;

    bSucc = TRUE;
    dwTotalSectors = 0;
    dwFirstSec = pObj->pCacheObj->DiskRange.dwSecAddr;
    pCacheObjList = pCache->pObjList;
    for (pCacheObj = pObj->pCacheObj;
	 pCacheObj &&
	 bSucc	   &&
	 dwTotalSectors < pObj->dwSectors &&
	 pCacheObj->DiskRange.dwSecAddr < dwFirstSec+pObj->dwSectors;
	 pCacheObj = pNextCacheObj)
    {
	pNextCacheObj = (PCACHE_OBJECT)YG_List_Get_Next(pCacheObjList,(PVOID)pCacheObj);
	if (pCacheObj->DiskRange.dwSectors > 0)
	{
	    pCacheObj->bDirty = bDirty;
	    ASSERT(pCacheObj->dwUseCnt > 0);
	    dwTotalSectors += pCacheObj->DiskRange.dwSectors;
	}
    }
    ASSERT(dwTotalSectors == pObj->dwSectors);
    return bSucc;
}




// This function frees a BM Object list
BOOL FreeObjectList(PYG_LIST pObjList, PDISK_CACHE pCache)
{
    PBM_OBJECT		       pObj;
    BOOL		       bSucc;

    bSucc = TRUE;
    ASSERT(pObjList);
    for (pObj = (PBM_OBJECT)YG_List_Get_First(pObjList);
	 pObj && bSucc;
	 pObj = (PBM_OBJECT)YG_List_Get_Next(pObjList,(PVOID)pObj))
    {
	bSucc = FreeObject(pObj,pCache);
    }
    YG_List_Destroy_To_Pool(pObjList,g_pListPool,g_pBMObjNodePool);
    return bSucc;
}
// This function frees a BM Object.
BOOL FreeObject(PBM_OBJECT pObj, PDISK_CACHE pCache)
{
    PYG_LIST		    pCacheObjList;
    PCACHE_OBJECT	    pCacheObj;
    PCACHE_OBJECT	    pNextCacheObj;
    PCACHE_OBJECT	    pPrev;
    PCACHE_OBJECT	    pNext;
    BOOL		    bSucc;
    DWORD		    dwFirstSec;
    DWORD		    dwTotalSectors;

    bSucc = TRUE;
    dwTotalSectors = 0;
    dwFirstSec = pObj->pCacheObj->DiskRange.dwSecAddr;
    pCacheObjList = pCache->pObjList;
    for (pCacheObj = pObj->pCacheObj;
	 pCacheObj &&
	 bSucc	   &&
     dwTotalSectors < pObj->dwSectors &&
	 pCacheObj->DiskRange.dwSecAddr < dwFirstSec+pObj->dwSectors;
	 pCacheObj = pNextCacheObj)
    {
	pNextCacheObj = (PCACHE_OBJECT)YG_List_Get_Next(pCacheObjList,(PVOID)pCacheObj);
	if (pCacheObj->DiskRange.dwSectors > 0)
	{
	    ASSERT(pCacheObj->dwUseCnt > 0);
	    pCacheObj->dwUseCnt--;
	    dwTotalSectors += pCacheObj->DiskRange.dwSectors;
	}
    }
    ASSERT(dwTotalSectors == pObj->dwSectors);
    return bSucc;
}
PVOID GetFirstMemObject(PDISK_CACHE pCache, PYG_LIST pObjList, PBM_OBJECT *pObject)
{
    PBM_OBJECT	   pObj;
    PCACHE_OBJECT  pCacheObj;

    pObj = (PBM_OBJECT)YG_List_Get_First(pObjList);
    if (!pObj)
	return NULL;
    if (pObject)
	*pObject = pObj;
    pCacheObj = pObj->pCacheObj;
    return &pCache->pCache[pCacheObj->MemRange.dwSecOfs*SECTOR_SIZE+pObj->dwOfs];
}

PVOID GetNextMemObject(PDISK_CACHE pCache, PYG_LIST pObjList, PVOID pData, PBM_OBJECT *pObject)
{
    PBM_OBJECT	   pObj;
    PCACHE_OBJECT  pCacheObj;
    PBYTE	   p;
    DWORD	   dwNumObjs;
    DWORD	   dwObjSize;
    DWORD	   dwStartAddr;

    pObj = *pObject;
    p = pData;
    p += pObj->dwObjSize;
    pCacheObj = pObj->pCacheObj;
    dwStartAddr = (DWORD)&pCache->pCache[pCacheObj->MemRange.dwSecOfs*SECTOR_SIZE+pObj->dwOfs];
    dwNumObjs = pObj->dwNumObjs;
    dwObjSize = pObj->dwObjSize;
    if ((DWORD)p >= dwStartAddr+dwNumObjs*dwObjSize)
    {
	pObj = (PBM_OBJECT)YG_List_Get_Next(pObjList,pObj);
	*pObject = pObj;
	if (pObj)
	{
	    pCacheObj = pObj->pCacheObj;
	    p = (PVOID)&pCache->pCache[pCacheObj->MemRange.dwSecOfs*SECTOR_SIZE+pObj->dwOfs];
	}
	else
	    p = NULL;
    }
    return p;
}

BOOL InsertToLRUList(PDISK_CACHE pCache, PCACHE_OBJECT pNewObj, PCACHE_OBJECT pObj)
{
    PCACHE_OBJECT   pPrev;

    if (pObj)
	pPrev = pObj->pPrev;
    else
	pPrev = pCache->pLRUTail;
    if (pPrev)
	pPrev->pNext = pNewObj;
    pNewObj->pPrev = pPrev;
    pNewObj->pNext = pObj;
    if (pObj)
	pObj->pPrev = pNewObj;
    if (pCache->pLRUHead == pObj)
	pCache->pLRUHead = pNewObj;
    if (pCache->pLRUTail == pPrev)
	pCache->pLRUTail = pNewObj;
    return TRUE;
}
BOOL MoveToLRUTail(PDISK_CACHE pCache, PCACHE_OBJECT pObj)
{
    PCACHE_OBJECT   pPrev;
    PCACHE_OBJECT   pNext;

    DeleteFromLRUList(pCache,pObj);
    InsertToLRUList(pCache,pObj,NULL);
    return TRUE;
}

BOOL DeleteFromLRUList(PDISK_CACHE pCache, PCACHE_OBJECT pObj)
{
    PCACHE_OBJECT   pPrev;
    PCACHE_OBJECT   pNext;

    pPrev = pObj->pPrev;
    pNext = pObj->pNext;
    if (pPrev)
	pPrev->pNext = pNext;
    if (pNext)
	pNext->pPrev = pPrev;
    if (pCache->pLRUHead == pObj)
	pCache->pLRUHead = pNext;
    if (pCache->pLRUTail == pObj)
	pCache->pLRUTail = pPrev;
    return TRUE;
}

BOOL GetImgSectorGroup(PGET_SEC_GROUP pGetGroup, DWORD dwLogicalSec, DWORD dwSectors, PSEC_GROUP pGroup)
{
    PSEC_GROUP	pCurGroup;
    PSEC_GROUP	pImgSecGroup;
    DWORD	dwRangeLeft;
    DWORD	dwRangeRight;
    DWORD	dwGroupLeft;
    DWORD	dwGroupRight;
    DWORD	dwCurSec;
    DWORD	i;


    if (!pGetGroup)
	return FALSE;
    dwRangeLeft = dwLogicalSec;
    dwRangeRight = dwLogicalSec+dwSectors;
    pCurGroup = pGetGroup->pCurGroup;
    if (!pCurGroup)
    {
	// First time, find the first group in range
	dwCurSec = 0;
	pImgSecGroup = (PSEC_GROUP)YG_List_Get_First(g_pImgSecAddrList);
	while (pImgSecGroup)
	{
	    dwGroupLeft = dwCurSec;
	    dwGroupRight = dwCurSec+pImgSecGroup->dwSectors;
	    if (MIN(dwGroupRight,dwRangeRight) > MAX(dwGroupLeft,dwRangeLeft))
	    {
		pCurGroup = pImgSecGroup;
		break;
	    }
	    dwCurSec += pImgSecGroup->dwSectors;
	    pImgSecGroup = (PSEC_GROUP)YG_List_Get_Next(g_pImgSecAddrList,pImgSecGroup);
	}
    }
    else
    {
	dwCurSec =  pGetGroup->dwCurLogicalSecAddr+pCurGroup->dwSectors;
	pCurGroup = (PSEC_GROUP)YG_List_Get_Next(g_pImgSecAddrList,pCurGroup);
    }
    if (pCurGroup)
    {
	dwGroupLeft = dwCurSec;
	dwGroupRight = dwCurSec+pCurGroup->dwSectors;
	if (MIN(dwGroupRight,dwRangeRight) > MAX(dwGroupLeft,dwRangeLeft))
	{
	    pGroup->dwStartSec = pCurGroup->dwStartSec+MAX(dwGroupLeft,dwRangeLeft)-dwCurSec;
	    pGroup->dwSectors = MIN(dwGroupRight,dwRangeRight)-MAX(dwGroupLeft,dwRangeLeft);
	    pGetGroup->dwCurLogicalSecAddr = dwCurSec;
	    pGetGroup->pCurGroup = pCurGroup;
	    return TRUE;
	}
    }
    return FALSE;
}

BOOL CopyRangeFromCache(PYG_LIST pObjList, PCACHE_OBJECT pObj, PDISK_CACHE pCache, PDISK_CACHE pFromCache)
{
    DWORD	    dwRangeLeft;
    DWORD	    dwRangeRight;
    DWORD	    dwCacheLeft;
    DWORD	    dwCacheRight;
    DWORD	    dwCommonLeft;
    DWORD	    dwCommonRight;
    DWORD	    dwSectorsBefore;
    DWORD	    dwSectorsAfter;
    PCACHE_OBJECT   pCacheObj;
    PCACHE_OBJECT   pNewObj;
    PBYTE	    pSrc;
    PBYTE	    pTar;

    dwRangeLeft  = pObj->DiskRange.dwSecAddr;
    dwRangeRight = dwRangeLeft+pObj->DiskRange.dwSectors;
    for (pCacheObj = (PCACHE_OBJECT)YG_List_Get_First(pFromCache->pObjList);
	 pCacheObj;
	 pCacheObj = (PCACHE_OBJECT)YG_List_Get_Next(pFromCache->pObjList,pCacheObj))
    {
	dwCacheLeft  = pCacheObj->DiskRange.dwSecAddr;
	dwCacheRight = dwCacheLeft+pCacheObj->DiskRange.dwSectors;
	if (dwCacheLeft >= dwRangeRight)
	    return TRUE;
	dwCommonRight = MIN(dwCacheRight,dwRangeRight);
	dwCommonLeft = MAX(dwCacheLeft,dwRangeLeft);
	if (dwCommonRight > dwCommonLeft)
	{
	    // To regions overlap. Copy data
	    pTar = &pCache->pCache[(dwCommonLeft-dwRangeLeft+pObj->MemRange.dwSecOfs)*SECTOR_SIZE];
	    pSrc = &pFromCache->pCache[(dwCommonLeft-dwCacheLeft+pCacheObj->MemRange.dwSecOfs)*SECTOR_SIZE];
	    memcpy(pTar,pSrc,(dwCommonRight-dwCommonLeft)*SECTOR_SIZE);

	    dwSectorsBefore = dwCommonLeft-dwRangeLeft;
	    dwSectorsAfter  = dwRangeRight-dwCommonRight;
	    if (dwSectorsBefore > 0)
	    {
		pObj->DiskRange.dwSectors = dwSectorsBefore;
		pObj->MemRange.dwSectors = dwSectorsBefore;
		if (dwSectorsAfter > 0)
		{
		    pNewObj = YG_List_Allocate_From_Pool(pObjList,g_pCacheObjNodePool);
		    if (pNewObj)
		    {
			pNewObj->DiskRange.dwSecAddr = dwCommonRight;
			pNewObj->DiskRange.dwSectors = dwSectorsAfter;
			pNewObj->MemRange.dwSecOfs   = pObj->MemRange.dwSecOfs+dwCommonRight-dwRangeLeft;
			pNewObj->MemRange.dwSectors  = dwSectorsAfter;
			YG_List_Insert(pObjList,pNewObj,pObj);
		    }
		}
	    }
	    else
	    {
		if (dwSectorsAfter > 0)
		{
		    pObj->DiskRange.dwSecAddr  = dwCommonRight;
		    pObj->DiskRange.dwSectors  = dwSectorsAfter;
		    pObj->MemRange.dwSecOfs    = pObj->MemRange.dwSecOfs+dwCommonRight-dwRangeLeft;
		    pObj->MemRange.dwSectors   = dwSectorsAfter;
		}
		else
		{
		    pObj->DiskRange.dwSecAddr  = 0;
		    pObj->DiskRange.dwSectors  = 0;
		    pObj->MemRange.dwSecOfs    = 0;
		    pObj->MemRange.dwSectors   = 0;
		}
	    }
	    dwRangeLeft  = pObj->DiskRange.dwSecAddr;
	    dwRangeRight = dwRangeLeft+pObj->DiskRange.dwSectors;
	}
    }
    return TRUE;
}

BOOL CopyRangeToCache(PYG_LIST pObjList, PCACHE_OBJECT pObj, PDISK_CACHE pCache, PDISK_CACHE pToCache)
{
    DWORD	    dwRangeLeft;
    DWORD	    dwRangeRight;
    DWORD	    dwCacheLeft;
    DWORD	    dwCacheRight;
    DWORD	    dwCommonLeft;
    DWORD	    dwCommonRight;
    DWORD	    dwSectorsBefore;
    DWORD	    dwSectorsAfter;
    PCACHE_OBJECT   pCacheObj;
    PCACHE_OBJECT   pNewObj;
    PBYTE	    pSrc;
    PBYTE	    pTar;

    dwRangeLeft  = pObj->DiskRange.dwSecAddr;
    dwRangeRight = dwRangeLeft+pObj->DiskRange.dwSectors;
    for (pCacheObj = (PCACHE_OBJECT)YG_List_Get_First(pToCache->pObjList);
	 pCacheObj;
	 pCacheObj = (PCACHE_OBJECT)YG_List_Get_Next(pToCache->pObjList,pCacheObj))
    {
	dwCacheLeft  = pCacheObj->DiskRange.dwSecAddr;
	dwCacheRight = dwCacheLeft+pCacheObj->DiskRange.dwSectors;
	if (dwCacheLeft >= dwRangeRight)
	    return TRUE;
	dwCommonRight = MIN(dwCacheRight,dwRangeRight);
	dwCommonLeft = MAX(dwCacheLeft,dwRangeLeft);
	if (dwCommonRight > dwCommonLeft)
	{
	    // To regions overlap. Copy data
	    pSrc = &pCache->pCache[(dwCommonLeft-dwRangeLeft+pObj->MemRange.dwSecOfs)*SECTOR_SIZE];
	    pTar = &pToCache->pCache[(dwCommonLeft-dwCacheLeft+pCacheObj->MemRange.dwSecOfs)*SECTOR_SIZE];
	    memcpy(pTar,pSrc,(dwCommonRight-dwCommonLeft)*SECTOR_SIZE);

	    dwSectorsBefore = dwCommonLeft-dwRangeLeft;
	    dwSectorsAfter  = dwRangeRight-dwCommonRight;
	    if (dwSectorsBefore > 0)
	    {
		pObj->DiskRange.dwSectors = dwSectorsBefore;
		pObj->MemRange.dwSectors = dwSectorsBefore;
		if (dwSectorsAfter > 0)
		{
		    pNewObj = YG_List_Allocate_From_Pool(pObjList,g_pCacheObjNodePool);
		    if (pNewObj)
		    {
			pNewObj->DiskRange.dwSecAddr = dwCommonRight;
			pNewObj->DiskRange.dwSectors = dwSectorsAfter;
			pNewObj->MemRange.dwSecOfs   = pObj->MemRange.dwSecOfs+dwCommonRight-dwRangeLeft;
			pNewObj->MemRange.dwSectors  = dwSectorsAfter;
			YG_List_Insert(pObjList,pNewObj,pObj);
		    }
		}
	    }
	    else
	    {
		if (dwSectorsAfter > 0)
		{
		    pObj->DiskRange.dwSecAddr  = dwCommonRight;
		    pObj->DiskRange.dwSectors  = dwSectorsAfter;
		    pObj->MemRange.dwSecOfs    = pObj->MemRange.dwSecOfs+dwCommonRight-dwRangeLeft;
		    pObj->MemRange.dwSectors   = dwSectorsAfter;
		}
		else
		{
		    pObj->DiskRange.dwSecAddr  = 0;
		    pObj->DiskRange.dwSectors  = 0;
		    pObj->MemRange.dwSecOfs    = 0;
		    pObj->MemRange.dwSectors   = 0;
		}
	    }
	    dwRangeLeft  = pObj->DiskRange.dwSecAddr;
	    dwRangeRight = dwRangeLeft+pObj->DiskRange.dwSectors;
	}
    }
    return TRUE;
}


