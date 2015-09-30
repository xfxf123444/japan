// SetSecNum.cpp : implementation file
//

#include "stdafx.h"
#include "DeleteMaster.h"
#include "SetSecNum.h"

/////////////////////////////////////////////////////////////////////////////
// CSetSecNum dialog


CSetSecNum::CSetSecNum(CWnd* pParent /*=NULL*/)
	: CDialog(CSetSecNum::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetSecNum)
	m_szSecNum = _T("");
	m_szRange = _T("");
	//}}AFX_DATA_INIT
}


void CSetSecNum::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetSecNum)
	DDX_Text(pDX, IDC_SECTOR_NUMBER, m_szSecNum);
	DDV_MaxChars(pDX, m_szSecNum, 12);
	DDX_Text(pDX, IDC_SECTOR_RANGE, m_szRange);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetSecNum, CDialog)
	//{{AFX_MSG_MAP(CSetSecNum)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetSecNum message handlers

BOOL CSetSecNum::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_szRange.Format(IDS_SECTOR_RANGE,m_nSecMin,m_nSecMax-1);
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
