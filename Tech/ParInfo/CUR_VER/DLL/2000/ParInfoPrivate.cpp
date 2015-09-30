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

#define DLL_PARINFO_API __declspec(dllexport)
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
	LARGE_INTEGER				lnExtendedBegin,lnExtendedEnd;
	PARTITION_INFORMATION		*ppi;
	PARTITION_INFORMATION		*ppiExtended;
	LOGICPARINFO				*ppeLog; 
	PARTITION_ENTRY				*ppePri;
	//init pointers
	ppiExtended = NULL;
	ppeLog = ppihd->peLogParInfo;
	ppePri = ppihd->pePriParInfo;
	//init partition numbers
	ppihd->wNumOfLogic = 0;
	ppihd->wNumOfPri = 0;
	//search mbr partitions
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
		if( ppi->PartitionType == PARTITION_ENTRY_UNUSED )
			continue;
		//store extend partition entrys
		if( ( ppi->PartitionType == PARTITION_EXTENDED )||
			( ppi->PartitionType == PARTITION_XINT13_EXTENDED))
		{
			ppiExtended = ppi;
		}
		//store primary partition information
		MakePartitionPhysicalEntry(ppi,ppePri,pbdp);
		ppihd->wNumOfPri++;
		ppePri++;
	}
	//get extend start and end
	if(ppiExtended != NULL)
	{
		lnExtendedBegin.QuadPart = ppiExtended->StartingOffset.QuadPart;
		lnExtendedEnd.QuadPart = ppiExtended->PartitionLength.QuadPart
									+ ppiExtended->StartingOffset.QuadPart
									- 1;
	}else
	{
		lnExtendedBegin.QuadPart = -1;
		lnExtendedEnd.QuadPart = -1;
	}
	//search logicals
	ppi = &pdliDrive->dliDrive.PartitionEntry[4];//first logical
	for( nPartitionCount = pdliDrive->dliDrive.PartitionCount;
		 nPartitionCount >= 4 ;
		 nPartitionCount--, ppi++)
	{
		if( (ppi->PartitionType == PARTITION_ENTRY_UNUSED)||
			( ppi->PartitionType == PARTITION_EXTENDED )||
			( ppi->PartitionType == PARTITION_XINT13_EXTENDED))
			continue;
		// logical partition		
		ppeLog->dwLogicStart = BytesOffsetToSectorsOffset(pbdp,ppi->StartingOffset);
		//use his embr start sector
		ppeLog->dwLogicStart -= ppi->HiddenSectors;
		MakePartitionPhysicalEntry( ppi, &ppeLog->LogParInfo, pbdp);
		ppihd->wNumOfLogic++;
		ppeLog++;
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

BOOL MakePartitionPhysicalEntry(PARTITION_INFORMATION		*ppi,
								PARTITION_ENTRY				*ppe,
								PBIOS_DRIVE_PARAM			pbdp)
{
	DWORD				dwStart,dwLength;
	//get start and length in sectors
	dwStart = BytesOffsetToSectorsOffset( pbdp, ppi->StartingOffset );
	dwLength = BytesOffsetToSectorsOffset( pbdp, ppi->PartitionLength);
	//tanslate to physical
	if(ppi->BootIndicator)
		ppe->BootFlag = 0x80;
	ppe->SystemFlag = ppi->PartitionType;
	//the relative sectors
	ppe->StartSector = ppi->HiddenSectors;
	ppe->SectorsInPartition = dwLength;
	FromRelativeToCHS(pbdp, dwStart, ppe->StartOfPartition);
	FromRelativeToCHS(pbdp, dwStart + dwLength -1 , ppe->EndOfPartition);
	return TRUE;
}

void FromRelativeToCHS(PBIOS_DRIVE_PARAM		pbdp,
					   DWORD					dwRelativeSector,
					   BYTE						btCHS[3])
{
	WORD		wCylinder;
	BYTE		btHead, btSector, *pbt;
	int			SectorsPerCylinder = pbdp->dwHeads * pbdp->dwSecPerTrack;

	wCylinder = (WORD)(dwRelativeSector / SectorsPerCylinder);
	if(wCylinder > 1023)
		wCylinder = 1023;
	dwRelativeSector = dwRelativeSector % SectorsPerCylinder;
	btHead = (BYTE)(dwRelativeSector / pbdp->dwSecPerTrack);
	btSector = (BYTE)(( dwRelativeSector % pbdp->dwSecPerTrack ) + 1);
	pbt = ( BYTE* ) &wCylinder;
	btCHS[0] = btHead;
	btCHS[2] = *(pbt++);
	btCHS[1] = (*pbt<<6)|(btSector & 0x3f);
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
									IOCTL_DISK_GET_DRIVE_LAYOUT,
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
	nParType = pdli->dliDrive.PartitionEntry[nCount].PartitionType;
	if( nParType == 0x05 || nParType == 0x0f )
	{
		nCount = nCount - ( nCount % 4 ) + 4 ;
		nParType = pdli->dliDrive.PartitionEntry[nCount].PartitionType;
	}
	if( nParType == 0x00 )
	{
		return -1;
	}
	return nCount;
}

BOOLEAN
PrependSzToMultiSz(
    IN		LPTSTR  SzToPrepend,
    IN		LPTSTR  MultiSz
    )
{
	_TCHAR	newMultiSz[MAX_PATH];
    size_t szLen;
    size_t multiSzLen;

    ASSERT(SzToPrepend != NULL);
    ASSERT(MultiSz != NULL);

    // get the size, in bytes, of the two buffers
    szLen = (_tcslen(SzToPrepend)+1)*sizeof(_TCHAR);
    multiSzLen = MultiSzLength(MultiSz)*sizeof(_TCHAR);

    // recopy the old MultiSz into proper position into the new buffer.
    // the (char*) cast is necessary, because newMultiSz may be a wchar*, and
    // szLen is in bytes.

    memcpy( ((char*)newMultiSz) + szLen, MultiSz, multiSzLen );

    // copy in the new string
    _tcscpy( newMultiSz, SzToPrepend );

    memcpy(MultiSz, newMultiSz, multiSzLen+szLen );

    return (TRUE);
}

size_t
MultiSzLength(
    IN LPTSTR MultiSz
    )
{
    size_t len = 0;
    size_t totalLen = 0;

    ASSERT( MultiSz != NULL );

    // search for trailing null character
    while( *MultiSz != _T('\0') )
    {
        len = _tcslen(MultiSz)+1;
        MultiSz += len;
        totalLen += len;
    }

    // add one for the trailing null character
    return (totalLen+1);
}


/*
 * Deletes all instances of a string from within a multi-sz.
 *
 * parameters:
 *   FindThis        - the string to find and remove
 *   FindWithin      - the string having the instances removed
 *   NewStringLength - the new string length
 */
size_t
MultiSzSearchAndDeleteCaseInsensitive(
    IN  LPTSTR FindThis,
    IN  LPTSTR FindWithin,
    OUT size_t *NewLength
    )
{
    LPTSTR search;
    size_t currentOffset;
    DWORD  instancesDeleted;
    size_t searchLen;

    ASSERT(FindThis != NULL);
    ASSERT(FindWithin != NULL);
    ASSERT(NewLength != NULL);

    currentOffset = 0;
    instancesDeleted = 0;
    search = FindWithin;

    *NewLength = MultiSzLength(FindWithin);

    // loop while the multisz null terminator is not found
    while ( *search != _T('\0') )
    {
        // length of string + null char; used in more than a couple places
        searchLen = _tcslen(search) + 1;

        // if this string matches the current one in the multisz...
        if( _tcsicmp(search, FindThis) == 0 )
        {
            // they match, shift the contents of the multisz, to overwrite the
            // string (and terminating null), and update the length
            instancesDeleted++;
            *NewLength -= searchLen;
            memmove( search,
                     search + searchLen,
                     (*NewLength - currentOffset) * sizeof(TCHAR) );
        }
        else
        {
            // they don't mactch, so move pointers, increment counters
            currentOffset += searchLen;
            search        += searchLen;
        }
    }

    return (instancesDeleted);
}

LPTSTR
MultiSzSearch(IN  LPTSTR FindThis,
			  IN  LPTSTR FindWithin )
{
    LPTSTR	search;
    size_t	currentOffset;
    size_t	searchLen;
	DWORD	NewLength;

    ASSERT(FindThis != NULL);
    ASSERT(FindWithin != NULL);

    currentOffset = 0;
    search = FindWithin;

    NewLength = MultiSzLength(FindWithin);

    // loop while the multisz null terminator is not found
    while ( *search != _T('\0') )
    {
        // length of string + null char; used in more than a couple places
        searchLen = _tcslen(search) + 1;

        // if this string matches the current one in the multisz...
        if( _tcsicmp(search, FindThis) == 0 )
        {
            // they match, shift the contents of the multisz, to overwrite the
            // string (and terminating null), and update the length
			return search;
        }else
        {
            // they don't mactch, so move pointers, increment counters
            search        += searchLen;
        }
    }

    return NULL;
}
