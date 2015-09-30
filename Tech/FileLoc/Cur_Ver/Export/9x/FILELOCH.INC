COMMENT     ^

    FileLocH.INC
    AUTHOR: ZHOU LONG (ZL)

	16-Aug-1999
    ^

FileLocH_Device_ID	    EQU     3FB3H
FileLocH_MAJOR_VERSION	    EQU     1
FileLocH_MINOR_VERSION	    EQU     0


Begin_Service_Table	FileLocH
FileLocH_Service	FileLocH_GetVersion,LOCAL
FileLocH_Service	FileLocH_OpenHandle,LOCAL
FileLocH_Service	FileLocH_CloseHandle,LOCAL
FileLocH_Service	FileLocH_LogicSector2PhysSector,LOCAL
End_Service_Table	FileLocH
