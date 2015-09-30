// SetPasswordDlg.cpp : implementation file
//

#include "stdafx.h"
#include "am01.h"
#include "SetPasswordDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetPasswordDlg dialog


CSetPasswordDlg::CSetPasswordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetPasswordDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetPasswordDlg)
	m_strPassword = _T("");
	m_strConfirmPassword = _T("");
	//}}AFX_DATA_INIT
}


void CSetPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetPasswordDlg)
	DDX_Text(pDX, IDC_PASSWORD, m_strPassword);
	DDV_MaxChars(pDX, m_strPassword, YG_ENCRYPT_KEY_SIZE);
	DDX_Text(pDX, IDC_CONFIRM_PASSWORD, m_strConfirmPassword);
	DDV_MaxChars(pDX, m_strConfirmPassword, YG_ENCRYPT_KEY_SIZE);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetPasswordDlg, CDialog)
	//{{AFX_MSG_MAP(CSetPasswordDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSetPasswordDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	UpdateData(TRUE);

	if (m_strPassword != m_strConfirmPassword)
	{
		AfxMessageBox(IDS_PASSWORD_NOT_MATCH,MB_OK|MB_ICONINFORMATION,NULL);
		ClearPassword();
		GetDlgItem(IDC_PASSWORD)->SetFocus();
		return;
	}
	else if (m_strPassword.GetLength() > YG_ENCRYPT_KEY_SIZE)
	{
		AfxMessageBox(IDS_MAX_PASSWORD,MB_OK|MB_ICONINFORMATION,NULL);
		ClearPassword();
		GetDlgItem(IDC_PASSWORD)->SetFocus();
		return;
	}

	CDialog::OnOK();
}

void CSetPasswordDlg::ClearPassword()
{
	m_strPassword.Empty();
	m_strConfirmPassword.Empty();
	UpdateData(FALSE);
}