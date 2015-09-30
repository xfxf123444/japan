// FolderForm.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "FolderForm.h"
#include "Fun.h"

#define	IMG_NORMAL	2

extern CListCtrl *g_pListView;
extern CString g_strCurPath;

extern WCHAR		g_szDir[MAX_PATH][MAX_PATH];
extern int			g_nSelItem;

/////////////////////////////////////////////////////////////////////////////
// CFolderForm dialog

CFolderForm::CFolderForm(CWnd* pParent /*=NULL*/)
	: CDialog(CFolderForm::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFolderForm)
	//}}AFX_DATA_INIT
}


void CFolderForm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFolderForm)
	DDX_Control(pDX, IDC_TREE, m_Tree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFolderForm, CDialog)
	//{{AFX_MSG_MAP(CFolderForm)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE, OnItemexpandingTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFolderForm message handlers

BOOL CFolderForm::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CBitmap		Bitmap;

	Bitmap.LoadBitmap(IDB_TREE);
	m_ImageList.Create(16,16,ILC_COLOR16,0,10);
	m_ImageList.Add(&Bitmap, RGB(0,0,0));
	m_Tree.SetImageList(&m_ImageList, TVSIL_NORMAL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFolderForm::OnItemexpandingTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pView = (NM_TREEVIEW*)pNMHDR;
	CString	csDir;
	DWORD dwData;

	dwData = m_Tree.GetItemData(pView->itemNew.hItem);
	if(dwData == 0)//the first expand
	{
		m_Tree.SetItemData(pView->itemNew.hItem, 1);
		csDir = ConvertHTREEToPath(pView->itemNew.hItem, &m_Tree);
		CreateFolder(csDir, &m_Tree, pView->itemNew.hItem);
	}
	
	*pResult = 0;
}

void CFolderForm::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HANDLE hFind;
	WCHAR  szPath[MAX_PATH];
	int    nItem;
	WIN32_FIND_DATAW FileInfo = {0};
	CString strText;
	CTime  FileTime;

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	if (!m_bInitlizing)
	{
		g_strCurPath = ConvertHTREEToPath(pNMTreeView->itemNew.hItem, &m_Tree);

		g_nSelItem = 1;
		swprintf(g_szDir[0],L"%s",g_strCurPath.GetBuffer(0));

		if (g_pListView)
		{
			g_pListView->DeleteAllItems();

			wcscpy(szPath,g_strCurPath.GetBuffer(0));
			wcscat(szPath,L"\\*.*");

			hFind = FindFirstFileW(szPath,&FileInfo);
			if (hFind != INVALID_HANDLE_VALUE)
			{
				do
				{
					if (wcscmp(FileInfo.cFileName,L".") &&
						wcscmp(FileInfo.cFileName,L".."))
					{
						if (FileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
						{
							nItem = g_pListView->InsertItem(0,FileInfo.cFileName,2);
							g_pListView->SetItemData(nItem,g_pListView->GetItemCount()+0x10000);
						}
						else
						{
							nItem = g_pListView->InsertItem(0,FileInfo.cFileName,10);
							g_pListView->SetItemData(nItem,g_pListView->GetItemCount());
						}
						g_pListView->SetItemText(nItem,1,g_strCurPath.GetBuffer(0));
						FileTime = CTime(FileInfo.ftLastWriteTime);
						strText.Format(L"%d-%d-%d %d:%d",FileTime.GetYear(),FileTime.GetMonth(),FileTime.GetDay(),FileTime.GetHour(),FileTime.GetMinute());
						g_pListView->SetItemText(nItem,2,strText);
					}
				}
				while (FindNextFileW(hFind,&FileInfo));
				FindClose(hFind);
				g_pListView->SortItems(FileListCompare,(DWORD)TRUE);
			}
		}	
	}
	*pResult = 0;
}

BOOL CFolderForm::CreateFolder(CString csDir, CTreeCtrl* pTree, HTREEITEM hPar, BOOL bIsSecond)
{
	WIN32_FIND_DATAW dirData;
	HANDLE	hDir;
	CString	csPath = csDir + "\\*.*";
	HTREEITEM	hSon;
	TVITEM	item;
	item.mask = TVIF_CHILDREN;
	item.cChildren = 1;
	
	hDir = FindFirstFile(csPath, &dirData);
	if(hDir == INVALID_HANDLE_VALUE)	return FALSE;
	do
	{
		if(wcscmp(dirData.cFileName,L".") && wcscmp(dirData.cFileName,L".."))
		{
			if(dirData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)//directory
			{
				if (bIsSecond)
				{
					FindClose(hDir);
					return TRUE;
				}

				hSon = pTree->InsertItem(dirData.cFileName, IMG_NORMAL, IMG_NORMAL + 1, hPar);
				csPath = csDir + "\\";
				csPath += dirData.cFileName;
				if(CreateFolder(csPath, pTree, hSon, TRUE))
				{
					item.hItem = hSon;
					pTree->SetItem(&item);
					pTree->SetItemData(hSon, 0);
				}
			}
		}
	}while(FindNextFile(hDir, &dirData) != 0);
	FindClose(hDir);

	return FALSE;
}

CString CFolderForm::ConvertHTREEToPath(HTREEITEM hTree,CTreeCtrl* pTree)
{
	CString		csNow("");
	HTREEITEM hTree1;
	if(hTree == NULL)	return "";
	do
	{
		hTree1=hTree;
		if(pTree->GetParentItem(hTree1)==NULL)
			csNow = pTree->GetItemText(hTree).Left(2)+ csNow;
		else
			csNow = pTree->GetItemText(hTree) + csNow;
		csNow = "\\" + csNow;
	}
	while((hTree = pTree->GetParentItem(hTree)) != NULL);
	csNow.Delete(0);
	return csNow;
}

LRESULT CFolderForm::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch(message)  
	{  
	case  WM_REFRESH_ITEM: 
		InitTreeView();
		break;  
	case WM_UPDATE_VIEW:
		UpdateFolderView();
		break;
	}  
	return CDialog::WindowProc(message, wParam, lParam);
}

void CFolderForm::OnSize(UINT nType, int cx, int cy) 
{
	RECT RectWin;
	CDialog::OnSize(nType, cx, cy);
	
	if (IsWindow(m_Tree.m_hWnd))
	{
		GetClientRect(&RectWin);
		RectWin.top += 10;
		RectWin.left += 5;
		RectWin.bottom -= 8;
		RectWin.right -= 5;
		m_Tree.MoveWindow(&RectWin);
	}
	// TODO: Add your message handler code here
	
}

void CFolderForm::InitTreeView()
{
	HTREEITEM	hPar;
	DWORD       dwLen;
	DWORD		i;
	int			nType;
	WCHAR		*p;
	WCHAR		szLetter[4];
	WCHAR		szDrivesLetter[MAX_PATH];

	m_bInitlizing = TRUE;
	m_Tree.DeleteAllItems();
	g_pListView->DeleteAllItems();

	dwLen = GetLogicalDriveStrings(0,NULL);
	::GetLogicalDriveStrings(dwLen,szDrivesLetter);
	_wcsupr( szDrivesLetter );
	p = szDrivesLetter;

	dwLen = dwLen/4;
	for(i = 0;i<dwLen;i++)
	{
		wcscpy(szLetter,p);
		nType = GetLetterType(szLetter);
		szLetter[2] = 0;
		if (nType < 7)
		{
			hPar = m_Tree.InsertItem(szLetter, nType, nType);
			m_Tree.SetItemData(hPar, 1);
			CreateFolder(szLetter, &m_Tree, hPar);
			m_Tree.SortChildren(hPar);
		}
		p += 4;
	}
	m_bInitlizing = FALSE;
}

void CFolderForm::UpdateFolderView()
{
	HTREEITEM hTree,hChild;
	hTree = m_Tree.GetSelectedItem();
	if (GetFileAttributes(g_strCurPath.GetBuffer(0)) != -1)
	{
		do
		{
			hChild = m_Tree.GetChildItem(hTree);
			if (hChild) m_Tree.DeleteItem(hChild);
		}while (hChild);

		CreateFolder(g_strCurPath.GetBuffer(0),&m_Tree,hTree,FALSE);
		m_Tree.SortChildren(hTree);
	}
	else m_Tree.DeleteItem(hTree);
}

void CFolderForm::OnOK() 
{
	return;	
}

