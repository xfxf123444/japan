// SchedulePage2.cpp : implementation file
//

#include "stdafx.h"
#include "am01.h"
#include "SchedulePage2.h"
#include "Function.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern SCHEDULE_DATA g_ScheduleData;

/////////////////////////////////////////////////////////////////////////////
// CSchedulePage2 property page

IMPLEMENT_DYNCREATE(CSchedulePage2, CPropertyPage)

CSchedulePage2::CSchedulePage2() : CPropertyPage(CSchedulePage2::IDD)
{
	//{{AFX_DATA_INIT(CSchedulePage2)
	m_strDateTitle = _T("");
	//}}AFX_DATA_INIT
}

CSchedulePage2::~CSchedulePage2()
{
}

void CSchedulePage2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSchedulePage2)
	DDX_Control(pDX, IDC_COMBO_DATE, m_DateCtrl);
	DDX_Text(pDX, IDC_DATE_TITLE, m_strDateTitle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSchedulePage2, CPropertyPage)
	//{{AFX_MSG_MAP(CSchedulePage2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSchedulePage2 message handlers

BOOL CSchedulePage2::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class

	CPropertySheet* pSheet = (CPropertySheet*)GetParent();
	ASSERT_KINDOF(CPropertySheet, pSheet);
	pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
	(pSheet->GetDlgItem(IDHELP))->ShowWindow(FALSE);

	CString OneString;

	TRACE(L"\ng_ScheduleData.nBaseWeekDay=%d",g_ScheduleData.nBaseWeekDay);

	if( WEEK_PERIOD == g_ScheduleData.nBackupPeriod )
	{
		OneString.LoadString(IDS_DAY_OF_WEEK);
		m_strDateTitle=OneString;
		UpdateData(FALSE);

		m_DateCtrl.ResetContent();

		for (int i = 0;i < 7;i ++)
		{
			GetWeekDayString(i,OneString);
			OneString.TrimRight(L" ");
			m_DateCtrl.AddString( LPCTSTR(OneString) );
		}
		
		if( (g_ScheduleData.nBaseWeekDay >=0) && (g_ScheduleData.nBaseWeekDay <= 6) )
		{
		   m_DateCtrl.SetCurSel(g_ScheduleData.nBaseWeekDay); // sunday to saturday
		}
		else
		{
		   TRACE(L"\ng_ScheduleData.nBaseWeekDay error, perhaps no this data in data file");
		   m_DateCtrl.SetCurSel(0);
		}

	}
	else if( MONTH_PERIOD == g_ScheduleData.nBackupPeriod )
	{
		OneString.LoadString(IDS_DATE_OF_MONTH);
		m_strDateTitle=OneString;
		UpdateData(FALSE);

		m_DateCtrl.ResetContent();

		for(int i=1; i<=31; i++)
		{
			OneString.Format(L"%-2d",i);
			m_DateCtrl.AddString((LPCTSTR)OneString);
		}
		
		// m_DateCtrl.SetCurSel(0);
		
		if( (g_ScheduleData.nBaseMonthDay >=1) && (g_ScheduleData.nBaseMonthDay <=31) )
		{
		    m_DateCtrl.SetCurSel(g_ScheduleData.nBaseMonthDay-1);
		}
		else
		{
			m_DateCtrl.SetCurSel(0);
			TRACE(L"\ng_ScheduleData.nBaseDay error, perhaps no this record in data file.");
		}

	}

	UpdateData(FALSE);

	return CPropertyPage::OnSetActive();
}

LRESULT CSchedulePage2::OnWizardNext() 
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateData(TRUE);
    
	if( WEEK_PERIOD == g_ScheduleData.nBackupPeriod )
	{
	   g_ScheduleData.nBaseWeekDay = m_DateCtrl.GetCurSel();
	}
	else if( MONTH_PERIOD == g_ScheduleData.nBackupPeriod )
	{
	   g_ScheduleData.nBaseMonthDay = m_DateCtrl.GetCurSel()+1;
	}
	
	return CPropertyPage::OnWizardNext();
}

