/*
    This is a virtual disk driver for Windows NT and Windows 2000 that 
*/

#include <ntddk.h>
#include <WDMSec.h>
#include <stdarg.h>
#include <stdio.h>
#include <ntdddisk.h>
#include <mountmgr.h>
#include "..\..\Export\2000\YGVPar.h"
#include "\tech\lib\hashsrch\Cur_ver\Export\2000\HashSrch.h"

#define MOUNTDEVCONTROLTYPE  ((ULONG) 'M')

#define IOCTL_MOUNTDEV_QUERY_DEVICE_NAME    CTL_CODE(MOUNTDEVCONTROLTYPE, 2, METHOD_BUFFERED, FILE_ANY_ACCESS)

// {E533ED75-C568-4a7b-AC89-2D83DF4C7631}
static const GUID GUID_DEVCLASS_YGVPAR = 
{ 0xe533ed75, 0xc568, 0x4a7b, { 0xac, 0x89, 0x2d, 0x83, 0xdf, 0x4c, 0x76, 0x31 } };

#define DEFAULT_HASH_SIZE   0x2000

typedef struct _DEVICE_EXTENSION {
    BOOLEAN                 bMediaInDevice;
	BOOLEAN					bInWorking;
	YGVPAR_PARAMETER        MediaInfo;
    LIST_ENTRY              pListHead;
    KSPIN_LOCK              kListLock;
    KEVENT                  keRequestEvent;
    PVOID                   pThreadPointer;
    BOOLEAN                 bTerminateThread;
	ULONG					ulMinSecAddr;
	ULONG					ulMaxSecAddr;
	HANDLE					hHash;
	ULONG                   ulHashSize;
	ULONG					ulSysSectors;
	HANDLE                  hHashData;
	LARGE_INTEGER           lnCurPointer;
	ULONG                   ulNumber;
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING  RegistryPath);
NTSTATUS CreateDevice(IN PDRIVER_OBJECT DriverObject,IN ULONG Number);

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

BOOLEAN 
IsSameBytesSector(
	IN UCHAR *pBuf
);

NTSTATUS
FlushMedia (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
);

NTSTATUS FreeFsData(IN PDEVICE_EXTENSION pDeviceExt);
ULONG    GetSectorGroup(IN PDEVICE_EXTENSION pDeviceExt,IN UCHAR *pSector,IN ULONG *pulSectorNum,IN ULONG ulMaxSectors);
BOOLEAN WriteSector(IN PDEVICE_EXTENSION pDeviceExt,IN ULONG ulLinSectorNum,IN UCHAR *pBuf,IN ULONG ulSectors);
ULONG ReadSector(IN PDEVICE_EXTENSION pDeviceExt,IN ULONG ulLinSectorNum,IN OUT UCHAR *pBuf,IN ULONG ulSecNumber,IN BOOLEAN bFillGabage);
ULONG FastUpdateFSData(IN PDEVICE_EXTENSION pDeviceExt,IN ULONG *pBitmap,IN ULONG dwLen);
VOID  FillParInfoEx(IN PDEVICE_EXTENSION pDeviceExt,IN OUT PPARTITION_INFORMATION_EX pPartitionInfoEx);
NTSTATUS CreateGlobalDosDeviceName(LPSTR lpSymbolicLinkName, LPSTR lpDeviceName);
NTSTATUS DeleteGlobalDosDeviceName(LPSTR lpSymbolicLinkName);

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

    for (i = 0, created_devices = 0; i < MAX_VPAR_DRIVE; i++)
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

    swprintf(device_name_buffer,YGVPAR_DIR_NAME L"%d", Number);
    RtlInitUnicodeString(&device_name,device_name_buffer);

    (void) RtlInitUnicodeString( &sddlString, L"D:P(A;;GA;;;SY)(A;;GA;;;BA)(A;;GA;;;WD)(A;;GA;;;RC)");
    status = IoCreateDeviceSecure(
                DriverObject,
                sizeof(DEVICE_EXTENSION),
                &device_name,
                FILE_DEVICE_DISK,
                FILE_DEVICE_SECURE_OPEN,
                (BOOLEAN) FALSE,
                &sddlString,
                (LPCGUID)&GUID_DEVCLASS_YGVPAR,
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

    if (NT_SUCCESS(status))
    {
		device_object->Flags |= DO_DIRECT_IO;
		pDeviceExt = (PDEVICE_EXTENSION) device_object->DeviceExtension;

		swprintf(wcDosNameBuffer,L"\\DosDevices\\YGVPar%d", Number);
		RtlInitUnicodeString(&usSymLnkName,wcDosNameBuffer);
		status = IoCreateSymbolicLink(&usSymLnkName,&device_name);
	    if (NT_SUCCESS(status))
	    {
			RtlZeroMemory(device_object->DeviceExtension,sizeof(DEVICE_EXTENSION));
			pDeviceExt->ulNumber = Number;
			InitializeListHead(&pDeviceExt->pListHead);
			KeInitializeSpinLock(&pDeviceExt->kListLock);

			KeInitializeEvent(&pDeviceExt->keRequestEvent,SynchronizationEvent,FALSE);

			status = PsCreateSystemThread(&thread_handle,(ACCESS_MASK) 0L,
											NULL,NULL,NULL,Thread,device_object);

			if (NT_SUCCESS(status))
			{
				status = ObReferenceObjectByHandle(thread_handle,THREAD_ALL_ACCESS,
													NULL,KernelMode, &pDeviceExt->pThreadPointer,NULL);
				
				ZwClose(thread_handle);
				if (NT_SUCCESS(status))	return STATUS_SUCCESS;

				pDeviceExt->bTerminateThread = TRUE;
				KeSetEvent(&pDeviceExt->keRequestEvent,	(KPRIORITY) 0,FALSE);
			}
			IoDeleteSymbolicLink(&usSymLnkName);
		}
        IoDeleteDevice(device_object);
	}
	return status;
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

    pDeviceExt->bTerminateThread = TRUE;

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

    swprintf(wcDosNameBuffer,L"\\DosDevices\\YGVPar%d", pDeviceExt->ulNumber);
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
        KdPrint(("YGVPar: ReadWrite: No media in device\n"));

        Irp->IoStatus.Status = STATUS_NO_MEDIA_IN_DEVICE;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        return STATUS_NO_MEDIA_IN_DEVICE;
    }

    io_stack = IoGetCurrentIrpStackLocation(Irp);

    if (pDeviceExt->bInWorking == FALSE && io_stack->MajorFunction == IRP_MJ_WRITE)
    {
        Irp->IoStatus.Status = STATUS_SUCCESS;
        Irp->IoStatus.Information = io_stack->Parameters.Write.Length;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return STATUS_SUCCESS;
    }


    offset = io_stack->Parameters.Read.ByteOffset.QuadPart;

    length = io_stack->Parameters.Read.Length;

    partition_length =
        pDeviceExt->MediaInfo.PartitionInformation.PartitionLength.QuadPart;

    if ((length == 0) ||
        ((offset + length) > partition_length) ||
        (offset % SECTOR_SIZE) ||
        (length % SECTOR_SIZE))
    {
        KdPrint(("YGVPar: ReadWrite: Invalid parameter\n"));

        Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        return STATUS_INVALID_PARAMETER;
    }

    IoMarkIrpPending(Irp);

    ExInterlockedInsertTailList(&pDeviceExt->pListHead,
								&Irp->Tail.Overlay.ListEntry,
								&pDeviceExt->kListLock);

    KeSetEvent(&pDeviceExt->keRequestEvent,(KPRIORITY) 0,FALSE);

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
        io_stack->Parameters.DeviceIoControl.IoControlCode != IOCTL_YGVPAR_OPEN_MEDIA &&
		io_stack->Parameters.DeviceIoControl.IoControlCode != IOCTL_YGVPAR_DRIVE_ISUSEFUL)
    {
        KdPrint(("YGVPar: DeviceControl: No media in device\n"));

        Irp->IoStatus.Status = STATUS_NO_MEDIA_IN_DEVICE;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        return STATUS_NO_MEDIA_IN_DEVICE;
    }

    switch (io_stack->Parameters.DeviceIoControl.IoControlCode)
    {
	case IOCTL_YGVPAR_ENABLE_DISABLE:
        if (io_stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(DIOC_REGISTERS))
        {
            status = STATUS_INVALID_PARAMETER;
			Irp->IoStatus.Information = 0;
			break;
		}
		pDeviceExt->bInWorking = (BOOLEAN)((DIOC_REGISTERS *)Irp->AssociatedIrp.SystemBuffer)->reg_EAX;
        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = 0;
		break;
	case IOCTL_YGVPAR_DRIVE_ISUSEFUL:
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

	case IOCTL_YGVPAR_GET_VERSION:
        if (io_stack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(DIOC_REGISTERS))
        {
            status = STATUS_INVALID_PARAMETER;
			Irp->IoStatus.Information = 0;
			break;
		}
    	((DIOC_REGISTERS *)Irp->AssociatedIrp.SystemBuffer)->reg_EAX = YGVPAR_VERSION_MAJOR << 16 | YGVPAR_VERSION_MINOR;
        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = sizeof(DIOC_REGISTERS);
		break;

	case IOCTL_YGVPAR_SYSSEC_NUMBER:
        if (io_stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(DIOC_REGISTERS) ||
			io_stack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(DIOC_REGISTERS))
        {
            status = STATUS_INVALID_PARAMETER;
            Irp->IoStatus.Information = 0;
            break;
        }
    	((DIOC_REGISTERS *)Irp->AssociatedIrp.SystemBuffer)->reg_EAX = pDeviceExt->ulSysSectors;
        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = sizeof(DIOC_REGISTERS);
		break;

	case IOCTL_YGVPAR_UPDATE_FSDATA:

        if (io_stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(DIOC_REGISTERS) ||
			io_stack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(DIOC_REGISTERS))
        {
            status = STATUS_INVALID_PARAMETER;
            Irp->IoStatus.Information = 0;
            break;
        }
        Irp->MdlAddress = IoAllocateMdl((PVOID)((DIOC_REGISTERS *)Irp->AssociatedIrp.SystemBuffer)->reg_EAX,
										((DIOC_REGISTERS *)Irp->AssociatedIrp.SystemBuffer)->reg_EBX,
										  FALSE, FALSE,NULL);
		if (Irp->MdlAddress == NULL)
		{
            Irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
            Irp->IoStatus.Information = 0;
            break;
		}
		MmProbeAndLockPages(Irp->MdlAddress,KernelMode,IoReadAccess);
        status = STATUS_PENDING;
        break;

	case IOCTL_YGVPAR_SECTOR_GROUPS:
        if (io_stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(DIOC_REGISTERS) ||
			io_stack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(DIOC_REGISTERS))
        {
            status = STATUS_INVALID_PARAMETER;
            Irp->IoStatus.Information = 0;
            break;
        }
        Irp->MdlAddress = IoAllocateMdl((PVOID)((DIOC_REGISTERS *)Irp->AssociatedIrp.SystemBuffer)->reg_EBX,
										((DIOC_REGISTERS *)Irp->AssociatedIrp.SystemBuffer)->reg_EAX*SECTOR_SIZE,
										  FALSE, FALSE,NULL);
		if (Irp->MdlAddress == NULL)
		{
            Irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
            Irp->IoStatus.Information = 0;
            break;
		}
		MmProbeAndLockPages(Irp->MdlAddress,KernelMode,IoWriteAccess);
        status = STATUS_PENDING;
        break;

    case IOCTL_YGVPAR_OPEN_MEDIA:
        if (pDeviceExt->bMediaInDevice == TRUE)
        {
            status = STATUS_INVALID_DEVICE_REQUEST;
            Irp->IoStatus.Information = 0;
            break;
        }

        if (io_stack->Parameters.DeviceIoControl.InputBufferLength <
            sizeof(YGVPAR_PARAMETER))
        {
            status = STATUS_INVALID_PARAMETER;
            Irp->IoStatus.Information = 0;
            break;
        }
        status = STATUS_PENDING;
        break;
	case IOCTL_YGVPAR_FLUSH_MEDIA:
        status = STATUS_PENDING;
        break;
    case IOCTL_YGVPAR_CLOSE_MEDIA:
        status = STATUS_PENDING;
        break;
	case IOCTL_YGVPAR_READ_DISK:

        if (io_stack->Parameters.DeviceIoControl.OutputBufferLength != sizeof(YGVPARRWPARAM))
        {
            status = STATUS_INVALID_PARAMETER;
            Irp->IoStatus.Information = 0;
            break;
        }
        Irp->MdlAddress = IoAllocateMdl(((PYGVPARRWPARAM)Irp->AssociatedIrp.SystemBuffer)->pBuffer,
										((PYGVPARRWPARAM)Irp->AssociatedIrp.SystemBuffer)->dwSectors*SECTOR_SIZE,
										  FALSE, FALSE,NULL);
		if (Irp->MdlAddress == NULL)
		{
            Irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
            Irp->IoStatus.Information = 0;
            break;
		}
		MmProbeAndLockPages(Irp->MdlAddress,KernelMode,IoWriteAccess);
        status = STATUS_PENDING;
        break;

	case IOCTL_YGVPAR_WRITE_DISK:
        if (io_stack->Parameters.DeviceIoControl.InputBufferLength != sizeof(YGVPARRWPARAM))
        {
            status = STATUS_INVALID_PARAMETER;
            Irp->IoStatus.Information = 0;
            break;
        }
        Irp->MdlAddress = IoAllocateMdl(((PYGVPARRWPARAM)Irp->AssociatedIrp.SystemBuffer)->pBuffer,
										((PYGVPARRWPARAM)Irp->AssociatedIrp.SystemBuffer)->dwSectors*SECTOR_SIZE,
										  FALSE, FALSE,NULL);
		if (Irp->MdlAddress == NULL)
		{
            Irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
            Irp->IoStatus.Information = 0;
            break;
		}
		MmProbeAndLockPages(Irp->MdlAddress,KernelMode,IoReadAccess);
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
                "YGVPar: Unknown IoControlCode %#x\n",
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
        KeSetEvent(&pDeviceExt->keRequestEvent, (KPRIORITY) 0, FALSE);
	}
    return status;
}

#pragma code_seg("page")

BOOLEAN YGVParReadDisk(PDEVICE_EXTENSION pDeviceExt,ULONG ulStartSec,ULONG ulSectors,UCHAR *pBuffer)
{
	ULONG i;
	for (i = 0;i < ulSectors;i++)
	{
		ReadSector(pDeviceExt,ulStartSec,pBuffer,1,TRUE);
		ulStartSec ++;
		pBuffer += SECTOR_SIZE;
	}
	return TRUE;
}

BOOLEAN YGVParWriteDisk(PDEVICE_EXTENSION pDeviceExt,ULONG ulStartSec,ULONG ulSectors,UCHAR *pBuffer)
{
	ULONG i;
	for (i = 0;i < ulSectors;i++)
	{
		WriteSector(pDeviceExt,ulStartSec,pBuffer,1);
		ulStartSec ++;
		pBuffer += SECTOR_SIZE;
	}
	return TRUE;
}

VOID
Thread (
    IN PVOID Context
    )
{
	DIOC_REGISTERS      *pIoReg;
    PDEVICE_OBJECT      device_object;
    PDEVICE_EXTENSION   pDeviceExt;
	ULONG               ulSectorNo,ulOffset;
    PLIST_ENTRY         request;
	UCHAR               *pbtMemData;
    PIRP                irp;
    PIO_STACK_LOCATION  io_stack;

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

        if (pDeviceExt->bTerminateThread)
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
				ulSectorNo = io_stack->Parameters.Read.Length/SECTOR_SIZE;
			    ulOffset = (ULONG)(io_stack->Parameters.Read.ByteOffset.QuadPart/SECTOR_SIZE);

			    pbtMemData = (UCHAR*)MmGetSystemAddressForMdlSafe(irp->MdlAddress,NormalPagePriority);

				ReadSector(pDeviceExt,ulOffset,pbtMemData,ulSectorNo,TRUE);
				irp->IoStatus.Status = STATUS_SUCCESS;
				irp->IoStatus.Information = io_stack->Parameters.Read.Length;
                break;

            case IRP_MJ_WRITE:
				ulSectorNo = io_stack->Parameters.Write.Length/SECTOR_SIZE;
			    ulOffset = (ULONG)(io_stack->Parameters.Write.ByteOffset.QuadPart/SECTOR_SIZE);

			    pbtMemData = (UCHAR*)MmGetSystemAddressForMdlSafe(irp->MdlAddress,NormalPagePriority);

				if (WriteSector(pDeviceExt,ulOffset,pbtMemData,ulSectorNo))
					irp->IoStatus.Status = STATUS_SUCCESS;
				else
					irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
				irp->IoStatus.Information = io_stack->Parameters.Read.Length;
                break;

            case IRP_MJ_DEVICE_CONTROL:
                switch (io_stack->Parameters.DeviceIoControl.IoControlCode)
                {
                case IOCTL_YGVPAR_OPEN_MEDIA:
                    OpenMedia(device_object, irp);
                    break;
                case IOCTL_YGVPAR_CLOSE_MEDIA:
                    CloseMedia(device_object, irp);
                    break;
				case IOCTL_YGVPAR_FLUSH_MEDIA:
                    FlushMedia(device_object, irp);
					break;
				case IOCTL_YGVPAR_SECTOR_GROUPS:
					pIoReg = (DIOC_REGISTERS *)irp->AssociatedIrp.SystemBuffer;
				    pbtMemData = (UCHAR*)MmGetSystemAddressForMdlSafe(irp->MdlAddress,NormalPagePriority);
					pIoReg->reg_EAX = GetSectorGroup(pDeviceExt,pbtMemData,&pIoReg->reg_EDX,pIoReg->reg_EAX);
					MmUnlockPages(irp->MdlAddress);
					IoFreeMdl(irp->MdlAddress);
					irp->MdlAddress = NULL;
					irp->IoStatus.Status = STATUS_SUCCESS;
					irp->IoStatus.Information = sizeof(DIOC_REGISTERS);
					break;
				case IOCTL_YGVPAR_UPDATE_FSDATA:
					pIoReg = (DIOC_REGISTERS *)irp->AssociatedIrp.SystemBuffer;
				    pbtMemData = (UCHAR*)MmGetSystemAddressForMdlSafe(irp->MdlAddress,NormalPagePriority);
					pIoReg->reg_EAX = FastUpdateFSData(pDeviceExt,(ULONG *)pbtMemData,pIoReg->reg_EBX);
					MmUnlockPages(irp->MdlAddress);
					IoFreeMdl(irp->MdlAddress);
					irp->MdlAddress = NULL;
					irp->IoStatus.Status = STATUS_SUCCESS;
					irp->IoStatus.Information = sizeof(DIOC_REGISTERS);
					break;
				case IOCTL_YGVPAR_READ_DISK:
				    pbtMemData = (UCHAR*)MmGetSystemAddressForMdlSafe(irp->MdlAddress,NormalPagePriority);
					YGVParReadDisk(pDeviceExt,((PYGVPARRWPARAM)irp->AssociatedIrp.SystemBuffer)->dwStartSec,((PYGVPARRWPARAM)irp->AssociatedIrp.SystemBuffer)->dwSectors,pbtMemData);
					MmUnlockPages(irp->MdlAddress);
					IoFreeMdl(irp->MdlAddress);
					irp->MdlAddress = NULL;
					irp->IoStatus.Status = STATUS_SUCCESS;
					irp->IoStatus.Information = sizeof(DIOC_REGISTERS);
					break;
				case IOCTL_YGVPAR_WRITE_DISK:
				    pbtMemData = (UCHAR*)MmGetSystemAddressForMdlSafe(irp->MdlAddress,NormalPagePriority);
					YGVParWriteDisk(pDeviceExt,((PYGVPARRWPARAM)irp->AssociatedIrp.SystemBuffer)->dwStartSec,((PYGVPARRWPARAM)irp->AssociatedIrp.SystemBuffer)->dwSectors,pbtMemData);
					MmUnlockPages(irp->MdlAddress);
					IoFreeMdl(irp->MdlAddress);
					irp->MdlAddress = NULL;
					irp->IoStatus.Status = STATUS_SUCCESS;
					irp->IoStatus.Information = sizeof(DIOC_REGISTERS);
					break;
                default:
                    KdPrint((
                        "YGVPar: Thread: "
                        "Unknown IoControlCode 0x%x\n",
                        io_stack->Parameters.DeviceIoControl.IoControlCode
                        ));

                    irp->IoStatus.Status = STATUS_DRIVER_INTERNAL_ERROR;
                    irp->IoStatus.Information = 0;
                }
                break;

            default:
                KdPrint((
                    "YGVPar: Thread: "
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

NTSTATUS
OpenMedia (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
    )
{
	UNICODE_STRING			        usDataFile;              
	CHAR							szDeviceName[32],szSymLnk[32];
    OBJECT_ATTRIBUTES               ObjectAttributes;
    WCHAR							wFileName[MAXIMUM_FILENAME_LENGTH];
    PDEVICE_EXTENSION               pDeviceExt;

    PAGED_CODE();
    ASSERT(DeviceObject != NULL);
    ASSERT(Irp != NULL);

    pDeviceExt = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    if ((pDeviceExt->hHash = CreateHashList(DEFAULT_HASH_SIZE,sizeof(ULONG),PagedPool)))
    {
		RtlCopyMemory(	&pDeviceExt->MediaInfo,
						Irp->AssociatedIrp.SystemBuffer,
						sizeof(YGVPAR_PARAMETER));
		swprintf(wFileName,L"\\??\\%c:\\YGVPar%d.DAT",pDeviceExt->MediaInfo.btFileDrive,pDeviceExt->ulNumber);
	    RtlInitUnicodeString(&usDataFile,wFileName);

	    InitializeObjectAttributes(&ObjectAttributes,&usDataFile,OBJ_CASE_INSENSITIVE,NULL,NULL);

	    Irp->IoStatus.Status = ZwCreateFile(&pDeviceExt->hHashData,GENERIC_READ | GENERIC_WRITE,
									        &ObjectAttributes,&Irp->IoStatus,NULL,
											FILE_ATTRIBUTE_NORMAL,
									        0,FILE_OPEN_IF,
											FILE_NON_DIRECTORY_FILE |FILE_RANDOM_ACCESS |
									        FILE_NO_INTERMEDIATE_BUFFERING | FILE_SYNCHRONOUS_IO_NONALERT,
									        NULL, 0);
		if (Irp->IoStatus.Status == STATUS_SUCCESS)
		{
			pDeviceExt->ulSysSectors = 0;
			pDeviceExt->lnCurPointer.QuadPart = 0;
			pDeviceExt->ulHashSize = DEFAULT_HASH_SIZE;
			pDeviceExt->ulMaxSecAddr = 0;
			pDeviceExt->ulMinSecAddr = (ULONG)(pDeviceExt->MediaInfo.PartitionInformation.PartitionLength.QuadPart/SECTOR_SIZE);

			pDeviceExt->MediaInfo.DiskGeometry.MediaType = 0x0c;
			pDeviceExt->MediaInfo.PartitionInformation.PartitionType = 0;
			pDeviceExt->MediaInfo.PartitionInformation.BootIndicator = FALSE;
			pDeviceExt->MediaInfo.PartitionInformation.RecognizedPartition = FALSE;
			pDeviceExt->MediaInfo.PartitionInformation.RewritePartition = FALSE;

/*			pDeviceExt->MediaInfo.PartitionInformation.PartitionType = 0;
			pDeviceExt->MediaInfo.PartitionInformation.BootIndicator = FALSE;
			pDeviceExt->MediaInfo.PartitionInformation.RecognizedPartition = FALSE;
			pDeviceExt->MediaInfo.PartitionInformation.RewritePartition = FALSE;
			pDeviceExt->MediaInfo.PartitionInformation.StartingOffset.QuadPart = 0;
			pDeviceExt->MediaInfo.PartitionInformation.HiddenSectors = 1;
			pDeviceExt->MediaInfo.PartitionInformation.PartitionNumber = 0;
*/
			if (!pDeviceExt->MediaInfo.DiskGeometry.TracksPerCylinder)
				pDeviceExt->MediaInfo.DiskGeometry.TracksPerCylinder = 0xff;

			if (!pDeviceExt->MediaInfo.DiskGeometry.SectorsPerTrack)
				pDeviceExt->MediaInfo.DiskGeometry.SectorsPerTrack = 0x3f;

			if (!pDeviceExt->MediaInfo.DiskGeometry.BytesPerSector)
				pDeviceExt->MediaInfo.DiskGeometry.BytesPerSector = SECTOR_SIZE;

//			pDeviceExt->MediaInfo.DiskGeometry.Cylinders.QuadPart =  
//					pDeviceExt->MediaInfo.PartitionInformation.PartitionLength.QuadPart / pDeviceExt->MediaInfo.DiskGeometry.TracksPerCylinder / pDeviceExt->MediaInfo.DiskGeometry.SectorsPerTrack / pDeviceExt->MediaInfo.DiskGeometry.BytesPerSector;
			pDeviceExt->MediaInfo.DiskGeometry.MediaType = FixedMedia;

			DeviceObject->AlignmentRequirement = SECTOR_SIZE;

			sprintf(szDeviceName,"\\Device\\YGVPar\\YGVPar%d",pDeviceExt->ulNumber);
			sprintf(szSymLnk,"\\DosDevices\\Global\\%C:",pDeviceExt->MediaInfo.ucDriveLetter);
			CreateGlobalDosDeviceName(szSymLnk, szDeviceName);

			pDeviceExt->bMediaInDevice = TRUE;
			pDeviceExt->bInWorking = TRUE;
		}
		else
		{
			DestroyHashList(pDeviceExt->hHash);
			pDeviceExt->hHash = NULL;
		}
	}
	else Irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
	Irp->IoStatus.Information = 0;
	return Irp->IoStatus.Status;
}

NTSTATUS
FlushMedia (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
    )
{
	UNICODE_STRING			        usDataFile;              
    OBJECT_ATTRIBUTES               ObjectAttributes;
    WCHAR							wFileName[MAXIMUM_FILENAME_LENGTH];
    PDEVICE_EXTENSION               pDeviceExt;

    PAGED_CODE();
    ASSERT(DeviceObject != NULL);
    ASSERT(Irp != NULL);

    pDeviceExt = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
	if (pDeviceExt->bMediaInDevice)
	{
		ZwClose(pDeviceExt->hHashData);

		swprintf(wFileName,L"\\??\\%c:\\YGVPar%d.DAT",pDeviceExt->MediaInfo.btFileDrive,pDeviceExt->ulNumber);
		RtlInitUnicodeString(&usDataFile,wFileName);

		InitializeObjectAttributes(&ObjectAttributes,&usDataFile,OBJ_CASE_INSENSITIVE,NULL,NULL);

		return ZwCreateFile(&pDeviceExt->hHashData,GENERIC_READ | GENERIC_WRITE,
											&ObjectAttributes,&Irp->IoStatus,NULL,
											FILE_ATTRIBUTE_NORMAL,
											0,FILE_OPEN_IF,
											FILE_NON_DIRECTORY_FILE |FILE_RANDOM_ACCESS |
											FILE_NO_INTERMEDIATE_BUFFERING | FILE_SYNCHRONOUS_IO_NONALERT,
											NULL, 0);
	}
	else
	{
		Irp->IoStatus.Status = STATUS_DEVICE_NOT_READY;
		Irp->IoStatus.Information = 0;
	}
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

	ZwClose(pDeviceExt->hHashData);
	FreeFsData(pDeviceExt);

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
		for (ulSecAddr = pDeviceExt->ulMinSecAddr; ulSecAddr <= pDeviceExt->ulMaxSecAddr; ulSecAddr++)
		{
			ulIndex = ulSecAddr%pDeviceExt->ulHashSize;
			pSector = (ULONG *)GetObj(pDeviceExt->hHash,ulSecAddr,ulIndex);
			if (pSector)
			{
				RemoveObj(pDeviceExt->hHash,(PVOID)pSector,ulIndex);
				DeallocateObj(pDeviceExt->hHash,(PVOID)pSector,ulIndex);
			}
		}
		DestroyHashList(pDeviceExt->hHash);
		pDeviceExt->hHash = NULL;
    }
    pDeviceExt->ulSysSectors = 0;
    return  STATUS_SUCCESS;
}   //FreeFsData

ULONG GetSectorGroup(IN PDEVICE_EXTENSION pDeviceExt,IN UCHAR *pSector,IN ULONG *pulSectorNum,IN ULONG ulMaxSectors)
{
    ULONG	ulSecAddr;
    ULONG 	n,ulSectors;

	if (*pulSectorNum) ulSecAddr = *pulSectorNum - (ULONG)(pDeviceExt->MediaInfo.PartitionInformation.StartingOffset.QuadPart/SECTOR_SIZE);
	else ulSecAddr = 0;
    for (ulSecAddr = ulSecAddr>pDeviceExt->ulMinSecAddr?ulSecAddr:pDeviceExt->ulMinSecAddr;
		 ulSecAddr <= pDeviceExt->ulMaxSecAddr;ulSecAddr++)
    {
		n = ReadSector(pDeviceExt,ulSecAddr,pSector,ulMaxSectors,FALSE);
		if (n)
		{
			ulSectors = n;
		    *pulSectorNum = ulSecAddr+(ULONG)(pDeviceExt->MediaInfo.PartitionInformation.StartingOffset.QuadPart/SECTOR_SIZE);
/*			while (ulSectors < ulMaxSectors)
			{
				ulSecAddr += n;
				pSector += (n*SECTOR_SIZE);
				n = ReadSector(pDeviceExt,ulSecAddr,pSector,ulMaxSectors-ulSectors,FALSE);
				if (!n) break;
				ulSectors += n;
			}*/
			return ulSectors;
		}
    }
	return 0;
}

ULONG ReadSector(IN PDEVICE_EXTENSION pDeviceExt,IN ULONG ulLinSectorNum,IN OUT UCHAR *pBuf,IN ULONG ulSecNumber,IN BOOLEAN bFillGabage)
{
	IO_STATUS_BLOCK IoStatus;
	LARGE_INTEGER   lnOffset;
	ULONG    	    ulIndex,*pulSector,i,ulCurSec,ulPreSec,ulSectors;

	for (i = 0;i < ulSecNumber;i++)
	{
		ulIndex = ulLinSectorNum % pDeviceExt->ulHashSize;
		pulSector = (ULONG *)GetObj(pDeviceExt->hHash,ulLinSectorNum,ulIndex);
		if (pulSector)
		{
			if ((long)(*pulSector) < 0)
			{
				RtlFillMemory(pBuf,SECTOR_SIZE,(UCHAR)((*pulSector)&0xFF));
				ulLinSectorNum ++;
				pBuf += SECTOR_SIZE;
			}
			else
			{
				ulPreSec = (*pulSector)/0x100;
				ulLinSectorNum ++;
				ulSectors = 1;
				lnOffset.QuadPart = ulPreSec * SECTOR_SIZE;
				for(ulCurSec = i+1;ulCurSec < ulSecNumber;ulCurSec++)
				{
					ulIndex = (ulLinSectorNum) % pDeviceExt->ulHashSize;
					pulSector = (ULONG *)GetObj(pDeviceExt->hHash,ulLinSectorNum,ulIndex);
					if (pulSector)
					{
						if ((long)(*pulSector) >= 0)
						{
							if (ulPreSec+1 == (*pulSector)/0x100) ulPreSec++;
							else	break;
						}
						else break;
					}
					else break;
					ulLinSectorNum ++;
					ulSectors ++;
				}
				if (STATUS_SUCCESS != ZwReadFile(pDeviceExt->hHashData,NULL,NULL,NULL,
							&IoStatus,pBuf,ulSectors * SECTOR_SIZE,&lnOffset,NULL))
				{
					break;
				}
				pBuf += (ulSectors * SECTOR_SIZE);
				i = ulCurSec - 1;
			}
		}
		else
		{
			if (bFillGabage)
			{
				ulLinSectorNum ++;
				RtlFillMemory(pBuf,SECTOR_SIZE,GARBAGE);
				pBuf += SECTOR_SIZE;
			}
			else break;
		}
	}
    return i;
}   //	ReadSector

BOOLEAN WriteSector(IN PDEVICE_EXTENSION pDeviceExt,IN ULONG ulLinSectorNum,IN UCHAR *pBuf,IN ULONG ulSectors)
{
	IO_STATUS_BLOCK IoStatus;
	LARGE_INTEGER   lnOffset;
    ULONG	    ulIndex,ulSector,*pulSector,i,ulPreSector,ulCurSec = 0;
    BOOLEAN	    bCreated;

	for (i = 0; i < ulSectors;i ++)
	{
		ulIndex = ulLinSectorNum % pDeviceExt->ulHashSize;
		pulSector = (ULONG *)GetObj(pDeviceExt->hHash,ulLinSectorNum,ulIndex);
		if (IsSameBytesSector(pBuf))
		{
			if (!pulSector)		ulSector = -0x100;
			else ulSector = (*pulSector | 0x80000000);
			ulSector |= pBuf[0];
			if (ulCurSec)
			{
				if (STATUS_SUCCESS != ZwWriteFile(pDeviceExt->hHashData, NULL,NULL,NULL,
							&IoStatus,pBuf-ulCurSec*SECTOR_SIZE,ulCurSec*SECTOR_SIZE,&lnOffset,NULL))
				{
					return FALSE;
				}
				ulPreSector = -1;
				ulCurSec = 0;
			}
		}
		else
		{
			if (pulSector)
			{
				ulSector = (*pulSector&0x7FFFFFFF)/0x100;
				if (ulSector == 0x7FFFFF) 
				{
					if (!ulCurSec)
						lnOffset.QuadPart = pDeviceExt->lnCurPointer.QuadPart;
					else
					{
						if (ulPreSector != pDeviceExt->lnCurPointer.QuadPart/SECTOR_SIZE)
						{
							if (STATUS_SUCCESS != ZwWriteFile(pDeviceExt->hHashData, NULL,NULL,NULL,
										&IoStatus,pBuf-ulCurSec*SECTOR_SIZE,ulCurSec*SECTOR_SIZE,&lnOffset,NULL))
							{
								return FALSE;
							}
							lnOffset.QuadPart = pDeviceExt->lnCurPointer.QuadPart;
							ulCurSec = 0;
						}
					}
					ulSector = (ULONG)(lnOffset.QuadPart/SECTOR_SIZE);
					ulPreSector = ulSector;
					pDeviceExt->lnCurPointer.QuadPart += SECTOR_SIZE;
				}
				else
				{
					if (!ulCurSec)
						lnOffset.QuadPart = ulSector*SECTOR_SIZE;
					else
					{
						if (ulPreSector+1 != ulSector)
						{
							if (STATUS_SUCCESS != ZwWriteFile(pDeviceExt->hHashData, NULL,NULL,NULL,
										&IoStatus,pBuf-ulCurSec*SECTOR_SIZE,ulCurSec*SECTOR_SIZE,&lnOffset,NULL))
							{
								return FALSE;
							}
							lnOffset.QuadPart = ulSector*SECTOR_SIZE;
							ulCurSec = 0;
						}
					}
					ulPreSector = ulSector;
				}
			}
			else
			{
				ulSector = (ULONG)(pDeviceExt->lnCurPointer.QuadPart/SECTOR_SIZE);
				if (!ulCurSec)
					lnOffset.QuadPart = pDeviceExt->lnCurPointer.QuadPart;
				else
				{
					if (ulPreSector+1 != ulSector)
					{
						if (STATUS_SUCCESS != ZwWriteFile(pDeviceExt->hHashData, NULL,NULL,NULL,
									&IoStatus,pBuf-ulCurSec*SECTOR_SIZE,ulCurSec*SECTOR_SIZE,&lnOffset,NULL))
						{
							return FALSE;
						}
						lnOffset.QuadPart = ulSector*SECTOR_SIZE;
						ulCurSec = 0;
					}
				}
				ulPreSector = ulSector;
				pDeviceExt->lnCurPointer.QuadPart += SECTOR_SIZE;
			}
			ulCurSec ++;
			ulSector *= 0x100;
		}
		if (!SetObj(pDeviceExt->hHash,ulLinSectorNum,ulIndex,&ulSector,&bCreated))
		{
			return FALSE;
		}
		if (bCreated)
		{
			pDeviceExt->ulMinSecAddr = pDeviceExt->ulMinSecAddr<ulLinSectorNum?pDeviceExt->ulMinSecAddr:ulLinSectorNum;
			pDeviceExt->ulMaxSecAddr = pDeviceExt->ulMaxSecAddr>ulLinSectorNum?pDeviceExt->ulMaxSecAddr:ulLinSectorNum;
			pDeviceExt->ulSysSectors++;
		}
		ulLinSectorNum ++;
		pBuf += SECTOR_SIZE;
	}
	if (ulCurSec)
	{
		if (STATUS_SUCCESS != ZwWriteFile(pDeviceExt->hHashData, NULL,NULL,NULL,
					&IoStatus,pBuf-ulCurSec*SECTOR_SIZE,ulCurSec*SECTOR_SIZE,&lnOffset,NULL))
		{
			return FALSE;
		}
	}
    return TRUE;
}   //	WriteSector

BOOLEAN IsSameBytesSector(IN UCHAR *pBuf)
{
	int i;
	BOOLEAN  bResult = TRUE;
	for (i = 1; i < SECTOR_SIZE;i++)
	{
		if (pBuf[i] != pBuf[0])
		{
			bResult = FALSE;
			break;
		}
	}
	return bResult;
}

ULONG FastUpdateFSData(IN PDEVICE_EXTENSION pDeviceExt,IN ULONG *pBitmap,IN ULONG dwLen)
{
    PVOID	pObj,pNextObj;
    ULONG	dwIndex,dwSector,dwNextIndex,dwNextSector;
    ULONG	dwFinalIndex;
    ULONG	dwMask,dwBase,dwCeiling;

    dwMask = 0x01;
	dwBase = 0;
	dwCeiling = pDeviceExt->ulMaxSecAddr;

    ASSERT(pDeviceExt->hHash);
	pObj = GetNextObjEx(pDeviceExt->hHash,NULL,&dwIndex,&dwSector);
    while (pObj)
	{
		dwNextIndex = dwIndex;
		dwNextSector = dwSector;
		pNextObj = GetNextObjEx(pDeviceExt->hHash,pObj,&dwNextIndex,&dwNextSector);
		if (dwSector >= dwBase && dwSector < dwCeiling)
		{
			dwFinalIndex = dwSector-dwBase;
			if (dwFinalIndex/8 < dwLen)
			{
				if (pBitmap[dwFinalIndex/32] & (dwMask << (dwFinalIndex%32)))
				{
					RemoveObj(pDeviceExt->hHash,pObj,dwIndex);
					DeallocateObj(pDeviceExt->hHash,pObj,dwIndex);
					pDeviceExt->ulSysSectors--;
				}
			}
		}
		pObj = pNextObj;
		dwIndex = dwNextIndex;
		dwSector = dwNextSector;
    }
    return  pDeviceExt->ulSysSectors;
}   //	FastUpdateFSData

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
