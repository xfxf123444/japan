// FileDecrypt.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "FileDecrypt.h"
#include "ShellDecryptDlg.h"
#include "DecryptStatus.h"
#include "..\..\DavinciFunction\DavinciFunction.h"
#include "../../EncryptToolStruct/EncryptToolStruct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern DECRYPT_INFO  g_DecryptInfo;

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
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
	BOOL    bAutoDecrypt = TRUE;
	CString strCommandLine;
#ifndef _DEBUG
	strCommandLine = GetCommandLine();
	//MessageBox(0, LPCTSTR(strCommandLine), LPCTSTR(strCommandLine), 0);
	if (strCommandLine.Find(L'?') != -1)
	{
		bAutoDecrypt = TRUE;
	}
	else
	{
		bAutoDecrypt = FALSE;
	}
	int nIndex = strCommandLine.Find(L':');
	if (nIndex == -1)
	{
		//MessageBox(0, LPCTSTR(L"false1"), LPCTSTR(L"false1"), 0);
		return false;
	}
	else
	{
		++nIndex;
		nIndex = strCommandLine.Find(L':', nIndex);
		if (nIndex == -1)
		{
			//MessageBox(0, LPCTSTR(L"false2"), LPCTSTR(L"false2"), 0);
			return false;
		}
		++nIndex;
		nIndex = strCommandLine.Find(L':', nIndex);
		if (nIndex != -1)
		{
			//MessageBox(0, LPCTSTR(L"false3"), LPCTSTR(L"false3"), 0);
			return false;
		}
	}

	CString strFileName;
	if (!bAutoDecrypt)
	{
		strFileName = strCommandLine.Mid(strCommandLine.Find(L'*') + 1); // skip the first ":"
	}
	else
	{
		strFileName = strCommandLine.Mid(strCommandLine.Find(L'?') + 1); // skip the first ":"
	}

	WCHAR wcBuf[MAX_PATH];
	ZeroMemory(wcBuf, MAX_PATH * sizeof(WCHAR));
	GetLongPathNameW(strFileName, wcBuf, MAX_PATH);
	strFileName = wcBuf;

	//MessageBox(0, LPCTSTR(strFileName), LPCTSTR(strFileName), 0);
	DWORD dwFileAttribute;
	dwFileAttribute = GetFileAttributes((LPCTSTR)strFileName);
	CString strFormat = strFileName.Right(4);
	//MessageBox(0, LPCTSTR(strFormat), LPCTSTR(strFormat), 0);
	if (wcscmp(L".chy", LPCTSTR(strFormat)) != 0)
	{
		//MessageBox(0, LPCTSTR(L"false4"), LPCTSTR(L"false4"), 0);
		return false;
	}

	if(-1 == dwFileAttribute )
	{
		//MessageBox(0, LPCTSTR(L"false5"), LPCTSTR(L"false5"), 0);
		TRACE(L"\nThe file is not exist: %s",(LPCTSTR)strFileName );
		return FALSE;
	}

	if( FILE_ATTRIBUTE_DIRECTORY & dwFileAttribute )
	{
		//MessageBox(0, L"\nCan not decrypt a directory:%s", L"\nCan not decrypt a directory:%s", 0);
		TRACE(L"\nCan not decrypt a directory:%s",(LPCTSTR)strFileName);
	}
	//MessageBox(0, LPCTSTR(strFileName), LPCTSTR(strFileName), 0);
#else
	bAutoDecrypt = TRUE;
	CString strFileName = L"I:\\847587328291.txt.chy";
#endif
	CShellDecryptDlg ShellDecryptDlg;
	ShellDecryptDlg.m_strImageFile = strFileName;
	wcscpy(g_DecryptInfo.szImageFile,strFileName);

	if (!bAutoDecrypt) 
	{
		ShellDecryptDlg.m_bOpen = FALSE;
		ShellDecryptDlg.DoModal();
	}
	else
	{
		if (!ParseAutoDecrypt(strFileName))
		{
			CString strText,strTitle;
			strText.LoadString(IDS_AUTO_DECRYPT_FAIL);
			strTitle.LoadString(IDS_APP_NAME);
			if (MessageBox(GetFocus(),strText,strTitle,MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				ShellDecryptDlg.m_bOpen = FALSE;
				ShellDecryptDlg.DoModal();
			}
		}
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

BOOL ParseAutoDecrypt(CString strFile)
{
	CONVERT_BUF DecryptFileInfo = {0};
	DWORD dwImageFileCount;
	CString strOneFileName;
	CString strOriginalFilePath;
	CShellDecryptDlg ShellDecryptDlg;
	
	ShellDecryptDlg.m_strImageFile = strFile;
	ZeroMemory(DecryptFileInfo.szMonitorFile, sizeof(DecryptFileInfo.szMonitorFile));
	WideCharToMultiByte(CP_ACP, 0, (LPCTSTR)strFile, strFile.GetLength(), DecryptFileInfo.szMonitorFile, MAX_PATH, 0, 0);
	// wcscpy(DecryptFileInfo.szMonitorFile,strFile);

	if (!YGNSMGetAutoDecryptInfo(&DecryptFileInfo)) return FALSE;

	WCHAR wchBuf[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, DecryptFileInfo.szMonitorFile, MAX_PATH,
		wchBuf, MAX_PATH);
	strOriginalFilePath = wchBuf;
	
	if (DecryptFileInfo.bCheckPasswordAlways)
	{
		ShellDecryptDlg.m_bOpen = TRUE;
		ShellDecryptDlg.DoModal();
	}
	else
	{
		wcscpy(g_DecryptInfo.szImageFile,strFile);
		ZeroMemory(g_DecryptInfo.szPassword, sizeof(g_DecryptInfo.szPassword));
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, DecryptFileInfo.szPassword, PASSWORD_SIZE,
			g_DecryptInfo.szPassword, PASSWORD_SIZE);
		//wcscpy(g_DecryptInfo.szPassword,DecryptFileInfo.szPassword);
		
		if (!GetTempDir((WCHAR *)&g_DecryptInfo.szTargetPath)) return FALSE;

		if( FALSE == GetImageInfo(&g_DecryptInfo,dwImageFileCount,strOneFileName))
		{
			return FALSE;
		}

		if (dwImageFileCount != 1) return FALSE;
		
		g_DecryptInfo.nRestoreType = DECRYPT_PART;
		g_DecryptInfo.nTargetType = DECRYPT_NEW_PLACE;
		memset(g_DecryptInfo.szSelectedPath,0,sizeof(g_DecryptInfo.szSelectedPath));
		wcsncpy(g_DecryptInfo.szSelectedPath,(LPCTSTR)strOneFileName,MAX_PATH-1);
		
		ARRAY_DATA ArrayData;
		wcscpy(ArrayData.szFileName,strOneFileName);
		AdjustOnePathString(ArrayData,g_DecryptInfo);
		SetFileAttributes(ArrayData.szFileName,FILE_ATTRIBUTE_NORMAL);
		DeleteFile(ArrayData.szFileName);
		
		CDecryptStatus DecryptStatusDlg;
		DecryptStatusDlg.DoModal();
		
		if (GetFileAttributes(ArrayData.szFileName) != -1)
		{
			ZeroMemory(DecryptFileInfo.szMonitorFile, sizeof(DecryptFileInfo.szMonitorFile));
			ZeroMemory(DecryptFileInfo.szDestFile, sizeof(DecryptFileInfo.szDestFile));
			WideCharToMultiByte(CP_ACP, 0, ArrayData.szFileName, -1,
				DecryptFileInfo.szMonitorFile, MAX_PATH, 0, 0);
			WideCharToMultiByte(CP_ACP, 0, strFile, strFile.GetLength(),
				DecryptFileInfo.szDestFile, MAX_PATH, 0, 0);
			//wcscpy(DecryptFileInfo.szMonitorFile,ArrayData.szFileName);
			//wcscpy(DecryptFileInfo.szDestFile,strFile);
			DecryptFileInfo.usNodeType = CONVERT_NODETYPE_AUTO_FILE;
			DecryptFileInfo.ulCompressLevel = g_DecryptInfo.ulCompressLevel;
			ZeroMemory(DecryptFileInfo.szPassword, PASSWORD_SIZE);
			WideCharToMultiByte(CP_ACP, 0, g_DecryptInfo.szPassword, PASSWORD_SIZE,
				DecryptFileInfo.szPassword, PASSWORD_SIZE, 0, 0);
			//memcpy(DecryptFileInfo.szPassword,g_DecryptInfo.szPassword,PASSWORD_SIZE*sizeof(WCHAR));
			DecryptFileInfo.bErrorLimit = g_DecryptInfo.EncryptOption.bOptionOn && g_DecryptInfo.EncryptOption.bErrorLimit;
			DecryptFileInfo.ulMaxErrLimit = g_DecryptInfo.EncryptOption.ulErrorLimit;
#ifndef NO_DRIVER_MODE
			YGNSMRemoveMonitorFile(&DecryptFileInfo);
#endif
			if ((ULONG)ShellExecute(NULL,L"open",ArrayData.szFileName,NULL,NULL,SW_SHOWNORMAL) > 32)
			{
#ifdef NO_DRIVER_MODE
				BOOL bWriteSuc = TRUE;
				DWORD cnt = 0;
				CString strInfoPath;
				if (GetTempInfoFilePath(strInfoPath))
				{
					do 
					{
						HANDLE hFile = INVALID_HANDLE_VALUE;
						DWORD dwTemp;
						if (GetFileAttributes(strInfoPath) != -1)
						{
							hFile = CreateFile( strInfoPath,GENERIC_READ | GENERIC_WRITE,NULL,NULL,
								OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN,NULL);
						}
						else
						{
							hFile = CreateFile( strInfoPath,GENERIC_READ | GENERIC_WRITE,NULL,NULL,
								CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN,NULL);
						}
						if (hFile != INVALID_HANDLE_VALUE)
						{
							SetFilePointer(hFile, 0, 0, FILE_END);
							DWORD temp = 0;
							FILETIME ft;
							HANDLE hFileTarget = CreateFile( ArrayData.szFileName,
								GENERIC_READ,
								FILE_SHARE_READ,
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL,
								NULL);
							if (hFileTarget != INVALID_HANDLE_VALUE)
							{
								ft = GetLastModifyTime(hFileTarget);
								CloseHandle(hFileTarget);
							}
							else
							{
								SYSTEMTIME st;
								GetSystemTime(&st);
								SystemTimeToFileTime(&st, &ft);
							}
							DecryptFileInfo.ulCompressLevel = ft.dwHighDateTime;
							DecryptFileInfo.ulMaxErrLimit = ft.dwLowDateTime;
							if (WriteFile(hFile, &DecryptFileInfo, sizeof(CONVERT_BUF), &temp, 0)
								&& temp == sizeof(CONVERT_BUF))
							{
								bWriteSuc = TRUE;
							}
							else
							{
								bWriteSuc = FALSE;
							}
							CloseHandle(hFile);
						}
						else
						{
							bWriteSuc = FALSE;
						}
						if (!bWriteSuc)
						{
							Sleep(10);
						}
						++cnt;
					} while(!bWriteSuc && cnt < 100);	
				}
#else
				YGNSMAddMonitorFile(&DecryptFileInfo);
#endif
			}

		}
	}

	return TRUE; 
}