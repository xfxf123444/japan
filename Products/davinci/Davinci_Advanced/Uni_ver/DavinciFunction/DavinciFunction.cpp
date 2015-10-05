#include "AFXDLGS.H"
#include "SHLOBJ.H"
#include "DavinciFunction.h"

#ifdef _SHELL_ENCRYPT
#include "..\Encrypt\ShellEncrypt\stdafx.h"
#include "..\Encrypt\ShellEncrypt\fileencrypt.h"
#endif

#ifdef _FILE_ENCRYPT
#include "..\Encrypt\FileEncrypt\stdafx.h"
#include "..\Encrypt\FileEncrypt\fileencrypt.h"
#endif

#ifdef FILE_DECRYPT
#include "..\Decrypt\FileDecrypt\stdafx.h"
#include "..\Decrypt\FileDecrypt\resource.h"
#endif

#ifdef	_YG_FS_MON
#include "..\YGFSMon\stdafx.h"
#include "..\YGFSMon\resource.h"
#endif

#ifdef SHELL_DECRYPT
#include "..\Decrypt\ShellDecrypt\stdafx.h"
#include "..\Decrypt\ShellDecrypt\resource.h"
#endif

#ifdef _YG_COMMAND_LINE
#include "..\FED\stdafx.h"
#include "..\FED\resource.h"
//#include "IOSTREAM.H"
#endif

DA_WORK_STATE g_WorkState;
BYTE g_byBuffer[ENCRYPT_BUFFER_SIZE+ENCRYPT_BUFFER_SIZE/5];
BYTE g_byBufferUnCompress[ENCRYPT_BUFFER_SIZE];

BOOL UpdateDecryptOption(HANDLE hImageFile,LPCTSTR szPassword,ENCRYPTOPTION *pEncryOption)
{
	FILE_ENCRYPT_HEAD FileEncryptHead;
	switch (GetFileEncryptHead(hImageFile,szPassword,FileEncryptHead))
	{
	case 100:
		break;
	case 0x200:
	case CURRENT_FILE_VERSION:
		memcpy(&FileEncryptHead.OptionInfo,pEncryOption,sizeof(ENCRYPTOPTION));
		SetFileEncryptHead(hImageFile,szPassword,FileEncryptHead,TRUE);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

ULONG GetFileEncryptHead(HANDLE hImageFile,LPCTSTR szPassword,FILE_ENCRYPT_HEAD &FileEncryptHead, BOOL isSelfExtractingFile = FALSE, const ULARGE_INTEGER* address = 0)
{
    LARGE_INTEGER liFilePointer;
	if (isSelfExtractingFile && address) {
		liFilePointer.QuadPart = address->QuadPart;
	}
	ULONG ulImageVersion = 0;
	DWORD dwReadBytes;
	
	YGSetFilePointer(hImageFile,0,FILE_BEGIN,liFilePointer.QuadPart);

	if( FALSE == ReadFile(hImageFile,&FileEncryptHead,sizeof(FileEncryptHead),&dwReadBytes,NULL) )
	{
		TRACE(L"\nReadFile error in GetFileEncryptHead.");
		return 0;
	}
	

	if (!wcscmp((WCHAR *)&FileEncryptHead,IMAGE_IDENTITY))//old version
		return 100;
	//if (!strcmp((char *)&FileEncryptHead,IMAGE_IDENTITY))//old version
	//	return 100;

	char chPassward[MAX_PATH];
	ZeroMemory(chPassward, sizeof(chPassward));
	WideCharToMultiByte(CP_ACP,0,szPassword,wcslen(szPassword), chPassward, MAX_PATH, NULL, NULL);
	if (FileEncryptHead.dwStructSize == sizeof(FILE_ENCRYPT_HEAD) && FileEncryptHead.dwFileVer == CURRENT_FILE_VERSION)
	{
		if( FALSE == YGAESDecryptData((UCHAR*)chPassward,strlen(chPassward),
			(unsigned char*)&FileEncryptHead.szImageIdentity,
			(unsigned char*)&FileEncryptHead.szImageIdentity,
			(IMAGE_IDENTITY_SIZE+PASSWORD_SIZE) * sizeof(WCHAR)))
		{
			return 0;
		}

		YGNormalDecryCode((unsigned char*)&FileEncryptHead.OptionInfo,sizeof(ENCRYPTOPTION));

		return FileEncryptHead.dwFileVer;
	}
	else
	{
		
		if( FALSE == YGAESDecryptData((unsigned char*)chPassward,strlen(chPassward),(unsigned char*)&FileEncryptHead,(unsigned char*)&FileEncryptHead,sizeof(FileEncryptHead)))
			return 0;

		if (!wcscmp(FileEncryptHead.szImageIdentity,IMAGE_IDENTITY))//new version
			return FileEncryptHead.dwFileVer;
	}

	//if (FileEncryptHead.dwStructSize == sizeof(FILE_ENCRYPT_HEAD) && FileEncryptHead.dwFileVer == CURRENT_FILE_VERSION)
	//{
	//	if( FALSE == YGAESDecryptData((unsigned char*)szPassword,strlen(szPassword),(unsigned char*)&FileEncryptHead.szImageIdentity,(unsigned char*)&FileEncryptHead.szImageIdentity,IMAGE_IDENTITY_SIZE+PASSWORD_SIZE))
	//		return 0;
	//	YGNormalDecryCode((unsigned char*)&FileEncryptHead.OptionInfo,sizeof(ENCRYPTOPTION));

	//	return FileEncryptHead.dwFileVer;
	//}
	//else
	//{
	//	if( FALSE == YGAESDecryptData((unsigned char*)szPassword,strlen(szPassword),(unsigned char*)&FileEncryptHead,(unsigned char*)&FileEncryptHead,sizeof(FileEncryptHead)))
	//		return 0;

	//	if (!strcmp(FileEncryptHead.szImageIdentity,IMAGE_IDENTITY))//new version
	//		return FileEncryptHead.dwFileVer;
	//}
	return 0;
}

BOOL SetFileEncryptHead(HANDLE hImageFile,LPCTSTR szPassword,FILE_ENCRYPT_HEAD &FileEncryptHead,BOOL bOptionOnly, const ULARGE_INTEGER* address = 0)
{
	FILE_ENCRYPT_HEAD FileEncryptHeadTemp;
    LARGE_INTEGER liFilePointer;
	if (address){
		liFilePointer.QuadPart = address->QuadPart;
	}
	DWORD dwWriteBytes;
	char chPassward[MAX_PATH];
	ZeroMemory(chPassward, sizeof(chPassward));
	WideCharToMultiByte(CP_ACP,0,szPassword,wcslen(szPassword), chPassward, MAX_PATH, NULL, NULL);
	switch (FileEncryptHead.dwFileVer)
	{
	case CURRENT_FILE_VERSION:
		if (bOptionOnly)
		{
			YGSetFilePointer(hImageFile,0,FILE_BEGIN,liFilePointer.QuadPart);

			if( FALSE == ReadFile(hImageFile,&FileEncryptHeadTemp,sizeof(FileEncryptHead),&dwWriteBytes,NULL) )
			{
				TRACE(L"\nReadFile error in GetFileEncryptHead.");
				return 0;
			}
			memcpy(&FileEncryptHead.szImageIdentity,&FileEncryptHeadTemp.szImageIdentity,(IMAGE_IDENTITY_SIZE+PASSWORD_SIZE)*sizeof(WCHAR));
		}
		else
		{
			//if( FALSE == YGAESEncryptData((unsigned char*)szPassword,wcslen(szPassword),(unsigned char*)&FileEncryptHead.szImageIdentity,(unsigned char*)&FileEncryptHead.szImageIdentity,IMAGE_IDENTITY_SIZE+PASSWORD_SIZE))
			//	return FALSE;
			if( FALSE == YGAESEncryptData((unsigned char*)chPassward,
				strlen(chPassward),
				(unsigned char*)&FileEncryptHead.szImageIdentity,
				(unsigned char*)&FileEncryptHead.szImageIdentity,
				(IMAGE_IDENTITY_SIZE+PASSWORD_SIZE) * sizeof(WCHAR)))
				return FALSE;
		}

		YGNormalEncryCode((unsigned char*)&FileEncryptHead.OptionInfo,sizeof(ENCRYPTOPTION));

		FileEncryptHead.ulCheckSum = 0;
		FileEncryptHead.ulCheckSum = YGNSMCheckSum((UCHAR *)&FileEncryptHead,sizeof(FILE_ENCRYPT_HEAD));
		break;
	case 0x200:
		FileEncryptHead.ulCheckSum = 0;
		FileEncryptHead.ulCheckSum = YGNSMCheckSum((UCHAR *)&FileEncryptHead,sizeof(FILE_ENCRYPT_HEAD));

		//if( FALSE == YGAESEncryptData((unsigned char*)szPassword,wcslen(szPassword),(unsigned char*)&FileEncryptHead,(unsigned char*)&FileEncryptHead,sizeof(FileEncryptHead)))
		//	return FALSE;
		if( FALSE == YGAESEncryptData((unsigned char*)chPassward,
			strlen(chPassward),
			(unsigned char*)&FileEncryptHead,
			(unsigned char*)&FileEncryptHead,
			sizeof(FileEncryptHead)))
			return FALSE;
		break;
	default:
		return FALSE;
	}


	YGSetFilePointer(hImageFile,0,FILE_BEGIN,liFilePointer.QuadPart);
	
	if( FALSE == WriteFile(hImageFile,&FileEncryptHead,sizeof(FILE_ENCRYPT_HEAD),&dwWriteBytes,NULL) )
	{
		DWORD lasterr = GetLastError();
		TRACE(L"\nWriteFile error in SetFileEncryptHead.");
		return FALSE;
	}

	return TRUE;

}

// 2004.01.07 added begin

BOOL YGSetFilePointer(HANDLE hFile, __int64 linDistance, DWORD dwMoveMethod, __int64 &linTargetFilePointer)
{
   LARGE_INTEGER liFilePointer;

   liFilePointer.QuadPart = linDistance;

   liFilePointer.LowPart = SetFilePointer (hFile, liFilePointer.LowPart, &liFilePointer.HighPart, dwMoveMethod);

   if (liFilePointer.LowPart == -1 && GetLastError() != NO_ERROR)
   {
	  linTargetFilePointer = -1;
	  return FALSE;
   }
   else
   {
	   linTargetFilePointer = liFilePointer.QuadPart;
	   return TRUE;
   }
}

// 2004.01.07 added end

void GetWorkState(DA_WORK_STATE &WorkState)
{
	WorkState = g_WorkState;
}

/*===============================================================
 *
 * Function Name: SelectFile();
 * Purpose:       Choose a file to open
 *===============================================================*/
BOOL SelectFile(LPCTSTR szFileExt,LPCTSTR szFileType,WCHAR *szFile)
{
	BOOL bResult = FALSE;
	CString strFilter;
	strFilter.Format(L"%s (*.%s)|*.%s||",szFileType,szFileExt,szFileExt);
	CFileDialog dlg (TRUE, szFileExt, NULL, OFN_HIDEREADONLY, strFilter);

	// if user selected a file then open it as a database
	if (IDOK == dlg.DoModal())
	{
		wcsncpy(szFile,dlg.GetPathName(),MAX_PATH-1);
		bResult = TRUE;
	}
	return bResult;
}


/*===============================================================
 *
 * Function Name: SelectFilePro();
 * Purpose:       Choose a file to save
 *===============================================================*/
BOOL SelectFilePro(LPCTSTR szFileExt,LPCTSTR szFileType,WCHAR *szFile)
{

	BOOL bResult = FALSE;
	CString strFilter;
	strFilter.Format(L"%s (*.%s)|*.%s||",szFileType,szFileExt,szFileExt);
	CFileDialog dlg (FALSE, szFileExt, NULL, OFN_HIDEREADONLY, strFilter);

	// if user selected a file then open it as a database
	if (IDOK == dlg.DoModal())
	{
		wcsncpy(szFile,dlg.GetPathName(),MAX_PATH-1);
		bResult = TRUE;
	}
	return bResult;
}

BOOL SelectFolder(HWND hWnd,WCHAR *ach)
{
	BOOL    bResult = FALSE;
    IMalloc *pm;
    BROWSEINFO bi;
    LPITEMIDLIST pidl;

    if (!FAILED(CoInitialize(NULL)))
	{
		bi.hwndOwner      = hWnd;
		bi.pidlRoot       = NULL;
		bi.pszDisplayName = ach;
		bi.lpszTitle      = NULL;
		bi.ulFlags        = BIF_RETURNONLYFSDIRS;
		bi.lpfn           = NULL;
		bi.lParam         = 0;
		bi.iImage         = 0;

		pidl = SHBrowseForFolder(&bi);

		if (pidl)
		{
			SHGetPathFromIDList(pidl, ach);

			// TrimRightChar(ach,'\\',MAX_PATH);

			SHGetMalloc(&pm);
			if (pm)
			{
				 pm->Free(pidl);
				 pm->Release();
			}
			bResult = TRUE;
		}
		CoUninitialize();
	}
	return bResult;
}

ULONG YGNSMCheckSum(UCHAR *pBuffer,ULONG ulSize)
{
	ULONG i,k=0;
	UCHAR ucCheckSum[4] = {0,0,0,0};
	for (i = 0;i < ulSize;i ++)
	{
		if (k >= 4) k = 0;
		ucCheckSum[k] += pBuffer[i];
		k++;
	}
	return *((ULONG *)&ucCheckSum[0]);
}

BOOL CheckDecryptOption(PENCRYPTOPTION pOption)
{
	CString strText,strTitle;

	if (!pOption->bOptionOn) return TRUE;
	if (pOption->bErrorLimit)
	{
		if	(pOption->uCurrentErr >= pOption->ulErrorLimit)
		{
			strText.Format(IDS_LIMIT_ERROR,pOption->ulErrorLimit);
#ifndef _YG_COMMAND_LINE
			strTitle.LoadString(IDS_APP_NAME);
			MessageBox(GetFocus(),strText,strTitle,MB_OK | MB_ICONWARNING);
#else
			wcout << (LPCTSTR)strText << endl;
#endif
			return FALSE;
		}
	}

	if (pOption->bLimitCount)
	{
		if (pOption->ulCurrentCount >= pOption->ulMaxCount)
		{
			strText.Format(IDS_REACH_LIMIT_COUNT,pOption->ulMaxCount);
#ifndef _YG_COMMAND_LINE
			strTitle.LoadString(IDS_APP_NAME);
			MessageBox(GetFocus(),strText,strTitle,MB_OK | MB_ICONWARNING);
#else
		    wcout << (LPCTSTR)strText << endl;
#endif
			return FALSE;
		}
	}

	if (pOption->bLimitTime)
	{
		CTime CurrentTime = CTime::GetCurrentTime();

		if ((pOption->LimitTime.GetYear() < CurrentTime.GetYear()) ||
			((pOption->LimitTime.GetYear() == CurrentTime.GetYear()) && (pOption->LimitTime.GetMonth() < CurrentTime.GetMonth())) ||
			((pOption->LimitTime.GetYear() == CurrentTime.GetYear()) && (pOption->LimitTime.GetMonth() == CurrentTime.GetMonth()) && (pOption->LimitTime.GetDay() < CurrentTime.GetDay())))
		{
			strText.Format(IDS_LIMIT_TIME,pOption->LimitTime.GetYear(),pOption->LimitTime.GetMonth(),pOption->LimitTime.GetDay());
#ifndef _YG_COMMAND_LINE
			strTitle.LoadString(IDS_APP_NAME);
			MessageBox(GetFocus(),strText,strTitle,MB_OK | MB_ICONWARNING);
#else
		    wcout << (LPCTSTR)strText << endl;
#endif
			return FALSE;
		}
	}

	return TRUE;
}

