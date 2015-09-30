// Confirm.cpp : implementation file
//

#include "stdafx.h"
#include "DeleteMaster.h"
#include "Confirm.h"


extern	int					g_nDelMothed;

/////////////////////////////////////////////////////////////////////////////
// CConfirm dialog


CConfirm::CConfirm(CWnd* pParent /*=NULL*/)
	: CDialog(CConfirm::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfirm)
	m_szSelTar = _T("");
	m_method = _T("");
	//}}AFX_DATA_INIT
}


void CConfirm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfirm)
	DDX_Text(pDX, IDC_SELECT_TARGET, m_szSelTar);
	DDX_Text(pDX, IDC_DEL_MOTHED, m_method);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfirm, CDialog)
	//{{AFX_MSG_MAP(CConfirm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfirm message handlers

BOOL CConfirm::OnInitDialog() 
{
	CString		csMothed;
	CDialog::OnInitDialog();
	
	if(m_bDisk)
	{
		if(m_nDisk >= _T('A'))
		{
			if(HIWORD(m_nDisk))
				m_szSelTar.Format(IDS_CONFIRM_RD,m_nDisk,m_dwMinSec,m_dwMaxSec);
			else
				m_szSelTar.Format(IDS_CONFIRM_FD,m_nDisk,m_dwMinSec,m_dwMaxSec);
		}
		else
			m_szSelTar.Format(IDS_CONFIRM_DISK,m_nDisk+1,m_dwMinSec,m_dwMaxSec);
	}
	else
		m_szSelTar.Format(IDS_CONFIRM_PARTITION,m_DriveLetter,m_nDisk+1,
						m_dwMinSec,m_dwMaxSec);

	switch(g_nDelMothed)
	{
	case 0:
		csMothed.LoadString(IDS_00_MOTHED);
		break;
	case 1:
		csMothed.LoadString(IDS_FF_MOTHED);
		break;
	case 2:
		csMothed.LoadString(IDS_RANDOM_MOTHED);
		break;
	case 3:
		csMothed.LoadString(IDS_NSA_MOTHED);
		break;
	case 4:
		csMothed.LoadString(IDS_OLD_NSA_MOTHED);
		break;
	case 5:
		csMothed.LoadString(IDS_DOD_STD_MOTHED);
		break;
	case 6:
		csMothed.LoadString(IDS_NATO_MOTHED);
		break;
	case 7:
		csMothed.LoadString(IDS_GUTMANN_MOTHED);
		break;
	}
	m_method.Format(IDS_METHOD,csMothed);
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
