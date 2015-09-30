// DecryptWiz_2.cpp : implementation file
//

#include "stdafx.h"
#include "filedecrypt.h"
#include "DecryptWiz_2.h"
#include "..\..\EncryptToolStruct\EncryptToolStruct.h"
#include "..\DecryptFunction\DecryptFunction.h"
#include "..\..\DavinciFunction\DavinciFunction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern DECRYPT_INFO g_DecryptInfo;

/////////////////////////////////////////////////////////////////////////////
// CDecryptWiz_2 property page

IMPLEMENT_DYNCREATE(CDecryptWiz_2, CPropertyPage)

CDecryptWiz_2::CDecryptWiz_2() : CPropertyPage(CDecryptWiz_2::IDD)
{
	//{{AFX_DATA_INIT(CDecryptWiz_2)
	m_strPassword = _T("");
	//}}AFX_DATA_INIT
}

CDecryptWiz_2::~CDecryptWiz_2()
{
}

void CDecryptWiz_2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDecryptWiz_2)
	DDX_Text(pDX, IDC_PASSWORD, m_strPassword);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDecryptWiz_2, CPropertyPage)
	//{{AFX_MSG_MAP(CDecryptWiz_2)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDecryptWiz_2 message handlers

BOOL CDecryptWiz_2::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertySheet* pSheet = (CPropertySheet*)GetParent();
	ASSERT_KINDOF(CPropertySheet, pSheet);
	pSheet->SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT);
	(pSheet->GetDlgItem(IDHELP))->ShowWindow(FALSE);

	return CPropertyPage::OnSetActive();
}

LRESULT CDecryptWiz_2::OnWizardNext() 
{
	// TODO: Add your specialized code here and/or call the base class
	CString strText,strTitle;
	UpdateData(TRUE);

	if( 0 == m_strPassword.GetLength() )
	{
		strText.LoadString(IDS_EMPTY_PASSWORD);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
		return -1;
	}

	if( m_strPassword.GetLength() > PASSWORD_SIZE-1 )
	{
		strText.LoadString(IDS_PASSWORD_TOO_LONG);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
		return -1;
	}

	wcsncpy(g_DecryptInfo.szPassword,(LPCTSTR)m_strPassword,PASSWORD_SIZE-1);

	DWORD dwImageFileCount;

	CString strOneFileName;

	if( FALSE == GetImageInfo(&g_DecryptInfo,dwImageFileCount,strOneFileName) )
	{
//		strText.LoadString(IDS_GET_IMAGEINFO_FAIL);
//		strTitle.LoadString(IDS_APP_NAME);
//		MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
		return -1;
	}

	return CPropertyPage::OnWizardNext();
}
