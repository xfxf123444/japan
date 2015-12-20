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
#include "ParInfoPrivate.h"
#include "SymbolicLink.h"
#include "Volume.h"

BOOL g_bVista = FALSE;
DWORD SECTOR_SIZE = 512;

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

extern	"C"
{
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

	dwBytesToWrite = wSectors * pDriveParam->SectorsSize;
	lnStartBytes.QuadPart = (__int64)dwStartSec * pDriveParam->SectorsSize;

	if(lnStartBytes.QuadPart >= 0)
	{
		hDrv = GetDriveHandle(btUnit);
		ASSERT( hDrv != INVALID_HANDLE_VALUE);
		if(hDrv != INVALID_HANDLE_VALUE)
		{
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

	dwBytesToRead = wSectors * pDriveParam->SectorsSize;
	lnStartBytes.QuadPart = (__int64)dwStartSec * pDriveParam->SectorsSize;

	if(lnStartBytes.QuadPart >= 0)
	{
		hDrv = GetDriveHandle(btUnit);
		ASSERT( hDrv != INVALID_HANDLE_VALUE);
		
		if(hDrv != INVALID_HANDLE_VALUE)
		{
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

	if (dliDrive.dliDrive.PartitionStyle == PARTITION_STYLE_GPT){
		GUID_PARTITION_TYPE guidType = GetGUIDPartitionType(dliDrive.dliDrive.PartitionEntry[nCount].Gpt.PartitionType);
		if (guidType != PARTITION_BASIC_DATA_GUID
			&& guidType != PARTITION_LDM_DATA_GUID){
				return 0xff;
		}
	}
	else if (dliDrive.dliDrive.PartitionStyle == PARTITION_STYLE_MBR){
		if (!dliDrive.dliDrive.PartitionEntry[nCount].Mbr.RecognizedPartition){
			return 0xff;
		}
	}
	else
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
		pInfo->PartitionStyle = pi2000.pi.PartitionStyle;
		pInfo->DriveNum = pi2000.nHardDiskNum;
		pInfo->DriveNum |= 0x80;	//	80H , 81H
		if (pi2000.pi.PartitionStyle == PARTITION_STYLE_MBR) {
			pInfo->ParType = pi2000.pi.Mbr.PartitionType;	//	04H , 06H 
		}
		else if (pi2000.pi.PartitionStyle == PARTITION_STYLE_GPT) {
			pInfo->ParType = 0x07;
		}
		else {
			pInfo->ParType = 0;
		}
		pInfo->wReserve = 0;	
		pInfo->dwStart = (DWORD)(pi2000.pi.StartingOffset.QuadPart / SECTOR_SIZE);	//	linear sector number
		return TRUE;
	}else
		return FALSE;
}

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
	DWORD					startAddress;
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
	
	startAddress=sdDriveMapInfo.dwStart;
	//It's primary partition
	for(i=0;i<ParHard.wNumOfPri;i++)
	{
		if(ParHard.pePriParInfo[i].StartSector==startAddress)
		{
			pParInfo->PartitionStyle = ParHard.pePriParInfo[i].PartitionStyle;
			pParInfo->GUIDType = ParHard.pePriParInfo[i].GUIDType;
			pParInfo->btDrive = btLetter+'A'-1;
			pParInfo->btHardDisk = sdDriveMapInfo.DriveNum;
			pParInfo->nPartition = i;
			pParInfo->dwPartitionType = PRIMARY;
			pParInfo->dwSectorsInPartition = ParHard.pePriParInfo[i].SectorsInPartition;
			pParInfo->btSystemFlag=ParHard.pePriParInfo[i].SystemFlag;  
			pParInfo->dwStartSector = ParHard.pePriParInfo[i].StartSector;

			char	szStr[6]=" :\\\0";
			char	lpVolumeNameBuffer[MAX_LABELNAME];
			memset(lpVolumeNameBuffer,0x20,MAX_LABELNAME);
			szStr[0]=(char)(btLetter+'A'-1);
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
				if(pParInfo->btSystemFlag==MBR_PT_PARTITION_FAT32)
				{
					BOOT_SEC32 bsBoot32;
					if(ReadSector(pParInfo->dwStartSector,0x01,(PBYTE)&bsBoot32,sdDriveMapInfo.DriveNum,&DriveParam))
					{
						memcpy(pParInfo->szLabelName,bsBoot32.VolumeLabel,0x0b);	
					}
				}
				if(pParInfo->btSystemFlag==MBR_PT_PARTITION_FAT_16)
				{
					BOOT_SEC16 bsBoot16;
					if(ReadSector(pParInfo->dwStartSector,0x01,(PBYTE)&bsBoot16,sdDriveMapInfo.DriveNum,&DriveParam))
					{
						memcpy(pParInfo->szLabelName,bsBoot16.VolumeLabel,0x0b);	
					}
				}
			}
		return TRUE;
		}
	}
	startAddress=startAddress-0x3f;
	for(i=0;i<ParHard.wNumOfLogic;i++)
	{
		if(ParHard.peLogParInfo[i].peCurParInfo.StartSector==startAddress)
		{
			pParInfo->PartitionStyle = PARTITION_STYLE_MBR;
			pParInfo->btDrive = btLetter+'A'-1;
			pParInfo->btHardDisk = sdDriveMapInfo.DriveNum;
			pParInfo->nPartition = i;
			pParInfo->dwPartitionType = LOGICAL;
			pParInfo->dwSectorsInPartition = ParHard.peLogParInfo[i].peCurParInfo.SectorsInPartition;
			pParInfo->btSystemFlag= ParHard.peLogParInfo[i].peCurParInfo.SystemFlag ; 
			pParInfo->dwStartSector = ParHard.peLogParInfo[i].peCurParInfo.StartSector; //it's really !!
			
			char	szStr[6]=" :\\\0";
			char	lpVolumeNameBuffer[MAX_LABELNAME];
			memset(lpVolumeNameBuffer,0x20,MAX_LABELNAME);
			szStr[0]=(char)(btLetter+'A'-1);
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
				if(pParInfo->btSystemFlag==MBR_PT_PARTITION_FAT32)
				{
					BOOT_SEC32 bsBoot32;
					if(ReadSector(pParInfo->dwStartSector+0x3f,0x01,(PBYTE)&bsBoot32,sdDriveMapInfo.DriveNum,&DriveParam))
					{
						memcpy(pParInfo->szLabelName,bsBoot32.VolumeLabel,0x0b);	
					}
				}
				if(pParInfo->btSystemFlag==MBR_PT_PARTITION_FAT_16)
				{
					BOOT_SEC16 bsBoot16;
					if(ReadSector(pParInfo->dwStartSector+0x3f,0x01,(PBYTE)&bsBoot16,sdDriveMapInfo.DriveNum,&DriveParam))
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
			OldParHard.wNumOfPri*sizeof(PARTITION_ENTRY));

	if (OldParHard.wNumOfPri > 0 && OldParHard.pePriParInfo[0].PartitionStyle == PARTITION_STYLE_MBR) {
		for( nCount=0 ; nCount<OldParHard.wNumOfPri ; nCount++ )
		{
			if( OldParHard.pePriParInfo[nCount].SystemFlag == MBR_PT_PARTITION_EXTENDED)
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
	DISK_GEOMETRY_EX		dgDrive;

	hDev = GetDriveHandle(btHardDrive);
	ASSERT( hDev != INVALID_HANDLE_VALUE );
	
	ZeroMemory(pDriveParam,sizeof(PBIOS_DRIVE_PARAM ));

	if( hDev != INVALID_HANDLE_VALUE )
	{
		bResult = DeviceIoControl( hDev,
									IOCTL_DISK_GET_DRIVE_GEOMETRY_EX,
									NULL,0,
									&dgDrive,sizeof(DISK_GEOMETRY_EX),
									&dwBytesReturned,
									NULL);

		//close handle
		CloseHandle(hDev);

		if(bResult)
		{
// 			DISK_PARTITION_INFO* ppiDrive = DiskGeometryGetPartition(&dgDrive);
// 			DISK_DETECTION_INFO* pdiDrive = DiskGeometryGetDetect(&dgDrive) ;

			DISK_GEOMETRY& geo = dgDrive.Geometry;
 			pDriveParam->wInfoSize = sizeof(BIOS_DRIVE_PARAM);
 			pDriveParam->dwHeads = geo.TracksPerCylinder;
 			pDriveParam->dwCylinders = (DWORD)geo.Cylinders.QuadPart;
 			pDriveParam->dwSecPerTrack = geo.SectorsPerTrack;
 			pDriveParam->SectorsSize = (WORD)geo.BytesPerSector;
			if (pDriveParam->SectorsSize) {
				SECTOR_SIZE = pDriveParam->SectorsSize;
			}
 			pDriveParam->dwSectors = geo.Cylinders.QuadPart
 										* geo.TracksPerCylinder
 										* geo.SectorsPerTrack;
 			switch(geo.MediaType)
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
							IOCTL_DISK_GET_PARTITION_INFO_EX,
							NULL,0,
							&pPi2000->pi,sizeof(PARTITION_INFORMATION_EX),
							&dwBytesReturned,
							NULL ))
		{
			//close handle
			CloseHandle( hDriveX );
			//get drive information
			pPi2000->nHardDiskNum = DriveOnWhichHardDisk( btDriveLetter,
														  pPi2000->pi.PartitionNumber );
			//if fail return
			if ( pPi2000->nHardDiskNum == -1 ) {
				return FALSE;
			}
			
			if (pPi2000->pi.PartitionStyle == PARTITION_STYLE_GPT) {
				pPi2000->nPartitionType = PRIMARY;
				return TRUE;
			}
			else if (pPi2000->pi.PartitionStyle == PARTITION_STYLE_MBR) {
				if( (pPi2000->pi.StartingOffset.QuadPart
					- (__int64)pPi2000->pi.Mbr.HiddenSectors * SECTOR_SIZE ) != 0 )
				{
					//logical hidden sectors is 0x3f
					//but start offset is greater than 0x3f
					pPi2000->nPartitionType = LOGICAL;
				}else
				{
					if( (pPi2000->pi.Mbr.PartitionType == PARTITION_EXTENDED )||
						(pPi2000->pi.Mbr.PartitionType == PARTITION_XINT13_EXTENDED ))
					{
						//extend has no drive letter
						return FALSE;
					}
					else
					{
						//primary partition
						pPi2000->nPartitionType = PRIMARY;
					}
				}
				return TRUE;
			}
			else {
				return FALSE;
			}
		}
	}
	return FALSE;
}

}

void GUIDToStr(const GUID& guid, char* strGUID)
{
	sprintf(strGUID, "%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX", 
		guid.Data1, guid.Data2, guid.Data3, 
		guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
}

GUID_PARTITION_TYPE GetGUIDPartitionType(const GUID& id)
{
	char strGUID[100];
	ZeroMemory(strGUID, 100);
	GUIDToStr(id, strGUID);
	if (stricmp(strGUID, "ebd0a0a2-b9e5-4433-87c0-68b6b72699c7") == 0) {
		return PARTITION_BASIC_DATA_GUID;
	}
	else if (stricmp(strGUID, "00000000-0000-0000-0000-000000000000") == 0) {
		return PARTITION_ENTRY_UNUSED_GUID;
	}
	else if (stricmp(strGUID, "c12a7328-f81f-11d2-ba4b-00a0c93ec93b") == 0) {
		return PARTITION_SYSTEM_GUID;
	}
	else if (stricmp(strGUID, "e3c9e316-0b5c-4db8-817d-f92df00215ae") == 0) {
		return PARTITION_MSFT_RESERVED_GUID;
	}
	else if (stricmp(strGUID, "5808c8aa-7e8f-42e0-85d2-e1e90434cfb3") == 0) {
		return PARTITION_LDM_METADATA_GUID;
	}
	else if (stricmp(strGUID, "ebaf9b60a0-1431-4f62-bc68-3311714a69ad") == 0) {
		return PARTITION_LDM_DATA_GUID;
	}
	else if (stricmp(strGUID, "de94bba4-06d1-4d40-a16a-bfd50179d6ac") == 0) {
		return PARTITION_MSFT_RECOVERY_GUID;
	}
	else {
		return PARTITION_ENTRY_UNUSED_GUID;
	}
}

