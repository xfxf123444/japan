// SchedulePage3.cpp : implementation file
//

#include "stdafx.h"
#include "am01.h"
#include "Function.h"
#include "SchedulePage3.h"
#include "ScheduleResult.h"

extern CSCHEDULE_ARRAY g_ScheduleArray;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern SCHEDULE_DATA g_ScheduleData;

/////////////////////////////////////////////////////////////////////////////
// CSchedulePage3 property page

IMPLEMENT_DYNCREATE(CSchedulePage3, CPropertyPage)

CSchedulePage3::CSchedulePage3() : CPropertyPage(CSchedulePage3::IDD)
{
	//{{AFX_DATA_INIT(CSchedulePage3)
	m_bShutdown = FALSE;
	//}}AFX_DATA_INIT
}

CSchedulePage3::~CSchedulePage3()
{
}

void CSchedulePage3::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSchedulePage3)
	DDX_Control(pDX, IDC_BACKUP_TIME, m_BackupTime);
	DDX_Check(pDX, IDC_SHUTDOWN_OPTION, m_bShutdown);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSchedulePage3, CPropertyPage)
	//{{AFX_MSG_MAP(CSchedulePage3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSchedulePage3 message handlers

BOOL CSchedulePage3::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertySheet* pSheet = (CPropertySheet*)GetParent();
	ASSERT_KINDOF(CPropertySheet, pSheet);
	pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);

	// (pSheet->GetDlgItem(ID_WIZBACK))->ShowWindow(FALSE);
	(pSheet->GetDlgItem(IDHELP))->ShowWindow(FALSE);
	

	CTime ZeroTime;
	memset(&ZeroTime,0,sizeof(ZeroTime));

	CString strTimeFormat;
	strTimeFormat = L" H:mm ";

	if( ZeroTime == g_ScheduleData.PeriodTask.OneTaskTime )
	{
		CTime OneTime(2003, 01, 01, 0, 0, 0);
	    m_BackupTime.SetTime(&OneTime);
	}
	else
	{
		m_BackupTime.SetTime(&(g_ScheduleData.PeriodTask.OneTaskTime) );
	}

	m_BackupTime.SetFormat(strTimeFormat);

	m_bShutdown = g_ScheduleData.PeriodTask.bShutdown;

	UpdateData(FALSE);

	return CPropertyPage::OnSetActive();
}

BOOL CSchedulePage3::OnWizardFinish() 
{
	// TODO: Add your specialized code here and/or call the base class
	int nItem;

	UpdateData(TRUE);

	g_ScheduleData.PeriodTask.bShutdown = m_bShutdown;
	m_BackupTime.GetTime(g_ScheduleData.PeriodTask.OneTaskTime);
	for (nItem = 0;nItem < g_ScheduleArray.GetSize();nItem ++)
	{
		if (g_ScheduleArray.GetAt(nItem).nScheduleType == ADVANCED_SCHEDULE && 
			g_ScheduleArray.GetAt(nItem).nBackupPeriod == g_ScheduleData.nBackupPeriod)
		{
			if (g_ScheduleArray.GetAt(nItem).PeriodTask.OneTaskTime.GetHour() == g_ScheduleData.PeriodTask.OneTaskTime.GetHour() &&
				g_ScheduleArray.GetAt(nItem).PeriodTask.OneTaskTime.GetMinute() == g_ScheduleData.PeriodTask.OneTaskTime.GetMinute())
			{
				switch (g_ScheduleArray.GetAt(nItem).nBackupPeriod)
				{
				case WEEK_PERIOD:
					if (g_ScheduleArray.GetAt(nItem).nBaseWeekDay == g_ScheduleData.nBaseWeekDay)
					{
						AfxMessageBox(IDS_SCHEDULE_EXIST);
						return FALSE;
					}
					break;
				case MONTH_PERIOD:
					if (g_ScheduleArray.GetAt(nItem).nBaseMonthDay == g_ScheduleData.nBaseMonthDay)
					{
						AfxMessageBox(IDS_SCHEDULE_EXIST);
						return FALSE;
					}
					break;
				}
			}
		}
	}

	CScheduleResult ScheduleResult;

	int nReturnValue;
	
	nReturnValue = ScheduleResult.DoModal();
	
	if( DIALOG_YES == nReturnValue  )
	{
		// here we should record this information to the schedule file.
		g_ScheduleData.nScheduleType = ADVANCED_SCHEDULE;
		g_ScheduleArray.Add(g_ScheduleData);
        RecordSchedule(&g_ScheduleArray);
	    return CPropertyPage::OnWizardFinish();
	}
	else if(DIALOG_NO == nReturnValue)
	{
		CPropertySheet* pSheet = (CPropertySheet*)GetParent();
		ASSERT_KINDOF(CPropertySheet, pSheet);
		pSheet->SetActivePage(pSheet->GetPage(0));
		return FALSE;
	}
	else if(DIALOG_CANCEL == nReturnValue)
	{
   	    return CPropertyPage::OnWizardFinish();
	}

 	return CPropertyPage::OnWizardFinish();

}


