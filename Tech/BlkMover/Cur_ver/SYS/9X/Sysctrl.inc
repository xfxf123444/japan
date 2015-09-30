;   SYSCTRL.INC
;   Copyright (C) 1999 by YG Corp.
;   All rights reserved
;   Author: ZL
;   Date:   25-Nov-1999

EXTERNDEF   _GetPhysMemSize:NEAR
EXTERNDEF   _Get_Cur_Thread_Priority:NEAR
EXTERNDEF   _Set_Cur_Thread_Priority:NEAR
EXTERNDEF	_GetSysTime:NEAR
EXTERNDEF	_ThreadSleep:NEAR
IFDEF	DEBUG
EXTERNDEF   _GetDebugChar:NEAR
EXTERNDEF   _GetCurTime:NEAR
EXTERNDEF   _Reboot:NEAR
ENDIF
