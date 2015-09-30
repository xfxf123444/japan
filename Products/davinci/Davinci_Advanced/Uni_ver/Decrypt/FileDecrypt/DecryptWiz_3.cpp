// DecryptWiz_3.cpp : implementation file
//

#include "stdafx.h"
#include "filedecrypt.h"
#include "DecryptWiz_3.h"
#include "..\DecryptFunction\DecryptFunction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern DECRYPT_INFO g_DecryptInfo;

/////////////////////////////////////////////////////////////////////////////
// CDecryptWiz_3 property page

IMPLEMENT_DYNCREATE(CDecryptWiz_3, CPropertyPage)

CDecryptWiz_3::CDecryptWiz_3() : CPropertyPage(CDecryptWiz_3::IDD)
{
	//{{AFX_DATA_INIT(CDecryptWiz_3)
	m_nRestoreType = 0;
	//}}AFX_DATA_INIT
}

CDecryptWiz_3::~CDecryptWiz_3()
{
}

void CDecryptWiz_3::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDecryptWiz_3)
	DDX_Radio(pDX, IDC_ENTIRE_RESTORE, m_nRestoreType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDecryptWiz_3, CPropertyPage)
	//{{AFX_MSG_MAP(CDecryptWiz_3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDecryptWiz_3 message handlers

BOOL CDecryptWiz_3::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertySheet* pSheet = (CPropertySheet*)GetParent();
	ASSERT_KINDOF(CPropertySheet, pSheet);
	pSheet->SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT);
	(pSheet->GetDlgItem(IDHELP))->ShowWindow(FALSE);

	return CPropertyPage::OnSetActive();
}


LRESULT CDecryptWiz_3::OnWizardNext() 
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateData(TRUE);
	// g_nRestoreType = m_RestoreType;

	if( ( 0 == m_nRestoreType ) ||
		( 1 == m_nRestoreType )
	  )
	{
	    g_DecryptInfo.nRestoreType=m_nRestoreType;
	    return CPropertyPage::OnWizardNext();
	}
	else
	{
		return -1;
	}
}
