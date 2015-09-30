// DiskView.cpp : implementation file
//

#include "stdafx.h"
#include "DeleteMaster.h"
#include "DiskView.h"
#include "Fun.h"
#include "SectorView.h"

extern	CImageList				g_imgList_BOX;
extern  YG_PARTITION_INFO*		g_pFixDiskInfo;
/////////////////////////////////////////////////////////////////////////////
// CDiskView property page

IMPLEMENT_DYNCREATE(CDiskView, CPropertyPage)

CDiskView::CDiskView() : CPropertyPage(CDiskView::IDD)
{
	//{{AFX_DATA_INIT(CDiskView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CDiskView::~CDiskView()
{
}

void CDiskView::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDiskView)
	DDX_Control(pDX, IDC_DM_VIEW_PAR_LIST, m_ViewParList);
	DDX_Control(pDX, IDC_VIEW_ENTIR_DISK, m_CheckEntireDisk);
	DDX_Control(pDX, IDC_VIEW_DISK_COMBO, m_ViewDiskList);
	DDX_Control(pDX, IDC_DM_VIEW, m_ButtonView);
	DDX_Control(pDX, IDC_VIEW_CLEAR, m_ButtonClear);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDiskView, CPropertyPage)
	//{{AFX_MSG_MAP(CDiskView)
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_VIEW_DISK_COMBO, OnSelchangeViewDiskCombo)
	ON_BN_CLICKED(IDC_VIEW_CLEAR, OnViewClear)
	ON_BN_CLICKED(IDC_VIEW_ENTIR_DISK, OnViewEntirDisk)
	ON_NOTIFY(NM_CLICK, IDC_DM_VIEW_PAR_LIST, OnClickDmViewParList)
	ON_BN_CLICKED(IDC_DM_VIEW, OnDmView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDiskView message handlers

BOOL CDiskView::OnInitDialog() 
{
	CRect		rect;
	int			nSel = 0;
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_ViewDiskList.SetImageList(&g_imgList_BOX);
	AddFixDisk(&m_ViewDiskList,TRUE);
	while(m_ViewDiskList.GetItemData(nSel) >= _T('A'))
	{
		nSel ++;
	}
	m_ViewDiskList.SetCurSel(nSel);
	
	m_ViewParList.GetWindowRect(&rect);
	ScreenToClient(rect);
	InitList(m_ViewParList.m_hWnd,rect);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDiskView::OnSize(UINT nType, int cx, int cy) 
{
	CPropertyPage::OnSize(nType, cx, cy);
	
	CRect		rect;
	int			nWidth,nButtonHigh,nDelAndCle;
	CPropertyPage::OnSize(nType, cx, cy);
	
	if(m_ViewParList)
	{
		m_ViewParList.GetWindowRect (&rect);
		ScreenToClient (&rect);
		rect.right = cx - rect.left;
		nWidth	   = rect.left;
		m_ViewParList.MoveWindow (rect);
		ResizeListColumn(m_ViewParList.m_hWnd);
	}

	if(m_ButtonClear)
	{
		m_ButtonClear.GetClientRect(&rect);
		rect.left  = cx - (rect.right + nWidth );
		rect.right = cx - nWidth;
		rect.top   = cy - (rect.bottom + nWidth);
		rect.bottom= cy - nWidth;
		m_ButtonClear.MoveWindow (rect);
	}
	
	if(m_ButtonView)
	{
		nDelAndCle = 3*rect.Width ()/2;
		rect.left  -= nDelAndCle;
		rect.right -= nDelAndCle;
		m_ButtonView.MoveWindow (rect);
		nButtonHigh = rect.Height ();
	}

	if(m_ViewParList)
	{
		m_ViewParList.GetWindowRect (&rect);
		ScreenToClient (&rect);
		rect.bottom = cy - nWidth - 2*nButtonHigh;
		m_ViewParList.MoveWindow (rect);
		//ResizeListColumn(m_MakeParList.m_hWnd);
	}
	
}

void CDiskView::OnSelchangeViewDiskCombo() 
{
	int		nSel;

	nSel = m_ViewDiskList.GetCurSel ();
	nSel = m_ViewDiskList.GetItemData (nSel);
	m_ViewParList.DeleteAllItems();
	if(nSel >= _T('A'))
	{
		m_CheckEntireDisk.SetCheck(1);
		m_CheckEntireDisk.EnableWindow(TRUE);
		m_ViewParList.EnableWindow(FALSE);
	}
	else
	{
		AddList(&m_ViewParList, nSel+DISK_BASE);
		m_CheckEntireDisk.SetCheck(0);
		m_ViewParList.EnableWindow(TRUE);
	}
}

BOOL CDiskView::OnSetActive() 
{
	OnSelchangeViewDiskCombo();
	
	return CPropertyPage::OnSetActive();
}

void CDiskView::OnViewClear() 
{
	int					i,nItemCount;
	LVITEM				lvitem;

	lvitem.mask		= LVIF_IMAGE;
	lvitem.iSubItem = 0;

	m_ViewParList.EnableWindow(TRUE);
	nItemCount = m_ViewParList.GetItemCount ();
	for(i=0;i<nItemCount;i++)
	{
		lvitem.iItem = i;
		lvitem.iImage= 0;
		m_ViewParList.SetItem (&lvitem);
	}
	m_CheckEntireDisk.SetCheck(0);
}

void CDiskView::OnViewEntirDisk() 
{
	if(m_CheckEntireDisk.GetCheck())
	{
		m_ViewParList.EnableWindow(FALSE);
	}
	else
	{
		m_ViewParList.EnableWindow(TRUE);
	}	
}

void CDiskView::OnClickDmViewParList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int					i,nItemCount;
	CRect				rect;
	CListCtrl*			plist;
	LVITEM				lvitem;
	NMITEMACTIVATE		*pItem = (LPNMITEMACTIVATE)pNMHDR;

	lvitem.mask		= LVIF_IMAGE;
	lvitem.iSubItem = 0;

	plist = (CListCtrl*)FromHandle(pItem->hdr.hwndFrom);
	nItemCount = plist->GetItemCount ();
	for(i=0;i<nItemCount;i++)
	{
		lvitem.iItem = i;
		lvitem.iImage= 0;
		plist->SetItem (&lvitem);
	}
	
	lvitem.iItem = pItem->iItem;
	plist->GetItem(&lvitem);
	lvitem.iImage = (lvitem.iImage+1) % 2;
	plist->SetItem(&lvitem);

	*pResult = 0;
}

void CDiskView::OnDmView() 
{
	LVITEM				lvitem;
	BIOS_DRIVE_PARAM	DriveParam;
	int					i;
	YG_PARTITION_INFO	*pTargetParInfo = NULL;
	CSectorView			ViewSecDlg;
	int					nSel;
	int					nSelDisk;
#ifdef WIN_9X
	TCHAR				szDrv;
	DWORD				dwSetValue;
#endif
	CString				cstr,csCaption;

	csCaption.LoadString (IDS_DM_ERROR);
	ViewSecDlg.m_SecInfo.dwMaxSec = 0;

	if(m_CheckEntireDisk.GetCheck())
	{
		nSel = m_ViewDiskList.GetCurSel ();
		nSel = m_ViewDiskList.GetItemData (nSel);
		nSelDisk = HIWORD(nSel);
		nSel = LOWORD(nSel);
		if(nSel >= _T('A'))
		{
			if(!HasMedia(nSel))
			{
				cstr.LoadString (IDS_FD_NOT_READY);
				MessageBox(cstr,csCaption,MB_OK|MB_ICONSTOP);
				return;
			}
#ifdef WIN_9X
			if(!nSelDisk)
			{
				if(!GetDriveParam(nSel-'A',&DriveParam)) return;
			}
			else
			{
				if(!GetRemovableDiskSize(DISK_BASE+nSelDisk,&szDrv,&DriveParam)) return;
			}
#else
			if(!GetFDParam(nSel,&DriveParam)) return;
			if(HIWORD(nSel))
			{
				if(!GetRemovableDiskSize(DISK_BASE+HIWORD(nSel), NULL,(DWORD*)(&DriveParam.dwSectors)))
					return ;
			}
#endif
		}
		else
		{
			if(!GetDriveParam(DISK_BASE+nSel,&DriveParam)) return;
		}
		//pTargetParInfo = g_pFixDiskInfo;
		//if(!GetDriveParam(pTargetParInfo->btDiskNum+DISK_BASE,&DriveParam)) return;
		ViewSecDlg.m_SecInfo.dwSelSec = 0;
		ViewSecDlg.m_SecInfo.dwMaxSec = (DWORD)DriveParam.dwSectors;
		ViewSecDlg.m_SecInfo.nSelDisk = MAKELONG((WORD)nSel,(WORD)nSelDisk); ;
		ViewSecDlg.m_SecInfo.dwMinSec = 0;
	}
	else
	{
		lvitem.mask = LVIF_IMAGE;
		lvitem.iSubItem = 0;
	
		for(i = 0; i < m_ViewParList.GetItemCount (); i++) 
		{
			lvitem.iItem = i;
			m_ViewParList.GetItem(&lvitem);
			if(lvitem.iImage == IMG_LIST_SELECT) 
			{
				pTargetParInfo = (YG_PARTITION_INFO*)m_ViewParList.GetItemData (i);
				ViewSecDlg.m_SecInfo.dwSelSec = pTargetParInfo->dwStartSector ;
				ViewSecDlg.m_SecInfo.dwMaxSec = pTargetParInfo->dwStartSector + pTargetParInfo->dwPartSize ;
				ViewSecDlg.m_SecInfo.nSelDisk = pTargetParInfo->btDiskNum ;
				ViewSecDlg.m_SecInfo.dwMinSec = pTargetParInfo->dwStartSector ;
				if(pTargetParInfo->bLogic)
				{
					ViewSecDlg.m_SecInfo.dwSelSec += DISK_MIN_SECTOR ;
					ViewSecDlg.m_SecInfo.dwMinSec += DISK_MIN_SECTOR;
				}
				break;
			}
		}
	}
	if(ViewSecDlg.m_SecInfo.dwMaxSec)
		ViewSecDlg.DoModal ();
}
