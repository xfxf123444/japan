/*========================================================================
ParInfo.cpp for WIN2K
WQ
=========================================================================*/

#include "stdafx.h"
#include "stdio.h"
#include "tchar.h"
#include "assert.h"

#ifndef ASSERT
#define ASSERT assert
#endif

#ifdef _DEBUG
#define DEBUG _DEBUG
#endif

#include "..\..\export\2000\ParInfo.h"
#include    "..\..\..\..\YGDiskRW\Cur_ver\Export\YGDiskRW.h"
#include    "..\..\..\..\BlkMover\Cur_ver\Export\BlkMover.h"
#include "ParInfoPrivate.h"
#include "SymbolicLink.h"
#include "Volume.h"

BOOL g_bVista = FALSE;

#define YGDISKRW_DISKCLASS_KEY "SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E967-E325-11CE-BFC1-08002BE10318}"
#define SYSTEM_CURRENT_CONTROL_SET "SYSTEM\\CurrentControlSet\\Services"


BOOL IsWindowsVista()
{
    OSVERSIONINFOEX osvi;

    // Try calling GetVersionEx using the OSVERSIONINFOEX structure.
    // If that fails, try using the OSVERSIONINFO structure.
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    if( !GetVersionEx ((OSVERSIONINFO *) &osvi))
    {
        // If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.
        osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
        if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
           return FALSE;
    }

	//return (osvi.dwMajorVersion == 6) && (osvi.dwPlatformId == 2);
	return osvi.dwMajorVersion >= 6;
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
			g_bVista = IsWindowsVista();
            break;
        case DLL_PROCESS_DETACH:
			break;
        case DLL_THREAD_ATTACH:
             break;
        case DLL_THREAD_DETACH:
			break;
    }
    return TRUE;
}

void DLL_PARINFO_API InitOperationSystemType()
{
	if (!g_bVista) {
		g_bVista = IsWindowsVista();
	}
}
/*------------------------------------------------------------------------
Init_PartitionInfo()
Purpose:
	Initialize the global viables;
	g_DriveLetters to Zero;
	Special g_DriveLetters['A'-'A'] is a indicator to initialize;
Return value:
	True;

-------------------------------------------------------------------------*/
BOOL DLL_PARINFO_API Init_PartitionInfo()
{
	return TRUE;
}

/*-------------------------------------------
Free_PartitionInfo()
Purpose:
	Close all open handles;
Return Value:
	True;
-------------------------------------------*/
BOOL DLL_PARINFO_API Free_PartitionInfo()
{
	return TRUE;
}

#ifndef _VMM_

extern	"C"
{
BOOL DeleteRegKey(
	HKEY ReRootKey,
	LPCSTR ReSubKey,
	TCHAR *ReKeyName)
{
	BOOL	bSucc=1;
	HKEY hKey;
	if(RegOpenKeyEx(ReRootKey,ReSubKey,0,KEY_WRITE,&hKey)==ERROR_SUCCESS)
	{
		if((RegDeleteKey(hKey,ReKeyName))!=ERROR_SUCCESS)
		{
			bSucc=0;
		}
		RegCloseKey(hKey);
		
	}else
		bSucc=FALSE;
	return bSucc;
}
BOOL DeleteRegValue(
	HKEY ReRootKey,
	LPCSTR ReSubKey,
	TCHAR *ReValueName)
{
	BOOL bSucc=1; 
	HKEY hKey;
	if(RegOpenKeyEx(ReRootKey,ReSubKey,0,KEY_WRITE,&hKey)==ERROR_SUCCESS)
	{
		if(RegDeleteValue(hKey,ReValueName)!=ERROR_SUCCESS)
		{
			bSucc=0;
		}
		RegCloseKey(hKey);
	}
	else
	{
		bSucc=0;
	}
	return bSucc;
}

BOOL GetSetRegValueEx(
		BOOL	bGet,
		HKEY	hKey,
		LPCSTR	szKey, 
		LPCSTR	szValue,
		DWORD	&dwType,
		LPBYTE	lpBuf,
		DWORD	&dwBufSize)
{
	HKEY		hAppKey = NULL;
	BOOL		bSucc = TRUE;
	
	if (RegCreateKey(hKey, szKey, &hAppKey))
		return FALSE;
	if (hAppKey == NULL)
		return FALSE;
	if (bGet)
	{
		if (RegQueryValueEx(
				hAppKey,
				szValue,
				NULL,
				&dwType,
				lpBuf,
				&dwBufSize))
			bSucc = FALSE;
	}
	else
	{
		if (RegSetValueEx(
				hAppKey,
				szValue,
				0,
				dwType,
				lpBuf,
				dwBufSize))
			bSucc = FALSE;
	}
	// close key	
	RegCloseKey(hAppKey);

	return bSucc;
}

BOOL GetSetRegValue(
		BOOL	bGet,
		HKEY	hKey,
		LPCSTR	szKey, 
		LPCSTR	szValue,
		LPCSTR  szData)
{
	DWORD	dwType = REG_SZ;
	DWORD	dwBufSize;
	BOOL	bSucc;
	
	if (bGet)
		dwBufSize = MAX_PATH;
	else
		dwBufSize = strlen(szData) + 1;
	bSucc = GetSetRegValueEx(
		bGet, hKey, szKey, szValue, dwType, 
		(LPBYTE)szData, dwBufSize);
	return bSucc;	
}

BOOL DLL_PARINFO_API InstallYGDiskRWFilter(LPSTR szDriverName)
{
    LPTSTR filterToAdd    = szDriverName;
	BOOL	bSuccess	= FALSE;
	HKEY	hClassKey;
	TCHAR	tstr[MAX_PATH],tstrKey[MAX_PATH];
	DWORD	dwType,dwSize,dwValue;

	unsigned int nLength;

    if(filterToAdd == NULL )
	{
		return FALSE;
	}

	GetSystemDirectory(tstr,MAX_PATH);
	strcat(tstr,"\\Drivers\\");
	strcat(tstr,szDriverName);
	strcat(tstr,".sys");
	if (GetFileAttributes(tstr) == -1) return FALSE;

	dwType = REG_DWORD;
	dwSize = sizeof(DWORD);

	strcpy(tstrKey,SYSTEM_CURRENT_CONTROL_SET);
	strcat(tstrKey,"\\");
	strcat(tstrKey,szDriverName);
	dwValue = 0;
	GetSetRegValueEx(FALSE,HKEY_LOCAL_MACHINE,tstrKey,"Start",dwType,(LPBYTE)&dwValue,dwSize);
	dwValue = 1;
	GetSetRegValueEx(FALSE,HKEY_LOCAL_MACHINE,tstrKey,"ErrorControl",dwType,(LPBYTE)&dwValue,dwSize);
	GetSetRegValueEx(FALSE,HKEY_LOCAL_MACHINE,tstrKey,"Type",dwType,(LPBYTE)&dwValue,dwSize);
	dwValue=7;
	GetSetRegValueEx(FALSE,HKEY_LOCAL_MACHINE,tstrKey,"Tag",dwType,(LPBYTE)&dwValue,dwSize);

	GetSetRegValue(FALSE, HKEY_LOCAL_MACHINE, tstrKey, "Group","Filter");
	GetSetRegValue(FALSE, HKEY_LOCAL_MACHINE, tstrKey, "DisplayName",szDriverName);

	sprintf(tstr,"System32\\Drivers\\%s.sys",szDriverName);
	GetSetRegValue(FALSE, HKEY_LOCAL_MACHINE, tstrKey, "ImagePath",tstr);

	memset(tstr, 0, sizeof(TCHAR) * MAX_PATH);
	RegOpenKeyEx(HKEY_LOCAL_MACHINE,
				YGDISKRW_DISKCLASS_KEY,
				0,KEY_ALL_ACCESS,
				&hClassKey);
	if( hClassKey != INVALID_HANDLE_VALUE )
	{
		dwSize = sizeof(TCHAR) * MAX_PATH;
		if( RegQueryValueEx(hClassKey,
							"UpperFilters",
							NULL,
							&dwType,
							(LPBYTE)tstr,
							&dwSize) == ERROR_SUCCESS )
		{
			// check if the drive has add to the device
			if( MultiSzLength( tstr ) > 1 )
			{
				MultiSzSearchAndDeleteCaseInsensitive( filterToAdd, tstr, &nLength );
			}
			// add the driver to the driver list
			PrependSzToMultiSz(filterToAdd,tstr);
		}else
		{
			// copy the string into the new buffer
			_tcscpy(tstr, filterToAdd);
		}
		//write registry
		dwSize = MultiSzLength(tstr);
		if( RegSetValueEx(hClassKey,
							"UpperFilters",
							NULL,
							REG_MULTI_SZ,
							(LPBYTE)tstr,
							dwSize) == ERROR_SUCCESS )
		{
			bSuccess = TRUE;
		}
		RegCloseKey(hClassKey);
    }
	return bSuccess;
}

//remove filter driver under win2000
BOOL DLL_PARINFO_API RemoveYGDiskRWFilter(LPSTR szDriverName)
{
    LPTSTR filterToRemove = szDriverName;
	BOOL	bSuccess	= TRUE;
	HKEY	hClassKey;
	TCHAR	tstr[MAX_PATH],tstrKey[MAX_PATH];
	DWORD	dwType,dwSize;

	unsigned int nLength;


    if(filterToRemove == NULL )
	{
		return FALSE;
	}

	strcpy(tstrKey,SYSTEM_CURRENT_CONTROL_SET);
	strcat(tstrKey,"\\");
	strcat(tstrKey,szDriverName);

	DeleteRegValue(HKEY_LOCAL_MACHINE,tstrKey,"ErrorControl");
	DeleteRegValue(HKEY_LOCAL_MACHINE,tstrKey,"Start");
	DeleteRegValue(HKEY_LOCAL_MACHINE,tstrKey,"Type");
	DeleteRegValue(HKEY_LOCAL_MACHINE,tstrKey,"Tag");
	DeleteRegValue(HKEY_LOCAL_MACHINE,tstrKey,"Group");
	DeleteRegValue(HKEY_LOCAL_MACHINE,tstrKey,"DisplayName");
	DeleteRegValue(HKEY_LOCAL_MACHINE,tstrKey,"ImagePath");
	DeleteRegKey(HKEY_LOCAL_MACHINE,SYSTEM_CURRENT_CONTROL_SET,szDriverName);

	memset(tstr, 0, sizeof(TCHAR) * MAX_PATH);
	RegOpenKeyEx(HKEY_LOCAL_MACHINE,
				YGDISKRW_DISKCLASS_KEY,
				0,KEY_ALL_ACCESS,
				&hClassKey);

	if( hClassKey != INVALID_HANDLE_VALUE )
	{
		dwSize = sizeof(TCHAR) * MAX_PATH;
		if( RegQueryValueEx(hClassKey,
							"UpperFilters",
							NULL,
							&dwType,
							(LPBYTE)tstr,
							&dwSize) == ERROR_SUCCESS )
		{
			if( MultiSzLength( tstr ) > 1 )
			{
				MultiSzSearchAndDeleteCaseInsensitive( filterToRemove, tstr, &nLength );
			}
			//write registry
			dwSize = MultiSzLength(tstr);
			if( dwSize < 1 )
			{
				//empty multiple string
				if( RegDeleteValue(hClassKey,"UpperFilters") != ERROR_SUCCESS )
				{
					bSuccess = FALSE;
				}
			}else
			{
				if( RegSetValueEx(hClassKey,
								"UpperFilters",
								NULL,
								dwType,
								(LPBYTE)tstr,
								dwSize) != ERROR_SUCCESS )
				{
					bSuccess = FALSE;
				}
			}
		}
		//close handle
		RegCloseKey(hClassKey);
    }
	return bSuccess;
}

BOOL DLL_PARINFO_API CheckYGDiskRW(BYTE btDisk)
{
	DWORD			cb,dwVersion;
	BOOL			bResult = FALSE;
	HANDLE			Handle;
	char			szDrive[30];

	sprintf(szDrive,"\\\\.\\PhysicalDrive%d",btDisk);
	Handle =  CreateFile(szDrive,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,
						NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if(Handle == INVALID_HANDLE_VALUE)
		return	FALSE;

	bResult = DeviceIoControl(Handle,IOCTL_YGDISKRW_GET_VERSION,
							 &dwVersion,sizeof(DWORD),
							 &dwVersion,sizeof(DWORD),
							 &cb,0);
	CloseHandle(Handle);

	return bResult;
}

/*--------------------------------------------------------------------------
WriteSector()
Purpose:
	Write sector to physical disk by logical block access;
Parameters:
	dwStartSec:
		Logical offset of the start of sectors to write;
	wSectors:
		Number of the sectors to write;
	pBuf:
		Buffer holding the sector's data to write;
	pDriveParam:
		Disk parameters in bios;
Return Value:
	TRUE if Succeed, FALSE if fail;
--------------------------------------------------------------------------*/
BOOL DLL_PARINFO_API WriteSector(DWORD					dwStartSec,
								 WORD					wSectors,
								 PBYTE					pBuf,
								 BYTE					btUnit,
								 PBIOS_DRIVE_PARAM		pDriveParam)
{
	BOOL					bResult = FALSE;
	DWORD					dwBytesWriten = 0;
	DWORD					dwBytesToWrite;
	HANDLE					hDrv;
	LARGE_INTEGER			lnStartBytes;
	YGDISKRWPARAM			Buffer;
	READ_WRITE_BUFFER		BlkMoverWrite;

	dwBytesToWrite = wSectors * pDriveParam->SectorsSize;
	lnStartBytes.QuadPart = (__int64)dwStartSec * pDriveParam->SectorsSize;

	if(lnStartBytes.QuadPart >= 0)
	{
		hDrv = GetDriveHandle(btUnit);
		ASSERT( hDrv != INVALID_HANDLE_VALUE);
		if(hDrv != INVALID_HANDLE_VALUE)
		{
			//if (g_bVista)
			//{
			//	BlkMoverWrite.dwStartSec = dwStartSec;
			//	BlkMoverWrite.dwLength = wSectors;
			//	BlkMoverWrite.bSys = FALSE;
			//	BlkMoverWrite.dwMovedRecNum = YGBLK_READ_WRITE;
			//	BlkMoverWrite.pBuffer = pBuf;

			//	bResult = DeviceIoControl(hDrv,IOCTL_YG_WRITE,
			//		&BlkMoverWrite,sizeof(READ_WRITE_BUFFER),
			//		&BlkMoverWrite,sizeof(READ_WRITE_BUFFER),
			//		&dwBytesWriten,0);
			//	if (!bResult)
			//	{
			//		Buffer.ulDisk	= btUnit;
			//		Buffer.dwStartSec	 = dwStartSec;
			//		Buffer.dwLength		 = wSectors;
			//		Buffer.pBuffer		 = pBuf;
			//		bResult = DeviceIoControl(hDrv,IOCTL_YGDISKRW_WRITE_DISK,
			//			&Buffer,sizeof(YGDISKRWPARAM),
			//			&Buffer,sizeof(YGDISKRWPARAM),
			//			&dwBytesWriten,0);
			//		DWORD temp = GetLastError();
			//		int i = 0;
			//	}
			//}

			if (!bResult)
			{
				DWORD dwtemp = SetFilePointer(hDrv,lnStartBytes.LowPart,&lnStartBytes.HighPart,FILE_BEGIN);
				if (dwtemp == INVALID_SET_FILE_POINTER) {
					dwtemp = GetLastError();
					int i = 0;
				}
				bResult = WriteFile(hDrv,pBuf,dwBytesToWrite,&dwBytesWriten,NULL);
				if (bResult == 0) {
					dwtemp = GetLastError();
					int i = 0;
				}
				bResult = bResult ? dwBytesToWrite == dwBytesWriten : FALSE;
			}
			//close handle
			CloseHandle(hDrv);
		}
	}
	return bResult;
}

/*--------------------------------------------------------------------------
ReadSector()
Purpose:
	Read sectors from physical disk by logical block access;
Parameters:
	dwStartSec:
		Logical offset of the start of sectors to read;
	wSectors:
		Number of the sectors to read;
	pBuf:
		Buffer holding the sector's data to read;
	pDriveParam:
		Disk parameters in bios;
Return Value:
	TRUE if Succeed, FALSE if fail;
--------------------------------------------------------------------------*/
BOOL DLL_PARINFO_API ReadSector(DWORD					dwStartSec,
								WORD					wSectors,
								PBYTE					pBuf,
								BYTE					btUnit,
								PBIOS_DRIVE_PARAM		pDriveParam)
{
	BOOL					bResult = FALSE;
	DWORD					dwBytesRead = 0;
	DWORD					dwBytesToRead;
	HANDLE					hDrv;
	LARGE_INTEGER			lnStartBytes;
	YGDISKRWPARAM			Buffer;
	READ_WRITE_BUFFER		BlkMoverRead;

	dwBytesToRead = wSectors * pDriveParam->SectorsSize;
	lnStartBytes.QuadPart = (__int64)dwStartSec * pDriveParam->SectorsSize;

	if(lnStartBytes.QuadPart >= 0)
	{
		hDrv = GetDriveHandle(btUnit);
		ASSERT( hDrv != INVALID_HANDLE_VALUE);
		
		if(hDrv != INVALID_HANDLE_VALUE)
		{
			//if (g_bVista)
			//{
			//	BlkMoverRead.dwStartSec = dwStartSec;
			//	BlkMoverRead.dwLength = wSectors;
			//	BlkMoverRead.bSys = FALSE;
			//	BlkMoverRead.dwMovedRecNum = 0;
			//	BlkMoverRead.pBuffer = pBuf;

			//	bResult = DeviceIoControl(hDrv,IOCTL_YG_READ,
			//							 &BlkMoverRead,sizeof(READ_WRITE_BUFFER),
			//							 &BlkMoverRead,sizeof(READ_WRITE_BUFFER),
			//							 &dwBytesRead,0);
			//	if (!bResult)
			//	{
			//		Buffer.ulDisk	= btUnit;
			//		Buffer.dwStartSec	 = dwStartSec;
			//		Buffer.dwLength		 = wSectors;
			//		Buffer.pBuffer		 = pBuf;
			//		bResult = DeviceIoControl(hDrv,IOCTL_YGDISKRW_READ_DISK,
			//								 &Buffer,sizeof(YGDISKRWPARAM),
			//								 &Buffer,sizeof(YGDISKRWPARAM),
			//								 &dwBytesRead,0);
			//	}
			//}
			if (!bResult)
			{
				SetFilePointer(hDrv,lnStartBytes.LowPart,&lnStartBytes.HighPart,FILE_BEGIN);
				bResult = ReadFile(hDrv,pBuf,dwBytesToRead,&dwBytesRead,NULL);	
				bResult = bResult ? dwBytesToRead == dwBytesRead : FALSE;
			}
			//close handle
			CloseHandle(hDrv);

		}
	}
	return bResult;
}

/*---------------------------------------------------------------------------
GetBootupDrive()
Purpose:
	Retrieve the drive letter accord the hard disk num and the partition start
	setctor;
ReturnValues:
	return 0xff indicate failure, otherwise return the right drive letter;
---------------------------------------------------------------------------*/
BYTE DLL_PARINFO_API GetBootupDrive(BYTE *nDisk)
{
	char							chDriveLetter = (char)0xff;
	char							szAnsi[MAX_PATH],szArcName[MAX_PATH];
	UNICODE_STRING					usLinkName;
	WCHAR							LinkName[MAX_PATH];
	UNICODE_STRING					usLinkContent;
	WCHAR							LinkContent[MAX_PATH];
	NTSTATUS						status;
	BIOS_DRIVE_PARAM		DriveParam;
	PARTITION_SEC           MBRInfo;
	HKEY				hKey;
	DWORD				dwType,dwSize;
	int                 i;
	char                *p;

	//Get boot drive parameter
	if( RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
						"SYSTEM\\CurrentControlSet\\Control",
						NULL,
						KEY_ALL_ACCESS,
						&hKey) == ERROR_SUCCESS )
	{	
		dwType = REG_SZ;
		dwSize = MAX_PATH;
		if( RegQueryValueEx(hKey,
							"SystemBootDevice",
							NULL,
							&dwType,
							(BYTE*)szAnsi,
							&dwSize) != ERROR_SUCCESS )
		{
			strcpy(szAnsi,"multi(0)disk(0)rdisk(0)partition(1)");
		}
		p = strrchr(szAnsi,'p');
		if (p) *p = '\0';
		RegCloseKey(hKey);
	}
	else return 0xff;
	strcpy(szArcName,"\\ArcName\\");
	strcat(szArcName,szAnsi);
	//init unicode strings
	usLinkName.MaximumLength = sizeof(LinkName);
	usLinkName.Buffer = LinkName;
	usLinkContent.MaximumLength = sizeof(LinkContent);
	usLinkContent.Buffer = LinkContent;
	//query by hard disk
	AnsiToUnicode(&usLinkName,szArcName);
	status = QuerySymbolicLink(&usLinkName,&usLinkContent);
	if(!NT_SUCCESS( status ))
	{
		return 0xff;
	}
	UnicodeToAnsi(&usLinkContent,szAnsi);
	p = strrchr(szAnsi,'\\');
	if (*p)
	{
		p--;
		*nDisk = *p-'0';
		if( !GetDriveParam(*nDisk,&DriveParam))
			return 0xff;
		ReadSector(0,1,(BYTE *)&MBRInfo,*nDisk,&DriveParam);
		for (i = 0;i < 4;i++)
		{
			if (MBRInfo.Partition[i].BootFlag)
			{
				chDriveLetter = RetrieveDriveLttr(*nDisk,PRIMARY,MBRInfo.Partition[i].StartSector);
				chDriveLetter += 'A';
				chDriveLetter -= 1;
				break;
			}
		}
	}
	return chDriveLetter;
}

/*---------------------------------------------------------------------------
RetrieveDriveLttr()
Purpose:
	Retrieve the drive letter accord the hard disk num and the partition start
	setctor;
Parameters:
	DriveNum:
		Physical drive num;
	flags:
		PRIMARY or LOGICAL;
	dwStart:
		start sector of the specified partition;
ReturnValues:
	return 0xff indicate failure, otherwise return the right drive letter;
---------------------------------------------------------------------------*/
BYTE DLL_PARINFO_API RetrieveDriveLttr(BYTE		DriveNum,
									   DWORD	flags,
									   DWORD	dwStart)
{
	char							chDriveLetter;
	DWORD							dwLogical;
	int								nCount = 0;
	GET_DRIVE_LAYOUT				dliDrive;
	BOOL							bResult;
	int								nPartitionNum;
	//viriable for symbolic operation
	char							szAnsi[MAX_PATH];
	UNICODE_STRING					usLinkName;
	WCHAR							LinkName[MAX_PATH];
	UNICODE_STRING					usLinkContent;
	WCHAR							LinkContent[MAX_PATH];
	UNICODE_STRING					usLinkContentSave;
	WCHAR							LinkContentSave[MAX_PATH];
	NTSTATUS						status;
	int								nTimes;

	DriveNum &= 0x7f;
	//get layout
	bResult = GetDriveLayout(DriveNum,&dliDrive);
	if(	!bResult )
	{
		return 0xff;
	}
	//search in layout
	nCount = FindInDriveLayout(dwStart, &dliDrive);
	if( nCount < 0 )
	{
		return 0xff;
	}
	if(	!dliDrive.dliDrive.PartitionEntry[nCount].RecognizedPartition )
	{
		return 0xff;
	}

	nPartitionNum = dliDrive.dliDrive.PartitionEntry[nCount].PartitionNumber;
	//init unicode strings
	usLinkName.MaximumLength = sizeof(LinkName);
	usLinkName.Buffer = LinkName;
	usLinkContent.MaximumLength = sizeof(LinkContent);
	usLinkContent.Buffer = LinkContent;
	usLinkContentSave.MaximumLength = sizeof(LinkContentSave);
	usLinkContentSave.Buffer = LinkContentSave;
	//query by hard disk
	sprintf(szAnsi,"\\Device\\Harddisk%d\\Partition%d",DriveNum,nPartitionNum);
	AnsiToUnicode(&usLinkName,szAnsi);
	status = QuerySymbolicLink(&usLinkName,&usLinkContentSave);
	if( !NT_SUCCESS( status ))
	{
		return 0xff;
	}
	for( nTimes = 0 ; nTimes < 10 ; nTimes++ )
	{
		//get logical drives
		dwLogical = GetLogicalDrives();
		dwLogical >>= 2;
		strcpy(szAnsi,"\\??\\X:");
		for( chDriveLetter = 'C' ; chDriveLetter <= 'Z' ; chDriveLetter++ )
		{
			szAnsi[4] = chDriveLetter;
			AnsiToUnicode(&usLinkName,szAnsi);
			status = QuerySymbolicLink(&usLinkName,&usLinkContent);
			if( NT_SUCCESS( status ) && 
				( usLinkContent.Length == usLinkContentSave.Length ) )
			{
				if( !memcmp( LinkContentSave,LinkContent,usLinkContent.Length ))
				{
					break;
				}
			}
		}
		if ( chDriveLetter <= 'Z' )
		{
			//succeed
			break;
		}
		else
		{
			Sleep(200);
		}
	}

	if ( chDriveLetter <= 'Z' )
		return( chDriveLetter - 0x40 );
	else
		return 0xff;
}
/*----------------------------------------------------------------------------------
GetDriveMapInfo()
Parameters:
	btDriveLetter:
		[in]drive letter;
	pInfo:
		[out]buffer;		
Return:
	TRUE or FALSE
-----------------------------------------------------------------------------------*/
BOOL DLL_PARINFO_API GetDriveMapInfo(BYTE							btDriveLetter,
									 PSIMULATE_DRIVEMAPINFO			pInfo)
{
	PARTITION_INFO_2000			pi2000;
	if( PartitionInfoByDriveLetter( btDriveLetter + 'A' -1,&pi2000) )
	{
		pInfo->DriveNum = pi2000.nHardDiskNum;
		pInfo->DriveNum |= 0x80;	//	80H , 81H
		pInfo->ParType = pi2000.pi.PartitionType;	//	04H , 06H 
		pInfo->wReserve = 0;	
		pInfo->dwStart = (DWORD)(pi2000.pi.StartingOffset.QuadPart / SECTOR_SIZE);	//	linear sector number
		return TRUE;
	}else
		return FALSE;
}

#endif // _VMM_

BOOL DLL_PARINFO_API GetPartitionInfo(BYTE						btHardDrive,
									  PPARINFOONHARDDISK		pParHard)
{
	BOOL						bResult;
	DWORD						dwBytesReturned = 0;
	BIOS_DRIVE_PARAM			bdp;
	GET_DRIVE_LAYOUT			dliDrive;

	bResult = FALSE;
	ZeroMemory( &bdp,sizeof(BIOS_DRIVE_PARAM));
	ZeroMemory( &dliDrive,sizeof(GET_DRIVE_LAYOUT));
	ZeroMemory( pParHard,sizeof(PARINFOONHARDDISK));
	//open drive handle
	bResult = GetDriveParam(btHardDrive,&bdp);

	ASSERT(bResult);

	if( bResult )
	{
		bResult = GetDriveLayout(btHardDrive,&dliDrive);
	}

	ASSERT(bResult);

	if(bResult)
	{
		//do translation
		SearchPartitions( &dliDrive,pParHard,&bdp );
	}
	//return
	return bResult;
}
/*	
BOOL DLL_PARINFO_API OSExist(BYTE btHardDisk,
							 BYTE btSystemFlag,
							 DWORD dwStartSec,
							 PBIOS_DRIVE_PARAM pbdp)
{
	return FALSE;
}
*/

/*
the following function is copied from 9x
*/
BOOL DLL_PARINFO_API PartitionInfoOfDriveLetter(BYTE				btLetter,
												PPARTITION_INFO		pParInfo)
{
	BIOS_DRIVE_PARAM		DriveParam;
	DWORD					cb;
	int						i;
	BOOL					blResult = FALSE;
	BOOL					blSuc=FALSE;
	PARINFOONHARDDISKEX		ParHard;
	SIMULATE_DRIVEMAPINFO	sdDriveMapInfo;

	memset( pParInfo,0,sizeof(PPARTITION_INFO));
	memset( &sdDriveMapInfo,0,sizeof(SIMULATE_DRIVEMAPINFO));
	memset( &ParHard,0,sizeof(PARINFOONHARDDISKEX));
	//get necessary information
	if( !GetDriveMapInfo( btLetter,&sdDriveMapInfo))
		return FALSE;

	if( !GetDriveParam(sdDriveMapInfo.DriveNum,&DriveParam))
		return FALSE;

	if( !GetPartitionInfoEx(sdDriveMapInfo.DriveNum,&ParHard))
		return FALSE;
	
	cb=sdDriveMapInfo.dwStart;
	//It's primary partition
	for(i=0;i<4;i++)
	{
		if(ParHard.pePriParInfo[i].StartSector==cb)
		{
			pParInfo->btDrive = btLetter+'A'-1;
			pParInfo->btHardDisk = sdDriveMapInfo.DriveNum;
			pParInfo->nPartition = i;
			pParInfo->dwPartitionType = PRIMARY;
			pParInfo->dwSectorsInPartition = ParHard.pePriParInfo[i].SectorsInPartition;
			pParInfo->btSystemFlag=ParHard.pePriParInfo[i].SystemFlag;  
			pParInfo->dwStartLogicalSector = ParHard.pePriParInfo[i].StartSector;
			pParInfo->wStartSector = ParHard.pePriParInfo[i].StartOfPartition[1] & 0x3f;
			pParInfo->wStartHead = ParHard.pePriParInfo[i].StartOfPartition[0];
			WORD	wCylinder;
			wCylinder = (WORD)(ParHard.pePriParInfo[i].StartOfPartition[1] & 0xc0) & 0xFF;
			wCylinder = wCylinder << 2;
			wCylinder = wCylinder | ((WORD)(ParHard.pePriParInfo[i].StartOfPartition[2]) & 0xFF);
			pParInfo->wStartCylinder = wCylinder;
			pParInfo->wEndSector = ParHard.pePriParInfo[i].EndOfPartition[1] &0x3f;
			pParInfo->wEndHead = ParHard.pePriParInfo[i].EndOfPartition[0];
			wCylinder = (WORD)(ParHard.pePriParInfo[i].EndOfPartition[1] & 0xc0) & 0xFF;
			wCylinder = wCylinder << 2;
			wCylinder = wCylinder | ((WORD)(ParHard.pePriParInfo[i].EndOfPartition[2]) & 0xFF);
			pParInfo->wEndCylinder = wCylinder;
			char	szStr[6]=" :\\\0";
			char	lpVolumeNameBuffer[MAX_LABELNAME];
			memset(lpVolumeNameBuffer,0x20,MAX_LABELNAME);
			szStr[0]=(char)(btLetter+'A'-1);
			memcpy(pParInfo->szOsLabel,"NO SYSTEM",10);
			memset(pParInfo->szLabelName,0x20,11);
			if(GetVolumeInformation(szStr,lpVolumeNameBuffer,MAX_LABELNAME,NULL,NULL,NULL,NULL,0))
			{
				for(i=0;i<MAX_LABELNAME-1;i++)
				{
					if(lpVolumeNameBuffer[i+1]!=0x20)
					{	
						blSuc=TRUE;
						break;
					}
				}
				memcpy(pParInfo->szLabelName,(PBYTE)lpVolumeNameBuffer,MAX_LABELNAME);
			}
			if(!blSuc)
			{
				if(pParInfo->btSystemFlag==0x0b || pParInfo->btSystemFlag==0x0c)
				{
					BOOT_SEC32 bsBoot32;
					if(ReadSector(pParInfo->dwStartLogicalSector,0x01,(PBYTE)&bsBoot32,sdDriveMapInfo.DriveNum,&DriveParam))
					{
						memcpy(pParInfo->szLabelName,bsBoot32.VolumeLabel,0x0b);	
					}
				}
				if(pParInfo->btSystemFlag==0x06 || pParInfo->btSystemFlag==0x0e)
				{
					BOOT_SEC16 bsBoot16;
					if(ReadSector(pParInfo->dwStartLogicalSector,0x01,(PBYTE)&bsBoot16,sdDriveMapInfo.DriveNum,&DriveParam))
					{
						memcpy(pParInfo->szLabelName,bsBoot16.VolumeLabel,0x0b);	
					}
				}
			}
		return TRUE;
		}
	}
	cb=cb-0x3f;
	for(i=0;i<ParHard.wNumOfLogic;i++)
	{
		if(ParHard.peLogParInfo[i].peCurParInfo.StartSector==cb)
		{
			pParInfo->btDrive = btLetter+'A'-1;
			pParInfo->btHardDisk = sdDriveMapInfo.DriveNum;
			pParInfo->nPartition = i;
			pParInfo->dwPartitionType = LOGICAL;
			pParInfo->dwSectorsInPartition = ParHard.peLogParInfo[i].peCurParInfo.SectorsInPartition;
			pParInfo->btSystemFlag= ParHard.peLogParInfo[i].peCurParInfo.SystemFlag ; 
			pParInfo->dwStartLogicalSector = ParHard.peLogParInfo[i].peCurParInfo.StartSector; //it's really !!
			pParInfo->wStartSector = ParHard.peLogParInfo[i].peCurParInfo.StartOfPartition[1] & 0x3f;
			pParInfo->wStartHead = ParHard.peLogParInfo[i].peCurParInfo.StartOfPartition[0];
			WORD	wCylinder;
			wCylinder = (WORD)(ParHard.peLogParInfo[i].peCurParInfo.StartOfPartition[1] & 0xc0) & 0xFF;
			wCylinder = wCylinder << 2;
			wCylinder = wCylinder | ((WORD)(ParHard.peLogParInfo[i].peCurParInfo.StartOfPartition[2]) & 0xFF);
			pParInfo->wStartCylinder = wCylinder;
			pParInfo->wEndSector = ParHard.peLogParInfo[i].peCurParInfo.EndOfPartition[1] &0x3f;
			pParInfo->wEndHead = ParHard.peLogParInfo[i].peCurParInfo.EndOfPartition[0];
			wCylinder = (WORD)(ParHard.peLogParInfo[i].peCurParInfo.EndOfPartition[1] & 0xc0) & 0xFF;
			wCylinder = wCylinder << 2;
			wCylinder = wCylinder | ((WORD)(ParHard.peLogParInfo[i].peCurParInfo.EndOfPartition[2]) & 0xFF);
			pParInfo->wEndCylinder = wCylinder;
			char	szStr[6]=" :\\\0";
			char	lpVolumeNameBuffer[MAX_LABELNAME];
			memset(lpVolumeNameBuffer,0x20,MAX_LABELNAME);
			szStr[0]=(char)(btLetter+'A'-1);
			memcpy(pParInfo->szOsLabel,"NO SYSTEM",10);
			memset(pParInfo->szLabelName,0x20,11);
			if(GetVolumeInformation(szStr,lpVolumeNameBuffer,MAX_LABELNAME,NULL,NULL,NULL,NULL,0))
			{
				for(i=0;i<MAX_LABELNAME-1;i++)
				{
					if(lpVolumeNameBuffer[i+1]!=0x20)
					{	
						blSuc=TRUE;
						break;
					}
				}
				memcpy(pParInfo->szLabelName,(PBYTE)lpVolumeNameBuffer,MAX_LABELNAME);
			}
			
			if(!blSuc) 
			{
				if(pParInfo->btSystemFlag==0x0b || pParInfo->btSystemFlag==0x0c)
				{
					BOOT_SEC32 bsBoot32;
					if(ReadSector(pParInfo->dwStartLogicalSector+0x3f,0x01,(PBYTE)&bsBoot32,sdDriveMapInfo.DriveNum,&DriveParam))
					{
						memcpy(pParInfo->szLabelName,bsBoot32.VolumeLabel,0x0b);	
					}
				}
				if(pParInfo->btSystemFlag==0x06 || pParInfo->btSystemFlag==0x0e)
				{
					BOOT_SEC16 bsBoot16;
					if(ReadSector(pParInfo->dwStartLogicalSector+0x3f,0x01,(PBYTE)&bsBoot16,sdDriveMapInfo.DriveNum,&DriveParam))
					{
						memcpy(pParInfo->szLabelName,bsBoot16.VolumeLabel,0x0b);	
					}
				}
			}
			return TRUE;
		}
	}
	return blResult;
}
/*----------------------------------------------------------------------------
AlignToCylinder()
Purpose:
	Make the sector's offset aligned to cylinder boundaries;
Parameters:
	dwSectorOffset:
		Sector's offset which need to be aligned;
	pbdpDrive:
		Physical hard disk's geometry parameters;
	btFlag:
		Align drection, which can be ALIGN_FOREWARD or ALIGN_BACKWARD;
Return Value:
	Offset after align; Negative indicate FAILURE;
----------------------------------------------------------------------------*/
DWORD DLL_PARINFO_API AlignToCylinder(DWORD					dwSectorOffset,
									  PBIOS_DRIVE_PARAM		pbdpDrive,
									  BYTE					btFlag,
									  BYTE					btExtend)
{
	DWORD		dwSectorsPerCylinder;
	DWORD		dwSectorOffsetAligned = -1;

	dwSectorsPerCylinder = pbdpDrive->dwHeads * pbdpDrive->dwSecPerTrack;
	//if beyond disk geometry
	if( dwSectorOffset >= pbdpDrive->dwSectors )
	{
		dwSectorOffset = (DWORD)(pbdpDrive->dwSectors - 1);
	}
	//otherwise align to cylinder head
	dwSectorOffsetAligned = dwSectorOffset
							- (dwSectorOffset % dwSectorsPerCylinder);
	//align to tail
	if(btFlag == ALIGN_BACKWARD)
	{
		dwSectorOffsetAligned += dwSectorsPerCylinder - 1;
	}else
	{
		//check if in the first cylinder
		if( dwSectorOffsetAligned < dwSectorsPerCylinder )
		{
			if( btExtend == ALIGN_PRIMARY )
			{
				dwSectorOffsetAligned = pbdpDrive->dwSecPerTrack;
			}else 
			{
				//extend or logical
				dwSectorOffsetAligned = dwSectorsPerCylinder;
			}
		}
		//align head
		if( btExtend == ALIGN_LOGICAL)
		{
			//logical start is at head1
			dwSectorOffsetAligned += pbdpDrive->dwSecPerTrack;
		}
	}

	return dwSectorOffsetAligned;
}

BOOL DLL_PARINFO_API GetPartitionInfoEx(BYTE					btHardDrive,
										PPARINFOONHARDDISKEX	pParHardEx)
{
	PARINFOONHARDDISK			OldParHard;
	int							nCount ;
	DWORD						dwStartSec;
	int							i = 0;

	memset(&OldParHard,0,sizeof(PARINFOONHARDDISK));
	//get partition information in old format
	if(!GetPartitionInfo(btHardDrive,&OldParHard))
	{
		return FALSE;
		ASSERT(FALSE);
	}
	//translate primary partition information
	pParHardEx->wNumOfLogic = OldParHard.wNumOfLogic ;
	pParHardEx->wNumOfPri = OldParHard.wNumOfPri ;
	memcpy(	&(pParHardEx->pePriParInfo),
			&OldParHard.pePriParInfo,
			4*sizeof(PARTITION_ENTRY));

	for( nCount=0 ; nCount<OldParHard.wNumOfPri ; nCount++ )
	{
		if( OldParHard.pePriParInfo[nCount].SystemFlag == 0x05 ||
			OldParHard.pePriParInfo[nCount].SystemFlag == 0x0f )
			break;
	}
	if( nCount < 4 && OldParHard.wNumOfLogic != 0 )
	{	
		//store extend partition start
		dwStartSec = OldParHard.pePriParInfo[nCount].StartSector;
		//traslate logical partition information
		while( i < OldParHard.wNumOfLogic )
		{
			memcpy( &pParHardEx->peLogParInfo[i].peCurParInfo,
					&OldParHard.peLogParInfo[i].LogParInfo,
					sizeof(PARTITION_ENTRY));
			//if i == 0 then the extend start is his pre start
			pParHardEx->peLogParInfo[i].dwPreStart = 
				i ? OldParHard.peLogParInfo[i-1].dwLogicStart : dwStartSec;
			//next start
			pParHardEx->peLogParInfo[i].dwNextStart = 
				OldParHard.peLogParInfo[i+1].dwLogicStart;
			//cur start	
			pParHardEx->peLogParInfo[i].dwCurOffset = 
				OldParHard.peLogParInfo[i].LogParInfo.StartSector;
			//logic start
			pParHardEx->peLogParInfo[i].peCurParInfo.StartSector = 
				OldParHard.peLogParInfo[i].dwLogicStart;

			i++;
		}
	}
	return TRUE;
}

/*-------------------------------------------------------------------------------
GetDriveParam()
Purpose:
	Get the physical hard disk's bios parameter;
Parameters:
	BYTE btHardDrive:
		Physical disk number, 0x00-0x7f or 0x80-0xff;
	PBIOS_DRIVE_PARAM pDriveParam:
		Buffer which contains the returned structure;
Return Value:
	BOOL:
		TRUE if success, else FALSE;
-------------------------------------------------------------------------------*/
BOOL DLL_PARINFO_API GetDriveParam(	BYTE					btHardDrive,
									PBIOS_DRIVE_PARAM		pDriveParam)
{
	BOOL				bResult = FALSE;
	HANDLE				hDev;
	DWORD				dwBytesReturned = 0;
	WORD				wFlags = 0;
	DISK_GEOMETRY		dgDrive;

	hDev = GetDriveHandle(btHardDrive);
	ASSERT( hDev != INVALID_HANDLE_VALUE );
	
	ZeroMemory(pDriveParam,sizeof(PBIOS_DRIVE_PARAM ));

	if( hDev != INVALID_HANDLE_VALUE )
	{
		bResult = DeviceIoControl( hDev,
									IOCTL_DISK_GET_DRIVE_GEOMETRY,
									NULL,0,
									&dgDrive,sizeof(DISK_GEOMETRY),
									&dwBytesReturned,
									NULL);

		//close handle
		CloseHandle(hDev);

		if(bResult)
		{
			pDriveParam->wInfoSize = sizeof(BIOS_DRIVE_PARAM);
			pDriveParam->dwHeads = dgDrive.TracksPerCylinder;
			pDriveParam->dwCylinders = (DWORD)dgDrive.Cylinders.QuadPart;
			pDriveParam->dwSecPerTrack = dgDrive.SectorsPerTrack;
			pDriveParam->SectorsSize = (WORD)dgDrive.BytesPerSector;
			pDriveParam->dwSectors = dgDrive.Cylinders.QuadPart
										* dgDrive.TracksPerCylinder
										* dgDrive.SectorsPerTrack;
			switch(dgDrive.MediaType)
			{
			case FixedMedia:
				break;
			default:
				//remove able media
				wFlags |= 0x04;
				break;
			}
			pDriveParam->wFlags = wFlags;
		}
	}
	return bResult;
}

/*---------------------------------------------------------------------------------
GetHardDiskNum()
Purpose:
	Get the number of physical disks installed.
Parameters:
	No.
Return Value:
	if succeed return the actual disk number.
----------------------------------------------------------------------------------*/
DWORD  DLL_PARINFO_API GetHardDiskNum()
{
	DWORD				dwPhysicalDiskNum = 0;
	HANDLE				hDev;

	for( int i=0 ; i<MAX_PHYSICALDISK_NUM ; i++ )
	{
		hDev = GetDriveHandle(i);
		if(hDev != INVALID_HANDLE_VALUE )
		{
			dwPhysicalDiskNum++;
			//close handle
			CloseHandle(hDev);
		}
	}
	return dwPhysicalDiskNum;
}
/*-----------------------------------------------------------------
GetStartupDrive()
Purpose:
	Reture the first hard disk's 0x80 drive letter;
Parameters:
	No;
Return value:
	Allways 'C' drive;
-----------------------------------------------------------------*/
//DEL BYTE DLL_PARINFO_API GetStartupDrive()
//DEL {
//DEL 	return 0xff;
//DEL }

/*-----------------------------------------------------------------------------------
IsValidEMBR()
Purpose:
	Validate a EMBR sector
Parameters:
	DWORD dwExtStart:
		Unknown, not use in function;
	PPARTITION_SEC pEMBR:
		Buffer holding the EMBR sector data;
	PBIOS_DRIVE_PARAM pDriveParam:
		Hard disk's parameters;
Return Value:
	TRUE if it's Valid, else FALSE;
-----------------------------------------------------------------------------------*/
BOOL DLL_PARINFO_API IsValidEMBR(DWORD						dwExtStart,
								 PPARTITION_SEC				pEMBR,
								 PBIOS_DRIVE_PARAM			pDriveParam)
{
	BOOL					bResult = FALSE;
	PARTITION_ENTRY			PartiEntry[2];
	DWORD					dwMaxSectors = (DWORD)pDriveParam->dwSectors;

	//Check signature
	bResult = pEMBR->Signature == 0xAA55;
	if ( !bResult )
		return FALSE;
	//check last two entry
	memset( &PartiEntry,0,sizeof(PARTITION_ENTRY)*2 );
	bResult = memcmp( &pEMBR->Partition[2],
						&PartiEntry[0],
						sizeof(PARTITION_ENTRY)*2 );
	if ( bResult )
		return FALSE;
	//check first
	if ( memcmp( &pEMBR->Partition[0],
				 &PartiEntry[0],
				 sizeof(PARTITION_ENTRY)) )
	{
		bResult = ( pEMBR->Partition[0].StartSector < dwMaxSectors )&&
					(pEMBR->Partition[0].SectorsInPartition < dwMaxSectors)&&
					(pEMBR->Partition[0].SystemFlag);
	}else 
		bResult = TRUE;
	//check second
	if ( memcmp( &pEMBR->Partition[1],
				 &PartiEntry[0],
				 sizeof(PARTITION_ENTRY)) && 
				 pEMBR->Partition[1].SystemFlag )
	{
		bResult = !( (pEMBR->Partition[1].StartSector > dwMaxSectors)||
						(pEMBR->Partition[1].SectorsInPartition > dwMaxSectors));
	}
	return bResult;
}

BOOL DLL_PARINFO_API PartitionInfoByDriveLetter(BYTE					btDriveLetter,
												PPARTITION_INFO_2000	pPi2000)
{
	TCHAR							szDriveLetter[] = "\\\\.\\X:";
	HANDLE							hDriveX;
	DWORD							dwBytesReturned;

	if( !
		(( btDriveLetter >= 'a' && btDriveLetter <= 'z' )||
		( btDriveLetter >= 'A' && btDriveLetter <= 'Z' ))
		)
		return FALSE;

	szDriveLetter[4] = btDriveLetter;
	//open drive handle
	hDriveX = CreateFile(	szDriveLetter,
							GENERIC_READ,
							FILE_SHARE_READ|FILE_SHARE_WRITE,
							NULL,
							OPEN_EXISTING,
							NULL,NULL);

	if(hDriveX != INVALID_HANDLE_VALUE)
	{
		if( DeviceIoControl(hDriveX,
							IOCTL_DISK_GET_PARTITION_INFO,
							NULL,0,
							&pPi2000->pi,sizeof(PARTITION_INFORMATION),
							&dwBytesReturned,
							NULL ))
		{
			//close handle
			CloseHandle( hDriveX );
			//get drive information
			pPi2000->nHardDiskNum = DriveOnWhichHardDisk( btDriveLetter,
														  pPi2000->pi.PartitionNumber );
			//if fail return
			if ( pPi2000->nHardDiskNum == -1 )
				return FALSE;
			
			if( (pPi2000->pi.StartingOffset.QuadPart
					- (__int64)pPi2000->pi.HiddenSectors * SECTOR_SIZE ) != 0 )
			{
				//logical hidden sectors is 0x3f
				//but start offset is greater than 0x3f
				pPi2000->nPartitionType = LOGICAL;
			}else
			{
				if( (pPi2000->pi.PartitionType == PARTITION_EXTENDED )||
					(pPi2000->pi.PartitionType == PARTITION_XINT13_EXTENDED ))
				{
					//extend has no drive letter
					return FALSE;
				}else
				{
					//primary partition
					pPi2000->nPartitionType = PRIMARY;
				}
			}
			return TRUE;
		}
	}
	return FALSE;
}

BOOL DLL_PARINFO_API UnmountVolume( TCHAR tchDriveLetter )
{
	TCHAR	tszMountPoint[] = "X:\\";
	
	tszMountPoint[0] = tchDriveLetter;
	return DeleteVolumeMountPoint(tszMountPoint);
}

BOOL DLL_PARINFO_API MountVolume(BYTE		btHardDiskNum,
								 DWORD		dwStartSector,
								 TCHAR		tchDriveLetter)
{
	TCHAR							tszMountPoint[] = "X:\\";
	TCHAR							tszVolumeName[MAX_PATH];
	char							szAnsi[MAX_PATH];
	UNICODE_STRING					usLinkName;
	WCHAR							LinkName[MAX_PATH];
	UNICODE_STRING					usLinkContent;
	WCHAR							LinkContent[MAX_PATH];
	UNICODE_STRING					usLinkContentSave;
	WCHAR							LinkContentSave[MAX_PATH];
	NTSTATUS						status;
	GET_DRIVE_LAYOUT				dliDrive;
	BOOL							bResult;
	int								nCount;
	int								nPartitionNum;
	HANDLE							hSearch;
	BOOL							bFound = FALSE;


	if( tchDriveLetter >= 'a' &&
		tchDriveLetter <= 'z' )
	{
		tchDriveLetter += 'A' - 'a';
	}
	if( tchDriveLetter < 'A' ||
		tchDriveLetter > 'Z' )
	{
		return FALSE;
	}
	
	tszMountPoint[0] = tchDriveLetter;
	btHardDiskNum &= 0x7f;
	//search in drive layout
	bResult = GetDriveLayout(btHardDiskNum,&dliDrive);
	if( !bResult )
	{
		return FALSE;
	}
	//start search
	nCount = FindInDriveLayout(dwStartSector, &dliDrive);
	if( nCount < 0 )
	{
		return FALSE;
	}
	if(	!dliDrive.dliDrive.PartitionEntry[nCount].RecognizedPartition )
	{
//		return FALSE;
	}
	//init unicode strings
	usLinkName.MaximumLength = sizeof(LinkName);
	usLinkName.Buffer = LinkName;
	usLinkContent.MaximumLength = sizeof(LinkContent);
	usLinkContent.Buffer = LinkContent;
	usLinkContentSave.MaximumLength = sizeof(LinkContentSave);
	usLinkContentSave.Buffer = LinkContentSave;
	//query by hard disk
	nPartitionNum = dliDrive.dliDrive.PartitionEntry[nCount].PartitionNumber;
	sprintf(szAnsi,"\\Device\\Harddisk%d\\Partition%d",btHardDiskNum,nPartitionNum);
	AnsiToUnicode(&usLinkName,szAnsi);
	status = QuerySymbolicLink(&usLinkName,&usLinkContentSave);
	if( !NT_SUCCESS( status ))
	{
		return FALSE;
	}
	//found the volume
	hSearch = FindFirstVolume(tszVolumeName,MAX_PATH );
	if( hSearch == INVALID_HANDLE_VALUE )
	{
		return FALSE;
	}
	do{
		tszVolumeName[1] = '?';
		tszVolumeName[48] = '\0';
		AnsiToUnicode(&usLinkName,tszVolumeName);
		status = QuerySymbolicLink(&usLinkName,&usLinkContent);
		if( NT_SUCCESS( status ) && 
			( usLinkContent.Length == usLinkContentSave.Length ) )
		{
			if( !memcmp( LinkContentSave,LinkContent,usLinkContent.Length ))
			{
				bFound = TRUE;
				break;
			}
		}
	}while( FindNextVolume( hSearch, tszVolumeName, MAX_PATH ));
	FindVolumeClose( hSearch );

	if( bFound )
	{
		//found and set point
		tszVolumeName[1] = '\\';
		tszVolumeName[48] = '\\';
		return SetVolumeMountPoint(tszMountPoint, tszVolumeName);
	}else
	{
		return FALSE;
	}
}

#ifndef _VMM_
}
#endif // _VMM_