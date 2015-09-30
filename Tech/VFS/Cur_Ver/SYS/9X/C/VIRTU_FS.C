// VIRTU_FS.C -- Virtual File System
// Copyright (C) 1999 by YG Corp.
// All rights reserved
// Author:  ZL
// Date:    25-Nov-1999

#include    "iosdcls.h"    // VMM and IOS headers
#include    <vmm.h>
#include    <vwin32.h>
#include    <debug.h>
#include    <vmmreg.h>
#include    <winerror.h>
#include    <vxdwraps.h>

#define     VXD_H

#pragma hdrstop

#include    "..\..\..\..\..\vportdrv\cur_ver\sys\9x\c\vportdrv.h"
#include    "share.h"
#include    "..\..\..\..\..\lib\hashsrch\cur_ver\Vxd\c\list.h"
#include    "..\..\..\..\..\lib\hashsrch\cur_ver\Vxd\c\hashsrch.h"
#include    "virtu_fs.h"

#define     HASH_SIZE	0x4000

#pragma VxD_LOCKED_DATA_SEG
extern		DRP theDRP;  // device registration packet
BOOL		g_bWorking     = FALSE;
DWORD		g_dwFinal1st  = 0;
DWORD		g_dwFinalLast = 0;
DWORD		g_dwMinSecAddr = 0;
DWORD		g_dwMaxSecAddr = 0;
HANDLE		g_hHash = NULL;
int		g_nSysSectors = 0;
#ifdef DEBUG
DWORD		g_dwTotalSectors = 0;
#endif

#pragma VxD_LOCKED_CODE_SEG

BOOL	OnSysDynamicDeviceInit()
{
    BOOL    bSucc;

    g_nSysSectors = 0;
    #ifdef DEBUG
    g_dwTotalSectors = 0;
    #endif
    IOS_Register(&theDRP);
    bSucc = (theDRP.DRP_reg_result == DRP_REMAIN_RESIDENT);
    return  bSucc;
}   // OnSysDynamicDeviceInit


BOOL	OnSysDynamicDeviceExit()
{
    FreeFsData();
    return TRUE;
}   // OnSysDynamicDeviceExit


// Asynchronous event processing:

#pragma VxD_LOCKED_CODE_SEG
#pragma VxD_LOCKED_DATA_SEG

VOID	OnAsyncRequest(PAEP aep)
{
    typedef USHORT (*PEF)(PAEP);
    static PEF evproc[AEP_MAX_FUNC+1] =
	    {(PEF) DefaultHandler   //	0 AEP_INITIALIZE
	    ,NULL		    //	1 AEP_SYSTEM_CRIT_SHUTDOWN
	    ,(PEF) DefaultHandler   //	2 AEP_BOOT_COMPLETE
	    ,(PEF) OnConfigDcb	    //	3 AEP_CONFIG_DCB
	    ,(PEF) OnUnConfigDcb    //	4 AEP_UNCONFIG_DCB
	    ,NULL		    //	5 AEP_IOP_TIMEOUT
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
USHORT	DefaultHandler(PAEP aep)
{
    return AEP_SUCCESS;
}   //	DefaultHandler


USHORT	OnUnConfigDcb(PAEP_dcb_unconfig aep)
{
    PDCB    dcb = (PDCB)aep->AEP_d_u_dcb;

    if(IsOurMoverDCB(dcb))
    {
	//  must re-initialize variable
    }
    return  AEP_SUCCESS;
}   //	OnUnConfigDcb


// AEP_CONFIG_DCB gives us the chance to hook into the calldown stack for
// every DCB in the system

USHORT	OnConfigDcb(PAEP_dcb_config aep)
{
    PDCB		dcb = (PDCB)aep->AEP_d_c_dcb;

    ASSERT(dcb);

    //	We only care for our virtual MOVER_DISK

    if(!(dcb->DCB_cmn.DCB_device_flags & DCB_DEV_PHYSICAL))
	return	AEP_SUCCESS;

    if(!(dcb->DCB_cmn.DCB_device_type == DCB_type_disk))
	return	AEP_SUCCESS;

    if(!IsOurMoverDCB(dcb))
	return	AEP_SUCCESS;

    //
    //	We are processing MOVER_DISK disk
    //
    IspInsertCalldown(dcb,OnRequest,(PDDB)aep->AEP_d_c_hdr.AEP_ddb,0,
		      dcb->DCB_cmn.DCB_dmd_flags,aep->AEP_d_c_hdr.AEP_lgn);
    return  AEP_SUCCESS;
}   //	OnConfigDcb


// DoCallDown passes a request to the next lower layer. Note that the
// documentation about how to do this is totally wrong: you don't just
// add sizeof(DCB_cd_entry) to the calldown pointer, you follow a
// linked list from one calldown entry to the next.

void __declspec(naked) DoCallDown(PIOP iop)
{
    _asm
	{
	    // call down to next layer
	    mov     ecx, [esp+4]
	    mov     eax, [ecx]IOP.IOP_calldown_ptr
	    mov eax, [eax]DCB_cd_entry.DCB_cd_next
	    mov [ecx]IOP.IOP_calldown_ptr, eax
	    jmp [eax]DCB_cd_entry.DCB_cd_io_address
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

// InsertCallBack adds an entry to the callback list

void	InsertCallBack(PIOP iop, VOID (*callback)(PIOP), ULONG refdata)
{
    IOP_callback_entry* cbp = (IOP_callback_entry*) iop->IOP_callback_ptr;
    ASSERT(cbp >= iop->IOP_callback_table && cbp < iop->IOP_callback_table + arraysize(iop->IOP_callback_table));
    cbp->IOP_CB_address = (ULONG) callback;
    cbp->IOP_CB_ref_data = refdata;
    iop->IOP_callback_ptr += sizeof(IOP_callback_entry);
}   //	InsertCallBack


#pragma VxD_LOCKED_CODE_SEG
VOID OnRequest(PIOP iop)
{
    #define ior iop->IOP_ior
    DWORD	funcode = ior.IOR_func;
    PDCB	dcb = (PDCB) iop->IOP_physical_dcb;
    DWORD	dwRdWtSectorNo;
    int 	nSectors;
    PBYTE	pbtMemData;
    PBYTE	pSector;
    int 	nCount;
    int 	nTotal_SG_Block;
    int 	nSG_Block;
    _BlockDev_Scatter_Gather	*sgp;


    if (g_bWorking == FALSE || g_dwFinal1st == 0 || g_dwFinalLast == 0)
    {
	DoCallDown(iop);
	return;
    }

    ior.IOR_status = IORS_SUCCESS; // assume it'll succeed

    switch (funcode)
    {
	case	IOR_READ:
	case	IOR_WRITE:
		dwRdWtSectorNo = ior.IOR_start_addr[0];

		//
		//  VFS only care for the disk io ( only READ & WRITE ) on virtual partition
		//

		if (dwRdWtSectorNo >= g_dwFinal1st && dwRdWtSectorNo < g_dwFinalLast)
		{
		    // Serialize this section
		    //Wait_Semaphore(hIoSem,BLOCK_THREAD_IDLE);
		    pbtMemData = (PBYTE)ior.IOR_buffer_ptr;

		    ASSERT(pbtMemData);
		    ASSERT(ior.IOR_start_addr[1] == 0);

		    nSectors = ior.IOR_xfer_count;

		    //	Not need ? (only port driver need IORF_CHAR_COMMAND)
		    //if(ior.IOR_flags & IORF_CHAR_COMMAND)
		    //	  nSectors = (nSectors+SECTOR_SIZE-1)/SECTOR_SIZE;

		    if(ior.IOR_flags & IORF_SCATTER_GATHER)
		    {
			nTotal_SG_Block = 0;

			sgp = (_BlockDev_Scatter_Gather *)ior.IOR_buffer_ptr;
			while(sgp->BD_SG_Count)
			{
			    nSG_Block = sgp->BD_SG_Count;

			    //	Not need ? (only port driver need IORF_CHAR_COMMAND)
			    //if(ior.IOR_flags & IORF_CHAR_COMMAND)
			    //	  nSG_Block = (nSG_Block+SECTOR_SIZE-1)/SECTOR_SIZE;

			    for(nCount = 0;nCount < nSG_Block;nCount++)
			    {
				if(funcode == IOR_READ)
				{
				    if (!ReadSector(dwRdWtSectorNo+nTotal_SG_Block+nCount,(PSECTOR_REC)((PBYTE)sgp->BD_SG_Buffer_Ptr+SECTOR_SIZE*nCount)))
				    {
					memset((PVOID)((PBYTE)sgp->BD_SG_Buffer_Ptr+SECTOR_SIZE*nCount),
						GARBAGE,
						SECTOR_SIZE);
				    }
				}
				else
				{
				    WriteSector(dwRdWtSectorNo + nTotal_SG_Block + nCount,
						(PSECTOR_REC)((PBYTE)sgp->BD_SG_Buffer_Ptr+SECTOR_SIZE*nCount));
				}
			    }	//  End For Loop

			    nTotal_SG_Block += nSG_Block;
			    ++sgp;
			}   //	End While Loop
		    }	//  End If
		    else
		    {
			//  Read , Write (normal mode)

			for(nCount = 0; nCount < nSectors; nCount++)
			{
			    if(funcode == IOR_READ)
			    {
				//
				//  read operation
				//

				if (!ReadSector(dwRdWtSectorNo+nCount,(PSECTOR_REC)(pbtMemData+SECTOR_SIZE*nCount)))
				{
				    memset((PVOID)(pbtMemData+SECTOR_SIZE*nCount),
					    GARBAGE,
					    SECTOR_SIZE);
				}
			    }	//  If READ
			    else
			    {
				//
				//  write operation
				//

				WriteSector(dwRdWtSectorNo + nCount,(PSECTOR_REC)(pbtMemData+SECTOR_SIZE*nCount));
			    }	//  End Else
			}   //	End For Loop
		    }

		    //Signal_Semaphore(hIoSem);
		    //
		    //	we have already handle the operation , so skip to transfer
		    //	control to the next level driver
		    //

		    DoCallBack(iop);
		    return;

		}

		//
		//  if read or write operation not in virtual partition scope,
		//  we transfer control to the next level driver ( we not handle it )
		//

		/*
		    The very serious mistake !!!
		    DCB_DEV_IO_ACTIVE is only used by port driver

		dcb->DCB_cmn.DCB_device_flags &= ~DCB_DEV_IO_ACTIVE;
		*/
		DoCallDown(iop);
		return;

	case	IOR_MEDIA_CHECK_RESET:	//  6
	//case	  IOR_FLUSH_DRIVE:	  //  27
	//case	  IOR_FLUSH_DRIVE_AND_DISCARD:	  //  28

	    //
	    //	I understand these operation , so handle them.
	    //	I only tell the upper level driver that I have successfully
	    //	process these request.
	    //

	    DoCallBack(iop);
	    return;

	default:

	    //
	    //	These request that I couldn't understand is not handled.
	    //

	    DoCallDown(iop);
	    return;

    }	//  End Switch (funcode)

    DoCallDown(iop);

    #undef ior
}   //	OnRequest


BOOL ReadSector(DWORD dwLinSectorNum, PSECTOR_REC pBuf)
{
    DWORD	    dwIndex;
    PSECTOR_REC     *p;
    PSECTOR_REC     pSector;

    dwIndex = dwLinSectorNum % HASH_SIZE;
    p = (PSECTOR_REC *)GetObj(g_hHash,dwLinSectorNum,dwIndex);
    if (p)
    {
	pSector = *p;
	if (pSector)
	{
	    *pBuf = *pSector;
	    return TRUE;
	}
    }
    return FALSE;
}   //	ReadSector

BOOL WriteSector(DWORD dwLinSectorNum, PSECTOR_REC pBuf)
{
    DWORD	    dwIndex;
    BOOL	    bCreated;
    PSECTOR_REC     pSector;
    PSECTOR_REC     *p;

    dwIndex = dwLinSectorNum % HASH_SIZE;

    p = (PSECTOR_REC *)GetObj(g_hHash,dwLinSectorNum,dwIndex);
    if (!p)
    {
	// The sector does not exist. Create one
	pSector = (PSECTOR_REC)_HeapAllocate(SECTOR_SIZE,HEAPSWAP);
	if (pSector)
	{
	    SetObj(g_hHash,dwLinSectorNum,dwIndex,&pSector,&bCreated);
	    g_dwMinSecAddr = MIN(g_dwMinSecAddr,dwLinSectorNum);
	    g_dwMaxSecAddr = MAX(g_dwMaxSecAddr,dwLinSectorNum);
	    g_nSysSectors++;
	}
    }
    else
    {
	// The sector already exist, overwrite it
	pSector = *p;
    }
    if (pSector)
	*pSector = *pBuf;
    return (BOOL)pSector;
}   //	WriteSector

/*
VOID	List_Insert_Wrapper(PYG_LIST list,PSECTOR_REC pSr)
{
    PSECTOR_REC pPrev,pCur;

    for(
	pPrev = NULL,pCur = (PSECTOR_REC)YG_List_Get_First(list);
	pCur;
	pCur = (PSECTOR_REC)YG_List_Get_Next(list,pCur)
       )
    {
	if(pSr->dwLogicSecNo < pCur->dwLogicSecNo)
	{
	    YG_List_Insert(list,pSr,pPrev);
	    return;
	}
	pPrev = pCur;
    }
    YG_List_Attach_Tail(list,pSr);
    return;
}   //	List_Insert_Wrapper
*/

BOOL	IsOurMoverDCB(PDCB  dcb)
{
    if(!IsOurVirtualDCB(dcb))
	return	FALSE;

    Reset_Direction_Flag();
    if(memcmp(dcb->DCB_rev_level,MOVER_DISK,REV_LEVEL_LEN))
	return	FALSE;

    return  TRUE;
}   //	IsOurMoverDCB


BOOL	IsOurVirtualDCB(PDCB  dcb)
{
    Reset_Direction_Flag();
    if(memcmp(dcb->DCB_vendor_id,VENDOR_ID,VENDOR_ID_LEN))
	return	FALSE;

    Reset_Direction_Flag();
    if(memcmp(dcb->DCB_product_id,PRODUCT_ID,PRODUCT_ID_LEN))
	return	FALSE;

    return  TRUE;
}   //	IsOurVirtualDCB


BOOL	GetParameterForMoverDsk(PPARA_FOR_MOVER_DSK  pParInfo)
{
    VMMREGRET	RegRet;
    VMMHKEY	hKey;
    DWORD	cbData;

    RegRet = _RegOpenKey(HKEY_LOCAL_MACHINE,VFS_ID,&hKey);
    if(ERROR_SUCCESS != RegRet)
	return	FALSE;

    cbData = sizeof(PARA_FOR_MOVER_DSK);

    RegRet = _RegQueryValueEx(hKey,VIRTU_INFO,NULL,REG_BINARY,(PBYTE)pParInfo,&cbData);
    _RegCloseKey(hKey);
    _RegDeleteKey(HKEY_LOCAL_MACHINE,VFS_ID);
    _RegFlushKey(HKEY_LOCAL_MACHINE);

    if(ERROR_SUCCESS != RegRet)
	return	FALSE;
    else
	return	TRUE;
}   //	GetParameterForMoverDsk


int FastUpdateFSData(PDWORD pBitmap, DWORD dwBase, DWORD dwCeiling)
{
    PVOID	pObj;
    DWORD	dwIndex;
    PSECTOR_REC pSector;
    DWORD	dwFinalIndex;
    DWORD	dwSector;
    PVOID	pNextObj;
    DWORD	dwNextIndex;
    DWORD	dwNextSector;
    DWORD	dwMask;
    int 	n;
    #ifdef  DEBUG
    DWORD	dwActualSectors;
    #endif


    dwIndex = 0;
    dwSector = 0;
    dwMask = 0x01;
    ASSERT(g_hHash);
    #ifdef  DEBUG
    dwActualSectors = 0;
    #endif
    for (pObj = GetNextObjEx(g_hHash,NULL,&dwIndex,&dwSector), n = 0;
	 pObj;
	 n++)
    {
	/*
	#ifdef	DEBUG
	Debug_Printf("Update FS, n = %x, dwSector = %x\r\n",n,dwSector);
	#endif
	*/
	#ifdef	DEBUG
	dwActualSectors++;
	#endif
	dwNextIndex = dwIndex;
	dwNextSector = dwSector;
	pNextObj = GetNextObjEx(g_hHash,pObj,&dwNextIndex,&dwNextSector);
	if (dwSector >= dwBase && dwSector < dwCeiling)
	{
	    dwFinalIndex = dwSector-dwBase;
	    if (pBitmap[dwFinalIndex/32] & (dwMask << (dwFinalIndex%32)))
	    {
		// This sector is used by a file. Discard this sector
		pSector = *((PSECTOR_REC *)pObj);
		if (pSector)
		    _HeapFree(pSector,0);
		RemoveObj(g_hHash,pObj,dwIndex);
		DeallocateObj(g_hHash,pObj,dwIndex);
		g_nSysSectors--;
		#ifdef	DEBUG
		dwActualSectors--;
		#endif
	    }
	}
	pObj = pNextObj;
	dwIndex = dwNextIndex;
	dwSector = dwNextSector;
    }
    #ifdef  DEBUG
    n = 0;
    for (pObj = GetNextObjEx(g_hHash,NULL,&dwIndex,&dwSector);
	 pObj;
	 pObj = GetNextObjEx(g_hHash,pObj,&dwIndex,&dwSector))
    {
	n++;
    }
    Debug_Printf("Total File System Sectors =  %x, Calculated Sectors = %x, Actual Sector = %x\r\n",
		  g_nSysSectors,dwActualSectors,n);
    _asm {int 03h}
    #endif
    return  g_nSysSectors;
}   //	FastUpdateFSData

/*

int	BinSearch(SEC_PTR a[],int nLow,int nHigh,DWORD dwKey)
{
    int     nMid;
    DWORD   dwMidValue;

    while(nLow <= nHigh)
    {
	nMid = (nLow + nHigh) / 2;
	dwMidValue = a[nMid].dwSector;
	if(dwKey == dwMidValue)
	    return  nMid;
	else if(dwKey < dwMidValue)
	    nHigh = nMid - 1;
	else
	    nLow = nMid + 1;
    }
    return  -1;
}   //	BinSearch
*/

PBYTE GetFstNxtFsData(PBYTE pSector, PDWORD pdwIndex, PDWORD pdwSectorNum)
{
    ASSERT(g_hHash);

    return (PBYTE)GetNextObjEx(g_hHash,pSector,pdwIndex,pdwSectorNum);
}   //	GetFstNxtFsData

int GetSectorGroup(PBYTE pSector, PDWORD pdwSectorNum, int nMaxSectors)
{
    DWORD	dwSecAddr;
    PSECTOR_REC p;
    int 	n;

    p = (PSECTOR_REC)pSector;
    for (dwSecAddr = MAX(*pdwSectorNum,g_dwMinSecAddr);
	 dwSecAddr < g_dwMaxSecAddr;
	 dwSecAddr++)
    {
	if (ReadSector(dwSecAddr,p))
	    break;
    }
    if (dwSecAddr >= g_dwMaxSecAddr)
	return 0;
    *pdwSectorNum = dwSecAddr++;
    p++;
    n = 1;
    while (n < nMaxSectors)
    {
	if (!ReadSector(dwSecAddr,p))
	    break;
	dwSecAddr++;
	p++;
	n++;
    }
    #ifdef  DEBUG
    g_dwTotalSectors += n;
    Debug_Printf("Total File System Sectors =  %x, Current Group =%x, %x\r\n",
		  g_dwTotalSectors,*pdwSectorNum,n);
    #endif
    return n;
}


BOOL FreeFsData(VOID)
{
    PSECTOR_REC     pSector;
    PSECTOR_REC     *p;
    DWORD	    dwSecAddr;
    DWORD	    dwIndex;

    if (g_hHash)
    {
		for (dwSecAddr = g_dwMinSecAddr;
			 dwSecAddr < g_dwMaxSecAddr;
			 dwSecAddr++)
		{
			dwIndex = dwSecAddr%HASH_SIZE;
			p = (PSECTOR_REC *)GetObj(g_hHash,dwSecAddr,dwIndex);
			if (p)
			{
			pSector = *p;
			if (pSector)
				_HeapFree(pSector,0);
			RemoveObj(g_hHash,(PVOID)p,dwIndex);
			DeallocateObj(g_hHash,(PVOID)p,dwIndex);
			}
		}
		DestroyHashList(g_hHash);
		g_hHash = NULL;
    }
    g_nSysSectors = 0;
    #ifdef DEBUG
    g_dwTotalSectors = 0;
    #endif
    return  TRUE;
}   //FreeFsData

BOOL WriteEMBR(DWORD dwSecAddr, PBYTE pEMBR)
{
    // Make sure the sector falls with or target range
    // The make map info logic cannot handle target sectors falls outside
    // of [g_dwFinal1st,g_dwFinalLast>
    if (dwSecAddr >= g_dwFinal1st && dwSecAddr < g_dwFinalLast)
    {
	return WriteSector(dwSecAddr,(PSECTOR_REC)pEMBR);
    }
    return FALSE;
}

BOOL EnableVFS(BOOL bEnable, DWORD dwParLeft, DWORD dwParRight)
{
    if (bEnable)
    {
		if (!g_hHash)
		{
			if (!(g_hHash = CreateHashList(HASH_SIZE,sizeof(PSECTOR_REC),0)))
			{
				ASSERT(FALSE);
				return	FALSE;
			}
		}
		g_dwFinal1st = dwParLeft;
		g_dwFinalLast = dwParRight;
		g_nSysSectors = 0;
		g_dwMinSecAddr = g_dwFinalLast;
		g_dwMaxSecAddr = g_dwFinal1st;
		#ifdef DEBUG
		g_dwTotalSectors = 0;
		#endif
    }
    g_bWorking = bEnable;
    return TRUE;
}

#ifdef	DEBUG

#define CRLF	Debug_Printf("\r\n")


BOOL	DebugQueryHandler(VOID)
{
    CHAR    cDebugChar;

    CRLF;
    Debug_Printf("******************    VIRTU_FS DEBUG INFO ******************");
    CRLF;
    Debug_Printf("[1] List All Sectors in Virtual Mover Disk");
    CRLF;
    Debug_Printf("Please select an option: ");

    cDebugChar = GetDebugChar();
    CRLF;
    Debug_Printf("%c",cDebugChar);

    switch(cDebugChar)
    {
	case	'1':
	    ListSectorsInMoverDisk();
	    break;

	default:
	    CRLF;
	    Debug_Printf("Invalid VIRTU_FS DEBUG OPTION!");
	    break;
    }
    return  TRUE;
}   //	DebugQueryHandler


VOID	ListSectorsInMoverDisk(VOID)
{
    int 	    nCount = 0;

    DWORD	    dwBase = -1;
    DWORD	    dwCeiling = 0;
    int 	    nTotal = 0;

    CRLF;
    Debug_Printf("Sector Number:");
    CRLF;

    /*
    for(
	pSr = (PSECTOR_REC)YG_List_Get_First(lVFS_Pool);
	pSr;
	pSr = (PSECTOR_REC)YG_List_Get_Next(lVFS_Pool,pSr),nTotal++
       )
    {
	Debug_Printf("%8x\t",pSr->dwLogicSecNo);
	if(pSr->dwLogicSecNo < dwBase)
	    dwBase = pSr->dwLogicSecNo;

	if(pSr->dwLogicSecNo > dwCeiling)
	    dwCeiling = pSr->dwLogicSecNo;

	++nCount;
	if(nCount > 5)
	{
	    CRLF;
	    nCount = 0;
	}
    }
    */
    CRLF;
    Debug_Printf("Total Sector(s): %ld\tMin Sector Number : %8x\tMax Sector Number : %8x",nTotal,dwBase,dwCeiling);
    CRLF;
}   //	ListSectorsInMoverDisk

#endif
