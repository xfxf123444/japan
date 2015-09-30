/*++
Copyright (C) YuGuang Corporation, 2000 - 2001

Module Name:

    IoMon.c

Abstract:

    This driver monitors disk accesses capturing performance data.

Environment:

    kernel mode only

Notes:

--*/


#define INITGUID

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

extern PPM_FREE_SPACE_GROUP		g_FreeSpaceList;
extern BOOL						g_bEnableProtect;
extern PBLKMOVER_DATA			g_pMoverData;
extern PBLKMOVER_DATA			g_pSharkData;
extern BOOL						g_bReLocate;
extern BOOL						g_bBlkMovSucc;
extern BOOL						g_bPartitionSet;
extern DWORD					g_dwMinSec,g_dwMaxSec;
extern DWORD					g_dwRePointer;
extern PRM_SEC_GROUP_TABLE		g_pDataRegionListHead;
extern PRM_SEC_GROUP_TABLE		g_pDataRegionListTail;
extern DWORD					g_dwNumDataRegionLoaded;
extern PYG_LIST					g_pDataRegionLRUList;
extern DWORD					g_dwMovedSecs;
extern DWORD					g_dwMovedRecNum;
extern PBYTE					g_pCompInitBuf;
extern DWORD					g_dwSysRecNum,g_dwDataRecNum;

extern DWORD					g_dwDataStart;
extern KSPIN_LOCK				g_IrpSpinLock;
extern PPM_PROTECT_GROUP		g_pProtectFreeList;
extern PM_RELOCATE_GROUP		g_FinalGroup[32];

extern MOVING_GROUP				g_MovingGroup;
//
// Define the sections that allow for discarding (i.e. paging) some of
// the code.
//
#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#endif

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
{
    ULONG               ulIndex;
    PDRIVER_DISPATCH  * dispatch;

    //
    // Remember registry path
    //

    DiskPerfRegistryPath.MaximumLength = RegistryPath->Length
                                            + sizeof(UNICODE_NULL);
    DiskPerfRegistryPath.Buffer = ExAllocatePool(
                                    PagedPool,
                                    DiskPerfRegistryPath.MaximumLength);
    if (DiskPerfRegistryPath.Buffer != NULL)
    {
        RtlCopyUnicodeString(&DiskPerfRegistryPath, RegistryPath);
    }
	else
	{
        DiskPerfRegistryPath.Length = 0;
        DiskPerfRegistryPath.MaximumLength = 0;
    }
    //
    // Create dispatch points
    //
    for (ulIndex = 0, dispatch = DriverObject->MajorFunction;
         ulIndex <= IRP_MJ_MAXIMUM_FUNCTION;
         ulIndex++, dispatch++) {

        *dispatch = BlkMovSendToNextDriver;
    }

//		 _asm{int 3}
    //
    // Set up the device driver entry points.
    //

    DriverObject->MajorFunction[IRP_MJ_CREATE]          = BlkMovCreate;
    DriverObject->MajorFunction[IRP_MJ_READ]            = BlkMovReadWrite;
    DriverObject->MajorFunction[IRP_MJ_WRITE]           = BlkMovReadWrite;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL]  = BlkMovDeviceControl;
//    DriverObject->MajorFunction[IRP_MJ_SYSTEM_CONTROL]  = DiskPerfWmi;

    DriverObject->MajorFunction[IRP_MJ_SHUTDOWN]        = BlkMovShutdownFlush;
    DriverObject->MajorFunction[IRP_MJ_FLUSH_BUFFERS]   = BlkMovShutdownFlush;
    DriverObject->MajorFunction[IRP_MJ_PNP]             = BlkMovDispatchPnp;
    DriverObject->MajorFunction[IRP_MJ_POWER]           = BlkMovDispatchPower;

    DriverObject->DriverExtension->AddDevice            = BlkMovAddDevice;
    DriverObject->DriverUnload                          = BlkMovUnload;

	KeInitializeSpinLock(&g_IrpSpinLock);

	MmLockPagableCodeSection(BlkMovReadWrite);

	MmLockPagableDataSection(&g_MovingGroup);
	MmLockPagableDataSection(&g_pMoverData);
	MmLockPagableDataSection(&g_bEnableProtect);
	MmLockPagableDataSection(&g_FinalGroup);
    return(STATUS_SUCCESS);

} // end DriverEntry()

#define FILTER_DEVICE_PROPOGATE_FLAGS            0
#define FILTER_DEVICE_PROPOGATE_CHARACTERISTICS (FILE_REMOVABLE_MEDIA |  \
                                                 FILE_READ_ONLY_DEVICE | \
                                                 FILE_FLOPPY_DISKETTE    \
                                                 )

VOID
BlkMovSyncFilterWithTarget(
    IN PDEVICE_OBJECT FilterDevice,
    IN PDEVICE_OBJECT TargetDevice
    )
{
    ULONG                   propFlags;

    PAGED_CODE();

    //
    // Propogate all useful flags from target to diskperf. MountMgr will look
    // at the diskperf object capabilities to figure out if the disk is
    // a removable and perhaps other things.
    //
    propFlags = TargetDevice->Flags & FILTER_DEVICE_PROPOGATE_FLAGS;
    FilterDevice->Flags |= propFlags;

    propFlags = TargetDevice->Characteristics & FILTER_DEVICE_PROPOGATE_CHARACTERISTICS;
    FilterDevice->Characteristics |= propFlags;


}

NTSTATUS
BlkMovAddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject
    )
{
    NTSTATUS                status;
    IO_STATUS_BLOCK         ioStatus;
    PDEVICE_OBJECT          filterDeviceObject;
    PDEVICE_EXTENSION       deviceExtension;
    STRING                  ntNameString;
    UNICODE_STRING          ntUnicodeString;
    PIRP                    irp;
    STORAGE_DEVICE_NUMBER   number;
    ULONG                   registrationFlag = 0;
    PWMILIB_CONTEXT         wmilibContext;
    PCHAR                   buffer;
    ULONG                   buffersize;

    PAGED_CODE();

	if(PhysicalDeviceObject->DeviceType == FILE_DEVICE_MASS_STORAGE)
	{
		//
		// Create a filter device object for this device (partition).
		//

		DbgPrint("BLKMOVER:\tBlkMovAddDevice\n");

		status = IoCreateDevice(DriverObject,
								DEVICE_EXTENSION_SIZE,
								NULL,
								FILE_DEVICE_DISK,
								0,
								FALSE,
								&filterDeviceObject);

		if (!NT_SUCCESS(status)) {
		   return status;
		}

		filterDeviceObject->Flags |= DO_DIRECT_IO;

		deviceExtension = (PDEVICE_EXTENSION) filterDeviceObject->DeviceExtension;

		RtlZeroMemory(deviceExtension, DEVICE_EXTENSION_SIZE);
		//
		// Attaches the device object to the highest device object in the chain and
		// return the previously highest device object, which is passed to
		// IoCallDriver when pass IRPs down the device stack
		//

		deviceExtension->TargetDeviceObject =
			IoAttachDeviceToDeviceStack(filterDeviceObject, PhysicalDeviceObject);

		if (deviceExtension->TargetDeviceObject == NULL)
		{
			IoDeleteDevice(filterDeviceObject);
			return STATUS_NO_SUCH_DEVICE;
		}

		CreateThread(filterDeviceObject);
		//
		// Save the filter device object in the device extension
		//
		deviceExtension->DeviceObject = filterDeviceObject;

		//
		// default to DO_POWER_PAGABLE
		//

		filterDeviceObject->Flags |=  DO_POWER_PAGABLE;

		//
		// Clear the DO_DEVICE_INITIALIZING flag
		//

		filterDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
	}
    return STATUS_SUCCESS;

} // end BlkMovAddDevice()


NTSTATUS
BlkMovDispatchPnp(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    PIO_STACK_LOCATION  irpSp = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS            status;
    PDEVICE_EXTENSION   deviceExtension;

    PAGED_CODE();

    switch(irpSp->MinorFunction)
	{

        case IRP_MN_START_DEVICE:
            //
            // Call the Start Routine handler to schedule a completion routine
            //
            status = BlkMovStartDevice(DeviceObject, Irp);
            break;

        /*case IRP_MN_REMOVE_DEVICE:
        {
            //
            // Call the Remove Routine handler to schedule a completion routine
            //
            status = BlkMovRemoveDevice(DeviceObject, Irp);
            break;
        }*/
        case IRP_MN_DEVICE_USAGE_NOTIFICATION:
        {
            PIO_STACK_LOCATION irpStack;
            ULONG count;
            BOOLEAN setPagable;

            DbgPrint("BlkMovDispatchPnp: Processing DEVICE_USAGE_NOTIFICATION\n");
            irpStack = IoGetCurrentIrpStackLocation(Irp);

            if (irpStack->Parameters.UsageNotification.Type != DeviceUsageTypePaging)
			{
                status = BlkMovSendToNextDriver(DeviceObject, Irp);
                break; // out of case statement
            }

            deviceExtension = DeviceObject->DeviceExtension;

            //
            // wait on the paging path event
            //

            status = KeWaitForSingleObject(&deviceExtension->PagingPathCountEvent,
                                           Executive, KernelMode,
                                           FALSE, NULL);
            //
            // if removing last paging device, need to set DO_POWER_PAGABLE
            // bit here, and possible re-set it below on failure.
            //
			setPagable = FALSE;
            if (!irpStack->Parameters.UsageNotification.InPath &&
                deviceExtension->PagingPathCount == 1 )
			{
                //
                // removing the last paging file
                // must have DO_POWER_PAGABLE bits set
                //

                if (DeviceObject->Flags & DO_POWER_INRUSH)
				{
                    DbgPrint("BlkMovDispatchPnp: last paging file "
                                "removed but DO_POWER_INRUSH set, so not "
                                "setting PAGABLE bit "
                                "for DO %p\n", DeviceObject);
                }
				else
				{
                    DbgPrint("BlkMovDispatchPnp: Setting  PAGABLE "
                                "bit for DO %p\n", DeviceObject);
                    DeviceObject->Flags |= DO_POWER_PAGABLE;
                    setPagable = TRUE;
                }

            }

            //
            // send the irp synchronously
            //

            status = BlkMovForwardIrpSynchronous(DeviceObject, Irp);

            //
            // now deal with the failure and success cases.
            // note that we are not allowed to fail the irp
            // once it is sent to the lower drivers.
            //

            if (NT_SUCCESS(status))
			{
                IoAdjustPagingPathCount(
                    &deviceExtension->PagingPathCount,
                    irpStack->Parameters.UsageNotification.InPath);

                if (irpStack->Parameters.UsageNotification.InPath)
				{
                    if (deviceExtension->PagingPathCount == 1)
					{

                        //
                        // first paging file addition
                        //

                        DeviceObject->Flags &= ~DO_POWER_PAGABLE;
                    }
                }

            }
			else
			{

                //
                // cleanup the changes done above
                //

                if (setPagable == TRUE)
				{
                    DeviceObject->Flags &= ~DO_POWER_PAGABLE;
                    setPagable = FALSE;
                }

            }

            //
            // set the event so the next one can occur.
            //

            KeSetEvent(&deviceExtension->PagingPathCountEvent,
                       IO_NO_INCREMENT, FALSE);

            //
            // and complete the irp
            //

            IoCompleteRequest(Irp, IO_NO_INCREMENT);
            return status;
            break;

        }
		case IRP_MN_REMOVE_DEVICE:
			return BlkMovSendToNextDriver(DeviceObject, Irp);
		case IRP_MN_STOP_DEVICE:
			return BlkMovSendToNextDriver(DeviceObject, Irp);
        default:
            //
            // Simply forward all other Irps
            //
            return BlkMovSendToNextDriver(DeviceObject, Irp);

    }

    return status;

} // end BlkMovDispatchPnp()


NTSTATUS
DiskPerfIrpCompletion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    )
{
    PKEVENT Event = (PKEVENT) Context;

    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);

    KeSetEvent(Event, IO_NO_INCREMENT, FALSE);

    return(STATUS_MORE_PROCESSING_REQUIRED);

} // end DiskPerfIrpCompletion()

NTSTATUS
BlkMovStartDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    PDEVICE_EXTENSION   deviceExtension;
    KEVENT              event;
    NTSTATUS            status;
	DWORD				MBR[128];
	MOVEHEADSEC			Sector;

    PAGED_CODE();

    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    status = BlkMovForwardIrpSynchronous(DeviceObject, Irp);

    BlkMovSyncFilterWithTarget(DeviceObject,
                                 deviceExtension->TargetDeviceObject);
    //
    // Complete the Irp
    //
    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}

NTSTATUS
BlkMovRemoveDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    NTSTATUS            status;
    PDEVICE_EXTENSION   deviceExtension;

    PAGED_CODE();

    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    status = BlkMovForwardIrpSynchronous(DeviceObject, Irp);

    IoDetachDevice(deviceExtension->TargetDeviceObject);
	DeleteThread(DeviceObject);
    IoDeleteDevice(DeviceObject);

    //
    // Complete the Irp
    //
    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}


NTSTATUS
BlkMovSendToNextDriver(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    PDEVICE_EXTENSION   deviceExtension;

    IoSkipCurrentIrpStackLocation(Irp);
    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    return IoCallDriver(deviceExtension->TargetDeviceObject, Irp);

} // end BlkMovSendToNextDriver()

NTSTATUS
BlkMovDispatchPower(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    PDEVICE_EXTENSION	deviceExtension;
    PIO_STACK_LOCATION  currentIrpStack = IoGetCurrentIrpStackLocation(Irp);

    PoStartNextPowerIrp(Irp);
    IoSkipCurrentIrpStackLocation(Irp);

    deviceExtension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;
    return PoCallDriver(deviceExtension->TargetDeviceObject, Irp);

} // end BlkMovDispatchPower

NTSTATUS
BlkMovForwardIrpSynchronous(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    PDEVICE_EXTENSION   deviceExtension;
    KEVENT event;
    NTSTATUS status;

    KeInitializeEvent(&event, NotificationEvent, FALSE);
    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    //
    // copy the irpstack for the next device
    //

    IoCopyCurrentIrpStackLocationToNext(Irp);

    //
    // set a completion routine
    //

    IoSetCompletionRoutine(Irp, DiskPerfIrpCompletion,
                            &event, TRUE, TRUE, TRUE);

    //
    // call the next lower device
    //

    status = IoCallDriver(deviceExtension->TargetDeviceObject, Irp);

    //
    // wait for the actual completion
    //

    if (status == STATUS_PENDING)
	{
        KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
        status = Irp->IoStatus.Status;
    }

    return status;

} // end DiskPerfForwardIrpSynchronous()

NTSTATUS
BlkMovCreate(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    UNREFERENCED_PARAMETER(DeviceObject);

    Irp->IoStatus.Status = STATUS_SUCCESS;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;

} // end BlkMovCreate()


NTSTATUS
BlkMovReadWrite(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
	DWORD				dwStartSec;
	DWORD				dwLength;
	KIRQL				Irql;
	NTSTATUS			status;
	PVOID				pBuffer;
	PDEVICE_EXTENSION	deviceExtension = DeviceObject->DeviceExtension;
    PIO_STACK_LOCATION	currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
    PIO_STACK_LOCATION	nextIrpStack = IoGetNextIrpStackLocation(Irp);

	dwLength	= currentIrpStack->Parameters.Read.Length/SECTOR_SIZE;
	dwStartSec	= (DWORD)(currentIrpStack->Parameters.Read.ByteOffset.QuadPart/SECTOR_SIZE);

	if(g_pMoverData)// && g_dwMovedRecNum)
	{
		if(g_pMoverData->DeviceObject == DeviceObject &&
			!(dwStartSec >= g_dwOrigLast ||	dwStartSec + dwLength <= g_dwOrig1st))
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
			return STATUS_PENDING;
		}
	}

	if(g_bReLocate && deviceExtension->bInitMover)
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
		return STATUS_PENDING;
	}
    //
    // Copy current stack to next stack.
    //

    *nextIrpStack = *currentIrpStack;
    //
    // Set completion routine callback.
    //
	IoSetCompletionRoutine(Irp,
		                       BlkMovIoCompletion,
		                       DeviceObject,
		                       TRUE,
		                       TRUE,
		                       TRUE);

	status = IoCallDriver(deviceExtension->TargetDeviceObject,
								    Irp);
	
	return status;

} // end BlkMovReadWrite()

NTSTATUS
AsyncReadWriteSec(
    IN PDEVICE_OBJECT DeviceObject,
	IN PIRP	 ParentIrp,
	IN ULONG ulStartSec,
	IN ULONG ulSectors,
	IN PVOID pBuffer,
    IN UCHAR MajorFunction
)
{
	PMDL				MDL;
    KEVENT				event;
    PIRP				Irp;
	ULONG				ulBytes;
    LARGE_INTEGER		Start;
    IO_STATUS_BLOCK		ioStatus;
	NTSTATUS			status = STATUS_SUCCESS;
    PIO_STACK_LOCATION	NextIrpStack;
	PDEVICE_EXTENSION	deviceExtension = DeviceObject->DeviceExtension;

	ulBytes = ulSectors * SECTOR_SIZE;
	Start.QuadPart = ((LONGLONG)ulStartSec)*SECTOR_SIZE;

	Irp = IoAllocateIrp(LOBYTE(LOWORD(deviceExtension->TargetDeviceObject->StackSize+1)),FALSE);
	//Irp = IoBuildSynchronousFsdRequest( MajorFunction,
	//	                                deviceExtension->TargetDeviceObject,
	//	                                pBuffer,ulBytes,
	//	                                &Start,&event,
	//	                                &ioStatus );

	ASSERT(Irp);
	if (!Irp)
	{
	    return STATUS_INSUFFICIENT_RESOURCES;
	}

	MDL = IoAllocateMdl(pBuffer,ulBytes,FALSE,FALSE,Irp);
	if (!MDL)
		return STATUS_INSUFFICIENT_RESOURCES;

	MmBuildMdlForNonPagedPool(MDL);
	IoSetNextIrpStackLocation(Irp);
	NextIrpStack = IoGetNextIrpStackLocation(Irp);

	NextIrpStack->DeviceObject  = deviceExtension->TargetDeviceObject;
	NextIrpStack->MajorFunction = MajorFunction;
	NextIrpStack->MinorFunction = 0;

	NextIrpStack->Parameters.Read.Length = ulBytes;
	NextIrpStack->Parameters.Read.Key		 = 0;
	NextIrpStack->Parameters.Read.ByteOffset.QuadPart = Start.QuadPart;

	RtlCopyMemory(&(NextIrpStack->Parameters.Write),&(NextIrpStack->Parameters.Read),sizeof(NextIrpStack->Parameters.Read));
	IoSetCompletionRoutine(Irp,AsyncCompletion,ParentIrp,TRUE,TRUE,TRUE);
	status = IoCallDriver(deviceExtension->TargetDeviceObject,Irp);
	return status;
} // end SyncReadWriteSec()

NTSTATUS
SyncReadWriteSec(
    IN PDEVICE_OBJECT DeviceObject,
	IN ULONG ulStartSec,
	IN ULONG ulSectors,
	IN PVOID pBuffer,
    IN UCHAR MajorFunction
)
{
    KEVENT				event;
    PIRP				Irp;
	ULONG				ulBytes;
    LARGE_INTEGER		Start;
    IO_STATUS_BLOCK		ioStatus;
	NTSTATUS			status = STATUS_SUCCESS;
	PDEVICE_EXTENSION	deviceExtension = DeviceObject->DeviceExtension;

	ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

	ulBytes = ulSectors * SECTOR_SIZE;
	Start.QuadPart = ((LONGLONG)ulStartSec)*SECTOR_SIZE;

	KeInitializeEvent( &event, NotificationEvent, FALSE );

	Irp = IoBuildSynchronousFsdRequest( MajorFunction,
		                                deviceExtension->TargetDeviceObject,
		                                pBuffer,ulBytes,
		                                &Start,&event,
		                                &ioStatus );

	ASSERT(Irp);
	if (!Irp)
	{
	    return STATUS_INSUFFICIENT_RESOURCES;
	}

	//Irp->Flags |= IRP_PAGING_IO;
	
	status = IoCallDriver( deviceExtension->TargetDeviceObject, Irp );

	if (status == STATUS_PENDING)
	{
	    (VOID) KeWaitForSingleObject( &event,
	                              Executive,
	                              KernelMode,
	                              FALSE,
	                              (PLARGE_INTEGER) NULL);
	    status = ioStatus.Status;
	}

	return status;
} // end SyncReadWriteSec()

NTSTATUS
FlushBuf(IN PDEVICE_OBJECT DeviceObject)
{
    KEVENT				event;
    PIRP				Irp;
    IO_STATUS_BLOCK		ioStatus;
	NTSTATUS			status = STATUS_SUCCESS;
	PDEVICE_EXTENSION	deviceExtension = DeviceObject->DeviceExtension;

	ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

	KeInitializeEvent( &event, NotificationEvent, FALSE );

	Irp = IoBuildSynchronousFsdRequest( IRP_MJ_FLUSH_BUFFERS,
		                                deviceExtension->TargetDeviceObject,
		                                NULL,0,
		                                NULL,&event,
		                                &ioStatus );

	ASSERT(Irp);
	if (!Irp)
	{
	    return STATUS_INSUFFICIENT_RESOURCES;
	}

	status = IoCallDriver( deviceExtension->TargetDeviceObject, Irp );

	if (status == STATUS_PENDING)
	{
	    (VOID) KeWaitForSingleObject( &event,
	                              Executive,
	                              KernelMode,
	                              FALSE,
	                              (PLARGE_INTEGER) NULL);
	    status = ioStatus.Status;
	}

	return status;
} // end SyncReadWriteSec()

NTSTATUS
AsyncCompletion(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp,IN PVOID Context)
{
  //  PIO_STACK_LOCATION  irpStack          = IoGetCurrentIrpStackLocation(Irp);
	KIRQL				Irql;
	PIRP				ParentIrp = (PIRP)Context;
	PIO_STACK_LOCATION	ParentIrpNextStack = IoGetNextIrpStackLocation(ParentIrp);

	IoFreeMdl(Irp->MdlAddress);
    IoFreeIrp(Irp);

	KeAcquireSpinLock(&g_IrpSpinLock,&Irql);
	if (InterlockedDecrement((LPDWORD)&ParentIrpNextStack->FileObject) == 0)
    {
		KeReleaseSpinLock(&g_IrpSpinLock,Irql);
		IoCompleteRequest(ParentIrp, IO_NO_INCREMENT);
	}
	else
		KeReleaseSpinLock(&g_IrpSpinLock,Irql);

	return STATUS_MORE_PROCESSING_REQUIRED;
} 

NTSTATUS
BlkMovIoCompletion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp,
    IN PVOID          Context
    )
{
  //  PIO_STACK_LOCATION  irpStack          = IoGetCurrentIrpStackLocation(Irp);
	PVOID				pBuffer;
	ULONG				ulOffset;
	ULONG				ulLength;

    UNREFERENCED_PARAMETER(Context);

    if (Irp->PendingReturned)
	{
        IoMarkIrpPending(Irp);
    }
    return STATUS_SUCCESS;

} // BlkMovIoCompletion

NTSTATUS
BlkMovShutdownFlush(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    PDEVICE_EXTENSION  deviceExtension = DeviceObject->DeviceExtension;

    //
    // Set current stack back one.
    //
    Irp->CurrentLocation++;
    Irp->Tail.Overlay.CurrentStackLocation++;

    return IoCallDriver(deviceExtension->TargetDeviceObject, Irp);

} // end BlkMovShutdownFlush()


VOID
BlkMovUnload(
    IN PDRIVER_OBJECT DriverObject
    )
{
    PAGED_CODE();

    return;
}

NTSTATUS
BlkMoverRelocateReadWrite(
	  IN PDEVICE_OBJECT DeviceObject,
	  IN UCHAR MajorFunction,
	  IN ULONG dwStartSec,	
	  IN ULONG dwLength,
	  IN PBYTE pBuffer
	  )
{
	DWORD				i;
	PBYTE				pCurBuffer;
	NTSTATUS			status;

	status = STATUS_ACCESS_VIOLATION;

	if(g_bReLocate)
	{
		status  = DoReLocate(MajorFunction,dwStartSec,	dwLength,pBuffer,DeviceObject);
	}
	else
	{
		if(g_pMoverData)
		{
			if(g_pMoverData->bWorking)
			{
				if(!IsRWToFreeSpace(dwStartSec,dwLength))
					status  = ReadWriteFromGroup(MajorFunction,dwStartSec,dwLength,pBuffer,TRUE,g_pMoverData);
			}
			else
			{
				if(g_pMoverData->bInitSec && MajorFunction == IRP_MJ_WRITE)
				{
					pCurBuffer = (PBYTE)pBuffer;
					for(i=0;i<dwLength;i++)
					{
						if(RtlCompareMemory(pCurBuffer,g_pCompInitBuf,SECTOR_SIZE) == SECTOR_SIZE)
							pCurBuffer += SECTOR_SIZE;
						else
							break;
					}
					if(i==dwLength)
					{
						status = STATUS_SUCCESS;
					}
					else
					{
						status = SyncReadWriteSec(DeviceObject,dwStartSec,dwLength,
									pBuffer,MajorFunction);
					}
				}
				else
				{
					if(!IsRWToFreeSpace(dwStartSec,dwLength))
					{
						if(g_bEnableProtect)
						{
							status  = ReadWriteFromGroup(MajorFunction,dwStartSec,dwLength,pBuffer,FALSE,g_pMoverData);
							if(status == STATUS_SUCCESS && MajorFunction == IRP_MJ_WRITE)
							{
								status  = ReadWriteFromGroup(MajorFunction,dwStartSec,dwLength,pBuffer,TRUE,g_pMoverData);
							}
						}
						else
							status = SyncReadWriteSec(DeviceObject,dwStartSec,dwLength,
										pBuffer,MajorFunction);
					}
				}
			}
		}
	}
	return status;
}

VOID
Thread (
    IN PVOID Context
    )
{
    PDEVICE_OBJECT      DeviceObject;
    PDEVICE_EXTENSION   deviceExtension;
    PLIST_ENTRY         request;
    PIRP                Irp;
    PIO_STACK_LOCATION  currentIrpStack;
    PIO_STACK_LOCATION  nextIrpStack;
	DWORD				dwStartSec;
	DWORD				dwLength;
	PVOID				pBuffer;
	NTSTATUS			status;
	DWORD				dwRetVal;
	BOOL				bReadSys;
	PBLK_MOVER_PARAM	pParam;
    DIOC_REGISTERS		*pRegs;
	PREAD_WRITE_BUFFER	pReadWriteBuffer;
	KIRQL				Irql;
//  PAGED_CODE();
    ASSERT(Context != NULL);

    DeviceObject = (PDEVICE_OBJECT) Context;

    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    KeSetPriorityThread(KeGetCurrentThread(),HIGH_PRIORITY);//LOW_REALTIME_PRIORITY);// HIGH_PRIORITY);

    while(TRUE)
    {
        KeWaitForSingleObject(
            &deviceExtension->request_event,
            Executive,
            KernelMode,
            FALSE,
            NULL
            );

		ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);
        if (deviceExtension->terminate_thread)
        {
            PsTerminateSystemThread(STATUS_SUCCESS);
        }

        while (request = ExInterlockedRemoveHeadList(
            &deviceExtension->list_head,
            &deviceExtension->list_lock
            ))
        {
            Irp = CONTAINING_RECORD(request, IRP, Tail.Overlay.ListEntry);

            currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
			nextIrpStack = IoGetNextIrpStackLocation(Irp);
			
			ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);
            switch (currentIrpStack->MajorFunction)
            {
            case IRP_MJ_READ:
            case IRP_MJ_WRITE:
				pBuffer		= MmGetSystemAddressForMdlSafe(Irp->MdlAddress,NormalPagePriority);
				dwLength	= currentIrpStack->Parameters.Read.Length/SECTOR_SIZE;
				dwStartSec	= (DWORD)(currentIrpStack->Parameters.Read.ByteOffset.QuadPart/SECTOR_SIZE);

				status = BlkMoverRelocateReadWrite(DeviceObject,currentIrpStack->MajorFunction,dwStartSec,dwLength,pBuffer);

				//ASSERT(status == STATUS_SUCCESS);
				Irp->IoStatus.Status	  = status;
				Irp->IoStatus.Information = dwLength*SECTOR_SIZE;
				break;
            case IRP_MJ_DEVICE_CONTROL:
                switch (currentIrpStack->Parameters.DeviceIoControl.IoControlCode)
                {
				case IOCTL_YG_BLOCK_MOVER_FLUSH_BUFFER:
					pRegs = (DIOC_REGISTERS *)Irp->AssociatedIrp.SystemBuffer;
					pRegs->reg_EAX = TRUE;
					
					status = FlushBuf(g_pMoverData->DeviceObject);
			        Irp->IoStatus.Status	  = status;
					Irp->IoStatus.Information = sizeof(DIOC_REGISTERS);
                    break;
                case IOCTL_YG_BLOCK_MOVING_GROUP_CURRENT:
					pRegs = (DIOC_REGISTERS *)Irp->AssociatedIrp.SystemBuffer;

					g_MovingGroup.bFront		= pRegs->reg_EAX;
					g_MovingGroup.dwSStart		= pRegs->reg_EBX;
					g_MovingGroup.dwTStart		= pRegs->reg_ECX;
					g_MovingGroup.dwSize		= pRegs->reg_EDX;
					g_MovingGroup.dwMovedSize	= 0;
					g_dwMovedRecNum				= pRegs->reg_EDI;
					g_pMoverData->bWorking		= TRUE;

					pRegs->reg_EAX				= TRUE;

			        Irp->IoStatus.Status	  = STATUS_SUCCESS;
					Irp->IoStatus.Information = sizeof(DIOC_REGISTERS);
                    break;
				case IOCTL_YG_GET_MOVED_SECTORS:
					pRegs = (DIOC_REGISTERS *)Irp->AssociatedIrp.SystemBuffer;
					pRegs->reg_EAX = g_dwMovedSecs;

			        Irp->IoStatus.Status	  = STATUS_SUCCESS;
					Irp->IoStatus.Information = sizeof(DIOC_REGISTERS);
                    break;
				case IOCTL_YG_READ:
					pReadWriteBuffer = (PREAD_WRITE_BUFFER)Irp->AssociatedIrp.SystemBuffer;
					dwLength		 = pReadWriteBuffer->dwLength;
					pBuffer			 = MmGetSystemAddressForMdlSafe(Irp->MdlAddress,NormalPagePriority);
					dwStartSec		 = pReadWriteBuffer->dwStartSec;

					if (pReadWriteBuffer->dwMovedRecNum == YGBLK_READ_WRITE)
					{
						status = BlkMoverRelocateReadWrite(DeviceObject,IRP_MJ_READ,dwStartSec,dwLength,pBuffer);
					}
					else
					{
						status = SyncReadWriteSec(DeviceObject,dwStartSec,dwLength ,pBuffer ,IRP_MJ_READ);
					}
					MmUnlockPages(Irp->MdlAddress);
					IoFreeMdl(Irp->MdlAddress);
					Irp->MdlAddress = NULL;
			        Irp->IoStatus.Status	  = STATUS_SUCCESS;
					Irp->IoStatus.Information = sizeof(READ_WRITE_BUFFER);
                    break;
				case IOCTL_YG_WRITE:
					pReadWriteBuffer = (PREAD_WRITE_BUFFER)Irp->AssociatedIrp.SystemBuffer;
					dwLength		 = pReadWriteBuffer->dwLength;
					pBuffer			 = MmGetSystemAddressForMdlSafe(Irp->MdlAddress,NormalPagePriority);
					dwStartSec		 = pReadWriteBuffer->dwStartSec;

					if (pReadWriteBuffer->dwMovedRecNum == YGBLK_READ_WRITE)
					{
						status = BlkMoverRelocateReadWrite(DeviceObject,IRP_MJ_WRITE,dwStartSec,dwLength,pBuffer);
					}
					else
					{
						status = SyncReadWriteSec(DeviceObject,dwStartSec,dwLength ,pBuffer ,IRP_MJ_WRITE);
					}
					MmUnlockPages(Irp->MdlAddress);
					IoFreeMdl(Irp->MdlAddress);
					Irp->MdlAddress = NULL;

					if (pReadWriteBuffer->dwMovedRecNum != YGBLK_READ_WRITE)
					{
						if(!pReadWriteBuffer->bSys && g_dwDataRecNum)
						{
							//g_dwMovedRecNum = pReadWriteBuffer->dwMovedRecNum;
							g_MovingGroup.dwMovedSize += dwLength;
						}

						if(g_bReLocate && pReadWriteBuffer->bSys) 
							g_dwRePointer = pReadWriteBuffer->dwStartSec + dwLength;
					}

			        Irp->IoStatus.Status	  = STATUS_SUCCESS;
					Irp->IoStatus.Information = sizeof(READ_WRITE_BUFFER);
                    break;
                default:
                    Irp->IoStatus.Status = STATUS_DRIVER_INTERNAL_ERROR;
                    Irp->IoStatus.Information = 0;
                }
                break;
            default:
                DbgPrint("BlkMover:Thread default conditions.");

                Irp->IoStatus.Status = STATUS_DRIVER_INTERNAL_ERROR;
                Irp->IoStatus.Information = 0;
            }
			IoCompleteRequest(Irp,
							(CCHAR) (NT_SUCCESS(Irp->IoStatus.Status) ?
							IO_DISK_INCREMENT : IO_NO_INCREMENT));
        }
    }
}

NTSTATUS
CreateThread(
    IN PDEVICE_OBJECT DeviceObject
)
{
	PDEVICE_EXTENSION  deviceExtension = DeviceObject->DeviceExtension;
    HANDLE			   thread_handle;
    NTSTATUS           status;
	//
	//create thread and set spin lock
	//
	ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

    InitializeListHead(&deviceExtension->list_head);

    KeInitializeSpinLock(&deviceExtension->list_lock);

	KeInitializeEvent(&deviceExtension->PagingPathCountEvent,
						  NotificationEvent, TRUE);
	KeInitializeEvent(&deviceExtension->request_event,
						  SynchronizationEvent,FALSE);

    deviceExtension->terminate_thread = FALSE;

    status = PsCreateSystemThread(
        &thread_handle,
        (ACCESS_MASK) 0L,
        NULL,
        NULL,
        NULL,
        Thread,
        DeviceObject
        );

    if (!NT_SUCCESS(status))
    {
        //DeleteDevice(DeviceObject);
        return status;
    }

    status = ObReferenceObjectByHandle(
        thread_handle,
        THREAD_ALL_ACCESS,
        NULL,
        KernelMode,
        &deviceExtension->thread_pointer,
        NULL
        );

    if (!NT_SUCCESS(status))
    {
        ZwClose(thread_handle);

        deviceExtension->terminate_thread = TRUE;

        KeSetEvent(
            &deviceExtension->request_event,
            (KPRIORITY) 0,
            FALSE
            );

        return status;
    }

    ZwClose(thread_handle);
    return status;
}

NTSTATUS
DeleteThread (
    IN PDEVICE_OBJECT DeviceObject
    )
{
    PDEVICE_EXTENSION   deviceExtension;

    PAGED_CODE();
    ASSERT(DeviceObject != NULL);

    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    deviceExtension->terminate_thread = TRUE;

    KeSetEvent(
        &deviceExtension->request_event,
        (KPRIORITY) 0,
        FALSE
        );

    KeWaitForSingleObject(
        deviceExtension->thread_pointer,
        //Executive,
		Executive,
        KernelMode,
        FALSE,
        NULL
        );

    ObDereferenceObject(deviceExtension->thread_pointer);
    return STATUS_SUCCESS;
}
