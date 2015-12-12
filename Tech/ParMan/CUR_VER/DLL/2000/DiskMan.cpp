/*======================================================================
DiskMan.cpp : Defines the entry point for the DLL application.
WQ
======================================================================*/

#include "DiskMan.h"
#include "..\..\..\..\ParInfo\Cur_Ver\export\2000\ParInfo.h"
//#include "..\..\..\..\BlkMover\Cur_ver\export\2000\blkmover.h"

//User  apis

HANDLE GetDriveHandle(int nHardDiskNum)
{
	TCHAR szDriveName[MAX_PATH];
	
	sprintf( szDriveName,"\\\\.\\PhysicalDrive%d", nHardDiskNum);

	return CreateFile(szDriveName,GENERIC_READ|GENERIC_WRITE,
		FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,NULL);
}

LRESULT  GetDiskGeometry(int nHardDiskNum, DISK_GEOMETRY *pdg)
{
	LRESULT				lResult = 0;
	TCHAR				szDriveName[MAX_PATH];
	DWORD				dwBytesReturned = 0;
	HANDLE				hDrive;
	
	//zero buffer
	memset(pdg, 0, sizeof(DISK_GEOMETRY));
	//init error num
	lResult = DISKMAN_ERROR_SYSTEM_IO_ERROR;
	//open handle
	nHardDiskNum &= 0x7f;
	sprintf(szDriveName,"\\\\.\\PhysicalDrive%d",nHardDiskNum);
	hDrive = CreateFile(szDriveName,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if(hDrive != INVALID_HANDLE_VALUE)
	{
		if( DeviceIoControl(hDrive,IOCTL_DISK_GET_DRIVE_GEOMETRY,NULL,0,
			pdg,sizeof(DISK_GEOMETRY),&dwBytesReturned,NULL))
		{
			CloseHandle(hDrive);
			//indicate success
			lResult = DISKMAN_ERROR_SUCCESS;
		}
	}
	return lResult;
}

LRESULT  GetDriveLayout(int nHardDiskNum, PDRIVE_LAYOUT pDriveLayout)
{
	LRESULT				lResult = 0;
	TCHAR				szDriveName[MAX_PATH];
	DWORD				dwBytesReturned = 0;
	HANDLE				hDrive;

	//zero the out buffer
	memset(pDriveLayout, 0, sizeof(DRIVE_LAYOUT));
	//init error num
	lResult = DISKMAN_ERROR_SYSTEM_IO_ERROR;
	//open handle
	sprintf(szDriveName,"\\\\.\\PhysicalDrive%d",nHardDiskNum);
	hDrive = CreateFile(szDriveName,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if(hDrive != INVALID_HANDLE_VALUE)
	{
		//open success
		if( DeviceIoControl(hDrive,IOCTL_DISK_GET_DRIVE_LAYOUT,NULL,0,
			pDriveLayout,sizeof(DRIVE_LAYOUT),&dwBytesReturned,NULL))
		{
			//close handle
			CloseHandle(hDrive);
			//indicate success
			lResult = DISKMAN_ERROR_SUCCESS;

			if( pDriveLayout->dli.PartitionCount > 4 )
			{
				//if there has a error,correct it
				if( pDriveLayout->dli.PartitionEntry[4].PartitionType
					== PARTITION_EXTENDED ||
					pDriveLayout->dli.PartitionEntry[4].PartitionType
					== PARTITION_XINT13_EXTENDED )
				{
					memcpy( &pDriveLayout->dli.PartitionEntry[5],
							&pDriveLayout->dli.PartitionEntry[4],
							sizeof(PARTITION_INFORMATION));
					memset( &pDriveLayout->dli.PartitionEntry[4],
							0,
							sizeof(PARTITION_INFORMATION));
					pDriveLayout->dli.PartitionEntry[4].RewritePartition = TRUE;
					pDriveLayout->dli.PartitionEntry[5].RewritePartition = TRUE;
					SetDriveLayout(nHardDiskNum, pDriveLayout);
					pDriveLayout->dli.PartitionEntry[4].RewritePartition = FALSE;
					pDriveLayout->dli.PartitionEntry[5].RewritePartition = FALSE;
				}
			}
		}
	}
	
	return lResult;
}

LRESULT  SetDriveLayout(int					nHardDiskNum,
						PDRIVE_LAYOUT		pDriveLayout,
						BOOL				bLow)
{
	LRESULT				lResult = 0;
	TCHAR				szDriveName[MAX_PATH];
	DWORD				dwBytesReturned = 0;
	HANDLE				hDrive;
	DWORD				dwCtrlCode;
	//init error num
	lResult = DISKMAN_ERROR_SYSTEM_IO_ERROR;
	//open handle
	sprintf(szDriveName,"\\\\.\\PhysicalDrive%d",nHardDiskNum);
	hDrive = CreateFile(szDriveName,MAXIMUM_ALLOWED,0,NULL,OPEN_EXISTING,0,NULL);
	
//	dwCtrlCode = bLow ? IOCTL_YG_BLOCK_LOW_SET_DRIVE_LAYOUT : IOCTL_DISK_SET_DRIVE_LAYOUT;
	dwCtrlCode = IOCTL_DISK_SET_DRIVE_LAYOUT;
	if(hDrive != INVALID_HANDLE_VALUE)
	{
		//open success, set the drive layout
		if( DeviceIoControl(hDrive,
							dwCtrlCode,
							pDriveLayout,
							sizeof(DRIVE_LAYOUT),
							NULL,
							0,

							&dwBytesReturned,NULL))
		{
			CloseHandle(hDrive);
			lResult = DISKMAN_ERROR_SUCCESS;
		}
	}

	return lResult;
}

LRESULT  DeletePrimary(PDRIVE_LAYOUT pDriveLayout, int nIndex)
{
	LRESULT						lResult;
	BOOL						blExistLogical;
	BOOL						bDeleteExtend;
	PARTITION_INFORMATION		*ppiTemp;
	int							nCount;

	bDeleteExtend = GetPartitionType(pDriveLayout,nIndex) == DISKMAN_PAR_EXTENDED_MBR;
	if(( nIndex <= 3)&&( nIndex >=0 ))
	{
		if( bDeleteExtend )
		{			
			blExistLogical = 
				(( pDriveLayout->dli.PartitionEntry[4].PartitionType != PARTITION_ENTRY_UNUSED )||
				 ( pDriveLayout->dli.PartitionEntry[5].PartitionType != PARTITION_ENTRY_UNUSED ));
		}else
			blExistLogical = FALSE;
		if( blExistLogical )
		{
			lResult = DISKMAN_ERROR_DELETE_EXTEND_EXIST_LOGICAL;
		}else
		{
			//set to zero
			memset(&pDriveLayout->dli.PartitionEntry[nIndex],0,sizeof(PARTITION_INFORMATION));
			//if delete extend, partition count = 4;
			if( bDeleteExtend )
			{			
				pDriveLayout->dli.PartitionCount = 4;
			}
			//set rewrite flag
			pDriveLayout->dli.PartitionEntry[nIndex].RewritePartition = TRUE;
			//indicate success
			lResult = DISKMAN_ERROR_SUCCESS;
		}
	}else
		lResult = DISKMAN_ERROR_DELETE_PRIMARY_NOT_PRIMARY;
	//zero the rest partition entries
	nCount = pDriveLayout->dli.PartitionCount;
	ppiTemp = & pDriveLayout->dli.PartitionEntry[nCount];
	memset( ppiTemp, 0,
			sizeof(PARTITION_INFORMATION) * ( MAX_DISKMAN_PAR_NUM - nCount ));

	return lResult;
}

LRESULT  DeleteLogical(PDRIVE_LAYOUT  pDriveLayout, int nIndex)
{
	LRESULT						lResult = 0;
	PARTITION_INFORMATION		*ppiTemp;
	int							nCount;

	switch( nIndex % 4 )
	{
	case 0:
		break;
	case 1:
		//extended embr enntry
		nIndex += 3;
		break;
	case 2:
	case 3:
		//unused embr entry
		nIndex = -1;
		break;
	}
	if(( nIndex < 4 )||( nIndex >= (int)pDriveLayout->dli.PartitionCount))
	{
		lResult = DISKMAN_ERROR_DELETE_LOGICAL_NOT_LOGICAL;
		return lResult;
	}
	//first logical
	if(nIndex == 4)
	{
		ppiTemp = &pDriveLayout->dli.PartitionEntry[nIndex];
		memset(ppiTemp,0,sizeof(PARTITION_INFORMATION));
		//set rewrite flag
		ppiTemp->RewritePartition = TRUE;
		//indicate success
		lResult = DISKMAN_ERROR_SUCCESS;
	}else
	{
		//other logical
		ppiTemp = &pDriveLayout->dli.PartitionEntry[nIndex - 3];
		//copy the next extended embr entry to previous extended embr entry
		memcpy(ppiTemp, ppiTemp + 4, sizeof(PARTITION_INFORMATION));
		//set rewrite flag
		ppiTemp->RewritePartition = TRUE;
		//delet the current embr by copy the next embrs
		for(nCount = pDriveLayout->dli.PartitionCount - 8 ;
			nCount >= nIndex ; nCount -= 4 )
		{
			ppiTemp = &pDriveLayout->dli.PartitionEntry[nCount];
			memcpy(ppiTemp, ppiTemp + 4 , 2 * sizeof(PARTITION_INFORMATION));
		}
		pDriveLayout->dli.PartitionCount -= 4;
		//indicate success
		lResult = DISKMAN_ERROR_SUCCESS;
	}
	//zero the rest partition entries
	nCount = pDriveLayout->dli.PartitionCount;
	ppiTemp = & pDriveLayout->dli.PartitionEntry[nCount];
	memset( ppiTemp, 0,
			sizeof(PARTITION_INFORMATION) * ( MAX_DISKMAN_PAR_NUM - nCount ));
	return lResult;
}

LRESULT  CreatePrimary(PDRIVE_LAYOUT				pDriveLayout,
					   PARTITION_INFORMATION		*ppi,
					   DISK_GEOMETRY				*pdg)
{
	LRESULT						lResult;
	LARGE_INTEGER				lnTotalBytes;
	int							nUnusedEntry = -1;
	int							nIndex;
	BOOL						blBootExist,blExtendedExist,blIsOverLap;
	PPARTITION_INFORMATION		ppiTemp;
	int							nCount;
	
	//do alignment and check
	LegalizePartition(ppi,pdg,DISKMAN_PAR_PRIMARY_MBR);
	//check if beyond disk geometry
	lnTotalBytes.QuadPart = pdg->Cylinders.QuadPart * pdg->SectorsPerTrack
							* pdg->TracksPerCylinder * pdg->BytesPerSector;
	lnTotalBytes.QuadPart -= ppi->StartingOffset.QuadPart;
	lnTotalBytes.QuadPart -= ppi->PartitionLength.QuadPart;
	if( lnTotalBytes.QuadPart < 0 )
	{
		lResult = DISKMAN_ERROR_CREATE_BEYOND_GEOMETRY;
		return lResult;
	}
	//scan mbr to get information
	blExtendedExist = blBootExist = blIsOverLap = FALSE;
	for( nIndex = 3 ; nIndex >=0  ; nIndex-- )
	{
		ppiTemp = &pDriveLayout->dli.PartitionEntry[nIndex];

		if(ppiTemp->PartitionType == PARTITION_ENTRY_UNUSED)
		{
			nUnusedEntry = nIndex;
			continue;
		}
		if( ( ppiTemp->PartitionType == PARTITION_EXTENDED )||
			( ppiTemp->PartitionType == PARTITION_XINT13_EXTENDED ))
			blExtendedExist = TRUE;

		if(ppiTemp->BootIndicator)
			blBootExist = TRUE;

		//detect overlaped
		if( IsOverlap(ppi, ppiTemp) )
			blIsOverLap = TRUE;
	}
	//if create extend partition
	if( ( ppi->PartitionType == PARTITION_EXTENDED )||
		( ppi->PartitionType == PARTITION_XINT13_EXTENDED ))
	{
		if( blExtendedExist ) 
		{
			lResult = DISKMAN_ERROR_CREATE_PRIMARY_EXIST_EXTENDED;
			return lResult;
		}else
		{
			//first create extend,should clear the logical list
			for( nIndex = 4; nIndex < 8 ; nIndex++ )
			{
				ppiTemp = &pDriveLayout->dli.PartitionEntry[nIndex];
				memset( ppiTemp, 0, sizeof(PARTITION_INFORMATION));
				ppiTemp->RewritePartition = TRUE;
			}
			pDriveLayout->dli.PartitionCount = 8;
		}
	}
	//is overlap
	if( blIsOverLap )
	{
		lResult = DISKMAN_ERROR_CREATE_OVERLAP;
		return lResult;
	}
	//is mbr full
	if( !( nUnusedEntry >= 0 && nUnusedEntry < 4 ))
	{
		lResult = DISKMAN_ERROR_CREATE_PRIMARY_MBR_FULL;
		return lResult;
	}
	//if exist boot partiton, Bootindecator = 0;
	if( blBootExist )
		ppi->BootIndicator = 0;
	//do create
	ppi->RewritePartition = TRUE;
	memcpy(&pDriveLayout->dli.PartitionEntry[nUnusedEntry],
			ppi,sizeof(PARTITION_INFORMATION));

	lResult = DISKMAN_ERROR_SUCCESS;
	//zero the rest partition entries
	nCount = pDriveLayout->dli.PartitionCount;
	ppiTemp = & pDriveLayout->dli.PartitionEntry[nCount];
	memset( ppiTemp, 0,
			sizeof(PARTITION_INFORMATION) * ( MAX_DISKMAN_PAR_NUM - nCount ));

	return lResult;
}

LRESULT  CreateLogical(PDRIVE_LAYOUT				pDriveLayout,
					   PARTITION_INFORMATION		*ppi,
					   DISK_GEOMETRY				*pdg)
{
	LRESULT						lResult = 0;
	LARGE_INTEGER				lnExtBegin, lnExtEnd,
								lnBegin,lnEnd,lnTotalBytes;
	int							nInsertIndex = 0;
	int							nIndex;
	PARTITION_INFORMATION		*ppiTemp;
	int							nCount;

	//do alignment and check
	LegalizePartition(ppi,pdg,DISKMAN_PAR_LOGICAL_EMBR);
	//check if beyond disk geometry
	lnTotalBytes.QuadPart = pdg->Cylinders.QuadPart * pdg->SectorsPerTrack
							* pdg->TracksPerCylinder * pdg->BytesPerSector;
	lnTotalBytes.QuadPart -= ppi->StartingOffset.QuadPart;
	lnTotalBytes.QuadPart -= ppi->PartitionLength.QuadPart;
	if( lnTotalBytes.QuadPart < 0 )
	{
		lResult = DISKMAN_ERROR_CREATE_BEYOND_GEOMETRY;
		return lResult;
	}
	//found extend partition
	for(nIndex = 0; nIndex < 4 ; nIndex++)
	{
		if( GetPartitionType(pDriveLayout,nIndex) == DISKMAN_PAR_EXTENDED_MBR )
			break;
	}
	if(nIndex >= 4)
	{
		//no extend partition
		lResult = DISKMAN_ERROR_CREATE_LOGICAL_NO_EXTENDED;
		return lResult;
	}
	//exist extend, nIndex is the entry
	ppiTemp = &pDriveLayout->dli.PartitionEntry[nIndex];
	//check start and size
	lnExtBegin.QuadPart = ppiTemp->StartingOffset.QuadPart;
	lnExtEnd.QuadPart = lnExtBegin.QuadPart + ppiTemp->PartitionLength.QuadPart - 1;
	lnBegin.QuadPart = ppi->StartingOffset.QuadPart;
	lnEnd.QuadPart = ppi->StartingOffset.QuadPart + ppi->PartitionLength.QuadPart -1;
	//check if beyond the extend partition boundary
	if( ( lnBegin.QuadPart < lnExtBegin.QuadPart)||
		( lnEnd.QuadPart > lnExtEnd.QuadPart ))
	{
		lResult = DISKMAN_ERROR_CREATE_LOGICAL_BEYOND_EXTENDED;
		return lResult;
	}
	//detect overlaped and find insert position
	for( nIndex = pDriveLayout->dli.PartitionCount - 4 ; nIndex >= 4 ; nIndex -= 4 )
	{
		if(GetPartitionType(pDriveLayout,nIndex) == DISKMAN_PAR_UNUSED)
			continue;
		//detect overlap
		ppiTemp = &pDriveLayout->dli.PartitionEntry[nIndex];
		if( IsOverlap(ppi,ppiTemp) )
			break;
		//recorde the insert position before which logical drive
		if( ppiTemp->StartingOffset.QuadPart > ppi->StartingOffset.QuadPart )
			nInsertIndex = nIndex;
	}

	if( nIndex < 4 )
	{
		//no overlap
		//at extend begin
		if(ppi->StartingOffset.QuadPart == lnExtBegin.QuadPart)
		{
			ppiTemp = &pDriveLayout->dli.PartitionEntry[4];
			memcpy(ppiTemp,ppi,sizeof(PARTITION_INFORMATION));
			ppiTemp->RewritePartition = TRUE;
			ppiTemp->BootIndicator = 0;
			ppiTemp->HiddenSectors = pdg->SectorsPerTrack;
			ppiTemp->StartingOffset.QuadPart += pdg->SectorsPerTrack * pdg->BytesPerSector;
			ppiTemp->PartitionLength.QuadPart -= pdg->SectorsPerTrack * pdg->BytesPerSector; 
			if(pDriveLayout->dli.PartitionCount <= 8)
			{
				ppiTemp ++;//pointer to next embr
				memset( ppiTemp, 0, sizeof(PARTITION_INFORMATION));
				ppiTemp->RewritePartition = TRUE;
				//modify partition count
				pDriveLayout->dli.PartitionCount = 8;
			}
		}else
		{
			if( nInsertIndex == 0 )// last logical
				nInsertIndex = pDriveLayout->dli.PartitionCount;
			//move the embr to insert
			for( nIndex = pDriveLayout->dli.PartitionCount - 4;
				nIndex >= nInsertIndex ; nIndex -= 4 )
			{
				ppiTemp = &pDriveLayout->dli.PartitionEntry[nIndex];
				memcpy(ppiTemp + 4, ppiTemp, 2 * sizeof(PARTITION_INFORMATION));
			}
			//ppiTemp point to the previous logical embr entry
			ppiTemp = &pDriveLayout->dli.PartitionEntry[nInsertIndex - 3];
			ppiTemp->RewritePartition = TRUE;
			//copy to next logical embr entry to insert
			memcpy( ppiTemp + 4, ppiTemp, sizeof(PARTITION_INFORMATION));
			//insert the logical embr entry
			memcpy(ppiTemp,ppi,sizeof(PARTITION_INFORMATION));
			ppiTemp->BootIndicator = 0;
			ppiTemp->HiddenSectors = 0;
			ppiTemp->RewritePartition = TRUE;
			//set embr type
			ppiTemp->PartitionType = 0x05;
			//insert the logical partition entry
			ppiTemp = &pDriveLayout->dli.PartitionEntry[nInsertIndex];
			memcpy(ppiTemp,ppi,sizeof(PARTITION_INFORMATION));
			ppiTemp->HiddenSectors = pdg->SectorsPerTrack;
			ppiTemp->StartingOffset.QuadPart += pdg->SectorsPerTrack * pdg->BytesPerSector;
			ppiTemp->PartitionLength.QuadPart -= pdg->SectorsPerTrack * pdg->BytesPerSector;
			ppiTemp->RewritePartition = TRUE;
			pDriveLayout->dli.PartitionCount += 4;
			//new extend partition table should be wrote whole
			ppiTemp = &pDriveLayout->dli.PartitionEntry[nInsertIndex];
			for( nIndex = 0 ; nIndex < 4 ; nIndex++ )
			{
				ppiTemp->RewritePartition = TRUE;
				ppiTemp++;
			}			
		}
		//indicate success
		lResult = DISKMAN_ERROR_SUCCESS;
	}else
	{
		lResult = DISKMAN_ERROR_CREATE_OVERLAP;
	}

	//zero the rest partition entries
	nCount = pDriveLayout->dli.PartitionCount;
	ppiTemp = & pDriveLayout->dli.PartitionEntry[nCount];
	memset( ppiTemp, 0,
			sizeof(PARTITION_INFORMATION) * ( MAX_DISKMAN_PAR_NUM - nCount ));

	return lResult;
}

LRESULT  ResizePrimary(PDRIVE_LAYOUT			pDriveLayout,
					   PARTITION_INFORMATION	*ppi,
					   DISK_GEOMETRY			*pdg,
					   int						nIndex)
{
	LRESULT							lResult;
	int								i;
	BOOL							blFirstLogicalEntryUnused;
	BOOL							blExistLogical;
	LARGE_INTEGER					lnStart,lnEnd,lnTotalBytes;
	PPARTITION_INFORMATION			ppiTemp;
	int								nCount;

	if ( nIndex < 0 || nIndex >= 4 )
	{
		lResult = DISKMAN_ERROR_RESIZE_PRIMARY_NOT_PRIMARY;
		return lResult;
	}
	//do alignment and check
	LegalizePartition(ppi,pdg,DISKMAN_PAR_PRIMARY_MBR);

	// if length <= 0 then delete
	if( ppi->PartitionLength.QuadPart <= 0 )
		return DeletePrimary(pDriveLayout,nIndex);

	//check if beyond disk geometry
	lnTotalBytes.QuadPart = pdg->Cylinders.QuadPart * pdg->SectorsPerTrack
							* pdg->TracksPerCylinder * pdg->BytesPerSector;
	lnTotalBytes.QuadPart -= ppi->StartingOffset.QuadPart;
	lnTotalBytes.QuadPart -= ppi->PartitionLength.QuadPart;
	if( lnTotalBytes.QuadPart < 0 )
	{
		lResult = DISKMAN_ERROR_CREATE_BEYOND_GEOMETRY;
		return lResult;
	}
	//check overlap
	for ( i=0 ; i<4 ; i++)
	{
		//detect overlaped
		ppiTemp = &pDriveLayout->dli.PartitionEntry[i];
		if(( i == nIndex )||( ppiTemp->PartitionType == PARTITION_ENTRY_UNUSED ))
			continue;//skip itself and empty
		if( IsOverlap(ppi, ppiTemp) )
			break;
	}
	//has overlap
	if ( i < 4 )
	{
		lResult = DISKMAN_ERROR_CREATE_OVERLAP;
		return lResult;
	}
	//if resize a extend partiton, rearrange logical drives
	if( GetPartitionType(pDriveLayout, nIndex) == DISKMAN_PAR_EXTENDED_MBR )
	{
		if( pDriveLayout->dli.PartitionCount == 4 )
		{
			blFirstLogicalEntryUnused = TRUE;
			blExistLogical = FALSE;
			//if no logical, should clear the first embr
			pDriveLayout->dli.PartitionCount = 8;
		}else
		{
			ppiTemp = &pDriveLayout->dli.PartitionEntry[4];
			blFirstLogicalEntryUnused = ( ppiTemp->PartitionType == PARTITION_ENTRY_UNUSED );
			if ( blFirstLogicalEntryUnused )
			{
				ppiTemp++;
				blExistLogical = ( ppiTemp->PartitionType != PARTITION_ENTRY_UNUSED );
			}else
				blExistLogical = TRUE;
		}
		//rearrange logical drives
		if( blExistLogical )
		{
			//get first logical start
			if ( blFirstLogicalEntryUnused )
			{
				ppiTemp = &pDriveLayout->dli.PartitionEntry[5];
				lnStart.QuadPart = ppiTemp->StartingOffset.QuadPart;
			}
			else
			{
				//or get extend start as the first logical start
				ppiTemp = &pDriveLayout->dli.PartitionEntry[nIndex];
				lnStart.QuadPart = ppiTemp->StartingOffset.QuadPart;
			}
			//last logical embr entry
			i = pDriveLayout->dli.PartitionCount - 4;
			//get last logical end;			
			ppiTemp = &pDriveLayout->dli.PartitionEntry[i];
			lnEnd.QuadPart = ppiTemp->StartingOffset.QuadPart
							+ ppiTemp->PartitionLength.QuadPart - 1;
			//end not valid
			if( ( ppi->StartingOffset.QuadPart + ppi->PartitionLength.QuadPart - 1 )
				  < lnEnd.QuadPart )
			{
				lResult = DISKMAN_ERROR_RESIZE_PRIMARY_INVALID_END;
				return lResult;
			}
			//begin not valid
			if(ppi->StartingOffset.QuadPart  > lnStart.QuadPart)
			{
				lResult = DISKMAN_ERROR_RESIZE_PRIMARY_INVALID_BEGIN;
				return lResult;
			}
			//ppiTemp point to the extend entry
			ppiTemp = &pDriveLayout->dli.PartitionEntry[nIndex];
			//larger the extend start and the new start is equal to the
			//first logical drive start, remove a embr
			if( ( ppiTemp->StartingOffset.QuadPart < ppi->StartingOffset.QuadPart )&&
				( ppi->StartingOffset.QuadPart == lnStart.QuadPart ))
			{
				//remove a embr
				for( i = 8 ; i < (int)pDriveLayout->dli.PartitionCount ; i += 4 )
				{
					memcpy( &pDriveLayout->dli.PartitionEntry[i-4],
							&pDriveLayout->dli.PartitionEntry[i], 
							2 * sizeof(PARTITION_INFORMATION));
				}
				pDriveLayout->dli.PartitionCount -= 4;
			}
			//lower the extend start and the first logical entry used, insert an embr
			if( ( ppiTemp->StartingOffset.QuadPart > ppi->StartingOffset.QuadPart )&&
				!blFirstLogicalEntryUnused )
			{
				//insert a embr
				for(i = pDriveLayout->dli.PartitionCount; i > 4 ; i -= 4)
				{
					memcpy( &pDriveLayout->dli.PartitionEntry[i],
							&pDriveLayout->dli.PartitionEntry[i-4], 
							2 * sizeof(PARTITION_INFORMATION));
				}
				ppiTemp = &pDriveLayout->dli.PartitionEntry[5];
				memcpy(ppiTemp, ppiTemp - 1,sizeof(PARTITION_INFORMATION));
				//zero first embr entry
				memset(ppiTemp - 1,0,sizeof(PARTITION_INFORMATION));
				//adjust embr entry
				ppiTemp->StartingOffset.QuadPart -= pdg->BytesPerSector 
														* pdg->SectorsPerTrack;
				ppiTemp->PartitionLength.QuadPart += pdg->BytesPerSector
														* pdg->SectorsPerTrack;
				ppiTemp->PartitionType = PARTITION_EXTENDED;
				pDriveLayout->dli.PartitionCount += 4;
			}
			//modify the hidden sectors and rewrite flag
			for( i = pDriveLayout->dli.PartitionCount - 7 ; i >= 5 ; i -= 4 )
			{
				pDriveLayout->dli.PartitionEntry[i].HiddenSectors = 0;
				pDriveLayout->dli.PartitionEntry[i].RewritePartition = TRUE;
			}
		}// if exist logical
		//new first embr must be wrote back
		for( i = 4; i < 8 ; i++ )
		{
			ppiTemp = &pDriveLayout->dli.PartitionEntry[i];
			ppiTemp->RewritePartition = TRUE;
		}
	}//if resize extend
	//do resize primary
	ppiTemp = &pDriveLayout->dli.PartitionEntry[nIndex];
	ppiTemp->StartingOffset.QuadPart = ppi->StartingOffset.QuadPart;
	ppiTemp->PartitionLength.QuadPart = ppi->PartitionLength.QuadPart;
	ppiTemp->HiddenSectors = 0;
	ppiTemp->RewritePartition = TRUE;
	//indicate success
	lResult = DISKMAN_ERROR_SUCCESS;

	//zero the rest partition entries
	nCount = pDriveLayout->dli.PartitionCount;
	ppiTemp = & pDriveLayout->dli.PartitionEntry[nCount];
	memset( ppiTemp, 0,
			sizeof(PARTITION_INFORMATION) * ( MAX_DISKMAN_PAR_NUM - nCount ));

	return lResult;
}


LRESULT  ResizeLogical(PDRIVE_LAYOUT				pDriveLayout,
					   PARTITION_INFORMATION		*ppi,
					   DISK_GEOMETRY				*pdg,
					   int							nIndex)
{
	return 1;
}

LRESULT  LogicalToPrimary(PDRIVE_LAYOUT				pDriveLayout,
						  DISK_GEOMETRY				*pdg,
						  int						nFlag)
{
	LRESULT							lResult;
	int								nIndex;
	int								nUnusedEntry,nExtendIndex,nLogicalIndex;
	BOOL							blExistLogical;
	PARTITION_INFORMATION			*ppiUnused = NULL;
	PARTITION_INFORMATION			*ppiExtend = NULL;
	PARTITION_INFORMATION			*ppiLogical = NULL;
	PARTITION_INFORMATION			piLogical,piExtend,*ppiTemp;//temp storage
	LARGE_INTEGER					lnNewStart,lnNewEnd; //for resize the extended

	//check if exist logical
	if( pDriveLayout->dli.PartitionCount >= 8 )
	{
		ppiTemp = &pDriveLayout->dli.PartitionEntry[4];
		if ( ppiTemp->PartitionType == PARTITION_ENTRY_UNUSED )
		{
			ppiTemp++;
			blExistLogical = ( ppiTemp->PartitionType != PARTITION_ENTRY_UNUSED );
		}else
			blExistLogical = TRUE;
	}else
		blExistLogical = FALSE;
	
	if( !blExistLogical )
		return DISKMAN_ERROR_NO_LOGICAL;
	
	nUnusedEntry = -1;
	nExtendIndex = -1;
	for( nIndex = 4; nIndex >= 0 ; nIndex-- )
	{
		//get extend entry index
		if( GetPartitionType(pDriveLayout, nIndex) == DISKMAN_PAR_EXTENDED_MBR)
		{
			nExtendIndex = nIndex;
			ppiTemp = &pDriveLayout->dli.PartitionEntry[nExtendIndex];
			memcpy(&piExtend,ppiTemp,sizeof(PARTITION_INFORMATION));
			lnNewStart.QuadPart = piExtend.StartingOffset.QuadPart;
			lnNewEnd.QuadPart = lnNewStart.QuadPart
								+ piExtend.PartitionLength.QuadPart -1;
		}
		//get unused entry index
		if( GetPartitionType(pDriveLayout, nIndex) == DISKMAN_PAR_UNUSED)
			nUnusedEntry = nIndex;
	}
	//if the logical is the first and the last
	//over write the extend partition
	if( ( nFlag & DISKMAN_LOGTOPRI_FIRST )&&
		( nFlag & DISKMAN_LOGTOPRI_LAST ) )
	{
		nUnusedEntry = nExtendIndex;
	}

	if( nExtendIndex == -1 ) 
		return DISKMAN_ERROR_CREATE_LOGICAL_NO_EXTENDED;
	if( nUnusedEntry == -1 )
		return DISKMAN_ERROR_CREATE_PRIMARY_MBR_FULL;

	ppiUnused = &pDriveLayout->dli.PartitionEntry[nUnusedEntry];
	ppiExtend = &pDriveLayout->dli.PartitionEntry[nExtendIndex];

	if( nFlag & DISKMAN_LOGTOPRI_FIRST )
	{
		//first
		ppiLogical = &pDriveLayout->dli.PartitionEntry[4];
		if( ppiLogical->PartitionType == PARTITION_ENTRY_UNUSED )
		{
			ppiLogical += 4;
			nLogicalIndex = 8;
		}else
			nLogicalIndex = 4;
		lnNewStart.QuadPart = ppiLogical->StartingOffset.QuadPart
							+ ppiLogical->PartitionLength.QuadPart;
	}else
	{
		//last
		nLogicalIndex = pDriveLayout->dli.PartitionCount - 4;
		ppiLogical = &pDriveLayout->dli.PartitionEntry[nLogicalIndex];
		lnNewEnd.QuadPart = (ppiLogical - 3)->StartingOffset.QuadPart - 1;							
	}
	//back up the logical
	memcpy(&piLogical,ppiLogical,sizeof(PARTITION_INFORMATION));
	//delet logical
	if( lResult = DeleteLogical(pDriveLayout,nLogicalIndex))
		return lResult;
	//resize extend
	if( ( nFlag & DISKMAN_LOGTOPRI_FIRST )&&
		( nFlag & DISKMAN_LOGTOPRI_LAST ) )
	{
		//is the first and last logical delete the extend partition
		//delete the extend partition
		if( lResult = DeletePrimary(pDriveLayout,nExtendIndex))
			return lResult;
	}else
	{
		//resize the extend partition
		piExtend.StartingOffset.QuadPart = lnNewStart.QuadPart;
		piExtend.PartitionLength.QuadPart = lnNewEnd.QuadPart - lnNewStart.QuadPart + 1;
		piExtend.HiddenSectors = 0;
		if( lResult = ResizePrimary(pDriveLayout,&piExtend,pdg,nExtendIndex))
			return lResult;
	}
	//create new primary
	memcpy(ppiUnused,&piLogical,sizeof(PARTITION_INFORMATION));
	ppiUnused->RewritePartition = TRUE;
	ppiUnused->HiddenSectors = 0;

	return DISKMAN_ERROR_SUCCESS;
}

DWORD  AlignSectorOffset(DWORD			dwSectorOffset,
						 DISK_GEOMETRY	*pdg,
						 DWORD			dwAlignFlag)
{
	BIOS_DRIVE_PARAM		bdp;
	//do convert
	bdp.wInfoSize = sizeof(BIOS_DRIVE_PARAM);
	bdp.dwHeads = pdg->TracksPerCylinder;
	bdp.dwCylinders = (DWORD)pdg->Cylinders.QuadPart;
	bdp.dwSecPerTrack = pdg->SectorsPerTrack;
	bdp.SectorsSize = (WORD)pdg->BytesPerSector;
	bdp.dwSectors = pdg->Cylinders.QuadPart
									* pdg->TracksPerCylinder
									* pdg->SectorsPerTrack;
	//use old align
	switch( dwAlignFlag )
	{
	case DISKMAN_ALIGN_FORWARD:
		dwSectorOffset = AlignToCylinder(dwSectorOffset,&bdp,ALIGN_FOREWARD);
		break;
	case DISKMAN_ALIGN_BACKWARD:
		dwSectorOffset = AlignToCylinder(dwSectorOffset,&bdp,ALIGN_BACKWARD);	
		break;
	default:
		dwSectorOffset = -1;
	}

//	dwSectorsPerCylinder = pdg->TracksPerCylinder * pdg->SectorsPerTrack;
//	dwSectorOffset = dwSectorOffset - (dwSectorOffset % dwSectorsPerCylinder);
//	if(dwAlignFlag == DISKMAN_ALIGN_BACKWARD)
//		dwSectorOffset += dwSectorsPerCylinder - 1;
//	if(dwAlignFlag == DISKMAN_ALIGN_HEAD1)
//		dwSectorOffset += pdg->SectorsPerTrack;
	return dwSectorOffset;
}

int  GetDriveLetter(int nHardDisk, int nPartitionNum)
{
	return 0xff;
}

void  GetDriveLocation(int nDrive,int& nHardDisk,int& nPartitionNum)
{
}

//create partition call
void  LegalizePartition(PPARTITION_INFORMATION			ppi,
						DISK_GEOMETRY					*pdg,
						DWORD							dwFlag)
{
	DWORD			dwStart,dwEnd,dwEndCylinder;
	//do alignment
	dwStart = (DWORD)(ppi->StartingOffset.QuadPart / pdg->BytesPerSector);
	dwEnd = dwStart + (DWORD)(ppi->PartitionLength.QuadPart / pdg->BytesPerSector) - 1;
	//align start
	if( dwStart < pdg->SectorsPerTrack * pdg->TracksPerCylinder )
	{
		if( dwStart < pdg->SectorsPerTrack * pdg->Cylinders.QuadPart )
		{
			if(	( ppi->PartitionType == PARTITION_EXTENDED )||
				( ppi->PartitionType == PARTITION_XINT13_EXTENDED)||
				dwFlag == DISKMAN_PAR_LOGICAL_EMBR )
			{
				//extend partition must start at cylinder head
				dwStart = pdg->SectorsPerTrack * pdg->TracksPerCylinder;
			}
		}else
			dwStart = pdg->SectorsPerTrack;
	}else
		dwStart = AlignSectorOffset(dwStart,pdg,DISKMAN_ALIGN_FORWARD);
	//align end
	dwEnd = AlignSectorOffset(dwEnd,pdg,DISKMAN_ALIGN_BACKWARD);
	dwEndCylinder = dwEnd / ( pdg->SectorsPerTrack * pdg->TracksPerCylinder );
	dwEnd = dwEnd - dwStart + 1;

	ppi->StartingOffset.QuadPart = (__int64)dwStart * pdg->BytesPerSector;
	ppi->PartitionLength.QuadPart = (__int64)dwEnd * pdg->BytesPerSector;
	//check type
	dwEndCylinder = dwEnd / ( pdg->SectorsPerTrack * pdg->TracksPerCylinder );
	switch( ppi->PartitionType & 0x0f)
	{
	case 0x06:
	case 0x0e:
		ppi->PartitionType &= 0xf0;
		if( ( dwEndCylinder >= 1024 )&&
			( ( dwFlag == DISKMAN_PAR_PRIMARY_MBR )||
			  ( dwFlag == DISKMAN_PAR_PRIMARY_MBR ))
			)
		{
			ppi->PartitionType |= 0x0e;
		}else
			ppi->PartitionType |= 0x06;
		break;
	case 0x0b:
	case 0x0c:
		ppi->PartitionType &= 0xf0;
		if( ( dwEndCylinder >= 1024 )&&
			( ( dwFlag == DISKMAN_PAR_PRIMARY_MBR )||
			  ( dwFlag == DISKMAN_PAR_PRIMARY_MBR ))
			)
		{
			ppi->PartitionType |= 0x0c;
		}else
			ppi->PartitionType |= 0x0b;
		break;
	case 0x05:
	case 0x0f:
		if (dwEndCylinder > 1023 )
			ppi->PartitionType = 0x0f;
		else
			ppi->PartitionType = 0x05;
		break;
	default:
		//unknown types
		break;
	}
}

int  GetPartitionType(PDRIVE_LAYOUT		pDriveLayout,
					  int				nIndex)
{
	int				nType;

	switch(pDriveLayout->dli.PartitionEntry[nIndex].PartitionType)
	{
	case PARTITION_ENTRY_UNUSED:
		nType = DISKMAN_PAR_UNUSED;
		break;
	case PARTITION_XINT13_EXTENDED:
	case PARTITION_EXTENDED:
		if( nIndex >= 4 )
			nType = DISKMAN_PAR_EXTENDED_EMBR;
		else
			nType = DISKMAN_PAR_EXTENDED_MBR;
		break;
	default:
		if( nIndex >= 4 )
			nType = DISKMAN_PAR_LOGICAL_EMBR;
		else
			nType = DISKMAN_PAR_PRIMARY_MBR;
	}

	return nType;
}

BOOL  IsOverlap(PPARTITION_INFORMATION	ppiFirst,
				PPARTITION_INFORMATION	ppiSecond)
{
	LARGE_INTEGER			lnFirstBegin,lnFirstEnd;
	LARGE_INTEGER			lnSecondBegin,lnSecondEnd;

	lnFirstBegin.QuadPart = ppiFirst->StartingOffset.QuadPart;
	lnSecondBegin.QuadPart = ppiSecond->StartingOffset.QuadPart;
	lnFirstEnd.QuadPart = ppiFirst->PartitionLength.QuadPart
						+ lnFirstBegin.QuadPart - 1;
	lnSecondEnd.QuadPart = ppiSecond->PartitionLength.QuadPart
						+ lnSecondBegin.QuadPart - 1;
	//Check begin
	if( ( lnFirstBegin.QuadPart >= lnSecondBegin.QuadPart )&&
		( lnFirstBegin.QuadPart <= lnSecondEnd.QuadPart ))
		return TRUE;
	//check end
	if( ( lnFirstEnd.QuadPart >= lnSecondBegin.QuadPart )&&
		( lnFirstEnd.QuadPart <= lnSecondEnd.QuadPart ))
		return TRUE;
	//Check begin
	if( ( lnSecondBegin.QuadPart >= lnFirstBegin.QuadPart )&&
		( lnSecondBegin.QuadPart <= lnFirstEnd.QuadPart ))
		return TRUE;
	//check end
	if( ( lnSecondEnd.QuadPart >= lnFirstBegin.QuadPart )&&
		( lnSecondEnd.QuadPart <= lnFirstEnd.QuadPart ))
		return TRUE;

	return FALSE;
}