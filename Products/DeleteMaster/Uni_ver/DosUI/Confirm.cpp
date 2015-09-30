#include "comm.h"
#include "main.h"

extern 	_sysinfo					g_SysInfo;     
extern	YG_PARTITION_INFO			*g_pCurFixDiskInfo;
extern	_deletediskinfo				g_deldiskinfo;
extern	_deleteparinfo              g_delparinfo;
extern	_diskviewinfo				g_diskviewinfo;
extern  BYTE						g_btCurFun;
extern	int							g_nMethod;

void confirmshowselected();

char showconfirmwin()
{
	char			ch;
	int				nWidth,nselect;
	char			*str[] = {"[A]pply","[C]ancel"};
	static point	ptapply,ptcancel;
	char			Method[80];
	char			*szMethod[] = {"overwrite with 00","overwrite with FF",
						"random data","NSA method",
						"Old NSA method","DoD STD method",
						"NATO method","Gutmann method"};

	_setvisualpage(PAGE_CONFIRM);  
	_setactivepage(PAGE_CONFIRM);  
    
    drawbox(1, 1, g_SysInfo.nmaxrows, g_SysInfo.nmaxcols);

	_settextposition(3, 35);          
	_outtext("Warning");
    drawbox(2, 30, 4, 45);

	_settextposition(6, 4);
	sprintf(Method,"The selected partition (disk) is completely deleted by %s",szMethod[g_nMethod]);          
	_outtext(Method);
	_settextposition(7, 4);          
	_outtext("It cannot restore,no matter what method it may uses.");

	nWidth = strlen(str[1]);
	ptcancel.row = 23;
	ptcancel.col = g_SysInfo.nmaxcols - 2 - nWidth;	           
	_settextposition(ptcancel.row, ptcancel.col);
	_outtext(str[1]);  
	drawbox(ptcancel.row - 1, ptcancel.col - 1, ptcancel.row + 1, ptcancel.col + nWidth);

	nWidth = strlen(str[0]);
	ptapply.row = ptcancel.row;
	ptapply.col = ptcancel.col - 2 -nWidth;	           
	_settextposition(ptapply.row, ptapply.col);
	_outtext(str[0]);  
	drawbox(ptapply.row - 1, ptapply.col - 1, ptapply.row + 1, ptapply.col + nWidth);
	
	confirmshowselected();
	nselect = 0;
	selectstr(str[0], ptapply.row, ptapply.col, SELCOLOR);
	while(1)
	{                                               
	  	ch = _getch();
		switch(ch)
		{
		case 'A':
		case 'a':
			_clearscreen(_GCLEARSCREEN); 
			return 'a';
		case 'C':
		case 'c':
		case ESC :
			_clearscreen(_GCLEARSCREEN); 
			return 'c';
		case TAB:
			switch(nselect)
			{
			case 0:
				nselect = 1;
				selectstr(str[0], ptapply.row, ptapply.col, TEXTCOLOR);
				selectstr(str[1], ptcancel.row,ptcancel.col, SELCOLOR);
				break;
			case 1:
				nselect = 0;
				selectstr(str[1], ptcancel.row, ptcancel.col, TEXTCOLOR);
				selectstr(str[0], ptapply.row, ptapply.col, SELCOLOR);
				break;
			}
			break;
		case CR:
			if(nselect == 0)
			{
				_clearscreen(_GCLEARSCREEN);
				return 'a';
			}
			else
			{
				_clearscreen(_GCLEARSCREEN);
				return 'c';
			}
		case 0:
			switch(ch = _getch())
			{
			case SHIFT_TAB:
				switch(nselect)
				{
				case 0:
					nselect = 1;
					selectstr(str[0], ptapply.row, ptapply.col, TEXTCOLOR);
					selectstr(str[1], ptcancel.row,ptcancel.col, SELCOLOR);
					break;
				case 1:
					nselect = 0;
					selectstr(str[1], ptcancel.row, ptcancel.col, TEXTCOLOR);
					selectstr(str[0], ptapply.row, ptapply.col, SELCOLOR);
					break;
				}
			}
			break;
		}
	}
}

void confirmshowselected()
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
		szdriveletter = pDiskInfo->DriveLetter;
		dwStart 	  = pDiskInfo->dwStartSector;
		if(pDiskInfo->DriveLetter < 0x40) szdriveletter = '*';
		if(pDiskInfo->bLogic) dwStart += 0x3f;

	   	sprintf(szsel,PARDELSELPAR,szdriveletter,
	   		dwStart,pDiskInfo->dwStartSector+pDiskInfo->dwPartSize-1);

		_settextposition(12, 4);          
		_outtext("Selected Partition:");

		_settextposition(12, 23);          
		_outtext(szsel);
	}
	else
	{
		_settextposition(12, 4);          
		_outtext("Selected Disk:");
	   	sprintf(szsel,DISKSELINFO,g_deldiskinfo.nSelDisk-DISKBASE+1,
	   		g_deldiskinfo.dwCylinders,g_deldiskinfo.dwHeads,g_deldiskinfo.dwSecPerTrack);
		_settextposition(12, 19);          
		_outtext(szsel);
    }
}