#include "comm.h"
#include "main.h"

extern 	_sysinfo					g_SysInfo;     
extern  int							g_nMethod;
extern	YG_PARTITION_INFO			*g_pCurFixDiskInfo;
extern	_deletediskinfo				g_deldiskinfo;
extern	_diskviewinfo				g_diskviewinfo;
extern 	int 						g_nDiskNum;
		cwindow*					g_pDelDiskWin;
		cwindow*					g_pDelDiskSelWin;

void DeleteDelDiskWin();
void SetDelDiskSel(int nSelItem);
void showandsettardiskinfo();

char showdeldiskwin()
{
	char				ch,szdrv[40];
	int					i,nWidth,nselect = 0;
	BIOS_DRIVE_PARAM    DriveParam;
	char				*str[] = {"[V]iew Disk","Delete [M]thod" ,"[I]dentify","[D]elete ","[C]ancel"};
	static point		ptvdisk, ptoption,ptidentify,ptdeldiskdel,ptdeldiskcancel;
	char				*szMethod[] = {"00 overwrite","FF overwrite",
						"random data overwrite","NSA method",
						"Old NSA method","DoD STD method",
						"NATO method","Gutmann method"};
	 
	_setvisualpage(PAGE_DELDISK);  
	_setactivepage(PAGE_DELDISK);  
	ptvdisk.row = 23;
	ptvdisk.col = 5;
    
    drawbox(1, 1, g_SysInfo.nmaxrows, g_SysInfo.nmaxcols);

	_settextposition(2, 4);          
	_outtext("Select the disk to be deleted");
    
	_settextposition(4, 5);          
	_outtext("PhysicalDisk:");

	_settextposition(12, 4);
	_outtext("Selected disk");  

	//Add&Init Disk select window
	g_pDelDiskWin = new cwindow(PAGE_DELDISK, 5, 4, 11, 35);
	for(i = 0; i < g_nDiskNum; i++)   // no A: B:
	{            
		GetDriveParam(i+DISKBASE,&DriveParam);
		sprintf(szdrv, DISKFORMAT, i + 1,DriveParam.dwSectorL/2048);
		g_pDelDiskWin->additemNotSort(szdrv,i); 
	}
	g_pDelDiskWin->enabledscroll();    
	if(!g_deldiskinfo.nSelDisk)
		g_deldiskinfo.nSelDisk = g_pDelDiskWin->m_pitemactive->btItem+DISKBASE;    
	SetDelDiskSel(g_deldiskinfo.nSelDisk - DISKBASE);
	
	//Add show selected partition information window
	g_pDelDiskSelWin = new cwindow(PAGE_DELDISK, 13, 4, 18, 78);
	
	//show delete method
	sprintf(szdrv,"Delete disk with %s",szMethod[g_nMethod]);
	_settextposition(19, 4);
	_outtext(szdrv);  

	//Add function button
	nWidth = strlen(str[0]);	                                 
	_settextposition(ptvdisk.row, ptvdisk.col);
	_outtext(str[0]);  
	drawbox(ptvdisk.row - 1, ptvdisk.col - 1, ptvdisk.row + 1, ptvdisk.col + nWidth);

	ptoption.row = ptvdisk.row;
	ptoption.col = ptvdisk.col + nWidth + 2;
	nWidth = strlen(str[1]);
	_settextposition(ptoption.row, ptoption.col);
	_outtext(str[1]);  
	drawbox(ptoption.row - 1, ptoption.col - 1, ptoption.row + 1, ptoption.col + nWidth);

	ptidentify.row = ptoption.row;
	ptidentify.col = ptoption.col + nWidth + 2;
	nWidth = strlen(str[2]);
	_settextposition(ptidentify.row, ptidentify.col);
	_outtext(str[2]);  
	drawbox(ptidentify.row - 1, ptidentify.col - 1, ptidentify.row + 1, ptidentify.col + nWidth);

	nWidth = strlen(str[4]);
	ptdeldiskcancel.row = ptvdisk.row;
	ptdeldiskcancel.col = g_SysInfo.nmaxcols - 2 - nWidth;	           
	_settextposition(ptdeldiskcancel.row, ptdeldiskcancel.col);
	_outtext(str[4]);  
	drawbox(ptdeldiskcancel.row - 1, ptdeldiskcancel.col - 1, ptdeldiskcancel.row + 1, ptdeldiskcancel.col + nWidth);

	nWidth = strlen(str[3]);
	ptdeldiskdel.row = ptdeldiskcancel.row;
	ptdeldiskdel.col = ptdeldiskcancel.col - 2 -nWidth;	           
	_settextposition(ptdeldiskdel.row, ptdeldiskdel.col);
	_outtext(str[3]);  
	drawbox(ptdeldiskdel.row - 1, ptdeldiskdel.col - 1, ptdeldiskdel.row + 1, ptdeldiskdel.col + nWidth);
    
	showandsettardiskinfo();
    //Active selected item 
	switch(g_deldiskinfo.nselect)
	{
	case 0:
		winproc(g_pDelDiskWin, WM_ACTIVE);
		break;
	case 1:
		selectstr(str[0], ptvdisk.row, ptvdisk.col, SELCOLOR);
		break;
	case 2:
		selectstr(str[1], ptoption.row, ptoption.col, SELCOLOR);
		break;
	case 3:
		selectstr(str[2], ptidentify.row, ptidentify.col, SELCOLOR);
		break;
	case 4:
		selectstr(str[3], ptdeldiskdel.row, ptdeldiskdel.col, SELCOLOR);
		break;
	case 5:
		selectstr(str[4], ptdeldiskcancel.row, ptdeldiskcancel.col, SELCOLOR);
		break;
	}
	while(1)
	{                                               
	  	ch = _getch();
		switch(ch)
		{
		case 'i':
		case 'I':
			_clearscreen(_GCLEARSCREEN);
			DeleteDelDiskWin();
			return 'i';
		case 'v':
		case 'V':
			_clearscreen(_GCLEARSCREEN); 
			DeleteDelDiskWin();
			return 'v';		
		case 'd':
		case 'D':
			_clearscreen(_GCLEARSCREEN); 
			DeleteDelDiskWin();
		    return 'd';
		case 'm':
		case 'M':
			_clearscreen(_GCLEARSCREEN); 
			DeleteDelDiskWin();
		    return 'm';
		case 'c':
		case 'C':
		case ESC:
			_clearscreen(_GCLEARSCREEN); 
    		DeleteDelDiskWin();
			return 'c'; 
		case TAB:
			switch(g_deldiskinfo.nselect)
			{
			case 0:
				g_deldiskinfo.nselect = 1;  
				winproc(g_pDelDiskWin, WM_INACTIVE);
				selectstr(str[0], ptvdisk.row, ptvdisk.col, SELCOLOR);
				break;
			case 1:
				g_deldiskinfo.nselect = 2;
				selectstr(str[0], ptvdisk.row, ptvdisk.col, TEXTCOLOR);
				selectstr(str[1], ptoption.row, ptoption.col, SELCOLOR);
				break;
			case 2:
				g_deldiskinfo.nselect = 3;
				selectstr(str[1], ptoption.row, ptoption.col, TEXTCOLOR);
				selectstr(str[2], ptidentify.row, ptidentify.col, SELCOLOR);
				break;
			case 3:
				g_deldiskinfo.nselect = 4;
				selectstr(str[2], ptidentify.row, ptidentify.col, TEXTCOLOR);
				selectstr(str[3], ptdeldiskdel.row, ptdeldiskdel.col, SELCOLOR);
				break;
			case 4:
				g_deldiskinfo.nselect = 5;
				selectstr(str[3], ptdeldiskdel.row, ptdeldiskdel.col, TEXTCOLOR);
				selectstr(str[4], ptdeldiskcancel.row, ptdeldiskcancel.col, SELCOLOR);
				break;
			case 5:
				g_deldiskinfo.nselect = 0;
				winproc(g_pDelDiskWin, WM_ACTIVE);
				selectstr(str[4], ptdeldiskcancel.row, ptdeldiskcancel.col, TEXTCOLOR);
				break;
			}
			break;
		case CR:
			switch(g_deldiskinfo.nselect)
			{
			case 1:
				_clearscreen(_GCLEARSCREEN); 
				DeleteDelDiskWin();
				return 'v';
			case 2:
				_clearscreen(_GCLEARSCREEN); 
				DeleteDelDiskWin();
				return 'm';
			case 3:
				_clearscreen(_GCLEARSCREEN); 
				DeleteDelDiskWin();
				return 'i';
			case 4:
				_clearscreen(_GCLEARSCREEN); 
				DeleteDelDiskWin();
				return 'd';
			case 5:
				_clearscreen(_GCLEARSCREEN); 
				DeleteDelDiskWin();
				return 'c';
			}
			break;
		case 0:
			switch(ch = _getch())
			{
			case SHIFT_TAB:
				switch(g_deldiskinfo.nselect)
				{
				case 0:
					g_deldiskinfo.nselect = 5;  
					winproc(g_pDelDiskWin, WM_INACTIVE);
					selectstr(str[4], ptdeldiskcancel.row, ptdeldiskcancel.col, SELCOLOR);
					break;
				case 1:
					g_deldiskinfo.nselect = 0;
					winproc(g_pDelDiskWin, WM_ACTIVE);
					selectstr(str[0], ptvdisk.row, ptvdisk.col, TEXTCOLOR);
					break;
				case 2:
					g_deldiskinfo.nselect = 1;
					selectstr(str[0], ptvdisk.row, ptvdisk.col, SELCOLOR);
					selectstr(str[1], ptoption.row, ptoption.col, TEXTCOLOR);
					break;
				case 3:
					g_deldiskinfo.nselect = 2;
					selectstr(str[2], ptidentify.row, ptidentify.col, TEXTCOLOR);
					selectstr(str[1], ptoption.row, ptoption.col, SELCOLOR);
					break;
				case 4:
					g_deldiskinfo.nselect = 3;
					selectstr(str[3], ptdeldiskdel.row, ptdeldiskdel.col, TEXTCOLOR);
					selectstr(str[2], ptidentify.row, ptidentify.col, SELCOLOR);
					break;
				case 5:
					g_deldiskinfo.nselect = 4;
					selectstr(str[4], ptdeldiskcancel.row, ptdeldiskcancel.col, TEXTCOLOR);
					selectstr(str[3], ptdeldiskdel.row, ptdeldiskdel.col, SELCOLOR);
					break;
				}
				break;
			case UP:
			case DOWN:
				if(g_deldiskinfo.nselect == 0)
				{
					g_pDelDiskWin->m_pitemactive->szitemtext[1] = ' ';
					writestr(PAGE_DELDISK, 5+g_pDelDiskWin->m_nactiverow , 6, " ");
					winproc(g_pDelDiskWin, WM_KEY, ch);
					g_pDelDiskWin->m_pitemactive->szitemtext[1] = '*';
					writestr(PAGE_DELDISK, 5+g_pDelDiskWin->m_nactiverow , 6, "*");
					g_deldiskinfo.nSelDisk = g_pDelDiskWin->m_pitemactive->btItem+DISKBASE;
					
					showandsettardiskinfo();
				}
				break;
			}
			break;
		}
	}
}

void DeleteDelDiskWin()
{
	delete g_pDelDiskWin;
}

void SetDelDiskSel(int nSelItem)
{
	while(g_pDelDiskWin->m_pitemactive->btItem != nSelItem)
	{
		winproc(g_pDelDiskWin, WM_KEY, DOWN);
	}
	g_pDelDiskWin->m_pitemactive->szitemtext[1] = '*';
	writestr(PAGE_DELDISK, 5+g_pDelDiskWin->m_nactiverow , 6, "*");
}

void showandsettardiskinfo()
{
	char				szseldisk[80];
	BIOS_DRIVE_PARAM    DriveParam;
	
    g_pDelDiskSelWin->emptyitem();
   	g_pDelDiskSelWin->emptyscreen();

	GetDriveParam(g_deldiskinfo.nSelDisk,&DriveParam);
   	sprintf(szseldisk,DISKSELINFO,g_deldiskinfo.nSelDisk-DISKBASE+1,
	   		DriveParam.dwCylinders,DriveParam.dwHeads,DriveParam.dwSecPerTrack);
    g_pDelDiskSelWin->additem(szseldisk);
	
    g_diskviewinfo.bDisableSwitchDisk 	= TRUE;
    g_diskviewinfo.dwMinSec 			= 0;
    g_diskviewinfo.dwMaxSec				= DriveParam.dwSectorL-1;
    g_diskviewinfo.nSelDisk				= g_deldiskinfo.nSelDisk;
    g_diskviewinfo.dwSelSec				= 0;
    
    g_deldiskinfo.dwDiskSize			= DriveParam.dwSectorL;
    g_deldiskinfo.dwCylinders			= DriveParam.dwCylinders;
    g_deldiskinfo.dwHeads				= DriveParam.dwHeads;
    g_deldiskinfo.dwSecPerTrack			= DriveParam.dwSecPerTrack;
}