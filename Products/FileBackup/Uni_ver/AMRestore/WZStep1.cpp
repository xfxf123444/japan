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
#include "AMRestor.h"
#include "WZStep1.h"

#include "..\AM01Struct\AM01Struct.h"
#include "..\AM01Expt\AM01Expt.h"


// #define  9

// #define DATA_FILE_RESERVED_BYTE 80
// #define IMAGE_COMMENT_SIZE 80
// #define FILE_TYPE_OFFSET 192

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// extern CString g_ImageFileName;
// WORD g_wFileType;

extern AM_RESTORE_INFO g_RestoreInfo;


// IMAGE_HEAD g_ImageHead;

/////////////////////////////////////////////////////////////////////////////
// CWZStep1 property page

IMPLEMENT_DYNCREATE(CWZStep1, CPropertyPageEx)

CWZStep1::CWZStep1() : CPropertyPageEx(CWZStep1::IDD, 0, IDS_RESTORE_IMAGE_TITLE, IDS_RESTORE_IMAGE_SUBTITLE)
{
	//{{AFX_DATA_INIT(CWZStep1)
	m_SourceImage = _T("");
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
	DDX_Text(pDX, IDC_SOURCE_IMAGE, m_SourceImage);
	DDV_MaxChars(pDX, m_SourceImage, 260);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWZStep1, CPropertyPageEx)
	//{{AFX_MSG_MAP(CWZStep1)
	ON_BN_CLICKED(IDC_REFER, OnRefer)
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
	DWORD dwFileAttr;
	UpdateData(TRUE);
	// TODO: Add your specialized code here and/or call the base class
	dwFileAttr = GetFileAttributes(m_SourceImage);
	if (dwFileAttr == -1 || (dwFileAttr & FILE_ATTRIBUTE_DIRECTORY))
	{
		AfxMessageBox(IDS_INVALID_SOURCE,MB_OK | MB_ICONINFORMATION);
		return -1;
	}

	CString ImageIdentity;
	CString ImageFilePath;
	ImageFilePath=m_SourceImage;

	WCHAR szLastImage[MAX_PATH];
	ZeroMemory(szLastImage, MAX_PATH * sizeof(WCHAR));
    wcsncpy(szLastImage , (LPCTSTR)ImageFilePath , MAX_PATH-1 );

	DWORD dwErrorCode;

	if ( CheckLastImage(szLastImage,&dwErrorCode)  == FALSE )
	{
		if(dwErrorCode == NOT_LAST_IMAGE)
		{
			if (SearchLastImage(szLastImage, &dwErrorCode) == FALSE)
			{
				AfxMessageBox(IDS_NOT_LAST_IMAGE,MB_OK|MB_ICONINFORMATION);
				return -1;
			}
			else
			{
				// copy the correct image path
				ImageFilePath = szLastImage;
				m_SourceImage = szLastImage;
			}
		}
	}

	IMAGE_HEAD ImageHead;

	if( FALSE == GetImageHead((LPCTSTR)ImageFilePath,&ImageHead) )
	{
		TRACE(L"\nGetImageHead error in OnWizardNext of WZStep1.");
		AfxMessageBox(IDS_OPEN_IMAGE_FAIL,MB_OK|MB_ICONINFORMATION,NULL);
		return -1;
	}

	if( ImageHead.bEncrypt )
	{
		if(FALSE == CheckImagePassword((LPCTSTR)ImageFilePath))
		{
			return -1;
		}
	}

	g_RestoreInfo.wImageType=ImageHead.wBackupType;

	// get szImageComment
	memset(g_RestoreInfo.szImageComment,0,MAX_PATH * sizeof(WCHAR));	
	wcsncpy(g_RestoreInfo.szImageComment ,ImageHead.szComment,MAX_PATH);

	UpdateData(TRUE);
	m_SourceImage = szLastImage;
	wcsncpy( g_RestoreInfo.szImageFile,(LPCTSTR)m_SourceImage,MAX_PATH-1);
	wcsncpy(g_RestoreInfo.szLastImageFile , (LPCTSTR)m_SourceImage, MAX_PATH-1 );
	return CPropertyPage::OnWizardNext();
}

void CWZStep1::OnRefer() 
{
	// TODO: Add your control notification handler code here
	WCHAR szFile[MAX_PATH];
	if (SelectFile(L"AMG",L"AM01 Image File",szFile))
	{
		m_SourceImage = szFile;
		UpdateData(FALSE);
	}
}


