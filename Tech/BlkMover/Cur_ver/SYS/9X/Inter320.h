// INTER320.H -- Ring3 communicate with ring0 module header file
// Copyright (C) 1999 by YG Corp.
// All rights reserved
// Author:  ZL
// Date:    25-Nov-1999


#define BLKMOVER_GET_VERSION	    1
#define BLKMOVER_SET_MOVER_PARA     2
#define BLKMOVER_MOVE_BLOCK			3
#define BLKMOVER_ENABLE_OR_DISABLE  4

typedef struct DIOCParams   *LPDIOC;

DWORD	_stdcall BLKMOVER_W32_DeviceIOControl(DWORD,DWORD,DWORD,LPDIOC);
DWORD	_stdcall BLKMOVER_CleanUp(VOID);
DWORD	_stdcall BLKMOVER_Get_Version(DWORD,DWORD,LPDIOC);
DWORD	_stdcall BLKMOVER_SetMoverPara(DWORD,DWORD,LPDIOC);
DWORD	_stdcall BLKMOVER_MoveBlock(DWORD,DWORD,LPDIOC);
DWORD	_stdcall BLKMOVER_EnableOrDisable(DWORD,DWORD,LPDIOC);
DWORD	_stdcall BLKMOVER_Preface(DWORD,DWORD,LPDIOC);
DWORD	_stdcall BLKMOVER_PostScript(DWORD,DWORD,LPDIOC);
DWORD	_stdcall BLKMOVER_RM_Init(DWORD,DWORD,LPDIOC);
DWORD	_stdcall BLKMOVER_RM_SetGroupTable(DWORD,DWORD,LPDIOC);
DWORD	_stdcall BLKMOVER_RM_EanbleOrDisRelocate(DWORD,DWORD,LPDIOC);
DWORD	_stdcall BLKMOVER_RM_WriteSectors(DWORD,DWORD,LPDIOC);
DWORD   _stdcall BLKMOVER_RM_MoveGroup(DWORD,DWORD,LPDIOC);
DWORD   _stdcall BLKMOVER_PM_ReadSectors(DWORD,DWORD,LPDIOC);
DWORD   _stdcall BLKMOVER_PM_WriteSectors(DWORD,DWORD,LPDIOC);
DWORD   _stdcall BLKMOVER_MovingCurrentGroup(DWORD,DWORD,LPDIOC);
DWORD	_stdcall BLKMOVER_EnableWritePotect(DWORD,DWORD,LPDIOC);



