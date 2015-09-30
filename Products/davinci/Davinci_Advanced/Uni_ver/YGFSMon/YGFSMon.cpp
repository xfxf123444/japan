// magnaram.cpp : Defines the class behaviors for the application.
//
#include "stdafx.h"
#include "AFXCMN.H"
#include <afxpriv.h>
#include "YGFSMon.h"
#include "MainWnd.h"
#include "..\EncryptToolStruct\EncryptToolStruct.h"
#include "..\Decrypt\DecryptFunction\DecryptFunction.h"
#include "YGFSMonConst.h"
#include "Function.h"

#ifdef NO_DRIVER_MODE
vector<CONVERT_BUF> g_MonitorPathSet;
map<CString, ExcludeFileInfo> g_ExcludeFileSet;
CRITICAL_SECTION g_csUpdate;
CRITICAL_SECTION g_csEncrypt;
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CMONITOR_ITEM_ARRAY g_MonitorItemArray;

HANDLE  g_hYGFSMonDriver = INVALID_HANDLE_VALUE;
DWORD   g_dwPlatformVer;
PCONVERT_BUF        MonitorFileList = NULL;

/////////////////////////////////////////////////////////////////////////////
// CYGFSMonApp

BEGIN_MESSAGE_MAP(CYGFSMonApp, CWinApp)
//{{AFX_MSG_MAP(CYGFSMonApp)
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
// The one and only CYGFSMonApp object

CYGFSMonApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CYGFSMonApp construction

CYGFSMonApp::CYGFSMonApp()
{
}


CYGFSMonApp::~CYGFSMonApp()
{
}

BOOL CYGFSMonApp::InitInstance()
{
#ifdef _DEBUG
//	CMfxTrace::Init();
#endif
	HANDLE	hMapping;
#ifdef NO_DRIVER_MODE
	InitializeCriticalSection(&g_csUpdate);
	InitializeCriticalSection(&g_csEncrypt);
#endif
	BOOL    bPrevInstance = FALSE;
	WNDCLASS wndcls;
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
			return FALSE;
    }
	g_dwPlatformVer = osvi.dwPlatformId;
	hMapping = CreateFileMapping((HANDLE)0xffffffff, NULL, PAGE_READONLY, 0, 32, L"YGFSMONMAP");
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
		
        memset(&wndcls, 0, sizeof(WNDCLASS));   // start with NULL
		// defaults
        wndcls.style = CS_BYTEALIGNWINDOW;
        wndcls.lpfnWndProc = ::DefWindowProc;
        wndcls.hInstance = AfxGetInstanceHandle();
        wndcls.hIcon = LoadIcon(IDI_ICON_MAIN); // or load a different
		// icon
		wndcls.hCursor = LoadCursor(IDC_ARROW_NORMAL);
        wndcls.hbrBackground = (HBRUSH)::GetStockObject(WHITE_BRUSH);
        wndcls.lpszMenuName = NULL;
        wndcls.lpszClassName = L"ClassName";
		
        if(!RegisterClass(&wndcls)) 
		{
			return FALSE;
		}
		
		//WCHAR szPath[MAX_PATH];
		//ZeroMemory(szPath, MAX_PATH * sizeof(WCHAR));
		//GetModuleFileName(AfxGetApp()-> m_hInstance,szPath,MAX_PATH);
		//swprintf(szPath, L"\\\\.\\%c:", szPath[0]);
		// swprintf(szPath, L"\\\\.\\PhysicalDrive0");
		// swprintf(szPath, L"\\\\.\\YGFSMon");
		switch (g_dwPlatformVer)
		{
			// Tests for Windows NT product family.
		case VER_PLATFORM_WIN32_NT:
			
#ifdef NO_DRIVER_MODE
#else
			g_hYGFSMonDriver = CreateFile(L"\\\\.\\YGFSMon",GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,OPEN_EXISTING,0,NULL);
			if( INVALID_HANDLE_VALUE == g_hYGFSMonDriver)
			{
				TRACE(L"\nCreateFile error in InitInstance.");
				return FALSE;
			}
#endif

			break;
		default:
			g_hYGFSMonDriver = INVALID_HANDLE_VALUE;
			break;
		}
		
		if (YGFSMonCheckVersion())
		{
			if( FALSE == InitDavinciStatus() )
			{
				TRACE(L"\nInitDavinciStatus error in Initinstance.");
			}
			
			if( FALSE == m_MainWnd.CreateEx(0,
				L"ClassName",
				L"WindowName",
				WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				NULL,
				NULL,
				NULL) )
			{
				TRACE(L"\nCreateEx error.");
				return FALSE;
			}
			
			if( FALSE == YGFSMonStartWork() )
			{
				TRACE(L"\nYGFSMonStartWork error in InitInstance.");
			}
			else
			{
#ifdef _DEBUG
#ifndef NO_DRIVER_MODE

				YGFSMonGetMonitorDrive();
				// TRACE(L"\nYGFSMonStartWork succeed in InitInstance.");
#endif
#endif
			}
		}
		else
		{
			TRACE(L"\nYGFSMonCheckVersion error, maybe drive not installed.");
			return FALSE;
		}
		
		// 2004.09.14 added begin
		
		if( FALSE == ReadSetting(g_MonitorItemArray) )
		{
			TRACE(L"\nReadSetting error in InitInstance.");
		}

		CONVERT_BUF MonitorItem;
		memset(&MonitorItem,0,sizeof(CONVERT_BUF));
				
		for(int nItemIndex=0; nItemIndex<g_MonitorItemArray.GetSize(); nItemIndex++ )
		{
			WideCharToMultiByte(CP_ACP, 0, g_MonitorItemArray[nItemIndex].szMonitorPath, MAX_PATH,
				MonitorItem.szMonitorFile, MAX_PATH, 0, 0);
			WideCharToMultiByte(CP_ACP, 0, g_MonitorItemArray[nItemIndex].szPassword, PASSWORD_SIZE,
				MonitorItem.szPassword, PASSWORD_SIZE, 0, 0);
			//wcsncpy(MonitorItem.szMonitorFile,g_MonitorItemArray[nItemIndex].szMonitorPath,MAX_PATH-1);
			//wcsncpy(MonitorItem.szPassword,g_MonitorItemArray[nItemIndex].szPassword,PASSWORD_SIZE-1);
			MonitorItem.ulCompressLevel = g_MonitorItemArray[nItemIndex].ulCompressLevel;
			MonitorItem.usNodeType = CONVERT_NODETYPE_DIRECTORY;
			MonitorItem.bCheckPasswordAlways = g_MonitorItemArray[nItemIndex].bCheckPasswordAlways;
			MonitorItem.bErrorLimit = g_MonitorItemArray[nItemIndex].bErrorLimit;
			MonitorItem.ulMaxErrLimit = g_MonitorItemArray[nItemIndex].ulMaxErrLimit;

			if( FALSE == YGFSMonAddMonitorItem(&MonitorItem) )
			{
				TRACE(L"\nYGFSMonAddIncludePath error.");
			}
			else
			{
				TRACE(L"\nSuccessfully add %s as monitor folder.",g_MonitorItemArray[0].szMonitorPath);
			}
		}
		
		// 2004.09.14 added end
		
		m_pMainWnd = &m_MainWnd;
		m_MainWnd.ShowWindow(SW_HIDE);
		m_MainWnd.UpdateWindow();

#ifdef NO_DRIVER_MODE
		HANDLE h = GetDataInfoFileHandle(TRUE);
		CloseHandle(h);
#endif

		return TRUE;
	}
	else
	{
		HWND  hWnd = FindWindow(MaClassName,NULL);
		if (hWnd) PostMessage(hWnd,WM_COMMAND,ID_MAIN_CONTROL,0);
	}
	
	return FALSE;
}

int CYGFSMonApp::ExitInstance() 
{
	if (g_hYGFSMonDriver != INVALID_HANDLE_VALUE)
	{
		// first make it not work
		// then there will be no more new handle
        if( FALSE == YGFSMonStopWork() )
		{
			TRACE(L"\nYGFSMonStopWork error in ExitInstance.");
		}
		else
		{
			// TRACE(L"\nYGFSMonStopWork succeed.");
		}
		
		// then free the memroy
        if( FALSE == YGFSMonClearState() )
		{
			TRACE(L"\nYGFSMonClearState error in ExitInstance.");
		}
		else
		{
			// TRACE(L"\nYGFSMonClearState succeed.");
		}
		
		CloseHandle(g_hYGFSMonDriver);
		g_hYGFSMonDriver = INVALID_HANDLE_VALUE;
	}
	else
	{
		TRACE(L"\ng_hYGFSMonDriver is invalid handle value.");
	}
	
	// the following line can be omitted
	g_MonitorItemArray.RemoveAll();
#ifdef NO_DRIVER_MODE
	CString strInfoPath;
	DeleteFile(strInfoPath);
	DeleteCriticalSection(&g_csEncrypt);
	DeleteCriticalSection(&g_csUpdate);
	GetDataInfoFilePath(strInfoPath);
	DeleteFile(strInfoPath);
	GetEncryptInfoFilePath(strInfoPath);
	DeleteFile(strInfoPath);
#endif
	return CWinApp::ExitInstance();
}

BOOL YGFSMonCheckVersion()
{
#ifdef NO_DRIVER_MODE
	return true;
#else
	BOOL bResult = FALSE;
	DWORD dwRead,dwVersion;
	if (g_hYGFSMonDriver != INVALID_HANDLE_VALUE)
	{
		bResult = DeviceIoControl(g_hYGFSMonDriver,
			IOCTL_YGFSMON_GET_VERSION_NT,
			NULL,0,&dwVersion,sizeof(DWORD),
			&dwRead,NULL);
	}
	return bResult;
#endif

}

BOOL YGFSMonStartWork()
{
#ifdef NO_DRIVER_MODE
	return true;
#else
	BOOL bResult = FALSE;
	DWORD dwRead;
	if (g_hYGFSMonDriver != INVALID_HANDLE_VALUE)
	{
		bResult = DeviceIoControl(g_hYGFSMonDriver,
			IOCTL_YGFSMON_START_WORK_NT,
			NULL,0,NULL,0,
			&dwRead,NULL);
	}
	return bResult;
#endif

}

BOOL YGFSMonStopWork()
{
#ifdef NO_DRIVER_MODE
	return true;
#else
	BOOL bResult = FALSE;
	DWORD dwRead;
	if (g_hYGFSMonDriver != INVALID_HANDLE_VALUE)
	{
		bResult = DeviceIoControl(g_hYGFSMonDriver,
			IOCTL_YGFSMON_STOP_WORK_NT,
			NULL,0,NULL,0,
			&dwRead,NULL);
	}
	return bResult;
#endif
}

BOOL YGFSMonClearState()
{
#ifdef NO_DRIVER_MODE
	return true;
#else
	BOOL bResult = FALSE;
	DWORD dwRead;
	if (g_hYGFSMonDriver != INVALID_HANDLE_VALUE)
	{
		bResult = DeviceIoControl(g_hYGFSMonDriver,
			IOCTL_YGFSMON_CLEAR_STATE_NT,
			NULL,0,NULL,0,
			&dwRead,NULL);
	}
	return bResult;
#endif
}

BOOL YGFSMonAddExcludeProcess(LPTSTR szProcess)
{
#ifdef NO_DRIVER_MODE
	return true;
#else
	BOOL bResult = FALSE;
	DWORD dwRead;
	if (g_hYGFSMonDriver != INVALID_HANDLE_VALUE)
	{
		bResult = DeviceIoControl(g_hYGFSMonDriver,
			IOCTL_YGFSMON_ADD_EXCLUDE_PROCESS_NT,
			szProcess,(wcslen(szProcess)+1) * sizeof(WCHAR),NULL,0,
			&dwRead,NULL);
	}
	return bResult;
#endif

}

BOOL YGFSMonRemoveExcludeProcess(LPTSTR szProcess)
{
#ifdef NO_DRIVER_MODE
	return true;
#else
	BOOL bResult = FALSE;
	DWORD dwRead;
	if (g_hYGFSMonDriver != INVALID_HANDLE_VALUE)
	{
		bResult = DeviceIoControl(g_hYGFSMonDriver,
			IOCTL_YGFSMON_REMOVE_EXCLUDE_PROCESS_NT,
			szProcess,(wcslen(szProcess)+1) * sizeof(WCHAR),NULL,0,
			&dwRead,NULL);
	}
	return bResult;
#endif
}

BOOL YGFSMonAddExcludePath(LPTSTR szPath)
{
#ifdef NO_DRIVER_MODE
	return true;
#else
	BOOL bResult = FALSE;
	DWORD dwRead;
	if (g_hYGFSMonDriver != INVALID_HANDLE_VALUE)
	{
		bResult = DeviceIoControl(g_hYGFSMonDriver,
			IOCTL_YGFSMON_ADD_EXCLUDE_PATH_NT,
			szPath,(wcslen(szPath)+1) * sizeof(WCHAR),NULL,0,
			&dwRead,NULL);
	}
	return bResult;
#endif
}

BOOL YGFSMonRemoveExcludePath(LPTSTR szPath)
{
#ifdef NO_DRIVER_MODE
	return true;
#else
	BOOL bResult = FALSE;
	DWORD dwRead;
	if (g_hYGFSMonDriver != INVALID_HANDLE_VALUE)
	{
		bResult = DeviceIoControl(g_hYGFSMonDriver,
			IOCTL_YGFSMON_REMOVE_EXCLUDE_PATH_NT,
			szPath,(wcslen(szPath)+1) * sizeof(WCHAR),NULL,0,
			&dwRead,NULL);
	}
	return bResult;
#endif
}

BOOL YGFSMonAddMonitorItem(PCONVERT_BUF pMonitorItem)
{
#ifdef NO_DRIVER_MODE
	EnterCriticalSection(&g_csEncrypt);
	g_MonitorPathSet.push_back(*pMonitorItem);
	CString strPath;
	if (GetEncryptInfoFilePath(strPath))
	{
		HANDLE hFile = CreateFile( strPath,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,
			CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN,NULL);
		for (vector<CONVERT_BUF>::iterator it = g_MonitorPathSet.begin();
			it != g_MonitorPathSet.end();
			++it)
		{
			DWORD w;
			WriteFile(hFile, &*it, sizeof(CONVERT_BUF), &w, 0);
			if (w != sizeof(CONVERT_BUF))
			{
				break;
			}
		}
		CloseHandle(hFile);
	}
	LeaveCriticalSection(&g_csEncrypt);
	return true;
#else
	DWORD dwRead;
	WCHAR szShortPath[MAX_PATH];
	WCHAR szLongPath[MAX_PATH];

	if (g_hYGFSMonDriver != INVALID_HANDLE_VALUE)
	{
		if (DeviceIoControl(g_hYGFSMonDriver,
			IOCTL_YGFSMON_ADD_MONITOR_ITEM_NT,
			pMonitorItem,sizeof(CONVERT_BUF),NULL,0,
			&dwRead,NULL))
		{
			WCHAR szMonitorFile[MAX_PATH];
			ZeroMemory(szMonitorFile, sizeof(szMonitorFile));
			WCHAR szPassword[PASSWORD_SIZE];
			ZeroMemory(szPassword, sizeof(szPassword));
			WCHAR szDestFile[MAX_PATH];
			ZeroMemory(szDestFile, sizeof(szDestFile));
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pMonitorItem->szMonitorFile, MAX_PATH, szMonitorFile, MAX_PATH);
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pMonitorItem->szDestFile, MAX_PATH, szDestFile, MAX_PATH);
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pMonitorItem->szPassword, PASSWORD_SIZE, szPassword, PASSWORD_SIZE); 

			if( FALSE == GetShortPathName(szMonitorFile,szShortPath,MAX_PATH) )
			{
				TRACE(L"\nGetShortPathName %s error in OnAdd",szShortPath);
			}
			else
			{
				if(wcsicmp(szMonitorFile,szShortPath) )
				{
					wcsncpy(szMonitorFile,szShortPath,MAX_PATH-1);
					WideCharToMultiByte(CP_ACP, 0, szMonitorFile, MAX_PATH, pMonitorItem->szMonitorFile, MAX_PATH, 0, 0);
					return DeviceIoControl(g_hYGFSMonDriver,
						IOCTL_YGFSMON_ADD_MONITOR_ITEM_NT,
						pMonitorItem,sizeof(CONVERT_BUF),NULL,0,
						&dwRead,NULL);
				}
			}

			memset(szLongPath,0,sizeof(szLongPath));

			if( FALSE == GetLongPathName(szMonitorFile,szLongPath,MAX_PATH) )
			{
				TRACE(L"\nGetLongPathName %s error in OnAdd",szLongPath);
			}
			else
			{
				if(wcsicmp(szMonitorFile,szLongPath) )
				{
					wcsncpy(szMonitorFile,szLongPath,MAX_PATH-1);
					WideCharToMultiByte(CP_ACP, 0, szMonitorFile, MAX_PATH, pMonitorItem->szMonitorFile, MAX_PATH, 0, 0);
					return DeviceIoControl(g_hYGFSMonDriver,
						IOCTL_YGFSMON_ADD_MONITOR_ITEM_NT,
						pMonitorItem,sizeof(CONVERT_BUF),NULL,0,
						&dwRead,NULL);
				}
			}
			return TRUE;
		}
	}
	return FALSE;
#endif
}

BOOL YGFSMonRemoveMonitorItem(PCONVERT_BUF pMonitorItem)
{
#ifdef NO_DRIVER_MODE
	EnterCriticalSection(&g_csEncrypt);
	for (vector<CONVERT_BUF>::iterator it = g_MonitorPathSet.begin();
		it != g_MonitorPathSet.end();
		++it)
	{
		CONVERT_BUF& info = *it;
		if (!stricmp(pMonitorItem->szMonitorFile, info.szMonitorFile))
		{
			g_MonitorPathSet.erase(it);
			LeaveCriticalSection(&g_csEncrypt);
			break;
		}
	}
	CString strPath;
	if (GetEncryptInfoFilePath(strPath))
	{
		HANDLE hFile = CreateFile( strPath,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,
			CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN,NULL);
		for (vector<CONVERT_BUF>::iterator it = g_MonitorPathSet.begin();
			it != g_MonitorPathSet.end();
			++it)
		{
			DWORD w;
			WriteFile(hFile, &*it, sizeof(CONVERT_BUF), &w, 0);
			if (w != sizeof(CONVERT_BUF))
			{
				break;
			}
		}
		CloseHandle(hFile);
	}
	LeaveCriticalSection(&g_csEncrypt);
	return true;
#else
	DWORD dwRead;
	WCHAR szShortPath[MAX_PATH];
	WCHAR szLongPath[MAX_PATH];

	if (g_hYGFSMonDriver != INVALID_HANDLE_VALUE)
	{
		if (DeviceIoControl(g_hYGFSMonDriver,
							IOCTL_YGFSMON_REMOVE_MONITOR_ITEM_NT,
							pMonitorItem,sizeof(CONVERT_BUF),NULL,0,
							&dwRead,NULL))
		{
			WCHAR szMonitorFile[MAX_PATH];
			ZeroMemory(szMonitorFile, sizeof(szMonitorFile));
			WCHAR szPassword[PASSWORD_SIZE];
			ZeroMemory(szPassword, sizeof(szPassword));
			WCHAR szDestFile[MAX_PATH];
			ZeroMemory(szDestFile, sizeof(szDestFile));
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pMonitorItem->szMonitorFile, MAX_PATH, szMonitorFile, MAX_PATH);
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pMonitorItem->szDestFile, MAX_PATH, szDestFile, MAX_PATH);
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pMonitorItem->szPassword, PASSWORD_SIZE, szPassword, PASSWORD_SIZE); 

			if( FALSE == GetShortPathName(szMonitorFile,szShortPath,MAX_PATH) )
			{
				TRACE(L"\nGetShortPathName %s error in OnAdd",szShortPath);
			}
			else
			{
				if(wcsicmp(szMonitorFile,szShortPath) )
				{
					wcsncpy(szMonitorFile,szShortPath,MAX_PATH-1);
					WideCharToMultiByte(CP_ACP, 0, szMonitorFile, MAX_PATH, pMonitorItem->szMonitorFile, MAX_PATH, 0, 0);
					return DeviceIoControl(g_hYGFSMonDriver,
										IOCTL_YGFSMON_REMOVE_MONITOR_ITEM_NT,
										pMonitorItem,sizeof(CONVERT_BUF),NULL,0,
										&dwRead,NULL);
				}
			}
			
			memset(szLongPath,0,sizeof(szLongPath));
			
			if( FALSE == GetLongPathName(szMonitorFile,szLongPath,MAX_PATH) )
			{
				TRACE(L"\nGetLongPathName %s error in OnAdd",szLongPath);
			}
			else
			{
				if(wcsicmp(szMonitorFile,szLongPath) )
				{
					wcsncpy(szMonitorFile,szLongPath,MAX_PATH-1);
					WideCharToMultiByte(CP_ACP, 0, szMonitorFile, MAX_PATH, pMonitorItem->szMonitorFile, MAX_PATH, 0, 0);
					return DeviceIoControl(g_hYGFSMonDriver,
										IOCTL_YGFSMON_REMOVE_MONITOR_ITEM_NT,
										pMonitorItem,sizeof(CONVERT_BUF),NULL,0,
										&dwRead,NULL);
				}
			}
			return TRUE;
		}
	}
	return FALSE;
#endif
}

BOOL YGFSMonGetModifyFile(CONVERT_BUF *pModifyInfo)
{
	BOOL bResult = FALSE;
	DWORD dwRead;
	if (g_hYGFSMonDriver != INVALID_HANDLE_VALUE)
	{
		if (DeviceIoControl(g_hYGFSMonDriver,
			IOCTL_YGFSMON_GET_MODIFY_FILE_NT,
			NULL,0,pModifyInfo,sizeof(CONVERT_BUF),
			&dwRead,NULL))
		{
			bResult = ((pModifyInfo->usNodeType == CONVERT_NODETYPE_FILE) ||
				(pModifyInfo->usNodeType == CONVERT_NODETYPE_AUTO_FILE) ||
				(pModifyInfo->usNodeType == CONVERT_NODETYPE_DIRECTORY ));
		}
	}
	return bResult;
}

BOOL YGFSMonGetMonitorDrive()
{
	BOOL bResult = FALSE;
	CString strTitle;
	char chDrive[MAX_PATH] = {0};
	WCHAR szDrive[MAX_PATH] = {0};
	DWORD dwRead;

	if (g_hYGFSMonDriver != INVALID_HANDLE_VALUE)
	{
		if (DeviceIoControl(g_hYGFSMonDriver,
			IOCTL_YGFSMON_GET_MONITOR_DRIVE_NT,
			NULL,0,chDrive,MAX_PATH,
			&dwRead,NULL))
		{
			strTitle.LoadString(IDS_APP_NAME);
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, chDrive, MAX_PATH, szDrive, MAX_PATH);
			MessageBox(GetFocus(),szDrive,strTitle,MB_OK | MB_ICONINFORMATION);
			bResult = TRUE;
		}
	}
	return bResult;
}

BOOL CYGFSMonApp::InitDavinciStatus()
{
	// for debug temp comment
	BOOL bReturnValue;
	bReturnValue = TRUE;
	
	if (!UpgradeSettingFile()) return FALSE;

	int nProcessCount;
	nProcessCount  = sizeof(YG_FS_MON_PROCESS)/sizeof(YG_FS_MON_PROCESS[0]);
	
	int nProcessIndex;
	for( nProcessIndex = 0; nProcessIndex<nProcessCount; nProcessIndex++)
	{
		WCHAR szMonProcess[MAX_PATH];
		memset(szMonProcess,0,sizeof(szMonProcess));
		wcsncpy(szMonProcess,YG_FS_MON_PROCESS[nProcessIndex],MAX_PATH-1);
		
		TRACE(L"\nExcludeProcess:%s",szMonProcess);
		if( FALSE == YGFSMonAddExcludeProcess(szMonProcess) )
		{
			TRACE(L"\nAddExcludeProcess %s error.",szMonProcess);
			bReturnValue = FALSE;
		}
	}

	// begin exclude path
	// If we do not invoke the interface it may be fast?

	WCHAR szWindowsDir[MAX_PATH];
	memset(szWindowsDir,0,sizeof(szWindowsDir));
	GetWindowsDirectory(szWindowsDir,MAX_PATH);
    
	TRACE(L"\nAddExcludePath: %s",szWindowsDir);
	if( FALSE == YGFSMonAddExcludePath(szWindowsDir) )
	{
		TRACE(L"\nYGFSMonAddExcludePath error in InitDavinciStatus:%s",szWindowsDir);
		bReturnValue = FALSE;
	}
	
	CString strProgDir;
	strProgDir = szWindowsDir;
	strProgDir = strProgDir.Left(2) + L"\\" + L"Progra~1";
	WCHAR szProgDir[MAX_PATH];
	memset(szProgDir,0,sizeof(szProgDir));
	wcsncpy(szProgDir,(LPCTSTR)strProgDir,MAX_PATH-1);
	
	TRACE(L"\nAddExcludePath: %s",szProgDir);
	if( FALSE == YGFSMonAddExcludePath(szProgDir) )
	{
		TRACE(L"\nYGFSMonAddExcludePath error in InitDavinciStatus: %s",szProgDir);
		bReturnValue = FALSE;
	}

	// 2004.09.22 comment end
	// end exclude path
	
	return bReturnValue;
}