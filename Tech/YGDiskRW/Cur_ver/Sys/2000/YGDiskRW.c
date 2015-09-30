/*++
Copyright (C) Microsoft Corporation, 1991 - 1999

Module Name:

    YGDiskRW.c

Abstract:

    This driver monitors disk accesses capturing performance data.

Environment:

    kernel mode only

Notes:

--*/

#include "ntddk.h"
#include "ntdddisk.h"
#include "stdarg.h"
#include "stdio.h"
#include <ntddvol.h>
#include "..\..\Export\YGDiskRW.h"

#ifndef SECTOR_SIZE
#define SECTOR_SIZE 512
#endif

#ifdef POOL_TAGGING
#ifdef ExAllocatePool
#undef ExAllocatePool
#endif
#define ExAllocatePool(a,b) ExAllocatePoolWithTag(a,b,'frPD')
#endif

//
// Device Extension
//

typedef struct _DEVICE_EXTENSION {

    //
    // Back pointer to device object
    //

    PDEVICE_OBJECT DeviceObject;

    //
    // Target Device Object
    //

    PDEVICE_OBJECT TargetDeviceObject;

    //
    // Physical device object
    //
    PDEVICE_OBJECT PhysicalDeviceObject;

    //
    // Disk number for reference in WMI
    //

    ULONG       DiskNumber;

    //
    // must synchronize paging path notifications
    //
    KEVENT PagingPathCountEvent;
    LONG  PagingPathCount;

    LIST_ENTRY              list_head;
    KSPIN_LOCK              list_lock;
    KEVENT                  request_event;
    PVOID                   thread_pointer;
    BOOLEAN                 terminate_thread;
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

#define DEVICE_EXTENSION_SIZE sizeof(DEVICE_EXTENSION)


UNICODE_STRING YGDiskRWRegistryPath;


//
// Function declarations
//

DRIVER_INITIALIZE DriverEntry;

DRIVER_ADD_DEVICE YGDiskRWAddDevice;

DRIVER_DISPATCH YGDiskRWForwardIrpSynchronous;
DRIVER_DISPATCH YGDiskRWDispatchPnp;
DRIVER_DISPATCH YGDiskRWDispatchPower;
DRIVER_DISPATCH YGDiskRWSendToNextDriver;
DRIVER_DISPATCH YGDiskRWCreate;
DRIVER_DISPATCH YGDiskRWDeviceControl;
DRIVER_DISPATCH YGDiskRWShutdownFlush;

DRIVER_DISPATCH YGDiskRWStartDevice;
DRIVER_DISPATCH YGDiskRWRemoveDevice;

IO_COMPLETION_ROUTINE YGDiskRWIrpCompletion;

DRIVER_UNLOAD YGDiskRWUnload;

VOID
YGDiskRWSyncFilterWithTarget(
    IN PDEVICE_OBJECT FilterDevice,
    IN PDEVICE_OBJECT TargetDevice
    );

NTSTATUS
YGDiskRWCreateThread(
    IN PDEVICE_OBJECT DeviceObject
);

NTSTATUS
YGDiskRWDeleteThread (
    IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
YGDiskRWSyncReadWriteSec(
    IN PDEVICE_OBJECT DeviceObject,
	IN ULONG ulStartSec,
	IN ULONG ulSectors,
	IN PVOID pBuffer,
    IN UCHAR MajorFunction
);

VOID
YGDiskRWThread (
    IN PVOID Context
    );

#if DBG

ULONG YGDiskRWDebug = 0;

VOID
YGDiskRWDebugPrint(
    ULONG DebugPrintLevel,
    PCCHAR DebugMessage,
    ...
    );

#define DebugPrint(x)   YGDiskRWDebugPrint x

#else

#define DebugPrint(x)

#endif

//
// Define the sections that allow for discarding (i.e. paging) some of
// the code.
//

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, YGDiskRWCreate)
#pragma alloc_text (PAGE, YGDiskRWAddDevice)
#pragma alloc_text (PAGE, YGDiskRWDispatchPnp)
#pragma alloc_text (PAGE, YGDiskRWStartDevice)
#pragma alloc_text (PAGE, YGDiskRWRemoveDevice)
#pragma alloc_text (PAGE, YGDiskRWUnload)
#pragma alloc_text (PAGE, YGDiskRWSyncFilterWithTarget)
#endif


#define YGDiskRWGetClock(a, b) KeQuerySystemTime(&(a))


NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )

/*++

Routine Description:

    Installable driver initialization entry point.
    This entry point is called directly by the I/O manager to set up the disk
    performance driver. The driver object is set up and then the Pnp manager
    calls YGDiskRWAddDevice to attach to the boot devices.

Arguments:

    DriverObject - The disk performance driver object.

    RegistryPath - pointer to a unicode string representing the path,
                   to driver-specific key in the registry.

Return Value:

    STATUS_SUCCESS if successful

--*/

{

    ULONG               ulIndex;
    PDRIVER_DISPATCH  * dispatch;

    //
    // Remember registry path
    //

    YGDiskRWRegistryPath.MaximumLength = RegistryPath->Length
                                            + sizeof(UNICODE_NULL);
    YGDiskRWRegistryPath.Buffer = ExAllocatePool(
                                    PagedPool,
                                    YGDiskRWRegistryPath.MaximumLength);
    if (YGDiskRWRegistryPath.Buffer != NULL)
    {
        RtlCopyUnicodeString(&YGDiskRWRegistryPath, RegistryPath);
    } else {
        YGDiskRWRegistryPath.Length = 0;
        YGDiskRWRegistryPath.MaximumLength = 0;
    }

    //
    // Create dispatch points
    //
    for (ulIndex = 0, dispatch = DriverObject->MajorFunction;
         ulIndex <= IRP_MJ_MAXIMUM_FUNCTION;
         ulIndex++, dispatch++) {

        *dispatch = YGDiskRWSendToNextDriver;
    }

    //
    // Set up the device driver entry points.
    //

    DriverObject->MajorFunction[IRP_MJ_CREATE]          = YGDiskRWCreate;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL]  = YGDiskRWDeviceControl;

    DriverObject->MajorFunction[IRP_MJ_SHUTDOWN]        = YGDiskRWShutdownFlush;
    DriverObject->MajorFunction[IRP_MJ_FLUSH_BUFFERS]   = YGDiskRWShutdownFlush;
    DriverObject->MajorFunction[IRP_MJ_PNP]             = YGDiskRWDispatchPnp;
    DriverObject->MajorFunction[IRP_MJ_POWER]           = YGDiskRWDispatchPower;

    DriverObject->DriverExtension->AddDevice            = YGDiskRWAddDevice;
    DriverObject->DriverUnload                          = YGDiskRWUnload;

    return(STATUS_SUCCESS);

} // end DriverEntry()

#define FILTER_DEVICE_PROPOGATE_FLAGS            0
#define FILTER_DEVICE_PROPOGATE_CHARACTERISTICS (FILE_REMOVABLE_MEDIA |  \
                                                 FILE_READ_ONLY_DEVICE | \
                                                 FILE_FLOPPY_DISKETTE    \
                                                 )

VOID
YGDiskRWSyncFilterWithTarget(
    IN PDEVICE_OBJECT FilterDevice,
    IN PDEVICE_OBJECT TargetDevice
    )
{
    ULONG                   propFlags;

    PAGED_CODE();

    //
    // Propogate all useful flags from target to YGDiskRW. MountMgr will look
    // at the YGDiskRW object capabilities to figure out if the disk is
    // a removable and perhaps other things.
    //
    propFlags = TargetDevice->Flags & FILTER_DEVICE_PROPOGATE_FLAGS;
    FilterDevice->Flags |= propFlags;

    propFlags = TargetDevice->Characteristics & FILTER_DEVICE_PROPOGATE_CHARACTERISTICS;
    FilterDevice->Characteristics |= propFlags;


}

NTSTATUS
YGDiskRWAddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject
    )
/*++
Routine Description:

    Creates and initializes a new filter device object FiDO for the
    corresponding PDO.  Then it attaches the device object to the device
    stack of the drivers for the device.

Arguments:

    DriverObject - Disk performance driver object.
    PhysicalDeviceObject - Physical Device Object from the underlying layered driver

Return Value:

    NTSTATUS
--*/

{
    NTSTATUS                status;
    PDEVICE_OBJECT          filterDeviceObject;
    PDEVICE_EXTENSION       deviceExtension;
    PCHAR                   buffer;
    ULONG                   buffersize;

    PAGED_CODE();

    //
    // Create a filter device object for this device (partition).
    //

    DebugPrint((2, "YGDiskRWAddDevice: Driver %X Device %X\n",
            DriverObject, PhysicalDeviceObject));

    status = IoCreateDevice(DriverObject,
                            DEVICE_EXTENSION_SIZE,
                            NULL,
                            FILE_DEVICE_DISK,
                            FILE_DEVICE_SECURE_OPEN,
                            FALSE,
                            &filterDeviceObject);

    if (!NT_SUCCESS(status)) {
       DebugPrint((1, "YGDiskRWAddDevice: Cannot create filterDeviceObject\n"));
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

    deviceExtension->PhysicalDeviceObject = PhysicalDeviceObject;

    deviceExtension->TargetDeviceObject =
        IoAttachDeviceToDeviceStack(filterDeviceObject, PhysicalDeviceObject);

    if (deviceExtension->TargetDeviceObject == NULL) {
        IoDeleteDevice(filterDeviceObject);
        DebugPrint((1, "YGDiskRWAddDevice: Unable to attach %X to target %X\n",
            filterDeviceObject, PhysicalDeviceObject));
        return STATUS_NO_SUCH_DEVICE;
    }

    //
    // Save the filter device object in the device extension
    //
    deviceExtension->DeviceObject = filterDeviceObject;

    KeInitializeEvent(&deviceExtension->PagingPathCountEvent,
                      NotificationEvent, TRUE);


	YGDiskRWCreateThread(filterDeviceObject);

    //
    // default to DO_POWER_PAGABLE
    //

    filterDeviceObject->Flags |=  DO_POWER_PAGABLE;

    //
    // Clear the DO_DEVICE_INITIALIZING flag
    //

    filterDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

    return STATUS_SUCCESS;

} // end YGDiskRWAddDevice()


NTSTATUS
YGDiskRWDispatchPnp(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    Dispatch for PNP

Arguments:

    DeviceObject    - Supplies the device object.

    Irp             - Supplies the I/O request packet.

Return Value:

    NTSTATUS

--*/

{
    PIO_STACK_LOCATION  irpSp = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS            status;
    PDEVICE_EXTENSION   deviceExtension;

    PAGED_CODE();

    DebugPrint((2, "YGDiskRWDispatchPnp: Device %X Irp %X\n",
        DeviceObject, Irp));

    switch(irpSp->MinorFunction) {

        case IRP_MN_START_DEVICE:
            //
            // Call the Start Routine handler to schedule a completion routine
            //
            DebugPrint((3,
               "YGDiskRWDispatchPnp: Schedule completion for START_DEVICE"));
            status = YGDiskRWStartDevice(DeviceObject, Irp);
            break;

        case IRP_MN_REMOVE_DEVICE:
        {
            //
            // Call the Remove Routine handler to schedule a completion routine
            //
            DebugPrint((3,
               "YGDiskRWDispatchPnp: Schedule completion for REMOVE_DEVICE"));
            status = YGDiskRWRemoveDevice(DeviceObject, Irp);
            break;
        }
        case IRP_MN_DEVICE_USAGE_NOTIFICATION:
        {
            PIO_STACK_LOCATION irpStack;
            BOOLEAN setPagable;

            DebugPrint((3,
               "YGDiskRWDispatchPnp: Processing DEVICE_USAGE_NOTIFICATION"));
            irpStack = IoGetCurrentIrpStackLocation(Irp);

            if (irpStack->Parameters.UsageNotification.Type != DeviceUsageTypePaging) {
                status = YGDiskRWSendToNextDriver(DeviceObject, Irp);
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
                deviceExtension->PagingPathCount == 1 ) {

                //
                // removing the last paging file
                // must have DO_POWER_PAGABLE bits set
                //

                if (DeviceObject->Flags & DO_POWER_INRUSH) {
                    DebugPrint((3, "YGDiskRWDispatchPnp: last paging file "
                                "removed but DO_POWER_INRUSH set, so not "
                                "setting PAGABLE bit "
                                "for DO %p\n", DeviceObject));
                } else {
                    DebugPrint((2, "YGDiskRWDispatchPnp: Setting  PAGABLE "
                                "bit for DO %p\n", DeviceObject));
                    DeviceObject->Flags |= DO_POWER_PAGABLE;
                    setPagable = TRUE;
                }

            }

            //
            // send the irp synchronously
            //

            status = YGDiskRWForwardIrpSynchronous(DeviceObject, Irp);

            //
            // now deal with the failure and success cases.
            // note that we are not allowed to fail the irp
            // once it is sent to the lower drivers.
            //

            if (NT_SUCCESS(status)) {

                IoAdjustPagingPathCount(
                    &deviceExtension->PagingPathCount,
                    irpStack->Parameters.UsageNotification.InPath);

                if (irpStack->Parameters.UsageNotification.InPath) {
                    if (deviceExtension->PagingPathCount == 1) {

                        //
                        // first paging file addition
                        //

                        DebugPrint((3, "YGDiskRWDispatchPnp: Clearing PAGABLE bit "
                                    "for DO %p\n", DeviceObject));
                        DeviceObject->Flags &= ~DO_POWER_PAGABLE;
                    }
                }

            } else {

                //
                // cleanup the changes done above
                //

                if (setPagable == TRUE) {
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

        default:
            DebugPrint((3,
               "YGDiskRWDispatchPnp: Forwarding irp"));
            //
            // Simply forward all other Irps
            //
            return YGDiskRWSendToNextDriver(DeviceObject, Irp);

    }

    return status;

} // end YGDiskRWDispatchPnp()


NTSTATUS
YGDiskRWIrpCompletion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    )

/*++

Routine Description:

    Forwarded IRP completion routine. Set an event and return
    STATUS_MORE_PROCESSING_REQUIRED. Irp forwarder will wait on this
    event and then re-complete the irp after cleaning up.

Arguments:

    DeviceObject is the device object of the WMI driver
    Irp is the WMI irp that was just completed
    Context is a PKEVENT that forwarder will wait on

Return Value:

    STATUS_MORE_PORCESSING_REQUIRED

--*/

{
    PKEVENT Event = (PKEVENT) Context;

    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);

    KeSetEvent(Event, IO_NO_INCREMENT, FALSE);

    return(STATUS_MORE_PROCESSING_REQUIRED);

} // end YGDiskRWIrpCompletion()


NTSTATUS
YGDiskRWStartDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine is called when a Pnp Start Irp is received.
    It will schedule a completion routine to initialize and register with WMI.

Arguments:

    DeviceObject - a pointer to the device object

    Irp - a pointer to the irp


Return Value:

    Status of processing the Start Irp

--*/
{
    PDEVICE_EXTENSION   deviceExtension;
    NTSTATUS            status;

    PAGED_CODE();

    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    status = YGDiskRWForwardIrpSynchronous(DeviceObject, Irp);

    YGDiskRWSyncFilterWithTarget(DeviceObject,
                                 deviceExtension->TargetDeviceObject);

    //
    // Complete the Irp
    //
    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}


NTSTATUS
YGDiskRWRemoveDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine is called when the device is to be removed.
    It will de-register itself from WMI first, detach itself from the
    stack before deleting itself.

Arguments:

    DeviceObject - a pointer to the device object

    Irp - a pointer to the irp


Return Value:

    Status of removing the device

--*/
{
    NTSTATUS            status;
    PDEVICE_EXTENSION   deviceExtension;

    PAGED_CODE();

    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    //
    // quickly zero out the count first to invalid the structure
    //

    status = YGDiskRWForwardIrpSynchronous(DeviceObject, Irp);

    IoDetachDevice(deviceExtension->TargetDeviceObject);
	YGDiskRWDeleteThread(DeviceObject);
    IoDeleteDevice(DeviceObject);

    //
    // Complete the Irp
    //
    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}


NTSTATUS
YGDiskRWSendToNextDriver(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine sends the Irp to the next driver in line
    when the Irp is not processed by this driver.

Arguments:

    DeviceObject
    Irp

Return Value:

    NTSTATUS

--*/

{
    PDEVICE_EXTENSION   deviceExtension;

    IoSkipCurrentIrpStackLocation(Irp);
    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    return IoCallDriver(deviceExtension->TargetDeviceObject, Irp);

} // end YGDiskRWSendToNextDriver()

NTSTATUS
YGDiskRWDispatchPower(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    PDEVICE_EXTENSION deviceExtension;


#if (NTDDI_VERSION < NTDDI_LONGHORN)
    PoStartNextPowerIrp(Irp);
    IoSkipCurrentIrpStackLocation(Irp);

    deviceExtension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;
    return PoCallDriver(deviceExtension->TargetDeviceObject, Irp);
#else
    IoSkipCurrentIrpStackLocation(Irp);

    deviceExtension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;
    return IoCallDriver(deviceExtension->TargetDeviceObject, Irp);
#endif

} // end YGDiskRWDispatchPower

NTSTATUS
YGDiskRWForwardIrpSynchronous(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine sends the Irp to the next driver in line
    when the Irp needs to be processed by the lower drivers
    prior to being processed by this one.

Arguments:

    DeviceObject
    Irp

Return Value:

    NTSTATUS

--*/

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

    IoSetCompletionRoutine(Irp, YGDiskRWIrpCompletion,
                            &event, TRUE, TRUE, TRUE);

    //
    // call the next lower device
    //

    status = IoCallDriver(deviceExtension->TargetDeviceObject, Irp);

    //
    // wait for the actual completion
    //

    if (status == STATUS_PENDING) {
        KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
        status = Irp->IoStatus.Status;
    }

    return status;

} // end YGDiskRWForwardIrpSynchronous()


NTSTATUS
YGDiskRWCreate(
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
    PAGED_CODE();

    UNREFERENCED_PARAMETER(DeviceObject);

    Irp->IoStatus.Status = STATUS_SUCCESS;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;

} // end YGDiskRWCreate()

NTSTATUS
YGDiskRWDeviceControl(
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
    PDEVICE_EXTENSION  deviceExtension = DeviceObject->DeviceExtension;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS status = STATUS_SUCCESS;

    DebugPrint((2, "YGDiskRWDeviceControl: DeviceObject %X Irp %X\n",
                    DeviceObject, Irp));


    switch (irpStack->Parameters.DeviceIoControl.IoControlCode)
    {
	case IOCTL_YGDISKRW_GET_VERSION:
		if (irpStack->Parameters.DeviceIoControl.OutputBufferLength != sizeof(ULONG))
		{
			status = STATUS_INVALID_PARAMETER;
			Irp->IoStatus.Information = 0;
		}
		else
		{
			*((ULONG *)Irp->AssociatedIrp.SystemBuffer) = YGDISKRW_VERSION_MAJOR << 8 | YGDISKRW_VERSION_MINOR;
			Irp->IoStatus.Information = sizeof(ULONG);
		}
		IoCompleteRequest(Irp,IO_NO_INCREMENT);
		return status; 
		break;
	case IOCTL_YGDISKRW_READ_DISK:
	case IOCTL_YGDISKRW_WRITE_DISK:
		if (irpStack->Parameters.DeviceIoControl.InputBufferLength != sizeof(YGDISKRWPARAM) ||
			irpStack->Parameters.DeviceIoControl.OutputBufferLength != sizeof(YGDISKRWPARAM))
		{
			status = STATUS_INVALID_PARAMETER;
			Irp->IoStatus.Information = 0;
			IoCompleteRequest(Irp,IO_NO_INCREMENT);
			return status; 
		}
        Irp->MdlAddress = IoAllocateMdl((PVOID)((PYGDISKRWPARAM)Irp->AssociatedIrp.SystemBuffer)->pBuffer,
										((PYGDISKRWPARAM)Irp->AssociatedIrp.SystemBuffer)->dwLength*SECTOR_SIZE,
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
	case IOCTL_YGDISKRW_READ_DISK_KERNEL:
	case IOCTL_YGDISKRW_WRITE_DISK_KERNEL:
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
		Irp->CurrentLocation++,
		Irp->Tail.Overlay.CurrentStackLocation++;

		//
		// Pass unrecognized device control requests
		// down to next driver layer.
		//

		return IoCallDriver(deviceExtension->TargetDeviceObject, Irp);
	}
} // end YGDiskRWDeviceControl()

NTSTATUS
YGDiskRWShutdownFlush(
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
    PDEVICE_EXTENSION  deviceExtension = DeviceObject->DeviceExtension;

    DebugPrint((2, "YGDiskRWShutdownFlush: DeviceObject %X Irp %X\n",
                    DeviceObject, Irp));

    IoSkipCurrentIrpStackLocation(Irp);
    return IoCallDriver(deviceExtension->TargetDeviceObject, Irp);

} // end YGDiskRWShutdownFlush()


VOID
YGDiskRWUnload(
    IN PDRIVER_OBJECT DriverObject
    )

/*++

Routine Description:

    Free all the allocated resources, etc.

Arguments:

    DriverObject - pointer to a driver object.

Return Value:

    VOID.

--*/
{
    PAGED_CODE();

    UNREFERENCED_PARAMETER(DriverObject);

    return;
}

VOID
YGDiskRWThread (
    IN PVOID Context
    )
{
    PDEVICE_OBJECT      DeviceObject;
    PDEVICE_EXTENSION   deviceExtension;
    PLIST_ENTRY         request;
    PIRP                Irp;
    PIO_STACK_LOCATION  currentIrpStack;
    PIO_STACK_LOCATION  nextIrpStack;
	ULONG				dwStartSec;
	ULONG				dwLength;
	PVOID				pBuffer;
	NTSTATUS			status;
	PYGDISKRWPARAM	    pReadWriteBuffer;

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
            case IRP_MJ_DEVICE_CONTROL:
                switch (currentIrpStack->Parameters.DeviceIoControl.IoControlCode)
                {
				case IOCTL_YGDISKRW_READ_DISK:
					pReadWriteBuffer = (PYGDISKRWPARAM)Irp->AssociatedIrp.SystemBuffer;
					dwLength		 = pReadWriteBuffer->dwLength;
					pBuffer			 = MmGetSystemAddressForMdlSafe(Irp->MdlAddress,NormalPagePriority);

					YGDiskRWSyncReadWriteSec(DeviceObject,pReadWriteBuffer->dwStartSec ,
									dwLength ,pBuffer ,IRP_MJ_READ);
					MmUnlockPages(Irp->MdlAddress);
					IoFreeMdl(Irp->MdlAddress);
					Irp->MdlAddress = NULL;
			        Irp->IoStatus.Status	  = STATUS_SUCCESS;
					Irp->IoStatus.Information = sizeof(YGDISKRWPARAM);
                    break;
				case IOCTL_YGDISKRW_WRITE_DISK:
					pReadWriteBuffer = (PYGDISKRWPARAM)Irp->AssociatedIrp.SystemBuffer;
					dwLength		 = pReadWriteBuffer->dwLength;
					pBuffer			 = MmGetSystemAddressForMdlSafe(Irp->MdlAddress,NormalPagePriority);

					YGDiskRWSyncReadWriteSec(DeviceObject,pReadWriteBuffer->dwStartSec ,
									dwLength ,pBuffer ,IRP_MJ_WRITE);
					MmUnlockPages(Irp->MdlAddress);
					IoFreeMdl(Irp->MdlAddress);
					Irp->MdlAddress = NULL;

			        Irp->IoStatus.Status	  = STATUS_SUCCESS;
					Irp->IoStatus.Information = sizeof(YGDISKRWPARAM);
                    break;
				case IOCTL_YGDISKRW_READ_DISK_KERNEL:
					pReadWriteBuffer = (PYGDISKRWPARAM)Irp->AssociatedIrp.SystemBuffer;
					dwLength		 = pReadWriteBuffer->dwLength;
					pBuffer			 = pReadWriteBuffer->pBuffer;

					YGDiskRWSyncReadWriteSec(DeviceObject,pReadWriteBuffer->dwStartSec,dwLength ,pBuffer ,IRP_MJ_READ);

			        Irp->IoStatus.Status	  = STATUS_SUCCESS;
					Irp->IoStatus.Information = sizeof(YGDISKRWPARAM);
                    break;
				case IOCTL_YGDISKRW_WRITE_DISK_KERNEL:
					pReadWriteBuffer = (PYGDISKRWPARAM)Irp->AssociatedIrp.SystemBuffer;
					dwLength		 = pReadWriteBuffer->dwLength;
					pBuffer			 = pReadWriteBuffer->pBuffer;

					YGDiskRWSyncReadWriteSec(DeviceObject,pReadWriteBuffer->dwStartSec,dwLength ,pBuffer ,IRP_MJ_WRITE);

			        Irp->IoStatus.Status	  = STATUS_SUCCESS;
					Irp->IoStatus.Information = sizeof(YGDISKRWPARAM);
                    break;
                default:
                    Irp->IoStatus.Status = STATUS_DRIVER_INTERNAL_ERROR;
                    Irp->IoStatus.Information = 0;
                }
                break;
            default:
                DbgPrint("YGDiskRWer:Thread default conditions.");

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
YGDiskRWCreateThread(
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

	KeInitializeEvent(&deviceExtension->request_event,
					  SynchronizationEvent,FALSE);
    InitializeListHead(&deviceExtension->list_head);

    KeInitializeSpinLock(&deviceExtension->list_lock);

    deviceExtension->terminate_thread = FALSE;

    status = PsCreateSystemThread(
        &thread_handle,
        (ACCESS_MASK) 0L,
        NULL,
        NULL,
        NULL,
        YGDiskRWThread,
        DeviceObject
        );

    if (!NT_SUCCESS(status))
    {
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
YGDiskRWDeleteThread (
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

NTSTATUS
YGDiskRWSyncReadWriteSec(
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

#if DBG

VOID
YGDiskRWDebugPrint(
    ULONG DebugPrintLevel,
    PCCHAR DebugMessage,
    ...
    )

/*++

Routine Description:

    Debug print for all YGDiskRW

Arguments:

    Debug print level between 0 and 3, with 3 being the most verbose.

Return Value:

    None

--*/

{
    va_list ap;

    va_start(ap, DebugMessage);


    if ((DebugPrintLevel <= (YGDiskRWDebug & 0x0000ffff)) ||
        ((1 << (DebugPrintLevel + 15)) & YGDiskRWDebug)) {

        DbgPrint(DebugMessage, ap);
    }

    va_end(ap);

}
#endif


