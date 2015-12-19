// DeleteMaster.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DeleteMaster.h"
#include "Main.h"
#include "Fun.h"

//CFont				g_Font;
CImageList			g_imgList_BOX,g_imgList_LIST;
YG_PARTITION_INFO*	g_pFixDiskInfo;
YG_PARTITION_INFO*	g_pTargetParInfo = NULL;
BYTE				g_btSecBuf[BYTEINSEC];
int					g_nDelMothed;
int					g_nWipeMothed;
__int64				g_nTotalDelSize;
__int64				g_nDeletedSize;
BOOL				g_bVerify;
BOOL				g_bFormat;
DWORD				g_dwMaxTry;
BOOL                g_bBeepOff = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CDeleteMasterApp

BEGIN_MESSAGE_MAP(CDeleteMasterApp, CWinApp)
	//{{AFX_MSG_MAP(CDeleteMasterApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeleteMasterApp construction

CDeleteMasterApp::CDeleteMasterApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDeleteMasterApp object

CDeleteMasterApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDeleteMasterApp initialization

BOOL CDeleteMasterApp::InitInstance()
{
//	LOGFONT		lf;
	HANDLE		hMapping;
	BOOL		bPrevInstance = FALSE;
	CString		csError,csCaption;

	csCaption.LoadString (IDS_DM_ERROR);

	hMapping = CreateFileMapping((HANDLE)0xffffffff, NULL, PAGE_READONLY, 0, 32, _T("YGDM2003MAP"));
	if(hMapping)
	{
		if( GetLastError() == ERROR_ALREADY_EXISTS )
			bPrevInstance = TRUE;
	}
	else
	{
		// Some other error; handle error.
		csError.LoadString(IDS_APP_INIT_ERR);
		::MessageBox(NULL,csError,csCaption,MB_OK|MB_ICONSTOP);
		ExitProcess(1);
	}

	if(!bPrevInstance)
	{
		if(!IsUserAdmin())
		{
			csError.LoadString(IDS_NOT_ADMIN);
			::MessageBox(NULL,csError,csCaption,MB_OK|MB_ICONSTOP);
    		return FALSE;
		}
		if(!RegClass())	return FALSE;

		g_imgList_BOX.Create(IDB_BOX,16,0,RGB(255,255,255));
		g_imgList_LIST.Create(IDB_LIST,16,0,RGB(255,255,255));
		g_pFixDiskInfo = NULL;
		g_nDelMothed   = 0;
		g_nWipeMothed  = 0;
		g_bVerify	   = FALSE;
		g_bFormat	   = FALSE;
		g_dwMaxTry	   = 10;

		m_pMainWnd = new CMain();
		m_pMainWnd->ShowWindow(SW_SHOW);
	}
	return TRUE;
}

BOOL CDeleteMasterApp::RegClass()
{
	WNDCLASS	wndcls,wndSecInfo;
	CBrush		bs(bkRGB);

	memset(&wndcls, 0, sizeof(WNDCLASS));
    wndcls.style			= CS_BYTEALIGNWINDOW;
    wndcls.lpfnWndProc		= ::DefWindowProc;
    wndcls.hInstance		= AfxGetInstanceHandle();
    wndcls.hIcon			= LoadIcon(IDR_MAINFRAME); // or load a different
    wndcls.hCursor			= LoadStandardCursor(IDC_ARROW);
    wndcls.hbrBackground	= (HBRUSH)COLOR_WINDOW;
    wndcls.lpszMenuName		= NULL;
    wndcls.lpszClassName	= DMMainWinClass;

	memset(&wndSecInfo, 0, sizeof(WNDCLASS));
    wndSecInfo.style		 = CS_BYTEALIGNWINDOW;
    wndSecInfo.lpfnWndProc	 = ::DefWindowProc;
    wndSecInfo.hInstance	 = AfxGetInstanceHandle();
	wndSecInfo.hCursor		 = LoadStandardCursor (IDC_ARROW);
    wndSecInfo.hbrBackground = (HBRUSH)COLOR_WINDOWFRAME;
    wndSecInfo.lpszMenuName	 = NULL;
    wndSecInfo.lpszClassName = DMSecWinClass;

    if(!RegisterClass(&wndcls)) 
    	return FALSE;
    if(!RegisterClass(&wndSecInfo)) 
    	return FALSE;
	return TRUE;
}

typedef
BOOL (WINAPI * PCheckTokenMembership)(
  HANDLE TokenHandle,  // handle to access token
  PSID SidToCheck,     // SID
  PBOOL IsMember       // result
);

BOOL IsUserAdmin(VOID) 
{
	BOOL						b;
	PSID						AdministratorsGroup; 
	SID_IDENTIFIER_AUTHORITY	NtAuthority = SECURITY_NT_AUTHORITY;
	HMODULE						hModule;
	PCheckTokenMembership		func;
	
	b = AllocateAndInitializeSid(&NtAuthority,
									2,
									SECURITY_BUILTIN_DOMAIN_RID,
									DOMAIN_ALIAS_RID_ADMINS,
									0, 0, 0, 0, 0, 0,
									&AdministratorsGroup); 

	hModule = GetModuleHandle(_T("advapi32.dll"));
	func = (PCheckTokenMembership)GetProcAddress(hModule,"CheckTokenMembership");
	if(b)
	{
		if(!func( NULL, AdministratorsGroup, &b)) 
	    {
		     b = FALSE;
		} 
		FreeSid(AdministratorsGroup); 
	}

	return(b);
}

int CDeleteMasterApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	return CWinApp::ExitInstance();
}
