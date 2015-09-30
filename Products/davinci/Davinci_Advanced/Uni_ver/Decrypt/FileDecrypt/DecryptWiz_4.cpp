// DecryptWiz_4.cpp : implementation file
//

#include "stdafx.h"
#include "filedecrypt.h"
#include "DecryptWiz_4.h"
#include "..\..\EncryptToolStruct\EncryptToolStruct.h"
#include "..\..\DavinciFunction\DavinciFunction.h"

extern DECRYPT_INFO g_DecryptInfo;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDecryptWiz_4 property page

IMPLEMENT_DYNCREATE(CDecryptWiz_4, CPropertyPage)

CDecryptWiz_4::CDecryptWiz_4() : CPropertyPage(CDecryptWiz_4::IDD)
{
	//{{AFX_DATA_INIT(CDecryptWiz_4)
	m_strTargetPath = _T("");
	m_nTargetType = 0;
	//}}AFX_DATA_INIT
}

CDecryptWiz_4::~CDecryptWiz_4()
{
}

void CDecryptWiz_4::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDecryptWiz_4)
	DDX_Text(pDX, IDC_TARGET_PATH, m_strTargetPath);
	DDX_Radio(pDX, IDC_ORIGINAL_PLACE, m_nTargetType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDecryptWiz_4, CPropertyPage)
	//{{AFX_MSG_MAP(CDecryptWiz_4)
	ON_BN_CLICKED(IDC_ORIGINAL_PLACE, OnOriginalPlace)
	ON_BN_CLICKED(IDC_RELATIVE_PLACE, OnRelativePlace)
	ON_BN_CLICKED(IDC_CURRENT_PLACE, OnCurrentPlace)
	ON_BN_CLICKED(IDC_NEW_PLACE, OnNewPlace)
	ON_BN_CLICKED(IDC_REFER, OnRefer)
//	ON_BN_CLICKED(IDC_RELATIVE_PLACE, OnRelativePlace)
//	ON_BN_CLICKED(IDC_CURRENT_PLACE, OnCurrentPlace)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDecryptWiz_4 message handlers

BOOL CDecryptWiz_4::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertySheet* pSheet = (CPropertySheet*)GetParent();
	ASSERT_KINDOF(CPropertySheet, pSheet);
	pSheet->SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT);
	(pSheet->GetDlgItem(IDHELP))->ShowWindow(FALSE);

	m_strTargetPath = g_DecryptInfo.szTargetPath;
	
	m_strStorePath  = g_DecryptInfo.szTargetPath;

	InitControl();

	return CPropertyPage::OnSetActive();
}




BOOL CDecryptWiz_4::OnInitDialog() 
{ 
	CPropertyPage::OnInitDialog();

	// TODO: Add extra initialization here
	m_nTargetType = 0;
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDecryptWiz_4::InitControl()
{
	switch (m_nTargetType)
	{
	case 0:
	case 1:
	case 2:
		GetDlgItem(IDC_TARGET_PATH)->EnableWindow(FALSE);
		GetDlgItem(IDC_REFER)->ShowWindow(SW_HIDE);
		
		m_strStorePath = m_strTargetPath;
		m_strTargetPath.Empty();
		
		UpdateData(FALSE);
		break;
	case 3:
		GetDlgItem(IDC_TARGET_PATH)->EnableWindow(TRUE);
		GetDlgItem(IDC_REFER)->ShowWindow(SW_SHOW);
		m_strTargetPath = m_strStorePath;
		UpdateData(FALSE);
		break;
	}
}

void CDecryptWiz_4::OnOriginalPlace() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	InitControl();	
}

void CDecryptWiz_4::OnNewPlace() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	InitControl();	
}

void CDecryptWiz_4::OnRefer() 
{
	// TODO: Add your control notification handler code here
	WCHAR  szPath[MAX_PATH];
	if (SelectFolder(m_hWnd,szPath))
	{
		m_strTargetPath = szPath;
		UpdateData(FALSE);
	}	
}

LRESULT CDecryptWiz_4::OnWizardNext() 
{
	CString strText,strTitle;
	// TODO: Add your specialized code here and/or call the base class
	DWORD dwFileAttribute;
	WCHAR *p;

	UpdateData(TRUE);

	switch (m_nTargetType)
	{
	case 0:
	    g_DecryptInfo.nTargetType = DECRYPT_ORIGINAL_PLACE;
		break;
	case 1:
	    g_DecryptInfo.nTargetType = DECRYPT_RELATIVE_PLACE;
		wcsncpy(g_DecryptInfo.szTargetPath,(LPCTSTR)&g_DecryptInfo.szImageFile,MAX_PATH-1);
		p = wcschr(g_DecryptInfo.szTargetPath,L'\\');
		if (p)	*p = L'\0';
		else 
		{
			strText.LoadString(IDS_EMPTY_TARGET_PATH);
			strTitle.LoadString(IDS_APP_NAME);
			MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
		}

		break;
	case 2:
		g_DecryptInfo.nTargetType = DECRYPT_CURRENT_PLACE;
		wcsncpy(g_DecryptInfo.szTargetPath,(LPCTSTR)&g_DecryptInfo.szImageFile,MAX_PATH-1);
		p = wcschr(g_DecryptInfo.szTargetPath,L'\\');
		if (p)	*p = L'\0';
		else 
		{
			strText.LoadString(IDS_EMPTY_TARGET_PATH);
			strTitle.LoadString(IDS_APP_NAME);
			MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
		}
		break;
	case 3:
		g_DecryptInfo.nTargetType = DECRYPT_NEW_PLACE;
		if( 0 == m_strTargetPath.GetLength() )
		{
			strText.LoadString(IDS_EMPTY_TARGET_PATH);
			strTitle.LoadString(IDS_APP_NAME);
			MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
			return -1;
		}

		if(
			( m_strTargetPath.GetLength() < (int)wcslen(L"C:\\") ) ||
			(
			( 0 != (m_strTargetPath.Mid(1,1).Compare(L":") )  )&&
			( 0 != (m_strTargetPath.Mid(1,1).Compare(L"\\") ) )
			)
			)
		{
			strText.LoadString(IDS_INVALID_TARGET_PATH);
			strTitle.LoadString(IDS_APP_NAME);
			MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
			return -1;
		}
		
		dwFileAttribute = GetFileAttributes((LPCTSTR)m_strTargetPath);
		
		if(
			( -1 != dwFileAttribute ) &&
			( 0 != (dwFileAttribute & FILE_ATTRIBUTE_DIRECTORY ) )
			)
		{
			wcsncpy(g_DecryptInfo.szTargetPath,(LPCTSTR)m_strTargetPath,MAX_PATH-1);
		}
		else
		{
			strText.LoadString(IDS_INVALID_TARGET_PATH);
			strTitle.LoadString(IDS_APP_NAME);
			MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
			return -1;
		}
		break;
	default:
		return -1;
	}
	
	return CPropertyPage::OnWizardNext();
}


void CDecryptWiz_4::OnRelativePlace() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	InitControl();	
}

void CDecryptWiz_4::OnCurrentPlace() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	InitControl();	
}
