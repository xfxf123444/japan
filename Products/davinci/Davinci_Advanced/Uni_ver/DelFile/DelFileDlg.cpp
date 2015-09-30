// DelFileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DelFile.h"
#include "DelFileDlg.h"
#include "Folder.h"
#include "Fun.h"

extern WCHAR	g_szDir[MAX_PATH][MAX_PATH];
extern int		g_nSelItem;
extern BOOL		g_bSave;
extern WCHAR	g_szSaveDir[MAX_PATH];
/////////////////////////////////////////////////////////////////////////////
// CDelFileDlg dialog

CDelFileDlg::CDelFileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDelFileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDelFileDlg)
	m_csSelInfo = _T("");
	m_csPathName = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDelFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDelFileDlg)
	DDX_Text(pDX, IDC_SEL_ITEM, m_csSelInfo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDelFileDlg, CDialog)
	//{{AFX_MSG_MAP(CDelFileDlg)
	ON_BN_CLICKED(IDC_BACKUP_DELTE, OnBackupDelte)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDelFileDlg message handlers

BOOL CDelFileDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(g_nSelItem == 1)
	{
		m_csSelInfo.Format(IDS_SINGLE_SELECTED,g_szDir[0]);
	}
	else
	{
		m_csSelInfo.Format(IDS_MULTY_SELECTED,g_nSelItem);
	}
	UpdateData(FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CDelFileDlg::OnBackupDelte() 
{
	CFolder		Dlg(this);
	int			i;
	CString		csText,csCaption;

	csCaption.LoadString (IDS_PRODUCT_NAME);

	Dlg.m_bShowFile = FALSE;
	if(IDOK == Dlg.DoModal())
	{
		m_csPathName = Dlg.m_csFileName;
		if(m_csPathName == "")
			return ;

		m_csPathName.MakeUpper();
		for(i=0;i<g_nSelItem;i++)
		{
			if(IsInCludePath(g_szDir[i]))
			{
				csText.Format (IDS_SELECT_OTHER_SAVE_PATH,m_csPathName);
				MessageBox(csText,csCaption,MB_OK|MB_ICONSTOP);
				m_csPathName = "";
				return;
			}
			if(IsParentPath(g_szDir[i]))
			{
				csText.Format (IDS_SELECT_PARENT_PATH,m_csPathName);
				MessageBox(csText,csCaption,MB_OK|MB_ICONSTOP);
				m_csPathName = "";
				return;
			}
		}
		if(m_csPathName == "")
		{
			g_bSave = FALSE;
		}
		else
		{
			g_bSave = TRUE;
		}
		swprintf(g_szSaveDir,L"%s",m_csPathName.GetBuffer(0));
		CDialog::OnOK();
	}
	else
		CDialog::OnCancel ();
}

BOOL CDelFileDlg::IsInCludePath(WCHAR  *pszDir)
{
	HANDLE				hFind;
    WIN32_FIND_DATAW     FindData;
	WCHAR				*pdest;
	CString				csDelPath;

	if ( NULL == pszDir)
	{
		return FALSE ;
	}

	csDelPath = pszDir;
	csDelPath.MakeUpper ();

	hFind = FindFirstFileW(pszDir,&FindData);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		return FALSE;
	}
	if ( !(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
	{
		FindClose( hFind);
		return FALSE;
	}
	FindClose( hFind);
	pdest = wcsstr(m_csPathName.GetBuffer(0),csDelPath.GetBuffer(0));

	if(!pdest) return FALSE;
	return TRUE;
}

BOOL CDelFileDlg::IsParentPath(WCHAR  *pszDir)
{
	WCHAR				*pdest;
	WCHAR				szPath[MAX_PATH];
	CString				csDelPath;

	if ( NULL == pszDir)
	{
		return FALSE ;
	}

	csDelPath = pszDir;
	csDelPath.MakeUpper ();

	wcscpy(szPath,csDelPath.GetBuffer(0));
	pdest = wcsrchr(szPath,L'\\');
	*pdest = 0;
	if(wcscmp(m_csPathName.GetBuffer(0),szPath))
	{
		return FALSE;
	}

	return TRUE;
}
