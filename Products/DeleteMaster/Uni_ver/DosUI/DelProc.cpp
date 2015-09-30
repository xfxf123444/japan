#include "comm.h"
#include "main.h"

extern 	_sysinfo					g_SysInfo;     
extern	YG_PARTITION_INFO			*g_pCurFixDiskInfo;
extern	YG_PARTITION_INFO			*g_pDiskInfo;
extern	_deleteparinfo              g_delparinfo;
extern	_deletediskinfo				g_deldiskinfo;
extern	_diskviewinfo				g_diskviewinfo;
extern  BYTE						g_btCurFun;
extern  double						g_dDeletedSize;
extern  double						g_dTotalSize;
extern	int							g_nMethod;
extern  struct tm					g_StartTime;
extern  struct tm					g_EndTime;
extern  clock_t 					g_start;
extern  BOOL						g_bBeepOff;
extern  int							g_nMaxTry;
void progressshowtarget();
BOOL deletecontent();

char showprogresswin()
{
	char	str[256];
	char	*szMethod[] = {"overwrite with 00","overwrite with FF",
						"random data","NSA method",
						"Old NSA method","DoD STD method",
						"NATO method","Gutmann method"};
	
	_setvisualpage(PAGE_PROGRESS);  
	_setactivepage(PAGE_PROGRESS);
    
    drawbox(1, 1, g_SysInfo.nmaxrows, g_SysInfo.nmaxcols);
	progressshowtarget();

	_settextposition(7, 4);
	sprintf(str,"Delete Method : %s",szMethod[g_nMethod]);
	_outtext(str);

	_settextposition(7, 6+strlen(str)); 
	if (g_bBeepOff)
		sprintf(str,"Beep Status : Off");
	else
		sprintf(str,"Beep Status : On");
	_outtext(str);
	
    drawbox(11, 4, 13, g_SysInfo.nmaxcols - 3);
	              
	_settextposition(12,6 );     
	charstostr(str,PROGRESS_CHAR,g_SysInfo.nmaxcols - 10);
	_outtext(str); 
	if(deletecontent())
	{
		_clearscreen(_GCLEARSCREEN);
	    return 's';
	}
	else
	{
		_clearscreen(_GCLEARSCREEN);
		return 'e';
	}
}

void progressshowtarget()
{
	int							i;
	YG_PARTITION_INFO			*pDiskInfo = g_pCurFixDiskInfo;
	char						szdriveletter,szsel[80];
	DWORD						dwStart;

	if(g_btCurFun == DELETEPARTITION)
	{
		for(i=0;i<g_delparinfo.nselpar;i++)
		{
			pDiskInfo = pDiskInfo->pNext;
		}
		g_pDiskInfo   = pDiskInfo;
		szdriveletter = pDiskInfo->DriveLetter;
		dwStart 	  = pDiskInfo->dwStartSector;
		if(pDiskInfo->DriveLetter < 0x40) szdriveletter = '*';
		if(pDiskInfo->bLogic) dwStart += 0x3f;
	   	sprintf(szsel,PARDELSELPAR,szdriveletter,
	   		dwStart,pDiskInfo->dwStartSector+pDiskInfo->dwPartSize-1);

		_settextposition(6, 4);          
		_outtext("Partition:");

		_settextposition(6, 15);          
		_outtext(szsel);
	}
	else
	{
		_settextposition(6, 4);          
	   	sprintf(szsel,DISKSELINFO,g_deldiskinfo.nSelDisk-DISKBASE+1,
	   		g_deldiskinfo.dwCylinders,g_deldiskinfo.dwHeads,g_deldiskinfo.dwSecPerTrack);
		_outtext(szsel);
    }
}

BOOL deletecontent()
{
	int							i;
	YG_PARTITION_INFO			*pDiskInfo = g_pCurFixDiskInfo;
	DWORD						dwStart,dwSize;
	CREATE_PAR_FLAG				cfg;
	struct tm					*newtime;
	time_t						ltime; 
	
	time(&ltime );
	newtime = localtime( &ltime );
	memcpy(&g_StartTime,newtime,sizeof(tm));
	
	if (!g_bBeepOff) Beep(1000,100);
	g_start = clock();
	if(g_btCurFun == DELETEPARTITION)
	{
		for(i=0;i<g_delparinfo.nselpar;i++)
		{
			pDiskInfo = pDiskInfo->pNext;
		}
		dwStart 	  = pDiskInfo->dwStartSector;
		if(pDiskInfo->bLogic) dwStart += 0x3f;
		dwSize = pDiskInfo->dwStartSector+pDiskInfo->dwPartSize - dwStart;

		cfg.Extended = 1;
		cfg.Active	 = 0;
		if(pDiskInfo->BootFlag == BOOT_FLAG)
		{
			cfg.Active = 1;
		}	
		g_dDeletedSize = 0;

		if(!DoDeleteSectors(dwStart,dwSize,pDiskInfo->btDiskNum+DISK_BASE))
		{
			if (!g_bBeepOff)
			{
				Beep(1000,200);
				Beep(1000,200);
				Beep(1000,200);
			}
			return FALSE;
		}
	}
	else
	{
		dwStart 		= 0;
		dwSize  		= g_deldiskinfo.dwDiskSize;
		g_dDeletedSize  = 0;
		if(!DoDeleteSectors(dwStart,dwSize,g_deldiskinfo.nSelDisk))
		{
			if (!g_bBeepOff)
			{
				Beep(1000,200);
				Beep(1000,200);
				Beep(1000,200);
			}
			return FALSE;
		}
    }
	time(&ltime );
	newtime = localtime( &ltime );
	memcpy(&g_EndTime,newtime,sizeof(tm));
	return TRUE;
}