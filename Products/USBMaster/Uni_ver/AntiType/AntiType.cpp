// AntiType.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "AntiType.h"
#include "AntiTypeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef

BOOL (WINAPI * PCheckTokenMembership)(
  HANDLE TokenHandle,  // handle to access token
  PSID SidToCheck,     // SID
  PBOOL IsMember       // result
);

BOOL IsUserAdmin(VOID) 
{
	BOOL						b;
	PSID						AdministratorsGroup; 
	SID_IDENTIFIER_AUTHORITY	NtAuthority = SECURITY_NT_AUTHORITY;
	HMODULE						hModule;
	PCheckTokenMembership		func;
	
	b = AllocateAndInitializeSid(&NtAuthority,
									2,
									SECURITY_BUILTIN_DOMAIN_RID,
									DOMAIN_ALIAS_RID_ADMINS,
									0, 0, 0, 0, 0, 0,
									&AdministratorsGroup); 
	if(b)
	{
		b = FALSE;
		hModule = GetModuleHandle("advapi32.dll");
		func = (PCheckTokenMembership)GetProcAddress(hModule,"CheckTokenMembership");
		if( func != NULL) 
	    {
			func( NULL, AdministratorsGroup, &b);		     
		} 
		FreeSid(AdministratorsGroup); 
	}else
	{
		if( GetLastError() == ERROR_CALL_NOT_IMPLEMENTED )
		{
			//win9x
			b = TRUE;
		}
	}

	return(b);
}

/////////////////////////////////////////////////////////////////////////////
// CAntiTypeApp

BEGIN_MESSAGE_MAP(CAntiTypeApp, CWinApp)
	//{{AFX_MSG_MAP(CAntiTypeApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
//	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAntiTypeApp construction

CAntiTypeApp::CAntiTypeApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAntiTypeApp object

CAntiTypeApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAntiTypeApp initialization

BOOL CAntiTypeApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

//#ifdef _AFXDLL
//	Enable3dControls();			// Call this when using MFC in a shared DLL
//#else
//	Enable3dControlsStatic();	// Call this when linking to MFC statically
//#endif
	if (!IsUserAdmin())
	{
		AfxMessageBox(IDS_NEED_ADMIN);
		return FALSE;
	}

	CAntiTypeDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
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

BOOL NormalKeyToDESKey(char *szKey,UCHAR *ucDesKey)
{
	memset(ucDesKey,0,24);
	strncpy((char *)ucDesKey,szKey,24);
	return TRUE;
}