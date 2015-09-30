/*================================================================
ParMan.cpp
WQ
================================================================*/
#include <assert.h>
//win2000 disk management
#include "DiskMan.h"
//other
#include "..\..\export\2000\ParMan.h"
#include "ParManPrivate.h"

BOOL DLL_EXPORT	DeletePartition(DWORD dwStartSector, 
								  BYTE btHardDisk, 
								  CREATE_PAR_FLAG flags, 
								  PINT pnError)
{
	int					nErrorCode; 
	int					nIndex;
	LARGE_INTEGER		lnStartByte;
	DISK_GEOMETRY		dg;
	DRIVE_LAYOUT		DriveLayout;
	BIOS_DRIVE_PARAM	bdp;
	//standard mbr
	BYTE				stdMBR[512] = 
						{0x33,0xc0,0x8e,0xd0,0xbc,0x0 ,0x7c,0xfb,0x50,0x7 ,
						 0x50,0x1f,0xfc,0xbe,0x1b,0x7c,0xbf,0x1b,0x6 ,0x50,
						 0x57,0xb9,0xe5,0x1 ,0xf3,0xa4,0xcb,0xbe,0xbe,0x7 ,
						 0xb1,0x4 ,0x38,0x2c,0x7c,0x9 ,0x75,0x15,0x83,0xc6,
						 0x10,0xe2,0xf5,0xcd,0x18,0x8b,0x14,0x8b,0xee,0x83,
						 0xc6,0x10,0x49,0x74,0x16,0x38,0x2c,0x74,0xf6,0xbe,
						 0x10,0x7 ,0x4e,0xac,0x3c,0x0 ,0x74,0xfa,0xbb,0x7 ,
						 0x0 ,0xb4,0xe ,0xcd,0x10,0xeb,0xf2,0x89,0x46,0x25,
						 0x96,0x8a,0x46,0x4 ,0xb4,0x6 ,0x3c,0xe ,0x74,0x11,
						 0xb4,0xb ,0x3c,0xc ,0x74,0x5 ,0x3a,0xc4,0x75,0x2b,
						 0x40,0xc6,0x46,0x25,0x6 ,0x75,0x24,0xbb,0xaa,0x55,
						 0x50,0xb4,0x41,0xcd,0x13,0x58,0x72,0x16,0x81,0xfb,
						 0x55,0xaa,0x75,0x10,0xf6,0xc1,0x1 ,0x74,0xb ,0x8a,
						 0xe0,0x88,0x56,0x24,0xc7,0x6 ,0xa1,0x6 ,0xeb,0x1e,
						 0x88,0x66,0x4 ,0xbf,0xa ,0x0 ,0xb8,0x1 ,0x2 ,0x8b,
						 0xdc,0x33,0xc9,0x83,0xff,0x5 ,0x7f,0x3 ,0x8b,0x4e,
						 0x25,0x3 ,0x4e,0x2 ,0xcd,0x13,0x72,0x29,0xbe,0x46,
						 0x7 ,0x81,0x3e,0xfe,0x7d,0x55,0xaa,0x74,0x5a,0x83,
						 0xef,0x5 ,0x7f,0xda,0x85,0xf6,0x75,0x83,0xbe,0x27,
						 0x7 ,0xeb,0x8a,0x98,0x91,0x52,0x99,0x3 ,0x46,0x8 ,
						 0x13,0x56,0xa ,0xe8,0x12,0x0 ,0x5a,0xeb,0xd5,0x4f,
						 0x74,0xe4,0x33,0xc0,0xcd,0x13,0xeb,0xb8,0x0 ,0x0 ,
						 0x0 ,0x0 ,0x0 ,0x0 ,0x56,0x33,0xf6,0x56,0x56,0x52,
						 0x50,0x6 ,0x53,0x51,0xbe,0x10,0x0 ,0x56,0x8b,0xf4,
						 0x50,0x52,0xb8,0x0 ,0x42,0x8a,0x56,0x24,0xcd,0x13,
						 0x5a,0x58,0x8d,0x64,0x10,0x72,0xa ,0x40,0x75,0x1 ,
						 0x42,0x80,0xc7,0x2 ,0xe2,0xf7,0xf8,0x5e,0xc3,0xeb,
						 0x74,0x49,0x6e,0x76,0x61,0x6c,0x69,0x64,0x20,0x70,
						 0x61,0x72,0x74,0x69,0x74,0x69,0x6f,0x6e,0x20,0x74,
						 0x61,0x62,0x6c,0x65,0x0 ,0x45,0x72,0x72,0x6f,0x72,
						 0x20,0x6c,0x6f,0x61,0x64,0x69,0x6e,0x67,0x20,0x6f,
						 0x70,0x65,0x72,0x61,0x74,0x69,0x6e,0x67,0x20,0x73,
						 0x79,0x73,0x74,0x65,0x6d,0x0 ,0x4d,0x69,0x73,0x73,
						 0x69,0x6e,0x67,0x20,0x6f,0x70,0x65,0x72,0x61,0x74,
						 0x69,0x6e,0x67,0x20,0x73,0x79,0x73,0x74,0x65,0x6d,
						 0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,
						 0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,
						 0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,
						 0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x8b,0xfc,0x1e,
						 0x57,0x8b,0xf5,0xcb,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,
						 0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,
						 0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,
						 0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,
						 0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,
						 0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,
						 0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,
						 0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,
						 0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,
						 0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,
						 0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,
						 0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,
						 0x55,0xaa};

	btHardDisk &= 0x7f;	
	//Get disk geometry and layout information
	*pnError = nErrorCode = ERR_PARMAN_PARAM; // 102 indicate false
	if ( GetDiskGeometry(btHardDisk,&dg))
		return FALSE;
	if ( GetDriveLayout(btHardDisk, &DriveLayout))
		return FALSE;
	//found the partition
	lnStartByte.QuadPart = (__int64)dg.BytesPerSector * dwStartSector;
	for( nIndex = DriveLayout.dli.PartitionCount -1 ; nIndex >=0 ; nIndex-- )
	{
		if(DriveLayout.dli.PartitionEntry[nIndex].StartingOffset.QuadPart 
			== lnStartByte.QuadPart)
			break;
	}
	//usb disk
	if((DriveLayout.dli.PartitionCount == 1)&&(nIndex == 0))
	{
		GetDriveParam(btHardDisk,&bdp);
		WriteSector(6,1,stdMBR,btHardDisk,&bdp);
		return WriteSector(0,1,stdMBR,btHardDisk,&bdp);	
	}
	//if found do delete
	if(nIndex >= 0)
	{
		switch( GetPartitionType(&DriveLayout, nIndex) )
		{
		case DISKMAN_PAR_UNUSED:
			nErrorCode = ERR_PARMAN_PARAM;
			break;
		case DISKMAN_PAR_EXTENDED_MBR:
			if ( flags.Extended )
			{
				//delete first logical
				if( DeleteLogical(&DriveLayout,4) )
					nErrorCode = ERR_PARMAN_PARAM;
				else
					nErrorCode = 0;
				break;
			}//else consider as primary
		case DISKMAN_PAR_PRIMARY_MBR:
			if( DeletePrimary(&DriveLayout,nIndex) )
				nErrorCode = ERR_PARMAN_PARAM;
			else
				nErrorCode = 0;
			break;
		case DISKMAN_PAR_EXTENDED_EMBR:
			nIndex +=3;
		case DISKMAN_PAR_LOGICAL_EMBR:
			if( DeleteLogical(&DriveLayout,nIndex) )
				nErrorCode = ERR_PARMAN_PARAM;
			else
				nErrorCode = 0;
			break;
		}
		//if success, nErrorCode == 0
		//write the layout information back
		if( !nErrorCode )
			nErrorCode = SetDriveLayout(btHardDisk, &DriveLayout);			
	}

	*pnError = nErrorCode;
	// nErrorCode == 0, return TRUE;
	return !nErrorCode;
}

BOOL DLL_EXPORT CreatePartition(PPARTITION_ENTRY ppeParEntry, 
								  BYTE btHardDisk, 
								  DWORD dwFlag,
								  BOOL blIsFormat, 
								  PBYTE pLabel, 
								  HWND hWnd,
								  PINT pnError)
{
	int						nErrorCode;
	PARTITION_INFORMATION	pi,piExtend;
	DRIVE_LAYOUT			DriveLayout;
	DISK_GEOMETRY			dg;
	int						nIndex;
	BYTE					btPartitionType;
	BOOL					bExistExtend;
	BOOL					bNeedResize,bResizeSuccess;
	LARGE_INTEGER			lnExtendStart,lnExtendEnd;
	char					chDriveLetter;
	char					szLabel[12];
//	int						nLogicalNumber;

	btHardDisk &= 0x7f;	
	//Get disk geomtry and layout information
	*pnError = nErrorCode = ERR_PARMAN_PARAM; // 102 indicate false
	if ( GetDiskGeometry(btHardDisk,&dg))
		return FALSE;
	if ( GetDriveLayout(btHardDisk, &DriveLayout))
		return FALSE;
	//check if usb sigle partition
	if((DriveLayout.dli.PartitionCount % 4) != 0 )
		return FALSE;

	//translate to partition information
	memset(&pi,0,sizeof(PARTITION_INFORMATION));
	pi.StartingOffset.QuadPart = (__int64)ppeParEntry->StartSector * dg.BytesPerSector;
	pi.PartitionLength.QuadPart = (__int64)ppeParEntry->SectorsInPartition * dg.BytesPerSector;
	pi.BootIndicator = ppeParEntry->BootFlag ? 1:0;
	pi.PartitionType = ppeParEntry->SystemFlag;
	
	*pnError = ERR_PARMAN_PARAM;
	//stat logical drive numbers
//	nLogicalNumber = 0;
//	for( nIndex = 4 ; nIndex < DriveLayout.dli.PartitionCount ; nIndex +=4 )
//	{
//		if ( GetPartitionType( &DriveLayout,nIndex) != DISKMAN_PAR_UNUSED )
//			nLogicalNumber++;
//	}
	//detect if exist extend partition
	bExistExtend = FALSE;
	for( nIndex = 0 ; nIndex < 4 ; nIndex++ )
	{
		if ( GetPartitionType( &DriveLayout,nIndex) == DISKMAN_PAR_EXTENDED_MBR )
		{
			bExistExtend = TRUE;
			memcpy(&piExtend,&DriveLayout.dli.PartitionEntry[nIndex],
					sizeof(PARTITION_INFORMATION));
			lnExtendStart.QuadPart = piExtend.StartingOffset.QuadPart;
			lnExtendEnd.QuadPart = piExtend.PartitionLength.QuadPart;
			lnExtendEnd.QuadPart += lnExtendStart.QuadPart - 1;
			break;
		}
	}
	//do alignment
	if ( dwFlag == LOGICAL )
	{
		LegalizePartition(&pi,&dg,DISKMAN_PAR_LOGICAL_EMBR);
	}else
		LegalizePartition(&pi,&dg,DISKMAN_PAR_PRIMARY_MBR);
	//create partition	
	bNeedResize = FALSE;
	bResizeSuccess = TRUE;
	if( dwFlag == LOGICAL )
	{
		if( bExistExtend )
		{
			//beyond the extend low boundary
			if(	pi.StartingOffset.QuadPart  
				<
				lnExtendStart.QuadPart )
			{
				//justify the extend head
				piExtend.StartingOffset.QuadPart = pi.StartingOffset.QuadPart;
				piExtend.PartitionLength.QuadPart = lnExtendEnd.QuadPart
													- piExtend.StartingOffset.QuadPart
													+ 1;
				bNeedResize = TRUE;
			}
			//beyond the extend high boundary
			if( pi.StartingOffset.QuadPart + pi.PartitionLength.QuadPart 
				> 
				lnExtendEnd.QuadPart + 1 )
			{
				//justify the extend end
				//piExtend.StartingOffset has modified;
				piExtend.PartitionLength.QuadPart = pi.StartingOffset.QuadPart
													+ pi.PartitionLength.QuadPart
													- piExtend.StartingOffset.QuadPart;
				bNeedResize = TRUE;
			}
			//if there existing only one logical drive is required to
			//move to the free space acrross a primary partition, parman
			//should move the extend with the logical
//			if( nLogicalNumber == 1 )
//			{
//				if(	pi.StartingOffset.QuadPart  
//					<
//					lnExtendStart.QuadPart )
//
//				if( pi.StartingOffset.QuadPart + pi.PartitionLength.QuadPart 
//				> 
//				lnExtendEnd.QuadPart + 1 )
//			}
			if ( bNeedResize )
			{
				nErrorCode = ResizePrimary(&DriveLayout,&piExtend,&dg,nIndex);
				bResizeSuccess = ! nErrorCode;
			}
			//resize success create logical
			if( bResizeSuccess )
				nErrorCode = CreateLogical(&DriveLayout,&pi,&dg);
		}else
		{
			//no extend partition, should create one
			//Backup partition type;
			btPartitionType = pi.PartitionType;
			pi.PartitionType = PARTITION_EXTENDED;
			nErrorCode = CreatePrimary(&DriveLayout,&pi,&dg);
			if( !nErrorCode ) 
			{
				pi.PartitionType = btPartitionType;
				nErrorCode = CreateLogical(&DriveLayout,&pi,&dg);
			}
		}
	}else
	{
		// if primary overlap the extend then resize extend partition
		if( bExistExtend )
		{
			if( IsOverlap(&piExtend,&pi) )
			{
				//try to resize extend partition forward
				piExtend.PartitionLength.QuadPart = pi.StartingOffset.QuadPart
													- lnExtendStart.QuadPart;
				//if length < 0 then length = 0
				if( piExtend.PartitionLength.QuadPart < 0 )
					piExtend.PartitionLength.QuadPart = 0;
				//resize forward
				nErrorCode = ResizePrimary(&DriveLayout,&piExtend,&dg,nIndex);
				if( nErrorCode != 0 )
				{
					//try to resize backward
					piExtend.StartingOffset.QuadPart =  pi.StartingOffset.QuadPart
														+ pi.PartitionLength.QuadPart;
					piExtend.PartitionLength.QuadPart = lnExtendEnd.QuadPart
														- piExtend.StartingOffset.QuadPart
														+ 1;
					nErrorCode = ResizePrimary(&DriveLayout,&piExtend,&dg,nIndex);
					
					bResizeSuccess = ! nErrorCode;
				}
			}
		}

		if( bResizeSuccess )
			nErrorCode = CreatePrimary(&DriveLayout,&pi,&dg);
	}
	//create success, write back
	if( !nErrorCode )
		nErrorCode = SetDriveLayout(btHardDisk,&DriveLayout);
	//format partition
	if(( blIsFormat )&&( !nErrorCode ))
	{
		for( nIndex = 0; nIndex < 100 ; nIndex++ )
		{
			Sleep(500);
			chDriveLetter = RetrieveDriveLttr(btHardDisk,
												dwFlag,
												(DWORD)
												(pi.StartingOffset.QuadPart
													/ dg.BytesPerSector));

			if( (BYTE)chDriveLetter != 0xff )
			{
				chDriveLetter += 0x40;
				break;
			}
		}
		if ( chDriveLetter >= 'A' && chDriveLetter <= 'Z' )
		{
			memcpy(szLabel,pLabel,11);
			szLabel[11] = '\0';
			//if no name
			if( !memcmp(szLabel,"NO NAME",7))
				szLabel[0] = '\0';
			switch( pi.PartitionType & 0x0f)
			{
			case 0x01://fat12
				FormatDrive(chDriveLetter, szLabel,FORMAT_FAT_16,hWnd);
				break;
			case 0x04://fat16(old)
			case 0x06://fat16(big dos)
			case 0x0e://fat16(eint13)
			case 0x0f://fat16(int13)
				FormatDrive(chDriveLetter, szLabel,FORMAT_FAT_16,hWnd)
					||(nErrorCode=ERR_PARMAN_FORMATDRIVE);
				break;
			case 0x07://ntfs
				FormatDrive(chDriveLetter, szLabel,FORMAT_NTFS,hWnd)
					||(nErrorCode=ERR_PARMAN_FORMATDRIVE);
				break;
			case 0x0b://fat32(int13)
			case 0x0c://fat32(eint13)
				FormatDrive(chDriveLetter, szLabel,FORMAT_FAT_32,hWnd)
					||(nErrorCode=ERR_PARMAN_FORMATDRIVE);
				break;
			default:
				//unknow type
				break;
			}
		}
	}
	*pnError = nErrorCode;
	return !nErrorCode;
}

BOOL DLL_EXPORT Log2Pri(DWORD dwStartSec,
						  BYTE btHardDisk,
						  HWND hWnd)
{
	DRIVE_LAYOUT			DriveLayout;
	DISK_GEOMETRY			dg;
	int						nFirst,nLast;
	LARGE_INTEGER			lnStart;
	int						nFlag = 0;

	btHardDisk &= 0x7f;	
	//get neccesory information
	if(GetDiskGeometry(btHardDisk,&dg))
		return FALSE;
	if(GetDriveLayout(btHardDisk,&DriveLayout))
		return FALSE;
	//usb disk and signle partition
	if((DriveLayout.dli.PartitionCount % 4) != 0 )
		return FALSE;
	if( DriveLayout.dli.PartitionCount <= 4 )
		return FALSE;

	if( DriveLayout.dli.PartitionEntry[4].PartitionType == PARTITION_ENTRY_UNUSED )
	{
		nFirst = 8;
	}else
	{
		nFirst = 4;
	}

	nLast = DriveLayout.dli.PartitionCount - 4;
	
	lnStart.QuadPart = (__int64)dwStartSec * dg.BytesPerSector;
	if(lnStart.QuadPart == DriveLayout.dli.PartitionEntry[nFirst].StartingOffset.QuadPart )
	{
		nFlag |= DISKMAN_LOGTOPRI_FIRST;
	}
	if(lnStart.QuadPart == DriveLayout.dli.PartitionEntry[nLast].StartingOffset.QuadPart )
	{
		nFlag |= DISKMAN_LOGTOPRI_LAST;
	}
	//Invalid start
	if( nFlag == 0 )
		return FALSE;

	if( LogicalToPrimary(&DriveLayout,&dg,nFlag))
		return FALSE;

	if(SetDriveLayout(btHardDisk,&DriveLayout))
		return FALSE;
	else
		return TRUE;
}

BOOL DLL_EXPORT ResizeExtendPartition(BYTE					btHardDisk,
									  PPARTITION_ENTRY		ppeParEntry,
									  PINT					pnError)
{
	PARTITION_INFORMATION			piNew;
	int								nIndex;
	DRIVE_LAYOUT					DriveLayout;
	DISK_GEOMETRY					dg;

	btHardDisk &= 0x7f;
	if( GetDriveLayout(btHardDisk,&DriveLayout))
		return FALSE;
	if( GetDiskGeometry(btHardDisk,&dg))
		return FALSE;
	for( nIndex = 0 ; nIndex < 4 ; nIndex++ )
	{
		if( GetPartitionType(&DriveLayout,nIndex) == DISKMAN_PAR_EXTENDED_MBR )
			break;
	}
	if( nIndex < 4 )
	{
		//Get new information
		memset(&piNew,0,sizeof(PARTITION_INFORMATION));
		piNew.StartingOffset.QuadPart = (__int64)ppeParEntry->StartSector * dg.BytesPerSector;
		piNew.PartitionLength.QuadPart = (__int64)ppeParEntry->SectorsInPartition * dg.BytesPerSector;
		piNew.PartitionType = ppeParEntry->SystemFlag;
		if( ResizePrimary(&DriveLayout, &piNew, &dg, nIndex))
		{
			return FALSE;
		}else if ( SetDriveLayout( btHardDisk, &DriveLayout ))
		{
			return FALSE;
		}else
			return TRUE;
	}else
		return FALSE;
}

BOOL DLL_EXPORT	DoFormat(FORMAT_INFO & FormatInfo)
{
	BYTE	DriveLetter = RetrieveDriveLttr(FormatInfo.btDriveNum,0,FormatInfo.dwStartSec);
	TCHAR	szLabel[12];
	memcpy(szLabel,&FormatInfo.Label,11);
	szLabel[11] = '\0';
	if( !memcmp(szLabel,"NO NAME",7))
		szLabel[0] = '\0';
	return FormatDrive(DriveLetter,szLabel,FormatInfo.dwType,FormatInfo.hWnd);
}

BOOL DLL_EXPORT	FormatDrive(BYTE	DriveLetter, 
							LPSTR	szLabel, 
							DWORD	dwFileSystem,
							HWND	hWnd)
{
	HANDLE					hChildStdinRd, hChildStdinWr,hChildStdinWrDup, 
							hChildStdoutRd, hChildStdoutWr, hChildStdoutRdDup,
							hChildStderrRd, hChildStderrWr, hChildStderrRdDup;
	TCHAR					szCommand[MAX_PATH];
	DWORD					dwExitCode, dwWrited;;
	TCHAR					szEnter[] = "\x0d\x0a";
	TCHAR					szDrive[] = "X:";
	TCHAR					szYes[] = "y\x0d\x0a";
	TCHAR					szFileSystem[16];
	TCHAR					sz[MAX_PATH];
	PROCESS_INFORMATION		pi; 
	STARTUPINFO				si;

	if(!(
		( DriveLetter >= 'A' && DriveLetter <= 'Z') ||
		( DriveLetter >= 'a' && DriveLetter <= 'z')
		))
	{
		return FALSE;
	}else
	{
		szDrive[0] = DriveLetter;
	}

	switch(dwFileSystem)
	{
//	case FORMAT_FAT_12:
//		strcpy(szFileSystem,"FAT");
//		break;
	case FORMAT_FAT_16:
		strcpy(szFileSystem,"FAT");
		break;
	case FORMAT_FAT_32:
		strcpy(szFileSystem,"FAT32");
		break;
	case FORMAT_NTFS:
		strcpy(szFileSystem,"NTFS");
		break;
	default:
		//un known format
		return FALSE;		
	}
	ZeroMemory(szCommand, MAX_PATH* sizeof(TCHAR));
	//construct format command
//	sprintf(szCommand,"format.com  %s /q /x /v:\"%s\" /fs:%s",szDrive,szLabel,szFileSystem);
	sprintf(szCommand,"format.com  %s /q /x /fs:%s",szDrive,szFileSystem);

	if(!CreatePipes(hChildStdinRd,hChildStdinWr,hChildStdinWrDup,
					hChildStdoutRd,hChildStdoutWr,hChildStdoutRdDup,
					hChildStderrRd,hChildStderrWr,hChildStderrRdDup))
		return FALSE;
	//create format process
	ZeroMemory(&pi,sizeof(PROCESS_INFORMATION));
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.hStdInput = hChildStdinRd;
	si.hStdOutput = hChildStdoutWr;
	si.hStdError = hChildStderrWr;
	si.dwFlags = STARTF_USESTDHANDLES;

	CreateProcess(NULL,szCommand,NULL,NULL,TRUE,
//					0,NULL,NULL,&si,&pi);
					CREATE_NO_WINDOW,NULL,NULL,&si,&pi);
	//write format input
	ZeroMemory(sz,MAX_PATH);
	GetVolumeInformation(szDrive,sz,MAX_PATH, NULL, NULL, NULL, NULL, NULL);
	if(strlen(sz)>0)
	{
		strcat(sz,szEnter);
		WriteFile(hChildStdinWrDup,sz,strlen(sz),&dwWrited,NULL);
	}
	WriteFile(hChildStdinWrDup,szYes,strlen(szYes),&dwWrited,NULL);
	//set label
	WriteFile(hChildStdinWrDup,szLabel,strlen(szLabel),&dwWrited,NULL);
	WriteFile(hChildStdinWrDup,szEnter,strlen(szEnter),&dwWrited,NULL);
#ifdef _DEBUG
	ReadFile(hChildStdoutRdDup,sz,MAX_PATH,&dwWrited,NULL);
#endif
	//write additional Enter to avoid invalid volume label
	WriteFile(hChildStdinWrDup,szEnter,strlen(szEnter),&dwWrited,NULL);
	WriteFile(hChildStdinWrDup,szEnter,strlen(szEnter),&dwWrited,NULL);
	//wait for format	
	dwExitCode = QueryFormatProcess(hChildStdoutRdDup, pi.hProcess,hWnd);
	//close all pipes handle
	ClosePipes(	hChildStdinRd,hChildStdinWr,
				hChildStdoutRd,hChildStdoutWr,
				hChildStderrRd,hChildStderrWr);
	//close duplicate handles
	CloseHandle(hChildStdinWrDup);
	CloseHandle(hChildStdoutRdDup);
	CloseHandle(hChildStderrRdDup);
	if(dwExitCode == 0)
		return TRUE;
	else
		return FALSE;
}
/*
*	do delete and create in one step
*/
BOOL DLL_EXPORT	ChangeDriveLayout(BYTE					btHardDisk,
								  DWORD					dwStartSector,
								  CREATE_PAR_FLAG		flags,//indicate is logical or primary
								  DWORD					dwFlag,//create flags
								  PPARTITION_ENTRY		ppeParEntry,
								  int					*pnError)
{
	PROTECT_SECTOR	ps;
	return
	GetProtectSectors(	btHardDisk,
						dwStartSector,
						flags,
						dwFlag,
						ppeParEntry,
						&ps,
						TRUE,
						pnError);
}

BOOL DLL_EXPORT	GetProtectSectors(BYTE					btHardDisk,
								  DWORD					dwStartSector,
								  CREATE_PAR_FLAG		flags,//indicate is logical or primary
								  DWORD					dwFlag,//create flags
								  PPARTITION_ENTRY		ppeParEntry,
								  PPROTECT_SECTOR		pProtect,//[out]
								  BOOL					bWriteToDisk,									
								  int					*pnError)
{
	DISK_GEOMETRY				dg;
	DRIVE_LAYOUT				DriveLayout;
	int							nErrorCode; 
	int							nIndex;
	LARGE_INTEGER				lnStartByte;
	BOOL						bDeleteSuccess;
	PARTITION_INFORMATION		pi,piExtend;
	BYTE						btPartitionType;
	BOOL						bExistExtend,bNeedResize,bResizeSuccess;
	LARGE_INTEGER				lnExtendStart,lnExtendEnd;
	//viriable need to get the protect sectors
	BOOL						bIsDeleteLogical;
	BOOL						bIsCreateLogical;
	DWORD						dwExtOriginalHead,dwExtFinalHead;
	DWORD						dwOriginalHead,dwOriginalEnd;
	DWORD						dwFinalHead,dwFinalEnd;
	DWORD						dwOriginalEmbr,dwFinalEmbr;

	//p//init protect logic viriables
	memset( pProtect, 0, sizeof(PROTECT_SECTOR));
	bIsDeleteLogical = FALSE;
	bIsCreateLogical = FALSE;
	dwExtOriginalHead = 0;
	dwExtFinalHead = 0;
	dwOriginalHead = 0;
	dwOriginalEnd = 0;
	dwFinalHead = 0;
	dwFinalEnd = 0;
	dwOriginalEmbr = 0;
	dwFinalEmbr = 0;

	btHardDisk &= 0x7f;	
	bDeleteSuccess = FALSE;
	// 102 indicate false
	*pnError = nErrorCode = ERR_PARMAN_PARAM;
	//Get disk geometry and layout information
	if ( GetDiskGeometry(btHardDisk,&dg))
		return FALSE;
	if ( GetDriveLayout(btHardDisk, &DriveLayout))
		return FALSE;

	/************************************************
		Delete partition
	************************************************/
	//look for the partition
	lnStartByte.QuadPart = (__int64)dg.BytesPerSector * dwStartSector;
	for( nIndex = DriveLayout.dli.PartitionCount -1 ; nIndex >=0 ; nIndex-- )
	{
		if(DriveLayout.dli.PartitionEntry[nIndex].StartingOffset.QuadPart 
			== lnStartByte.QuadPart)
			break;
	}
	//usb disk
	if( ( DriveLayout.dli.PartitionCount == 1 ) && ( nIndex == 0 ) )
	{
		//p//back up the partition information
		memcpy(&pi,
				&DriveLayout.dli.PartitionEntry[nIndex],
				sizeof(PARTITION_INFORMATION));

		//zero partition table
		memset(	DriveLayout.dli.PartitionEntry,
				0,
				4*sizeof(PARTITION_INFORMATION) );
		
		DriveLayout.dli.PartitionCount = 4;
		
		for( nIndex = 0 ; nIndex < 4 ; nIndex++ )
			DriveLayout.dli.PartitionEntry[nIndex].RewritePartition = TRUE;

		bDeleteSuccess = TRUE;
	}else
	{
		//if found do delete
		if(nIndex >= 0)
		{
			switch( GetPartitionType(&DriveLayout, nIndex) )
			{
			case DISKMAN_PAR_UNUSED:
				break;
			case DISKMAN_PAR_EXTENDED_MBR:
				if ( flags.Extended )
				{
					//delete first logical
					nIndex = 4;
					//p//back up the partition information
					memcpy(&pi,
							&DriveLayout.dli.PartitionEntry[nIndex],
							sizeof(PARTITION_INFORMATION));

					bDeleteSuccess = !DeleteLogical(&DriveLayout,nIndex);
					bIsDeleteLogical = TRUE;
					break;
				}
				//else consider as primary
			case DISKMAN_PAR_PRIMARY_MBR:
				//p//back up the partition information
				memcpy(&pi,
						&DriveLayout.dli.PartitionEntry[nIndex],
						sizeof(PARTITION_INFORMATION));

				bDeleteSuccess = !DeletePrimary(&DriveLayout,nIndex);
				break;
			case DISKMAN_PAR_EXTENDED_EMBR:
				nIndex +=3;
			case DISKMAN_PAR_LOGICAL_EMBR:
				//p//back up the partition information
				memcpy(&pi,
						&DriveLayout.dli.PartitionEntry[nIndex],
						sizeof(PARTITION_INFORMATION));

				bDeleteSuccess = !DeleteLogical(&DriveLayout,nIndex);
				bIsDeleteLogical = TRUE;
				break;
			}
		}//if nIndex >= 0;
	}
	//delete fail
	if( bDeleteSuccess )
	{
		//p//pi now store the original start and end
		dwOriginalHead = (DWORD)(pi.StartingOffset.QuadPart / dg.BytesPerSector);
		dwOriginalEnd = dwOriginalHead + (DWORD)(pi.PartitionLength.QuadPart / dg.BytesPerSector);
		if( bIsDeleteLogical )
			dwOriginalEmbr = dwOriginalHead - pi.HiddenSectors;//dwOriginalEmbr == embr
	}else
	{
		return FALSE;
	}
	/************************************************
		Create partition
	************************************************/
	//check if usb sigle partition
	if((DriveLayout.dli.PartitionCount % 4) != 0 )
		return FALSE;

	//translate to partition information
	memset(&pi,0,sizeof(PARTITION_INFORMATION));
	//bootable?
	pi.BootIndicator = ppeParEntry->BootFlag ? 1:0;
	//file system
	pi.PartitionType = ppeParEntry->SystemFlag;
	//start
	pi.StartingOffset.QuadPart = (__int64)ppeParEntry->StartSector 
											* dg.BytesPerSector;
	//length
	pi.PartitionLength.QuadPart = (__int64)ppeParEntry->SectorsInPartition
											* dg.BytesPerSector;
	
	//detect if exist extend partition
	bExistExtend = FALSE;
	for( nIndex = 0 ; nIndex < 4 ; nIndex++ )
	{
		if ( GetPartitionType( &DriveLayout,nIndex) == DISKMAN_PAR_EXTENDED_MBR )
		{
			bExistExtend = TRUE;
			memcpy(&piExtend,&DriveLayout.dli.PartitionEntry[nIndex],
					sizeof(PARTITION_INFORMATION));
			lnExtendStart.QuadPart = piExtend.StartingOffset.QuadPart;
			lnExtendEnd.QuadPart = piExtend.PartitionLength.QuadPart;
			lnExtendEnd.QuadPart += lnExtendStart.QuadPart - 1;
			//p//Get extend original start
			dwExtOriginalHead = (DWORD)(lnExtendStart.QuadPart / dg.BytesPerSector);
			break;
		}
	}
	//do alignment
	if ( dwFlag == LOGICAL )
	{
		LegalizePartition(&pi,&dg,DISKMAN_PAR_LOGICAL_EMBR);
	}else
		LegalizePartition(&pi,&dg,DISKMAN_PAR_PRIMARY_MBR);
	
	bNeedResize = FALSE;
	bResizeSuccess = TRUE;
	if( dwFlag == LOGICAL )
	{
		bIsCreateLogical = TRUE;
		if( bExistExtend )
		{
			//beyond the extend low boundary
			if(	pi.StartingOffset.QuadPart  
				<
				lnExtendStart.QuadPart )
			{
				//justify the extend head
				piExtend.StartingOffset.QuadPart = pi.StartingOffset.QuadPart;
				piExtend.PartitionLength.QuadPart = lnExtendEnd.QuadPart
													- piExtend.StartingOffset.QuadPart
													+ 1;
				bNeedResize = TRUE;
			}
			//beyond the extend high boundary
			if( pi.StartingOffset.QuadPart + pi.PartitionLength.QuadPart 
				> 
				lnExtendEnd.QuadPart + 1 )
			{
				//justify the extend end
				//piExtend.StartingOffset has modified;
				piExtend.PartitionLength.QuadPart = pi.StartingOffset.QuadPart
													+ pi.PartitionLength.QuadPart
													- piExtend.StartingOffset.QuadPart;
				bNeedResize = TRUE;
			}
			if ( bNeedResize )
			{
				nErrorCode = ResizePrimary(&DriveLayout,&piExtend,&dg,nIndex);
				bResizeSuccess = ! nErrorCode;
			}
			//resize success create logical
			if( bResizeSuccess )
			{
				//p//get extend final head
				dwExtFinalHead = (DWORD)(piExtend.StartingOffset.QuadPart / dg.BytesPerSector);
				nErrorCode = CreateLogical(&DriveLayout,&pi,&dg);
			}
		}else
		{
			//no extend partition, should create one
			//Backup partition type;
			btPartitionType = pi.PartitionType;
			pi.PartitionType = PARTITION_EXTENDED;
			nErrorCode = CreatePrimary(&DriveLayout,&pi,&dg);
			if( !nErrorCode ) 
			{
				pi.PartitionType = btPartitionType;
				nErrorCode = CreateLogical(&DriveLayout,&pi,&dg);
			}
		}
	}else
	{
		// if primary overlap the extend then resize extend partition
		if( bExistExtend )
		{
			if( IsOverlap(&piExtend,&pi))
			{
				//try to resize extend partition forward
				piExtend.PartitionLength.QuadPart = pi.StartingOffset.QuadPart
													- lnExtendStart.QuadPart;
				//if length < 0 then length = 0
				if( piExtend.PartitionLength.QuadPart < 0 )
					piExtend.PartitionLength.QuadPart = 0;
				//resize forward
				nErrorCode = ResizePrimary(&DriveLayout,&piExtend,&dg,nIndex);
				if( nErrorCode != 0 )
				{
					//try to resize backward
					piExtend.StartingOffset.QuadPart =  pi.StartingOffset.QuadPart
														+ pi.PartitionLength.QuadPart;
					piExtend.PartitionLength.QuadPart = lnExtendEnd.QuadPart
														- piExtend.StartingOffset.QuadPart
														+ 1;
					nErrorCode = ResizePrimary(&DriveLayout,&piExtend,&dg,nIndex);
					
					bResizeSuccess = ! nErrorCode;
				}
			}
		}

		if( bResizeSuccess )
		{
			//p//get extend final head
			dwExtFinalHead = (DWORD)(piExtend.StartingOffset.QuadPart / dg.BytesPerSector);
			nErrorCode = CreatePrimary(&DriveLayout,&pi,&dg);
		}
	}
	//get the protect sectors group
	if( !nErrorCode )
	{
		//pi now store the partition information created
		dwFinalHead = (DWORD)(pi.StartingOffset.QuadPart / dg.BytesPerSector);
		dwFinalEnd = dwFinalHead + (DWORD)(pi.PartitionLength.QuadPart / dg.BytesPerSector);
		if( bIsCreateLogical )
		{
			dwFinalEmbr = dwFinalHead;//dwFinalEmbr == embr
			dwFinalHead += dg.SectorsPerTrack;
		}
		if( dwExtOriginalHead >= dwFinalHead &&
			dwExtOriginalHead <= dwFinalEnd - 1 )
		{
			pProtect->bProtectReadOne = TRUE;
			pProtect->dwProtectReadOne = dwExtOriginalHead;
		}
		if( bIsCreateLogical ^ bIsDeleteLogical )
		{
			//delete primary, create logical or
			//delete logical, create primary
			if( dwExtFinalHead >= dwOriginalHead &&
				dwExtFinalHead <= dwOriginalEnd - 1 )
			{
				pProtect->bProtectWriteOne = TRUE;
				pProtect->dwProtectWriteOne = dwExtFinalHead;
			}
		}
		if(	bIsDeleteLogical )
		{
			if( dwOriginalEmbr >= dwFinalHead &&
				dwOriginalEmbr <= dwFinalEnd - 1 )
			{
				pProtect->bProtectReadTwo = TRUE;
				pProtect->dwProtectReadTwo = dwOriginalEmbr;
			}
			if( bIsCreateLogical )
			{
				//delete logical, create logical
				if( dwFinalEmbr >= dwOriginalHead &&
					dwFinalEmbr <= dwOriginalEnd - 1 )
				{
					pProtect->bProtectWriteTwo = TRUE;
					pProtect->dwProtectWriteTwo = dwFinalEmbr;
				}
			}
		}
	}
	//if write set drive layout
	if( !nErrorCode && bWriteToDisk )
		nErrorCode = SetDriveLayout(btHardDisk, &DriveLayout, TRUE);
	//return
	*pnError = nErrorCode;
	return( nErrorCode == 0 );
}
