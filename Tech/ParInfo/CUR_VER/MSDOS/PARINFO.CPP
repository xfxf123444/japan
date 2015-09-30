#include <string.h>
#include"private.h"
#include"parinfo.h"
#include<memory.h>
#include<dos.h>
#include <stdio.h>
#include <malloc.h>
BOOL							blSupportExt13=FALSE;
const BYTE StdMBR[512]= {0x33,0xc0,0x8e,0xd0,0xbc,0x0 ,0x7c,0xfb,0x50,0x7,0x50,0x1f,0xfc,0xbe,0x1b,0x7c,0xbf,0x1b,0x6 ,0x50,
						 0x57,0xb9,0xe5,0x1 ,0xf3,0xa4,0xcb,0xbe,0xbe,0x7 , 0xb1,0x4 ,0x38,0x2c,0x7c,0x9 ,0x75,0x15,0x83,0xc6,
						 0x10,0xe2,0xf5,0xcd,0x18,0x8b,0x14,0x8b,0xee,0x83,0xc6,0x10,0x49,0x74,0x16,0x38,0x2c,0x74,0xf6,0xbe,
						 0x10,0x7 ,0x4e,0xac,0x3c,0x0 ,0x74,0xfa,0xbb,0x7 ,0x0 ,0xb4,0xe ,0xcd,0x10,0xeb,0xf2,0x89,0x46,0x25,
						 0x96,0x8a,0x46,0x4 ,0xb4,0x6 ,0x3c,0xe ,0x74,0x11,0xb4,0xb ,0x3c,0xc ,0x74,0x5 ,0x3a,0xc4,0x75,0x2b,
						 0x40,0xc6,0x46,0x25,0x6 ,0x75,0x24,0xbb,0xaa,0x55,0x50,0xb4,0x41,0xcd,0x13,0x58,0x72,0x16,0x81,0xfb,
						 0x55,0xaa,0x75,0x10,0xf6,0xc1,0x1 ,0x74,0xb ,0x8a,0xe0,0x88,0x56,0x24,0xc7,0x6 ,0xa1,0x6 ,0xeb,0x1e,
						 0x88,0x66,0x4 ,0xbf,0xa ,0x0 ,0xb8,0x1 ,0x2 ,0x8b,0xdc,0x33,0xc9,0x83,0xff,0x5 ,0x7f,0x3 ,0x8b,0x4e,
						 0x25,0x3 ,0x4e,0x2 ,0xcd,0x13,0x72,0x29,0xbe,0x46,0x7 ,0x81,0x3e,0xfe,0x7d,0x55,0xaa,0x74,0x5a,0x83,
						 0xef,0x5 ,0x7f,0xda,0x85,0xf6,0x75,0x83,0xbe,0x27,0x7 ,0xeb,0x8a,0x98,0x91,0x52,0x99,0x3 ,0x46,0x8 ,
						 0x13,0x56,0xa ,0xe8,0x12,0x0 ,0x5a,0xeb,0xd5,0x4f,0x74,0xe4,0x33,0xc0,0xcd,0x13,0xeb,0xb8,0x0 ,0x0 ,
						 0x0 ,0x0 ,0x0 ,0x0 ,0x56,0x33,0xf6,0x56,0x56,0x52,0x50,0x6 ,0x53,0x51,0xbe,0x10,0x0 ,0x56,0x8b,0xf4,
						 0x50,0x52,0xb8,0x0 ,0x42,0x8a,0x56,0x24,0xcd,0x13, 0x5a,0x58,0x8d,0x64,0x10,0x72,0xa ,0x40,0x75,0x1 ,
						 0x42,0x80,0xc7,0x2 ,0xe2,0xf7,0xf8,0x5e,0xc3,0xeb,0x74,0x49,0x6e,0x76,0x61,0x6c,0x69,0x64,0x20,0x70,
						 0x61,0x72,0x74,0x69,0x74,0x69,0x6f,0x6e,0x20,0x74,0x61,0x62,0x6c,0x65,0x0 ,0x45,0x72,0x72,0x6f,0x72,
						 0x20,0x6c,0x6f,0x61,0x64,0x69,0x6e,0x67,0x20,0x6f,0x70,0x65,0x72,0x61,0x74,0x69,0x6e,0x67,0x20,0x73,
						 0x79,0x73,0x74,0x65,0x6d,0x0 ,0x4d,0x69,0x73,0x73,0x69,0x6e,0x67,0x20,0x6f,0x70,0x65,0x72,0x61,0x74,
						 0x69,0x6e,0x67,0x20,0x73,0x79,0x73,0x74,0x65,0x6d,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,
						 0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,
						 0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x8b,0xfc,0x1e,0x57,0x8b,0xf5,0xcb,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,
						 0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,
						 0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,
						 0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,
						 0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,
						 0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,
						 0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x55,0xaa};                 

BOOL GetDriveParam(BYTE btHardDrive,BIOS_DRIVE_PARAM *pDiskParam)
{
	DWORD 	dwCylinders = 0;
	BOOL 	bResult=FALSE;
	WORD 	wSeg,wOff;
	DWORD	dwSectorNum = 0;
	
	_fmemset(pDiskParam,0,sizeof(BIOS_DRIVE_PARAM));
	pDiskParam->wInfoSize=sizeof(BIOS_DRIVE_PARAM);
	wOff=_FP_OFF(pDiskParam);
	wSeg=_FP_SEG(pDiskParam);
	blSupportExt13 = IsSupportExt13(btHardDrive);
	if(blSupportExt13)
    {
    	_asm
    	{
      		push ds
      		mov  ah,48h
		    mov  dl,btHardDrive
            mov  si,wOff;
			mov  ds,wSeg            
            int  13h
            pop  ds
            jc   NOT_SUPPORT13EXT
			mov  bx,wOff
			cmp  WORD PTR [bx.wFlags],02h
			jne	 NOT_SUPPORT13EXT
			mov  bResult,1
NOT_SUPPORT13EXT:
		}
	}
	if (!bResult)
	{
		_asm
		{
    		mov ah,08h
    		mov dl,btHardDrive
    		int 13h
            jc  END_NOSUPPORT
			mov bResult,1
			mov bx,wOff
		    mov BYTE PTR [bx.dwHeads],dh
		    inc BYTE PTR [bx.dwHeads]
		    mov dx,cx
		    and dx,0ffc0h
		    ror dx,8
		    shr dh,6
		    inc dx
		    and cx,03Fh
		    mov WORD PTR [bx.dwSecPerTrack],cx
			mov WORD PTR [bx.dwCylinders],dx
END_NOSUPPORT:
		}
	}
	
	dwCylinders = pDiskParam->dwCylinders;
	dwSectorNum = pDiskParam->dwSectorL;
	_fmemset(pDiskParam,0,sizeof(BIOS_DRIVE_PARAM));
	_asm
	{
   		mov ah,08h
   		mov dl,btHardDrive
   		int 13h
        jc  END
		mov bResult,1
		mov bx,wOff
	    mov BYTE PTR [bx.dwHeads],dh
	    inc BYTE PTR [bx.dwHeads]
	    mov dx,cx
	    and dx,0ffc0h
	    ror dx,8
	    shr dh,6
	    inc dx
	    and cx,03Fh
	    mov WORD PTR [bx.dwSecPerTrack],cx
		mov WORD PTR [bx.dwCylinders],dx
END:
	}
		
	if (!pDiskParam->dwHeads) pDiskParam->dwHeads = 0xff;
	if (!pDiskParam->dwSecPerTrack) pDiskParam->dwSecPerTrack = 0x3f;
	if (!pDiskParam->SectorsSize) pDiskParam->SectorsSize = SECTOR_SIZE;

	if (dwCylinders)
		pDiskParam->dwCylinders = dwCylinders;

   	if(dwSectorNum)
   		pDiskParam->dwSectorL = dwSectorNum;

	if (!pDiskParam->dwSectorL) 
		pDiskParam->dwSectorL = dwCylinders * pDiskParam->dwHeads * pDiskParam->dwSecPerTrack;

	pDiskParam->dwCylinders = pDiskParam->dwSectorL/pDiskParam->dwHeads/pDiskParam->dwSecPerTrack;

	return bResult;
}

BOOL IsSupportExt13(BYTE btHardDrive)
{
	BOOL  	bResult= FALSE;
	_asm
	{
		mov  ax,4100h	
		mov  bx,55aah
		mov  dl,btHardDrive
		stc
		int  13h
        jc   NOT_SUPPORT
	    cmp  bx,0aa55h
        jne  NOT_SUPPORT
        test cx,01h
        jz   NOT_SUPPORT
        mov  bResult,1
NOT_SUPPORT:
	}
	return (bResult);
}

BOOL IsValidEMBR(DWORD dwExtStart,PARTITION_SEC *pPartitionMBR,BIOS_DRIVE_PARAM	*pDriveParam)
{
	PARTITION_ENTRY PartiEntry[2];
	BOOL bResult = FALSE;
	DWORD dwMaxSectors;
	if (pPartitionMBR->Signature == 0xAA55)
	{
		_fmemset(&PartiEntry,0,sizeof(PARTITION_ENTRY)*2);
		if (!_fmemcmp(&pPartitionMBR->Partition[2],&PartiEntry,sizeof(PARTITION_ENTRY)*2))
		{
			dwMaxSectors = pDriveParam->dwSectorL;
			if (_fmemcmp(&pPartitionMBR->Partition,&PartiEntry,sizeof(PARTITION_ENTRY)))
			{
				if (pPartitionMBR->Partition[0].SystemFlag)
				{
					bResult = (pPartitionMBR->Partition[0].StartSector < dwMaxSectors) &&
						      (pPartitionMBR->Partition[0].SectorsInPartition < dwMaxSectors);// &&
//						  (dwExtStart + pPartitionMBR->Partition[0].StartSector + pPartitionMBR->Partition[0].SectorsInPartition <= dwMaxSectors);
				}
			}
			else bResult = TRUE;
			if (_fmemcmp(&pPartitionMBR->Partition[1],&PartiEntry,sizeof(PARTITION_ENTRY)) && pPartitionMBR->Partition[1].SystemFlag)
			{
				if ((pPartitionMBR->Partition[1].StartSector > dwMaxSectors) ||
				   (pPartitionMBR->Partition[1].SectorsInPartition > dwMaxSectors)) //||
//   				   (dwExtStart + pPartitionMBR->Partition[1].StartSector + pPartitionMBR->Partition[1].SectorsInPartition > dwMaxSectors))
			    	bResult = FALSE;
			}
		}
	}
	return bResult;
}

DWORD LocatePartition(DWORD dwSectors,BYTE btHardDisk,BYTE btStartOrEnd)
{
	DWORD				      dwTrack;
	DWORD       		      dwSector = 0;
	DWORD			          dwHeads= 0 ;
	BIOS_DRIVE_PARAM DriveParam;
	
	if(!GetDriveParam(btHardDisk,&DriveParam))
		return  ERRDRIVEPARAM;

	if(dwSectors==0x3f && btStartOrEnd==STARTPARTITION)
		return dwSectors;
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
	}
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

BOOL GetPartitionInfo(BYTE btHardDrive,PARINFOONHARDDISK *pParHard)
{
	PARTITION_SEC		 PartitionMBR;
	int i;	
	DWORD				dwTempSec,dwLogicStart;
	BOOL				blResult = FALSE;
	BIOS_DRIVE_PARAM DriveParam;
	
	if(!GetDriveParam(btHardDrive,&DriveParam))
		return  0;
	// read MBR sector
	if (ReadSector(0,1,&PartitionMBR,btHardDrive,&DriveParam))
	{
		if(btHardDrive>0x80 && (DriveParam.wFlags&0x04))
		{
			pParHard->pePriParInfo[0].SystemFlag=0;
			for(i=0;i<4;i++)
			{
				if(PartitionMBR.Partition[i].BootFlag!=0 && PartitionMBR.Partition[i].BootFlag!=0x80)	
				{
					pParHard->wNumOfPri = 1;
					pParHard->pePriParInfo[0].StartSector=0;
					pParHard->pePriParInfo[0].SectorsInPartition=DriveParam.dwSectorL ;
					if(PartitionMBR.StartCode[86]==0x32 && PartitionMBR.StartCode[85]==0x33)
						pParHard->pePriParInfo[0].SystemFlag=0x0b;
					if(PartitionMBR.StartCode[58]==0x36 && PartitionMBR.StartCode[57]==0x31 )
						pParHard->pePriParInfo[0].SystemFlag=0x06;
					if(PartitionMBR.StartCode[58]==0x32 && PartitionMBR.StartCode[57]==0x31)
						pParHard->pePriParInfo[0].SystemFlag=0x04;
					if(pParHard->pePriParInfo[0].SystemFlag==0)
					{
						if (ReadSector(6,1,&PartitionMBR,btHardDrive,&DriveParam))
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
				_fmemcpy(&pParHard->pePriParInfo[pParHard->wNumOfPri],&PartitionMBR.Partition[i],sizeof(PARTITION_ENTRY));
				pParHard->wNumOfPri ++;
			}
		}	
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
							_fmemcpy(&pParHard->peLogParInfo[pParHard->wNumOfLogic].LogParInfo,&PartitionMBR.Partition,sizeof(PARTITION_ENTRY));
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

BOOL GetPartitionInfoEx(BYTE btHardDrive,PARINFOONHARDDISKEX  *pParHard)
{
	PARINFOONHARDDISK	 OldParHard;
	int					nCount ;
	DWORD				dwStartSec;
	WORD				i = 0;
	_fmemset(&OldParHard,0,sizeof(PARINFOONHARDDISK));
	if(!GetPartitionInfo(btHardDrive,&OldParHard))
		return FALSE;
	pParHard->wNumOfLogic = OldParHard.wNumOfLogic ;
	pParHard->wNumOfPri = OldParHard.wNumOfPri ;
	_fmemcpy(&(pParHard->pePriParInfo),&OldParHard.pePriParInfo,4*sizeof(PARTITION_ENTRY));
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
			_fmemcpy(&pParHard->peLogParInfo[i].peCurParInfo,&OldParHard.peLogParInfo[i].LogParInfo,sizeof(PARTITION_ENTRY));
			pParHard->peLogParInfo[i].dwPreStart = i?OldParHard.peLogParInfo[i-1].dwLogicStart:dwStartSec;
			pParHard->peLogParInfo[i].dwNextStart = OldParHard.peLogParInfo[i+1].dwLogicStart;
			pParHard->peLogParInfo[i].dwCurOffset = OldParHard.peLogParInfo[i].LogParInfo.StartSector;
			pParHard->peLogParInfo[i].peCurParInfo.StartSector = OldParHard.peLogParInfo[i].dwLogicStart;
			i++;
		}
	}
	return TRUE;
}

BOOL ReadSector(DWORD dwStaSec,WORD wSectors,PVOID pBuf,BYTE btHardDisk,BIOS_DRIVE_PARAM *pDriveParam)
{
	DISK_ADDR_PKT dapDisk,*pdapDisk;
	DWORD dwPhySec;
	WORD wOff,wSeg,wOffset = 0,wCount;	
	BYTE btResult=1;
	int  i,k;
	blSupportExt13 = IsSupportExt13(btHardDisk);
	i=wSectors/MAX_RW_SECTORS+1;  //  read MAX_READ_SECS times  
	pdapDisk = &dapDisk;
	for (k=0 ; k<i ; k++)
	{
		if (wOffset+MAX_RW_SECTORS > wSectors)
			wCount = wSectors-wOffset;
		else wCount = MAX_RW_SECTORS;

		if (!wCount) break;
		if(blSupportExt13)
		{
			dapDisk.btPaketSize = sizeof(DISK_ADDR_PKT);
			dapDisk.btReserved = 0;
			dapDisk.wCount = wCount;
			dapDisk.pbtBuffer =(DWORD)pBuf+512*wOffset;
			dapDisk.dwStartSectorL = dwStaSec+wOffset;
			dapDisk.dwStartSectorH =0;
			wOff=_FP_OFF(pdapDisk);
			wSeg=FP_SEG(pdapDisk);
			_asm
			{
	            push ds
				mov ah,42h
				mov dl,btHardDisk
				mov si,wOff
				mov ds,wSeg
				int 13h
				mov btResult,ah
	            pop ds
			}
			if(btResult) return 0;
		}
		else
		{
 	   	    WORD c1,d1,wAx;
	   	    dwPhySec=LogicToPhy(dwStaSec+wOffset,pDriveParam);
	   	    c1=HIWORD(dwPhySec);
			wAx = wCount+0x200;
	   	    d1=(LOWORD(dwPhySec) & 0xff00)+btHardDisk;
	   		wOff = LOWORD((DWORD)pBuf)+wOffset*512;
	   		wSeg = HIWORD((DWORD)pBuf);
		   	_asm
		   	{   
		   		push es
		   		mov ax,wAx
		   		mov dx,d1
		   	    mov cx,c1
		   		mov bx,wOff
		   		mov es,wSeg
		   		int 13h
				mov btResult,ah
				pop es
			}
			if(btResult) return 0;
		}
		wOffset += wCount;
	}
    return (BOOL)!btResult;
}   //	ReadSector		

DWORD   LogicToPhy( DWORD  dwLogicSec ,BIOS_DRIVE_PARAM  *pDiskParam)
{
    DWORD   dwCylinder ;
    DWORD   dwHeader   ;
    DWORD   dwSector   ;
    
    dwSector   = ( dwLogicSec % pDiskParam->dwSecPerTrack)+1;
    dwHeader   = ( dwLogicSec / pDiskParam->dwSecPerTrack) %
	               pDiskParam->dwHeads;
    dwCylinder = ( dwLogicSec / pDiskParam->dwSecPerTrack)/
                   pDiskParam->dwHeads;
    return ((dwCylinder & 0x00FF) << 24) | ((dwCylinder & 0x0300) << 14)| (dwHeader << 8) | (dwSector << 16); 
}


int GetHardDiskNum()
{
	BYTE btResult=0;
	_asm
	{
		mov ah,8
		mov dl,80h
		int 13h
		mov btResult,dl
	}
    return btResult; 
}

BOOL HidePartition(DWORD dwStartSec,BYTE btPriOrLog,BYTE btHardDrive,BYTE blHideOrShow)
{
	PARTITION_SEC        PartitionMBR;
	BIOS_DRIVE_PARAM DriveParam;
	
	if(!GetDriveParam(btHardDrive,&DriveParam))
		return  0;// read MBR sector
	if(btPriOrLog==PRIMARY)
	{
		if(!ReadSector(0,1,&PartitionMBR,btHardDrive,&DriveParam))		
	         return 0;
		for(int i=0;i<4;i++)
		{
			if(dwStartSec==PartitionMBR.Partition[i].StartSector)
			{
				if(btHardDrive==0x80)
				{
				//	if(PartitionMBR.Partition[i].BootFlag!=0x80)
				//	{
					if(blHideOrShow)
						PartitionMBR.Partition[i].SystemFlag+=16;
					else
					    PartitionMBR.Partition[i].SystemFlag-=16;
					return WriteSector(0,1,&PartitionMBR,btHardDrive,&DriveParam);
				//	}
				}
				else
				{
						if(blHideOrShow)
							PartitionMBR.Partition[i].SystemFlag+=16;
						else
						    PartitionMBR.Partition[i].SystemFlag-=16;
						return WriteSector(0,1,&PartitionMBR,btHardDrive,&DriveParam);
				}
			}
		}
	}
    if(btPriOrLog==LOGICAL)
	{
	   	dwStartSec=dwStartSec;
	   	if(!ReadSector(dwStartSec,1,&PartitionMBR,btHardDrive,&DriveParam))		
	         return 0;
	    if(blHideOrShow)
			PartitionMBR.Partition[0].SystemFlag+=16;
		else
		    PartitionMBR.Partition[0].SystemFlag-=16;
	    return WriteSector(dwStartSec,1,&PartitionMBR,btHardDrive,&DriveParam);
	}
	return 0;
}

BOOL ActivePartition(DWORD dwStartSec,BYTE btHardDrive)
{
	PARTITION_SEC        PartitionMBR;
	BIOS_DRIVE_PARAM DriveParam;
	int j;
	if(!GetDriveParam(btHardDrive,&DriveParam))
		return  0;
	if(!ReadSector(0,1,&PartitionMBR,btHardDrive,&DriveParam))		
	   return 0;
	for(j=0;j<4;j++)	
	{
		if(PartitionMBR.Partition[j].BootFlag==0x80)
			break;
	}
	for(int i=0;i<4;i++)
	{
		if(dwStartSec==PartitionMBR.Partition[i].StartSector)
		{
			if(i==j)
				return 1;
			else
			{	
				PartitionMBR.Partition[i].BootFlag=0x80;
				if(j<4)
					PartitionMBR.Partition[j].BootFlag=0;
			}
			return WriteSector(0,1,&PartitionMBR,btHardDrive,&DriveParam);
		}
	}
   return 0;
}

BOOL WriteSector(DWORD dwStaSec,WORD wSectors,PVOID pBuf,BYTE btHardDisk,BIOS_DRIVE_PARAM *pDriveParam)
{
	DISK_ADDR_PKT dapDisk,*pdapDisk;
	DWORD 		  dwPhySec;
	WORD 		  wOff,wSeg,wOffset = 0,wCount;	
	BYTE 		  btResult=1;//,btVerify=0;
	int  		  i,k;
	
	blSupportExt13 = IsSupportExt13(btHardDisk);
	i=wSectors/MAX_RW_SECTORS+1;  //  read MAX_READ_SECS times  
	pdapDisk = &dapDisk;
	for (k=0 ; k<i ; k++)
	{
		if (wOffset+MAX_RW_SECTORS > wSectors)
			wCount = wSectors-wOffset;
		else wCount = MAX_RW_SECTORS;

		if (!wCount) break;
		if(blSupportExt13)
		{   
			//FMV can not check this flag
//			if(pDriveParam->wFlags & 0x0008)
//				btVerify = 1;
			dapDisk.btPaketSize = sizeof(DISK_ADDR_PKT);
			dapDisk.btReserved = 0;
			dapDisk.wCount = wCount;
			dapDisk.pbtBuffer =(DWORD)pBuf+512*wOffset;
			dapDisk.dwStartSectorL = dwStaSec+wOffset;
			dapDisk.dwStartSectorH =0;
			wOff=_FP_OFF(pdapDisk);
			wSeg=FP_SEG(pdapDisk);
			_asm
			{
	            push ds
				mov  ax,4300h 
//				mov  al,btVerify
				mov  dl,btHardDisk
				mov  si,wOff
				mov  ds,wSeg 
				stc
				int  13h
	            pop  ds
				jc   EXT_WRITE_FAIL
				mov  btResult,ah
			}
EXT_WRITE_FAIL:			
			if(btResult) return 0;
		}
		else
		{
 	   	    WORD c1,d1,wAx;
	   	    dwPhySec=LogicToPhy(dwStaSec+wOffset,pDriveParam);
	   	    c1=HIWORD(dwPhySec);
			wAx = wCount+0x300;
	   	    d1=(LOWORD(dwPhySec) & 0xff00)+btHardDisk;
	   		wOff = LOWORD((DWORD)pBuf)+wOffset*512;
	   		wSeg = HIWORD((DWORD)pBuf);
		   	_asm
		   	{         
		   		push es
		   		mov  ax,wAx
		   		mov  dx,d1
		   	    mov  cx,c1
		   		mov  bx,wOff
		   		mov  es,wSeg
		   		stc
		   		int  13h
		   		pop  es
		   		jc   WRITE_FAIL //check carry flag 2001-5-9
				mov  btResult,ah
			}
WRITE_FAIL:
			if(btResult) return 0;
		}

		wOffset += wCount;
	}
    return (BOOL)!btResult;
}  

BOOL DoDeletePartition(DWORD dwStartSector,BYTE btHardDisk,CREATE_PAR_FLAG flags,PINT pnErr)
{
    PARTITION_SEC			 PartitionMBR, PartitionLogicalMBR;
    PARINFOONHARDDISKEX	     ParHard;
    DWORD					 dwStartSec,dwPreStart;
    WORD 					 i;
	*pnErr=0;
	BIOS_DRIVE_PARAM DriveParam;
	
	if(!GetDriveParam(btHardDisk,&DriveParam))
		return  FALSE;
	//	read MBR sector
    if(!ReadSector(0,1,&PartitionMBR,btHardDisk,&DriveParam))
	{	
		*pnErr=ERR_PARMAN_READSECTOR;
		return FALSE;
    }
	if(btHardDisk>FirstHardDisk && (DriveParam.wFlags&0x04) && dwStartSector==0)
	{	//not first harddisk && it's removeable && start is 0 
		if(!WriteSector(0,1,(PVOID)StdMBR,btHardDisk,&DriveParam))		
		{	
			*pnErr=ERR_PARMAN_WRITESECTOR;
			return 0;
		}
		if(!WriteSector(6,1,(PVOID)StdMBR,btHardDisk,&DriveParam))		
		{	
			*pnErr=ERR_PARMAN_WRITESECTOR;
			return 0;
		}
		return 1;
	}
	for(i = 0; i < 4; i++)
    {	//it's primary partition
		if(dwStartSector==PartitionMBR.Partition[i].StartSector && 
			(PartitionMBR.Partition[i].SystemFlag!=0x05 &&
			PartitionMBR.Partition[i].SystemFlag!=0x0f))
			break;
	}
	if(i!=4 && (PartitionMBR.Partition[i].SystemFlag!=0x05 &&
			PartitionMBR.Partition[i].SystemFlag!=0x0f))
    {    	
		if(PartitionMBR.Partition[i].BootFlag==FirstHardDisk && !flags.Active)//active partition cann't  be delete
			return 0;
		_fmemset(&PartitionMBR.Partition[i],0,sizeof(PARTITION_ENTRY));
		if(!WriteSector(0,1,&PartitionMBR,btHardDisk,&DriveParam))
		{	
			*pnErr=ERR_PARMAN_WRITESECTOR;
			return FALSE;
		}
		return TRUE;
	}
    for(i=0;i<4;i++)
	{
		if(PartitionMBR.Partition[i].SystemFlag==0x05 ||
			PartitionMBR.Partition[i].SystemFlag==0x0f)
			break;
	}
	if(i!=4 && flags.Extended == FALSE)
	{	//detete extend partition
		_fmemset(&PartitionMBR.Partition[i],0,sizeof(PARTITION_ENTRY));	
		if(!WriteSector(0,1,&PartitionMBR,btHardDisk,&DriveParam))
		{
			*pnErr=ERR_PARMAN_WRITESECTOR;
			return FALSE;
		}
		return TRUE;
	}
	if(!GetPartitionInfoEx(btHardDisk,&ParHard))
	{	
		*pnErr=ERR_PARMAN_GETPARTITIONINFO;
		return FALSE;
	}
	if(i!=4 && ParHard.wNumOfLogic)
	{
		dwStartSec = PartitionMBR.Partition[i].StartSector;
		if(!ReadSector(dwStartSec,1,&PartitionLogicalMBR,btHardDisk,&DriveParam))
		{	
			*pnErr=ERR_PARMAN_READSECTOR;
			return FALSE;
		}
		if(dwStartSector==dwStartSec)
		{	//the partition is the first partition 
			_fmemset(&PartitionLogicalMBR.Partition[0],0,sizeof(PARTITION_ENTRY));
			if(!WriteSector(dwStartSec,1,&PartitionLogicalMBR,btHardDisk,&DriveParam))
			{
				*pnErr=ERR_PARMAN_WRITESECTOR;
				return FALSE;
			}
			return TRUE;
		}
		else
		{
			for(i=0;i<ParHard.wNumOfLogic;i++)
			{
				if(dwStartSector==ParHard.peLogParInfo[i].peCurParInfo.StartSector)
					break;
			}
			if(i==ParHard.wNumOfLogic)
			{	
				*pnErr=ERR_PARMAN_PARAM;
				return FALSE;
			}
			dwPreStart=ParHard.peLogParInfo[i].dwPreStart; 
			if(!ReadSector(dwStartSector,1,&PartitionLogicalMBR,btHardDisk,&DriveParam))
			{	
				*pnErr=ERR_PARMAN_READSECTOR;
				return FALSE;
			}
			if(!ReadSector(dwPreStart,1,&PartitionMBR,btHardDisk,&DriveParam))
			{	
				*pnErr=ERR_PARMAN_READSECTOR;
				return FALSE;
			}
			else
			{
				_fmemcpy(&PartitionMBR.Partition[1],&PartitionLogicalMBR.Partition[1],sizeof(PARTITION_ENTRY));  
				if(!WriteSector(dwPreStart,1,&PartitionMBR,btHardDisk,&DriveParam))
				{	
					*pnErr=ERR_PARMAN_WRITESECTOR;
					return FALSE;
				}
				return TRUE;
			}
		}
	}
	return FALSE;
}   //	End DeletePartition

BOOL VerifyLogicalPartition(PARINFOONHARDDISKEX  *ph, PARTITION_ENTRY  *pe)//0 is success,1 is error
{
	WORD i=0;//,j=0;
	BOOL bl=0;
	for(i=0;i<ph->wNumOfLogic;i++)
	{
		if(ph->peLogParInfo[i].peCurParInfo.StartSector>=ph->peLogParInfo[i+1].peCurParInfo.StartSector &&
			ph->peLogParInfo[i].peCurParInfo.StartSector+ph->peLogParInfo[i].peCurParInfo.SectorsInPartition<=ph->peLogParInfo[i+1].peCurParInfo.StartSector+ph->peLogParInfo[i+1].peCurParInfo.SectorsInPartition)     
		{	
			bl=1;
			return 1;
		}
	}
	for(i=0;i<ph->wNumOfLogic;i++)
	{
		if(((pe->StartSector>=ph->peLogParInfo[i].peCurParInfo.StartSector) &&
			(pe->StartSector<=ph->peLogParInfo[i].peCurParInfo.StartSector+ph->peLogParInfo[i].peCurParInfo.SectorsInPartition)) || 
			((pe->StartSector+pe->SectorsInPartition>=ph->peLogParInfo[i].peCurParInfo.StartSector) &&
			(pe->StartSector+pe->SectorsInPartition<=ph->peLogParInfo[i].peCurParInfo.StartSector+ph->peLogParInfo[i].peCurParInfo.SectorsInPartition)))			
		{	
			bl=1;
			break;
		}
	}
	return bl;
}

BOOL DoCreatePartition(PARTITION_ENTRY *peParEntry,BYTE btHardDisk,DWORD dwFlag,BOOL blIsFormat,PBYTE pLabel,DWORD hWnd,INT* nCreateErrorCode)
{
	int					nPartition;
	WORD				nExtend;
	PARTITION_ENTRY		ParEntryTemp1;
	DWORD				dwStartSec=0;
	DWORD				dwTempStart=0;
	BIOS_DRIVE_PARAM	DriveParam;
	PARTITION_SEC		PartitionMBR,PartitionLogicalMBR;
	PARINFOONHARDDISKEX	ParHard;
	WORD				i;
	DWORD				dwTemp=0;
	*nCreateErrorCode=0;
	INT nResizeErrCode=0;
	INT nDeleteErrCode=0;
	if (peParEntry->BootFlag && peParEntry->BootFlag != 0x80)
		peParEntry->BootFlag = 0;
	_fmemset(&ParHard,0,sizeof(PARINFOONHARDDISKEX));
	if(!GetDriveParam(btHardDisk,&DriveParam))
	{	
		*nCreateErrorCode=ERR_PARMAN_GETDRIVEPARAM;
		return FALSE;
	}
	if(!ReadSector(0,1,(PBYTE)&PartitionMBR,btHardDisk,&DriveParam))		
	{	
		*nCreateErrorCode=ERR_PARMAN_READSECTOR;
		return FALSE;
	}
	if(btHardDisk>FirstHardDisk && (DriveParam.wFlags&0x04))
	{	//It's second drive & It's removeable drive
		for(i=0;i<4;i++)
		{
			if(PartitionMBR.Partition[i].SystemFlag != 0 )
				break;
		}
		if(peParEntry->StartSector!=0 && i==4 )
		{
			if(!WriteSector(0,1,(PBYTE)&StdMBR,btHardDisk,&DriveParam))		
			{	
				*nCreateErrorCode=ERR_PARMAN_WRITESECTOR;
				return FALSE;
			}
		}
	}
	if(btHardDisk>FirstHardDisk)
	{
		if(PartitionMBR.StartCode[0]==0) //It's new harddisk
		{
			if(!WriteSector(0,1,(PBYTE)StdMBR,btHardDisk,&DriveParam))		
			{
				*nCreateErrorCode=ERR_PARMAN_WRITESECTOR;
				return FALSE;
			}
		}
	}
	//at least partition>1 cylinder
	if(peParEntry->SectorsInPartition<DriveParam.dwHeads*DriveParam.dwSecPerTrack)
	{
		//peParEntry->SectorsInPartition=peParEntry->SectorsInPartition+DriveParam.dwHeads*DriveParam.dwSecPerTrack;
		*nCreateErrorCode=ERR_PARMAN_NUMBEROFSECTORS;
		return FALSE;
	}
	//if larger than harddisk
	if(peParEntry->StartSector+peParEntry->SectorsInPartition > DriveParam.dwSectorL)
	{
		*nCreateErrorCode=ERR_PARMAN_NUMBEROFSECTORS;
		return FALSE;
	}
	CaculateStartAndEnd(peParEntry,DriveParam,btHardDisk,dwFlag);
	//It's test version
	if(peParEntry->StartSector<=0x3f && dwFlag==LOGICAL)
	{
		peParEntry->StartSector=DriveParam.dwSecPerTrack*DriveParam.dwHeads;  
		peParEntry->SectorsInPartition=peParEntry->SectorsInPartition-DriveParam.dwSecPerTrack*DriveParam.dwHeads; 
		CaculateStartAndEnd(peParEntry,DriveParam,btHardDisk,dwFlag);
	}//end test version
	if(peParEntry->StartSector<=DriveParam.dwSecPerTrack && dwFlag==LOGICAL)
	{
		*nCreateErrorCode=ERR_PARMAN_EXTENDPARTITIONSTARTSECTOR;
		return FALSE;	
	}
	if(!GetPartitionInfoEx(btHardDisk,&ParHard))
	{
		*nCreateErrorCode=ERR_PARMAN_GETPARTITIONINFO;
		return FALSE;
	}
	//create partition'size < 10M
	if(dwFlag == PRIMARY || dwFlag == EXTEND)
	{
		for(nPartition=0;nPartition<4;nPartition++)
		{
			if(PartitionMBR.Partition[nPartition].SystemFlag == 0)// && 
				break;
		}
		if(nPartition==4)
		{	
			*nCreateErrorCode=ERR_PARMAN_MBRPARTITIONFULL;
			return FALSE;
		}
		for(nExtend=0;nExtend<4;nExtend++)
		{
			if(PartitionMBR.Partition[nExtend].SystemFlag==0x05 ||
				PartitionMBR.Partition[nExtend].SystemFlag==0x0f)
				break;
		}
		//check (include extend)
		for(i=0;i<4;i++)
		{
			if(nExtend!=i && PartitionMBR.Partition[i].SystemFlag!=0)
			{
				if((peParEntry->StartSector >= PartitionMBR.Partition[i].StartSector &&    
					peParEntry->StartSector < PartitionMBR.Partition[i].StartSector+PartitionMBR.Partition[i].SectorsInPartition)) 
				{	
					*nCreateErrorCode=ERR_PARMAN_PARTITIONINPARTITION;
					return FALSE;
				}
				if((peParEntry->StartSector+peParEntry->SectorsInPartition > PartitionMBR.Partition[i].StartSector &&    
					peParEntry->StartSector+peParEntry->SectorsInPartition < PartitionMBR.Partition[i].StartSector+PartitionMBR.Partition[i].SectorsInPartition)) 
				{	
					*nCreateErrorCode=ERR_PARMAN_PARTITIONINPARTITION;
					return FALSE;
				}
			}			
		}//end check
		//create primary partition in the extend partition
		if(dwFlag!=EXTEND && nExtend!=4 && 
			(peParEntry->StartSector>=PartitionMBR.Partition[nExtend].StartSector && peParEntry->StartSector+peParEntry->SectorsInPartition<=PartitionMBR.Partition[nExtend].StartSector+PartitionMBR.Partition[nExtend].SectorsInPartition))
		{
			if(VerifyPrimaryPartition(&ParHard,peParEntry))////0 is success,1 is error
			{
				*nCreateErrorCode=ERR_PARMAN_PARTITIONINPARTITION;
				return FALSE;
			}
			if(peParEntry->StartSector==PartitionMBR.Partition[nExtend].StartSector && 
				peParEntry->StartSector+peParEntry->SectorsInPartition==PartitionMBR.Partition[nExtend].StartSector+PartitionMBR.Partition[nExtend].SectorsInPartition)
			{
				CREATE_PAR_FLAG cpf={0,0};
				DoDeletePartition(peParEntry->StartSector,btHardDisk,cpf,&nDeleteErrCode);
			}
			else
			{
				_fmemset(&ParEntryTemp1,0,sizeof(PARTITION_ENTRY));
				if(ParHard.wNumOfLogic)
				{
					if(peParEntry->StartSector>=PartitionMBR.Partition[nExtend].StartSector && 
						peParEntry->StartSector+peParEntry->SectorsInPartition<=ParHard.peLogParInfo[0].peCurParInfo.StartSector)
					{  //in the front of extend
						ParEntryTemp1.StartSector=peParEntry->StartSector+peParEntry->SectorsInPartition;
						ParEntryTemp1.SectorsInPartition=PartitionMBR.Partition[nExtend].StartSector+PartitionMBR.Partition[nExtend].SectorsInPartition-ParEntryTemp1.StartSector; 
					}
					if(peParEntry->StartSector>=ParHard.peLogParInfo[ParHard.wNumOfLogic-1].peCurParInfo.StartSector+ParHard.peLogParInfo[ParHard.wNumOfLogic-1].peCurParInfo.SectorsInPartition) 
					{	//in the end of extend
						ParEntryTemp1.StartSector=PartitionMBR.Partition[nExtend].StartSector;
						ParEntryTemp1.SectorsInPartition=peParEntry->StartSector-ParEntryTemp1.StartSector;
					}
				}
				else
				{
					ParEntryTemp1.StartSector=peParEntry->StartSector+peParEntry->SectorsInPartition;
					ParEntryTemp1.SectorsInPartition=PartitionMBR.Partition[nExtend].StartSector+PartitionMBR.Partition[nExtend].SectorsInPartition-ParEntryTemp1.StartSector; 
				}
				if(!DoResizeExtendPartition(btHardDisk,&ParEntryTemp1,&nResizeErrCode))
				{
					*nCreateErrorCode=ERR_PARMAN_RESIZEEXTENDPARTITION;
					return FALSE;
				}
			}
			if(!ReadSector(0,1,(PBYTE)&PartitionMBR,btHardDisk,&DriveParam))		
			{	
				*nCreateErrorCode=ERR_PARMAN_READSECTOR;
				return FALSE;
			}
		}
		_fmemcpy(&PartitionMBR.Partition[nPartition],peParEntry,sizeof(PARTITION_ENTRY));
		if(!WriteSector(0,1,(PBYTE)&PartitionMBR,btHardDisk,&DriveParam))
		{
			*nCreateErrorCode=ERR_PARMAN_WRITESECTOR;
			return FALSE;
		}
	}		
	if(dwFlag == LOGICAL)
	{
		dwStartSec = 0;
		for(nExtend=0;nExtend<ParHard.wNumOfPri;nExtend++)
		{
			if(ParHard.pePriParInfo[nExtend].SystemFlag == 0x05 ||
				ParHard.pePriParInfo[nExtend].SystemFlag == 0x0f)
			{
				BOOL  bNeedResize = FALSE;
				_fmemcpy(&ParEntryTemp1,&ParHard.pePriParInfo[nExtend],sizeof(PARTITION_ENTRY));
				if(ParHard.pePriParInfo[nExtend].StartSector > peParEntry->StartSector)
				{	//in the front of extend partition 
					ParEntryTemp1.StartSector = peParEntry->StartSector; 
					ParEntryTemp1.SectorsInPartition += ParHard.pePriParInfo[nExtend].StartSector;
					ParEntryTemp1.SectorsInPartition -= peParEntry->StartSector;
					bNeedResize = TRUE;
				}
				if(peParEntry->StartSector+peParEntry->SectorsInPartition>ParEntryTemp1.StartSector+ParEntryTemp1.SectorsInPartition)
				{	//in the end of extend partition 
					ParEntryTemp1.SectorsInPartition =  peParEntry->StartSector + peParEntry->SectorsInPartition - ParEntryTemp1.StartSector;
					bNeedResize = TRUE;
				}
				if (bNeedResize)
				{
					if(!DoResizeExtendPartition(btHardDisk,&ParEntryTemp1,&nResizeErrCode))
					{
						*nCreateErrorCode=ERR_PARMAN_RESIZEEXTENDPARTITION;
						return FALSE;
					}
					if(!GetPartitionInfoEx(btHardDisk,&ParHard))
					{
						*nCreateErrorCode=ERR_PARMAN_GETPARTITIONINFO;
						return FALSE;
					}
				}
				dwStartSec = ParHard.pePriParInfo[nExtend].StartSector;
				break;
			}
		}
		if(VerifyLogicalPartition(&ParHard,peParEntry))
		{
			*nCreateErrorCode=ERR_PARMAN_PARTITIONINPARTITION;
			return FALSE;
		}
		//that's no extend-> create logical partition
		if(!dwStartSec)
		{
			_fmemcpy(&ParEntryTemp1,peParEntry,sizeof(PARTITION_ENTRY));
			ParEntryTemp1.SystemFlag = 0x05;
			CaculateStartAndEnd(&ParEntryTemp1,DriveParam,btHardDisk,EXTEND);
			if (!DoCreatePriPar(&ParEntryTemp1,btHardDisk,&DriveParam))
			{
				*nCreateErrorCode=ERR_PARMAN_CREATEPARTITION;
				return FALSE;
			}
			if(!GetPartitionInfoEx(btHardDisk,&ParHard))
			{
				*nCreateErrorCode=ERR_PARMAN_GETPARTITIONINFO;
				return FALSE;
			}
			for(nExtend=0;nExtend<ParHard.wNumOfPri;nExtend++)
			{
				if(ParHard.pePriParInfo[nExtend].SystemFlag == 0x05 ||
					ParHard.pePriParInfo[nExtend].SystemFlag == 0x0f)
				{
					dwStartSec = ParHard.pePriParInfo[nExtend].StartSector;
					break;
				}
			}
		}
		//extend is existed && start create in extend partition
		if((peParEntry->StartSector+peParEntry->SectorsInPartition<=ParHard.pePriParInfo[nExtend].StartSector+ParHard.pePriParInfo[nExtend].SectorsInPartition && 
			peParEntry->StartSector>=ParHard.pePriParInfo[nExtend].StartSector))
		{
			dwTemp = dwStartSec;
			for(i=0;i<ParHard.wNumOfLogic;i++)
			{
				if(ParHard.peLogParInfo[i].peCurParInfo.StartSector > peParEntry->StartSector)
					break;
				dwTemp=ParHard.peLogParInfo[i].peCurParInfo.StartSector;
			}

			_fmemset(&PartitionLogicalMBR,0,SECTOR_SIZE);
			PartitionLogicalMBR.Signature = 0xAA55;
			_fmemcpy(&PartitionLogicalMBR.Partition[0],peParEntry,sizeof(PARTITION_ENTRY));
			dwTempStart = peParEntry->StartSector;
			PartitionLogicalMBR.Partition[0].StartSector = DriveParam.dwSecPerTrack;
			PartitionLogicalMBR.Partition[0].StartOfPartition[0]=1; 
			PartitionLogicalMBR.Partition[0].SectorsInPartition=peParEntry->SectorsInPartition-DriveParam.dwSecPerTrack; 
			if (i != ParHard.wNumOfLogic)
			{
				_fmemcpy(&PartitionLogicalMBR.Partition[1],&ParHard.peLogParInfo[i].peCurParInfo,sizeof(PARTITION_ENTRY));
				PartitionLogicalMBR.Partition[1].StartOfPartition[0]=0; 
				PartitionLogicalMBR.Partition[1].SystemFlag=0x05;
				PartitionLogicalMBR.Partition[1].SectorsInPartition += ParHard.peLogParInfo[i].dwCurOffset; 
				PartitionLogicalMBR.Partition[1].StartSector -= dwStartSec;
			}
			if(!WriteSector(dwTempStart,1,(PBYTE)&PartitionLogicalMBR,btHardDisk,&DriveParam))
			{	
				*nCreateErrorCode=ERR_PARMAN_WRITESECTOR;
				return FALSE;
			}

			if (dwTemp != dwTempStart)
			{
				if(!ReadSector(dwTemp,1,(PBYTE)&PartitionLogicalMBR,btHardDisk,&DriveParam))
				{	
					*nCreateErrorCode=ERR_PARMAN_READSECTOR;
					return FALSE;
				}
				if (!IsValidEMBR(dwStartSec,&PartitionLogicalMBR,&DriveParam))
					_fmemset(&PartitionLogicalMBR,0,SECTOR_SIZE);
				_fmemcpy(&ParEntryTemp1,&PartitionLogicalMBR.Partition[0],sizeof(PARTITION_ENTRY));
				_fmemset(&PartitionLogicalMBR,0,512);
				PartitionLogicalMBR.Signature = 0xAA55;

				_fmemcpy(&PartitionLogicalMBR.Partition[0],&ParEntryTemp1,sizeof(PARTITION_ENTRY));
				_fmemcpy(&PartitionLogicalMBR.Partition[1],peParEntry,sizeof(PARTITION_ENTRY)); 
				PartitionLogicalMBR.Partition[1].StartOfPartition[0]=0; 
				PartitionLogicalMBR.Partition[1].SystemFlag=0x05;
				PartitionLogicalMBR.Partition[1].StartSector -= dwStartSec;
				if(!WriteSector(dwTemp,1,(PBYTE)&PartitionLogicalMBR,btHardDisk,&DriveParam))
				{	
					*nCreateErrorCode=ERR_PARMAN_WRITESECTOR;
					return FALSE;
				}
			}
			return TRUE;
		}
	}
	return TRUE;
}

BOOL CaculateStartAndEnd(PARTITION_ENTRY *pEntry,BIOS_DRIVE_PARAM DriveParam,BYTE btHardDisk,DWORD dwFlag)
{
	DWORD dwTemp=0;
	BYTE	btHead=0;
	BYTE	btSector=0;
	DWORD	dwCylinder=0;
	dwTemp = LocatePartition(pEntry->StartSector,btHardDisk,STARTPARTITION);
	if(dwTemp == ERRDRIVEPARAM)
		return FALSE;
	else
		pEntry->StartSector = dwTemp;
	dwTemp = LocatePartition(pEntry->StartSector+pEntry->SectorsInPartition,btHardDisk,ENDPARTITION)-1;
	if(dwTemp == ERRDRIVEPARAM)
		return FALSE;
	pEntry->SectorsInPartition=dwTemp-pEntry->StartSector+1;//-DriveParam.dwSecPerTrack;
	btHead=(BYTE)(((pEntry->StartSector/DriveParam.dwSecPerTrack)%DriveParam.dwHeads)&0xff); 
	btSector = (BYTE)(((pEntry->StartSector % DriveParam.dwSecPerTrack)+1)&0xff);
	dwCylinder=(pEntry->StartSector/DriveParam.dwSecPerTrack)/DriveParam.dwHeads; 
	if(dwCylinder>0x400)//   >7.8G
//		dwCylinder = 1023;
		dwCylinder = dwCylinder%0x400;
	pEntry->StartOfPartition[0]=btHead;
	pEntry->StartOfPartition[1]=(BYTE)(LOWORD(((dwCylinder&0x300)>>2))|(btSector&DriveParam.dwSecPerTrack)); 
	pEntry->StartOfPartition[2]=(BYTE)(LOWORD(dwCylinder&0xffff)); 

	btHead=(BYTE)(((dwTemp/DriveParam.dwSecPerTrack)%DriveParam.dwHeads)&0xff); 
	btSector = (BYTE)(((dwTemp % DriveParam.dwSecPerTrack)+1)&0xff);
	dwCylinder=(dwTemp/DriveParam.dwSecPerTrack)/DriveParam.dwHeads; 
	switch(dwFlag)
	{
	case LOGICAL:
		switch (pEntry->SystemFlag)
		{
		case 0x06:
		case 0x0e:
			pEntry->SystemFlag=0x06;
			break;
		case 0x16:
		case 0x1e:
			pEntry->SystemFlag=0x16;
			break;
		case 0x0b:
		case 0x0c:
			pEntry->SystemFlag=0x0b;
			break;
		case 0x1b:
		case 0x1c:
			pEntry->SystemFlag=0x1b;
		default:
			break;
		}
		break;
	case PRIMARY:
		switch (pEntry->SystemFlag)
		{
		case 0x06:
		case 0x0e:
			if(dwCylinder>=0x400)	pEntry->SystemFlag=0x0e;
			else	pEntry->SystemFlag=0x06;
			break;
		case 0x16:
		case 0x1e:
			if(dwCylinder>=0x400) pEntry->SystemFlag=0x1e;
			else	pEntry->SystemFlag=0x16;
			break;
			case 0x0b:
		case 0x0c:
			if(dwCylinder>=0x400)	pEntry->SystemFlag=0x0c;
			else pEntry->SystemFlag=0x0b;
			break;
		case 0x1b:
		case 0x1c:
			if(dwCylinder>=0x400)	pEntry->SystemFlag=0x1c;
			else	pEntry->SystemFlag=0x1b;
		default:
			break;
		}
		break;
	case EXTEND:
		if (dwCylinder>=0x400) pEntry->SystemFlag=0x0f;
		else pEntry->SystemFlag=0x05;
	default:
		break;
	}

	if(dwCylinder>0x400)//   >7.8G
	{
//		dwCylinder = 1023;
		dwCylinder = dwCylinder%0x400;
	}
	pEntry->EndOfPartition[0]=btHead;
	pEntry->EndOfPartition[1]=(BYTE)(LOWORD(((dwCylinder&0x300)>>2))|(btSector&DriveParam.dwSecPerTrack)); 
	pEntry->EndOfPartition[2]=(BYTE)(LOWORD(dwCylinder&0xffff)); 
	return TRUE;
}
BOOL DoResizeExtendPartition(BYTE btHardDisk,PARTITION_ENTRY *peParEntry,PINT pnErr)
{
	BIOS_DRIVE_PARAM	DriveParam;
	BOOL                bResult = FALSE;
	PARTITION_SEC		PartitionMBR,PartitionLogicalMBR;
	PARINFOONHARDDISKEX	ParHard;
	WORD				nExtend,i;
	DWORD				dwTemp=0;
	*pnErr=0;
	
	if(!GetDriveParam(btHardDisk,&DriveParam))
	{	
		*pnErr=ERR_PARMAN_GETDRIVEPARAM;	
		return FALSE;
	}
	if(peParEntry->SectorsInPartition<DriveParam.dwHeads*DriveParam.dwSecPerTrack)
	{	
		*pnErr=ERR_PARMAN_NUMBEROFSECTORS;
		return FALSE;
	}
	if(peParEntry->StartSector<DriveParam.dwHeads*DriveParam.dwSecPerTrack)
	{
		peParEntry->StartSector+=DriveParam.dwHeads*DriveParam.dwSecPerTrack;
		peParEntry->SectorsInPartition=peParEntry->SectorsInPartition-DriveParam.dwHeads*DriveParam.dwSecPerTrack;
	}
	//caculate the peParEntry's cylinder,head,sector....
	CaculateStartAndEnd(peParEntry,DriveParam,btHardDisk,EXTEND);
	
	_fmemset(&ParHard,0,sizeof(PARINFOONHARDDISK));
	if(!GetPartitionInfoEx(btHardDisk,&ParHard))
	{	
		*pnErr=ERR_PARMAN_GETPARTITIONINFO;
		return FALSE;
	}
	if(!ReadSector(0,1,(PBYTE)&PartitionMBR,btHardDisk,&DriveParam))
	{
		*pnErr=ERR_PARMAN_READSECTOR;
		return FALSE;
	}
	for(i=0;i<4;i++)
	{
		if(PartitionMBR.Partition[i].SystemFlag == 0x05 ||
			PartitionMBR.Partition[i].SystemFlag == 0x0f)
		{
			nExtend = i;
			if(ParHard.wNumOfLogic!=0)
			{
				if(peParEntry->StartSector+peParEntry->SectorsInPartition<ParHard.peLogParInfo[ParHard.wNumOfLogic-1].peCurParInfo.StartSector+ParHard.peLogParInfo[ParHard.wNumOfLogic-1].peCurParInfo.SectorsInPartition)
				{	
					*pnErr=ERR_PARMAN_PARAM;
					return FALSE;
				}
			}
		}
		else
		{
			if (PartitionMBR.Partition[i].SystemFlag && PartitionMBR.Partition[i].SectorsInPartition)
			{
				if((peParEntry->StartSector >= PartitionMBR.Partition[i].StartSector &&    
					peParEntry->StartSector < PartitionMBR.Partition[i].StartSector+PartitionMBR.Partition[i].SectorsInPartition)) 
				{	
					*pnErr=ERR_PARMAN_PARTITIONINPARTITION;
					return FALSE;
				}
				if((peParEntry->StartSector+peParEntry->SectorsInPartition > PartitionMBR.Partition[i].StartSector &&    
					peParEntry->StartSector+peParEntry->SectorsInPartition <= PartitionMBR.Partition[i].StartSector+PartitionMBR.Partition[i].SectorsInPartition)) 
				{
					*pnErr=ERR_PARMAN_PARTITIONINPARTITION;
					return FALSE;
				}
			}
		}			
	}//end check

	_fmemcpy(&PartitionMBR.Partition[nExtend],peParEntry,sizeof(PARTITION_ENTRY));
	if(WriteSector(0,1,(PBYTE)&PartitionMBR,btHardDisk,&DriveParam))
	{	
		dwTemp=PartitionMBR.Partition[nExtend].StartSector;
		_fmemset(&PartitionLogicalMBR,0,sizeof(PARTITION_SEC));
		PartitionLogicalMBR.Signature = 0xAA55;
		WriteSector(dwTemp,1,(PBYTE)&PartitionLogicalMBR,btHardDisk,&DriveParam);
		for (i = 0;i < ParHard.wNumOfLogic;i++)
		{
			if (i)
			{
				_fmemcpy(&PartitionLogicalMBR.Partition[0],&ParHard.peLogParInfo[i-1].peCurParInfo,sizeof(PARTITION_ENTRY));
				_fmemcpy(&PartitionLogicalMBR.Partition[1],&ParHard.peLogParInfo[i].peCurParInfo,sizeof(PARTITION_ENTRY));
				PartitionLogicalMBR.Partition[0].StartSector = ParHard.peLogParInfo[i-1].dwCurOffset;
				PartitionLogicalMBR.Partition[1].SectorsInPartition += ParHard.peLogParInfo[i].dwCurOffset;
			}
			else
			{
				if (PartitionMBR.Partition[nExtend].StartSector == ParHard.peLogParInfo[i].peCurParInfo.StartSector)
				{
					_fmemcpy(&PartitionLogicalMBR.Partition[0],&ParHard.peLogParInfo[i].peCurParInfo,sizeof(PARTITION_ENTRY));
					_fmemcpy(&PartitionLogicalMBR.Partition[1],&ParHard.peLogParInfo[i+1].peCurParInfo,sizeof(PARTITION_ENTRY));
					PartitionLogicalMBR.Partition[0].StartSector = ParHard.peLogParInfo[i].dwCurOffset;
					PartitionLogicalMBR.Partition[1].SectorsInPartition += ParHard.peLogParInfo[i+1].dwCurOffset;
				}
				else
				{
					_fmemcpy(&PartitionLogicalMBR.Partition[1],&ParHard.peLogParInfo[i].peCurParInfo,sizeof(PARTITION_ENTRY));
					PartitionLogicalMBR.Partition[1].SectorsInPartition += ParHard.peLogParInfo[i].dwCurOffset;
				}
			}
			PartitionLogicalMBR.Partition[1].StartSector -= PartitionMBR.Partition[nExtend].StartSector;
			PartitionLogicalMBR.Partition[1].StartOfPartition[0] = 0;
			if (PartitionLogicalMBR.Partition[1].SystemFlag) 
				PartitionLogicalMBR.Partition[1].SystemFlag = 0x05;
			if(!WriteSector(dwTemp,1,(PBYTE)&PartitionLogicalMBR,btHardDisk,&DriveParam))
			{	
				*pnErr=ERR_PARMAN_WRITESECTOR;
				return FALSE;
			}
			dwTemp = ParHard.peLogParInfo[i].peCurParInfo.StartSector;
		}
		bResult = TRUE;
	}
	else *pnErr=ERR_PARMAN_WRITESECTOR;
	return bResult;
}

/*
BOOL MakePartition(DWORD dwSourceStart,DWORD dwSourceSize,DWORD dwSystemFlag,DWORD dwTargetStart,DWORD dwTargetSize,DWORD dwHardDisk)
{
    PARINFOONHARDDISKEX	     ParHard;    
    BIOS_DRIVE_PARAM         DriveParam;
	PARTITION_ENTRY pe[MAX_PAR_NUM], peTemp;//max drive
	CREATE_PAR_FLAG cpf={1,0};
	DWORD i=0,j=0,k=0,nCount,nExtend;
	DWORD dwSize,dwPreSize,dwNextSize;
	DWORD dwDelStartSec,dwLogOrPri;
	BOOL blSourceThanTarget=1;
	INT Err;                            
	if(!GetDriveParam((BYTE)dwHardDisk+0x80,&DriveParam))   return 0;

	if((dwSourceSize==dwTargetSize+63) || (dwTargetSize==dwSourceSize+63) || (dwSourceSize==dwTargetSize))
		return 1;
	if(dwSourceSize>dwTargetSize)//>DriveParam.dwSecPerTrack)
		dwSize=dwSourceSize-dwTargetSize;
	if(dwTargetSize>dwSourceSize)//>DriveParam.dwSecPerTrack)
	{	
		dwSize=dwTargetSize-dwSourceSize;
		blSourceThanTarget=0;
	}
	_fmemset(pe,0,sizeof(PARTITION_ENTRY)*MAX_PAR_NUM);
	_fmemset(&ParHard,0,sizeof(PARINFOONHARDDISKEX));
	dwHardDisk+=0x80;
	if(!GetPartitionInfoEx(dwHardDisk+0x80,&ParHard))
		return FALSE;
	while(ParHard.pePriParInfo[i].SystemFlag)
	{
		if(ParHard.pePriParInfo[i].SystemFlag!=0x05 && 
			ParHard.pePriParInfo[i].SystemFlag!=0x0f && ParHard.pePriParInfo[i].SystemFlag!=0)
		{	
			_fmemcpy(&pe[j],&ParHard.pePriParInfo[i],sizeof(PARTITION_ENTRY));
			j++;
		}
		if(ParHard.pePriParInfo[i].SystemFlag==0x05 ||
			ParHard.pePriParInfo[i].SystemFlag==0x0f )
			nExtend=i;
		i++;
	}
	for(i=0;i<ParHard.wNumOfLogic;i++)
	{
		_fmemcpy(&pe[j],&ParHard.peLogParInfo[i].peCurParInfo,sizeof(PARTITION_ENTRY));
		pe[j].StartSector+=DriveParam.dwSecPerTrack;
		pe[j].BootFlag=LOGICAL; 
		j++;
	}
	for(i=0;i<j;i++)
	{
		nCount=i;
		for(k=i+1;k<j;k++)
		{
			if(pe[k].StartSector<pe[nCount].StartSector)
				nCount=k;
		}
		_fmemcpy(&peTemp,&pe[nCount],sizeof(PARTITION_ENTRY));
		_fmemcpy(&pe[nCount],&pe[i],sizeof(PARTITION_ENTRY));
		_fmemcpy(&pe[i],&peTemp,sizeof(PARTITION_ENTRY));
	}
	_fmemset(&peTemp,0,sizeof(PARTITION_ENTRY));
	for(i=0;i<j;i++)
	{
		if(pe[i].StartSector==dwTargetStart)
			break;
	}
	if(blSourceThanTarget)//it's "enlarge target partition"
	{
		if(pe[i].BootFlag==LOGICAL)
		{
			dwLogOrPri=LOGICAL;
			dwDelStartSec=pe[i].StartSector-DriveParam.dwSecPerTrack;
			peTemp.SectorsInPartition=dwSourceSize; 
			peTemp.SystemFlag=dwSystemFlag;  
			dwPreSize=0;
			if(!i || pe[i-1].BootFlag!=LOGICAL)
				dwPreSize=pe[i].StartSector-DriveParam.dwSecPerTrack-ParHard.pePriParInfo[nExtend].StartSector;
			else
				dwPreSize=pe[i].StartSector-DriveParam.dwSecPerTrack-(pe[i-1].StartSector+pe[i-1].SectorsInPartition);
			if(dwPreSize>dwSize)
			{	
				peTemp.StartSector=pe[i].StartSector-DriveParam.dwSecPerTrack-dwSize;
				goto End_Handler;
			}
			else
			{
				if(pe[i+1].BootFlag!=LOGICAL)
					dwNextSize=ParHard.pePriParInfo[nExtend].StartSector+ParHard.pePriParInfo[nExtend].SectorsInPartition-(pe[i].StartSector+pe[i].SectorsInPartition);
				else
					dwNextSize=pe[i+1].StartSector-(pe[i].StartSector+pe[i].SectorsInPartition);
				if(dwNextSize+dwPreSize>dwSize)
					peTemp.StartSector=ParHard.pePriParInfo[nExtend].StartSector;
				else
					return 0;
			}
		}	
		else
		{//It's Primary partition
			dwLogOrPri=PRIMARY;
			dwDelStartSec=pe[i].StartSector;
			peTemp.SectorsInPartition=dwSourceSize; 
			peTemp.SystemFlag=dwSystemFlag;  
			dwPreSize=0;
			if(i==0)//it's the first primary partition
				dwPreSize=pe[0].StartSector-DriveParam.dwSecPerTrack; 			
			else
			{	
				if(pe[i-1].BootFlag!=LOGICAL)
					dwPreSize=pe[i].StartSector-(pe[i-1].StartSector+pe[i-1].SectorsInPartition); 
				else
					dwPreSize=pe[i].StartSector-(ParHard.pePriParInfo[nExtend].StartSector+ParHard.pePriParInfo[nExtend].SectorsInPartition); 
			}
			if(dwPreSize>dwSize)
			{	
				peTemp.StartSector=pe[i].StartSector-dwSize;
				goto End_Handler;
			}
			else
			{
				if(pe[i+1].BootFlag==LOGICAL) 
					dwNextSize=ParHard.pePriParInfo[nExtend].StartSector-(pe[i].StartSector+pe[i].SectorsInPartition);
				else
					dwNextSize=pe[i+1].StartSector-(pe[i].StartSector+pe[i].SectorsInPartition);
				if(dwNextSize+dwPreSize>dwSize)
					peTemp.StartSector=pe[i].StartSector-dwPreSize;
				else
					return 0;
			}
		}
	}
	else
	{
		_fmemset(&peTemp,0,sizeof(PARTITION_ENTRY));
		peTemp.SectorsInPartition=dwSourceSize; 
		peTemp.SystemFlag=dwSystemFlag;  
		if(pe[i].BootFlag==LOGICAL)
		{
			peTemp.StartSector=pe[i].StartSector-DriveParam.dwSecPerTrack;
			dwDelStartSec=pe[i].StartSector-DriveParam.dwSecPerTrack;
			dwLogOrPri=LOGICAL;
		}
		else
		{
			peTemp.StartSector=pe[i].StartSector;
			dwDelStartSec=pe[i].StartSector;
			dwLogOrPri=PRIMARY;
		}
	}
End_Handler:
	if(!DoDeletePartition(dwDelStartSec,(BYTE)dwHardDisk,cpf,&Err))
		return 0;
	else
	{	
		if(!DoCreatePartition(&peTemp,(BYTE)dwHardDisk,dwLogOrPri,0,NULL,NULL,&Err))
			return 0;
	}
	return 1;
}
*/
BOOL DoCreatePriPar(PARTITION_ENTRY *peParEntry,BYTE btHardDisk,	BIOS_DRIVE_PARAM *DriveParam)
{	
	PARTITION_SEC mbr;
	int nPartition;
	DWORD dw;

	if(ReadSector(0,1,(PBYTE)&mbr,btHardDisk,DriveParam))
	{
		for(nPartition=0;nPartition<4;nPartition++)
		{
			if(mbr.Partition[nPartition].SystemFlag == 0)// && 
			{
				_fmemcpy(&mbr.Partition[nPartition],peParEntry,sizeof(PARTITION_ENTRY));
				dw=mbr.Partition[nPartition].StartSector ;
				if(WriteSector(0,1,(PBYTE)&mbr,btHardDisk,DriveParam))
				{
					if(peParEntry->SystemFlag==0x05 || peParEntry->SystemFlag==0x0f)
					{
						_fmemset(&mbr,0,sizeof(PARTITION_SEC));
						mbr.Signature=MBRSIGNATURE;
						WriteSector(dw,1,(PBYTE)&mbr,btHardDisk,DriveParam);
					}
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

int VerifyPrimaryPartition(PARINFOONHARDDISKEX *ph, PARTITION_ENTRY *pe)////0 is success,1 is error
{
	WORD i;
	for(i=0;i<ph->wNumOfLogic;i++ )		
	{
		if(pe->StartSector >= ph->peLogParInfo[i].peCurParInfo.StartSector &&
		   pe->StartSector < ph->peLogParInfo[i].peCurParInfo.StartSector+ph->peLogParInfo[i].peCurParInfo.SectorsInPartition) 
			return ERR_PARMAN_PARTITIONINPARTITION;
		if((pe->StartSector+pe->SectorsInPartition > ph->peLogParInfo[i].peCurParInfo.StartSector &&    
			pe->StartSector+pe->SectorsInPartition < ph->peLogParInfo[i].peCurParInfo.StartSector+ph->peLogParInfo[i].peCurParInfo.SectorsInPartition)) 
			return ERR_PARMAN_PARTITIONINPARTITION;
		if(pe->StartSector >= ph->peLogParInfo[i].peCurParInfo.StartSector+ph->peLogParInfo[i].peCurParInfo.SectorsInPartition &&
		   pe->StartSector<ph->peLogParInfo[i].dwNextStart)
		{  //primary partition between the two logical partition
			return ERR_PARMAN_PARTITIONINPARTITION;
		}
	}
	return 0;
}

BOOL GetDriveMapInfo(int nDrive, DRIVE_MAP_INFO  *p)
{
    int HardDrive=0;
    DWORD dwStartSec=0;
    if(!GetStartSector(nDrive,&dwStartSec,&HardDrive))
    {	
    	 p->dmiInt13Unit=0;
		p->dmiPartitionStartRBA[0]=0;
    	return 0;
    }
    p->dmiInt13Unit=HardDrive;
	p->dmiPartitionStartRBA[0]=dwStartSec;
	return 1;
}                     
																								//which is harddisk		
BOOL GetStartSector(int nDrive,DWORD* dwStartSector,int *Drive) //not support floopy
{
	MyDiskIo    DiskIo,*pDiskIo;
	WORD 	    j,i,nHardDisk;
	BYTE        btResult=1;
	BYTE 	    pBuf[512],pBuf1[512];
	WORD        wOff;
	PARINFOONHARDDISKEX	ParHard;
	BIOS_DRIVE_PARAM DriveParam;
	
	_fmemset(pBuf,0,512);
	*Drive=0;
	_fmemset(&DiskIo,0,sizeof(MyDiskIo));

	pDiskIo = &DiskIo;
	wOff=_FP_OFF(pDiskIo);	
	DiskIo.dwStartSector=0;
	DiskIo.wSectors=1;
	DiskIo.dwBuffer=(DWORD)pBuf;
	_asm
	{
		mov ax,7305h
		mov bx,wOff
		mov cx,-1
		mov dx,nDrive
		xor si,si
		int 21h
		mov btResult,ah
	}	

	nHardDisk=GetHardDiskNum();
	for(j=0;j<nHardDisk;j++)
	{
		_fmemset(&DriveParam,0,sizeof(BIOS_DRIVE_PARAM));
		_fmemset(&ParHard,0,sizeof(PARINFOONHARDDISKEX));
		if(!GetDriveParam(0x80+j,&DriveParam)) break;
		if(!GetPartitionInfoEx(0x80+j,&ParHard))	break;
		for(i=0;i<4;i++)
		{
			if(ParHard.pePriParInfo[i].SystemFlag!=5 && ParHard.pePriParInfo[i].SystemFlag!=0x0f)
			{
				if(!ReadSector(ParHard.pePriParInfo[i].StartSector,1,pBuf1,0x80+j,&DriveParam))
						return 0;			
				if(!_fmemcmp(pBuf,pBuf1,512))
				{
			     	*Drive=0x80+j;
			     	*dwStartSector=ParHard.pePriParInfo[i].StartSector;	
				 	return 1;
				}
			}
		}	
		for(i=0;i<ParHard.wNumOfLogic;i++)
		{
			if(!ReadSector(ParHard.peLogParInfo[i].peCurParInfo.StartSector+0x3f,1,pBuf1,0x80+j,&DriveParam))
				return 0;			
			if(!_fmemcmp(pBuf,pBuf1,512))
			{    
			 	*Drive=0x80+j;
			 	*dwStartSector=ParHard.peLogParInfo[i].peCurParInfo.StartSector+0x3f;
				return 1;
			}
		}
	}
	return 0;
}

BOOL GetMID(int nDrive,MID *pMid)
{
	BYTE   btResult=1;
	WORD wOff;
	_fmemset(pMid,0,sizeof(MID));
	wOff=_FP_OFF(pMid);		
	_asm
	{
		mov ax,440dh
		mov bx,nDrive
		mov cx,4866h
		mov dx,wOff
		int    21h
		mov btResult,ah
	}
	return (WORD)!btResult;
}


BOOL IsCDRomDrive (int nDrive)
{
   nDrive--;
   BOOL bResult = FALSE;      // Assume not a CD-ROM drive
   __asm {
         mov  ax, 150Bh       // MSCDEX CD-ROM Drive Check
         xor  bx, bx
         mov  cx, nDrive
         int  2Fh
         cmp  bx, 0ADADh      // Check MSCDEX signature
         jne  not_cd_drive
         or   ax, ax          // Check the drive type
         jz   not_cd_drive    // 0 (zero) means not CD-ROM
         mov  bResult, TRUE
      not_cd_drive:
         }
   return (bResult);
}


BOOL IsFloppyDisk(int nDrive)
{
    BOOL 			 bResult = 0;
    DOSDPB         dpb,*pdpb;
    WORD			 wOff;
    pdpb=&dpb;
    wOff=_FP_OFF(pdpb);		
   dpb.specialFunc = 0;  // return default type; do not hit disk
   _asm
   {
   		mov ax,440dh
   		mov bx,nDrive
		mov cx,0860h
		mov dx,wOff   
   		int 21h
   }  
   switch (dpb.devType)
   {
         case 2: // 3.5  720K floppy
         case 7: // 3.5  1.44MB floppy
         case 9: // 3.5  2.88MB floppy
         bResult = 1; break;
         default: // other
            break;
   }
   return bResult;
}

DWORD GetDriveFreeSpace(char *Drive)
{
	ExtGetDskFreSpcStruc DiskFree,*pDiskFree;
	WORD wOffDrive,wOff,wSeg,wSize;
	
	_fmemset(&DiskFree,0,sizeof(ExtGetDskFreSpcStruc));
	DiskFree.ExtFree_Size=sizeof(ExtGetDskFreSpcStruc);
	wOffDrive=_FP_OFF(Drive);      
	pDiskFree =  &DiskFree;
	wOff=_FP_OFF(pDiskFree);
	wSeg=_FP_SEG(pDiskFree);
	wSize=sizeof(ExtGetDskFreSpcStruc);
	_asm
	{
		push es
		mov  dx,wSeg
		mov  es,dx
		mov  di,wOff 

		mov  cx, wSize       		
		mov  dx, wOffDrive
		mov  ax, 7303h            ;Get_ExtFreeSpace
		int  21h
		pop es
	}
	return DiskFree.ExtFree_AvailablePhysSectors;	
}

//it's from restart.com 
void RestartComputer()
{
	_asm
	{
		mov	ax,40h
		push	ax
		pop	ds
		mov	word ptr ds:[72h],1234h
		mov	ax,0FFFFh
		push	ax
		mov	ax,0
		push	ax
		retf			
	}
	//return 1;
}

/*BOOL OpenIMGFile(char *FileName)
{
	BOOL	bResult = FALSE;
	WORD 	wSeg,wOff;

	wOff=_FP_OFF(FileName);
	wSeg=_FP_SEG(FileName);
	_asm
	{
		mov ax, 716Ch
		mov bx, 0000h
		mov cx, 0
		mov dx, 0001h
		mov si, wSeg
		mov ds, si
		mov si, wOff
		mov di, 0400h
		int 21h
		
		jc error
//		mov [Handle], ax
//		mov [ActionTaken], cx
		mov bResult,1
error:
	}
	return bResult;	
}
*/