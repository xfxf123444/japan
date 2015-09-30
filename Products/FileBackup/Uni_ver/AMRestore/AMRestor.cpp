// Restore.cpp : Defines the class behaviors for the application.
//
#include "stdafx.h"
#include "AMRestor.h"
#include "wizsheet.h"

#include "..\AM01Struct\AM01Struct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

DWORD g_dwPlatformVer;
AM_RESTORE_INFO g_RestoreInfo;

/////////////////////////////////////////////////////////////////////////////
// CAMRestorApp

BEGIN_MESSAGE_MAP(CAMRestorApp, CWinApp)
	//{{AFX_MSG_MAP(CAMRestorApp)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)

	// Global Help Commands
	ON_COMMAND(ID_HELP_INDEX, CWinApp::OnHelpIndex)
	ON_COMMAND(ID_HELP_USING, CWinApp::OnHelpUsing)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CWinApp::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CWinApp::OnHelpIndex)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// The one and only CAMRestorApp object

CAMRestorApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAMRestorApp construction

CAMRestorApp::CAMRestorApp()
{
}


CAMRestorApp::~CAMRestorApp()
{
}

BOOL CAMRestorApp::InitInstance()
{
	CBitmap bmpWatermark;
	HANDLE	hMapping;
	BOOL    bPrevInstance = FALSE;
    OSVERSIONINFOEX osvi;
	int nResponse;
	
    // Try calling GetVersionEx using the OSVERSIONINFOEX structure.
    // If that fails, try using the OSVERSIONINFO structure.

    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    if( !GetVersionEx ((OSVERSIONINFO *) &osvi))
    {
        // If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.
        osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
        if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
           return FALSE;
    }
	g_dwPlatformVer = osvi.dwPlatformId;
   
	hMapping = CreateFileMapping((HANDLE)0xffffffff, NULL, PAGE_READONLY, 0, 32, L"AM01RESTOREMAP");
	if(hMapping)
	{
		if( GetLastError() == ERROR_ALREADY_EXISTS )
			bPrevInstance = TRUE;
	}
	else
	{
		ExitProcess(1);
	}

	
	if (!bPrevInstance)
	{
		OnFileNew();	// do this so theApp window handle is valid

		switch (g_dwPlatformVer)
		{
			// Tests for Windows NT product family.
			case VER_PLATFORM_WIN32_NT:
				break;
			case VER_PLATFORM_WIN32_WINDOWS:
				break;
		}

		// g_nRestoreType = 0;
		g_RestoreInfo.nRestoreType=RESTORE_ENTIRE;
		
		VERIFY(bmpWatermark.LoadBitmap(IDB_WATERMARK));

		CWizSheet dlg(IDS_SETTING_WIZARD, NULL, 0, bmpWatermark, NULL,NULL);

		dlg.m_psh.hInstance = ::GetModuleHandle(NULL);
		
		//-- for debug begin
		dlg.m_psh.dwFlags &= ~(PSH_HASHELP);
		//-- for debug end

		nResponse = dlg.DoModal();

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
	}

	return FALSE;
}
