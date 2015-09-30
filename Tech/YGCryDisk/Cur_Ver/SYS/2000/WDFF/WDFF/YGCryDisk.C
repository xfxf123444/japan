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
#include <ntdddisk.h>
#include <WDMSec.h>
#include <stdio.h>
#include <mountmgr.h>
#include "..\..\..\..\Export\2000\YGCryDisk.h"
#include "..\..\..\..\..\..\YGDiskRW\Cur_ver\Export\YGDiskRW.h"
#include "..\..\..\..\..\..\Lib\crypto\Cur_ver\Export\Crypto.h"

USHORT	g_usCount = 0;
KSPIN_LOCK			g_IrpSpinLock;

#define MOUNTDEVCONTROLTYPE  ((ULONG) 'M')

#define IOCTL_MOUNTDEV_QUERY_DEVICE_NAME    CTL_CODE(MOUNTDEVCONTROLTYPE, 2, METHOD_BUFFERED, FILE_ANY_ACCESS)


// {6FDBC0BC-5A30-4a4b-BEA7-DE9AC3DE793C}
static const GUID GUID_INTERFACECLASS_YGCRYDISK = 
{ 0x6fdbc0bc, 0x5a30, 0x4a4b, { 0xbe, 0xa7, 0xde, 0x9a, 0xc3, 0xde, 0x79, 0x3c } };

// {AC75EE44-C6C4-4dee-8C4E-B7F6CBE231D3}
static const GUID GUID_DEVCLASS_YGCRYDISK = 
{ 0xac75ee44, 0xc6c4, 0x4dee, { 0x8c, 0x4e, 0xb7, 0xf6, 0xcb, 0xe2, 0x31, 0xd3 } };

typedef struct _DEVICE_EXTENSION {
    BOOLEAN                 bMediaInDevice;
	PDEVICE_OBJECT          DeviceObject;
	PFILE_OBJECT			FileObject;
//    LARGE_INTEGER			ByteOffset;
	BOOLEAN					bHasYGDiskRW;
	YGCRYDISK_PARAMETER     MediaInfo;
    LIST_ENTRY              pListHead;
    KSPIN_LOCK              kListLock;
    KEVENT                  keRequestEvent;
    PVOID                   pThreadPointer;
    BOOLEAN                 bTerminate_Thread;
	ULONG                   ulNumber;
	UCHAR					uCheckSumAndEncryptMark;
	UCHAR					uEncrytTypeAndKeyLen;
	BOOLEAN					bLastIsBootSec;
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
YGCryDiskSyncRead(
    IN PDEVICE_OBJECT DeviceObject,
	IN LARGE_INTEGER  Start,
	IN ULONG ulBytes,
	IN PVOID pBuffer,
	IN BOOLEAN	bHasYGDiskRW
);

NTSTATUS
YGCryDiskSyncWrite(
    IN PDEVICE_OBJECT DeviceObject,
	IN LARGE_INTEGER  Start,
	IN ULONG ulBytes,
	IN PVOID pBuffer,
	IN BOOLEAN	bHasYGDiskRW
);

BOOLEAN YGCryDiskCheckYGDiskRW(IN PDEVICE_OBJECT DeviceObject);
NTSTATUS CreateGlobalDosDeviceName(LPSTR lpSymbolicLinkName, LPSTR lpDeviceName);
NTSTATUS DeleteGlobalDosDeviceName(LPSTR lpSymbolicLinkName);

VOID  FillParInfoEx(IN PDEVICE_EXTENSION pDeviceExt,IN OUT PPARTITION_INFORMATION_EX pPartitionInfoEx);
BOOLEAN	CheckAndInitCryDisk(IN OUT PDEVICE_EXTENSION pDeviceExt);
BOOLEAN YGCryDiskEncryptData(IN PYGCRYDISK_PARAMETER pDiskParam,IN UCHAR *pInBuf,OUT UCHAR *pOutBuf,ULONG ulLength);
BOOLEAN YGCryDiskDecryptData(IN PYGCRYDISK_PARAMETER pDiskParam,IN UCHAR *pInBuf,OUT UCHAR *pOutBuf,ULONG ulLength);
BOOLEAN YGDecryptBootSector(UCHAR *pBuf,UCHAR *pKey,int nKeyLen);
BOOLEAN YGEncryptBootSector(UCHAR *pBuf,UCHAR *pKey,int nKeyLen);

#pragma code_seg("init")

NTSTATUS
DriverEntry (
    IN PDRIVER_OBJECT   DriverObject,
    IN PUNICODE_STRING  RegistryPath
    )
{
    NTSTATUS                    status;
    UNICODE_STRING              device_dir_name;
    ULONG                       i;
    USHORT                      created_devices = 0;


    DriverObject->DriverUnload = DriverUnload;
	
    DriverObject->MajorFunction[IRP_MJ_CREATE]          = CreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE]           = CreateClose;
    DriverObject->MajorFunction[IRP_MJ_READ]            = ReadWrite;
    DriverObject->MajorFunction[IRP_MJ_WRITE]           = ReadWrite;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL]  = DeviceControl;

    for (i = 0, created_devices = 0; i < MAX_CRYDISK_DRIVE; i++)
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

   KeInitializeSpinLock(&g_IrpSpinLock);
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

	swprintf(device_name_buffer,L"\\Device\\YGCryDisk%d", Number);
    swprintf(wcDosNameBuffer,L"\\DosDevices\\YGCryDisk%d", Number);

	RtlInitUnicodeString(&device_name, device_name_buffer);
    RtlInitUnicodeString(&usSymLnkName,wcDosNameBuffer);

    (void) RtlInitUnicodeString( &sddlString, L"D:P(A;;GA;;;SY)(A;;GA;;;BA)(A;;GA;;;WD)(A;;GA;;;RC)");

    status = IoCreateDeviceSecure(
                DriverObject,
                sizeof(DEVICE_EXTENSION),
                &device_name,
                FILE_DEVICE_DISK,
                FILE_DEVICE_SECURE_OPEN,
                (BOOLEAN) FALSE,
                &sddlString,
                (LPCGUID)&GUID_DEVCLASS_YGCRYDISK,
                &device_object
                );

/*    status = IoCreateDevice(
        DriverObject,
        sizeof(DEVICE_EXTENSION),
        &device_name,
        FILE_DEVICE_DISK,
        FILE_DEVICE_SECURE_OPEN,
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

	if (pDeviceExt->ulNumber == -1)
		swprintf(wcDosNameBuffer,L"\\DosDevices\\YGCryDisk");
	else
		swprintf(wcDosNameBuffer,L"\\DosDevices\\YGCryDisk%d",pDeviceExt->ulNumber);
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
        KdPrint(("YGCryDisk: ReadWrite: No media in device\n"));

        Irp->IoStatus.Status = STATUS_NO_MEDIA_IN_DEVICE;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        return STATUS_NO_MEDIA_IN_DEVICE;
    }

    io_stack = IoGetCurrentIrpStackLocation(Irp);
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
        KdPrint(("YGCryDisk: ReadWrite: Invalid parameter\n"));

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

BOOLEAN FillPropertyQuery(PSTORAGE_PROPERTY_QUERY pQuery,PDEVICE_EXTENSION   pDeviceExt)
{
	PSTORAGE_DEVICE_DESCRIPTOR pDescriptor = (PSTORAGE_DEVICE_DESCRIPTOR)pQuery;

	switch (pQuery->PropertyId)
	{
	case StorageDeviceProperty:
		switch (pQuery->QueryType)
		{
		case PropertyStandardQuery:
			pDescriptor->Version = sizeof(STORAGE_DEVICE_DESCRIPTOR);
			pDescriptor->Size = sizeof(STORAGE_DEVICE_DESCRIPTOR);
			pDescriptor->DeviceType = FILE_DEVICE_DISK;
			pDescriptor->DeviceTypeModifier = 0;
			if (pDeviceExt->MediaInfo.DiskGeometry.MediaType == RemovableMedia)
				pDescriptor->RemovableMedia = TRUE;
			else
				pDescriptor->RemovableMedia = TRUE;
			pDescriptor->CommandQueueing = FALSE;
			pDescriptor->VendorIdOffset = 0;
			pDescriptor->ProductIdOffset = 0;
			pDescriptor->ProductRevisionOffset = 0;
			pDescriptor->SerialNumberOffset = 0;
			pDescriptor->BusType = BusTypeUsb;
			pDescriptor->RawPropertiesLength = 1;
			pDescriptor->RawDeviceProperties[0] = 0;
			return TRUE;
			break;
		}
		break;
	}

	return FALSE;
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
		io_stack->Parameters.DeviceIoControl.IoControlCode != IOCTL_YGCRYDISK_OPEN_MEDIA &&
		io_stack->Parameters.DeviceIoControl.IoControlCode != IOCTL_YGCRYDISK_DRIVE_ISUSEFUL &&
		io_stack->Parameters.DeviceIoControl.IoControlCode != IOCTL_YGCRYDISK_ADD_COUNT &&
		io_stack->Parameters.DeviceIoControl.IoControlCode != IOCTL_YGCRYDISK_GET_COUNT)
	{
		KdPrint(("YGCryDisk: DeviceControl: No media in device\n"));

		Irp->IoStatus.Status = STATUS_NO_MEDIA_IN_DEVICE;
		Irp->IoStatus.Information = 0;
		IoCompleteRequest(Irp, IO_NO_INCREMENT);

		return STATUS_NO_MEDIA_IN_DEVICE;
	}

	switch (io_stack->Parameters.DeviceIoControl.IoControlCode)
	{
	case IOCTL_YGCRYDISK_DRIVE_ISUSEFUL:
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

	case IOCTL_YGCRYDISK_GET_VERSION:
		if (io_stack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(DIOC_REGISTERS))
		{
			status = STATUS_INVALID_PARAMETER;
			Irp->IoStatus.Information = 0;
			break;
		}
   		((DIOC_REGISTERS *)Irp->AssociatedIrp.SystemBuffer)->reg_EAX = YGCRYDISK_VERSION_MAJOR << 16 | YGCRYDISK_VERSION_MINOR;
		status = STATUS_SUCCESS;
		Irp->IoStatus.Information = sizeof(DIOC_REGISTERS);
		break;
	case IOCTL_YGCRYDISK_OPEN_MEDIA:

		if (pDeviceExt->bMediaInDevice == TRUE)
		{
			KdPrint((
				"YGCryDisk: DeviceControl: "
				"IOCTL_YGCRYDISK_OPEN_MEDIA: Media already opened\n"
				));

			status = STATUS_INVALID_DEVICE_REQUEST;
			Irp->IoStatus.Information = 0;
			break;
		}

		if (io_stack->Parameters.DeviceIoControl.InputBufferLength <
			sizeof(YGCRYDISK_PARAMETER))
		{
			status = STATUS_INVALID_PARAMETER;
			Irp->IoStatus.Information = 0;
			break;
		}
		status = STATUS_PENDING;
		break;
	case IOCTL_YGCRYDISK_FLUSH_MEDIA:
		status = STATUS_PENDING;
		break;
	case IOCTL_YGCRYDISK_CLOSE_MEDIA:
		status = STATUS_PENDING;
		break;

	case IOCTL_YGCRYDISK_GET_MPDRIVE_INFO:

		if (io_stack->Parameters.DeviceIoControl.OutputBufferLength !=
			sizeof(YGCRYDISK_PARAMETER))
		{
			status = STATUS_INVALID_PARAMETER;
			Irp->IoStatus.Information = 0;
			break;
		}

		RtlCopyMemory(Irp->AssociatedIrp.SystemBuffer,
						&pDeviceExt->MediaInfo,
						sizeof(YGCRYDISK_PARAMETER)
						);

        status = STATUS_SUCCESS;
		Irp->IoStatus.Information = sizeof(YGCRYDISK_PARAMETER);

		break;

	case IOCTL_YGCRYDISK_GET_COUNT:
		if (io_stack->Parameters.DeviceIoControl.OutputBufferLength !=
			sizeof(DIOC_REGISTERS))
		{
			status = STATUS_INVALID_PARAMETER;
			Irp->IoStatus.Information = 0;
			break;
		}

		((DIOC_REGISTERS *)Irp->AssociatedIrp.SystemBuffer)->reg_EAX = g_usCount;
        status = STATUS_SUCCESS;
		Irp->IoStatus.Information = sizeof(DIOC_REGISTERS);

		break;

	case IOCTL_YGCRYDISK_ADD_COUNT:
		if (io_stack->Parameters.DeviceIoControl.InputBufferLength != 0 ||
			io_stack->Parameters.DeviceIoControl.OutputBufferLength != 0)
		{
			status = STATUS_INVALID_PARAMETER;
			Irp->IoStatus.Information = 0;
			break;
		}
		g_usCount ++;
        status = STATUS_SUCCESS;
		Irp->IoStatus.Information = 0;
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

	case IOCTL_STORAGE_GET_MEDIA_TYPES:
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

	case IOCTL_STORAGE_GET_MEDIA_TYPES_EX:
        if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(GET_MEDIA_TYPES))
        {
            status = STATUS_BUFFER_TOO_SMALL;
            Irp->IoStatus.Information = 0;
            break;
        }

		((PGET_MEDIA_TYPES)Irp->AssociatedIrp.SystemBuffer)->DeviceType = FILE_DEVICE_DISK;
		((PGET_MEDIA_TYPES)Irp->AssociatedIrp.SystemBuffer)->MediaInfoCount = 1;
		((PGET_MEDIA_TYPES)Irp->AssociatedIrp.SystemBuffer)->MediaInfo[0].DeviceSpecific.RemovableDiskInfo.Cylinders.QuadPart = pDeviceExt->MediaInfo.DiskGeometry.Cylinders.QuadPart;
		((PGET_MEDIA_TYPES)Irp->AssociatedIrp.SystemBuffer)->MediaInfo[0].DeviceSpecific.RemovableDiskInfo.MediaType = pDeviceExt->MediaInfo.DiskGeometry.MediaType;
		((PGET_MEDIA_TYPES)Irp->AssociatedIrp.SystemBuffer)->MediaInfo[0].DeviceSpecific.RemovableDiskInfo.TracksPerCylinder = pDeviceExt->MediaInfo.DiskGeometry.TracksPerCylinder;
		((PGET_MEDIA_TYPES)Irp->AssociatedIrp.SystemBuffer)->MediaInfo[0].DeviceSpecific.RemovableDiskInfo.SectorsPerTrack = pDeviceExt->MediaInfo.DiskGeometry.SectorsPerTrack;
		((PGET_MEDIA_TYPES)Irp->AssociatedIrp.SystemBuffer)->MediaInfo[0].DeviceSpecific.RemovableDiskInfo.BytesPerSector = pDeviceExt->MediaInfo.DiskGeometry.BytesPerSector;
		((PGET_MEDIA_TYPES)Irp->AssociatedIrp.SystemBuffer)->MediaInfo[0].DeviceSpecific.RemovableDiskInfo.NumberMediaSides = 1;
		((PGET_MEDIA_TYPES)Irp->AssociatedIrp.SystemBuffer)->MediaInfo[0].DeviceSpecific.RemovableDiskInfo.MediaCharacteristics = MEDIA_READ_WRITE;

        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = sizeof(DEVICE_MEDIA_INFO);
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
/*	case IOCTL_STORAGE_QUERY_PROPERTY:
        if (io_stack->Parameters.DeviceIoControl.InputBufferLength <
            sizeof(STORAGE_PROPERTY_QUERY))
        {
            status = STATUS_INVALID_PARAMETER;
            Irp->IoStatus.Information = 0;
            break;
        }

		if (FillPropertyQuery((PSTORAGE_PROPERTY_QUERY)Irp->AssociatedIrp.SystemBuffer,pDeviceExt))
		{
			status = STATUS_SUCCESS;
			Irp->IoStatus.Information = sizeof(STORAGE_DEVICE_DESCRIPTOR);
		}
		else
		{
			status = STATUS_INVALID_DEVICE_REQUEST;
			Irp->IoStatus.Information = 0;
		}

		break;*/
    default:
        KdPrint((
                "YGCryDisk: Unknown IoControlCode %#x\n",
                io_stack->Parameters.DeviceIoControl.IoControlCode
                ));

        status = STATUS_INVALID_DEVICE_REQUEST;
        Irp->IoStatus.Information = 0;
		break;
    }

    Irp->IoStatus.Status = status;
    if (status != STATUS_PENDING)
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
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
    PIRP                irp;
    PIO_STACK_LOCATION  io_stack;
	UCHAR				*pBuf;
	PYGCRYDISKREADWRITE	pReadWrite;
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
				pBuf = MmGetSystemAddressForMdlSafe(irp->MdlAddress,NormalPagePriority);
				YGCryDiskSyncRead(pDeviceExt->DeviceObject,ByteOffset,io_stack->Parameters.Read.Length,pBuf,pDeviceExt->bHasYGDiskRW);
				if (io_stack->Parameters.Read.ByteOffset.QuadPart == 0)
				{
					YGDecryptBootSector(pBuf,pDeviceExt->MediaInfo.ucKey,pDeviceExt->MediaInfo.usKeyLen);
					pDeviceExt->bLastIsBootSec = (((PYGCRYDISK_BOOTSECTOR)pBuf)->uEncrytTypeAndKeyLen == pDeviceExt->uEncrytTypeAndKeyLen &&
													((PYGCRYDISK_BOOTSECTOR)pBuf)->uCheckSumAndEncryptMark == pDeviceExt->uCheckSumAndEncryptMark);
					if (pDeviceExt->bLastIsBootSec)
					{
						((PYGCRYDISK_BOOTSECTOR)pBuf)->uMark = 0;
						((PYGCRYDISK_BOOTSECTOR)pBuf)->uEncrytTypeAndKeyLen = 0x55;
						((PYGCRYDISK_BOOTSECTOR)pBuf)->uCheckSumAndEncryptMark = 0xAA;
					}
					YGCryDiskDecryptData(&pDeviceExt->MediaInfo,&pBuf[SECTOR_SIZE],&pBuf[SECTOR_SIZE],io_stack->Parameters.Read.Length-SECTOR_SIZE);
				}
				else YGCryDiskDecryptData(&pDeviceExt->MediaInfo,pBuf,pBuf,io_stack->Parameters.Read.Length);
				irp->IoStatus.Status = 0;
                irp->IoStatus.Information = io_stack->Parameters.Read.Length;
				break;
            case IRP_MJ_WRITE:
				ByteOffset.QuadPart = pDeviceExt->MediaInfo.PartitionInformation.StartingOffset.QuadPart + io_stack->Parameters.Write.ByteOffset.QuadPart;
			    pBuf = (UCHAR *)ExAllocatePoolWithTag(PagedPool ,io_stack->Parameters.Write.Length,'YGCD');
			    if (pBuf)
				{
					RtlCopyMemory(pBuf,MmGetSystemAddressForMdlSafe(irp->MdlAddress,NormalPagePriority),io_stack->Parameters.Write.Length);
					if (io_stack->Parameters.Write.ByteOffset.QuadPart == 0)
					{
						pDeviceExt->bLastIsBootSec = (((PYGCRYDISK_BOOTSECTOR)pBuf)->uEncrytTypeAndKeyLen == 0x55 &&
														((PYGCRYDISK_BOOTSECTOR)pBuf)->uCheckSumAndEncryptMark == 0xAA);
						if (pDeviceExt->bLastIsBootSec)
						{
							((PYGCRYDISK_BOOTSECTOR)pBuf)->uMark = 'Y';
							((PYGCRYDISK_BOOTSECTOR)pBuf)->uEncrytTypeAndKeyLen = pDeviceExt->uEncrytTypeAndKeyLen;
							((PYGCRYDISK_BOOTSECTOR)pBuf)->uCheckSumAndEncryptMark = pDeviceExt->uCheckSumAndEncryptMark;
						}
						YGEncryptBootSector(pBuf,pDeviceExt->MediaInfo.ucKey,pDeviceExt->MediaInfo.usKeyLen);
						YGCryDiskEncryptData(&pDeviceExt->MediaInfo,&pBuf[SECTOR_SIZE],&pBuf[SECTOR_SIZE],io_stack->Parameters.Write.Length-SECTOR_SIZE);
					}
					else YGCryDiskEncryptData(&pDeviceExt->MediaInfo,pBuf,pBuf,io_stack->Parameters.Write.Length);
					YGCryDiskSyncWrite(pDeviceExt->DeviceObject,ByteOffset,io_stack->Parameters.Write.Length,pBuf,pDeviceExt->bHasYGDiskRW);
					ExFreePool(pBuf);
				}
                irp->IoStatus.Status = 0;
                irp->IoStatus.Information = io_stack->Parameters.Write.Length;
                break;

            case IRP_MJ_DEVICE_CONTROL:
                switch (io_stack->Parameters.DeviceIoControl.IoControlCode)
                {
                case IOCTL_YGCRYDISK_OPEN_MEDIA:
                    OpenMedia(device_object, irp);
                    break;
                case IOCTL_YGCRYDISK_CLOSE_MEDIA:
                    CloseMedia(device_object, irp);
                    break;
				case IOCTL_YGCRYDISK_FLUSH_MEDIA:
                    FlushMedia(device_object, irp);
                    break;
                default:
                    KdPrint((
                        "YGCryDisk: Thread: "
                        "Unknown IoControlCode 0x%x\n",
                        io_stack->Parameters.DeviceIoControl.IoControlCode
                        ));

                    irp->IoStatus.Status = STATUS_DRIVER_INTERNAL_ERROR;
                    irp->IoStatus.Information = 0;
                }
                break;

            default:
                KdPrint((
                    "YGCryDisk: Thread: "
                    "Unknown MajorFunction 0x%x\n",
                    io_stack->MajorFunction
                    ));

                irp->IoStatus.Status = STATUS_DRIVER_INTERNAL_ERROR;
                irp->IoStatus.Information = 0;
            }

            IoCompleteRequest(
                irp,
                (CCHAR) (NT_SUCCESS(irp->IoStatus.Status) ?
                IO_DISK_INCREMENT : IO_NO_INCREMENT)
                );
        }
    }
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

NTSTATUS
OpenMedia (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
    )
{
    WCHAR                           file_name_buffer[MAXIMUM_FILENAME_LENGTH];
	CHAR							szDeviceName[32],szSymLnk[32];
    PDEVICE_EXTENSION               pDeviceExt;
    NTSTATUS                        status;
    OBJECT_ATTRIBUTES               object_attributes;
    UNICODE_STRING                  file_name;

    PAGED_CODE();
    ASSERT(DeviceObject != NULL);
    ASSERT(Irp != NULL);

    pDeviceExt = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

	RtlCopyMemory(	&pDeviceExt->MediaInfo,
					Irp->AssociatedIrp.SystemBuffer,
					sizeof(YGCRYDISK_PARAMETER));

/*	pDeviceExt->ByteOffset.QuadPart = pDeviceExt->MediaInfo.PartitionInformation.StartingOffset.QuadPart;
    pDeviceExt->MediaInfo.PartitionInformation.StartingOffset.QuadPart = 0;
    pDeviceExt->MediaInfo.PartitionInformation.HiddenSectors = 1;

    if (!pDeviceExt->MediaInfo.DiskGeometry.TracksPerCylinder)
		pDeviceExt->MediaInfo.DiskGeometry.TracksPerCylinder = 0xff;

    if (!pDeviceExt->MediaInfo.DiskGeometry.SectorsPerTrack)
		pDeviceExt->MediaInfo.DiskGeometry.SectorsPerTrack = 0x3f;

    if (!pDeviceExt->MediaInfo.DiskGeometry.BytesPerSector)
		pDeviceExt->MediaInfo.DiskGeometry.BytesPerSector = SECTOR_SIZE;

    pDeviceExt->MediaInfo.DiskGeometry.Cylinders.QuadPart =  
			pDeviceExt->MediaInfo.PartitionInformation.PartitionLength.QuadPart / pDeviceExt->MediaInfo.DiskGeometry.TracksPerCylinder / pDeviceExt->MediaInfo.DiskGeometry.SectorsPerTrack / pDeviceExt->MediaInfo.DiskGeometry.BytesPerSector;*/
	pDeviceExt->MediaInfo.DiskGeometry.MediaType = FixedMedia;
//	pDeviceExt->MediaInfo.PartitionInformation.PartitionNumber = 0;

    swprintf(file_name_buffer,
	        L"\\Device\\Harddisk%d\\Partition0",
		    pDeviceExt->MediaInfo.DiskNumber);

    RtlInitUnicodeString(&file_name, file_name_buffer);

	status = IoGetDeviceObjectPointer(&file_name,FILE_ALL_ACCESS,
										  &pDeviceExt->FileObject,&pDeviceExt->DeviceObject);
    if (!NT_SUCCESS(status))      
	{
        Irp->IoStatus.Status = STATUS_DRIVER_INTERNAL_ERROR;
        Irp->IoStatus.Information = 0;
		return status;
	}
	
	if (!CheckAndInitCryDisk(pDeviceExt))
	{
        Irp->IoStatus.Status = STATUS_DRIVER_INTERNAL_ERROR;
        Irp->IoStatus.Information = 0;
		ObDereferenceObject(pDeviceExt->FileObject);
		pDeviceExt->DeviceObject = NULL;
		return STATUS_DRIVER_INTERNAL_ERROR;
	}

	DeviceObject->AlignmentRequirement = pDeviceExt->DeviceObject->AlignmentRequirement;
	sprintf(szDeviceName,"\\Device\\YGCryDisk%d",pDeviceExt->ulNumber);
	sprintf(szSymLnk,"\\DosDevices\\Global\\%C:",pDeviceExt->MediaInfo.ucDriveLetter);
	CreateGlobalDosDeviceName(szSymLnk, szDeviceName);

    pDeviceExt->bMediaInDevice = TRUE;
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    return STATUS_SUCCESS;
}

NTSTATUS
CloseMedia (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
    )
{
    PDEVICE_EXTENSION pDeviceExt;
	CHAR				szSymLnk[32];

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

NTSTATUS
FlushMedia (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
    )
{
    WCHAR                           file_name_buffer[MAXIMUM_FILENAME_LENGTH];
    PDEVICE_EXTENSION               pDeviceExt;
    OBJECT_ATTRIBUTES               object_attributes;
    UNICODE_STRING                  file_name;

    PAGED_CODE();
    ASSERT(DeviceObject != NULL);
    ASSERT(Irp != NULL);

    pDeviceExt = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
	if (pDeviceExt->bMediaInDevice)
	{
	}
	else
	{
		Irp->IoStatus.Status = STATUS_DEVICE_NOT_READY;
		Irp->IoStatus.Information = 0;
	}
	return Irp->IoStatus.Status;
}

BOOLEAN YGCryDiskEncryptData(IN PYGCRYDISK_PARAMETER pDiskParam,IN UCHAR *pInBuf,OUT UCHAR *pOutBuf,ULONG ulLength)
{
	if (!ulLength) return TRUE;

	switch(pDiskParam->usEncryptType)
	{
	case 1:
		YGAESEncryptData(pDiskParam->ucKey,pDiskParam->usKeyLen,pInBuf,pOutBuf,ulLength);
		break;
	case 2:
		YGDESEncryCode(pDiskParam->ucKey,pInBuf,pOutBuf,ulLength,YGDESCRYPTO);
		break;
	case 3:
		YGDESEncryCode(pDiskParam->ucKey,pInBuf,pOutBuf,ulLength,YGDESEDE2);
		break;
	case 4:
        YGDESEncryCode(pDiskParam->ucKey,pInBuf,pOutBuf,ulLength,YGDESEDE3);
		break;
	case 5:
		YGDESEncryCode(pDiskParam->ucKey,pInBuf,pOutBuf,ulLength,YGDESXEX3);
		break;
	default:
		return FALSE;
		break;
	}
	return TRUE;
}

BOOLEAN YGCryDiskDecryptData(IN PYGCRYDISK_PARAMETER pDiskParam,IN UCHAR *pInBuf,OUT UCHAR *pOutBuf,ULONG ulLength)
{
	switch(pDiskParam->usEncryptType)
	{
	case 1:
		YGAESDecryptData(pDiskParam->ucKey,pDiskParam->usKeyLen,pInBuf,pOutBuf,ulLength);
		break;
	case 2:
		YGDESDecryCode(pDiskParam->ucKey,pInBuf,pOutBuf,ulLength,YGDESCRYPTO);
		break;
	case 3:
		YGDESDecryCode(pDiskParam->ucKey,pInBuf,pOutBuf,ulLength,YGDESEDE2);
		break;
	case 4:
        YGDESDecryCode(pDiskParam->ucKey,pInBuf,pOutBuf,ulLength,YGDESEDE3);
		break;
	case 5:
		YGDESDecryCode(pDiskParam->ucKey,pInBuf,pOutBuf,ulLength,YGDESXEX3);
		break;
	default:
		return FALSE;
		break;
	}
	return TRUE;
}

BOOLEAN	CheckAndInitCryDisk(IN OUT PDEVICE_EXTENSION pDeviceExt)
{
	IO_STATUS_BLOCK IoStatus;
	YGCRYDISK_BOOTSECTOR	BootSector;

	pDeviceExt->bHasYGDiskRW = YGCryDiskCheckYGDiskRW(pDeviceExt->DeviceObject);

	YGCryDiskSyncRead(pDeviceExt->DeviceObject,pDeviceExt->MediaInfo.PartitionInformation.StartingOffset,SECTOR_SIZE,&BootSector,pDeviceExt->bHasYGDiskRW);
	YGDecryptBootSector((UCHAR *)&BootSector,pDeviceExt->MediaInfo.ucKey,pDeviceExt->MediaInfo.usKeyLen);

	if (((BootSector.uEncrytTypeAndKeyLen >> 5)  == (UCHAR)pDeviceExt->MediaInfo.usEncryptType) &&
		(BootSector.uMark == 'Y'))
	{
		pDeviceExt->uCheckSumAndEncryptMark = BootSector.uCheckSumAndEncryptMark;
		pDeviceExt->uEncrytTypeAndKeyLen = BootSector.uEncrytTypeAndKeyLen;
		pDeviceExt->bLastIsBootSec = TRUE;
		return TRUE;
	}
	return FALSE;
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

BOOLEAN YGCryDiskCheckYGDiskRW(IN PDEVICE_OBJECT DeviceObject)
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
YGCryDiskSyncRead(
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
		irp = IoBuildSynchronousFsdRequest( IRP_MJ_READ,
		                                DeviceObject,
		                                pBuffer,ulBytes,
		                                &Start,&event,
		                                &ioStatus );
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
} // end YGCryDiskSyncRead()

NTSTATUS
YGCryDiskSyncWrite(
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

    KeInitializeEvent(&event, NotificationEvent, FALSE);
	if (bHasYGDiskRW)
	{
		ReadWriteBuffer.dwStartSec = (ULONG)(Start.QuadPart/SECTOR_SIZE);
		ReadWriteBuffer.dwLength = ulBytes/SECTOR_SIZE;
		ReadWriteBuffer.pBuffer = pBuffer;

		irp = IoBuildDeviceIoControlRequest(
						IOCTL_YGDISKRW_WRITE_DISK_KERNEL,
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
		irp = IoBuildSynchronousFsdRequest( IRP_MJ_WRITE,
		                                DeviceObject,
		                                pBuffer,ulBytes,
		                                &Start,&event,
		                                &ioStatus );
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
} // end YGCryDiskSyncRead()

BOOLEAN YGEncryptBootSector(UCHAR *pBuf,UCHAR *pKey,int nKeyLen)
{
	USHORT wEncyptMark = 'YG';
	int		k;

	YGAESEncryptData(pKey,nKeyLen,pBuf,pBuf,256);

	for (k = 128;k < 254;k++)
	{
		((USHORT *)pBuf)[k] ^= wEncyptMark;
	}

	return TRUE;
}

BOOLEAN YGDecryptBootSector(UCHAR *pBuf,UCHAR *pKey,int nKeyLen)
{
	USHORT wDecryptMark = 'YG';
	int		k;

	YGAESDecryptData(pKey,nKeyLen,pBuf,pBuf,256);

	for (k = 128;k < 254;k++)
	{
		((USHORT *)pBuf)[k] ^= wDecryptMark;
	}

	return TRUE;
}

