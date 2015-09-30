;   SYSCTRL.INC
;   Copyright (C) 1999 by YG Corp.
;   All rights reserved
;   Author: ZL
;   Date:   24-Nov-1999


;------------------------------------------------------------------------------
;Exported Functions & Variables
;------------------------------------------------------------------------------


    EXTERNDEF	_Reset_Direction_Flag:NEAR

IFDEF	DEBUG
    ;EXTRN	 _DebugQueryHandler:NEAR
    EXTERNDEF	_GetDebugChar:NEAR
ENDIF
