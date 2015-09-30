// EasyPage2.cpp : implementation file
//

#include "stdafx.h"
#include "am01.h"
#include "EasyPage2.h"
#include "../AM01Struct/AM01Struct.h"
#include "Function.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern EASY_BACKUP_INFO g_EasyBackupInfo;

/////////////////////////////////////////////////////////////////////////////
// CEasyPage2 property page

IMPLEMENT_DYNCREATE(CEasyPage2, CPropertyPage)

CEasyPage2::CEasyPage2() : CPropertyPage(CEasyPage2::IDD)
{
	//{{AFX_DATA_INIT(CEasyPage2)
	m_nMailType = -1;
	//}}AFX_DATA_INIT
}

CEasyPage2::~CEasyPage2()
{
}

void CEasyPage2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEasyPage2)
	DDX_Radio(pDX, IDC_RADIO_OUTLOOK_EXPRESS, m_nMailType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEasyPage2, CPropertyPage)
	//{{AFX_MSG_MAP(CEasyPage2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEasyPage2 message handlers

BOOL CEasyPage2::OnSetActive() 
{
	CString strText;
	// TODO: Add your specialized code here and/or call the base class

	CPropertySheet* pSheet = (CPropertySheet*)GetParent();
	ASSERT_KINDOF(CPropertySheet, pSheet);
	pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);

	//-- needn't the help button begin
	// (pSheet->GetDlgItem(ID_WIZBACK))->ShowWindow(TRUE);
	(pSheet->GetDlgItem(IDHELP))->ShowWindow(FALSE);
	//-- needn't the help button end

	WCHAR szMailPath[MAX_PATH];
	WCHAR szAddressPath[MAX_PATH];

	GetDlgItem(IDC_RADIO_OUTLOOK_EXPRESS)->EnableWindow(NewGetOutlookExpressDataPath(szMailPath,szAddressPath));
	if (GetOsVersion())
		strText.LoadString(IDS_WINDOWS_MAIL);
	else
		strText.LoadString(IDS_OUTLOOK_EXPRESS);
	GetDlgItem(IDC_RADIO_OUTLOOK_EXPRESS)->SetWindowText(strText);
	GetDlgItem(IDC_RADIO_OUTLOOK)->EnableWindow(GetOutlookDataPath(szMailPath));
	GetDlgItem(IDC_RADIO_THE_BECKY)->EnableWindow(GetBecky2DataPath(szMailPath));

	return CPropertyPage::OnSetActive();
}

LRESULT CEasyPage2::OnWizardNext() 
{
	// TODO: Add your specialized code here and/or call the base class

	CPropertySheet* pSheet = (CPropertySheet*)GetParent();
	ASSERT_KINDOF(CPropertySheet, pSheet);

	// invalid m_nBackupContent
	m_nMailType = -1;

	UpdateData(TRUE);

    // OUTLOOK_EXPRESS_MAIL is equal to 0;
    // OUTLOOK_MAIL is equal to 1 ;
    // BECKY_MAIL is equal to 2;
	
	if(
		( OUTLOOK_EXPRESS_MAIL == m_nMailType ) ||
	    ( OUTLOOK_MAIL == m_nMailType ) ||
		( BECKY_MAIL == m_nMailType )
	  )
	{
	   g_EasyBackupInfo.nMailType = m_nMailType;
	   return CPropertyPage::OnWizardNext();
	}
	else
	{
		TRACE(L"\nYou do not choose a number.");
		return -1;
	}

	// return CPropertyPage::OnWizardNext();
}

BOOL CEasyPage2::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	// to avoid one control is disable while it is selected.

	m_nMailType = -1;
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
