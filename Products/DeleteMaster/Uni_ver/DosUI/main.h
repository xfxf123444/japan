
//**************************************************************************//
// main.h
//
//**************************************************************************//
#include <string.h>
#include <stdio.h>
#include <conio.h>  
#include <graph.h>   
#include <malloc.h>
#include <stdlib.h>
#include <dos.h>            
#include <direct.h>
#include <bios.h>         
#include <graph.h>
#include <time.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <io.h>
#include "..\..\..\..\tech\parinfo\cur_ver\msdos\parinfo.h"

class cwindow;   
////////////////////////////////////////////////////

//	define								//////

////////////////////////////////////////////////////  
#define DELETEDISK		1
#define DELETEPARTITION	2
#define DISKVIEW		3

#define	PAGE_MAINWIN	0
#define PAGE_DELPAR		1
#define PAGE_DELDISK	1
#define PAGE_DISKVIEW	2
#define PAGE_SEL_SECTOR	3
#define PAGE_CONFIRM	3
#define PAGE_PROGRESS	3
#define PAGE_IDENTIFY	6
#define PAGE_FINISH		4
#define PAGE_METHOD		5
#define PAGE_WRITE_LOG	6
#define PAGE_ERROR		7

#define WHITE			7
#define LWHITE			15
#define BLUE			1
#define YELLOW			14  
#define LYELLOW			10  
#define	GREEN			2    
#define RED				4
#define BKCOLOR			0

#define	TEXTCOLOR		LWHITE
#define	SELCOLOR		LYELLOW     

#define ESC				27
#define CR				13
#define TAB				9         
#define BACKSPACE 		8
#define SPACE			32
#define DEL				83
#define LEFT			75
#define RIGHT			77       
#define UP				72
#define DOWN			80

#define KEY0			48
#define KEY1			49
#define KEY2			50
#define KEY3			51
#define KEY4			52
#define KEY5			53
#define KEY6			54
#define KEY7			55
#define KEY8			56
#define KEY9			57

#define PAGEDOWN		81
#define PAGEUP			73
#define CTRL_END		117
#define CTRL_HOME		119
#define SHIFT_TAB		15    
#define X_FAT32	0x0B		
#define X_FAT16	0x06		                  

#define	WM_KEY		1   
#define WM_ACTIVE	2     
#define WM_INACTIVE	3    
#define WM_CREATEITEM 4

#define DATA_BUFFER_SIZE 		51200
#define DATA_BUFFER_SECTOR 		100
       
#define PAR "%5lu    %s  %s"
#define PAR1 "%5lu    %s    %s"
#define PAR2 "  %5lu    %s    %s"

#define UNKNOWNFILESYSTEM "ID=%X"
//#define PARINFOFORMAT	" Partition %5c     <Total:%5luMB>   <Free:%5luMb>   <Used:%5luMb>"
#define DRIVEFORMAT "( )Disk : %d" 
#define DISKFORMAT "( )HardDisk : %d - %7lu MB"
#define DISKVIEWDISKINFO "%d Range: %lu - %lu"
#define ABSLUTESECTOR "%lu"
#define NULLSTRING "                                      "
#define DISKCHS "Cylinder:%lu Head:%lu Sector:%lu" 
#define PARDELSELPAR "%c:\\   Start Sector:%lu         End Sector:%lu" 
#define DISKSELINFO "Hard Disk:%d  Cylinder:%lu  Head:%lu  Sector:%lu"
#define DELPARREPORT "Deleted partition %c: On disk - %d Start:%lu End:%lu"
#define DELDISKREPORT "Deleted disk - %d C:%d H:%d S:%d"
#define DELMETHODREPORT "Used Method: %s"
#define TIMESTAMP "Time start: %s %.2d %.2d:%.2d:%.2d %d  Time end:  %s %.2d %.2d:%.2d:%.2d %d" 

 

//#define SECTORINFO "%.4X:  %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X - %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c"
#define SECTORINFO "%.4X:  %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X - %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X "
//#define REBOOT	" Restart Your Computer!!!"  


#define DIRFLAG_L	'/'	
#define DIRFLAG_R	'/'	
#define DIRFLAG_L_STR	"/"	
#define DIRFLAG_R_STR	"/"	
                     
#define SELFLAG		'X'                     
#define SELFLAG_STRING	"X"                     
#define SELPREFIX  " < > " //must len = 3 5 9 ....
#define SELPOINT	(_fstrlen(SELPREFIX) / 2)
#define SELPREFIXLEN  _fstrlen(SELPREFIX)       
#define PRI "PRIMARY"
#define LOG "LOGICAL"
#define PROGRESS_CHAR		219
///////////////////////////////////////////////
//
//    Disk
//////////////////////////////////////////////
#define MBR_ITEM			4
#define BOOT_FLAG			0x80
#define DISK_BASE			0x80
#define DISK_MIN_SECTOR     0x3f
#define REMOVEABLE_DISK		0x0004
#define MAX_DISK_NUM		0x019 //0x7f

#define OPERATE_DELETE_ALL_DISK		1
#define OPERATE_DELETE_DISK			2
#define OPERATE_DELETE_PARTITION	3

//////////////////////////////////////////////////
//                                              /
//	Error message                              /
//                                             \\ 
//////////////////////////////////////////////////
#define NO_SELPAR 				"No partition been selected,\nPlease select again."
#define NO_MEMORY 				"No enough memory."
#define GET_DISK_PARAM_FAIL     "Get disk parameter fail."
//////////////////////////////////////////////////
//
//	 struct                                //////
//
//////////////////////////////////////////////////
typedef	struct tagsysinfo 
{ 
	int		nmaxrows;
	int		nmaxcols;
} _sysinfo;

typedef	struct _rccoord point;      

typedef struct tagitemnode
{  
	char *	szitemtext;
	BYTE	btItem;
	tagitemnode	*pnext, *ppre;
} _itemnode;

typedef struct tagPARMAPINFO{
	char			cDrv;
    BYTE			dmiInt13Unit;
    DWORD			dmiPartitionStartRBA;   
    DWORD    		dwPartSize    ;// Size of this partition (Unit:byte)
    DWORD    		dwUsedSize    ;// Used sectors( Unit:byte) 
	tagPARMAPINFO	*pNext;    
}PARMAPINFO;

typedef struct tagDISKVIEWINFO{ 
	DWORD			dwMinSec;
	DWORD			dwMaxSec;
	int				nSelDisk;
	DWORD			dwSelSec;
	int				nselect;
	BOOL			bDisableSwitchDisk;
}_diskviewinfo;

typedef struct tagDELETEPARINFO{
	int				nseldisk;
	int				nselect;
	int				nselpar;
}_deleteparinfo;

typedef struct tagDELETEDISKINFO{ 
	DWORD			dwDiskSize;
	DWORD			dwCylinders;
	DWORD			dwHeads;
	DWORD			dwSecPerTrack;
	int				nSelDisk;
	int				nselect;
}_deletediskinfo;

/*=======================================================
 * Information of a partition 
 *=====================================================*/    
struct YG_PARTITION_INFO  {
    BYTE     btDiskNum     ; // 0 mean first hard disk	
    char     DriveLetter   ; // 0 mean HAVE_NO_DRIVE_LETTER
	BOOL     bLogic        ; // Primary or logic?  
	BOOL     Reserved;
	DWORD    dwSystemFlag  ; // File system flag ,0b mean FAT32 ,etc.
	DWORD    dwStartSector ; // From MBR
    DWORD    dwPartSize    ; // Unit :sector
    DWORD    dwUsedSize    ; //
	char	 szOsLabel[0x0C]; //OS label
	DWORD	 dwPhyUsedSize ; // File used + system used sectors.
	char	 szLabel[0x0C] ; // Partition Label;
	DWORD    nLevel        ; // Compressed level 
	BYTE     BootFlag      ; // 80 mean bootable
    struct YG_PARTITION_INFO *pNext;
};

#define DISPLAY_FILE_INFO_NULL					0x00000001
#define DISPLAY_FILE_INFO_NOT_FILE				0x00000002
#define DISPLAY_FILE_INFO_OPEN_FILE_FAIL        0x00000003
#define DISPLAY_FILE_INFO_GET_PAR_FAIL			0x00000004
#define DISPLAY_FILE_INFO_READ_FILE_FAIL		0x00000005
#define DISPLAY_FILE_INFO_NOT_RM_IMAGE			0x00000006
////////////////////////////////////////////////////
//
// 	function
//
////////////////////////////////////////////////////
long	ParseCmdLine(int argc,char **argv,int *pnDisk,char *pcDrive);
void	PrintUsage();
BOOL	SecDeleteDisk(int nDisk);
BOOL	SecDeletePartition(char cDrive);
long	ParseSettingFile(char *szFile,int *pnDisk,char *pcDrive);
void 	run();
void 	exitprocess();
char	showidentifywin();
char 	showmainwin();
char 	showdelparwin();
char 	showdeldiskwin();
char 	showdiskviewwin();
char 	showsectorselwin();
char 	showconfirmwin();
char 	showprogresswin();
char	showfinishwin();
char	showmethodwin();
char	showsellogdrivewin();
void 	Sleep( clock_t wait );
void 	Beep( int frequency, int duration );
void 	drawbox(int r1, int c1, int r2, int c2, int right = 179);     
void 	selectstr(char * szstr, int nrow, int ncol, short ncolor);
void 	modifyattrib(int npage, int nrow, int ncol, char ncolor, char nbkcolor, int nlen);
void 	writemem(int npage, int nrow, int ncol, char ch, int nlen);
void 	winproc(cwindow* pwin, int message, DWORD param1 = 0, DWORD param2 = 0, DWORD param3 = 0);
void 	SortParInfo(PARINFOONHARDDISKEX *ParInfo);
DWORD   MapszLettersToParts(YG_PARTITION_INFO* pNew,int *kk);
BOOL 	Getallparmapinfo();
void 	writedriveletter(cwindow *pwin0, cwindow *pwin1);
void 	writestr(int npage, int nrow, int ncol, char * szstr);
BOOL 	ShowSectorInfo(cwindow *pwin,DWORD dwSec,int nSelDisk,BIOS_DRIVE_PARAM *pDriveParam,int *nErr);
void 	ErrorMessageBox(char  * str);
BOOL	GetFixDiskInfo(int nDisk);
BOOL 	DoDeleteSectors(DWORD dwStart, DWORD dwSize ,BYTE btDisk);
void 	FlushProgressInfo() ;
void 	charstostr(char *str, BYTE c, int nLen);
BOOL 	InitDelBuf(BYTE *pBuf,BYTE btFir,BYTE btSec,BYTE btThr,BOOL bRand,BOOL bOneByte);
BOOL 	WriteSecsWithBuf(DWORD dwStartSec,DWORD dwSize,BYTE btDisk,BYTE *pBuf,BOOL bVerify);
BOOL 	SaveReport(char csDrive);
BOOL 	SaveVerifyLog(char csDrive,DWORD dwStartSec,DWORD dwSectors,BOOL bWriteFail);

                                                          
///////////////////////////////////////////////////////////////
//
//		class
//
///////////////////////////////////////////////////////////////        

class cwindow
{       
	public:          
		int			m_npage;//number of display page
		int			m_nrow1, m_ncol1, m_nrow2, m_ncol2;//physical row && col  
		int			m_nwritedrow;//already be write's rows
		int			m_nactiverow;//light display item  
		int			m_nmaxrows,//can display max rows
					m_nmaxcols;//can display max cols
		_itemnode	*m_pitemhead,//point to item-link's head
					*m_pitemend,//point to item-link's end
					*m_pitemactive;//point to current acitve's item 
		int			m_nitemcount;
		char		m_cfilename[_MAX_PATH];
	public:  
		cwindow(int page, int r1, int c1, int r2, int c2);   
		~cwindow();
		void additem(char * szstr);
		void cwindow::additemNotSort(char* szstr,BYTE btItem);
		void writeitem(int nrow, char * szstr);  
		void sortitem();
		void onkey(int nkey);
		void onkeydown();                    
		void onkeyup();  
		void onpagedown();                  
		void onpageup();                  
		void onctrl_home();
		void onctrl_end();
		void onactive();             
		void oninactive();             
		int gettext(char * szstr, int nrow);     
		BOOL emptyitem();
		BOOL emptyscreen();     
		BOOL oncreateitem(char * szpath);  
		void enabledscroll();
};                                          
