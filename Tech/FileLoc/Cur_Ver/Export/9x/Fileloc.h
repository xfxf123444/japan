/******************************************************************************
*	FileLoc.H   header file
*
*	AUTHOR	:   ZL
*
*	Copyright 1999 - Shanghai YuGuang Science & Technology Co. Ltd .
*	All rights reserved.
*
*	Date:	02-Dec-1999
*
******************************************************************************/
#ifndef FILELOC_H
#define FILELOC_H

#define FILELOCH_GetVersion				    0x01
#define FILELOCH_OpenHandle					0x02
#define FILELOCH_CloseHandle				0x03
#define FILELOCH_LogicSector2PhysSector     0x04
#define FILELOCH_LogicSector2PhysSectorFast 0x05
#define FILELOCH_Flush						0x06
#define FILELOCH_OpenBuffer					0x07
#define FILELOCH_CloseBuffer				0x08
#define FILELOCH_CopyData					0x09
#define FILELOCH_EnableDisable				0x0A
#define FILELOCH_SetPhysSectors				0x0B

#ifndef EXPORT
#define EXPORT	__declspec(dllexport)
#endif


#ifndef	MAX_CAPACITY_SUPPORTED_BY_OLD_INT13
#define	MAX_CAPACITY_SUPPORTED_BY_OLD_INT13		63 * 1024 *	256
#endif	

#define	MAX_DRIVE_MAP	10		

typedef	struct	tagDRIVE_MAP
	{
		BYTE	original_drive;
		BYTE	mirror_drive;
	} DRIVE_MAP,*PDRIVE_MAP;	

typedef struct	tagResizeInfo
{
	DWORD	Type	:   4;
	DWORD	Index	:   4;
	DWORD	SysID	:	8;
	DWORD	ExtParBase;
	DWORD	PrevParStart;
	DWORD	PrevParEnd;
	DWORD	StartOfVP;
	DWORD	EndOfVP;
	DWORD	NextParStart;
	DWORD	NextParEnd;
	DWORD	MirrorType	:   4;
	DWORD	MirrorSysID	:	8;
	DWORD	StartOfMP;
	DWORD	EndOfMP;
	DWORD	SharkType	:   4;
	DWORD	SharkSysID	:	8;
	DWORD	StartOfSP;
	DWORD	EndOfSP;
} RESIZE_INFO,*PRESIZE_INFO;



extern	"C"
{
	
	BOOL	EXPORT	Init_FileLocater(VOID);
	DWORD	EXPORT	Open_FL_Handle(LPCSTR);
	DWORD	EXPORT	Close_FL_Handle(DWORD);
	BOOL	EXPORT	LogicSector2PhysSector(DWORD dwHandle,DWORD dwLogicSectorNo,
				       __int64	*pqwPhysSectorNo);
	BOOL	EXPORT	LogicSector2PhysSectorEx(DWORD dwHandle,DWORD dwLogicSectorNo,
				       __int64	*pqwPhysSectorNo, int nSectors);
	BOOL	EXPORT LogicSector2PhysSectorFast(DWORD dwHandle, DWORD dwLogicalSec, PVOID pPhysSecArr, DWORD dwArrIndex, int nSectors);
	BOOL	EXPORT FileLocatorFlush(DWORD dwVol);
	BOOL	EXPORT	Free_FileLocater(VOID);
	BOOL	EXPORT	EnableDisable_FL(BOOL);
	BOOL	EXPORT GuardPartition(BYTE btDrive);
	BOOL	EXPORT UngardPartition(BYTE btDrive);
	BOOL	EXPORT MakeMirrorDrive(DRIVE_MAP Drive[]);
	BOOL	EXPORT DiscardMirrorDrive();
	BOOL	EXPORT	VolFlush(BYTE	btDrive );		//	1 based , A : --- 1 , C : --- 3
	PVOID	EXPORT OpenBuffer(DWORD dwCount);
	BOOL	EXPORT CloseBuffer(PVOID pBuf);
	BOOL	EXPORT CopyData(PVOID pTar, PVOID pSrc, DWORD nCount);
	BOOL	EXPORT EnableDisableFileLocator(BOOL bEnable);
	BOOL	EXPORT SetPhysSectors(PVOID pPhysSecArr, DWORD dwArrIndex, int nSectors, DWORD dwValue);
}

#define	VWIN32_DIOC_DOS_IOCTL	1

	
#endif