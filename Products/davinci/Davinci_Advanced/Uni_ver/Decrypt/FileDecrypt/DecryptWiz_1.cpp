 // DecryptWiz_1.cpp : implementation file
//

#include "stdafx.h"
#include "filedecrypt.h"
#include "DecryptWiz_1.h"
#include "..\..\DavinciFunction\DavinciFunction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DECRYPT_INFO g_DecryptInfo;
BOOL g_bDecryptSelectedFolderOnly;
int nMaxInputNumber;
BOOL bSecureDeleteSource = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CDecryptWiz_1 property page

IMPLEMENT_DYNCREATE(CDecryptWiz_1, CPropertyPage)

CDecryptWiz_1::CDecryptWiz_1() : CPropertyPage(CDecryptWiz_1::IDD)
{
	//{{AFX_DATA_INIT(CDecryptWiz_1)
	m_strImageFile = _T("");
	m_bSecureDeleteSource = FALSE;
	//}}AFX_DATA_INIT
}

CDecryptWiz_1::~CDecryptWiz_1()
{
}

void CDecryptWiz_1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDecryptWiz_1)
	DDX_Text(pDX, IDC_IMAGE_FILE, m_strImageFile);
	DDX_Check(pDX, IDC_SECURE_DELETE_SOURCE, m_bSecureDeleteSource);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDecryptWiz_1, CPropertyPage)
	//{{AFX_MSG_MAP(CDecryptWiz_1)
	ON_BN_CLICKED(IDC_REFER, OnRefer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDecryptWiz_1 message handlers

BOOL CDecryptWiz_1::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertySheet* pSheet = (CPropertySheet*)GetParent();
	ASSERT_KINDOF(CPropertySheet, pSheet);
	pSheet->SetWizardButtons(PSWIZB_NEXT);
	(pSheet->GetDlgItem(IDHELP))->ShowWindow(FALSE);
	
	return CPropertyPage::OnSetActive();
}

LRESULT CDecryptWiz_1::OnWizardNext() 
{
	CString strText,strTitle;
	// TODO: Add your specialized code here and/or call the base class
	UpdateData(TRUE);

	bSecureDeleteSource = m_bSecureDeleteSource;
	
	if( 0 == m_strImageFile.GetLength() )
	{
		strText.LoadString(IDS_EMPTY_IMAGE_FILE);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
		return -1;
	}

	// the file name should not be too long
	// it will be stored in a struct
	if( m_strImageFile.GetLength() >= MAX_PATH )
	{
		strText.LoadString(IDS_INVALID_IMAGE_FILE);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
		return -1;
	}

	if( -1 == GetFileAttributes( (LPCTSTR)m_strImageFile ) )
	{
		strText.LoadString(IDS_INVALID_IMAGE_FILE);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
		return -1;
	}

	wcsncpy(g_DecryptInfo.szImageFile,(LPCTSTR)m_strImageFile,MAX_PATH-1);

	return CPropertyPage::OnWizardNext();
}

void CDecryptWiz_1::OnRefer() 
{
	// TODO: Add your control notification handler code here
	WCHAR szImageFile[MAX_PATH];

	CString strImageDescription;
	strImageDescription.LoadString(IDS_IMAGE_DESCRIPTION);
	if( SelectFile(FILE_ENCRYPT_EXTENSION,(LPCTSTR)strImageDescription,szImageFile) )
	{
		m_strImageFile = szImageFile;
		UpdateData(FALSE);
	}	
}




