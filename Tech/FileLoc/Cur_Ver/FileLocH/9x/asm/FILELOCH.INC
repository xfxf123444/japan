COMMENT     ^

    FileLocH.INC
    AUTHOR: ZHOU LONG (ZL)

	16-Aug-1999
    ^

FileLocH_Device_ID	    EQU     3FB3H
FileLocH_MAJOR_VERSION	    EQU     1
FileLocH_MINOR_VERSION	    EQU     0


EXTRN	_GetVersion:NEAR
EXTRN	_OpenHandle:NEAR
EXTRN	_CloseHandle:NEAR
EXTRN	_LogicSector2PhysSector:NEAR


Begin_Service_Table	FileLocH
FileLocH_Service	_GetVersion,LOCAL
FileLocH_Service	_OpenHandle,LOCAL
FileLocH_Service	_CloseHandle,LOCAL
FileLocH_Service	_LogicSector2PhysSector,LOCAL
End_Service_Table	FileLocH
