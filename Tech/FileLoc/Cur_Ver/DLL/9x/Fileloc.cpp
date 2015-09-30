/******************************************************************************
*	FileLoc.CPP    MODULE
*
*	AUTHOR	:   ZL
*
*	Copyright 1999 - Shanghai YuGuang Science & Technology Co. Ltd .
*	All rights reserved.
*
*	Date:	01-Dec-1999
*
******************************************************************************/
#include    <windows.h>
#include    <string.h>
#include    <assert.h>
#include    "..\..\Export\9x\fileloc.h"
#include    "fileloc.h"
#include	"..\..\..\..\shark\Cur_Ver\Dll\CVxD.h"
#include	"..\..\..\..\Shark\Cur_Ver\Dll\9x\CVdskLdr.h"
#include	"..\..\..\..\Shark\Cur_Ver\Dll\9x\CVirPar.h"
#include	"..\..\..\..\vportdrv\cur_ver\Export\9x\vportdrv.h"
#include	"..\..\..\..\Shark\Cur_Ver\Dll\9x\Misc.h"


#define ASSERT	assert


#ifndef	DIOC_REG
#define	DIOC_REG

//  In vwin32.h
// DIOCRegs
// Structure with i386 registers for making DOS_IOCTLS
// vwin32 DIOC handler interprets lpvInBuffer , lpvOutBuffer to be this struc.
// and does the int 21
// reg_flags is valid only for lpvOutBuffer->reg_Flags
typedef struct DIOCRegs	{
	DWORD	reg_EBX;
	DWORD	reg_EDX;
	DWORD	reg_ECX;
	DWORD	reg_EAX;
	DWORD	reg_EDI;
	DWORD	reg_ESI;
	DWORD	reg_Flags;		
	} DIOC_REGISTERS;


#endif	//	DIOC_REG	

//  Global Variable
HANDLE	    g_hVxD  =	INVALID_HANDLE_VALUE;

BOOL EXPORT	Init_FileLocater(VOID)
{
    DWORD	    dw_cb;
    DIOC_REGISTERS  reg;
    BOOL	    bCheck;

    g_hVxD = CreateFile("\\\\.\\FileLocH.VxD",0,0,NULL,0,FILE_FLAG_DELETE_ON_CLOSE,NULL);
    if(g_hVxD == INVALID_HANDLE_VALUE)
	return	FALSE;

    bCheck = DeviceIoControl(g_hVxD,FILELOCH_GetVersion,
			     &reg,sizeof(DIOC_REGISTERS),
			     &reg,sizeof(DIOC_REGISTERS),
			     &dw_cb,0);
    if(!bCheck)
	goto	Init_Error;

    if(!reg.reg_EAX)
	goto	Init_Error;

    return  TRUE;

Init_Error:
    if(INVALID_HANDLE_VALUE != g_hVxD)
    {
	bCheck = CloseHandle(g_hVxD);
	ASSERT(bCheck);
	g_hVxD = INVALID_HANDLE_VALUE;
    }

    return  FALSE;
}   //	Init_FileLocater



PVOID EXPORT OpenBuffer(DWORD dwCount)
{
    DWORD	    dw_cb;
    DIOC_REGISTERS  reg;
    BOOL	    bCheck;

    ASSERT(INVALID_HANDLE_VALUE != g_hVxD);
    reg.reg_ECX = dwCount;
    bCheck = DeviceIoControl(g_hVxD,FILELOCH_OpenBuffer,
			     &reg,sizeof(DIOC_REGISTERS),
			     &reg,sizeof(DIOC_REGISTERS),
			     &dw_cb,0);
    if(!bCheck)
		return	0;

    return  (PVOID)reg.reg_EAX;
}

BOOL EXPORT CloseBuffer(PVOID pBuf)
{
    DWORD	    dw_cb;
    DIOC_REGISTERS  reg;
    BOOL	    bCheck;

    ASSERT(INVALID_HANDLE_VALUE != g_hVxD);
    reg.reg_EAX = (DWORD)pBuf;
    bCheck = DeviceIoControl(g_hVxD,FILELOCH_CloseBuffer,
			     &reg,sizeof(DIOC_REGISTERS),
			     &reg,sizeof(DIOC_REGISTERS),
			     &dw_cb,0);
    if(!bCheck)
		return	0;
    return (BOOL)reg.reg_EAX;
}

BOOL EXPORT CopyData(PVOID pTar, PVOID pSrc, DWORD nCount)
{
    DWORD	    dw_cb;
    DIOC_REGISTERS  reg;
    BOOL	    bCheck;

    ASSERT(INVALID_HANDLE_VALUE != g_hVxD);
    reg.reg_ECX = (DWORD)nCount;
    reg.reg_ESI = (DWORD)pSrc;
    reg.reg_EDI = (DWORD)pTar;
    bCheck = DeviceIoControl(g_hVxD,FILELOCH_CopyData,
			     &reg,sizeof(DIOC_REGISTERS),
			     &reg,sizeof(DIOC_REGISTERS),
			     &dw_cb,0);
    if(!bCheck)
		return	0;
    return (BOOL)reg.reg_EAX;
}

DWORD	EXPORT	Open_FL_Handle(LPCSTR	lpszPathName)
{
    DWORD	    dw_cb;
    DIOC_REGISTERS  reg;
    BOOL	    bCheck;

    ASSERT(INVALID_HANDLE_VALUE != g_hVxD);

    reg.reg_EAX = (DWORD)lpszPathName;

    bCheck = DeviceIoControl(g_hVxD,FILELOCH_OpenHandle,
			     &reg,sizeof(DIOC_REGISTERS),
			     &reg,sizeof(DIOC_REGISTERS),
			     &dw_cb,0);
    if(!bCheck)
	return	0;

    return  reg.reg_EAX;
}   //	Open_FL_Handle


DWORD	EXPORT	Close_FL_Handle(DWORD dwHandle)
{
    DWORD	    dw_cb;
    DIOC_REGISTERS  reg;
    BOOL	    bCheck;

    ASSERT(INVALID_HANDLE_VALUE != g_hVxD);

    reg.reg_EAX = dwHandle;

    bCheck = DeviceIoControl(g_hVxD,FILELOCH_CloseHandle,
			     &reg,sizeof(DIOC_REGISTERS),
			     &reg,sizeof(DIOC_REGISTERS),
			     &dw_cb,0);
    if(!bCheck)
	return	0;

    return  reg.reg_EAX;
}   //	Close_FL_Handle


BOOL	EXPORT	LogicSector2PhysSector(DWORD dwHandle,DWORD dwLogicSectorNo,
				       __int64	*pqwPhysSectorNo)
{
	return	LogicSector2PhysSectorEx(dwHandle,dwLogicSectorNo,pqwPhysSectorNo,1);
}

BOOL	EXPORT	LogicSector2PhysSectorEx(DWORD dwHandle,DWORD dwLogicSectorNo,
				       __int64	*pqwPhysSectorNo, int nSectors)
{
    DWORD	    dw_cb;
    DIOC_REGISTERS  reg;
    BOOL	    bCheck;

    ASSERT(INVALID_HANDLE_VALUE != g_hVxD);

    reg.reg_EAX = dwHandle;
    reg.reg_ECX = nSectors;
    reg.reg_EDX = dwLogicSectorNo;
    reg.reg_EBX = (DWORD)pqwPhysSectorNo;
    bCheck = DeviceIoControl(g_hVxD,FILELOCH_LogicSector2PhysSector,
			     &reg,sizeof(DIOC_REGISTERS),
			     &reg,sizeof(DIOC_REGISTERS),
			     &dw_cb,0);
    if(!bCheck)
		return	0;

    return  (BOOL)reg.reg_EAX;
}   //	LogicSector2PhysSector

BOOL	EXPORT LogicSector2PhysSectorFast(DWORD dwHandle, DWORD dwLogicalSec, PVOID pPhysSecArr, DWORD dwArrIndex, int nSectors)
{
    DWORD	    dw_cb;
    DIOC_REGISTERS  reg;
    BOOL	    bCheck;

    ASSERT(INVALID_HANDLE_VALUE != g_hVxD);

    reg.reg_EAX = dwHandle;
    reg.reg_ECX = nSectors;
    reg.reg_EDX = dwLogicalSec;
    reg.reg_EBX = (DWORD)pPhysSecArr;
    reg.reg_ESI = dwArrIndex;
    bCheck = DeviceIoControl(g_hVxD,FILELOCH_LogicSector2PhysSectorFast,
			     &reg,sizeof(DIOC_REGISTERS),
			     &reg,sizeof(DIOC_REGISTERS),
			     &dw_cb,0);
    if(!bCheck)
		return	0;

    return  (BOOL)reg.reg_EAX;
}

BOOL	EXPORT SetPhysSectors(PVOID pPhysSecArr, DWORD dwArrIndex, int nSectors, DWORD dwValue)
{
    DWORD	    dw_cb;
    DIOC_REGISTERS  reg;
    BOOL	    bCheck;

    ASSERT(INVALID_HANDLE_VALUE != g_hVxD);

    reg.reg_EAX = dwValue;
    reg.reg_ECX = nSectors;
    reg.reg_EBX = (DWORD)pPhysSecArr;
    reg.reg_ESI = dwArrIndex;
    bCheck = DeviceIoControl(g_hVxD,FILELOCH_SetPhysSectors,
			     &reg,sizeof(DIOC_REGISTERS),
			     &reg,sizeof(DIOC_REGISTERS),
			     &dw_cb,0);
    if(!bCheck)
		return	0;

    return  (BOOL)reg.reg_EAX;
}


BOOL	EXPORT FileLocatorFlush(DWORD dwVol)
{
    DWORD	    dw_cb;
    DIOC_REGISTERS  reg;
    BOOL	    bCheck;

    ASSERT(INVALID_HANDLE_VALUE != g_hVxD);
    reg.reg_EBX = dwVol;
    bCheck = DeviceIoControl(g_hVxD,FILELOCH_Flush,
			     &reg,sizeof(DIOC_REGISTERS),
			     &reg,sizeof(DIOC_REGISTERS),
			     &dw_cb,0);
    if(!bCheck)
		return	0;

    return  (BOOL)reg.reg_EAX;
}

BOOL EXPORT EnableDisableFileLocator(BOOL bEnable)
{
    DWORD	    dw_cb;
    DIOC_REGISTERS  reg;
    BOOL	    bCheck;

    ASSERT(INVALID_HANDLE_VALUE != g_hVxD);
    reg.reg_EAX = (DWORD)bEnable;
    bCheck = DeviceIoControl(g_hVxD,FILELOCH_EnableDisable,
			     &reg,sizeof(DIOC_REGISTERS),
			     &reg,sizeof(DIOC_REGISTERS),
			     &dw_cb,0);
    if(!bCheck)
		return	0;

    return  (BOOL)reg.reg_EAX;
}

BOOL EXPORT	Free_FileLocater(VOID)
{
    BOOL    bCheck;

	if (INVALID_HANDLE_VALUE != g_hVxD)
	{
	    bCheck = CloseHandle(g_hVxD);
		ASSERT(bCheck);
	}
    return  TRUE;
}   //	Free_FileLocater


BOOL	EXPORT	EnableDisable_FL(BOOL	bEnableDisable)
{
    DWORD	    dw_cb;
    DIOC_REGISTERS  reg;
    BOOL	    bCheck;
    HANDLE	    hVSD;

    #define VSD_Version 		0x01
    #define VSD_Enable_Disable_Func	0x02

    hVSD = CreateFile("\\\\.\\FileLocL.VxD",0,0,NULL,0,FILE_FLAG_DELETE_ON_CLOSE,NULL);
    if(hVSD == INVALID_HANDLE_VALUE)
	return	FALSE;

    reg.reg_EAX = bEnableDisable;

    bCheck = DeviceIoControl(hVSD,VSD_Enable_Disable_Func,
			     &reg,sizeof(DIOC_REGISTERS),
			     &reg,sizeof(DIOC_REGISTERS),
			     &dw_cb,0);
    CloseHandle(hVSD);
    return  bCheck;
}   //	EnableDisable_FL


//	1 based	( 0 is default drive )
BOOL	EXPORT GuardPartition(BYTE btDrive)
{

	if( ! LockDrive( btDrive , 1 , 2 ) )
		return	FALSE;

	if( ! LockDrive( btDrive , 2 , 2 ) )
	{
		UnlockDrive( btDrive ); 
		return	FALSE;
	}
			
	return	TRUE;																	
}	//	GuardPartition

//	1 based	( 0 is default drive )
BOOL	EXPORT UngardPartition(BYTE btDrive)
{
	if( ! UnlockDrive( btDrive ) )
	{
		UnlockDrive( btDrive );		
		return	FALSE;	
	}																
	if( ! UnlockDrive( btDrive ) )
		return	FALSE;

	return	TRUE;
}	//	UngardPartition



DWORD  LogicToPhy(DWORD dwSectors,PBIOS_DRIVE_PARAM pDriveParam)
{
	DWORD	dwTrack;
	DWORD	dwHead;
	DWORD	dwSector;
	dwSector = (dwSectors % pDriveParam->dwSecPerTrack)+1;
	dwHead   = (dwSectors / pDriveParam->dwSecPerTrack) % pDriveParam->dwHeads ;
	dwTrack  = (dwSectors / pDriveParam->dwSecPerTrack) / pDriveParam->dwHeads ;
	return ((dwTrack & 0x00FF) << 24) | ((dwTrack & 0x0300) << 14) | (dwHead << 8) | (dwSector << 16);
}	//	LogicToPhy	


/*
VOID	AssignTCH( PPARTITION_ENTRY ppeParEntry , DWORD dwStarting , DWORD dwEnding , PBIOS_DRIVE_PARAM	pParams )
{
	DWORD	dwPhySector; 

	if( dwEnding < MAX_CAPACITY_SUPPORTED_BY_OLD_INT13 )
	{	
		dwPhySector = LogicToPhy( dwStarting , pParams  );
		ppeParEntry->StartOfPartition[0] = HIBYTE(LOWORD(dwPhySector));
		ppeParEntry->StartOfPartition[1] = LOBYTE(HIWORD(dwPhySector));
		ppeParEntry->StartOfPartition[2] = HIBYTE(HIWORD(dwPhySector));

		dwPhySector = LogicToPhy( dwEnding , pParams ); 
		ppeParEntry->EndOfPartition[0] = HIBYTE(LOWORD(dwPhySector));
		ppeParEntry->EndOfPartition[1] = LOBYTE(HIWORD(dwPhySector));
		ppeParEntry->EndOfPartition[2] = HIBYTE(HIWORD(dwPhySector));
	}
	else if( dwStarting < MAX_CAPACITY_SUPPORTED_BY_OLD_INT13 )
	{
		dwPhySector = LogicToPhy( dwStarting , pParams  );
		ppeParEntry->StartOfPartition[0] = HIBYTE(LOWORD(dwPhySector));
		ppeParEntry->StartOfPartition[1] = LOBYTE(HIWORD(dwPhySector));
		ppeParEntry->StartOfPartition[2] = HIBYTE(HIWORD(dwPhySector));

		ppeParEntry->EndOfPartition[0] = 0xFE;
		ppeParEntry->EndOfPartition[1] = 0xFF;
		ppeParEntry->EndOfPartition[2] = 0xFF;
	}
	else
	{
		ppeParEntry->StartOfPartition[0] = 0x00; 
		ppeParEntry->StartOfPartition[1] = 0xC1;
		ppeParEntry->StartOfPartition[2] = 0xFF;

		ppeParEntry->EndOfPartition[0] = 0xFE;
		ppeParEntry->EndOfPartition[1] = 0xFF;
		ppeParEntry->EndOfPartition[2] = 0xFF;
	}	
}	//	AssignTCH

*/

//	btDrive = 0	: A
//			= 1	: B
//		
//		zero based
//	
BOOL	MakeMirrorDrive(DRIVE_MAP Drive[])
{
	HKEY				hKey;
	VIRTUAL_DISK		mirror_disk;
	int					i;
	CVirParti			*pVirtuialPartition_OBJ;
	CYgDskLdr			*pVDiskLdr_OBJ;
    PARTITION_INFO		ParInfo;
	BIOS_DRIVE_PARAM	BiosParams;
	RESIZE_INFO			ResizeInfo;


	//	inform the vportdrv.pdr to simulate one mirror disk
	if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE , VPORTDRV_KEY , 0 , KEY_SET_VALUE , &hKey ) )
		if( ERROR_SUCCESS != RegCreateKey( HKEY_LOCAL_MACHINE , VPORTDRV_KEY , &hKey ) )
			return	FALSE;
	memset( &mirror_disk , 0 , sizeof(VIRTUAL_DISK) );
	mirror_disk.dwVDiskNum = 1;		//	only one mirror disk
	memcpy( mirror_disk.vdisk[0].strType , MIRROR_DISK , REV_LEVEL_LEN );
	if( ERROR_SUCCESS != RegSetValueEx( hKey , VPORTDRV_DATA , NULL , REG_BINARY ,
									(PBYTE)&mirror_disk , sizeof(VIRTUAL_DISK) ) )
		return	FALSE;

	if(	ERROR_SUCCESS != RegFlushKey( hKey ) )
		return	FALSE;

	RegCloseKey( hKey );

	pVirtuialPartition_OBJ = new CVirParti;

	if( pVirtuialPartition_OBJ == NULL )
		return	FALSE;

	if( ! pVirtuialPartition_OBJ->Initialize("\\\\.\\Virparti")	)
		return	FALSE;

	if(!Init_PartitionInfo())
		return	FALSE;

	if(!PartitionInfoOfDriveLetter(Drive[0].original_drive+1,&ParInfo))
		return FALSE;

	if (!GetDriveParam(ParInfo.btHardDisk,&BiosParams))	
		return FALSE;

	memset(&ResizeInfo,0,sizeof(RESIZE_INFO));
	ResizeInfo.MirrorType    = ParInfo.dwPartitionType;
	ResizeInfo.MirrorSysID   = ParInfo.btSystemFlag;	
	ResizeInfo.StartOfMP     = ParInfo.dwStartLogicalSector;
	ResizeInfo.EndOfMP       = ParInfo.dwStartLogicalSector+ParInfo.dwSectorsInPartition-1;
	if (!pVirtuialPartition_OBJ->SetDriveParameter(&ResizeInfo,&BiosParams)) 
		return FALSE;

	pVDiskLdr_OBJ = new CYgDskLdr;
	if( pVDiskLdr_OBJ == NULL )
		return	FALSE;

	if( ! pVDiskLdr_OBJ->Initialize("\\\\.\\YgDskLdr.VxD") )
		return	FALSE;

	if( ! pVDiskLdr_OBJ->Virtualize(Drive[0].original_drive,0xFF))
		return	FALSE;

	if( ! pVDiskLdr_OBJ->LoadYgDisk( ) )
		return	FALSE;
		
	for( i = 0; i < MAX_DRIVE_MAP; i++)
	{
		if( Drive[i].original_drive == 0 )
			break;		
		Drive[i].mirror_drive =	pVirtuialPartition_OBJ->Real2MirrorDriveLttr(Drive[i].original_drive);
		if( Drive[i].mirror_drive == 0xFF )
		{
			pVDiskLdr_OBJ->UnloadYgDisk();			
				return	FALSE;
		}
	}
	return	TRUE;
}	//	MakeMirrorDrive


BOOL	DiscardMirrorDrive( )
{
	CYgDskLdr			*pVDiskLdr_OBJ;
//	int					n;
	int					nDriveIndex = 0;
//	BOOL				bFlag;
//	BYTE				btMirrorDrive[26];

	pVDiskLdr_OBJ = new CYgDskLdr;
	if( pVDiskLdr_OBJ == NULL )
		return	FALSE;
	/*

	memset( btMirrorDrive , 0 , sizeof(btMirrorDrive) );
	if( GetTotalMirrorDrive( btMirrorDrive ) )
	{
		while(btMirrorDrive[nDriveIndex])
		{		
			n = 0;
			while(TRUE)
			{
				bFlag = EnumerateOpenHandle(btMirrorDrive[nDriveIndex] + 1);	//	1 based

			}

			nDriveIndex ++; 						
		}
	}

	*/

	if( ! pVDiskLdr_OBJ->Initialize("\\\\.\\YgDskLdr.VxD") )
		return	FALSE;
	
	return	pVDiskLdr_OBJ->UnloadYgDisk();
}	//	DiscardMirrorDrive
