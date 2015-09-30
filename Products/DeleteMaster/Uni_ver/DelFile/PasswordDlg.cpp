// PasswordDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Delfile.h"
#include "PasswordDlg.h"
#include "Fun.h"


/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg dialog


CPasswordDlg::CPasswordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPasswordDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPasswordDlg)
	m_Password = _T("");
	m_PasswordConfirm = _T("");
	m_OldPassword = _T("");
	m_EnterPass = _T("");
	//}}AFX_DATA_INIT
}


void CPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPasswordDlg)
	DDX_Text(pDX, IDC_PASSWORD, m_Password);
	DDX_Text(pDX, IDC_PASSWORD_CONFIRM, m_PasswordConfirm);
	DDX_Text(pDX, IDC_OLD_PASSWORD, m_OldPassword);
	DDX_Text(pDX, IDC_ENTER_PASS, m_EnterPass);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPasswordDlg, CDialog)
	//{{AFX_MSG_MAP(CPasswordDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg message handlers

BOOL CPasswordDlg::OnInitDialog() 
{
	CString	strText;
	CDialog::OnInitDialog();
	if (m_bSetPassword)
	{
		GetDlgItem(IDC_OLD_PASSWORD)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_OLD_PASS)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_PASSWORD_CONFIRM)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CONFIRM_PASS)->ShowWindow(SW_SHOW);
		m_EnterPass.LoadString(IDS_NEW_PASSWORD);
	}
	else
	{
		GetDlgItem(IDC_OLD_PASSWORD)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_OLD_PASS)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_PASSWORD_CONFIRM)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CONFIRM_PASS)->ShowWindow(SW_HIDE);
		m_EnterPass.LoadString(IDS_ENTER_PASSWORD);
	}
	UpdateData(FALSE);
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPasswordDlg::OnOK() 
{
	UpdateData(TRUE);
	// TODO: Add extra validation here
	if (m_bSetPassword)
	{
		char temp[MAX_PATH];
		ZeroMemory(temp, MAX_PATH);
		WideCharToMultiByte(CP_ACP,0,m_OldPassword,m_OldPassword.GetLength(), temp, MAX_PATH, NULL, NULL);
		if (!CheckPassword("Administrator",temp))
		{
			AfxMessageBox(IDS_INVALID_PASSWORD);
			return;
		}
		if (m_Password != m_PasswordConfirm)
		{
			AfxMessageBox(IDS_PASSWORD_MATCH);
			return;
		}
		ZeroMemory(temp, MAX_PATH);
		WideCharToMultiByte(CP_ACP,0,m_Password,m_Password.GetLength(), temp, MAX_PATH, NULL, NULL);
		SavePassword("Administrator",temp,TRUE);
	}
	else
	{
		char temp[MAX_PATH];
		ZeroMemory(temp, MAX_PATH);
		WideCharToMultiByte(CP_ACP,0,m_Password,m_Password.GetLength(), temp, MAX_PATH, NULL, NULL);
		if (!CheckPassword("Administrator",temp))
		{
			AfxMessageBox(IDS_INVALID_PASSWORD);
			return;
		}
	}
	CDialog::OnOK();
}
