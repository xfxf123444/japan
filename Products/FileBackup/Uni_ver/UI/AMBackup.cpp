// AMBackup.cpp : implementation file
//

#include "stdafx.h"
#include "AM01.h"
#include "AMBackup.h"
#include "SaveSetting.h"
#include "BackupState.h"
#include "..\AM01Expt\AM01Expt.h"
#include "function.h"
#include "SetPasswordDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CStringArray g_SettingArray;
/////////////////////////////////////////////////////////////////////////////
// CAMBackup dialog

IMPLEMENT_DYNCREATE(CAMBackup, CPropertyPage)

CAMBackup::CAMBackup() : CPropertyPage(CAMBackup::IDD)
{
	//{{AFX_DATA_INIT(CAMBackup)
	m_SourcePath = _T("");
	m_TargetPath = _T("");
	m_Comment = _T("");
	m_bLogFail = FALSE;
	m_dwMinutes = 10;
	m_bRetry = FALSE;
	m_bAutoSpawn = FALSE;
	//}}AFX_DATA_INIT
	m_strPassword = _T("");
	m_bExecuting = FALSE;
}

CAMBackup::~CAMBackup()
{
}

void CAMBackup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAMBackup)
	DDX_Control(pDX, IDC_LIST1, m_SourceList);
	DDX_Text(pDX, IDC_SOURCE_PATH, m_SourcePath);
	DDV_MaxChars(pDX, m_SourcePath, 260);
	DDX_Text(pDX, IDC_TARGET_PATH, m_TargetPath);
	DDV_MaxChars(pDX, m_TargetPath, 260);
	DDX_Text(pDX, IDC_COMMENT, m_Comment);
	DDV_MaxChars(pDX, m_Comment, 50);
	DDX_Check(pDX, IDC_LOG_FAIL, m_bLogFail);
	DDX_Text(pDX, IDC_MINUTES, m_dwMinutes);
	DDX_Check(pDX, IDC_RETRY, m_bRetry);
	DDX_Check(pDX, IDC_AUTO_SPAWN, m_bAutoSpawn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAMBackup, CPropertyPage)
	//{{AFX_MSG_MAP(CAMBackup)
	ON_BN_CLICKED(IDC_SAVE_SETTING, OnSaveSetting)
	ON_BN_CLICKED(IDC_REFER_TARGET, OnReferTarget)
	ON_BN_CLICKED(IDC_REFER_SOURCE, OnReferSource)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_RETRY, OnRetry)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAMBackup message handlers


BOOL CAMBackup::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	CString		csLine;
	CString		csHeading;
	LV_COLUMN	Column;
	CRect		ListRect;

	m_SourceList.GetClientRect(ListRect);

	Column.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
	Column.fmt = LVCFMT_LEFT;
	Column.cx  = ListRect.right-ListRect.left;
	csHeading.LoadString(IDS_SOURCE);
	Column.pszText = csHeading.GetBuffer(0);
	Column.cchTextMax = 0;		// ignored
	Column.iSubItem = 0;		
	m_SourceList.InsertColumn(0,&Column);

	m_pImageList = new CImageList;
	m_pImageList->Create(IDB_STATEICONS,16,0,RGB(255,255,255));
	m_SourceList.SetImageList(m_pImageList,LVSIL_STATE);
	
	return TRUE;
}

void CAMBackup::OnSaveSetting() 
{
	int nTotal,i;
	AM01_PROG_SETTING Setting;
	CSaveSetting SaveSettingDlg;

	UpdateData(TRUE);
	if (!CheckValidSetting()) return;

	if (SaveSettingDlg.DoModal() == IDOK)
	{
		//memset(&Setting,0,sizeof(Setting));
		Setting.DataInfo.wBackupType = FILE_BACKUP;
		wcscpy(Setting.DataInfo.szTargetImage,m_TargetPath);
		wcscpy(Setting.DataInfo.szComment,m_Comment);
		nTotal = m_SourceList.GetItemCount();
		for (i = 0;i < nTotal;i ++)
		{
			Setting.pcaSource.Add(m_SourceList.GetItemText(i,0));
		}
	    // 2004.05.19 added begin
		wcsncpy(Setting.DataInfo.szSettingName,(LPCTSTR)(SaveSettingDlg.m_SettingName),MAX_SETTING_NAME_LEN-1);
		// we do not consider encrypt in this dialog

		if(m_strPassword.GetLength()>0)
		{
			Setting.DataInfo.BackupOption.bEncrypt = TRUE;
			wcsncpy(Setting.DataInfo.BackupOption.pchPassword,(LPCTSTR)m_strPassword,PASSWORD_SIZE);
		}
		else
		{
			Setting.DataInfo.BackupOption.bEncrypt = FALSE;
			memset(Setting.DataInfo.BackupOption.pchPassword,0,PASSWORD_SIZE*sizeof(WCHAR));
		}

		Setting.DataInfo.BackupOption.bAutoSpawn = m_bAutoSpawn;
		Setting.DataInfo.BackupOption.bLogFail = m_bLogFail;
		Setting.DataInfo.BackupOption.bRetryFail = m_bRetry;
		Setting.DataInfo.BackupOption.dwRetryMinutes = m_dwMinutes;

		if( FALSE == AddOneSetting(&Setting))
		{
			TRACE(L"\nAddOneSetting error in OnSaveSetting.");
		}

		g_SettingArray.Add(SaveSettingDlg.m_SettingName);
		
	}
	// TODO: Add your control notification handler code here
	
}

void CAMBackup::OnReferTarget() 
{
	WCHAR szTarget[MAX_PATH];
	int  nTotal,i;
	CString strSource;
	UpdateData(TRUE);
	if (SelectFile(L"AMG",L"AM01 Image File",szTarget))
	{
		nTotal = m_SourceList.GetItemCount();
		for (i = 0;i < nTotal;i ++)
		{
			strSource = m_SourceList.GetItemText(i,0);
			if (CheckConflict(strSource.GetBuffer(0),szTarget,TRUE))
			{
				AfxMessageBox(IDS_CONFLICT,MB_OK | MB_ICONINFORMATION);
				return;
			}
		}
		m_TargetPath = szTarget;
		UpdateData(FALSE);
	}
	// TODO: Add your control notification handler code here
	
}

void CAMBackup::OnReferSource() 
{
	WCHAR szSource[MAX_PATH];
	UpdateData(TRUE);
	if (SelectFolder(m_hWnd,szSource))
	{
		m_SourcePath = szSource;
		UpdateData(FALSE);
	}
	
}

void CAMBackup::OnAdd() 
{
	int nTotal,i;
	WCHAR szPath[MAX_PATH];
	CString strPath;
	UpdateData(TRUE);

	if (GetLongPathName(m_SourcePath.GetBuffer(0),szPath,MAX_PATH))
	{
		if (CheckConflict(szPath,m_TargetPath.GetBuffer(0),TRUE))
		{
			AfxMessageBox(IDS_CONFLICT,MB_OK | MB_ICONINFORMATION);
			return;
		}

		m_SourcePath = szPath;
		nTotal = m_SourceList.GetItemCount();
		for (i = 0;i < nTotal;i++)
		{
			strPath = m_SourceList.GetItemText(i,0);
			if (CheckConflict(strPath.GetBuffer(0),szPath,FALSE)) return;
			if (CheckConflict(szPath,strPath.GetBuffer(0),FALSE))
			{
				m_SourceList.DeleteItem(i);
				nTotal --;
				i --;
			}
		}
		m_SourceList.InsertItem(nTotal,m_SourcePath);
		m_SourceList.SetItem(nTotal,0,LVIF_STATE,L"",0,INDEXTOSTATEIMAGEMASK(1),LVIS_STATEIMAGEMASK,0);
	}
	else AfxMessageBox(IDS_INVALID_SOURCE,MB_OK | MB_ICONINFORMATION);
	// TODO: Add your control notification handler code here
	
}

void CAMBackup::OnRemove() 
{
	int nTotal,i;
	
	nTotal = m_SourceList.GetItemCount();
	for (i = 0;i < nTotal;i ++)
	{
		if (m_SourceList.GetItemState(i,LVIS_SELECTED))
		{
			m_SourceList.DeleteItem(i);
			break;
		}
	}
	// TODO: Add your control notification handler code here
	
}

void CAMBackup::OnStart() 
{
	int nTotal,i;
	CBackupState BackupStateDlg;
	UpdateData(TRUE);

	if (!CheckValidSetting()) return;

	if (!m_bExecuting)
	{
		m_bExecuting = TRUE;
		//memset(&BackupStateDlg.m_Setting,0,sizeof(BackupStateDlg.m_Setting));
		BackupStateDlg.m_Setting.DataInfo.wBackupType = FILE_BACKUP;
		wcscpy(BackupStateDlg.m_Setting.DataInfo.szTargetImage,m_TargetPath);
		wcscpy(BackupStateDlg.m_Setting.DataInfo.szComment,m_Comment);

		nTotal = m_SourceList.GetItemCount();
		for (i = 0;i < nTotal;i ++)
		{
			BackupStateDlg.m_Setting.pcaSource.Add(m_SourceList.GetItemText(i,0));
		}

		if(m_strPassword.GetLength()>0)
		{
			BackupStateDlg.m_Setting.DataInfo.BackupOption.bEncrypt = TRUE;
			wcsncpy(BackupStateDlg.m_Setting.DataInfo.BackupOption.pchPassword,(LPCTSTR)m_strPassword,PASSWORD_SIZE);
		}
		else
		{
			BackupStateDlg.m_Setting.DataInfo.BackupOption.bEncrypt = FALSE;
			memset(BackupStateDlg.m_Setting.DataInfo.BackupOption.pchPassword,0,PASSWORD_SIZE*sizeof(WCHAR));
		}

		BackupStateDlg.m_Setting.DataInfo.BackupOption.bAutoSpawn = m_bAutoSpawn;
		BackupStateDlg.m_Setting.DataInfo.BackupOption.bLogFail = m_bLogFail;
		BackupStateDlg.m_Setting.DataInfo.BackupOption.bRetryFail = m_bRetry;
		BackupStateDlg.m_Setting.DataInfo.BackupOption.dwRetryMinutes = m_dwMinutes;
		BackupStateDlg.m_Setting.DataInfo.BackupOption.bCmdLine = FALSE;

		BackupStateDlg.DoModal();

		m_bExecuting = FALSE;
	}
	else AfxMessageBox(IDS_BACKUP_RUNING,MB_OK | MB_ICONINFORMATION);
	// TODO: Add your control notification handler code here
}

void CAMBackup::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	delete m_pImageList;
	m_pImageList = NULL;
	CPropertyPage::OnClose();
}

BOOL CAMBackup::CheckValidSetting()
{
	int nTotal,i;
	CString strSource;

	if (!IsValidAMGPath(m_TargetPath.GetBuffer(0)))
	{
		AfxMessageBox(IDS_INVALID_IMAGE,MB_OK | MB_ICONINFORMATION);
		return FALSE;
	}

	nTotal = m_SourceList.GetItemCount();

	if (nTotal <= 0)
	{
		AfxMessageBox(IDS_SOURCE_EMPTY,MB_OK | MB_ICONINFORMATION);
		return FALSE;
	}

	for (i = 0;i < nTotal;i ++)
	{
		strSource = m_SourceList.GetItemText(i,0);
		if (CheckConflict(strSource.GetBuffer(0),m_TargetPath.GetBuffer(0),TRUE))
		{
			AfxMessageBox(IDS_CONFLICT,MB_OK | MB_ICONINFORMATION);
			return FALSE;
		}
	}
	if (m_bRetry && !m_dwMinutes)
	{
		AfxMessageBox(IDS_INVALID_TIME);
		return FALSE;
	}
	return TRUE;
}

void CAMBackup::OnDestroy() 
{
	CPropertyPage::OnDestroy();
	
	// TODO: Add your message handler code here
	if( NULL != m_pImageList )
	{
		delete m_pImageList;
		m_pImageList=NULL;
	}
}

void CAMBackup::OnRetry() 
{
	UpdateData(TRUE);
	if (m_bRetry) m_bLogFail = TRUE;
	UpdateData(FALSE);
}
