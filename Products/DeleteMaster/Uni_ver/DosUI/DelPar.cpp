#include "comm.h"
#include "main.h"

extern 	_sysinfo					g_SysInfo;     
extern	int							g_nMethod;
extern	YG_PARTITION_INFO			*g_pCurFixDiskInfo;
extern	_deleteparinfo              g_delparinfo;
extern	_diskviewinfo				g_diskviewinfo;
extern 	int 						g_nDiskNum;
		cwindow*					g_pDelParDiskWin;
		cwindow*					g_pDelParPartWin;
		cwindow*					g_pDelParSelWin;

void DeleteDelParWin();
void showtarparinfo();
void SetDelParDiskSel(int nSelItem);
void SetDelParSelDisk();
		
char showdelparwin()
{   
	int				nWidth;
	int				i;
	char			ch;
	char			szResult = 'c';
	char			szdrv[40],szStrCheck[65];
	char			*str[] = {"[V]iew Partition","Delete [M]othed" ,"[D]elete ","[C]ancel"};
	static point	ptvpar, ptoption,ptdelpardel,ptdelparcancel;
	_itemnode 		*pitemHD;
	char			*szMethod[] = {"00 overwrite","FF overwrite",
						"random data","NSA method",
						"Old NSA method","DoD STD method",
						"NATO method","Gutmann method"};
	             

	_setvisualpage(PAGE_DELPAR);  
	_setactivepage(PAGE_DELPAR);  
    
	ptvpar.row = 23;
	ptvpar.col = 5;

    drawbox(1, 1, g_SysInfo.nmaxrows, g_SysInfo.nmaxcols);
    
	_settextposition(2, 4);          
	_outtext("Select the partition to be deleted");
	
	_settextposition(4, 5);          
	_outtext("PhysicalDisk:        Drive            Type   Size(MB)  Active    Pri/Log");
	
	_settextposition(12, 4);
	_outtext("Selected partition");  
	//Add&Init Disk select window
	g_pDelParDiskWin = new cwindow(PAGE_DELPAR, 5, 4, 11, 20);
	for(i = 0; i < g_nDiskNum; i++)   // no A: B:
	{            
		sprintf(szdrv, DRIVEFORMAT, i + 1);
		g_pDelParDiskWin->additemNotSort(szdrv,i); 
	}
	g_pDelParDiskWin->enabledscroll();    
	if(!g_delparinfo.nseldisk)
		g_delparinfo.nseldisk = g_pDelParDiskWin->m_pitemactive->btItem+DISKBASE;    
	SetDelParDiskSel(g_delparinfo.nseldisk-DISKBASE);
    
    //Add&Init Partition select window
	g_pDelParPartWin = new cwindow(PAGE_DELPAR, 5, 22, 11, 78);
	writedriveletter(g_pDelParDiskWin, g_pDelParPartWin);
	
	//Add show selected partition information window
	g_pDelParSelWin = new cwindow(PAGE_DELPAR, 13, 4, 18, 78);
	
	//show delete method
	sprintf(szdrv,"Delete partition with %s",szMethod[g_nMethod]);
	_settextposition(19, 4);
	_outtext(szdrv);  
	
	//Add function box
	nWidth = strlen(str[0]);	                                 
	_settextposition(ptvpar.row, ptvpar.col);
	_outtext(str[0]);  
	drawbox(ptvpar.row - 1, ptvpar.col - 1, ptvpar.row + 1, ptvpar.col + nWidth);

	ptoption.row = ptvpar.row;
	ptoption.col = ptvpar.col + nWidth + 2;
	nWidth = strlen(str[1]);
	_settextposition(ptoption.row, ptoption.col);
	_outtext(str[1]);  
	drawbox(ptoption.row - 1, ptoption.col - 1, ptoption.row + 1, ptoption.col + nWidth);


	nWidth = strlen(str[3]);
	ptdelparcancel.row = ptvpar.row;
	ptdelparcancel.col = g_SysInfo.nmaxcols - 2 - nWidth;	           
	_settextposition(ptdelparcancel.row, ptdelparcancel.col);
	_outtext(str[3]);  
	drawbox(ptdelparcancel.row - 1, ptdelparcancel.col - 1, ptdelparcancel.row + 1, ptdelparcancel.col + nWidth);

	nWidth = strlen(str[2]);
	ptdelpardel.row = ptdelparcancel.row;
	ptdelpardel.col = ptdelparcancel.col - 2 -nWidth;	           
	_settextposition(ptdelpardel.row, ptdelpardel.col);
	_outtext(str[2]);  
	drawbox(ptdelpardel.row - 1, ptdelpardel.col - 1, ptdelpardel.row + 1, ptdelpardel.col + nWidth);
	
	SetDelParSelDisk();
	showtarparinfo();
	winproc(g_pDelParPartWin, WM_INACTIVE);
	switch(g_delparinfo.nselect)
	{
	case 0:
		winproc(g_pDelParDiskWin, WM_ACTIVE);
		break;
	case 1:
		winproc(g_pDelParPartWin, WM_ACTIVE);
		break;
	case 2:
		selectstr(str[0], ptvpar.row, ptvpar.col, SELCOLOR);
		break;
	case 3:
		selectstr(str[1], ptoption.row, ptoption.col, SELCOLOR);
		break;
	case 4:
		selectstr(str[2], ptdelpardel.row, ptdelpardel.col, SELCOLOR);
		break;
	case 5:
		selectstr(str[3], ptdelparcancel.row, ptdelparcancel.col, SELCOLOR);
		break;
	}
	while(1)
	{                                               
	  	ch = _getch();
		switch(ch)
		{
		case 'm':
		case 'M':
			_clearscreen(_GCLEARSCREEN); 
			DeleteDelParWin();
		    return 'm';
		case 'v':
		case 'V':
			if(g_delparinfo.nselpar == -1)
			{
				ErrorMessageBox(NO_SELPAR);
				break;
			}
			_clearscreen(_GCLEARSCREEN); 
			DeleteDelParWin();
			return 'v';
		case 'd':
		case 'D':
			if(g_delparinfo.nselpar == -1)
			{
				ErrorMessageBox(NO_SELPAR);
				break;
			}
			_clearscreen(_GCLEARSCREEN); 
			DeleteDelParWin();
		    return 'd';
		case 'C':
		case 'c':
		case ESC :
			_clearscreen(_GCLEARSCREEN); 
			DeleteDelParWin();
			return 'c';
		case SPACE :
			if(g_delparinfo.nselect == 1)
			{
				pitemHD = g_pDelParPartWin->m_pitemhead;
				while(pitemHD!=NULL)
				{
				    if(pitemHD->szitemtext[SELPOINT] == SELFLAG)//no selected
					{
					    pitemHD->szitemtext[SELPOINT] = ' ';     	
					}
					pitemHD = pitemHD->pnext;
				}
				i=1;
				while(i<6)
				{  
					g_pDelParPartWin->gettext(szStrCheck,i);
					szStrCheck[0]=0x20;
					if(szStrCheck[2]=='X')		
						writestr(PAGE_DELPAR, 5 + i, 23 + SELPOINT, " "); 
					i++;
				}
				if(g_delparinfo.nselpar !=  g_pDelParPartWin->m_pitemactive->btItem)
				{
					g_pDelParPartWin->m_pitemactive->szitemtext[SELPOINT] = SELFLAG;     
					writestr(PAGE_DELPAR, 5 + g_pDelParPartWin->m_nactiverow, 23 + SELPOINT, SELFLAG_STRING); 
					g_delparinfo.nselpar = g_pDelParPartWin->m_pitemactive->btItem;
				}
				else//already selected
				{
					g_delparinfo.nselpar  =-1;  
				}
				showtarparinfo();
			}
			break;
		case 0:
			switch(ch = _getch())
			{
			case SHIFT_TAB:
				switch(g_delparinfo.nselect)
				{
				case 0:
					g_delparinfo.nselect = 5;  
					selectstr(str[3], ptdelparcancel.row, ptdelparcancel.col, SELCOLOR);
					winproc(g_pDelParDiskWin, WM_INACTIVE);
					break;
				case 1:
					g_delparinfo.nselect = 0;
					winproc(g_pDelParDiskWin, WM_ACTIVE);
					winproc(g_pDelParPartWin, WM_INACTIVE);                                        
					break;
				case 2:
					g_delparinfo.nselect = 1;
					selectstr(str[0], ptvpar.row, ptvpar.col, TEXTCOLOR);
					winproc(g_pDelParPartWin, WM_ACTIVE);                                        
					//showtarparinfo();
					break;
				case 3:
					g_delparinfo.nselect = 2;
					selectstr(str[0], ptvpar.row, ptvpar.col, SELCOLOR); 
					selectstr(str[1], ptoption.row, ptoption.col, TEXTCOLOR);
					break;
				case 4:
					g_delparinfo.nselect = 3;
					selectstr(str[1], ptoption.row, ptoption.col, SELCOLOR);
					selectstr(str[2], ptdelpardel.row, ptdelpardel.col, TEXTCOLOR);
					break;
				case 5:
					g_delparinfo.nselect = 4;
					selectstr(str[2], ptdelpardel.row, ptdelpardel.col, SELCOLOR);
					selectstr(str[3], ptdelparcancel.row, ptdelparcancel.col, TEXTCOLOR);
					break;
				}
				break;
			case PAGEDOWN:
			case PAGEUP:
			case CTRL_END:
			case CTRL_HOME:  
			case UP:
			case DOWN:
				if(g_delparinfo.nselect == 0)
				{
					g_pDelParDiskWin->m_pitemactive->szitemtext[1] = ' ';
					writestr(PAGE_DELPAR, 5+g_pDelParDiskWin->m_nactiverow , 6, " ");
					winproc(g_pDelParDiskWin, WM_KEY, ch);
					g_pDelParDiskWin->m_pitemactive->szitemtext[1] = '*';
					writestr(PAGE_DELPAR, 5+g_pDelParDiskWin->m_nactiverow , 6, "*");
					g_delparinfo.nseldisk = g_pDelParDiskWin->m_pitemactive->btItem+DISKBASE;

					writedriveletter(g_pDelParDiskWin, g_pDelParPartWin);
					g_delparinfo.nselpar = -1;
					showtarparinfo();
				}
				if(g_delparinfo.nselect == 1)
				{
					winproc(g_pDelParPartWin, WM_KEY, ch);
					//showtarparinfo();
				}
				break;
			}
			break;
		case TAB:
			switch(g_delparinfo.nselect)
			{
			case 0:
				g_delparinfo.nselect = 1;  
				winproc(g_pDelParDiskWin, WM_INACTIVE);
				winproc(g_pDelParPartWin, WM_ACTIVE);                                        
				//showtarparinfo();
				break;
			case 1:
				g_delparinfo.nselect = 2;
				selectstr(str[0], ptvpar.row, ptvpar.col, SELCOLOR);
				winproc(g_pDelParPartWin, WM_INACTIVE);                                        
				break;
			case 2:
				g_delparinfo.nselect = 3;
				selectstr(str[0], ptvpar.row, ptvpar.col, TEXTCOLOR); 
				selectstr(str[1], ptoption.row, ptoption.col, SELCOLOR);
				break;
			case 3:
				g_delparinfo.nselect = 4;
				selectstr(str[1], ptoption.row, ptoption.col, TEXTCOLOR);
				selectstr(str[2], ptdelpardel.row, ptdelpardel.col, SELCOLOR);
				break;
			case 4:
				g_delparinfo.nselect = 5;
				selectstr(str[2], ptdelpardel.row, ptdelpardel.col, TEXTCOLOR);
				selectstr(str[3], ptdelparcancel.row, ptdelparcancel.col, SELCOLOR);
				break;
			case 5:
				g_delparinfo.nselect = 0;
				selectstr(str[3], ptdelparcancel.row, ptdelparcancel.col, TEXTCOLOR);
				winproc(g_pDelParDiskWin, WM_ACTIVE);
				break;
			}
			break;
		case CR:
			switch(g_delparinfo.nselect)
			{
			case 2:
				if(g_delparinfo.nselpar == -1)
				{
					ErrorMessageBox(NO_SELPAR);
					break;
				}
				_clearscreen(_GCLEARSCREEN); 
				DeleteDelParWin();
				return 'v';
			case 3:
				_clearscreen(_GCLEARSCREEN); 
				DeleteDelParWin();
			    return 'm';
			case 4:
				if(g_delparinfo.nselpar == -1)
				{
					ErrorMessageBox(NO_SELPAR);
					break;
				}
				_clearscreen(_GCLEARSCREEN); 
				DeleteDelParWin();
				return 'd';
			case 5:
				_clearscreen(_GCLEARSCREEN); 
				DeleteDelParWin();
				return 'c';
			}
		default:break;
		}
	}
}

void DeleteDelParWin()
{
	//g_nSelPar = -1;
	delete g_pDelParDiskWin;
	delete g_pDelParPartWin;
	delete g_pDelParSelWin;
}

void SetDelParDiskSel(int nSelItem)
{
	while(g_pDelParDiskWin->m_pitemactive->btItem != nSelItem)
	{
		winproc(g_pDelParDiskWin, WM_KEY, DOWN);
	}
	g_pDelParDiskWin->m_pitemactive->szitemtext[1] = '*';
	writestr(PAGE_DELPAR, 5+g_pDelParDiskWin->m_nactiverow , 6, "*");
}

void SetDelParSelDisk()
{
	if(g_delparinfo.nselpar >= 0)
	{
		while(g_pDelParPartWin->m_pitemactive->btItem != g_delparinfo.nselpar)
		{
			winproc(g_pDelParPartWin, WM_KEY, DOWN);
		}
		g_pDelParPartWin->m_pitemactive->szitemtext[2] = 'X';
		writestr(PAGE_DELPAR, 5+g_pDelParPartWin->m_nactiverow , 25, "X");
	}
}

void showtarparinfo()
{
	int							i;
	YG_PARTITION_INFO			*pDiskInfo = g_pCurFixDiskInfo;
	char						szdriveletter,szselpar[80];
	DWORD						dwStart;
	
    g_pDelParSelWin->emptyitem();
   	g_pDelParSelWin->emptyscreen();

	if(g_delparinfo.nselpar >= 0)
	{
		for(i=0;i<g_delparinfo.nselpar;i++)
		{
			pDiskInfo = pDiskInfo->pNext;
		}
		szdriveletter = pDiskInfo->DriveLetter;
		dwStart 	  = pDiskInfo->dwStartSector;
		
		if(pDiskInfo->DriveLetter < 0x40) szdriveletter = '*';
		if(pDiskInfo->bLogic) dwStart += 0x3f;
	   	
	   	sprintf(szselpar,PARDELSELPAR,szdriveletter,
	   		dwStart,pDiskInfo->dwStartSector+pDiskInfo->dwPartSize-1);
	    g_pDelParSelWin->additem(szselpar);
	    g_diskviewinfo.bDisableSwitchDisk 	= TRUE;
	    g_diskviewinfo.dwMinSec 			= dwStart;
	    g_diskviewinfo.dwMaxSec				= pDiskInfo->dwStartSector+pDiskInfo->dwPartSize-1;
	    g_diskviewinfo.nSelDisk				= g_delparinfo.nseldisk;
	    g_diskviewinfo.dwSelSec				= dwStart;
	}
}
