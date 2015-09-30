// ShellEncryptDlg.cpp : implementation file
//

#include "stdafx.h"
#include "fileencrypt.h"
#include "ShellEncryptDlg.h"
#include "EncryptStatus.h"
#include "..\..\EncryptToolStruct\EncryptToolStruct.h"
#include "..\EncryptFunction\EncryptInfo.h"
#include "..\EncryptFunction\EncryptFunction.h"
#include "..\..\DavinciFunction\DeleteMethodDlg.h"
#include "..\..\DavinciFunction\DeleteProgressDlg.h"
#include "EncryptOption.h"

#define UNLIMITED 128;

CEncryptInfo g_EncryptInfo;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShellEncryptDlg dialog


CShellEncryptDlg::CShellEncryptDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShellEncryptDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShellEncryptDlg)
	m_strPassword = _T("");
	m_strConfirmPassword = _T("");
	m_bDeleteSource = FALSE;
	m_CompressLevel = 1;
	//}}AFX_DATA_INIT
}


void CShellEncryptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShellEncryptDlg)
	DDX_Text(pDX, IDC_PASSWORD, m_strPassword);
	DDX_Text(pDX, IDC_REENTER_PASSWORD, m_strConfirmPassword);
	DDX_Check(pDX, IDC_DELETE_SOURCE, m_bDeleteSource);
	DDX_Radio(pDX, IDC_COMPRESS_LEVEL, m_CompressLevel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShellEncryptDlg, CDialog)
	//{{AFX_MSG_MAP(CShellEncryptDlg)
	ON_BN_CLICKED(IDC_ENCRYPT, OnEncrypt)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnButtonCancel)
	ON_BN_CLICKED(IDC_OPTION, OnOption)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShellEncryptDlg message handlers

void CShellEncryptDlg::OnEncrypt() 
{
	// TODO: Add your control notification handler code here
	CString strText,strTitle;
	UpdateData(TRUE);

	if( m_strPassword != m_strConfirmPassword )
	{
		strText.LoadString(IDS_PASSWORD_NOT_MATCH);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
		return;
	}

	if( 0 == m_strPassword.GetLength() )
	{
		strText.LoadString(IDS_EMPTY_PASSWORD);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
		return;
	}

	if( m_strPassword.GetLength() > PASSWORD_SIZE)
	{
		strText.LoadString(IDS_PASSWORD_TOO_LONG);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
		return;
	}

	// get the target file name begin

	CString strTargetFile;
	DWORD dwFileAttribute;

	if( FALSE == GetTargetFileName((LPCTSTR)(m_strSource),strTargetFile) )
	{
		TRACE(L"\nGetTargetFileName error in OnEncrypt.");
	}

	dwFileAttribute = GetFileAttributes( (LPCTSTR)strTargetFile );

	if( 
		(-1 != dwFileAttribute ) &&
		( 0 != (dwFileAttribute & FILE_ATTRIBUTE_DIRECTORY ) )
	  )
	{
		CString strOpenTargetFailed;
		strOpenTargetFailed.Format(IDS_OPEN_TARGET_FAILED,(LPCTSTR)strTargetFile);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(strOpenTargetFailed,strTitle,MB_OK | MB_ICONWARNING);
		return;
	}

	// get the target file name end

	g_EncryptInfo.m_strSourceArray.RemoveAll();
	g_EncryptInfo.m_strSourceArray.Add(m_strSource);
    g_EncryptInfo.m_strPassword = m_strPassword;
	g_EncryptInfo.m_dwCompressLevel = m_CompressLevel;
	g_EncryptInfo.m_strTarget = strTargetFile;

	ShowWindow(SW_HIDE);
	CEncryptStatus EncryptStatusDlg;
	EncryptStatusDlg.DoModal();

	if(m_bDeleteSource)
	{
		CDeleteMethodDlg DeleteMethodDlg;
		DeleteMethodDlg.m_SourceArray.RemoveAll();
		DeleteMethodDlg.m_SourceArray.Append(g_EncryptInfo.m_strSourceArray);
		DeleteMethodDlg.DoModal();
	}

	EndDialog(TRUE);
}

void CShellEncryptDlg::OnOK()
{
	// overwrite the CDialog::OnOK();
}

void CShellEncryptDlg::OnButtonCancel() 
{
	// TODO: Add your control notification handler code here
	EndDialog(TRUE);
}

void CShellEncryptDlg::OnOption() 
{
	CEncryptOption  OptionDialog;
	OptionDialog.m_bOptionOn = (BOOL)g_EncryptInfo.m_EncryptOption.bOptionOn;
	OptionDialog.m_bCheckTime = (BOOL)g_EncryptInfo.m_EncryptOption.bLimitTime;
	OptionDialog.m_bCheckCount = (BOOL)g_EncryptInfo.m_EncryptOption.bLimitCount;
	OptionDialog.m_bErrorLimition = (BOOL)g_EncryptInfo.m_EncryptOption.bErrorLimit;
	OptionDialog.m_dwLimitCount = g_EncryptInfo.m_EncryptOption.ulMaxCount;
	OptionDialog.m_LimitTime = g_EncryptInfo.m_EncryptOption.LimitTime;
	OptionDialog.m_nMaxInputNumber = g_EncryptInfo.m_EncryptOption.ulErrorLimit;

	if (IDOK == OptionDialog.DoModal())
	{
		g_EncryptInfo.m_EncryptOption.bOptionOn = OptionDialog.m_bOptionOn;
		g_EncryptInfo.m_EncryptOption.bLimitTime = OptionDialog.m_bCheckTime;
		g_EncryptInfo.m_EncryptOption.bLimitCount = OptionDialog.m_bCheckCount;
		g_EncryptInfo.m_EncryptOption.bErrorLimit = OptionDialog.m_bErrorLimition;
		g_EncryptInfo.m_EncryptOption.ulMaxCount = OptionDialog.m_dwLimitCount;
		g_EncryptInfo.m_EncryptOption.LimitTime = OptionDialog.m_LimitTime;
		g_EncryptInfo.m_EncryptOption.ulErrorLimit = OptionDialog.m_nMaxInputNumber;
	}
	// TODO: Add your control notification handler code here
	
}
