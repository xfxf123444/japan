// DeleteMethodDlg.cpp : implementation file
//

#ifdef _FILE_ENCRYPT
#include "..\FileEncrypt\stdafx.h"
#include "..\FileEncrypt\resource.h"
#endif

#ifdef _SHELL_ENCRYPT
#include "..\ShellEncrypt\stdafx.h"
#include "..\ShellEncrypt\resource.h"
#endif

#include "DeleteMethodDlg.h"
#include "..\..\EncryptToolStruct\EncryptToolStruct.h"
#include "..\EncryptFunction\EncryptInfo.h"
#include "..\EncryptFunction\EncryptFunction.h"
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
				TRACE("\nDeleteProcessDlg Canceled");
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
	
	AddDelMethod(&m_DeleteMethod);

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
