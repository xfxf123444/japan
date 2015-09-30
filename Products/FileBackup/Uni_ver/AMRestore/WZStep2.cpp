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
#include "AMRestor.h"
#include "WZStep2.h"

#include "..\AM01Struct\AM01Struct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// extern g_nRestoreType;
// extern WORD g_wFileType;
// extern CString g_ImageFileName;

extern AM_RESTORE_INFO g_RestoreInfo;

/////////////////////////////////////////////////////////////////////////////
// CWZStep2 property page

IMPLEMENT_DYNCREATE(CWZStep2, CPropertyPageEx)

CWZStep2::CWZStep2() : CPropertyPageEx(CWZStep2::IDD, 0, IDS_RESTORE_TYPE_TITLE, IDS_RESTORE_TYPE_SUBTITLE)
{
	//{{AFX_DATA_INIT(CWZStep2)
	m_Comment = _T("");
	m_SourceImage = _T("");
	m_RestoreType = 0;
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
	DDX_Text(pDX, IDC_COMMENT, m_Comment);
	DDV_MaxChars(pDX, m_Comment, 100);
	DDX_Text(pDX, IDC_SOURCE_IMAGE, m_SourceImage);
	DDV_MaxChars(pDX, m_SourceImage, 260);
	DDX_Radio(pDX, IDC_RADIO1, m_RestoreType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWZStep2, CPropertyPageEx)
	//{{AFX_MSG_MAP(CWZStep2)
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

	// this is what I added.
	// m_SourceImage=g_ImageFileName;
	m_SourceImage=g_RestoreInfo.szImageFile;

	m_Comment = g_RestoreInfo.szImageComment;
	
	UpdateData(FALSE);

    //GetDlgItem(IDC_COMMENT)->SetWindowText("Comment");

	// GetDlgItem(IDC_SOURCE_IMAGE)->SetWindowText(g_ImageFileName);
    
	//-- this is what I added.
	//-- GetDlgItem(IDC_RADIO2)->EnableWindow(FALSE);

	// MAIL_BACKUP is 1
    // Outlook Express Mail is a bit different, so we
	// give it a special type.

	if(
		(MAIL_BACKUP == g_RestoreInfo.wImageType) ||
		(OE_MAIL_BACKUP == g_RestoreInfo.wImageType ) ||
		(WIN_MAIL_BACKUP == g_RestoreInfo.wImageType )
	  )
	{
		// first make RestoreType to total restore
		// then disable the part restore choice
		m_RestoreType=0;
		UpdateData(FALSE);
		GetDlgItem(IDC_RADIO2)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_RADIO2)->EnableWindow(TRUE);
	}

	return CPropertyPageEx::OnSetActive();
}

LRESULT CWZStep2::OnWizardNext() 
{
	UpdateData(TRUE);
	// g_nRestoreType = m_RestoreType;
	g_RestoreInfo.nRestoreType=m_RestoreType;
	// TODO: Add your specialized code here and/or call the base class
	return CPropertyPageEx::OnWizardNext();
}


