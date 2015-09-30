#ifndef YGVPARMP_H
#define YGVPARMP_H

#ifndef __T
#ifdef _NTDDK_
#define __T(x)      L ## x
#else
#define __T(x)      x
#endif /* _NTDDK_ */
#endif /* __T */

#ifndef _T
#define _T(x)       __T(x)
#endif


#define YGVPARMP_VERSION_MAJOR	    2
#define YGVPARMP_VERSION_MINOR	    0

#ifndef     DWORD
#define     DWORD  ULONG
#endif

#ifndef MAXIMUM_FILENAME_LENGTH
#define MAXIMUM_FILENAME_LENGTH 256
#endif

#ifndef SECTOR_SIZE
#define SECTOR_SIZE   512
#endif

#define MAX_VPARMP_DRIVE          6

#define YGVPARMP_DIR_NAME		_T("\\Device\\YGVParMP")

#define IOCTL_YGVPARMP_GET_VERSION \
	CTL_CODE(FILE_DEVICE_DISK, 0xa00, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_YGVPARMP_OPEN_MEDIA \
	CTL_CODE(FILE_DEVICE_DISK, 0xa01, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_YGVPARMP_CLOSE_MEDIA \
	CTL_CODE(FILE_DEVICE_DISK, 0xa02, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_YGVPARMP_DRIVE_ISUSEFUL \
	CTL_CODE(FILE_DEVICE_DISK, 0xa03, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_YGVPARMP_ENABLE_DISABLE \
	CTL_CODE(FILE_DEVICE_DISK, 0xa04, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_YGVPARMP_FLUSH_MEDIA \
	CTL_CODE(FILE_DEVICE_DISK, 0xa05, METHOD_BUFFERED, FILE_ANY_ACCESS)

#ifndef	DIOC_REG
#define	DIOC_REG

typedef struct DIOCRegs	{
	DWORD	reg_EBX;
	DWORD	reg_EDX;
	DWORD	reg_ECX;
	DWORD	reg_EAX;
	DWORD	reg_EDI;
	DWORD	reg_ESI;
	DWORD	reg_Flags;		
} DIOC_REGISTERS;

#endif

typedef struct _YGVPARMP_PARAMETER {
	UCHAR                   btFileDrive;
	ULONG                   DiskNumber;
	DISK_GEOMETRY           DiskGeometry;
	ULONG                   GeometryStatus;
    PARTITION_INFORMATION   PartitionInformation;
	ULONG                   ParInfoStatus;
	UCHAR					ucDriveLetter;
} YGVPARMP_PARAMETER, *PYGVPARMP_PARAMETER;

#endif /* YGVPARMP_H */
