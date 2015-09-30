/*==============================================================
ParInfoPrivate.h
Purpose:
	ParInfo private function's declaration;
=============================================================*/
#ifndef PARINFO_PRIVATE_H
#define PARINFO_PRIVATE_H

#pragma once

#include "..\..\export\2000\ParInfo.h"

#define MAX_PARTITION_NUM			128
#define MAX_LETTER_NUM				26

#define MAKE_DRIVE_NUM(btDriveNum) ((btDriveNum)&(0x7f))

#pragma pack(8)

typedef struct tagGET_DRIVE_LAYOUT
{
	DRIVE_LAYOUT_INFORMATION		dliDrive;
	PARTITION_INFORMATION			piDrive[MAX_PARTITION_NUM - 1];
} GET_DRIVE_LAYOUT,*PGET_DRIVE_LAYOUT;

typedef struct tagDRIVE_LETTER_INFORMATION
{
	BYTE		btPartitionType;
	BYTE		btHardDiskNum;
	DWORD		dwStartSector;
}DRIVE_LETTER_INFORMATION, *PDRIVE_LETTER_INFORMATION;

/*------------------------------------------------------------------
GetDriveHandle()
Purpose:
	Return the specified physical drive's handle with READ and Write
	access;
Parameter:
	btDrive:
		Physical drive number;
Return Value:
	Return the right Handle, if fail return INVALIDE_HANDLE_VALUE
------------------------------------------------------------------*/
HANDLE GetDriveHandle(BYTE		btDrive);

/*--------------------------------------------------------------------------------------
SearchPartitions()
Purpose:
	Convert the drive layout information struct retrieved from win2k system to hard 
	disk partition information struct;
Parameters:
	pdliDrive:
		pointer to drive layout informtion struct;
	ppihd:
		pointer to hard disk partition information struct;
	pbdp:
		pointer to the physical hard disk parameters struct;
Return Values:
	Void;
---------------------------------------------------------------------------------------*/
VOID SearchPartitions(GET_DRIVE_LAYOUT			*pdliDrive, 
					  PPARINFOONHARDDISK		ppihd,
					  PBIOS_DRIVE_PARAM			pbdp);

/*--------------------------------------------------------------------------------------
BytesOffsetToSectorsOffset()
Purpose:
	Convert the partition's offset counted in  Bytes to counted in sectors;
Parameters:
	pbdp:
		pointer to the physical hard disk parameters struct;
	lnBytesOffset:
		Offset in bytes;
Return Value:
	Offset in sectors; return -1 if exist alignment error;
--------------------------------------------------------------------------------------*/
DWORD BytesOffsetToSectorsOffset(PBIOS_DRIVE_PARAM		pbdp,
								 LARGE_INTEGER			lnBytesOffset);

/*--------------------------------------------------------------------------------
MakePartitionPhysicalEntry()
Purpose:
	Read the partition information struct and make a relevant physical partition
	entry in the MBR or EMBR;
Parameters:
	ppi:
		pointer to a PARTITION_INFORMATION struct;
	ppe:
		pointer to a physical partition entry;
	pbdp:
		pointer to the physical hard disk parameters struct;
	dwFlag:
		is a primary or a logical, or embr
	dwExtendStart:
		the extend partition start, not use while dwFlag is primary
Return value:
	TRUE;
--------------------------------------------------------------------------------*/
BOOL MakePartitionPhysicalEntry(PARTITION_INFORMATION		*ppi,
								PARTITION_ENTRY				*ppe,
								PBIOS_DRIVE_PARAM			pbdp);

/*------------------------------------------------------------------------------
FromRelativeToCHS()
Purpose:
	Convert the sector relative offset to CHS format;
Parameter:
	pbdp:
		pointer to the physical hard disk parameters struct;
	dwRelativeSector:
		sector relative offset;
	btCHS[3]:
		sector offset in Cylinder-Head-Sector format;
Return value:
	No;
-------------------------------------------------------------------------------*/
void FromRelativeToCHS(PBIOS_DRIVE_PARAM		pbdp,
					   DWORD					dwRelativeSector,
					   BYTE						btCHS[3]);
/*-------------------------------------------------------------------------------
DriveLetterOnWhichDisk();
Parameters:
	btDriveLetter:
		Drive letter;
Return:
	Hard disk num on which a partition is assigned the drive letter;
	if the letter is not used then return 0xff;
-------------------------------------------------------------------------------*/
BOOL DriveLetterInformation(BYTE							btDriveLetter,
							PDRIVE_LETTER_INFORMATION		pDriveLetterInfo);
/*
Use symbolic to detect drive letter
on which hard disk
*/
int DriveOnWhichHardDisk(char		chDriveLetter,
						 int		nPartitionNum);

BOOL GetDriveLayout(int						nHardDisk,
					PGET_DRIVE_LAYOUT		pdli);

int FindInDriveLayout(DWORD					dwStart,
					  PGET_DRIVE_LAYOUT		pdli);

BOOLEAN
PrependSzToMultiSz(
    IN		LPTSTR  SzToPrepend,
    IN		LPTSTR  MultiSz
    );

size_t
MultiSzLength(
    IN LPTSTR MultiSz
    );

LPTSTR
MultiSzSearch(
	IN  LPTSTR FindThis,
	IN  LPTSTR FindWithin
	);

size_t
MultiSzSearchAndDeleteCaseInsensitive(
    IN  LPTSTR  FindThis,
    IN  LPTSTR  FindWithin,
    OUT size_t  *NewStringLength
    );

#endif //PARINFO_PRIVATE_H