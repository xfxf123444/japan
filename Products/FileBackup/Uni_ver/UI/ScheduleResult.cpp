// ScheduleResult.cpp : implementation file
//

#include "stdafx.h"
#include "am01.h"
#include "ScheduleResult.h"
#include "../AM01Struct/AM01Struct.h"
#include "Function.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern SCHEDULE_DATA g_ScheduleData;

/////////////////////////////////////////////////////////////////////////////
// CScheduleResult dialog


CScheduleResult::CScheduleResult(CWnd* pParent /*=NULL*/)
	: CDialog(CScheduleResult::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScheduleResult)
	m_strBaseBackup = _T("");
	m_strDiffBackup = _T("");
	m_strShutdown = _T("");
	//}}AFX_DATA_INIT
}


void CScheduleResult::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScheduleResult)
	DDX_Text(pDX, IDC_STATIC_BASE_BACKUP, m_strBaseBackup);
	DDX_Text(pDX, IDC_STATIC_DIFF_BACKUP, m_strDiffBackup);
	DDX_Text(pDX, IDC_STATIC_SHUTDOWN_OPTION, m_strShutdown);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScheduleResult, CDialog)
	//{{AFX_MSG_MAP(CScheduleResult)
	ON_BN_CLICKED(IDC_BUTTON_YES, OnButtonYes)
	ON_BN_CLICKED(IDC_BUTTON_NO, OnButtonNo)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL_SCHEDULE, OnButtonCancelSchedule)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScheduleResult message handlers

void CScheduleResult::OnButtonYes() 
{
	// TODO: Add your control notification handler code here
	EndDialog(DIALOG_YES);
}

void CScheduleResult::OnButtonNo() 
{
	// TODO: Add your control notification handler code here
	EndDialog(DIALOG_NO);
}

void CScheduleResult::OnButtonCancelSchedule() 
{
	// TODO: Add your control notification handler code here
	EndDialog(DIALOG_CANCEL);
}

BOOL CScheduleResult::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	GetBaseBackupString(g_ScheduleData,m_strBaseBackup);

	if (g_ScheduleData.wExecuteType == EXCUTE_TYPE_COMBINATION)
		GetDiffBackupString(g_ScheduleData.PeriodTask.OneTaskTime,m_strDiffBackup);

	GetShutdownString(g_ScheduleData.PeriodTask.bShutdown,m_strShutdown);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CScheduleResult::GetBaseBackupString(SCHEDULE_DATA ScheduleData,CString &strBaseBackup)
{
	CString OneString;

	if (ScheduleData.wExecuteType == EXCUTE_TYPE_INCREMENTAL)
		OneString.LoadString(IDS_DIFF_BACKUP);
	else OneString.LoadString(IDS_BASE_BACKUP); // L"Make a total backup"
	
	strBaseBackup = OneString;

	
	if(WEEK_PERIOD  == ScheduleData.nBackupPeriod)
	{
		OneString.LoadString(IDS_EVERY_WEEK); // L"every week"
		
		strBaseBackup=strBaseBackup + OneString;

		// OneString.LoadString(IDS_ON);
		// for japanese letters
		OneString.Empty();

		strBaseBackup=strBaseBackup + OneString;
		GetWeekDayString(ScheduleData.nBaseWeekDay,OneString);
		strBaseBackup=strBaseBackup + OneString;

	}
	else if(MONTH_PERIOD == ScheduleData.nBackupPeriod)
	{
		OneString.LoadString(IDS_EVERY_MONTH);
		strBaseBackup=strBaseBackup + OneString;
		if( (ScheduleData.nBaseMonthDay >=1) && (ScheduleData.nBaseMonthDay <=31) )
		{
		  
		  if(ScheduleData.nBaseMonthDay<=9)
		  {
		     OneString.Format(L" %1d ",ScheduleData.nBaseMonthDay);
		  }
		  else
		  {
		     OneString.Format(L" %2d ",ScheduleData.nBaseMonthDay);
		  }
		  strBaseBackup=strBaseBackup+OneString;
		  OneString.LoadString(IDS_DATE);
		  strBaseBackup=strBaseBackup+OneString;
		}
		else
		{
			TRACE(L"\nmonth nBaseDay error in CScheduleResult::OninitDialog()");
		}
	}
	else
	{
		TRACE(L"\nnBackupPeriod error in GetBaseBackupString");
	}

	OneString.Empty();
	strBaseBackup = strBaseBackup + OneString;

    GetTimeString(ScheduleData.PeriodTask.OneTaskTime, OneString);
	strBaseBackup=strBaseBackup + OneString;

	return TRUE;
}

BOOL CScheduleResult::GetDiffBackupString(CTime OneTime,CString &strDiffBackupInfo)
{

	strDiffBackupInfo.Empty();
	strDiffBackupInfo.LoadString(IDS_DIFF_BACKUP);
	
	CString OneString;
    GetTimeString(OneTime, OneString);
	
	strDiffBackupInfo= strDiffBackupInfo + OneString;
	
	return TRUE;
}

BOOL CScheduleResult::GetTimeString(CTime OneTime, CString &strTimeString)
{
   strTimeString.Empty();
   strTimeString.Format(L"%2d:%.2d",OneTime.GetHour(),OneTime.GetMinute());
   return TRUE;
}

BOOL CScheduleResult::GetShutdownString(BOOL bShutdown, CString &strShutdown)
{
	if(bShutdown)
	{
		strShutdown.LoadString(IDS_SHUTDOWN_COMPUTER);
	}
	else
	{
		strShutdown.LoadString(IDS_DO_NOT_SHUTDOWN);
	}
	return TRUE;
}
