;   VFS.INC
;   Copyright (C) 2000 by YG Corp.
;   All rights reserved
;   Author: Liren
;   Date:   18-Oct-2000

VIRTU_FS_Device_ID  =	3FB4H
VIRTU_FS_Major_Ver  =	1
VIRTU_FS_Minor_Ver  =	0


EXTERNDEF   VIRTU_FS_GetFstNxtFsData:NEAR
EXTERNDEF   VIRTU_FS_GetVersion:NEAR
EXTERNDEF   VIRTU_FS_VirtualWriteSector:NEAR

Begin_Service_Table	VIRTU_FS
    VIRTU_FS_Service	    VIRTU_FS_GetVersion,LOCAL
    VIRTU_FS_Service	    VIRTU_FS_GetFstNxtFsData,LOCAL
    VIRTU_FS_Service	    VIRTU_FS_GetSectorGroup,LOCAL
End_Service_Table	VIRTU_FS
