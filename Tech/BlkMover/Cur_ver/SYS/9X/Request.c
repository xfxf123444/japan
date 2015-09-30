// REQUEST.C
// Copyright (C) 1999 by YG Corp.
// All rights reserved
// Author:  ZL
// Date:    3-Sep-2000

#include    "iosdcls.h"    // VMM and IOS headers
//#include    <debug.h>
//#include	<string.h>
#include    <ifs.h>
#include    <vwin32.h>
#include    <vmmreg.h>
#include    <winerror.h>
#include    <vxdwraps.h>
#pragma hdrstop

#define     BLKMOVER_VXD

#include    "ifsmgr.h"
#include    "share.h"
#include    "partype.h"
#include    "..\..\..\..\vportdrv\cur_ver\sys\9x\c\vportdrv.h"
#include    "..\..\..\..\VFS\cur_ver\sys\9x\c\virtu_fs.h"
#include    "ERRORNO.H"
#include    "..\..\..\..\lib\hashsrch\cur_ver\export\list.h"
#include    "..\..\export\blkmover.h"
#include    "blkmover.h"


#pragma VxD_LOCKED_DATA_SEG
extern			BOOL		g_bEnableProtect;
extern			BOOL		g_bReLocate;
extern			DWORD		g_dwMinSec,g_dwMaxSec;
extern			DWORD		g_dwSysRecPages;
extern			DWORD		g_dwSysStart;
extern			DWORD		g_dwSysRecNum;
extern			DWORD		g_dwDataRecPages;
extern			DWORD		g_dwDataStart;
extern			DWORD		g_dwMovedRecNum;
extern			DWORD		g_dwFreeStart;
extern			DWORD		g_dwFreeSize;

extern  MOVING_GROUP			g_MovingGroup;
extern	PM_RELOCATE_GROUP		g_FinalGroup[32];
extern  PPM_PROTECT_GROUP		g_pProtectFreeList;
extern  PPM_PROTECT_GROUP		g_pProtectList;
extern			PM_SEC_GROUP_BUF	g_SecGroupBuf;

VOID	OnAsyncRequest(PAEP aep)
{
    typedef USHORT (*PEF)(PAEP);
    static PEF evproc[AEP_MAX_FUNC+1] =
	    {(PEF) DefaultHandler   //	0 AEP_INITIALIZE
	    ,NULL		    //	1 AEP_SYSTEM_CRIT_SHUTDOWN
	    ,(PEF) DefaultHandler   //	2 AEP_BOOT_COMPLETE
	    ,(PEF) OnConfigDcb	    //	3 AEP_CONFIG_DCB
	    ,(PEF) OnUnconfigDcb    //	4 AEP_UNCONFIG_DCB
	    ,(PEF) OnTimeOut	    //	5 AEP_IOP_TIMEOUT
	    ,NULL		    //	6 AEP_DEVICE_INQUIRY
	    ,NULL		    //	7 AEP_HALF_SEC
	    ,NULL		    //	8 AEP_1_SEC
	    ,NULL		    //	9 AEP_2_SECS
	    ,NULL		    // 10 AEP_4_SECS
	    ,NULL		    // 11 AEP_DBG_DOT_CMD
	    ,NULL		    // 12 AEP_ASSOCIATE_DCB
	    ,NULL		    // 13 AEP_REAL_MODE_HANDOFF
	    ,NULL		    // 14 AEP_SYSTEM_SHUTDOWN
	    ,(PEF) DefaultHandler   // 15 AEP_UNINITIALIZE
	    ,NULL		    // 16 AEP_DCB_LOCK
	    ,NULL		    // 17 AEP_MOUNT_VERIFY
	    ,NULL		    // 18 AEP_CREATE_VRP
	    ,NULL		    // 19 AEP_DESTROY_VRP
	    ,NULL		    // 20 AEP_REFRESH_DRIVE
	    ,NULL		    // 21 AEP_PEND_UNCONFIG_DCB
	    ,NULL		    // 22 AEP_1E_VEC_UPDATE
	    ,NULL		    // 23 AEP_CHANGE_RPM
	    };
    PEF proc;

    ASSERT(aep->AEP_func < arraysize(evproc));

    if (aep->AEP_func < arraysize(evproc) && (proc = evproc[aep->AEP_func]))
	aep->AEP_result = proc(aep);
    else
	aep->AEP_result = (USHORT) AEP_FAILURE;
}   //	OnAsyncRequest


#pragma VxD_LOCKED_CODE_SEG

void __declspec(naked) DoCallDown(PIOP iop)
{
    _asm
	{
	    // call down to next layer
	    mov     ecx, [esp+4]
	    mov     eax, [ecx]IOP.IOP_calldown_ptr
	    mov     eax, [eax]DCB_cd_entry.DCB_cd_next
	    mov     [ecx]IOP.IOP_calldown_ptr, eax
	    jmp     [eax]DCB_cd_entry.DCB_cd_io_address
	}
}   // DoCallDown

// DoCallBack handles completion of an I/O request by calling the
// previous level's callback routine.

void __declspec(naked) DoCallBack(PIOP iop)
{
    _asm
	{
	    // call back to previous layer
	    mov ecx, [esp+4]
	    sub [ecx]IOP.IOP_callback_ptr, size IOP_callback_entry
	    mov eax, [ecx]IOP.IOP_callback_ptr
	    jmp [eax]IOP_callback_entry.IOP_CB_address
	}
}   // DoCallBack

VOID	OnRequest(PIOP iop)
{
    #define ior iop->IOP_ior

    DWORD						funcode = ior.IOR_func;
    PDCB						dcb = (PDCB)iop->IOP_physical_dcb;
    PBLKMOVER_DATA				pBlkMoverData;
    DWORD						dwRdWtSectorNo;
    int 						nSectors;
    PBYTE						pbtMemData;
    _BlockDev_Scatter_Gather	*sgp,*sgpTmp;
    BOOL						bSucc;
    int 						nSectorOnMover;
    DWORD						dwClusterNum;
    DWORD						dwSectorNum;

    DWORD						dw_SG_SectorNo;
    DWORD						dw_SG_Part;
    DWORD						dwLinPart;

    pDCB_cd_entry				pCur,pPrev,pNext;
    pDCB_cd_entry				pSys;
    DWORD						dwIoMode;


    ASSERT(dcb);

    //
    //	not virtual disk
    //

    ASSERT(!IsOurVirtualDCB(dcb));
    for (pBlkMoverData = (PBLKMOVER_DATA)List_Get_First(g_lDCB);
	 pBlkMoverData;
	 pBlkMoverData = (PBLKMOVER_DATA)List_Get_Next(g_lDCB,pBlkMoverData))
    {
		if (pBlkMoverData->dcb == dcb)
		{
			break;
		}
    }

    ASSERT(pBlkMoverData);
    ior.IOR_status = IORS_SUCCESS; // assume it'll succeed

    switch (funcode)
    {
    case    IOR_READ:
    case    IOR_WRITE:
	    Wait_Semaphore(g_hSemSyncIO,BLOCK_THREAD_IDLE);
	    //if (pBlkMoverData->bWorking && pBlkMoverData == g_pMoverData)
		if (pBlkMoverData == g_pMoverData)
	    {
			if(funcode == IOR_READ)
				dwIoMode = READ_OPERATION;
			else
				dwIoMode = WRITE_OPERATION;

			dwRdWtSectorNo = ior.IOR_start_addr[0];
			pbtMemData = (PBYTE)ior.IOR_buffer_ptr;
			nSectors = ior.IOR_xfer_count;

		//  IORF_CHAR_COMMAND only to port driver
		//  convert bytes to sectors
			if(ior.IOR_flags & IORF_CHAR_COMMAND)
				nSectors = (nSectors+SECTOR_SIZE-1)/SECTOR_SIZE;

			if (ior.IOR_flags & IORF_SCATTER_GATHER)
			{
			    // This is scatter & gather method
			    //	for each s/g structure
			    sgp = (_BlockDev_Scatter_Gather *)pbtMemData;
			    dw_SG_SectorNo = dwRdWtSectorNo;
			    while(sgp->BD_SG_Count)
			    {
					if(g_bReLocate)
					{
						DoReLocate(dwIoMode,dw_SG_SectorNo,sgp->BD_SG_Count,
								(PBYTE)sgp->BD_SG_Buffer_Ptr,pBlkMoverData);
					}
					else
					{
						if(pBlkMoverData->bWorking)
						{
							ReadFromGroup(dwIoMode,dw_SG_SectorNo,sgp->BD_SG_Count,
										(PBYTE)sgp->BD_SG_Buffer_Ptr,TRUE,pBlkMoverData);
						}
						else
						{
							if(g_bEnableProtect)
							{
								ReadFromGroup(dwIoMode,dw_SG_SectorNo,sgp->BD_SG_Count,
												(PBYTE)sgp->BD_SG_Buffer_Ptr,FALSE,pBlkMoverData);
								if(dwIoMode == WRITE_OPERATION)
									ReadFromGroup(dwIoMode,dw_SG_SectorNo,sgp->BD_SG_Count,
												(PBYTE)sgp->BD_SG_Buffer_Ptr,TRUE,pBlkMoverData);
							}
							else
								SecureReadWriteSector(dwIoMode,dw_SG_SectorNo,sgp->BD_SG_Count,
												(PBYTE)sgp->BD_SG_Buffer_Ptr,pBlkMoverData);
						}
					}
					dw_SG_SectorNo += sgp->BD_SG_Count;
					++sgp;
			    }
			}
			else
			{
				if(g_bReLocate)
				{
					DoReLocate(dwIoMode,dwRdWtSectorNo,nSectors,pbtMemData,pBlkMoverData);
				}
				else
				{
					if(pBlkMoverData->bWorking)
					{
						ReadFromGroup(dwIoMode,dwRdWtSectorNo,nSectors,pbtMemData,TRUE,pBlkMoverData);
					}
					else
					{
						if(g_bEnableProtect)
						{
							ReadFromGroup(dwIoMode,dwRdWtSectorNo,nSectors,pbtMemData,FALSE,pBlkMoverData);
							if(dwIoMode == WRITE_OPERATION)
								ReadFromGroup(dwIoMode,dwRdWtSectorNo,nSectors,pbtMemData,TRUE,pBlkMoverData);
						}
						else
							SecureReadWriteSector(dwIoMode,dwRdWtSectorNo,nSectors,pbtMemData,pBlkMoverData);
					}
				}
			}
			Signal_Semaphore(g_hSemSyncIO);
			DoCallBack(iop);
			return;
		}
	    Signal_Semaphore(g_hSemSyncIO);
	    DoCallDown(iop);
	    return;
	    break;
    default:
	    DoCallDown(iop);
	    return;

    }	//  End Switch

    //DoCallBack(iop);		      // we're done with this request
    return;
    #undef ior
}   //	OnRequest

BOOL DoLinearIo(DWORD dwIoMode,
		DWORD dwStart,
		DWORD dwSectors,
		PVOID pvBuffer,
		PBLKMOVER_DATA pBlkMoverData)
{
	DWORD	i;
    DWORD   dwSecs;
	BOOL	bSearch = TRUE;
    BOOL    bSucc;
	DWORD	dwTempStart;

	bSucc = SearchFromSysGroup(dwIoMode,dwStart,dwSectors,pvBuffer,pBlkMoverData);

    return  bSucc;
}   //	DoLinear

BOOL SearchFromSysGroup(DWORD dwIoMode,	DWORD dwStart,
						DWORD dwSectors,PVOID pvBuffer,
						PBLKMOVER_DATA pBlkMoverData)
{
    DWORD					dwStartSec;
    DWORD					dwEndSec,dwCurPos;
    DWORD					dwSecs,dwStop;
	DWORD					i,k;
	DWORD					dwRecStart;
	PPM_SEC_GROUP			pGroup = NULL;
	BOOL 					bResult = TRUE;
	
	dwStartSec 				= dwStart;
	dwEndSec 				= dwStart+dwSectors;
	dwCurPos 				= dwStart;
	
	for(i=0;i<g_dwSysRecPages;i+=BUFFER_OF_PAGES)
	{
		bResult	= SecureReadWriteSector(READ_OPERATION,g_dwSysStart+i*DATA_PAGE_SIZE,
									DATA_PAGE_SIZE*BUFFER_OF_PAGES,
									g_SecGroupBuf.pGroupsBuff,pBlkMoverData);
		if(!bResult) break;
		
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
					bResult = SearchFromGroup(dwIoMode,dwCurPos,dwEndSec-dwCurPos,
											pvBuffer,pBlkMoverData);
					if(!bResult) return bResult;
					//Restore Group Buffer;
					bResult	= SecureReadWriteSector(READ_OPERATION,g_dwSysStart+i*DATA_PAGE_SIZE,
												DATA_PAGE_SIZE*BUFFER_OF_PAGES,
												g_SecGroupBuf.pGroupsBuff,pBlkMoverData);
					if(!bResult) return bResult;
						
					(PBYTE)pvBuffer += dwSecs*SECTOR_SIZE;
					dwCurPos 		+= dwSecs;
					if(dwCurPos == dwEndSec) break;
				}

				if (dwCurPos >= pGroup->dwSStartSec 
					&& dwCurPos < pGroup->dwSStartSec+pGroup->dwSize)
				{
					dwStop = MIN(dwEndSec,pGroup->dwSStartSec+pGroup->dwSize);
					dwSecs = dwStop-dwCurPos;
					bResult	= SecureReadWriteSector(READ_OPERATION,	dwCurPos-pGroup->dwSStartSec+pGroup->dwTStartSec,
												dwSecs,pvBuffer,pBlkMoverData);
					if(!bResult) return bResult;
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
		bResult = SearchFromGroup(dwIoMode,dwCurPos,dwEndSec-dwCurPos,pvBuffer,pBlkMoverData);
	}
	return bResult;
}

BOOL SearchFromGroup(DWORD dwIoMode,DWORD dwStart,
					DWORD dwSectors,PVOID pvBuffer,
					PBLKMOVER_DATA pBlkMoverData)
{
	BOOL					bProtect = FALSE;
	int						nCurPos = 0;
	DWORD					i,k;
	DWORD					dwRecStart,dwSecs,dwStop;
	DWORD					dwStartSec,dwEndSec,dwCurPos;
	PPM_SEC_GROUP			pGroup = NULL;
	PM_RELOCATE_GROUP		FinalGroup;
	BOOL 					bResult = TRUE;
	

	for(i=0;i<g_dwDataRecPages;i+=BUFFER_OF_PAGES)
	{
		bResult = SecureReadWriteSector(READ_OPERATION,
								   g_dwDataStart+i*DATA_PAGE_SIZE,
								   DATA_PAGE_SIZE*BUFFER_OF_PAGES,
								   g_SecGroupBuf.pGroupsBuff,
								   pBlkMoverData);
		if(!bResult) break;
		
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
						return FALSE;

					nCurPos ++;
				}
			}
			else
			{
					break;
			}
		}
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
			bResult = ReadWriteFromMovedGroup(dwIoMode,dwCurPos,dwSecs,pvBuffer,pBlkMoverData);
			if(!bResult) return bResult;
			(PBYTE)pvBuffer += dwSecs*SECTOR_SIZE;
			dwCurPos 		+= dwSecs;
		}

		if (dwCurPos >= g_FinalGroup[i].dwOriginal 
			&& dwCurPos < g_FinalGroup[i].dwOriginal + g_FinalGroup[i].dwSize)
		{
			// Do the second part
			dwStop = MIN(dwEndSec,g_FinalGroup[i].dwOriginal + g_FinalGroup[i].dwSize);
			dwSecs = dwStop-dwCurPos;
			bResult = ReadWriteFromMovedGroup(dwIoMode,
									dwCurPos - g_FinalGroup[i].dwOriginal + g_FinalGroup[i].dwFinal,
									dwSecs,  pvBuffer,  pBlkMoverData);
			if(!bResult) return bResult;
			(PBYTE)pvBuffer += dwSecs*SECTOR_SIZE;
			dwCurPos 		+= dwSecs;
		}
	}

	if(dwCurPos < dwEndSec)
	{
		dwSecs = dwEndSec-dwCurPos;
		bResult = ReadWriteFromMovedGroup(dwIoMode,dwCurPos,dwSecs,pvBuffer,pBlkMoverData);
	}
	return bResult;
}

BOOL ReadWriteFromMovedGroup(DWORD dwIoMode,
					   DWORD dwRWStart,
					   DWORD dwSize,
					   PVOID pvBuffer,
					   PBLKMOVER_DATA pBlkMoverData)
{
	BOOL				bResult = TRUE;
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
			}

			if(dwCur < dwMovedStart)
			{
				dwStop = MIN(dwEnd , dwMovedStart);
				dwSecs = dwStop - dwCur;
				bResult = SecureReadWriteSector(dwIoMode,
												dwCur-g_MovingGroup.dwTStart+g_MovingGroup.dwSStart,
												dwSecs,pvBuffer,pBlkMoverData);
				if(!bResult) return bResult;
				(PBYTE)pvBuffer += dwSecs*SECTOR_SIZE;
				dwCur	 		+= dwSecs;
				if(dwCur == dwEnd) return bResult;
			}

			if (dwCur >= dwMovedStart
				&& dwCur < dwMovedStart + dwMovedSize)
			{
				dwStop = MIN(dwEnd,dwMovedStart + dwMovedSize);
				dwSecs = dwStop-dwCur;
				bResult = SecureReadWriteSector(dwIoMode,dwCur,dwSecs,pvBuffer,pBlkMoverData);
				if(!bResult) return bResult;
				(PBYTE)pvBuffer += dwSecs*SECTOR_SIZE;
				dwCur		+= dwSecs;
				if(dwCur == dwEnd) return bResult;
			}

			if(dwCur < dwEnd)
			{
				dwSecs = dwEnd-dwCur;
				bResult = SecureReadWriteSector(dwIoMode,
												dwCur-g_MovingGroup.dwTStart+g_MovingGroup.dwSStart,
												dwSecs,pvBuffer,pBlkMoverData);
			}
		}
		else
			bResult = SecureReadWriteSector(dwIoMode,dwRWStart,	dwSize,pvBuffer,pBlkMoverData);
	}
	else
		bResult = SecureReadWriteSector(dwIoMode,dwRWStart,	dwSize,pvBuffer,pBlkMoverData);
	return bResult;
}

BOOL  ReadFromGroup(DWORD dwIoMode,
					DWORD dwStart,
				    DWORD dwSectors,
					PVOID pvBuffer,
					BOOL  bRelocate,
					PBLKMOVER_DATA pBlkMoverData)
{
	BOOL			bResult = TRUE;
    DWORD			dwStartSec;
    DWORD			dwEndSec;
    DWORD			dwCurPos;
    DWORD			dwStop;
    DWORD			dwSecs;

    dwStartSec	= dwStart;
    dwEndSec	= dwStart+dwSectors;
    dwCurPos	= dwStartSec;

    if (dwCurPos < g_dwOrig1st)
    {
	// Do the first part
		dwStop  = MIN(dwEndSec,g_dwOrig1st);
		dwSecs  = dwStop-dwCurPos;
		bResult = SecureReadWriteSector(
					dwIoMode,
					dwCurPos,
					dwSecs,
					(PBYTE)pvBuffer,
					pBlkMoverData);
		ASSERT(bResult);
		(PBYTE)pvBuffer += dwSecs*SECTOR_SIZE;
		dwCurPos += dwSecs;
    }

    if (dwCurPos >= g_dwOrig1st && dwCurPos < g_dwOrigLast)
    {
	// Do the second part
		dwStop = MIN(dwEndSec,g_dwOrigLast);
		dwSecs = dwStop-dwCurPos;
		if (dwSecs)
		{
			if(bRelocate)
				bResult = DoLinearIo(dwIoMode,dwCurPos,dwSecs,pvBuffer,pBlkMoverData);
			else
				bResult = SecureReadWriteSector(dwIoMode,dwCurPos,dwSecs,pvBuffer,pBlkMoverData);
			ASSERT(bResult);
			(PBYTE)pvBuffer += dwSecs*SECTOR_SIZE;
			dwCurPos += dwSecs;
		}
    }

    if (dwCurPos >= g_dwOrigLast)
    {
	// Do the third part
		dwStop = dwEndSec;
		dwSecs = dwStop-dwCurPos;
		bResult = SecureReadWriteSector(dwIoMode,dwCurPos,dwSecs,(PBYTE)pvBuffer,pBlkMoverData);
		ASSERT(bResult);
		(PBYTE)pvBuffer += dwSecs*SECTOR_SIZE;
		dwCurPos += dwSecs;
    }

    return  bResult;
}

PPM_PROTECT_GROUP GetProtectFreeGroup()
{
	PPM_PROTECT_GROUP	pProtectFreeGroup;

	pProtectFreeGroup	= g_pProtectFreeList;
	g_pProtectFreeList	= g_pProtectFreeList->pNext;
	if(g_pProtectFreeList)
		g_pProtectFreeList->pPrev = NULL;
	return pProtectFreeGroup;
}

BOOL AddToProtectGroup(DWORD dwStart,
					   DWORD dwSize,
					   PVOID pvBuffer,
					   PBLKMOVER_DATA pBlkMoverData)
{
	BOOL				bResult = TRUE;
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
		if(pProtectGroup->dwSStart > dwCurPos)
		{
			dwStop = MIN(dwEndSec,pProtectGroup->dwSStart);
			dwSecs = dwStop-dwCurPos;
			pProtectFreeGroup = GetProtectFreeGroup();
			if(!pProtectFreeGroup) return bResult;

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

			bResult = SecureReadWriteSector(WRITE_OPERATION,pProtectFreeGroup->dwFStart,
											dwSecs,pvBuffer,pBlkMoverData);
			if(!bResult) return bResult;
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
			bResult = SecureReadWriteSector(WRITE_OPERATION,
											dwCurPos - pProtectGroup->dwSStart + pProtectGroup->dwFStart,
											dwSecs,pvBuffer,pBlkMoverData);
			if(!bResult) return bResult;
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
		if(!pProtectFreeGroup) return bResult;

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

		bResult = SecureReadWriteSector(WRITE_OPERATION,pProtectFreeGroup->dwFStart,
										dwSecs,pvBuffer,pBlkMoverData);
	}
	return bResult;
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
				memcpy(&g_FinalGroup[j+1],&g_FinalGroup[j],sizeof(PM_RELOCATE_GROUP));
			}
			memcpy(&g_FinalGroup[i],&FinalGroup,sizeof(PM_RELOCATE_GROUP));
			break;
		}
	}
	if(i == nCurPos)
		memcpy(&g_FinalGroup[i],&FinalGroup,sizeof(PM_RELOCATE_GROUP));
	return TRUE;
}

BOOL	ReadWriteOnMover(BOOL  bRDWT,
			 DWORD dwStartingSector,
			 DWORD dwSectors,
			 PBYTE pBuffer,
			 PBLKMOVER_DATA pBlkMoverData,
			 PIOR  pior
			 )
{
    DWORD	dwCurSectors;
    DWORD	i;
    BOOL	bSucc;

    bSucc = TRUE;
    for (i = 0; i < dwSectors && bSucc; i += dwCurSectors)
    {
		dwCurSectors = MIN(MAX_ASYNC_SECTORS,dwSectors-i);
		bSucc = AsyncReadWriteSectors(bRDWT,dwStartingSector+i,dwCurSectors,&pBuffer[i*SECTOR_SIZE],pBlkMoverData);
    }
    if (bSucc)
		FlushSharkCache(g_pSharkCache,g_pSharkData);
    if (bSucc)
		FlushMoverCache(g_pMoverCache,g_pMoverData);
    return bSucc;
}

BOOL	AsyncReadWriteSectors(BOOL bRDWT, DWORD dwStartingSector,
			      DWORD dwSectors,
			      PBYTE pBuffer,
			      PBLKMOVER_DATA pBlkMoverData)
{
    PYG_LIST	pOrigMapObjList;
    PYG_LIST	pSecObjList;
    PBM_OBJECT	pOrigMapObj;
    PSECMAP	pOrigMap;
    PSECMAP	pOrigMapNext;
    DWORD	dwCurBlkBegin;
    PBM_OBJECT	pSecObj;
    PBYTE	pSec;
    DWORD	i;
    BOOL	bSucc;



    bSucc = TRUE;
    pOrigMapObjList = NULL;
    pSecObjList = NULL;
    pOrigMapObjList = AllocMapEntryObj(NULL,dwStartingSector,dwSectors);
    ASSERT(pOrigMapObjList);
    if (!pOrigMapObjList)
	bSucc = FALSE;
    if (bSucc)
	bSucc = ReadMapObjectList(pOrigMapObjList,g_pSharkCache,g_pSharkData);
    ASSERT(bSucc);
    // Go through the original list to load current src
    if (bSucc)
    {
	pOrigMap = (PSECMAP)GetFirstMemObject(g_pSharkCache,pOrigMapObjList,&pOrigMapObj);
	ASSERT(pOrigMap);
	if (!pOrigMap)
	    bSucc = FALSE;
	if (bSucc)
	    pOrigMapNext = (PSECMAP)GetNextMemObject(g_pSharkCache,pOrigMapObjList,pOrigMap,&pOrigMapObj);
	dwCurBlkBegin = pOrigMap->dwCurAddr;
	while (pOrigMapNext && bSucc)
	{
	    if (pOrigMap->dwCurAddr+1 != pOrigMapNext->dwCurAddr)
	    {
		// not continuous, load current block
		// Alloc src sectors maps
		if (bSucc)
		{
		    pSecObjList = AllocSectorObj(pSecObjList,dwCurBlkBegin,pOrigMap->dwCurAddr-dwCurBlkBegin+1);
		    if (!pSecObjList)
			bSucc = FALSE;
		}
		dwCurBlkBegin = pOrigMapNext->dwCurAddr;
	    }
	    pOrigMap = pOrigMapNext;
	    if (bSucc)
		pOrigMapNext = (PSECMAP)GetNextMemObject(g_pSharkCache,pOrigMapObjList,pOrigMap,&pOrigMapObj);
	}
	if (bSucc)
	{
	    pSecObjList = AllocSectorObj(pSecObjList,dwCurBlkBegin,pOrigMap->dwCurAddr-dwCurBlkBegin+1);
	    if (!pSecObjList)
		bSucc = FALSE;
	}
    }
    if (bRDWT == READ_OPERATION)
    {
	// Load the data
	if (bSucc)
	    bSucc = ReadObjectList(pSecObjList,g_pMoverCache,g_pMoverData);
	if (bSucc)
	{
	    for (pSec = (PBYTE)GetFirstMemObject(g_pMoverCache,pSecObjList,&pSecObj),
		 i = 0;
		 pSec && i < dwSectors;
		 pSec = (PBYTE)GetNextMemObject(g_pMoverCache,pSecObjList,pSec,&pSecObj),
		 i++)
	    {
		memcpy(&pBuffer[i*SECTOR_SIZE],pSec,SECTOR_SIZE);
	    }
	}
    }
    else
    {
	if (bSucc)
	{
	    for (pSec = (PBYTE)GetFirstMemObject(g_pMoverCache,pSecObjList,&pSecObj),
		 i = 0;
		 pSec && i < dwSectors;
		 pSec = (PBYTE)GetNextMemObject(g_pMoverCache,pSecObjList,pSec,&pSecObj),
		 i++)
	    {
		memcpy(pSec,&pBuffer[i*SECTOR_SIZE],SECTOR_SIZE);
	    }
	}
	if (bSucc)
	{
	    SetDirtyFlag(pSecObjList,TRUE,g_pMoverCache);
	    bSucc = WriteObjectList(pSecObjList,g_pMoverCache,g_pMoverData);
	}
    }
    if (pOrigMapObjList)
	FreeObjectList(pOrigMapObjList,g_pSharkCache);
    if (pSecObjList)
	FreeObjectList(pSecObjList,g_pMoverCache);
    return  TRUE;
}
BOOL	OnSysDynamicDeviceInit()
{
    BOOL    bSucc;

    g_lDCB = List_Create(LF_ALLOC_ERROR | LF_ASYNC,sizeof(BLKMOVER_DATA));
    if (!g_lDCB)
    {
	return	FALSE;
    }
    g_hSemSyncIO = Create_Semaphore(1);
    if(!g_hSemSyncIO)
    {
	return	FALSE;
    }

    IOS_Register(&theDRP);

    bSucc = (theDRP.DRP_reg_result == DRP_REMAIN_RESIDENT);
    if (!bSucc)
    {
	List_Destroy(g_lDCB);
	g_lDCB = NULL;
    }
    return  bSucc;
}   //	OnSysDynamicDeviceInit


BOOL	OnSysDynamicDeviceExit()
{
    if (g_lDCB)
	List_Destroy(g_lDCB);
    g_lDCB = NULL;
    if(g_hSemSyncIO)
	Destroy_Semaphore(g_hSemSyncIO);
    return TRUE;
}   //	OnSysDynamicDeviceExit


// Asynchronous event processing:

USHORT	DefaultHandler(PAEP aep)
{
    return AEP_SUCCESS;
}   //	DefaultHandler


USHORT	OnConfigDcb(PAEP_dcb_config aep)
{
    PDCB	    dcb = (PDCB)aep->AEP_d_c_dcb;
    PBLKMOVER_DATA  pMoverData;

    ASSERT(dcb);

    //
    //	I only care for the real hard disk drive , not including my virtual
    //	ones .
    //

    if ((dcb->DCB_cmn.DCB_device_flags & DCB_DEV_PHYSICAL)  &&
	(dcb->DCB_cmn.DCB_device_type == DCB_type_disk)     &&
	!IsOurVirtualDCB(dcb))
    {
	//
	//  physical, real, disk
	//
	ASSERT(g_lDCB);
	pMoverData = List_Allocate(g_lDCB);
	if(!pMoverData)
	    return  AEP_SUCCESS;

	pMoverData->dcb = dcb;
	pMoverData->dwSignature = BLKMOVER_SIG;
	pMoverData->bWorking = FALSE;
	//pMoverData->pChain = NULL;
	List_Attach_Tail(g_lDCB,pMoverData);

	IspInsertCalldown(dcb,OnRequest,(PDDB)aep->AEP_d_c_hdr.AEP_ddb,
			  0,dcb->DCB_cmn.DCB_dmd_flags,aep->AEP_d_c_hdr.AEP_lgn);
    }
    return  AEP_SUCCESS;
}   //	OnConfigDcb


// AEP_UNCONFIG_DCB informs us that the physical device represented by a DCB
// is going away

USHORT	OnUnconfigDcb(PAEP_dcb_unconfig aep)
{
    PDCB	    dcb = (PDCB)aep->AEP_d_u_dcb;
    PBLKMOVER_DATA  pMoverData;
    pDCB_cd_entry   pNext,pCur;

    BOOL	    bSucc;

    /*
    for(pMoverData = (PBLKMOVER_DATA)List_Get_First(g_lDCB);
	pMoverData;
	pMoverData =  (PBLKMOVER_DATA)List_Get_Next(g_lDCB,pMoverData))
    {
	if (pMoverData->dcb == dcb)
	{
	    //	free calldown chain
	    pCur = pMoverData->pChain;
	    while(pCur)
	    {
		pNext = (pDCB_cd_entry)pCur->DCB_cd_next;
		bSucc = _HeapFree(pCur,0);
		ASSERT(bSucc);
		pCur = pNext;
	    }

	    List_Remove(g_lDCB,pMoverData);
	    List_Deallocate(g_lDCB,pMoverData);
	    break;
	}
    }
    */
    return AEP_SUCCESS;
}   // OnUnconfigDcb


USHORT	OnTimeOut(PAEP_iop_timeout_occurred aep)
{
    PIOP    iop = (PIOP)(aep->AEP_i_t_o_iop);

    if(!IsOurVirtualDCB((PDCB)(iop->IOP_physical_dcb)))
    {
	if(iop->IOP_ior.IOR_func == IOR_READ || iop->IOP_ior.IOR_func == IOR_WRITE)
	{
	    iop->IOP_timer = iop->IOP_timer_orig;
	    return  AEP_SUCCESS;
	}
    }

    ASSERT(FALSE);
    return  AEP_FAILURE;
}   //	OnTimeOut

USHORT	OnDCBLock(PAEP_iop_timeout_occurred aep)
{
    return AEP_SUCCESS;
}   //	OnDCBLock


void  YG_IlbInternalRequest(PIOP iop, PDCB dcb)
{
    PIOR			ior;
    PDWORD			pdwSemInternalIo;
    VMM_SEMAPHORE	hSem;

    ior = &iop->IOP_ior;
    Wait_Semaphore(g_hSemInternalReq,BLOCK_THREAD_IDLE);

    hSem = Create_Semaphore(0);
    // Insert call back
    iop->IOP_callback_ptr = (DWORD)iop->IOP_callback_table;
    InsertCallBack(iop,InternalProcess,(DWORD)hSem);
    ior->IOR_status = IORS_SUCCESS_WITH_RETRY;
    DoCallDown(iop);
    Wait_Semaphore(hSem,0);
    Destroy_Semaphore(hSem);

    Signal_Semaphore(g_hSemInternalReq);
    return;
}

VOID InternalProcess(PIOP iop)
{
    PDWORD			pdwSemInternalIo;
    VMM_SEMAPHORE	hSem;

    IOP_callback_entry * cbp = (IOP_callback_entry *)iop->IOP_callback_ptr;

    hSem = (VMM_SEMAPHORE)cbp->IOP_CB_ref_data;

    Signal_Semaphore(hSem);
    return;
}


// DoCallDown passes a request to the next lower layer. Note that the
// documentation about how to do this is totally wrong: you don't just
// add sizeof(DCB_cd_entry) to the calldown pointer, you follow a
// linked list from one calldown entry to the next.

// InsertCallBack adds an entry to the callback list

void	InsertCallBack(PIOP iop, VOID (*callback)(PIOP), ULONG refdata)
{
    IOP_callback_entry* cbp = (IOP_callback_entry*) iop->IOP_callback_ptr;

    ASSERT(cbp >= iop->IOP_callback_table && cbp < iop->IOP_callback_table + arraysize(iop->IOP_callback_table));

    cbp->IOP_CB_address = (ULONG) callback;
    cbp->IOP_CB_ref_data = refdata;
    iop->IOP_callback_ptr += sizeof(IOP_callback_entry);
}   // InsertCallBack


//
//  judge whether the physical dcb is virtualized by us
//

BOOL	IsOurVirtualDCB(PDCB  dcb)
{
    if(memcmp(dcb->DCB_vendor_id,VENDOR_ID,VENDOR_ID_LEN))
	return	FALSE;

    if(memcmp(dcb->DCB_product_id,PRODUCT_ID,PRODUCT_ID_LEN))
	return	FALSE;

    return  TRUE;
}   //	IsOurVirtualDCB

BOOL	SecureReadWriteSector(
				BOOL bRDWT,
				DWORD dwStartingSector,
				DWORD dwSectors,
				PVOID pvBuffer,
				PBLKMOVER_DATA pBlkMoverData
			     )
{
    BOOL    bSucc;

    //ASSERT(dwStartingSector > 0);
    bSucc = SynReadWrite(
		bRDWT,
		dwStartingSector,
		dwSectors,
		pvBuffer,
		pBlkMoverData
		);
    ASSERT(bSucc);
    return  bSucc;
}   //	SecureReadWriteSector

BOOL	SynReadWrite(
			BOOL bRDWT,
			DWORD dwStartingSector,
			DWORD dwSectorsToTransfer,
			PVOID pvBuffer,
			PBLKMOVER_DATA pBlkMoverData
		    )
{
    PIOP    iop;
    PIOR    ior;
    USHORT  offset;
    USHORT  size;
    PDCB    dcb;
    PDCB    pPhysDcb;
    BOOL    bResult;
    pDCB_cd_entry pCallDown;
    DWORD   dwSectors;
    DWORD   dwSectorsTransferred;

    dcb		= pBlkMoverData->dcb;
    offset  = (USHORT)(dcb->DCB_cmn.DCB_expansion_length + FIELDOFFSET(IOP, IOP_ior));
    size    = offset + sizeof(IOR) + dcb->DCB_max_sg_elements * sizeof(SGD);
    iop		= IspCreateIop(size,offset,0);
    ASSERT(iop);
    if(NULL == iop)
		return	FALSE;
    ior = &iop->IOP_ior;
    //ior->IOR_private_client = offset;

    // Find the next call down ptr
    for (pCallDown = (pDCB_cd_entry)dcb->DCB_cmn.DCB_ptr_cd;
		 pCallDown;
		 pCallDown = (pDCB_cd_entry)pCallDown->DCB_cd_next)
    {
		if (pCallDown->DCB_cd_io_address == OnRequest)
		{
			break;
		}
    }
    ASSERT(pCallDown);

    pPhysDcb = (PDCB)dcb->DCB_cmn.DCB_physical_dcb;
    dwSectorsTransferred = 0;
    bResult = TRUE;
    while (dwSectorsTransferred < dwSectorsToTransfer && bResult)
    {
		dwSectors = dwSectorsToTransfer-dwSectorsTransferred;
		if (dwSectors*SECTOR_SIZE > pPhysDcb->DCB_max_xfer_len)
			dwSectors = pPhysDcb->DCB_max_xfer_len/SECTOR_SIZE;
		if (pPhysDcb->DCB_cmn.DCB_dmd_flags & DCB_dmd_phys_sgd)
			if (dwSectors*SECTOR_SIZE > (pPhysDcb->DCB_max_sg_elements-1)*0x1000)
			dwSectors = (pPhysDcb->DCB_max_sg_elements-1)*0x1000/SECTOR_SIZE;
		bResult = DoSectorTransfer(bRDWT,iop,dcb,pPhysDcb,pCallDown,
				 dwStartingSector+dwSectorsTransferred,dwSectors,
				 (PBYTE)pvBuffer+dwSectorsTransferred*SECTOR_SIZE);
		dwSectorsTransferred += dwSectors;
    }
    //IspDeallocMem((PVOID)((DWORD)ior - ior->IOR_private_client));
    IspDeallocMem((PVOID)((DWORD)ior-offset));
    return bResult;
}   //	SynReadWrite


BOOL DoSectorTransfer(BOOL bRDWT, PIOP iop, PDCB dcb, PDCB pPhysDcb, pDCB_cd_entry pCallDown,
		      DWORD dwStartingSector, DWORD dwSectors,
		      PBYTE pBuf)
{
    PIOR    ior;
    BOOL    bResult;
    ULONG   flags;

    iop->IOP_calldown_ptr = (DWORD)pCallDown;
    ior = &iop->IOP_ior;
    iop->IOP_original_dcb = (ULONG)dcb;
    iop->IOP_physical_dcb = (ULONG)pPhysDcb;




    ior->IOR_next = 0;
    ior->IOR_start_addr[1] = 0;
    ior->IOR_req_vol_handle = dcb->DCB_cmn.DCB_vrp_ptr;
    ior->IOR_sgd_lin_phys = (ULONG)(ior + 1);
    ior->IOR_num_sgds = 0;
    //	Oney's bug
    //	reference DDK ---> VSD sample drive ---> helpreq.asm (line 722)
    //	DCB_unit_number record hard drive number (for example , 0x80)
    //	DCB_drive_lttr_equiv is drive letter (A: , B: etc.)
    //	but the field of physical dcb is no meaning ?
    //ior->IOR_vol_designtr = dcb->DCB_cmn.DCB_unit_number;
    ior->IOR_vol_designtr = dcb->DCB_cmn.DCB_drive_lttr_equiv;


    //	fill in remaining IOR fields to describe request
    if(READ_OPERATION == bRDWT)
	ior->IOR_func = IOR_READ;
    else
	ior->IOR_func = IOR_WRITE;

    ior->IOR_start_addr[0] = dwStartingSector;
    ior->IOR_xfer_count = dwSectors;
    ior->IOR_buffer_ptr = (ULONG)pBuf;
    flags =
	    //IORF_HIGH_PRIORITY   |  //  move the request to the head of any internal queues
	    IORF_SWAPPER_IO	 |
	    //IORF_BYPASS_QUEUE    |
	    IORF_SYNC_COMMAND;	     //  synchronous command
	    //IORF_BYPASS_VOLTRK |
	    //IORF_IDE_RESERVED;

    ior->IOR_flags = flags;

    //ior->IOR_flags |= IORF_VERSION_002; //  indicate that they are providing an IOR structure
    //ior->IOR_flags &= ~IORF_BYPASS_QUEUE;
    ior->IOR_flags &= ~IORF_CHAR_COMMAND;
    ior->IOR_flags &= ~IORF_SCATTER_GATHER;


    //	save original call down chain entry
    //original_ptr_cd = dcb->DCB_cmn.DCB_ptr_cd;
    //dcb->DCB_cmn.DCB_ptr_cd = pBlkMoverData->pChain;

    bResult = IlbIntIoCriteria(iop);
    ASSERT(!bResult);
    // Must release semaphore here. The following function can cause
    // reentrancy problem, thus dead lock.
    //Signal_Semaphore(g_hSemSyncIO);
    YG_IlbInternalRequest(iop,dcb);
    //Wait_Semaphore(g_hSemSyncIO,BLOCK_THREAD_IDLE);

    //	restore original call down chain entry
    //dcb->DCB_cmn.DCB_ptr_cd = original_ptr_cd;

    bResult = (ior->IOR_status == IORS_SUCCESS);
    if (!bResult)
	ASSERT(FALSE);
    return  bResult;
}
