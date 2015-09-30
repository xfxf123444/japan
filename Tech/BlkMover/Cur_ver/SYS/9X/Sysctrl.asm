;   SYSCTRL.ASM
;   Copyright (C) 1999 by YG Corp.
;   All rights reserved
;   Author: ZL
;   Date:   25-Nov-1999

.386p
include vmm.inc
include vtd.inc
include drp.inc
include ilb.inc
include debug.inc
include ..\..\..\..\VFS\cur_ver\Export\9x\vfs.inc
include SYSCTRL.INC

extrn _OnAsyncRequest:near

VxD_LOCKED_DATA_SEG
    public _theDRP,_theILB
;   Declare ourselves as a level 8 driver (VSD)

_theDRP  DRP   <EyeCatcher, DRP_VSD_3, offset32 _OnAsyncRequest, \
	       offset32 _theILB, 'Block Mover     ', 0, 0, 0>

_theILB  ILB   <>	; I/O subsystem linkage block
VxD_LOCKED_DATA_ENDS

Declare_Virtual_Device BLKMOVER, 4, 4, BLKMOVER_control,\
		       Undefined_Device_ID, Undefined_Init_Order,,,_theDRP

VxD_LOCKED_CODE_SEG

Begin_Control_Dispatch BLKMOVER
Control_Dispatch Sys_Dynamic_Device_Init, _OnSysDynamicDeviceInit, cCall
Control_Dispatch Sys_Dynamic_Device_Exit, _OnSysDynamicDeviceExit, cCall
Control_Dispatch W32_DeviceIoControl,	  BLKMOVER_W32_DeviceIOControl, \
					  sCall, <ecx,ebx,edx,esi>

IFDEF	DEBUG

;Control_Dispatch Debug_Query,	 _DebugQueryHandler, cCall

ENDIF

End_Control_Dispatch   BLKMOVER


BeginProc   _GetFstNxtFsData
    VxDJmp  VIRTU_FS_GetFstNxtFsData
EndProc     _GetFstNxtFsData

BeginProc   _GetSectorGroup
    VxDJmp  VIRTU_FS_GetSectorGroup
EndProc     _GetSectorGroup


BeginProc   _GetPhysMemSize
    push    ebp
    mov     ebp,esp
GPMS_DemandInfoStruc EQU DWORD PTR [ebp - SIZE DemandInfoStruc]
    sub     esp,SIZE DemandInfoStruc

    lea     eax,GPMS_DemandInfoStruc
    VMMcall _GetDemandPageInfo,<eax,0>
    lea     ebx,GPMS_DemandInfoStruc
    ;mov     eax,[ebx.DIPhys_Count]
    mov     eax,[ebx.DIUnlock_Count]
    shl     eax,12  ;	convert to bytes

    mov     esp,ebp
    pop     ebp
    ret
EndProc     _GetPhysMemSize

BeginProc   _Reboot
    mov     eax,Reboot_Processor
    VMMcall System_Control
    ret
EndProc     _Reboot

BeginProc _Get_Cur_Thread_Priority
    push    ebp
    mov     ebp,esp
GCTP_ThreadHandle   EQU [ebp+08h]
    push    ecx
    push    edx
    push    esi
    push    edi
    mov     edi, GCTP_ThreadHandle
    VMMCall Get_Thread_Win32_Pri
    pop     edi
    pop     esi
    pop     edx
    pop     ecx
    mov     esp,ebp
    pop     ebp
    ret
EndProc   _Get_Cur_Thread_Priority

BeginProc _Set_Cur_Thread_Priority
    push    ebp
    mov     ebp,esp
SCTP_ThreadHandle   EQU [ebp+08h]
SCTP_Pri	    EQU [ebp+0Ch]
    push    edi
    mov     edi, SCTP_ThreadHandle
    mov     eax, SCTP_Pri
    VMMCall Set_Thread_Win32_Pri
    mov     eax,00h
    jc	    SCTP_Exit
    mov     eax,01h
SCTP_Exit:
    pop     edi
    mov     esp,ebp
    pop     ebp
    ret
EndProc   _Set_Cur_Thread_Priority


BeginProc   _ThreadSleep
	mov		eax,01h
    VMMcall Time_Slice_Sleep
    ret
EndProc     _ThreadSleep

BeginProc   _GetSysTime
    VMMcall Get_System_Time
    mov     ebx,[esp+04h]
    mov     [ebx],eax
    ret
EndProc     _GetSysTime

IFDEF	DEBUG

BeginProc   _GetDebugChar
	 VMMcall In_Debug_Chr
    ret
EndProc     _GetDebugChar


;   void GetCurTime(double *);
BeginProc   _GetCurTime
    VxDCall VTD_Get_Real_Time
    mov     ebx,[esp+04h]
    mov     [ebx],eax
    mov     [ebx+04h],edx
    ret
EndProc     _GetCurTime

ENDIF

VxD_LOCKED_CODE_ENDS

	 end
