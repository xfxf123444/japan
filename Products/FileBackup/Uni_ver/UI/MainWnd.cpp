// MainWnd.cpp : implementation file
//

#include "stdafx.h"
#include "AM01.h"
#include "MainWnd.h"
#include "WizSheet.h"
#include "BackupState.h"
#include "SqueezeImage.h"
#include "ShutdownDlg.h"
#include "..\AM01Expt\AM01Expt.h"
#include "function.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_MY_NOTIFYICON		(WM_USER+100)

AM01_PROG_SETTING *g_pSetting;
CStringArray g_SettingArray;
CSCHEDULE_ARRAY g_ScheduleArray; // g_ScheduleArray will be reload if schedule is changed
CFAILRETRYSCHEDULE ScheduleItemArray;


// extern AM01TASK g_TaskInfo;

extern WCHAR g_szPath[MAX_PATH];

extern AM01_PROG_SETTING g_CurSetting;

/////////////////////////////////////////////////////////////////////////////
// CMainWnd

CMainWnd::CMainWnd()
{
	//-- this is what I added begin
	m_bIsWorking = FALSE;
	//-- this is what I added end

	m_pControl = NULL;
	m_bExecuting = FALSE;
	
	GetSettingArray(&g_SettingArray);
	GetScheduleArray(&g_ScheduleArray);

	CreateEx(0,MaClassName,L"BackupMaster",
			 WS_POPUP,0,
			 0,
			 0,0,NULL,0,NULL); 
	
}

CMainWnd::~CMainWnd()
{
}


BEGIN_MESSAGE_MAP(CMainWnd, CWnd)
	//{{AFX_MSG_MAP(CMainWnd)
	ON_MESSAGE(WM_MY_NOTIFYICON, OnMyNotify)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMainWnd message handlers

LRESULT CMainWnd::OnMyNotify(WPARAM wParam, LPARAM lParam) 
{
	CMenu menu ;
	CMenu SubMenu;
	TCHAR szString[256];

	switch (lParam)
	{
		case WM_LBUTTONDBLCLK:
			PostMessage(WM_COMMAND,ID_MAIN_CONTROL,0);
			break;
		case WM_RBUTTONDOWN:
			{
				VERIFY(menu.LoadMenu(IDR_MAINMENU));
				CMenu* pPopup = menu.GetSubMenu ( 0 ) ;
				ASSERT(pPopup != NULL);
				InitSubMenu(&SubMenu);
				pPopup->GetMenuString(ID_REMBER_SETTING,szString,sizeof(szString),MF_BYCOMMAND);
				VERIFY(pPopup->ModifyMenu(ID_REMBER_SETTING,MF_BYCOMMAND | MF_POPUP,(UINT)SubMenu.m_hMenu,szString));
				POINT pt ;
				GetCursorPos ( &pt ) ;
			    SetForegroundWindow ();
				pPopup->TrackPopupMenu ( TPM_LEFTALIGN | TPM_RIGHTBUTTON,
											  pt.x, pt.y, AfxGetMainWnd() );

				break ;
			}
		case WM_MOUSEMOVE:
			   m_pTray->SetState(L"FileBackup");
		default: 
			break;
	}
	return 0;
}

int CMainWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_pTray = new CTrayNot (this, WM_MY_NOTIFYICON, L"BackupMaster") ;
	m_nTimer = 101;
	
	// SetTimer to one minute
#ifdef FEIX_TEST
	SetTimer(m_nTimer,10000,NULL);
#else
	SetTimer(m_nTimer,60000,NULL);
#endif

	return TRUE;
}

void CMainWnd::OnSearch()
{
	WCHAR szFile[MAX_PATH];
	wcscpy(szFile,g_szPath);

	wcscat(szFile,L"\\FileFind.exe");

	char temp[MAX_PATH];
	ZeroMemory(temp, MAX_PATH);
	WideCharToMultiByte(CP_ACP, 0, szFile, -1, temp, MAX_PATH, 0, 0);
	WinExec(temp,SW_SHOW);
}

void CMainWnd::OnCloseApp()
{
	PostMessage(WM_CLOSE,0,0);
}

void CMainWnd::OnSetCurrent()
{
	CAMSetting	AMSettingDlg;

	if (!m_pControl)
	{
		m_pControl = new CSetup(IDS_SETUP,this);
		m_pControl->AddPage(&AMSettingDlg);
		m_pControl->DoModal();
		m_pControl->RemovePage(&AMSettingDlg);
		
		if( NULL != m_pControl)
		{
			delete m_pControl;
		    m_pControl = NULL;
		}
	}
	else m_pControl->SetForegroundWindow();
}

void CMainWnd::OnNewBackup()
{
	CBitmap bmpWatermark;
	VERIFY(bmpWatermark.LoadBitmap(IDB_WATERMARK));
	// g_pSetting = (AM01_PROG_SETTING *)malloc(sizeof(AM01_PROG_SETTING));
	g_pSetting = new AM01_PROG_SETTING;
	if (g_pSetting)
	{
		// memset(g_pSetting,0,sizeof(AM01_PROG_SETTING));
		CWizSheet dlg(IDS_SETTING_WIZARD, NULL, 0, bmpWatermark, NULL,NULL);
		dlg.m_psh.hInstance = ::GetModuleHandle(NULL);
		if (ID_WIZFINISH == dlg.DoModal())
		{
    		if( FALSE == AddOneSetting(g_pSetting) )
			{
				TRACE(L"\nAddOneSetting error in OnNewBackup.");
			}
			g_SettingArray.Add(g_pSetting->DataInfo.szSettingName);
		}
		//free(g_pSetting);
		delete g_pSetting;
		g_pSetting = 0;
	}
}

void CMainWnd::OnRestore()
{
	TRACE(L"\nAddOneSetting error in OnNewBackup.");
	WCHAR szFile[MAX_PATH];
	wcscpy(szFile,g_szPath);
    wcscat(szFile,L"\\FileRestore.exe");
	char temp[MAX_PATH];
	ZeroMemory(temp, MAX_PATH);
	WideCharToMultiByte(CP_ACP, 0, szFile, -1, temp, MAX_PATH, 0, 0);
	WinExec(temp,SW_SHOW);
}

void CMainWnd::OnBackupShutdown()
{
	if( wcslen(g_CurSetting.DataInfo.szSettingName) > 0)
	{
	  ExecuteOneSetting(g_CurSetting.DataInfo.szSettingName);
	  ShutdownSystem();
	}
	else
	{
		CString strMsg;
		strMsg.LoadString(IDS_NO_CURRENT_SETTING);
		AfxMessageBox((LPCTSTR)strMsg,MB_OK|MB_ICONINFORMATION,NULL);
	}
}

void CMainWnd::OnOpenMainControl()
{
	CAMBackup      AMBackupDlg;
	CAMSetting	   AMSettingDlg;
	CAMTask        AMTaskDlg;
//	CAMMakeCDR     AMMakeCDRDlg;
	CSqueezeImage  SqueezeImageDlg;
	CAMAbout       AMAboutDlg;


	if (!m_pControl)
	{
		m_pControl = new CSetup(IDS_SETUP,this);
		m_pControl->AddPage(&AMSettingDlg);
		m_pControl->AddPage(&AMBackupDlg);
		m_pControl->AddPage(&AMTaskDlg);
//		if (!GetOsVersion())
//			m_pControl->AddPage(&AMMakeCDRDlg);
		m_pControl->AddPage(&SqueezeImageDlg);
		m_pControl->AddPage(&AMAboutDlg);
		m_pControl->DoModal();
		m_pControl->RemovePage(&AMAboutDlg);
		m_pControl->RemovePage(&SqueezeImageDlg);
		m_pControl->RemovePage(&AMTaskDlg);
//		if (!GetOsVersion())
//			m_pControl->RemovePage(&AMMakeCDRDlg);
		m_pControl->RemovePage(&AMBackupDlg);
		m_pControl->RemovePage(&AMSettingDlg);
		delete m_pControl;
		m_pControl = NULL;
	}
	else m_pControl->SetForegroundWindow();
}

void CMainWnd::OnDestroy() 
{
	if (m_pControl)
	{
		::SendMessage(m_pControl->m_hWnd,WM_COMMAND,IDOK,0);
		m_pControl->DestroyWindow();
	}

	CWnd::OnDestroy();

	KillTimer(m_nTimer);	

	if (m_pTray)
	{
		Shell_NotifyIcon ( NIM_DELETE, &m_pTray->m_tnd ) ;
		delete m_pTray;
		m_pTray=NULL;
	}

	// TODO: Add your message handler code here
	
}


void CMainWnd::OnTimer(UINT nIDEvent) 
{
	CTime CurTime,LastTryTime;
	CTimeSpan TimeSpan;
	int nScheduleIndex;
	BOOL bStartRun = FALSE;
	BOOL bNewBase = FALSE;
	BOOL bShutdown = FALSE;
	
	if (!m_bExecuting)
	{
		m_bExecuting = TRUE;
		CurTime = CTime::GetCurrentTime();
		if (ReadFailRetrySchedule(ScheduleItemArray))
		{
			for (nScheduleIndex = 0;nScheduleIndex < ScheduleItemArray.GetSize();nScheduleIndex ++)
			{
				LastTryTime = ScheduleItemArray[nScheduleIndex].LastTryTime;
				TimeSpan = CurTime - LastTryTime;
				if (TimeSpan.GetTotalMinutes() >= (long)ScheduleItemArray[nScheduleIndex].dwRetryMinutes)
				{
					ExcuteFailRetrySchedule(&ScheduleItemArray[nScheduleIndex]);
				}
			}
			FreeFailRetrySchedule(ScheduleItemArray);
		}
		for(nScheduleIndex = 0; nScheduleIndex < g_ScheduleArray.GetSize(); nScheduleIndex++)
		{
			if(CheckSettingSchedule(g_ScheduleArray.GetAt(nScheduleIndex),CurTime,&bStartRun,&bNewBase,&bShutdown))
			{
				if(bStartRun)
				{
					if(bNewBase)
					{
					   ExecuteSettingNewBase(g_ScheduleArray.GetAt(nScheduleIndex).szSettingName);
					}
					else
					{
					   ExecuteOneSetting(g_ScheduleArray.GetAt(nScheduleIndex).szSettingName);
					}
					if(bShutdown)
					{
						ShutdownSystem();
					}
				}
			}
		}
		m_bExecuting = FALSE;
	}
	CWnd::OnTimer(nIDEvent);
}


void CMainWnd::InitSubMenu(CMenu *pMenu)
{
	int i,nTotal;
	
	VERIFY(pMenu->CreateMenu());
	pMenu->AppendMenu(MF_SEPARATOR);
	nTotal = g_SettingArray.GetSize();
	for(i=0; i < nTotal; i++)
	{
		pMenu->AppendMenu(MF_STRING,WM_USER+i+101,g_SettingArray.ElementAt(i));
	}
}

BOOL CMainWnd::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (nCode == CN_COMMAND)
	{
		// TODO: Add your specialized code here and/or call the base class
		switch (nID)
		{
		case ID_MAIN_CONTROL:
			OnOpenMainControl();
			return TRUE; 
			break;
		case ID_SEARCH:
			OnSearch();
			return TRUE; 
			break;
		case ID_CLOSE:
			OnCloseApp();
			return TRUE; 
			break;
		case ID_SET_CURRENT:
			OnSetCurrent();
			return TRUE; 
			break;
		case ID_NEW_BACKUP:
			OnNewBackup();
			return TRUE; 
			break;
		case ID_RESTORE:
			OnRestore();
			return TRUE; 
			break;
		case ID_BACKUP_SHUTDOWN:
			OnBackupShutdown();
			return TRUE; 
			break;
		default:
			break;
		}

		if (nID > WM_USER+100 && nID < WM_USER+101+(UINT)g_SettingArray.GetSize())
		{
			nID -= (WM_USER+101);
			ExecuteOneSetting(g_SettingArray.ElementAt(nID));
			return TRUE;
		}
	}
	return CWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


BOOL CMainWnd::CheckSettingSchedule(SCHEDULE_DATA OneScheduleData,CTime CurTime,BOOL *pbStartRun, BOOL *pbNewBase, BOOL *pbShutdown)
{
	*pbStartRun = FALSE;
	*pbShutdown = FALSE;
	*pbNewBase  = FALSE; 

	if( 
		 ( NORMAL_SCHEDULE != OneScheduleData.nScheduleType )  &&
		 ( ADVANCED_SCHEDULE != OneScheduleData.nScheduleType )
	  )
	{
		TRACE(L"\nSchedule type error in CheckSettingSchedle");
		return FALSE;
	}

//#ifdef FEIX_TEST
//	static int i = 0;
//	int  nDayOfWeek;
//	// 1 for sunday, 2 for monday...
//	// after subtracting 1, 0 for sunday, 1 for monday ... 
//
//	nDayOfWeek = OneScheduleData.nBaseWeekDay; 
//
//	int nDayOfMonth;
//	nDayOfMonth = OneScheduleData.nBaseMonthDay + i % 7;
//	++i;
//	int temp = CurTime.GetDayOfWeek()-1; 
//	if( ADVANCED_SCHEDULE == OneScheduleData.nScheduleType )
//	{
//		CurTime = OneScheduleData.PeriodTask.OneTaskTime;
//	}
//	else if( NORMAL_SCHEDULE == OneScheduleData.nScheduleType )
//	{
//		CurTime = OneScheduleData.DayTask[nDayOfWeek].OneTaskTime;
//	}
//#else
	int  nDayOfWeek;
	// 1 for sunday, 2 for monday...
	// after subtracting 1, 0 for sunday, 1 for monday ... 

	nDayOfWeek = CurTime.GetDayOfWeek()-1; 

	int nDayOfMonth;
	nDayOfMonth = CurTime.GetDay();
//#endif


	CTime ScheduleTime;

	if( ADVANCED_SCHEDULE == OneScheduleData.nScheduleType )
	{
		ScheduleTime = OneScheduleData.PeriodTask.OneTaskTime;
	}
	else if( NORMAL_SCHEDULE == OneScheduleData.nScheduleType )
	{
		ScheduleTime = OneScheduleData.DayTask[nDayOfWeek].OneTaskTime;
	}

	if(
		( ScheduleTime.GetHour() != CurTime.GetHour() ) ||
		( ScheduleTime.GetMinute() != CurTime.GetMinute() )
	  )
	{
		*pbStartRun = FALSE;
		*pbShutdown = FALSE;
		*pbNewBase  = FALSE; 
		return TRUE;
	}

	if( ADVANCED_SCHEDULE == OneScheduleData.nScheduleType )
	{
		// advanced_schedule, it will run backup every day

		if (OneScheduleData.wExecuteType == EXCUTE_TYPE_FULL ||
			OneScheduleData.wExecuteType == EXCUTE_TYPE_INCREMENTAL)
		{
			if( ((WEEK_PERIOD == OneScheduleData.nBackupPeriod) && (nDayOfWeek == OneScheduleData.nBaseWeekDay)) ||
				((MONTH_PERIOD == OneScheduleData.nBackupPeriod) && (nDayOfMonth == OneScheduleData.nBaseMonthDay)))

			{
				*pbNewBase = (OneScheduleData.wExecuteType == EXCUTE_TYPE_FULL);
				*pbStartRun = TRUE;
				*pbShutdown = OneScheduleData.PeriodTask.bShutdown;
				return TRUE;
			}
			return FALSE;
		}

		*pbShutdown = OneScheduleData.PeriodTask.bShutdown;
		*pbStartRun = TRUE;
		if( WEEK_PERIOD == OneScheduleData.nBackupPeriod )
		{
			if( nDayOfWeek == OneScheduleData.nBaseWeekDay )
			{
			  *pbNewBase = TRUE;
			}
		}
		else if( MONTH_PERIOD == OneScheduleData.nBackupPeriod )
		{
			if( nDayOfMonth == OneScheduleData.nBaseMonthDay )
			{
				*pbNewBase = TRUE;
			}
		}
		else
		{
			TRACE(L"\nnScheduleType error in CheckSettingSchedule.");
			*pbStartRun = FALSE;
			*pbShutdown = FALSE;
			*pbNewBase  = FALSE; 
			return FALSE;
		}
	}
	else if ( NORMAL_SCHEDULE == OneScheduleData.nScheduleType )
	{
		if( FALSE  == OneScheduleData.DayTask[nDayOfWeek].bSchedule )
		{
			// no schedule for today
			*pbStartRun = FALSE;
			return TRUE;
		}

		// compare with the current date of the schedule
		ScheduleTime = OneScheduleData.DayTask[nDayOfWeek].OneTaskTime;

		*pbStartRun = TRUE;
		*pbShutdown = OneScheduleData.DayTask[nDayOfWeek].bShutdown;
	}

	return TRUE;
}



