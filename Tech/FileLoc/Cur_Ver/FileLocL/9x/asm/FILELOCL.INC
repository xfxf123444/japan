;   FileLocL.INC -- Device declaration and control proc header file
;   Copyright (C) 1999 by YG Corp.
;   All rights reserved
;   Author: ZL
;   Date:   22-Nov-1999

FILELOCL_Major_Ver	=   4
FILELOCL_Minor_Ver	=   4


FILELOCL_DEVICE_ID	=   3FB1H

Begin_Service_Table	FILELOCL
    FILELOCL_Service	FILELOCL_GetVersion,LOCAL
    FILELOCL_Service	FILELOCL_EnableOrDisable,LOCAL
End_Service_Table	FILELOCL



IFDEF	DEBUG
EXTERNDEF   _GetDebugChar:NEAR
;pEXTRN       _DebugQueryHandler:NEAR
ENDIF
