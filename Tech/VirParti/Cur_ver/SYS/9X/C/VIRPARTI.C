// VIRPARTI.C -- Virtual Partition interfere I/O requests
// Copyright (C) 1999 by YG Corp.
// All rights reserved
// Author:  ZL
// Date:    24-Nov-1999

#include    "iosdcls.h"    // VMM and IOS headers
#include    <debug.h>
#include    <vwin32.h>
#include    <winerror.h>

#pragma hdrstop

#include    "..\..\..\..\..\vportdrv\cur_ver\sys\9x\c\vportdrv.h"
#include    "..\..\..\..\..\BlkMover\Cur_Ver\sys\9x\c\BlkMover.h"
#include    "share.h"
#include    "virparti.h"
#include    "misc.h"

#pragma VxD_LOCKED_DATA_SEG
extern		    DRP theDRP;  // device registration packet
PDCB		    MonitorDCB	= (PDCB)NULL;
pDCB_cd_entry	    pChain  = NULL;
PDCB		    SharkDCB  = (PDCB)NULL;
pDCB_cd_entry	    pSharkChain  = NULL;
VMMLIST 	    lDCB = 0;

DiskParams	    dpDiskPara;
RESIZE_INFO	    resize_info;
PRI2LOG_INFO	    convert_info;
DWORD		    dwStatus;
BOOL		    g_bWorking;


VMMLIST 	    lVirtual  = 0;

/*============================================================
 *  Boot Sector (FAT32) 
 *============================================================*/

#pragma VxD_LOCKED_CODE_SEG

BOOL	OnSysDynamicDeviceInit()
{
    BOOL    bSucc;

    if(NULL == (lDCB = List_Create(LF_ALLOC_ERROR | LF_ASYNC,sizeof(DCB_DATA))))
	return	FALSE;

    IOS_Register(&theDRP);
    bSucc = theDRP.DRP_reg_result == DRP_REMAIN_RESIDENT;

    if(bSucc)
	return	bSucc;
    else
    {
	List_Destroy(lDCB);
	return	bSucc;
    }
}   // OnSysDynamicDeviceInit


BOOL	OnSysDynamicDeviceExit()
{
    ASSERT(lDCB);
    List_Destroy(lDCB);
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


USHORT	DefaultHandler(PAEP aep)
{
    return AEP_SUCCESS;
}   //	DefaultHandler


// AEP_CONFIG_DCB gives us the chance to hook into the calldown stack for
// every DCB in the system

USHORT	OnConfigDcb(PAEP_dcb_config aep)
{
    PDCB		    dcb = (PDCB)aep->AEP_d_c_dcb;
    PDCB_DATA		    pData;
    DWORD		    dwExpan = 0;

    typedef	VOID(*REQ_PROC)(PIOP);
    REQ_PROC	pReqProc;

    ASSERT(dcb);

    /*
     *	we only care for physical hard disk drive
     */

    if(
	(dcb->DCB_cmn.DCB_device_flags & DCB_DEV_PHYSICAL) &&
	(dcb->DCB_cmn.DCB_device_type == DCB_type_disk)
      )
    {
	if(IsOurVirtualDCB(dcb))
	{
	    if(IsOurMirrorDCB(dcb))
	    {
		for(
		    pData = (PDCB_DATA)List_Get_First(lDCB);
		    pData;
		    pData = (PDCB_DATA)List_Get_Next(lDCB,pData)
		   )
		{
		    if(pData->dcb == MonitorDCB)
		    {
			if (pData->dwExpanLen > dcb->DCB_cmn.DCB_expansion_length)
			    dwExpan = pData->dwExpanLen - dcb->DCB_cmn.DCB_expansion_length;
			else
			    dwExpan = 0;
			pReqProc = OnRequest_OnMirrorDisk;
			break;
		    }
		}
	    }
	    else
	    {
		if (IsOurMoverDCB(dcb))
		{
		    for(
			pData = (PDCB_DATA)List_Get_First(lDCB);
			pData;
			pData = (PDCB_DATA)List_Get_Next(lDCB,pData)
		       )
		    {
			if(pData->dcb == MonitorDCB)
			{
			    if (pData->dwExpanLen > dcb->DCB_cmn.DCB_expansion_length)
				dwExpan = pData->dwExpanLen - dcb->DCB_cmn.DCB_expansion_length;
			    else
				dwExpan = 0;
			    pReqProc = OnRequest_OnMoverDisk;
			    break;
			}
		    }
		}
		else
		{
		    if (IsOurSharkDCB(dcb))
		    {
			for(
			    pData = (PDCB_DATA)List_Get_First(lDCB);
			    pData;
			    pData = (PDCB_DATA)List_Get_Next(lDCB,pData)
			   )
			{
			    if(pData->dcb == SharkDCB)
			    {
				if (pData->dwExpanLen - dcb->DCB_cmn.DCB_expansion_length)
				    dwExpan = pData->dwExpanLen - dcb->DCB_cmn.DCB_expansion_length;
				else
				    dwExpan = 0;
				pReqProc = OnRequest_OnSharkDisk;
				break;
			    }
			}
		    }
		}
	    }
	}
	else
	{
	    pReqProc = OnRequest_OnRealDisk;

	    pData = (PDCB_DATA)List_Allocate(lDCB);
	    if(!pData)
		return AEP_SUCCESS;

	    pData->dcb = dcb;
	    pData->dwSignature = DCB_DATA_SIGN;
	    pData->dwExpanLen = dcb->DCB_cmn.DCB_expansion_length;
	    pData->pChain = NULL;
	    List_Attach_Tail(lDCB,pData);

	    dwExpan = 0;
	}

	IspInsertCalldown(dcb,pReqProc,
	  (PDDB)aep->AEP_d_c_hdr.AEP_ddb,(int)dwExpan,
	  dcb->DCB_cmn.DCB_dmd_flags,
	  aep->AEP_d_c_hdr.AEP_lgn);
    }
    return AEP_SUCCESS;
}   //	OnConfigDcb


PDCB	DriveLetter2PhysDcb(BYTE    btDriveLetter)
{
    PDCB    dcb;

    for(
	dcb = IspGetFirstNextDcb((PDCB)NULL,DCB_type_disk);
	dcb;
	dcb = IspGetFirstNextDcb(dcb,DCB_type_disk)
       )
    {
	if(dcb->DCB_cmn.DCB_device_flags & DCB_DEV_LOGICAL)
	{
	    if(dcb->DCB_cmn.DCB_drive_lttr_equiv == btDriveLetter)
	    {
		return	(PDCB)dcb->DCB_cmn.DCB_physical_dcb;
	    }
	}
    }
    return  dcb;
}   //	DriveLetter2PhysDcb


// AEP_UNCONFIG_DCB informs us that the physical device represented by a DCB
// is going away

USHORT	OnUnconfigDcb(PAEP_dcb_unconfig aep)
{
    PDCB	    dcb = (PDCB)aep->AEP_d_u_dcb;
    PDCB_DATA	    pData;
    pDCB_cd_entry   pNext,pCur;

    for(
	pData = (PDCB_DATA)List_Get_First(lDCB);
	pData;
	pData = (PDCB_DATA)List_Get_Next(lDCB,pData)
       )
    {
	if(pData->dcb == dcb)
	{
	    List_Remove(lDCB,pData);
	    pCur = pData->pChain;
	    if(pCur)
	    {
		//  free call down chain

		pNext = (pDCB_cd_entry)pCur->DCB_cd_next;
		_HeapFree(pCur,0);
		pCur = pNext;
	    }
	    List_Deallocate(lDCB,pData);

	    break;
	}
    }

    return AEP_SUCCESS;
}   //	OnUnconfigDcb



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
}   // InsertCallBack


VOID	OnRequest_OnMirrorDisk(PIOP iop)
{
    #define		ior iop->IOP_ior
    DWORD		funcode = ior.IOR_func;
    PDCB		dcb = (PDCB) iop->IOP_physical_dcb;
    PCallBackProcess	pRestore;

    ASSERT(IsOurMirrorDCB(dcb));

    ior.IOR_status = IORS_SUCCESS; // assume it'll succeed

    switch (funcode)
    {
    case    IOR_WRITE:
	    //	mirror disk is a read-only disk, so we skip all write operation
	    break;

    case    IOR_READ:
	    pRestore = (PCallBackProcess)_HeapAllocate(sizeof(CallBackProcess),0);
	    ASSERT(pRestore);
	    // For some unknown reasons the TSD some times sets the
	    // "DCB_TSD_USER_SET" flag. This causes drive letters not been
	    // assigned correctly. We must make sure that the flag is
	    // not set here.
	    dcb->DCB_cmn.DCB_TSD_Flags &= ~DCB_TSD_USER_SET;

	    pRestore->ulPhysicalDCB = iop->IOP_physical_dcb;
	    pRestore->ulOriginalDCB = iop->IOP_original_dcb;
	    pRestore->ulCallDownPtr = iop->IOP_calldown_ptr;

	    iop->IOP_physical_dcb = (ULONG)MonitorDCB;
	    iop->IOP_original_dcb = (ULONG)MonitorDCB;
	    iop->IOP_calldown_ptr = (ULONG)pChain;
	    iop->IOP_srb	  = NULL;

	    InsertCallBack(iop,MirrorProcess,(ULONG)pRestore);

	    DoCallDown(iop);
	    return;
	    break;
    default:
	    DoCallDown(iop);
	    return;
	    break;
    }	//  End Switch

    DoCallBack(iop);		    // we're done with this request
    return;

    #undef ior
}   //	OnRequest_OnMirrorDisk

VOID	OnRequest_OnSharkDisk(PIOP iop)
{
    #define		ior iop->IOP_ior
    DWORD		funcode = ior.IOR_func;
    PDCB		dcb = (PDCB) iop->IOP_physical_dcb;
    PCallBackProcess	pRestore;

    ASSERT(IsOurSharkDCB(dcb));

    ior.IOR_status = IORS_SUCCESS; // assume it'll succeed

    switch (funcode)
    {
    case    IOR_WRITE:
	    //	shark disk is a read-only disk, so we skip all write operation
	    break;

    case    IOR_READ:
	    pRestore = (PCallBackProcess)_HeapAllocate(sizeof(CallBackProcess),0);
	    ASSERT(pRestore);
	    if(pRestore)
	    {
		// For some unknown reasons the TSD some times sets the
		// "DCB_TSD_USER_SET" flag. This causes drive letters not been
		// assigned correctly. We must make sure that the flag is
		// not set here.
		dcb->DCB_cmn.DCB_TSD_Flags &= ~DCB_TSD_USER_SET;

		pRestore->ulPhysicalDCB = iop->IOP_physical_dcb;
		pRestore->ulOriginalDCB = iop->IOP_original_dcb;
		pRestore->ulCallDownPtr = iop->IOP_calldown_ptr;

		iop->IOP_physical_dcb = (ULONG)SharkDCB;
		iop->IOP_original_dcb = (ULONG)SharkDCB;
		iop->IOP_calldown_ptr = (ULONG)pSharkChain;

		InsertCallBack(iop,SharkProcess,(ULONG)pRestore);

		DoCallDown(iop);
		return;
	    }
	    else
	    {
		break;	//  skip the operation
	    }
	    break;
    default:
	    DoCallDown(iop);
	    return;
	    break;
    }	//  End Switch

    DoCallBack(iop);		    // we're done with this request
    return;

    #undef ior
}   //	OnRequest_OnSharkDisk



pDCB_cd_entry	GetCallDownChain(PDCB  dcb)
{
    PDCB_DATA	    pData;
    pDCB_cd_entry   pChainHdr;

    for(
	pData = (PDCB_DATA)List_Get_First(lDCB);
	pData;
	pData = (PDCB_DATA)List_Get_Next(lDCB,pData)
       )
    {
	ASSERT(pData->dwSignature == DCB_DATA_SIGN);

	if(pData->dcb == dcb)
	{
	    ASSERT(pData->pChain);
	    return  pData->pChain;
	}
    }
    return  0;
}   //	GetCallDownChain



VOID	OnRequest_OnMoverDisk(PIOP iop)
{
    #define ior iop->IOP_ior
    DWORD		funcode = ior.IOR_func;
    PDCB		dcb = (PDCB) iop->IOP_physical_dcb;
    PCallBackProcess	pRestore;
    PBYTE		pDataBuf;
    DWORD		dwSectors;

    ASSERT(IsOurMoverDCB(dcb));


    ior.IOR_status = IORS_SUCCESS; // assume it'll succeed

    switch (funcode)
    {
    case    IOR_WRITE:

	    //
	    //	Mover disk is a read-only disk, so we skip all write operation
	    //	Virtu_Fs should intercept all WRITE operation .
	    //
	    //	we skip all WRITE operation io
	    //
	    ASSERT(FALSE);
	    break;

    case    IOR_READ:
	    ASSERT(ior.IOR_start_addr[1] == 0);

	    pRestore = (PCallBackProcess)_HeapAllocate(sizeof(CallBackProcess),0);
	    ASSERT(pRestore);
	    if(!pRestore)
		break;	//  If memory starves, we only skip the READ operation
			//  The client will read nothing .

	    pRestore->ulPhysicalDCB   = iop->IOP_physical_dcb;
	    pRestore->ulOriginalDCB   = iop->IOP_original_dcb;
	    pRestore->ulCallDownPtr   = iop->IOP_calldown_ptr;
	    pRestore->ulStartSector   = ior.IOR_start_addr[0];

	    iop->IOP_physical_dcb = (ULONG)MonitorDCB;
	    iop->IOP_original_dcb = (ULONG)MonitorDCB;
	    iop->IOP_calldown_ptr = (ULONG)pChain;

	    if (resize_info.Type == LOGICAL_PAR)
	    {
		if (ior.IOR_start_addr[0] == resize_info.StartOfVP + PARTBL2BOOTSEC_OFFSET)
		{
		    // Remap the sector to its original location.
		    // This is needed for logical partitions to be generated correctly.
		    // System must be able to read in the boot sector in order to
		    // set the logical partition.
		    ior.IOR_start_addr[0] += resize_info.StartOfMP-resize_info.StartOfVP;
		}
	    }
	    else
	    {
		if (ior.IOR_start_addr[0] == resize_info.StartOfVP)
		{
		    // Remap the sector to its original location.
		    // This is needed for logical partitions to be generated correctly.
		    // System must be able to read in the boot sector in order to
		    // set the logical partition.
		    ior.IOR_start_addr[0] += resize_info.StartOfMP-resize_info.StartOfVP;
		}
	    }
	    InsertCallBack(iop,MoverProcess,(ULONG)pRestore);
	    DoCallDown(iop);
	    return;
	    break;
    case    IOR_COMPUTE_GEOM:
	    break;
    default:
	    DoCallDown(iop);
	    return;
	    break;
    }	//  End Switch

    DoCallBack(iop);		    // we're done with this request
    return;
    #undef ior
}   //	OnRequest_OnMoverDisk


/*
 *  get the call down routine ptr of the DCB about the monitored real disk
 *
 */

VOID	OnRequest_OnRealDisk(PIOP iop)
{
    PDCB_DATA		    pData;
    pDCB_cd_entry	    pSys,pCur,pPrev,pNext;
    PDCB		    dcb;

    dcb = (PDCB) iop->IOP_physical_dcb;

    for(
	pData = (PDCB_DATA)List_Get_First(lDCB);
	pData;
	pData = (PDCB_DATA)List_Get_Next(lDCB,pData)
       )
    {
	if(pData->dcb == dcb)
	{
	    pData->dwExpanLen = dcb->DCB_cmn.DCB_expansion_length;

	    if(pData->pChain == NULL)
	    {
		//
		//  snapshot current calldown chain list
		//
		for(
		    pSys = (pDCB_cd_entry)iop->IOP_calldown_ptr;
		    pSys;
		    pSys = (pDCB_cd_entry)pSys->DCB_cd_next
		   )
		{
		    pCur = (pDCB_cd_entry)_HeapAllocate(sizeof(DCB_cd_entry),0);
		    if(!pCur)
		    {
			//  error handle
			pCur = (pDCB_cd_entry)pData->pChain;
			while(!pCur)
			{
			    pNext = (pDCB_cd_entry)pCur->DCB_cd_next;
			    _HeapFree(pCur,0);
			    pCur = pNext;
			}
			pData->pChain = NULL;

			DoCallDown(iop);
			return;
		    }
		    memcpy(pCur,pSys,sizeof(DCB_cd_entry));

		    if(!pData->pChain)
			pData->pChain = pCur;
		    else
			pPrev->DCB_cd_next = (ULONG)pCur;

		    pPrev = pCur;
		}
	    }
	}
    }

    DoCallDown(iop);
}   //	OnRequest_OnRealDisk


VOID	MirrorProcess(PIOP iop)
{
    #define ior iop->IOP_ior

    DWORD		dwRdWtHeadLo;
    PBYTE		pbtMemData;
    PBYTE		pbtParTbl;
    int 		nSectors;
    PPARTITION_ENTRY	pParTbl;
    PDCB		dcb = (PDCB) iop->IOP_physical_dcb;
    PCallBackProcess	pRestore;
    IOP_callback_entry * cbp = (IOP_callback_entry *)iop->IOP_callback_ptr;

    pRestore = (PCallBackProcess)cbp->IOP_CB_ref_data;

    iop->IOP_physical_dcb = pRestore->ulPhysicalDCB;
    iop->IOP_original_dcb = pRestore->ulOriginalDCB;
    iop->IOP_calldown_ptr = pRestore->ulCallDownPtr;

    _HeapFree(pRestore,0);

    if (ior.IOR_func == IOR_READ)
    {
	pbtMemData = (PBYTE)ior.IOR_buffer_ptr;
	dwRdWtHeadLo = ior.IOR_start_addr[0];
	nSectors = ior.IOR_xfer_count;
	if(ior.IOR_flags & IORF_CHAR_COMMAND)
	    nSectors = (nSectors+SECTOR_SIZE-1)/SECTOR_SIZE;

	SimulatePartition(dwRdWtHeadLo,nSectors,pbtMemData,
			  resize_info.MirrorType,resize_info.MirrorSysID,
			  resize_info.StartOfMP,resize_info.EndOfMP);
    }
    DoCallBack(iop);
    return;
    #undef ior
}   //	MirrorProcess

VOID	SharkProcess(PIOP iop)
{
    #define ior iop->IOP_ior

    DWORD		dwRdWtHeadLo;
    PBYTE		pbtMemData;
    PBYTE		pbtParTbl;
    int 		nSectors;
    PPARTITION_ENTRY	pParTbl;
    PDCB		dcb = (PDCB) iop->IOP_physical_dcb;
    PCallBackProcess	pRestore;
    IOP_callback_entry * cbp = (IOP_callback_entry *)iop->IOP_callback_ptr;

    pRestore = (PCallBackProcess)cbp->IOP_CB_ref_data;

    iop->IOP_physical_dcb = pRestore->ulPhysicalDCB;
    iop->IOP_original_dcb = pRestore->ulOriginalDCB;
    iop->IOP_calldown_ptr = pRestore->ulCallDownPtr;

    _HeapFree(pRestore,0);


    pbtMemData = (PBYTE)ior.IOR_buffer_ptr;
    dwRdWtHeadLo = ior.IOR_start_addr[0];
    nSectors = ior.IOR_xfer_count;
    if(ior.IOR_flags & IORF_CHAR_COMMAND)
	nSectors = (nSectors+SECTOR_SIZE-1)/SECTOR_SIZE;

    SimulatePartition(dwRdWtHeadLo,nSectors,pbtMemData,
		      resize_info.SharkType,resize_info.SharkSysID,
		      resize_info.StartOfSP,resize_info.EndOfSP);

    DoCallBack(iop);
    return;
    #undef ior
}   //	SharkProcess




VOID	MoverProcess(PIOP iop)
{
    #define ior iop->IOP_ior
    DWORD		dwRdWtHeadLo;
    PBYTE		pbtMemData;
    PBYTE		pbtParTbl;
    int 		nSectors;
    PDCB		dcb = (PDCB) iop->IOP_physical_dcb;
    PPARTITION_ENTRY	pParTbl;
    PCallBackProcess	pRestore;
    PWORD		pSign;
    DWORD		dwEndOfPar;

    PPARTITION_ENTRY	pExtParTbl;

    //
    //	First, we must restore original iop content
    //

    IOP_callback_entry * cbp = (IOP_callback_entry *)iop->IOP_callback_ptr;

    pRestore = (PCallBackProcess)cbp->IOP_CB_ref_data;

    iop->IOP_physical_dcb  = pRestore->ulPhysicalDCB;
    iop->IOP_original_dcb  = pRestore->ulOriginalDCB;
    iop->IOP_calldown_ptr  = pRestore->ulCallDownPtr;
    ior.IOR_start_addr[0]  = pRestore->ulStartSector;

    _HeapFree(pRestore,0);


    pbtMemData = (PBYTE)ior.IOR_buffer_ptr;
    dwRdWtHeadLo = ior.IOR_start_addr[0];
    nSectors = ior.IOR_xfer_count;
    if(ior.IOR_flags & IORF_CHAR_COMMAND)
	nSectors = (nSectors+SECTOR_SIZE-1)/SECTOR_SIZE;

    SimulateMoverPartition(dwRdWtHeadLo,nSectors,pbtMemData,
			   resize_info.Type,resize_info.SysID,
			   resize_info.StartOfVP,resize_info.EndOfVP);
    DoCallBack(iop);
    return;
    #undef ior
}   //	MoverProcess



//
//  return  PHYSICAL sector number
//
//  comment : T1 --- Track's 1 bit
//	      H5 --- Head's 5 bit
//	      S4 --- Sector's 4 bit
//	      XX --- no meaning     etc.
//  ________________________________________________________________________________________________
// |  |  |  |  |  |  |	|  |  |  |  |  |  |  |	|  |  |  |  |  |  |  |	|  |  |  |  |  |  |  |	|  |
// |T7|T6|T5|T4|T3|T2|T1|T0|T8|T9|S5|S4|S3|S2|S1|S0|H7|H6|H5|H4|H3|H2|H1|H0|XX|XX|XX|XX|XX|XX|XX|XX|
// |__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7	6  5  4  3  2  1  0
//

DWORD	Logic2Phys(DWORD dwLogicSectorNo)
{
    DWORD   dwTrack,dwHead,dwSector;

    dwSector = dwLogicSectorNo % dpDiskPara.dwSecPerTrack + 1;
    dwHead   = (dwLogicSectorNo / dpDiskPara.dwSecPerTrack) % dpDiskPara.dwHeads;
    dwTrack  = (dwLogicSectorNo / dpDiskPara.dwSecPerTrack) / dpDiskPara.dwHeads;

    return  ((dwTrack & 0xFF) << 24) | ((dwTrack & 0x0300) << 14) | (dwHead << 8) | (dwSector <<16);
}   //	Logic2Phys


//
//  convert logical sector number into physical one --- Track,Sector,Head(TCH),
//  and assign to the appropriate partition table entry
//

VOID	AssignTCH(PPARTITION_ENTRY p,DWORD dwStartingLogicSecNo,DWORD dwEndingLogicSecNo)
{
    DWORD   dwPhysSecNo;

    ASSERT(p);

    if(dwEndingLogicSecNo <= MAX_CAPACITY_SUPPORTED_BY_OLD_INT13)
    {
	dwPhysSecNo = Logic2Phys(dwStartingLogicSecNo);
	p->StartOfPartition[0] = HIBYTE(LOWORD(dwPhysSecNo));
	p->StartOfPartition[1] = LOBYTE(HIWORD(dwPhysSecNo));
	p->StartOfPartition[2] = HIBYTE(HIWORD(dwPhysSecNo));

	dwPhysSecNo = Logic2Phys(dwEndingLogicSecNo);
	p->EndOfPartition[0] = HIBYTE(LOWORD(dwPhysSecNo));
	p->EndOfPartition[1] = LOBYTE(HIWORD(dwPhysSecNo));
	p->EndOfPartition[2] = HIBYTE(HIWORD(dwPhysSecNo));
    }
    else if(dwStartingLogicSecNo <= MAX_CAPACITY_SUPPORTED_BY_OLD_INT13)
    {
	dwPhysSecNo = Logic2Phys(dwStartingLogicSecNo);
	p->StartOfPartition[0] = HIBYTE(LOWORD(dwPhysSecNo));
	p->StartOfPartition[1] = LOBYTE(HIWORD(dwPhysSecNo));
	p->StartOfPartition[2] = HIBYTE(HIWORD(dwPhysSecNo));

	p->EndOfPartition[0] = 0xFE;
	p->EndOfPartition[1] = 0xFF;
	p->EndOfPartition[2] = 0xFF;
    }
    else
    {
	p->StartOfPartition[0] = 0x00;
	p->StartOfPartition[1] = 0xC1;
	p->StartOfPartition[2] = 0xFF;

	p->EndOfPartition[0] = 0xFE;
	p->EndOfPartition[1] = 0xFF;
	p->EndOfPartition[2] = 0xFF;
    }
}   //	AssignTCH


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

BOOL	IsOurSharkDCB(PDCB  dcb)
{
    if(!IsOurVirtualDCB(dcb))
	return	FALSE;

    if(memcmp(dcb->DCB_rev_level,SHARK_DISK,REV_LEVEL_LEN))
	return	FALSE;

    return  TRUE;
}   //	IsOurSharkDCB


//  if fail ,return 0xFF (-1)
//
//	btDrive = 0 = A:    (ZERO based)
//
//
BYTE	Real2MirrorDriveLttr(BYTE btDrive)
{
    PDCB    dcb;
    DWORD   dwPartiStart = 0;

    //	search btDrive's partition start
    for(
	dcb = IspGetFirstNextDcb(NULL,DCB_type_disk);
	dcb;
	dcb = IspGetFirstNextDcb(dcb,DCB_type_disk)
       )
    {
	if(dcb->DCB_cmn.DCB_device_flags & DCB_DEV_LOGICAL)
	{
	    if(dcb->DCB_cmn.DCB_drive_lttr_equiv == btDrive)
	    {
		//  get partition start
		dwPartiStart = dcb->DCB_cmn.DCB_Partition_Start;
		break;
	    }
	}
    }

    if(0 == dwPartiStart)
	return	0xFF;

    //	match partition start in Mirror disk


    for(
	dcb = IspGetFirstNextDcb(NULL,DCB_type_disk);
	dcb;
	dcb = IspGetFirstNextDcb(dcb,DCB_type_disk)
       )
    {
	if(dcb->DCB_cmn.DCB_device_flags & DCB_DEV_LOGICAL)
	{
	    if(IsOurMirrorDCB((PDCB)dcb->DCB_cmn.DCB_physical_dcb))
	    {
		if(dcb->DCB_cmn.DCB_Partition_Start == dwPartiStart)
		{
		    ASSERT(dcb->DCB_cmn.DCB_drive_lttr_equiv);
		    return  dcb->DCB_cmn.DCB_drive_lttr_equiv;
		}
	    }
	}
    }
    return  0xFF;
}   //	Real2MirrorDriveLttr

//  if fail ,return 0xFF (-1)
//
//	btDrive = 0 = A:    (ZERO based)
//
//
BYTE	Real2SharkDriveLttr(BYTE btDrive)
{
    PDCB    dcb;
    DWORD   dwPartiStart = 0;
    QWORD   tick_1,tick_2;
    QWORD   qwTicksWaited;

    //	search btDrive's partition start
    for(
	dcb = IspGetFirstNextDcb(NULL,DCB_type_disk);
	dcb;
	dcb = IspGetFirstNextDcb(dcb,DCB_type_disk)
       )
    {
	if(dcb->DCB_cmn.DCB_device_flags & DCB_DEV_LOGICAL)
	{
	    if(dcb->DCB_cmn.DCB_drive_lttr_equiv == btDrive)
	    {
		//  get partition start
		dwPartiStart = dcb->DCB_cmn.DCB_Partition_Start;
		break;
	    }
	}
    }

    if(0 == dwPartiStart)
	return	0xFF;

	//  match partition start in Mirror disk


    for(
	dcb = IspGetFirstNextDcb(NULL,DCB_type_disk);
	dcb;
	dcb = IspGetFirstNextDcb(dcb,DCB_type_disk)
       )
    {
	if(dcb->DCB_cmn.DCB_device_flags & DCB_DEV_LOGICAL)
	{
	    if(IsOurSharkDCB((PDCB)dcb->DCB_cmn.DCB_physical_dcb))
	    {
		if(dcb->DCB_cmn.DCB_Partition_Start == dwPartiStart)
		{
		    ASSERT(dcb->DCB_cmn.DCB_drive_lttr_equiv);
		    return  dcb->DCB_cmn.DCB_drive_lttr_equiv;
		}
	    }
	}
    }
    return  0xFF;
}   //	Real2SharkDriveLttr


BYTE	MatchVirtualDriveLttr(BYTE btSysFlag,DWORD dwStart)
{
    PDCB    dcb;

    //	match partition start in Mirror disk
    for(
	dcb = IspGetFirstNextDcb(NULL,DCB_type_disk);
	dcb;
	dcb = IspGetFirstNextDcb(dcb,DCB_type_disk)
       )
    {
	if(dcb->DCB_cmn.DCB_device_flags & DCB_DEV_LOGICAL)
	{
	    if(IsOurMoverDCB((PDCB)dcb->DCB_cmn.DCB_physical_dcb))
	    {
		if(btSysFlag == PRIMARY_PAR)
		{
		    if(dwStart == dcb->DCB_cmn.DCB_Partition_Start)
			return	dcb->DCB_cmn.DCB_drive_lttr_equiv;
		}
		else
		{
		    ASSERT(btSysFlag == LOGICAL_PAR);
		    if(dwStart + PARTBL2BOOTSEC_OFFSET == dcb->DCB_cmn.DCB_Partition_Start)
			return	dcb->DCB_cmn.DCB_drive_lttr_equiv;
		}
	    }
	}
    }
    return  0xFF;
}   //	MatchVirtualDriveLttr


BOOL	Logical2PhysicalDriveLttr(BYTE DriveLttr,PSIMULATE_DRIVEMAPINFO pInfo)
{
    PDCB    dcb;

    for(
	dcb = IspGetFirstNextDcb(NULL,DCB_type_disk);
	dcb;
	dcb = IspGetFirstNextDcb(dcb,DCB_type_disk)
       )
    {
	if(dcb->DCB_cmn.DCB_device_flags & DCB_DEV_LOGICAL)
	{
	    if(!IsOurMoverDCB((PDCB)dcb->DCB_cmn.DCB_physical_dcb) &&
	       !IsOurSharkDCB((PDCB)dcb->DCB_cmn.DCB_physical_dcb))
	    {
		if(dcb->DCB_cmn.DCB_drive_lttr_equiv == DriveLttr)
		{
		    pInfo->DriveNum = ((PDCB)dcb->DCB_cmn.DCB_physical_dcb)->DCB_cmn.DCB_unit_number;
		    pInfo->ParType = dcb->DCB_cmn.DCB_partition_type;
		    pInfo->dwStart = dcb->DCB_cmn.DCB_Partition_Start;
		    return  TRUE;
		}
	    }
	}
    }
    return  FALSE;
}   //	Logical2PhysicalDriveLttr


BYTE	RetrieveDriveLttr(BYTE DriveNum,DWORD flag,DWORD dwParStart)
{
    PDCB    dcb;

    for(
	dcb = IspGetFirstNextDcb(NULL,DCB_type_disk);
	dcb;
	dcb = IspGetFirstNextDcb(dcb,DCB_type_disk)
       )
    {
	if(dcb->DCB_cmn.DCB_device_flags & DCB_DEV_LOGICAL)
	{
	    if(!IsOurMoverDCB((PDCB)dcb->DCB_cmn.DCB_physical_dcb))
	    {
		if(((PDCB)dcb->DCB_cmn.DCB_physical_dcb)->DCB_cmn.DCB_unit_number == DriveNum)
		{
		    if(flag == PRIMARY_PAR)
		    {
			if(dwParStart == dcb->DCB_cmn.DCB_Partition_Start)
			    return  dcb->DCB_cmn.DCB_drive_lttr_equiv;
		    }
		    else
		    {
			ASSERT(flag == LOGICAL_PAR);
			if(dwParStart + PARTBL2BOOTSEC_OFFSET == dcb->DCB_cmn.DCB_Partition_Start)
			    return  dcb->DCB_cmn.DCB_drive_lttr_equiv;
		    }
		}
	    }
	}
    }
    return  0xFF;
}   //	RetrieveDriveLttr


#pragma VxD_PAGEABLE_CODE_SEG

DWORD	(_stdcall *VIRPARTI_W32_Proc[])(DWORD,DWORD,LPDIOC)=
    {
	VIRPARTI_Get_Version,
	VIRPARTI_Set_Params,
	VIRPARTI_Real2MirrorDriveLttr,
	VIRPARTI_GetDriveletterOfMover,
	VIRPARTI_RetrieveDriveLttr,
	VIRPARTI_DriveMapInfo,
	VIRPARTI_Real2SharkDriveLttr,
	VIRPARTI_EnableOrDisable
    };

#define MAX_VIRPARTI_W32_API  (sizeof(VIRPARTI_W32_Proc)/sizeof(DWORD))

DWORD	_stdcall VIRPARTI_W32_DeviceIOControl(DWORD dwService,
					      DWORD dwDDB,
					      DWORD hDevice,
					      LPDIOC lpDIOCParms)
{
    DWORD   dwRetVal=0;
    // DIOC_OPEN is sent when VxD is loaded w/ CreateFile
    //	(this happens just after SYS_DYNAMIC_INIT)
    if ( dwService == DIOC_OPEN )
    {
	Out_Debug_String("VIRPARTI: WIN32 DEVIOCTL supported here!\n\r");
	// Must return 0 to tell WIN32 that this VxD supports DEVIOCTL
	dwRetVal = 0;
    }
    // DIOC_CLOSEHANDLE is sent when VxD is unloaded w/ CloseHandle
    //	(this happens just before SYS_DYNAMIC_EXIT)
    else if ( dwService == DIOC_CLOSEHANDLE )
    {
	// Dispatch to cleanup proc
	dwRetVal = VIRPARTI_CleanUp();
    }
    else if ( dwService > MAX_VIRPARTI_W32_API )
    {
	// Returning a positive value will cause the WIN32 DeviceIOControl
	// call to return FALSE, the error code can then be retrieved
	// via the WIN32 GetLastError
	dwRetVal = ERROR_NOT_SUPPORTED;
    }
    else
    {
	// CALL requested service
	dwRetVal = (VIRPARTI_W32_Proc[dwService-1])(dwDDB, hDevice, lpDIOCParms);
    }
    return(dwRetVal);
}   //	VIRPARTI_W32_DeviceIOControl


DWORD _stdcall VIRPARTI_CleanUp(void)
{
    return(VXD_SUCCESS);
}   //	VIRPARTI_CleanUp


DWORD _stdcall VIRPARTI_Get_Version(DWORD  dwDDB,
				    DWORD  hDevice,
				    LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pRegs;

    pRegs = (DIOC_REGISTERS *)lpDIOCParms->lpvOutBuffer;
    pRegs->reg_EAX = VIRPARTI_Major << 16 | VIRPARTI_Minor;
    return  NO_ERROR;
}   //	VIRPARTI_Get_Version


DWORD _stdcall VIRPARTI_GetDriveletterOfMover(DWORD  dwDDB,
					      DWORD  hDevice,
					      LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pRegs;
    pRegs = (DIOC_REGISTERS *)lpDIOCParms->lpvOutBuffer;

    pRegs->reg_EAX = MatchVirtualDriveLttr(LOBYTE(LOWORD(pRegs->reg_EAX)),pRegs->reg_EBX);
    return  NO_ERROR;
}   //	VIRPARTI_GetDriveletterOfMover


DWORD _stdcall VIRPARTI_RetrieveDriveLttr(DWORD  dwDDB,
					  DWORD  hDevice,
					  LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pRegs;
    pRegs = (DIOC_REGISTERS *)lpDIOCParms->lpvOutBuffer;

    pRegs->reg_EAX = RetrieveDriveLttr(LOBYTE(LOWORD(pRegs->reg_EAX)),pRegs->reg_EBX,pRegs->reg_ECX);
    return  NO_ERROR;
}   //	VIRPARTI_RetrieveDriveLttr


DWORD _stdcall VIRPARTI_DriveMapInfo(DWORD  dwDDB,
				     DWORD  hDevice,
				     LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pRegs;
    pRegs = (DIOC_REGISTERS *)lpDIOCParms->lpvOutBuffer;

    pRegs->reg_EAX = Logical2PhysicalDriveLttr(LOBYTE(LOWORD(pRegs->reg_EAX)),(PSIMULATE_DRIVEMAPINFO)pRegs->reg_EBX);
    return  NO_ERROR;
}   //	VIRPARTI_DriveMapInfo

DWORD _stdcall VIRPARTI_Real2MirrorDriveLttr(DWORD  dwDDB,
					     DWORD  hDevice,
					     LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pRegs;
    pRegs = (DIOC_REGISTERS *)lpDIOCParms->lpvOutBuffer;

    pRegs->reg_EAX = (DWORD)Real2MirrorDriveLttr(LOBYTE(LOWORD(pRegs->reg_EAX)));
    pRegs->reg_EAX &= 0xFF;

    return  NO_ERROR;
}   //	VIRPARTI_Real2MirrorDriveLttr


DWORD _stdcall VIRPARTI_Real2SharkDriveLttr(DWORD  dwDDB,
					     DWORD  hDevice,
					     LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pRegs;
    pRegs = (DIOC_REGISTERS *)lpDIOCParms->lpvOutBuffer;

    pRegs->reg_EAX = (DWORD)Real2SharkDriveLttr(LOBYTE(LOWORD(pRegs->reg_EAX)));
    pRegs->reg_EAX &= 0xFF;

    return  NO_ERROR;
}   //	VIRPARTI_Real2SharkDriveLttr


DWORD _stdcall VIRPARTI_EnableOrDisable(DWORD  dwDDB,
					DWORD  hDevice,
					LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pRegs;
    pRegs = (DIOC_REGISTERS *)lpDIOCParms->lpvOutBuffer;

    g_bWorking = pRegs->reg_EAX;
    pRegs->reg_EAX = 0x01;

    return  NO_ERROR;
}   //	EnableOrDisable



DWORD _stdcall VIRPARTI_Set_Params(
				    DWORD  dwDDB,
				    DWORD  hDevice,
				    LPDIOC lpDIOCParms
				  )
{
    DIOC_REGISTERS  *pRegs;
    PBIOS_DRIVE_PARAM		pBIOS;

    pRegs = (DIOC_REGISTERS *)lpDIOCParms->lpvInBuffer;

    dwStatus = pRegs->reg_ECX;	//  Convert partition or Resize partition

    if(dwStatus == RESIZE_PARTITION)
	memcpy(&resize_info,(PVOID)pRegs->reg_EAX,sizeof(RESIZE_INFO));
    else
	memcpy(&convert_info,(PVOID)pRegs->reg_EAX,sizeof(PRI2LOG_INFO));

    pBIOS = (PBIOS_DRIVE_PARAM)pRegs->reg_EBX;
    dpDiskPara.dwCylinders = pBIOS->dwCylinders;
    dpDiskPara.dwHeads = pBIOS->dwHeads;
    dpDiskPara.dwSecPerTrack = pBIOS->dwSecPerTrack;

    ASSERT(dpDiskPara.dwSecPerTrack == 0x3F);

    pRegs->reg_EAX = TRUE;
    return  NO_ERROR;
}   //	VIRPARTI_Set_Params


PDCB	GetMonitorDCB()
{
    return  MonitorDCB;
}   //	GetMonitorDCB

PDCB	GetSharkDCB()
{
    return  SharkDCB;
}   //	GetSharkDCB


BOOL SetMonitorDCB(DWORD dwDrive)
{
    MonitorDCB = DriveLetter2PhysDcb(LOBYTE(LOWORD(dwDrive)));
    if(NULL == MonitorDCB)
	return	FALSE;

    pChain = GetCallDownChain(MonitorDCB);
    if(NULL == pChain)
	return	FALSE;

    return  TRUE;
}   // SetMonitorDCB

BOOL SetSharkDCB(DWORD dwDrive)
{
    SharkDCB = DriveLetter2PhysDcb(LOBYTE(LOWORD(dwDrive)));
    if(NULL == SharkDCB)
	return	FALSE;

    pSharkChain = GetCallDownChain(SharkDCB);
    if(NULL == pSharkChain)
	return	FALSE;
    return  TRUE;
}   // SetSharkDCB

void SimulatePartition(DWORD dwStartSector, DWORD dwSectors, PBYTE pMemBuf,
		       DWORD dwType, BYTE btSysID, DWORD dwParStart, DWORD dwParEnd)
{
    PBYTE		pbtMemData;
    PPARTITION_ENTRY	pParTbl;
    PWORD		pSign;

    pbtMemData = pMemBuf;
    if (dwStartSector == 0)
    {
	//
	//  if include MBR sector , modify the appropriate partition
	//  table entry
	//
	pParTbl = (PPARTITION_ENTRY)(pbtMemData + PARTITABLEOFFSET);
	// Init all partitions to empty
	memset(pParTbl,0,4*sizeof(PARTITION_ENTRY));
	// Set the 1st. entry to our target partition
	if (dwType == PRIMARY_PAR)
	{
	    pParTbl->SystemFlag = btSysID;
	}
	else
	{
	    if (dwParEnd+1 <= HARD_DISK_LIMIT_8G)
		pParTbl->SystemFlag = 0x05; // Mark it as extended partition
	    else
		pParTbl->SystemFlag = 0x0F; // Mark it as extended partition
	}
	pParTbl->StartSector = dwParStart;
	pParTbl->SectorsInPartition = dwParEnd-pParTbl->StartSector+1;
	AssignTCH(pParTbl,pParTbl->StartSector,pParTbl->StartSector+pParTbl->SectorsInPartition-1);
    }
    if (dwType == LOGICAL_PAR)
    {
	if(dwStartSector <= dwParStart && dwStartSector+dwSectors > dwParStart)
	{
	    //
	    //	If reading extended partition table fake one
	    //
	    pbtMemData += (dwParStart-dwStartSector)*SECTOR_SIZE;
	    memset(pbtMemData,0,SECTOR_SIZE);
	    pParTbl = (PPARTITION_ENTRY)(pbtMemData + PARTITABLEOFFSET);
	    pParTbl->SystemFlag = btSysID;
	    pParTbl->StartSector = PARTBL2BOOTSEC_OFFSET;
	    pParTbl->SectorsInPartition = dwParEnd-dwParStart-PARTBL2BOOTSEC_OFFSET+1;
	    AssignTCH(pParTbl,pParTbl->StartSector,pParTbl->StartSector+pParTbl->SectorsInPartition-1);
	    pSign = (PWORD)(pbtMemData + PARTITION_SIGN_OFF);
	    *pSign = PARTITION_SIGN;
	}
    }
    return;
}

void SimulateMoverPartition(DWORD dwStartSector, DWORD dwSectors, PBYTE pMemBuf,
			    DWORD dwType, BYTE btSysID, DWORD dwParStart, DWORD dwParEnd)
{
    PBYTE		pbtMemData;
    PBYTE		pDataPtr;
    DWORD		dwBytes;
    PPARTITION_ENTRY	pParTbl;
    PWORD		pSign;
    PBOOT32_SEC 	pBootSec;

    pbtMemData = pMemBuf;
    pDataPtr = pbtMemData;
    if (dwStartSector == 0)
    {
	//
	//  if include MBR sector , modify the appropriate partition
	//  table entry
	//
	pParTbl = (PPARTITION_ENTRY)(pbtMemData + PARTITABLEOFFSET);
	// Init all partitions to empty
	memset(pParTbl,0,4*sizeof(PARTITION_ENTRY));
	// Set the 1st. entry to our target partition
	if (dwType == PRIMARY_PAR)
	{
	    pParTbl->SystemFlag = btSysID;
	}
	else
	{
	    if (dwParEnd+1 <= HARD_DISK_LIMIT_8G)
		pParTbl->SystemFlag = 0x05; // Mark it as extended partition
	    else
		pParTbl->SystemFlag = 0x0F; // Mark it as extended partition
	}
	pParTbl->StartSector = dwParStart;
	pParTbl->SectorsInPartition = dwParEnd-pParTbl->StartSector+1;
	AssignTCH(pParTbl,pParTbl->StartSector,pParTbl->StartSector+pParTbl->SectorsInPartition-1);
	pDataPtr = (PBYTE)((DWORD)pbtMemData + SECTOR_SIZE);
    }
    if (dwType == LOGICAL_PAR)
    {
	if(dwStartSector <= dwParStart && dwStartSector+dwSectors > dwParStart)
	{
	    //
	    //	If reading extended partition table fake one
	    //
	    pbtMemData = pMemBuf+(dwParStart-dwStartSector)*SECTOR_SIZE;
	    memset(pbtMemData,0,SECTOR_SIZE);
	    pParTbl = (PPARTITION_ENTRY)(pbtMemData + PARTITABLEOFFSET);
	    pParTbl->SystemFlag = btSysID;
	    pParTbl->StartSector = PARTBL2BOOTSEC_OFFSET;
	    pParTbl->SectorsInPartition = dwParEnd-dwParStart-PARTBL2BOOTSEC_OFFSET+1;
	    AssignTCH(pParTbl,pParTbl->StartSector,pParTbl->StartSector+pParTbl->SectorsInPartition-1);
	    pSign = (PWORD)(pbtMemData + PARTITION_SIGN_OFF);
	    *pSign = PARTITION_SIGN;
	    // Set gap region to zero
	    if (pbtMemData > pDataPtr)
	    {
		dwBytes = (DWORD)pbtMemData-(DWORD)pDataPtr;
		memset(pDataPtr,0x00,dwBytes);
	    }
	    pDataPtr = (PBYTE)((DWORD)pbtMemData + SECTOR_SIZE);
	}
	if(dwStartSector <= dwParStart+PARTBL2BOOTSEC_OFFSET &&
	   dwStartSector+dwSectors > dwParStart+PARTBL2BOOTSEC_OFFSET)
	{
	    //
	    //	If reading boot sector, modify the BPB to reflect the new size
	    //
	    pbtMemData = pMemBuf+(dwParStart+PARTBL2BOOTSEC_OFFSET-dwStartSector)*SECTOR_SIZE;
	    pBootSec = (PBOOT32_SEC)pbtMemData;
	    pBootSec->HiddenSectors = dwParStart+PARTBL2BOOTSEC_OFFSET;
	    pBootSec->SectorsLarger = dwParEnd-dwParStart-PARTBL2BOOTSEC_OFFSET+1;
	    pBootSec->SectorsSmall = 0;
	    // Set gap region to zero
	    if (pbtMemData > pDataPtr)
	    {
		dwBytes = (DWORD)pbtMemData-(DWORD)pDataPtr;
		memset(pDataPtr,0x00,dwBytes);
	    }
	    pDataPtr = (PBYTE)((DWORD)pbtMemData + SECTOR_SIZE);
	}
	// Skip all 3 FAT32 boot sectors.
	// Notice that FAT16 only has 1 boot sector. This is ok.
	if ((pDataPtr-pMemBuf)/SECTOR_SIZE+dwStartSector < dwParStart+PARTBL2BOOTSEC_OFFSET+3 &&
	    (pDataPtr-pMemBuf)/SECTOR_SIZE+dwStartSector >= dwParStart+PARTBL2BOOTSEC_OFFSET)
	{
	    pDataPtr = pMemBuf+(dwParStart+PARTBL2BOOTSEC_OFFSET+3-dwStartSector)*SECTOR_SIZE;
	}
    }
    else
    {
	if(dwStartSector <= dwParStart && dwStartSector+dwSectors > dwParStart)
	{
	    //
	    //	If reading boot sector, modify the BPB to reflect the new size
	    //
	    pbtMemData = pMemBuf+(dwParStart-dwStartSector)*SECTOR_SIZE;
	    pBootSec = (PBOOT32_SEC)pbtMemData;
	    pBootSec->HiddenSectors = dwParStart;
	    pBootSec->SectorsLarger = dwParEnd-dwParStart+1;
	    pBootSec->SectorsSmall = 0;
	    // Set gap region to zero
	    if (pbtMemData > pDataPtr)
	    {
		dwBytes = (DWORD)pbtMemData-(DWORD)pDataPtr;
		memset(pDataPtr,0x00,dwBytes);
	    }
	    pDataPtr = (PBYTE)((DWORD)pbtMemData + SECTOR_SIZE);
	}
	// Skip all 3 FAT32 boot sectors.
	// Notice that FAT16 only has 1 boot sector. This is ok.
	if ((pDataPtr-pMemBuf)/SECTOR_SIZE+dwStartSector < dwParStart+3 &&
	    (pDataPtr-pMemBuf)/SECTOR_SIZE+dwStartSector >= dwParStart)
	{
	    pDataPtr = pMemBuf+(dwParStart+3-dwStartSector)*SECTOR_SIZE;
	}
    }
    // Set gap region to zero
    pbtMemData = (PBYTE)((DWORD)pMemBuf + dwSectors*SECTOR_SIZE);
    if (pbtMemData > pDataPtr)
    {
	dwBytes = (DWORD)pbtMemData-(DWORD)pDataPtr;
	memset(pDataPtr,0x00,dwBytes);
    }

    return;
}



VOID	CalcTick(QWORD	tick_1, QWORD  tick_2, PQWORD ticks)
{
    ticks->qword_lo += tick_2.qword_lo - tick_1.qword_lo;
}   //	CalcTick


#ifdef	DEBUG

VOID	CRLF(VOID)
{
    Debug_Printf("\r\n");
}   //	CRLF


BOOL	DebugQueryHandler(VOID)
{
    CHAR    cDebugChar;

    CRLF();
    Debug_Printf("***************      VIRPARTI DEBUG INFO      ***************");
    CRLF();
    Debug_Printf("[1] List All Physical Drive DCB");
    CRLF();
    Debug_Printf("[2] List All Logical Drive DCB");
    CRLF();
    Debug_Printf("Please select an option: ");

    cDebugChar = GetDebugChar();
    CRLF();
    Debug_Printf("%c",cDebugChar);

    switch(cDebugChar)
    {
	case	'1':
	    ListPhysDcb();
	    break;

	case	'2':
	    ListLogicDriveDCB();
	    break;

	default:
	    CRLF();
	    Debug_Printf("Invalid VIRPARTI DEBUG OPTION!");
	    break;
    }
    return  TRUE;
}   //	DebugQueryHandler


VOID	ListPhysDcb(VOID)
{
    PDCB    dcb;

    CRLF();

    for(
	dcb = IspGetFirstNextDcb(NULL,DCB_type_disk);
	dcb;
	dcb = IspGetFirstNextDcb(dcb,DCB_type_disk)
       )
    {
	if(dcb->DCB_cmn.DCB_device_flags & DCB_DEV_PHYSICAL)
	{
	    Debug_Printf("%8x\t",(DWORD)dcb);
	}
    }

    CRLF();
}   //	ListPhysDcb


VOID	ListLogicDriveDCB(VOID)
{
    PDCB    dcb;

    CRLF();
    Debug_Printf("Logical DCB\tDrive Letter\tPartition Start");

    for(
	dcb = IspGetFirstNextDcb(NULL,DCB_type_disk);
	dcb;
	dcb = IspGetFirstNextDcb(dcb,DCB_type_disk)
       )
    {
	if(dcb->DCB_cmn.DCB_device_flags & DCB_DEV_LOGICAL)
	{
	    CRLF();
	    Debug_Printf("%8x\t",(DWORD)dcb);
	    Debug_Printf("%c\t\t",dcb->DCB_cmn.DCB_drive_lttr_equiv + 'A');
	    Debug_Printf("%8x\t",dcb->DCB_cmn.DCB_Partition_Start);
	}
    }

    CRLF();
}   //	ListLogicDriveDCB

#endif
