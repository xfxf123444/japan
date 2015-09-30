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

BOOL CheckRegValue()
{
	HKEY hKey;
	if (RegOpenKeyEx(HKEY_CURRENT_USER,L"Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers",0,KEY_READ,&hKey) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return FALSE;
	}
	RegCloseKey(hKey);
	return TRUE;
}

BOOL SetRegValue()
{
	HKEY hKey;
	HKEY hKeySub0;
	HKEY hKeySub1;
	DWORD dwDisPosition;
	DWORD dwType = REG_SZ;
	WCHAR chCurrentProcessName[MAX_PATH];
	WCHAR *pDest = NULL;
	WCHAR chTemp[MAX_PATH];
	WCHAR szBuf[80];

	if (RegOpenKeyEx(HKEY_CURRENT_USER,L"Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers",0,KEY_READ|KEY_WRITE,&hKey) != ERROR_SUCCESS)
	{
		if (RegOpenKeyEx(HKEY_CURRENT_USER,L"Software\\Microsoft\\Windows NT\\CurrentVersion",0,KEY_READ|KEY_WRITE,&hKey) != ERROR_SUCCESS)
		{
			return FALSE;
		}
		
		if (RegCreateKeyEx(hKey,L"AppCompatFlags",0,L"",0,KEY_READ|KEY_WRITE,NULL,&hKeySub0,&dwDisPosition) != ERROR_SUCCESS)
		{
			return FALSE;
		}
		
		if (RegCreateKeyEx(hKeySub0,L"Layers",0,L"",0,KEY_READ|KEY_WRITE,NULL,&hKeySub1,&dwDisPosition) != ERROR_SUCCESS)
		{
			return FALSE;
		}
		
		GetModuleFileName(NULL,chCurrentProcessName,MAX_PATH);
		wcscpy(szBuf,L"RUNASADMIN");
		
		if (RegSetValueEx(hKeySub1,chCurrentProcessName,0,dwType,(LPBYTE)szBuf,wcslen(szBuf)*sizeof(WCHAR)+1) != ERROR_SUCCESS)
		{
			return FALSE;
		}
		
		wcscpy(chTemp,chCurrentProcessName);
		pDest = wcsstr(chTemp,L"\\DelFile.exe");
		*pDest = L'\0';		
		wcscat(chTemp,L"\\DMFSrv.exe");
		
		if (RegSetValueEx(hKeySub1,chTemp,0,dwType,(LPBYTE)szBuf,wcslen(szBuf)*sizeof(WCHAR) + 1) != ERROR_SUCCESS)
		{
			return FALSE;
		}
		
		pDest = wcsstr(chTemp,L"\\DMFSrv.exe");
		*pDest = L'\0';	
		wcscat(chTemp,L"\\DeleteMaster.exe");
		
		if (RegSetValueEx(hKeySub1,chTemp,0,dwType,(LPBYTE)szBuf,wcslen(szBuf)*sizeof(WCHAR) + 1) != ERROR_SUCCESS)
		{
			return FALSE;
		}
		
		pDest = wcsstr(chTemp,L"\\DeleteMaster.exe");
		*pDest = L'\0';	
		wcscat(chTemp,L"\\uninst.exe");
		
		if (RegSetValueEx(hKeySub1,chTemp,0,dwType,(LPBYTE)szBuf,wcslen(szBuf)*sizeof(WCHAR) + 1) != ERROR_SUCCESS)
		{
			return FALSE;
		}
						  
		RegCloseKey(hKeySub0);
		RegCloseKey(hKeySub1);
	}

	RegCloseKey(hKey);
	return TRUE;
}

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

	WCHAR chCurrentProcessName[MAX_PATH];

    pszCommand = GetCommandLineW();
	p = wcschr(pszCommand,L' ');
	//if (!CheckRegValue())
	//{
	//	SetRegValue();
	//	GetModuleFileNameW(NULL,chCurrentProcessName,MAX_PATH);
	//	ShellExecuteW(GetFocus(),L"open",chCurrentProcessName,p,NULL,SW_SHOW);
	//	return FALSE;
	//}

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
	p = wcschr(pszCommand,L'"');
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
				ParseCmdLine(p);
				return FALSE;
			}
		}	
	}

	UpdateScheduleSetting();
	CMainDlg MainDlg;
	MainDlg.DoModal();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

