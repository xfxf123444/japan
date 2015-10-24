// MainWnd.cpp : implementation file
//

#include "stdafx.h"
#include "YGFSMon.h"
#include "MainWnd.h"
#include <process.h>      // for _beginthreadex
#include "Function.h"
#include "..\Decrypt\DecryptFunction\DecryptFunction.h"
#include "..\Encrypt\EncryptFunction\EncryptInfo.h"
#include "..\Encrypt\EncryptFunction\EncryptFunction.h"
#include "YGFSMonConst.h"


#ifdef NO_DRIVER_MODE
extern vector<CONVERT_BUF> g_MonitorPathSet;
extern map<CString, ExcludeFileInfo> g_ExcludeFileSet;
extern CRITICAL_SECTION g_csUpdate;
extern CRITICAL_SECTION g_csEncrypt;
DWORD WINAPI UpdateExcludeFileInfo(LPVOID pIn);
DWORD WINAPI UpdateTempFileInfo(LPVOID pIn);
#endif

typedef unsigned (__stdcall *PTHREAD_START) (void *);

BOOL g_bHasThread;
#ifdef NO_DRIVER_MODE
BOOL g_bHasThreadForUpdate;
#endif
extern CMONITOR_ITEM_ARRAY g_MonitorItemArray;

DWORD WINAPI ThreadEncryptFile(LPVOID pIn);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_MY_NOTIFYICON		(WM_APP+100)


/////////////////////////////////////////////////////////////////////////////
// CMainWnd

CMainWnd::CMainWnd()
{
	m_pControl = NULL;
	g_bHasThread = FALSE;
#ifdef NO_DRIVER_MODE
	g_bHasThreadForUpdate = FALSE;
#endif
}

CMainWnd::~CMainWnd()
{
}


BEGIN_MESSAGE_MAP(CMainWnd, CWnd)
	//{{AFX_MSG_MAP(CMainWnd)
	ON_MESSAGE(WM_MY_NOTIFYICON, OnMyNotify)
	ON_COMMAND(ID_MAIN_CONTROL,OnOpenMainControl)
	ON_COMMAND(ID_CLOSE_WINDOW,OnCloseWindow)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_QUERYENDSESSION()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMainWnd message handlers

LRESULT CMainWnd::OnMyNotify(WPARAM wParam, LPARAM lParam) 
{
	CMenu menu ;
	switch (lParam)
	{
	case WM_LBUTTONDBLCLK:
		PostMessage(WM_COMMAND,ID_MAIN_CONTROL,0);
		break;
	case WM_RBUTTONDOWN:
		{
			VERIFY(menu.LoadMenu(IDR_MAINMENU));
			CMenu* pPopup = menu.GetSubMenu ( 0 ) ;
			ASSERT(pPopup != NULL);
			POINT pt ;
			GetCursorPos ( &pt ) ;
		    SetForegroundWindow ();
			pPopup->TrackPopupMenu ( TPM_LEFTALIGN | TPM_RIGHTBUTTON,
									  pt.x, pt.y, AfxGetMainWnd() );
			PostMessage(WM_USER,0,0);

			break ;
		}
	case WM_MOUSEMOVE:
	default: 
		break;
	}
	return 1;
}

int CMainWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	CString strIconTitle;
	strIconTitle.LoadString(IDS_TRAY_ICON_TITLE);
	m_pTray = new CTrayNot (this, WM_MY_NOTIFYICON, strIconTitle) ;
	m_nTimer = 100;
	SetTimer(m_nTimer,1000,NULL);
#ifdef NO_DRIVER_MODE
	//m_nTimerForUpdate = 101;
	//SetTimer(m_nTimerForUpdate, 400, NULL);
#endif
	return TRUE;
}

void CMainWnd::OnMyParseFile()
{
	if (!g_bHasThread)
	{
		DWORD dwThreadID;
		g_bHasThread = TRUE;
	    m_hThreadHandle = (HANDLE)_beginthreadex(NULL,0,(PTHREAD_START)ThreadEncryptFile,&m_ThreadParam,0,(unsigned *)(&dwThreadID));
	}
}

void CMainWnd::OnCloseWindow()
{
	PostMessage(WM_CLOSE,0,NULL);
}

void CMainWnd::OnOpenMainControl()
{
	CProtect	SettingDlg;

	if (NULL == m_pControl)
	{
		m_pControl = new CSetup(IDS_SETUP,this);
		m_pControl->AddPage(&SettingDlg);
		m_pControl->DoModal();

		m_pControl->RemovePage(&SettingDlg);
		delete m_pControl;
		m_pControl = NULL;
	}
	else m_pControl->SetForegroundWindow();
}

void CMainWnd::OnDestroy() 
{
	if (m_pControl)
	{
		::SendMessage(m_pControl->m_hWnd,WM_COMMAND,IDOK,0);
		m_pControl->DestroyWindow();
	}

	CWnd::OnDestroy();
	
	KillTimer(m_nTimer);	
#ifdef NO_DRIVER_MODE
	KillTimer(m_nTimerForUpdate);
#endif
	if (m_pTray)
	{
		Shell_NotifyIcon ( NIM_DELETE, &m_pTray->m_tnd ) ;
		delete m_pTray;
	}

	// TODO: Add your message handler code here
	
}


void CMainWnd::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
#ifdef NO_DRIVER_MODE
	//if (nIDEvent == m_nTimer)
	//{
		OnMyParseFile();
	//}
	//else
	//{
	//	OnUpdateFileInfo();
	//}
#else
	OnMyParseFile();
#endif

	CWnd::OnTimer(nIDEvent);
}

void GetAllFilePath( CString root, vector<CString>& filePathSet )
{
	CFileFind finder;

	// build a string with wildcards
	CString strWildcard(root);
	strWildcard += _T("\\*.*");

	// start working for files
	BOOL bWorking = finder.FindFile(strWildcard);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		// skip . and .. files; otherwise, we'd
		// recur infinitely!

		if (finder.IsDots())
			continue;

		// if it's a directory, recursively search it

		if (finder.IsDirectory())
		{
			CString str = finder.GetFilePath();
			GetAllFilePath(str, filePathSet);
		}
		else
		{
			if (!CheckSpeicalFile(finder.GetFilePath())) {
				filePathSet.push_back(finder.GetFilePath());
			}
		}
	}

	finder.Close();

}

DWORD WINAPI ThreadEncryptFile(LPVOID pIn)
{
#ifdef NO_DRIVER_MODE
	UpdateTempFileInfo(pIn);

	EnterCriticalSection(&g_csEncrypt);
	vector<CONVERT_BUF> temp = g_MonitorPathSet;
	for (size_t i = 0;
		i != temp.size();
		++i)
	{
		memcpy(&temp[i], &g_MonitorPathSet[i], sizeof(CONVERT_BUF));
	}
	LeaveCriticalSection(&g_csEncrypt);

	for (vector<CONVERT_BUF>::iterator it = temp.begin();
		it != temp.end();
		++it)
	{
		CONVERT_BUF& rootInfo = *it;
		WCHAR szPassword[PASSWORD_SIZE];
		ZeroMemory(szPassword, sizeof(szPassword));
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, rootInfo.szPassword, PASSWORD_SIZE, szPassword, PASSWORD_SIZE);
		WCHAR szMonitorFile[MAX_PATH];
		ZeroMemory(szMonitorFile, sizeof(szMonitorFile));
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, rootInfo.szMonitorFile, MAX_PATH, szMonitorFile, MAX_PATH);
		vector<CString> filePathSet;
		GetAllFilePath(szMonitorFile, filePathSet);
		for (vector<CString>::iterator itPath = filePathSet.begin();
			itPath != filePathSet.end();
			++itPath)
		{
			CString& path = *itPath;
			if (path.GetLength() <= 4 
				|| _tcsicmp(_T(".chy"), path.Right(4)) == 0
				|| _tcsicmp(_T(".ary"), path.Right(4)) == 0)
			{
				continue;
			}

			// 更新
			DWORD dwThreadID;
			UpdateExcludeFileInfo(&dwThreadID);
			
			// 检查
			EnterCriticalSection(&g_csUpdate);
			map<CString, ExcludeFileInfo>::iterator itInfo = g_ExcludeFileSet.find(path);
			BOOL bFound = FALSE;
			if (itInfo != g_ExcludeFileSet.end())
			{
				bFound = TRUE;
				if (!itInfo->second.bOpenedByOtherProcess)
				{
					LeaveCriticalSection(&g_csUpdate);
					continue;
				} 
			}
			LeaveCriticalSection(&g_csUpdate);

			// 加密
			BOOL ret = EncryptOneFile(path,path + _T(".chy"),szPassword,
				TRUE,rootInfo.ulCompressLevel,rootInfo.bErrorLimit,rootInfo.ulMaxErrLimit);

			// 清理
			if (ret && bFound)
			{
				EnterCriticalSection(&g_csUpdate);
				itInfo = g_ExcludeFileSet.find(path);
				if (itInfo != g_ExcludeFileSet.end())
				{
					g_ExcludeFileSet.erase(itInfo);
				}
				LeaveCriticalSection(&g_csUpdate);
			}
		}
	}

	g_bHasThread = FALSE;
	return TRUE;
#else
	CONVERT_BUF ModifyInfo;
	WCHAR		*p;
	DWORD		dwFileAttribute;

	while (YGFSMonGetModifyFile(&ModifyInfo))
	{

		WCHAR szMonitorFile[MAX_PATH];
		ZeroMemory(szMonitorFile, sizeof(szMonitorFile));
		WCHAR szPassword[PASSWORD_SIZE];
		ZeroMemory(szPassword, sizeof(szPassword));
		WCHAR szDestFile[MAX_PATH];
		ZeroMemory(szDestFile, sizeof(szDestFile));
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, ModifyInfo.szMonitorFile, MAX_PATH, szMonitorFile, MAX_PATH);
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, ModifyInfo.szDestFile, MAX_PATH, szDestFile, MAX_PATH);
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, ModifyInfo.szPassword, PASSWORD_SIZE, szPassword, PASSWORD_SIZE);
		dwFileAttribute = GetFileAttributes(szMonitorFile);
		p = &szMonitorFile[wcslen(szMonitorFile)-4];
		if( dwFileAttribute != -1 && wcsicmp(p,L".chy") &&wcsicmp(p,L".ary"))
		{
			if( 0 == ( FILE_ATTRIBUTE_DIRECTORY & dwFileAttribute ) )
			{
				// if this is a file

				if (ModifyInfo.usModifyType == FILE_OPEN_MODIFYED)
					EncryptOneFile(szMonitorFile,szDestFile,szPassword,TRUE,ModifyInfo.ulCompressLevel,ModifyInfo.bErrorLimit,ModifyInfo.ulMaxErrLimit);
				/*				if (ModifyInfo.usNodeType == CONVERT_NODETYPE_AUTO_FILE)
				{
				SetFileAttributes(ModifyInfo.szMonitorFile,FILE_ATTRIBUTE_NORMAL);
				DeleteFile(ModifyInfo.szMonitorFile);
				}*/
			}
			else
			{
				EncryptOneDirSeparately(szMonitorFile,szPassword,TRUE,ModifyInfo.ulCompressLevel,ModifyInfo.bErrorLimit,ModifyInfo.ulMaxErrLimit);
			}
		}
	}
	g_bHasThread = FALSE;
	return TRUE;
#endif
}

BOOL CMainWnd::OnQueryEndSession() 
{
	
	// TODO: Add your specialized query end session code here
    SHELL_FOLDER_INFO ShellFolder;
	GetShellFolderInfo(ShellFolder);

    if(-1 == GetFileAttributes(ShellFolder.szTempDir))  
	{
		TRACE(L"\nThis is not a dir name or file name.");
		return FALSE;
	}

	CString strTempDir;
	strTempDir = ShellFolder.szTempDir;
	strTempDir.TrimRight(L'\\');
	
	if( 
		( 0 != strTempDir.Right(4).CompareNoCase(L"Temp") ) &&     
		( 0 != strTempDir.Right(3).CompareNoCase(L"Tmp") )
	  )
	{
		// we will not delete the directory that not like the temp file
		TRACE(L"\nCan not find the temp dir.");
		return FALSE;
	}

	CDeleteProgressDlg DeleteProgressDlg;
	DeleteProgressDlg.SetDeleteParameter(CString(ShellFolder.szTempDir),0);
	DeleteProgressDlg.DoModal();

	return TRUE;
}

void CheckExcludeFileStatus()
{
#ifdef NO_DRIVER_MODE

	if (g_ExcludeFileSet.empty())
	{
		return;
	}

	set<CString> invalidFileSet;
	EnterCriticalSection(&g_csUpdate);
	for (map<CString, ExcludeFileInfo>::iterator it = g_ExcludeFileSet.begin();
		it != g_ExcludeFileSet.end();
		++it)
	{
		ExcludeFileInfo& info = it->second;
		DWORD f = GetFileAttributes(info.strFilePath);
		if (f == -1 || f == FILE_ATTRIBUTE_DIRECTORY)
		{
			invalidFileSet.insert(info.strFilePath);
			continue;
		}

		HANDLE h = CreateFile( info.strFilePath,GENERIC_READ | GENERIC_WRITE, NULL, NULL,
			OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

		if (h == INVALID_HANDLE_VALUE)
		{
			info.bOpenedByOtherProcess = false;
		}
		else
		{
			FILETIME t = GetLastModifyTime(h);
			if (CompareFileTime(&t, &info.dwLastModifyTime) > 0)
			{
				info.bOpenedByOtherProcess = true;
			}
			CloseHandle(h);
		}
	}

	for (set<CString>::iterator it = invalidFileSet.begin();
		it != invalidFileSet.end();
		++it)
	{
		g_ExcludeFileSet.erase(*it);
	}
	LeaveCriticalSection(&g_csUpdate);
#endif
}

DWORD WINAPI UpdateExcludeFileInfo(LPVOID pIn)
{
#ifdef NO_DRIVER_MODE
	EnterCriticalSection(&g_csEncrypt);
	vector<CONVERT_BUF> tempMonitorPathSet = g_MonitorPathSet;
	for (size_t i = 0;
		i != tempMonitorPathSet.size();
		++i)
	{
		memcpy(&tempMonitorPathSet[i], &g_MonitorPathSet[i], sizeof(CONVERT_BUF));
	}
	LeaveCriticalSection(&g_csEncrypt);

	do 
	{
		// 打开数据文件
		CString strInfoPath;
		if (!GetDataInfoFilePath(strInfoPath))
		{
			break;
		}
		HANDLE hFile = GetDataInfoFileHandle(false);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			hFile = GetDataInfoFileHandle(true);
			CloseHandle(hFile);
			break;
		}

		// 读取信息并清理
		DWORD nRead = 0;
		ExcludeFileInfo temp;
		vector<ExcludeFileInfo> newExcludeInfoSet;
		BOOL suc;
		do 
		{
			suc = ReadFile(hFile, &temp, sizeof(ExcludeFileInfo), &nRead, 0);
			if (suc && nRead == sizeof(ExcludeFileInfo))
			{
				newExcludeInfoSet.push_back(temp);
			}
		} while(suc && nRead == sizeof(ExcludeFileInfo));
		CloseHandle(hFile);
		DeleteFile(strInfoPath);
		HANDLE h = CreateFile( strInfoPath,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,
			CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN,NULL);
		if (h != INVALID_HANDLE_VALUE)
		{
			CloseHandle(h);
		}

		// 检查信息
		for (vector<ExcludeFileInfo>::iterator itInfo = newExcludeInfoSet.begin();
			itInfo != newExcludeInfoSet.end();)
		{
			ExcludeFileInfo& newInfo = *itInfo;
			CString newPath = newInfo.strFilePath;
			BOOL bValidInfo = FALSE;
			for (vector<CONVERT_BUF>::iterator it = tempMonitorPathSet.begin();
				it != tempMonitorPathSet.end();
				++it)
			{
				WCHAR szMonitorFile[MAX_PATH];
				ZeroMemory(szMonitorFile, sizeof(szMonitorFile));
				MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, it->szMonitorFile, MAX_PATH, szMonitorFile, MAX_PATH);
				CString temp = newPath.Left(wcslen(szMonitorFile));
				if (temp == szMonitorFile)
				{
					bValidInfo = true;
					break;
				}
			}
			if (!bValidInfo)
			{
				itInfo = newExcludeInfoSet.erase(itInfo);
			}
			else
			{
				++itInfo;
			}
		}

		// 更新信息
		EnterCriticalSection(&g_csUpdate);
		for (vector<ExcludeFileInfo>::iterator itInfo = newExcludeInfoSet.begin();
			itInfo != newExcludeInfoSet.end();
			++itInfo)
		{
			ExcludeFileInfo& newInfo = *itInfo;
			CString newPath = newInfo.strFilePath;
			map<CString, ExcludeFileInfo>::iterator it = g_ExcludeFileSet.find(newPath);
			if (it == g_ExcludeFileSet.end())
			{
				g_ExcludeFileSet.insert(make_pair(newPath, newInfo));
			}
			else
			{
				it->second.bOpenedByOtherProcess = false;
				it->second.dwLastModifyTime = newInfo.dwLastModifyTime;
			}
		}
		LeaveCriticalSection(&g_csUpdate);
	} while(0);

	CheckExcludeFileStatus();
	
	g_bHasThreadForUpdate = FALSE;
	return TRUE;
#else
	return false;
#endif
}

DWORD WINAPI UpdateTempFileInfo( LPVOID pIn )
{
#ifdef NO_DRIVER_MODE
	BOOL bReadSuc = TRUE;
	DWORD cnt1 = 0;
	CString strInfoPath;
	do 
	{
		if (!GetTempInfoFilePath(strInfoPath)
			|| GetFileAttributes(strInfoPath) == -1)
		{
			break;
		}
		HANDLE hFile = CreateFile( strInfoPath,GENERIC_READ | GENERIC_WRITE,NULL,NULL,
			OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN,NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			Sleep(10);
			bReadSuc = FALSE;
			++cnt1;
			continue;
		}

		DWORD nRead = 0;
		CONVERT_BUF temp;
		vector<CONVERT_BUF> tempInfoSet;
		while (ReadFile(hFile, &temp, sizeof(CONVERT_BUF), &nRead, 0) && nRead == sizeof(CONVERT_BUF))
		{
			tempInfoSet.push_back(temp);
		}
		CloseHandle(hFile);
		vector<pair<size_t, FILETIME> > FileFilterSet;
		for (vector<CONVERT_BUF>::iterator it = tempInfoSet.begin();
			it != tempInfoSet.end();
			++it)
		{
			CONVERT_BUF& info = *it;
			FILETIME ft1;
			ft1.dwHighDateTime = info.ulCompressLevel;
			ft1.dwLowDateTime = info.ulMaxErrLimit;
			BOOL bFound = false;
			for (vector<pair<size_t, FILETIME> >::iterator itf = FileFilterSet.begin();
				itf != FileFilterSet.end();
				++itf)
			{
				pair<size_t, FILETIME>& filter = *itf;
				if (filter.first < tempInfoSet.size())
				{
					CONVERT_BUF& info2 =  tempInfoSet[filter.first];
					if (strcmp(info.szMonitorFile, info2.szMonitorFile) == 0)
					{
						FILETIME& ft2 = filter.second;
						if (CompareFileTime(&ft2, &ft1) > 0)
						{
							ft2 = ft1;
						}
						bFound = true;
						break;
					}
				}
			}
			if (!bFound)
			{
				FileFilterSet.push_back(make_pair(distance(tempInfoSet.begin(), it), ft1));
			}
		}
		vector<CONVERT_BUF> tempInfoSetBackup = tempInfoSet;
		tempInfoSet.clear();
		for (vector<pair<size_t, FILETIME> >::iterator itf = FileFilterSet.begin();
			itf != FileFilterSet.end();
			++itf)
		{
			pair<size_t, FILETIME>& filter = *itf;
			if (filter.first < tempInfoSetBackup.size())
			{
				tempInfoSet.push_back(tempInfoSetBackup[filter.first]);
			}
		}
		for (vector<CONVERT_BUF>::iterator it = tempInfoSet.begin();
			it != tempInfoSet.end();)
		{
			CONVERT_BUF& info = *it;
			TCHAR bufSource[MAXPATHLEN];
			TCHAR bufDest[MAXPATHLEN];
			ZeroMemory(bufSource, sizeof(bufSource));
			ZeroMemory(bufDest, sizeof(bufDest));
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, info.szMonitorFile, strlen(info.szMonitorFile), 
				bufSource, MAXPATHLEN);
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, info.szDestFile, strlen(info.szDestFile), 
				bufDest, MAXPATHLEN);
			GetLongPathName(bufSource, bufSource, MAXPATHLEN);
			GetLongPathName(bufDest, bufDest, MAXPATHLEN);
			CString strSource = bufSource;
			CString strDest = bufDest;
			strDest = strDest.Left(strDest.ReverseFind(_T('.')));
			if (GetFileAttributes(strSource) == -1)
			{
				it = tempInfoSet.erase(it);
				continue;
			}
			FILETIME ft;
			ft.dwHighDateTime = info.ulCompressLevel;
			ft.dwLowDateTime = info.ulMaxErrLimit;
			HANDLE hTemp = CreateFile( strSource,GENERIC_READ | GENERIC_WRITE,NULL,NULL,
				OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN,NULL);
			if (hTemp != INVALID_HANDLE_VALUE)
			{
				FILETIME cft = GetLastModifyTime(hTemp);
				CloseHandle(hTemp);
				if (CompareFileTime(&cft, &ft) > 0)
				{
					if (CopyFile(strSource, strDest, FALSE))
					{
						info.ulCompressLevel = cft.dwHighDateTime;
						info.ulMaxErrLimit = cft.dwLowDateTime;
					} 
				}
			}
			++it;
		}
		if (!tempInfoSet.empty())
		{
			BOOL bWriteBackSuc = TRUE;
			DWORD cnt2 = 0;
			CString strInfoPathTemp = strInfoPath + _T("backup");
			do 
			{
				hFile = CreateFile( strInfoPathTemp,GENERIC_READ | GENERIC_WRITE,NULL,NULL,
					CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN,NULL);
				if (hFile != INVALID_HANDLE_VALUE)
				{
					for (vector<CONVERT_BUF>::iterator it = tempInfoSet.begin();
						it != tempInfoSet.end();
						++it)
					{
						DWORD dwTemp;
						if (WriteFile(hFile, &*it, sizeof(CONVERT_BUF), &dwTemp, 0)
							&& dwTemp == sizeof(CONVERT_BUF))
						{
							bWriteBackSuc = TRUE;
						}
						else
						{
							bWriteBackSuc = FALSE;
							break;
						}
					}
					CloseHandle(hFile);
				}
				else
				{
					bWriteBackSuc = FALSE;
				}
				if (!bWriteBackSuc)
				{
					Sleep(10);
					++cnt2;
				}
			} while(!bWriteBackSuc && cnt2 < 100);
			if (bWriteBackSuc)
			{
				CopyFile(strInfoPathTemp, strInfoPath, FALSE);
				DeleteFile(strInfoPathTemp);
			}
		}
		else
		{
			DeleteFile(strInfoPath);
		}
	} while(!bReadSuc && cnt1 < 100);

	return TRUE;
#else
	return false;
#endif
}
void CMainWnd::OnUpdateFileInfo()
{
	//if (!g_bHasThreadForUpdate)
	//{
	//	DWORD dwThreadID;
	//	g_bHasThreadForUpdate = TRUE;
	//	m_hThreadHandleForUpdate = (HANDLE)_beginthreadex(NULL,0,
	//		(PTHREAD_START)UpdateExcludeFileInfo,&m_ThreadParamForUpdate,0,(unsigned *)(&dwThreadID));
	//}
}
