;   SYSCTRL.ASM
;   Copyright (C) 1999 by YG Corp.
;   All rights reserved
;   Author: ZL
;   Date:   25-Nov-1999

.386p
.xlist
include vmm.inc
include drp.inc
include ilb.inc
VIRTU_FS    =	1
Create_VIRTU_FS_Service_Table	=   1
include ..\..\Export\9x\vfs.INC
include SYSCTRL.INC
.list

extrn _OnAsyncRequest:near

VxD_LOCKED_DATA_SEG
    public _theDRP,_theILB
;   Declare ourselves as a level 8 driver (VSD)

_theDRP  DRP   <EyeCatcher, DRP_VSD_2, offset32 _OnAsyncRequest, \
	       offset32 _theILB, 'Virt File System', 0, 0, 0>

_theILB  ILB   <>	; I/O subsystem linkage block
VxD_LOCKED_DATA_ENDS

Declare_Virtual_Device VIRTU_FS, VIRTU_FS_Major_Ver, VIRTU_FS_Minor_Ver, VIRTU_FS_control,\
		       VIRTU_FS_Device_ID, Undefined_Init_Order,,,_theDRP

VxD_LOCKED_CODE_SEG

Begin_Control_Dispatch VIRTU_FS
Control_Dispatch Sys_Dynamic_Device_Init, _OnSysDynamicDeviceInit, cCall
Control_Dispatch Sys_Dynamic_Device_Exit, _OnSysDynamicDeviceExit, cCall
Control_Dispatch W32_DeviceIoControl,	  VIRTU_FS_W32_DeviceIOControl, \
					  sCall, <ecx,ebx,edx,esi>
IFDEF	DEBUG
Control_Dispatch Debug_Query,_DebugQueryHandler,cCall
ENDIF

End_Control_Dispatch   VIRTU_FS

BeginProc   _Reset_Direction_Flag
    cld
    ret
EndProc     _Reset_Direction_Flag


BeginProc   VIRTU_FS_GetVersion
    mov     eax,(VIRTU_FS_Major_Ver SHL 16) OR VIRTU_FS_Minor_Ver
    ret
EndProc     VIRTU_FS_GetVersion


EXTRN	_GetFstNxtFsData:NEAR
BeginProc   VIRTU_FS_GetFstNxtFsData
    jmp     NEAR PTR _GetFstNxtFsData
EndProc     VIRTU_FS_GetFstNxtFsData


EXTRN	_GetSectorGroup:NEAR
BeginProc   VIRTU_FS_GetSectorGroup
    jmp     NEAR PTR _GetSectorGroup
EndProc     VIRTU_FS_GetSectorGroup

IFDEF	DEBUG

BeginProc   _GetDebugChar
    VMMjmp  In_Debug_Chr
EndProc     _GetDebugChar

ENDIF

VxD_LOCKED_CODE_ENDS

	 end
