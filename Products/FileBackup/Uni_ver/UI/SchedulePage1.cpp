// SchedulePage1.cpp : implementation file
//

#include "stdafx.h"
#include "am01.h"
#include "../AM01Struct/AM01Struct.h"
#include "SchedulePage1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern SCHEDULE_DATA g_ScheduleData; // this variable is used to record the data received by wizard.

/////////////////////////////////////////////////////////////////////////////
// CSchedulePage1 property page

IMPLEMENT_DYNCREATE(CSchedulePage1, CPropertyPage)

CSchedulePage1::CSchedulePage1() : CPropertyPage(CSchedulePage1::IDD)
{
	//{{AFX_DATA_INIT(CSchedulePage1)
	m_nBackupPeriod = 0;
	m_nExecuteType = 0;
	//}}AFX_DATA_INIT
}

CSchedulePage1::~CSchedulePage1()
{
}

void CSchedulePage1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSchedulePage1)
	DDX_Radio(pDX, IDC_BACKUP_PERIOD, m_nBackupPeriod);
	DDX_Radio(pDX, IDC_BACKUP_TYPE, m_nExecuteType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSchedulePage1, CPropertyPage)
	//{{AFX_MSG_MAP(CSchedulePage1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSchedulePage1 message handlers

BOOL CSchedulePage1::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class

	CPropertySheet* pSheet = (CPropertySheet*)GetParent();
	ASSERT_KINDOF(CPropertySheet, pSheet);
	pSheet->SetWizardButtons(PSWIZB_NEXT);

	(pSheet->GetDlgItem(IDHELP))->ShowWindow(FALSE);
	
	// this is original value
	if(
		 ( 
		    ADVANCED_SCHEDULE == g_ScheduleData.nScheduleType
		 )  &&
		 
		 (
		    ( WEEK_PERIOD == g_ScheduleData.nBackupPeriod ) ||
	        ( MONTH_PERIOD == g_ScheduleData.nBackupPeriod )
		 )
	  )
	{
	     m_nBackupPeriod = g_ScheduleData.nBackupPeriod;
	}
	else
	{
		 m_nBackupPeriod = WEEK_PERIOD;
	}

	if(
		 ( 
		    ADVANCED_SCHEDULE == g_ScheduleData.nScheduleType
		 )  &&
		 
		 (
		    ( EXCUTE_TYPE_COMBINATION == g_ScheduleData.wExecuteType ) ||
		    ( EXCUTE_TYPE_INCREMENTAL == g_ScheduleData.wExecuteType ) ||
	        ( EXCUTE_TYPE_FULL == g_ScheduleData.wExecuteType )
		 )
	  )
	{
	     m_nExecuteType = g_ScheduleData.wExecuteType;
	}
	else
	{
		 m_nExecuteType = EXCUTE_TYPE_COMBINATION;
	}

	UpdateData(FALSE);

	return CPropertyPage::OnSetActive();
}

LRESULT CSchedulePage1::OnWizardNext() 
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateData(TRUE);
	g_ScheduleData.nBackupPeriod = m_nBackupPeriod;
	g_ScheduleData.wExecuteType = m_nExecuteType;
	return CPropertyPage::OnWizardNext();
}
