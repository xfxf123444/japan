#include "comm.h"
#include "main.h"

extern 	_sysinfo					g_SysInfo;     
extern	_diskviewinfo				g_diskviewinfo;

void showselsecnumber(char *szselsec,int ncursor);
void _delchar(int ncharpos,char *szselsec,DWORD *pdwselsec);
void _insertchar(int ncharpos,char *szselsec,DWORD *pdwselsec ,char ch);

char showsectorselwin()
{
	int				ncursor;
	int				nTextLen;
	int				nSelect = 0;
	char			ch,szselsec[11];
	char			szdiskinfo[40];
	char			*str[] = {"[O]k","[C]ancel"};
	static point	ptok, ptcancel;
	DWORD			dwselsec = g_diskviewinfo.dwSelSec;
	
	_setvisualpage(PAGE_DISKVIEW);  
	_setactivepage(PAGE_DISKVIEW);  
    
    drawbox(1, 1, g_SysInfo.nmaxrows, g_SysInfo.nmaxcols);
    
   	_settextposition(2, 4); 
	_outtext("Select physical sector of hard disk");

  	_settextposition(2, 40); 
	sprintf(szdiskinfo,DISKVIEWDISKINFO,g_diskviewinfo.nSelDisk-DISKBASE +1,
			g_diskviewinfo.dwMinSec,g_diskviewinfo.dwMaxSec);
	_outtext(szdiskinfo);

  	_settextposition(10, 12); 
	_outtext("Starting Sector:");
	
	ptcancel.row = 23;
	ptcancel.col = g_SysInfo.nmaxcols - 2 - strlen(str[1]);	           
	_settextposition(ptcancel.row, ptcancel.col);
	_outtext(str[1]); 
	drawbox(ptcancel.row - 1, ptcancel.col - 1, ptcancel.row + 1, g_SysInfo.nmaxcols - 2);
	
	ptok.row = ptcancel.row;
	ptok.col = ptcancel.col - 4 - strlen(str[0]);	           
	_settextposition(ptok.row, ptok.col);
	_outtext(str[0]); 
	drawbox(ptok.row - 1, ptok.col - 1, ptok.row + 1, ptok.col +strlen(str[0]));
    
    sprintf(szselsec,"%lu",dwselsec);
    nTextLen = strlen(szselsec);
    ncursor = nTextLen + 30;
	showselsecnumber(szselsec,ncursor);
    
	while(1)
	{                                               
	  	ch = _getch();
		switch(ch)
		{
		case BACKSPACE:
			if(ncursor > 30 && nSelect == 0)
			{
				nTextLen -- ;
				_delchar(ncursor-31,szselsec,&dwselsec);
				showselsecnumber(szselsec,ncursor);
				ncursor --;
				_settextposition(10, ncursor);
			}
			break;
		case KEY0:
		case KEY1:
		case KEY2:
		case KEY3:
		case KEY4:
		case KEY5:
		case KEY6:
		case KEY7:
		case KEY8:
		case KEY9:
			if(nTextLen < 10 && nSelect == 0)
			{
				_insertchar(ncursor-30,szselsec,&dwselsec,ch);
				showselsecnumber(szselsec,ncursor);
				if(nTextLen == ncursor - 30)
				{
					ncursor ++;
					_settextposition(10, ncursor);
				}
				nTextLen ++ ;
			}
			break;
		case 'o':
		case 'O':
			if(dwselsec < g_diskviewinfo.dwMinSec) dwselsec = g_diskviewinfo.dwMinSec;
			if(dwselsec > g_diskviewinfo.dwMaxSec) dwselsec = g_diskviewinfo.dwMaxSec;
			g_diskviewinfo.dwSelSec = dwselsec;
			_clearscreen(_GCLEARSCREEN); 
			_displaycursor(_GCURSOROFF);
			return 'o';
		case 'C':
		case 'c':
		case ESC :
			_clearscreen(_GCLEARSCREEN); 
			_displaycursor(_GCURSOROFF);
			return 'c';
		case TAB:
			switch(nSelect)
			{
			case 0:
				nSelect = 1;
				_displaycursor(_GCURSOROFF);
				selectstr(str[0], ptok.row, ptok.col, SELCOLOR);
				break;
			case 1:
				nSelect = 2;
				selectstr(str[0], ptok.row, ptok.col, TEXTCOLOR);
				selectstr(str[1], ptcancel.row, ptcancel.col, SELCOLOR);
				break;
			case 2:
				nSelect = 0;
				selectstr(str[1], ptcancel.row, ptcancel.col, TEXTCOLOR);
				_settextposition(10, ncursor);
				_displaycursor(_GCURSORON);
				break;
			}
			break;
		case CR:
			switch(nSelect)
			{
			case 1:
				if(dwselsec < g_diskviewinfo.dwMinSec) dwselsec = g_diskviewinfo.dwMinSec;
				if(dwselsec > g_diskviewinfo.dwMaxSec) dwselsec = g_diskviewinfo.dwMaxSec;
				g_diskviewinfo.dwSelSec = dwselsec;
				_clearscreen(_GCLEARSCREEN); 
				_displaycursor(_GCURSOROFF);
				return 'o';
			case 2:
				_clearscreen(_GCLEARSCREEN); 
				_displaycursor(_GCURSOROFF);
				return 'c';
			}
			break;
		case 0:
			switch(ch = _getch())
			{
			case DEL:
				if(nSelect == 0)
				{
					if(ncursor-30 < nTextLen && nTextLen > 0)
					{
						nTextLen -- ;
						_delchar(ncursor-30,szselsec,&dwselsec);
						showselsecnumber(szselsec,ncursor);
					}
				}
				break;
			case RIGHT:
				if(nSelect == 0)
				{
					if(ncursor != nTextLen+30)
					{
						ncursor ++;
						_settextposition(10, ncursor);
					}
				}
				break;
			case LEFT:
				if(nSelect == 0)
				{
					if(ncursor != 30)
					{
						ncursor --;
						_settextposition(10, ncursor);
					}
				}
				break;
			case SHIFT_TAB:
				switch(nSelect)
				{
				case 0:
					nSelect = 2;
					_displaycursor(_GCURSOROFF);
					selectstr(str[1], ptcancel.row, ptcancel.col, SELCOLOR);
					break;
				case 1:
					nSelect = 0;
					selectstr(str[0], ptok.row, ptok.col, TEXTCOLOR);
					_settextposition(10, ncursor);
					_displaycursor(_GCURSORON);
					break;
				case 2:
					nSelect = 1;
					selectstr(str[1], ptcancel.row, ptcancel.col, TEXTCOLOR);
					selectstr(str[0], ptok.row, ptok.col, SELCOLOR);
					break;
				}
				break;
			}
			break;
		default: break;
		}
	}
}

void showselsecnumber(char *szselsec,int ncursor)
{
	long			oldbdg;

    oldbdg = _getbkcolor();
  	_settextposition(10, 29); 
	_outtext("[");

    _setbkcolor( RED );
  	_settextposition(10, 30); 
	_outtext("..........");

  	_settextposition(10, 30); 
	_outtext(szselsec);
    _setbkcolor( oldbdg );

  	_settextposition(10, 40); 
	_outtext("]");

  	_settextposition(10, ncursor); 
	_displaycursor(_GCURSORON);                          
}

void _delchar(int ncharpos,char *szselsec,DWORD *pdwselsec)
{
	char		*p;
	char   		*stopstring;
	
	p 	= szselsec + ncharpos;
	*p 	= 0;
	strcat(szselsec,p+1);
	*pdwselsec = strtoul(szselsec,&stopstring,0);
}

void _insertchar(int ncharpos,char *szselsec,DWORD *pdwselsec ,char ch)
{
	char		*p;
	char		str[11];
	char   		*stopstring;
	
	p 	= szselsec + ncharpos;
	strcpy(str,p);
	*p 	= ch;
	*(p+1) = 0;
	strcat(szselsec,str);
	*pdwselsec = strtoul(szselsec,&stopstring,10);
}