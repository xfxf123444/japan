// Protect.cpp : implementation file
//

#include "stdafx.h"
#include "YGFSMon.h"
#include "Protect.h"
#include "AddPath.h"
#include "YGFSMonConst.h"
#include "..\Encrypt\EncryptFunction\EncryptInfo.h"
#include "..\Encrypt\EncryptFunction\EncryptFunction.h"
#include "Function.h"
#include "RemoveFolder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define UNLIMITED 128
extern CMONITOR_ITEM_ARRAY g_MonitorItemArray;

/////////////////////////////////////////////////////////////////////////////
// CProtect property page

IMPLEMENT_DYNCREATE(CProtect, CPropertyPage)

CProtect::CProtect() : CPropertyPage(CProtect::IDD)
{
	//{{AFX_DATA_INIT(CProtect)
	//}}AFX_DATA_INIT
}

CProtect::~CProtect()
{
}

void CProtect::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProtect)
	DDX_Control(pDX, IDC_LIST1, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProtect, CPropertyPage)
	//{{AFX_MSG_MAP(CProtect)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


////////////////////////////////////////////////
// CProtect message handlers

BOOL CProtect::OnInitDialog() 
{
	CString		csHeading;
	LV_COLUMN	Column;
	CRect		ListRect;
	CPropertyPage::OnInitDialog();

	m_List.GetClientRect(ListRect);

	Column.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
	Column.fmt = LVCFMT_LEFT;
	Column.cx  = (ListRect.right-ListRect.left)/3*2;
	csHeading.LoadString(IDS_PATH_NAME);
	
	Column.pszText = csHeading.GetBuffer(0);
	Column.cchTextMax = 0;		// ignored
	Column.iSubItem = 0;		
	m_List.InsertColumn(0,&Column);

	Column.cx  = (ListRect.right-ListRect.left)/3;
	csHeading.LoadString(IDS_COMPRESS_LEVEL);
	Column.pszText = csHeading.GetBuffer(0);
	m_List.InsertColumn(1,&Column);

	m_ImageList.Create(IDB_STATEICONS,16,0,RGB(0,255,0));
	m_List.SetImageList(&m_ImageList,LVSIL_SMALL);


	int nItemIndex,nItem;
	for(nItemIndex=0; nItemIndex<g_MonitorItemArray.GetSize(); nItemIndex++ )
	{
	   nItem = m_List.InsertItem(nItemIndex,g_MonitorItemArray[nItemIndex].szMonitorPath,1);
	   switch (g_MonitorItemArray[nItemIndex].ulCompressLevel)
	   {
	   case LEVEL_FAST_COMPRESS:
		   csHeading.LoadString(IDS_FAST_COMPRESS);
		   m_List.SetItemText(nItem,1,csHeading);
		   break;
	   case LEVEL_HIGH_COMPRESS:
		   csHeading.LoadString(IDS_HIGH_COMPRESS);
		   m_List.SetItemText(nItem,1,csHeading);
		   break;
	   case LEVEL_NO_COMPRESS:
	   default:
		   g_MonitorItemArray[nItemIndex].ulCompressLevel = LEVEL_NO_COMPRESS;
		   csHeading.LoadString(IDS_NO_COMPRESS);
		   m_List.SetItemText(nItem,1,csHeading);
		   break;
	   }
	}

	// 2004.09.13 for debug begin

	if(g_MonitorItemArray.GetSize()>1)
	{
		TRACE(L"\nMore than one directory to monitor.");
	}

	// 2004.09.13 for debug end

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CProtect::OnAdd() 
{
	int nIndex;
	CAddPath AddPathDlg;
	CString csText,strText,strTitle;
	WCHAR szMonitorFile[MAX_PATH];
	ZeroMemory(szMonitorFile, sizeof(szMonitorFile));
	WCHAR szPassword[PASSWORD_SIZE];
	ZeroMemory(szPassword, sizeof(szPassword));
	WCHAR szDestFile[MAX_PATH];
	ZeroMemory(szDestFile, sizeof(szDestFile));

	if (AddPathDlg.DoModal() == IDOK)
	{
		CONVERT_BUF MonitorItem;
		for(nIndex=1;nIndex<m_List.GetItemCount();nIndex++)
		{
			csText = m_List.GetItemText(nIndex,0) ;
			if ( !wcsicmp (AddPathDlg.m_Path.Left(csText.GetLength()),csText)) return;
			if ( !wcsicmp (AddPathDlg.m_Path,csText.Left(AddPathDlg.m_Path.GetLength())))
			{
				memset(&MonitorItem,0,sizeof(CONVERT_BUF));
				wcsncpy(szMonitorFile,(LPCTSTR)csText,MAX_PATH-1);
				MonitorItem.usNodeType = CONVERT_NODETYPE_DIRECTORY;
				WideCharToMultiByte(CP_ACP, 0, szMonitorFile, MAX_PATH, MonitorItem.szMonitorFile, MAX_PATH, 0, 0);
				if( FALSE == YGFSMonRemoveMonitorItem(&MonitorItem) )
				{
					TRACE(L"\nYGFSMonRemoveIncludePath error.");
				    m_List.DeleteItem(nIndex);

					// modify nIndex in for loop, not easy to understand?
				    nIndex -- ;
				}
			}
		}

		memset(&MonitorItem,0,sizeof(CONVERT_BUF));
		wcsncpy(szMonitorFile,(LPCTSTR)(AddPathDlg.m_Path),MAX_PATH-1);
		wcsncpy(szPassword,(LPCTSTR)AddPathDlg.m_strPassword,PASSWORD_SIZE-1);
		WideCharToMultiByte(CP_ACP, 0, szMonitorFile, MAX_PATH, MonitorItem.szMonitorFile, MAX_PATH, 0, 0);
		WideCharToMultiByte(CP_ACP, 0, szPassword, PASSWORD_SIZE, MonitorItem.szPassword, PASSWORD_SIZE, 0, 0);
		MonitorItem.ulCompressLevel = AddPathDlg.m_CompressLevel;
		MonitorItem.usNodeType = CONVERT_NODETYPE_DIRECTORY;
		MonitorItem.bCheckPasswordAlways = AddPathDlg.m_bCheckPasswordAlways;
		MonitorItem.bErrorLimit = AddPathDlg.m_bErrorLimition && AddPathDlg.m_bCheckPasswordAlways;
		MonitorItem.ulMaxErrLimit = AddPathDlg.m_nMaxInputNumber;

		if( FALSE == YGFSMonAddMonitorItem(&MonitorItem) )
		{
			TRACE(L"\nYGFSMonAddIncludePath %s error.",MonitorItem.szMonitorFile);
		}
		else
		{
			TRACE(L"\nSuccessfully add %s as monitor folder.",MonitorItem.szMonitorFile);
			int nItem = m_List.InsertItem(0,AddPathDlg.m_Path,1);
			
			MONITOR_ITEM MonitorItem;
			memset(&MonitorItem,0,sizeof(MonitorItem));

			wcsncpy(MonitorItem.szMonitorPath,AddPathDlg.m_Path,MAX_PATH-1);
			wcsncpy(MonitorItem.szPassword,(LPCTSTR)AddPathDlg.m_strPassword,PASSWORD_SIZE-1);

			MonitorItem.ulCompressLevel = AddPathDlg.m_CompressLevel;
			MonitorItem.bCheckPasswordAlways = AddPathDlg.m_bCheckPasswordAlways;
			MonitorItem.bErrorLimit = AddPathDlg.m_bErrorLimition && AddPathDlg.m_bCheckPasswordAlways;
			MonitorItem.ulMaxErrLimit = AddPathDlg.m_nMaxInputNumber;

		    switch (MonitorItem.ulCompressLevel)
			{
		    case LEVEL_FAST_COMPRESS:
			   csText.LoadString(IDS_FAST_COMPRESS);
			   m_List.SetItemText(nItem,1,csText);
			   break;
		    case LEVEL_HIGH_COMPRESS:
			   csText.LoadString(IDS_HIGH_COMPRESS);
			   m_List.SetItemText(nItem,1,csText);
			   break;
		    case LEVEL_NO_COMPRESS:
		    default:
			   MonitorItem.ulCompressLevel = LEVEL_NO_COMPRESS;
			   csText.LoadString(IDS_NO_COMPRESS);
			   m_List.SetItemText(nItem,1,csText);
			   break;
			}
			
			g_MonitorItemArray.Add(MonitorItem);
			
			if( FALSE == RecordSetting(g_MonitorItemArray) )
			{
				TRACE(L"\nRecordSetting error in OnAdd.");
			}
			strText.LoadString(IDS_ENCRYPT_NOW);
			strTitle.LoadString(IDS_APP_NAME);
			if (MessageBox(strText,strTitle,MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				EncryptOneDirSeparately(MonitorItem.szMonitorPath,MonitorItem.szPassword,TRUE,MonitorItem.ulCompressLevel,MonitorItem.bErrorLimit,MonitorItem.ulMaxErrLimit);
			}
		}
	}
}

void CProtect::OnRemove() 
{
	// TODO: Add your control notification handler code here
	CString strText,strTitle;

	if( 0 == m_List.GetItemCount() )
	{
		strText.LoadString(IDS_EMPTY_ITEM);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
		return;
	}

	if( 0 ==  m_List.GetSelectedCount() )
	{
		strText.LoadString(IDS_SELECT_REMOVE_ITEM);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
		return;
	}

	WCHAR szMonitorFile[MAX_PATH];
	int nTotal,i;
	
	nTotal = m_List.GetItemCount();
	for (i = 0;i < nTotal;i ++)
	{
		if (m_List.GetItemState(i,LVIS_SELECTED))
		{
			int nItemIndex;
			m_List.GetItemText(i,0,szMonitorFile,MAX_PATH);

			for( nItemIndex=0; nItemIndex<g_MonitorItemArray.GetSize(); nItemIndex++ )
			{
		        TRACE(L"\ng_MonitorItemArray[%d].szMonitorPath is:%s",nItemIndex,g_MonitorItemArray[nItemIndex].szMonitorPath);
				if( 0 == wcsicmp(g_MonitorItemArray[nItemIndex].szMonitorPath,szMonitorFile) )
				{
					CRemoveFolder RemoveFolderDlg;
					RemoveFolderDlg.m_MonitorItem = g_MonitorItemArray[nItemIndex];
					if (RemoveFolderDlg.DoModal() == IDOK)
					{
						g_MonitorItemArray.RemoveAt(nItemIndex);
						if( FALSE == RecordSetting(g_MonitorItemArray) )
						{
							TRACE(L"\nRecordSetting error in OnRemove.");
							break;
						}

						m_List.DeleteItem(i);
					}
					break;
				}
			}
			break;
		}
	}	
}
