#include "comm.h"
#include "main.h"   
#include "intrrpt.h"

_sysinfo						g_SysInfo;
_deletediskinfo					g_deldiskinfo;
_diskviewinfo					g_diskviewinfo;
_deleteparinfo                  g_delparinfo;
BYTE							g_btCurFun;
int								g_nMethod  = 3;
int 							g_nDiskNum;
int								g_nCurDrv = 0;
PARMAPINFO*		   				g_pParMapInfo = NULL;
YG_PARTITION_INFO* 				g_pCurFixDiskInfo = NULL;
YG_PARTITION_INFO*				g_pDiskInfo;
double							g_dDeletedSize;
double							g_dTotalSize;
struct tm						g_StartTime;
struct tm						g_EndTime;
clock_t 						g_start, g_finish;
long							g_nElasped;
ATA_DEVICE_TYPE					g_DevType[4];
BOOL                            g_bBeepOff = FALSE;
int								g_nMaxTry = 10;
char			   				g_drives[27] = {0};
char							g_chLogDrive = -1;

extern void (_interrupt far *PrevTimerHandler)();
extern void (_interrupt far *PrevIntHandlerPri)();
extern void (_interrupt far *PrevIntHandlerSec)();

void main(int argc,char **argv)
{
	struct 	_videoconfig vc;
	BYTE				 SecBuf[512];
	BYTE				 btDisk;
	BIOS_DRIVE_PARAM	 DriveParam;
	int					 nIDE,nDevice,nDisk;
	char                 cDrive;
	WORD 				 wIoBase[] = {PRIMARY_IDE_IO_BASE, SECONDARY_IDE_IO_BASE};

	_getvideoconfig(&vc);   
	g_SysInfo.nmaxrows = vc.numtextrows;
	g_SysInfo.nmaxcols = vc.numtextcols;   
	_clearscreen(_GCLEARSCREEN);
	_remappalette(BKCOLOR, 0x2f0000L); 
	_displaycursor(_GCURSOROFF);                          
	_wrapon(_GWRAPOFF);     
	_settextcolor(TEXTCOLOR);
	//Get the current (default) drive (1=A, 2=B, etc.).
	g_nCurDrv = _getdrive();

	//Get hard disk number;
	if(!Getallparmapinfo())
	{
		ErrorMessageBox("Getallparmapinfo error.");      
		exitprocess();
		return;
	}
	
	g_nDiskNum = GetHardDiskNum();
	if(g_nDiskNum == 0)
	{
		ErrorMessageBox("Get hard disk number error.");      
		exitprocess();
		return;
	}
             
	for(btDisk=0;btDisk<g_nDiskNum;btDisk++)
	{
		if(!GetDriveParam(btDisk+DISKBASE,&DriveParam))
		{
			printf("Get disk parameter fail.\n");
			exitprocess();
			return ;
		}

		memset(&SecBuf,0,512);
		if(!WriteSector(3,1,&SecBuf,btDisk+DISKBASE,&DriveParam))
		{
			printf("Write sector fail.\n");
			exitprocess();
			return ;
		}
		SecBuf[0] = btDisk+DISKBASE;
		if(!WriteSector(3,1,&SecBuf,btDisk+DISKBASE,&DriveParam))
		{
			printf("Write sector fail.\n");
			exitprocess();
			return ;
		}
	}             
	PrevIntHandlerPri = _dos_getvect(0x76); 
	PrevIntHandlerSec = _dos_getvect(0x77);
	PrevTimerHandler = _dos_getvect(0x08);
	_dos_setvect(0x76, HarddiskIntHandlerPri);
	_dos_setvect(0x77, HarddiskIntHandlerSec);
	_dos_setvect(0x08, TimerHandler);
	
	for (nIDE = 0; nIDE < 2; nIDE++)
	{
		for (nDevice = ATA_MASTER; nDevice <= ATA_SLAVE; nDevice++)
		{
			//Read Sector
			memset(&SecBuf,0xff,512); 
			if (ATARead(wIoBase[nIDE],nDevice,3,1,(LPBYTE)&SecBuf) == ERR_SUCCESS)
			{
				g_DevType[SecBuf[0]-DISKBASE].wIoBase = wIoBase[nIDE];
				g_DevType[SecBuf[0]-DISKBASE].nDevice = nDevice;							
			}
		}
	}
	_dos_setvect(0x76, PrevIntHandlerPri);
	_dos_setvect(0x77, PrevIntHandlerSec);
	_dos_setvect(0x08, PrevTimerHandler);

	if (argc > 1) 
	{
		switch (ParseCmdLine(argc-1,&argv[1],&nDisk,&cDrive))
		{
		case OPERATE_DELETE_ALL_DISK:
			SecDeleteDisk(-1);
			break;
		case OPERATE_DELETE_DISK:
			SecDeleteDisk(nDisk);
			break;
		case OPERATE_DELETE_PARTITION:
			SecDeletePartition(cDrive);
			break;
		default:
			PrintUsage();
			break;
		}
	}
	else run();
	exitprocess();
}                        

void PrintUsage()
{
	printf("DM.exe can use without parameters or use parameters like follow \n\n");
	printf("DM -FSettingFile -A -DDiskNumber -LDriverLetter -SDeleteMethod [Optional]-B [Optional]-GLogDrive\n\n");
	printf("Important:\n / can consider as - \n Case insensitive \n eg. -F equ -f equ /F equ /f\n\n");
	printf("-S Name of setting file, 8.3 format file name\n");
	printf("-A Delete all hard disks found\n");
	printf("-D Disk Number 1 means first hard disk and so on.\n");
	printf("-L Driver letter of the partition to delete\n");
	printf("-M Delete Method 0-Fill Zero 1-Fill FF 2-Fill Random 3-NSA 4-Old NSA 5-Dod 6-Nato 7-Gutmann\n");
	printf("-B is optional, default is beep off\n");
	printf("-G is optional, default is save log to A drive\n\n");
	printf("Press any key to continue.\n");      
	getch();
	return;
}

long ParseCmdLine(int argc,char **argv,int *pnDisk,char *pcDrive)
{
	int i;
	long ulOperate = 0;

	if (argc < 1 || argc > 3) 	return FALSE;

	g_bBeepOff = TRUE;
	g_nMaxTry = 10;

	for (i = 0;i < argc;i ++)
	{
		switch (argv[i][0])
		{
		case '-':
		case '/':
			switch (argv[i][1])
			{
			case 'S':
			case 's':
				if (argc != 1) return FALSE;
				ulOperate = ParseSettingFile(&argv[i][2],pnDisk,pcDrive);
				break;
			case 'A':
			case 'a':
				if (ulOperate) return FALSE;
				if (argv[i][2] != '\0') return FALSE;
				ulOperate = OPERATE_DELETE_ALL_DISK;
				*pnDisk = -1;
				break;
			case 'D':
			case 'd':
				if (ulOperate) return FALSE;
				if (argv[i][3] != '\0') return FALSE;
				ulOperate = OPERATE_DELETE_DISK;
				*pnDisk = argv[i][2] - '1';
				if (*pnDisk < 0 || *pnDisk > g_nDiskNum) 
				{
					printf("Invalid harddisk to delete!\n");
					return FALSE;
				}
				break;
			case 'L':
			case 'l':
				if (ulOperate) return FALSE;
				if (argv[i][3] != '\0') return FALSE;
				ulOperate = OPERATE_DELETE_PARTITION;
				*pcDrive = argv[i][2];
				if (*pcDrive >= 'a' && *pcDrive <= 'z')
				{
					*pcDrive -= 'a';
					*pcDrive += 'A';
				}
				if (_chdrive(*pcDrive - 'A' + 1)) 
				{
					printf("Invalid drive letter to delete!\n");
					return FALSE;
				}
				break;
			case 'B':
			case 'b':
				g_bBeepOff = FALSE;
				break;
			case 'M':
			case 'm':
				g_nMethod = (int)(argv[i][2]-'0');
				if (g_nMethod < 0 || g_nMethod > 7) 
				{
					printf("Invalid delete method!\n");
					return FALSE;
				}
				break;
			case 'G':
			case 'g':
				g_chLogDrive = argv[i][2];
				if (g_chLogDrive >= 'a' && g_chLogDrive <= 'z')
				{
					g_chLogDrive -= 'a';
					g_chLogDrive += 'A';
				}
				if (_chdrive(g_chLogDrive - 'A' + 1)) 
				{
					printf("Invalid log drive!\n");
					return FALSE;
				}
				break;
			default:
				return FALSE;
			}
			break;
		default:
			return FALSE;
		}
	}

	return ulOperate;
}

BOOL SecDeleteDisk(int nDisk)
{
	BOOL				bResult = FALSE;
	int                 i;
	BIOS_DRIVE_PARAM    DriveParam;

	memset(&g_deldiskinfo,0,sizeof(_deletediskinfo));

	g_btCurFun = DELETEDISK;
	
	if (nDisk == -1)
	{
		for (i = 0;i< g_nDiskNum;i ++)
		{
			if (GetDriveParam(i+DISKBASE,&DriveParam))
			{
	   			g_deldiskinfo.nSelDisk = i + DISKBASE;
			    g_deldiskinfo.dwDiskSize			= DriveParam.dwSectorL;
			    g_deldiskinfo.dwCylinders			= DriveParam.dwCylinders;
			    g_deldiskinfo.dwHeads				= DriveParam.dwHeads;
			    g_deldiskinfo.dwSecPerTrack			= DriveParam.dwSecPerTrack;
				if ('s' == showprogresswin()) 
				{
					SaveReport(g_chLogDrive);
					bResult = TRUE;
				}
			}
		}
	}
	else
	{
		if (GetDriveParam(nDisk+DISKBASE,&DriveParam))
		{
   			g_deldiskinfo.nSelDisk = nDisk + DISKBASE;
		    g_deldiskinfo.dwDiskSize			= DriveParam.dwSectorL;
		    g_deldiskinfo.dwCylinders			= DriveParam.dwCylinders;
		    g_deldiskinfo.dwHeads				= DriveParam.dwHeads;
		    g_deldiskinfo.dwSecPerTrack			= DriveParam.dwSecPerTrack;
			if ('s' == showprogresswin()) 
			{
				SaveReport(g_chLogDrive);
				bResult = TRUE;
			}
		}
	}

	if (!bResult) ErrorMessageBox("Delete disk fail!");
	return bResult;
}

long ParseSettingFile(char *szFile,int *pnDisk,char *pcDrive)
{
	UINT i;
	int  fh;
	char szBuff[MAX_PATH * 3];
	long ulOperate = 0,lFileLen;

 	fh = _open( szFile, _O_RDWR );
 	if( fh == -1 ) return FALSE;

	lFileLen = _filelength(fh);

	if (lFileLen < 0 || lFileLen > (MAX_PATH * 3 - 1)) 
	{
		_close(fh);
		return FALSE;
	}
	_read( fh,szBuff,(UINT)lFileLen);
	szBuff[lFileLen] = '\0';
	_close(fh);

	for (i = 0;i < strlen(szBuff);i ++)
	{
		switch (szBuff[i])
		{
		case '-':
		case '/':
			switch (szBuff[i+1])
			{
			case 'A':
			case 'a':
				if (ulOperate) return FALSE;
				ulOperate = OPERATE_DELETE_ALL_DISK;
				*pnDisk = -1;
				break;
			case 'D':
			case 'd':
				if (ulOperate) return FALSE;
				ulOperate = OPERATE_DELETE_DISK;
				*pnDisk = szBuff[i+2] - '1';
				if (*pnDisk < 0 || *pnDisk > g_nDiskNum) 
				{
					printf("Invalid harddisk to delete!\n");
					return FALSE;
				}
				break;
			case 'L':
			case 'l':
				if (ulOperate) return FALSE;
				ulOperate = OPERATE_DELETE_PARTITION;
				*pcDrive = szBuff[i+2];
				if (*pcDrive >= 'a' && *pcDrive <= 'z')
				{
					*pcDrive -= 'a';
					*pcDrive += 'A';
				}
				if (_chdrive(*pcDrive - 'A' + 1)) 
				{
					printf("Invalid drive letter to delete!\n");
					return FALSE;
				}
				break;
			case 'B':
			case 'b':
				g_bBeepOff = FALSE;
				break;
			case 'M':
			case 'm':
				g_nMethod = (int)(szBuff[i+2]-'0');
				if (g_nMethod < 0 || g_nMethod > 7) 
				{
					printf("Invalid delete method,\n");
					return FALSE;
				}
				break;
			case 'G':
			case 'g':
				g_chLogDrive = szBuff[i+2];
				if (g_chLogDrive >= 'a' && g_chLogDrive <= 'z')
				{
					g_chLogDrive -= 'a';
					g_chLogDrive += 'A';
				}
				if (_chdrive(g_chLogDrive - 'A' + 1))
				{
					printf("Invalid log drive!\n");
					return FALSE;
				}
				break;
			default:
				return FALSE;
			}
			break;
		default:
			return FALSE;
		}
	}
	return ulOperate;
}

BOOL SecDeletePartition(char cDrive)
{
	BOOL            bResult = FALSE;
	PARMAPINFO		*pCur;
	YG_PARTITION_INFO	*p;

	for (pCur = g_pParMapInfo;pCur;pCur = pCur->pNext)
	{
		if (pCur->cDrv == cDrive)
			break;
	}
	if (pCur)
	{
    	memset(&g_delparinfo,0,sizeof(_deleteparinfo));
		p = g_pCurFixDiskInfo;
		while(p != NULL)
		{
			g_pCurFixDiskInfo = g_pCurFixDiskInfo->pNext;
			free(g_pCurFixDiskInfo);
			p = g_pCurFixDiskInfo;	
		}

		if (GetFixDiskInfo(pCur->dmiInt13Unit - DISK_BASE))
		{
			for (p = g_pCurFixDiskInfo; p; p = p->pNext)
			{
				if(p->dwSystemFlag && p->DriveLetter == cDrive)
					break;
				g_delparinfo.nselpar ++;
			}
			if (p)
			{
				g_delparinfo.nseldisk = pCur->dmiInt13Unit;
				g_btCurFun = DELETEPARTITION;
				if ('s' == showprogresswin())
					SaveReport(g_chLogDrive);
				else ErrorMessageBox("Fail to delete partition");
				bResult = TRUE;
			}
		}
	}

	if (!bResult) ErrorMessageBox("Can not locate partition to delete!");

	return bResult;
}

void run()   //it's 'dead' circle,it exists in the whole program
{
	char	ch;    
label_mainwin:	     
	ch = showmainwin();
	switch(ch)
	{             
	case ESC : 
		return;
	case 'p':
    	 memset(&g_delparinfo,0,sizeof(_deleteparinfo));
    	 g_delparinfo.nselpar = -1;
		 g_btCurFun = DELETEPARTITION;
		 goto label_delpar;
	case 'd':
		memset(&g_deldiskinfo,0,sizeof(_deletediskinfo));
		g_btCurFun = DELETEDISK;
	 	goto label_deldisk;
	case 'v': 
	    memset(&g_diskviewinfo,0,sizeof(_diskviewinfo));
		g_btCurFun = DISKVIEW;
		goto label_diskview;
	case 'l':
		goto label_logdrive;
	default:
		goto label_mainwin;
	}
label_logdrive:
	ch = showsellogdrivewin();
	goto label_mainwin;
label_delpar:
	ch = showdelparwin();
	switch(ch)
	{
	case 'd': goto label_confirm;
	case 'v': goto label_diskview;
	case 'm': goto label_methodwin;
	case 'c': goto label_mainwin;
	}
label_deldisk:	
	ch = showdeldiskwin();
	switch(ch)
	{
	case 'd': goto label_confirm;
	case 'v': goto label_diskview;
	case 'i': goto label_identify;
	case 'm': goto label_methodwin;
	case 'c': goto label_mainwin;
	}
label_diskview:	
	ch = showdiskviewwin();
	switch(ch)
	{
	case 'c':
		switch(g_btCurFun)
		{
		case DISKVIEW:
			goto label_mainwin;
		case DELETEPARTITION:
			goto label_delpar;
		case DELETEDISK:
			goto label_deldisk;
		}
	case 's': goto label_selsec;
	}
label_selsec:
	ch = showsectorselwin();
	switch(ch)
	{
	case 'o': goto label_diskview;
	case 'c': goto label_diskview;
	}
label_identify:
	ch = showidentifywin();
	switch(ch)
	{
	case 'o': goto label_deldisk;
	}
label_confirm:
	ch = showconfirmwin();
	switch(ch)
	{
	case 'c':
		switch(g_btCurFun)
		{
		case DELETEPARTITION:
			goto label_delpar;
		case DELETEDISK:
			goto label_deldisk;
		}
		break;
	case 'a':
		goto label_progress;
	}
label_progress:
	ch = showprogresswin();
	switch(ch)
	{
	case 'e':goto label_mainwin;
	case 's':goto label_finishwin;
	}
label_finishwin:
	ch = showfinishwin();
	goto label_mainwin;
label_methodwin:
	ch = showmethodwin();
	switch(ch)
	{
	case 'c':
	case 'o':
		switch(g_btCurFun)
		{
		case DELETEPARTITION:
			goto label_delpar;
		case DELETEDISK:
			goto label_deldisk;
		}
	}
}

void drawbox(int r1, int c1, int r2, int c2, int right)
{                          
	#define	left_top	218 
	#define	left_buttom 192
	#define	right_top	191
	#define right_buttom 217

	#define top_line	196 
	#define buttom_line	196
	#define left_line 	179
	#define	right_line  right
	
	int		i;   
	char    sztext[2];                                             
	                          
	_wrapon(_GWRAPOFF);	                          
	_settextposition(r1, c1);    //left-top
	sprintf(sztext, "%c", left_top);	                
	_outtext(sztext);               
	for(i = c1 + 1; i < c2; i++) //top-line
	{
		_settextposition(r1, i);
		sprintf(sztext, "%c",top_line);	                
		_outtext(sztext);
	}
	_settextposition(r1, i); //right-top
	sprintf(sztext, "%c",right_top);	                
	_outtext(sztext);
		      
	_settextposition(r2, c1); //left-buttom
	sprintf(sztext, "%c",left_buttom);	                
	_outtext(sztext);
	for(i = c1 + 1; i < c2; i++) //buttom-line
	{
		_settextposition(r2, i);
		sprintf(sztext, "%c",buttom_line);	                
		_outtext(sztext);
	}
	_settextposition(r2, i);
	sprintf(sztext, "%c",right_buttom);	                
	_outtext(sztext);

	for(i = r1 + 1; i < r2; i++) //left-line
	{
		_settextposition(i, c1);
		sprintf(sztext, "%c", left_line);	                
		_outtext(sztext);
	}

	for(i = r1 + 1; i < r2; i++)//right-line
	{
		_settextposition(i, c2);
		sprintf(sztext, "%c",right_line);	                
		_outtext(sztext);
	}
}

void selectstr(char * szstr, int nrow, int ncol, short ncolor)
{                                                           
	short noldcolor;
	
	noldcolor = _gettextcolor(); 
	_settextcolor(ncolor);
	_settextposition(nrow, ncol);
	_outtext(szstr);
	_settextcolor(noldcolor);
}

void writemem(int npage, int nrow, int ncol, char ch, int nlen)
{                                                                      
	char __far*	szmem =(char __far*)(0x0b8000000 + npage * 1024 * 4);
	
	szmem += ((nrow - 1) * g_SysInfo.nmaxcols + ncol) * 2; 
	
	_fmemset(szmem, ch, nlen);
}

void modifyattrib(int npage, int nrow, int ncol, char ncolor, char nbkcolor, int nlen)
{
	char __far*	szmem =(char __far*)(0x0b8000001 + npage * 1024 * 4 + ((nrow - 1) * g_SysInfo.nmaxcols + ncol - 1) * 2);
	char	attrib;   
	int		i;
	        
	nbkcolor *= 16;
	attrib = ncolor + nbkcolor;
	            
	for(i = 0; i < nlen; i++)	          
		_fmemcpy(szmem + i * 2, &attrib, 1);
}

void writestr(int npage, int nrow, int ncol, char * szstr)
{                                       
	unsigned int i;
	char __far*	szmem =(char __far*)(0x0b8000000 + npage * 1024 * 4);
	
	szmem += ((nrow - 1) * g_SysInfo.nmaxcols + ncol) * 2;
	for(i = 0; i < _fstrlen(szstr); i++)	          
		_fmemcpy(szmem + i * 2, szstr + i, 1);
}

void winproc(cwindow* pwin, int message, DWORD param1, DWORD param2, DWORD param3)
{                   
	switch(message)
	{    
	case WM_KEY:
		pwin->onkey((int)param1);
		break;
	case WM_ACTIVE:
		pwin->onactive(); 
		break;
	case WM_INACTIVE:
		pwin->oninactive();
		break;   
	}
}

void exitprocess()
{   
	unsigned			number_of_drives;
	PARMAPINFO			*pParMapInfo;     
	YG_PARTITION_INFO 	*pDiskInfo;
	
    
    pParMapInfo = g_pParMapInfo;
	while(pParMapInfo != NULL)
	{
		g_pParMapInfo = pParMapInfo->pNext;
		free(pParMapInfo);
		pParMapInfo = g_pParMapInfo;	
	}
	
    pDiskInfo = g_pCurFixDiskInfo;
	while(pDiskInfo != NULL)
	{
		g_pCurFixDiskInfo = g_pCurFixDiskInfo->pNext;
		free(g_pCurFixDiskInfo);
		pDiskInfo = g_pCurFixDiskInfo;	
	}
	_chdrive(g_nCurDrv);
	_dos_setdrive(g_nCurDrv,&number_of_drives);
    _clearscreen(_GCLEARSCREEN);
    _setvideomode(_DEFAULTMODE);    
	_clearscreen(_GCLEARSCREEN);
	_displaycursor(_GCURSORON);                          
}

void ErrorMessageBox(char  * str)
{
	 int  nAPage,nVPage;
	 nVPage=_getvisualpage();  
	 nAPage=_getactivepage(); 	
	_setvisualpage(PAGE_ERROR);  
	_setactivepage(PAGE_ERROR); 
	_clearscreen(_GCLEARSCREEN);
	drawbox(9, 19, 16, 61);
	_settextwindow(10, 20, 15, 60);   
	_outtext("               WARNING MESSAGE \n   -----------------------------------\n");
	_wrapon(_GWRAPON);
	_outtext(str);
	_settextwindow(1, 1, g_SysInfo.nmaxrows, g_SysInfo.nmaxcols);
	_settextposition(24, 40);
	_outtext("please press any key to exit.");
	_getch();
	_setvisualpage(nVPage);  
	_setactivepage(nAPage); 
}

void charstostr(char *str, BYTE c, int nLen)
{
	for(int i = 0; i < nLen; i ++)
	{
		*(str + i) = (char)c;
	}                
	str[i] = '\0';
}

void Sleep( clock_t wait )
{
    clock_t goal;

    goal = wait + clock();
    while( goal >= clock() );
}

// Beep: Sounds the speaker for a time specified in microseconds by
// duration at a pitch specified in hertz by frequency.
//
void Beep( int frequency, int duration )
{

    int control;

    // If frequency is 0, Beep doesn't try to make a sound. It
    // just sleeps for the duration.

    if( frequency )
    {
        // 75 is about the shortest reliable duration of a sound.
        if( duration < 75 )
            duration = 75;

        // Prepare timer by sending 10111100 to port 43.
        _outp( 0x43, 0xb6 );

        // Divide input frequency by timer ticks per second and
        // write (byte by byte) to timer.

        frequency = (unsigned)(1193180L / frequency);
        _outp( 0x42, (char)frequency );
        _outp( 0x42, (char)(frequency >> 8) );

        // Save speaker control byte.
        control = _inp( 0x61 );

        // Turn on the speaker (with bits 0 and 1).
        _outp( 0x61, control | 0x3 );
    }

    Sleep( (clock_t)duration );

    // Turn speaker back on if necessary.
    if( frequency )
        _outp( 0x61, control );

}

void FlushProgressInfo() 
{
	char 	str[256];
	short	oldcolor;
	int		npresent,nHour,nMin,nSec;
	long    nElasped;
	long    nRemain;
	
    g_finish = clock();
    nElasped = (g_finish - g_start)/CLOCKS_PER_SEC;
	
	if (nElasped != g_nElasped)
	{
		g_nElasped = nElasped;

		npresent = int(g_dDeletedSize*100/g_dTotalSize);
		if (npresent > 100) npresent = 100;

		_settextposition(9, 6);     
		sprintf(str,"Finished : %d%%",npresent);
		_outtext(str);

		nHour = (int)(g_nElasped/3600);
		nMin = (int)((g_nElasped%3600)/60);
		nSec = (int)(g_nElasped%60);
		_settextposition(10, 6);     
		sprintf(str,"Time elapsed : %d:%2.2d:%2.2d",nHour,nMin,nSec);
		_outtext(str);
                    
		nRemain  = (long)((g_dTotalSize-g_dDeletedSize)*g_nElasped/g_dDeletedSize);
		nHour = (int)(nRemain/3600);
		nMin = (int)((nRemain%3600)/60);
		nSec = (int)(nRemain%60);
		_settextposition(10, 36);     
		sprintf(str,"Time remained : %d:%2.2d:%2.2d",nHour,nMin,nSec);
		_outtext(str);

		_settextposition(12, 6);     
		oldcolor = _settextcolor(RED);
		nElasped = ((g_SysInfo.nmaxcols - 10) * npresent) / 100;
		charstostr(str,PROGRESS_CHAR,(int)nElasped);
		_outtext(str);              
		_settextcolor(oldcolor);
	}

	if((npresent == 100) && (!g_bBeepOff))
	{
		Beep(1000,500);
		Beep(1000,500);
	}
}
