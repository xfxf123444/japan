// DelFile.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DelFile.h"
#include "Fun.h"
#include "MainDlg.h"
#include "windows.h"
#include <shlwapi.h>

WCHAR			g_szDir[MAX_PATH][MAX_PATH] = {0};
int				g_nSelItem = 0;
__int64			g_nTotalSize;
__int64			g_nDelSize;
int				g_nDelMethod;
BOOL			g_bSave;
WCHAR			g_szSaveDir[MAX_PATH];
BOOL            g_bBeepOff = FALSE;
BOOL			g_bDeleteObjMark = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CDelFileApp

BEGIN_MESSAGE_MAP(CDelFileApp, CWinApp)
	//{{AFX_MSG_MAP(CDelFileApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDelFileApp construction

CDelFileApp::CDelFileApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDelFileApp object

CDelFileApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDelFileApp initialization

BOOL CDelFileApp::InitInstance()
{
	HANDLE			hMapping;
	LPWSTR			pszCommand;
	WCHAR			*p;
	hMapping = CreateFileMapping((HANDLE)0xffffffff, NULL, PAGE_READONLY, 0, 32, L"YGDELF2004MAP");
	if(!hMapping)
	{
		ExitProcess(1);
	}

	AfxEnableControlContainer();

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
    pszCommand = GetCommandLineW();
	p = wcschr(pszCommand,L'"');

	if (wcsstr(pszCommand,L"DeleteFileInRecycled") != NULL ||
		wcsstr(pszCommand,L"Recycled") != NULL ||
		wcsstr(pszCommand,L"RECYCLER") != NULL)
	{
		g_bDeleteObjMark = TRUE;
		if (SecDeleteFileInRecycled())
		{
			SHEmptyRecycleBin(NULL,NULL,SHERB_NOCONFIRMATION|SHERB_NOPROGRESSUI|SHERB_NOSOUND);
		}
		//			HWND hDeskTop = GetDesktopWindow();
		//			UpdateWindow(hDeskTop);
		return FALSE;
	}

	if(p)
	{
		p ++;
		pszCommand = p;
		p = wcschr(pszCommand,L'"');
		if(p)
		{
			p ++;
			if (wcslen(p) > 2)
			{
				p ++;
				ParseCmdLine(p);
				return FALSE;
			}
		}	
	}
/*
	UpdateScheduleSetting();
	CMainDlg MainDlg;
	MainDlg.DoModal();
*/
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

