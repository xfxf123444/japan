#include "comm.h"
#include "main.h"
#include "intrrpt.h"

extern		int							g_nDiskNum;
extern		char		   				g_drives[27];
extern		YG_PARTITION_INFO			*g_pCurFixDiskInfo;
extern		YG_PARTITION_INFO			*g_pDiskInfo;
extern		PARMAPINFO					*g_pParMapInfo;
extern		double						g_dDeletedSize;
extern		double						g_dTotalSize;
extern		int							g_nMethod;
extern  	BYTE						g_btCurFun;
extern  	struct tm					g_StartTime;
extern  	struct tm					g_EndTime;
extern 		ATA_DEVICE_TYPE				g_DevType[4];
extern 		ATA_IDENTIFY_INFO 			AtaIdentifyInfo;
extern      int							g_nTotalDrive;
extern      char                        g_chLogDrive;
extern	_deleteparinfo					g_delparinfo;
extern	_deletediskinfo					g_deldiskinfo;

extern void (_interrupt far *PrevTimerHandler)();
extern void (_interrupt far *PrevIntHandlerPri)();
extern void (_interrupt far *PrevIntHandlerSec)();

BOOL Getallparmapinfo()
{    
	BOOL            bFindFloppy = FALSE,bFloppy;
	int				i, j = 0;                
	PARMAPINFO		*pNew = NULL;     
	DRIVE_MAP_INFO  drvinfo;

	g_nTotalDrive = 0;
    for(i='A';i<='Z';i++)
    {   
		if(!_chdrive(i-'A'+1))
       	{
			g_nTotalDrive ++;
			g_drives[i-'A'] = i;

			bFloppy = IsFloppyDisk(i-'A'+1);

			if (!bFindFloppy)
			{
				g_chLogDrive = i;
	    		bFindFloppy = bFloppy;
			}
			
			if (!bFloppy)
			{
				_fmemset(&drvinfo,0,sizeof(DRIVE_MAP_INFO));
				if (GetDriveMapInfo(i - 'A' + 1, &drvinfo))
				{
					pNew = (PARMAPINFO*)malloc(sizeof(PARMAPINFO));	   
					pNew->cDrv=i;
					pNew->dmiInt13Unit = drvinfo.dmiInt13Unit;
					pNew->dmiPartitionStartRBA = drvinfo.dmiPartitionStartRBA[0];
				
					pNew->pNext = g_pParMapInfo;
					g_pParMapInfo = pNew;
				}
			}
		}
		else
		{
			g_drives[i-'A'] = 0;
		}
	}   
	return TRUE;
}

void SortParInfo(PARINFOONHARDDISKEX *ParInfo)
{
	WORD	m,n;
	PARINFOONHARDDISKEX DiskIfTemp;

	//sort Primary item
	for(m=0;m<MBR_ITEM;m++)
	{
		if(ParInfo->pePriParInfo[m].SystemFlag == 0)
			break;
		for(n=m;n<MBR_ITEM;n++)
		{
			if(ParInfo->pePriParInfo[n].SystemFlag == 0)
				break;
			if(ParInfo->pePriParInfo[m].StartSector
				> ParInfo->pePriParInfo[n].StartSector)
			{
				DiskIfTemp.pePriParInfo[m]   = ParInfo->pePriParInfo[m];
				ParInfo->pePriParInfo[m]	 = ParInfo->pePriParInfo[n];
				ParInfo->pePriParInfo[n]	 = DiskIfTemp.pePriParInfo[m];
			}
		}
	}
	//sort logical partition
	for(m=0;m<ParInfo->wNumOfLogic;m++)
	{
		if(ParInfo->peLogParInfo[m].peCurParInfo.SystemFlag == 0)
			break;
		for(n=m;n<ParInfo->wNumOfLogic;n++)
		{
			if(ParInfo->peLogParInfo[n].peCurParInfo.SystemFlag == 0)
				break;
			if(ParInfo->peLogParInfo[m].peCurParInfo.StartSector
				> ParInfo->peLogParInfo[n].peCurParInfo.StartSector)
			{
				DiskIfTemp.peLogParInfo[m]	= ParInfo->peLogParInfo[m];
				ParInfo->peLogParInfo[m]	= ParInfo->peLogParInfo[n];
				ParInfo->peLogParInfo[n]	= DiskIfTemp.peLogParInfo[m];
			}
					
		}
	}
}

DWORD   MapszLettersToParts(YG_PARTITION_INFO* pNew,int *kk)
{           
	PARMAPINFO	*p = g_pParMapInfo;     
	char drive[]=" :\\";
	while(p != NULL)		
	{
		if((p->dmiInt13Unit == (pNew->btDiskNum + DISK_BASE))
		 	&&(
		    	(p->dmiPartitionStartRBA - 0x3f == pNew->dwStartSector) ||
		    	(p->dmiPartitionStartRBA == pNew->dwStartSector)
		      )
		  	)
		{//have drive letter
            pNew->DriveLetter = p->cDrv;
            drive[0]=p->cDrv;
            return GetDriveFreeSpace(drive);
        }  
        p = p->pNext;
	}       
	//the partition no drive letter be mapped
    pNew->DriveLetter = *kk;
    *kk=(*kk)+1;
    return   0 ;
}

BOOL GetFixDiskInfo(int nDisk)
{
	PARINFOONHARDDISKEX  info;
	WORD				 j=0;
	WORD				 n;
	int					 nCount=0,k;
	YG_PARTITION_INFO	 *pNew, *pEnd;
	
	BIOS_DRIVE_PARAM	 DriveParam;
	DWORD				 dwPartitionTop;
	DWORD				 dwLogPartitionTop;
	
	_fmemset(&info,0,sizeof(PARINFOONHARDDISKEX));
	if(!GetDriveParam(DISK_BASE + nDisk,&DriveParam)) return FALSE;

	if(!GetPartitionInfoEx(DISK_BASE + nDisk, &info))
		return FALSE; 
	//g_PriNum[i] = info.wNumOfPri;
	SortParInfo(&info);

    j		=0;
	k		=0;
	dwPartitionTop = DISK_MIN_SECTOR;
	while(info.pePriParInfo[j].SystemFlag != 0x00 && j < MBR_ITEM)
	{
		if(info.pePriParInfo[j].StartSector > dwPartitionTop)
		{
			pNew = (YG_PARTITION_INFO*)malloc(sizeof(YG_PARTITION_INFO));
			_fmemset(pNew,0,sizeof(YG_PARTITION_INFO));
			pNew->BootFlag		 = 0;
			pNew->bLogic		 = FALSE;
			pNew->btDiskNum		 = (BYTE)nDisk;
			pNew->dwPartSize	 = info.pePriParInfo[j].StartSector - dwPartitionTop;
			pNew->dwStartSector  = dwPartitionTop;
			pNew->dwSystemFlag	 = 0;
			pNew->pNext			 = NULL;
			pNew->dwUsedSize	 = 0;
			pNew->DriveLetter	 = (char) k++;
			if(g_pCurFixDiskInfo == NULL) g_pCurFixDiskInfo = pNew;
			else
			{
				pEnd = g_pCurFixDiskInfo;
				while(pEnd->pNext != NULL) pEnd = pEnd->pNext;
				pEnd->pNext = pNew;
			}
			dwPartitionTop = info.pePriParInfo[j].StartSector;
		}
		dwPartitionTop += info.pePriParInfo[j].SectorsInPartition; 
		if(info.pePriParInfo[j].SystemFlag == 0x05 || info.pePriParInfo[j].SystemFlag == 0x0f)
		{
			n = 0;
			dwLogPartitionTop = info.pePriParInfo[j].StartSector;
			while(info.peLogParInfo[n].peCurParInfo.SystemFlag != 0x00 && n< info.wNumOfLogic)
			{
				if(info.peLogParInfo[n].peCurParInfo.StartSector > dwLogPartitionTop)
				{
					pNew = (YG_PARTITION_INFO*)malloc(sizeof(YG_PARTITION_INFO));
					_fmemset(pNew,0,sizeof(YG_PARTITION_INFO));
					pNew->BootFlag		= 0;
					pNew->bLogic		= TRUE;
					pNew->btDiskNum		= (BYTE)nDisk;
					pNew->dwPartSize	= info.peLogParInfo[n].peCurParInfo.StartSector - 
											dwLogPartitionTop;
					pNew->dwStartSector = dwLogPartitionTop;
					pNew->dwSystemFlag  = 0;
					pNew->pNext			= NULL;
					pNew->dwUsedSize	= 0;
					pNew->DriveLetter	= (char)k++;
					if(g_pCurFixDiskInfo == NULL) g_pCurFixDiskInfo = pNew;
					else
					{
						pEnd = g_pCurFixDiskInfo;
						while(pEnd->pNext != NULL) pEnd = pEnd->pNext;
						pEnd->pNext = pNew;
					}
					pNew = (YG_PARTITION_INFO*)malloc(sizeof(YG_PARTITION_INFO));
					_fmemset(pNew,0,sizeof(YG_PARTITION_INFO));
					pNew->BootFlag		= info.peLogParInfo[n].peCurParInfo.BootFlag;
					pNew->bLogic		= TRUE;
					pNew->btDiskNum		= (BYTE)nDisk;
					pNew->dwPartSize	= info.peLogParInfo[n].peCurParInfo.SectorsInPartition;
					pNew->dwStartSector = info.peLogParInfo[n].peCurParInfo.StartSector;
					pNew->dwSystemFlag	= info.peLogParInfo[n].peCurParInfo.SystemFlag;
					pNew->pNext			= NULL;
					pNew->dwUsedSize	= MapszLettersToParts(pNew,&k);
					if(g_pCurFixDiskInfo == NULL) g_pCurFixDiskInfo = pNew;
					else
					{
						pEnd = g_pCurFixDiskInfo;
						while(pEnd->pNext != NULL) pEnd = pEnd->pNext;
						pEnd->pNext = pNew;
					}
						
					dwLogPartitionTop = info.peLogParInfo[n].peCurParInfo.StartSector;
				}
				else
				{
					pNew = (YG_PARTITION_INFO*)malloc(sizeof(YG_PARTITION_INFO));
					_fmemset(pNew,0,sizeof(YG_PARTITION_INFO));
					pNew->BootFlag		= info.peLogParInfo[n].peCurParInfo.BootFlag;
					pNew->bLogic		= TRUE;
					pNew->btDiskNum		= (BYTE)nDisk;
					pNew->dwPartSize	= info.peLogParInfo[n].peCurParInfo.SectorsInPartition;
					pNew->dwStartSector = info.peLogParInfo[n].peCurParInfo.StartSector;
					pNew->dwSystemFlag	= info.peLogParInfo[n].peCurParInfo.SystemFlag;
					pNew->pNext = NULL;
					pNew->dwUsedSize	= MapszLettersToParts(pNew,&k);
					if(g_pCurFixDiskInfo == NULL) g_pCurFixDiskInfo = pNew;
					else
					{
						pEnd = g_pCurFixDiskInfo;
						while(pEnd->pNext != NULL) pEnd = pEnd->pNext;
						pEnd->pNext = pNew;
					}
				}
				dwLogPartitionTop += info.peLogParInfo[n].peCurParInfo.SectorsInPartition+DISK_MIN_SECTOR; 
				n++;
			}
			if(dwLogPartitionTop != (info.pePriParInfo[j].SectorsInPartition+
				info.pePriParInfo[j].StartSector))
			{
				pNew = (YG_PARTITION_INFO*)malloc(sizeof(YG_PARTITION_INFO));
				_fmemset(pNew,0,sizeof(YG_PARTITION_INFO));
				pNew->BootFlag		= 0;
				pNew->bLogic		= TRUE;
				pNew->btDiskNum		= (BYTE)nDisk;
				pNew->dwPartSize	= info.pePriParInfo[j].SectorsInPartition+
									  info.pePriParInfo[j].StartSector-dwLogPartitionTop;
				pNew->dwStartSector = dwLogPartitionTop;
				pNew->dwSystemFlag	= 0;
				pNew->pNext			= NULL;
				pNew->dwUsedSize	= 0;
				pNew->DriveLetter	= (char)k++;
				if(g_pCurFixDiskInfo == NULL) g_pCurFixDiskInfo = pNew;
				else
				{
					pEnd = g_pCurFixDiskInfo;
					while(pEnd->pNext != NULL) pEnd = pEnd->pNext;
					pEnd->pNext = pNew;
				}
			}
		}
		else
		{
			pNew = (YG_PARTITION_INFO*)malloc(sizeof(YG_PARTITION_INFO));
			_fmemset(pNew,0,sizeof(YG_PARTITION_INFO));
			pNew->BootFlag		= info.pePriParInfo[j].BootFlag;
			pNew->bLogic		= FALSE;
			pNew->btDiskNum		= (BYTE)nDisk;
			pNew->dwPartSize	= info.pePriParInfo[j].SectorsInPartition;
			pNew->dwStartSector = info.pePriParInfo[j].StartSector;
			pNew->dwSystemFlag	= info.pePriParInfo[j].SystemFlag;
			pNew->pNext			= NULL;
			pNew->dwUsedSize	= MapszLettersToParts(pNew,&k);
			if(g_pCurFixDiskInfo == NULL) g_pCurFixDiskInfo = pNew;
			else
			{
				pEnd = g_pCurFixDiskInfo;
				while(pEnd->pNext != NULL) pEnd = pEnd->pNext;
				pEnd->pNext = pNew;
			}
		}
		j++;
	}
	if(dwPartitionTop < DriveParam.dwSectorL)
	{
		pNew = (YG_PARTITION_INFO*)malloc(sizeof(YG_PARTITION_INFO));
		_fmemset(pNew,0,sizeof(YG_PARTITION_INFO));
		pNew->BootFlag		= 0;
		pNew->bLogic		= FALSE;
		pNew->btDiskNum		= (BYTE)nDisk;
		pNew->dwPartSize	= (DWORD)DriveParam.dwSectorL-dwPartitionTop;
		pNew->dwStartSector = dwPartitionTop;
		pNew->dwSystemFlag	= 0;
		pNew->pNext			= NULL;
		pNew->dwUsedSize	= 0;
		pNew->DriveLetter	= (char) k++;
		if(g_pCurFixDiskInfo == NULL) g_pCurFixDiskInfo = pNew;
		else
		{
			pEnd = g_pCurFixDiskInfo;
			while(pEnd->pNext != NULL) pEnd = pEnd->pNext;
			pEnd->pNext = pNew;
		}
	}
	return TRUE;
}

void writedriveletter(cwindow *pwin0, cwindow *pwin1)
{                                                  
	char				str[65],strX[35],strPriOrLog[10],strActive[10],szfat[9];
	YG_PARTITION_INFO	*p;
	BYTE				btitem = 0;
	MID 				mid;
	//find disk number	                                             
    p = g_pCurFixDiskInfo;
	while(p != NULL)
	{
		g_pCurFixDiskInfo = g_pCurFixDiskInfo->pNext;
		free(g_pCurFixDiskInfo);
		p = g_pCurFixDiskInfo;	
	}

	GetFixDiskInfo(pwin0->m_pitemactive->btItem);

	p = g_pCurFixDiskInfo;
    pwin1->emptyitem();
    pwin1->emptyscreen();

	//2004.08.02 for debug begin

#ifdef _DEBUG
	// ShowMessage("writedriveletter");
#endif

	//2004.08.02 for debug end

	// 2004.08.02 huapage added begin
   	pwin1->additemNotSort(" < > All Partitions",btitem);
	btitem++;
	// 2004.08.02 huapage added end

	while(p != NULL)
	{              
		_fmemset(str,0x20,65);
		_fstrcpy(str, SELPREFIX);
		if(p->dwSystemFlag)
		{      
			if((int)p->DriveLetter< 0x40 )    
			{
				_fmemset(szfat,32,9);
				_fstrcat(str, "    ");
				str[SELPREFIXLEN]='*'; 
	    		str[9]= 0x20;
	    		switch(p->dwSystemFlag)
				{
					case 0x0b:
					case 0x0c:
						_fmemcpy(szfat,"FAT32",5);
						break;
					case 0x01:
					case 0x04:
					case 0x06:
					case 0x0e:
						_fmemcpy(szfat, "FAT",3); 
						break;
					case 0x1b: 
					case 0x1c:
						_fmemcpy(szfat, "H-FAT32",7); 
						break;
					case 0x11:
					case 0x14:
					case 0x16:
					case 0x1e:
						_fmemcpy(szfat, "H-FAT",5); 
						break;
					case 0x17:
						_fmemcpy(szfat, "H-NTFS",6); 
						break;
					case 0x07:
						_fmemcpy(szfat, "NTFS",4); 
						break;			
					case 0x82:
						 _fmemcpy(szfat, "LinSwap",7); 
						 break;
					case 0x83:
						 _fmemcpy(szfat, "LinExt2",7); 
						 break;
					default:
					    if(!p->dwSystemFlag)
							_fmemcpy(szfat,"FREE",4);
						else
						    sprintf(szfat,UNKNOWNFILESYSTEM,p->dwSystemFlag);
						break;
				}                                            	
	  			for(int i=0;i<9;i++)
	  				if(szfat[i]==0) szfat[i]=32;
	            
	 			_fmemcpy(&str[19],szfat,8);
	  			str[27]=0;
	    		if(p->bLogic)
	    			_fstrcpy(strPriOrLog,"LOGICAL");
	    		else
	  	  			_fstrcpy(strPriOrLog,"PRIMARY");
			    _fstrcpy(strActive,"   -    ");
	    		sprintf(strX,PAR,p->dwPartSize/2048,strActive,strPriOrLog);	
				_fstrcat(str,strX);  
			}
			else
			{          
				_fmemset(&mid,0x20,sizeof(MID));
				GetMID(p->DriveLetter-'A'+1,&mid);
				_fstrcat(str, " :\\");
	    		str[SELPREFIXLEN] = p->DriveLetter;
	    		for(int i=0;i<11;i++)
	    		{
	    			if(mid.midVolLabel[i]==0)
						mid.midVolLabel[i]=0x20;    				
	    		}
	    		_fmemcpy(&str[SELPREFIXLEN+3],mid.midVolLabel,11);
	    		switch(p->dwSystemFlag)
				{
					case 0x0b:
					case 0x0c:
						_fmemcpy(&str[19],"FAT32   ",8);
						break;
					case 0x01:
					case 0x04:
					case 0x06:
					case 0x0e:
						_fmemcpy(&str[19],"FAT     ",8);
						break;
					default:
						break;
				}
				str[27]=0;
	    		if(p->bLogic)
	    			_fstrcpy(strPriOrLog,"LOGICAL");
	    		else
	  				_fstrcpy(strPriOrLog,"PRIMARY");
	    		
	    		if(p->BootFlag==0x80)  _fstrcpy(strActive,"Active");
	    		else  _fstrcpy(strActive,"   -    ");
	    		if(p->BootFlag!=0x80)
	    			sprintf(strX,PAR,p->dwPartSize/2048,strActive,strPriOrLog);
				else
					sprintf(strX,PAR1,p->dwPartSize/2048,strActive,strPriOrLog);
				_fstrcat(str,strX);  
	    	}
	    	pwin1->additemNotSort(str,btitem);
	    }
    	p = p->pNext;
    	btitem ++;
    }                  
    pwin1->enabledscroll();      
}

BOOL ShowSectorInfo(cwindow *pwin,DWORD dwSec,int nSelDisk,BIOS_DRIVE_PARAM *pDriveParam,int *nErr)
{
	int			i,j;
	int			nBufOffset = 0;
	BYTE		btSecBuf[SECTOR_SIZE];
	char		szitem[80];
	char		szch;
		
    pwin->emptyitem();
    pwin->emptyscreen();

 	if(!ReadSector(dwSec,1,btSecBuf,nSelDisk,pDriveParam))
 	{
 		*nErr = 1;
 		return FALSE;
 	}
 	for(i=0;i<32;i++)
 	{
		sprintf(szitem, SECTORINFO, nBufOffset,
				btSecBuf[nBufOffset],btSecBuf[nBufOffset+1],
 				btSecBuf[nBufOffset+2],btSecBuf[nBufOffset+3],
 				btSecBuf[nBufOffset+4],btSecBuf[nBufOffset+5],
 				btSecBuf[nBufOffset+6],btSecBuf[nBufOffset+7],
 				btSecBuf[nBufOffset+8],btSecBuf[nBufOffset+9],
 				btSecBuf[nBufOffset+10],btSecBuf[nBufOffset+11],
 				btSecBuf[nBufOffset+12],btSecBuf[nBufOffset+13],
 				btSecBuf[nBufOffset+14],btSecBuf[nBufOffset+15]);
 		for(j=0;j<16;j++)
 		{
 			szch = btSecBuf[nBufOffset+j];
 			if(	
 				
 				szch == 0x0a || 
 				szch == 0x0d
 				
			  )
 			{
 				//szch = szch;
 				szch = 0x00;
 			}
 			if(szch == 0) szch = 0x2e;
 			sprintf(szitem,"%s%c",szitem,szch);
 		}
 		pwin->additem(szitem);
 		nBufOffset += 16;
 	}
	return		TRUE;
}

BOOL DoDeleteSectors(DWORD dwStart, DWORD dwSize ,BYTE	btDisk)
{
	BYTE		*pBuf; 
	int			i;
	BOOL		bVerify = FALSE;
    
    pBuf = NULL;
	pBuf = (BYTE *) _fmalloc(DATA_BUFFER_SIZE);
	if(!pBuf)
	{
		ErrorMessageBox(NO_MEMORY);
		return FALSE;
	}
	switch(g_nMethod)
	{
	case 0:
		g_dTotalSize = dwSize;
		InitDelBuf(pBuf,0,0,0,FALSE,TRUE);
		bVerify = TRUE;
		WriteSecsWithBuf(dwStart,dwSize,btDisk,pBuf,bVerify);
		break;
	case 1:
		g_dTotalSize = dwSize;
		InitDelBuf(pBuf,0xFF,0,0,FALSE,TRUE);
		bVerify = TRUE;
		WriteSecsWithBuf(dwStart,dwSize,btDisk,pBuf,bVerify);
		break;
	case 2:
		g_dTotalSize = dwSize;
		InitDelBuf(pBuf,0,0,0,TRUE,TRUE);
		bVerify = TRUE;
		WriteSecsWithBuf(dwStart,dwSize,btDisk,pBuf,bVerify);
    	break;
    case 3:
		g_dTotalSize = (double)dwSize*3;
		for(i=0;i<3;i++)
		{
			switch(i)
			{
			case 0:
			case 1:
				InitDelBuf(pBuf,0,0,0,TRUE,TRUE);
				break;
			case 2:
				bVerify = TRUE;
				InitDelBuf(pBuf,0,0,0,FALSE,TRUE);
				break;
			}
			if(!WriteSecsWithBuf(dwStart,dwSize,btDisk,pBuf,bVerify))
				break;
		}
		break;
	case 4:
		g_dTotalSize = (double)dwSize*4;
		for(i=0;i<4;i++)
		{
			switch(i)
			{
			case 0:
			case 2:
				InitDelBuf(pBuf,0,0,0,FALSE,TRUE);
				break;
			case 1:
				InitDelBuf(pBuf,0XFF,0,0,FALSE,TRUE);
				break;
			case 3:
				bVerify = TRUE;
				InitDelBuf(pBuf,0XFF,0,0,FALSE,TRUE);
				break;
			}
			if(!WriteSecsWithBuf(dwStart,dwSize,btDisk,pBuf,bVerify))
				break;
		}
		break;
	case 5:
		g_dTotalSize = (double)dwSize*3;
		for(i=0;i<3;i++)
		{
			switch(i)
			{
			case 0:
				InitDelBuf(pBuf,0,0,0,FALSE,TRUE);
				break;
			case 1:
				InitDelBuf(pBuf,0XFF,0,0,FALSE,TRUE);
				break;
			case 2:
				bVerify = TRUE;
				InitDelBuf(pBuf,0X35,0XCA,0X97,FALSE,FALSE);
				break;
			}
			if(!WriteSecsWithBuf(dwStart,dwSize,btDisk,pBuf,bVerify))
				break;
		}
		break;
	case 6:
		g_dTotalSize = (double)dwSize*7;
		for(i=0;i<7;i++)
		{
			switch(i)
			{
			case 0:
			case 2:
			case 4:
				InitDelBuf(pBuf,0,0,0,FALSE,TRUE);
				break;
			case 1:
			case 3:
			case 5:
				InitDelBuf(pBuf,0XFF,0,0,FALSE,TRUE);
				break;
			case 6:
				bVerify = TRUE;
				InitDelBuf(pBuf,0X35,0XCA,0X97,FALSE,FALSE);
				break;
			}
			if(!WriteSecsWithBuf(dwStart,dwSize,btDisk,pBuf,bVerify))
				break;
		}
		break;
	case 7:
		g_dTotalSize = (double)dwSize*35;
		for(i=0;i<35;i++)
		{
			switch(i)
			{
			case 0:
			case 1:
			case 2:
			case 3:
			case 31:
			case 32:
			case 33:
				InitDelBuf(pBuf,0,0,0,TRUE,TRUE);
				break;
			case 34:
				bVerify = TRUE;
				InitDelBuf(pBuf,0,0,0,TRUE,TRUE);
				break;
			case 4:
				InitDelBuf(pBuf,0X55,0,0,FALSE,TRUE);
				break;
			case 5:
				InitDelBuf(pBuf,0XAA,0,0,FALSE,TRUE);
				break;
			case 6:
			case 25:
				InitDelBuf(pBuf,0X92,0X49,0X24,FALSE,FALSE);
				break;
			case 7:
			case 26:
				InitDelBuf(pBuf,0X49,0X24,0X92,FALSE,FALSE);
				break;
			case 8:
			case 27:
				InitDelBuf(pBuf,0X24,0X92,0X49,FALSE,FALSE);
				break;
			case 9:
				InitDelBuf(pBuf,0,0,0,FALSE,TRUE);
				break;
			case 10:
				InitDelBuf(pBuf,0X11,0,0,FALSE,TRUE);
				break;
			case 11:
				InitDelBuf(pBuf,0X22,0,0,FALSE,TRUE);
				break;
			case 12:
				InitDelBuf(pBuf,0X33,0,0,FALSE,TRUE);
				break;
			case 13:
				InitDelBuf(pBuf,0X44,0,0,FALSE,TRUE);
				break;
			case 14:
				InitDelBuf(pBuf,0X55,0,0,FALSE,TRUE);
				break;
			case 15:
				InitDelBuf(pBuf,0X66,0,0,FALSE,TRUE);
				break;
			case 16:
				InitDelBuf(pBuf,0x77,0,0,FALSE,TRUE);
				break;
			case 17:
				InitDelBuf(pBuf,0X88,0,0,FALSE,TRUE);
				break;
			case 18:
				InitDelBuf(pBuf,0X99,0,0,FALSE,TRUE);
				break;
			case 19:
				InitDelBuf(pBuf,0XAA,0,0,FALSE,TRUE);
				break;
			case 20:
				InitDelBuf(pBuf,0XBB,0,0,FALSE,TRUE);
				break;
			case 21:
				InitDelBuf(pBuf,0XCC,0,0,FALSE,TRUE);
				break;
			case 22:
				InitDelBuf(pBuf,0XDD,0,0,FALSE,TRUE);
				break;
			case 23:
				InitDelBuf(pBuf,0XEE,0,0,FALSE,TRUE);
				break;
			case 24:
				InitDelBuf(pBuf,0XFF,0,0,FALSE,TRUE);
				break;
			case 28:
				InitDelBuf(pBuf,0X6D,0XB6,0XDB,FALSE,FALSE);
				break;
			case 29:
				InitDelBuf(pBuf,0XB6,0XDB,0X6D,FALSE,FALSE);
				break;
			case 30:
				InitDelBuf(pBuf,0XDB,0X6D,0XB6,FALSE,FALSE);
				break;
			}
			if(!WriteSecsWithBuf(dwStart,dwSize,btDisk,pBuf,bVerify))
				break;
		}
		break;
	}
	
	_ffree(pBuf);
	return TRUE;
}

BOOL InitDelBuf(BYTE *pBuf,BYTE btFir,BYTE btSec,BYTE btThr,BOOL bRand,BOOL bOneByte)
{
	BYTE *pOneBuf;
	int	 i,nRemain;

	if(bRand)
	{
		srand( (unsigned)time( NULL ) );
   		btFir = (BYTE)rand();
	}
	if(bOneByte)
	{
		memset(pBuf,btFir,DATA_BUFFER_SIZE);
	}
	else
	{
		pOneBuf = (BYTE*)malloc(SECTORSIZE);
		if(!pOneBuf) return FALSE;
		for(i=0;i<SECTORSIZE;i++)
		{
			nRemain = i % 3;
			switch(nRemain)
			{
			case 0:
				*(pOneBuf+i) = btFir;
				break;
			case 1:
				*(pOneBuf+i) = btSec;
				break;
			case 2:
				*(pOneBuf+i) = btThr;
				break;
			}
		}
		for(i=0;i<100;i++)
		{
			memcpy(pBuf+SECTORSIZE*i,pOneBuf,SECTORSIZE);
		}
		free(pOneBuf);
	}
	return TRUE;
}

BOOL WriteSecsWithBuf(DWORD dwStart,DWORD dwSize,BYTE btDisk,BYTE *pBuf,BOOL bVerify)
{
	DWORD				i,j,k;
	BYTE				*pCompBuf; 
    
	BIOS_DRIVE_PARAM	DriveParam;
    
	pCompBuf = NULL;
    if(bVerify)
    {
		pCompBuf = (BYTE *) _fmalloc(DATA_BUFFER_SIZE);
		if(!pBuf)
		{
			ErrorMessageBox(NO_MEMORY);
			return FALSE;
		}
	}

	if(!GetDriveParam(btDisk,&DriveParam))
	{
		if(pCompBuf) free(pCompBuf);
		ErrorMessageBox(GET_DISK_PARAM_FAIL);
		return FALSE;
	}

	i = dwSize / DATA_BUFFER_SECTOR;
    j = dwSize % DATA_BUFFER_SECTOR;
	for (k=0; k<i ;k++)
	{
		if(!WriteSector(dwStart+k*DATA_BUFFER_SECTOR,DATA_BUFFER_SECTOR,pBuf,btDisk,&DriveParam))
		{
			ErrorMessageBox("Write sector fail.");
		}
		g_dDeletedSize += DATA_BUFFER_SECTOR;
		FlushProgressInfo(); 
		
		if(bVerify)
		{
			ReadSector(dwStart+k*DATA_BUFFER_SECTOR,DATA_BUFFER_SECTOR,pCompBuf,btDisk,&DriveParam);
			if(memcmp(pBuf,pCompBuf,DATA_BUFFER_SIZE))
			{
				SaveVerifyLog(g_chLogDrive,dwStart+k*DATA_BUFFER_SECTOR,DATA_BUFFER_SECTOR);
			}
		}

	}
	if (j)
	{
		if(!WriteSector(dwStart+i*DATA_BUFFER_SECTOR,(WORD)j,pBuf,btDisk,&DriveParam))
		{
			ErrorMessageBox("Write sector fail.");
		}
		g_dDeletedSize += j;
		FlushProgressInfo();
		if(bVerify)
		{
			ReadSector(dwStart+i*DATA_BUFFER_SECTOR,(WORD)j,pCompBuf,btDisk,&DriveParam);
			if(memcmp(pBuf,pCompBuf,(int)j*512))
			{
				SaveVerifyLog(g_chLogDrive,dwStart+i*DATA_BUFFER_SECTOR,j);
			}
		}
	}
    if(pCompBuf) free(pCompBuf);
	return TRUE;
}

BOOL SaveReportEx(char csDrive)
{    
	BOOL                        bResult = FALSE;
	YG_PARTITION_INFO			*pDiskInfo = g_pCurFixDiskInfo;

	if(g_btCurFun == DELETEPARTITION)
	{
		if (g_delparinfo.nselpar == -1)
		{
			while (pDiskInfo)
			{
				g_pDiskInfo   = pDiskInfo;
				bResult = SaveReport(csDrive);
				pDiskInfo = pDiskInfo->pNext;
			}
		}
		else bResult = SaveReport(csDrive);
	}
	else
	{
		if( 0 == g_deldiskinfo.nSelDisk -DISKBASE+1 ) // means the user choose delete all disks
		{
			// Delete all disks
			int nDiskIndex;
			for( nDiskIndex=0; nDiskIndex<g_nDiskNum; nDiskIndex++ )
			{
				g_pCurFixDiskInfo->btDiskNum = nDiskIndex;
				bResult = SaveReport(csDrive);
			}
		}
		else bResult = SaveReport(csDrive);
	}                  
	return bResult;
}

BOOL SaveReport(char csDrive)
{
	char				szInfo[80],szdriveletter;
	int 				fh;
	WORD				wData = 0x0a0d;
	DWORD				dwStart,dwEnd;
	BIOS_DRIVE_PARAM    DriveParam;
	char				*szMethod[] = {"Overwrite with 00","Overwrite with FF",
						"Overwrite with random data","NSA method",
						"Old NSA method","DoD STD method",
						"NATO method","Gutmann method"};
	char				*szMonth[] = {"Jan","Feb","Mar","Apr","May","Jun",
									  "Jul","Aug","Sep","Oct","Nov","Dec"}; 
	BYTE				btDisk;

	if(_chdrive(csDrive-'A'+1)) return FALSE;
	
	strcpy(szInfo,"A:\\DmReport.txt");
	szInfo[0] = csDrive;

 	fh = _open( szInfo, _O_CREAT | _O_RDWR );
 	if( fh == -1 )   return FALSE;

 	_lseek(fh,0,SEEK_END);
 	if(g_btCurFun == DELETEPARTITION)
 	{
 		dwStart 	  = g_pDiskInfo->dwStartSector; 
 		dwEnd		  = dwStart + g_pDiskInfo->dwPartSize;
 		szdriveletter = g_pDiskInfo->DriveLetter;
 		if(g_pDiskInfo->bLogic) dwStart += 0x3f;
 		if(g_pDiskInfo->DriveLetter < 0x40) szdriveletter = '*';
 		sprintf(szInfo,DELPARREPORT,g_pDiskInfo->DriveLetter,
 				g_pDiskInfo->btDiskNum+1, dwStart, dwEnd);
 		btDisk = g_pDiskInfo->btDiskNum;
 	}
 	else
 	{
		GetDriveParam(g_pCurFixDiskInfo->btDiskNum+DISKBASE,&DriveParam);
 		sprintf(szInfo,DELDISKREPORT,g_pCurFixDiskInfo->btDiskNum+1,
 				DriveParam.dwCylinders,DriveParam.dwHeads,DriveParam.dwSecPerTrack);
 		btDisk = g_pCurFixDiskInfo->btDiskNum;
 	}
 	//get disk indentify
	PrevIntHandlerPri = _dos_getvect(0x76); 
	PrevIntHandlerSec = _dos_getvect(0x77);
	PrevTimerHandler = _dos_getvect(0x08);
	_dos_setvect(0x76, HarddiskIntHandlerPri);
	_dos_setvect(0x77, HarddiskIntHandlerSec);
	_dos_setvect(0x08, TimerHandler);
		
	identify(g_DevType[btDisk].wIoBase,g_DevType[btDisk].nDevice);
		
	_dos_setvect(0x76, PrevIntHandlerPri);
	_dos_setvect(0x77, PrevIntHandlerSec);
	_dos_setvect(0x08, PrevTimerHandler);
		
 	_write( fh, szInfo, strlen(szInfo));
 	_write( fh, &wData, sizeof( WORD ));

	//Write disk identify to file
	strcpy(szInfo,"----------Disk Information----------");
 	_write( fh, szInfo, strlen(szInfo));
 	_write( fh, &wData, sizeof( WORD ));
        
    sprintf(szInfo,"Module Number: %s",AtaIdentifyInfo.ModelNumber);
 	_write( fh, szInfo, strlen(szInfo));
 	_write( fh, &wData, sizeof( WORD ));
 		
	sprintf(szInfo,"Firmware Rer : %s", AtaIdentifyInfo.FirmwareRevision);
 	_write( fh, szInfo, strlen(szInfo));
 	_write( fh, &wData, sizeof( WORD ));

	sprintf(szInfo,"Serial Number: %s", AtaIdentifyInfo.SerialNumber);
 	_write( fh, szInfo, strlen(szInfo));
 	_write( fh, &wData, sizeof( WORD ));                            

	sprintf(szInfo,"Capicity : %lu sectors, %lu MB", 
		AtaIdentifyInfo.TotalNumberUserAddressableSectors, 
		AtaIdentifyInfo.TotalNumberUserAddressableSectors / 2048);
 	_write( fh, szInfo, strlen(szInfo));
 	_write( fh, &wData, sizeof( WORD ));

	sprintf(szInfo,DELMETHODREPORT,szMethod[g_nMethod]);
 	_write( fh, szInfo, strlen(szInfo));
 	_write( fh, &wData, sizeof( WORD ));
 	sprintf(szInfo,TIMESTAMP,szMonth[g_StartTime.tm_mon],g_StartTime.tm_mday,
 			g_StartTime.tm_hour,g_StartTime.tm_min,g_StartTime.tm_sec,
 			g_StartTime.tm_year+1900,
			szMonth[g_EndTime.tm_mon],g_EndTime.tm_mday,g_EndTime.tm_hour,
			g_EndTime.tm_min,g_EndTime.tm_sec,
			g_EndTime.tm_year+1900);
 	_write( fh, szInfo, strlen(szInfo));
 	_write( fh, &wData, sizeof( WORD ));
 	_write( fh, &wData, sizeof( WORD ));
	_close(fh);
	return TRUE;
}

BOOL SaveVerifyLog(char csDrive,DWORD dwStartSec,DWORD dwSectors)
{
	char				szInfo[80],szdriveletter;
	int 				fh;
	WORD				wData = 0x0a0d;
	DWORD				dwStart,dwEnd;
	BIOS_DRIVE_PARAM    DriveParam;
	char				*szMethod[] = {"Overwrite with 00","Overwrite with FF",
						"Overwrite with random data","NSA method",
						"Old NSA method","DoD STD method",
						"NATO method","Gutmann method"};
	BYTE				btDisk;
	
	if(_chdrive(csDrive-'A'+1)) return FALSE;
	strcpy(szInfo,"A:\\Dmverify.log");
	szInfo[0] = csDrive;
	
 	fh = _open( szInfo, _O_CREAT | _O_RDWR );
 	if( fh == -1 )   return FALSE;

 	_lseek(fh,0,SEEK_END);
 	if(g_btCurFun == DELETEPARTITION)
 	{
 		dwStart 	  = g_pDiskInfo->dwStartSector; 
 		dwEnd		  = dwStart + g_pDiskInfo->dwPartSize;
 		szdriveletter = g_pDiskInfo->DriveLetter;
 		if(g_pDiskInfo->bLogic) dwStart += 0x3f;
 		if(g_pDiskInfo->DriveLetter < 0x40) szdriveletter = '*';
 		sprintf(szInfo,DELPARREPORT,g_pDiskInfo->DriveLetter,
 				g_pDiskInfo->btDiskNum+1, dwStart, dwEnd);
 		btDisk = g_pDiskInfo->btDiskNum;
 	}
 	else
 	{
		GetDriveParam(g_pCurFixDiskInfo->btDiskNum+DISKBASE,&DriveParam);
 		sprintf(szInfo,DELDISKREPORT,g_pCurFixDiskInfo->btDiskNum+1,
 				DriveParam.dwCylinders,DriveParam.dwHeads,DriveParam.dwSecPerTrack);
 		btDisk = g_pCurFixDiskInfo->btDiskNum;
 	}
 		//get disk indentify
	PrevIntHandlerPri = _dos_getvect(0x76); 
	PrevIntHandlerSec = _dos_getvect(0x77);
	PrevTimerHandler = _dos_getvect(0x08);
	_dos_setvect(0x76, HarddiskIntHandlerPri);
	_dos_setvect(0x77, HarddiskIntHandlerSec);
	_dos_setvect(0x08, TimerHandler);
		
	identify(g_DevType[btDisk].wIoBase,g_DevType[btDisk].nDevice);
		
	_dos_setvect(0x76, PrevIntHandlerPri);
	_dos_setvect(0x77, PrevIntHandlerSec);
	_dos_setvect(0x08, PrevTimerHandler);
		
 	_write( fh, szInfo, strlen(szInfo));
 	_write( fh, &wData, sizeof( WORD ));

		//Write disk identify to file
	strcpy(szInfo,"----------Disk Information----------");
 	_write( fh, szInfo, strlen(szInfo));
 	_write( fh, &wData, sizeof( WORD ));
        
    sprintf(szInfo,"Module Number: %s",AtaIdentifyInfo.ModelNumber);
 	_write( fh, szInfo, strlen(szInfo));
 	_write( fh, &wData, sizeof( WORD ));                            
 		
	sprintf(szInfo,"Firmware Rer : %s", AtaIdentifyInfo.FirmwareRevision);
 	_write( fh, szInfo, strlen(szInfo));
 	_write( fh, &wData, sizeof( WORD ));                            

	sprintf(szInfo,"Serial Number: %s", AtaIdentifyInfo.SerialNumber);
 	_write( fh, szInfo, strlen(szInfo));
 	_write( fh, &wData, sizeof( WORD ));                            

	sprintf(szInfo,"Capicity : %lu sectors, %lu MB", 
		AtaIdentifyInfo.TotalNumberUserAddressableSectors, 
		AtaIdentifyInfo.TotalNumberUserAddressableSectors / 2048);
 	_write( fh, szInfo, strlen(szInfo));
 	_write( fh, &wData, sizeof( WORD ));                            

	sprintf(szInfo,DELMETHODREPORT,szMethod[g_nMethod]);
 	_write( fh, szInfo, strlen(szInfo));
 	_write( fh, &wData, sizeof( WORD ));
         
    sprintf(szInfo,"Verify start sector:%lu sectors:%lu failed",dwStartSec,dwSectors);
    _write( fh, szInfo, strlen(szInfo));
 	_write( fh, &wData, sizeof( WORD ));
	_close(fh);
	return TRUE;
}
