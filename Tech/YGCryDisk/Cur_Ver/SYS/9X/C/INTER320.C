// INTER320.C -- Ring3 communicate with ring0 module
// Copyright (C) 1999 by YG Corp.
// All rights reserved
// Author:  ZL
// Date:    25-Nov-1999

#include "inter320.h"
#include <debug.h>
#include <winerror.h>
#include "share.h"
#include "virtu_fs.h"

extern	  int	 g_nSysSectors;


#pragma VxD_PAGEABLE_CODE_SEG

DWORD	(_stdcall *VIRTU_FS_W32_Proc[])(DWORD,DWORD,LPDIOC)=
    {
	VIRTU_FS_Get_Version,
	VIRTU_FS_EnableOrDisable,
	VIRTU_FS_UpdateFSData,
	VIRTU_FS_FreeFsData,
	VIRTU_FS_DstNxtFSData,
	VIRTU_FS_WriteEMBR,
	VIRTU_FS_GetSectorGroup,
	VIRTU_FS_GetNumOfSysSectors
    };

#define MAX_VIRTU_FS_W32_API  (sizeof(VIRTU_FS_W32_Proc)/sizeof(DWORD))

DWORD	_stdcall VIRTU_FS_W32_DeviceIOControl(DWORD dwService,
					      DWORD dwDDB,
					      DWORD hDevice,
					      LPDIOC lpDIOCParms)
{
    DWORD   dwRetVal=0;
    // DIOC_OPEN is sent when VxD is loaded w/ CreateFile
    //	(this happens just after SYS_DYNAMIC_INIT)
    if ( dwService == DIOC_OPEN )
    {
	Out_Debug_String("VIRTU_FS: WIN32 DEVIOCTL supported here!\n\r");
	// Must return 0 to tell WIN32 that this VxD supports DEVIOCTL
	dwRetVal = 0;
    }
    // DIOC_CLOSEHANDLE is sent when VxD is unloaded w/ CloseHandle
    //	(this happens just before SYS_DYNAMIC_EXIT)
    else if ( dwService == DIOC_CLOSEHANDLE )
    {
	// Dispatch to cleanup proc
	dwRetVal = VIRTU_FS_CleanUp();
    }
    else if ( dwService > MAX_VIRTU_FS_W32_API )
    {
	// Returning a positive value will cause the WIN32 DeviceIOControl
	// call to return FALSE, the error code can then be retrieved
	// via the WIN32 GetLastError
	dwRetVal = ERROR_NOT_SUPPORTED;
    }
    else
    {
	// CALL requested service
	dwRetVal = (VIRTU_FS_W32_Proc[dwService-1])(dwDDB, hDevice, lpDIOCParms);
    }
    return(dwRetVal);
}   //	VIRTU_FS_W32_DeviceIOControl


DWORD _stdcall VIRTU_FS_CleanUp(void)
{
    return(VXD_SUCCESS);
}   //	VIRTU_FS_CleanUp


DWORD _stdcall VIRTU_FS_Get_Version(DWORD  dwDDB,
				    DWORD  hDevice,
				    LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pRegs;

    pRegs = (DIOC_REGISTERS *)lpDIOCParms->lpvOutBuffer;
    pRegs->reg_EAX = VIRTU_FS_Major << 16 | VIRTU_FS_Minor;
    return  NO_ERROR;
}   //	VIRTU_FS_Get_Version




DWORD _stdcall VIRTU_FS_EnableOrDisable(DWORD  dwDDB,
					DWORD  hDevice,
					LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pInRegs;
    pInRegs = (DIOC_REGISTERS *)lpDIOCParms->lpvOutBuffer;
    pInRegs->reg_EAX = (DWORD)EnableVFS(pInRegs->reg_EAX,
					pInRegs->reg_EBX,
					pInRegs->reg_ECX);
    return  NO_ERROR;
}   //	VIRTU_FS_EnableOrDisable


DWORD _stdcall VIRTU_FS_UpdateFSData(DWORD  dwDDB,
				     DWORD  hDevice,
				     LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pInRegs,*pOutRegs;

    pInRegs = (DIOC_REGISTERS *)lpDIOCParms->lpvInBuffer;
    pOutRegs = (DIOC_REGISTERS *)lpDIOCParms->lpvOutBuffer;

    pOutRegs->reg_EAX = (DWORD)FastUpdateFSData(pInRegs->reg_EAX,pInRegs->reg_EBX,pInRegs->reg_ECX);

    return  NO_ERROR;
}   //	VIRTU_FS_UpdateFSData


DWORD _stdcall VIRTU_FS_FreeFsData(DWORD  dwDDB,
				   DWORD  hDevice,
				   LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pOutRegs;

    pOutRegs = (DIOC_REGISTERS *)lpDIOCParms->lpvOutBuffer;
    pOutRegs->reg_EAX = (DWORD)FreeFsData();

    return  NO_ERROR;
}   //	VIRTU_FS_FreeFsData


DWORD _stdcall VIRTU_FS_DstNxtFSData(DWORD  dwDDB,
				     DWORD  hDevice,
				     LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pOutRegs;
    pOutRegs = (DIOC_REGISTERS *)lpDIOCParms->lpvOutBuffer;
    pOutRegs->reg_EAX = (DWORD)GetFstNxtFsData(pOutRegs->reg_EAX,
						pOutRegs->reg_EBX,
						pOutRegs->reg_EDX);
    return  NO_ERROR;
}   //	VIRTU_FS_DstNxtFSData

DWORD _stdcall VIRTU_FS_WriteEMBR(DWORD  dwDDB,
				  DWORD  hDevice,
				  LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pInRegs;
    pInRegs = (DIOC_REGISTERS *)lpDIOCParms->lpvOutBuffer;
    pInRegs->reg_EAX = (DWORD)WriteEMBR(pInRegs->reg_EAX,
					(PBYTE)pInRegs->reg_EBX);
    return  NO_ERROR;
}   //	VIRTU_FS_WriteEMBR

DWORD _stdcall VIRTU_FS_GetSectorGroup(DWORD  dwDDB,
				       DWORD  hDevice,
				       LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pInRegs;
    pInRegs = (DIOC_REGISTERS *)lpDIOCParms->lpvOutBuffer;

    pInRegs->reg_EAX = GetSectorGroup((PBYTE)pInRegs->reg_EBX,(PDWORD)pInRegs->reg_EDX,pInRegs->reg_EAX);
    return  NO_ERROR;
}   //	VIRTU_FS_GetSectorGroup

DWORD _stdcall VIRTU_FS_GetNumOfSysSectors(DWORD  dwDDB,
					   DWORD  hDevice,
					   LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pInRegs;
    pInRegs = (DIOC_REGISTERS *)lpDIOCParms->lpvOutBuffer;

    pInRegs->reg_EAX = (DWORD)g_nSysSectors;
    return  NO_ERROR;
}   //	VIRTU_FS_GetNumOfSysSectors
