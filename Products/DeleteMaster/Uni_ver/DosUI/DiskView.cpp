#include "comm.h"
#include "main.h"

extern 	_sysinfo					g_SysInfo;     
extern	_diskviewinfo				g_diskviewinfo;
extern 	int 						g_nDiskNum;
		cwindow*					g_pDiskViewDiskWin;
		cwindow*					g_pDiskViewInfoWin;

void DeleteDiskViewWin();
void SetDiskSelecter(int nSelItem);
void showchs(BIOS_DRIVE_PARAM *pDriveParam);
		
char showdiskviewwin()
{
	int					i;
	int					nWidth,nErr;
	char				szdrv[40];
	char				ch;
	char				*str[] = {"[S]elect sector"};
	static point		ptselsec;
	BIOS_DRIVE_PARAM    DriveParam;
	
	_setvisualpage(PAGE_DISKVIEW);  
	_setactivepage(PAGE_DISKVIEW);  
    
    ptselsec.row = 5;
    ptselsec.col = 64;
    
    drawbox(1, 1, g_SysInfo.nmaxrows, g_SysInfo.nmaxcols);
    
   	_settextposition(2, 4); 
	_outtext("Sector information of hard disk");
    
    //Create and init disk selecter window.
    g_pDiskViewDiskWin = new cwindow(PAGE_DISKVIEW, 3, 4, 7, 35);
	//for(i = 0; i < g_nDiskNum; i++) 
	for(i = 0; i < g_nDiskNum; i++) 
	{            
		GetDriveParam(i+DISKBASE,&DriveParam);
		sprintf(szdrv, DISKFORMAT, i + 1,DriveParam.dwSectorL/2048);
		g_pDiskViewDiskWin->additemNotSort(szdrv,i); 
	}
	g_pDiskViewDiskWin->enabledscroll();
	if(!g_diskviewinfo.nSelDisk)
		g_diskviewinfo.nSelDisk = g_pDiskViewDiskWin->m_pitemactive->btItem+DISKBASE;    
	GetDriveParam(g_diskviewinfo.nSelDisk,&DriveParam);
	SetDiskSelecter(g_diskviewinfo.nSelDisk-DISKBASE);

	if(g_diskviewinfo.dwMinSec == g_diskviewinfo.dwMaxSec)
	{
		g_diskviewinfo.dwMinSec = 0;
		g_diskviewinfo.dwMaxSec = DriveParam.dwSectorL-1;
	}
	//Show selected disk number.
   	_settextposition(2, 36); 
	sprintf(szdrv,DISKVIEWDISKINFO,g_diskviewinfo.nSelDisk-DISKBASE +1,
			g_diskviewinfo.dwMinSec,g_diskviewinfo.dwMaxSec);
	_outtext(szdrv);

    //Create and init show selected sector detail window.
    g_pDiskViewInfoWin = new cwindow(PAGE_DISKVIEW,9,2,23,79);
	ShowSectorInfo(g_pDiskViewInfoWin,g_diskviewinfo.dwSelSec,g_diskviewinfo.nSelDisk,&DriveParam,&nErr);
	g_pDiskViewInfoWin->enabledscroll();
	if(g_diskviewinfo.nselect == 2)
		winproc(g_pDiskViewInfoWin, WM_ACTIVE);
	
	//Init and Show absolute sector
	_settextposition(8,4);
	_outtext("Physical Sector: Absolute Sector ");

	_settextposition(8,37);
	sprintf(szdrv,ABSLUTESECTOR,g_diskviewinfo.dwSelSec);
	_outtext(szdrv);

	//Add function box
	nWidth = strlen(str[0]);	                                 
	_settextposition(ptselsec.row, ptselsec.col);
	_outtext(str[0]);  
	drawbox(ptselsec.row - 1, ptselsec.col - 1, ptselsec.row + 1, ptselsec.col + nWidth);
	
   	_settextposition(24, 45); 
	_outtext("PageUp/PageDown:Prev/Next sector");
	showchs(&DriveParam);
	//Set active item
	switch(g_diskviewinfo.nselect)
	{
	case 0:
		winproc(g_pDiskViewDiskWin, WM_ACTIVE);
		break;
	case 1:
		if(g_diskviewinfo.nselect == 1)	selectstr(str[0], ptselsec.row, ptselsec.col, SELCOLOR);
		winproc(g_pDiskViewDiskWin, WM_INACTIVE);
		break;
	case 2:
		winproc(g_pDiskViewInfoWin, WM_ACTIVE);
		winproc(g_pDiskViewDiskWin, WM_INACTIVE);
		break;
	}

	while(1)
	{                                               
	  	ch = _getch();
		switch(ch)
		{
		case 's':
		case 'S':
			_clearscreen(_GCLEARSCREEN);
			DeleteDiskViewWin();
			return 's';			
		case 'C':
		case 'c':
		case ESC :
			_clearscreen(_GCLEARSCREEN); 
			DeleteDiskViewWin();
			memset(&g_diskviewinfo,0,sizeof(_diskviewinfo));
			return 'c';
		case CR:
			if(g_diskviewinfo.nselect == 1)
			{
				_clearscreen(_GCLEARSCREEN);
				DeleteDiskViewWin();
				return 's';			
			}
			break;
		case TAB:
			switch(g_diskviewinfo.nselect)
			{
			case 0:
				g_diskviewinfo.nselect = 1;
				selectstr(str[0], ptselsec.row, ptselsec.col, SELCOLOR);
				winproc(g_pDiskViewDiskWin, WM_INACTIVE);                                        
				break;
			case 1:
				g_diskviewinfo.nselect = 2;  
				selectstr(str[0], ptselsec.row, ptselsec.col, TEXTCOLOR);
				winproc(g_pDiskViewInfoWin, WM_ACTIVE);                                        
				break;
			case 2:
				g_diskviewinfo.nselect = 0;
				winproc(g_pDiskViewDiskWin, WM_ACTIVE);                                        
				winproc(g_pDiskViewInfoWin, WM_INACTIVE);                                        
				break;
			}
			break;
		case 0:
			switch(ch = _getch())
			{
			case SHIFT_TAB:
				switch(g_diskviewinfo.nselect)
				{
				case 0:
					g_diskviewinfo.nselect = 2;  
					winproc(g_pDiskViewDiskWin, WM_INACTIVE);                                        
					winproc(g_pDiskViewInfoWin, WM_ACTIVE);                                        
					break;
				case 1:
					g_diskviewinfo.nselect = 0;
					selectstr(str[0], ptselsec.row, ptselsec.col, TEXTCOLOR);
					winproc(g_pDiskViewDiskWin, WM_ACTIVE);                                        
					break;
				case 2:
					g_diskviewinfo.nselect = 1;
					selectstr(str[0], ptselsec.row, ptselsec.col, SELCOLOR);
					winproc(g_pDiskViewInfoWin, WM_INACTIVE);                                        
					break;
				}
				break;
			case PAGEDOWN:
				if(g_diskviewinfo.dwSelSec != g_diskviewinfo.dwMaxSec)
				{
					g_diskviewinfo.dwSelSec ++;
					_settextposition(8,37);
					_outtext(NULLSTRING);
					_settextposition(8,37);
					sprintf(szdrv,ABSLUTESECTOR,g_diskviewinfo.dwSelSec);
					_outtext(szdrv);
					ShowSectorInfo(g_pDiskViewInfoWin,g_diskviewinfo.dwSelSec,
									g_diskviewinfo.nSelDisk,&DriveParam,&nErr);
					g_pDiskViewInfoWin->enabledscroll();
					if(g_diskviewinfo.nselect == 2)
					{
						winproc(g_pDiskViewInfoWin, WM_ACTIVE);
					}
					showchs(&DriveParam);
				}
				break;
			case PAGEUP:
				if(g_diskviewinfo.dwSelSec != g_diskviewinfo.dwMinSec)
				{
					g_diskviewinfo.dwSelSec --;
					_settextposition(8,37);
					_outtext(NULLSTRING);
					_settextposition(8,37);
					sprintf(szdrv,ABSLUTESECTOR,g_diskviewinfo.dwSelSec);
					_outtext(szdrv);

					ShowSectorInfo(g_pDiskViewInfoWin,g_diskviewinfo.dwSelSec,
									g_diskviewinfo.nSelDisk,&DriveParam,&nErr);
					g_pDiskViewInfoWin->enabledscroll();
					if(g_diskviewinfo.nselect == 2)
					{
						winproc(g_pDiskViewInfoWin, WM_ACTIVE);
					}
					showchs(&DriveParam);
				}
				break;
			case CTRL_END:
			case CTRL_HOME:  
			case UP:
			case DOWN:
				if(g_diskviewinfo.nselect == 0 && !g_diskviewinfo.bDisableSwitchDisk)
				{
					g_pDiskViewDiskWin->m_pitemactive->szitemtext[1] = ' ';
					writestr(PAGE_DISKVIEW, 3+g_pDiskViewDiskWin->m_nactiverow , 6, " ");
					winproc(g_pDiskViewDiskWin, WM_KEY, ch);
					g_pDiskViewDiskWin->m_pitemactive->szitemtext[1] = '*';
					writestr(PAGE_DISKVIEW, 3+g_pDiskViewDiskWin->m_nactiverow , 6, "*");

					g_diskviewinfo.nSelDisk = g_pDiskViewDiskWin->m_pitemactive->btItem+DISKBASE;    
					GetDriveParam(g_diskviewinfo.nSelDisk,&DriveParam);
					
					g_diskviewinfo.dwSelSec = 0;
					g_diskviewinfo.dwMinSec = 0;
					g_diskviewinfo.dwMaxSec = DriveParam.dwSectorL-1;

					_settextposition(2,36);
					_outtext(NULLSTRING);
				   	_settextposition(2, 36); 
					sprintf(szdrv,DISKVIEWDISKINFO,g_diskviewinfo.nSelDisk-DISKBASE +1,
							g_diskviewinfo.dwMinSec,g_diskviewinfo.dwMaxSec);
					_outtext(szdrv);

					_settextposition(8,37);
					_outtext(NULLSTRING);
					_settextposition(8,37);
					sprintf(szdrv,ABSLUTESECTOR,g_diskviewinfo.dwSelSec);
					_outtext(szdrv);

					ShowSectorInfo(g_pDiskViewInfoWin,g_diskviewinfo.dwSelSec,
								g_diskviewinfo.nSelDisk,&DriveParam,&nErr);
					g_pDiskViewInfoWin->enabledscroll();
					showchs(&DriveParam);
				}
				if(g_diskviewinfo.nselect == 2)
				{
					winproc(g_pDiskViewInfoWin, WM_KEY, ch);
				}
				break;
			}
			break;
		}
	}

}

void DeleteDiskViewWin()
{
    delete g_pDiskViewDiskWin;
    delete g_pDiskViewInfoWin;
}

void SetDiskSelecter(int nSelItem)
{
	while(g_pDiskViewDiskWin->m_pitemactive->btItem != nSelItem)
	{
		winproc(g_pDiskViewDiskWin, WM_KEY, DOWN);
	}
	g_pDiskViewDiskWin->m_pitemactive->szitemtext[1] = '*';
	writestr(PAGE_DISKVIEW, 3+g_pDiskViewDiskWin->m_nactiverow , 6, "*");
}

void showchs(BIOS_DRIVE_PARAM *pDriveParam)
{
	char		str[40];
	DWORD 		dwHead,dwSector,dwCylinder;
	
	dwHead		= (((g_diskviewinfo.dwSelSec/pDriveParam->dwSecPerTrack)%pDriveParam->dwHeads)&0xff); 
	dwSector 	= ((g_diskviewinfo.dwSelSec % pDriveParam->dwSecPerTrack)+1)&0xff;
	dwCylinder	= (g_diskviewinfo.dwSelSec/pDriveParam->dwSecPerTrack)/pDriveParam->dwHeads; 
	
	sprintf(str, DISKCHS, dwCylinder,dwHead,dwSector);
   	_settextposition(24, 2); 
	_outtext(NULLSTRING);
   	_settextposition(24, 2); 
	_outtext(str);
}