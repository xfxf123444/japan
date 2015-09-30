// AddPath.cpp : implementation file
//

#include "stdafx.h"
#include "ygfsmon.h"
#include "AddPath.h"
#include "Function.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddPath dialog


CAddPath::CAddPath(CWnd* pParent /*=NULL*/)
	: CDialog(CAddPath::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddPath)
	m_Path = _T("");
	m_strPassword = _T("");
	m_strConfirmPassword = _T("");
	m_CompressLevel = 1;
	m_bCheckPasswordAlways = FALSE;
	m_bErrorLimition = FALSE;
	m_nMaxInputNumber = 3;
	//}}AFX_DATA_INIT
}


void CAddPath::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddPath)
	DDX_Text(pDX, IDC_PATH, m_Path);
	DDX_Text(pDX, IDC_PASWORD, m_strPassword);
	DDX_Text(pDX, IDC_CONFIRM_PASSWORD, m_strConfirmPassword);
	DDX_Radio(pDX, IDC_COMPRESS_LEVEL, m_CompressLevel);
	DDX_Check(pDX, IDC_CHECK_PASSWORD_ALWAYS, m_bCheckPasswordAlways);
	DDX_Check(pDX, IDC_ERROR_LIMITION, m_bErrorLimition);
	DDX_Text(pDX, IDC_MAX_INPUT_NUMBER, m_nMaxInputNumber);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddPath, CDialog)
	//{{AFX_MSG_MAP(CAddPath)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_REFER, OnButtonRefer)
	ON_BN_CLICKED(IDC_ERROR_LIMITION, OnErrorLimition)
	ON_BN_CLICKED(IDC_CHECK_PASSWORD_ALWAYS, OnCheckPasswordAlways)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddPath message handlers


void CAddPath::OnButtonOk() 
{
	// TODO: Add your control notification handler code here
	CString strText,strTitle;
	ULONG ulAttr;
	// TODO: Add extra validation here
	UpdateData(TRUE);
	ulAttr = GetFileAttributes((LPCTSTR)m_Path);
	if (ulAttr == -1 || !(ulAttr & FILE_ATTRIBUTE_DIRECTORY))
	{
		strText.LoadString(IDS_INVALID_PATH);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
		return;
	}

	WCHAR szAddPath[MAX_PATH];
	memset(szAddPath,0,sizeof(szAddPath));
	wcsncpy(szAddPath,(LPCTSTR)(m_Path),MAX_PATH-1);

	BOOL bExcludeFolder;
	bExcludeFolder =  FALSE;
	if( FALSE == CheckExcludeFolder(szAddPath,bExcludeFolder) )
	{
		TRACE("\nCheckExcludeFolder error in OnAdd");
	}

	if(bExcludeFolder)
	{
		// TRACE("\nCan not add exclude folder %s as monitor folder",szAddPath);
		CString strNotAddSpecialFolder;
		strNotAddSpecialFolder.Format(IDS_NOT_ADD_SPECIAL_FOLDER,szAddPath);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(strNotAddSpecialFolder,strTitle,MB_OK | MB_ICONWARNING);
		return;
	}

	if (0 == m_strPassword.GetLength())
	{
		strText.LoadString(IDS_INVALID_PASSWORD);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
		return;
	}

	if (m_strPassword.GetLength() > PASSWORD_SIZE)
	{
		strText.LoadString(IDS_PASSWORD_TOO_LONG);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
		return;
	}

	if( 0 != m_strPassword.Compare(m_strConfirmPassword))
	{
		strText.LoadString(IDS_PASSWORD_NOT_MATCH);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
		return;
	}
	CDialog::OnOK();	
}

void CAddPath::OnButtonRefer() 
{
	// TODO: Add your control notification handler code here
	WCHAR szSourceDir[MAX_PATH];
	memset(szSourceDir,0,sizeof(szSourceDir));
	if(SelectFolder(m_hWnd,szSourceDir))
	{
		m_Path = szSourceDir;
		UpdateData(FALSE);
	}	
}

BOOL CAddPath::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	GetDlgItem(IDC_MAX_INPUT_NUMBER)->EnableWindow(m_bErrorLimition && m_bCheckPasswordAlways);
	GetDlgItem(IDC_ERROR_LIMITION)->EnableWindow(m_bCheckPasswordAlways);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAddPath::OnErrorLimition() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	GetDlgItem(IDC_MAX_INPUT_NUMBER)->EnableWindow(m_bErrorLimition && m_bCheckPasswordAlways);
}

void CAddPath::OnCheckPasswordAlways() 
{
	UpdateData(TRUE);
	GetDlgItem(IDC_ERROR_LIMITION)->EnableWindow(m_bCheckPasswordAlways);
	GetDlgItem(IDC_MAX_INPUT_NUMBER)->EnableWindow(m_bErrorLimition && m_bCheckPasswordAlways);
	
}
