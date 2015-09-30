// LowLoc.C
// Copyright (C) 1999 by YG Corp.
// All rights reserved
// Author:  ZL
// Date:    22-Nov-1999

#include    "iosdcls.h"    // VMM and IOS headers
#include    <vwin32.h>
#include    <winerror.h>
#pragma hdrstop

#include    "..\..\..\..\..\vportdrv\cur_ver\Export\9x\vportdrv.h"
#include    "..\..\..\..\..\blkmover\cur_ver\sys\9x\c\share.h"
#include    "..\LowLoc.h"
#include    "LowLoc.h"

#pragma VxD_LOCKED_DATA_SEG
BOOL		bWorking = FALSE;
char		cSignStr[] = "Yg File Locater.";    //	16 bytes
char		cSignSector[SECTOR_SIZE];
VMMLIST 	lDiskList = 0;
VMM_SEMAPHORE	hIoSem = 0;

// Initialization code and data

#pragma VxD_LOCKED_CODE_SEG

extern DRP theDRP;  // device registration packet

BOOL OnSysDynamicDeviceInit()
{
    int     nCount;
    BOOL    bSucc;

    #define TOKENCOUNT	    0x01

    if(!(hIoSem = Create_Semaphore(TOKENCOUNT)))
	return	FALSE;

    //	Initialize cSignSector

    for(nCount = 0;
	nCount < sizeof(cSignSector) / sizeof(cSignStr);
	nCount++)
    {
	memcpy(cSignSector + (nCount * sizeof(cSignStr)),
	       cSignStr,
	       sizeof(cSignStr));
    }

    lDiskList = List_Create(LF_ALLOC_ERROR,sizeof(DskList));
    ASSERT(lDiskList);
    if(!lDiskList)
    {
	Destroy_Semaphore(hIoSem);
	return	FALSE;
    }

    IOS_Register(&theDRP);
    bSucc = (theDRP.DRP_reg_result == DRP_REMAIN_RESIDENT);
    if(!bSucc)
    {
	Destroy_Semaphore(hIoSem);
	List_Destroy(lDiskList);
    }

    return  bSucc;
}   // OnSysDynamicDeviceInit


BOOL OnSysDynamicDeviceExit()
{
    PDskList	p;
    PMonRdWt	q;

    if(lDiskList)
    {
	for(
	    p = (PDskList)List_Get_First(lDiskList);
	    p;
	    p = (PDskList)List_Get_Next(lDiskList,p)
	   )
	{
	    if(p->lLocater)
	    {
		List_Destroy(p->lLocater);
		#ifdef	DEBUG
		p->pPhysDcb = (PDCB)NULL;
		p->lLocater = 0;
		#endif
	    }
	}
    }
    Destroy_Semaphore(hIoSem);
    List_Destroy(lDiskList);

    #ifdef  DEBUG
    lDiskList = 0;
    #endif

    return TRUE;
}   // OnSysDynamicDeviceExit


// Asynchronous event processing:

#pragma VxD_LOCKED_CODE_SEG
#pragma VxD_LOCKED_DATA_SEG

VOID OnAsyncRequest(PAEP aep)
{
    typedef USHORT (*PEF)(PAEP);
    static PEF evproc[AEP_MAX_FUNC+1] =
	    {(PEF) OnInitialize     //	0 AEP_INITIALIZE
	    ,NULL		    //	1 AEP_SYSTEM_CRIT_SHUTDOWN
	    ,(PEF) OnBootComplete   //	2 AEP_BOOT_COMPLETE
	    ,(PEF) OnConfigDcb	    //	3 AEP_CONFIG_DCB
	    ,(PEF) OnUnconfigDcb    //	4 AEP_UNCONFIG_DCB
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
	    ,(PEF) OnUninitialize   // 15 AEP_UNINITIALIZE
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

///////////////////////////////////////////////////////////////////////////////
// VSDs needn't do anything during AEP_INITIALIZE and should always
// return success

#pragma VxD_PAGEABLE_CODE_SEG

USHORT OnInitialize(PAEP_bi_init aep)
{
    return AEP_SUCCESS;
}   // OnInitialize


#pragma VxD_PAGEABLE_CODE_SEG

USHORT OnUninitialize(PAEP_bi_uninit aep)
{
    return AEP_SUCCESS;
}   // OnUninitialize


// AEP_BOOT_COMPLETE asks us if we want to stay loaded or not.

#pragma VxD_PAGEABLE_CODE_SEG

USHORT OnBootComplete(PAEP_boot_done aep)
{
    return  AEP_SUCCESS;
}   // OnBootComplete


// AEP_CONFIG_DCB gives us the chance to hook into the calldown stack for
// every DCB in the system

#pragma VxD_PAGEABLE_CODE_SEG


USHORT OnConfigDcb(PAEP_dcb_config aep)
{
    PDCB    dcb = (PDCB)aep->AEP_d_c_dcb;

    //	We only care for our virtual physical DCB

    if(!(dcb->DCB_cmn.DCB_device_flags & DCB_DEV_PHYSICAL))
	return	AEP_SUCCESS;

    if(dcb->DCB_cmn.DCB_device_type != DCB_type_disk)
	return	AEP_SUCCESS;

    if(!IsOurVirtualDCB(dcb))
	return	AEP_SUCCESS;

    IspInsertCalldown(dcb,OnRequest,(PDDB)aep->AEP_d_c_hdr.AEP_ddb,0,dcb->DCB_cmn.DCB_dmd_flags,aep->AEP_d_c_hdr.AEP_lgn);

    return AEP_SUCCESS;
}   // OnConfigDcb

USHORT OnUnconfigDcb(PAEP_dcb_unconfig aep)
{
    return AEP_SUCCESS;
}   // OnUnconfigDcb


// Request handler:

#pragma VxD_LOCKED_CODE_SEG

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

void InsertCallBack(PIOP iop, VOID (*callback)(PIOP), ULONG refdata)
{
    IOP_callback_entry* cbp = (IOP_callback_entry*) iop->IOP_callback_ptr;
    ASSERT(cbp >= iop->IOP_callback_table && cbp < iop->IOP_callback_table + arraysize(iop->IOP_callback_table));
    cbp->IOP_CB_address = (ULONG) callback;
    cbp->IOP_CB_ref_data = refdata;
    iop->IOP_callback_ptr += sizeof(IOP_callback_entry);
}   // InsertCallBack


VOID OnRequest(PIOP iop)
{
    #define ior iop->IOP_ior

    ULONG	    ulSectorLo,ulSectorHi;
    PMonRdWt	    pMonRdWtIo;
    _BlockDev_Scatter_Gather	*sgp;
    int 	    nSectors;

    int 	    nCount;

    DWORD	    dwAccSecNum;

    DWORD	    funcode = ior.IOR_func;
    PDCB	    dcb = (PDCB)iop->IOP_physical_dcb;

    PLOC_INFO	    pLocInfo;

    BOOL	    bAction;
    BOOL	    bSucc;

    //	Are we working ?
    if(!bWorking)
    {
	DoCallDown(iop);
	return;
    }

    //	Yes, we are working !

    ior.IOR_status = IORS_SUCCESS;  // assume it'll succeed

    // dispatch function processor

    //	We are only care for READ and WRITE operation
    switch (funcode)
    {
	case	IOR_READ:	    // IOR_func == 0
	case	IOR_WRITE:	    // IOR_func == 1
	    bAction = (BOOL)funcode;

	    ulSectorLo = ior.IOR_start_addr[0];
	    ulSectorHi = ior.IOR_start_addr[1];

	    //ASSERT(ulSectorLo < dcb->DCB_actual_sector_cnt[0]);
	    ASSERT(ulSectorHi == 0);

	    if(ior.IOR_flags & IORF_SCATTER_GATHER)
	    {
		dwAccSecNum = ulSectorLo;

		sgp = (_BlockDev_Scatter_Gather *)ior.IOR_buffer_ptr;

		while(sgp->BD_SG_Count)
		{
		    nSectors = sgp->BD_SG_Count;
		    if(ior.IOR_flags & IORF_CHAR_COMMAND)
		    {
			nSectors = (nSectors+SECTOR_SIZE-1)/SECTOR_SIZE;
		    }

		    pLocInfo = (PLOC_INFO)sgp->BD_SG_Buffer_Ptr;

		    for(nCount = 0; nCount < nSectors; nCount++)
		    {
			if(bAction)
			{
			    //	Write
			    WriteSector(dwAccSecNum + nCount,ulSectorHi,1,pLocInfo + nCount,iop,dcb);
			}
			else
			{
			    //	Read, Not our's
			    DoCallDown(iop);
			    return;
			    //bSucc = ReadSector(dwAccSecNum + nCount,ulSectorHi,1,pLocInfo + nCount,iop,dcb);
			    //if(!bSucc)
			    //	  goto	  SkipRequest;

			}
		    }
		    ++sgp;
		    dwAccSecNum += nSectors;
		}   //	End While Loop
	    }
	    else
	    {
		nSectors = ior.IOR_xfer_count;
		if(ior.IOR_flags & IORF_CHAR_COMMAND)
		    nSectors = (nSectors+SECTOR_SIZE-1)/SECTOR_SIZE;

		pLocInfo = (PLOC_INFO)ior.IOR_buffer_ptr;

		for(nCount = 0; nCount < nSectors; nCount++)
		{
		    if(bAction)
		    {
			//  Write
			WriteSector(ulSectorLo + nCount,ulSectorHi,1,pLocInfo + nCount,iop,dcb);
		    }
		    else
		    {
			//  Read
			DoCallDown(iop);
			return;
			//bSucc = ReadSector(ulSectorLo + nCount,ulSectorHi,1,pLocInfo + nCount,iop,dcb);
			//if(!bSucc)
			//    goto    SkipRequest;

		    }
		}
	    }

	    break;

	default:

	    DoCallDown(iop);
	    return;
    }	// dispatch function processor

    DoCallBack(iop);		    // we're done with this request

    #undef ior
}   // OnRequest



PIOP	GetIOP(PDCB dcb)
{
    PIOP    iop;
    DWORD   dwDelta;

    dwDelta = dcb->DCB_cmn.DCB_expansion_length + FIELDOFFSET(IOP,IOP_ior);
    iop = IspCreateIop(dwDelta + sizeof(IOR) + 2 * sizeof(SGD),dwDelta,0);
    ASSERT(iop);

    //	ISP_M_FL_MUST_SUCCEED

    //	Now fill in the default linear pointer to the sgd in the IOR
    iop->IOP_ior.IOR_sgd_lin_phys = (ULONG)((PBYTE)iop + sizeof(IOP));

    //	Set initial value in IOR
    iop->IOP_ior.IOR_next	    = 0;
    iop->IOP_ior.IOR_req_vol_handle = 0;
    iop->IOP_ior.IOR_xfer_count     = 0;
    iop->IOP_ior.IOR_flags	    = IORF_HIGH_PRIORITY | IORF_BYPASS_VOLTRK | IORF_BYPASS_QUEUE | IORF_IDE_RESERVED;
    return  iop;
}   //	GetIOP


BOOL	RetIOP(PIOP iop,PDCB dcb)
{
    PBYTE   p = (PBYTE)iop;

    p = p - dcb->DCB_cmn.DCB_expansion_length;
    return  IspDeallocMem(p);
}   //	RetIOP



BOOL	IsLocatingSector(PLOC_INFO  pLoc)
{
    /*
    if(pLoc->qwLinSectorNum.qword_lo != 0)
	return	FALSE;

    if(pLoc->qwLinSectorNum.qword_hi != 0)
	return	FALSE;

    ASSERT(pLoc->dwReserve == 0);
    */
    if(memcmp(pLoc->LocSign,cSignSector + SIGN_HEAD_LEN,SIGN_LEN))
	return	FALSE;

    return  TRUE;
}   //	IsLocatingSector


BOOL	WriteSector(
		    DWORD dwSectorLo,
		    DWORD dwSectorHi,
		    DWORD dwSectors,
		    PVOID pvBuf,
		    PIOP  iop,
		    PDCB  dcb
		   )
{
    PQWORD	pPhysSec;
    PLOC_INFO	pLocSector;

    ASSERT(dwSectorHi == 0);

    pLocSector = (PLOC_INFO)pvBuf;
    if(IsLocatingSector(pLocSector))
    {
	pPhysSec = pLocSector->pqwPhysSector;
	pPhysSec->qword_lo = dwSectorLo;
	pPhysSec->qword_hi = dwSectorHi;
	return TRUE;
    }
    else
    {
	return SynReadWrite(WRITE_OPERATION,dwSectorLo,dwSectors,pvBuf,(PDCB)iop->IOP_physical_dcb);
    }
}   /*	WriteSector */

BOOL SynReadWrite(BOOL bRDWT,
		  DWORD dwStartingSector,
		  DWORD dwSectors,
		  PVOID pvBuffer,
		  PDCB	dcb)
{
    PIOP    iop;
    PIOR    ior;
    USHORT  offset;
    USHORT  size;
    PVOID   original_ptr_cd;
    DWORD   dwRet;
    ULONG   flags;
    BOOL    bResult;

    offset = (USHORT)(dcb->DCB_cmn.DCB_expansion_length + FIELDOFFSET(IOP, IOP_ior));
    size  = offset + sizeof(IOR) + dcb->DCB_max_sg_elements * sizeof(SGD);
    iop = IspCreateIop(size,offset,0);
    ASSERT(iop);
    if(NULL == iop)
	return	FALSE;

    ior = &iop->IOP_ior;

    iop->IOP_original_dcb = (ULONG)dcb;
    iop->IOP_physical_dcb = (ULONG)dcb->DCB_cmn.DCB_physical_dcb;

    ior->IOR_next = 0;
    ior->IOR_start_addr[1] = 0;
    //	Oney do ! BUG ?
    //ior->IOR_flags = IORF_VERSION_002;
    ior->IOR_flags |= IORF_VERSION_002; //  indicate that they are providing an IOR structure
    ior->IOR_private_client = offset;
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
    ior->IOR_buffer_ptr = (ULONG)pvBuffer;
    flags = //IORF_HIGH_PRIORITY |  //	move the request to the head of any internal queues
	    IORF_SYNC_COMMAND; //  |  //  synchronous command
	    //IORF_BYPASS_VOLTRK |
	    //IORF_BYPASS_QUEUE;
	    //IORF_IDE_RESERVED;

    ior->IOR_flags |= flags;

    ior->IOR_flags &= ~IORF_BYPASS_QUEUE;
    //ior->IOR_flags &= ~IORF_CHAR_COMMAND;

    //	save original call down chain entry
    original_ptr_cd = dcb->DCB_cmn.DCB_ptr_cd;
    //dcb->DCB_cmn.DCB_ptr_cd = pBlkMoverData->pChain;

    dwRet = IlbIntIoCriteria(iop);
    ASSERT(!dwRet);

    IlbInternalRequest(iop,dcb,OnRequest);

    //	restore original call down chain entry
    //dcb->DCB_cmn.DCB_ptr_cd = original_ptr_cd;
    IspDeallocMem((PVOID)((DWORD)ior - ior->IOR_private_client));
    bResult = ior->IOR_status == IORS_SUCCESS;


    if (bResult)
	return	TRUE;
    else
    {
	ASSERT(FALSE);
	return	FALSE;
    }
}   //	SynReadWrite




VMMLIST CreateOrOpenList(PDCB	dcb)
{
    PDskList	p;
    VMMLIST	list;

    list = SearchList(dcb);
    if(list)
	return	list;	//  Open List

    //	Create List

    p = (PDskList)List_Allocate(lDiskList);
    if(!p)
	return	0;
    p->pPhysDcb = dcb;
    p->lLocater = List_Create(LF_ALLOC_ERROR,sizeof(MonRdWt));
    if(!p->lLocater)
    {
	List_Deallocate(lDiskList,p);
	return	0;
    }

    Wait_Semaphore(hIoSem,BLOCK_THREAD_IDLE);

    List_Attach(lDiskList,p);

    Signal_Semaphore(hIoSem);

    return  p->lLocater;
}   //	CreateOrOpenList


VMMLIST SearchList(PDCB dcb)
{
    PDskList	p;

    Wait_Semaphore(hIoSem,BLOCK_THREAD_IDLE);

    for(
	p = List_Get_First(lDiskList);
	p;
	p = List_Get_Next(lDiskList,p)
       )
    {
	if(dcb == p->pPhysDcb)
	{
	    //	Open List

	    Signal_Semaphore(hIoSem);

	    ASSERT(p->lLocater);
	    return  p->lLocater;
	}
    }	//  End For Loop

    Signal_Semaphore(hIoSem);

    return  0;
}   //	SearchList


PMonRdWt    SearchSector(VMMLIST list,DWORD dwSecNoLo,DWORD dwSecNoHi)
{
    PMonRdWt	p;

    Wait_Semaphore(hIoSem,BLOCK_THREAD_IDLE);

    for(
	p = (PMonRdWt)List_Get_First(list);
	p;
	p = (PMonRdWt)List_Get_Next(list,p)
       )
    {
	ASSERT_MonRdWt(p);
	if(
	    p->qwPhysSector.qword_lo == dwSecNoLo   &&
	    p->qwPhysSector.qword_hi == dwSecNoHi
	  )
	{
	    break;
	}
    }

    Signal_Semaphore(hIoSem);

    return  p;
}   //	SearchSector


BOOL	IsOurVirtualDCB(PDCB  dcb)
{
    if(memcmp(dcb->DCB_vendor_id,VENDOR_ID,VENDOR_ID_LEN))
	return	FALSE;

    if(memcmp(dcb->DCB_product_id,PRODUCT_ID,PRODUCT_ID_LEN))
	return	FALSE;

    return  TRUE;
}   //	IsOurVirtualDCB


BOOL	IsOurMoverDCB(PDCB  dcb)
{
    if(!IsOurVirtualDCB(dcb))
	return	FALSE;

    if(memcmp(dcb->DCB_rev_level,MOVER_DISK,REV_LEVEL_LEN))
	return	FALSE;

    return  TRUE;
}   //	IsOurMoverDCB


BOOL	IsOurMirrorDCB(PDCB  dcb)
{
    if(!IsOurVirtualDCB(dcb))
	return	FALSE;

    if(memcmp(dcb->DCB_rev_level,MIRROR_DISK,REV_LEVEL_LEN))
	return	FALSE;

    return  TRUE;
}   //	IsOurMirrorDCB


#ifdef	DEBUG

VOID	CRLF(VOID)
{
    Debug_Printf("\r\n");
}   //	CRLF


BOOL	DebugQueryHandler(VOID)
{
    CHAR    cDebugChar;

    CRLF();
    Debug_Printf("**********    FILELOCL DEBUG INFO     **********");
    CRLF();
    Debug_Printf("[1] List Sectors in CACHE list");
    CRLF();
    Debug_Printf("Please select an option: ");

    cDebugChar = GetDebugChar();
    Debug_Printf("%c",cDebugChar);
    GetDebugChar();
    CRLF();

    switch(cDebugChar)
    {
	case	'1':
	    ListSectorNum();
	    break;

	default:
	    CRLF();
	    Debug_Printf("Invalid DEBUG option!");
	    break;
    }
    return  TRUE;
}   //	DebugQueryHandler


VOID	ListSectorNum(VOID)
{
    PDskList	p;
    int 	nCount;
    int 	nFormat;
    PMonRdWt	q;

    for(
	p = (PDskList)List_Get_First(lDiskList);
	p;
	p = (PDskList)List_Get_Next(lDiskList,p)
       )
    {
	CRLF();
	Debug_Printf("%8x\t",(DWORD)(p->pPhysDcb));
	Debug_Printf("Sectors:  ");
	CRLF();
	nCount = 0;
	nFormat = 0;

	for(
	    q = (PMonRdWt)List_Get_First(p->lLocater);
	    q;
	    q = (PMonRdWt)List_Get_Next(p->lLocater,q)
	   )
	{
	    ++nFormat;

	    if(nFormat >= 3)
	    {
		CRLF();
		nFormat = 0;
	    }
	    Debug_Printf("%8x\t",(DWORD)(q->qwPhysSector.qword_lo));
	    Debug_Printf("%4x\t",(DWORD)(q->wCount));
	}
    }
    CRLF();
    return;
}   //	ListSectorNum

#endif
