
COMMENT     ^

    FileLocH.ASM    MODULE

    AUTHOR:
	    ZHOU LONG	(ZL)

    Date:   22-Nov-1999

    ^

.386p
.xlist
include VMM.INC
include DEBUG.INC
include VXDLDR.INC
include VTD.INC
FileLocH = 1
Create_FileLocH_Service_Table	=   1
include FileLocH.INC
include ..\..\filelocl\9x\asm\filelocl.inc
.list

FileLocH_DYNAMIC      EQU 1


Declare_Virtual_Device FileLocH,FileLocH_MAJOR_VERSION,FileLocH_MINOR_VERSION,\
		       FileLocH_Control,FileLocH_Device_ID,Undefined_Init_Order


VxD_LOCKED_DATA_SEG



VxD_LOCKED_DATA_ENDS


VxD_LOCKED_CODE_SEG
BeginProc   FileLocH_Control
    Control_Dispatch	SYS_DYNAMIC_DEVICE_INIT,_OnSysDynamicDeviceInit,cCall
    Control_Dispatch	SYS_DYNAMIC_DEVICE_EXIT,_OnSysDynamicDeviceExit,cCall
    Control_Dispatch	W32_DEVICEIOCONTROL,HighLoc_W32_DeviceIoControl,\
			sCall,<ecx,ebx,edx,esi>
    clc
    ret
EndProc     FileLocH_Control




EXTERNDEF   _CanWeWork:NEAR
BeginProc   _CanWeWork
    VxDCall FILELOCL_GetVersion
    or	    eax,eax
    jz	    exit

    mov     eax,01h
exit:
    ret
EndProc     _CanWeWork


EXTERNDEF   _WorkingOrResting:NEAR
BeginProc   _WorkingOrResting
    push    DWORD PTR [esp+04h]
    VxDCall FILELOCL_EnableOrDisable
    add     esp,04h
    ret
EndProc     _WorkingOrResting

;   void GetCurTime(double *);
BeginProc   _GetCurTime
    VxDCall VTD_Get_Real_Time
    mov     ebx,[esp+04h]
    mov     [ebx],eax
    mov     [ebx+04h],edx
    ret
EndProc     _GetCurTime

VxD_LOCKED_CODE_ENDS

     END
