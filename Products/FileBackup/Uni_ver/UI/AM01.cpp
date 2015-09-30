// AM01.cpp : Defines the class behaviors for the application.
//
#include "stdafx.h"
#include "AFXCMN.H"
#include <afxpriv.h>
#include <shellapi.h>   // for SHFileOperation
#include <shlobj.h>     // for SHBroweForFolder
#include <OBJBASE.H>
#include <tlhelp32.h> 
#include "AM01.h"
#include "splash.h"
#include "MainWnd.h"
#include "..\AM01Expt\AM01Expt.h"
#include "ShutdownDlg.h"
#include "function.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern	    CSplash		    *g_pSplash;

DWORD g_dwPlatformVer;

// AM01TASK g_TaskInfo;

AM01_PROG_SETTING g_CurSetting;

WCHAR g_szPath[MAX_PATH];
/////////////////////////////////////////////////////////////////////////////
// CAM01App

BEGIN_MESSAGE_MAP(CAM01App, CWinApp)
	//{{AFX_MSG_MAP(CAM01App)
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
// The one and only CAM01App object

CAM01App theApp;

/////////////////////////////////////////////////////////////////////////////
// CAM01App construction

CAM01App::CAM01App()
{
}

void CAM01App::CloseSplash()
{
	if (g_pSplash)
	{
		delete g_pSplash;
        g_pSplash = NULL;
	}
}

BOOL CAM01App::InitInstance()
{
	HANDLE	hMapping;
	BOOL    bPrevInstance = FALSE;
	WNDCLASS wndcls;
    OSVERSIONINFOEX osvi;

    // Try calling GetVersionEx using the OSVERSIONINFOEX structure.
    // If that fails, try using the OSVERSIONINFO structure.

 	g_pSplash = NULL;
	
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
   
	hMapping = CreateFileMapping((HANDLE)0xffffffff, NULL, PAGE_READONLY, 0, 32, L"AM01MAP");
	if(hMapping)
	{
		if( GetLastError() == ERROR_ALREADY_EXISTS )
			bPrevInstance = TRUE;

		// here I think should close handle of hMapping
		// CloseHandle(hMapping);
	}
	else
	{
		ExitProcess(1);
	}

#ifdef _NO_SPLASH
#else
	g_pSplash = new CSplash();
    g_pSplash->ShowWindow(SW_SHOW);
	g_pSplash->UpdateWindow();
#endif
	
	if (!bPrevInstance)
	{
		OnFileNew();	// do this so theApp window handle is valid

        memset(&wndcls, 0, sizeof(WNDCLASS));   // start with NULL
                                                // defaults
        wndcls.style = CS_BYTEALIGNWINDOW;
        wndcls.lpfnWndProc = ::DefWindowProc;
        wndcls.hInstance = AfxGetInstanceHandle();
        wndcls.hIcon = LoadIcon(IDI_ICON_MAIN); // or load a different
                                                  // icon
        wndcls.hCursor = LoadCursor(IDC_ARROW_NORMAL);
        wndcls.hbrBackground = 0;
        wndcls.lpszMenuName = NULL;
        wndcls.lpszClassName = MaClassName;

        if(!RegisterClass(&wndcls)) 
		{
    		return FALSE;
		} 
		GetModuleFileName(NULL,g_szPath,MAX_PATH);

		*strrchrpro(g_szPath,L'\\')=L'\0';
		
		//memset(&g_CurSetting,0,sizeof(g_CurSetting));
		g_CurSetting.Clear();
		GetCurrentSetting(&g_CurSetting);
		
		// to avoid memery leak
		m_pSaveMainWnd = new CMainWnd();

		m_pMainWnd = m_pSaveMainWnd;
		
		m_pMainWnd->ShowWindow(SW_HIDE);
		m_pMainWnd->UpdateWindow();
				
		return TRUE;
	}
	else
	{
		HWND  hWnd = FindWindow(MaClassName,NULL);
		if (hWnd) PostMessage(hWnd,WM_COMMAND,ID_MAIN_CONTROL,0);
	}

	return FALSE;
}

int CAM01App::ExitInstance()
{
	if( NULL != m_pSaveMainWnd)
	{
        delete m_pSaveMainWnd;
		m_pSaveMainWnd = NULL;
	}

#ifdef _NO_SPLASH
#else
	if( NULL != g_pSplash )
	{
		TRACE(L"\nDelete g_pSplash in CAM01App.");
		delete g_pSplash;
		g_pSplash=NULL;
	}
#endif

	return 1;
}

BOOL SelectFile(WCHAR *szFileExt,WCHAR *szFileType,WCHAR *szFile)
{
	BOOL bResult = FALSE;
	CString strFilter;
	strFilter.Format(L"%s (*.%s)|*.%s||",szFileType,szFileExt,szFileExt);

	CFileDialog dlg (TRUE, szFileExt, NULL, OFN_HIDEREADONLY, strFilter);

	// if user selected a file then open it as a database
	if (IDOK == dlg.DoModal())
	{
		wcscpy(szFile,dlg.GetPathName());
		bResult = TRUE;
	}
	return bResult;
}

BOOL CheckConflict(WCHAR *szSource,WCHAR *szTargetImg,BOOL bImage)
{
	WCHAR szTemp[MAX_PATH];
	WORD wSourceLen,wTargetLen;

	if (bImage)
	{
		if (!IsValidAMGPath(szTargetImg))
		{
			return FALSE;
		}
	}
	if (!wcsicmp(szSource,szTargetImg)) 
	{
		return TRUE;
	}

	wcscpy(szTemp,szTargetImg);
	if (bImage)
	{
		*(strrchrpro(szTemp,L'\\')+1) = L'\0';
	}
	wSourceLen = wcslen(szSource);
	wTargetLen = wcslen(szTemp);
	if (wSourceLen > wTargetLen) return FALSE;
	szTemp[wSourceLen] = L'\0';
	if (wcsicmp(szTemp,szSource)) return FALSE;

	// what is the meaning?
	if (szTargetImg[wSourceLen] == L'\\' || szTargetImg[wSourceLen-1] == L'\\') return TRUE;
	return FALSE;
}

BOOL GetCurrentSetting(AM01_PROG_SETTING *pSetting)
{
	HKEY	hKey;
	WCHAR    szSetting[MAX_SETTING_NAME_LEN];
	BOOL    bResult = FALSE;
	DWORD	dwSize,dwType;

	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,AMSOFTWAREKEY,0,KEY_QUERY_VALUE,&hKey) == ERROR_SUCCESS)
	{
		dwSize = sizeof(szSetting);
		bResult = (RegQueryValueEx(hKey,L"CurrentSetting",NULL,&dwType,(LPBYTE)szSetting,&dwSize) == ERROR_SUCCESS);
		if (bResult)
		{
			bResult = GetSettingInfo(szSetting,pSetting);
		}
		else
		{
			TRACE(L"\nRegQueryValueEx error in GetCurrentSetting.");
		}
		RegCloseKey(hKey);
	}
	else
	{
		TRACE(L"\nRegOpenKeyEx error in GetCurrentSetting.");
	}

	return bResult;
}

BOOL SetCurrentSetting(WCHAR *szSetting)
{
	HKEY	hKey;
	BOOL    bResult = FALSE;
	DWORD	dwSize;

	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,AMSOFTWAREKEY,0,KEY_SET_VALUE,&hKey) == ERROR_SUCCESS)
	{
		dwSize = (wcslen(szSetting)+1) * sizeof(WCHAR);
		bResult = (RegSetValueEx(hKey,L"CurrentSetting",0,REG_SZ,(LPBYTE)szSetting,dwSize) == ERROR_SUCCESS);
		RegCloseKey(hKey);
	}
	else
	{
		TRACE(L"\nRegOpenKeyEx error in SetCurrentSetting.");
	}
	
	if (bResult) 	bResult = GetSettingInfo(szSetting,&g_CurSetting);

	return bResult;
}

BOOL GetBecky2DataPath(WCHAR *szPath)
{
	HKEY	hKey;
	BOOL    bResult = FALSE;
	DWORD	dwSize,dwType;

	if(RegOpenKeyEx(HKEY_CURRENT_USER,BECKY2DATAKEY,0,KEY_QUERY_VALUE,&hKey) == ERROR_SUCCESS)
	{
		dwSize = MAX_PATH * sizeof(WCHAR);
		bResult = (RegQueryValueEx(hKey,L"DataDir",NULL,&dwType,(LPBYTE)szPath,&dwSize) == ERROR_SUCCESS);
		// dwSize = wcslen(szPath) - 1;
		// if (szPath[dwSize] == '\\') szPath[dwSize] = '\0';
		
		TrimRightChar(szPath,L'\\',MAX_PATH);

		if (bResult) bResult = (GetFileAttributes(szPath) != -1);
		RegCloseKey(hKey);
	}

	return bResult;
}
BOOL BinaryToText(LPBYTE pBinary,WCHAR *pText,ULONG ulLen)
{
	ULONG i;
	ulLen=16;
	 
	for (i = 0;i < ulLen;i++)
	{
		wprintf(&pText[i*2],L"%2.2x",pBinary[i]);
	}
	return TRUE;
}

DWORD GetPlatformVersion()
{
	DWORD dwPlatformVersion;
    OSVERSIONINFOEX osvi;

    // Try calling GetVersionEx using the OSVERSIONINFOEX structure.
    // If that fails, try using the OSVERSIONINFO structure.

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    if( !GetVersionEx ((OSVERSIONINFO *) &osvi))
    {
        // If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.
        osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
        if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
		{
           return 0;
		}
    }
	
	dwPlatformVersion = osvi.dwPlatformId;

	return dwPlatformVersion;

}

BOOL GetOutlookDataPath(WCHAR *szPath)
{
	HKEY hKey;
	WCHAR szKeyName[MAX_PATH],szTempKey[MAX_PATH];
	WCHAR szItemValue[MAX_PATH];
	DWORD dwPlatformVersion,cbData;
			
	dwPlatformVersion = GetPlatformVersion();

	switch (dwPlatformVersion)
	{
	case VER_PLATFORM_WIN32_NT:
		wcscpy(szKeyName,OUTLOOK_ACCOUNT_REG);
		break;
	case VER_PLATFORM_WIN32_WINDOWS:
		wcscpy(szKeyName,OUTLOOK_ACCOUNT_REG_9X);
		break;
	default:
		return FALSE;
	}

	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER,szKeyName,0,KEY_READ,&hKey)) return FALSE;

	cbData = MAX_PATH * sizeof(WCHAR);
	if (ERROR_SUCCESS != RegQueryValueEx(hKey,L"DefaultProfile",NULL,NULL,(unsigned char*)szItemValue,&cbData))
	{//szItemValue=outlook
		RegCloseKey(hKey);
		return FALSE;
	}
	RegCloseKey(hKey);

	wcscat(szKeyName,L"\\");
	wcscat(szKeyName,szItemValue);
	wcscat(szKeyName,L"\\");

	wcscpy(szTempKey,szKeyName);
	wcscat(szTempKey,OUTLOOK_MASTER_KEY);
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER,szTempKey,0,KEY_READ,&hKey)) return FALSE;
	cbData = MAX_PATH * sizeof(WCHAR);
	if (ERROR_SUCCESS != RegQueryValueEx(hKey,L"01023d00",NULL,NULL,(unsigned char*)szItemValue,&cbData))
	{
		RegCloseKey(hKey);
		return FALSE;
	}
	RegCloseKey(hKey);
	BinaryToText((LPBYTE)szItemValue,&szKeyName[wcslen(szKeyName)],cbData);

	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER,szKeyName,0,KEY_READ,&hKey)) return FALSE;
	cbData = MAX_PATH * sizeof(WCHAR);
	if (ERROR_SUCCESS != RegQueryValueEx(hKey,L"001e6700",NULL,NULL,(unsigned char*)szPath,&cbData))
	{
		//cbData = MAX_PATH;
		//if (ERROR_SUCCESS != RegQueryValueEx(hKey,L"001f6700",NULL,NULL,(unsigned char*)szItemValue,&cbData))
		//{
		//	RegCloseKey(hKey);
		//	return FALSE;
		//}
		//WideCharToMultiByte(CP_ACP,0,(WCHAR *)szItemValue,cbData,szPath,cbData,NULL,NULL);
		//szPath[cbData] = 0;
	}
	RegCloseKey(hKey);

	TRACE(L"\nszPath is: %s in GetOutlookDataPath",szPath);

	return TRUE;
}

BOOL ShutdownSystem()
{
	// give the user a chance to cancel
	CShutdownDlg ShutdownDlg;
	ShutdownDlg.SetShutdownSecond(SHUTDOWN_NOTICE_TIME);
	ShutdownDlg.m_bShutdown = FALSE;

	ShutdownDlg.DoModal();
	
	if( FALSE == ShutdownDlg.m_bShutdown )
	{
		return FALSE;
	}

	if (g_dwPlatformVer == VER_PLATFORM_WIN32_NT)
	{
		HANDLE hToken; 
		TOKEN_PRIVILEGES tkp; 
 
	   // Get a token for this process. 
		 
		if (!OpenProcessToken(GetCurrentProcess(), 
			TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
			return( FALSE ); 
 
	   // Get the LUID for the shutdown privilege. 
 
		LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, 
			&tkp.Privileges[0].Luid); 
 
		tkp.PrivilegeCount = 1;  // one privilege to set    
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
 
	   // Get the shutdown privilege for this process. 
 
		AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, 
			 (PTOKEN_PRIVILEGES)NULL, 0); 
 
		if (GetLastError() != ERROR_SUCCESS) 
		   return FALSE; 

	    return (ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, 0)); 
	} 
	else
	{
	    HANDLE         hSnapShot;
		PROCESSENTRY32 procentry;
		BOOL           bFlag;
	    // Get a handle to a ToolHelp snapshot of the systems
        // processes.
        hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0 );
        if( hSnapShot == INVALID_HANDLE_VALUE )
            return FALSE;

        // Get the first process's information.

        procentry.dwSize = sizeof(PROCESSENTRY32);
        bFlag = Process32First( hSnapShot, &procentry );

        // While there are processes, keep looping.
        while( bFlag )
		{
            HANDLE hProcess;
            INT nPos;

            nPos = lstrlen(procentry.szExeFile);

            if (nPos)
			{
				
				WCHAR szRightPart[MAX_PATH];
				memset(szRightPart,0,sizeof(szRightPart));
				GetRightPart(procentry.szExeFile,szRightPart);


				if(!lstrcmpi(L"explorer.exe",szRightPart))
				{
                     // Terminate the process.
                    hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,
                                           procentry.th32ProcessID);
                    TerminateProcess(hProcess, 1);
                    CloseHandle(hProcess);
				}


			}

            procentry.dwSize = sizeof(PROCESSENTRY32) ;
            bFlag = Process32Next( hSnapShot, &procentry );
		}
        CloseHandle(hSnapShot);
	    return (ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0)); 
	}
}

BOOL IsValidAMGPath(WCHAR *szPath)
{
	WCHAR szTemp[MAX_PATH];
	WCHAR *p;
	DWORD dwFileAttr;
	WORD  wLen;

	dwFileAttr = GetFileAttributes(szPath);
	
	// if szPath is a directory then retur FALSE
	if ((dwFileAttr != -1) && (dwFileAttr & FILE_ATTRIBUTE_DIRECTORY)) return FALSE;

	wLen = wcslen(szPath);
	if (wLen <= wcslen(L"C:\\.AMG")) return FALSE;
	
	// should have a '.'
	
	p=strrchrpro(szPath,L'.');

	if (!p) return FALSE;
	
	// if the last three letter is not .amg return false
	if (wcsicmp(p,L".AMG")) return FALSE;

	// wcscpy(szTemp,szPath);
	wcsncpy(szTemp,szPath,MAX_PATH-1);

	p = strrchrpro(szTemp,L'\\');
	
	if (!p) return FALSE;
	
	//cut of the string
	*p = L'\0';

	//create the Directory if needed
	
	return CreateOneDir(szTemp);
}

BOOL IsValidRMIMGPath(WCHAR *szPath)
{
	WCHAR szTemp[MAX_PATH];
	WCHAR *p;
	DWORD dwFileAttr;
	WORD  wLen;

	dwFileAttr = GetFileAttributes(szPath);
	if ((dwFileAttr != -1) && (dwFileAttr & FILE_ATTRIBUTE_DIRECTORY)) return FALSE;

	wLen = wcslen(szPath);
	if (wLen <= wcslen(L"C:\\.IMG")) return FALSE;
	
	p = strrchrpro(szPath,L'.');
	
	if (!p) return FALSE;
	if (wcsicmp(p,L".IMG")) return FALSE;
	wcsncpy(szTemp,szPath,MAX_PATH-1);
	
	p = strrchrpro(szTemp,L'\\');
	
	if (!p) return FALSE;
	*p = L'\0';
	return TRUE;
}

