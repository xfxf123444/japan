// DiskInfo.cpp : implementation file
//

#include "stdafx.h"
#include "DeleteMaster.h"
#include "DiskInfo.h"
#include "Fun.h"

extern CString						g_ModuleNumber;
extern CString						g_FirmWare;
extern CString						g_SerNumber;
extern CString						g_Capacity;

/////////////////////////////////////////////////////////////////////////////
// CDiskInfo dialog


CDiskInfo::CDiskInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CDiskInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDiskInfo)
	m_ModuleNumber = _T("");
	m_FirmWare = _T("");
	m_SerNumber = _T("");
	m_Capacity = _T("");
	//}}AFX_DATA_INIT
}


void CDiskInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDiskInfo)
	DDX_Text(pDX, IDC_MODULE_NUMBER, m_ModuleNumber);
	DDX_Text(pDX, IDC_FIRMWARE, m_FirmWare);
	DDX_Text(pDX, IDC_SERIAL_NUMBER, m_SerNumber);
	DDX_Text(pDX, IDC_CAPACITY, m_Capacity);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDiskInfo, CDialog)
	//{{AFX_MSG_MAP(CDiskInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDiskInfo message handlers

BOOL CDiskInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
#ifndef WIN_9X
	HDInfont(m_btDisk,m_hWnd);
#else
	HDInfo9x(m_btDisk,m_hWnd);
#endif
	m_ModuleNumber = g_ModuleNumber;
	m_FirmWare  = g_FirmWare;
	m_SerNumber = g_SerNumber;
	m_Capacity  = g_Capacity;
	UpdateData(FALSE);

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

