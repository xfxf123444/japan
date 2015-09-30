;   FileLocL.ASM (VSD)
;   Copyright (C) 1999 by YG Corp.
;   All rights reserved
;   Author: ZL
;   Date:   22-Nov-1999

.386p
.xlist
include vmm.inc
include drp.inc
include ilb.inc
include debug.inc
Create_FILELOCL_Service_Table	=   1
include FileLocL.inc
.list

extrn _OnAsyncRequest:near

VxD_LOCKED_DATA_SEG
    public _theDRP

;   Declare ourselves as a level 8 driver --- VSD

_theDRP  DRP   <EyeCatcher, DRP_VSD_1, offset32 _OnAsyncRequest, \
	       offset32 _theILB, 'File Locater Low', 0, 0, 0>

    public _theILB
_theILB  ILB   <>			       ; I/O subsystem linkage block
VxD_LOCKED_DATA_ENDS


Declare_Virtual_Device FileLocL, 4, 4, FileLocL_control,\
		       FILELOCL_DEVICE_ID, Undefined_Init_Order,,,_theDRP


VxD_LOCKED_CODE_SEG

Begin_Control_Dispatch FileLocL
Control_Dispatch Sys_Dynamic_Device_Init, _OnSysDynamicDeviceInit, cCall
Control_Dispatch Sys_Dynamic_Device_Exit, _OnSysDynamicDeviceExit, cCall
;Control_Dispatch W32_DeviceIoControl,	   FileLocL_W32_DeviceIoControl, \
;					   sCall, <ecx,ebx,edx,esi>

IFDEF	DEBUG

Control_Dispatch    Debug_Query,_DebugQueryHandler,cCall

ENDIF
End_Control_Dispatch   FileLocL


BeginProc   FILELOCL_GetVersion
    mov     eax,(FILELOCL_Major_Ver SHL 16) OR FILELOCL_Minor_Ver
    ret
EndProc     FILELOCL_GetVersion

EXTRN	_bWorking:DWORD
BeginProc   FILELOCL_EnableOrDisable
    mov     eax,DWORD PTR [esp+04h]
    mov     DWORD PTR [_bWorking],eax
    ret
EndProc     FILELOCL_EnableOrDisable


IFDEF	DEBUG
BeginProc   _GetDebugChar
    VMMcall In_Debug_Chr
    ret
EndProc     _GetDebugChar
ENDIF

VxD_LOCKED_CODE_ENDS

	 end
