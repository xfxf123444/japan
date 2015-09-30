// WZStep2.cpp : implementation file
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
#include "WZStep2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern AM01_PROG_SETTING *g_pSetting;
/////////////////////////////////////////////////////////////////////////////
// CWZStep2 property page

IMPLEMENT_DYNCREATE(CWZStep2, CPropertyPageEx)

CWZStep2::CWZStep2() : CPropertyPageEx(CWZStep2::IDD, 0, IDS_BACKUP_TYPE_TITLE, IDS_BACKUP_TYPE_SUBTITLE)
{
	//{{AFX_DATA_INIT(CWZStep2)
	m_nBackupType = 0;
	//}}AFX_DATA_INIT
//	m_psp.dwFlags |= PSP_HIDEHEADER;
}

CWZStep2::~CWZStep2()
{
}

void CWZStep2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWZStep2)
	DDX_Radio(pDX, IDC_RADIO1, m_nBackupType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWZStep2, CPropertyPageEx)
	//{{AFX_MSG_MAP(CWZStep2)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWZStep2 message handlers

BOOL CWZStep2::OnSetActive()
{
	CPropertySheet* pSheet = (CPropertySheet*)GetParent();
	ASSERT_KINDOF(CPropertySheet, pSheet);
	pSheet->SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT);

	//-- needn't the help button begin
	(pSheet->GetDlgItem(IDHELP))->ShowWindow(FALSE);
	//-- needn't the help button end

	return CPropertyPageEx::OnSetActive();
}

LRESULT CWZStep2::OnWizardNext() 
{
	UpdateData(TRUE);
	// TODO: Add your specialized code here and/or call the base class
	if( 0 == m_nBackupType)
	{
		g_pSetting->DataInfo.wBackupType = FILE_BACKUP;
	}
	else if( 1 == m_nBackupType)
	{
         g_pSetting->DataInfo.wBackupType = MAIL_BACKUP;
	}
	else if( 2 == m_nBackupType)
	{
	     g_pSetting->DataInfo.wBackupType = BACKUP_TYPE_DIFF;
	}
	
	return CPropertyPageEx::OnWizardNext();
}
