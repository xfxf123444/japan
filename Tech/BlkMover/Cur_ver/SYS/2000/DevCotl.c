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

#include "iomon.h"
#include "blkmover.h"

extern BOOL							g_bEnableProtect;
extern BOOL							g_bPartitionSet;
extern BOOL							g_bBlkMovSucc;
extern PBLKMOVER_DATA				g_pMoverData;
extern PBLKMOVER_DATA				g_pSharkData;
extern PDRIVE_LAYOUT_INFORMATION	g_partitionInfo;
extern BOOL							g_bReLocate	;
extern DWORD						g_dwRePointer ;
extern PYG_LIST						g_pDataRegionLRUList;
extern DWORD						g_dwMinSec,g_dwMaxSec;
extern PBYTE						g_pCompInitBuf;
extern DWORD						g_dwDataRecNum;


NTSTATUS
BlkMovDeviceControl(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
    )
{
	PBLK_MOVER_PARAM			pParam;
    DIOC_REGISTERS				*pRegs;
	NTSTATUS					status = STATUS_SUCCESS;
    KIRQL						currentIrql;
	DWORD						dwRetVal;
    PIRP						IrpParInfo;
	PARTITION_INFORMATION		ParInfo;
    IO_STATUS_BLOCK				ioStatusBlock;
    KEVENT						event;
    PIRP						irp;
    CCHAR						boost;
	UCHAR						*FileName;
    PDRIVE_LAYOUT_INFORMATION	driveLayout;
	DWORD						dwStartSec;
	PVOID						pBuffer;
    PIO_STACK_LOCATION			nextIrpStack;
    int							i;

    PDEVICE_EXTENSION  deviceExtension = DeviceObject->DeviceExtension;
    PIO_STACK_LOCATION currentIrpStack = IoGetCurrentIrpStackLocation(Irp);

    switch (currentIrpStack->Parameters.DeviceIoControl.IoControlCode)
    {
	case IOCTL_ENABLE_OR_DISABLE_FILTE_INIT_SECTOR:
		pRegs = (DIOC_REGISTERS *)Irp->AssociatedIrp.SystemBuffer;
		
		if(pRegs->reg_EAX)
		{
			g_pCompInitBuf = (PBYTE)ExAllocatePool(NonPagedPool,SECTOR_SIZE);
			
			if(g_pCompInitBuf)// RtlZeroMemory(g_pCompInitBuf, SECTOR_SIZE);
			{
				for (i = 0;i <SECTOR_SIZE ;i+=BLKMOV_SIGN_LEN)
				{
					RtlCopyMemory(g_pCompInitBuf+i,BLKMOV_SIGN_STRING,BLKMOV_SIGN_LEN);
				}
			}
			else
				pRegs->reg_EAX = FALSE;
			g_pMoverData->bInitSec = TRUE;

			g_dwOrig1st  = pRegs->reg_EBX;
			g_dwOrigLast = pRegs->reg_ECX;
		}
		else
		{
			g_pMoverData->bInitSec = FALSE;
			ExFreePool( g_pCompInitBuf );
			pRegs->reg_EAX	= TRUE;
		}

		if(status != STATUS_SUCCESS) pRegs->reg_EAX	= FALSE;

        Irp->IoStatus.Status	  = status;
		Irp->IoStatus.Information = sizeof(DIOC_REGISTERS);
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        return status;
	//check this filter is existing
	case IOCTL_YG_BLOCK_MOVER_EXIST:
        Irp->IoStatus.Status	  = STATUS_SUCCESS;
		Irp->IoStatus.Information = sizeof(DWORD);
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        return STATUS_SUCCESS;
	//main function for block mover
	case IOCTL_YG_MOVE_SYS_BLOCK:
	case IOCTL_YG_BLOCK_MOVING_GROUP_CURRENT:
	case IOCTL_YG_BLOCK_MOVER:
	//Using for recoverymaster
	case IOCTL_YG_MOVE_GROUP:
        status = STATUS_PENDING;
		break;;
	case IOCTL_YG_WRITE:
	case IOCTL_YG_READ:
        Irp->MdlAddress = IoAllocateMdl((PVOID)((PREAD_WRITE_BUFFER)Irp->AssociatedIrp.SystemBuffer)->pBuffer,
										((PREAD_WRITE_BUFFER)Irp->AssociatedIrp.SystemBuffer)->dwLength*SECTOR_SIZE,
										  FALSE, FALSE,NULL);
		if (Irp->MdlAddress == NULL)
		{
            Irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
            Irp->IoStatus.Information = 0;
	        status = Irp->IoStatus.Status;
			IoCompleteRequest(Irp,IO_NO_INCREMENT);
			return status;
		}
		MmProbeAndLockPages(Irp->MdlAddress,KernelMode,IoWriteAccess);

        status = STATUS_PENDING;
		break;
	//Using for recoverymaster
	case IOCTL_RM_BLKMOV_INIT:
		pRegs = (DIOC_REGISTERS *)Irp->AssociatedIrp.SystemBuffer;
		
		if(pRegs->reg_EAX)
		{
			g_dwDataRecNum		= 0;
			deviceExtension->bInitMover = TRUE;
			g_pDataRegionLRUList = YG_List_Create(NonPagedPool,sizeof(RM_SEC_GROUP_TABLE));
		}
		else
		{
			deviceExtension->bInitMover = FALSE;
			pRegs->reg_EAX = TRUE;
		}

        Irp->IoStatus.Status	  = status;
		Irp->IoStatus.Information = sizeof(DIOC_REGISTERS);
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        return status;
	//using for RecoveryMaster
	case IOCTL_SET_RM_SEC_GROUP_TABLE:
		pRegs = (DIOC_REGISTERS *)Irp->AssociatedIrp.SystemBuffer;
		
		pRegs->reg_EAX = RMSetGroup(pRegs->reg_EAX,pRegs->reg_EBX,DeviceObject);

        Irp->IoStatus.Status	  = status;
		Irp->IoStatus.Information = sizeof(DIOC_REGISTERS);
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        return status;
	//using for RecoveryMaster
	case IOCTL_RM_ENABLE_DISABLE_RELOCATE:
		pRegs = (DIOC_REGISTERS *)Irp->AssociatedIrp.SystemBuffer;
		
		if(pRegs->reg_EAX)
		{
			g_dwMinSec = pRegs->reg_EBX;
			g_dwMaxSec = pRegs->reg_ECX;
			g_bReLocate= TRUE;
			g_dwRePointer = 0;
		}
		else
		{
			g_bReLocate= FALSE;
			g_dwMinSec		= 0;
			g_dwMaxSec		= 0;
		}

		pRegs->reg_EAX	= TRUE;

        Irp->IoStatus.Status	  = status;
		Irp->IoStatus.Information = sizeof(DIOC_REGISTERS);
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        return status;
	//Set parameter
	case IOCTL_YG_BLOCK_MOVER_SET_PARAM:
		pRegs = (DIOC_REGISTERS *)Irp->AssociatedIrp.SystemBuffer;
		pParam = (PBLK_MOVER_PARAM)pRegs->reg_EAX; 
		
		status = SetMoverPara(pParam);
		if(status == STATUS_SUCCESS)
		{
			pRegs->reg_EAX = TRUE;
		}
		else
			pRegs->reg_EAX = FALSE;

        Irp->IoStatus.Status	  = status;
		Irp->IoStatus.Information = sizeof(DIOC_REGISTERS);
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        return status;
	case IOCTL_YG_BLOCK_MOVER_SET_DATA:
		pRegs = (DIOC_REGISTERS *)Irp->AssociatedIrp.SystemBuffer;
		pParam = (PBLK_MOVER_PARAM)pRegs->reg_EAX; 

		g_pSharkData->DeviceObject = DeviceObject;

		deviceExtension = g_pMoverData->DeviceObject->DeviceExtension;
        //
        // Allocate buffer for drive layout.
        //
        g_partitionInfo = ExAllocatePool(NonPagedPool,ALL_PARTIONT_INFO_SIZE);
        if (g_partitionInfo)
		{
			KeInitializeEvent(&event,
							  NotificationEvent,
							  FALSE);

			//
			// Create IRP for get drive layout device control.
			//
			irp = IoBuildDeviceIoControlRequest(IOCTL_DISK_GET_DRIVE_LAYOUT,
												deviceExtension->TargetDeviceObject,
												NULL,
												0,
												g_partitionInfo,
												ALL_PARTIONT_INFO_SIZE,
												FALSE,
												&event,
												&ioStatusBlock);
			if (irp) 
			{
				//
				// Set the event object to the unsignaled state.
				// It will be used to signal request completion.
				//
				status = IoCallDriver(deviceExtension->TargetDeviceObject,
									  irp);

				if (status == STATUS_PENDING) {

					KeWaitForSingleObject(&event,
										  Executive,
										  KernelMode,
										  FALSE,
										  NULL);

					status = ioStatusBlock.Status;
				}
				if (NT_SUCCESS(status))
					pRegs->reg_EAX = TRUE;
				else
				{
					pRegs->reg_EAX = FALSE;
				}
			}
			else
			{
				pRegs->reg_EAX = FALSE;
			}
        }
		else
			pRegs->reg_EAX = FALSE;

        Irp->IoStatus.Status	  = status;
		Irp->IoStatus.Information = sizeof(DIOC_REGISTERS);
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        return status;
	case IOCTL_YG_BLOCK_SET_FREE_SPACE_GROUP:
		pRegs = (DIOC_REGISTERS *)Irp->AssociatedIrp.SystemBuffer;
		
		pRegs->reg_EAX = SetFreeSpaceList(pRegs->reg_EAX,pRegs->reg_EBX);

        Irp->IoStatus.Status	  = status;
		Irp->IoStatus.Information = sizeof(DIOC_REGISTERS);
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        return status;
	case IOCTL_YG_BLOCK_MOVER_FLUSH_BUFFER:
        status = STATUS_PENDING;
		break;
	case IOCTL_YG_BLOCK_MOVER_ENABLE_WRITE_PROTECT:
		pRegs = (DIOC_REGISTERS *)Irp->AssociatedIrp.SystemBuffer;

		g_bEnableProtect = TRUE;
		pRegs->reg_EAX = TRUE;
		Irp->IoStatus.Status	  = status;
		Irp->IoStatus.Information = sizeof(DIOC_REGISTERS);
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
		return status;
	case IOCTL_YG_BLOCK_MOVER_FORCE_REBOOT:
		pRegs = (DIOC_REGISTERS *)Irp->AssociatedIrp.SystemBuffer;
		ForceReboot();

		pRegs->reg_EAX = TRUE;
		Irp->IoStatus.Status	  = status;
		Irp->IoStatus.Information = sizeof(DIOC_REGISTERS);
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
		return status;
	//set on/off and create/deleter thread
	case IOCTL_YG_BLOCK_MOVER_ENABLE_OR_DISABLE:
		pRegs = (DIOC_REGISTERS *)Irp->AssociatedIrp.SystemBuffer;
		pParam = (PBLK_MOVER_PARAM)pRegs->reg_EAX; 
	
		status = EnableOrDisable((BOOLEAN)pRegs->reg_EAX,DeviceObject);
		if(status == STATUS_SUCCESS)
		{
			pRegs->reg_EAX = TRUE;
		}
		else
		{
			pRegs->reg_EAX = FALSE;
		}

		Irp->IoStatus.Status	  = status;
		Irp->IoStatus.Information = sizeof(DIOC_REGISTERS);
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
		return status;
	//allocate locked memory and init it;
	case IOCTL_YG_BLOCK_MOVER_PERFACE:
		pRegs = (DIOC_REGISTERS *)Irp->AssociatedIrp.SystemBuffer;
		pParam = (PBLK_MOVER_PARAM)pRegs->reg_EAX; 

		pRegs->reg_EAX = TRUE;

        Irp->IoStatus.Status	  = status;
		Irp->IoStatus.Information = sizeof(DIOC_REGISTERS);
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
		return status;
	//Free locked memory ;
	case IOCTL_YG_BLOCK_MOVER_POST_SCRIPT:
		pRegs = (DIOC_REGISTERS *)Irp->AssociatedIrp.SystemBuffer;

		status = PostScript((BOOLEAN)pRegs->reg_EAX);
		if(status == STATUS_SUCCESS)
			pRegs->reg_EAX = TRUE;
		else
			pRegs->reg_EAX = FALSE;
        Irp->IoStatus.Status	  = status;
		Irp->IoStatus.Information = sizeof(DIOC_REGISTERS);
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
		return status;
	case IOCTL_DISK_GET_DRIVE_LAYOUT:
		if(g_pMoverData)
		{
			if(g_pMoverData->bWorking && g_partitionInfo && !g_bPartitionSet)
			{
				memcpy(Irp->AssociatedIrp.SystemBuffer,g_partitionInfo,ALL_PARTIONT_INFO_SIZE);

				status = STATUS_SUCCESS;

		        Irp->IoStatus.Status	  = status;
				Irp->IoStatus.Information = ALL_PARTIONT_INFO_SIZE;
				IoCompleteRequest(Irp, IO_NO_INCREMENT);
				return status;
			}
		}
        Irp->CurrentLocation++,
        Irp->Tail.Overlay.CurrentStackLocation++;
        return IoCallDriver(deviceExtension->TargetDeviceObject, Irp);
	case IOCTL_DISK_SET_DRIVE_LAYOUT:
		nextIrpStack = IoGetNextIrpStackLocation(Irp);
		*nextIrpStack = *currentIrpStack;
		//
		// Set completion routine callback.
		//
		IoSetCompletionRoutine(Irp,
							   Blk_Set_Drive_Layout_Completion,
							   DeviceObject,
							   TRUE,
							   TRUE,
							   TRUE);

		return IoCallDriver(deviceExtension->TargetDeviceObject,Irp);
	case IOCTL_YG_GET_MOVED_SECTORS:
        status = STATUS_PENDING;
		break;
	default:
		break;
	}

	if (status == STATUS_PENDING)
	{
		IoMarkIrpPending(Irp);

		ExInterlockedInsertTailList(
				&deviceExtension->list_head,
				&Irp->Tail.Overlay.ListEntry,
				&deviceExtension->list_lock
				);

		KeSetEvent(
				&deviceExtension->request_event,
				(KPRIORITY) 0,
				FALSE
				);
		return status;
	}
	else
	{
        //
        // Set current stack back one.
        //

        Irp->CurrentLocation++,
        Irp->Tail.Overlay.CurrentStackLocation++;

        //
        // Pass unrecognized device control requests
        // down to next driver layer.
        //

        return IoCallDriver(deviceExtension->TargetDeviceObject, Irp);
	}
} // end BlkMovDeviceControl()

NTSTATUS
Blk_Set_Drive_Layout_Completion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp,
    IN PVOID          Context
    )

{
	if(g_bBlkMovSucc)
		g_bPartitionSet = TRUE;

    if (Irp->PendingReturned)
	{
        IoMarkIrpPending(Irp);
    }
    return STATUS_SUCCESS;

} // Blk_Set_Drive_Layout_Completion

