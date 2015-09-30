/******************************************************************************
*	FileLoc.CPP    MODULE
*
*	AUTHOR	:   LHB
*
*	Copyright 2001 - Shanghai YuGuang Science & Technology Co. Ltd .
*	All rights reserved.
*
*	Date:	01-Dec-2001
*
******************************************************************************/
#include    <windows.h>
#include    <winioctl.h>
#include    <string.h>
#include    <mbstring.h>
#include    <assert.h>
#include    <stdio.h>
#include    <math.h>
#include	<DBT.h>
#include	<WINUSER.H>
#include    "..\..\Export\2000\fileloc.h"
#include    "FileLoc.h"
#include    "..\..\Export\2000\YGFLoc.h"
#include    "..\..\..\..\VirParti\Cur_ver\Export\2000\YGVPar.h"
#include    "..\..\..\..\YGCryDisk\Cur_ver\Export\2000\YGCryDisk.h"
#include    "..\..\..\..\VFS\Cur_ver\Export\2000\YGVParMP.h"
#include    "..\..\..\..\ParInfo\Cur_Ver\Export\2000\ParInfo.h"

VOLUMEINFORMATION g_VolumeInfo[26] = {0};
LOC_INFO          g_LocInfo[SECTORS_OF_CACHE] = {0};
#define ASSERT	assert

BOOL WINAPI DllMain(HINSTANCE hDLLInst, DWORD fdwReason, LPVOID lpvReserved)
{
	int    i,j;
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
			for (i = 0;i < SECTORS_OF_CACHE;i++)
			{
				g_LocInfo[i].usLocSign = YGLOCSIGN;
				g_LocInfo[i].usSectorBufIndex = 0;
				for (j = 0;j < TOTAL_SIGN_LEN;j+=SIGN_LEN)
				{
					memcpy(&g_LocInfo[i].LocSign[j],SIGN_STRING,SIGN_LEN);
				}
			}
            break;
        case DLL_PROCESS_DETACH:
			for (i = 0;i < 26;i++)
			{
				DiscardVirtualDrive((BYTE)(i+'A'));
			}
			break;
        case DLL_THREAD_ATTACH:
             break;
        case DLL_THREAD_DETACH:
			break;
    }
    return TRUE;
}

BOOL EXPORT	Init_FileLocater(BYTE btDriveLetter)
{
	int     nDrive;
	char    szDeviceName[50];
	DWORD   dwRead;
	OPENBUFFER OpenBufferInfo;
	if (btDriveLetter >= 'a' && btDriveLetter <= 'z') nDrive = (int)(btDriveLetter - 'a');
	else 
	{
		if (btDriveLetter >= 'A' && btDriveLetter <= 'Z') nDrive = (int)(btDriveLetter - 'A');
		else return FALSE;
	}
	if (g_VolumeInfo[nDrive].bInited) return TRUE;

	sprintf(szDeviceName, "\\\\.\\%c:", btDriveLetter);
	g_VolumeInfo[nDrive].hVolume =  CreateFile(szDeviceName,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,
												NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (g_VolumeInfo[nDrive].hVolume == INVALID_HANDLE_VALUE) return FALSE; 

	OpenBufferInfo.usAddressNum = SECTORS_OF_CACHE;
	OpenBufferInfo.llAddressBase = g_VolumeInfo[nDrive].llPartitionStart;
	if (!DeviceIoControl(g_VolumeInfo[nDrive].hVolume,IOCTL_YGFLOC_OPEN_BUFFER,
						&OpenBufferInfo,sizeof(OPENBUFFER),NULL,0,&dwRead,NULL))
		return FALSE;

	g_VolumeInfo[nDrive].dwLockSign = 0x01;
	g_VolumeInfo[nDrive].bInited = TRUE;
    return  TRUE;
}   //	Init_FileLocater

BOOL EXPORT	Free_FileLocater(BYTE btDriveLetter)
{
	int     nDrive,i=0;
	DWORD   dwRead;

	if (btDriveLetter >= 'a' && btDriveLetter <= 'z') nDrive = (int)(btDriveLetter - 'a');
	else 
	{
		if (btDriveLetter >= 'A' && btDriveLetter <= 'Z') nDrive = (int)(btDriveLetter - 'A');
		else return FALSE;
	}
	if (!g_VolumeInfo[nDrive].bInited) return TRUE;

	while (g_VolumeInfo[nDrive].pFileListHead)
		Close_FL_Handle(g_VolumeInfo[nDrive].pFileListHead);

	if (!g_VolumeInfo[nDrive].hVolume || g_VolumeInfo[nDrive].hVolume == INVALID_HANDLE_VALUE) return TRUE;

	if (!DeviceIoControl(g_VolumeInfo[nDrive].hVolume,IOCTL_YGFLOC_CLOSE_BUFFER,
						 NULL,0,NULL,0,&dwRead,NULL)) return FALSE;

	CloseHandle(g_VolumeInfo[nDrive].hVolume);
	g_VolumeInfo[nDrive].bInited = FALSE;
    return  TRUE;
}   //	Free_FileLocater

BOOL EXPORT CopyData(BYTE btDriveLetter,LONGLONG *pTar, DWORD dwIndexStart, DWORD dwCount)
{
	DWORD  dwIndex,dwReaded;
	int    nDrive;
	COPYDATASTRU CopyDataInfo;
    dwIndex = 0;
	if (btDriveLetter >= 'a' && btDriveLetter <= 'z') nDrive = (int)(btDriveLetter - 'a');
	else 
	{
		if (btDriveLetter >= 'A' && btDriveLetter <= 'Z') nDrive = (int)(btDriveLetter - 'A');
		else return FALSE;
	}
	if (!g_VolumeInfo[nDrive].bInited) return FALSE;
	CopyDataInfo.ulSectorBufIndex = dwIndexStart;
    do
    {
		if (dwCount > SECTORS_OF_CACHE)
			CopyDataInfo.ulCopyCount = SECTORS_OF_CACHE;
		else
			CopyDataInfo.ulCopyCount = dwCount;
		if (CopyDataInfo.ulCopyCount)
		{
			if (!DeviceIoControl(g_VolumeInfo[nDrive].hVolume,IOCTL_YGFLOC_COPY_DATA,
								&CopyDataInfo,sizeof(COPYDATASTRU),&CopyDataInfo,sizeof(COPYDATASTRU),&dwReaded,NULL))
				return FALSE;
			memcpy(&pTar[dwIndex],CopyDataInfo.llAddressArray,CopyDataInfo.ulCopyCount*sizeof(LONGLONG));
			dwIndex += CopyDataInfo.ulCopyCount;
			CopyDataInfo.ulSectorBufIndex += CopyDataInfo.ulCopyCount;
			dwCount -= CopyDataInfo.ulCopyCount;
		}
    } while (dwCount > 0);
    return  TRUE;
}

HANDLE	EXPORT	Open_FL_Handle(LPCSTR	lpszPathName)
{
    PFILE_LOC_HANDLE	hHandle;
	int                 nDrive;
	ULONG               ulFileSize,dwReaded;

	if (lpszPathName[0] >= 'a' && lpszPathName[0] <= 'z') nDrive = (int)(lpszPathName[0] - 'a');
	else 
	{
		if (lpszPathName[0] >= 'A' && lpszPathName[0] <= 'Z') nDrive = (int)(lpszPathName[0] - 'A');
		else return INVALID_HANDLE_VALUE;
	}
	if (!g_VolumeInfo[nDrive].bInited) return INVALID_HANDLE_VALUE;
    hHandle = (PFILE_LOC_HANDLE)malloc(sizeof(FILE_LOC_HANDLE));
	if (hHandle)
	{
		strcpy(hHandle->szFile,lpszPathName);
		if (GetFileAttributesEx(lpszPathName,GetFileExInfoStandard,&hHandle->FileAttrib))
		{
			if (hHandle->FileAttrib.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
				SetFileAttributes(lpszPathName,FILE_ATTRIBUTE_NORMAL);
			hHandle->hFile = CreateFile(lpszPathName, GENERIC_WRITE|GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
	           							  OPEN_EXISTING,0, 0);
			if (hHandle->hFile != INVALID_HANDLE_VALUE)
			{
				hHandle->wFixEnd = (WORD)(hHandle->FileAttrib.nFileSizeLow % SECTOR_SIZE);
				if (hHandle->wFixEnd == 1 || hHandle->wFixEnd == 2)
				{
					SetFilePointer(hHandle->hFile,((ULONG)(hHandle->FileAttrib.nFileSizeLow/SECTOR_SIZE))*SECTOR_SIZE,(LONG*)&hHandle->FileAttrib.nFileSizeHigh,FILE_BEGIN);
					ReadFile(hHandle->hFile,&hHandle->btFileEnd,hHandle->wFixEnd,&dwReaded,NULL);
				}
				else hHandle->wFixEnd = 0;
				ulFileSize = (hHandle->FileAttrib.nFileSizeLow+SECTOR_SIZE-1)/SECTOR_SIZE;
				ulFileSize *= SECTOR_SIZE;
				SetFilePointer(hHandle->hFile,ulFileSize,(LONG*)&hHandle->FileAttrib.nFileSizeHigh,FILE_BEGIN);
				SetEndOfFile(hHandle->hFile);
				if (SaveFLHandle(hHandle))	return (HANDLE)hHandle;
			}
		}
		free(hHandle);
    }
    return  INVALID_HANDLE_VALUE;
}   //	Open_FL_Handle

BOOL	EXPORT FL_LogicSector2PhysSector(HANDLE hFile,LONGLONG *pdAddrArr,DWORD dwLogicalSec,DWORD dwSectors)
{
    PFILE_LOC_HANDLE	hHandle;
	DWORD        dwMoveHi=0; 
	SET_SIGNSTRU SetSignInfo;
	int         nDrive;
    DWORD		dwWritten;
    DWORD		dwActualSectors;
    DWORD		dwIndex,dwCount;

	if (hFile == NULL || hFile == INVALID_HANDLE_VALUE) return FALSE;
	dwIndex = 0;
    hHandle = (PFILE_LOC_HANDLE)hFile;
	if (hHandle->szFile[0] >= 'a' && hHandle->szFile[0] <= 'z') nDrive = (int)(hHandle->szFile[0] - 'a');
	else nDrive = (int)(hHandle->szFile[0] - 'A');

	if (dwLogicalSec*SECTOR_SIZE > hHandle->FileAttrib.nFileSizeLow) return FALSE;

    do
    {
		if (dwSectors > SECTORS_OF_CACHE)
			dwActualSectors = SECTORS_OF_CACHE;
		else
			dwActualSectors = dwSectors;
		if (dwActualSectors)
		{
			SetSignInfo.dwIndex = 0;
			SetSignInfo.dwLocSign = g_VolumeInfo[nDrive].dwLockSign+1;
			SetSignInfo.dwSecNum = dwActualSectors;
			if (DeviceIoControl(g_VolumeInfo[nDrive].hVolume,IOCTL_YGFLOC_SET_SIGN,&SetSignInfo,sizeof(SET_SIGNSTRU),NULL,0,&dwWritten,NULL))
				g_VolumeInfo[nDrive].dwLockSign ++;
			for (dwCount = 0; dwCount < dwActualSectors; dwCount++)
			{
				g_LocInfo[dwCount].usSectorBufIndex = (WORD)dwCount;
				g_LocInfo[dwCount].ulSign = SetSignInfo.dwLocSign;
			}
			SetFilePointer(hHandle->hFile,dwLogicalSec*SECTOR_SIZE,(LONG*)&dwMoveHi,FILE_BEGIN);
			if (!WriteFile(hHandle->hFile,(PBYTE)g_LocInfo,SECTOR_SIZE*dwActualSectors,&dwWritten,NULL))
			{
				DWORD dwLastErr = GetLastError();
				return	FALSE;
			}
			FlushFileBuffers(hHandle->hFile);
			CopyData(hHandle->szFile[0],&pdAddrArr[dwIndex],0,dwActualSectors);

			dwIndex += dwActualSectors;
			dwLogicalSec += dwActualSectors;
			dwSectors -= dwActualSectors;
		}
    } while (dwSectors > 0);
    return  TRUE;
}

HANDLE  EXPORT   FL_Create_File(LPCSTR	lpszPathName,WIN32_FILE_ATTRIBUTE_DATA *pFileAttrib)
{
    PFILE_LOC_HANDLE	hHandle;
	int                 nDrive;

	if (lpszPathName[0] >= 'a' && lpszPathName[0] <= 'z') nDrive = (int)(lpszPathName[0] - 'a');
	else 
	{
		if (lpszPathName[0] >= 'A' && lpszPathName[0] <= 'Z') nDrive = (int)(lpszPathName[0] - 'A');
		else return NULL;
	}
	if (!g_VolumeInfo[nDrive].bInited) return NULL;
    hHandle = (PFILE_LOC_HANDLE)malloc(sizeof(FILE_LOC_HANDLE));
	if (hHandle)
	{
		strcpy(hHandle->szFile,lpszPathName);
		hHandle->hFile = CreateFile(lpszPathName, GENERIC_WRITE|GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
           							CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
		if (hHandle->hFile != INVALID_HANDLE_VALUE)
		{
			hHandle->wFixEnd = 0;
			memcpy(&hHandle->FileAttrib,pFileAttrib,sizeof(WIN32_FILE_ATTRIBUTE_DATA));
			if (SaveFLHandle(hHandle))	return (HANDLE)hHandle;
		}
		free(hHandle);
	}
	return INVALID_HANDLE_VALUE;
}

BOOL	EXPORT Close_FL_Handle(HANDLE hFile)
{
	PFILE_LOC_HANDLE	hHandle;
	DWORD dwWrited;
	if (hFile == NULL || hFile == INVALID_HANDLE_VALUE) return FALSE;
	hHandle = (PFILE_LOC_HANDLE)hFile;
	if (hHandle->wFixEnd)
	{
		SetFilePointer(hHandle->hFile,((ULONG)(hHandle->FileAttrib.nFileSizeLow/SECTOR_SIZE))*SECTOR_SIZE,(LONG*)&hHandle->FileAttrib.nFileSizeHigh,FILE_BEGIN);
		WriteFile(hHandle->hFile,&hHandle->btFileEnd,hHandle->wFixEnd,&dwWrited,NULL);
		FlushFileBuffers(hHandle->hFile);
	}
	SetFilePointer(hHandle->hFile,hHandle->FileAttrib.nFileSizeLow,(LONG*)&hHandle->FileAttrib.nFileSizeHigh,FILE_BEGIN);
	SetEndOfFile(hHandle->hFile);
	SetFileTime(hHandle->hFile,&hHandle->FileAttrib.ftCreationTime,
				&hHandle->FileAttrib.ftLastAccessTime,&hHandle->FileAttrib.ftLastWriteTime);
	CloseHandle(hHandle->hFile);
	SetFileAttributes(hHandle->szFile,hHandle->FileAttrib.dwFileAttributes);
	RemoveFLHandle(hHandle);
    return  TRUE;
}   //	Close_FL_Handle

BOOL	EXPORT LogicSector2PhysSectorFast(HANDLE hFile, DWORD dwLogicalSec,DWORD dwArrIndex, DWORD dwSectors)
{
	SET_SIGNSTRU SetSignInfo;
	DWORD        dwMoveHi = 0;
    PFILE_LOC_HANDLE	hHandle;
    DWORD		dwWritten;
	int         nDrive;
    DWORD		dwCount,dwLocation,dwSize;

	if (hFile == NULL || hFile == INVALID_HANDLE_VALUE) return FALSE;
	hHandle = (PFILE_LOC_HANDLE)hFile;
	if (hHandle->szFile[0] >= 'a' && hHandle->szFile[0] <= 'z') nDrive = (int)(hHandle->szFile[0] - 'a');
	else nDrive = (int)(hHandle->szFile[0] - 'A');
	dwLocation = dwLogicalSec*SECTOR_SIZE;
	dwSize = dwSectors*SECTOR_SIZE;
	if (dwSectors > SECTORS_OF_CACHE || 
		dwArrIndex >= SECTORS_OF_CACHE || 
		dwSectors+dwArrIndex > SECTORS_OF_CACHE || 
		dwLocation > hHandle->FileAttrib.nFileSizeLow) return FALSE;

	if (!dwArrIndex)
	{
		SetSignInfo.dwIndex = 0;
		SetSignInfo.dwSecNum = SECTORS_OF_CACHE;
		SetSignInfo.dwLocSign = g_VolumeInfo[nDrive].dwLockSign+1;
		if (DeviceIoControl(g_VolumeInfo[nDrive].hVolume,IOCTL_YGFLOC_SET_SIGN,&SetSignInfo,sizeof(SET_SIGNSTRU),NULL,0,&dwWritten,NULL))
			g_VolumeInfo[nDrive].dwLockSign ++;
	}
	else	SetSignInfo.dwLocSign = g_VolumeInfo[nDrive].dwLockSign;
	for (dwCount = 0; dwCount < dwSectors; dwCount++)
	{
		g_LocInfo[dwCount].usSectorBufIndex = (WORD)(dwArrIndex+dwCount);
		g_LocInfo[dwCount].ulSign = SetSignInfo.dwLocSign;
	}
	SetFilePointer(hHandle->hFile,dwLocation,(LONG*)&dwMoveHi,FILE_BEGIN);
	if (!WriteFile(hHandle->hFile,(PBYTE)g_LocInfo,dwSize,&dwWritten,NULL))
		return	FALSE;
	FlushFileBuffers(hHandle->hFile);
    return  TRUE;
}

BOOL EXPORT FileLocatorFlush(BYTE btDriveLetter)
{
//	DWORD  dwReaded,i;
	char   szDosDeviceName[MAX_PATH];
	int    nDrive;
	if (btDriveLetter >= 'a' && btDriveLetter <= 'z') nDrive = (int)(btDriveLetter - 'a');
	else 
	{
		if (btDriveLetter >= 'A' && btDriveLetter <= 'Z') nDrive = (int)(btDriveLetter - 'A');
		else return FALSE;
	}
	if (!g_VolumeInfo[nDrive].bInited)
	{
		sprintf(szDosDeviceName, "\\\\.\\%c:", btDriveLetter);
		g_VolumeInfo[nDrive].hVolume = CreateFile(szDosDeviceName,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,
							NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

		if (g_VolumeInfo[nDrive].hVolume == INVALID_HANDLE_VALUE) return FALSE;
	}
/*	while (!DeviceIoControl(g_VolumeInfo[nDrive].hVolume,
						FSCTL_LOCK_VOLUME,NULL,0,NULL,0,&dwReaded,NULL)&&i<10)
	{
		SleepEx(10,FALSE);
		i++;
	}
	DeviceIoControl(g_VolumeInfo[nDrive].hVolume,FSCTL_UNLOCK_VOLUME,NULL,0,NULL,0,&dwReaded,NULL);*/
	FlushFileBuffers(g_VolumeInfo[nDrive].hVolume);
	CloseHandle(g_VolumeInfo[nDrive].hVolume);
	if (g_VolumeInfo[nDrive].bInited)
	{
		sprintf(szDosDeviceName, "\\\\.\\%c:", btDriveLetter);
		g_VolumeInfo[nDrive].hVolume = CreateFile(szDosDeviceName,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,
							NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	}
	return TRUE;
}

//	btDrive = 0	: A
//			= 1	: B
//		
//		zero based
//

BOOL EXPORT	MakeMirrorDrive(DRIVE_MAP *pDrive)
{
	YGVPARMP_PARAMETER  VParMPParam;
	BOOL	bResult = FALSE;
	int     nDrive,nDriverNum;
	char	szDeviceName[MAX_PATH],szDosDeviceName[MAX_PATH],szDriveLetter[5];
	PARTITION_INFO_2000 PartitionInfo;
	HANDLE	hDevice;
	DWORD	dwRead;

	GetModuleFileName(NULL,szDeviceName,MAX_PATH);
	VParMPParam.btFileDrive = szDeviceName[0];

	FileLocatorFlush(pDrive->original_drive);
	if (PartitionInfoByDriveLetter(pDrive->original_drive,&PartitionInfo))
	{
		sprintf(szDosDeviceName, "\\\\.\\PhysicalDrive%d",PartitionInfo.nHardDiskNum);

		hDevice = CreateFile(szDosDeviceName,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,
							NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	
		if (hDevice == INVALID_HANDLE_VALUE)
		{
			return FALSE;
		}

		if (!DeviceIoControl(hDevice,IOCTL_DISK_GET_DRIVE_GEOMETRY,
							NULL,0,	&VParMPParam.DiskGeometry,sizeof(DISK_GEOMETRY),&dwRead,NULL))
		{
			VParMPParam.GeometryStatus = 0xC0000010;
		}
		else	VParMPParam.GeometryStatus = 0;

		CloseHandle(hDevice);
		memcpy(&VParMPParam.PartitionInformation,&PartitionInfo.pi,sizeof(PARTITION_INFORMATION));
		VParMPParam.ParInfoStatus = 0;
		VParMPParam.DiskNumber = PartitionInfo.nHardDiskNum;
	}
	else return FALSE;

	nDriverNum = GetAvailableVParDrive(VOLUME_PARTITION_MAP);
	if (nDriverNum == -1) return FALSE;
	sprintf(szDeviceName,YGVPARMP_DIR_NAME "%d",nDriverNum);

	if (!GetAvailableDriveLetter(szDriveLetter)) return FALSE;
	pDrive->mirror_drive = szDriveLetter[0];
	VParMPParam.ucDriveLetter = pDrive->mirror_drive;

	sprintf(szDosDeviceName,"\\\\.\\YGVParMP%d",nDriverNum);
	hDevice = CreateFile(szDosDeviceName,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,
						NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if (hDevice != INVALID_HANDLE_VALUE)
	{
		if (DeviceIoControl(hDevice,IOCTL_YGVPARMP_OPEN_MEDIA,&VParMPParam,
							sizeof(YGVPARMP_PARAMETER),NULL,0,&dwRead,NULL))
		{
			if (DefineDosDevice(DDD_RAW_TARGET_PATH,szDriveLetter,szDeviceName))
			{
				nDrive = (int)(szDriveLetter[0]-'A');		
				memset(&g_VolumeInfo[nDrive],0,sizeof(VOLUMEINFORMATION));
				g_VolumeInfo[nDrive].dwVolumeType = VOLUME_PARTITION_MAP;
				g_VolumeInfo[nDrive].dwDriverNum = nDriverNum;
				g_VolumeInfo[nDrive].llPartitionStart = VParMPParam.PartitionInformation.StartingOffset.QuadPart;
				g_VolumeInfo[nDrive].llPartitionStart /= SECTOR_SIZE;
				g_VolumeInfo[nDrive].btOrginal = pDrive->original_drive;
				BroadcastVolumeDeviceChange(DBT_DEVICEARRIVAL,szDriveLetter[0]);
				bResult = TRUE;
			}
			else DeviceIoControl(hDevice,IOCTL_YGVPARMP_CLOSE_MEDIA,NULL,0,NULL,0,&dwRead,NULL);
		}
		CloseHandle(hDevice);
	}
	
	return	bResult;
}	//	MakeMirrorDrive
	

BOOL	EXPORT	MakeVirtualDrive(DRIVE_MAP *pDrive)
{
	YGVPAR_PARAMETER  VParParam;
	BOOL	bResult = FALSE;
	int     nDrive,nDriverNum;
	char	szDeviceName[MAX_PATH],szDosDeviceName[MAX_PATH],szDriveLetter[5];
	HANDLE	hDevice;
	DWORD	dwRead;

	GetModuleFileName(NULL,szDeviceName,MAX_PATH);
	VParParam.btFileDrive = szDeviceName[0];
	sprintf(szDosDeviceName, "\\\\.\\PhysicalDrive%d",pDrive->btHardDisk&0x0f);

	hDevice = CreateFile(szDosDeviceName,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,
						NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	if (!DeviceIoControl(hDevice,IOCTL_DISK_GET_DRIVE_GEOMETRY,
						NULL,0,	&VParParam.DiskGeometry,sizeof(DISK_GEOMETRY),&dwRead,NULL))
	{
		VParParam.GeometryStatus = 0xC0000010;
	}
	else	VParParam.GeometryStatus = 0;
	CloseHandle(hDevice);

	nDriverNum = GetAvailableVParDrive(VOLUME_VIRTUAL_PARTITION);
	if (nDriverNum == -1) return FALSE;
	sprintf(szDeviceName,YGVPAR_DIR_NAME "%d",nDriverNum);

	if (!GetAvailableDriveLetter(szDriveLetter)) return FALSE;

	pDrive->mirror_drive = szDriveLetter[0];
	memset(&VParParam.PartitionInformation,0,sizeof(PARTITION_INFORMATION));
	VParParam.PartitionInformation.StartingOffset.QuadPart = pDrive->llParStart;
	VParParam.PartitionInformation.StartingOffset.QuadPart *= SECTOR_SIZE;
	VParParam.PartitionInformation.PartitionLength.QuadPart = pDrive->llParLen;
	VParParam.PartitionInformation.PartitionLength.QuadPart *= SECTOR_SIZE;
	VParParam.PartitionInformation.HiddenSectors = pDrive->dwHiddenSectors;
	VParParam.PartitionInformation.PartitionNumber = 1;
	VParParam.ParInfoStatus = 0;
	VParParam.ucDriveLetter = pDrive->mirror_drive;

	sprintf(szDosDeviceName,"\\\\.\\YGVPar%d",nDriverNum);
	hDevice = CreateFile(szDosDeviceName,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,
						NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if (hDevice != INVALID_HANDLE_VALUE)
	{
		if (DeviceIoControl(hDevice,IOCTL_YGVPAR_OPEN_MEDIA,&VParParam,
							sizeof(YGVPAR_PARAMETER),NULL,0,&dwRead,NULL))
		{
			if (DefineDosDevice(DDD_RAW_TARGET_PATH,szDriveLetter,szDeviceName))
			{
				nDrive = (int)(szDriveLetter[0]-'A');		
				memset(&g_VolumeInfo[nDrive],0,sizeof(VOLUMEINFORMATION));
				g_VolumeInfo[nDrive].dwVolumeType = VOLUME_VIRTUAL_PARTITION;
				g_VolumeInfo[nDrive].dwDriverNum = nDriverNum;
				g_VolumeInfo[nDrive].llPartitionStart = VParParam.PartitionInformation.StartingOffset.QuadPart;
				g_VolumeInfo[nDrive].llPartitionStart /= SECTOR_SIZE;
				BroadcastVolumeDeviceChange(DBT_DEVICEARRIVAL,szDriveLetter[0]);
				bResult = TRUE;
			}
			else DeviceIoControl(hDevice,IOCTL_YGVPAR_CLOSE_MEDIA,NULL,0,NULL,0,&dwRead,NULL);
		}
		CloseHandle(hDevice);
	}

	return	bResult;
}	//	MakeVirtualDrive

BOOL EXPORT	DiscardVirtualDrive(BYTE btDriveLetter)
{
	DWORD  dwReaded,dwActionCode;
	int    nDrive,i=0;
	char   szDrive[3] = "C:",szDeviceName[MAX_PATH];

	if (btDriveLetter >= 'a' && btDriveLetter <= 'z') nDrive = (int)(btDriveLetter - 'a');
	else 
	{
		if (btDriveLetter >= 'A' && btDriveLetter <= 'Z') nDrive = (int)(btDriveLetter - 'A');
		else return FALSE;
	}

	switch (g_VolumeInfo[nDrive].dwVolumeType)
	{
	case VOLUME_VIRTUAL_PARTITION:
		dwActionCode = IOCTL_YGVPAR_CLOSE_MEDIA;
		break;
	case VOLUME_PARTITION_MAP:
		dwActionCode = IOCTL_YGVPARMP_CLOSE_MEDIA;
		break;
	case VOLUME_CRYPT_PARTITION:
		dwActionCode = IOCTL_YGCRYDISK_CLOSE_MEDIA;
		break;
	default:
		return FALSE;
	}

	if (g_VolumeInfo[nDrive].bInited) Free_FileLocater(btDriveLetter);

	sprintf(szDeviceName, "\\\\.\\%c:", btDriveLetter);
	g_VolumeInfo[nDrive].hVolume =  CreateFile(szDeviceName,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,
												NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (g_VolumeInfo[nDrive].hVolume == INVALID_HANDLE_VALUE) 
		return FALSE;

	while (!DeviceIoControl(g_VolumeInfo[nDrive].hVolume,
						FSCTL_LOCK_VOLUME,NULL,0,NULL,0,&dwReaded,NULL)&&i<10)
	{
		SleepEx(10,FALSE);
		i++;
	}
	DeviceIoControl(g_VolumeInfo[nDrive].hVolume,FSCTL_DISMOUNT_VOLUME,NULL,0,NULL,0,&dwReaded,NULL);
	DeviceIoControl(g_VolumeInfo[nDrive].hVolume,dwActionCode,NULL,0,NULL,0,&dwReaded,NULL);
	DeviceIoControl(g_VolumeInfo[nDrive].hVolume,FSCTL_UNLOCK_VOLUME,NULL,0,NULL,0,&dwReaded,NULL);

	CloseHandle(g_VolumeInfo[nDrive].hVolume);
	szDrive[0] = btDriveLetter;	
	DefineDosDevice(DDD_REMOVE_DEFINITION,	szDrive,NULL);

	if (g_VolumeInfo[nDrive].dwVolumeType == VOLUME_VIRTUAL_PARTITION)
	{
		GetModuleFileName(NULL,szDeviceName,MAX_PATH);
		sprintf(&szDeviceName[1],":\\YGVPar%d.DAT",g_VolumeInfo[nDrive].dwDriverNum);
		DeleteFile(szDeviceName);
	}
	memset(&g_VolumeInfo[nDrive],0,sizeof(VOLUMEINFORMATION));
// Here's how you call the function when you've removed a drive letter
// In this call, DriveLetter is the drive letter that you removed
	BroadcastVolumeDeviceChange(DBT_DEVICEREMOVECOMPLETE, szDrive[0]);
	return TRUE;
}	//	DiscardMirrorDrive

BOOL  RemoveFLHandle(PFILE_LOC_HANDLE pHandle)
{
	int  nDrive;
	POPEN_FILE_LIST pCur,pNext;
	if (pHandle->szFile[0] >= 'a' && pHandle->szFile[0] <= 'z') nDrive = (int)(pHandle->szFile[0] - 'a');
	else 
	{
		if (pHandle->szFile[0] >= 'A' && pHandle->szFile[0] <= 'Z') nDrive = (int)(pHandle->szFile[0] - 'A');
		else return FALSE;
	}
	pCur = g_VolumeInfo[nDrive].pFileListHead;
	while (pCur)
	{
		pNext = pCur->pNextNode;
		if (pCur->LocFileHandle == pHandle)
		{
			if (pCur->pPrevNode) pCur->pPrevNode->pNextNode = pCur->pNextNode;
			else g_VolumeInfo[nDrive].pFileListHead = pCur->pNextNode;
			if (pCur->pNextNode) pCur->pNextNode->pPrevNode = pCur->pPrevNode;
			else g_VolumeInfo[nDrive].pFileListTail = pCur->pPrevNode;
			free(pCur);
		}
		pCur = pNext;
	}
	return TRUE;
}

BOOL  GetAvailableDriveLetter(char *szDrive)
{
	char cDriveLetter,szVolume[5] = "C:\\";
	for (cDriveLetter = 'C';cDriveLetter <= 'Z';cDriveLetter++)
	{
		szVolume[0] = cDriveLetter;
		if (GetDriveType(szVolume) == DRIVE_NO_ROOT_DIR)
		{
			szDrive[0] = cDriveLetter;
			szDrive[1] = ':';
			szDrive[2] = '\0';
			return TRUE;
		}
	}
	return FALSE;
}

BOOL  SaveFLHandle(PFILE_LOC_HANDLE pHandle)
{
	int nDrive;
	POPEN_FILE_LIST pCur;
	if (pHandle->szFile[0] >= 'a' && pHandle->szFile[0] <= 'z') nDrive = (int)(pHandle->szFile[0] - 'a');
	else 
	{
		if (pHandle->szFile[0] >= 'A' && pHandle->szFile[0] <= 'Z') nDrive = (int)(pHandle->szFile[0] - 'A');
		else return FALSE;
	}
	pCur = (POPEN_FILE_LIST)malloc(sizeof(OPEN_FILE_LIST));
	if (pCur)
	{
		pCur->LocFileHandle = pHandle;
		pCur->pNextNode = NULL;
		pCur->pPrevNode = g_VolumeInfo[nDrive].pFileListTail;
		if (pCur->pPrevNode) 
		{
			pCur->pPrevNode->pNextNode = pCur;
			g_VolumeInfo[nDrive].pFileListTail = pCur;
		}
		else 
		{
			g_VolumeInfo[nDrive].pFileListHead = pCur;
			g_VolumeInfo[nDrive].pFileListTail = pCur;
		}
		return TRUE;
	}
	return FALSE;
}

DWORD   EXPORT FL_ReadFile(HANDLE hFile,DWORD dwStart,LPBYTE pBuf,DWORD dwReadLen)
{
    PFILE_LOC_HANDLE	hHandle;
    DWORD		dwWrittenRead,dwMoveHi = 0;

	if (hFile == NULL || hFile == INVALID_HANDLE_VALUE) return -1;
	if (dwReadLen)
	{
	    hHandle = (PFILE_LOC_HANDLE)hFile;
		SetFilePointer(hHandle->hFile,dwStart,(LONG*)&dwMoveHi,FILE_BEGIN);
		if (!ReadFile(hHandle->hFile,pBuf,dwReadLen,&dwWrittenRead,NULL)) return -1;
		dwReadLen = dwWrittenRead;
	}
	return dwReadLen;
}

DWORD   EXPORT FL_GetFileSize(HANDLE hFile,DWORD *pdFileSizeHigh)
{
    PFILE_LOC_HANDLE	hHandle;

	if (hFile == NULL || hFile == INVALID_HANDLE_VALUE) return -1;
    hHandle = (PFILE_LOC_HANDLE)hFile;
	return GetFileSize(hHandle->hFile,pdFileSizeHigh);
}


DWORD   EXPORT FL_WriteFile(HANDLE hFile,DWORD dwStart,LPBYTE pBuf,DWORD dwWriteLen)
{
    PFILE_LOC_HANDLE	hHandle;
    DWORD		dwWrittenRead,dwMoveHi = 0;

	if (hFile == NULL || hFile == INVALID_HANDLE_VALUE) return -1;
	if (dwWriteLen)
	{
	    hHandle = (PFILE_LOC_HANDLE)hFile;
		SetFilePointer(hHandle->hFile,dwStart,(LONG*)&dwMoveHi,FILE_BEGIN);
		if (!WriteFile(hHandle->hFile,pBuf,dwWriteLen,&dwWrittenRead,NULL)) return -1;
		FlushFileBuffers(hHandle->hFile);
		dwWriteLen = dwWrittenRead;
	}
	return dwWriteLen;
}


BOOL EXPORT	MakeCryDisk(DRIVE_MAP *pDrive,char *key,UINT keyLen,UINT EncryptType)
{
	YGCRYDISK_PARAMETER  VParMPParam;
	int     nDrive,nDriverNum;
	char	szDeviceName[MAX_PATH],szDosDeviceName[MAX_PATH],szDriveLetter[5];
	PARTITION_INFO_2000 PartitionInfo;
	HANDLE	hDevice;
	DWORD	dwRead;
	BOOL	bResult = FALSE;

	FileLocatorFlush(pDrive->original_drive);
	if (PartitionInfoByDriveLetter(pDrive->original_drive,&PartitionInfo))
	{
		sprintf(szDosDeviceName, "\\\\.\\PhysicalDrive%d",PartitionInfo.nHardDiskNum);

		hDevice = CreateFile(szDosDeviceName,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,
							NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	
		if (hDevice == INVALID_HANDLE_VALUE)
		{
			return FALSE;
		}

		if (!DeviceIoControl(hDevice,IOCTL_DISK_GET_DRIVE_GEOMETRY,
							NULL,0,	&VParMPParam.DiskGeometry,sizeof(DISK_GEOMETRY),&dwRead,NULL))
		{
			VParMPParam.GeometryStatus = 0xC0000010;
		}
		else	VParMPParam.GeometryStatus = 0;

		CloseHandle(hDevice);
		memcpy(&VParMPParam.PartitionInformation,&PartitionInfo.pi,sizeof(PARTITION_INFORMATION));
		VParMPParam.ParInfoStatus = 0;
		VParMPParam.DiskNumber = PartitionInfo.nHardDiskNum;
		memset(VParMPParam.ucKey,0,32);
		strncpy((char *)VParMPParam.ucKey,key,32);
		VParMPParam.usKeyLen = keyLen;
		VParMPParam.usEncryptType = EncryptType;
		VParMPParam.ucDriveLetter = pDrive->mirror_drive;
	}
	else return FALSE;

	nDriverNum = GetAvailableVParDrive(VOLUME_CRYPT_PARTITION);
	if (nDriverNum == -1) 	return FALSE;

	sprintf(szDriveLetter,"%C:",VParMPParam.ucDriveLetter);
	sprintf(szDeviceName,YGCRYDISK_DEVICE_NAME "%d",nDriverNum);

	sprintf(szDosDeviceName,"\\\\.\\YGCryDisk%d",nDriverNum);
	hDevice = CreateFile(szDosDeviceName,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,
						NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if (hDevice != INVALID_HANDLE_VALUE)
	{
		if (DeviceIoControl(hDevice,IOCTL_YGCRYDISK_OPEN_MEDIA,&VParMPParam,
							sizeof(YGCRYDISK_PARAMETER),NULL,0,&dwRead,NULL))
		{
			if (DefineDosDevice(DDD_RAW_TARGET_PATH,szDriveLetter,szDeviceName))
			{
				nDrive = (int)(szDriveLetter[0]-'A');		
				memset(&g_VolumeInfo[nDrive],0,sizeof(VOLUMEINFORMATION));
				g_VolumeInfo[nDrive].dwVolumeType = VOLUME_PARTITION_MAP;
				g_VolumeInfo[nDrive].dwDriverNum = nDriverNum;
				g_VolumeInfo[nDrive].llPartitionStart = VParMPParam.PartitionInformation.StartingOffset.QuadPart;
				g_VolumeInfo[nDrive].llPartitionStart /= SECTOR_SIZE;
				g_VolumeInfo[nDrive].btOrginal = pDrive->original_drive;
				BroadcastVolumeDeviceChange(DBT_DEVICEARRIVAL,szDriveLetter[0]);
				bResult = TRUE;
			}
			else DeviceIoControl(hDevice,IOCTL_YGCRYDISK_CLOSE_MEDIA,NULL,0,NULL,0,&dwRead,NULL);

		}
		CloseHandle(hDevice);
	}
    return bResult;
}	//	MakeCryDisk

int	 GetAvailableVParDrive(DWORD dwVParType)
{
	char  szDeviceFormat[50],szDeviceName[50];
	HANDLE hDevice;
	DIOC_REGISTERS  IoRegs;
	ULONG	ulCtlCode,dwRead,ulLastError;
	int		i,nMaxDrive = 0;

	switch (dwVParType)
	{
	case VOLUME_VIRTUAL_PARTITION:
		ulCtlCode = IOCTL_YGVPAR_DRIVE_ISUSEFUL;
		strcpy(szDeviceFormat,"\\\\.\\YGVPar%d");
		nMaxDrive = MAX_VPAR_DRIVE;
		break;
	case VOLUME_PARTITION_MAP:
		ulCtlCode = IOCTL_YGVPARMP_DRIVE_ISUSEFUL;
		strcpy(szDeviceFormat,"\\\\.\\YGVParMP%d");
		nMaxDrive = MAX_VPARMP_DRIVE;
		break;
	case VOLUME_CRYPT_PARTITION:
		ulCtlCode = IOCTL_YGCRYDISK_DRIVE_ISUSEFUL;
		strcpy(szDeviceFormat,"\\\\.\\YGCryDisk%d");
		nMaxDrive = MAX_CRYDISK_DRIVE;
		break;
	}

	for (i = 0; i < nMaxDrive;i++)
	{
		sprintf(szDeviceName,szDeviceFormat,i);
		hDevice = CreateFile(szDeviceName,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,
							NULL,OPEN_EXISTING,0,NULL);
	
		if (hDevice != INVALID_HANDLE_VALUE)
		{
			if (DeviceIoControl(hDevice,ulCtlCode,
								NULL,0,&IoRegs,sizeof(DIOC_REGISTERS),&dwRead,NULL))
			{
				if (IoRegs.reg_EAX)
				{
					CloseHandle(hDevice);
					return i;
				}
			}
			CloseHandle(hDevice);
		}
		else ulLastError = GetLastError();
	}
	return (-1);
}

BYTE    EXPORT  GetMPDriveInfo(int nDrive)
{
	YGCRYDISK_PARAMETER  VParMPParam;
	char  szDeviceFormat[50],szDeviceName[50];
	HANDLE	hDevice;
	DWORD	dwGet;
	BYTE    tchar;
	BOOL    bResult = FALSE;

	strcpy(szDeviceFormat,"\\\\.\\YGCryDisk%d");
	sprintf(szDeviceName,szDeviceFormat,nDrive);

	hDevice = CreateFile(szDeviceName,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,
							NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if (hDevice != INVALID_HANDLE_VALUE)
	{
		bResult = DeviceIoControl(hDevice,IOCTL_YGCRYDISK_GET_MPDRIVE_INFO,NULL,0,
			&VParMPParam,sizeof(YGCRYDISK_PARAMETER),&dwGet,NULL);

		CloseHandle(hDevice);
	}

	if(bResult)
	{
		tchar = VParMPParam.ucDriveLetter;
		return tchar;
	}
	return 0;
}

int     EXPORT  GetCount()
{
	char szDosDeviceName[MAX_PATH];
	int count = 0;
	DWORD dwGet;
    BOOL bResult;
	HANDLE hDevice;
	//YGCRYDISK_PARAMETER  VParMPParam;
    DIOC_REGISTERS DIOCRegs;
	//PARTITION_INFO_2000 PartitionInfo;

//	if (PartitionInfoByDriveLetter(pDrive->original_drive,&PartitionInfo))
//	{
//		sprintf(szDosDeviceName, "\\\\.\\PhysicalDrive%d",PartitionInfo.nHardDiskNum);
		sprintf(szDosDeviceName,"\\\\.\\YGCryDisk0");
		hDevice = CreateFile(szDosDeviceName,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,
							NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

		if (hDevice == INVALID_HANDLE_VALUE)
		{
			return 0;
		}

		bResult = DeviceIoControl(hDevice,IOCTL_YGCRYDISK_GET_COUNT,NULL,0,
			&DIOCRegs,sizeof(DIOC_REGISTERS),&dwGet,NULL);

		if(bResult)
		{
			count = DIOCRegs.reg_EAX;
		}
		CloseHandle(hDevice);
//	}

	return count;
}

int    EXPORT  AddCount()
{
//	YGCRYDISK_PARAMETER  VParMPParam;
	char szDosDeviceName[MAX_PATH];
	DWORD dwSet;
	BOOL bResult;
	HANDLE hDevice;
	//PARTITION_INFO_2000 PartitionInfo;

//    if (PartitionInfoByDriveLetter(pDrive->original_drive,&PartitionInfo))
//	{
//		sprintf(szDosDeviceName, "\\\\.\\PhysicalDrive%d",PartitionInfo.nHardDiskNum);
		sprintf(szDosDeviceName,"\\\\.\\YGCryDisk0");
		hDevice = CreateFile(szDosDeviceName,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,
							NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

		if (hDevice == INVALID_HANDLE_VALUE)
		{
			return 0;
		}

		bResult = DeviceIoControl(hDevice,IOCTL_YGCRYDISK_ADD_COUNT,NULL,0,NULL,
			0,&dwSet,NULL);

		CloseHandle(hDevice);
//	}
	
	return bResult;
}


// Call this function when you've added/removed a drive letter symbolic
// link in kernel mode.  When using Win32 alone to create your symbolic
// link (using DefineDosDevice()), you'll end up with symbolic links in
// the session namespace (which you probably don't want) rather than
// the global namespace.  The solution is to create the drive letter
// symbolic links in kernel mode, but then you'll discover (as you did)
// that DefineDosDevice() does more than just create symbolic links... it
// also notifies everybody that a drive letter assignment was made.  So
// if you create your symbolic links in kernel mode you'll need to add this
// extra functionality, and as far as I can tell this function is sufficient.
HRESULT BroadcastVolumeDeviceChange(WPARAM notification, WCHAR DriveLetter)
{
	static PDEV_BROADCAST_VOLUME pMyDevHdr = NULL;
	DWORD dwFlag = BSM_ALLCOMPONENTS;
	DWORD volumeMask = 1 << (DriveLetter - L'A');

	// Allocate our broadcast header and keep it around (static)
	// We have to keep it around because if we post the broadcast the buffers that we
	// pass into the broadcast have to stick around after this call returns.
	if (NULL == pMyDevHdr)
	{
		pMyDevHdr = new DEV_BROADCAST_VOLUME;
	}

	// Initialize our broadcast header
	pMyDevHdr->dbcv_devicetype = DBT_DEVTYP_VOLUME;
	pMyDevHdr->dbcv_size = sizeof(DEV_BROADCAST_VOLUME);
	pMyDevHdr->dbcv_flags = DBTF_NET;
	pMyDevHdr->dbcv_unitmask = volumeMask;

	// Broadcast the device change notification
	BroadcastSystemMessage(BSF_IGNORECURRENTTASK,
	                       &dwFlag,
	                       WM_DEVICECHANGE,
	                       notification,
                             (LPARAM)pMyDevHdr);

	return S_OK;
}
