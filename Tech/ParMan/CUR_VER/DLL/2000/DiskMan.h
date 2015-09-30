/*================================================================
DiskMan.h
WQ
================================================================*/
#pragma once

#ifndef DISK_MAN
#define DISK_MAN


#include <stdio.h>
#include <windows.h>
#include <winioctl.h>


//logical to primary flags
#define DISKMAN_LOGTOPRI_FIRST			1
#define DISKMAN_LOGTOPRI_LAST			2

//partition entry types
#define DISKMAN_PAR_UNUSED				0
#define DISKMAN_PAR_PRIMARY_MBR			1
#define DISKMAN_PAR_LOGICAL_EMBR		2
#define DISKMAN_PAR_EXTENDED_MBR		3
#define DISKMAN_PAR_EXTENDED_EMBR		4
//align flag
#define DISKMAN_ALIGN_FORWARD			1
#define DISKMAN_ALIGN_BACKWARD			2
#define DISKMAN_ALIGN_HEAD1				3
//error code
#define DISKMAN_ERROR_SUCCESS								0
#define DISKMAN_ERROR_SYSTEM_IO_ERROR						1	
#define DISKMAN_ERROR_DELETE_EXTEND_EXIST_LOGICAL			2
#define DISKMAN_ERROR_DELETE_PRIMARY_NOT_PRIMARY			3
#define DISKMAN_ERROR_DELETE_LOGICAL_NOT_LOGICAL			4
#define DISKMAN_ERROR_CREATE_OVERLAP						5
#define DISKMAN_ERROR_CREATE_BEYOND_GEOMETRY				6
#define DISKMAN_ERROR_CREATE_PRIMARY_EXIST_EXTENDED			7
#define DISKMAN_ERROR_CREATE_PRIMARY_MBR_FULL				8
#define DISKMAN_ERROR_CREATE_LOGICAL_NO_EXTENDED			9
#define DISKMAN_ERROR_CREATE_LOGICAL_BEYOND_EXTENDED		10
#define DISKMAN_ERROR_RESIZE_PRIMARY_NOT_PRIMARY			11
#define DISKMAN_ERROR_RESIZE_PRIMARY_INVALID_END			12
#define DISKMAN_ERROR_RESIZE_PRIMARY_INVALID_BEGIN			13
#define DISKMAN_ERROR_NO_LOGICAL							14

#pragma pack(8)

#define MAX_DISKMAN_PAR_NUM				128

typedef struct
{
	DRIVE_LAYOUT_INFORMATION		dli;
	PARTITION_INFORMATION			pi[MAX_DISKMAN_PAR_NUM - 1];
}DRIVE_LAYOUT,*PDRIVE_LAYOUT;

#pragma pack()

/*------------------------------------------------------------------
GetDriveHandle()
Purpose:
	Return the specified physical drive's handle with READ and Write
	access;
Parameter:
Return Value:
	Return the right Handle, if fail return INVALIDE_HANDLE_VALUE
------------------------------------------------------------------*/
HANDLE  GetDriveHandle(int nHardDiskNum);

/*-------------------------------------------------------------------
GetDiskGeometry()
Purpose:
	Get the specified hard disk geometry;
Parameters:
	nHardDiskNum:
		Hard disk num of the target drive;
	pdg:
		Buffer holds the drive geometry information;
Return:
	0 if success, non 0 if fail;
-------------------------------------------------------------------*/
LRESULT  GetDiskGeometry(int nHardDiskNum, DISK_GEOMETRY *pdg);

/*-------------------------------------------------------------------
GetDriveLayout(),SetDriveLayout();
Purpose:
	Get and Set the specified hard disk partition layout;
Parameters:
	nHardDiskNum:
		Hard disk num of the target drive;
	pDriveLayout:
		Buffer holds the drive layout information;
Return:
	0 if success, non 0 if fail;
-------------------------------------------------------------------*/
LRESULT  GetDriveLayout(int nHardDiskNum, PDRIVE_LAYOUT pDriveLayout);
LRESULT  SetDriveLayout(int				nHardDiskNum, 
							  PDRIVE_LAYOUT		pDriveLayout,
							  BOOL				bLow = FALSE );

/*-------------------------------------------------------------------
GetPartitionInformation;
Purpose:
	Get the specified partition information;
Parameters:
	nHardDiskNum:
		Hard disk num of the target drive;
Return:
	0 if success, non 0 if fail;
-------------------------------------------------------------------*/
LRESULT  GetPartitionInformation(int						nHardDiskNum,
								 int						nPartitionNum,
								 PPARTITION_INFORMATION		ppi);
/*-----------------------------------------------------------------------
GetPartitionType;
Purpose:
Parameter:
Return Value:
	PARTITION_UNUSED, PARTITION_PRIMARY_MBR, PARTITION_EXTENDED_MBR,
	PARTITION_EXTENDED_EMBR, PARTITION_LOGICAL_EMBR
-----------------------------------------------------------------------*/
int  GetPartitionType(PDRIVE_LAYOUT pDriveLayout, int nIndex);

/*----------------------------------------------------------------------
DeletePrimary(),DeleteLogical();
Purpose:
	Delete the specfied partition;
Parameter:
	pDriveLayout, [in,out], drive layout information;
	nIndex, [in], the specified partition;
Return:
	0 if success, otherwise non 0;
----------------------------------------------------------------------*/
LRESULT  DeletePrimary(PDRIVE_LAYOUT pDriveLayout,  int nIndex);
LRESULT  DeleteLogical(PDRIVE_LAYOUT pDriveLayout,  int nIndex);


/*---------------------------------------------------------------------
CreatePrimary(),CreateLogical();
Purpose:
	Create a partition;
Parameter:
	pDriveLayout, [in,out], drive layout information;
	ppi, [in], the partition information to create;
	pdg, [in], disk geometry information(CreateLogical only);
Return:
	0 if success, otherwise non 0;
----------------------------------------------------------------------*/
LRESULT  CreatePrimary(PDRIVE_LAYOUT				pDriveLayout,
					   PARTITION_INFORMATION		*ppi,
					   DISK_GEOMETRY				*pdg);

LRESULT  CreateLogical(PDRIVE_LAYOUT				pDriveLayout,
					   PARTITION_INFORMATION		*ppi,
					   DISK_GEOMETRY				*pdg);

/*----------------------------------------------------------------------------
ResizePrimary(),ResizeLogical();
Purpose:
	Resize a partition;
Parameter:
	pDriveLayout, [in,out], drive layout information;
	ppi, [in], the partition information to create;
Return:
	0 if succeed;
----------------------------------------------------------------------------*/
LRESULT  ResizePrimary(PDRIVE_LAYOUT					pDriveLayout,
							 PARTITION_INFORMATION		*ppiNew,
							 DISK_GEOMETRY				*pdg,
							 int						nIndex);

LRESULT  ResizeLogical(PDRIVE_LAYOUT					pDriveLayout,
					   PARTITION_INFORMATION			*ppiNew,
					   DISK_GEOMETRY					*pdg,
					   int								nIndex);

/*---------------------------------------------------------------------------
LogicalToPrimary
Purpose:
Parameter:
	pDriveLayout,[in,out],drive layout information;
	nFirstOrLast,[in], 0 indictate the first logical,other wise the last;
Return:
	0 if success;
---------------------------------------------------------------------------*/
LRESULT  LogicalToPrimary(PDRIVE_LAYOUT				pDriveLayout,
						  DISK_GEOMETRY				*pdg,
						  int						nFlag);

/*---------------------------------------------------------------------------
AlignSectorOffset
Purpose:
	Do alignment;
Parameter:
Return:
	0 - total sectors - 1;
---------------------------------------------------------------------------*/
DWORD  AlignSectorOffset(DWORD			dwSectorOffset,
							   DISK_GEOMETRY	*pdg,
							   DWORD			dwAlignFlag);

/*---------------------------------------------------------------------------
GetDriveLetter,GetDriveLocation;
Purpose:
	Get the drive letter or do in reverse;
Parameter:
	nDrive, [in], 0 - 25 indicate a - z;
Return:
	0 - 25 indicate a - z;
	if fail nHardDisk and nPartitionNum are -1;
---------------------------------------------------------------------------*/
int  GetDriveLetter(int nHardDisk,
						  int nPartitionNum);
void  GetDriveLocation(int nDrive,int& nHardDisk,int& nPartitionNum);


/*----------------------------------------------------------------------
LegalizePartition();
Purpose:
	align partition boundary and check the partition type(system id);
----------------------------------------------------------------------*/
void  LegalizePartition(PPARTITION_INFORMATION		ppi,
							  DISK_GEOMETRY					*pdg,
							  DWORD							dwFlag);
/*---------------------------------------------------------------------
IsOverlap;
Purpose:
	Detect is two partition is overlap;
Parameter:
Return:
	TRUE if overlap;
---------------------------------------------------------------------*/
BOOL  IsOverlap(PPARTITION_INFORMATION	ppiFirst,
					  PPARTITION_INFORMATION	ppiSecond);

#endif//DISK_MAN