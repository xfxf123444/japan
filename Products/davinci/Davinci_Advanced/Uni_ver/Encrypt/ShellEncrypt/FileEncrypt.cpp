// FileEncrypt.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "FileEncrypt.h"
#include "ShellEncryptDlg.h"

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

	CString strCommandLine;
	strCommandLine = GetCommandLine();

	//MessageBox(0, LPCTSTR(strCommandLine), LPCTSTR(strCommandLine), 0);
	int nIndex = strCommandLine.Find(L':');
	if (nIndex == -1)
	{
		return false;
	}
	else
	{
		++nIndex;
		nIndex = strCommandLine.Find(L':', nIndex);
		if (nIndex == -1)
		{
			return false;
		}
		++nIndex;
		nIndex = strCommandLine.Find(L':', nIndex);
		if (nIndex != -1)
		{
			return false;
		}
	}

	CString strDir;
	strDir = strCommandLine.Mid(strCommandLine.Find(L'*') + 1); // skip the first ":"
	WCHAR wcBuf[MAX_PATH];
	ZeroMemory(wcBuf, MAX_PATH * sizeof(WCHAR));
	GetLongPathNameW(strDir, wcBuf, MAX_PATH);
	strDir = wcBuf;
	//MessageBox(0, LPCTSTR(strDir), LPCTSTR(strDir), 0);
	DWORD dwFileAttribute;
	dwFileAttribute = GetFileAttributes((LPCTSTR)strDir);

	if(-1 == dwFileAttribute )
	{
		//MessageBox(0, L"\nThe file or directory is not exist:", L"\nThe file or directory is not exist:", 0);
		TRACE(L"\nThe file or directory is not exist: %s",(LPCTSTR)strDir );
		return FALSE;
	}
	CString strFormat = strDir.Right(4);
	//MessageBox(0, LPCTSTR(strFormat), LPCTSTR(strFormat), 0);
	if (wcscmp(L".chy", LPCTSTR(strFormat)) == 0)
	{
		//MessageBox(0, LPCTSTR(L"false4"), LPCTSTR(L"false4"), 0);
		return false;
	}
	CShellEncryptDlg ShellEncryptDlg;
	ShellEncryptDlg.m_strSource = strDir;
	//MessageBox(0, LPCTSTR(strDir), LPCTSTR(strDir), 0);
	ShellEncryptDlg.DoModal();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
