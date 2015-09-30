// MainDlg.cpp : implementation file
//
#include "stdafx.h"
#include "DelFile.h"
#include "MainDlg.h"
#include "Fun.h"
#include "PasswordDlg.h"

extern WCHAR		g_szDir[MAX_PATH][MAX_PATH];
extern int			g_nSelItem;
extern CString g_strCurPath;

CSCHEDULE_ITEM_ARRAY g_ScheduleItemArray;
CListCtrl *g_pListView = NULL;
/////////////////////////////////////////////////////////////////////////////
// CMainDlg dialog

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMainDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMainDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pStep1 = NULL;
	m_pStep2 = NULL;
	m_pStep3 = NULL;
	m_bRectInitlized = FALSE;
	m_bVerified = FALSE;
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMainDlg)
	DDX_Control(pDX, IDC_FILE_LIST, m_FileList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMainDlg, CDialog)
	//{{AFX_MSG_MAP(CMainDlg)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_COMMAND(ID_SEARCH_VIEW, OnSearchView)
	ON_COMMAND(ID_FOLDER_VIEW, OnFolderView)
	ON_COMMAND(ID_EXIT, OnExit)
	ON_COMMAND(ID_SEC_DELETE, OnSecDelete)
	ON_COMMAND(ID_SCHEDULE_EDIT, OnScheduleEdit)
	ON_NOTIFY(NM_CLICK, IDC_FILE_LIST, OnClickFileList)
	ON_WM_SIZE()
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_FILE_LIST, OnColumnclickFileList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainDlg message handlers

BOOL CMainDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowVersion();
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CalcRect();
	SetStatusBar();
	InitPage();	
	g_pListView = &m_FileList;
	if (m_bIsWin9x)
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_SCHEDULE_EDIT,FALSE); 
		EnableMenu(2,FALSE);
	}
	PostMessage(WM_COMMAND,ID_FOLDER_VIEW,0);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMainDlg::CalcRect()
{
	RECT   ListRect;

	int nXEdge = ::GetSystemMetrics(SM_CXEDGE);  
	
	GetClientRect(&ListRect);

	m_FileList.GetClientRect(&ListRect);
	m_FileList.ClientToScreen(&ListRect);
	ScreenToClient(&ListRect);

	m_rectPage.top = ListRect.top - 10;
	m_rectPage.bottom = ListRect.bottom + 10;
	m_rectPage.left = 10; 
	m_rectPage.right = ListRect.left - 10;
	m_bRectInitlized = TRUE;
}


void CMainDlg::InitPage()
{
	CBitmap     Bitmap;
	CString		csHeading;
	LV_COLUMN	Column;
	CRect		ListRect;

	m_pPageLink=NULL;
	m_nPageCount=0;
	m_nCurrentPage=0;
	
	m_pStep1= new CFolderForm;
	m_pStep2= new CSearchForm;
	m_pStep3= new CScheduleEdit;
	
	AddPage(m_pStep1,IDD_FOLDER_FORM);
	AddPage(m_pStep2,IDD_SEARCH_FORM);
	AddPage(m_pStep3,IDD_SCHEDULE_EDIT);


	m_FileList.GetClientRect(ListRect);

	Column.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
	Column.fmt = LVCFMT_LEFT;
	Column.cx  = (ListRect.right-ListRect.left)/3;
	Column.cchTextMax = 0;		// ignored
	Column.iSubItem = 0;		
	
	csHeading.LoadString(IDS_FILE_NAME);
	Column.pszText = csHeading.GetBuffer(0);
	m_FileList.InsertColumn(0,&Column);

	csHeading.LoadString(IDS_DIRECTORY);
	Column.pszText = csHeading.GetBuffer(0);
	m_FileList.InsertColumn(1,&Column);

	csHeading.LoadString(IDS_LAST_MODIFY);
	Column.pszText = csHeading.GetBuffer(0);
	m_FileList.InsertColumn(2,&Column);

	Bitmap.LoadBitmap(IDB_TREE);
	m_ImageList.Create(16,16,ILC_COLOR16,0,10);
	m_ImageList.Add(&Bitmap, RGB(0,0,0));
	m_FileList.SetImageList(&m_ImageList,LVSIL_SMALL);
	m_FileList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
}

void CMainDlg::SetStatusBar()
{
	if (!m_wndToolBar.CreateEx(this) ||	!m_wndToolBar.LoadToolBar(IDR_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return;      // fail to create
	}
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);
}

void CMainDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
       // **************************************************************
	}
}

void CMainDlg::AddPage(CDialog *pDialog, UINT nID)
{
	RECT   RectDlg;
	struct PAGELINK* pTemp = m_pPageLink;
	struct PAGELINK* pNewPage = new PAGELINK;
	pNewPage->pDialog = pDialog;
	pNewPage->pDialog->Create(nID,this);
	
	ASSERT(::IsWindow(pNewPage->pDialog->m_hWnd));
	DWORD dwStyle = pNewPage->pDialog->GetStyle();
	ASSERT((dwStyle & WS_CHILD) != 0);
	ASSERT((dwStyle & WS_BORDER) == 0);
	pNewPage->pDialog->ShowWindow(SW_HIDE);
	if (nID == IDD_SCHEDULE_EDIT)
	{
		GetClientRect(&RectDlg);
		RectDlg.right = m_rectPage.left + (RectDlg.right - 20);
		RectDlg.left = m_rectPage.left;
		RectDlg.top = m_rectPage.top;
		RectDlg.bottom = m_rectPage.bottom;
		pNewPage->pDialog->MoveWindow(&RectDlg);
	}
	else
		pNewPage->pDialog->MoveWindow(m_rectPage);
	pNewPage->Next=NULL; 
	pNewPage->nNum=++m_nPageCount;
	if (pTemp)
	{
		while (pTemp->Next) pTemp=pTemp->Next;
		pTemp->Next=pNewPage; 
	}
	else m_pPageLink=pNewPage;
}

void CMainDlg::OnDestroy() 
{
	CDialog::OnDestroy();

	g_ScheduleItemArray.RemoveAll();

	delete m_pStep1;
	delete m_pStep2;
	delete m_pStep3;

	struct PAGELINK* pTemp=m_pPageLink; 
	struct PAGELINK* pNext=m_pPageLink; 

	while(pTemp) 
	{
		pNext = pTemp->Next;

		delete pTemp;
		pTemp=pNext; 
	} 
}

void CMainDlg::ShowPage(UINT nPos)
{
	struct PAGELINK* pTemp=m_pPageLink; 
	while(pTemp) 
	{ 
		if(pTemp->nNum==nPos) 
		{
			pTemp->pDialog->ShowWindow(SW_SHOW); 
		} 
		else 
			pTemp->pDialog->ShowWindow(SW_HIDE); 
		
		pTemp=pTemp->Next; 
		
	} 
	if (nPos>=m_nPageCount)  // the last page
	{ 
		m_nCurrentPage=m_nPageCount; 
		
		return; 
	} 
	if (nPos<=1) // the first page
	{ 
		m_nCurrentPage=1; 
		
		return; 
	} 
}

void CMainDlg::OnSearchView() 
{
	g_nSelItem = 0;
	ShowPage(2);
	m_FileList.ShowWindow(SW_SHOW);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_SEC_DELETE,TRUE); 
	EnableMenu(1,TRUE);
	::SendMessage(m_pStep2->m_hWnd,WM_REFRESH_ITEM,0,0);
}

void CMainDlg::OnScheduleEdit()
{
	CPasswordDlg PasswordDlg;

	if (!m_bVerified)
	{
		PasswordDlg.m_bSetPassword = FALSE;
		if (IDOK != PasswordDlg.DoModal())
			return;
		m_bVerified = TRUE;
	}
	g_nSelItem = 0;
	ShowPage(3);
	m_FileList.ShowWindow(SW_HIDE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_SEC_DELETE,FALSE); 
	EnableMenu(1,FALSE);
	::SendMessage(m_pStep3->m_hWnd,WM_REFRESH_ITEM,0,0);
}

void CMainDlg::OnSecDelete() 
{
	int nItem,nTotal,nLen;
	WCHAR szFile[MAX_PATH];
	BOOL bDelFolderView = FALSE;

	if (!g_nSelItem)
	{
		nTotal = m_FileList.GetSelectedCount();
		if (!nTotal)
		{
			AfxMessageBox(IDS_NO_ITEM,MB_OK | MB_ICONINFORMATION);
			return;
		}
		
		nTotal = m_FileList.GetItemCount();
		for (nItem = 0;nItem < nTotal;nItem++)
		{
			if (m_FileList.GetItemState(nItem,LVIS_SELECTED))
			{
				m_FileList.GetItemText(nItem,1,g_szDir[g_nSelItem],MAX_PATH);
				wcscat(g_szDir[g_nSelItem],L"\\");
				nLen = wcslen(g_szDir[g_nSelItem]);
				m_FileList.GetItemText(nItem,0,&g_szDir[g_nSelItem][nLen],MAX_PATH-nLen);
				g_nSelItem ++;
			}
		}
	}
	else bDelFolderView = TRUE;

	SecDeleteFile();

	g_nSelItem = 0;

	nTotal = m_FileList.GetItemCount();
	for (nItem = nTotal;nItem >= 0;nItem--)
	{
		if (m_FileList.GetItemState(nItem,LVIS_SELECTED))
		{
			m_FileList.GetItemText(nItem,1,szFile,MAX_PATH);
			wcscat(szFile,L"\\");
			nLen = wcslen(szFile);
			m_FileList.GetItemText(nItem,0,&szFile[nLen],MAX_PATH-nLen);
			if (GetFileAttributes(szFile) == -1)
				m_FileList.DeleteItem(nItem);
		}
	}
	if (m_nCurrentPage == 1) ::PostMessage(m_pStep1->m_hWnd,WM_UPDATE_VIEW,0,0);
	return;
}

void CMainDlg::OnFolderView() 
{
	g_nSelItem = 0;
	ShowPage(1);
	m_FileList.ShowWindow(SW_SHOW);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_SEC_DELETE,TRUE); 
	EnableMenu(1,TRUE);
	::SendMessage(m_pStep1->m_hWnd,WM_REFRESH_ITEM,0,0);
}

void CMainDlg::OnExit()
{
	PostMessage(WM_CLOSE,0,0);
}

void CMainDlg::OnClickFileList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if (m_FileList.GetSelectedCount())
		g_nSelItem = 0;

	*pResult = 0;
}

void CMainDlg::OnSize(UINT nType, int cx, int cy) 
{
	POINT  Point = {0};
	RECT   RectCtrl,RectDlg;
	if(nType  != SIZE_MINIMIZED)
	{
		if (m_bRectInitlized)
		{
			if (cx < m_nMinDlgWidth || cy < m_nMinDlgHigth)
			{
				cx = m_nMinDlgWidth;
				cy = m_nMinDlgHigth;
				ClientToScreen(&Point);
				MoveWindow(Point.x - 4,Point.y - 55,m_nMinDlgWidth + 8,m_nMinDlgHigth + 55);
			}
			GetClientRect(&RectDlg);

			if (IsWindow(m_FileList.m_hWnd))
			{
				RectCtrl.top = m_rectPage.top + 10;
				RectCtrl.left = m_rectPage.right + 10;
				RectCtrl.right = RectDlg.right - 10;
				RectCtrl.bottom = RectDlg.bottom - 20;
				m_FileList.MoveWindow(&RectCtrl);
				m_FileList.SetColumnWidth(0,(RectCtrl.right - RectCtrl.left)/3);
				m_FileList.SetColumnWidth(1,(RectCtrl.right - RectCtrl.left)/3);
				m_FileList.SetColumnWidth(2,(RectCtrl.right - RectCtrl.left)/3);
			}
			m_rectPage.bottom = RectDlg.bottom - 10;

			if (m_pStep1) m_pStep1->MoveWindow(m_rectPage);
			if (m_pStep2) m_pStep2->MoveWindow(m_rectPage);
			RectCtrl = m_rectPage;
			RectCtrl.right = RectCtrl.left + (RectDlg.right - 20);
			if (m_pStep3) m_pStep3->MoveWindow(&RectCtrl);
		}
		else
		{
			m_nMinDlgWidth = cx;
			m_nMinDlgHigth = cy;
		}
	}

	CDialog::OnSize(nType, cx, cy);
}

void CMainDlg::OnColumnclickFileList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	m_bAsc = !m_bAsc;
	m_FileList.SortItems(FileListCompare,(DWORD)m_bAsc);

	*pResult = 0;
}


void CMainDlg::EnableMenu(int nPos,BOOL bEnable)
{
	CMenu * pMenu = NULL;
	
	if (GetSafeHwnd())
		pMenu = GetMenu();
	if (pMenu)
	{
		pMenu->EnableMenuItem(nPos, MF_BYPOSITION |
			(bEnable ? MF_ENABLED : (MF_DISABLED | MF_GRAYED)));
		if (m_hWnd)
			DrawMenuBar();
	}
}

void CMainDlg::SetWindowVersion()
{
	OSVERSIONINFO VerInfo;  
	VerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&VerInfo);

	if (VerInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		m_bIsWin9x = TRUE;
	}
	else
	{
		m_bIsWin9x = FALSE;
	}
}

void CMainDlg::OnOK() 
{
	return;	
}



