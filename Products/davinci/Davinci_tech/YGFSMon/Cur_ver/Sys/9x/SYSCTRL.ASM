
    .386p
    include vmm.inc
    include VXDLdr.Inc

EXTERNDEF	_YGRegMonVMMRegOpenKey:NEAR
EXTERNDEF	_YGRegMonVMMRegCloseKey:NEAR
EXTERNDEF       _YGRegMonVMMRegCreateKey:NEAR
EXTERNDEF       _YGRegMonVMMRegDeleteKey:NEAR
EXTERNDEF       _YGRegMonVMMRegEnumKey:NEAR
EXTERNDEF       _YGRegMonVMMRegQueryValue:NEAR
EXTERNDEF       _YGRegMonVMMRegSetValue:NEAR
EXTERNDEF       _YGRegMonVMMRegDeleteValue:NEAR
EXTERNDEF       _YGRegMonVMMRegEnumValue:NEAR
EXTERNDEF       _YGRegMonVMMRegQueryValueEx:NEAR
EXTERNDEF       _YGRegMonVMMRegSetValueEx:NEAR
EXTERNDEF       _YGRegMonVMMRegFlushKey:NEAR
EXTERNDEF       _YGRegMonVMMRegRemapPreDefKey:NEAR
EXTERNDEF       _YGRegMonVMMRegQueryMultipleValues:NEAR
EXTERNDEF       _YGRegMonVMMRegCreateDynKey:NEAR
EXTERNDEF       _YGRegMonVMMRegQueryInfoKey:NEAR	

    Create_YGFSMON_Service_Table   EQU 1

Declare_Virtual_Device YGFSMON, 1, 0, YGFSMON_Control, \
	 Undefined_Device_ID, VXDLDR_INIT_ORDER

;Global  Variable
VxD_LOCKED_DATA_SEG
EXTERNDEF _OldVMMRegOpenKey:DWORD
EXTERNDEF _OldVMMRegCloseKey:DWORD
EXTERNDEF _OldVMMRegFlushKey:DWORD
EXTERNDEF _OldVMMRegCreateKey:DWORD
EXTERNDEF _OldVMMRegDeleteKey:DWORD
EXTERNDEF _OldVMMRegEnumKey:DWORD
EXTERNDEF _OldVMMRegQueryValue:DWORD
EXTERNDEF _OldVMMRegSetValue:DWORD
EXTERNDEF _OldVMMRegDeleteValue:DWORD
EXTERNDEF _OldVMMRegEnumValue:DWORD
EXTERNDEF _OldVMMRegQueryValueEx:DWORD
EXTERNDEF _OldVMMRegSetValueEx:DWORD
EXTERNDEF _OldVMMRegRemapPreDefKey:DWORD
EXTERNDEF _OldVMMRegQueryMultipleValues:DWORD
EXTERNDEF _OldVMMRegCreateDynKey:DWORD
EXTERNDEF _OldVMMRegQueryInfoKey:DWORD

VxD_LOCKED_DATA_ENDS

VxD_LOCKED_CODE_SEG

BeginProc  YGFSMON_Control

    Control_Dispatch  Sys_Critical_Init,_OnCriticalInit,cCall
    Control_Dispatch  Sys_Critical_Exit,_OnCriticalExit,cCall
    Control_Dispatch Device_Init, _OnDeviceInit, cCall
    Control_Dispatch Device_Reboot_Notify, _OnDeviceReboot, cCall

    Control_Dispatch W32_DeviceIoControl,     YGFSMON_W32_DeviceIOControl,\
    					      sCall, <ecx, ebx, edx, esi>
    clc
    ret
EndProc YGFSMON_Control

BeginProc _HookRegServices

        push  ebp
        mov   ebp, esp

	VxDcall	VXDLDR_GetVersion
	jc   		Fail_Init_Phase

	GetVxDServiceOrdinal eax,_RegOpenKey
	mov             esi,OFFSET32 VMMRegOpenKeyHandler
	VMMCall         Hook_Device_Service
	jc  		Fail_Init_Phase

	GetVxDServiceOrdinal eax,_RegCloseKey
	mov             esi,OFFSET32 VMMRegCloseKeyHandler
	VMMCall         Hook_Device_Service
	jc  		Fail_Init_Phase

	GetVxDServiceOrdinal eax,_RegCreateKey
	mov             esi,OFFSET32 VMMRegCreateKeyHandler
	VMMCall         Hook_Device_Service
	jc  		Fail_Init_Phase

	GetVxDServiceOrdinal eax,_RegDeleteKey
	mov             esi,OFFSET32 VMMRegDeleteKeyHandler
	VMMCall         Hook_Device_Service
	jc  		Fail_Init_Phase

	GetVxDServiceOrdinal eax,_RegEnumKey
	mov             esi,OFFSET32 VMMRegEnumKeyHandler
	VMMCall         Hook_Device_Service
	jc  		Fail_Init_Phase

	GetVxDServiceOrdinal eax,_RegQueryValue
	mov             esi,OFFSET32 VMMRegQueryValueHandler
	VMMCall         Hook_Device_Service
	jc  		Fail_Init_Phase

	GetVxDServiceOrdinal eax,_RegSetValue
	mov             esi,OFFSET32 VMMRegSetValueHandler
	VMMCall         Hook_Device_Service
	jc  		Fail_Init_Phase

	GetVxDServiceOrdinal eax,_RegDeleteValue
	mov             esi,OFFSET32 VMMRegDeleteValueHandler
	VMMCall         Hook_Device_Service
	jc  		Fail_Init_Phase

	GetVxDServiceOrdinal eax,_RegEnumValue
	mov             esi,OFFSET32 VMMRegEnumValueHandler
	VMMCall         Hook_Device_Service
	jc  		Fail_Init_Phase
	
	GetVxDServiceOrdinal eax,_RegQueryValueEx
	mov             esi,OFFSET32 VMMRegQueryValueExHandler
	VMMCall         Hook_Device_Service
	jc  		Fail_Init_Phase

	GetVxDServiceOrdinal eax,_RegSetValueEx
	mov             esi,OFFSET32 VMMRegSetValueExHandler
	VMMCall         Hook_Device_Service
	jc  		Fail_Init_Phase

	GetVxDServiceOrdinal eax,_RegFlushKey
	mov             esi,OFFSET32 VMMRegFlushKeyHandler
	VMMCall         Hook_Device_Service
	jc  		Fail_Init_Phase

	GetVxDServiceOrdinal eax,_RegRemapPreDefKey
	mov             esi,OFFSET32 VMMRegRemapPreDefKeyHandler
	VMMCall         Hook_Device_Service
	jc  		Fail_Init_Phase

	GetVxDServiceOrdinal eax,_RegQueryMultipleValues
	mov             esi,OFFSET32 VMMRegQueryMultipleValuesHandler
	VMMCall         Hook_Device_Service
	jc  		Fail_Init_Phase

	GetVxDServiceOrdinal eax,_RegCreateDynKey
	mov             esi,OFFSET32 VMMRegCreateDynKeyHandler
	VMMCall         Hook_Device_Service
	jc  		Fail_Init_Phase

	GetVxDServiceOrdinal eax,_RegQueryInfoKey
	mov             esi,OFFSET32 VMMRegQueryInfoKeyHandler
	VMMCall         Hook_Device_Service
	jc  		Fail_Init_Phase

Succeed_Init_Phase:
	mov             eax,1
	mov   esp, ebp
        pop   ebp
        ret
Fail_Init_Phase:
        mov             eax,0
        mov   esp, ebp
        pop   ebp
        ret 
EndProc _HookRegServices

BeginProc _UnHookRegServices

     push  ebp
     mov   ebp, esp

	GetVxDServiceOrdinal eax,_RegOpenKey
	mov             esi,OFFSET32 VMMRegOpenKeyHandler
	VMMCall         UnHook_Device_Service

	GetVxDServiceOrdinal eax,_RegCloseKey
	mov             esi,OFFSET32 VMMRegCloseKeyHandler
	VMMCall         UnHook_Device_Service

	GetVxDServiceOrdinal eax,_RegCreateKey
	mov             esi,OFFSET32 VMMRegCreateKeyHandler
	VMMCall         UnHook_Device_Service

	GetVxDServiceOrdinal eax,_RegDeleteKey
	mov             esi,OFFSET32 VMMRegDeleteKeyHandler
	VMMCall         UnHook_Device_Service

	GetVxDServiceOrdinal eax,_RegEnumKey
	mov             esi,OFFSET32 VMMRegEnumKeyHandler
	VMMCall         UnHook_Device_Service

	GetVxDServiceOrdinal eax,_RegQueryValue
	mov             esi,OFFSET32 VMMRegQueryValueHandler
	VMMCall         UnHook_Device_Service

	GetVxDServiceOrdinal eax,_RegSetValue
	mov             esi,OFFSET32 VMMRegSetValueHandler
	VMMCall         UnHook_Device_Service

	GetVxDServiceOrdinal eax,_RegDeleteValue
	mov             esi,OFFSET32 VMMRegDeleteValueHandler
	VMMCall         UnHook_Device_Service

	GetVxDServiceOrdinal eax,_RegEnumValue
	mov             esi,OFFSET32 VMMRegEnumValueHandler
	VMMCall         UnHook_Device_Service

	GetVxDServiceOrdinal eax,_RegQueryValueEx
	mov             esi,OFFSET32 VMMRegQueryValueExHandler
	VMMCall         UnHook_Device_Service

	GetVxDServiceOrdinal eax,_RegSetValueEx
	mov             esi,OFFSET32 VMMRegSetValueExHandler
	VMMCall         UnHook_Device_Service
	
	GetVxDServiceOrdinal eax,_RegFlushKey
	mov             esi,OFFSET32 VMMRegFlushKeyHandler
	VMMCall         UnHook_Device_Service

	GetVxDServiceOrdinal eax,_RegRemapPreDefKey
	mov             esi,OFFSET32 VMMRegRemapPreDefKeyHandler
	VMMCall         UnHook_Device_Service

	GetVxDServiceOrdinal eax,_RegQueryMultipleValues
	mov             esi,OFFSET32 VMMRegQueryMultipleValuesHandler
	VMMCall         UnHook_Device_Service

	GetVxDServiceOrdinal eax,_RegCreateDynKey
	mov             esi,OFFSET32 VMMRegCreateDynKeyHandler
	VMMCall         UnHook_Device_Service

	GetVxDServiceOrdinal eax,_RegQueryInfoKey
	mov             esi,OFFSET32 VMMRegQueryInfoKeyHandler
	VMMCall         UnHook_Device_Service

	mov             eax,1

     mov   esp, ebp
     pop   ebp
     ret

EndProc _UnHookRegServices

BeginProc VMMRegOpenKeyHandler, Hook_Proc, _OldVMMRegOpenKey
     push  ebp
     mov   ebp, esp
	
     push  [ebp+10h]
     push  [ebp+0ch]
     push  [ebp+08h]
     Call  _YGRegMonVMMRegOpenKey
     add   esp,0ch	

     mov   esp, ebp
     pop   ebp
     ret
EndProc VMMRegOpenKeyHandler

BeginProc VMMRegCloseKeyHandler, Hook_Proc, _OldVMMRegCloseKey
     push  ebp
     mov   ebp, esp

     push  [ebp+08h]
     Call  _YGRegMonVMMregCloseKey
     add   esp,	04h

     mov   esp, ebp
     pop   ebp
     ret
EndProc VMMRegCloseKeyHandler

BeginProc VMMRegCreateKeyHandler, Hook_Proc, _OldVMMRegCreateKey
     push  ebp
     mov   ebp, esp

     push  [ebp+10h]
     push  [ebp+0ch]
     push  [ebp+08h]
     Call  _YGRegMonVMMregCreateKey
     add   esp,	0ch

     mov   esp, ebp
     pop   ebp
     ret
EndProc VMMRegCreateKeyHandler

BeginProc VMMRegDeleteKeyHandler, Hook_Proc, _OldVMMRegDeleteKey
     push  ebp
     mov   ebp, esp

     push  [ebp+0ch]
     push  [ebp+08h]
     Call  _YGRegMonVMMRegDeleteKey
     add   esp,	08h

     mov   esp, ebp
     pop   ebp
     ret
EndProc VMMRegDeleteKeyHandler

BeginProc VMMRegEnumKeyHandler, Hook_Proc, _OldVMMRegEnumKey
     push  ebp
     mov   ebp, esp

     push  [ebp+14h]
     push  [ebp+10h]
     push  [ebp+0ch]
     push  [ebp+08h]
     Call  _YGRegMonVMMRegEnumKey
     add   esp,	10h

     mov   esp, ebp
     pop   ebp
     ret
EndProc VMMRegEnumKeyHandler

BeginProc VMMRegQueryValueHandler, Hook_Proc, _OldVMMRegQueryValue
     push  ebp
     mov   ebp, esp

     push  [ebp+14h]
     push  [ebp+10h]
     push  [ebp+0ch]
     push  [ebp+08h]
     Call  _YGRegMonVMMRegQueryValue
     add   esp,	10h

     mov   esp, ebp
     pop   ebp
     ret
EndProc VMMRegQueryValueHandler

BeginProc VMMRegSetValueHandler, Hook_Proc, _OldVMMRegSetValue
     push  ebp
     mov   ebp, esp

     push  [ebp+18h]
     push  [ebp+14h]
     push  [ebp+10h]
     push  [ebp+0ch]
     push  [ebp+08h]
     Call  _YGRegMonVMMRegSetValue
     add   esp,	14h

     mov   esp, ebp
     pop   ebp
     ret
EndProc VMMRegSetValueHandler

BeginProc VMMRegDeleteValueHandler, Hook_Proc, _OldVMMRegDeleteValue
     push  ebp
     mov   ebp, esp

     push  [ebp+0ch]
     push  [ebp+08h]
     Call  _YGRegMonVMMRegDeleteValue
     add   esp,	08h

     mov   esp, ebp
     pop   ebp
     ret
EndProc VMMRegDeleteValueHandler

BeginProc VMMRegEnumValueHandler, Hook_Proc, _OldVMMRegEnumValue
     push  ebp
     mov   ebp, esp

     push  [ebp+24h]
     push  [ebp+20h]
     push  [ebp+1ch]
     push  [ebp+18h]
     push  [ebp+14h]
     push  [ebp+10h]
     push  [ebp+0ch]
     push  [ebp+08h]
     Call  _YGRegMonVMMRegEnumValue
     add   esp,	20h

     mov   esp, ebp
     pop   ebp
     ret
EndProc VMMRegEnumValueHandler

BeginProc VMMRegQueryValueExHandler, Hook_Proc, _OldVMMRegQueryValueEx
     push  ebp
     mov   ebp, esp
     push  ebx

     push  [ebp+1ch]
     push  [ebp+18h]
     push  [ebp+14h]
     push  [ebp+10h]
     push  [ebp+0ch]
     push  [ebp+08h]
     Call  _YGRegMonVMMRegQueryValueEx
     add   esp,	18h

     pop   ebx
     mov   esp, ebp
     pop   ebp
     ret
EndProc VMMRegQueryValueExHandler

BeginProc VMMRegSetValueExHandler, Hook_Proc, _OldVMMRegSetValueEx
     push  ebp
     mov   ebp, esp

     push  [ebp+1ch]
     push  [ebp+18h]
     push  [ebp+14h]
     push  [ebp+10h]
     push  [ebp+0ch]
     push  [ebp+08h]
     Call  _YGRegMonVMMRegSetValueEx
     add   esp,	18h

     mov   esp, ebp
     pop   ebp
     ret
EndProc VMMRegSetValueExHandler

BeginProc VMMRegFlushKeyHandler, Hook_Proc, _OldVMMRegFlushKey
     push  ebp
     mov   ebp, esp

     push  [ebp+08h]
     Call  _YGRegMonVMMRegFlushKey
     add   esp,	04h

     mov   esp, ebp
     pop   ebp
     ret
EndProc VMMRegFlushKeyHandler

BeginProc VMMRegRemapPreDefKeyHandler, Hook_Proc, _OldVMMRegRemapPreDefKey
     push  ebp
     mov   ebp, esp

     push  [ebp+0ch]
     push  [ebp+08h]
     Call  _YGRegMonVMMRegRemapPreDefKey
     add   esp,	08h

     mov   esp, ebp
     pop   ebp
     ret
EndProc VMMRegRemapPreDefKeyHandler

BeginProc VMMRegQueryMultipleValuesHandler, Hook_Proc, _OldVMMRegQueryMultipleValues
     push  ebp
     mov   ebp, esp

     push  [ebp+18h]
     push  [ebp+14h]
     push  [ebp+10h]
     push  [ebp+0ch]
     push  [ebp+08h]
     Call  _YGRegMonVMMRegQueryMultipleValues
     add   esp,	14h

     mov   esp, ebp
     pop   ebp
     ret
EndProc VMMRegQueryMultipleValuesHandler

BeginProc VMMRegCreateDynKeyHandler, Hook_Proc, _OldVMMRegCreateDynKey
     push  ebp
     mov   ebp, esp

     push  [ebp+1ch]
     push  [ebp+18h]
     push  [ebp+14h]
     push  [ebp+10h]
     push  [ebp+0ch]
     push  [ebp+08h]
     Call  _YGRegMonVMMRegCreateDynKey
     add   esp,	18h

     mov   esp, ebp
     pop   ebp
     ret
EndProc VMMRegCreateDynKeyHandler

BeginProc VMMRegQueryInfoKeyHandler, Hook_Proc, _OldVMMRegQueryInfoKey
     push  ebp
     mov   ebp, esp

     push  [ebp+34h]
     push  [ebp+30h]
     push  [ebp+2ch]
     push  [ebp+28h]
     push  [ebp+24h]
     push  [ebp+20h]
     push  [ebp+1ch]
     push  [ebp+18h]
     push  [ebp+14h]
     push  [ebp+10h]
     push  [ebp+0ch]
     push  [ebp+08h]
     Call  _YGRegMonVMMRegQueryInfoKey
     add   esp,	30h

     mov   esp, ebp
     pop   ebp
     ret
EndProc VMMRegQueryInfoKeyHandler

BeginProc _RemoveDirectory

    push    ebp
    mov ebp, esp

    mov ax,713Ah
    push    [ebp+08h]
    pop edx

    push    dword ptr 21h
    VMMCall Exec_VxD_Int
    jc	RD_Return
    xor eax,eax
RD_Return:
    movzx eax, ax
    mov esp, ebp
    pop ebp
    ret
EndProc   _RemoveDirectory

BeginProc _CreateDirectory

	push	ebp
	mov	ebp, esp

	mov	ax,7139h
	push	[ebp+08h]
	pop	edx

	push	dword ptr 21h
	VMMCall Exec_VxD_Int
	jc	CD_Return
	xor	eax,eax
CD_Return:
	movzx eax, ax
	mov	esp, ebp
	pop	ebp
        ret

EndProc   _CreateDirectory

VxD_LOCKED_CODE_ENDS
	 END
