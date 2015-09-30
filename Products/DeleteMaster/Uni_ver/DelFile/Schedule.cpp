// Schedule.cpp : implementation file
//

#include "stdafx.h"
#include "delfile.h"
#include "Schedule.h"
#include "Folder.h"
#include "Fun.h"

/////////////////////////////////////////////////////////////////////////////
// CSchedule dialog


CSchedule::CSchedule(CWnd* pParent /*=NULL*/)
	: CDialog(CSchedule::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSchedule)
	m_bCheckLog = FALSE;
	m_Time = 0;
	m_FilePath = _T("");
	m_nTimeType = 0;
	//}}AFX_DATA_INIT
	m_bModify = FALSE;
}


void CSchedule::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSchedule)
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_ExecuteTime);
	DDX_Control(pDX, IDC_COMBO_WEEK, m_Week);
	DDX_Control(pDX, IDC_COMBO_MONTH, m_Month);
	DDX_Control(pDX, IDC_COMBO_METHOD, m_DeleteMethod);
	DDX_Check(pDX, IDC_CHECK_LOG, m_bCheckLog);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_Time);
	DDX_Text(pDX, IDC_FILE_PATH, m_FilePath);
	DDX_Radio(pDX, IDC_RADIO1, m_nTimeType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSchedule, CDialog)
	//{{AFX_MSG_MAP(CSchedule)
	ON_BN_CLICKED(IDC_REFER, OnRefer)
	ON_BN_CLICKED(IDC_RADIO1, OnRadioDayOfWeek)
	ON_BN_CLICKED(IDC_RADIO2, OnRadioDayOfMonth)
	ON_BN_CLICKED(IDC_RADIO_DELETE_FILE, OnRadioDeleteFile)
	ON_BN_CLICKED(IDC_RADIO_DELETE_RECYCLE, OnRadioDeleteRecycle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSchedule message handlers

BOOL CSchedule::OnInitDialog() 
{
	CString strText;
	int     i,nItem;
	CDialog::OnInitDialog();
	m_ScheduleInfo.ulDeleteObjectType = DELETE_OBJECT_NORMAL;
	//if (m_ScheduleInfo.ulDeleteObjectType == DELETE_OBJECT_RECYCLER)
	//{
	//	CButton *pb = (CButton *)GetDlgItem(IDC_RADIO_DELETE_RECYCLE);
	// 	pb->SetCheck(TRUE);
	//	OnRadioDeleteRecycle();
	//}
	//else
	//{
	//	CButton *pb = (CButton *)GetDlgItem(IDC_RADIO_DELETE_FILE);
	//	pb->SetCheck(TRUE);
		OnRadioDeleteFile();
	//}

	m_ExecuteTime.SetFormat(L"HH:mm");

	for (i = 1;i < 32;i ++)
	{
		strText.Format(L"%d",i);
		nItem = m_Month.InsertString(m_Month.GetCount(),strText);
		if (m_bModify && TIME_DAY_OF_MONTH == m_ScheduleInfo.ulDateType)
		{
			if (i == (int)m_ScheduleInfo.ulDate)
				m_Month.SetCurSel(nItem);
		}
	}

	for (i = 0;i < 7;i ++)
	{
		switch (i)
		{
		case 0:
			strText.LoadString(IDS_SUNDAY);
			break;
		case 1:
			strText.LoadString(IDS_MONDAY);
			break;
		case 2:
			strText.LoadString(IDS_TUESDAY);
			break;
		case 3:
			strText.LoadString(IDS_WEDNESDAY);
			break;
		case 4:
			strText.LoadString(IDS_THURSDAY);
			break;
		case 5:
			strText.LoadString(IDS_FRIDAY);
			break;
		case 6:
			strText.LoadString(IDS_SATURDAY);
			break;
		}
		nItem = m_Week.InsertString(m_Week.GetCount(),strText);
		if (m_bModify && TIME_DAY_OF_WEEK == m_ScheduleInfo.ulDateType)
		{
			if (i == (int)m_ScheduleInfo.ulDate)
				m_Week.SetCurSel(nItem);
		}
	}

	if (m_bModify)
	{
		m_Time = CTime(2003, 01, 01, (m_ScheduleInfo.ExecuteTime&0xff0000)/0x10000, (m_ScheduleInfo.ExecuteTime&0xff00)/0x100, 0);
		m_nTimeType = m_ScheduleInfo.ulDateType - 1;
		m_FilePath = m_ScheduleInfo.szPathFile;
		m_bCheckLog = m_ScheduleInfo.bRecordLog;

		AddDelMethod(&m_DeleteMethod,(int)m_ScheduleInfo.ulDeleteMethod);
	}
	else 
	{
		m_Time = CTime(2003, 01, 01, 8,0, 0);
		AddDelMethod(&m_DeleteMethod,3);
	}
	
	if (m_Week.GetCurSel() == CB_ERR)	m_Week.SetCurSel(0);
	if (m_Month.GetCurSel() == CB_ERR)	m_Month.SetCurSel(0);

	m_Week.EnableWindow(!m_nTimeType);
	m_Month.EnableWindow(m_nTimeType);
	// TODO: Add extra initialization here
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSchedule::OnRefer() 
{
	CFolder Dlg(this);

	Dlg.m_bShowFile = TRUE;
	if (Dlg.DoModal() == IDOK)
	{
		m_FilePath = Dlg.m_csName;
		UpdateData(FALSE);
	}
}

void CSchedule::OnOK() 
{
	ULONG ulFileAttr;
	CString recycled;
	BOOL bExcludeFolder = FALSE;
	// TODO: Add extra validation here
	UpdateData(TRUE);
	switch (m_ScheduleInfo.ulDeleteObjectType)
	{
	case DELETE_OBJECT_NORMAL:
		ulFileAttr = GetFileAttributes(m_FilePath);
		if (ulFileAttr == -1)
		{
			AfxMessageBox(IDS_INVALID_PATH,MB_OK | MB_ICONINFORMATION);
			return;
		}

		CheckExcludeFolder(m_FilePath,bExcludeFolder);
		if (bExcludeFolder)
		{
			AfxMessageBox(IDS_PROTECT_PATH,MB_OK | MB_ICONINFORMATION);
			return;
		}
		wcsncpy(m_ScheduleInfo.szPathFile,m_FilePath,MAX_PATH);
		break;
	case DELETE_OBJECT_RECYCLER:
		recycled.LoadString(IDS_RECYCLED);
		ulFileAttr = FILE_ATTRIBUTE_DIRECTORY;
		wcsncpy(m_ScheduleInfo.szPathFile,recycled.GetBuffer(0),MAX_PATH);
		break;
	}

	if (m_nTimeType)
		m_ScheduleInfo.ulDate = m_Month.GetCurSel() + 1;
	else
		m_ScheduleInfo.ulDate = m_Week.GetCurSel();
	if (m_ScheduleInfo.ulDate == -1)
	{
		AfxMessageBox(IDS_INVALID_DATE,MB_OK | MB_ICONINFORMATION);
		return;
	}

	m_ScheduleInfo.bRecordLog = m_bCheckLog;
	m_ScheduleInfo.ulDateType = m_nTimeType + 1;

	m_ScheduleInfo.ExecuteTime = m_Time.GetHour()*0x10000 + m_Time.GetMinute()*0x100;

	m_ScheduleInfo.ulDeleteMethod = m_DeleteMethod.GetCurSel();

	m_ScheduleInfo.ulFileattribute = ulFileAttr;

	GetLocalTime(&m_ScheduleInfo.LastExcuteTime);

	CDialog::OnOK();
}

void CSchedule::OnRadioDayOfWeek() 
{
	UpdateData(TRUE);
	m_Week.EnableWindow(!m_nTimeType);
	m_Month.EnableWindow(m_nTimeType);
	// TODO: Add your control notification handler code here
	
}

void CSchedule::OnRadioDayOfMonth() 
{
	UpdateData(TRUE);
	m_Week.EnableWindow(!m_nTimeType);
	m_Month.EnableWindow(m_nTimeType);
	// TODO: Add your control notification handler code here
}


void CSchedule::OnRadioDeleteFile() 
{
	// TODO: Add your control notification handler code here

	CEdit *pEdit;
	CButton *pButton;

	m_ScheduleInfo.ulDeleteObjectType = DELETE_OBJECT_NORMAL;

	pEdit = (CEdit *)GetDlgItem(IDC_FILE_PATH);
	pEdit->EnableWindow(TRUE);
	pButton = (CButton *)GetDlgItem(IDC_REFER);
	pButton->EnableWindow(TRUE);
	//pButton = (CButton *)GetDlgItem(IDC_CHECK_LOG);
	//pButton->EnableWindow(TRUE);
}

void CSchedule::OnRadioDeleteRecycle() 
{
	// TODO: Add your control notification handler code here
	
	CEdit *pEdit;
	CButton *pButton;

	m_ScheduleInfo.ulDeleteObjectType = DELETE_OBJECT_RECYCLER;

	pEdit = (CEdit *)GetDlgItem(IDC_FILE_PATH);
	pEdit->EnableWindow(FALSE);
	pButton = (CButton *)GetDlgItem(IDC_REFER);
	pButton->EnableWindow(FALSE);
//	pButton = (CButton *)GetDlgItem(IDC_CHECK_LOG);
//	pButton->EnableWindow(FALSE);
}
