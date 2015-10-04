// EncryptWiz_2.cpp : implementation file
//

#include "stdafx.h"
#include "fileencrypt.h"
#include "EncryptWiz_2.h"
#include "..\..\EncryptToolStruct\EncryptToolStruct.h"
#include "..\..\DavinciFunction\DavinciFunction.h"
#include "..\EncryptFunction\EncryptFunction.h"
#include "..\EncryptFunction\EncryptInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEncryptWiz_2 property page

extern CEncryptInfo g_EncryptInfo;

IMPLEMENT_DYNCREATE(CEncryptWiz_2, CPropertyPage)

CEncryptWiz_2::CEncryptWiz_2() : CPropertyPage(CEncryptWiz_2::IDD)
{
	//{{AFX_DATA_INIT(CEncryptWiz_2)
	m_strTarget = _T("");
	//}}AFX_DATA_INIT
}

CEncryptWiz_2::~CEncryptWiz_2()
{
}

void CEncryptWiz_2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEncryptWiz_2)
	DDX_Text(pDX, IDC_TARGET, m_strTarget);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEncryptWiz_2, CPropertyPage)
	//{{AFX_MSG_MAP(CEncryptWiz_2)
	ON_BN_CLICKED(IDC_REFER, OnRefer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEncryptWiz_2 message handlers

BOOL CEncryptWiz_2::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertySheet* pSheet = (CPropertySheet*)GetParent();
	ASSERT_KINDOF(CPropertySheet, pSheet);
	pSheet->SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT);
	
	return CPropertyPage::OnSetActive();
}

void CEncryptWiz_2::OnRefer() 
{
	// TODO: Add your control notification handler code here
	WCHAR szTargetFile[MAX_PATH];

	CString strTargetDescription;
	strTargetDescription.LoadString(IDS_IMAGE_DESCRIPTION);
	if( SelectFilePro(FILE_ENCRYPT_EXTENSION,(LPCTSTR)strTargetDescription,szTargetFile) )
	{
		m_strTarget = szTargetFile;
		UpdateData(FALSE);
	}	
}

LRESULT CEncryptWiz_2::OnWizardNext() 
{
	// TODO: Add your specialized code here and/or call the base class
	CString strText,strTitle;
	UpdateData(TRUE);
	if( 0 == m_strTarget.GetLength() )
	{
		strText.LoadString(IDS_EMPTY_TARGET);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
		return -1;
	}

	CString strFileEncryptSuffix;
	strFileEncryptSuffix.Empty();
	strFileEncryptSuffix = strFileEncryptSuffix + L"." + FILE_ENCRYPT_EXTENSION;

	if(
		( 0 != m_strTarget.Right(4).CompareNoCase(strFileEncryptSuffix) ) ||
		( m_strTarget.GetLength() <= (int)wcslen(L"C:\\.chy") ) || // if the file is in network, "\\a\b.chy will be longer".
		( 
		   ( 0 !=m_strTarget.Mid(1,1).Compare(L":") ) &&  // local disk
		   ( 0 !=m_strTarget.Mid(1,1).Compare(L"\\") )    // net work
		)
	  )

	{
		strText.LoadString(IDS_INVALID_TARGET);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
		return -1;
	}

	// check placement
	// the target can not be placed in the directory of the source
	int nSourceIndex;
	int nSourceCount;
	nSourceCount = g_EncryptInfo.m_strSourceArray.GetSize();

	for(nSourceIndex=0; nSourceIndex<nSourceCount; nSourceIndex++ )
	{
		if( IsOneDirectory( (LPCTSTR)(g_EncryptInfo.m_strSourceArray[nSourceIndex]) ) )
		{
			if( FALSE == CheckPlacement((LPCTSTR)(g_EncryptInfo.m_strSourceArray[nSourceIndex]),(LPCTSTR)m_strTarget) )
			{
				strText.LoadString(IDS_CONFLICT);
				strTitle.LoadString(IDS_APP_NAME);
				MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
				return -1;
			}
		}
	}

	g_EncryptInfo.m_strTarget = m_strTarget;

	return CPropertyPage::OnWizardNext();
}