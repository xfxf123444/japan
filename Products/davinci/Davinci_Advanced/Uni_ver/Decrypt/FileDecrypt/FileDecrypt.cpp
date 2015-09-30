// FileDecrypt.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "FileDecrypt.h"

#include "DecryptWiz_1.h"
#include "DecryptWiz_2.h"
#include "DecryptWiz_3.h"
#include "DecryptWiz_4.h"
#include "DecryptWiz_5.h"

#ifdef _DEBUG
#include "TraceWin.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileDecryptApp

BEGIN_MESSAGE_MAP(CFileDecryptApp, CWinApp)
	//{{AFX_MSG_MAP(CFileDecryptApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileDecryptApp construction

CFileDecryptApp::CFileDecryptApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CFileDecryptApp object

CFileDecryptApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CFileDecryptApp initialization

BOOL CFileDecryptApp::InitInstance()
{

#ifdef _DEBUG
	CMfxTrace::Init();
#endif
	if (!AfxOleInit())	return FALSE;
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	// CFileDecryptDlg dlg;
	// m_pMainWnd = &dlg;
	// int nResponse = dlg.DoModal();

	WCHAR buf[100];
	CString s;
	ZeroMemory(buf, 200);
	wcscpy(buf, (const WCHAR*)s);
	
	//s = buf;
	int i = 0;
	CPropertySheet DecryptWizard;
	CDecryptWiz_1 DecryptWiz_1;
	CDecryptWiz_2 DecryptWiz_2;
	CDecryptWiz_3 DecryptWiz_3;
	CDecryptWiz_4 DecryptWiz_4;
	CDecryptWiz_5 DecryptWiz_5;

	DecryptWizard.AddPage(&DecryptWiz_1);
	DecryptWizard.AddPage(&DecryptWiz_2);
	DecryptWizard.AddPage(&DecryptWiz_3);
	DecryptWizard.AddPage(&DecryptWiz_4);
	DecryptWizard.AddPage(&DecryptWiz_5);

	DecryptWizard.SetWizardMode();

	int nResponse =  DecryptWizard.DoModal();

	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
