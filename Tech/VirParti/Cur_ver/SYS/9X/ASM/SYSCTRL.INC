COMMENT     ^

SYSCTRL.INC
Copyright (C) 2000 by YG Corp.
All rights reserved
Author:  ZL
Date:	 24-Feb-2000

    ^

VIRPARTI_Device_ID	EQU 3FAFH



Begin_Service_Table VIRPARTI
VIRPARTI_Service    VIRPARTI_GetMonitorDCB,LOCAL
VIRPARTI_Service    VIRPARTI_GetSharkDCB,LOCAL
VIRPARTI_Service    VIRPARTI_SetMonitorDCB,LOCAL
VIRPARTI_Service    VIRPARTI_SetSharkDCB,LOCAL
End_Service_Table   VIRPARTI

EXTERNDEF   _GetCurTime:NEAR

IFDEF	DEBUG
;EXTRN	 _DebugQueryHandler:NEAR
ENDIF
