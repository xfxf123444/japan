// INTER320.C -- Ring3 communicate with ring0 module
// Copyright (C) 1999 by YG Corp.
// All rights reserved
// Author:  ZL
// Date:    25-Nov-1999

//#define WANTVXDWRAPS
#include "iosdcls.h"	// VMM and IOS headers

//#include <basedef.h>
//#include <vmm.h>
#include <debug.h>
#include <vxdwraps.h>
#include <vwin32.h>
#include <winerror.h>
#pragma hdrstop

#include "share.h"
#include "inter320.h"
#include "blkmover.h"
//#include "..\..\..\..\lib\hashsrch\cur_ver\export\list.h"
//#include "..\..\export\blkmover.h"

#pragma VxD_LOCKED_DATA_SEG

extern DWORD				g_dwSysRecNum;
extern DWORD				g_dwDataRecNum;
extern DWORD				g_dwMovedRecNum;
//extern MOVING_GROUP			g_MovingGroup;

#pragma VxD_LOCKED_CODE_SEG

DWORD	(_stdcall *BLKMOVER_W32_Proc[])(DWORD,DWORD,LPDIOC)=
{
	BLKMOVER_Get_Version,
	BLKMOVER_SetMoverPara,
	BLKMOVER_MoveBlock,
	BLKMOVER_EnableOrDisable,
	BLKMOVER_Preface,
	BLKMOVER_PostScript,
	BLKMOVER_RM_Init,
	BLKMOVER_RM_SetGroupTable,
	BLKMOVER_RM_EanbleOrDisRelocate,
	BLKMOVER_RM_WriteSectors,
	BLKMOVER_RM_MoveGroup,
	BLKMOVER_PM_ReadSectors,
	BLKMOVER_PM_WriteSectors,
	BLKMOVER_MovingCurrentGroup,
	BLKMOVER_EnableWritePotect
};

#define MAX_BLKMOVER_W32_API  (sizeof(BLKMOVER_W32_Proc)/sizeof(DWORD))

DWORD	_stdcall BLKMOVER_W32_DeviceIOControl(DWORD dwService,
					      DWORD dwDDB,
					      DWORD hDevice,
					      LPDIOC lpDIOCParms)
{
    DWORD   dwRetVal=0;
    // DIOC_OPEN is sent when VxD is loaded w/ CreateFile
    //	(this happens just after SYS_DYNAMIC_INIT)
    if ( dwService == DIOC_OPEN )
    {
	//Out_Debug_String("BLKMOVER: WIN32 DEVIOCTL supported here!\n\r");
	// Must return 0 to tell WIN32 that this VxD supports DEVIOCTL
	dwRetVal = 0;
    }
    // DIOC_CLOSEHANDLE is sent when VxD is unloaded w/ CloseHandle
    //	(this happens just before SYS_DYNAMIC_EXIT)
    else if ( dwService == DIOC_CLOSEHANDLE )
    {
	// Dispatch to cleanup proc
	//Out_Debug_String("BLKMOVER is unloaded!\n\r");
	dwRetVal = BLKMOVER_CleanUp();
    }
    else if ( dwService > MAX_BLKMOVER_W32_API )
    {
	// Returning a positive value will cause the WIN32 DeviceIOControl
	// call to return FALSE, the error code can then be retrieved
	Out_Debug_String("BLKMOVER: The service is not supported!\n\r");
	ASSERT(FALSE);
	dwRetVal = ERROR_NOT_SUPPORTED;
    }
    else
    {
	// CALL requested service
	dwRetVal = (BLKMOVER_W32_Proc[dwService-1])(dwDDB, hDevice, lpDIOCParms);
    }
    return(dwRetVal);
}   //	BLKMOVER_W32_DeviceIOControl


DWORD _stdcall BLKMOVER_CleanUp(VOID)
{
    return(VXD_SUCCESS);
}   //	BLKMOVER_CleanUp


DWORD _stdcall BLKMOVER_Get_Version(DWORD  dwDDB,DWORD  hDevice,LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pRegs;

    pRegs = (DIOC_REGISTERS *)lpDIOCParms->lpvOutBuffer;
    pRegs->reg_EAX = BLKMOVER_Major << 16 | BLKMOVER_Minor;
    return  NO_ERROR;
}   //	BLKMOVER_Get_Version



DWORD _stdcall BLKMOVER_MoveBlock(DWORD  dwDDB,DWORD  hDevice,LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pRegs;

    pRegs = (DIOC_REGISTERS *)lpDIOCParms->lpvOutBuffer;
    return  NO_ERROR;
}   //	BLKMOVER_MoveBlock


DWORD _stdcall BLKMOVER_SetMoverPara(DWORD  dwDDB,DWORD  hDevice,LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pRegs;
    pRegs = (DIOC_REGISTERS *)lpDIOCParms->lpvInBuffer;
    ASSERT(pRegs->reg_EAX > 0x400000 && pRegs->reg_EAX < 0x80000000);
    pRegs->reg_EAX = SetMoverPara(pRegs->reg_EAX);
    return  NO_ERROR;
}   //	BLKMOVER_SetMoverPara


DWORD _stdcall BLKMOVER_EnableOrDisable(DWORD  dwDDB,DWORD  hDevice,LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pRegs;
    pRegs = (DIOC_REGISTERS *)lpDIOCParms->lpvInBuffer;
    ASSERT(pRegs->reg_EAX == TRUE || pRegs->reg_EAX == FALSE);
    //EnableOrDisable(pRegs->reg_EAX);
    return  NO_ERROR;
}   //	BLKMOVER_EnableOrDisable


DWORD _stdcall BLKMOVER_Preface(DWORD  dwDDB,DWORD  hDevice,LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pRegs;
    pRegs = (DIOC_REGISTERS *)lpDIOCParms->lpvOutBuffer;
    pRegs->reg_EAX = Preface();
    return  NO_ERROR;
}   //	BLKMOVER_Preface


DWORD _stdcall BLKMOVER_PostScript(DWORD  dwDDB,DWORD  hDevice,LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pRegs;
    pRegs = (DIOC_REGISTERS *)lpDIOCParms->lpvOutBuffer;
    pRegs->reg_EAX = PostScript((BOOL)pRegs->reg_EAX);
    return  NO_ERROR;
}   //	BLKMOVER_PostScript

DWORD _stdcall 	BLKMOVER_RM_Init(DWORD  dwDDB,DWORD  hDevice,LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pRegs;
    pRegs = (DIOC_REGISTERS *)lpDIOCParms->lpvInBuffer;
    pRegs->reg_EAX = RmInit(pRegs->reg_EAX,(BYTE)pRegs->reg_EBX);
    return  NO_ERROR;
}   //	BLKMOVER_RM_Init

DWORD _stdcall 	BLKMOVER_RM_SetGroupTable(DWORD  dwDDB,DWORD  hDevice,LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pRegs;
    pRegs = (DIOC_REGISTERS *)lpDIOCParms->lpvInBuffer;
    pRegs->reg_EAX = SetGroupTable(pRegs->reg_EAX,pRegs->reg_EBX);
    return  NO_ERROR;
}   //	BLKMOVER_RM_SetGroupTable

DWORD _stdcall 	BLKMOVER_RM_EanbleOrDisRelocate(DWORD  dwDDB,DWORD  hDevice,LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pRegs;
    pRegs = (DIOC_REGISTERS *)lpDIOCParms->lpvInBuffer;
    pRegs->reg_EAX = EnabelOrDisRelocate(pRegs->reg_EAX,pRegs->reg_EBX,pRegs->reg_ECX);
    return  NO_ERROR;
}   //	BLKMOVER_RM_EanbleOrDisRelocate

DWORD _stdcall 	BLKMOVER_RM_WriteSectors(DWORD  dwDDB,DWORD  hDevice,LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pRegs;
	DWORD			dwR0Addr;
	DWORD			LinPageNum, LinOffset, nPages;

    pRegs		= (DIOC_REGISTERS *)lpDIOCParms->lpvInBuffer;
	LinOffset	= pRegs->reg_ECX & 0xfff;
	LinPageNum  = pRegs->reg_ECX >> 12;
	nPages		= ((pRegs->reg_ECX + pRegs->reg_EBX*SECTOR_SIZE) >> 12) - LinPageNum + 1;
	dwR0Addr	= _LinPageLock(LinPageNum, nPages, PAGEMAPGLOBAL);

//	pRegs->reg_EAX = WriteSectors(pRegs->reg_EAX,pRegs->reg_EBX,(PVOID)pRegs->reg_ECX);
	if(dwR0Addr)
	{
		pRegs->reg_EAX = WriteSectors(pRegs->reg_EAX,pRegs->reg_EBX,(PVOID)(dwR0Addr+LinOffset));

		if(!_LinPageUnLock(dwR0Addr>>12, nPages, PAGEMAPGLOBAL))
			pRegs->reg_EAX = FALSE;
	}
	else
		pRegs->reg_EAX = FALSE;

    return  NO_ERROR;
}   //	BLKMOVER_RM_WriteSectors

DWORD _stdcall 	BLKMOVER_RM_MoveGroup(DWORD  dwDDB,DWORD  hDevice,LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pRegs;

    pRegs	= (DIOC_REGISTERS *)lpDIOCParms->lpvInBuffer;
	pRegs->reg_EAX = MoveGroup(pRegs->reg_EAX,pRegs->reg_EBX,pRegs->reg_ECX);

    return  NO_ERROR;
}   //	BLKMOVER_RM_OpenBuffer

DWORD _stdcall 	BLKMOVER_EnableWritePotect(DWORD  dwDDB,DWORD  hDevice,LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pRegs;

    pRegs	= (DIOC_REGISTERS *)lpDIOCParms->lpvInBuffer;
	pRegs->reg_EAX = EnableWriteProtect();

    return  NO_ERROR;
}   //	BLKMOVER_MovingCurrentGroup

DWORD _stdcall 	BLKMOVER_MovingCurrentGroup(DWORD  dwDDB,DWORD  hDevice,LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pRegs;

    pRegs	= (DIOC_REGISTERS *)lpDIOCParms->lpvInBuffer;
	pRegs->reg_EAX = SetMoingGroupCur(pRegs->reg_EAX,pRegs->reg_EBX,pRegs->reg_ECX,pRegs->reg_EDX,pRegs->reg_EDI);

    return  NO_ERROR;
}   //	BLKMOVER_MovingCurrentGroup

DWORD _stdcall 	BLKMOVER_PM_WriteSectors(DWORD  dwDDB,DWORD  hDevice,LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pRegs;
	DWORD			dwR0Addr;
	DWORD			LinPageNum, LinOffset, nPages;

    pRegs		= (DIOC_REGISTERS *)lpDIOCParms->lpvInBuffer;
	LinOffset	= pRegs->reg_ECX & 0xfff;
	LinPageNum  = pRegs->reg_ECX >> 12;
	nPages		= ((pRegs->reg_ECX + pRegs->reg_EBX*SECTOR_SIZE) >> 12) - LinPageNum + 1;
	dwR0Addr	= _LinPageLock(LinPageNum, nPages, PAGEMAPGLOBAL);

	if(dwR0Addr)
	{
		pRegs->reg_EAX = WriteSectors(pRegs->reg_EAX,pRegs->reg_EBX,(PVOID)(dwR0Addr+LinOffset));

		if(!_LinPageUnLock(dwR0Addr>>12, nPages, PAGEMAPGLOBAL))
			pRegs->reg_EAX = FALSE;

		//if(pRegs->reg_EDI && g_dwSysRecNum && pRegs->reg_EDX == g_dwSysRecNum)
		//{
		//	g_pMoverData->bWorking = TRUE;
		//}
		SetWriteResult(pRegs->reg_EDI,pRegs->reg_EAX,pRegs->reg_EBX);
	}
	else
		pRegs->reg_EAX = FALSE;

    return  NO_ERROR;
}   //	BLKMOVER_PM_WriteSectors

DWORD _stdcall 	BLKMOVER_PM_ReadSectors(DWORD  dwDDB,DWORD  hDevice,LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pRegs;
	DWORD			dwR0Addr;
	DWORD			LinPageNum, LinOffset, nPages;

    pRegs		= (DIOC_REGISTERS *)lpDIOCParms->lpvInBuffer;
	LinOffset	= pRegs->reg_ECX & 0xfff;
	LinPageNum  = pRegs->reg_ECX >> 12;
	nPages		= ((pRegs->reg_ECX + pRegs->reg_EBX*SECTOR_SIZE) >> 12) - LinPageNum + 1;
	dwR0Addr	= _LinPageLock(LinPageNum, nPages, PAGEMAPGLOBAL);

	if(dwR0Addr)
	{
		pRegs->reg_EAX = ReadSectors(pRegs->reg_EAX,pRegs->reg_EBX,(PVOID)(dwR0Addr+LinOffset));

		if(!_LinPageUnLock(dwR0Addr>>12, nPages, PAGEMAPGLOBAL))
			pRegs->reg_EAX = FALSE;
	}
	else
		pRegs->reg_EAX = FALSE;

    return  NO_ERROR;
}   //	BLKMOVER_PM_ReadSectors
