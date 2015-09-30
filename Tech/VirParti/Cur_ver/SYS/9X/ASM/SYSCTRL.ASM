COMMENT     ^


SYSCTRL.ASM
Copyright (C) 2000 by YG Corp.
All rights reserved
Author:  ZL
Date:	 24-Feb-2000

    ^

.386p
include vmm.inc
include vtd.inc
include drp.inc
include ilb.inc
include debug.inc
VIRPARTI = 1
Create_VIRPARTI_Service_Table	=   1
include SYSCTRL.INC

extrn _OnAsyncRequest:near

VxD_LOCKED_DATA_SEG
    public _theDRP,_theILB
;   Declare ourselves as a level 10 driver (VSD)

_theDRP  DRP   <EyeCatcher, DRP_VSD_2, offset32 _OnAsyncRequest, \
	       offset32 _theILB, 'Virtual Part    ', 0, 0, 0>

_theILB  ILB   <>	; I/O subsystem linkage block
VxD_LOCKED_DATA_ENDS

Declare_Virtual_Device VIRPARTI, 4, 4, VIRPARTI_control,\
		       VIRPARTI_Device_ID, Undefined_Init_Order,,,_theDRP

VxD_LOCKED_CODE_SEG

Begin_Control_Dispatch VIRPARTI
Control_Dispatch Sys_Dynamic_Device_Init, _OnSysDynamicDeviceInit, cCall
Control_Dispatch Sys_Dynamic_Device_Exit, _OnSysDynamicDeviceExit, cCall
Control_Dispatch W32_DeviceIoControl,	  VIRPARTI_W32_DeviceIOControl, \
					  sCall, <ecx,ebx,edx,esi>
IFDEF	DEBUG
Control_Dispatch Debug_Query,	_DebugQueryHandler, cCall
ENDIF
End_Control_Dispatch   VIRPARTI


EXTRN	_GetMonitorDCB:NEAR
BeginProc   VIRPARTI_GetMonitorDCB
    JMP     _GetMonitorDCB
EndProc     VIRPARTI_GetMonitorDCB

EXTRN	_GetSharkDCB:NEAR
BeginProc   VIRPARTI_GetSharkDCB
    JMP     _GetSharkDCB
EndProc     VIRPARTI_GetSharkDCB


EXTRN	_SetMonitorDCB:NEAR
BeginProc   VIRPARTI_SetMonitorDCB
    JMP     _SetMonitorDCB
EndProc     VIRPARTI_SetMonitorDCB

EXTRN	_SetSharkDCB:NEAR
BeginProc   VIRPARTI_SetSharkDCB
    JMP     _SetSharkDCB
EndProc     VIRPARTI_SetSharkDCB


IFDEF	DEBUG
BeginProc   _GetDebugChar
    VMMcall In_Debug_Chr
    ret
EndProc     _GetDebugChar
ENDIF

;   void GetCurTime(double *);
BeginProc   _GetCurTime
    VxDCall VTD_Get_Real_Time
    mov     ebx,[esp+04h]
    mov     [ebx],eax
    mov     [ebx+04h],edx
    ret
EndProc     _GetCurTime

VxD_LOCKED_CODE_ENDS

	 end
