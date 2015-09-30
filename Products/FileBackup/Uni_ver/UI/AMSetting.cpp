// PSLdr.cpp : implementation file
//

#include "stdafx.h"
#include "AM01.h"
#include "AMSetting.h"
#include "WizSheet.h"
#include "BackupState.h"
#include "..\AM01Expt\AM01Expt.h"
#include "function.h"
#include "ViewSettingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern AM01_PROG_SETTING g_CurSetting;
extern AM01_PROG_SETTING *g_pSetting;
extern WCHAR g_szPath[MAX_PATH];
extern CStringArray g_SettingArray;
CString g_strSelectedSetting;
extern CSCHEDULE_ARRAY g_ScheduleArray;

/////////////////////////////////////////////////////////////////////////////
// CPLdr

IMPLEMENT_DYNAMIC(CAMSetting, CPropertyPage)

CAMSetting::CAMSetting() : CPropertyPage(CAMSetting::IDD)
{
	//{{AFX_DATA_INIT(CAMSetting)
	m_Comment = _T("");
	m_CurrentSetting = _T("");
	//}}AFX_DATA_INIT
}

CAMSetting::~CAMSetting()
{
}

void CAMSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAMSetting)
	DDX_Control(pDX, IDC_SETTING_LIST, m_SettingList);
	DDX_Text(pDX, IDC_COMMENT, m_Comment);
	DDV_MaxChars(pDX, m_Comment, 100);
	DDX_Text(pDX, IDC_CURRENT, m_CurrentSetting);
	//}}AFX_DATA_MAP
}
BEGIN_MESSAGE_MAP(CAMSetting, CPropertyPage)
    //{{AFX_MSG_MAP(CAMSetting)
	ON_BN_CLICKED(IDC_SETTING_WIZ, OnSettingWiz)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_EXECUTE, OnExecute)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_VIEW_SETTING, OnButtonViewSetting)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_EASY_BACKUP, OnButtonEasyBackup)
	ON_BN_CLICKED(IDC_CURRENT_SETTING, OnSetCurrent)
	ON_BN_CLICKED(IDC_BUTTON_SNAPSHOT, OnButtonSnapshot)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SETTING_LIST, OnItemchangedSettingList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAMSetting::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_pEasyPage1=NULL;
	m_pEasyPage2=NULL;
	m_pEasyPage3=NULL;

	m_pImageList=NULL;
	
	m_pImageList = new CImageList;

	InitSettingControl(&m_SettingList,m_pImageList);

	if(CheckSettingName(g_CurSetting.DataInfo.szSettingName))
	{
		m_CurrentSetting = g_CurSetting.DataInfo.szSettingName;
		m_Comment = g_CurSetting.DataInfo.szComment;
	}
	else
	{
		m_CurrentSetting.Empty();
		m_Comment.Empty();
		g_CurSetting.DataInfo.szSettingName[0]=L'\0';
		g_CurSetting.DataInfo.szComment[0]=L'\0';
		g_CurSetting.DataInfo.szTargetImage[0] = L'\0';
		g_CurSetting.DataInfo.wBackupType = 0;
	}

	UpdateData(FALSE);

	return TRUE;
}

void CAMSetting::OnSettingWiz() 
{
	CBitmap bmpWatermark;
	int nTotal,i;
	VERIFY(bmpWatermark.LoadBitmap(IDB_WATERMARK));
	//g_pSetting = (AM01_PROG_SETTING *)malloc(sizeof(AM01_PROG_SETTING));
	g_pSetting = new AM01_PROG_SETTING;
	if (g_pSetting)
	{
		//memset(g_pSetting,0,sizeof(AM01_PROG_SETTING));
		CWizSheet dlg(IDS_SETTING_WIZARD, NULL, 0, bmpWatermark, NULL,NULL);
		dlg.m_psh.hInstance = ::GetModuleHandle(NULL);
		if (ID_WIZFINISH == dlg.DoModal())
		{
			nTotal = m_SettingList.GetItemCount();
			i = m_SettingList.InsertItem(nTotal,g_pSetting->DataInfo.szSettingName);
	   		m_SettingList.SetItem(i,0,LVIF_STATE,L"",0,INDEXTOSTATEIMAGEMASK(2),LVIS_STATEIMAGEMASK,0);
    		
			if( FALSE == AddOneSetting(g_pSetting) )
			{
				TRACE(L"\nAddOneSetting error in OnSettingWiz.");
			}
			g_SettingArray.Add(g_pSetting->DataInfo.szSettingName);
		}
		//free(g_pSetting);
		delete g_pSetting;
		g_pSetting=NULL;  
	}
}

void CAMSetting::OnRemove() 
{
	int nTotal,i;
	int nItemIndex;
	CString strSetting;
	UpdateData(TRUE);

	nTotal = m_SettingList.GetItemCount();
	for (i = 0;i < nTotal;i ++)
	{
		if (m_SettingList.GetItemState(i,LVIS_SELECTED))
		{
			strSetting = m_SettingList.GetItemText(i,0);
			DeleteOneSetting(strSetting.GetBuffer(0));

			for(nItemIndex=g_ScheduleArray.GetSize()-1; nItemIndex>=0; nItemIndex-- )
			{
				if (!wcsicmp(g_ScheduleArray.GetAt(nItemIndex).szSettingName,strSetting.GetBuffer(0)))
				{
					g_ScheduleArray.RemoveAt(nItemIndex);
				}
			}
			RecordSchedule(&g_ScheduleArray);

			m_SettingList.DeleteItem(i);
			
			if (strSetting == m_CurrentSetting)
			{
				m_CurrentSetting = L"";
				g_CurSetting.DataInfo.szComment[0] = L'\0';
				g_CurSetting.DataInfo.szSettingName[0] = L'\0';
				g_CurSetting.DataInfo.szTargetImage[0] = L'\0';
				g_CurSetting.DataInfo.wBackupType = 0;
				UpdateData(FALSE);
			}

			for (i = 0;i < nTotal;i++)
			{
				if (g_SettingArray.ElementAt(i) == strSetting)
				{
					g_SettingArray.RemoveAt(i,1);
					break;
				}
			}
			break;
		}
	}
	// TODO: Add your control notification handler code here
	
}

void CAMSetting::OnExecute() 
{
	WCHAR szSelectedSetting[MAX_SETTING_NAME_LEN];
	
	if( FALSE == GetSelectedSetting(&m_SettingList,szSelectedSetting) )
	{
		TRACE(L"\nGetSeelectedSetting error in CAMSetting::OnExecute.");
		return;
	}

	ExecuteOneSetting(szSelectedSetting);

}

void CAMSetting::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	m_SettingList.DeleteAllItems();
	delete m_pImageList;
	m_pImageList = NULL;
	CPropertyPage::OnClose();
}

BOOL CAMSetting::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	RefreshSetting();
	return CPropertyPage::OnSetActive();
}


void CAMSetting::OnButtonViewSetting() 
{
	// TODO: Add your control notification handler code here

	int nTotal,i;
	
	CString strOriginalSelectedSetting;

	g_strSelectedSetting.Empty();
	
	BOOL bOneItemSelected;
	bOneItemSelected = FALSE;

	nTotal = m_SettingList.GetItemCount();
	for (i = 0;i < nTotal; i++)
	{
		if (m_SettingList.GetItemState(i,LVIS_SELECTED))
		{
			strOriginalSelectedSetting = m_SettingList.GetItemText(i,0);
			bOneItemSelected = TRUE;
			break;
		}
	}
	// TODO: Add your control notification handler code here

	if( FALSE != bOneItemSelected )
	{
	    CViewSettingDlg OneViewSettingDlg;
	    
		// copy the selected setting to g_strSelectedSetting
		// then it can be used in the other dialog

		g_strSelectedSetting = strOriginalSelectedSetting;
		OneViewSettingDlg.DoModal();

		CString strNewSettingName;
		int   nItemIndex;

		strNewSettingName = g_strSelectedSetting;

		if( 0 != strOriginalSelectedSetting.Compare(strNewSettingName) )
		{
			for( nItemIndex=0; nItemIndex<g_ScheduleArray.GetSize(); nItemIndex++ )
			{
				if (!wcsicmp(g_ScheduleArray.GetAt(nItemIndex).szSettingName,strOriginalSelectedSetting))
				{
					wcsncpy(g_ScheduleArray[nItemIndex].szSettingName,strNewSettingName,MAX_SETTING_NAME_LEN);
				}
			}
			RecordSchedule(&g_ScheduleArray);
		}

		RefreshSetting(); 
		
		m_SettingList.SetItemState(i,LVIS_SELECTED ,LVIS_SELECTED);
	}
	else
	{
		AfxMessageBox(IDS_NO_SETTING_SELECTED,MB_OK|MB_ICONINFORMATION,NULL);
	}

}


void CAMSetting::OnDestroy() 
{
	// TODO: Add your message handler code here
	
	if( NULL != m_pImageList)
	{
	  delete m_pImageList;
	  m_pImageList = NULL;
	}

	CPropertyPage::OnDestroy();
}

BOOL CAMSetting::RefreshSetting()
{
	return ( RefreshSettingInfo(&g_SettingArray,&m_SettingList) );
}

void CAMSetting::OnButtonEasyBackup() 
{
	// TODO: Add your control notification handler code here
   // Create the wizard
   m_pEasyWizard = new CEasyWizard(L"Easy backup wizard", this);
   ASSERT_VALID(m_pEasyWizard);

   // Construct the property pages
   m_pEasyPage1 = new CEasyPage1;
   ASSERT_VALID(m_pEasyPage1);
   m_pEasyPage1->Construct(IDD_EASY_WIZARD_1, 0);

   m_pEasyPage2 = new CEasyPage2;
   ASSERT_VALID(m_pEasyPage2);
   m_pEasyPage2->Construct(IDD_EASY_WIZARD_2, 0);

   m_pEasyPage3 = new CEasyPage3;
   ASSERT_VALID(m_pEasyPage3);
   m_pEasyPage3->Construct(IDD_EASY_WIZARD_3, 0);


   // Add the property pages to the property sheet
   m_pEasyWizard->AddPage(m_pEasyPage1);
   m_pEasyWizard->AddPage(m_pEasyPage2);
   m_pEasyWizard->AddPage(m_pEasyPage3);


   // Make the property sheet a Wizard
   m_pEasyWizard->SetWizardMode();   

   // Display the Wizard
   m_pEasyWizard->DoModal();

   if( NULL != m_pEasyPage1 )
   {
		delete m_pEasyPage1;
		m_pEasyPage1=NULL;
   }

   if( NULL != m_pEasyPage2)
   {
		delete m_pEasyPage2;
		m_pEasyPage2=NULL;
   }

   if( NULL != m_pEasyPage3)
   {
		delete m_pEasyPage3;
		m_pEasyPage3=NULL;
   }

   if( NULL != m_pEasyWizard )
   {
	  delete m_pEasyWizard;
	  m_pEasyWizard=NULL;

   }
}


void CAMSetting::OnSetCurrent() 
{
	// TODO: Add your control notification handler code here

	WCHAR szSelectedSetting[MAX_SETTING_NAME_LEN];
	if( FALSE == GetSelectedSetting(&m_SettingList,szSelectedSetting) )
	{
		TRACE(L"\nGetSelectedSetting error in CAMSetting::OnSetCurrent()");
		return;
	}

	if (SetCurrentSetting(szSelectedSetting))
	{
		m_CurrentSetting = szSelectedSetting;
		UpdateData(FALSE);
	}
	else
	{
		AfxMessageBox(IDS_FAILER_SET_CURRENT,MB_OK | MB_ICONINFORMATION);
	}
}

void CAMSetting::OnButtonSnapshot() 
{
	// TODO: Add your control notification handler code here
	WCHAR szSelectedSetting[MAX_SETTING_NAME_LEN];

	if(GetSelectedSetting(&m_SettingList,szSelectedSetting) )
	{
		ExecuteSettingNewBase(szSelectedSetting);
	}
}	



void CAMSetting::OnItemchangedSettingList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here

	CString strSetting;

	WCHAR szSelectedSetting[MAX_SETTING_NAME_LEN];
	if(FALSE == GetSelectedSetting(&m_SettingList,szSelectedSetting))
	{
		// TRACE(L"\nGetSelectedSetting Error in CAMSetting::OnItemchangedSettingList.");
		return;
	}
		
	if (GetSettingInfo(szSelectedSetting,&m_CurSetting))
	{
		m_Comment = m_CurSetting.DataInfo.szComment;
		UpdateData(FALSE);
	}

	*pResult = 0;
}

// if the current setting is deleted, then do not show the current setting
BOOL CAMSetting::CheckSettingName(WCHAR *szSettingName)
{
	BOOL bSettingNameExist;
	bSettingNameExist = FALSE;

	if(FALSE == IsSettingNameExist(szSettingName,&bSettingNameExist))
	{
		TRACE(L"\nIsSettingNameExist function error in DeleteOneSetting");
		return FALSE;
	}

	if(bSettingNameExist)
	{
		return TRUE;
	}

	return FALSE;
}
