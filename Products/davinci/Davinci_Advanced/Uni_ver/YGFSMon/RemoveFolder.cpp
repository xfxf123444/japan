// RemoveFolder.cpp : implementation file
//

#include "stdafx.h"
#include "ygfsmon.h"
#include "RemoveFolder.h"
#include "..\Decrypt\DecryptFunction\DecryptFunction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRemoveFolder dialog


CRemoveFolder::CRemoveFolder(CWnd* pParent /*=NULL*/)
	: CDialog(CRemoveFolder::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRemoveFolder)
	m_bDecryptFile = FALSE;
	m_Directory = _T("");
	m_Password = _T("");
	//}}AFX_DATA_INIT
}


void CRemoveFolder::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRemoveFolder)
	DDX_Check(pDX, IDC_DECRYPT_FILE, m_bDecryptFile);
	DDX_Text(pDX, IDC_DIRECTORY, m_Directory);
	DDX_Text(pDX, IDC_PASSWORD, m_Password);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRemoveFolder, CDialog)
	//{{AFX_MSG_MAP(CRemoveFolder)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRemoveFolder message handlers

void CRemoveFolder::OnOK() 
{
	CString strText,strTitle;
	UpdateData(TRUE);	
	if (m_Password != m_MonitorItem.szPassword)
	{
		strText.LoadString(IDS_INVALID_PASSWORD);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
		return;
	}
	CONVERT_BUF MonitorItem;
	WCHAR szMonitorFile[MAX_PATH];
	ZeroMemory(szMonitorFile, sizeof(szMonitorFile));
	wcscpy(szMonitorFile,m_Directory);
	WideCharToMultiByte(CP_ACP, 0, szMonitorFile, MAX_PATH, MonitorItem.szMonitorFile, MAX_PATH, 0, 0);
	MonitorItem.usNodeType = CONVERT_NODETYPE_DIRECTORY;
	if( FALSE == YGFSMonRemoveMonitorItem(&MonitorItem) )
	{
		TRACE(L"\nYGFSMonRemoveIncludePath error in OnRemove.");
	}

	if (m_bDecryptFile)
		DecryptOneDirSeparately((LPCTSTR)m_Directory,(LPCTSTR)m_Password,TRUE);

	CDialog::OnOK();
}

BOOL CRemoveFolder::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_Directory = m_MonitorItem.szMonitorPath;
	UpdateData(FALSE);
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
