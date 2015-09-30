// Folder.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "Folder.h"
#include "Fun.h"

/////////////////////////////////////////////////////////////////////////////
// CFolder dialog

CFolder::CFolder(CWnd* pParent /*=NULL*/)
	: CDialog(CFolder::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFolder)
	m_csName = _T("");
	//}}AFX_DATA_INIT
	m_bShowFile = TRUE;
}


void CFolder::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFolder)
	DDX_Control(pDX, IDC_TREE, m_Tree);
	DDX_Text(pDX, IDC_FILE, m_csName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFolder, CDialog)
	//{{AFX_MSG_MAP(CFolder)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE, OnItemexpandingTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFolder message handlers

BOOL CFolder::OnInitDialog() 
{
	CDialog::OnInitDialog();

	HTREEITEM	hPar;
	CString		csTitle;
	DWORD       dwLen;
	DWORD		i;
	int			nType;
	WCHAR		*p;
	WCHAR		szLetter[4];
	WCHAR		szDrivesLetter[MAX_PATH];
	CBitmap     Bitmap;
	
	if (m_bShowFile)
	{
		csTitle.LoadString(IDS_SELECT_FILE_OR_DIR);
		SetWindowText(csTitle);
	}
	Bitmap.LoadBitmap(IDB_TREE);
	m_ImageList.Create(16,16,ILC_COLOR16,0,10);
	m_ImageList.Add(&Bitmap, RGB(0,0,0));
	m_Tree.SetImageList(&m_ImageList, TVSIL_NORMAL);

	dwLen = GetLogicalDriveStrings(0,NULL);
	::GetLogicalDriveStrings(dwLen,szDrivesLetter);
	_wcsupr( szDrivesLetter );
	p = szDrivesLetter;

	dwLen = dwLen/4;
	for(i = 0;i<dwLen;i++)
	{
		wcscpy(szLetter,p);
		//szLetter[2] = 0;
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

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFolder::OnItemexpandingTree(NMHDR* pNMHDR, LRESULT* pResult) 
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

void CFolder::OnOK() 
{
	HTREEITEM	hSel;
	CString		csText,csCaption;

	csCaption.LoadString (IDS_PRODUCT_NAME);
	hSel = m_Tree.GetSelectedItem();
	m_csFileName = ConvertHTREEToPath(hSel, &m_Tree);
	if(GetFileAttributes(m_csFileName) == -1)
	{
		csText.LoadString (IDS_DEVICE_NOT_READY);
		MessageBox(csText,csCaption,MB_OK|MB_ICONSTOP);
		return ;
	}

	CDialog::OnOK();
}

void CFolder::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	m_csName = ConvertHTREEToPath(pNMTreeView->itemNew.hItem, &m_Tree);
	UpdateData(FALSE);
	 
	*pResult = 0;
}

BOOL CFolder::CreateFolder(CString csDir, CTreeCtrl* pTree, HTREEITEM hPar, BOOL bIsSecond)
{
	WIN32_FIND_DATA dirData;
	HANDLE	hDir;
	CString	csPath = csDir + L"\\*.*";
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
				
				csPath = csDir + L"\\";
				csPath += dirData.cFileName;
				if(CreateFolder(csPath, pTree, hSon, TRUE))
				{
					item.hItem = hSon;
					pTree->SetItem(&item);
					pTree->SetItemData(hSon, 0);
				}
			}
			else
			{
				if (!bIsSecond && m_bShowFile)
					hSon = pTree->InsertItem(dirData.cFileName, 10,10, hPar);
			}
		}
	}while(FindNextFile(hDir, &dirData) != 0);
	FindClose(hDir);

	return FALSE;
}

CString CFolder::ConvertHTREEToPath(HTREEITEM hTree,CTreeCtrl* pTree)
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


