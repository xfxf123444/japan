#ifndef PARMAN_H
#define PARMAN_H

#ifndef EXPORT
    #define EXPORT  __declspec(dllexport)
#endif

#define ERR_PARMAN_OPENVXD	101
#define ERR_PARMAN_GETDRIVEPARAM 102
#define ERR_PARMAN_READSECTOR 103
#define ERR_PARMAN_WRITESECTOR 104
#define ERR_PARMAN_NUMBEROFSECTORS 105
#define ERR_PARMAN_GETPARTITIONINFO 106
#define ERR_PARMAN_EXTENDPARTITIONSTARTSECTOR 107
#define ERR_PARMAN_MBRPARTITIONFULL 108
#define ERR_PARMAN_PARTITIONINPARTITION 109
#define ERR_PARMAN_FORMATDRIVE 110
#define ERR_PARMAN_PARAM 111
#define ERR_PARMAN_RESIZEEXTENDPARTITION 112
#define ERR_PARMAN_NOFREESPACE 113
#define ERR_PARMAN_CREATEPARTITION  114
extern "C"
{
	BOOL EXPORT	DoDeletePartition(DWORD dwStartSector,BYTE btHardDisk,CREATE_PAR_FLAG flags,PINT);
	BOOL EXPORT DoCreatePartition(PPARTITION_ENTRY peParEntry,BYTE btHardDisk,DWORD dwFlag,BOOL blIsFormat,PBYTE pLabel,HWND hWnd,PINT);
	BOOL EXPORT DoResizeExtendPartition(BYTE btHardDisk,PPARTITION_ENTRY ParEntry,PINT);
//	BOOL EXPORT DoPri2Log(DWORD dwStartSec,BYTE btHardDisk,HWND hWnd);
	BOOL EXPORT DoLog2Pri(DWORD dwStartSec,BYTE btHardDisk,HWND hWnd);
}
#endif