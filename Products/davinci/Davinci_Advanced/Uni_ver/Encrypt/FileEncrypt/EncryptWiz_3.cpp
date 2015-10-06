// EncryptWiz_3.cpp : implementation file
//

#include "stdafx.h"
#include "fileencrypt.h"
#include "EncryptWiz_3.h"
#include "..\..\EncryptToolStruct\EncryptToolStruct.h"
#include "..\EncryptFunction\EncryptInfo.h"
#include "..\EncryptFunction\EncryptFunction.h"
#include "EncryptStatus.h"
#include "..\..\DavinciFunction\DeleteMethodDlg.h"
#include "..\..\DavinciFunction\DeleteProgressDlg.h"
#include "EncryptOption.h"

#define UNLIMITED 128
extern BOOL g_bEncryptSucceed;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CEncryptInfo g_EncryptInfo;
extern BOOL g_bCreateSelfExtractFile;

/////////////////////////////////////////////////////////////////////////////
// CEncryptWiz_3 property page

IMPLEMENT_DYNCREATE(CEncryptWiz_3, CPropertyPage)

CEncryptWiz_3::CEncryptWiz_3() : CPropertyPage(CEncryptWiz_3::IDD)
{
	//{{AFX_DATA_INIT(CEncryptWiz_3)
	m_strPassword = _T("");
	m_strConfirmPassword = _T("");
	m_bDeleteSource = FALSE;
	m_CompressLevel = 1;
	//}}AFX_DATA_INIT
}

CEncryptWiz_3::~CEncryptWiz_3()
{
}

void CEncryptWiz_3::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEncryptWiz_3)
	DDX_Text(pDX, IDC_PASSWORD, m_strPassword);
	DDX_Text(pDX, IDC_REENTER_PASSWORD, m_strConfirmPassword);
	DDX_Check(pDX, IDC_DELETE_SOURCE, m_bDeleteSource);
	DDX_Radio(pDX, IDC_COMPRESS_LEVEL, m_CompressLevel);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_OPTION, m_btnOption);
}


BEGIN_MESSAGE_MAP(CEncryptWiz_3, CPropertyPage)
	//{{AFX_MSG_MAP(CEncryptWiz_3)
	ON_BN_CLICKED(IDC_OPTION, OnOption)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEncryptWiz_3 message handlers

BOOL CEncryptWiz_3::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertySheet* pSheet = (CPropertySheet*)GetParent();
	ASSERT_KINDOF(CPropertySheet, pSheet);
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);

	(pSheet->GetDlgItem(IDCANCEL))->GetWindowText(m_strTextCancel);
	
	CString strEncrypt;
	strEncrypt.LoadString(IDS_ENCRYPT);
	(pSheet->GetDlgItem(ID_WIZFINISH))->SetWindowText(strEncrypt);

	CString strClose;
	strClose.LoadString(IDS_CLOSE);
    
	(pSheet->GetDlgItem(IDCANCEL))->SetWindowText(strClose);

	(pSheet->GetDlgItem(IDHELP))->ShowWindow(FALSE);

	if (g_bCreateSelfExtractFile) {
		m_btnOption.EnableWindow(FALSE);
	}
	return CPropertyPage::OnSetActive();
}

LRESULT CEncryptWiz_3::OnWizardBack() 
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertySheet* pSheet = (CPropertySheet*)GetParent();
	ASSERT_KINDOF(CPropertySheet, pSheet);

	(pSheet->GetDlgItem(IDCANCEL))->SetWindowText(m_strTextCancel);
	
	return CPropertyPage::OnWizardBack();
}

BOOL CEncryptWiz_3::OnWizardFinish() 
{
	// TODO: Add your specialized code here and/or call the base class
	CString strText,strTitle;
	UpdateData(TRUE);

	if( m_strPassword != m_strConfirmPassword )
	{
		strText.LoadString(IDS_PASSWORD_NOT_MATCH);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
		return FALSE;
	}

	if( 0 == m_strPassword.GetLength() )
	{
		strText.LoadString(IDS_EMPTY_PASSWORD);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
		return FALSE;
	}

	if( m_strPassword.GetLength() > PASSWORD_SIZE)
	{
		strText.LoadString(IDS_PASSWORD_TOO_LONG);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
		return FALSE;
	}
	// do not close the wizard so that user may modify something
	// and do encryption again

	g_EncryptInfo.m_strPassword = m_strPassword;
	g_EncryptInfo.m_dwCompressLevel = m_CompressLevel;

	CEncryptStatus EncryptStatusDlg;
	EncryptStatusDlg.DoModal();

	if(	g_bEncryptSucceed ) 
	{
		if(m_bDeleteSource)
		{
			CDeleteMethodDlg DeleteMethodDlg;
			DeleteMethodDlg.m_SourceArray.RemoveAll();
			DeleteMethodDlg.m_SourceArray.Append(g_EncryptInfo.m_strSourceArray);
			DeleteMethodDlg.DoModal();
		}
		strText.LoadString(IDS_ENCRYPT_FINISHED);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(strText,strTitle,MB_OK | MB_ICONINFORMATION);
	}
	else
	{
		strText.LoadString(IDS_ENCRYPT_FAILED);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
	}

	// return FALSE;

	return CPropertyPage::OnWizardFinish();
}

void CEncryptWiz_3::OnOption() 
{
	CEncryptOption  OptionDialog;
	OptionDialog.m_bOptionOn = (BOOL)g_EncryptInfo.m_EncryptOption.bOptionOn;
	OptionDialog.m_bCheckTime = (BOOL)g_EncryptInfo.m_EncryptOption.bLimitTime;
	OptionDialog.m_bCheckCount = (BOOL)g_EncryptInfo.m_EncryptOption.bLimitCount;
	OptionDialog.m_dwLimitCount = g_EncryptInfo.m_EncryptOption.ulMaxCount;
	OptionDialog.m_LimitTime = g_EncryptInfo.m_EncryptOption.LimitTime;
	OptionDialog.m_nMaxInputNumber = g_EncryptInfo.m_EncryptOption.ulErrorLimit;
	OptionDialog.m_bErrorLimition = g_EncryptInfo.m_EncryptOption.bErrorLimit;

	if (IDOK == OptionDialog.DoModal())
	{
		g_EncryptInfo.m_EncryptOption.bOptionOn = OptionDialog.m_bOptionOn;
		g_EncryptInfo.m_EncryptOption.bLimitTime = OptionDialog.m_bCheckTime;
		g_EncryptInfo.m_EncryptOption.bLimitCount = OptionDialog.m_bCheckCount;
		g_EncryptInfo.m_EncryptOption.ulMaxCount = OptionDialog.m_dwLimitCount;
		g_EncryptInfo.m_EncryptOption.LimitTime = OptionDialog.m_LimitTime;
		g_EncryptInfo.m_EncryptOption.ulErrorLimit = OptionDialog.m_nMaxInputNumber;
		g_EncryptInfo.m_EncryptOption.bErrorLimit = OptionDialog.m_bErrorLimition;
	}
	// TODO: Add your control notification handler code here
	
}
