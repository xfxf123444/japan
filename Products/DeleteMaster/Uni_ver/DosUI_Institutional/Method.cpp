#include "comm.h"
#include "main.h"

extern	int							g_nMethod;
extern 	_sysinfo					g_SysInfo;     
		cwindow*					g_pMethodSelWin;

void SetSelMethod(int nMethod);

char showmethodwin()
{
	int				i,nWidth;
	int				nmethod;
	int				nselect = 0;
	char			ch;
	char			Method[40];
	char			*str[] = {"[O]k","[C]ancel"};
	static point	ptmethodok,ptmethodcancel;
	
	_setvisualpage(PAGE_MOTHED);  
	_setactivepage(PAGE_MOTHED);  
                    
	nmethod = g_nMethod;
	                    
    drawbox(1, 1, g_SysInfo.nmaxrows, g_SysInfo.nmaxcols);
 	_settextposition(3, 4);          
	_outtext("Please select the method :");
   
	g_pMethodSelWin = new cwindow(PAGE_MOTHED, 5, 4, 15, 40);
	for(i=0;i<8;i++)
	{
		sprintf(Method,"( ) %s",DELETE_METHOD[i]);
		g_pMethodSelWin->additemNotSort(Method,i); 
	}
	ptmethodcancel.row = 23;
	nWidth = strlen(str[1]);
	ptmethodcancel.col = g_SysInfo.nmaxcols - 2 - nWidth;	           
	_settextposition(ptmethodcancel.row, ptmethodcancel.col);
	_outtext(str[1]);  
	drawbox(ptmethodcancel.row - 1, ptmethodcancel.col - 1, ptmethodcancel.row + 1, ptmethodcancel.col + nWidth);

	nWidth = strlen(str[0]);
	ptmethodok.row = ptmethodcancel.row;
	ptmethodok.col = ptmethodcancel.col - 2 -nWidth;	           
	_settextposition(ptmethodok.row, ptmethodok.col);
	_outtext(str[0]);  
	drawbox(ptmethodok.row - 1, ptmethodok.col - 1, ptmethodok.row + 1, ptmethodok.col + nWidth);
	
	SetSelMethod(nmethod);
	winproc(g_pMethodSelWin, WM_ACTIVE);
	
	while(1)
	{                                               
	  	ch = _getch();
		switch(ch)
		{
		case 'o':
		case 'O':
			 g_nMethod = nmethod;
			_clearscreen(_GCLEARSCREEN); 
			delete g_pMethodSelWin;
		    return 'o';
		case 'c':
		case 'C':
		case ESC :
			_clearscreen(_GCLEARSCREEN); 
			delete g_pMethodSelWin;
		    return 'c';
		case SPACE :
			if(nmethod !=  g_pMethodSelWin->m_pitemactive->btItem)
			{
				nmethod = g_pMethodSelWin->m_pitemactive->btItem;
			}
		    g_pMethodSelWin->emptyitem();
		    g_pMethodSelWin->emptyscreen();
			for(i=0;i<8;i++)
			{
				sprintf(Method,"( ) %s",DELETE_METHOD[i]);
				g_pMethodSelWin->additemNotSort(Method,i); 
			}
			SetSelMethod(nmethod);
			break;
		case 0:
			switch(ch = _getch())
			{
			case SHIFT_TAB:
				switch(nselect)
				{
				case 0:
					nselect = 2;  
					winproc(g_pMethodSelWin, WM_INACTIVE);
					selectstr(str[1], ptmethodcancel.row, ptmethodcancel.col, SELCOLOR);
					break;
				case 1:
					nselect = 0;
					winproc(g_pMethodSelWin, WM_ACTIVE);
					selectstr(str[0], ptmethodok.row, ptmethodok.col, TEXTCOLOR);
					break;
				case 2:
					nselect = 1;  
					selectstr(str[0], ptmethodok.row, ptmethodok.col, SELCOLOR);
					selectstr(str[1], ptmethodcancel.row, ptmethodcancel.col, TEXTCOLOR);
					break;
				}
				break;
			case PAGEDOWN:
			case PAGEUP:
			case CTRL_END:
			case CTRL_HOME:  
			case UP:
			case DOWN:
				winproc(g_pMethodSelWin, WM_KEY, ch);
				break;
			}
			break;
		case TAB:
			switch(nselect)
			{
			case 0:
				nselect = 1;  
				winproc(g_pMethodSelWin, WM_INACTIVE);
				selectstr(str[0], ptmethodok.row, ptmethodok.col, SELCOLOR);
				break;
			case 1:
				nselect = 2;
				selectstr(str[1], ptmethodcancel.row, ptmethodcancel.col, SELCOLOR);
				selectstr(str[0], ptmethodok.row, ptmethodok.col, TEXTCOLOR);
				break;
			case 2:
				nselect = 0;  
				winproc(g_pMethodSelWin, WM_ACTIVE);
				selectstr(str[1], ptmethodcancel.row, ptmethodcancel.col, TEXTCOLOR);
				break;
			}
			break;
		case CR:
			switch(nselect)
			{
			case 1:
				 g_nMethod = nmethod;
				_clearscreen(_GCLEARSCREEN); 
				delete g_pMethodSelWin;
			    return 'o';
			case 2:
				_clearscreen(_GCLEARSCREEN); 
				delete g_pMethodSelWin;
			    return 'c';
			}
		}
	}
}

void SetSelMethod(int nMethod)
{
	while(g_pMethodSelWin->m_pitemactive->btItem != nMethod)
	{
		winproc(g_pMethodSelWin, WM_KEY, DOWN);
	}
	g_pMethodSelWin->m_pitemactive->szitemtext[1] = '*';
	writestr(PAGE_MOTHED, 5+g_pMethodSelWin->m_nactiverow , 6, "*");
}
