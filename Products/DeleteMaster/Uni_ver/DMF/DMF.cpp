// DMF.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "DMF.h"
#include "SecureDelete.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int			 g_nDeleteMethod = 3;
CSecureDelete g_SecureDelete;
WCHAR         g_szSourceFile[MAX_PATH] = {0};
BOOL         g_bBeepOff = TRUE;
BOOL         g_bConfirm = TRUE;

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	char         szText[100];
	CTime        StartTime;
	CTime        EndTime;
	CString strText,strTemp1,strTemp2;
	int nRetCode = 0;

	wcout.imbue(locale(""));
	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		if (!ParseCmdLine(argc-1,&argv[1]))
			PrintUsage();
		else
		{
			strTemp1.LoadString(g_bBeepOff?IDS_BEEP_OFF:IDS_BEEP_ON);
			strTemp2.LoadString(IDS_00_METHOD+g_nDeleteMethod);
			strText.Format(IDS_SINGLE_SELECTED,g_szSourceFile,strTemp2,strTemp1);

			if (g_bConfirm)
			{
CMDAskForChoice1:
				wcout << (LPCTSTR)strText << endl;
				cin.getline(szText, 100); 
				if (stricmp(szText,"Y"))
				{
					if (!stricmp(szText,"N"))
					{
						return 0;
					}
					else 
					{
						goto CMDAskForChoice1;
					}
				}
			}

			if (!g_bBeepOff) Beep(1000,100);

			strText.Format(IDS_START_SECDEL,g_szSourceFile);
			wcout << (LPCTSTR)strText << endl;
			StartTime = CTime::GetCurrentTime();
			if (!g_SecureDelete.DoDeleteMasterDel(g_szSourceFile,g_nDeleteMethod))
			{
				EndTime = CTime::GetCurrentTime();
				WriteLog(g_szSourceFile,StartTime,EndTime,FALSE);
				if (!g_bBeepOff)
				{
					Beep(1500,500);
					Beep(1500,500);
					Beep(1500,500);
					Beep(1500,500);
					Beep(1500,500);
				}
				strText.LoadString(IDS_SECURE_DELETE_FAILURE);
				wcout << (LPCTSTR)strText << endl;
			}
			else
			{
				EndTime = CTime::GetCurrentTime();
				WriteLog(g_szSourceFile,StartTime,EndTime,TRUE);
				if (!g_bBeepOff)
				{
					Beep(2000,500);
					Beep(2000,500);
					Beep(2000,500);
				}
				strText.LoadString(IDS_SECURE_DELETE_FINISHED);
				wcout << (LPCTSTR)strText << endl;
			}
		}
	}
	return nRetCode;
}

BOOL ParseCmdLine(int argc, TCHAR* argv[])
{
	int i;
	CString strText;
	BOOL bInitFile = FALSE,bInitMethod = FALSE,bInitBeep = FALSE,bInitConfirm = FALSE;

	for (i = 0;i < argc;i ++)
	{
		switch (argv[i][0])
		{
		case '-':
		case '/':
			switch (argv[i][1])
			{
			case 'S':
			case 's':
				if (argc > 1) return FALSE;
				if (GetFileAttributes(&argv[i][2]) == -1)
				{
					strText.Format(IDS_OPEN_SOURCE_FAILED,&argv[i][2]);
					wcout << (LPCTSTR)strText << endl;
					return FALSE;
				}
				return ParseSettingFile(&argv[i][2]);
				break;
			case 'R':
			case 'r':
				if (bInitFile) return FALSE;
				wcscpy(g_szSourceFile,L"Recycler");
				bInitFile = TRUE;
				break;
			case 'F':
			case 'f':
				if (bInitFile) return FALSE;
		        wcsncpy(g_szSourceFile,&argv[i][2], MAX_PATH);
				if (GetFileAttributesW(g_szSourceFile) == -1)
				{
					strText.Format(IDS_OPEN_SOURCE_FAILED,g_szSourceFile);
					wcout << (LPCTSTR)strText << endl;
					return FALSE;
				}
				bInitFile = TRUE;
				break;
			case 'M':
			case 'm':
				if (bInitMethod) return FALSE;
				g_nDeleteMethod = (int)(argv[i][2]-'0');
				if (g_nDeleteMethod < 0 || g_nDeleteMethod > 7) return FALSE;
				bInitMethod = TRUE;
				break;
			case 'B':
			case 'b':
				if (bInitBeep) return FALSE;
				g_bBeepOff = FALSE;
				bInitBeep = TRUE;
				break;
			case 'N':
			case 'n':
				if (bInitConfirm) return FALSE;
				g_bConfirm = FALSE;
				bInitConfirm = TRUE;
				break;
			default:
				return FALSE;
			}
			break;
		default:
			return FALSE;
		}
	}

	return bInitFile && bInitMethod;
}

BOOL ParseSettingFile(WCHAR *szSettingFile)
{
	BOOL bInitFile = FALSE,bInitMethod = FALSE,bInitBeep = FALSE;
	HANDLE hFile;
	BYTE   *pBuff;
	DWORD  dwFileSize,i,k;
	BOOL   bResult = FALSE;
	CString strText;

	hFile = CreateFile(szSettingFile,GENERIC_READ|GENERIC_WRITE,
					 FILE_SHARE_READ, NULL,
					 OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
					 NULL);
	if (hFile == INVALID_HANDLE_VALUE) return FALSE;

	dwFileSize = GetFileSize(hFile,NULL);
	if (dwFileSize > 0)
	{
		pBuff = (BYTE *)malloc(dwFileSize+2);
		if (pBuff)
		{
			ReadFile(hFile,pBuff,dwFileSize,&dwFileSize,NULL);
			pBuff[dwFileSize] = '\0';
			for (i = 0;i < dwFileSize;i ++)
			{
				if (*((USHORT *)&pBuff[i]) == 0x0a0d)
				{
					*((USHORT *)&pBuff[i]) = 0;
					break;
				}
			}
			bResult = TRUE;
		}
	}
	CloseHandle(hFile);
	if (bResult)
	{
		if (pBuff[0] == '/' || pBuff[0] == '-')
		{
			for (i = 0;i < dwFileSize;i++)
			{
				if (pBuff[i] == '/' || pBuff[i] == '-')
				{
					switch(pBuff[i+1])
					{
					case 'R':
					case 'r':
						if (bInitFile)
						{
							bResult = FALSE;
							break;
						}
						wcscpy(g_szSourceFile,L"Recycler");
						bInitFile = TRUE;
						break;
					case 'F':
					case 'f':
						if (bInitFile)
						{
							bResult = FALSE;
							break;
						}
						if (pBuff[i+2] == '"')
						{
							for (k = i+3;k < dwFileSize;k ++)
							{
								if (pBuff[k] == '"')
								{
									pBuff[k] = '\0';
									break;
								}
							}
					        MultiByteToWideChar(CP_ACP, 0,(char *)&pBuff[i+3], -1, g_szSourceFile, MAX_PATH);
							if (GetFileAttributesW(g_szSourceFile) == -1)
							{
								strText.Format(IDS_OPEN_SOURCE_FAILED,g_szSourceFile);
								wcout << (LPCTSTR)strText << endl;
								bResult = FALSE;
								break;
							}
							bInitFile = TRUE;
						}
						break;
					case 'M':
					case 'm':
						if (bInitMethod) 
						{
							bResult = FALSE;
							break;
						}
						g_nDeleteMethod = (int)(pBuff[i+2]-'0');
						if (g_nDeleteMethod < 0 || g_nDeleteMethod > 7) return FALSE;
						bInitMethod = TRUE;
						break;
					case 'B':
					case 'b':
						if (bInitBeep)
						{
							bResult = FALSE;
							break;
						}
						g_bBeepOff = FALSE;
						bInitBeep = TRUE;
						break;
					default:
						bResult = FALSE;
						break;
					}
				}
			}
		}
		free(pBuff);
	}

	return bResult && bInitFile && bInitMethod;
}

void PrintUsage()
{
	CString strText;

	strText.LoadString(IDS_USAGE);
	wcout << (LPCTSTR)strText << endl;

	strText.LoadString(IDS_IMPORTANT);
	wcout << (LPCTSTR)strText << endl;

	strText.LoadString(IDS_EXPLAIN);
	wcout << (LPCTSTR)strText << endl;
	
}

BOOL WriteLog(WCHAR *szFile,CTime StartTime,CTime EndTime,BOOL bSucc)
{
	CString strText,strResult;
	WCHAR    szUserName[MAX_PATH],szLogFile[MAX_PATH];
	HANDLE  hFile;
	WORD    wUnicodeSign = 0;
	ULONG	ulEnter = 0x000a000d;
	ULONG	ulLen = MAX_PATH,dwWrtited;

	GetUserName(szUserName,&ulLen);

	strResult.LoadString(bSucc?IDS_RESULT_SUCC:IDS_RESULT_FAIL);
	strText.Format(IDS_LOG_FORMAT,szFile,szUserName,
					StartTime.GetYear(),StartTime.GetMonth(),StartTime.GetDay(),
					StartTime.GetHour(),StartTime.GetMinute(),StartTime.GetSecond(),
					EndTime.GetYear(),EndTime.GetMonth(),EndTime.GetDay(),
					EndTime.GetHour(),EndTime.GetMinute(),EndTime.GetSecond(),
					strResult);

	GetSystemDirectory(szLogFile,MAX_PATH);
	wcscat(szLogFile,L"\\dm.log");

	hFile = CreateFile(szLogFile,GENERIC_READ|GENERIC_WRITE,
					 FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
					 OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,
					 NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		strText.Format(IDS_LOG_FAIL,szLogFile);
		wcout << (LPCTSTR)strText << endl;
		return FALSE;
	}

	ReadFile(hFile,&wUnicodeSign,sizeof(WORD),&dwWrtited,NULL);
	if (wUnicodeSign != 0xfeff)
	{
		wUnicodeSign = 0xfeff;
		SetFilePointer(hFile,0,NULL,FILE_BEGIN);
		WriteFile(hFile,&wUnicodeSign,sizeof(WORD),&dwWrtited,NULL);
	}
	else SetFilePointer(hFile,0,NULL,FILE_END);
	WriteFile(hFile,strText.GetBuffer(0),strText.GetLength()*sizeof(WCHAR),&dwWrtited,NULL);
	WriteFile(hFile,&ulEnter,sizeof(DWORD),&dwWrtited,NULL);
	SetEndOfFile(hFile);
	CloseHandle(hFile);
	return TRUE;
}
BOOL GetUserSid(PSID *ppSid)
{
	HANDLE hToken;
	BOOL bRes;
	DWORD cbBuffer, cbRequired;
	PTOKEN_USER pUserInfo;

	// The User's SID can be obtained from the process token
	bRes = OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken);
	if (FALSE == bRes)
	{
		return FALSE;
	}

	// Set buffer size to 0 for first call to determine
	// the size of buffer we need.
	cbBuffer = 0;
	bRes = GetTokenInformation(hToken, TokenUser, NULL, cbBuffer, &cbRequired);
	if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
	{
		return FALSE;
	}

	// Allocate a buffer for our token user data
	cbBuffer = cbRequired;
	pUserInfo = (PTOKEN_USER) HeapAlloc(GetProcessHeap(), 0, cbBuffer);
	if (NULL == pUserInfo)
	{
		return FALSE;
	}

	// Make the "real" call
	bRes = GetTokenInformation(hToken, TokenUser, pUserInfo, cbBuffer, &cbRequired);
	if (FALSE == bRes) 
	{
		return FALSE;
	}

	// Make another copy of the SID for the return value
	cbBuffer = GetLengthSid(pUserInfo->User.Sid);

	*ppSid = (PSID) HeapAlloc(GetProcessHeap(), 0, cbBuffer);
	if (NULL == *ppSid)
	{
		return FALSE;
	}

	bRes = CopySid(cbBuffer, *ppSid, pUserInfo->User.Sid);
	if (FALSE == bRes)
	{
	    HeapFree(GetProcessHeap(), 0, *ppSid);
		return FALSE;
	}

	bRes = HeapFree(GetProcessHeap(), 0, pUserInfo);

	return TRUE;
}

void GetSidString(PSID pSid, WCHAR *szBuffer)
{
	WCHAR szTemp[32];
	int i;
	int iSubAuthorityCount;
	SID_IDENTIFIER_AUTHORITY *psia = GetSidIdentifierAuthority( pSid );
	DWORD dwTopAuthority = psia->Value[5];
	swprintf(szBuffer, L"S-1-%lu", dwTopAuthority);

	iSubAuthorityCount = *(GetSidSubAuthorityCount(pSid));
	for (i = 0; i<iSubAuthorityCount; i++) 
	{
		DWORD dwSubAuthority = *(GetSidSubAuthority(pSid, i));
		swprintf(szTemp, L"%lu", dwSubAuthority);
		wcscat(szBuffer, L"-");
		wcscat(szBuffer, szTemp);
	}
}

WCHAR *GetFileNameFromPath(WCHAR *szPath)
{
	DWORD dwFileAttr;
	WCHAR *p,szBkPath[MAX_PATH];
	wcscpy(szBkPath,szPath);
	do
	{
		p = wcsrchr(szBkPath,'\\');
		if (!p) return NULL;
		else
		{
			*p = '\0';
			dwFileAttr = GetFileAttributesW(szBkPath);
			if (dwFileAttr != -1 && dwFileAttr & FILE_ATTRIBUTE_DIRECTORY)
			{
				return &szPath[wcslen(szBkPath)];
			}
		}
	}while (TRUE);
	return NULL;
}
