// SectorView.cpp : implementation file
//

#include "stdafx.h"
#include "DeleteMaster.h"
#include "Fun.h"
#include "SectorView.h"
#include "SecInfo.h"
#include "SetSecNum.h"

//extern	CFont				g_Font;
extern  BYTE				g_btSecBuf[BYTEINSEC];
		CSecInfo			*g_pwndSecInfo;
/////////////////////////////////////////////////////////////////////////////
// CSectorView dialog


CSectorView::CSectorView(CWnd* pParent /*=NULL*/)
	: CDialog(CSectorView::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSectorView)
	m_szSectorCHS = _T("");
	m_szSectorNum = _T("");
	//}}AFX_DATA_INIT
}


void CSectorView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSectorView)
	DDX_Control(pDX, IDC_SECTOR_PREV, m_PrevBut);
	DDX_Control(pDX, IDC_SECTOR_NEXT, m_NextBut);
	DDX_Control(pDX, IDC_SET_SECTOR_NUM, m_SectorNumBut);
	DDX_Text(pDX, IDC_SECTOR_CHS, m_szSectorCHS);
	DDX_Text(pDX, IDC_SECTOR_NUM, m_szSectorNum);
	DDV_MaxChars(pDX, m_szSectorNum, 12);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSectorView, CDialog)
	//{{AFX_MSG_MAP(CSectorView)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_SECTOR_NEXT, OnSectorNext)
	ON_BN_CLICKED(IDC_SECTOR_PREV, OnSectorPrev)
	ON_BN_CLICKED(IDC_SET_SECTOR_NUM, OnSetSectorNum)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSectorView message handlers

BOOL CSectorView::OnInitDialog() 
{
	CRect		rect,secrect,nextrect;
#ifdef WIN_9X
	TCHAR				szDrv;
	DWORD				dwSetValue;
#endif
	CDialog::OnInitDialog();

	GetClientRect(&rect);

	if(m_SecInfo.nSelDisk >= _T('A'))
	{
#ifdef WIN_9X
		if(HIWORD(m_SecInfo.nSelDisk))
		{
			if(!GetRemovableDiskSize(DISK_BASE+HIWORD(m_SecInfo.nSelDisk),&szDrv,&m_DriveParam)) return FALSE;
		}
		else
		{
			if(!GetDriveParam(m_SecInfo.nSelDisk-'A',&m_DriveParam)) return FALSE;
		}
#else
		if(!GetFDParam(m_SecInfo.nSelDisk,&m_DriveParam)) return FALSE;
		if(HIWORD(m_SecInfo.nSelDisk))
		{
			if(!GetRemovableDiskSize(DISK_BASE+HIWORD(m_SecInfo.nSelDisk), NULL,(DWORD*)(&m_DriveParam.dwSectors)))
				return FALSE;
		}
#endif
	}
	else
	{
		if(!GetDriveParam(m_SecInfo.nSelDisk+DISK_BASE,&m_DriveParam))
			return FALSE;
	}

	if(m_SecInfo.nSelDisk >= _T('A'))
	{
		if(!ReadFDSector(m_SecInfo.dwSelSec,1,g_btSecBuf,m_SecInfo.nSelDisk,&m_DriveParam)) return FALSE;
	}
	else
	{
 		if(!ReadSector(m_SecInfo.dwSelSec,1,g_btSecBuf,m_SecInfo.nSelDisk + DISK_BASE,&m_DriveParam)) return FALSE;
	}

	m_SectorNumBut.GetWindowRect (&secrect);
	ScreenToClient (&secrect);
	rect.top = secrect.bottom + 5;
	m_NextBut.GetWindowRect (&nextrect);
	ScreenToClient (&nextrect);
	rect.bottom  = nextrect.top - 5;

	m_szSectorNum.Format (_T("%d"),m_SecInfo.dwSelSec);
	Showchs();
	UpdateData(FALSE);
	g_pwndSecInfo = new CSecInfo(this,rect);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSectorView::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	delete(g_pwndSecInfo);
	CDialog::OnClose();
}

void CSectorView::OnOK() 
{
	// TODO: Add extra validation here
	delete(g_pwndSecInfo);
	CDialog::OnOK();
}

void CSectorView::OnSectorNext() 
{
	// TODO: Add your control notification handler code here
	if(	m_SecInfo.dwSelSec != m_SecInfo.dwMaxSec - 1)
	{
		m_SecInfo.dwSelSec ++;
		if(m_SecInfo.nSelDisk >= _T('A'))
		{
			if(!ReadFDSector(m_SecInfo.dwSelSec,1,g_btSecBuf,m_SecInfo.nSelDisk,&m_DriveParam))
			{
				m_SecInfo.dwSelSec --;
			}
		}
		else
		{
 			if(!ReadSector(m_SecInfo.dwSelSec,1,g_btSecBuf,m_SecInfo.nSelDisk + DISK_BASE,&m_DriveParam))
			{
				m_SecInfo.dwSelSec --;
			}
		}
 		//ReadSector(m_SecInfo.dwSelSec,1,g_btSecBuf,m_SecInfo.nSelDisk+DISK_BASE,&m_DriveParam);
		g_pwndSecInfo->Invalidate(); 
		m_szSectorNum.Format (_T("%d"),m_SecInfo.dwSelSec);
		Showchs();
		UpdateData(FALSE);
	}
}

void CSectorView::OnSectorPrev() 
{
	if(	m_SecInfo.dwSelSec != m_SecInfo.dwMinSec)
	{
		m_SecInfo.dwSelSec --;
		if(m_SecInfo.nSelDisk >= _T('A'))
		{
			if(!ReadFDSector(m_SecInfo.dwSelSec,1,g_btSecBuf,m_SecInfo.nSelDisk,&m_DriveParam))
			{
				m_SecInfo.dwSelSec ++;
			}
		}
		else
		{
 			if(!ReadSector(m_SecInfo.dwSelSec,1,g_btSecBuf,m_SecInfo.nSelDisk + DISK_BASE,&m_DriveParam))
			{
				m_SecInfo.dwSelSec ++;
			}
		}

// 		ReadSector(m_SecInfo.dwSelSec,1,g_btSecBuf,m_SecInfo.nSelDisk+DISK_BASE,&m_DriveParam);
		g_pwndSecInfo->Invalidate(); 
		m_szSectorNum.Format (_T("%d"),m_SecInfo.dwSelSec);
		Showchs();
		UpdateData(FALSE);
	}
}

void CSectorView::Showchs()
{
	DWORD 		dwHead,dwSector,dwCylinder;
	
	if(HIWORD(m_SecInfo.nSelDisk))
	{
		m_szSectorCHS = _T("");
		return ;
	}

	dwHead		= (((m_SecInfo.dwSelSec/m_DriveParam.dwSecPerTrack)%m_DriveParam.dwHeads)&0xff); 
	dwSector 	= ((m_SecInfo.dwSelSec % m_DriveParam.dwSecPerTrack)+1)&0xff;
	dwCylinder	= (m_SecInfo.dwSelSec/m_DriveParam.dwSecPerTrack)/m_DriveParam.dwHeads; 
	
	m_szSectorCHS.Format(IDS_CHS,dwCylinder,dwHead,dwSector);
}

void CSectorView::OnSetSectorNum() 
{
	CSetSecNum		SecNumDlg;
	DWORD			dwSector;
	TCHAR   			*stopstring;

	SecNumDlg.m_nSecMin = m_SecInfo.dwMinSec ;
	SecNumDlg.m_nSecMax = m_SecInfo.dwMaxSec ;
	SecNumDlg.DoModal ();
	if(!SecNumDlg.m_szSecNum.IsEmpty())
	{
		dwSector = _tcstoul(SecNumDlg.m_szSecNum,&stopstring,10);
		if(dwSector > m_SecInfo.dwMaxSec) dwSector = m_SecInfo.dwMaxSec ;
		if(dwSector < m_SecInfo.dwMinSec) dwSector = m_SecInfo.dwMinSec ;
		m_SecInfo.dwSelSec = dwSector;

		if(m_SecInfo.nSelDisk >= _T('A'))
		{
			if(!ReadFDSector(m_SecInfo.dwSelSec,1,g_btSecBuf,m_SecInfo.nSelDisk,&m_DriveParam)) return ;
		}
		else
		{
 			if(!ReadSector(m_SecInfo.dwSelSec,1,g_btSecBuf,m_SecInfo.nSelDisk + DISK_BASE,&m_DriveParam)) return ;
		}
		//ReadSector(m_SecInfo.dwSelSec,1,g_btSecBuf,m_SecInfo.nSelDisk+DISK_BASE,&m_DriveParam);
		g_pwndSecInfo->Invalidate(); 
		m_szSectorNum.Format (_T("%d"),m_SecInfo.dwSelSec);
		Showchs();
		UpdateData(FALSE);
	}
	
}
