// DeleteMethodDlg.cpp : implementation file
//

#ifdef _FILE_ENCRYPT
#include "..\Encrypt\FileEncrypt\stdafx.h"
#include "..\Encrypt\FileEncrypt\resource.h"
#endif

#ifdef _SHELL_ENCRYPT
#include "..\Encrypt\ShellEncrypt\stdafx.h"
#include "..\Encrypt\ShellEncrypt\resource.h"
#endif

#ifdef FILE_DECRYPT
#include "..\Decrypt\FileDecrypt\stdafx.h"
#include "..\Decrypt\FileDecrypt\resource.h"
#endif

#ifdef SHELL_DECRYPT
#include "..\Decrypt\ShellDecrypt\stdafx.h"
#include "..\Decrypt\ShellDecrypt\resource.h"
#endif

#include "DeleteMethodDlg.h"
#include "..\EncryptToolStruct\EncryptToolStruct.h"
#include "..\Encrypt\EncryptFunction\EncryptInfo.h"
#include "..\Encrypt\EncryptFunction\EncryptFunction.h"
#include "DeleteProgressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeleteMethodDlg dialog


CDeleteMethodDlg::CDeleteMethodDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDeleteMethodDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDeleteMethodDlg)
	//}}AFX_DATA_INIT
}


void CDeleteMethodDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeleteMethodDlg)
	DDX_Control(pDX, IDC_DELETE_METHOD, m_DeleteMethod);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDeleteMethodDlg, CDialog)
	//{{AFX_MSG_MAP(CDeleteMethodDlg)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnButtonCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeleteMethodDlg message handlers

void CDeleteMethodDlg::OnButtonDelete() 
{
	CString strTitle;
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	
	CString strConfirmDelete;
	
	strConfirmDelete.LoadString(IDS_CONFIRM_DELETE_FILE);
	strTitle.LoadString(IDS_APP_NAME);
	if( IDYES == MessageBox(strConfirmDelete,strTitle,MB_YESNO | MB_ICONQUESTION) )
	{
		int nDeleteMethod = 0;
		int nSourceCount = m_SourceArray.GetSize();
		ShowWindow(FALSE);

		for(int nSourceIndex=0; nSourceIndex<nSourceCount; nSourceIndex++)
		{
			CDeleteProgressDlg DeleteProgressDlg;

			DeleteProgressDlg.SetDeleteParameter(m_SourceArray[nSourceIndex],nDeleteMethod);
			DeleteProgressDlg.DoModal();

			// here may check the parameter

			if( DeleteProgressDlg.m_bCancel)
			{
				TRACE(L"\nDeleteProcessDlg Canceled");
				break;
			}
		}
		EndDialog(TRUE);
	}
}

BOOL CDeleteMethodDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	UpdateData(FALSE); // update m_strSource
	
	// TODO: Add extra initialization here
	
//	AddDelMethod(&m_DeleteMethod);
	COMBOBOXEXITEM		cbItem;
	BYTE				i;
	CString				csMethod;

	cbItem.mask			  = CBEIF_TEXT;
	cbItem.iItem		  = 0;
	cbItem.cchTextMax	  = MAX_PATH;
	cbItem.iImage		  = 0;
	cbItem.iSelectedImage = 0;

	for(i = 0; i < 8 ; i ++)
	{
		switch(i)
		{
		case 0:
			csMethod.LoadString (IDS_00_METHOD);
			break;
		case 1:
			csMethod.LoadString (IDS_FF_METHOD);
			break;
		case 2:
			csMethod.LoadString (IDS_RANDOM_METHOD);
			break;
		case 3:
			csMethod.LoadString (IDS_NSA_METHOD);
			break;
		case 4:
			csMethod.LoadString (IDS_OLD_NSA_METHOD);
			break;
		case 5:
			csMethod.LoadString (IDS_DOD_STD_METHOD);
			break;
		case 6:
			csMethod.LoadString (IDS_NATO_METHOD);
			break;
		case 7:
			csMethod.LoadString (IDS_GUTMANN_METHOD);
			break;
		}

		WCHAR szMethod[MAX_PATH];
		memset(szMethod,0,sizeof(szMethod));
		wcsncpy(szMethod,(LPCTSTR)csMethod,MAX_PATH-1);
		cbItem.pszText = szMethod;
		cbItem.iItem  = i;
		m_DeleteMethod.InsertItem(&cbItem);
		m_DeleteMethod.SetItemData(cbItem.iItem,i);
	}
	m_DeleteMethod.SetCurSel(3);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDeleteMethodDlg::OnButtonCancel() 
{
	// TODO: Add your control notification handler code here
	EndDialog(FALSE);
}

void CDeleteMethodDlg::OnOK()
{
	// override the CDialog::OnOK()
}
