// FED.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "FED.h"
#include "..\Encrypt\EncryptFunction\EncryptInfo.h"
#include "..\Encrypt\EncryptFunction\EncryptFunction.h"
#include "..\DavinciFunction\SecureDelete.h"
#include "..\Decrypt\DecryptFunction\DecryptFunction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define OPERATION_UNKNOWN	    0
#define OPERATION_ENCRYPTION    1
#define OPERATION_DECRYPTION    2

void PrintUsage();
ULONG ParseCommandLine(int argc, TCHAR* argv[]);
BOOL EncryptCheckValidParam(CEncryptInfo *pEncryptInfo);
BOOL DecryptCheckValidParam(DECRYPT_INFO *pDecryptInfo);
BOOL GetPasswordFromFile(LPCTSTR szFile);

CEncryptInfo g_EncryptInfo;
DECRYPT_INFO g_DecryptInfo;
BOOL		 g_bSecurDelete = FALSE;
int			 g_nDeleteMethod = 0;
CSecureDelete g_SecureDelete;

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	locale temp = wcin.imbue(locale("jpn"));
	ULONG ulOperation = OPERATION_ENCRYPTION;
	CString strText;
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		wcerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		memset(&g_DecryptInfo,0,sizeof(DECRYPT_INFO));
		ulOperation = ParseCommandLine(argc-1,&argv[1]);
		switch (ulOperation)
		{
		case OPERATION_ENCRYPTION:
			if (EncryptCheckValidParam(&g_EncryptInfo))
			{
				if(EncryptSelection(g_EncryptInfo))
				{
					strText.LoadString(IDS_ENCRYPT_FINISHED);
					wcout << (LPCTSTR)strText << endl;
				}
				else
				{
					strText.LoadString(IDS_ENCRYPT_FAILED);
					wcout << (LPCTSTR)strText << endl;
				}
				if (g_bSecurDelete)
				{
					g_SecureDelete.DoDeleteMasterDel(g_EncryptInfo.m_strSourceArray[0].GetBuffer(0),g_nDeleteMethod);
					strText.LoadString(IDS_SECURE_DELETE_FINISHED);
					wcout << (LPCTSTR)strText << endl;
				}
				wcin.imbue(temp);
				return 0;
			}
			break;
		case OPERATION_DECRYPTION:
			if (DecryptCheckValidParam(&g_DecryptInfo))
			{
				if(DecryptSelectionFile(g_DecryptInfo))
				{
					strText.LoadString(IDS_DECRYPT_FINISHED);
					wcout << (LPCTSTR)strText << endl;
				}
				else
				{
					strText.LoadString(IDS_DECRYPT_FAILED);
					wcout << (LPCTSTR)strText << endl;
				}
				wcin.imbue(temp);
				return 0;
			}
			break;
		default:
			break;
		}
	}
	PrintUsage();
	wcin.imbue(temp);
	return nRetCode;
}

ULONG ParseCommandLine(int argc, TCHAR* argv[])
{
	int i;
	ULONG	ulOperation = OPERATION_UNKNOWN;

	if (argc < 3 || argc > 5) 	return OPERATION_UNKNOWN;

	for (i = 0;i < argc;i ++)
	{
		switch (argv[i][0])
		{
		case L'-':
		case L'/':
			switch (argv[i][1])
			{
			case L'I':
			case L'i':
				g_EncryptInfo.m_strTarget = &argv[i][2];
				wcsncpy((WCHAR *)&g_DecryptInfo.szImageFile,&argv[i][2],MAX_PATH);
				break;
			case L'E':
			case L'e':
				if (ulOperation != OPERATION_UNKNOWN) return OPERATION_UNKNOWN;
				ulOperation = OPERATION_ENCRYPTION;
				g_EncryptInfo.m_strSourceArray.Add(&argv[i][2]);
				break;
			case L'D':
			case L'd':
				if (ulOperation != OPERATION_UNKNOWN) return OPERATION_UNKNOWN;
				ulOperation = OPERATION_DECRYPTION;
				wcsncpy((WCHAR *)&g_DecryptInfo.szTargetPath,&argv[i][2],MAX_PATH);
				g_DecryptInfo.nTargetType = DECRYPT_RELATIVE_PLACE;
				g_DecryptInfo.nRestoreType = DECRYPT_ENTIRE;
				memset(g_DecryptInfo.szSelectedPath,0,MAX_PATH);
				break;
			case L'P':
			case L'p':
				if (-1 != GetFileAttributes(&argv[i][2]))
				{
					GetPasswordFromFile(&argv[i][2]);
				}
				else
				{
					g_EncryptInfo.m_strPassword = &argv[i][2];
					wcsncpy((WCHAR *)&g_DecryptInfo.szPassword,&argv[i][2],PASSWORD_SIZE);
				}
				break;
			case L'C':
			case L'c':
				g_EncryptInfo.m_dwCompressLevel = (ULONG)(argv[i][2]-L'0');
				break;
			case L'S':
			case L's':
				if (argc != 5) return OPERATION_UNKNOWN;
				g_nDeleteMethod = (int)(argv[i][2]-L'0');
				if (g_nDeleteMethod < 0 || g_nDeleteMethod > 7) return OPERATION_UNKNOWN;
				g_bSecurDelete = TRUE;
				break;
			default:
				return OPERATION_UNKNOWN;
			}
			break;
		default:
			return OPERATION_UNKNOWN;
		}
	}

	return ulOperation;
}

BOOL EncryptCheckValidParam(CEncryptInfo *pEncryptInfo)
{
	CString strText;

	if (pEncryptInfo->m_dwCompressLevel < 0 || pEncryptInfo->m_dwCompressLevel > 2)
	{
		strText.LoadString(IDS_INVALID_COMPRESS_LEVEL);
		wcout << (LPCTSTR)strText << endl;
		return FALSE;
	}

	if (-1 == GetFileAttributes(pEncryptInfo->m_strSourceArray[0]))
	{
		strText.Format(IDS_OPEN_SOURCE_FAILED,pEncryptInfo->m_strSourceArray[0]);
		wcout << (LPCTSTR)strText << endl;
		return FALSE;
	}

	if(pEncryptInfo->m_strPassword.GetLength() > PASSWORD_SIZE)
	{
		strText.LoadString(PASSWORD_TOO_LONG);
		wcout << (LPCTSTR)strText << endl;
		return FALSE;
	}

	if(pEncryptInfo->m_strPassword.GetLength() == 0)
	{
		strText.LoadString(IDS_EMPTY_PASSWORD);
		wcout << (LPCTSTR)strText << endl;
		return FALSE;
	}

	return TRUE;
}

BOOL DecryptCheckValidParam(DECRYPT_INFO *pDecryptInfo)
{
	CString strText;
	DWORD dwImageFileCount;
	CString strOneFileName;

	if (-1 == GetFileAttributes(pDecryptInfo->szImageFile))
	{
		strText.LoadString(IDS_INVALID_IMAGE_FILE);
		wcout << (LPCTSTR)strText << endl;
		return FALSE;
	}

	if(wcslen((WCHAR *)&pDecryptInfo->szPassword) > PASSWORD_SIZE)
	{
		strText.LoadString(PASSWORD_TOO_LONG);
		wcout << (LPCTSTR)strText << endl;
		return FALSE;
	}

	if(wcslen((WCHAR *)&pDecryptInfo->szPassword) == 0)
	{
		strText.LoadString(IDS_EMPTY_PASSWORD);
		wcout << (LPCTSTR)strText << endl;
		return FALSE;
	}

	if( FALSE == GetImageInfo(pDecryptInfo,dwImageFileCount,strOneFileName) )
	{
//		strText.LoadString(IDS_GET_IMAGEINFO_FAIL);
//		wcout << (LPCTSTR)strText << endl;
		return FALSE;
	}
	return TRUE;
}

BOOL GetPasswordFromFile(LPCTSTR szFile)
{
	HANDLE hFile;
	WCHAR   cBuff[PASSWORD_SIZE+1];
	ZeroMemory(cBuff, sizeof(cBuff));
	DWORD  dwFileSize,i;
	BOOL   bResult = FALSE;

	hFile = CreateFile(szFile,GENERIC_READ|GENERIC_WRITE,
							 FILE_SHARE_READ | FILE_SHARE_WRITE,
							 NULL,
							 OPEN_EXISTING,
							 FILE_ATTRIBUTE_NORMAL,
							 NULL);
	if (hFile == INVALID_HANDLE_VALUE) return FALSE;
	dwFileSize = GetFileSize(hFile,NULL);
	if (dwFileSize <= PASSWORD_SIZE && dwFileSize > 0)
	{
		ReadFile(hFile,cBuff,dwFileSize,&dwFileSize,NULL);
		cBuff[dwFileSize] = L'\0';
		for (i = 0;i < dwFileSize - sizeof(DWORD);i ++)
		{
			if (*((DWORD *)&cBuff[i]) == 0x000a000d)
			{
				*((DWORD *)&cBuff[i]) = 0;
				break;
			}
		}
		g_EncryptInfo.m_strPassword = (WCHAR *)cBuff;
		wcsncpy((WCHAR *)&g_DecryptInfo.szPassword,(WCHAR *)cBuff,PASSWORD_SIZE);
		bResult = TRUE;
	}
	CloseHandle(hFile);
	return bResult;
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
