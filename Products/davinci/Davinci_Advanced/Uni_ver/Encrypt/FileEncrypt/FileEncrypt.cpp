// FileEncrypt.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "FileEncrypt.h"
#include "EncryptWiz_1.h"
#include "EncryptWiz_2.h"
#include "EncryptWiz_3.h"

#ifdef _DEBUG
#include "TraceWin.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileEncryptApp

BEGIN_MESSAGE_MAP(CFileEncryptApp, CWinApp)
	//{{AFX_MSG_MAP(CFileEncryptApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileEncryptApp construction

CFileEncryptApp::CFileEncryptApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CFileEncryptApp object

CFileEncryptApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CFileEncryptApp initialization

BOOL CFileEncryptApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _DEBUG
	CMfxTrace::Init();
#endif

	CPropertySheet EncryptWizard;
	CEncryptWiz_1 EncryptWiz_1;
	CEncryptWiz_2 EncryptWiz_2;
	CEncryptWiz_3 EncryptWiz_3;

	EncryptWizard.AddPage(&EncryptWiz_1);
	EncryptWizard.AddPage(&EncryptWiz_2);
	EncryptWizard.AddPage(&EncryptWiz_3);

	EncryptWizard.SetWizardMode();

	int nResponse =  EncryptWizard.DoModal();

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
