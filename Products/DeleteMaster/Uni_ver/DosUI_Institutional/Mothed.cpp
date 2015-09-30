#include "comm.h"
#include "main.h"

extern	int							g_nMethod;
extern 	_sysinfo					g_SysInfo;     
extern  BOOL						g_bBeepOff;
		cwindow*					g_pMethodSelWin;

void SetSelMethod(int nMethod);

char showmethodwin()
{
	int				i,nWidth;
	int				nmethod;
	int				nselect = 0;
	char			ch;
	char			Method[40];
	char			*str[] = {"( ) [B]eep Off","[O]k","[C]ancel"};
	static point	ptmethodok,ptmethodcancel,ptmethodbeep;
	char			*szMethod[] = {"Overwrite with 00","Overwrite with FF",
						"Overwrite with random data","NSA method",
						"Old NSA method","DoD STD method",
						"NATO method","Gutmann method"};
	
	_setvisualpage(PAGE_METHOD);  
	_setactivepage(PAGE_METHOD);  
                    
	nmethod = g_nMethod;
	                    
    drawbox(1, 1, g_SysInfo.nmaxrows, g_SysInfo.nmaxcols);
 	_settextposition(3, 4);          
	_outtext("Please select the method :");
   
	g_pMethodSelWin = new cwindow(PAGE_METHOD, 5, 4, 15, 40);
	for(i=0;i<8;i++)
	{
		sprintf(Method,"( ) %s",szMethod[i]);
		g_pMethodSelWin->additemNotSort(Method,i); 
	}

	ptmethodcancel.row = 23;
	nWidth = strlen(str[2]);
	ptmethodcancel.col = g_SysInfo.nmaxcols - 2 - nWidth;	           
	_settextposition(ptmethodcancel.row, ptmethodcancel.col);
	_outtext(str[2]);  
	drawbox(ptmethodcancel.row - 1, ptmethodcancel.col - 1, ptmethodcancel.row + 1, ptmethodcancel.col + nWidth);

	nWidth = strlen(str[1]);
	ptmethodok.row = ptmethodcancel.row;
	ptmethodok.col = ptmethodcancel.col - 2 -nWidth;	           
	_settextposition(ptmethodok.row, ptmethodok.col);
	_outtext(str[1]);  
	drawbox(ptmethodok.row - 1, ptmethodok.col - 1, ptmethodok.row + 1, ptmethodok.col + nWidth);

	ptmethodbeep.row = 18;
	ptmethodbeep.col = 6;	           
	_settextposition(ptmethodbeep.row, ptmethodbeep.col);
	if (g_bBeepOff) str[0][1] = '*';
	else str[0][1] = ' ';
	_outtext(str[0]);  
	
	SetSelMethod(nmethod);
	winproc(g_pMethodSelWin, WM_ACTIVE);
	
	while(1)
	{                                               
	  	ch = _getch();
		switch(ch)
		{
		case 'b':
		case 'B':
			nselect = 1;  
//			winproc(g_pMethodSelWin, WM_INACTIVE);
			selectstr(str[0], ptmethodbeep.row, ptmethodbeep.col, SELCOLOR);
			break;
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
			switch (nselect)
			{
			case 0:
				if(nmethod !=  g_pMethodSelWin->m_pitemactive->btItem)
				{
					nmethod = g_pMethodSelWin->m_pitemactive->btItem;
				}
				g_pMethodSelWin->emptyitem();
				g_pMethodSelWin->emptyscreen();
				for(i=0;i<8;i++)
				{
					sprintf(Method,"( ) %s",szMethod[i]);
					g_pMethodSelWin->additemNotSort(Method,i); 
				}
				SetSelMethod(nmethod);
				break;
			case 1:
				g_bBeepOff = !g_bBeepOff;
				if (g_bBeepOff) str[0][1] = '*';
				else str[0][1] = ' ';
				_settextposition(ptmethodbeep.row, ptmethodbeep.col);
				_outtext(str[0]);  
				selectstr(str[0], ptmethodbeep.row, ptmethodbeep.col, SELCOLOR);
				break;
			case 2:
				 g_nMethod = nmethod;
				_clearscreen(_GCLEARSCREEN); 
				delete g_pMethodSelWin;
			    return 'o';
			case 3:
				_clearscreen(_GCLEARSCREEN); 
				delete g_pMethodSelWin;
			    return 'c';
			}
			break;
		case 0:
			switch(ch = _getch())
			{
			case SHIFT_TAB:
				switch(nselect)
				{
				case 0:
					nselect = 3;  
					winproc(g_pMethodSelWin, WM_INACTIVE);
					selectstr(str[2], ptmethodcancel.row, ptmethodcancel.col, SELCOLOR);
					break;
				case 1:
					nselect = 0;
					winproc(g_pMethodSelWin, WM_ACTIVE);
					selectstr(str[0], ptmethodbeep.row, ptmethodbeep.col, TEXTCOLOR);
					break;
				case 2:
					nselect = 1;
					selectstr(str[0], ptmethodbeep.row, ptmethodbeep.col, SELCOLOR);
					selectstr(str[1], ptmethodok.row, ptmethodok.col, TEXTCOLOR);
					break;
				case 3:
					nselect = 2;  
					selectstr(str[1], ptmethodok.row, ptmethodok.col, SELCOLOR);
					selectstr(str[2], ptmethodcancel.row, ptmethodcancel.col, TEXTCOLOR);
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
				selectstr(str[0], ptmethodbeep.row, ptmethodbeep.col, SELCOLOR);
				break;
			case 1:
				nselect = 2;  
				selectstr(str[0], ptmethodbeep.row, ptmethodbeep.col, TEXTCOLOR);
				selectstr(str[1], ptmethodok.row, ptmethodok.col, SELCOLOR);
				break;
			case 2:
				nselect = 3;
				selectstr(str[2], ptmethodcancel.row, ptmethodcancel.col, SELCOLOR);
				selectstr(str[1], ptmethodok.row, ptmethodok.col, TEXTCOLOR);
				break;
			case 3:
				nselect = 0;  
				winproc(g_pMethodSelWin, WM_ACTIVE);
				selectstr(str[2], ptmethodcancel.row, ptmethodcancel.col, TEXTCOLOR);
				break;
			}
			break;
		case CR:
			switch(nselect)
			{
			case 2:
				 g_nMethod = nmethod;
				_clearscreen(_GCLEARSCREEN); 
				delete g_pMethodSelWin;
			    return 'o';
			case 3:
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
	writestr(PAGE_METHOD, 5+g_pMethodSelWin->m_nactiverow , 6, "*");
}
