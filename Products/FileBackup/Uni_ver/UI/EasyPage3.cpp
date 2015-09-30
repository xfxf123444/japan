// EasyPage3.cpp : implementation file
//

#include "stdafx.h"
#include "am01.h"
#include "EasyPage3.h"
#include "../AM01Struct/AM01Struct.h"
#include "Function.h"
#include "BackupState.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern EASY_BACKUP_INFO g_EasyBackupInfo;

/////////////////////////////////////////////////////////////////////////////
// CEasyPage3 property page

IMPLEMENT_DYNCREATE(CEasyPage3, CPropertyPage)

CEasyPage3::CEasyPage3() : CPropertyPage(CEasyPage3::IDD)
{
	//{{AFX_DATA_INIT(CEasyPage3)
	m_strTargetFile = _T("");
	//}}AFX_DATA_INIT
	m_bExecuting = FALSE;
}

CEasyPage3::~CEasyPage3()
{
}

void CEasyPage3::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEasyPage3)
	DDX_Text(pDX, IDC_EDIT_TARGET_FILE, m_strTargetFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEasyPage3, CPropertyPage)
	//{{AFX_MSG_MAP(CEasyPage3)
	ON_BN_CLICKED(IDC_BUTTON_REFER, OnButtonRefer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEasyPage3 message handlers

void CEasyPage3::OnButtonRefer() 
{
	// TODO: Add your control notification handler code here
    WCHAR szTargetFile[MAX_PATH];
	if( FALSE == NewSelectFile(L"amg",L"AMG File",szTargetFile) )
	{
		TRACE(L"\nYou did not select a valid file.");
		return;
	}
	
	wcsncpy(g_EasyBackupInfo.szImageFileName,szTargetFile,MAX_PATH-1);
	m_strTargetFile = szTargetFile;
	
	UpdateData(FALSE);

}

BOOL CEasyPage3::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class

	CPropertySheet* pSheet = (CPropertySheet*)GetParent();
	ASSERT_KINDOF(CPropertySheet, pSheet);
	pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);

	// (pSheet->GetDlgItem(ID_WIZBACK))->ShowWindow(FALSE);
	(pSheet->GetDlgItem(IDHELP))->ShowWindow(FALSE);
	
	return CPropertyPage::OnSetActive();
}

LRESULT CEasyPage3::OnWizardBack() 
{
	// TODO: Add your specialized code here and/or call the base class

	CPropertySheet* pSheet = (CPropertySheet*)GetParent();
	ASSERT_KINDOF(CPropertySheet, pSheet);

	if( 0 == g_EasyBackupInfo.nBackupContent )
	{
	   return ( pSheet->SetActivePage(pSheet->GetPage(0)) );
	}
	else if (  ( 1 == g_EasyBackupInfo.nBackupContent ) ||
		       ( 2 == g_EasyBackupInfo.nBackupContent )
			)
	{
		return CPropertyPage::OnWizardBack();
	}
	else
	{
		TRACE(L"\nYou do not choose a kind of backup content.");
		return -1;
	}
	
	// return CPropertyPage::OnWizardBack();
}

BOOL CEasyPage3::OnWizardFinish() 
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateData(TRUE);

	// if( FALSE == IsValidAMGPath((LPCTSTR)m_strTargetFile))
	if( FALSE == IsValidAMGPath(m_strTargetFile.GetBuffer(0)))
	{
        TRACE(L"\nInvalid AMG Path");
		m_strTargetFile.ReleaseBuffer();
		return -1;
	}
	m_strTargetFile.ReleaseBuffer();

	CBackupState BackupStateDlg;
	
	if (!m_bExecuting)
	{
		m_bExecuting = TRUE;
		memset(&BackupStateDlg.m_Setting.DataInfo,0,sizeof(BackupStateDlg.m_Setting.DataInfo));

		(BackupStateDlg.m_Setting.DataInfo.szComment)[0]=0;
		(BackupStateDlg.m_Setting.DataInfo.szSettingName)[0]=0;
		
		if( EASY_MY_MAIL == g_EasyBackupInfo.nBackupContent )
		{
			BackupStateDlg.m_Setting.DataInfo.wBackupType = MAIL_BACKUP; // mail backup
			if( OUTLOOK_EXPRESS_MAIL == g_EasyBackupInfo.nMailType )
			{
				if (GetOsVersion())
					BackupStateDlg.m_Setting.DataInfo.wBackupType = WIN_MAIL_BACKUP;
				else
					BackupStateDlg.m_Setting.DataInfo.wBackupType = OE_MAIL_BACKUP;
			}
		}
		else
		{
            BackupStateDlg.m_Setting.DataInfo.wBackupType = FILE_BACKUP; // normal backup
		}
		
		wcsncpy( BackupStateDlg.m_Setting.DataInfo.szTargetImage,(LPCTSTR)m_strTargetFile,MAX_PATH-1);

		// For Easy backup, there is no setting now.

		if ( GetEasySourceArray(BackupStateDlg.m_Setting.pcaSource) )
		{
			BackupStateDlg.m_Setting.DataInfo.BackupOption.bCmdLine = FALSE;
			BackupStateDlg.DoModal();
		}

		m_bExecuting = FALSE;
	}
	else
	{
		AfxMessageBox(IDS_BACKUP_RUNING,MB_OK | MB_ICONINFORMATION);
	}
	// TODO: Add your control notification handler code here

	return CPropertyPage::OnWizardFinish();
}

BOOL CEasyPage3::GetEasySourceArray(CStringArray &EasySourceArray)
{
	
	if (EasySourceArray.GetSize()) EasySourceArray.RemoveAll();

	if( 
		  ( EASY_MY_DOCUMENT != g_EasyBackupInfo.nBackupContent) &&
  	      ( EASY_MY_MAIL != g_EasyBackupInfo.nBackupContent ) &&
		  ( EASY_DOCUMENT_AND_MAIL != g_EasyBackupInfo.nBackupContent )
	  )
	{
		return FALSE;
	}

	// add the path of L"My Document"

	if( 
		( EASY_MY_DOCUMENT == g_EasyBackupInfo.nBackupContent) ||
		( EASY_DOCUMENT_AND_MAIL == g_EasyBackupInfo.nBackupContent )
	  )
    
	{
		WCHAR szMyDocPath[MAX_PATH];
		if( FALSE != GetMyDocPath(szMyDocPath) )
		{
		  EasySourceArray.Add(szMyDocPath);
		}
		else
		{
		  TRACE(L"\nGetMyDocPath failed.");
		  return FALSE;
		}
	}
	
    // add the source of L"My mail"
	if(
		 ( EASY_MY_MAIL == g_EasyBackupInfo.nBackupContent ) || 
		 ( EASY_DOCUMENT_AND_MAIL == g_EasyBackupInfo.nBackupContent )
	   )

	{
		CStringArray MailSourceArray;
		if( FALSE != GetMailSourceArray(MailSourceArray, g_EasyBackupInfo.nMailType) )
		{
			EasySourceArray.Append(MailSourceArray);
		}
		else
		{
			TRACE(L"\n GetMailSoruceArray error.");
			return FALSE;
		}
	}

	return TRUE;
}


BOOL CEasyPage3::GetMailSourceArray(CStringArray &MailSourceArray, int nMailType)
{
	
	if (MailSourceArray.GetSize()) MailSourceArray.RemoveAll();

	BOOL bGetOnePath;
	bGetOnePath=FALSE;

	if(OUTLOOK_EXPRESS_MAIL == nMailType)
	{
		WCHAR szDataPath[MAX_PATH];
		WCHAR szAddressPath[MAX_PATH];
		if( FALSE != NewGetOutlookExpressDataPath(szDataPath,szAddressPath))
		{
			if (GetOsVersion())
			{
//				wcscat(szDataPath,L"\\Local Folders");
				wcscat(szAddressPath,L"\\Contacts");
			}
			MailSourceArray.Add(szDataPath);
			MailSourceArray.Add(szAddressPath);
		    bGetOnePath=TRUE;
		}
		else
		{
			TRACE(L"\nNewGetOutlookExpressDataPath error.");
		}
	}
	else if(OUTLOOK_MAIL == nMailType)
	{
		WCHAR szOutlookDataPath[MAX_PATH];
		if( FALSE != GetOutlookDataPath(szOutlookDataPath) )
		{
			MailSourceArray.Add(szOutlookDataPath);
		    bGetOnePath=TRUE;
		}
		else
		{
			TRACE(L"\nGet OutlookDataPath Error.");
		}
	}
	else if(BECKY_MAIL == nMailType)
	{
		WCHAR szBecky2DataPath[MAX_PATH];
		if( FALSE != GetBecky2DataPath(szBecky2DataPath) )
		{
			MailSourceArray.Add(szBecky2DataPath);
		    bGetOnePath=TRUE;
		}
		else
		{
			TRACE(L"\nGetBecky2DataPath Error.");
		}
	}
	else
	{
		return FALSE;
	}

	if( FALSE == bGetOnePath)
	{
		return FALSE;
	}
	
	return TRUE;

}
