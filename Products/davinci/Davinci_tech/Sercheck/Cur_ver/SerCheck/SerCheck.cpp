// SerCheck.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "SerCheck.h"
#include "..\..\..\lib\Crypto\Cur_ver\Export\Crypto.h"
#include "..\export\SCApi.h"
#include "tracewin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CSerCheckApp

BEGIN_MESSAGE_MAP(CSerCheckApp, CWinApp)
	//{{AFX_MSG_MAP(CSerCheckApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSerCheckApp construction

CSerCheckApp::CSerCheckApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
#ifdef _DEBUG
	CMfxTrace::Init();
#endif

}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSerCheckApp object

CSerCheckApp theApp;

BOOL YGSCCheckSerial(char *szCompany,char *szProduct,char *szNumber,char *szSerial)
{
	DWORD dwKey,dwSerial;
	BYTE  btEcyKey[24];
	char  szKeyTmp[9],szKey1[11],szKey2[11],szTemp[24];
	int  i;

	if (strlen(szCompany) != 4 || 
		strlen(szProduct) != 3 || 
		strlen(szNumber) != 6 ||
		strlen(szSerial) != 6) return FALSE;

	dwKey = atoi(szNumber);
	strcpy(szKey1,szCompany);
	memcpy(&szKey2,&dwKey,4);
	for (i = 0;i < 4;i++)
	{
		szKeyTmp[2*i] = szKey1[i];
		szKeyTmp[2*i+1] = szKey2[i];
	}

	dwSerial = atoi(szProduct);
	YGGenerate3Key(dwKey,dwSerial,btEcyKey);
	YGDESEncryCode((LPBYTE)&btEcyKey,(LPBYTE)szKeyTmp,(LPBYTE)szKeyTmp,8,YGDESEDE3);

	dwKey = *((DWORD *)&szKeyTmp) + *((DWORD *)&szKeyTmp[4]);

	NumToText(dwKey,szTemp);
	return (!stricmp(szSerial,szTemp));
}

BOOL YGSCGenerateSerial(char *szSerial,PYGCSSTRU pGenSerInfo)
{
	DWORD dwKey,dwChkSerial = 0;
	BYTE  btEcyKey[24];
	char  szKeyTmp[9],szKey1[11],szKey2[11],szTemp[24];
	int  i;

	if (strlen(pGenSerInfo->szComp) != 4) return FALSE;
	if (pGenSerInfo->dwProduct < 1 || pGenSerInfo->dwProduct > 999) return FALSE;
	if (pGenSerInfo->dwNumber < 1 || pGenSerInfo->dwNumber > 999999) return FALSE;

	memcpy(&szKey1,&pGenSerInfo->szComp,4);
	memcpy(&szKey2,&pGenSerInfo->dwNumber,4);
	for (i = 0;i < 4;i++)
	{
		szKeyTmp[2*i] = szKey1[i];
		szKeyTmp[2*i+1] = szKey2[i];
	}

	YGGenerate3Key(pGenSerInfo->dwNumber,pGenSerInfo->dwProduct,btEcyKey);
	YGDESEncryCode((LPBYTE)&btEcyKey,(LPBYTE)szKeyTmp,(LPBYTE)szKeyTmp,8,YGDESEDE3);

	dwKey = *((DWORD *)&szKeyTmp) + *((DWORD *)&szKeyTmp[4]);

	strcpy(szSerial,pGenSerInfo->szComp);
	sprintf(szTemp,"-%3.3d",pGenSerInfo->dwProduct);
	strcat(szSerial,szTemp);
	sprintf(szTemp,"-%6.6d-",pGenSerInfo->dwNumber);
	strcat(szSerial,szTemp);
	NumToText(dwKey,szTemp);
	strcat(szSerial,szTemp);
	return TRUE;
}

BOOL NumToText(DWORD dwNum,char *szText)
{
	int i;
	DWORD dwNumLeft;
	for (i = 0;i < 6;i ++)
	{
		dwNumLeft = dwNum%36;
		if (dwNumLeft < 26)
			szText[i] = 'A' + (char)dwNumLeft;
		else
			szText[i] = '0' + (char)(dwNumLeft - 26);
		dwNum /= 36;
	}
	szText[6] = '\0';
	return TRUE;
}


BOOL JudgeWinXP()
{
    OSVERSIONINFOEX osvi;

	// Try calling GetVersionEx using the OSVERSIONINFOEX structure,
	// which is supported on Windows 2000.

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if( !( GetVersionEx ((OSVERSIONINFO *) &osvi) ) )
	{
		return FALSE;
	}
   
	if( VER_PLATFORM_WIN32_NT == osvi.dwPlatformId )
	{
		 if(
			  (
			    ( 5 == osvi.dwMajorVersion ) &&
			    ( osvi.dwMinorVersion >= 1)
			  )
			    ||
			  ( osvi.dwMajorVersion > 5 )
		   )
		 {
			// this is windows XP
			return TRUE;
		 }
   }
   return FALSE;
}


BOOL JudgeWin2K()
{
    OSVERSIONINFOEX osvi;

	// Try calling GetVersionEx using the OSVERSIONINFOEX structure,
	// which is supported on Windows 2000.

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if( !( GetVersionEx ((OSVERSIONINFO *) &osvi) ) )
	{
		return FALSE;
	}
   
	if( VER_PLATFORM_WIN32_NT == osvi.dwPlatformId )
	{
		 if(
			 ( 5 == osvi.dwMajorVersion ) &&
			 ( 0 == osvi.dwMinorVersion )
		   )
		 {
			// this is windows 2000
			return TRUE;
		 }
   }
   return FALSE;
}

BOOL InstallXPDriver()
{
	TRACE("\nInstallXPDriver.");

	char szWindowsDir[MAX_PATH];
	memset(szWindowsDir,0,sizeof(szWindowsDir));
	GetWindowsDirectory(szWindowsDir,sizeof(szWindowsDir));

	CString strInfLocation;
	strInfLocation = szWindowsDir;
	strInfLocation.TrimRight('\\');
	strInfLocation = strInfLocation + "\\" + "system32" + "\\" + "Drivers" + "\\" + YG_FILE_MONITOR_INF;

	if( -1 == GetFileAttributes(strInfLocation) )
	{
		TRACE("\nGetFileAttributes error in InstallXP Driver,can not locate inf file.");
		return FALSE;
	}

	CString strDriverLocation;
	strDriverLocation = szWindowsDir;
	strDriverLocation.TrimRight('\\');
	strDriverLocation = strDriverLocation + "\\" + "system32" + "\\" + "Drivers" + "\\" + YG_FILE_MONITOR_DRIVER;

	if( -1 == GetFileAttributes(strDriverLocation) )
	{
		TRACE("\nGetFileAttributes error in InstallXP Driver,can not locate driver file.");
		return FALSE;
	}

	CString strCommand;
	strCommand = strCommand + "rundll32.exe setupapi,InstallHinfSection DefaultInstall 132 " + strInfLocation;
	WinExec((LPCTSTR)strCommand,SW_HIDE);
	return TRUE;
}