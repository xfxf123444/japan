// AMFindView.cpp : implementation of the CAMFindView class
//

#include "stdafx.h"
#include "AMFind.h"

#include "AMFindDoc.h"
#include "AMFindView.h"
#include "SearchForm.h"

#include "..\AM01Expt\AM01Expt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern WCHAR g_szImgFile[MAX_PATH];

extern WCHAR g_szTempDir[MAX_PATH];

extern CSearchForm *g_pForm;

BOOL FileCanWrite(WCHAR *szFile);
/////////////////////////////////////////////////////////////////////////////
// CAMFindView

IMPLEMENT_DYNCREATE(CAMFindView, CListView)

BEGIN_MESSAGE_MAP(CAMFindView, CListView)
	//{{AFX_MSG_MAP(CAMFindView)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_RESTORE,OnRestore)
	ON_COMMAND(ID_RESTORE_TO,OnRestoreTo)
	ON_COMMAND(ID_FILE_OPEN, OnMyFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAMFindView construction/destruction

CAMFindView::CAMFindView()
{
	m_pImageList = NULL;
	// TODO: add construction code here

}

CAMFindView::~CAMFindView()
{
}

BOOL CAMFindView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style = (cs.style & ~LVS_TYPEMASK) | LVS_REPORT;
	cs.style |= LVS_AUTOARRANGE;
	cs.style &= ~LVS_SORTASCENDING;
	cs.style &= ~LVS_SORTDESCENDING;


	return CListView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CAMFindView drawing

void CAMFindView::OnInitialUpdate()
{
	RECT  ListRect;
	int   iCol;
	CString strHeading;
	CListView::OnInitialUpdate();

	CListCtrl& listView = GetListCtrl();
	GetClientRect(&ListRect);
	for (iCol = 0; iCol < 4; iCol++) 
	{
		strHeading.LoadString( IDS_FILE_NAME + iCol );
		listView.InsertColumn( iCol, strHeading, LVCFMT_CENTER,
			ListRect.right/4, iCol );
	}

	m_pImageList = new CImageList;
	m_pImageList->Create(IDB_STATUS_ICON,16,0,RGB(255,0,0));
	listView.SetImageList(m_pImageList,LVSIL_STATE);
	
	// TODO: You may populate your ListView with items by directly accessing
	//  its list control through a call to GetListCtrl().
}

/////////////////////////////////////////////////////////////////////////////
// CAMFindView printing

BOOL CAMFindView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CAMFindView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CAMFindView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CAMFindView diagnostics

#ifdef _DEBUG
void CAMFindView::AssertValid() const
{
	CListView::AssertValid();
}

void CAMFindView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CAMFindDoc* CAMFindView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAMFindDoc)));
	return (CAMFindDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAMFindView message handlers
void CAMFindView::OnSize(UINT nType, int cx, int cy) 
{
	RECT  ListRect;
	int   iCol;
	CListView::OnSize(nType, cx, cy);
	
	CListCtrl& listView = GetListCtrl();
	GetClientRect(&ListRect);
	for (iCol = 0; iCol < 4; iCol++) 
		listView.SetColumnWidth(iCol,ListRect.right/4);
	// TODO: Add your message handler code here
	
}

void CAMFindView::OnDestroy() 
{
	CListView::OnDestroy();

	if (NULL != m_pImageList)
	{
		delete m_pImageList;
		m_pImageList = NULL;
	}
	// TODO: Add your message handler code here
	
}

void CAMFindView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CListView::OnRButtonDown(nFlags, point);
	CMenu menu;
	CListCtrl& listView = GetListCtrl();
	if (listView.GetSelectedCount() > 0)
	{
		VERIFY(menu.LoadMenu(IDR_RESTORE));
		CMenu* pPopup = menu.GetSubMenu ( 0 ) ;
		ASSERT(pPopup != NULL);
		POINT pt ;
		GetCursorPos ( &pt ) ;
	    SetForegroundWindow ();
		pPopup->TrackPopupMenu ( TPM_LEFTALIGN | TPM_RIGHTBUTTON,
								  pt.x, pt.y, AfxGetMainWnd() );

	}
}

void CAMFindView::OnMyFileOpen()
{
	::PostMessage(g_pForm->m_hWnd,WM_COMMAND,ID_FILE_OPEN,0);
}

void CAMFindView::OnRestore()
{
	int nSelCount,nCount,nItem;
	DWORD dwPathID,dwTimeStamp;
	WCHAR szFile[MAX_PATH],szPath[MAX_PATH];
	CListCtrl& listView = GetListCtrl();
	nSelCount = listView.GetSelectedCount();
	if (nSelCount > 0)
	{
		nCount = listView.GetItemCount();
		for (nItem = 0;nItem < nCount;nItem ++)
		{
			if (listView.GetItemState(nItem,LVIS_SELECTED))
			{
				listView.GetItemText(nItem,0,szFile,MAX_PATH);
				listView.GetItemText(nItem,1,szPath,MAX_PATH);

				if( CreateOneDir(szPath) )
				{
					// if (szPath[wcslen(szPath)-1] != L'\\')
					// wcscat(szPath,"\\");
					TrimRightChar(szPath,L'\\',MAX_PATH);
					wcscat(szPath,L"\\");
					
					wcscat(szPath,szFile);
					dwPathID = listView.GetItemData(nItem);
					if (dwPathID)
					{
						dwTimeStamp = g_pForm->m_TimeStamp.GetItemData(g_pForm->m_TimeStamp.GetCurSel());
						if (FileCanWrite(szPath))
							
						  RestoreFindFile(g_szImgFile,g_szTempDir,dwPathID,dwTimeStamp,szPath);
					}
				}
				else AfxMessageBox(IDS_CREATE_DIR_FAIL,MB_OK | MB_ICONINFORMATION);
			}
		}
	}
	else AfxMessageBox(IDS_NO_FILE_SELECTED,MB_OK | MB_ICONINFORMATION);
}

void CAMFindView::OnRestoreTo()
{
	int nSelCount,nCount,nItem,nPathLen;
	DWORD dwPathID,dwTimeStamp;
	WCHAR szFile[MAX_PATH],szPath[MAX_PATH];
	CListCtrl& listView = GetListCtrl();
	nSelCount = listView.GetSelectedCount();
	if (nSelCount > 0)
	{
		if (SelectFolder(m_hWnd,szPath))
		{
			if ( CreateOneDir(szPath))
			{
				nPathLen = wcslen(szPath);
				nCount = listView.GetItemCount();
				for (nItem = 0;nItem < nCount;nItem ++)
				{
					if (listView.GetItemState(nItem,LVIS_SELECTED))
					{
						listView.GetItemText(nItem,0,szFile,MAX_PATH);
						
						// if (szPath[wcslen(szPath)-1] != L'\\')
						// wcscat(szPath,"\\");
						TrimRightChar(szPath,L'\\',MAX_PATH);
						wcscat(szPath,L"\\");
						wcscat(szPath,szFile);
						dwPathID = listView.GetItemData(nItem);
						if (dwPathID)
						{
							dwTimeStamp = g_pForm->m_TimeStamp.GetItemData(g_pForm->m_TimeStamp.GetCurSel());
							if (FileCanWrite(szPath))
								RestoreFindFile(g_szImgFile,g_szTempDir,dwPathID,dwTimeStamp,szPath);

						}
						szPath[nPathLen] = L'\0';
					}
				}
			}
			else AfxMessageBox(IDS_CREATE_DIR_FAIL,MB_OK | MB_ICONINFORMATION);
		}
	}
	else AfxMessageBox(IDS_NO_FILE_SELECTED,MB_OK | MB_ICONINFORMATION);
}

BOOL FileCanWrite(WCHAR *szFile)
{
	CString strText;
	DWORD   dwFileAttr;
	dwFileAttr = GetFileAttributes(szFile);
	if (dwFileAttr != -1)
	{
		if (dwFileAttr & FILE_ATTRIBUTE_DIRECTORY)
		{
			strText.Format(IDS_CREATE_FILE_FAIL,szFile);
			AfxMessageBox(strText,MB_OK | MB_ICONINFORMATION);
			return FALSE;
		}
		strText.Format(IDS_OVER_WRITE,szFile);
		if (AfxMessageBox(strText,MB_YESNO | MB_ICONQUESTION) != IDYES)
			return FALSE;
	}
	return TRUE;
}

