// WZStep3.cpp : implementation file
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "AM01.h"
#include "WZStep3.h"
#include "Function.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern AM01_PROG_SETTING *g_pSetting;

/////////////////////////////////////////////////////////////////////////////
// Interior2 property page

IMPLEMENT_DYNCREATE(CWZStep3, CPropertyPageEx)

CWZStep3::CWZStep3()
	: CPropertyPageEx(CWZStep3::IDD, 0, IDS_BACKUP_SOURCE_TITLE, IDS_BACKUP_SOURCE_SUBTITLE)
{
	//{{AFX_DATA_INIT(CWZStep3)
	m_SourcePath = _T("");
	m_ImagePath = _T("");
	m_nMailType = 0;
	// m_ImageType = 0;
	//}}AFX_DATA_INIT
//	m_psp.dwFlags |= PSP_HIDEHEADER;
}

CWZStep3::~CWZStep3()
{
}

void CWZStep3::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWZStep3)
	DDX_Control(pDX, IDC_LIST1, m_SourceList);
	DDX_Text(pDX, IDC_EDIT_DIRECTORY, m_SourcePath);
	DDV_MaxChars(pDX, m_SourcePath, 260);
	DDX_Text(pDX, IDC_IMAGE_PATH, m_ImagePath);
	DDV_MaxChars(pDX, m_ImagePath, 260);
	DDX_Radio(pDX, IDC_RADIO1, m_nMailType);
	// DDX_Radio(pDX, IDC_RADIO4, m_ImageType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWZStep3, CPropertyPageEx)
	//{{AFX_MSG_MAP(CWZStep3)
	ON_BN_CLICKED(IDC_REFER, OnRefer)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REFER_IMAGE, OnReferImage)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWZStep3 message handlers

BOOL CWZStep3::OnSetActive()
{
	CPropertySheet* pSheet = (CPropertySheet*)GetParent();
	ASSERT_KINDOF(CPropertySheet, pSheet);
	pSheet->SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT);

	//-- needn't the help button begin
	(pSheet->GetDlgItem(IDHELP))->ShowWindow(FALSE);
	//-- needn't the help button end

	InitControl();
	return CPropertyPageEx::OnSetActive();
}


void CWZStep3::InitControl()
{
	CString strText;
	WCHAR szMailPath[MAX_PATH];
	WCHAR szAddressPath[MAX_PATH];

	switch(g_pSetting->DataInfo.wBackupType)
	{
	case FILE_BACKUP:
		GetDlgItem(IDC_TXT_MAIL_CLIENT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_DIRECTORY)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LIST1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_REFER)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_ADD)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_REMOVE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_TXT_INDIVALDIR)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_TXT_SOURCE_DIR)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_IMAGE_PATH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_REFER_IMAGE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TXT_IMAGE)->ShowWindow(SW_HIDE);
		break;
	case OE_MAIL_BACKUP:
	case WIN_MAIL_BACKUP:
		// here should fall through
		// the user may user wizard to go forward and back
		// so we just consider the case of OE_MAIL_BACKUP
	case MAIL_BACKUP:
		GetDlgItem(IDC_TXT_MAIL_CLIENT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RADIO1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RADIO2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RADIO3)->ShowWindow(SW_SHOW);
		if (GetOsVersion())
			strText.LoadString(IDS_WINDOWS_MAIL);
		else
			strText.LoadString(IDS_OUTLOOK_EXPRESS);
		GetDlgItem(IDC_RADIO1)->SetWindowText(strText);
		GetDlgItem(IDC_RADIO1)->EnableWindow(NewGetOutlookExpressDataPath(szMailPath,szAddressPath));
		GetDlgItem(IDC_RADIO2)->EnableWindow(GetOutlookDataPath(szMailPath));
		GetDlgItem(IDC_RADIO3)->EnableWindow(GetBecky2DataPath(szMailPath));
		GetDlgItem(IDC_EDIT_DIRECTORY)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_REFER)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ADD)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_REMOVE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TXT_INDIVALDIR)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TXT_SOURCE_DIR)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_IMAGE_PATH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_REFER_IMAGE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TXT_IMAGE)->ShowWindow(SW_HIDE);
		break;
	case BACKUP_TYPE_DIFF:
		GetDlgItem(IDC_TXT_MAIL_CLIENT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_DIRECTORY)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_REFER)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ADD)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_REMOVE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TXT_INDIVALDIR)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TXT_SOURCE_DIR)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_IMAGE_PATH)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_REFER_IMAGE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_TXT_IMAGE)->ShowWindow(SW_SHOW);
		break;
	default:
		break;
	}

}

void CWZStep3::OnRefer() 
{
    WCHAR szSelectPath[MAX_PATH];

	UpdateData(TRUE);
	if (SelectFolder(m_hWnd,szSelectPath))
	{
		m_SourcePath = szSelectPath;
		UpdateData(FALSE);	
	}
}

void CWZStep3::OnAdd() 
{
	int nTotal,i;
	WCHAR szPath[MAX_PATH];
	CString strPath;
	UpdateData(TRUE);

	if (GetLongPathName(m_SourcePath.GetBuffer(0),szPath,MAX_PATH))
	{
		m_SourcePath = szPath;
		nTotal = m_SourceList.GetItemCount();
		for (i = 0;i < nTotal;i++)
		{
			strPath = m_SourceList.GetItemText(i,0);
			if (CheckConflict(strPath.GetBuffer(0),szPath,FALSE)) return;
			if (CheckConflict(szPath,strPath.GetBuffer(0),FALSE))
			{
				m_SourceList.DeleteItem(i);
				nTotal --;
				i --;
			}
		}
		m_SourceList.InsertItem(nTotal,m_SourcePath);
		m_SourceList.SetItem(nTotal,0,LVIF_STATE,L"",0,INDEXTOSTATEIMAGEMASK(1),LVIS_STATEIMAGEMASK,0);
	}
	else AfxMessageBox(IDS_INVALID_SOURCE,MB_OK | MB_ICONINFORMATION);
	// TODO: Add your control notification handler code here
	
}

void CWZStep3::OnReferImage() 
{
	WCHAR szFile[MAX_PATH];
	UpdateData(TRUE);


	// now we do not support differential backup from RM3 file.
	if (SelectFile(L"AMG",L"AM01 Image",szFile))
	{
		m_ImagePath = szFile;
	}
	
	UpdateData(FALSE);
	// TODO: Add your control notification handler code here
	
}

void CWZStep3::OnRemove() 
{
	int nTotal,i;
	
	nTotal = m_SourceList.GetItemCount();
	for (i = 0;i < nTotal;i ++)
	{
		if (m_SourceList.GetItemState(i,LVIS_SELECTED))
		{
			m_SourceList.DeleteItem(i);
			break;
		}
	}
	// TODO: Add your control notification handler code here
	
}

BOOL CWZStep3::OnInitDialog() 
{
	CPropertyPageEx::OnInitDialog();
	
	CString		csLine;
	CString		csHeading;
	LV_COLUMN	Column;
	CRect		ListRect;

	m_SourceList.GetClientRect(ListRect);

	Column.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
	Column.fmt = LVCFMT_LEFT;
	Column.cx  = ListRect.right-ListRect.left;
	csHeading.LoadString(IDS_SOURCE);
	Column.pszText = csHeading.GetBuffer(0);
	Column.cchTextMax = 0;		// ignored
	Column.iSubItem = 0;		
	m_SourceList.InsertColumn(0,&Column);

	m_pImageList = new CImageList;
	m_pImageList->Create(IDB_STATEICONS,16,0,RGB(255,255,255));
	m_SourceList.SetImageList(m_pImageList,LVSIL_STATE);

	// to avoid one control is disabled while it is selected.
	m_nMailType=-1;
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CWZStep3::OnWizardNext() 
{
	int  nTotal,i;
	WCHAR szMailPath[MAX_PATH];
	WCHAR szAddressPath[MAX_PATH];

	UpdateData(TRUE);

	if(MAIL_BACKUP == g_pSetting->DataInfo.wBackupType )
	{
		if( -1 == m_nMailType)
		{
			AfxMessageBox(IDS_SELECT_MAIL_CLIENT,MB_OK|MB_ICONINFORMATION,NULL);
			return -1;
		}

		if( 0 == m_nMailType ) // 1 is the second radio box
		{
			if (GetOsVersion())
				(*g_pSetting).DataInfo.wBackupType = WIN_MAIL_BACKUP;
			else
				(*g_pSetting).DataInfo.wBackupType = OE_MAIL_BACKUP;
		}
	}

	// TODO: Add your specialized code here and/or call the base class
	if (g_pSetting->pcaSource.GetSize()) g_pSetting->pcaSource.RemoveAll();
	switch(g_pSetting->DataInfo.wBackupType)
	{
	case FILE_BACKUP:
		nTotal = m_SourceList.GetItemCount();
		if (nTotal <= 0)
		{
			AfxMessageBox(IDS_SOURCE_EMPTY,MB_OK | MB_ICONINFORMATION);
			return -1;
		}
		for (i = 0; i < nTotal;i ++)
		{
			CString temp = m_SourceList.GetItemText(i,0);
			g_pSetting->pcaSource.Add(temp);
		}
			
		break;
	case OE_MAIL_BACKUP:
	case WIN_MAIL_BACKUP:
		// here should fall through
	case MAIL_BACKUP:
		switch (m_nMailType)
		{
		case 0:
			// if (GetOutlookExpressDataPath(szMailPath))
			if (NewGetOutlookExpressDataPath(szMailPath,szAddressPath))
			{
				// the sequence is important
				// in the restore program we will use the first element
				// as the mail data path and check it exist or not
				// if not exist we will ask the user
				// if he want to restore the file to current mail address
				if (GetOsVersion())
				{
//					wcscat(szMailPath,L"\\Local Folders");
					wcscat(szAddressPath,L"\\Contacts");
				}
				g_pSetting->pcaSource.Add(szMailPath);
				g_pSetting->pcaSource.Add(szAddressPath);
			}
			else
			{
				AfxMessageBox(IDS_SOURCE_EMPTY,MB_OK | MB_ICONINFORMATION);
				return -1;
			}
			break;
		case 1:
			if (GetOutlookDataPath(szMailPath))
				g_pSetting->pcaSource.Add(szMailPath);
			else
			{
				AfxMessageBox(IDS_SOURCE_EMPTY,MB_OK | MB_ICONINFORMATION);
				return -1;
			}
			break;
		case 2:
			if (GetBecky2DataPath(szMailPath))
				g_pSetting->pcaSource.Add(szMailPath);
			else
			{
				AfxMessageBox(IDS_SOURCE_EMPTY,MB_OK | MB_ICONINFORMATION);
				return -1;
			}
			break;
		default:
			break;
		}
		break;
	case BACKUP_TYPE_DIFF:

		if ((GetFileAttributes(m_ImagePath.GetBuffer(0)) == -1) ||
			!IsValidAMGPath(m_ImagePath.GetBuffer(0)))
		{
			AfxMessageBox(IDS_INVALID_IMAGE,MB_OK | MB_ICONINFORMATION);
			return -1;
		}

		g_pSetting->DataInfo.wBackupType = BACKUP_TYPE_DIFF;
		wcscpy(g_pSetting->DataInfo.szTargetImage,(LPCTSTR)m_ImagePath);

		break;

	}
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPageEx::OnWizardNext();
}

void CWZStep3::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	delete m_pImageList;
	m_pImageList = NULL;
	CPropertyPageEx::OnClose();
}



void CWZStep3::OnDestroy() 
{
	if( NULL != m_pImageList)
	{
	   delete m_pImageList;
	   m_pImageList = NULL;
	}
	CPropertyPageEx::OnDestroy();
	
}
