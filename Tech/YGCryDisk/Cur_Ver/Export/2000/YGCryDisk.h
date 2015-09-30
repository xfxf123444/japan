#ifndef YGCRYDISK_H
#define YGCRYDISK_H

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


#define YGCRYDISK_VERSION_MAJOR	    2
#define YGCRYDISK_VERSION_MINOR	    0

#ifndef     DWORD
#define     DWORD  ULONG
#endif

#define		YGCRYDISK_MARK	'Y'

#define YGCRYDISK_DEVICE_NAME		_T("\\Device\\YGCryDisk")

#ifndef MAXIMUM_FILENAME_LENGTH
#define MAXIMUM_FILENAME_LENGTH 256
#endif

#ifndef SECTOR_SIZE
#define SECTOR_SIZE   512
#endif

#define MAX_CRYDISK_DRIVE          6

#define IOCTL_YGCRYDISK_GET_VERSION \
	CTL_CODE(FILE_DEVICE_DISK, 0xa00, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_YGCRYDISK_OPEN_MEDIA \
	CTL_CODE(FILE_DEVICE_DISK, 0xa01, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_YGCRYDISK_CLOSE_MEDIA \
	CTL_CODE(FILE_DEVICE_DISK, 0xa02, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_YGCRYDISK_DRIVE_ISUSEFUL \
	CTL_CODE(FILE_DEVICE_DISK, 0xa03, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_YGCRYDISK_ENABLE_DISABLE \
	CTL_CODE(FILE_DEVICE_DISK, 0xa04, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_YGCRYDISK_FLUSH_MEDIA \
	CTL_CODE(FILE_DEVICE_DISK, 0xa05, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_YGCRYDISK_GET_MPDRIVE_INFO \
	CTL_CODE(FILE_DEVICE_DISK,0xa06,METHOD_BUFFERED,FILE_ANY_ACCESS)

#define IOCTL_YGCRYDISK_GET_COUNT \
	CTL_CODE(FILE_DEVICE_DISK,0xa07,METHOD_BUFFERED,FILE_ANY_ACCESS) //DIOCRegs.reg_EAX

#define IOCTL_YGCRYDISK_ADD_COUNT \
	CTL_CODE(FILE_DEVICE_DISK,0xa08,METHOD_BUFFERED,FILE_ANY_ACCESS)//NULL

#pragma pack(1)
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

typedef struct _YGCRYDISKREADWRITE{
    ULONG Length;
    LARGE_INTEGER ByteOffset;
	UCHAR	*pBuf;
}YGCRYDISKREADWRITE,*PYGCRYDISKREADWRITE;

/*
typedef struct _YGCRYDISK_BOOTSECTOR 
{
	UCHAR  uUnused1[31];
	UCHAR  uMark;
	UCHAR  uUnused2[476];
	UCHAR  uEncrytType;
	UCHAR  uCheckSum;
	UCHAR  ucKeyLen;
	UCHAR  uEncryptMark;
} YGCRYDISK_BOOTSECTOR,*PYGCRYDISK_BOOTSECTOR;*/

typedef struct _YGCRYDISK_BOOTSECTOR 
{
	UCHAR  uUnused1[31];
	UCHAR  uMark;
	UCHAR  uUnused2[478];
	UCHAR  uEncrytTypeAndKeyLen;
	UCHAR  uCheckSumAndEncryptMark;
//	UCHAR  ucKeyLen;
//	UCHAR  uEncryptMark;
} YGCRYDISK_BOOTSECTOR,*PYGCRYDISK_BOOTSECTOR;

typedef struct _YGCRYDISK_PARAMETER {
	ULONG                   DiskNumber;
	DISK_GEOMETRY           DiskGeometry;
	ULONG                   GeometryStatus;
    PARTITION_INFORMATION   PartitionInformation;
	ULONG                   ParInfoStatus;
	UCHAR		            ucKey[32];
	USHORT					usEncryptType;
	USHORT					usKeyLen;
	UCHAR					ucDriveLetter;
} YGCRYDISK_PARAMETER, *PYGCRYDISK_PARAMETER;

#pragma pack()

#endif /* YGCRYDISK_H */
