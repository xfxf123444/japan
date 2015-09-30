// EasyPage1.cpp : implementation file
//

#include "stdafx.h"
#include "am01.h"
#include "EasyPage1.h"
#include "../AM01Struct/AM01Struct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern EASY_BACKUP_INFO g_EasyBackupInfo;

/////////////////////////////////////////////////////////////////////////////
// CEasyPage1 property page

IMPLEMENT_DYNCREATE(CEasyPage1, CPropertyPage)

CEasyPage1::CEasyPage1() : CPropertyPage(CEasyPage1::IDD)
{
	//{{AFX_DATA_INIT(CEasyPage1)
	m_nBackupContent = -1;
	//}}AFX_DATA_INIT
}

CEasyPage1::~CEasyPage1()
{
}

void CEasyPage1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEasyPage1)
	DDX_Radio(pDX, IDC_RADIO_MY_DOCUMENT, m_nBackupContent);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEasyPage1, CPropertyPage)
	//{{AFX_MSG_MAP(CEasyPage1)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEasyPage1 message handlers

BOOL CEasyPage1::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class

	CPropertySheet* pSheet = (CPropertySheet*)GetParent();
	ASSERT_KINDOF(CPropertySheet, pSheet);
	pSheet->SetWizardButtons(PSWIZB_NEXT);

	// (pSheet->GetDlgItem(ID_WIZBACK))->ShowWindow(FALSE);
	(pSheet->GetDlgItem(IDHELP))->ShowWindow(FALSE);

	return CPropertyPage::OnSetActive();
}

LRESULT CEasyPage1::OnWizardNext() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	// return CPropertyPage::OnWizardNext();

	CPropertySheet* pSheet = (CPropertySheet*)GetParent();
	ASSERT_KINDOF(CPropertySheet, pSheet);

	// invalid m_nBackupContent
	m_nBackupContent=-1;

	UpdateData(TRUE);
	
	if( 0 == m_nBackupContent )
	{
	   g_EasyBackupInfo.nBackupContent = m_nBackupContent;
	   return ( pSheet->SetActivePage(pSheet->GetPage(2)) );
	}
	else if (
		       (1 == m_nBackupContent) ||
		       (2 == m_nBackupContent)
			)
	{
	   g_EasyBackupInfo.nBackupContent = m_nBackupContent;
	   return CPropertyPage::OnWizardNext();
	}
	else
	{
		TRACE(L"\nYou do not choose a number.");
		return -1;
	}

	// return CPropertyPage::OnWizardNext();
}
