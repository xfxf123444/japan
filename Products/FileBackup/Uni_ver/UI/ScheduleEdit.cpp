// DelShredder.cpp : implementation file
//

#include "stdafx.h"
#include "AM01.h"
#include "ScheduleEdit.h"
#include "AMTask.h"
#include "..\AM01Expt\AM01Expt.h"
#include "Function.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CSCHEDULE_ARRAY g_ScheduleArray;
SCHEDULE_DATA g_ScheduleData; // this variable is used to record the data received by wizard.

/////////////////////////////////////////////////////////////////////////////
// CScheduleEdit dialog


CScheduleEdit::CScheduleEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CScheduleEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScheduleEdit)
	//}}AFX_DATA_INIT
}


void CScheduleEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScheduleEdit)
	DDX_Control(pDX, IDC_SCHEDULE_LIST, m_ScheduleList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScheduleEdit, CDialog)
	//{{AFX_MSG_MAP(CScheduleEdit)
	ON_BN_CLICKED(IDC_ADD_SCHEDULE, OnAddSchedule)
	ON_BN_CLICKED(IDC_DELETE_SCHEDULE, OnDeleteSchedule)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, OnButtonClose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScheduleEdit message handlers

BOOL CScheduleEdit::OnInitDialog() 
{
	CBitmap Bitmap;
	CDialog::OnInitDialog();
	
	Bitmap.LoadBitmap(IDB_STATEICONS);
	m_ImgList.Create(16,16,ILC_COLOR16,0,10);
	m_ImgList.Add(&Bitmap, RGB(0,0,0));
	InitList();
	AddList();
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CScheduleEdit::InitList()
{
	CRect	ListRect;
	int		nWidth;
	CString cstr;

	m_ScheduleList.GetWindowRect(&ListRect);
	ScreenToClient(ListRect);

	m_ScheduleList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	m_ScheduleList.GetClientRect(ListRect);
	nWidth = (ListRect.right-ListRect.left)/7;

	cstr.LoadString (IDS_SETTING_NAME_TITLE);
	m_ScheduleList.InsertColumn(0,cstr,LVCFMT_CENTER,nWidth*3);

	cstr.LoadString (IDS_BACKUP_TYPE_TITLE);
	m_ScheduleList.InsertColumn(1,cstr,LVCFMT_CENTER,nWidth);

	cstr.LoadString (IDS_EXCUTE_DATE);
	m_ScheduleList.InsertColumn(2,cstr,LVCFMT_CENTER,nWidth);

	cstr.LoadString (IDS_EXCUTE_TIME);
	m_ScheduleList.InsertColumn(3,cstr,LVCFMT_CENTER,nWidth);

	cstr.LoadString (IDS_SHUTDOWN);
	m_ScheduleList.InsertColumn(4,cstr,LVCFMT_CENTER,nWidth);

	m_ScheduleList.SetImageList(&m_ImgList,LVSIL_SMALL);

	return;
}

void CScheduleEdit::AddList()
{
	CString strText;
	int   nItemIndex,nItem;

	m_ScheduleList.DeleteAllItems();
	for(nItemIndex=0; nItemIndex<g_ScheduleArray.GetSize(); nItemIndex++ )
	{
		if (g_ScheduleArray.GetAt(nItemIndex).nScheduleType == ADVANCED_SCHEDULE &&
			!wcsicmp(g_ScheduleArray.GetAt(nItemIndex).szSettingName,m_strSetting.GetBuffer(0)))
		{
			nItem = m_ScheduleList.InsertItem(m_ScheduleList.GetItemCount(),g_ScheduleArray.GetAt(nItemIndex).szSettingName,2);

			switch (g_ScheduleArray.GetAt(nItemIndex).wExecuteType)
			{
			case EXCUTE_TYPE_COMBINATION:
				strText.LoadString(IDS_COMBINATION_BACKUP);
				break;
			case EXCUTE_TYPE_FULL:
				strText.LoadString(IDS_FULL_BACKUP);
				break;
			case EXCUTE_TYPE_INCREMENTAL:
				strText.LoadString(IDS_INCREMENTAL_BACKUP);
				break;
			default:
				strText.LoadString(IDS_UNKNOW);
				break;
			}
			m_ScheduleList.SetItemText(nItem,1,strText);

			switch (g_ScheduleArray.GetAt(nItemIndex).nBackupPeriod)
			{
			case WEEK_PERIOD:
				GetWeekDayString(g_ScheduleArray.GetAt(nItemIndex).nBaseWeekDay,strText);
				break;
			case MONTH_PERIOD:
				strText.Format(IDS_DAY_OF_MONTH,g_ScheduleArray.GetAt(nItemIndex).nBaseMonthDay);
				break;
			default:
				strText.LoadString(IDS_UNKNOW);
				break;
			}
			m_ScheduleList.SetItemText(nItem,2,strText);

			strText.Format(L"%2.2d:%2.2d",g_ScheduleArray.GetAt(nItemIndex).PeriodTask.OneTaskTime.GetHour(),g_ScheduleArray[nItemIndex].PeriodTask.OneTaskTime.GetMinute());
			m_ScheduleList.SetItemText(nItem,3,strText);

			strText.LoadString(g_ScheduleArray.GetAt(nItemIndex).PeriodTask.bShutdown?IDS_YES:IDS_NO);
			m_ScheduleList.SetItemText(nItem,4,strText);

			m_ScheduleList.SetItemData(nItem,nItemIndex);
		}
	}
	return ;
}

void CScheduleEdit::OnAddSchedule() 
{
	memset(&g_ScheduleData,0,sizeof(g_ScheduleData));

	wcsncpy(g_ScheduleData.szSettingName,m_strSetting,MAX_SETTING_NAME_LEN);
    
	m_pScheduleWizard = new CScheduleWizard(L"Schedule Wizard", this);
	m_pSchedulePage1 = new CSchedulePage1;
	m_pSchedulePage1->Construct(IDD_SCHEDULE_PAGE_1, 0);

	m_pSchedulePage2 = new CSchedulePage2;
	m_pSchedulePage2->Construct(IDD_SCHEDULE_PAGE_2, 0);

	m_pSchedulePage3 = new CSchedulePage3;
	m_pSchedulePage3->Construct(IDD_SCHEDULE_PAGE_3, 0);

	// Add the property pages to the property sheet
	m_pScheduleWizard->AddPage(m_pSchedulePage1);
	m_pScheduleWizard->AddPage(m_pSchedulePage2);
	m_pScheduleWizard->AddPage(m_pSchedulePage3);
	m_pScheduleWizard->SetWizardMode();   
	m_pScheduleWizard->DoModal();

	delete m_pSchedulePage1;
	delete m_pSchedulePage2;
	delete m_pSchedulePage3;
	delete m_pScheduleWizard;
	AddList();
	// TODO: Add your control notification handler code here
}

void CScheduleEdit::OnDeleteSchedule() 
{
	// TODO: Add your control notification handler code here
	CString strText;
	WCHAR szSchedule[MAX_PATH];
	int nTotal,i;

	if( 0 ==  m_ScheduleList.GetSelectedCount() )
	{
		strText.LoadString(IDS_SELECT_SCHEDULE);
		AfxMessageBox(strText,MB_OK | MB_ICONINFORMATION);
		return;
	}

	nTotal = m_ScheduleList.GetItemCount();
	for (i = 0;i < nTotal;i ++)
	{
		if (m_ScheduleList.GetItemState(i,LVIS_SELECTED))
		{
			int nItemIndex;
			m_ScheduleList.GetItemText(i,0,szSchedule,MAX_PATH);

			for( nItemIndex=0; nItemIndex<g_ScheduleArray.GetSize(); nItemIndex++ )
			{
				if (g_ScheduleArray.GetAt(nItemIndex).nScheduleType == ADVANCED_SCHEDULE &&
					!wcsicmp(g_ScheduleArray.GetAt(nItemIndex).szSettingName,szSchedule))
				{
					if (nItemIndex == (int)m_ScheduleList.GetItemData(i))
					{
						g_ScheduleArray.RemoveAt(nItemIndex);
						RecordSchedule(&g_ScheduleArray);
						break;
					}
				}
			}
			AddList();
			break;
		}
	}	
}

void CScheduleEdit::OnButtonClose() 
{
	CDialog::OnOK();
	// TODO: Add your control notification handler code here
	
}
