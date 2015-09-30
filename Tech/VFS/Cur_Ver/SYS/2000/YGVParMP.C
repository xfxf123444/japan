/*
    This is a virtual disk driver for Windows NT and Windows 2000 that uses
    one or more files to emulate physical disks, release 4.
    Copyright (C) 1999, 2000 Bo Branten
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Please send comments, corrections and contributions to bosse@acc.umu.se

    The most recent version of this program is available from:
    http://www.acc.umu.se/~bosse/

    Revision history:

    4. 2001-07-08
       Formating to FAT on Windows 2000 now works.

    3. 2001-05-14
       Corrected the error messages from the usermode control application.

    2. 2000-03-15
       Added handling of IOCTL_DISK_CHECK_VERIFY to make the driver work on
       Windows 2000 (tested on beta 3, build 2031). Formating to FAT still
       doesn't work but formating to NTFS does.

    1. 1999-06-09
       Initial release.
*/

#include <ntddk.h>
#include <WDMSec.h>
#include <ntdddisk.h>
#include <mountmgr.h>
#include <stdio.h>
#include "..\..\..\..\YGDiskRW\Cur_ver\Export\YGDiskRW.h"
#include "..\..\..\Export\2000\YGVParMP.h"
#include "..\..\..\..\lib\hashsrch\Cur_ver\Export\2000\HashSrch.h"
#include "..\..\..\..\BlkMover\Cur_ver\Export\BlkMover.h"

#define MOUNTDEVCONTROLTYPE  ((ULONG) 'M')

#define IOCTL_MOUNTDEV_QUERY_DEVICE_NAME    CTL_CODE(MOUNTDEVCONTROLTYPE, 2, METHOD_BUFFERED, FILE_ANY_ACCESS)

// {1DAE0C22-1B09-48d5-B7FB-E50E59393C52}
static const GUID GUID_DEVCLASS_YGVPARMP = 
{ 0x1dae0c22, 0x1b09, 0x48d5, { 0xb7, 0xfb, 0xe5, 0xe, 0x59, 0x39, 0x3c, 0x52 } };

#define DEFAULT_HASH_SIZE   0x2000

typedef struct _DEVICE_EXTENSION {
    BOOLEAN                 bMediaInDevice;
	PDEVICE_OBJECT          DeviceObject;
	PFILE_OBJECT			FileObject;
//    LARGE_INTEGER			ByteOffset;
	BOOLEAN					bHasYGDiskRW;
	YGVPARMP_PARAMETER      MediaInfo;
    LIST_ENTRY              pListHead;
    KSPIN_LOCK              kListLock;
    KEVENT                  keRequestEvent;
    PVOID                   pThreadPointer;
    BOOLEAN                 bTerminate_Thread;
	HANDLE					hHash;
	ULONG                   ulHashSize;
	ULONG                   ulNumber;
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

NTSTATUS
DriverEntry (
    IN PDRIVER_OBJECT   DriverObject,
    IN PUNICODE_STRING  RegistryPath
);

NTSTATUS
FlushMedia (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
);

NTSTATUS
CreateDevice (
    IN PDRIVER_OBJECT   DriverObject,
    IN ULONG            Number
);

VOID
DriverUnload (
    IN PDRIVER_OBJECT   DriverObject
);

PDEVICE_OBJECT
DeleteDevice (
    IN PDEVICE_OBJECT   DeviceObject
);

NTSTATUS
CreateClose (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
);

NTSTATUS
ReadWrite (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
);

NTSTATUS
DeviceControl (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
);

VOID
Thread (
    IN PVOID            Context
);

NTSTATUS
OpenMedia (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
);

NTSTATUS
CloseMedia (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
);

NTSTATUS
YGDiskMPSyncRead(
    IN PDEVICE_OBJECT DeviceObject,
	IN LARGE_INTEGER  Start,
	IN ULONG ulBytes,
	IN PVOID pBuffer,
	IN BOOLEAN	bHasYGDiskRW
);

BOOLEAN YGDiskMPCheckYGDiskRW(IN PDEVICE_OBJECT DeviceObject);
VOID  FillParInfoEx(IN PDEVICE_EXTENSION pDeviceExt,IN OUT PPARTITION_INFORMATION_EX pPartitionInfoEx);
NTSTATUS CreateGlobalDosDeviceName(LPSTR lpSymbolicLinkName, LPSTR lpDeviceName);
NTSTATUS DeleteGlobalDosDeviceName(LPSTR lpSymbolicLinkName);
NTSTATUS FreeFsData(IN PDEVICE_EXTENSION pDeviceExt);
BOOLEAN WriteSector(IN PDEVICE_EXTENSION pDeviceExt,IN ULONG ulLinSectorNum,IN UCHAR *pBuf,IN ULONG ulSectors);
ULONG ReadSector(IN PDEVICE_EXTENSION pDeviceExt,IN ULONG ulLinSectorNum,IN OUT UCHAR *pBuf,IN ULONG ulSecNumber);

#pragma code_seg("init")

NTSTATUS
DriverEntry (
    IN PDRIVER_OBJECT   DriverObject,
    IN PUNICODE_STRING  RegistryPath
    )
{
    NTSTATUS                    status;
    ULONG                       i;
    USHORT                      created_devices;


    DriverObject->DriverUnload = DriverUnload;
	
    DriverObject->MajorFunction[IRP_MJ_CREATE]          = CreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE]           = CreateClose;
    DriverObject->MajorFunction[IRP_MJ_READ]            = ReadWrite;
    DriverObject->MajorFunction[IRP_MJ_WRITE]           = ReadWrite;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL]  = DeviceControl;

    for (i = 0, created_devices = 0; i < MAX_VPARMP_DRIVE; i++)
    {
        status = CreateDevice(DriverObject, i);

        if (NT_SUCCESS(status))
        {
            created_devices++;
        }
    }

    if (created_devices >= 1)
    {
        status = STATUS_SUCCESS;
    }

    return status;
}

NTSTATUS
CreateDevice (
    IN PDRIVER_OBJECT   DriverObject,
    IN ULONG            Number
    )
{
    WCHAR               device_name_buffer[MAXIMUM_FILENAME_LENGTH];
    WCHAR               wcDosNameBuffer[MAXIMUM_FILENAME_LENGTH];
    UNICODE_STRING      device_name,usSymLnkName;
    NTSTATUS            status;
    PDEVICE_OBJECT      device_object;
    PDEVICE_EXTENSION   pDeviceExt;
    HANDLE              thread_handle;
    UNICODE_STRING		sddlString;

    ASSERT(DriverObject != NULL);

    swprintf(device_name_buffer,YGVPARMP_DIR_NAME L"%d", Number);
    RtlInitUnicodeString(&device_name, device_name_buffer);

    (void) RtlInitUnicodeString( &sddlString, L"D:P(A;;GA;;;SY)(A;;GA;;;BA)(A;;GA;;;WD)(A;;GA;;;RC)");

    status = IoCreateDeviceSecure(
                DriverObject,
                sizeof(DEVICE_EXTENSION),
                &device_name,
                FILE_DEVICE_DISK,
                FILE_DEVICE_SECURE_OPEN,
                (BOOLEAN) FALSE,
                &sddlString,
                (LPCGUID)&GUID_DEVCLASS_YGVPARMP,
                &device_object
                );
/*    status = IoCreateDevice(
        DriverObject,
        sizeof(DEVICE_EXTENSION),
        &device_name,
        FILE_DEVICE_DISK,
        0,
        FALSE,
        &device_object
        );*/

    if (!NT_SUCCESS(status))
    {
        return status;
    }

    device_object->Flags |= DO_DIRECT_IO;
    device_object->AlignmentRequirement = SECTOR_SIZE;
    pDeviceExt = (PDEVICE_EXTENSION) device_object->DeviceExtension;

    swprintf(wcDosNameBuffer,L"\\DosDevices\\YGVParMP%d", Number);
    RtlInitUnicodeString(&usSymLnkName,wcDosNameBuffer);
	status = IoCreateSymbolicLink(&usSymLnkName,&device_name);
    if (!NT_SUCCESS(status))
    {
        IoDeleteDevice(device_object);
        return status;
    }


    pDeviceExt->bMediaInDevice = FALSE;
	pDeviceExt->ulNumber = Number;
    InitializeListHead(&pDeviceExt->pListHead);
    KeInitializeSpinLock(&pDeviceExt->kListLock);

    KeInitializeEvent(
        &pDeviceExt->keRequestEvent,
        SynchronizationEvent,
        FALSE
        );

    pDeviceExt->bTerminate_Thread = FALSE;

    status = PsCreateSystemThread(
        &thread_handle,
        (ACCESS_MASK) 0L,
        NULL,
        NULL,
        NULL,
        Thread,
        device_object
        );

    if (!NT_SUCCESS(status))
    {
		IoDeleteSymbolicLink(&usSymLnkName);
        IoDeleteDevice(device_object);
        return status;
    }

    status = ObReferenceObjectByHandle(
        thread_handle,
        THREAD_ALL_ACCESS,
        NULL,
        KernelMode,
        &pDeviceExt->pThreadPointer,
        NULL
        );

    if (!NT_SUCCESS(status))
    {
        ZwClose(thread_handle);

        pDeviceExt->bTerminate_Thread = TRUE;

        KeSetEvent(
            &pDeviceExt->keRequestEvent,
            (KPRIORITY) 0,
            FALSE
            );

		IoDeleteSymbolicLink(&usSymLnkName);
        IoDeleteDevice(device_object);

        return status;
    }

    ZwClose(thread_handle);

    return STATUS_SUCCESS;
}

#pragma code_seg("page")

VOID
DriverUnload (
    IN PDRIVER_OBJECT DriverObject
    )
{
    PDEVICE_OBJECT device_object;

    PAGED_CODE();

    device_object = DriverObject->DeviceObject;

    while (device_object)
    {
        device_object = DeleteDevice(device_object);
    }

}

PDEVICE_OBJECT
DeleteDevice (
    IN PDEVICE_OBJECT DeviceObject
    )
{
    WCHAR               wcDosNameBuffer[MAXIMUM_FILENAME_LENGTH];
    UNICODE_STRING      usSymLnkName;
    PDEVICE_EXTENSION   pDeviceExt;
    PDEVICE_OBJECT      next_device_object;

    PAGED_CODE();
    ASSERT(DeviceObject != NULL);

    pDeviceExt = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    pDeviceExt->bTerminate_Thread = TRUE;

    KeSetEvent(
        &pDeviceExt->keRequestEvent,
        (KPRIORITY) 0,
        FALSE
        );

    KeWaitForSingleObject(
        pDeviceExt->pThreadPointer,
        Executive,
        KernelMode,
        FALSE,
        NULL
        );

    ObDereferenceObject(pDeviceExt->pThreadPointer);

    next_device_object = DeviceObject->NextDevice;

    swprintf(wcDosNameBuffer,L"\\DosDevices\\YGVParMP%d",pDeviceExt->ulNumber);
    RtlInitUnicodeString(&usSymLnkName,wcDosNameBuffer);
	IoDeleteSymbolicLink(&usSymLnkName);

    IoDeleteDevice(DeviceObject);

    return next_device_object;
}

#pragma code_seg()

NTSTATUS
CreateClose (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
    )
{
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = FILE_OPENED;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

NTSTATUS
ReadWrite (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
    )
{
    PDEVICE_EXTENSION   pDeviceExt;
    PIO_STACK_LOCATION  io_stack;
    LONGLONG            offset;
    ULONG               length;
    LONGLONG            partition_length;

    pDeviceExt = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    if (pDeviceExt->bMediaInDevice == FALSE)
    {
        KdPrint(("YGVParMP: ReadWrite: No media in device\n"));

        Irp->IoStatus.Status = STATUS_NO_MEDIA_IN_DEVICE;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        return STATUS_NO_MEDIA_IN_DEVICE;
    }

    io_stack = IoGetCurrentIrpStackLocation(Irp);
	if (io_stack->MajorFunction == IRP_MJ_WRITE)
	{
//		WriteSector(pDeviceExt,(ULONG)(io_stack->Parameters.Write.ByteOffset.QuadPart/SECTOR_SIZE),
//					MmGetSystemAddressForMdlSafe(Irp->MdlAddress,NormalPagePriority),
//					io_stack->Parameters.Write.Length/SECTOR_SIZE);
		Irp->IoStatus.Status = STATUS_SUCCESS;
		Irp->IoStatus.Information = io_stack->Parameters.Write.Length;
        IoCompleteRequest(Irp,IO_NO_INCREMENT);
		return Irp->IoStatus.Status;
	}

	if (io_stack->MajorFunction == IRP_MJ_WRITE)
	{
		offset = io_stack->Parameters.Write.ByteOffset.QuadPart;
		length = io_stack->Parameters.Write.Length;
	}
	else
	{
		offset = io_stack->Parameters.Read.ByteOffset.QuadPart;
		length = io_stack->Parameters.Read.Length;
	}

    partition_length =
        pDeviceExt->MediaInfo.PartitionInformation.PartitionLength.QuadPart;

    if ((length == 0) ||
        ((offset + length) > partition_length) ||
        (offset % SECTOR_SIZE) ||
        (length % SECTOR_SIZE))
    {
        KdPrint(("YGVParMP: ReadWrite: Invalid parameter\n"));

        Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        return STATUS_INVALID_PARAMETER;
    }

    IoMarkIrpPending(Irp);

    ExInterlockedInsertTailList(
        &pDeviceExt->pListHead,
        &Irp->Tail.Overlay.ListEntry,
        &pDeviceExt->kListLock
        );

    KeSetEvent(
        &pDeviceExt->keRequestEvent,
        (KPRIORITY) 0,
        FALSE
        );

    return STATUS_PENDING;
}

NTSTATUS
DeviceControl (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
    )
{
    PDEVICE_EXTENSION   pDeviceExt;
    PIO_STACK_LOCATION  io_stack;
    NTSTATUS            status = STATUS_INVALID_DEVICE_REQUEST;

    pDeviceExt = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    io_stack = IoGetCurrentIrpStackLocation(Irp);

    if (pDeviceExt->bMediaInDevice == FALSE &&
        io_stack->Parameters.DeviceIoControl.IoControlCode != IOCTL_YGVPARMP_OPEN_MEDIA &&
		io_stack->Parameters.DeviceIoControl.IoControlCode != IOCTL_YGVPARMP_DRIVE_ISUSEFUL)
    {
        KdPrint(("YGVParMP: DeviceControl: No media in device\n"));

        Irp->IoStatus.Status = STATUS_NO_MEDIA_IN_DEVICE;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        return STATUS_NO_MEDIA_IN_DEVICE;
    }

    switch (io_stack->Parameters.DeviceIoControl.IoControlCode)
    {
	case IOCTL_YGVPARMP_DRIVE_ISUSEFUL:
        if (io_stack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(DIOC_REGISTERS))
        {
            status = STATUS_INVALID_PARAMETER;
			Irp->IoStatus.Information = 0;
			break;
		}
		((DIOC_REGISTERS *)Irp->AssociatedIrp.SystemBuffer)->reg_EAX = pDeviceExt->bMediaInDevice?FALSE:TRUE;
        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = sizeof(DIOC_REGISTERS);
		break;

	case IOCTL_YGVPARMP_GET_VERSION:
        if (io_stack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(DIOC_REGISTERS))
        {
            status = STATUS_INVALID_PARAMETER;
			Irp->IoStatus.Information = 0;
			break;
		}
    	((DIOC_REGISTERS *)Irp->AssociatedIrp.SystemBuffer)->reg_EAX = YGVPARMP_VERSION_MAJOR << 16 | YGVPARMP_VERSION_MINOR;
        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = sizeof(DIOC_REGISTERS);
		break;
    case IOCTL_YGVPARMP_OPEN_MEDIA:

        if (pDeviceExt->bMediaInDevice == TRUE)
        {
            KdPrint((
                "YGVParMP: DeviceControl: "
                "IOCTL_YGVPARMP_OPEN_MEDIA: Media already opened\n"
                ));

            status = STATUS_INVALID_DEVICE_REQUEST;
            Irp->IoStatus.Information = 0;
            break;
        }

        if (io_stack->Parameters.DeviceIoControl.InputBufferLength <
            sizeof(YGVPARMP_PARAMETER))
        {
            status = STATUS_INVALID_PARAMETER;
            Irp->IoStatus.Information = 0;
            break;
        }
        status = STATUS_PENDING;
        break;
	case IOCTL_YGVPARMP_FLUSH_MEDIA:
        status = STATUS_PENDING;
        break;
    case IOCTL_YGVPARMP_CLOSE_MEDIA:
        status = STATUS_PENDING;
        break;

	case IOCTL_MOUNTDEV_QUERY_DEVICE_NAME:
        if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
                sizeof(MOUNTDEV_NAME)) 
		{
            status = STATUS_BUFFER_TOO_SMALL;
            Irp->IoStatus.Information = sizeof(MOUNTDEV_NAME);
            break;
        }

        ((MOUNTDEV_NAME *)Irp->AssociatedIrp.SystemBuffer)->NameLength = 30;

        if (io_stack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(USHORT) + 30) 
		{

            status = STATUS_BUFFER_OVERFLOW;
            Irp->IoStatus.Information = sizeof(MOUNTDEV_NAME);
            break;
        }
		swprintf(((MOUNTDEV_NAME *)Irp->AssociatedIrp.SystemBuffer)->Name,L"\\DosDevices\\%C:",pDeviceExt->MediaInfo.ucDriveLetter);

        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = sizeof(USHORT) + ((MOUNTDEV_NAME *)Irp->AssociatedIrp.SystemBuffer)->NameLength;
        break;
    case IOCTL_DISK_CHECK_VERIFY:
    case IOCTL_STORAGE_CHECK_VERIFY:
    case IOCTL_STORAGE_CHECK_VERIFY2:
        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = 0;
        break;

	case IOCTL_DISK_GET_DRIVE_GEOMETRY:
        if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(DISK_GEOMETRY))
        {
            status = STATUS_BUFFER_TOO_SMALL;
            Irp->IoStatus.Information = 0;
            break;
        }

		RtlCopyMemory(Irp->AssociatedIrp.SystemBuffer,&pDeviceExt->MediaInfo.DiskGeometry,sizeof(DISK_GEOMETRY));

        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = sizeof(DISK_GEOMETRY);
        break;

	case IOCTL_DISK_GET_LENGTH_INFO:
        if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(GET_LENGTH_INFORMATION))
        {
            status = STATUS_BUFFER_TOO_SMALL;
            Irp->IoStatus.Information = 0;
            break;
        }

        ((PGET_LENGTH_INFORMATION)Irp->AssociatedIrp.SystemBuffer)->Length.QuadPart = pDeviceExt->MediaInfo.PartitionInformation.PartitionLength.QuadPart;

        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = sizeof(GET_LENGTH_INFORMATION);

		break;

	case IOCTL_DISK_GET_PARTITION_INFO:
        if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(PARTITION_INFORMATION))
        {
            status = STATUS_BUFFER_TOO_SMALL;
            Irp->IoStatus.Information = 0;
            break;
        }

		RtlCopyMemory(Irp->AssociatedIrp.SystemBuffer,&pDeviceExt->MediaInfo.PartitionInformation,sizeof(PARTITION_INFORMATION));

        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = sizeof(PARTITION_INFORMATION);
        break;
	case IOCTL_DISK_GET_PARTITION_INFO_EX:
        if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(PARTITION_INFORMATION_EX))
        {
            status = STATUS_BUFFER_TOO_SMALL;
            Irp->IoStatus.Information = 0;
            break;
        }

		FillParInfoEx(pDeviceExt,(PPARTITION_INFORMATION_EX)Irp->AssociatedIrp.SystemBuffer);

        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = sizeof(PARTITION_INFORMATION_EX);
        break;

    case IOCTL_DISK_MEDIA_REMOVAL:
    case IOCTL_STORAGE_MEDIA_REMOVAL:
        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = 0;
        break;
    case IOCTL_DISK_SET_PARTITION_INFO:
        if (io_stack->Parameters.DeviceIoControl.InputBufferLength <
            sizeof(SET_PARTITION_INFORMATION))
        {
            status = STATUS_INVALID_PARAMETER;
            Irp->IoStatus.Information = 0;
            break;
        }

        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = 0;

        break;

 	case IOCTL_DISK_IS_WRITABLE:
        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = 0;
        break;

	case IOCTL_DISK_VERIFY:
        if (io_stack->Parameters.DeviceIoControl.InputBufferLength <
            sizeof(VERIFY_INFORMATION))
        {
            status = STATUS_INVALID_PARAMETER;
            Irp->IoStatus.Information = 0;
            break;
        }

        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = ((PVERIFY_INFORMATION) Irp->AssociatedIrp.SystemBuffer)->Length;

        break;
    default:
        KdPrint((
                "YGDiskMP: Unknown IoControlCode %#x\n",
                io_stack->Parameters.DeviceIoControl.IoControlCode
                ));

        status = STATUS_INVALID_DEVICE_REQUEST;
        Irp->IoStatus.Information = 0;
		break;
    }

    Irp->IoStatus.Status = status;
    if (status != STATUS_PENDING) IoCompleteRequest(Irp,IO_NO_INCREMENT);
/*            IoCompleteRequest(
                Irp,
                (CCHAR) (NT_SUCCESS(Irp->IoStatus.Status) ?
                IO_DISK_INCREMENT : IO_NO_INCREMENT)
                );*/
	else
	{
		IoMarkIrpPending(Irp);
		ExInterlockedInsertTailList(&pDeviceExt->pListHead,
									&Irp->Tail.Overlay.ListEntry,
									&pDeviceExt->kListLock);
		KeSetEvent(&pDeviceExt->keRequestEvent,(KPRIORITY) 0,FALSE	);
	}

    return status;
}

#pragma code_seg("page")

VOID
Thread (
    IN PVOID Context
    )
{
    PDEVICE_OBJECT      device_object;
    PDEVICE_EXTENSION   pDeviceExt;
    PLIST_ENTRY         request;
	UCHAR               *pbtMemData;
    PIRP                irp;
    PIO_STACK_LOCATION  io_stack;
    LARGE_INTEGER		ByteOffset;

    PAGED_CODE();
    ASSERT(Context != NULL);

    device_object = (PDEVICE_OBJECT) Context;

    pDeviceExt = (PDEVICE_EXTENSION) device_object->DeviceExtension;

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

        if (pDeviceExt->bTerminate_Thread)
        {
            PsTerminateSystemThread(STATUS_SUCCESS);
        }

        while (request = ExInterlockedRemoveHeadList(
            &pDeviceExt->pListHead,
            &pDeviceExt->kListLock
            ))
        {
            irp = CONTAINING_RECORD(request, IRP, Tail.Overlay.ListEntry);

            io_stack = IoGetCurrentIrpStackLocation(irp);

            switch (io_stack->MajorFunction)
            {
            case IRP_MJ_READ:
				ByteOffset.QuadPart = pDeviceExt->MediaInfo.PartitionInformation.StartingOffset.QuadPart + io_stack->Parameters.Read.ByteOffset.QuadPart;
			    pbtMemData = (UCHAR*)MmGetSystemAddressForMdlSafe(irp->MdlAddress,NormalPagePriority);
				YGDiskMPSyncRead(pDeviceExt->DeviceObject,ByteOffset,io_stack->Parameters.Read.Length,pbtMemData,pDeviceExt->bHasYGDiskRW);
				ReadSector(pDeviceExt,(ULONG)(io_stack->Parameters.Read.ByteOffset.QuadPart/SECTOR_SIZE),pbtMemData,io_stack->Parameters.Read.Length/SECTOR_SIZE);
				irp->IoStatus.Status = STATUS_SUCCESS;
                irp->IoStatus.Information = io_stack->Parameters.Read.Length;
				break;

            case IRP_MJ_WRITE:
			    pbtMemData = (UCHAR*)MmGetSystemAddressForMdlSafe(irp->MdlAddress,NormalPagePriority);
				WriteSector(pDeviceExt,(ULONG)(io_stack->Parameters.Write.ByteOffset.QuadPart/SECTOR_SIZE),pbtMemData,io_stack->Parameters.Write.Length/SECTOR_SIZE);
				irp->IoStatus.Status = STATUS_SUCCESS;
                irp->IoStatus.Information = io_stack->Parameters.Write.Length;
                break;

            case IRP_MJ_DEVICE_CONTROL:
                switch (io_stack->Parameters.DeviceIoControl.IoControlCode)
                {
                case IOCTL_YGVPARMP_OPEN_MEDIA:
                    OpenMedia(device_object, irp);
                    break;

                case IOCTL_YGVPARMP_CLOSE_MEDIA:
                    CloseMedia(device_object, irp);
                    break;
				case IOCTL_YGVPARMP_FLUSH_MEDIA:
                    FlushMedia(device_object, irp);
                    break;
                default:
                    KdPrint((
                        "YGVParMP: Thread: "
                        "Unknown IoControlCode 0x%x\n",
                        io_stack->Parameters.DeviceIoControl.IoControlCode
                        ));

                    irp->IoStatus.Status = STATUS_DRIVER_INTERNAL_ERROR;
                    irp->IoStatus.Information = 0;
                }
                break;

            default:
                KdPrint((
                    "YGVParMP: Thread: "
                    "Unknown MajorFunction 0x%x\n",
                    io_stack->MajorFunction
                    ));

                irp->IoStatus.Status = STATUS_DRIVER_INTERNAL_ERROR;
                irp->IoStatus.Information = 0;
            }

            IoCompleteRequest(irp,IO_NO_INCREMENT);
/*            IoCompleteRequest(
                irp,
                (CCHAR) (NT_SUCCESS(irp->IoStatus.Status) ?
                IO_DISK_INCREMENT : IO_NO_INCREMENT)
                );*/
        }
    }
}

NTSTATUS
OpenMedia (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
    )
{
	CHAR							szDeviceName[32],szSymLnk[32];
    PDEVICE_EXTENSION               pDeviceExt;
    NTSTATUS                        status;
    OBJECT_ATTRIBUTES               ObjectAttributes;
    WCHAR							wFileName[MAXIMUM_FILENAME_LENGTH];
	UNICODE_STRING			        usDataFile;              

    PAGED_CODE();
    ASSERT(DeviceObject != NULL);
    ASSERT(Irp != NULL);

    pDeviceExt = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    if ((pDeviceExt->hHash = CreateHashList(DEFAULT_HASH_SIZE,SECTOR_SIZE,PagedPool)))
	{
		RtlCopyMemory(	&pDeviceExt->MediaInfo,
						Irp->AssociatedIrp.SystemBuffer,
						sizeof(YGVPARMP_PARAMETER));

		pDeviceExt->ulHashSize = DEFAULT_HASH_SIZE;

//		pDeviceExt->ByteOffset.QuadPart = pDeviceExt->MediaInfo.PartitionInformation.StartingOffset.QuadPart;
//		pDeviceExt->MediaInfo.PartitionInformation.StartingOffset.QuadPart = 0;
//		pDeviceExt->MediaInfo.PartitionInformation.HiddenSectors = 1;

//		if (!pDeviceExt->MediaInfo.DiskGeometry.TracksPerCylinder)
//			pDeviceExt->MediaInfo.DiskGeometry.TracksPerCylinder = 0xff;

//		if (!pDeviceExt->MediaInfo.DiskGeometry.SectorsPerTrack)
//			pDeviceExt->MediaInfo.DiskGeometry.SectorsPerTrack = 0x3f;

//		if (!pDeviceExt->MediaInfo.DiskGeometry.BytesPerSector)
//			pDeviceExt->MediaInfo.DiskGeometry.BytesPerSector = SECTOR_SIZE;

//		pDeviceExt->MediaInfo.DiskGeometry.Cylinders.QuadPart =  
//				pDeviceExt->MediaInfo.PartitionInformation.PartitionLength.QuadPart / pDeviceExt->MediaInfo.DiskGeometry.TracksPerCylinder / pDeviceExt->MediaInfo.DiskGeometry.SectorsPerTrack / pDeviceExt->MediaInfo.DiskGeometry.BytesPerSector;
		pDeviceExt->MediaInfo.DiskGeometry.MediaType = FixedMedia;
//		pDeviceExt->MediaInfo.PartitionInformation.PartitionNumber = 0;

		swprintf(wFileName,
				L"\\Device\\Harddisk%d\\Partition0",
				pDeviceExt->MediaInfo.DiskNumber);

		RtlInitUnicodeString(&usDataFile, wFileName);
		status = IoGetDeviceObjectPointer(&usDataFile,FILE_READ_ATTRIBUTES,
											  &pDeviceExt->FileObject,&pDeviceExt->DeviceObject);

		if (NT_SUCCESS(status))      
		{
			DeviceObject->AlignmentRequirement = pDeviceExt->DeviceObject->AlignmentRequirement;
			pDeviceExt->bHasYGDiskRW = YGDiskMPCheckYGDiskRW(pDeviceExt->DeviceObject);

			sprintf(szDeviceName,"\\Device\\YGVParMP\\YGVParMP%d",pDeviceExt->ulNumber);
			sprintf(szSymLnk,"\\DosDevices\\Global\\%C:",pDeviceExt->MediaInfo.ucDriveLetter);
			CreateGlobalDosDeviceName(szSymLnk, szDeviceName);

			pDeviceExt->bMediaInDevice = TRUE;
			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = 0;
		    return Irp->IoStatus.Status;
		}
		DestroyHashList(pDeviceExt->hHash);
		pDeviceExt->hHash = NULL;
	}

	Irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
	Irp->IoStatus.Information = 0;

    return Irp->IoStatus.Status;
}

NTSTATUS
CloseMedia (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
    )
{
    PDEVICE_EXTENSION pDeviceExt;
	CHAR			szSymLnk[32];

    PAGED_CODE();
    ASSERT(DeviceObject != NULL);
    ASSERT(Irp != NULL);

    pDeviceExt = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    pDeviceExt->bMediaInDevice = FALSE;

	sprintf(szSymLnk,"\\DosDevices\\Global\\%C:",pDeviceExt->MediaInfo.ucDriveLetter);
	DeleteGlobalDosDeviceName(szSymLnk);

	ObDereferenceObject(pDeviceExt->FileObject);
	pDeviceExt->DeviceObject = NULL;

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    return STATUS_SUCCESS;
}

NTSTATUS FreeFsData(IN PDEVICE_EXTENSION pDeviceExt)
{
    ULONG		    ulSecAddr,*pSector;
    ULONG		    ulIndex;

    if (pDeviceExt->hHash)
    {
		pSector = (ULONG *)GetNextObj(pDeviceExt->hHash,NULL,&ulIndex);
		while (pSector)
		{
			RemoveObj(pDeviceExt->hHash,(PVOID)pSector,ulIndex);
			DeallocateObj(pDeviceExt->hHash,(PVOID)pSector,ulIndex);
			pSector = (ULONG *)GetNextObj(pDeviceExt->hHash,NULL,&ulIndex);
		}
		DestroyHashList(pDeviceExt->hHash);
		pDeviceExt->hHash = NULL;
    }
    return  STATUS_SUCCESS;
}   //FreeFsData

NTSTATUS
FlushMedia (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
    )
{
    PDEVICE_EXTENSION               pDeviceExt;

    PAGED_CODE();
    ASSERT(DeviceObject != NULL);
    ASSERT(Irp != NULL);

    pDeviceExt = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
	if (pDeviceExt->bMediaInDevice)
	{
		Irp->IoStatus.Status = STATUS_SUCCESS;
		Irp->IoStatus.Information = 0;
	}
	else
	{
		Irp->IoStatus.Status = STATUS_DEVICE_NOT_READY;
		Irp->IoStatus.Information = 0;
	}
	return Irp->IoStatus.Status;
}

VOID  FillParInfoEx(IN PDEVICE_EXTENSION pDeviceExt,IN OUT PPARTITION_INFORMATION_EX pPartitionInfoEx)
{
	pPartitionInfoEx->PartitionStyle = PARTITION_STYLE_MBR;
	pPartitionInfoEx->StartingOffset = pDeviceExt->MediaInfo.PartitionInformation.StartingOffset;
	pPartitionInfoEx->PartitionLength = pDeviceExt->MediaInfo.PartitionInformation.PartitionLength;
	pPartitionInfoEx->PartitionNumber = pDeviceExt->MediaInfo.PartitionInformation.PartitionNumber;
	pPartitionInfoEx->RewritePartition = pDeviceExt->MediaInfo.PartitionInformation.RewritePartition;
	pPartitionInfoEx->Mbr.PartitionType = pDeviceExt->MediaInfo.PartitionInformation.PartitionType;
	pPartitionInfoEx->Mbr.BootIndicator = pDeviceExt->MediaInfo.PartitionInformation.BootIndicator;
	pPartitionInfoEx->Mbr.RecognizedPartition = pDeviceExt->MediaInfo.PartitionInformation.RecognizedPartition;
	pPartitionInfoEx->Mbr.HiddenSectors = pDeviceExt->MediaInfo.PartitionInformation.HiddenSectors;
	return;
}

NTSTATUS CreateGlobalDosDeviceName(LPSTR lpSymbolicLinkName, LPSTR lpDeviceName)
{
	UNICODE_STRING		DeviceName; 
	UNICODE_STRING		SymbolicLinkName; // Initializing below.
	ANSI_STRING			AnsiString;
	NTSTATUS			Status = STATUS_DRIVER_INTERNAL_ERROR;

	if (IoIsWdmVersionAvailable(1,0x10)) 
	{
		// We're on Windows 2000 or later, so we use \DosDevices\Global.
		RtlInitAnsiString(&AnsiString,lpDeviceName);
		Status = RtlAnsiStringToUnicodeString(&DeviceName,&AnsiString,TRUE);
		if (NT_SUCCESS(Status)) 
		{
			RtlInitAnsiString(&AnsiString,lpSymbolicLinkName);
			Status = RtlAnsiStringToUnicodeString(&SymbolicLinkName,&AnsiString,TRUE);
			if (NT_SUCCESS(Status)) 
			{
				Status = IoCreateSymbolicLink(&SymbolicLinkName,&DeviceName);
				RtlFreeUnicodeString(&SymbolicLinkName);
			}
			RtlFreeUnicodeString(&DeviceName);
		}
	}
	return Status;
}

NTSTATUS DeleteGlobalDosDeviceName(LPSTR lpSymbolicLinkName)
{
	UNICODE_STRING		SymbolicLinkName;
	ANSI_STRING			AnsiString;
	NTSTATUS			Status = STATUS_DRIVER_INTERNAL_ERROR;

	if (IoIsWdmVersionAvailable(1,0x10)) 
	{
		// We're on Windows 2000 or later, so we use \DosDevices\Global.
		RtlInitAnsiString(&AnsiString,lpSymbolicLinkName);
		Status = RtlAnsiStringToUnicodeString(&SymbolicLinkName,&AnsiString,TRUE);
		if (NT_SUCCESS(Status)) 
		{
			Status = IoDeleteSymbolicLink(&SymbolicLinkName);
			RtlFreeUnicodeString(&SymbolicLinkName);
		}
	}
	return Status;
}

BOOLEAN YGDiskMPCheckYGDiskRW(IN PDEVICE_OBJECT DeviceObject)
{
	NTSTATUS                    status;
    PIRP						irp;
	ULONG						ulVersion;
    IO_STATUS_BLOCK				ioStatus;
    KEVENT						event;

    KeInitializeEvent(&event, NotificationEvent, FALSE);
    irp = IoBuildDeviceIoControlRequest(
				    IOCTL_YGDISKRW_GET_VERSION,
		            DeviceObject,
				    &ulVersion,
		            sizeof(ULONG),
				    &ulVersion,
		            sizeof(ULONG),
		            FALSE,
		            &event,
		            &ioStatus
				    );

	if (irp)
	{
		status = IoCallDriver(DeviceObject,irp);
	    if (status == STATUS_PENDING)
		{
		    KeWaitForSingleObject( &event, Executive, KernelMode, FALSE, NULL);
			status = ioStatus.Status;
		}
	}
	else status = ioStatus.Status;
	if (NT_SUCCESS(status)) 
	{
		if (ulVersion == (YGDISKRW_VERSION_MAJOR << 8 | YGDISKRW_VERSION_MINOR))
			return TRUE;
	}
	return FALSE;
} // end YGCryDiskCheckYGDiskRW()

NTSTATUS
YGDiskMPSyncRead(
    IN PDEVICE_OBJECT DeviceObject,
	IN LARGE_INTEGER  Start,
	IN ULONG ulBytes,
	IN PVOID pBuffer,
	IN BOOLEAN bHasYGDiskRW
)
{
	NTSTATUS                    status;
    PIRP						irp;
    IO_STATUS_BLOCK				ioStatus;
    KEVENT						event;
	YGDISKRWPARAM				ReadWriteBuffer;
	READ_WRITE_BUFFER			BlkMoverRead;

    KeInitializeEvent(&event, NotificationEvent, FALSE);
	if (bHasYGDiskRW)
	{
		ReadWriteBuffer.dwStartSec = (ULONG)(Start.QuadPart/SECTOR_SIZE);
		ReadWriteBuffer.dwLength = ulBytes/SECTOR_SIZE;
		ReadWriteBuffer.pBuffer = pBuffer;

		irp = IoBuildDeviceIoControlRequest(
						IOCTL_YGDISKRW_READ_DISK_KERNEL,
						DeviceObject,
						&ReadWriteBuffer,
						sizeof(YGDISKRWPARAM),
						&ReadWriteBuffer,
						sizeof(YGDISKRWPARAM),
						FALSE,
						&event,
						&ioStatus
						);
	}
	else
	{
		BlkMoverRead.dwStartSec = (ULONG)(Start.QuadPart/SECTOR_SIZE);
		BlkMoverRead.dwLength = ulBytes/SECTOR_SIZE;
		BlkMoverRead.bSys = FALSE;
		BlkMoverRead.dwMovedRecNum = 0;
		BlkMoverRead.pBuffer = pBuffer;

		irp = IoBuildDeviceIoControlRequest(
						IOCTL_YG_READ,
						DeviceObject,
						&BlkMoverRead,
						sizeof(READ_WRITE_BUFFER),
						&BlkMoverRead,
						sizeof(READ_WRITE_BUFFER),
						FALSE,
						&event,
						&ioStatus
						);
	}
	if (irp)
	{
		status = IoCallDriver(DeviceObject,irp);
		if (status == STATUS_PENDING)
		{
			KeWaitForSingleObject( &event, Executive, KernelMode, FALSE, NULL);
			status = ioStatus.Status;
		}
	}
	else status = ioStatus.Status;
	return status;
} // end YGDiskMPSyncRead()

ULONG ReadSector(IN PDEVICE_EXTENSION pDeviceExt,IN ULONG ulLinSectorNum,IN OUT UCHAR *pBuf,IN ULONG ulSecNumber)
{
	UCHAR			*pSector;
	ULONG    	    ulIndex,i;

	for (i = 0;i < ulSecNumber;i++)
	{
		ulIndex = ulLinSectorNum % pDeviceExt->ulHashSize;
		pSector = (UCHAR *)GetObj(pDeviceExt->hHash,ulLinSectorNum,ulIndex);
		if (pSector)
		{
			RtlCopyMemory(pBuf,pSector,SECTOR_SIZE);
		}
		ulLinSectorNum ++;
		pBuf += SECTOR_SIZE;
	}
    return i;
}   //	ReadSector

BOOLEAN WriteSector(IN PDEVICE_EXTENSION pDeviceExt,IN ULONG ulLinSectorNum,IN UCHAR *pBuf,IN ULONG ulSectors)
{
    ULONG	    ulIndex,i;
    BOOLEAN	    bCreated;

	for (i = 0; i < ulSectors;i ++)
	{
		ulIndex = ulLinSectorNum % pDeviceExt->ulHashSize;
		SetObj(pDeviceExt->hHash,ulLinSectorNum,ulIndex,pBuf,&bCreated);
		ulLinSectorNum ++;
		pBuf += SECTOR_SIZE;
	}
    return TRUE;
}   //	WriteSector

