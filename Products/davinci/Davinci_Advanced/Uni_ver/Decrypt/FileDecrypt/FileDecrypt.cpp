// FileDecrypt.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "FileDecrypt.h"

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

#define CREATE_SELF_EXTRACTING_FILE

ULARGE_INTEGER g_selfExtractingFileAddress;
ULARGE_INTEGER g_selfExtractingFileSize;
BOOL g_isSelfExtractingFile;
BOOL g_checkSelfExtraingFileValid;
CString g_selfExtractingFilePath;
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
	CheckIsSelfExtractingFile();

	if (!g_isSelfExtractingFile || !g_checkSelfExtraingFileValid || g_selfExtractingFilePath.GetLength() >= MAX_PATH) {
			CString strText;
			CString strTitle;
			strText.LoadString(IDS_INVALID_IMAGE_FILE);
			strTitle.LoadString(IDS_APP_NAME);
			MessageBox(0, strText, strTitle, MB_OK | MB_ICONWARNING);
	}
	else {
		wcsncpy(g_DecryptInfo.szImageFile,(LPCTSTR)g_selfExtractingFilePath,MAX_PATH-1);
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

void CFileDecryptApp::CheckIsSelfExtractingFile()
{
	g_isSelfExtractingFile = FALSE;
	g_checkSelfExtraingFileValid = FALSE;
	TCHAR buf[MAX_PATH];
	memset(buf,0,sizeof(buf));
	GetModuleFileName(NULL,buf,MAX_PATH);
	GetLongPathName(buf, buf, MAX_PATH);
	g_selfExtractingFilePath = buf;
	
	ULARGE_INTEGER fileSize;
	fileSize.QuadPart = 0;
	HANDLE hFile = CreateFile(g_selfExtractingFilePath,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		fileSize.LowPart = GetFileSize(hFile, &fileSize.HighPart);
		if (SetFilePointer(hFile, sizeof(SELF_EXTRACTING_IDENTITY) + sizeof(ULARGE_INTEGER), 0, FILE_END) != INVALID_SET_FILE_POINTER) {
			WCHAR identityBuf[IMAGE_IDENTITY_SIZE];
			ZeroMemory(identityBuf, sizeof(identityBuf));
			DWORD dwRead = 0;
			if (ReadFile(hFile, identityBuf, sizeof(identityBuf), &dwRead, 0) && dwRead == sizeof(identityBuf)) {
				if (wcscmp(identityBuf, SELF_EXTRACTING_IDENTITY) == 0) {
					g_isSelfExtractingFile = TRUE;
					if (ReadFile(hFile, &g_selfExtractingFileAddress, sizeof(g_selfExtractingFileAddress), &dwRead, 0) 
						&& dwRead == sizeof(ULARGE_INTEGER)
						&& ReadFile(hFile, &g_selfExtractingFileSize, sizeof(g_selfExtractingFileSize), &dwRead, 0) 
						&& dwRead == sizeof(ULARGE_INTEGER)) {
						if (g_selfExtractingFileAddress.QuadPart + g_selfExtractingFileSize.QuadPart <= fileSize.QuadPart){
							g_checkSelfExtraingFileValid = TRUE;
						}
					}
				}
			}
		} 
	}
}
