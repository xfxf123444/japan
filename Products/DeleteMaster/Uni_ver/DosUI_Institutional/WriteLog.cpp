#include "comm.h"
#include "main.h"

extern 	_sysinfo					g_SysInfo;
extern  char		   				g_drives[27];
		cwindow*					g_pDriveSelWin;
int                                 g_nTotalDrive;
extern  char						g_chLogDrive;

void SetSelDrive(int nDrive);
void InitDriveBox();
char GetSelDriverLetter(int nSelDrive);
int GetSelDriver(char chDrive);

char showsellogdrivewin()
{
	int				nSelectDrive;
	int				nWidth,nselect = 0;
	char			szInfo[50],ch;
	char			*str[] = {"[O]k","[C]ancel"};
	static point	ptok,ptcancel;

	_setvisualpage(PAGE_WRITE_LOG);  
	_setactivepage(PAGE_WRITE_LOG);  
	
	nSelectDrive = GetSelDriver(g_chLogDrive);

    drawbox(1, 1, g_SysInfo.nmaxrows, g_SysInfo.nmaxcols);    

    if (!g_nTotalDrive)
	{
		_settextposition(5, 6);
		printf("No drive found, can not save log!\n press any key to continue\n");
		ch = _getch();
		_clearscreen(_GCLEARSCREEN); 
		return ch;
	}

	nWidth = strlen(str[1]);
	ptcancel.row = 23;
	ptcancel.col = g_SysInfo.nmaxcols - 2 - nWidth;	           
	_settextposition(ptcancel.row, ptcancel.col);
	_outtext(str[1]);  
	drawbox(ptcancel.row - 1, ptcancel.col - 1, ptcancel.row + 1, ptcancel.col + nWidth);
	
	nWidth = strlen(str[0]);
	ptok.row = 23;
	ptok.col = ptcancel.col - 2 - nWidth;	           
	_settextposition(ptok.row, ptok.col);
	_outtext(str[0]);  
	drawbox(ptok.row - 1, ptok.col - 1, ptok.row + 1, ptok.col + nWidth);

	selectstr(str[0], ptok.row, ptok.col, SELCOLOR); 
	sprintf(szInfo,"Please choice a drive to save log.");
	_settextposition(4, 4);          
	_outtext(szInfo);

	g_pDriveSelWin = new cwindow(PAGE_WRITE_LOG, 6, 4, 24, 20);
	InitDriveBox();
	SetSelDrive(nSelectDrive);
	winproc(g_pDriveSelWin, WM_INACTIVE);

	while(1)
	{                                               
	  	ch = _getch();
		switch(ch)
		{
		case 'o':
		case 'O':
			g_chLogDrive = GetSelDriverLetter(nSelectDrive);
			_clearscreen(_GCLEARSCREEN); 
			delete g_pDriveSelWin;
			return 'o';
			break;
		case 'C':
		case 'c':
		case ESC :
			_clearscreen(_GCLEARSCREEN); 
			delete g_pDriveSelWin;
			return 'c';
			break;
		case TAB:
			switch(nselect)
			{
			case 0:
				nselect = 1;
				selectstr(str[0], ptok.row, ptok.col, TEXTCOLOR);
				selectstr(str[1], ptcancel.row,ptcancel.col, SELCOLOR);
				break;
			case 1:
				nselect = 2;
				selectstr(str[1], ptcancel.row, ptcancel.col, TEXTCOLOR);
				winproc(g_pDriveSelWin, WM_ACTIVE);
				break;
			case 2:
				nselect = 0;
				selectstr(str[0], ptok.row, ptok.col, SELCOLOR);
				winproc(g_pDriveSelWin, WM_INACTIVE);
				break;
			}
			break;
		case CR:
			switch (nselect)
			{
			case 0:
				g_chLogDrive = GetSelDriverLetter(nSelectDrive);
				_clearscreen(_GCLEARSCREEN); 
				delete g_pDriveSelWin;
				return 's';
			case 1:
				_clearscreen(_GCLEARSCREEN); 
				delete g_pDriveSelWin;
				return 'c';
				break;
			}
			break;
		case SPACE:
			switch (nselect)
			{
			case 0:
				g_chLogDrive = GetSelDriverLetter(nSelectDrive);
				_clearscreen(_GCLEARSCREEN); 
				delete g_pDriveSelWin;
				return 's';
			case 1:
				_clearscreen(_GCLEARSCREEN); 
				delete g_pDriveSelWin;
				return 'c';
			case 2:
				if(nSelectDrive !=  g_pDriveSelWin->m_pitemactive->btItem)
				{
					nSelectDrive = g_pDriveSelWin->m_pitemactive->btItem;
				}
				g_pDriveSelWin->emptyitem();
				g_pDriveSelWin->emptyscreen();
				InitDriveBox();
				SetSelDrive(nSelectDrive);
				break;
			}
			break;
		case 0:
			switch(ch = _getch())
			{
			case SHIFT_TAB:
				switch(nselect)
				{
				case 0:
					nselect = 1;
					selectstr(str[0], ptok.row, ptok.col, TEXTCOLOR);
					selectstr(str[1], ptcancel.row,ptcancel.col, SELCOLOR);
					break;
				case 1:
					nselect = 2;
					selectstr(str[1], ptcancel.row, ptcancel.col, TEXTCOLOR);
					winproc(g_pDriveSelWin, WM_ACTIVE);
					break;
				case 2:
					nselect = 0;
					selectstr(str[0], ptok.row, ptok.col, SELCOLOR);
					winproc(g_pDriveSelWin, WM_INACTIVE);
					break;
				}
				break;
			case PAGEDOWN:
			case PAGEUP:
			case CTRL_END:
			case CTRL_HOME:  
			case UP:
			case DOWN:
				if (nselect == 2)
					winproc(g_pDriveSelWin, WM_KEY, ch);
				break;
			}
			break;
		}
	}
}

void SetSelDrive(int nDrive)
{
	while(g_pDriveSelWin->m_pitemactive->btItem != nDrive)
	{
		winproc(g_pDriveSelWin, WM_KEY, DOWN);
	}
	g_pDriveSelWin->m_pitemactive->szitemtext[1] = '*';
	writestr(PAGE_WRITE_LOG, 6+g_pDriveSelWin->m_nactiverow , 6, "*");
}

int GetSelDriver(char chDrive)
{
	int i,drive;

	i = 0;
	for(drive=0;drive<26;drive++)
	{
		if (g_drives[drive])
		{
			if (chDrive == drive + 'A')
			{
				return i;
				break;
			}
			i ++;
		}
	}
	return 0;
}

char GetSelDriverLetter(int nSelDrive)
{
	int i,drive;
	char chDrive = 'A';

	i = 0;
	for(drive=0;drive<26;drive++)
	{
		if (g_drives[drive])
		{
			if (nSelDrive == i)
			{
				chDrive = drive + 'A';
				break;
			}
			i ++;
		}
	}
	return chDrive;
}

void InitDriveBox()
{
	char szInfo[20];
	int i,drive;

	i = 0;
	for(drive=0;drive<26;drive++)
	{
		if (g_drives[drive])
		{
			sprintf(szInfo,"( ) %c:\\ ",drive+'A');
			g_pDriveSelWin->additemNotSort(szInfo,i); 
			i ++;
		}
	}
}