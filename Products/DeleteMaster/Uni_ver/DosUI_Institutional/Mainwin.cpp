#include "comm.h"
#include "main.h"

extern _sysinfo					g_SysInfo;     

char showmainwin()
{                    
	static point	ptdelpar, ptdeldisk,ptdiskview,pLogDrive;
	
	char	*str[] = {"DeleteMaster",
					  "FRONTLINE Inc.",
					  "Delete [P]artition",
					  "Delete [D]isk",
					  "Disk [V]iewer",
					  "Change [L]og Drive"},
			strhelp[] = "TAB   -- Next        SHIFT+TAB -- Back",
			strhelp1[]= "ENTER -- Select      Space bar -- <X>",
			strhelp2[]= "ESC   -- Exit",
			ch;					  
	int		nWidth,nWidthPar;
	int		nCurSelete;                      
	
	_setvisualpage(PAGE_MAINWIN); 
	_setactivepage(PAGE_MAINWIN); 
	
	_settextposition(21, 5);
	_outtext(strhelp);
	              
	_settextposition(22, 5);
	_outtext(strhelp1);

	_settextposition(23, 5);
	_outtext(strhelp2);

	drawbox(1, 1, g_SysInfo.nmaxrows, g_SysInfo.nmaxcols);//draw agle
	_settextposition(5,11);
	_outtext(str[0]);
	_settextposition(5, g_SysInfo.nmaxcols - strlen(str[1]) - 8);
	_outtext(str[1]);  
	                        
	nWidthPar = strlen(str[2]);	                                 
	ptdelpar.row = 8;//g_SysInfo.nmaxrows / 2 + 1;
	ptdelpar.col = (g_SysInfo.nmaxcols  - nWidthPar) / 2;    
	_settextposition(ptdelpar.row, ptdelpar.col);
	_outtext(str[2]);  
	drawbox(ptdelpar.row - 1, ptdelpar.col - 1, ptdelpar.row + 1, ptdelpar.col + nWidthPar);
	selectstr(str[2], ptdelpar.row, ptdelpar.col, SELCOLOR);
	nCurSelete = 1;
    
	nWidth = strlen(str[3]);	
	ptdeldisk.row = 11;                                
	ptdeldisk.col = (g_SysInfo.nmaxcols - nWidth) / 2;    
	_settextposition(ptdeldisk.row, ptdeldisk.col);
	_outtext(str[3]);
	drawbox(ptdeldisk.row - 1, ptdelpar.col - 1, ptdeldisk.row + 1, ptdelpar.col + nWidthPar);
	
	nWidth = strlen(str[4]);	
	ptdiskview.row = 14;                                
	ptdiskview.col = (g_SysInfo.nmaxcols - nWidth) / 2;    
	_settextposition(ptdiskview.row, ptdiskview.col);
	_outtext(str[4]);
	drawbox(ptdiskview.row - 1, ptdelpar.col - 1, ptdiskview.row + 1, ptdelpar.col + nWidthPar);
	
	nWidth = strlen(str[5]);	
	pLogDrive.row = 17;                                
	pLogDrive.col = (g_SysInfo.nmaxcols - nWidth) / 2;    
	_settextposition(pLogDrive.row, pLogDrive.col);
	_outtext(str[5]);
	drawbox(pLogDrive.row - 1, ptdelpar.col - 1, pLogDrive.row + 1, ptdelpar.col + nWidthPar);

	while(1)
	{
		ch = _getch();    
		switch(ch)
		{
		case 'D':
		case 'd':
			return 'd';
		case 'V':
		case 'v':
			return 'v';
		case 'P':
		case 'p':
			return 'p';  
		case 'l':
		case 'L':
			return 'l';
		case ESC:
			return ESC;//esc--exit	
		case CR:
			switch(nCurSelete)
			{            
			case 1:
				return 'p';
			case 2:
				return 'd';
			case 3:
				return 'v';
			case 4:
				return 'l';
			}					
			break;
		case TAB://tab            
			switch(nCurSelete)
			{            
			case 1:
				selectstr(str[2], ptdelpar.row, ptdelpar.col, TEXTCOLOR);
				nCurSelete = 2;
				selectstr(str[3], ptdeldisk.row, ptdeldisk.col, SELCOLOR);
				break;
			case 2:
				selectstr(str[3], ptdeldisk.row, ptdeldisk.col, TEXTCOLOR);
				nCurSelete = 3;
				selectstr(str[4], ptdiskview.row, ptdiskview.col, SELCOLOR);
				break;
			case 3:
				selectstr(str[4], ptdiskview.row, ptdiskview.col, TEXTCOLOR);
				nCurSelete = 4;
				selectstr(str[5], pLogDrive.row, pLogDrive.col, SELCOLOR);
				break;
			case 4:
				selectstr(str[5], pLogDrive.row, pLogDrive.col, TEXTCOLOR);
				nCurSelete = 1;
				selectstr(str[2], ptdelpar.row, ptdelpar.col, SELCOLOR);
				break;
			default: 
				break;
			}
			break;
		case 0 ://ARROW
			switch(ch = _getch())
			{  
			case SHIFT_TAB:
	 		case UP:
				switch(nCurSelete)
				{            
				case 1:
					selectstr(str[2], ptdelpar.row, ptdelpar.col, TEXTCOLOR);
					nCurSelete = 4;
					selectstr(str[5], pLogDrive.row, pLogDrive.col, SELCOLOR);
					break;
				case 2:
					selectstr(str[3], ptdeldisk.row, ptdeldisk.col, TEXTCOLOR);
					nCurSelete = 1;
					selectstr(str[2], ptdelpar.row, ptdelpar.col, SELCOLOR);
					break;
				case 3:
					selectstr(str[4], ptdiskview.row, ptdiskview.col, TEXTCOLOR);
					nCurSelete = 2;
					selectstr(str[3], ptdeldisk.row, ptdeldisk.col, SELCOLOR);
					break;
				case 4:
					selectstr(str[5], pLogDrive.row, pLogDrive.col, TEXTCOLOR);
					nCurSelete = 3;
					selectstr(str[4], ptdiskview.row, ptdiskview.col, SELCOLOR);
					break;
				default: 
					break;
				}
				break;
			case DOWN:
				switch(nCurSelete)
				{            
				case 1:
					selectstr(str[2], ptdelpar.row, ptdelpar.col, TEXTCOLOR);
					nCurSelete = 2;
					selectstr(str[3], ptdeldisk.row, ptdeldisk.col, SELCOLOR);
					break;
				case 2:
					selectstr(str[3], ptdeldisk.row, ptdeldisk.col, TEXTCOLOR);
					nCurSelete = 3;
					selectstr(str[4], ptdiskview.row, ptdiskview.col, SELCOLOR);
					break;
				case 3:
					selectstr(str[4], ptdiskview.row, ptdiskview.col, TEXTCOLOR);
					nCurSelete = 4;
					selectstr(str[5], pLogDrive.row, pLogDrive.col, SELCOLOR);
					break;
				case 4:
					selectstr(str[5], pLogDrive.row, pLogDrive.col, TEXTCOLOR);
					nCurSelete = 1;
					selectstr(str[2], ptdelpar.row, ptdelpar.col, SELCOLOR);
					break;
				default: 
					break;
				}
				break;
			}	
		}			
	}
} 