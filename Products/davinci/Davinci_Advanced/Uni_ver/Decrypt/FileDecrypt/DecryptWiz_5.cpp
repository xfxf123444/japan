// DecryptWiz_5.cpp : implementation file
//

#include "stdafx.h"
#include "filedecrypt.h"
#include "DecryptWiz_5.h"
#include "..\..\EncryptToolStruct\EncryptToolStruct.h"
#include "..\DecryptFunction\DecryptFunction.h"
#include "..\..\DavinciFunction\deletemethoddlg.h"
#include "DecryptStatus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern DECRYPT_INFO g_DecryptInfo;
extern BOOL g_bDecryptSelectedFolderOnly;
extern BOOL bSecureDeleteSource;
extern BOOL g_bDecryptSucceed;

/////////////////////////////////////////////////////////////////////////////
// CDecryptWiz_5 property page

IMPLEMENT_DYNCREATE(CDecryptWiz_5, CPropertyPage)

CDecryptWiz_5::CDecryptWiz_5() : CPropertyPage(CDecryptWiz_5::IDD)
, m_bDecryptSelectedFolderOnly(FALSE)
{
	//{{AFX_DATA_INIT(CDecryptWiz_5)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CDecryptWiz_5::~CDecryptWiz_5()
{
}

void CDecryptWiz_5::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDecryptWiz_5)
	DDX_Control(pDX, IDC_DIR_TREE, m_DirTree);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHECK_CURRENT_FOLDER_ONLY, m_bDecryptSelectedFolderOnly);
	DDX_Control(pDX, IDC_CHECK_CURRENT_FOLDER_ONLY, m_checkDecryptCurrentFolderOnly);
}


BEGIN_MESSAGE_MAP(CDecryptWiz_5, CPropertyPage)
	//{{AFX_MSG_MAP(CDecryptWiz_5)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDecryptWiz_5 message handlers

BOOL CDecryptWiz_5::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
		
	// TODO: Add extra initialization here

	m_ImageList.Create(IDB_DRIVER, 16, 1, RGB (255, 0, 255));
	CBitmap FileBitmap;
	FileBitmap.LoadBitmap(IDB_FILE);
	m_ImageList.Add(&FileBitmap,RGB(255,0,255));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDecryptWiz_5::OnWizardFinish() 
{
	// TODO: Add your specialized code here and/or call the base class

	UpdateData(TRUE);
	g_bDecryptSelectedFolderOnly = m_bDecryptSelectedFolderOnly;

	if( FALSE == GetUIInfo() )
	{
		TRACE(L"\nGetUIInfo error in OnWizardFinish.");
	}

	FixupDecryptInfo(&g_DecryptInfo);

	CDecryptStatus DecryptStatusDlg;
	DecryptStatusDlg.m_DecryptInfo = g_DecryptInfo;
	DecryptStatusDlg.m_bDecryptSelectedFolderOnly = g_bDecryptSelectedFolderOnly;
	DecryptStatusDlg.DoModal();

	if (g_bDecryptSucceed)
	{
		if (bSecureDeleteSource)
		{
			CStringArray strSourceArray;
			CString strSourceFile = g_DecryptInfo.szImageFile;
			strSourceArray.Add(strSourceFile);
			CDeleteMethodDlg DeleteMethodDlg;
			DeleteMethodDlg.m_SourceArray.RemoveAll();
			DeleteMethodDlg.m_SourceArray.Append(strSourceArray);
			DeleteMethodDlg.DoModal();
		}
	}

	// do not close the wizard dialog
	return FALSE;

	// return CPropertyPage::OnWizardFinish();	
}

LRESULT CDecryptWiz_5::OnWizardBack() 
{
	// TODO: Add your specialized code here and/or call the base class
	// TODO: Add your specialized code here and/or call the base class
	CPropertySheet* pSheet = (CPropertySheet*)GetParent();
	ASSERT_KINDOF(CPropertySheet, pSheet);

	(pSheet->GetDlgItem(IDCANCEL))->SetWindowText(m_strTextCancel);
	
	return CPropertyPage::OnWizardBack();
}

BOOL CDecryptWiz_5::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class

	// TODO: Add your specialized code here and/or call the base class
	CPropertySheet* pSheet = (CPropertySheet*)GetParent();
	ASSERT_KINDOF(CPropertySheet, pSheet);
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
	(pSheet->GetDlgItem(IDHELP))->ShowWindow(FALSE);

	(pSheet->GetDlgItem(IDCANCEL))->GetWindowText(m_strTextCancel);
	
	CString strDecrypt;
	strDecrypt.LoadString(IDS_DECRYPT);
	(pSheet->GetDlgItem(ID_WIZFINISH))->SetWindowText(strDecrypt);

	CString strClose;
	strClose.LoadString(IDS_CLOSE);
    
	(pSheet->GetDlgItem(IDCANCEL))->SetWindowText(strClose);

	InitDialogItem();

	return CPropertyPage::OnSetActive();
}

void CDecryptWiz_5::InitDialogItem()
{
	m_DirTree.DeleteAllItems();

	if( DECRYPT_ENTIRE == g_DecryptInfo.nRestoreType )
	{
		m_DirTree.EnableWindow(FALSE);
		m_bDecryptSelectedFolderOnly = FALSE;
		m_checkDecryptCurrentFolderOnly.SetCheck(FALSE);
		m_checkDecryptCurrentFolderOnly.ShowWindow(SW_HIDE);
	}
	else if( DECRYPT_PART == g_DecryptInfo.nRestoreType )
	{
		m_DirTree.EnableWindow(TRUE);
		m_checkDecryptCurrentFolderOnly.ShowWindow(SW_SHOW);
		// load tree
		m_DirTree.SetImageList(&m_ImageList,TVSIL_NORMAL);
		m_DirTree.LoadTreeFromFile(g_DecryptInfo.szImageFile);
		m_DirTree.m_ExpandedNodeArray.RemoveAll();
	}
}

BOOL CDecryptWiz_5::GetUIInfo()
{
	if(DECRYPT_PART == g_DecryptInfo.nRestoreType)
	{
	    CString strSelectedPath;
		strSelectedPath=GetItemPath();
		wcsncpy(g_DecryptInfo.szSelectedPath , (LPCTSTR)strSelectedPath,MAX_PATH-1);
	}
	else
	{
	   memset(g_DecryptInfo.szSelectedPath,0,sizeof(g_DecryptInfo.szSelectedPath));
	}
	return TRUE;
}

CString CDecryptWiz_5::GetItemPath()
{
    CString OnePathString;
	
	HTREEITEM hItem = m_DirTree.GetSelectedItem();
	OnePathString=m_DirTree.GetFullPath(hItem);

	OnePathString.TrimRight(L"\\");
	OnePathString=OnePathString+L"\\";

	return OnePathString;
}
