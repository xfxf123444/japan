// WZStep4.cpp : implementation file
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
#include "WZStep4.h"
#include "Function.h"
#include "SetPasswordDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern AM01_PROG_SETTING *g_pSetting;
/////////////////////////////////////////////////////////////////////////////
// CWZStep4 property page

IMPLEMENT_DYNCREATE(CWZStep4, CPropertyPageEx)

CWZStep4::CWZStep4() : CPropertyPageEx(CWZStep4::IDD, 0, IDS_BACKUP_DEST_TITLE, IDS_BACKUP_DEST_SUBTITLE)
{
	//{{AFX_DATA_INIT(CWZStep4)
	m_TargetPath = _T("");
	m_Comment = _T("");
	m_bLogFail = FALSE;
	m_dwMinutes = 10;
	m_bRetry = FALSE;
	m_bAutoSpawn = TRUE;
	//}}AFX_DATA_INIT
//	m_psp.dwFlags |= PSP_HIDEHEADER;
}

CWZStep4::~CWZStep4()
{
}

void CWZStep4::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWZStep4)
	DDX_Text(pDX, IDC_EDIT1, m_TargetPath);
	DDV_MaxChars(pDX, m_TargetPath, 260);
	DDX_Text(pDX, IDC_COMMENT, m_Comment);
	DDV_MaxChars(pDX, m_Comment, 50);
	DDX_Check(pDX, IDC_LOG_FAIL, m_bLogFail);
	DDX_Text(pDX, IDC_MINUTES, m_dwMinutes);
	DDX_Check(pDX, IDC_RETRY, m_bRetry);
	DDX_Check(pDX, IDC_AUTO_SPAWN, m_bAutoSpawn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWZStep4, CPropertyPageEx)
	//{{AFX_MSG_MAP(CWZStep4)
	ON_BN_CLICKED(IDC_REFER, OnRefer)
	ON_BN_CLICKED(IDC_ENCRYPT, OnEncrypt)
	ON_BN_CLICKED(IDC_RETRY, OnRetry)
	ON_BN_CLICKED(IDC_LOG_FAIL, OnLogFail)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWZStep4 message handlers

BOOL CWZStep4::OnSetActive()
{
	CPropertySheet* pSheet = (CPropertySheet*)GetParent();
	ASSERT_KINDOF(CPropertySheet, pSheet);
	pSheet->SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT | PSWIZB_FINISH);

	//-- needn't the help button begin
	(pSheet->GetDlgItem(IDHELP))->ShowWindow(FALSE);
	//-- needn't the help button end

	GetDlgItem(IDC_EDIT1)->EnableWindow(TRUE);
	GetDlgItem(IDC_REFER)->EnableWindow(TRUE);
	GetDlgItem(IDC_ENCRYPT)->ShowWindow(SW_SHOW);

	if(BACKUP_TYPE_DIFF == g_pSetting->DataInfo.wBackupType)
	{
		GetDlgItem(IDC_EDIT1)->EnableWindow(FALSE);
		GetDlgItem(IDC_REFER)->EnableWindow(FALSE);
		GetDlgItem(IDC_ENCRYPT)->ShowWindow(SW_HIDE);
		m_TargetPath = g_pSetting->DataInfo.szTargetImage;
		UpdateData(FALSE);
	}
	//UpdateData(FALSE);
	m_strPassword.Empty();

	return CPropertyPageEx::OnSetActive();
}

void CWZStep4::OnRefer() 
{
    WCHAR szSelectPath[MAX_PATH];

	UpdateData(TRUE);
	if (NewSelectFile(L"AMG",L"AM01 Image File",szSelectPath))
	{
		m_TargetPath = szSelectPath;
		UpdateData(FALSE);	
	}
	// TODO: Add your control notification handler code here
	
}

BOOL CWZStep4::OnInitDialog() 
{
	CPropertyPageEx::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CWZStep4::OnWizardFinish() 
{
	int nTotal,i;
	UpdateData(TRUE); 
	// TODO: Add your specialized code here and/or call the base class
	if (!IsValidAMGPath(m_TargetPath.GetBuffer(0)))
	{
		AfxMessageBox(IDS_INVALID_IMAGE,MB_OK | MB_ICONINFORMATION);
		return FALSE;
	}
	
	nTotal = g_pSetting->pcaSource.GetSize();
	for (i = 0;i < nTotal;i ++)
	{
		if (CheckConflict(g_pSetting->pcaSource.ElementAt(i).GetBuffer(0),
							m_TargetPath.GetBuffer(0),TRUE))
		{
			AfxMessageBox(IDS_CONFLICT,MB_OK | MB_ICONINFORMATION);
			return FALSE;
		}
	}

	wcscpy(g_pSetting->DataInfo.szTargetImage,(LPCTSTR)m_TargetPath);
	wcscpy(g_pSetting->DataInfo.szComment,(LPCTSTR)m_Comment);

	if(m_strPassword.GetLength()>0)
	{
		g_pSetting->DataInfo.BackupOption.bEncrypt = TRUE;
		wcsncpy(g_pSetting->DataInfo.BackupOption.pchPassword,(LPCTSTR)m_strPassword,PASSWORD_SIZE);
	}
	else
	{
		g_pSetting->DataInfo.BackupOption.bEncrypt = FALSE;
		memset(g_pSetting->DataInfo.BackupOption.pchPassword,0,PASSWORD_SIZE*sizeof(WCHAR));
	}

	if (m_bRetry && !m_dwMinutes)
	{
		AfxMessageBox(IDS_INVALID_TIME);
		return FALSE;
	}

	g_pSetting->DataInfo.BackupOption.bAutoSpawn = m_bAutoSpawn;
	g_pSetting->DataInfo.BackupOption.bLogFail = m_bLogFail;
	g_pSetting->DataInfo.BackupOption.bRetryFail = m_bRetry;
	g_pSetting->DataInfo.BackupOption.dwRetryMinutes = m_dwMinutes;

	return CPropertyPageEx::OnWizardFinish();
}

void CWZStep4::OnEncrypt() 
{
	// TODO: Add your control notification handler code here
	CSetPasswordDlg SetPasswordDlg;
	if (SetPasswordDlg.DoModal() == IDOK)
	{
		m_strPassword = SetPasswordDlg.m_strPassword;
	}
}

void CWZStep4::OnRetry() 
{
	UpdateData(TRUE);
	if (m_bRetry) m_bLogFail = TRUE;
	UpdateData(FALSE);
	// TODO: Add your control notification handler code here
	
}

void CWZStep4::OnLogFail() 
{
	UpdateData(TRUE);
	if (!m_bLogFail) m_bRetry = FALSE;
	UpdateData(FALSE);
	
}
