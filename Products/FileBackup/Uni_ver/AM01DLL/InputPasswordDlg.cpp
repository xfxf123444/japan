// InputPasswordDlg.cpp : implementation file
//

#include "stdafx.h"
#include "am01dll.h"
#include "InputPasswordDlg.h"
#include "AMFunction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputPasswordDlg dialog


CInputPasswordDlg::CInputPasswordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInputPasswordDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInputPasswordDlg)
	m_strPassword = _T("");
	//}}AFX_DATA_INIT
}


void CInputPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputPasswordDlg)
	DDX_Text(pDX, IDC_IMAGE_PASSWORD, m_strPassword);
	DDV_MaxChars(pDX, m_strPassword, YG_ENCRYPT_KEY_SIZE);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputPasswordDlg, CDialog)
	//{{AFX_MSG_MAP(CInputPasswordDlg)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnButtonCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputPasswordDlg message handlers

void CInputPasswordDlg::OnButtonOk() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	WCHAR pchPassword[PASSWORD_SIZE];
	memset(pchPassword,0,sizeof(pchPassword));
	wcsncpy(pchPassword,(LPCTSTR)m_strPassword,PASSWORD_SIZE-1);

	EncryptPassword((WCHAR*)pchPassword,PASSWORD_SIZE);

	if( 0 == memcmp(pchPassword,m_pchPassword,sizeof(pchPassword)) )
	{
	   m_bPasswordCorrect = TRUE;
       OnOK();
	}
	else
	{
	  m_bPasswordCorrect = FALSE;
	  AfxMessageBox(IDS_INVALID_PASSWORD,MB_OK|MB_ICONINFORMATION,NULL);
	}		
}

void CInputPasswordDlg::OnButtonCancel() 
{
	// TODO: Add your control notification handler code here
    OnCancel();
}

BOOL CInputPasswordDlg::TestPasswordResult()
{
	return m_bPasswordCorrect;
}

void CInputPasswordDlg::SetPasswordValue(WCHAR *pchPassword)
{
	memcpy(m_pchPassword,pchPassword,sizeof(m_pchPassword));
}
