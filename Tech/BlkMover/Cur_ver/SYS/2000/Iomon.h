#ifndef DISKPERF_H
#define DISKPERF_H

#ifndef WINNT
#define WINNT
#endif

#ifdef POOL_TAGGING
#ifdef ExAllocatePool
#undef ExAllocatePool
#endif
#define ExAllocatePool(a,b) ExAllocatePoolWithTag(a,b,'frPD')
#endif

//#define DISKPERF_MAXSTR         64

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

	BOOLEAN			bInitMover;
    //
    // Physical device object
    //
    //
    // must synchronize paging path notifications
    //
    KEVENT PagingPathCountEvent;
    ULONG  PagingPathCount;

    LIST_ENTRY              list_head;
    KSPIN_LOCK              list_lock;
    KEVENT                  request_event;
    PVOID                   thread_pointer;
    BOOLEAN                 terminate_thread;
//	HANDLE					FileHandle;
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

#define DEVICE_EXTENSION_SIZE sizeof(DEVICE_EXTENSION)
#define PROCESSOR_COUNTERS_SIZE FIELD_OFFSET(DISK_PERFORMANCE, QueryTime)

/*
Layout of Per Processor Counters is a contiguous block of memory:
    Processor 1
+-----------------------+     +-----------------------+
|PROCESSOR_COUNTERS_SIZE| ... |PROCESSOR_COUNTERS_SIZE|
+-----------------------+     +-----------------------+
where PROCESSOR_COUNTERS_SIZE is less than sizeof(DISK_PERFORMANCE) since
we only put those we actually use for counting.
*/

UNICODE_STRING DiskPerfRegistryPath;


//
// Function declarations
//

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    );

NTSTATUS
BlkMovForwardIrpSynchronous(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
BlkMovAddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject
    );


NTSTATUS
BlkMovDispatchPnp(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
BlkMovDispatchPower(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
BlkMovStartDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
BlkMovRemoveDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
BlkMovSendToNextDriver(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );


NTSTATUS
BlkMovCreate(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
BlkMovReadWrite(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
FlushBuf(IN PDEVICE_OBJECT DeviceObject);

NTSTATUS
BlkMovIoCompletion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    );

NTSTATUS
BlkMovDeviceControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
BlkMovShutdownFlush(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

VOID
BlkMovUnload(
    IN PDRIVER_OBJECT DriverObject
    );
/*
NTSTATUS
BlkMovRegisterDevice(
    IN PDEVICE_OBJECT DeviceObject
    );
*/
NTSTATUS
DiskPerfIrpCompletion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    );


VOID
BlkMovSyncFilterWithTarget(
    IN PDEVICE_OBJECT FilterDevice,
    IN PDEVICE_OBJECT TargetDevice
    );

VOID
DiskPerfAddCounters(
//    IN OUT PDISK_PERFORMANCE TotalCounters,
//    IN PDISK_PERFORMANCE NewCounters,
    IN LARGE_INTEGER Frequency
    );

NTSTATUS
AsyncReadWriteSec(
    IN PDEVICE_OBJECT DeviceObject,
	IN PIRP	 ParentIrp,
	IN ULONG ulStartSec,
	IN ULONG ulSectors,
	IN PVOID pBuffer,
    IN UCHAR MajorFunction
);

NTSTATUS
AsyncCompletion(IN PDEVICE_OBJECT DeviceObject,
				IN PIRP Irp,
				IN PVOID Context
);

NTSTATUS
SyncReadWriteSec(
    IN PDEVICE_OBJECT DeviceObject,
	IN ULONG ulStartSec,
	IN ULONG ulSectors,
	IN PVOID pBuffer,
    IN UCHAR MajorFunction
);

NTSTATUS
CreateThread(
    IN PDEVICE_OBJECT DeviceObject
);
NTSTATUS
DeleteThread (
    IN PDEVICE_OBJECT DeviceObject
    );
VOID
Thread (
    IN PVOID Context
    );

NTSTATUS
Blk_Set_Drive_Layout_Completion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    );
#if DBG

#define DEBUG_BUFFER_LENGTH 256

#endif

#endif //end DISKPERF_H
