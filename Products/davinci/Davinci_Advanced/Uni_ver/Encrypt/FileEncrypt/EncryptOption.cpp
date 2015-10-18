// EncryptOption.cpp : implementation file
//

#include "stdafx.h"
#include "fileencrypt.h"
#include "EncryptOption.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEncryptOption dialog

extern BOOL g_bCreateSelfExtractFile;

CEncryptOption::CEncryptOption(CWnd* pParent /*=NULL*/)
	: CDialog(CEncryptOption::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEncryptOption)
	m_bCheckCount = FALSE;
	m_bCheckTime = FALSE;
	m_LimitTime = 0;
	m_dwLimitCount = 1;
	m_bOptionOn = FALSE;
	m_bErrorLimition = FALSE;
	m_nMaxInputNumber = 3;
	//}}AFX_DATA_INIT
}


void CEncryptOption::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEncryptOption)
	DDX_Control(pDX, IDC_LIMIT_COUNT, m_LimitCount);
	DDX_Control(pDX, IDC_SPIN_COUNT, m_SpinCount);
	DDX_Check(pDX, IDC_CHECK_COUNT, m_bCheckCount);
	DDX_Check(pDX, IDC_CHECK_TIME, m_bCheckTime);
	DDX_DateTimeCtrl(pDX, IDC_LIMIT_TIME, m_LimitTime);
	DDX_Text(pDX, IDC_LIMIT_COUNT, m_dwLimitCount);
	DDV_MinMaxDWord(pDX, m_dwLimitCount, 1, 10000);
	DDX_Check(pDX, IDC_CHECK_OPTION, m_bOptionOn);
	DDX_Check(pDX, IDC_ERROR_LIMITION, m_bErrorLimition);
	DDX_Text(pDX, IDC_MAX_INPUT_NUMBER, m_nMaxInputNumber);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEncryptOption, CDialog)
	//{{AFX_MSG_MAP(CEncryptOption)
	ON_BN_CLICKED(IDC_CHECK_OPTION, OnCheckOption)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEncryptOption message handlers

BOOL CEncryptOption::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_SpinCount.SetRange(1,10000);
	m_SpinCount.SetBuddy((CWnd*)&m_LimitCount);
	m_SpinCount.SetPos(m_dwLimitCount);
	UpdateData(FALSE);
	InitCtrlData();
	// TODO: Add extra initialization here
	
	if (g_bCreateSelfExtractFile) {
		GetDlgItem(IDC_CHECK_COUNT)->ShowWindow(FALSE);
		GetDlgItem(IDC_LIMIT_COUNT)->ShowWindow(FALSE);
		GetDlgItem(IDC_SPIN_COUNT)->ShowWindow(FALSE);
		GetDlgItem(IDC_ERROR_LIMITION)->ShowWindow(FALSE);
		GetDlgItem(IDC_MAX_INPUT_NUMBER)->ShowWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEncryptOption::InitCtrlData()
{
	if (g_bCreateSelfExtractFile){
		GetDlgItem(IDC_CHECK_TIME)->EnableWindow(m_bOptionOn);
		GetDlgItem(IDC_LIMIT_TIME)->EnableWindow(m_bOptionOn);
	} 
	else{
		GetDlgItem(IDC_CHECK_TIME)->EnableWindow(m_bOptionOn);
		GetDlgItem(IDC_LIMIT_TIME)->EnableWindow(m_bOptionOn);
		GetDlgItem(IDC_CHECK_COUNT)->EnableWindow(m_bOptionOn);
		GetDlgItem(IDC_LIMIT_COUNT)->EnableWindow(m_bOptionOn);
		GetDlgItem(IDC_SPIN_COUNT)->EnableWindow(m_bOptionOn);
		GetDlgItem(IDC_ERROR_LIMITION)->EnableWindow(m_bOptionOn);
		GetDlgItem(IDC_MAX_INPUT_NUMBER)->EnableWindow(m_bOptionOn);
	}
}

void CEncryptOption::OnCheckOption() 
{
	UpdateData(TRUE);
	InitCtrlData();
	// TODO: Add your control notification handler code here
	
}
