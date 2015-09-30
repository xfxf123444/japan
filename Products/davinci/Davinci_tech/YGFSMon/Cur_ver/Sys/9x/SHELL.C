/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright 1993-95  Microsoft Corporation.  All Rights Reserved.           *
*                                                                           *
****************************************************************************/

#define WANTVXDWRAPS
#include <basedef.h>
#include <vmm.h>
#include <debug.h>
#include <vwin32.h>
#include <winerror.h>
#include "..\..\export\YGFSMon.h"

typedef struct DIOCParams *LPDIOC;

#pragma VxD_LOCKED_CODE_SEG

DWORD	_stdcall YGFSMON_W32_DeviceIOControl(DWORD, DWORD, DWORD, LPDIOC);
DWORD	_stdcall YGFSMON_CleanUp(void);
DWORD	_stdcall YGFSMON_GetVersion(DWORD, DWORD,LPDIOC);
DWORD	_stdcall YGFSMON_RedirStart(DWORD, DWORD,LPDIOC);
DWORD	_stdcall YGFSMON_RedirStop(DWORD, DWORD,LPDIOC);
DWORD ( _stdcall *YGFSMON_W32_Proc[] )(DWORD, DWORD, LPDIOC) = {
		YGFSMON_GetVersion,
		YGFSMON_RedirStart,
		YGFSMON_RedirStop,
 	};

#define MAX_YGFSMON_W32_API (sizeof(YGFSMON_W32_Proc)/sizeof(DWORD))

/****************************************************************************
		  YGFSMON_W32_DeviceIOControl
****************************************************************************/
DWORD _stdcall YGFSMON_W32_DeviceIOControl(DWORD  dwService,
                                        DWORD  dwDDB,
                                        DWORD  hDevice,
                                        LPDIOC lpDIOCParms)
{
    DWORD dwRetVal = 0;

    // DIOC_OPEN is sent when VxD is loaded w/ CreateFile 
    //  (this happens just after SYS_DYNAMIC_INIT)
    if ( dwService == DIOC_OPEN )
    {
        // Must return 0 to tell WIN32 that this VxD supports DEVIOCTL
        dwRetVal = 0;
    }
    // DIOC_CLOSEHANDLE is sent when VxD is unloaded w/ CloseHandle
    //  (this happens just before SYS_DYNAMIC_EXIT)
    else if ( dwService == DIOC_CLOSEHANDLE )
    {
        // Dispatch to cleanup proc
	dwRetVal = YGFSMON_CleanUp();
    }
    else if ( dwService > MAX_YGFSMON_W32_API )
    {
        // Returning a positive value will cause the WIN32 DeviceIOControl
        // call to return FALSE, the error code can then be retrieved
        // via the WIN32 GetLastError
        dwRetVal = ERROR_NOT_SUPPORTED;
    }
    else
    {
        // CALL requested service
	dwRetVal = (YGFSMON_W32_Proc[dwService-1])(dwDDB, hDevice, lpDIOCParms);
    }
    return dwRetVal;
}

DWORD	_stdcall YGFSMON_GetVersion(DWORD dwDDB, DWORD hDevice,LPDIOC lpDIOCParms)
{

    *((ULONG *)lpDIOCParms->lpvOutBuffer) = YGFSMON_VERSION_MAJOR << 16 | YGFSMON_VERSION_MINOR;

    return (NO_ERROR);
}

DWORD	_stdcall YGFSMON_RedirStart(DWORD dwDDB, DWORD hDevice,LPDIOC lpDIOCParms)
{
	YGFSMonRedirStart();
    return (NO_ERROR);
}

DWORD	_stdcall YGFSMON_RedirStop(DWORD dwDDB, DWORD hDevice,LPDIOC lpDIOCParms)
{
	YGFSMonRedirStop();
    return (NO_ERROR);
}

DWORD _stdcall YGFSMON_CleanUp(void)
{
    return(VXD_SUCCESS);
}
