// EncryptWiz_1.cpp : implementation file
//

#include "stdafx.h"
#include "fileencrypt.h"
#include "EncryptWiz_1.h"
#include "..\..\DavinciFunction\DavinciFunction.h"
#include "..\EncryptFunction\EncryptFunction.h"
#include "..\..\EncryptToolStruct\EncryptToolStruct.h"
#include "..\EncryptFunction\EncryptInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CEncryptInfo g_EncryptInfo;
BOOL g_bCreateSelfExtractFile;

/////////////////////////////////////////////////////////////////////////////
// CEncryptWiz_1 property page

IMPLEMENT_DYNCREATE(CEncryptWiz_1, CPropertyPage)

CEncryptWiz_1::CEncryptWiz_1() : CPropertyPage(CEncryptWiz_1::IDD)
{
	//{{AFX_DATA_INIT(CEncryptWiz_1)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CEncryptWiz_1::~CEncryptWiz_1()
{
}

void CEncryptWiz_1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEncryptWiz_1)
	DDX_Control(pDX, IDC_SOURCE, m_Source);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEncryptWiz_1, CPropertyPage)
	//{{AFX_MSG_MAP(CEncryptWiz_1)
	ON_BN_CLICKED(IDC_ADD_DIRECTORY, OnAddDirectory)
	ON_BN_CLICKED(IDC_ADD_FILE, OnAddFile)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_WM_DROPFILES()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEncryptWiz_1 message handlers

BOOL CEncryptWiz_1::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertySheet* pSheet = (CPropertySheet*)GetParent();
	ASSERT_KINDOF(CPropertySheet, pSheet);
	pSheet->SetWizardButtons(PSWIZB_NEXT);
	(pSheet->GetDlgItem(IDHELP))->ShowWindow(FALSE);
	
	return CPropertyPage::OnSetActive();
}

BOOL CEncryptWiz_1::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	int nWidth;
	CRect Rect;
	m_Source.GetClientRect(Rect);
	nWidth = Rect.Width();

	CString strSource;
	strSource.LoadString(IDS_SOURCE_FILE);
	m_Source.InsertColumn(0,(LPCTSTR)strSource,LVCFMT_LEFT,nWidth);

	// Support dragging in files
	DragAcceptFiles(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEncryptWiz_1::OnAddDirectory() 
{
	// TODO: Add your control notification handler code here
	WCHAR szSourceDir[MAX_PATH];
	if(SelectFolder(m_hWnd,szSourceDir))
	{
		if( FALSE == AddOneItem(m_Source,szSourceDir) )
		{
			TRACE(L"\nAddOneItem error in OnAddDirectory.");
		}
	}
	
}

void CEncryptWiz_1::OnAddFile() 
{
	// TODO: Add your control notification handler code here
	WCHAR szSourceFile[MAX_PATH];

	CString strFileDescription;
	strFileDescription.LoadString(IDS_SOURCE_DESCRIPTION);
	if( SelectFile(L"*",(LPCTSTR)strFileDescription,szSourceFile) )
	{
		// m_strSource = szSourceFile;
		if( FALSE == AddOneItem(m_Source,szSourceFile) )
		{
			TRACE(L"\nAddOneItem error in OnAddFile.");
		}
	}
}

void CEncryptWiz_1::OnRemove() 
{
	CString strText,strTitle;
	// TODO: Add your control notification handler code here
	int nItemCount;
	nItemCount = m_Source.GetItemCount();
	if( 0 == nItemCount )
	{
		strText.LoadString(IDS_NO_ITEM_TO_REMOVE);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
		return;
	}

	int nSelectedCount;
	nSelectedCount = m_Source.GetSelectedCount();
	if( 0 == nSelectedCount )
	{
		strText.LoadString(IDS_NO_ITEM_SELECTED);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
		return;
	}

	int nItemIndex;
	for(nItemIndex=0; nItemIndex < nItemCount; nItemIndex++)
	{
		if( LVIS_SELECTED == m_Source.GetItemState(nItemIndex,LVIS_SELECTED) )
		{
			m_Source.DeleteItem(nItemIndex);
			break;
		}
	}
}

LRESULT CEncryptWiz_1::OnWizardNext() 
{
	// TODO: Add your specialized code here and/or call the base class
	CString strText,strTitle;
	int nItemCount;
	nItemCount = m_Source.GetItemCount();
	if( 0 == nItemCount )
	{
		strText.LoadString(IDS_EMPTY_SOURCE);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
		return -1;
	}

	g_EncryptInfo.m_strSourceArray.RemoveAll();
	
	int nItemIndex;

	for(nItemIndex=0; nItemIndex<nItemCount; nItemIndex++)
	{
		g_EncryptInfo.m_strSourceArray.Add(m_Source.GetItemText(nItemIndex,0));
	}

	return CPropertyPage::OnWizardNext();
}

BOOL CEncryptWiz_1::AddOneItem(CListCtrl &SourceList, LPCTSTR szFileName)
{
	int nItemCount;
	nItemCount = SourceList.GetItemCount();

	CString strFileName;
	strFileName = szFileName;

	// add an end "\\" to ensure we can compare correctly
	// for example
	// c:\\test
	// c:\\testdir
	// the second one is not a sub dir of the first one.

	strFileName.TrimRight(L"\\");
	strFileName = strFileName +L"\\";

	// first, check if it is the sub file or sub directory of the existing item
	CString strItem;

	int nItemIndex;
	for(nItemIndex=0; nItemIndex<nItemCount; nItemIndex++)
	{
	   strItem = SourceList.GetItemText(nItemIndex,0);
	   
	   strItem.TrimRight(L"\\");
	   strItem=strItem+L"\\";

	   if( 0 == strItem.Compare(strFileName) )
	   {
		   TRACE(L"\nThe item has alread exist.");
		   return FALSE;
	   }

	   if( strItem.GetLength() <= strFileName.GetLength() )
	   {
		   if( 0 == strItem.Compare( strFileName.Left( strItem.GetLength() ) ) )
		   {
			   TRACE(L"\nThe item is the sub item of an existing item.");
			   return FALSE;
		   }
	   }
	}

	// to this step, we know the item will be added
	// but we need to check if we need to delete some existing items
	// second, check if it is the parent directory of the existing item

	for(nItemIndex=nItemCount-1; nItemIndex>=0; nItemIndex--)
	{
	   strItem = SourceList.GetItemText(nItemIndex,0);
	   
	   strItem.TrimRight(L"\\");
	   strItem=strItem+L"\\";

	   if(strItem.GetLength() > strFileName.GetLength() )
	   {
		   if( 0 == strFileName.Compare(strItem.Left(strFileName.GetLength())) )
		   {
			   SourceList.DeleteItem(nItemIndex);
		   }
	   }
	}

	nItemCount = SourceList.GetItemCount();
	SourceList.InsertItem(nItemCount,szFileName);
	return TRUE;
}

void CEncryptWiz_1::OnDropFiles(HDROP hDropInfo) 
{
	// TODO: Add your message handler code here and/or call default

	WCHAR szFile[MAX_PATH];
	::DragQueryFile(hDropInfo,0,szFile,sizeof(szFile));
	::DragFinish(hDropInfo);

	// m_strSource = szFile;
	// UpdateData(FALSE);

	if( FALSE == AddOneItem(m_Source,szFile) )
	{
		TRACE(L"\nAddOneItem error in OnDropFiles.");
	}
	
	CPropertyPage::OnDropFiles(hDropInfo);
}
