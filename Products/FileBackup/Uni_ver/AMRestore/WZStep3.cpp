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
#include "AMRestor.h"
#include "WZStep3.h"

#include "..\AM01Struct\AM01Struct.h"
#include "..\AM01Expt\AM01Expt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// extern int g_nPathType;
// extern int g_nRestoreType;
// extern WORD g_wFileType;
// CString g_TargetPath;

extern AM_RESTORE_INFO g_RestoreInfo;

/////////////////////////////////////////////////////////////////////////////
// Interior2 property page

IMPLEMENT_DYNCREATE(CWZStep3, CPropertyPageEx)

CWZStep3::CWZStep3()
	: CPropertyPageEx(CWZStep3::IDD, 0, IDS_RESTORE_TARGET_TITLE,IDS_RESTORE_TARGET_SUBTITLE)
{
	//{{AFX_DATA_INIT(CWZStep3)
	m_PathType = 0;
	m_TargetPath = _T("");
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
	DDX_Radio(pDX, IDC_RADIO1, m_PathType);
	DDX_Text(pDX, IDC_TARGET_PATH, m_TargetPath);
	DDV_MaxChars(pDX, m_TargetPath, 260);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWZStep3, CPropertyPageEx)
	//{{AFX_MSG_MAP(CWZStep3)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_REFER, OnRefer)
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

	// if(g_nRestoreType==0)
	// {
	     // GetDlgItem(IDC_RADIO2)->EnableWindow(FALSE);
	// }
	// else if(g_nRestoreType==1)
	// {
	     // GetDlgItem(IDC_RADIO2)->EnableWindow(TRUE);
	// }

	// MAIL_BACKUP is 1
	if( 
		(MAIL_BACKUP == g_RestoreInfo.wImageType) ||
		(OE_MAIL_BACKUP == g_RestoreInfo.wImageType) ||
		(WIN_MAIL_BACKUP == g_RestoreInfo.wImageType)
	  )

	{
	   // can only restore to orginal place
       m_PathType=0;
	   m_TargetPath.Empty();
	   UpdateData(FALSE);
       GetDlgItem(IDC_RADIO2)->EnableWindow(FALSE);
	   GetDlgItem(IDC_TARGET_PATH)->EnableWindow(FALSE);
	   GetDlgItem(IDC_REFER)->ShowWindow(SW_HIDE);
	}
	else
	{
      GetDlgItem(IDC_RADIO2)->EnableWindow(TRUE);
	}

	return CPropertyPageEx::OnSetActive();
}

BOOL CWZStep3::OnInitDialog() 
{
	CPropertyPageEx::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CWZStep3::OnRadio1() 
{
	UpdateData(TRUE);
	InitControl();
	// TODO: Add your control notification handler code here
	
}

void CWZStep3::OnRadio2() 
{
	UpdateData(TRUE);
	InitControl();
	// TODO: Add your control notification handler code here
	
}

void CWZStep3::InitControl()
{
	switch (m_PathType)
	{
	case 0:
		GetDlgItem(IDC_TARGET_PATH)->EnableWindow(FALSE);
		GetDlgItem(IDC_REFER)->ShowWindow(SW_HIDE);
		m_TargetPath = m_DefaultPath;
		UpdateData(FALSE);
		break;
	case 1:
		GetDlgItem(IDC_TARGET_PATH)->EnableWindow(TRUE);
		GetDlgItem(IDC_REFER)->ShowWindow(SW_SHOW);
		break;
	}
}

void CWZStep3::OnRefer() 
{
	WCHAR  szPath[MAX_PATH];
	if (SelectFolder(m_hWnd,szPath))
	{
		m_TargetPath = szPath;
		UpdateData(FALSE);
	}
	// TODO: Add your control notification handler code here
	
}



LRESULT CWZStep3::OnWizardNext() 
{
	// TODO: Add your specialized code here and/or call the base class

	UpdateData(TRUE);

	// g_nPathType=m_PathType;
	// g_TargetPath=m_TargetPath;

	g_RestoreInfo.nTargetType=m_PathType;
	
	if(g_RestoreInfo.nTargetType == RESTORE_NEW_PLACE)
	{
	    wcsncpy(g_RestoreInfo.szTargetDir,(LPCTSTR)m_TargetPath,MAX_PATH-1);
	}

	if(
		(g_RestoreInfo.nTargetType==RESTORE_NEW_PLACE) &&
	    ( bPathExist(g_RestoreInfo.szTargetDir)==FALSE )
	  )
	{
		CString strMessage;
		// strMessage.LoadString(IDS_INVALID_TARGET_PATH);
		// MessageBox(strMessage);
		AfxMessageBox(IDS_INVALID_TARGET_DIR,MB_OK|MB_ICONINFORMATION);
		return -1;
	}

	return CPropertyPageEx::OnWizardNext();
}



