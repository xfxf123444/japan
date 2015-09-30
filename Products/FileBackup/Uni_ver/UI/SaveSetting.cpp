// SaveSetting.cpp : implementation file
//

#include "stdafx.h"
#include "am01.h"
#include "SaveSetting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSaveSetting dialog


CSaveSetting::CSaveSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CSaveSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSaveSetting)
	m_SettingName = _T("");
	//}}AFX_DATA_INIT
}


void CSaveSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSaveSetting)
	DDX_Text(pDX, IDC_SETTING_NAME, m_SettingName);
	DDV_MaxChars(pDX, m_SettingName, 50);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSaveSetting, CDialog)
	//{{AFX_MSG_MAP(CSaveSetting)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSaveSetting message handlers

void CSaveSetting::OnOK() 
{
	UpdateData(TRUE);
	if (m_SettingName == L"")
	{
		AfxMessageBox(IDS_SETTING_NAME_EMPTY,MB_OK | MB_ICONINFORMATION);
		return;
	}
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}
