#include "comm.h"
#include "main.h"

extern 	_sysinfo					g_SysInfo;     
extern	YG_PARTITION_INFO			*g_pCurFixDiskInfo;
extern	_deleteparinfo              g_delparinfo;
extern	_deletediskinfo				g_deldiskinfo;
extern	_diskviewinfo				g_diskviewinfo;
extern  BYTE						g_btCurFun;
extern  char                        g_chLogDrive;
extern  int							g_nMethod;

void finishshowtarget();

char showfinishwin()
{
	char			ch,szInfo[80];
	int				nWidth,nselect;
	char			*str[] = {"Change [L]og Drive","[S]ave report","[F]inish"};
	static point	ptlogdrive,ptsave,ptfinish;
	char				*szMethod[] = {"Overwrite with 00","Overwrite with FF",
						"Overwrite with random data","NSA method",
						"Old NSA method","DoD STD method",
						"NATO method","Gutmann method"};

label_start_show:
	_setvisualpage(PAGE_FINISH);  
	_setactivepage(PAGE_FINISH);  
    
    drawbox(1, 1, g_SysInfo.nmaxrows, g_SysInfo.nmaxcols);    
	_settextposition(3, (g_SysInfo.nmaxcols-20)/2);          
	_outtext("Delete Successfuly");
    drawbox(2, (g_SysInfo.nmaxcols-20)/2-3, 4, (g_SysInfo.nmaxcols-20)/2+20);

	sprintf(szInfo,"is deleted completely with Method: %s.",szMethod[g_nMethod]);
	_settextposition(7, 4);          
	_outtext(szInfo);

	nWidth = strlen(str[2]);
	ptfinish.row = 23;
	ptfinish.col = g_SysInfo.nmaxcols - 2 - nWidth;	           
	_settextposition(ptfinish.row, ptfinish.col);
	_outtext(str[2]);  
	drawbox(ptfinish.row - 1, ptfinish.col - 1, ptfinish.row + 1, ptfinish.col + nWidth);

	nWidth = strlen(str[1]);
	ptsave.row = 23;
	ptsave.col = ptfinish.col - 2 - nWidth;	           
	_settextposition(ptsave.row, ptsave.col);
	_outtext(str[1]);  
	drawbox(ptsave.row - 1, ptsave.col - 1, ptsave.row + 1, ptsave.col + nWidth);

   	nWidth = strlen(str[0]);
	ptlogdrive.row = 23;
	ptlogdrive.col = ptsave.col - 2 - nWidth;	           
	_settextposition(ptlogdrive.row, ptlogdrive.col);
	_outtext(str[0]);  
	drawbox(ptlogdrive.row - 1, ptlogdrive.col - 1, ptlogdrive.row + 1, ptlogdrive.col + nWidth);

    finishshowtarget();
	selectstr(str[1], ptsave.row, ptsave.col, SELCOLOR); 
	nselect = 1;

	while(1)
	{                                               
	  	ch = _getch();
		switch(ch)
		{
		case 's':
		case 'S':
			if (!SaveReport(g_chLogDrive))
			{
				ErrorMessageBox("Fail to save report!");
				break;
			}
			_clearscreen(_GCLEARSCREEN); 
			return 's';
		case 'F':
		case 'f':
		case ESC :
			_clearscreen(_GCLEARSCREEN); 
			return 'f';
		case 'l':
		case 'L':
			showsellogdrivewin();
			goto label_start_show;
			break;
		case TAB:
			switch(nselect)
			{
			case 1:
				nselect = 2;
				selectstr(str[1], ptsave.row, ptsave.col, TEXTCOLOR);
				selectstr(str[2], ptfinish.row,ptfinish.col, SELCOLOR);
				break;
			case 2:
				nselect = 0;
				selectstr(str[2], ptfinish.row, ptfinish.col, TEXTCOLOR);
				selectstr(str[0], ptlogdrive.row, ptlogdrive.col, SELCOLOR);
				break;
			case 0:
				nselect = 1;
				selectstr(str[0], ptlogdrive.row, ptlogdrive.col, TEXTCOLOR);
				selectstr(str[1], ptsave.row, ptsave.col, SELCOLOR);
				break;
			}
			break;
		case CR:
			switch (nselect)
			{
			case 0:
				showsellogdrivewin();
				goto label_start_show;
				break;
			case 1:
				if (!SaveReport(g_chLogDrive))
				{
					ErrorMessageBox("Fail to save report!");
					break;
				}
				_clearscreen(_GCLEARSCREEN);
				return 's';
			case 2:
				_clearscreen(_GCLEARSCREEN);
				return 'f';
			}
		case 0:
			switch(ch = _getch())
			{
			case SHIFT_TAB:
				switch(nselect)
				{
				case 1:
					nselect = 2;
					selectstr(str[1], ptsave.row, ptsave.col, TEXTCOLOR);
					selectstr(str[2], ptfinish.row,ptfinish.col, SELCOLOR);
					break;
				case 2:
					nselect = 0;
					selectstr(str[2], ptfinish.row, ptfinish.col, TEXTCOLOR);
					selectstr(str[0], ptlogdrive.row, ptlogdrive.col, SELCOLOR);
					break;
				case 0:
					nselect = 1;
					selectstr(str[0], ptlogdrive.row, ptlogdrive.col, TEXTCOLOR);
					selectstr(str[1], ptsave.row, ptsave.col, SELCOLOR);
					break;
				}
			}
			break;
		}
	}
}

void finishshowtarget()
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
		if(!pDiskInfo->DriveLetter) szdriveletter = '*';
		if(pDiskInfo->bLogic) dwStart += 0x3f;
	   	sprintf(szsel,PARDELSELPAR,szdriveletter,
	   		dwStart,pDiskInfo->dwStartSector+pDiskInfo->dwPartSize-1);

		_settextposition(6, 4);          
		_outtext("Partition:");

		_settextposition(6, 15);          
		_outtext(szsel);
	}
	else
	{
		_settextposition(6, 4);          
	   	sprintf(szsel,DISKSELINFO,g_deldiskinfo.nSelDisk-DISKBASE+1,
	   		g_deldiskinfo.dwCylinders,g_deldiskinfo.dwHeads,g_deldiskinfo.dwSecPerTrack);
		_outtext(szsel);
    }
}
