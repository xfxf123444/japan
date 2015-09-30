// INTER320.H -- Ring3 communicate with ring0 module header file
// Copyright (C) 1999 by YG Corp.
// All rights reserved
// Author:  ZL
// Date:    25-Nov-1999

#ifndef INTER320_H
#define INTER320_H

#define WANTVXDWRAPS
#include "iosdcls.h"
#include <vwin32.h>
#include "virtu_fs.h"

#define GET_VERSION  1

typedef struct DIOCParams   *LPDIOC;

BOOL	FreeFsData(VOID);
DWORD	_stdcall VIRTU_FS_W32_DeviceIOControl(DWORD,DWORD,DWORD,LPDIOC);
DWORD	_stdcall VIRTU_FS_CleanUp(void);
DWORD	_stdcall VIRTU_FS_Get_Version(DWORD,DWORD,LPDIOC);
DWORD	_stdcall VIRTU_FS_EnableOrDisable(DWORD,DWORD,LPDIOC);
DWORD	_stdcall VIRTU_FS_UpdateFSData(DWORD,DWORD,LPDIOC);
DWORD	_stdcall VIRTU_FS_FreeFsData(DWORD,DWORD,LPDIOC);
DWORD	_stdcall VIRTU_FS_DstNxtFSData(DWORD,DWORD,LPDIOC);
DWORD	_stdcall VIRTU_FS_WriteEMBR(DWORD  dwDDB,
				    DWORD  hDevice,
				    LPDIOC lpDIOCParms);
DWORD _stdcall VIRTU_FS_GetSectorGroup(DWORD  dwDDB,
				       DWORD  hDevice,
				       LPDIOC lpDIOCParms);
DWORD _stdcall VIRTU_FS_GetNumOfSysSectors(DWORD  dwDDB,
					   DWORD  hDevice,
					   LPDIOC lpDIOCParms);


#endif	//  INTER320_H
