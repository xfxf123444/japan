// WipeFree.cpp : implementation file
//

#include "stdafx.h"
#include "DeleteMaster.h"
#include "WipeFree.h"
#include "Fun.h"
#include "WipeProc.h"

extern	HWND					g_hMain;
extern	CImageList				g_imgList_BOX;
extern  YG_PARTITION_INFO*		g_pFixDiskInfo;
extern	int						g_nWipeMothed;
extern	YG_PARTITION_INFO*		g_pTargetParInfo ;
extern  BOOL                    g_bBeepOff;

/////////////////////////////////////////////////////////////////////////////
// CWipeFree property page

IMPLEMENT_DYNCREATE(CWipeFree, CPropertyPage)

CWipeFree::CWipeFree() : CPropertyPage(CWipeFree::IDD)
{
	//{{AFX_DATA_INIT(CWipeFree)
	//}}AFX_DATA_INIT
}

CWipeFree::~CWipeFree()
{
}

void CWipeFree::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWipeFree)
	DDX_Control(pDX, IDC_BEEP_OFF, m_BeepOff);
	//DDX_Control(pDX, IDC_CHECK_SHREDDER, m_CheckShredder);
	DDX_Control(pDX, IDC_WIPE_MOTHED, m_SelMothed);
	DDX_Control(pDX, IDC_WIPE_DISK_COMBO, m_WipeDiskList);
	DDX_Control(pDX, IDC_WIPE_CLEAR, m_ButtonClear);
	DDX_Control(pDX, IDC_DM_WIPE, m_ButtonWipe);
	DDX_Control(pDX, IDC_LABEL_MOTHED, m_LabelMothed);
	DDX_Control(pDX, IDC_WIPE_PAR_LIST, m_WipeParList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWipeFree, CPropertyPage)
	//{{AFX_MSG_MAP(CWipeFree)
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_WIPE_DISK_COMBO, OnSelchangeWipeDiskCombo)
	ON_NOTIFY(NM_CLICK, IDC_WIPE_PAR_LIST, OnClickWipeParList)
	ON_BN_CLICKED(IDC_DM_WIPE, OnDmWipe)
	ON_BN_CLICKED(IDC_WIPE_CLEAR, OnWipeClear)
	//ON_BN_CLICKED(IDC_CHECK_SHREDDER, OnCheckShredder)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWipeFree message handlers

void CWipeFree::OnOK() 
{
	::PostMessage(g_hMain,WM_CLOSE,0,0);
	CPropertyPage::OnOK();
}

void CWipeFree::OnSize(UINT nType, int cx, int cy) 
{
	CRect		rect;
	int			nWidth,nButtonHigh,nDelAndCle;
	int			nLabelR;
	CPropertyPage::OnSize(nType, cx, cy);
	
	if(m_WipeParList)
	{
		m_WipeParList.GetWindowRect (&rect);
		ScreenToClient (&rect);
		rect.right = cx - rect.left;
		nWidth	   = rect.left;
		m_WipeParList.MoveWindow (rect);
		ResizeListColumn(m_WipeParList.m_hWnd);
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
	
	if(m_ButtonWipe)
	{
		nDelAndCle = 3*rect.Width ()/2;
		rect.left  -= nDelAndCle;
		rect.right -= nDelAndCle;
		m_ButtonWipe.MoveWindow (rect);
		nButtonHigh = rect.Height ();
	}

	if(m_BeepOff)
	{
		rect.left  -= nDelAndCle;
		rect.right -= nDelAndCle;
		m_BeepOff.MoveWindow (rect);
	}

	if(m_WipeParList)
	{
		m_WipeParList.GetWindowRect (&rect);
		ScreenToClient (&rect);
		rect.bottom = cy - nWidth - 2*nButtonHigh;
		m_WipeParList.MoveWindow (rect);
	}
	
}

BOOL CWipeFree::OnInitDialog() 
{
	HANDLE				hFind;
    WIN32_FIND_DATA     FindData;
	CRect				rect;
    HKEY				hCU;
    DWORD				lpType;
    ULONG				ulSize = MAX_PATH;
	CString				csShredder;
	CPropertyPage::OnInitDialog();


    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
      _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"), 
      0,KEY_QUERY_VALUE,
      &hCU) == ERROR_SUCCESS)
    {
      RegQueryValueEx( hCU,
        _T("Common Desktop"),
        NULL,
        &lpType,
        (LPBYTE)&m_szDesktop,
        &ulSize);

      RegCloseKey(hCU);
    }
	_tcscpy(m_szLink,m_szDesktop);
	csShredder.LoadString (IDS_SHREDDER_LINK);
	_tcscat(m_szLink,csShredder.GetBuffer (0));
	hFind = FindFirstFile(m_szLink,&FindData);
	if (INVALID_HANDLE_VALUE != hFind)
	{
		m_CheckShredder.SetCheck(1);
	}
	FindClose(hFind);
	GetCurrentDirectory(MAX_PATH, m_szCurDir);
	_tcscpy(m_szCurFile,m_szCurDir);
	_tcscat(m_szCurFile,_T("\\DelFile.exe"));

	m_WipeDiskList.SetImageList(&g_imgList_BOX);
	AddFixDisk(&m_WipeDiskList,FALSE);
	m_WipeDiskList.SetCurSel(0);
	
	AddDelMothed(&m_SelMothed);

	m_WipeParList.GetWindowRect(&rect);
	ScreenToClient(rect);
	InitList(m_WipeParList.m_hWnd,rect);
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CWipeFree::OnSetActive() 
{
	OnSelchangeWipeDiskCombo();
	
	return CPropertyPage::OnSetActive();
}

void CWipeFree::OnSelchangeWipeDiskCombo() 
{
	int		nSel;

	nSel = m_WipeDiskList.GetCurSel ();
	nSel = m_WipeDiskList.GetItemData (nSel);
	m_WipeParList.DeleteAllItems();

	AddList(&m_WipeParList, nSel+DISK_BASE);
}

void CWipeFree::OnClickWipeParList(NMHDR* pNMHDR, LRESULT* pResult) 
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
		pTargetParInfo = (YG_PARTITION_INFO*)m_WipeParList.GetItemData (pItem->iItem);

		lvitem.iItem = pItem->iItem;
		plist->GetItem(&lvitem);
		lvitem.iImage = (lvitem.iImage+1) % 2;
		plist->SetItem(&lvitem);
	}	
	*pResult = 0;
}

void CWipeFree::OnDmWipe() 
{
	LVITEM				lvitem;
	int					nSel;
	int					i;
	YG_PARTITION_INFO	*pTargetParInfo = NULL;
	CWipeProc			WipeProcDlg;
	BOOL				bSel = FALSE;
	CString				cstr,csCaption;
	TCHAR				szCurDir[MAX_PATH];
	TCHAR				szDir[32];

	UpdateData(TRUE);
	g_bBeepOff = m_BeepOff.GetCheck();

	nSel = m_SelMothed.GetCurSel ();
	g_nWipeMothed = nSel;

	csCaption.LoadString (IDS_DM_ERROR);

	lvitem.mask = LVIF_IMAGE;
	lvitem.iSubItem = 0;
	
	for(i = 0; i < m_WipeParList.GetItemCount (); i++) 
	{
		lvitem.iItem = i;
		m_WipeParList.GetItem(&lvitem);
		if(lvitem.iImage == IMG_LIST_SELECT) 
		{
			pTargetParInfo		= (YG_PARTITION_INFO*)m_WipeParList.GetItemData (i);
			g_pTargetParInfo	= pTargetParInfo;
			bSel				= TRUE;
			break;
		}
	}
	if(bSel)
	{
		if(g_pTargetParInfo->DriveLetter == _T('*'))
		{
			cstr.LoadString (IDS_Q_FORMAT_PARTITION);
			csCaption.LoadString (IDS_DM_INFO);
			MessageBox(cstr,csCaption,MB_OK);
			return ;
		}
		ZeroMemory(szDir, 32  * sizeof(TCHAR));
		_tcscpy(szDir,_T("C:\\"));
		szDir[0] = g_pTargetParInfo->DriveLetter;
		GetCurrentDirectory(MAX_PATH,szCurDir);
		if(!SetCurrentDirectory(szDir))
		{
			cstr.LoadString (IDS_Q_FORMAT_PARTITION);
			csCaption.LoadString (IDS_DM_INFO);
			MessageBox(cstr,csCaption,MB_OK);
			return ;
		}
		SetCurrentDirectory(szCurDir);
		cstr.Format (IDS_CONFIRM_WIPE_FREE,g_pTargetParInfo->DriveLetter);
		csCaption.LoadString (IDS_DM_CONFIRM_CAPTITION);
		if(IDYES == MessageBox (cstr,csCaption,MB_YESNO))
		{
			WipeProcDlg.m_DriveLetter = g_pTargetParInfo->DriveLetter;
			if (!g_bBeepOff) Beep(1000,100);
			WipeProcDlg.DoModal ();
		}

	}
}

void CWipeFree::OnWipeClear() 
{
	int					i,nItemCount;
	LVITEM				lvitem;

	lvitem.mask		= LVIF_IMAGE;
	lvitem.iSubItem = 0;

	nItemCount = m_WipeParList.GetItemCount ();
	for(i=0;i<nItemCount;i++)
	{
		lvitem.iItem = i;
		lvitem.iImage= 0;
		m_WipeParList.SetItem (&lvitem);
	}
}

void CWipeFree::OnCheckShredder() 
{
	//if(m_CheckShredder.GetCheck())
	//{
	//	CreateIt(m_szCurFile, m_szLink);
	//}
	//else
	//{
	//	DeleteFile(m_szLink);
	//}
	
}

HRESULT CWipeFree::CreateIt(LPCTSTR pszShortcutFile, LPTSTR pszLink)
{
    HRESULT		hres;
    IShellLink* psl;

    // Get a pointer to the IShellLink interface.
	CoInitialize(NULL);
    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                            IID_IShellLink, (VOID**)(&psl));
    if (SUCCEEDED(hres))
    {
       IPersistFile* ppf;

       // Query IShellLink for the IPersistFile interface for 
       // saving the shell link in persistent storage.
       hres = psl->QueryInterface(IID_IPersistFile, (VOID **)&ppf);
       if (SUCCEEDED(hres))
       {   
         //WORD wsz[MAX_PATH];

         // Set the path to the shell link target.
         hres = psl->SetPath(pszShortcutFile);

         if (!SUCCEEDED(hres))
           AfxMessageBox(_T("SetPath failed!"));

         // Set the description of the shell link.
		 hres = psl->SetWorkingDirectory(m_szCurDir);

         if (!SUCCEEDED(hres))
           AfxMessageBox(_T("SetDescription failed!"));

         //// Ensure string is ANSI.
         //MultiByteToWideChar(CP_ACP, 0, pszLink, -1, wsz, MAX_PATH);

         // Save the link via the IPersistFile::Save method.
         //hres = ppf->Save(wsz, TRUE);
		 hres = ppf->Save(pszLink, TRUE);

         // Release pointer to IPersistFile.
         ppf->Release();
       }
       // Release pointer to IShellLink.
       psl->Release();
    }
	CoUninitialize();
    return hres;
}

