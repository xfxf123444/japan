/******************************************************************************
*	FileLoc.H   header file
*
*	AUTHOR	:   LHB
*
*	Copyright 2001 - Shanghai YuGuang Science & Technology Co. Ltd .
*	All rights reserved.
*
*	Date:	02-Dec-2001
*
******************************************************************************/

#ifndef  FILELOC_H
#define  FILELOC_H

#include "YGFloc.h"

#ifndef EXPORT
#define EXPORT	__declspec(dllexport)
#endif

#ifndef SECTOR_SIZE  
#define SECTOR_SIZE 512
#endif

//extern ENCRYPT_INFO encrypt_Info;

typedef	struct	tagDRIVE_MAP
{
	union
	{
		BYTE	original_drive;
		struct
		{
			BYTE     btHardDisk; 
			LONGLONG llParStart;
			LONGLONG llParLen;
			DWORD    dwHiddenSectors;
		};
	};
	BYTE	mirror_drive;
} DRIVE_MAP,*PDRIVE_MAP;	

extern	"C"
{
	BOOL    EXPORT	MakeMirrorDrive(DRIVE_MAP *pDrive);
	BOOL	EXPORT	MakeVirtualDrive(DRIVE_MAP *pDrive);
	BOOL	EXPORT	DiscardVirtualDrive(BYTE btDriveLetter);
	BOOL	EXPORT	Init_FileLocater(BYTE btDriveLetter);
	BOOL	EXPORT	Free_FileLocater(BYTE btDriveLetter);
	HANDLE  EXPORT   FL_Create_File(LPCSTR	lpszPathName,WIN32_FILE_ATTRIBUTE_DATA *pFileAttrib);
	HANDLE	EXPORT	Open_FL_Handle(LPCSTR	lpszPathName);
	BOOL	EXPORT Close_FL_Handle(HANDLE hFile);
	BOOL	EXPORT LogicSector2PhysSectorFast(HANDLE hFile, DWORD dwLogicalSec,DWORD dwArrIndex, DWORD dwSectors);
	BOOL	EXPORT FL_LogicSector2PhysSector(HANDLE hFile,LONGLONG *pdAddrArr,DWORD dwLogicalSec,DWORD dwSectors);
	BOOL	EXPORT FileLocatorFlush(BYTE btDriveLetter);
	BOOL	EXPORT CopyData(BYTE btDriveLetter,LONGLONG *pTar, DWORD dwIndexStart, DWORD dwCount);
	DWORD   EXPORT FL_ReadFile(HANDLE hFile,DWORD dwStart,LPBYTE pBuf,DWORD dwReadLen);
	DWORD   EXPORT FL_WriteFile(HANDLE hFile,DWORD dwStart,LPBYTE pBuf,DWORD dwWriteLen);
	DWORD   EXPORT FL_GetFileSize(HANDLE hFile,DWORD *pdFileSizeHigh);
	BOOL	EXPORT	MakeCryDisk(DRIVE_MAP *pDrive,PBYTE key,UINT keyLen,UINT EncryptType);
	BYTE    EXPORT  GetMPDriveInfo(int nDrive);
	int     EXPORT  GetCount();
	int     EXPORT  AddCount();
}

#endif
	
