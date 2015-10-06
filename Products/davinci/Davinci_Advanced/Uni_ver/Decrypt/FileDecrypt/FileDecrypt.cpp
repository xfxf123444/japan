// FileDecrypt.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "FileDecrypt.h"
#include "..\DecryptFunction\DecryptFunction.h"
#include "DecryptWiz_1.h"
#include "DecryptWiz_2.h"
#include "DecryptWiz_3.h"
#include "DecryptWiz_4.h"
#include "DecryptWiz_5.h"

#ifdef _DEBUG
#include "TraceWin.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TEST_SELF_EXTRACTING_FILE
#define CREATE_SELF_EXTRACTING_FILE
BOOL g_isSelfExtractingFile;
extern DECRYPT_INFO g_DecryptInfo;
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

#ifdef _DEBUG
	CMfxTrace::Init();
#endif
	if (!AfxOleInit())	return FALSE;
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	// CFileDecryptDlg dlg;
	// m_pMainWnd = &dlg;
	// int nResponse = dlg.DoModal();

#ifdef CREATE_SELF_EXTRACTING_FILE
	TCHAR buf[MAX_PATH];
	memset(buf,0,sizeof(buf));
	GetModuleFileName(NULL,buf,MAX_PATH);
	GetLongPathName(buf, buf, MAX_PATH);
	CString modulePath = buf;
	//memset(buf,0,sizeof(buf));
	//::GetTempPath(MAX_PATH, buf);
	//GetLongPathName(buf, buf, MAX_PATH);
	//CString tempDirPath = buf;
	BOOL isValid = FALSE;
	BOOL isSelfExtractingFile = FALSE;
	LARGE_INTEGER address;
	LARGE_INTEGER size;
	isSelfExtractingFile = CheckIsSelfExtractingFile(modulePath, address, size, isValid);
	if (!isSelfExtractingFile || !isValid || modulePath.GetLength() >= MAX_PATH) {
		CString strText;
		CString strTitle;
		strText.LoadString(IDS_INVALID_IMAGE_FILE);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(0, strText, strTitle, MB_OK | MB_ICONWARNING);
	}
	else {
		BOOL openationResult = FALSE;
		//CString tempFilePath = tempDirPath + modulePath.Mid(modulePath.ReverseFind(L'\\') + 1) + SELF_EXTRACTING_TEMP_EXTENSION;
		CString tempFilePath = modulePath + SELF_EXTRACTING_TEMP_EXTENSION;
		do {
			HANDLE hTempFile = CreateFile(tempFilePath,GENERIC_WRITE, NULL,
				NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_HIDDEN,NULL);
			SetFileAttributes(tempFilePath, FILE_ATTRIBUTE_HIDDEN);
			if (hTempFile == INVALID_HANDLE_VALUE) {
				break;
			}
			HANDLE hModule = CreateFile(modulePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
			if (hModule == INVALID_HANDLE_VALUE) {
				break;
			}
			if (!SetFilePointerEx(hModule, address, 0, FILE_BEGIN)) {
				break;
			}
			char chBuf[1024];
			ZeroMemory(chBuf, sizeof(chBuf));
			DWORD dwRead = 0;
			DWORD dwWrite = 0;
			LARGE_INTEGER remainToRead;
			remainToRead.QuadPart = size.QuadPart;
			LARGE_INTEGER totalRead;
			totalRead.QuadPart = 0;
			DWORD dwToRead = 0;
			if (remainToRead.QuadPart > sizeof(chBuf)) {
				dwToRead = sizeof(chBuf);
			}
			else {
				dwToRead = remainToRead.QuadPart;
			} 
			while (dwToRead != 0
				&& ReadFile(hModule, chBuf, dwToRead, &dwRead, 0)
				&& dwToRead == dwRead){
					totalRead.QuadPart += dwRead;
					remainToRead.QuadPart -= dwRead;
					if (WriteFile(hTempFile, chBuf, dwToRead, &dwWrite, 0)
						&& dwWrite == dwToRead) {
							if (remainToRead.QuadPart > sizeof(chBuf)) {
								dwToRead = sizeof(chBuf);
							}
							else {
								dwToRead = remainToRead.QuadPart;
							} 
							dwRead = dwWrite = 0;
							ZeroMemory(chBuf, sizeof(chBuf));
					}
					else {
						break;
					}
			}
			if (dwToRead == 0) {
				openationResult = TRUE;
			}
			CloseHandle(hTempFile);
			CloseHandle(hModule);
		} while (0);
		if (!openationResult) {
			CString strText;
			CString strTitle;
			strText.LoadString(IDS_INVALID_IMAGE_FILE);
			strTitle.LoadString(IDS_APP_NAME);
			MessageBox(0, strText, strTitle, MB_OK | MB_ICONWARNING);
			return FALSE;
		}

		g_isSelfExtractingFile = TRUE;
		wcsncpy(g_DecryptInfo.szImageFile,(LPCTSTR)tempFilePath,MAX_PATH-1);
		CPropertySheet DecryptWizard;
		CDecryptWiz_2 DecryptWiz_2;
		CDecryptWiz_3 DecryptWiz_3;
		CDecryptWiz_4 DecryptWiz_4;
		CDecryptWiz_5 DecryptWiz_5;

		DecryptWizard.AddPage(&DecryptWiz_2);
		DecryptWizard.AddPage(&DecryptWiz_3);
		DecryptWizard.AddPage(&DecryptWiz_4);
		DecryptWizard.AddPage(&DecryptWiz_5);

		DecryptWizard.SetWizardMode();

		DecryptWizard.DoModal();
		DeleteFile(tempFilePath);
	}
#else
	CPropertySheet DecryptWizard;
	CDecryptWiz_1 DecryptWiz_1;
	CDecryptWiz_2 DecryptWiz_2;
	CDecryptWiz_3 DecryptWiz_3;
	CDecryptWiz_4 DecryptWiz_4;
	CDecryptWiz_5 DecryptWiz_5;

	DecryptWizard.AddPage(&DecryptWiz_1);
	DecryptWizard.AddPage(&DecryptWiz_2);
	DecryptWizard.AddPage(&DecryptWiz_3);
	DecryptWizard.AddPage(&DecryptWiz_4);
	DecryptWizard.AddPage(&DecryptWiz_5);

	DecryptWizard.SetWizardMode();

	DecryptWizard.DoModal();
#endif
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}