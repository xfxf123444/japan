// ViewSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "am01.h"
#include "ViewSettingDlg.h"
#include "../AM01struct/AM01struct.h"
#include "../AM01Expt/AM01Expt.h"
#include "function.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CString g_strSelectedSetting;
EASY_BACKUP_INFO g_EasyBackupInfo;

/////////////////////////////////////////////////////////////////////////////
// CViewSettingDlg dialog


CViewSettingDlg::CViewSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CViewSettingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CViewSettingDlg)
	m_strTarget = _T("");
	m_strSettingName = _T("");
	m_strEncrypt = _T("");
	m_bLogFail = FALSE;
	m_dwMinutes = 0;
	m_bRetry = FALSE;
	m_bAutoSpawn = FALSE;
	//}}AFX_DATA_INIT
}


void CViewSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewSettingDlg)
	DDX_Control(pDX, IDC_LIST_SOURCE_FILE, m_SourceList);
	DDX_Control(pDX, IDC_BUTTON_CHANGE_NAME, m_btChangeName);
	DDX_Text(pDX, IDC_EDIT_TARGET, m_strTarget);
	DDX_Text(pDX, IDC_EDIT_SETTING_NAME, m_strSettingName);
	DDX_Text(pDX, IDC_VIEW_SETTING_ENCRYPT, m_strEncrypt);
	DDV_MaxChars(pDX, m_strEncrypt, YG_ENCRYPT_KEY_SIZE);
	DDX_Check(pDX, IDC_LOG_FAIL, m_bLogFail);
	DDX_Text(pDX, IDC_MINUTES, m_dwMinutes);
	DDX_Check(pDX, IDC_RETRY, m_bRetry);
	DDX_Check(pDX, IDC_AUTO_SPAWN, m_bAutoSpawn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CViewSettingDlg, CDialog)
	//{{AFX_MSG_MAP(CViewSettingDlg)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, OnButtonExit)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_NAME, OnButtonChangeName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewSettingDlg message handlers

BOOL CViewSettingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	InitDialogControl();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CViewSettingDlg::OnButtonExit() 
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CViewSettingDlg::OnButtonChangeName() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	CString strMessage;

	strMessage.LoadString(IDS_SAME_SETTING_NAME);
     
	if(m_strSettingName.Compare(g_strSelectedSetting) == FALSE )
	{
		AfxMessageBox((LPCTSTR)strMessage,MB_OK|MB_ICONINFORMATION,NULL);
	}
	else
	{
		// strMessage = L"Do you want to change the setting name from " + g_strSelectedSetting + " to " + m_strSettingName+ "?";

		strMessage.LoadString(IDS_CHANGE_SETTING_NAME);

		CString strAsk;

		strAsk.LoadString(IDS_ASK);

		strMessage= strMessage + m_strSettingName+ strAsk;
		
		if( IDYES == AfxMessageBox( (LPCTSTR)strMessage,MB_YESNOCANCEL|MB_ICONINFORMATION,NULL) )
		{
		  
		  BOOL bSettingNameExist;
		  bSettingNameExist = FALSE;

		  if( FALSE == IsSettingNameExist((LPCTSTR)m_strSettingName,&bSettingNameExist) )
		  {
			  TRACE(L"\nError while judge setting name exist.");
			  return;
		  }

		  if( FALSE == bSettingNameExist )
		  {
			// the setting name is not exist, setting name
			// can be changed to this name

		    if( FALSE == ChangeSettingName(g_strSelectedSetting, m_strSettingName) )
			{
		       TRACE(L"\nChanage setting name failed.");
			}
			else
			{
				// update the global variable g_strSelectedSeting.
				// is it useful? just update it.
				g_strSelectedSetting=m_strSettingName;
			}
		  }
		  else
		  {
			  AfxMessageBox(IDS_SETTING_NAME_EXIST,MB_OK|MB_ICONINFORMATION);
		  }
		}
		else
		{
		   m_strSettingName=g_strSelectedSetting;
	       UpdateData(FALSE);
		}
	}
		
}


BOOL CViewSettingDlg::InitDialogControl()
{
	m_strSettingName = g_strSelectedSetting;

    m_SourceList.DeleteAllItems();
	
	int nReturnValue;
	nReturnValue=0;

	do
	{
	  nReturnValue = m_SourceList.DeleteColumn(0);
	}while( 0 != nReturnValue);

	AM01_PROG_SETTING ProgSetting;
	//memset(&ProgSetting,0,sizeof(ProgSetting));

	if( FALSE == GetSettingInfo((LPCTSTR)g_strSelectedSetting,&ProgSetting))
	{
		TRACE(L"\nError when GetSettingInfo in InitDialogControl");
		return FALSE;
	}

	m_strTarget=ProgSetting.DataInfo.szTargetImage; 

	CString strListTitle;
	// note: now use IDS_SOURCE, perhaps in the future we will use others

	strListTitle.LoadString(IDS_SOURCE);
	m_SourceList.InsertColumn(0,strListTitle,LVCFMT_LEFT,m_SourceList.GetStringWidth(strListTitle + L"    ") );
	
	if( BACKUP_TYPE_DIFF == ProgSetting.DataInfo.wBackupType)
	{
		// here the program should know the first file to get selection array
		SELECTION_LINK *pSelectionHead;
		pSelectionHead=NULL;

		SELECTION_LINK *pSelectionLink;
		pSelectionLink=NULL;

		if( FALSE == GetSelectionLink(ProgSetting.DataInfo.szTargetImage,&pSelectionHead) )
		{
			TRACE(L"\nGetSelectionLink error in InitDialogControl.");
		}
		else
		{
			pSelectionLink = pSelectionHead;
		    int nSelectionIndex=0;
			while(NULL != pSelectionLink)
			{
				m_SourceList.InsertItem(nSelectionIndex,pSelectionLink->SelectionData.szFilePath);
				nSelectionIndex++;
				pSelectionLink=pSelectionLink->pNextSelection;
			}
			FreeSelectionLink(pSelectionHead);
		}
	}
	else
	{
		int nSelectionIndex;
		for(nSelectionIndex = 0; nSelectionIndex <= ProgSetting.pcaSource.GetUpperBound(); nSelectionIndex++ )
		{
			m_SourceList.InsertItem(nSelectionIndex,ProgSetting.pcaSource.ElementAt(nSelectionIndex));
		}

	}

	if(ProgSetting.DataInfo.BackupOption.bEncrypt)
	{
		m_strEncrypt.LoadString(IDS_YES);
	}
	else
	{
		m_strEncrypt.LoadString(IDS_NO);
	}

	m_bAutoSpawn = ProgSetting.DataInfo.BackupOption.bLogFail;
	m_bLogFail = ProgSetting.DataInfo.BackupOption.bLogFail;
	m_dwMinutes = ProgSetting.DataInfo.BackupOption.dwRetryMinutes;
	m_bRetry = ProgSetting.DataInfo.BackupOption.bRetryFail;


	UpdateData(FALSE);

	return TRUE;
}


