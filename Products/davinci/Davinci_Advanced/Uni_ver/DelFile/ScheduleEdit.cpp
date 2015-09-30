// DelShredder.cpp : implementation file
//

#include "stdafx.h"
#include "DelFile.h"
#include "ScheduleEdit.h"
#include "Schedule.h"
#include "PasswordDlg.h"

extern CSCHEDULE_ITEM_ARRAY g_ScheduleItemArray;

/////////////////////////////////////////////////////////////////////////////
// CScheduleEdit dialog


CScheduleEdit::CScheduleEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CScheduleEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScheduleEdit)
	//}}AFX_DATA_INIT
	m_ItemNum = 0;
}


void CScheduleEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScheduleEdit)
	DDX_Control(pDX, IDC_SET_PASSWORD, m_SetPassword);
	DDX_Control(pDX, IDC_MODIFY_SCHEDULE, m_ModifySchedule);
	DDX_Control(pDX, IDC_DELETE_SCHEDULE, m_DeleteSchedule);
	DDX_Control(pDX, IDC_ADD_SCHEDULE, m_AddSchedule);
	DDX_Control(pDX, IDC_SCHEDULE_LIST, m_ScheduleList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScheduleEdit, CDialog)
	//{{AFX_MSG_MAP(CScheduleEdit)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_ADD_SCHEDULE, OnAddSchedule)
	ON_BN_CLICKED(IDC_MODIFY_SCHEDULE, OnModifySchedule)
	ON_BN_CLICKED(IDC_DELETE_SCHEDULE, OnDeleteSchedule)
	ON_BN_CLICKED(IDC_SET_PASSWORD, OnSetPassword)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScheduleEdit message handlers

BOOL CScheduleEdit::OnInitDialog() 
{
	CBitmap Bitmap;
	CDialog::OnInitDialog();
	
	Bitmap.LoadBitmap(IDB_TREE);
	m_ImgList.Create(16,16,ILC_COLOR16,0,10);
	m_ImgList.Add(&Bitmap, RGB(0,0,0));
	InitList();
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
	nWidth = (ListRect.right-ListRect.left)/6;

	cstr.LoadString (IDS_DELETED_FILES);
	m_ScheduleList.InsertColumn(0,cstr,LVCFMT_CENTER,nWidth*4);

	cstr.LoadString (IDS_DELETE_DATE);
	m_ScheduleList.InsertColumn(1,cstr,LVCFMT_CENTER,nWidth);

	cstr.LoadString (IDS_DELETE_TIME);
	m_ScheduleList.InsertColumn(2,cstr,LVCFMT_CENTER,nWidth);

	m_ScheduleList.SetImageList(&m_ImgList,LVSIL_SMALL);

	return;
}

void CScheduleEdit::AddList()
{
	CString strText;
	int   nItemIndex;

	m_ScheduleList.DeleteAllItems();
	g_ScheduleItemArray.RemoveAll();

	if(ReadSetting(g_ScheduleItemArray))
	{
		for(nItemIndex=0; nItemIndex<g_ScheduleItemArray.GetSize(); nItemIndex++ )
		{
			ModifyListItem(m_ScheduleList.GetItemCount(),&g_ScheduleItemArray[nItemIndex],TRUE);
		}
	}
	return ;
}

LRESULT CScheduleEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch(message)  
	{  
	case  WM_REFRESH_ITEM: 
		AddList();
		break;  
	}  
	
	return CDialog::WindowProc(message, wParam, lParam);
}

void CScheduleEdit::OnSize(UINT nType, int cx, int cy) 
{
	CRect RectWin,RectButton;
	int   nButton,nButtonWidth;

	CDialog::OnSize(nType, cx, cy);

	GetClientRect(&RectWin);
	RectWin.left += 10;
	RectWin.right -= 10;

	if (IsWindow(m_ScheduleList.m_hWnd))
	{
		RectButton.top = RectWin.top + 10;
		RectButton.left = RectWin.left;
		RectButton.bottom = RectWin.bottom - 38;
		RectButton.right = RectWin.right;
		m_ScheduleList.MoveWindow(&RectButton);
		m_ScheduleList.SetColumnWidth(0,(RectButton.Width()/6)*4);
		m_ScheduleList.SetColumnWidth(1,RectButton.Width()/6);
		m_ScheduleList.SetColumnWidth(2,RectButton.Width()/6);
	}

	if (m_AddSchedule)
	{
		m_AddSchedule.GetClientRect(&RectButton);
		nButtonWidth = RectButton.Width();
		nButton = (RectWin.Width() - nButtonWidth*4)/8;

		RectButton.left = RectWin.left + nButton;
		RectButton.right = RectButton.left + nButtonWidth;
		RectButton.top = RectWin.bottom - 10 - RectButton.Height();
		RectButton.bottom = RectWin.bottom - 10;
		m_AddSchedule.MoveWindow(&RectButton);
	}

	if (m_ModifySchedule)
	{
		m_ModifySchedule.GetClientRect(&RectButton);
		nButtonWidth = RectButton.Width();
		nButton = (RectWin.Width() - nButtonWidth*4)/8;

		RectButton.left = RectWin.left + nButtonWidth + nButton * 3;
		RectButton.right = RectButton.left + nButtonWidth;
		RectButton.top = RectWin.bottom - 10 - RectButton.Height();
		RectButton.bottom = RectWin.bottom - 10;
		m_ModifySchedule.MoveWindow(&RectButton);
	}

	if (m_DeleteSchedule)
	{
		m_DeleteSchedule.GetClientRect(&RectButton);
		nButtonWidth = RectButton.Width();
		nButton = (RectWin.Width() - nButtonWidth*4)/8;

		RectButton.right = RectWin.right - nButtonWidth - nButton * 3;
		RectButton.left = RectButton.right - nButtonWidth;
		RectButton.top = RectWin.bottom - 10 - RectButton.Height();
		RectButton.bottom = RectWin.bottom - 10;
		m_DeleteSchedule.MoveWindow(&RectButton);
	}

	if (m_SetPassword)
	{
		m_SetPassword.GetClientRect(&RectButton);
		nButtonWidth = RectButton.Width();
		nButton = (RectWin.Width() - nButtonWidth*4)/8;

		RectButton.right = RectWin.right - nButton;
		RectButton.left = RectButton.right - nButtonWidth;
		RectButton.top = RectWin.bottom - 10 - RectButton.Height();
		RectButton.bottom = RectWin.bottom - 10;
		m_SetPassword.MoveWindow(&RectButton);
	}
	// TODO: Add your message handler code here
	
}

void CScheduleEdit::OnAddSchedule() 
{
	CSchedule ScheDlg(this);
	ScheDlg.m_bModify = FALSE;
	if (ScheDlg.DoModal() == IDOK)
	{
		ModifyListItem(m_ScheduleList.GetItemCount(),&ScheDlg.m_ScheduleInfo,TRUE);
		g_ScheduleItemArray.Add(ScheDlg.m_ScheduleInfo);
		RecordSetting(g_ScheduleItemArray);
	}
	// TODO: Add your control notification handler code here
}

void CScheduleEdit::OnModifySchedule() 
{
	CSchedule ScheDlg(this);
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

			for( nItemIndex=0; nItemIndex<g_ScheduleItemArray.GetSize(); nItemIndex++ )
			{
				if(!wcsicmp(g_ScheduleItemArray[nItemIndex].szPathFile,szSchedule))
				{
					ScheDlg.m_bModify = TRUE;
					ScheDlg.m_ScheduleInfo = g_ScheduleItemArray[nItemIndex];
					if (ScheDlg.DoModal() == IDOK)
					{
						ModifyListItem(i,&ScheDlg.m_ScheduleInfo,FALSE);
						g_ScheduleItemArray[nItemIndex] = ScheDlg.m_ScheduleInfo;
						RecordSetting(g_ScheduleItemArray);
					}
					break;
				}
			}
			break;
		}
	}
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

			for( nItemIndex=0; nItemIndex<g_ScheduleItemArray.GetSize(); nItemIndex++ )
			{
				if(!wcsicmp(g_ScheduleItemArray[nItemIndex].szPathFile,szSchedule))
				{
					g_ScheduleItemArray.RemoveAt(nItemIndex);
					if(!RecordSetting(g_ScheduleItemArray))
					{
						break;
					}

					m_ScheduleList.DeleteItem(i);
					break;
				}
			}
			break;
		}
	}	
}

void CScheduleEdit::ModifyListItem(int nItem,PSCHEDULE_ITEM pScheduleInfo,BOOL bInsert)
{
	CString strText;

	if (!bInsert) m_ScheduleList.DeleteItem(nItem);

	if (pScheduleInfo->ulFileattribute & FILE_ATTRIBUTE_DIRECTORY)
		nItem = m_ScheduleList.InsertItem(nItem,pScheduleInfo->szPathFile,IMG_NORMAL);
	else
		nItem = m_ScheduleList.InsertItem(nItem,pScheduleInfo->szPathFile,IMG_FILE);

	switch (pScheduleInfo->ulDateType)
	{
	case TIME_DAY_OF_WEEK:
		switch (pScheduleInfo->ulDate)
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
		default:
			strText.LoadString(IDS_UNKNOW);
			break;
		}
		break;
	case TIME_DAY_OF_MONTH:
		strText.Format(IDS_DAY_OF_MONTH,pScheduleInfo->ulDate);
		break;
	default:
		strText.LoadString(IDS_UNKNOW);
		break;
	}
	m_ScheduleList.SetItemText(nItem,1,strText);
	strText.Format(L"%2.2d:%2.2d",(pScheduleInfo->ExecuteTime&0xff0000)/0x10000,(pScheduleInfo->ExecuteTime&0xff00)/0x100);
	m_ScheduleList.SetItemText(nItem,2,strText);
}

void CScheduleEdit::OnOK() 
{
	return;	
}


void CScheduleEdit::OnSetPassword() 
{
	CPasswordDlg PasswordDlg;
	PasswordDlg.m_bSetPassword = TRUE;
	PasswordDlg.DoModal();
	// TODO: Add your control notification handler code here
}
