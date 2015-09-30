#ifndef YGFLOC_H
#define YGFLOC_H

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

#define YGFLOC_VERSION_MAJOR	    2
#define YGFLOC_VERSION_MINOR	    0

#define VOLUME_VIRTUAL_PARTITION  1
#define VOLUME_PARTITION_MAP      2
#define VOLUME_CRYPT_PARTITION    3

#ifndef     DWORD
#define     DWORD  ULONG
#endif

#ifndef     SECTOR_SIZE
#define     SECTOR_SIZE 	    512
#endif

#define SECTORS_OF_CACHE	    0x400
#define TOTAL_SIGN_LEN          504
#define SIGN_LEN				21
#define SIGN_STRING  "YuGuang File Locater."	//  21 bytes
#define YGLOCSIGN               0x5947

#define IOCTL_YGFLOC_GET_VERSION \
	CTL_CODE(FILE_DEVICE_DISK, 0xb00, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_YGFLOC_OPEN_BUFFER \
	CTL_CODE(FILE_DEVICE_DISK, 0xb01, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_YGFLOC_CLOSE_BUFFER \
	CTL_CODE(FILE_DEVICE_DISK, 0xb02, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_YGFLOC_COPY_DATA \
	CTL_CODE(FILE_DEVICE_DISK, 0xb03, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_YGFLOC_SET_SIGN \
	CTL_CODE(FILE_DEVICE_DISK, 0xb04, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_YGFLOC_CHECK_INIT \
	CTL_CODE(FILE_DEVICE_DISK, 0xb05, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct tagSET_SIGNSTRU
{
	DWORD  dwIndex;
	DWORD  dwSecNum;
	DWORD  dwLocSign;
} SET_SIGNSTRU,*PSET_SIGNSTRU;

typedef struct tagLOC_INFO
{
    USHORT   usLocSign;
    USHORT   usSectorBufIndex;
	ULONG    ulSign;
    CHAR    LocSign[TOTAL_SIGN_LEN];
} LOC_INFO,*PLOC_INFO;

typedef struct
{
	LONGLONG llAddressBase;
	USHORT   usAddressNum;
}OPENBUFFER,*POPENBUFFER;

typedef struct
{
	ULONG   ulSectorBufIndex;
	ULONG   ulCopyCount;
	LONGLONG llAddressArray[SECTORS_OF_CACHE];
}COPYDATASTRU,*PCOPYDATASTRU;

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

#endif /* YGFLOC_H */
