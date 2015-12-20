// DelPar.cpp : implementation file
//

#include "stdafx.h"
#include "DeleteMaster.h"
#include "DelPar.h"
#include "Fun.h"
#include "Confirm.h"
#include "DelProc.h"
#include "DiskInfo.h"
#include "DelOptions.h"

extern	HWND					g_hMain;
extern	CImageList				g_imgList_BOX;
extern  YG_PARTITION_INFO*		g_pFixDiskInfo;
extern	int						g_nDelMothed;
extern	YG_PARTITION_INFO*		g_pTargetParInfo ;
extern  BOOL                    g_bBeepOff;
/////////////////////////////////////////////////////////////////////////////
// CDelPar property page

IMPLEMENT_DYNCREATE(CDelPar, CPropertyPage)

CDelPar::CDelPar() : CPropertyPage(CDelPar::IDD)
{
	//{{AFX_DATA_INIT(CDelPar)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CDelPar::~CDelPar()
{
}

void CDelPar::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDelPar)
	DDX_Control(pDX, IDC_BEEP_OFF, m_BeepOff);
	DDX_Control(pDX, IDC_DM_DISK_INFO, m_ButtonDiskInfo);
	DDX_Control(pDX, IDC_LABEL_MOTHED, m_LabelMothed);
	DDX_Control(pDX, IDC_DELETE_MOTHED, m_SelMothed);
	DDX_Control(pDX, IDC_DELETE_CLEAR, m_ButtonClear);
	DDX_Control(pDX, IDC_DM_DELETE, m_ButtonDelete);
	DDX_Control(pDX, IDC_DM_DELETE_PAR_LIST, m_DelParList);
	DDX_Control(pDX, IDC_DELETE_ENTIR_DISK, m_CheckEntirDisk);
	DDX_Control(pDX, IDC_DELETE_DISK_COMBO, m_DelDiskList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDelPar, CPropertyPage)
	//{{AFX_MSG_MAP(CDelPar)
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_DELETE_DISK_COMBO, OnSelchangeDeleteDiskCombo)
	ON_NOTIFY(NM_CLICK, IDC_DM_DELETE_PAR_LIST, OnClickDmDeleteParList)
	ON_BN_CLICKED(IDC_DM_DELETE, OnDmDelete)
	ON_BN_CLICKED(IDC_DELETE_CLEAR, OnDeleteClear)
	ON_BN_CLICKED(IDC_DELETE_ENTIR_DISK, OnDeleteEntirDisk)
	ON_BN_CLICKED(IDC_DM_DISK_INFO, OnDmDiskInfo)
	ON_BN_CLICKED(IDC_DM_OPTIONS, OnDmOptions)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDelPar message handlers

void CDelPar::OnOK() 
{
	::PostMessage(g_hMain,WM_CLOSE,0,0);
	CPropertyPage::OnOK();
}

BOOL CDelPar::OnInitDialog() 
{
	CRect		rect;
	int			nSel = 0;
	CPropertyPage::OnInitDialog();

	m_DelDiskList.SetImageList(&g_imgList_BOX);
	AddFixDisk(&m_DelDiskList,TRUE);
	while(m_DelDiskList.GetItemData(nSel) >= _T('A'))
	{
		nSel ++;
	}
	m_DelDiskList.SetCurSel(nSel);
	
	AddDelMothed(&m_SelMothed);

	m_DelParList.GetWindowRect(&rect);
	ScreenToClient(rect);
	InitList(m_DelParList.m_hWnd,rect);
	UpdateData(FALSE);
	return TRUE;
}

void CDelPar::OnSize(UINT nType, int cx, int cy) 
{
	CRect		rect;
	int			nWidth,nButtonHigh,nDelAndCle;
	int			nLabelR;
	CPropertyPage::OnSize(nType, cx, cy);
	
	if(m_DelParList)
	{
		m_DelParList.GetWindowRect (&rect);
		ScreenToClient (&rect);
		rect.right = cx - rect.left;
		nWidth	   = rect.left;
		m_DelParList.MoveWindow (rect);
		ResizeListColumn(m_DelParList.m_hWnd);
	}

	if(m_LabelMothed)
	{
		m_LabelMothed.GetClientRect(&rect);
		rect.left  = nWidth;
		rect.right = nWidth+rect.right;
		nLabelR	   = rect.right;
		rect.top   = cy - (rect.bottom + nWidth);
		rect.bottom= cy - nWidth;
		m_LabelMothed.MoveWindow (rect);
	}

	if(m_SelMothed)
	{
		m_SelMothed.GetClientRect(&rect);
		rect.left  = nLabelR + 1;
		rect.right = rect.left+rect.right;
		rect.top   = cy - (rect.bottom + nWidth) + 3;
		rect.bottom= cy - nWidth +3;
		m_SelMothed.MoveWindow (rect);
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
	
	if(m_ButtonDelete)
	{
		nDelAndCle = 3*rect.Width ()/2;
		rect.left  -= nDelAndCle;
		rect.right -= nDelAndCle;
		m_ButtonDelete.MoveWindow (rect);
		nButtonHigh = rect.Height ();
	}

	if(m_BeepOff)
	{
		rect.left  -= nDelAndCle;
		rect.right -= nDelAndCle;
		m_BeepOff.MoveWindow (rect);
	}

	if(m_DelParList)
	{
		m_DelParList.GetWindowRect (&rect);
		ScreenToClient (&rect);
		rect.bottom = cy - nWidth - 2*nButtonHigh;
		m_DelParList.MoveWindow (rect);
	}
}

void CDelPar::OnSelchangeDeleteDiskCombo() 
{
	int		nSel;

	nSel = m_DelDiskList.GetCurSel ();
	nSel = m_DelDiskList.GetItemData (nSel);
	m_DelParList.DeleteAllItems();
	m_driveLetterSetOfCurrentDisk.clear();
	if(nSel >= _T('A'))
	{
		m_CheckEntirDisk.SetCheck(1);
		m_CheckEntirDisk.EnableWindow(TRUE);
		m_DelParList.EnableWindow(FALSE);
		m_ButtonDiskInfo.EnableWindow(FALSE);
		m_driveLetterSetOfCurrentDisk.push_back(nSel);
	}
	else
	{
		m_DelParList.EnableWindow(TRUE);
		m_ButtonDiskInfo.EnableWindow(TRUE);
		AddList(&m_DelParList, nSel+DISK_BASE);
		for(int i = 0; i < m_DelParList.GetItemCount (); i++) 
		{
			YG_PARTITION_INFO*	pTargetParInfo		= (YG_PARTITION_INFO*)m_DelParList.GetItemData (i);
			if (pTargetParInfo->DriveLetter != _T('*')) {
				m_driveLetterSetOfCurrentDisk.push_back(pTargetParInfo->DriveLetter);
			}
		}
		m_CheckEntirDisk.SetCheck(0);
		if(IsSystemDisk())
			m_CheckEntirDisk.EnableWindow(FALSE);
		else
			m_CheckEntirDisk.EnableWindow(TRUE);
	}
}

void CDelPar::OnClickDmDeleteParList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CString				cstr,csCaption;
	int					i,nItemCount;
	CRect				rect;
	CListCtrl*			plist;
	LVITEM				lvitem;
	NMITEMACTIVATE		*pItem = (LPNMITEMACTIVATE)pNMHDR;
	TCHAR				szSysDir[MAX_PATH];
	YG_PARTITION_INFO	*pTargetParInfo = NULL;

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
	
	if(pItem->iItem != -1)
	{
		GetSystemDirectory(szSysDir,MAX_PATH);
		pTargetParInfo = (YG_PARTITION_INFO*)m_DelParList.GetItemData (pItem->iItem);
		if(pTargetParInfo->DriveLetter == szSysDir[0] 
		|| pTargetParInfo->BootIndicator
		|| (pTargetParInfo->PartitionStyle == PARTITION_STYLE_GPT 
			&& pTargetParInfo->GUIDType == PARTITION_SYSTEM_GUID))
		{
			cstr.LoadString (IDS_NOT_DELETE_SYSTEM_PAR);
			csCaption.LoadString (IDS_DM_ERROR);
			::MessageBox(m_hWnd,cstr,csCaption,MB_OK|MB_ICONWARNING);
			*pResult = 0;
			return ;
		}
		else
		{
			lvitem.iItem = pItem->iItem;
			plist->GetItem(&lvitem);
			lvitem.iImage = (lvitem.iImage+1) % 2;
			plist->SetItem(&lvitem);
		}
	}	
	*pResult = 0;
}

void CDelPar::OnDmDelete() 
{
	LVITEM				lvitem;
	int					i;
	YG_PARTITION_INFO	*pTargetParInfo = NULL;
	CConfirm			ConfirmDlg;
	CDelProc			DelProcDlg;
	BOOL				bSel = FALSE;
	BIOS_DRIVE_PARAM	DriveParam;
	CString				cstr,csCaption;
	int					nSel;
	int					nSelDisk;

	UpdateData(TRUE);
	g_bBeepOff = m_BeepOff.GetCheck();

	nSel = m_SelMothed.GetCurSel ();
	g_nDelMothed = m_SelMothed.GetItemData (nSel);

	csCaption.LoadString (IDS_DM_ERROR);

	if(m_CheckEntirDisk.GetCheck())
	{

		nSel = m_DelDiskList.GetCurSel ();
		nSel = m_DelDiskList.GetItemData (nSel);
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
			if(nSelDisk)
			{
				GetRemovableDiskSize(DISK_BASE+nSelDisk, NULL,(DWORD*)(&DriveParam.dwSectors));
			}
			else
			{
				if(!GetFDParam(nSel,&DriveParam)) return ;
			}
		}
		else
		{
			if(!GetDriveParam(g_pFixDiskInfo->btDiskNum+DISK_BASE,&DriveParam)) return;
		}
		ConfirmDlg.m_bDisk = TRUE;
		bSel = TRUE;
	}
	else
	{
		lvitem.mask = LVIF_IMAGE;
		lvitem.iSubItem = 0;
	
		for(i = 0; i < m_DelParList.GetItemCount (); i++) 
		{
			lvitem.iItem = i;
			m_DelParList.GetItem(&lvitem);
			if(lvitem.iImage == IMG_LIST_SELECT) 
			{
				pTargetParInfo		= (YG_PARTITION_INFO*)m_DelParList.GetItemData (i);
				g_pTargetParInfo	= pTargetParInfo;
				bSel				= TRUE;
				ConfirmDlg.m_bDisk	= FALSE;
				break;
			}
		}
	}
	if(bSel)
	{
		if(ConfirmDlg.m_bDisk)
		{
			nSel				  = MAKELONG((WORD)nSel,(WORD)nSelDisk);
			ConfirmDlg.m_nDisk	  = nSel;
			ConfirmDlg.m_dwMinSec = 0;
			ConfirmDlg.m_dwMaxSec = (DWORD)DriveParam.dwSectors;
		}
		else
		{
			ConfirmDlg.m_nDisk = pTargetParInfo->btDiskNum;
			ConfirmDlg.m_dwMinSec = pTargetParInfo->dwStartSector;
			ConfirmDlg.m_dwMaxSec = pTargetParInfo->dwStartSector + pTargetParInfo->dwPartSize;
			ConfirmDlg.m_DriveLetter = pTargetParInfo->DriveLetter ;
		}
		
		if(ConfirmDlg.DoModal() == IDOK)
		{
			if (!g_bBeepOff) Beep(1000,100);
			DelProcDlg.m_bActive	= FALSE;
			DelProcDlg.m_bLogc		= FALSE;
			DelProcDlg.m_bDisk = ConfirmDlg.m_bDisk ;
			if (DelProcDlg.m_bDisk) {
				DelProcDlg.m_driveLetterSet = m_driveLetterSetOfCurrentDisk;
			}
			DelProcDlg.m_nDisk = ConfirmDlg.m_nDisk ;
			DelProcDlg.m_dwMinSec = ConfirmDlg.m_dwMinSec ;
			DelProcDlg.m_dwMaxSec = ConfirmDlg.m_dwMaxSec ;
			DelProcDlg.m_DriveLetter = ConfirmDlg.m_DriveLetter;
			if(!ConfirmDlg.m_bDisk)
			{
				if(pTargetParInfo->bLogic)
				{
					DelProcDlg.m_bLogc		= TRUE;
					DelProcDlg.m_dwMinSec += DISK_MIN_SECTOR ;
				}
				if(pTargetParInfo->BootIndicator || pTargetParInfo->GUIDType == PARTITION_SYSTEM_GUID)
				{
					DelProcDlg.m_bActive	= TRUE;
				}
			}
			if (!DelProcDlg.LockDiskOrVolume()) {
				CString msg;
				if (DelProcDlg.m_bDisk){
						CString temp;
						temp.LoadString(IDS_CANNOT_LOCK_DISK);
						msg.Format(temp, DelProcDlg.m_nDisk-DISK_BASE, DelProcDlg.m_nDisk-DISK_BASE);
				} 
				else{
					CString temp;
					temp.LoadString(IDS_CANNOT_LOCK_PARTITION);
					msg.Format(temp, DelProcDlg.m_DriveLetter, DelProcDlg.m_DriveLetter);
				}
				CString title;
				title.LoadString(IDS_DM_ERROR);
				MessageBox(msg, title, MB_OK);
			}
			else {
				DelProcDlg.DoModal ();
				DelProcDlg.UnlockDiskOrVolume();
				OnSelchangeDeleteDiskCombo();
			}
		}
	}
}

void CDelPar::OnDeleteClear() 
{
	int					i,nItemCount;
	LVITEM				lvitem;

	lvitem.mask		= LVIF_IMAGE;
	lvitem.iSubItem = 0;

	nItemCount = m_DelParList.GetItemCount ();
	for(i=0;i<nItemCount;i++)
	{
		lvitem.iItem = i;
		lvitem.iImage= 0;
		m_DelParList.SetItem (&lvitem);
	}
	m_CheckEntirDisk.SetCheck(0);
	m_DelParList.EnableWindow(TRUE);
}

BOOL CDelPar::OnSetActive() 
{
	OnSelchangeDeleteDiskCombo();

	return CPropertyPage::OnSetActive();
}

void CDelPar::OnDeleteEntirDisk() 
{
	if(m_CheckEntirDisk.GetCheck())
	{
		m_DelParList.EnableWindow(FALSE);
	}
	else
	{
		m_DelParList.EnableWindow(TRUE);
	}	
}

void CDelPar::OnDmDiskInfo() 
{
	CDiskInfo		DiskDlg;
	int				nSel;

	nSel = m_DelDiskList.GetCurSel ();
	nSel = m_DelDiskList.GetItemData (nSel);
	
	DiskDlg.m_btDisk = (BYTE)nSel;
	DiskDlg.DoModal ();
}

void CDelPar::OnDmOptions() 
{
	CDelOptions		DelOptDlg;

	DelOptDlg.DoModal();
	
}
