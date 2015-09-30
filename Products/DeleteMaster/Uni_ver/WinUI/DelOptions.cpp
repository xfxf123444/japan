// DelOptions.cpp : implementation file
//

#include "stdafx.h"
#include "DeleteMaster.h"
#include "DelOptions.h"


extern BOOL				g_bVerify;
extern BOOL				g_bFormat;
extern DWORD			g_dwMaxTry;
/////////////////////////////////////////////////////////////////////////////
// CDelOptions dialog


CDelOptions::CDelOptions(CWnd* pParent /*=NULL*/)
	: CDialog(CDelOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDelOptions)
	m_bVerify = FALSE;
	m_bFormat = FALSE;
	m_MaxTry = 0;
	//}}AFX_DATA_INIT
}


void CDelOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDelOptions)
	DDX_Check(pDX, IDC_OPTIONS_VERIFY, m_bVerify);
	DDX_Check(pDX, IDC_OPTIONS_FORMAT, m_bFormat);
	DDX_Text(pDX, IDC_TRY_NUM, m_MaxTry);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDelOptions, CDialog)
	//{{AFX_MSG_MAP(CDelOptions)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDelOptions message handlers

void CDelOptions::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData ();

	g_bVerify = m_bVerify;
	g_bFormat = m_bFormat;
	g_dwMaxTry = m_MaxTry;
	CDialog::OnOK();
}

BOOL CDelOptions::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_bVerify = g_bVerify;
	m_bFormat = g_bFormat;
	m_MaxTry = g_dwMaxTry;

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
