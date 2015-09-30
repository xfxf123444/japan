// AMTask.cpp : implementation file
//

#include "stdafx.h"
// #include "AM01.h"  // this is included in AMTask.h
#include "AMTask.h"
#include "..\AM01Expt\AM01Expt.h"
#include "Function.h"
#include "ScheduleEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// g_ScheduleArray is used in two places
// first, the timer in the main program will check it
// second, in the AMTASK, the program may modify g_ScheduleArray

extern CSCHEDULE_ARRAY g_ScheduleArray;

/////////////////////////////////////////////////////////////////////////////
// CAMTask property page

IMPLEMENT_DYNCREATE(CAMTask, CPropertyPage)

CAMTask::CAMTask() : CPropertyPage(CAMTask::IDD)
{
	//{{AFX_DATA_INIT(CAMTask)
	m_bCheckMonday = FALSE;
	m_bCheckWednesday = FALSE;
	m_bCheckThursday = FALSE;
	m_bCheckFriday = FALSE;
	m_bCheckSaturday = FALSE;
	m_bCheckSunday = FALSE;
	m_bCheckTuesday = FALSE;
	m_bShutFriday = FALSE;
	m_bShutMonday = FALSE;
	m_bShutSaturday = FALSE;
	m_bShutSunday = FALSE;
	m_bShutThursday = FALSE;
	m_bShutTuesday = FALSE;
	m_bShutWednesday = FALSE;
	//}}AFX_DATA_INIT

}

CAMTask::~CAMTask()
{
}

void CAMTask::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAMTask)
	DDX_Control(pDX, IDC_LIST_SETTING_NAME, m_SettingList);
	DDX_Control(pDX, IDC_DATETIMEPICKER8, m_SundayCtrl);
	DDX_Control(pDX, IDC_DATETIMEPICKER7, m_SaturdayCtrl);
	DDX_Control(pDX, IDC_DATETIMEPICKER6, m_FridayCtrl);
	DDX_Control(pDX, IDC_DATETIMEPICKER5, m_ThursdayCtrl);
	DDX_Control(pDX, IDC_DATETIMEPICKER4, m_WednesdayCtrl);
	DDX_Control(pDX, IDC_DATETIMEPICKER3, m_TuesdayCtrl);
	DDX_Control(pDX, IDC_DATETIMEPICKER2, m_MondayCtrl);
	DDX_Check(pDX, IDC_CHECK1, m_bCheckMonday);
	DDX_Check(pDX, IDC_CHECK3, m_bCheckWednesday);
	DDX_Check(pDX, IDC_CHECK4, m_bCheckThursday);
	DDX_Check(pDX, IDC_CHECK5, m_bCheckFriday);
	DDX_Check(pDX, IDC_CHECK6, m_bCheckSaturday);
	DDX_Check(pDX, IDC_CHECK7, m_bCheckSunday);
	DDX_Check(pDX, IDC_CHECK2, m_bCheckTuesday);
	DDX_Check(pDX, IDC_CHECK_SHUT_FRIDAY, m_bShutFriday);
	DDX_Check(pDX, IDC_CHECK_SHUT_MONDAY, m_bShutMonday);
	DDX_Check(pDX, IDC_CHECK_SHUT_SATURDAY, m_bShutSaturday);
	DDX_Check(pDX, IDC_CHECK_SHUT_SUNDAY, m_bShutSunday);
	DDX_Check(pDX, IDC_CHECK_SHUT_THURSDAY, m_bShutThursday);
	DDX_Check(pDX, IDC_CHECK_SHUT_TUESDAY, m_bShutTuesday);
	DDX_Check(pDX, IDC_CHECK_SHUT_WEDNESDAY, m_bShutWednesday);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAMTask, CPropertyPage)
	//{{AFX_MSG_MAP(CAMTask)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_SAVE_SCHEDULE, OnButtonSaveSchedule)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SETTING_NAME, OnItemchangedListSettingName)
	ON_BN_CLICKED(IDC_BUTTON_ADVANCED_SCHEDULE, OnButtonAdvancedSchedule)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAMTask message handlers

BOOL CAMTask::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// m_pImageList will be used in other function.
	// in the function to fill List Control

	// I think m_pImageList may be replaced by m_ImageList
	// It is also OK without using the L"new" operator
	m_pImageList = NULL;
	m_pImageList = new CImageList;

	InitSettingControl(&m_SettingList,m_pImageList);
	
	InitTimeFormat();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAMTask::SaveControl()
{
	int nItemIndex;
	BOOL bFound = FALSE;
	UpdateData(TRUE);

	SCHEDULE_DATA ScheduleData;

	if( FALSE != GetUITaskInfo(ScheduleData) )
	{

		ScheduleData.nScheduleType = NORMAL_SCHEDULE;
		
		for(nItemIndex=g_ScheduleArray.GetSize()-1; nItemIndex>=0; nItemIndex-- )
		{
			if (g_ScheduleArray.GetAt(nItemIndex).nScheduleType == NORMAL_SCHEDULE &&
				!wcsicmp(g_ScheduleArray.GetAt(nItemIndex).szSettingName,ScheduleData.szSettingName))
			{
				g_ScheduleArray[nItemIndex] = ScheduleData;
				bFound = TRUE;
			}
		}

		if (!bFound) g_ScheduleArray.Add(ScheduleData);

		RecordSchedule(&g_ScheduleArray);
	}
	else
	{
		TRACE(L"\nGetUITaskInfo Error.");
	}


}


BOOL CAMTask::OnSetActive() 
{
	CStringArray SettingArray;
	RefreshSettingInfo(&SettingArray,&m_SettingList);
	
	return CPropertyPage::OnSetActive();
}

void CAMTask::InitTimeFormat()
{
    CTime OneTime(2003, 01, 01, 0, 0, 0);

	// memset(&OneTime,0,sizeof(OneTime));

	CString strTimeFormat;
	strTimeFormat = L" H:mm ";
	// strTimeFormat = L"H:mm";
	m_MondayCtrl.SetTime(&OneTime);
	m_MondayCtrl.SetFormat(strTimeFormat);

	m_TuesdayCtrl.SetTime(&OneTime);
	m_TuesdayCtrl.SetFormat(strTimeFormat);

	m_WednesdayCtrl.SetTime(&OneTime);
	m_WednesdayCtrl.SetFormat(strTimeFormat);

	m_ThursdayCtrl.SetTime(&OneTime);
	m_ThursdayCtrl.SetFormat(strTimeFormat);

	m_FridayCtrl.SetTime(&OneTime);
	m_FridayCtrl.SetFormat(strTimeFormat);

	m_SaturdayCtrl.SetTime(&OneTime);
	m_SaturdayCtrl.SetFormat(strTimeFormat);

	m_SundayCtrl.SetTime(&OneTime);
	m_SundayCtrl.SetFormat(strTimeFormat);

	m_bCheckSunday=FALSE;
	m_bShutSunday=FALSE;

	m_bCheckMonday=FALSE;
	m_bShutMonday=FALSE;

	m_bCheckTuesday=FALSE;
	m_bShutTuesday=FALSE;

	m_bCheckWednesday=FALSE;
	m_bShutWednesday=FALSE;

	m_bCheckThursday=FALSE;
	m_bShutThursday=FALSE;

	m_bCheckFriday=FALSE;
	m_bShutFriday=FALSE;

	m_bCheckSaturday=FALSE;
	m_bShutSaturday=FALSE;

	UpdateData(FALSE);

}

void CAMTask::OnDestroy() 
{
	CPropertyPage::OnDestroy();
	
	// TODO: Add your message handler code here
	if( m_pImageList != NULL)
	{
		delete m_pImageList;
		m_pImageList=NULL;
	}
}

BOOL CAMTask::GetUITaskInfo(SCHEDULE_DATA &ScheduleData)
{
	
	UpdateData(TRUE);

	memset(&ScheduleData,0,sizeof(ScheduleData));

	if( FALSE == GetSelectedSetting(&m_SettingList,ScheduleData.szSettingName))
	{
		TRACE(L"\nYou did not select a setting, GetUITaskInfo error.");
		return FALSE;
	}


	if(m_bCheckSunday)
	{
	  ScheduleData.DayTask[0].bSchedule = TRUE;
	  m_SundayCtrl.GetTime(ScheduleData.DayTask[0].OneTaskTime);
	  ScheduleData.DayTask[0].bShutdown = m_bShutSunday;
	}
	

	if(m_bCheckMonday)
	{
	  ScheduleData.DayTask[1].bSchedule = TRUE;
	  m_MondayCtrl.GetTime(ScheduleData.DayTask[1].OneTaskTime);
	  ScheduleData.DayTask[1].bShutdown = m_bShutMonday;
	}

	if(m_bCheckTuesday)
	{
	  ScheduleData.DayTask[2].bSchedule = TRUE;
	  m_TuesdayCtrl.GetTime(ScheduleData.DayTask[2].OneTaskTime);
	  ScheduleData.DayTask[2].bShutdown = m_bShutTuesday;
	}

	if(m_bCheckWednesday)
	{
	  ScheduleData.DayTask[3].bSchedule = TRUE;
	  m_WednesdayCtrl.GetTime(ScheduleData.DayTask[3].OneTaskTime);
	  ScheduleData.DayTask[3].bShutdown = m_bShutWednesday;
	}
	
	if(m_bCheckThursday)
	{
	  ScheduleData.DayTask[4].bSchedule = TRUE;
	  m_ThursdayCtrl.GetTime(ScheduleData.DayTask[4].OneTaskTime);
	  ScheduleData.DayTask[4].bShutdown = m_bShutThursday;
	}

	if(m_bCheckFriday)
	{
	  ScheduleData.DayTask[5].bSchedule = TRUE;
	  m_FridayCtrl.GetTime(ScheduleData.DayTask[5].OneTaskTime);
	  ScheduleData.DayTask[5].bShutdown = m_bShutFriday;
	}

	if(m_bCheckSaturday)
	{
	  ScheduleData.DayTask[6].bSchedule = TRUE;
	  m_SaturdayCtrl.GetTime(ScheduleData.DayTask[6].OneTaskTime);
	  ScheduleData.DayTask[6].bShutdown = m_bShutSaturday;
	}

	return TRUE;
}

BOOL CAMTask::SetUITaskInfo(SCHEDULE_DATA ScheduleData)
{
	if( NORMAL_SCHEDULE != ScheduleData.nScheduleType )
	{
		TRACE(L"\nnScheduleType is not NORMAL_SCHEDULE, needn't SetUITaskInfo");
		InitTimeFormat();
		return FALSE;
	}

	if( FALSE == GetSelectedSetting(&m_SettingList,ScheduleData.szSettingName))
	{
		TRACE(L"\nGetSelectedSetting error in CAMTask::SetUITaskInfo, Maybe no TaskInfo");

		return FALSE;
	}

	InitTimeFormat();

	if(ScheduleData.DayTask[0].bSchedule)
	{
	  m_bCheckSunday = ScheduleData.DayTask[0].bSchedule;
	  m_SundayCtrl.SetTime(& (ScheduleData.DayTask[0].OneTaskTime ));
	  m_bShutSunday = ScheduleData.DayTask[0].bShutdown;
	}

	if(ScheduleData.DayTask[1].bSchedule)
	{
	  m_bCheckMonday = ScheduleData.DayTask[1].bSchedule;
	  m_MondayCtrl.SetTime(& (ScheduleData.DayTask[1].OneTaskTime));
	  m_bShutMonday = ScheduleData.DayTask[1].bShutdown;
	}

	if(ScheduleData.DayTask[2].bSchedule)
	{
	  m_bCheckTuesday = ScheduleData.DayTask[2].bSchedule;
	  m_TuesdayCtrl.SetTime(& (ScheduleData.DayTask[2].OneTaskTime));
	  m_bShutTuesday = ScheduleData.DayTask[2].bShutdown;
	}

	if(ScheduleData.DayTask[3].bSchedule)
	{
	  m_bCheckWednesday = ScheduleData.DayTask[3].bSchedule;
	  m_WednesdayCtrl.SetTime(& (ScheduleData.DayTask[3].OneTaskTime) );
	  m_bShutWednesday = ScheduleData.DayTask[3].bShutdown;
	}

	if(ScheduleData.DayTask[4].bSchedule)
	{
	  m_bCheckThursday = ScheduleData.DayTask[4].bSchedule;
	  m_ThursdayCtrl.SetTime(& (ScheduleData.DayTask[4].OneTaskTime));
	  m_bShutThursday = ScheduleData.DayTask[4].bShutdown;
	}

	if( ScheduleData.DayTask[5].bSchedule)
	{
	  m_bCheckFriday = ScheduleData.DayTask[5].bSchedule;
	  m_FridayCtrl.SetTime(& (ScheduleData.DayTask[5].OneTaskTime));
	  m_bShutFriday = ScheduleData.DayTask[5].bShutdown;
	}

	if( ScheduleData.DayTask[6].bSchedule )
	{
	  m_bCheckSaturday = ScheduleData.DayTask[6].bSchedule;
	  m_SaturdayCtrl.SetTime(& (ScheduleData.DayTask[6].OneTaskTime));
	  m_bShutSaturday = ScheduleData.DayTask[6].bShutdown;
	}

	UpdateData(FALSE);

	return TRUE;
}

void CAMTask::OnButtonSaveSchedule() 
{
	// TODO: Add your control notification handler code here
	SaveControl();
	
	// Update the schedule array
	GetScheduleArray(&g_ScheduleArray);
}

void CAMTask::OnItemchangedListSettingName(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here

    RefreshTaskInfo();

	*pResult = 0;
}


void CAMTask::OnButtonAdvancedSchedule() 
{
	CScheduleEdit ScheduleDlg;
	// TODO: Add your control notification handler code here
	WCHAR szSelectedSetting[MAX_SETTING_NAME_LEN];
	
	memset(szSelectedSetting,0,sizeof(szSelectedSetting));

	if( FALSE != GetSelectedSetting(&m_SettingList,szSelectedSetting) )
	{
		ScheduleDlg.m_strSetting = szSelectedSetting;
		ScheduleDlg.DoModal();
		RefreshTaskInfo();
	}
}

BOOL CAMTask::RefreshTaskInfo()
{
	WCHAR szSelectedSetting[MAX_SETTING_NAME_LEN];
	
	memset(szSelectedSetting,0,sizeof(szSelectedSetting));

	if( FALSE != GetSelectedSetting(&m_SettingList,szSelectedSetting) )
	{
		SCHEDULE_DATA ScheduleData;

		if ( ReadScheduleInfo(szSelectedSetting,&ScheduleData) )
		{
			SetUITaskInfo(ScheduleData);
		}
		else
		{
			TRACE(L"\nReadScheduleInfo error in RefresTaskInfo.");
			InitTimeFormat();
		}
	}
	else
	{
		TRACE(L"\nNo item selected, can not RefreshTaskInfo.");
		return FALSE;
	}

	return TRUE;
}
