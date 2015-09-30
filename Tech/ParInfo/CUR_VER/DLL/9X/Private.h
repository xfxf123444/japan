#ifndef	PRIVATE_H
#define	PRIVATE_H


#ifndef	DRIVE_MAP_INFO_H
#define	DRIVE_MAP_INFO_H


typedef struct	tagSimulateDriveMapInfo
{
	BYTE	DriveNum;	//	80H , 81H
	BYTE	ParType;	//	04H , 06H 
	WORD	wReserve;	
	DWORD	dwStart;	//	linear sector number
} SIMULATE_DRIVEMAPINFO,*PSIMULATE_DRIVEMAPINFO;	


#endif

#endif	//	PRIVATE_H