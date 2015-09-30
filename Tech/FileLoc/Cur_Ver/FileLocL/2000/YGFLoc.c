/*++

Copyright (c) 2001  Microsoft Corporation

Module Name:    YGFLoc.c

Abstract:

    This driver can help ring3 get the location of file data.

Authors: LHB

Environment:    kernel mode only
--*/

#include "ntddk.h"
#include "stdarg.h"
#include "stdio.h"
#include "ntdddisk.h"
#include "..\..\..\Export\2000\YGFloc.h"
#include "..\..\..\..\..\VFS\Cur_ver\Export\2000\YGVParMP.h"
#include "..\..\..\..\..\VirParti\Cur_ver\Export\2000\YGVPar.h"
//
// Device Extension
//

typedef struct _deviceExtension {
    PDEVICE_OBJECT TargetDeviceObject;// Target Device Object
	PDRIVER_OBJECT pDriverObject;
    LIST_ENTRY              pListHead;
    KSPIN_LOCK              klListLock;
    KEVENT                  keRequestEvent;
    PVOID                   pThreadPointer;
    BOOLEAN                 bTerminateThread;
	BOOLEAN                 bInited;
	LONGLONG                *pSectorAddrBuf;
	USHORT                  usAddressNum; 
	LONGLONG                llAddressBase;
	LOC_INFO                SignSector;
	USHORT                  ulNumber;
	ULONG                   ulDeviceType;
} DEVICEEXTENSION, *PDEVICEEXTENSION;

//
// Function declarations
//
VOID
DriverUnload (
    IN PDRIVER_OBJECT DriverObject
    );

VOID
Thread (
    IN PVOID Context
    );

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    );

VOID
YGFLocInitialize(
    IN PDRIVER_OBJECT DriverObject
    );

NTSTATUS
YGFLocCreate(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
YGFLocReadWrite(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
YGFLocDeviceControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
YGFLocShutdownFlush(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

BOOLEAN IsOurLockSector(
	IN PDEVICEEXTENSION  pDeviceExt,
	IN PLOC_INFO pLockSec,
	IN ULONG ulParOffset);

NTSTATUS 
SyncWriteDevice(
	IN PDEVICEEXTENSION pDeviceExt,
	IN UCHAR *pBuf,
	IN PLARGE_INTEGER lnOffset,
	IN ULONG ulLength);


NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )

/*++

Routine Description:

    This is the routine called by the system to initialize the disk
    performance driver. The driver object is set up and then the
    driver calls YGFLocInitialize to attach to the boot devices.

Arguments:

    DriverObject - The disk performance driver object.

Return Value:

    NTSTATUS

--*/

{
    //
    // Set up the device driver entry points.
    //
    DriverObject->DriverUnload = DriverUnload;
    DriverObject->MajorFunction[IRP_MJ_CREATE] = YGFLocCreate;
    DriverObject->MajorFunction[IRP_MJ_READ] = YGFLocReadWrite;
    DriverObject->MajorFunction[IRP_MJ_WRITE] = YGFLocReadWrite;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = YGFLocDeviceControl;
    DriverObject->MajorFunction[IRP_MJ_SHUTDOWN] = YGFLocShutdownFlush;
    DriverObject->MajorFunction[IRP_MJ_FLUSH_BUFFERS] = YGFLocShutdownFlush;

    //
    // Call the initialization routine for the first time.
    //
    YGFLocInitialize(DriverObject);

    return(STATUS_SUCCESS);

} // DriverEntry
VOID
DriverUnload (
    IN PDRIVER_OBJECT DriverObject
    )
{
	PDEVICE_OBJECT device_object;
	PDEVICEEXTENSION    pDeviceExt;
	PDEVICE_OBJECT      next_device_object;
	PAGED_CODE();

	device_object = DriverObject->DeviceObject;

	while (device_object)
	{
		pDeviceExt = (PDEVICEEXTENSION) device_object->DeviceExtension;
		pDeviceExt->bTerminateThread = TRUE;
		KeSetEvent(&pDeviceExt->keRequestEvent,(KPRIORITY) 0,FALSE);
		KeWaitForSingleObject(pDeviceExt->pThreadPointer,Executive,KernelMode,FALSE,NULL);
		ObDereferenceObject(pDeviceExt->pThreadPointer);
		next_device_object = device_object->NextDevice;
		IoDeleteDevice(device_object);
        device_object = next_device_object;
    }

}


VOID
YGFLocInitialize(
    IN PDRIVER_OBJECT DriverObject
    )

/*++

Routine Description:

    Attach to new disk devices and partitions.
    Set up device objects for counts and times.
    If this is the first time this routine is called,
    then register with the IO system to be called
    after all other disk device drivers have initiated.

Arguments:

    DriverObject - Disk performance driver object.
    NextDisk - Starting disk for this part of the initialization.
    Count - Not used. Number of times this routine has been called.

Return Value:

    NTSTATUS

--*/

{
    WCHAR						device_name_buffer[MAXIMUM_FILENAME_LENGTH];
    UNICODE_STRING             ntUnicodeString;
    PDEVICE_OBJECT             physicalDevice;
    PDEVICEEXTENSION           pDeviceExt;
    NTSTATUS                   status;
    USHORT                     ulDriverNum,i,j;
    HANDLE                     thread_handle;

    for (j = 0;j < MAX_VPAR_DRIVE + MAX_VPARMP_DRIVE;j++)
	{
		if (j < MAX_VPAR_DRIVE)
		{
			i = VOLUME_VIRTUAL_PARTITION;
			ulDriverNum = j;
			swprintf(device_name_buffer,YGVPAR_DIR_NAME L"%d", ulDriverNum);
		}
		else
		{
			i = VOLUME_PARTITION_MAP;
			ulDriverNum = j - MAX_VPAR_DRIVE;
			swprintf(device_name_buffer,YGVPARMP_DIR_NAME L"%d", ulDriverNum);
		}
		RtlInitUnicodeString(&ntUnicodeString, device_name_buffer);
        status = IoCreateDevice(DriverObject,
                                sizeof(DEVICEEXTENSION),
                                NULL,
                                FILE_DEVICE_DISK,
                                0,
                                FALSE,
                                &physicalDevice);

        if (!NT_SUCCESS(status)) continue;
		physicalDevice->Flags |= DO_DIRECT_IO;
		pDeviceExt = physicalDevice->DeviceExtension;

		pDeviceExt->pSectorAddrBuf = NULL;
		pDeviceExt->ulDeviceType = i;
		pDeviceExt->ulNumber = ulDriverNum;
		pDeviceExt->usAddressNum = 0;
		pDeviceExt->bTerminateThread = FALSE;
		pDeviceExt->pDriverObject = DriverObject;

		pDeviceExt->SignSector.usLocSign = YGLOCSIGN;
		for (i = 0;i < TOTAL_SIGN_LEN;i+=SIGN_LEN)
		{
			RtlCopyMemory(&pDeviceExt->SignSector.LocSign[i],SIGN_STRING,SIGN_LEN);
		}

		InitializeListHead(&pDeviceExt->pListHead);
		KeInitializeSpinLock(&pDeviceExt->klListLock);
		KeInitializeEvent(&pDeviceExt->keRequestEvent,SynchronizationEvent,FALSE);
		status = PsCreateSystemThread(&thread_handle,(ACCESS_MASK) 0L,NULL,
									NULL,NULL,Thread,physicalDevice);
		if (!NT_SUCCESS(status))
		{
			IoDeleteDevice(physicalDevice);
			continue;
		}

		status = ObReferenceObjectByHandle(
			thread_handle,
			THREAD_ALL_ACCESS,
			NULL,
			KernelMode,
			&pDeviceExt->pThreadPointer,
			NULL
			);

		if (NT_SUCCESS(status))
			status = IoAttachDevice(physicalDevice,
									&ntUnicodeString,
									&pDeviceExt->TargetDeviceObject);

		if (!NT_SUCCESS(status))
		{
			ZwClose(thread_handle);

			pDeviceExt->bTerminateThread = TRUE;

			KeSetEvent(
				&pDeviceExt->keRequestEvent,
				(KPRIORITY) 0,
				FALSE
				);

			IoDeleteDevice(physicalDevice);

			continue;
		}
		ZwClose(thread_handle);


		physicalDevice->AlignmentRequirement =
				pDeviceExt->TargetDeviceObject->AlignmentRequirement;

    }

    return;

} // end YGFLocInitialize()


NTSTATUS
YGFLocCreate(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine services open commands. It establishes
    the driver's existance by returning status success.

Arguments:

    DeviceObject - Context for the activity.
    Irp          - The device control argument block.

Return Value:

    NT Status

--*/

{
    UNREFERENCED_PARAMETER(DeviceObject);

    Irp->IoStatus.Status = STATUS_SUCCESS;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;

} // end YGFLocCreate()


NTSTATUS
YGFLocReadWrite(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This is the driver entry point for read and write requests
    to disks to which the YGFLoc driver has attached.
    This driver collects statistics and then sets a completion
    routine so that it can collect additional information when
    the request completes. Then it calls the next driver below
    it.

Arguments:

    DeviceObject
    Irp

Return Value:

    NTSTATUS

--*/

{
    PDEVICEEXTENSION    pDeviceExt;
    PIO_STACK_LOCATION  io_stack;
    ULONG               length;

    pDeviceExt = (PDEVICEEXTENSION) DeviceObject->DeviceExtension;

    io_stack = IoGetCurrentIrpStackLocation(Irp);
    length = io_stack->Parameters.Read.Length;

    if (pDeviceExt->bInited &&
		(length != 0) &&
        !(io_stack->Parameters.Read.ByteOffset.QuadPart % SECTOR_SIZE) &&
        !(length % SECTOR_SIZE))
    {
	    if (io_stack->MajorFunction == IRP_MJ_WRITE)
		{
		    IoMarkIrpPending(Irp);
			ExInterlockedInsertTailList(
					&pDeviceExt->pListHead,
					&Irp->Tail.Overlay.ListEntry,
					&pDeviceExt->klListLock
					);

			KeSetEvent(
					&pDeviceExt->keRequestEvent,
					(KPRIORITY) 0,
					FALSE
					);

			return STATUS_PENDING;
		}
	}
    //
    // Copy current stack to next stack.
    //

    Irp->CurrentLocation++,
    Irp->Tail.Overlay.CurrentStackLocation++;

    //
    // Return the results of the call to the disk driver.
    //

    return IoCallDriver(pDeviceExt->TargetDeviceObject,
                        Irp);

} // end YGFLocReadWrite()

BOOLEAN IsOurLockSector(IN PDEVICEEXTENSION  pDeviceExt,IN PLOC_INFO pLockSec,IN ULONG ulParOffset)
{
	ULONG   i;
	BOOLEAN bZero;
	if (pLockSec->usLocSign == pDeviceExt->SignSector.usLocSign &&
		pLockSec->usSectorBufIndex < pDeviceExt->usAddressNum)
	{
		bZero = FALSE;
		for (i=0;i<TOTAL_SIGN_LEN;i++)
		{
			if (bZero && pLockSec->LocSign[i]) return FALSE;
			else
			{
				if (pLockSec->LocSign[i] != pDeviceExt->SignSector.LocSign[i])
				{
					if (pLockSec->LocSign[i]) return FALSE;
					else bZero = TRUE;
				}
			}
		}
		return TRUE;
	}
	return FALSE;
}


NTSTATUS
YGFLocDeviceControl(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
    )

/*++

Routine Description:

    This device control dispatcher handles only the disk performance
    device control. All others are passed down to the disk drivers.
    The disk performane device control returns a current snapshot of
    the performance data.

Arguments:

    DeviceObject - Context for the activity.
    Irp          - The device control argument block.

Return Value:

    Status is returned.

--*/

{
    PDEVICEEXTENSION    pDeviceExt;
    PIO_STACK_LOCATION  io_stack;

    pDeviceExt = (PDEVICEEXTENSION) DeviceObject->DeviceExtension;

    io_stack = IoGetCurrentIrpStackLocation(Irp);

    switch (io_stack->Parameters.DeviceIoControl.IoControlCode)
    {
	case IOCTL_YGFLOC_GET_VERSION:
        if (io_stack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(DIOC_REGISTERS))
        {
            Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
			Irp->IoStatus.Information = 0;
		}
		else
		{
    		((DIOC_REGISTERS *)Irp->AssociatedIrp.SystemBuffer)->reg_EAX = YGFLOC_VERSION_MAJOR << 16 | YGFLOC_VERSION_MINOR;
			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = sizeof(DIOC_REGISTERS);
		}
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
		return Irp->IoStatus.Status;
		break;

	case IOCTL_YGFLOC_SET_SIGN:
        if (io_stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(SET_SIGNSTRU))
        {
            Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
			Irp->IoStatus.Information = 0;
		}
		else
		{
			pDeviceExt->SignSector.ulSign = (((PSET_SIGNSTRU)Irp->AssociatedIrp.SystemBuffer)->dwLocSign);
			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = 0;
		}
		IoCompleteRequest(Irp, IO_NO_INCREMENT);
		return Irp->IoStatus.Status;
		break;

    case IOCTL_YGFLOC_OPEN_BUFFER:
		if (io_stack->Parameters.DeviceIoControl.InputBufferLength <
		    sizeof(OPENBUFFER))
		{
	        Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
			Irp->IoStatus.Information = 0;
			IoCompleteRequest(Irp, IO_NO_INCREMENT);
			return Irp->IoStatus.Status;
		}
		Irp->IoStatus.Status = STATUS_PENDING;
		break;
    case IOCTL_YGFLOC_CLOSE_BUFFER:
		if (!pDeviceExt->bInited || !pDeviceExt->pSectorAddrBuf || !pDeviceExt->usAddressNum)
		{
			pDeviceExt->pSectorAddrBuf = NULL;
			pDeviceExt->usAddressNum = 0;
			pDeviceExt->llAddressBase = 0;
			pDeviceExt->bInited = FALSE;
			Irp->IoStatus.Information = 0;
			Irp->IoStatus.Status = STATUS_SUCCESS;
			IoCompleteRequest(Irp, IO_NO_INCREMENT);
			return Irp->IoStatus.Status;
		}
		else
			Irp->IoStatus.Status = STATUS_PENDING;
        break;
    case IOCTL_YGFLOC_COPY_DATA:
        if (io_stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(DIOC_REGISTERS) ||
			io_stack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(DIOC_REGISTERS))
        {
            Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
			Irp->IoStatus.Information = 0;
		}
		else
		{
			if (!pDeviceExt->pSectorAddrBuf || !pDeviceExt->usAddressNum ||
				((PCOPYDATASTRU)(Irp->AssociatedIrp.SystemBuffer))->ulSectorBufIndex >= pDeviceExt->usAddressNum ||
				((PCOPYDATASTRU)(Irp->AssociatedIrp.SystemBuffer))->ulCopyCount > pDeviceExt->usAddressNum ||
				((PCOPYDATASTRU)(Irp->AssociatedIrp.SystemBuffer))->ulSectorBufIndex + ((PCOPYDATASTRU)(Irp->AssociatedIrp.SystemBuffer))->ulCopyCount > pDeviceExt->usAddressNum)
			{
				Irp->IoStatus.Information = 0;
				Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
			}
			else
			{
				Irp->IoStatus.Status = STATUS_PENDING;
				break;
			}
		}
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
		return Irp->IoStatus.Status;
        break;
	case IOCTL_YGFLOC_CHECK_INIT:
        if (io_stack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(DIOC_REGISTERS))
        {
            Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
			Irp->IoStatus.Information = 0;
		}
		else
		{
    		((DIOC_REGISTERS *)Irp->AssociatedIrp.SystemBuffer)->reg_EAX = pDeviceExt->bInited;
			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = sizeof(DIOC_REGISTERS);
		}
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
		return Irp->IoStatus.Status;
		break;
    default:
		break;
    }

    if (Irp->IoStatus.Status != STATUS_PENDING)
    {
		Irp->CurrentLocation++,
		Irp->Tail.Overlay.CurrentStackLocation++;
		return IoCallDriver(pDeviceExt->TargetDeviceObject, Irp);
    }
	else
	{
		IoMarkIrpPending(Irp);
		ExInterlockedInsertTailList(&pDeviceExt->pListHead,&Irp->Tail.Overlay.ListEntry,&pDeviceExt->klListLock);
		KeSetEvent(&pDeviceExt->keRequestEvent,(KPRIORITY) 0, FALSE);
	}

    return Irp->IoStatus.Status;
}


NTSTATUS
YGFLocShutdownFlush(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine is called for a shutdown and flush IRPs.  These are sent by the
    system before it actually shuts down or when the file system does a flush.

Arguments:

    DriverObject - Pointer to device object to being shutdown by system.
    Irp          - IRP involved.

Return Value:

    NT Status

--*/

{
    PDEVICEEXTENSION  pDeviceExt = DeviceObject->DeviceExtension;

    //
    // Set current stack back one.
    //

    Irp->CurrentLocation++,
    Irp->Tail.Overlay.CurrentStackLocation++;

    return IoCallDriver(pDeviceExt->TargetDeviceObject, Irp);

} // end YGFLocShutdownFlush()

VOID
Thread (
    IN PVOID Context
    )
{
    PDEVICE_OBJECT      device_object;
	ULONG               i,ulSectors,ulStartOffset,j;
    LARGE_INTEGER       lnOffset;
	PLOC_INFO			pLockInfo;
    PDEVICEEXTENSION    pDeviceExt;
    PLIST_ENTRY         request;
    PIRP                Irp;
    PIO_STACK_LOCATION  io_stack;

    PAGED_CODE();
    ASSERT(Context != NULL);

    device_object = (PDEVICE_OBJECT) Context;

    pDeviceExt = (PDEVICEEXTENSION) device_object->DeviceExtension;

    KeSetPriorityThread(KeGetCurrentThread(), LOW_REALTIME_PRIORITY);

    for (;;)
    {
        KeWaitForSingleObject(
            &pDeviceExt->keRequestEvent,
            Executive,
            KernelMode,
            FALSE,
            NULL
            );

        if (pDeviceExt->bTerminateThread)
        {
            PsTerminateSystemThread(STATUS_SUCCESS);
        }

        while (request = ExInterlockedRemoveHeadList(
            &pDeviceExt->pListHead,
            &pDeviceExt->klListLock
            ))
        {
            Irp = CONTAINING_RECORD(request, IRP, Tail.Overlay.ListEntry);

            io_stack = IoGetCurrentIrpStackLocation(Irp);

            switch (io_stack->MajorFunction)
            {
            case IRP_MJ_WRITE:
				Irp->IoStatus.Status = STATUS_SUCCESS;
				ulSectors = io_stack->Parameters.Write.Length/SECTOR_SIZE;
				lnOffset = io_stack->Parameters.Write.ByteOffset;
				ulStartOffset = (ULONG)(io_stack->Parameters.Write.ByteOffset.QuadPart/SECTOR_SIZE);
                pLockInfo = (PLOC_INFO)MmGetSystemAddressForMdlSafe(Irp->MdlAddress,NormalPagePriority);
				j = 0;
				for (i = 0;i < ulSectors;i++)
				{
					if (!IsOurLockSector(pDeviceExt,&pLockInfo[i],ulStartOffset+i))	j++;
					else
					{
						if (pLockInfo[i].ulSign == pDeviceExt->SignSector.ulSign)
							pDeviceExt->pSectorAddrBuf[pLockInfo[i].usSectorBufIndex] = ulStartOffset + pDeviceExt->llAddressBase + i;
						if (j)
						{
							lnOffset.QuadPart = io_stack->Parameters.Read.ByteOffset.QuadPart + (i-j)*SECTOR_SIZE;
							Irp->IoStatus.Status = SyncWriteDevice(pDeviceExt,(UCHAR *)&pLockInfo[i-j],&lnOffset,j*SECTOR_SIZE);
							j = 0;
						}
					}
				}
				if (j)
				{
					lnOffset.QuadPart = io_stack->Parameters.Read.ByteOffset.QuadPart + (i-j)*SECTOR_SIZE;
					Irp->IoStatus.Status = SyncWriteDevice(pDeviceExt,(UCHAR *)&pLockInfo[i-j],&lnOffset,j*SECTOR_SIZE);
				}
				Irp->IoStatus.Information = io_stack->Parameters.Write.Length;
                break;

            case IRP_MJ_DEVICE_CONTROL:
                switch (io_stack->Parameters.DeviceIoControl.IoControlCode)
                {
				case IOCTL_YGFLOC_OPEN_BUFFER:
					if (pDeviceExt->bInited && pDeviceExt->pSectorAddrBuf && pDeviceExt->usAddressNum)
						ExFreePool(pDeviceExt->pSectorAddrBuf);
					pDeviceExt->usAddressNum = ((POPENBUFFER)(Irp->AssociatedIrp.SystemBuffer))->usAddressNum;
					pDeviceExt->pSectorAddrBuf = (LONGLONG *)ExAllocatePool(PagedPool,sizeof(LONGLONG)*pDeviceExt->usAddressNum);
					pDeviceExt->llAddressBase = ((POPENBUFFER)(Irp->AssociatedIrp.SystemBuffer))->llAddressBase;
					pDeviceExt->bInited = TRUE;
					Irp->IoStatus.Status = STATUS_SUCCESS;
					Irp->IoStatus.Information = 0;
					break;
				case IOCTL_YGFLOC_CLOSE_BUFFER:
					ExFreePool(pDeviceExt->pSectorAddrBuf);
					pDeviceExt->pSectorAddrBuf = NULL;
					pDeviceExt->usAddressNum = 0;
					pDeviceExt->llAddressBase = 0;
					pDeviceExt->bInited = FALSE;
					Irp->IoStatus.Status = STATUS_SUCCESS;
					Irp->IoStatus.Information = 0;
					break;
				case IOCTL_YGFLOC_COPY_DATA:
					RtlCopyMemory(&((PCOPYDATASTRU)(Irp->AssociatedIrp.SystemBuffer))->llAddressArray,
								&pDeviceExt->pSectorAddrBuf[((PCOPYDATASTRU)(Irp->AssociatedIrp.SystemBuffer))->ulSectorBufIndex],
								((PCOPYDATASTRU)(Irp->AssociatedIrp.SystemBuffer))->ulCopyCount*sizeof(LONGLONG));
					Irp->IoStatus.Status = STATUS_SUCCESS;
					Irp->IoStatus.Information = sizeof(COPYDATASTRU);
					break;
				default:
                    Irp->IoStatus.Status = STATUS_DRIVER_INTERNAL_ERROR;
                    Irp->IoStatus.Information = 0;
                }
                break;

            default:
                Irp->IoStatus.Status = STATUS_DRIVER_INTERNAL_ERROR;
                Irp->IoStatus.Information = 0;
            }

            IoCompleteRequest(
                Irp,
                (CCHAR) (NT_SUCCESS(Irp->IoStatus.Status) ?
                IO_DISK_INCREMENT : IO_NO_INCREMENT)
                );
        }
    }
}

NTSTATUS SyncWriteDevice(IN PDEVICEEXTENSION pDeviceExt,IN UCHAR *pBuf,IN PLARGE_INTEGER lnOffset,IN ULONG ulLength)
{
    IO_STATUS_BLOCK IoStatusBlock;
    NTSTATUS        status = STATUS_INSUFFICIENT_RESOURCES;
    PIRP            irp;
    KEVENT          event;
    if (irp = IoBuildSynchronousFsdRequest(IRP_MJ_WRITE,pDeviceExt->TargetDeviceObject,
											pBuf,ulLength,lnOffset,&event,&IoStatusBlock))
	{
        //
        // Set the event object to the unsignaled state.
        // It will be used to signal request completion.
        //

        KeInitializeEvent(&event,NotificationEvent,FALSE);

        status = IoCallDriver(pDeviceExt->TargetDeviceObject,irp);

        if (status == STATUS_PENDING) 
		{
            KeWaitForSingleObject(&event,Executive,KernelMode,FALSE,NULL);
		    status = IoStatusBlock.Status;
		}
	}
	return status;
}

