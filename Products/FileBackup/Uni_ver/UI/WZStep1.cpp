// WZStep1.cpp : implementation file
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
#include "WZStep1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern AM01_PROG_SETTING *g_pSetting;
/////////////////////////////////////////////////////////////////////////////
// CWZStep1 property page

IMPLEMENT_DYNCREATE(CWZStep1, CPropertyPageEx)

CWZStep1::CWZStep1() : CPropertyPageEx(CWZStep1::IDD, 0, IDS_SETTING_NAME_TITLE, IDS_SETTING_NAME_SUBTITLE)
{
	//{{AFX_DATA_INIT(CWZStep1)
	m_szSettingName = _T("");
	//}}AFX_DATA_INIT
//	m_psp.dwFlags |= PSP_HIDEHEADER;
}

CWZStep1::~CWZStep1()
{
}

void CWZStep1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWZStep1)
	DDX_Text(pDX, IDC_SETTING_NAME, m_szSettingName);
	DDV_MaxChars(pDX, m_szSettingName, 40);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWZStep1, CPropertyPageEx)
	//{{AFX_MSG_MAP(CWZStep1)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWZStep1 message handlers

BOOL CWZStep1::OnSetActive()
{
	CPropertySheet* pSheet = (CPropertySheet*)GetParent();
	ASSERT_KINDOF(CPropertySheet, pSheet);
	
	pSheet->SetWizardButtons(PSWIZB_NEXT);

	//-- needn't the help button begin
	(pSheet->GetDlgItem(IDHELP))->ShowWindow(FALSE);
	//-- needn't the help button end

	return CPropertyPageEx::OnSetActive();
}

LRESULT CWZStep1::OnWizardNext() 
{
	UpdateData(TRUE);
	if (m_szSettingName == L"")
	{
		AfxMessageBox(IDS_SETTING_NAME_EMPTY,MB_OK | MB_ICONINFORMATION);
		return -1;
	}
	// TODO: Add your specialized code here and/or call the base class
	wcscpy(g_pSetting->DataInfo.szSettingName,(LPCTSTR)m_szSettingName);
	return CPropertyPageEx::OnWizardNext();
}
