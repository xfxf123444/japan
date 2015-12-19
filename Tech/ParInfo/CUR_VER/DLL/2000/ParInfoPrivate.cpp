/*======================================================================
ParInfoPrivate.cpp
=======================================================================*/

#include "stdafx.h"
#include "stdio.h"
#include "tchar.h"
#include "assert.h"

#ifndef ASSERT
#define ASSERT assert
#endif

//#define DLL_PARINFO_API __declspec(dllexport)
#include "ParInfoPrivate.h"
//include query symbolic head file
#include "SymbolicLink.h"

HANDLE GetDriveHandle(BYTE btDrive)
{
	TCHAR			szDriveName[MAX_PATH];

	btDrive = MAKE_DRIVE_NUM(btDrive);
	sprintf(szDriveName,"\\\\.\\PhysicalDrive%d",btDrive);
	//open handle in rw mode
	return CreateFile(	szDriveName,
						GENERIC_READ|GENERIC_WRITE,
						FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,OPEN_EXISTING,
						0,
						NULL);
}

VOID SearchPartitions(GET_DRIVE_LAYOUT			*pdliDrive,
					  PPARINFOONHARDDISK		ppihd,
					  PBIOS_DRIVE_PARAM			pbdp)
{
	int							nPartitionCount = 0;
	PARTITION_INFORMATION_EX		*ppi;
	LOGICPARINFO				*ppeLog; 
	PARTITION_ENTRY				*ppePri;
	//init pointers
	ppeLog = ppihd->peLogParInfo;
	ppePri = ppihd->pePriParInfo;
	//init partition numbers
	ppihd->wNumOfLogic = 0;
	ppihd->wNumOfPri = 0;

	//search mbr partitions
	if (pdliDrive->dliDrive.PartitionStyle == PARTITION_STYLE_MBR) {
		ppi = pdliDrive->dliDrive.PartitionEntry;
		//usb disk and single partition
		if( pdliDrive->dliDrive.PartitionCount == 1 )
		{
			//store primary partition information
			MakePartitionPhysicalEntry(ppi,ppePri,pbdp);
			ppihd->wNumOfPri++;
			return;
		}
		for( nPartitionCount = 0 ; nPartitionCount < 4 ; nPartitionCount++,ppi++)
		{
			BYTE PartitionType = ppi->Mbr.PartitionType;
			if(PartitionType == PARTITION_ENTRY_UNUSED )
				continue;
			//store primary partition information
			MakePartitionPhysicalEntry(ppi,ppePri,pbdp);
			ppihd->wNumOfPri++;
			ppePri++;
		}
		//search logicals
		ppi = &pdliDrive->dliDrive.PartitionEntry[4];//first logical
		for( nPartitionCount = pdliDrive->dliDrive.PartitionCount;
			nPartitionCount >= 4 ;
			nPartitionCount--, ppi++)
		{
			BYTE PartitionType = ppi->Mbr.PartitionType;
			if( (PartitionType == PARTITION_ENTRY_UNUSED)||
				( PartitionType == PARTITION_EXTENDED )||
				( PartitionType == PARTITION_XINT13_EXTENDED))
				continue;
			// logical partition		
			ppeLog->dwLogicStart = BytesOffsetToSectorsOffset(pbdp,ppi->StartingOffset);
			MakePartitionPhysicalEntry( ppi, &ppeLog->LogParInfo, pbdp);
			ppihd->wNumOfLogic++;
			ppeLog++;
		}
	}
	else {
		ppi = pdliDrive->dliDrive.PartitionEntry;
		for( nPartitionCount = 0 ; nPartitionCount < pdliDrive->dliDrive.PartitionCount; nPartitionCount++,ppi++)
		{
			//store primary partition information
			MakePartitionPhysicalEntry(ppi,ppePri,pbdp);
			ppihd->wNumOfPri++;
			ppePri++;
		}
	}
}

DWORD BytesOffsetToSectorsOffset(PBIOS_DRIVE_PARAM			pbdp,
								 LARGE_INTEGER				lnBytesOffset)
{
	DWORD			dwSectorsOffset = -1;

	ASSERT(pbdp!=NULL);
	if(pbdp!=NULL)
	{
		dwSectorsOffset = (DWORD)(lnBytesOffset.QuadPart / pbdp->SectorsSize);
	}
	return dwSectorsOffset;
}

BOOL MakePartitionPhysicalEntry(PARTITION_INFORMATION_EX		*ppi,
								PARTITION_ENTRY				*ppe,
								PBIOS_DRIVE_PARAM			pbdp)
{
	DWORD				dwStart,dwLength;
	//get start and length in sectors
	dwStart = BytesOffsetToSectorsOffset( pbdp, ppi->StartingOffset );
	dwLength = BytesOffsetToSectorsOffset( pbdp, ppi->PartitionLength);
	//tanslate to physical
	ppe->PartitionStyle = ppi->PartitionStyle;
	if (ppe->PartitionStyle == PARTITION_STYLE_GPT) {
		ppe->GUIDType = GetGUIDPartitionType(ppi->Gpt.PartitionType);
		ppe->SystemFlag = 0x07;
	}
	else if (ppe->PartitionStyle == PARTITION_STYLE_MBR){
		if(ppi->Mbr.BootIndicator) {
			ppe->BootFlag = 0x80;
		}
		ppe->SystemFlag = ppi->Mbr.PartitionType;
	}
	ppe->StartSector = dwStart;
	ppe->SectorsInPartition = dwLength;
	return TRUE;
}

int DriveOnWhichHardDisk(char		chDriveLetter,
						 int		nPartitionNum)
{
	DWORD					dwHardDiskNum;
	char					szAnsi[MAX_PATH];
	UNICODE_STRING			usLinkName;
	WCHAR					LinkName[MAX_PATH];
	UNICODE_STRING			usLinkContent;
	WCHAR					LinkContent[MAX_PATH];
	UNICODE_STRING			usLinkContentSave;
	WCHAR					LinkContentSave[MAX_PATH];
	NTSTATUS				status;
	DWORD					dwTemp;

	//check parameters
	if( nPartitionNum <= 0 )
		return -1;
	//make uper case
	if( ( chDriveLetter >= 'a' && chDriveLetter <= 'z' ))
		chDriveLetter += 'A' - 'a';
	//check drive letter
	if( !( chDriveLetter >= 'A' && chDriveLetter <= 'Z' ))
		return -1;		
	//Get hard disk num
	dwHardDiskNum = GetHardDiskNum();
	//init unicode strings
	usLinkName.MaximumLength = sizeof(LinkName);
	usLinkName.Buffer = LinkName;
	usLinkContent.MaximumLength = sizeof(LinkContent);
	usLinkContent.Buffer = LinkContent;
	usLinkContentSave.MaximumLength = sizeof(LinkContentSave);
	usLinkContentSave.Buffer = LinkContentSave;
	//Query by drive letter
	sprintf(szAnsi,"\\??\\%c:",chDriveLetter);
	AnsiToUnicode(&usLinkName,szAnsi);
	status = QuerySymbolicLink(&usLinkName,&usLinkContentSave);
	if ( !NT_SUCCESS( status ))
	{
		return -1;
	}
	//Query by hard disks
	for( dwTemp = 0 ; dwTemp < dwHardDiskNum ; dwTemp++ )
	{
		sprintf(szAnsi,"\\Device\\Harddisk%d\\Partition%d",dwTemp,nPartitionNum);
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
	if ( dwTemp == dwHardDiskNum )
		return -1;
	return dwTemp;
}

BOOL GetDriveLayout(int						nHardDisk,
					PGET_DRIVE_LAYOUT		pdli)
{
	HANDLE		hDev;
	BOOL		bResult = FALSE;
	DWORD		dwBytesReturned;

	hDev = GetDriveHandle(nHardDisk);
	ASSERT( hDev != INVALID_HANDLE_VALUE );

	if(hDev != INVALID_HANDLE_VALUE)
	{
		//get disk geometry
		bResult = DeviceIoControl(	hDev,
									IOCTL_DISK_GET_DRIVE_LAYOUT_EX,
									NULL,
									0,
									pdli,
									sizeof(GET_DRIVE_LAYOUT),
									&dwBytesReturned,
									NULL);
		//close handle
		CloseHandle(hDev);
	}
	return bResult;
}

int FindInDriveLayout(DWORD					dwStart,
					  PGET_DRIVE_LAYOUT		pdli)
{
	LARGE_INTEGER					lnStart;
	LARGE_INTEGER					lnTemp;
	int								nParType;
	int								nCount = -1;

	lnStart.QuadPart = (__int64) dwStart * SECTOR_SIZE;
	if (pdli->dliDrive.PartitionStyle == PARTITION_STYLE_MBR) {
		for( nCount = pdli->dliDrive.PartitionCount - 1 ; nCount >= 0 ; nCount-- )
		{
			lnTemp.QuadPart = pdli->dliDrive.PartitionEntry[nCount].StartingOffset.QuadPart;
			if( lnStart.QuadPart == lnTemp.QuadPart )
				break;
		}
		if( nCount < 0 )
		{
			return -1;
		}
		//if is a extend entry
		nParType = pdli->dliDrive.PartitionEntry[nCount].Mbr.PartitionType;
		if( nParType == 0x05 || nParType == 0x0f )
		{
			nCount = nCount - ( nCount % 4 ) + 4 ;
			nParType = pdli->dliDrive.PartitionEntry[nCount].Mbr.PartitionType;
		}
		if( nParType == 0x00 )
		{
			return -1;
		}
	}
	else if (pdli->dliDrive.PartitionStyle == PARTITION_STYLE_GPT) {
		for( nCount = pdli->dliDrive.PartitionCount - 1 ; nCount >= 0 ; nCount-- )
		{
			lnTemp.QuadPart = pdli->dliDrive.PartitionEntry[nCount].StartingOffset.QuadPart;
			if( lnStart.QuadPart == lnTemp.QuadPart )
				break;
		}
		if( nCount < 0 )
		{
			return -1;
		}
	}
	return nCount;
}
