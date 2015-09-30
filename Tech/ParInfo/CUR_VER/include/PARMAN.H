

#ifndef EXPORT
    #define EXPORT  __declspec(dllexport)
#endif


extern "C"
{
	BOOL EXPORT	DoDeletePartition(DWORD dwStartSector,BYTE btHardDisk,CREATE_PAR_FLAG flags);
	BOOL EXPORT DoCreatePartition(PPARTITION_ENTRY peParEntry,BYTE btHardDisk,DWORD dwFlag,BOOL blIsFormat,PBYTE pLabel,HWND hWnd);
	BOOL EXPORT DoResizeExtendPartition(BYTE btHardDisk,PPARTITION_ENTRY ParEntry);

	BOOL CaculateStartAndEnd(PPARTITION_ENTRY pEntry,BIOS_DRIVE_PARAM DriveParam,BYTE btHardDisk);
//	BOOL EXPORT DoPri2Log(DWORD dwStartSec,BYTE btHardDisk,HWND hWnd);
	BOOL EXPORT DoLog2Pri(DWORD dwStartSec,BYTE btHardDisk,HWND hWnd);
    BOOL CreateFormat(BYTE btHardDisk,PPARTITION_ENTRY peParEntry,PBYTE pLabel,HWND hWnd);
}
