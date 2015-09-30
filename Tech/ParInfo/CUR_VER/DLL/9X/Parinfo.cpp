#include <windows.h>
#include <assert.h>
//#include <stdio.h>

#include	"..\..\Export\9x\ParInfo.H"
#include	"head.h"
#include    "..\..\..\..\fileloc\cur_ver\Export\9x\fileloc.h"
#include	"..\..\..\..\shark\cur_ver\dll\9x\CVirpar.h"
	
#define ASSERT	assert
#define OS_WINDOWS98  "Windows 98 [Version" //19
#define OS_WINDOWSME  "Windows Millennium [Version"//27
#define OS_WINDOWS95  "Windows 95"//10
#define OS_DOS		  "DOS Version"//11

HANDLE	hYGDev = INVALID_HANDLE_VALUE;
BOOL	bSupportExten = FALSE;

BOOL EXPORT Init_PartitionInfo(VOID)
{
	if(hYGDev == INVALID_HANDLE_VALUE)
	{
		TCHAR	chDir[MAX_PATH];
		TCHAR	chPath[MAX_PATH];
	
		//
		// set current directory.
		GetCurrentDirectory(MAX_PATH, chDir);
		GetModuleFileName(NULL, chPath, MAX_PATH);
		*strrchr(chPath, '\\') = 0;
		SetCurrentDirectory(chPath);
		
		hYGDev = CreateFile("\\\\.\\Yg_Disk.vxd",0,0,NULL,0,0,NULL);
		if(hYGDev == INVALID_HANDLE_VALUE)
		{//it's in the memory
			hYGDev = CreateFile("\\\\.\\Yg_Disk",0,0,NULL,0,0,NULL);
			if(hYGDev == INVALID_HANDLE_VALUE)
				return	FALSE;
		}
		
		//
		// set current directory back.
		SetCurrentDirectory(chDir);
	}	
	
	return TRUE;
}   //	Init_PartitionInfo

BOOL EXPORT Free_PartitionInfo(VOID)
{
    if(INVALID_HANDLE_VALUE != hYGDev)
	{
		CloseHandle(hYGDev);
		hYGDev = INVALID_HANDLE_VALUE;			
	}
    return  TRUE;
}   //	Free_PartitionInfo

BOOL IsSupportExt13(HANDLE hYGDev,BYTE btHardDrive)
{	
	DIOC_REGISTERS		reg;
	DWORD				cb;
	BOOL                blResult = FALSE;

	reg.reg_EAX = 0x4100;
	reg.reg_EBX = 0x55aa;
	reg.reg_EDX = btHardDrive;
	blResult = DeviceIoControl(hYGDev,YG_DISK_W32_INT13,&reg,sizeof(reg),&reg,sizeof(reg),&cb,0);
	//reg_Flags is Carry Falg(00h),AH -- major version (>=1) BX -- aa55h CX -- API subset support bitmap
	return blResult && !(reg.reg_Flags & 0x01) && ((reg.reg_EAX & 0x0000FF00) >= 0x01),
			((reg.reg_EBX & 0xFFFF) == 0xaa55) && (reg.reg_ECX & 0x01);
}

BOOL EXPORT IsValidEMBR(DWORD dwExtStart,PARTITION_SEC *pPartitionMBR,BIOS_DRIVE_PARAM	*pDriveParam)
{
	PARTITION_ENTRY PartiEntry[2];
	BOOL bResult = FALSE;
	DWORD dwMaxSectors;
	if (pPartitionMBR->Signature == 0xAA55)
	{
		memset(&PartiEntry,0,sizeof(PARTITION_ENTRY)*2);
		if (!memcmp(&pPartitionMBR->Partition[2],&PartiEntry,sizeof(PARTITION_ENTRY)*2))
		{
			dwMaxSectors = (DWORD)pDriveParam->dwSectors;
			if (memcmp(&pPartitionMBR->Partition,&PartiEntry,sizeof(PARTITION_ENTRY)))
			{
				if (pPartitionMBR->Partition[0].SystemFlag)
				{
					bResult = (pPartitionMBR->Partition[0].StartSector < dwMaxSectors) &&
						      (pPartitionMBR->Partition[0].SectorsInPartition < dwMaxSectors);// &&
//						  (dwExtStart + pPartitionMBR->Partition[0].StartSector + pPartitionMBR->Partition[0].SectorsInPartition <= dwMaxSectors);
				}
			}
			else bResult = TRUE;
			if (memcmp(&pPartitionMBR->Partition[1],&PartiEntry,sizeof(PARTITION_ENTRY)) && pPartitionMBR->Partition[1].SystemFlag)
			{
				if ((pPartitionMBR->Partition[1].StartSector > dwMaxSectors) ||
				   (pPartitionMBR->Partition[1].SectorsInPartition > dwMaxSectors))// ||
//   				   (dwExtStart + pPartitionMBR->Partition[1].StartSector + pPartitionMBR->Partition[1].SectorsInPartition > dwMaxSectors))
			    	bResult = FALSE;
			}
		}
	}
	return bResult;
}

BOOL EXPORT GetPartitionInfo(BYTE btHardDrive,PPARINFOONHARDDISK pParHard)
{
	int i;
	PARTITION_SEC		PartitionMBR;
	BIOS_DRIVE_PARAM	DriveParam;
	DWORD				dwTempSec,dwLogicStart;
	BOOL				blResult = FALSE;
	
	if(INVALID_HANDLE_VALUE==hYGDev)
	{	
		//MessageBox(NULL,"Get PartitionInfo handle error In GetPartitionInfo failed!","fail!",MB_OK);
		return FALSE;
	}
	bSupportExten = IsSupportExt13(hYGDev,btHardDrive);
	if(!GetDriveParam(btHardDrive,&DriveParam))
	{
		//MessageBox(NULL,"Get Drive Param In GetPartitionInfo failed!","fail!",MB_OK);
		return FALSE;
	}
	//Read the MBR
	if (ReadSector(0,1,(PBYTE)&PartitionMBR,btHardDrive,&DriveParam))
	{	//if it's USB harddisk & it have no MBR
		if(btHardDrive>0x80 && (DriveParam.wFlags&0x04))
		{
			pParHard->pePriParInfo[0].SystemFlag=0;
			for(i=0;i<4;i++)
			{
				if(PartitionMBR.Partition[i].BootFlag!=0 && PartitionMBR.Partition[i].BootFlag!=0x80)	
				{
					pParHard->wNumOfPri = 1;
					pParHard->pePriParInfo[0].StartSector=0;
					pParHard->pePriParInfo[0].SectorsInPartition=(DWORD)DriveParam.dwSectors ;
					if(PartitionMBR.StartCode[86]==0x32 && PartitionMBR.StartCode[85]==0x33)
						pParHard->pePriParInfo[0].SystemFlag=0x0b;
					if(PartitionMBR.StartCode[58]==0x36 && PartitionMBR.StartCode[57]==0x31 )
						pParHard->pePriParInfo[0].SystemFlag=0x06;
					if(PartitionMBR.StartCode[58]==0x32 && PartitionMBR.StartCode[57]==0x31)
						pParHard->pePriParInfo[0].SystemFlag=0x04;
					if(pParHard->pePriParInfo[0].SystemFlag==0)
					{
						if (ReadSector(6,1,(PBYTE)&PartitionMBR,btHardDrive,&DriveParam))
						{
							if(PartitionMBR.StartCode[86]==0x32 && PartitionMBR.StartCode[85]==0x33)
								pParHard->pePriParInfo[0].SystemFlag=0x0b;
							if(PartitionMBR.StartCode[58]==0x36 && PartitionMBR.StartCode[57]==0x31 )
								pParHard->pePriParInfo[0].SystemFlag=0x06;
							if(PartitionMBR.StartCode[58]==0x32 && PartitionMBR.StartCode[57]==0x31)
								pParHard->pePriParInfo[0].SystemFlag=0x04;
						}
					}
					return TRUE;
				}
			}
		}
		pParHard->wNumOfPri = 0;
		pParHard->wNumOfLogic = 0;
		//  get how many primary partition table entry
		for(i=0;i<4;i++)
		{
			if (PartitionMBR.Partition[i].SystemFlag != 0)
			{
				memcpy(&pParHard->pePriParInfo[pParHard->wNumOfPri],&PartitionMBR.Partition[i],sizeof(PARTITION_ENTRY));
				pParHard->wNumOfPri ++;
			}
		}	
		//Search the logical partition tabel
		for(i=0;i<4;i++)
		{
			if(PartitionMBR.Partition[i].SystemFlag == 0x05 || 
			   PartitionMBR.Partition[i].SystemFlag == 0x0f)
			{
				dwTempSec = PartitionMBR.Partition[i].StartSector;
				dwLogicStart = dwTempSec;
				for(i=0;i<MAX_PAR_NUM;i++)
				{	//  read logic partition table chain
					if(ReadSector(dwTempSec,1,(PBYTE)&PartitionMBR,btHardDrive,&DriveParam))
					{
						if (!IsValidEMBR(dwLogicStart,&PartitionMBR,&DriveParam)) break;

						if (PartitionMBR.Partition[0].SystemFlag)
						{
							memcpy(&pParHard->peLogParInfo[pParHard->wNumOfLogic].LogParInfo,&PartitionMBR.Partition,sizeof(PARTITION_ENTRY));
							pParHard->peLogParInfo[pParHard->wNumOfLogic].dwLogicStart = dwTempSec;
							pParHard->wNumOfLogic ++;
						}
						if(PartitionMBR.Partition[1].SystemFlag)
							dwTempSec = dwLogicStart+PartitionMBR.Partition[1].StartSector;
						else break;
					}
					else    break;
				}
				pParHard->peLogParInfo[pParHard->wNumOfLogic].dwLogicStart = 0;
				break;
			}
		}
		blResult = TRUE;
	}			
	return blResult;
}   //	GetPartitionInfo


DWORD  LogicToPhy(DWORD dwSectors,PBIOS_DRIVE_PARAM pDriveParam)
{
	DWORD	dwTrack;
	DWORD	dwHead;
	DWORD	dwSector;
	dwSector = (dwSectors % pDriveParam->dwSecPerTrack)+1;
	dwHead   = (dwSectors / pDriveParam->dwSecPerTrack) % pDriveParam->dwHeads ;
	dwTrack  = (dwSectors / pDriveParam->dwSecPerTrack) / pDriveParam->dwHeads ;
	return ((dwTrack & 0x00FF) << 24) | ((dwTrack & 0x0300) << 14) | (dwHead << 8) | (dwSector << 16);
}	//	LogicToPhy	

BOOL EXPORT ReadSector(DWORD dwStartSec, WORD wSectors,
						PBYTE pBuf,BYTE btUnit,PBIOS_DRIVE_PARAM pDriveParam)
{
	DWORD	        dwPhySec;
	BOOL			blResult;
	DIOC_REGISTERS	reg;
	int             i,k;
	DWORD			cb;
	WORD            wCount,wOffset = 0;
	DWORD	        dwUnit;
	DISK_ADDR_PKT	dapDisk;
	bSupportExten = IsSupportExt13(hYGDev,btUnit);
	i=wSectors/MAX_RW_SECS+1;  //  read MAX_READ_SECS times  
	for (k=0; k<i ; k++)
	{
		if (wOffset+MAX_RW_SECS > wSectors)
			wCount = wSectors-wOffset;
		else wCount = MAX_RW_SECS;

		if (!wCount) break;

		if(bSupportExten)
		{
			dapDisk.btPaketSize = sizeof(DISK_ADDR_PKT);
			dapDisk.wCount = wCount;
			dapDisk.btReserved = 0;
			dapDisk.pbtBuffer = (PBYTE)((DWORD)pBuf+SECTOR_SIZE*wOffset);
			dapDisk.qwStartSector = dwStartSec+wOffset;
			reg.reg_EAX = 0x04200;
			reg.reg_EDX = btUnit;
			reg.reg_ESI = (DWORD)&dapDisk;
			blResult = DeviceIoControl(hYGDev,YG_DISK_W32_INT13,&reg,sizeof(reg),&reg,sizeof(reg),&cb,0); 
			if (!blResult || !reg.reg_EAX)  
				return FALSE;
		}
		else
		{
			dwUnit=(DWORD)(btUnit)&0x000000ff;
			dwPhySec =	LogicToPhy(dwStartSec+wOffset,pDriveParam);
			reg.reg_EAX = 0x200+wCount;
			reg.reg_EBX = (DWORD)pBuf + SECTOR_SIZE*wOffset;			// Buf to recive data
			reg.reg_ECX = HIWORD(dwPhySec);               // First sector
			reg.reg_EDX = (LOWORD(dwPhySec) & 0xFF00) + LOWORD(dwUnit);                // Phys drive number and head
			blResult = DeviceIoControl(hYGDev, 
    									YG_DISK_W32_INT13,		
										&reg,sizeof(reg), 
										&reg,sizeof(reg), 
										&cb,0);
			if (!blResult || !reg.reg_EAX)  
				return FALSE;
		}
		wOffset += wCount;
	}
	return blResult;
}   //	End ReadSector

BOOL EXPORT WriteSector(DWORD dwStartSec,WORD wSectors,
						PBYTE pBuf,BYTE btUnit,PBIOS_DRIVE_PARAM pDriveParam)
{
	DWORD	        dwPhySec;
	BOOL			blResult;
	DIOC_REGISTERS	reg;
	int             i,k;
	DWORD			cb;
	WORD            wCount,wOffset = 0;
	DWORD	        dwUnit;
	DISK_ADDR_PKT	dapDisk;
	bSupportExten = IsSupportExt13(hYGDev,btUnit);
	i=wSectors/MAX_RW_SECS+1;  //  read MAX_READ_SECS times  
	for (k=0; k<i ; k++)
	{
		if (wOffset+MAX_RW_SECS > wSectors)
			wCount = wSectors-wOffset;
		else wCount = MAX_RW_SECS;

		if (!wCount) break;
		if(bSupportExten)
		{
			dapDisk.btPaketSize = sizeof(DISK_ADDR_PKT);
			dapDisk.wCount = wCount;
			dapDisk.btReserved = 0;
			dapDisk.pbtBuffer = (PBYTE)((DWORD)pBuf+SECTOR_SIZE*wOffset);
			dapDisk.qwStartSector = dwStartSec+wOffset;
//			if(pDriveParam->wFlags & 0x0008)
//				reg.reg_EAX = 0x4301;
//			else
			reg.reg_EAX = 0x4300;
			reg.reg_EDX = btUnit;
			reg.reg_ESI = (DWORD)&dapDisk;
			blResult = DeviceIoControl(hYGDev,YG_DISK_W32_INT13,&reg,sizeof(reg),&reg,sizeof(reg),&cb,0); 
			if (!blResult || !reg.reg_EAX)  
				return FALSE;
		}
		else
		{
			dwUnit=(DWORD)(btUnit)&0x000000ff;
			dwPhySec =	LogicToPhy(dwStartSec+wOffset,pDriveParam);
			reg.reg_EAX = 0x300+wCount;
			reg.reg_EBX = (DWORD)pBuf + SECTOR_SIZE*wOffset;			// Buf to recive data
			reg.reg_ECX = HIWORD(dwPhySec);               // First sector
			reg.reg_EDX = (LOWORD(dwPhySec) & 0xFF00) + LOWORD(dwUnit);                // Phys drive number and head
			blResult = DeviceIoControl(hYGDev, 
    									YG_DISK_W32_INT13,		
										&reg,sizeof(reg), 
										&reg,sizeof(reg), 
										&cb,0);
			if (!blResult || !reg.reg_EAX)  
				return FALSE;
		}
		wOffset += wCount;
	}
	return blResult;
}

BOOL EXPORT PartitionInfoOfDriveLetter(BYTE btLetter,PPARTITION_INFO pPartitionInfo)
{
	PARTITION_SEC		PartitionLogicalMBR;
	BIOS_DRIVE_PARAM	DriveParam;
	DWORD				cb;
	int					i;
	BOOL				blResult = FALSE;
	BOOL	blSuc=FALSE;
	PARINFOONHARDDISKEX ParHard;
	SIMULATE_DRIVEMAPINFO	sdDriveMapInfo;

	if (hYGDev == INVALID_HANDLE_VALUE)
		return FALSE;
	memset(&sdDriveMapInfo,0,sizeof(SIMULATE_DRIVEMAPINFO));
	memset(&ParHard,0,sizeof(PARINFOONHARDDISKEX));
	
	if(!GetDriveMapInfo(btLetter,&sdDriveMapInfo))
		return FALSE;
	
	bSupportExten = IsSupportExt13(hYGDev,sdDriveMapInfo.DriveNum);
	//else 
	//	return FALSE;

	if(!GetDriveParam(sdDriveMapInfo.DriveNum,&DriveParam))
		return FALSE;


	if(!GetPartitionInfoEx(sdDriveMapInfo.DriveNum,&ParHard))
		return FALSE;
	
	cb=sdDriveMapInfo.dwStart;
	//It's primary partition
	for(i=0;i<4;i++)
	{
		if(ParHard.pePriParInfo[i].StartSector==cb)
		{
			pPartitionInfo->btDrive = btLetter+'A'-1;
			pPartitionInfo->btHardDisk = sdDriveMapInfo.DriveNum;
			pPartitionInfo->nPartition = i;
			pPartitionInfo->dwPartitionType = PRIMARY;
			pPartitionInfo->btBootFlag = ParHard.pePriParInfo[i].BootFlag ;
			pPartitionInfo->dwSectorsInPartition = ParHard.pePriParInfo[i].SectorsInPartition;
			pPartitionInfo->btSystemFlag=ParHard.pePriParInfo[i].SystemFlag;  
			pPartitionInfo->dwStartLogicalSector = ParHard.pePriParInfo[i].StartSector;
			pPartitionInfo->wStartSector = ParHard.pePriParInfo[i].StartOfPartition[1] & 0x3f;
			pPartitionInfo->wStartHead = ParHard.pePriParInfo[i].StartOfPartition[0];
			WORD	wCylinder;
			wCylinder = (WORD)(ParHard.pePriParInfo[i].StartOfPartition[1] & 0xc0) & 0xFF;
			wCylinder = wCylinder << 2;
			wCylinder = wCylinder | ((WORD)(ParHard.pePriParInfo[i].StartOfPartition[2]) & 0xFF);
			pPartitionInfo->wStartCylinder = wCylinder;
			pPartitionInfo->wEndSector = ParHard.pePriParInfo[i].EndOfPartition[1] &0x3f;
			pPartitionInfo->wEndHead = ParHard.pePriParInfo[i].EndOfPartition[0];
			wCylinder = (WORD)(ParHard.pePriParInfo[i].EndOfPartition[1] & 0xc0) & 0xFF;
			wCylinder = wCylinder << 2;
			wCylinder = wCylinder | ((WORD)(ParHard.pePriParInfo[i].EndOfPartition[2]) & 0xFF);
			pPartitionInfo->wEndCylinder = wCylinder;
			char	szStr[6]=" :\\\0";
			char	lpVolumeNameBuffer[MAX_LABELNAME+2];
			memset(lpVolumeNameBuffer,0x20,MAX_LABELNAME);
			szStr[0]=(char)(btLetter+'A'-1);
			memcpy(pPartitionInfo->szOsLabel,"NO SYSTEM",10);
			memset(pPartitionInfo->szLabelName,0x20,11);
			if(GetVolumeInformation(szStr,lpVolumeNameBuffer,MAX_LABELNAME+2,NULL,NULL,NULL,NULL,0))
			{
				for(i=0;i<MAX_LABELNAME-1;i++)
				{
					if(lpVolumeNameBuffer[i+1]!=0x20)
					{	
						blSuc=TRUE;
						break;
					}
				}
				memcpy(pPartitionInfo->szLabelName,(PBYTE)lpVolumeNameBuffer,MAX_LABELNAME);
			}
			if(!blSuc)
			{
				if(pPartitionInfo->btSystemFlag==0x0b || pPartitionInfo->btSystemFlag==0x0c)
				{
					BOOT_SEC32 bsBoot32;
					if(ReadSector(pPartitionInfo->dwStartLogicalSector,0x01,(PBYTE)&bsBoot32,sdDriveMapInfo.DriveNum,&DriveParam))
					{
						memcpy(pPartitionInfo->szLabelName,bsBoot32.VolumeLabel,0x0b);	
					}
				}
				if(pPartitionInfo->btSystemFlag==0x06 || pPartitionInfo->btSystemFlag==0x0e)
				{
					BOOT_SEC16 bsBoot16;
					if(ReadSector(pPartitionInfo->dwStartLogicalSector,0x01,(PBYTE)&bsBoot16,sdDriveMapInfo.DriveNum,&DriveParam))
					{
						memcpy(pPartitionInfo->szLabelName,bsBoot16.VolumeLabel,0x0b);	
					}
				}
				//memset(pPartitionInfo->szLabelName,0x20,MAX_LABELNAME);
			}
			if(ReadSector(pPartitionInfo->dwStartLogicalSector,0x01,(PBYTE)&PartitionLogicalMBR,sdDriveMapInfo.DriveNum,&DriveParam))
			{
				if(PartitionLogicalMBR.StartCode[5]==0x57)
				{
					if(OSExist1(sdDriveMapInfo.DriveNum,pPartitionInfo->btSystemFlag,pPartitionInfo->dwStartLogicalSector,&DriveParam,pPartitionInfo->btDrive))
					{
						if(DriveParam.SectorsSize==0)
							memcpy(pPartitionInfo->szOsLabel,"WINDOWSME",10);
						if(DriveParam.SectorsSize==1)
							memcpy(pPartitionInfo->szOsLabel,"WINDOWS98SE",10);
						if(DriveParam.SectorsSize==2)
							memcpy(pPartitionInfo->szOsLabel,"WINDOWS98",10);
						if(DriveParam.SectorsSize==3)
							memcpy(pPartitionInfo->szOsLabel,"DOS622",10);
						if(DriveParam.SectorsSize==4)
							memcpy(pPartitionInfo->szOsLabel,"WINDOWS95",10);
						DriveParam.SectorsSize=0x200;
					}

				}
				if(PartitionLogicalMBR.StartCode[5]==0x44)
				{
					if(pPartitionInfo->btSystemFlag !=0x04 || pPartitionInfo->btSystemFlag !=0x01)
					{
						if(OSExist1(sdDriveMapInfo.DriveNum,pPartitionInfo->btSystemFlag,pPartitionInfo->dwStartLogicalSector,&DriveParam,pPartitionInfo->btDrive))
							memcpy(pPartitionInfo->szOsLabel,"MSDOS622",10);
					}
				}
			}
			return TRUE;
		}
	}
	cb=cb-0x3f;
	for(i=0;i<ParHard.wNumOfLogic;i++)
	{
		if(ParHard.peLogParInfo[i].peCurParInfo.StartSector==cb)
		{
			pPartitionInfo->btDrive = btLetter+'A'-1;
			pPartitionInfo->btHardDisk = sdDriveMapInfo.DriveNum;
			pPartitionInfo->nPartition = i;
			pPartitionInfo->dwPartitionType = LOGICAL;
			pPartitionInfo->dwSectorsInPartition = ParHard.peLogParInfo[i].peCurParInfo.SectorsInPartition;
			pPartitionInfo->btSystemFlag= ParHard.peLogParInfo[i].peCurParInfo.SystemFlag ; 
			pPartitionInfo->dwStartLogicalSector = ParHard.peLogParInfo[i].peCurParInfo.StartSector; //it's really !!
			pPartitionInfo->wStartSector = ParHard.peLogParInfo[i].peCurParInfo.StartOfPartition[1] & 0x3f;
			pPartitionInfo->wStartHead = ParHard.peLogParInfo[i].peCurParInfo.StartOfPartition[0];
			WORD	wCylinder;
			wCylinder = (WORD)(ParHard.peLogParInfo[i].peCurParInfo.StartOfPartition[1] & 0xc0) & 0xFF;
			wCylinder = wCylinder << 2;
			wCylinder = wCylinder | ((WORD)(ParHard.peLogParInfo[i].peCurParInfo.StartOfPartition[2]) & 0xFF);
			pPartitionInfo->wStartCylinder = wCylinder;
			pPartitionInfo->wEndSector = ParHard.peLogParInfo[i].peCurParInfo.EndOfPartition[1] &0x3f;
			pPartitionInfo->wEndHead = ParHard.peLogParInfo[i].peCurParInfo.EndOfPartition[0];
			wCylinder = (WORD)(ParHard.peLogParInfo[i].peCurParInfo.EndOfPartition[1] & 0xc0) & 0xFF;
			wCylinder = wCylinder << 2;
			wCylinder = wCylinder | ((WORD)(ParHard.peLogParInfo[i].peCurParInfo.EndOfPartition[2]) & 0xFF);
			pPartitionInfo->wEndCylinder = wCylinder;
			char	szStr[6]=" :\\\0";
			char	lpVolumeNameBuffer[MAX_LABELNAME+2];
			memset(lpVolumeNameBuffer,0x20,MAX_LABELNAME);
			szStr[0]=(char)(btLetter+'A'-1);
			memcpy(pPartitionInfo->szOsLabel,"NO SYSTEM",10);
			memset(pPartitionInfo->szLabelName,0x20,11);
			if(GetVolumeInformation(szStr,lpVolumeNameBuffer,MAX_LABELNAME+2,NULL,NULL,NULL,NULL,0))
			{
				for(i=0;i<MAX_LABELNAME-1;i++)
				{
					if(lpVolumeNameBuffer[i+1]!=0x20)
					{	
						blSuc=TRUE;
						break;
					}
				}
				memcpy(pPartitionInfo->szLabelName,(PBYTE)lpVolumeNameBuffer,MAX_LABELNAME);
			}
			
			if(!blSuc) 
			{
				if(pPartitionInfo->btSystemFlag==0x0b || pPartitionInfo->btSystemFlag==0x0c)
				{
					BOOT_SEC32 bsBoot32;
					if(ReadSector(pPartitionInfo->dwStartLogicalSector+0x3f,0x01,(PBYTE)&bsBoot32,sdDriveMapInfo.DriveNum,&DriveParam))
					{
						memcpy(pPartitionInfo->szLabelName,bsBoot32.VolumeLabel,0x0b);	
					}
				}
				if(pPartitionInfo->btSystemFlag==0x06 || pPartitionInfo->btSystemFlag==0x0e)
				{
					BOOT_SEC16 bsBoot16;
					if(ReadSector(pPartitionInfo->dwStartLogicalSector+0x3f,0x01,(PBYTE)&bsBoot16,sdDriveMapInfo.DriveNum,&DriveParam))
					{
						memcpy(pPartitionInfo->szLabelName,bsBoot16.VolumeLabel,0x0b);	
					}
				}
			}//	memset(pPartitionInfo->szLabelName,0x20,MAX_LABELNAME);
			return TRUE;
		}
	}
	return blResult;
}   //	PartitionInfoOfDriveLetter



DWORD EXPORT LocatePartition(DWORD dwSectors,BYTE btHardDisk,BYTE btStartOrEnd)
{
	DWORD				dwTrack;
	DWORD       		dwSector = 0;
	DWORD				dwHeads= 0 ;
	BIOS_DRIVE_PARAM	DriveParam ;
	if(dwSectors==0x3f && btStartOrEnd==STARTPARTITION)
	{
		return dwSectors;
	}
	if(!GetDriveParam(btHardDisk,&DriveParam))
		return  ERRDRIVEPARAM;
	if(btStartOrEnd==STARTPARTITION)
		dwSector=1;
	if(btStartOrEnd==ENDPARTITION)
	{
		dwSector=DriveParam.dwSecPerTrack;
		dwHeads=DriveParam.dwHeads-1;
	}
	dwTrack  = (dwSectors / DriveParam.dwSecPerTrack) / DriveParam.dwHeads;
	DWORD dwHead1   = (dwSectors / DriveParam.dwSecPerTrack) % DriveParam.dwHeads ;
	if(btStartOrEnd==STARTPARTITION)
	{
		if(dwHead1==DriveParam.dwHeads-1)
			dwTrack++;
		//if(dwHead1==1)
		//	dwTrack--;
	}//dwPhySec=((dwTrack & 0x00FF) << 24) | ((dwTrack & 0x0300) << 14) | (dwHeads << 8) | (dwSector << 16);
	if(btStartOrEnd==ENDPARTITION)
	{
		if(dwHead1==0)
			dwTrack--;
	}
	if(btStartOrEnd==STARTPARTITION)
	{	
		if(dwHead1==1)
			return ((dwTrack*DriveParam.dwHeads+dwHead1)*DriveParam.dwSecPerTrack+dwSector-1);
		else
			return ((dwTrack*DriveParam.dwHeads+dwHeads)*DriveParam.dwSecPerTrack+dwSector-1);
	}
	if(btStartOrEnd==ENDPARTITION)
		return (((dwTrack*DriveParam.dwHeads+dwHeads)*DriveParam.dwSecPerTrack+dwSector-1)+1);
	return ERRDRIVEPARAM;
}

BOOL EXPORT GetPartitionInfoEx(BYTE btHardDrive,PPARINFOONHARDDISKEX pParHard)
{
	PARINFOONHARDDISK	OldParHard;
	int					nCount ;
	DWORD				dwStartSec;
	int					i = 0;
	memset(&OldParHard,0,sizeof(PARINFOONHARDDISK));

	if(!GetPartitionInfo(btHardDrive,&OldParHard))
		return FALSE;
	pParHard->wNumOfLogic = OldParHard.wNumOfLogic ;
	pParHard->wNumOfPri = OldParHard.wNumOfPri ;
	memcpy(&(pParHard->pePriParInfo),&OldParHard.pePriParInfo,4*sizeof(PARTITION_ENTRY));
	for(nCount=0;nCount<4;nCount++)
	{
		if(OldParHard.pePriParInfo[nCount].SystemFlag == 0x05 ||
			OldParHard.pePriParInfo[nCount].SystemFlag == 0x0f)
			break;
	}
	if(nCount != 4 && OldParHard.wNumOfLogic != 0)
	{	//only one and in the extend point to
		dwStartSec = OldParHard.pePriParInfo[nCount].StartSector;
		while(i <= OldParHard.wNumOfLogic)
		{
			memcpy(&pParHard->peLogParInfo[i].peCurParInfo,&OldParHard.peLogParInfo[i].LogParInfo,sizeof(PARTITION_ENTRY));
			pParHard->peLogParInfo[i].dwPreStart = i?OldParHard.peLogParInfo[i-1].dwLogicStart:dwStartSec;
			pParHard->peLogParInfo[i].dwNextStart = OldParHard.peLogParInfo[i+1].dwLogicStart;
			pParHard->peLogParInfo[i].dwCurOffset = OldParHard.peLogParInfo[i].LogParInfo.StartSector;
			pParHard->peLogParInfo[i].peCurParInfo.StartSector = OldParHard.peLogParInfo[i].dwLogicStart;
			i++;
		}
	}
	return TRUE;
}

BOOL EXPORT	GetDriveParam(BYTE btHardDrive,PBIOS_DRIVE_PARAM pDriveParam)
{
	DIOC_REGISTERS		reg;
	DWORD				cb,dwCylinders = 0;
	BOOL				blResult=FALSE;
	__int64				dwTotalSecs = 0;
	
	memset(pDriveParam,0,sizeof(BIOS_DRIVE_PARAM));
	bSupportExten = IsSupportExt13(hYGDev,btHardDrive);

	if(hYGDev != INVALID_HANDLE_VALUE)
	{
		pDriveParam->wInfoSize = sizeof(BIOS_DRIVE_PARAM);
		if (bSupportExten) 
		{
			reg.reg_EAX = 0x4800;               
			reg.reg_EDX = ((DWORD)btHardDrive & 0xFF);           
			reg.reg_ESI = (DWORD)pDriveParam;
			blResult = DeviceIoControl(hYGDev,YG_DISK_W32_INT13,&reg,sizeof(reg),&reg,sizeof(reg),&cb,0);	
			blResult = blResult && reg.reg_EAX && (pDriveParam->wFlags & 0x0002);  
		}
		if (!blResult)
		{
			reg.reg_EAX = 0x0800;
			reg.reg_EDX = ((DWORD)btHardDrive & 0xFF);           
   			reg.reg_ESI = (DWORD)pDriveParam;
			blResult = DeviceIoControl(hYGDev, 
   									   YG_DISK_W32_INT13,		
									   &reg,sizeof(reg), 
									   &reg,sizeof(reg), 
									   &cb,0);	
			blResult = blResult && reg.reg_EAX;
		}
		
		dwCylinders = pDriveParam->dwCylinders;
		dwTotalSecs = pDriveParam->dwSectors ; //Add 2001-5-11
		reg.reg_EAX = 0x0800;
		reg.reg_EDX = ((DWORD)btHardDrive & 0xFF);           
		reg.reg_ESI = (DWORD)pDriveParam;
		blResult = DeviceIoControl(hYGDev, 
								   YG_DISK_W32_INT13,		
									&reg,sizeof(reg), 
									&reg,sizeof(reg), 
									&cb,0);
		blResult = blResult && reg.reg_EAX;

		if (!pDriveParam->dwHeads) pDriveParam->dwHeads = 0xff;
		if (!pDriveParam->dwSecPerTrack) pDriveParam->dwSecPerTrack = 0x3f;
		if (!pDriveParam->dwCylinders)
			pDriveParam->dwCylinders = (DWORD)pDriveParam->dwSectors/pDriveParam->dwHeads/pDriveParam->dwSecPerTrack;
		else
			pDriveParam->dwCylinders = dwCylinders;

    	if(!pDriveParam->dwSectors)//Add 2001-5-11
			pDriveParam->dwSectors = dwTotalSecs;

		if(!dwTotalSecs)//Add 2001-5-11
    		pDriveParam->dwSectors=pDriveParam->dwCylinders*pDriveParam->dwHeads*pDriveParam->dwSecPerTrack;

		if (!pDriveParam->SectorsSize) pDriveParam->SectorsSize = SECTOR_SIZE;
	}
	return blResult;
}

DWORD  EXPORT GetHardDiskNum()
{
	DIOC_REGISTERS		reg;
	DWORD				cb;
	BOOL				blResult=FALSE;
	
	if(hYGDev != INVALID_HANDLE_VALUE)
	{
		memset(&reg,0,sizeof(reg));
		blResult = DeviceIoControl(hYGDev, 
    							   YG_GetDriveNum,		
								   &reg,sizeof(reg), 
								   &reg,sizeof(reg), 
								   &cb,0);	
		if (!blResult || (reg.reg_Flags & 0x0001))
			return	0;
	}
	else
		return 0;
	return  reg.reg_EAX;
}



BYTE	RetrieveDriveLttr(BYTE	DriveNum,DWORD flags,DWORD dwStart)
{
	CVirParti	Obj;

	if( ! Obj.Initialize("\\\\.\\Virparti") )
	{
	#ifdef	_DEBUG
		MessageBox( NULL , "Can't load Virparti driver" , "Error" , MB_OK );
	#endif
		return	0xFF;
	}

	return Obj.RetrieveDriveLttr(DriveNum,flags,dwStart);
}	//	RetrieveDriveLttr

BOOL	GetDriveMapInfo(BYTE	DriveLttr,PSIMULATE_DRIVEMAPINFO pInfo)
{
	CVirParti	Obj;

	if( ! Obj.Initialize("\\\\.\\Virparti") )
	{
		#ifdef	_DEBUG
		MessageBox( NULL , "Can't load Virparti driver" , "Error" , MB_OK );
		#endif
		return	FALSE;
	}

	return	Obj.GetDriveMapInfo(DriveLttr,pInfo);
}	//	GetDriveMapInfo

BYTE EXPORT GetStartupDrive()
{
	PARTITION_SEC		PartitionMBR;
	BIOS_DRIVE_PARAM	DriveParam;
	DWORD				cb;
	BOOL				blResult = FALSE;
	if(INVALID_HANDLE_VALUE == hYGDev)
		return	FALSE;
	
	bSupportExten = IsSupportExt13(hYGDev,0x80);
	
	if(!GetDriveParam(0x80,&DriveParam))
		return FALSE;	
	if (!ReadSector(0,1,(PBYTE)&PartitionMBR,0x80,&DriveParam))
		return FALSE;

	for(int i=0;i<4;i++)
	{
		if(PartitionMBR.Partition[i].BootFlag==0x80)
		{
			if(PartitionMBR.Partition[i].SystemFlag!=0x05 && PartitionMBR.Partition[i].SystemFlag!=0x0f)
			{
				cb=PartitionMBR.Partition[i].StartSector;	
				break;
			}
		}
	}
	return RetrieveDriveLttr(0x80,PRIMARY,cb);
 	

}


/*BOOL EXPORT OSExist(BYTE btDriveLetter,char *szOsLabel)
{
	HANDLE hFind=INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA wfdFileData;
	SIMULATE_DRIVEMAPINFO	sdDriveMapInfo;
	BIOS_DRIVE_PARAM DriveParam;
	BYTE	pBuf[512];
	int		i;
	DWORD	cb;
	HANDLE hFile=INVALID_HANDLE_VALUE;
	char *Str[3];
	char Str0[]=" :\\io.sys\0";
	char Str1[]=" :\\msdos.sys\0";
	char Str2[]=" :\\command.com\0";

	Str0[0]=btDriveLetter;
	Str1[0]=btDriveLetter;
	Str2[0]=btDriveLetter;
	Str[0]=Str0;
	Str[1]=Str1;
	Str[2]=Str2;
	for(i=0;i<3;i++)
	{
		hFind=FindFirstFile(Str[i],&wfdFileData);
		if(hFind==INVALID_HANDLE_VALUE)
			break;
		hFind=INVALID_HANDLE_VALUE;
	}
	if(i==3 )
	{
		if(szOsLabel!=NULL)
		{
			btDriveLetter=btDriveLetter-'A'+1;
			memcpy(szOsLabel,"NO SYSTEM",10);
			if(!GetDriveMapInfo(btDriveLetter,&sdDriveMapInfo))
				return FALSE;
			if(!GetDriveParam(sdDriveMapInfo.DriveNum,&DriveParam))
				return FALSE;
			if(!ReadSector(sdDriveMapInfo.dwStart,1,pBuf,sdDriveMapInfo.DriveNum,&DriveParam))
				return FALSE;
			if(pBuf[5]==0x44)
				memcpy(szOsLabel,"MSDOS622",10);
			if(pBuf[5]==0x57 && pBuf[10]==0x30)
				memcpy(szOsLabel,"WINDOWS95",10);
			if(pBuf[5]==0x57 && pBuf[10]==0x31)
				memcpy(szOsLabel,"WINDOWS98",10);
			return TRUE;
			hFile=CreateFile(Str[0],GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_FLAG_RANDOM_ACCESS,NULL);
			if(INVALID_HANDLE_VALUE==hFile)
				return FALSE;
			else
			{
				if(hFile!=NULL)
				{
					if(ReadFile(hFile,pBuf,512,&cb,NULL))
					{
						if(pBuf[2]==0)
						{
							memcpy(szOsLabel,"WINDOWSME",10);
						}
					}
				}
			}
		}
	}
	else
	{
		if(i==1)
		{
			hFile=CreateFile(Str[0],GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_FLAG_RANDOM_ACCESS,NULL);
			if(INVALID_HANDLE_VALUE==hFile)
				return FALSE;
			else
			{
				if(hFile!=NULL)
				{
					if(ReadFile(hFile,pBuf,512,&cb,NULL))
					{
						if(pBuf[2]==0)
						{
							memcpy(szOsLabel,"WINDOWSME",10);
						}
					}
				}
			}
		}	
	}	
	return TRUE;
}*/

BOOL SearchSysFile(PFATBOOTSECT pBootSec,BYTE btHardDisk,PBIOS_DRIVE_PARAM DriveParam)
{
	DWORD i;
	int n,j,m=0;
	BYTE pRoot[512];
	PBYTE pTemp;
	DWORD dwTemp1=0;
	BOOL  blSuc=TRUE,blFindIosys=FALSE;
	BYTE *pSystem[3];
	WORD Month=0;
	WORD Day=0;
	WORD Temp;

	char szStr0[]="IO      SYS";
	BYTE szStr00[11];
	memcpy(szStr00,(PBYTE)szStr0,11);
	pSystem[0]=szStr00;
	
	char szStr1[]="MSDOS   SYS";
	BYTE szStr01[11];
	memcpy(szStr01,(PBYTE)szStr1,11);
	pSystem[1]=szStr01;
	
	char szStr2[]="COMMAND COM";
	BYTE szStr02[11];
	memcpy(szStr02,(PBYTE)szStr2,11);
	pSystem[2]=szStr02;

	//memset(blRenameSuc,0,3*4);

	if(pBootSec->dwRoot!=0)//it's FAT32
	{
		DWORD dwRootStart,dwRootClusters;
		DWORD dwTemp,dwRoot;

		dwRootStart=pBootSec->dwRootStart;
		dwRootClusters=GetRootClusters(pBootSec->dwRoot,pBootSec->dwFatStart,btHardDisk,DriveParam);
		if(dwRootClusters==0)
			return FALSE;
		dwRoot=pBootSec->dwRoot;
		for(dwTemp=0;dwTemp<dwRootClusters;dwTemp++)
		{
			for(i=dwRootStart;i<dwRootStart+pBootSec->dwSectorsPerCluster;i++)
			{
				if(i>DriveParam->dwCylinders*DriveParam->dwHeads*DriveParam->dwSecPerTrack)
					return FALSE;
				if(!ReadSector(i,1,pRoot,btHardDisk,DriveParam))
					return FALSE;
				for(j=0;j<3;j++)
				{
					pTemp=pRoot;
					while(pTemp<pRoot+512)
					{
						blSuc=TRUE;
						for(n=0;n<11;n++)
						{
							if(*(pSystem[j]+n)!=*(pTemp+n))
							{
								blSuc=FALSE;
								break;
							}
						}
						if(blSuc)
						{
							if(j==0) 
							{	
								blFindIosys=TRUE;
								Day=(WORD)(*(pTemp+24))&0x001f;
								Month=(WORD)(*(pTemp+25))<<8&0xff00;
								Month|=(WORD)(*(pTemp+24))&0x00ff;
								Temp=Month;
								Month=(Month&0x01e0)>>5;
								Temp=(Temp&0xfe00)>>9;
								Temp+=1980;
								dwTemp1|=(DWORD)Temp<<16;
								dwTemp1|=(DWORD)(Month<<8);
								dwTemp1|=(DWORD)Day;
								if(dwTemp1>0x07cc0401)//1996/11/4 chinesesimple
									pBootSec->blIs95=TRUE;
								if(dwTemp1>0x07cd0101)  //1997/2/25chinesesimple
								{	pBootSec->blIs97=TRUE;
									pBootSec->blIs95=0;}
								if(dwTemp1>0x07ce0401)//1999/4/23
								{	pBootSec->blIs98=TRUE;
									pBootSec->blIs95=0;
									pBootSec->blIs97=0;
								}
								if(dwTemp1>0x07cf0201)//1999/4/23
								{	pBootSec->blIs98Se=TRUE;
									pBootSec->blIs95=0;
									pBootSec->blIs97=0;
									pBootSec->blIs98=0;}
								if(dwTemp1>0x07d00400)//2000/4/4
								{	pBootSec->blIsME=TRUE;
									pBootSec->blIs95=0;
									pBootSec->blIs97=0;
									pBootSec->blIs98=0;
									pBootSec->blIs98Se=0;}				
							}
							m++;
						}
						if(m==3)
							break; 
						pTemp+=32;
					}
					if(m==3)
						break; 
				}
				if(m==3)
					break; 
			}
			if(m==3)
				break; 
			dwRoot=GetRootNextCluster(dwRoot,pBootSec->dwFatStart,btHardDisk,DriveParam);
			if(dwRoot==ERRDRIVEPARAM)
				return FALSE;
			dwRootStart=ClusterToSector(dwRoot,pBootSec->dwFatStart+2*pBootSec->dwSectorsPerFAT,pBootSec->dwSectorsPerCluster);
			//ASSERT(dwRootStart<DriveParam->dwCylinders*DriveParam->dwHeads*DriveParam->dwSecPerTrack);
		}	
	}
	else // it's FAT16
	{
		for(i=pBootSec->dwRootStart;i<pBootSec->dwRootStart+32;i++)
		{
			if(i>DriveParam->dwCylinders*DriveParam->dwHeads*DriveParam->dwSecPerTrack)
				return FALSE;

			if(!ReadSector(i,1,pRoot,btHardDisk,DriveParam))
				return FALSE;
			for(j=0;j<3;j++)
			{
				pTemp=pRoot;
				while(pTemp<pRoot+512)
				{
					blSuc=TRUE;
					for(n=0;n<11;n++)
					{
						if(*(pSystem[j]+n)!=*(pTemp+n))
						{
							blSuc=FALSE;
							break;
						}
					}
					if(blSuc)
					{
						if(j==0) 
						{	
								blFindIosys=TRUE;
								Day=(WORD)(*(pTemp+24))&0x001f;
								Month=(WORD)(*(pTemp+25))<<8&0xff00;
								Month|=(WORD)(*(pTemp+24))&0x00ff;
								Temp=Month;
								Month=(Month&0x01e0)>>5;
								Temp=(Temp&0xfe00)>>9;
								Temp+=1980;
								dwTemp1|=(DWORD)Temp<<16;
								dwTemp1|=(DWORD)(Month<<8);
								dwTemp1|=(DWORD)Day;
								if(dwTemp1>0x07cc0401)//1996/11/4 chinesesimple
									pBootSec->blIs95=TRUE;
								if(dwTemp1>0x07cd0101)  //1997/2/25chinesesimple
								{	pBootSec->blIs97=TRUE;
									pBootSec->blIs95=0;}
								if(dwTemp1>0x07ce0401)//1999/4/23
								{	pBootSec->blIs98=TRUE;
									pBootSec->blIs95=0;
									pBootSec->blIs97=0;
								}
								if(dwTemp1>0x07cf0201)//1999/4/23
								{	pBootSec->blIs98Se=TRUE;
									pBootSec->blIs95=0;
									pBootSec->blIs97=0;
									pBootSec->blIs98=0;}
								if(dwTemp1>0x07d00400)//2000/4/4
								{	pBootSec->blIsME=TRUE;
									pBootSec->blIs95=0;
									pBootSec->blIs97=0;
									pBootSec->blIs98=0;
									pBootSec->blIs98Se=0;}							}
						m++;
					}
					if(m==3)
						goto End_handler; 
					pTemp+=32;
				}
			}
		}
	}

End_handler:
	if(m!=3 && blFindIosys==FALSE)
		return FALSE;
	return TRUE;
}

DWORD GetRootNextCluster(DWORD dwPriClu,DWORD dwFatStartSec,BYTE btHardDisk,PBIOS_DRIVE_PARAM DriveParam)
{
    DWORD    dwNextSec   ;                    /* the start fat if the cache need to flush */
    DWORD    pdwFat[FAT32ENTRY_PER_SECTOR]  ;
	
	dwNextSec = dwFatStartSec + dwPriClu/FAT32ENTRY_PER_SECTOR; 
	if(dwNextSec>DriveParam->dwCylinders*DriveParam->dwHeads*DriveParam->dwSecPerTrack)
		return ERRDRIVEPARAM;

	if(!ReadSector(dwNextSec,1,(PBYTE)pdwFat,btHardDisk,DriveParam))
		  return ERRDRIVEPARAM;

    return pdwFat[dwPriClu % FAT32ENTRY_PER_SECTOR];
}

DWORD GetRootClusters(DWORD  dwStaClu,DWORD dwFatStartSec,BYTE btHardDisk,PBIOS_DRIVE_PARAM DriveParam)
{
	DWORD    dwCluCounter = 1 ;  // clusters counter  
    while(1)
	{
        dwStaClu = GetRootNextCluster(dwStaClu,dwFatStartSec,btHardDisk,DriveParam);//get next cluster and 
		if(dwStaClu!=ERRDRIVEPARAM)
		{
			//ASSERT(dwStaClu>0x0ffffff0);
			if  (dwStaClu == END_OF_FAT32_CLAIN || dwStaClu ==0x0ffffff8)                  //cmp it with 0x0fffffff
			{
				break;                                            // end of cluster clain 
			}
			dwCluCounter ++;
		}
		else
			return 0;
	}
    return dwCluCounter;
}

inline  DWORD  ClusterToSector(DWORD  dwCluster,DWORD dwDataStartSec,DWORD dwSectorsPerCluster)
{
	return  dwDataStartSec + (dwCluster-2) * dwSectorsPerCluster ;
}


BOOL FatSet(BYTE btHardDisk,BYTE btSystemFlag,DWORD dwStartSec,PBIOS_DRIVE_PARAM DriveParam,PFATBOOTSECT pSec)
{
	DWORD dwSecPerFAT;
	DWORD dwSectorsPerCluster;
	//	  return FALSE;
	if(btSystemFlag==0x0e || btSystemFlag==0x06 || btSystemFlag==0x04 || btSystemFlag==0x01 )
	{
		BOOT_SEC16 Buffer16;
		if(dwStartSec>DriveParam->dwCylinders*DriveParam->dwHeads*DriveParam->dwSecPerTrack)
			return FALSE;
		if(!ReadSector(dwStartSec,1,(PBYTE)&Buffer16,btHardDisk,DriveParam))
			return FALSE;

		dwSectorsPerCluster=((DWORD)(Buffer16.SectorsPerCluster))&0x000000ff;
		dwSecPerFAT=((DWORD)(Buffer16.SectorsPerFAT_16))&0x0000ffff;
	    pSec->dwRootStart=dwStartSec+2*dwSecPerFAT;
		pSec->dwFatStart=dwStartSec+1;
		pSec->dwRoot =0;
		pSec->dwSectorsPerFAT=dwSecPerFAT;
		pSec->dwSectorsPerCluster =dwSectorsPerCluster;
	
	}
	if(btSystemFlag==0x0b || btSystemFlag==0x0c)
	{
		BOOT_SEC32 Buffer32;
		DWORD dwResearved;
		DWORD dwRootStart;
		if(dwStartSec>DriveParam->dwCylinders*DriveParam->dwHeads*DriveParam->dwSecPerTrack)
			return FALSE;

		if(!ReadSector(dwStartSec,1,(PBYTE)&Buffer32,btHardDisk,DriveParam))
			return FALSE;
		
		dwSectorsPerCluster=((DWORD)(Buffer32.SectorsPerCluster))&0x000000ff;
		dwResearved=((DWORD)(Buffer32.BootSectors))&0x0000ffff;
		dwSecPerFAT=Buffer32.SectorsPerFAT_32;
		dwRootStart=Buffer32.RootCluster_Start;
		pSec->dwRootStart=dwStartSec+dwResearved+2*dwSecPerFAT+(dwRootStart-2)*dwSectorsPerCluster;//2 is Start
		pSec->dwFatStart=dwStartSec+dwResearved;
		pSec->dwRoot =dwRootStart;
		pSec->dwSectorsPerFAT =dwSecPerFAT;
		pSec->dwSectorsPerCluster =dwSectorsPerCluster;
	}
	return TRUE;
}


BOOL EXPORT OSExist(BYTE btHardDisk,BYTE btSystemFlag,DWORD dwStartSec,PBIOS_DRIVE_PARAM DriveParam)
{
	FATBOOTSECT  BootSec;
	memset(&BootSec,0,sizeof(FATBOOTSECT));
	if(!FatSet(btHardDisk,btSystemFlag,dwStartSec,DriveParam,&BootSec))
		return FALSE;
	if(!SearchSysFile(&BootSec,btHardDisk,DriveParam))
		return FALSE;
	else
	{
		if(BootSec.blIsME==TRUE)	
			DriveParam->SectorsSize=0; 
		if(BootSec.blIs98Se==TRUE)
			DriveParam->SectorsSize=1; 
		if(BootSec.blIs98==TRUE)
			DriveParam->SectorsSize=2; 
		if(BootSec.blIs97==TRUE)
			DriveParam->SectorsSize=3; 
		if(BootSec.blIs95==TRUE)
			DriveParam->SectorsSize=4; 
	}	
	return TRUE;
}


BOOL OSExist1(BYTE btHardDisk,BYTE btSystemFlag,DWORD dwStartSec,PBIOS_DRIVE_PARAM DriveParam,int nDrive)
{
	FATBOOTSECT  BootSec;
	memset(&BootSec,0,sizeof(FATBOOTSECT));
	BootSec.blIs95=0; 
	BootSec.blIs97=0;
	BootSec.blIs98=0;
	BootSec.blIs98Se=0;
	if(!FatSet(btHardDisk,btSystemFlag,dwStartSec,DriveParam,&BootSec))
		return FALSE;
	if(!SearchSysFile1(&BootSec,btHardDisk,DriveParam,nDrive))
		return FALSE;
	else
	{
		if(BootSec.blIsME==TRUE)	
			DriveParam->SectorsSize=0; 
		if(BootSec.blIs98Se==TRUE)
			DriveParam->SectorsSize=1; 
		if(BootSec.blIs98==TRUE)
			DriveParam->SectorsSize=2; 
		if(BootSec.blIs97==TRUE)
			DriveParam->SectorsSize=3; 
		if(BootSec.blIs95==TRUE)
			DriveParam->SectorsSize=4; 
	}	
	return TRUE;
}


BOOL SearchSysFile1(PFATBOOTSECT pBootSec,BYTE btHardDisk,PBIOS_DRIVE_PARAM DriveParam,int nDrive1)
{
	DWORD i;
	int n,j,m=0;
	BYTE pRoot[512];
	PBYTE pTemp;
	DWORD dwTemp1=0;
	BOOL  blSuc=TRUE,blFindIosys=FALSE;
	BYTE *pSystem[3];
	WORD Month=0;
	WORD Day=0;

	char szStr0[]="IO      SYS";
	BYTE szStr00[11];
	memcpy(szStr00,(PBYTE)szStr0,11);
	pSystem[0]=szStr00;
	
	char szStr1[]="MSDOS   SYS";
	BYTE szStr01[11];
	memcpy(szStr01,(PBYTE)szStr1,11);
	pSystem[1]=szStr01;
	
	char szStr2[]="COMMAND COM";
	BYTE szStr02[11];
	memcpy(szStr02,(PBYTE)szStr2,11);
	pSystem[2]=szStr02;

	if(pBootSec->dwRoot!=0)//it's FAT32
	{
		DWORD dwRootStart,dwRootClusters;
		DWORD dwTemp,dwRoot;

		dwRootStart=pBootSec->dwRootStart;
		dwRootClusters=GetRootClusters(pBootSec->dwRoot,pBootSec->dwFatStart,btHardDisk,DriveParam);
		if(dwRootClusters==0)
			return FALSE;
		dwRoot=pBootSec->dwRoot;
		for(dwTemp=0;dwTemp<dwRootClusters;dwTemp++)
		{
			for(i=dwRootStart;i<dwRootStart+pBootSec->dwSectorsPerCluster;i++)
			{
				if(i>DriveParam->dwCylinders*DriveParam->dwHeads*DriveParam->dwSecPerTrack)
					return FALSE;
				if(!ReadSector(i,1,pRoot,btHardDisk,DriveParam))
					return FALSE;
				for(j=0;j<3;j++)
				{
					pTemp=pRoot;
					while(pTemp<pRoot+512)
					{
						blSuc=TRUE;
						for(n=0;n<11;n++)
						{
							if(*(pSystem[j]+n)!=*(pTemp+n))
							{
								blSuc=FALSE;
								break;
							}
						}
						if(blSuc)
						{
							if(j==2) 
							{	
								blFindIosys=TRUE;
								WhichOS(pBootSec,nDrive1);
							}
							m++;
						}
						if(m==3)
							break; 
						pTemp+=32;
					}
					if(m==3)
						break; 
				}
				if(m==3)
					break; 
			}
			if(m==3)
				break; 
			dwRoot=GetRootNextCluster(dwRoot,pBootSec->dwFatStart,btHardDisk,DriveParam);
			if(dwRoot==ERRDRIVEPARAM)
				return FALSE;
			dwRootStart=ClusterToSector(dwRoot,pBootSec->dwFatStart+2*pBootSec->dwSectorsPerFAT,pBootSec->dwSectorsPerCluster);
			//ASSERT(dwRootStart<DriveParam->dwCylinders*DriveParam->dwHeads*DriveParam->dwSecPerTrack);
		}	
	}
	else // it's FAT16
	{
		for(i=pBootSec->dwRootStart;i<pBootSec->dwRootStart+32;i++)
		{
			if(i>DriveParam->dwCylinders*DriveParam->dwHeads*DriveParam->dwSecPerTrack)
				return FALSE;

			if(!ReadSector(i,1,pRoot,btHardDisk,DriveParam))
				return FALSE;
			for(j=0;j<3;j++)
			{
				pTemp=pRoot;
				while(pTemp<pRoot+512)
				{
					blSuc=TRUE;
					for(n=0;n<11;n++)
					{
						if(*(pSystem[j]+n)!=*(pTemp+n))
						{
							blSuc=FALSE;
							break;
						}
					}
					if(blSuc)
					{
						if(j==0) 
						{	
							blFindIosys=TRUE;
							WhichOS(pBootSec,nDrive1);
						}
						m++;
					}
					if(m==3)
						goto End_handler; 
					pTemp+=32;
				}
			}
		}
	}

End_handler:
	if(m!=3 && blFindIosys==FALSE)
		return FALSE;
	return TRUE;
}

void WhichOS(FATBOOTSECT* p,int nDrive2)
{
	HANDLE hFile=INVALID_HANDLE_VALUE,
		hFileConfig=INVALID_HANDLE_VALUE,hFileAuto=INVALID_HANDLE_VALUE;
	char szOSName[32],strTarget[32];
	char szCommandFile[]=" :\\command.com";
	char szConfig[]=" :\\config.sys";
	char szAuto[]=" :\\autoexec.bat";
	int	 nLen ,result,nTm;
	BOOL blBreak=0;
	DWORD dwLowSize,dwWord,nTimes;
	WIN32_FIND_DATA FindFileData;
	p->blIs95=0;
	p->blIs97=0;
	p->blIs98=0;
	p->blIsME=0;
	szCommandFile[0]=nDrive2;
	szConfig[0]=nDrive2;
	szAuto[0]=nDrive2;
	hFile = CreateFile(szCommandFile,GENERIC_READ|GENERIC_WRITE, 
		                FILE_SHARE_READ,NULL,OPEN_ALWAYS, 
		                FILE_ATTRIBUTE_NORMAL,NULL);
	
	if(hFile==INVALID_HANDLE_VALUE)
	{	
		if(INVALID_HANDLE_VALUE==FindFirstFile(szConfig,&FindFileData))
			return;
		else
		{
			if(INVALID_HANDLE_VALUE==FindFirstFile(szAuto,&FindFileData))
				return;
			else
			{
				p->blIsME=1;
				return;
			}
		}
	}
	dwLowSize = 4096;//GetFileSize(hFile,&dwHighSize);
	for(nTm=0;nTm<4;nTm++)
	{
		if(nTm==0)	memcpy(szOSName,OS_WINDOWS95,11);
		if(nTm==1)	memcpy(szOSName,OS_WINDOWS98,20);
		if(nTm==2)	memcpy(szOSName,OS_WINDOWSME,28);
		if(nTm==3)	memcpy(szOSName,OS_DOS,12);
		nLen = strlen(szOSName);
		for(nTimes = 0 ;nTimes<(dwLowSize - nLen);nTimes++)
		{
			SetFilePointer( hFile, nTimes,NULL,FILE_BEGIN);
			ReadFile(hFile, strTarget, nLen, &dwWord, NULL) ; 
			//strTarget[11] = 0;
			if(nTm==0)
				result=memcmp(szOSName,strTarget,10);
			if(nTm==1)
				result=memcmp(szOSName,strTarget,19);
			if(nTm==2)
				result=memcmp(szOSName,strTarget,27);
			if(nTm==3)
				result=memcmp(szOSName,strTarget,11);
			if(!result)
			{	
				blBreak=1;
				break;
			}
		}
		if(blBreak)
		{	
			if(nTm==0) p->blIs95=1; 
			if(nTm==1) p->blIs98=1; 
			if(nTm==2) p->blIsME=1; 
			if(nTm==3) p->blIs97=1; 
			break;
		}		
	}
	CloseHandle(hFile);
}

