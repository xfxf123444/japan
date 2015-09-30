#include "Function.h"
#include "..\FileDecrypt\Resource.h"
#include "..\..\DavinciFunction\DavinciFunction.h"
#include "..\..\..\..\Davinci_tech\lib\Crypto\Cur_ver\Export\Crypto.h"
#include "..\..\..\..\Davinci_tech\ZLib\Cur_ver\Export\ZLib.h"

/*===============================================================
 *
 * Function Name: SelectFile();
 * Purpose:       Choose a file to open
 *===============================================================*/
BOOL SelectFile(LPCTSTR szFileExt,LPCTSTR szFileType,char *szFile)
{
	BOOL bResult = FALSE;
	CString strFilter;
	strFilter.Format("%s (*.%s)|*.%s||",szFileType,szFileExt,szFileExt);
	CFileDialog dlg (TRUE, szFileExt, NULL, OFN_HIDEREADONLY, strFilter);

	// if user selected a file then open it as a database
	if (IDOK == dlg.DoModal())
	{
		strncpy(szFile,dlg.GetPathName(),MAX_PATH-1);
		bResult = TRUE;
	}
	return bResult;
}

BOOL SelectFolder(HWND hWnd,char *ach)
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

BOOL CheckPassword(UCHAR *pEcyPassword,LPCTSTR szPassword)
{
	unsigned char chStoreKey[PASSWORD_SIZE];
	memset(chStoreKey,0,sizeof(chStoreKey));

	int nStoreSize;
	nStoreSize = strlen(szPassword);

	if(nStoreSize > PASSWORD_SIZE )
	{
		nStoreSize = PASSWORD_SIZE;
	}

	// Encrypt key with key itself
	// may be this is single direction
	if( FALSE == YGAESEncryptData((unsigned char*)szPassword,strlen(szPassword),(unsigned char*)szPassword,chStoreKey,nStoreSize))
	{
		TRACE("\nYGAESEncryptData error in WriteTargetHead.");
		return FALSE;
	}

	return (!memcmp(pEcyPassword,chStoreKey,PASSWORD_SIZE));
}

BOOL GetRestoreFileHandle(LPCTSTR szSourceFile,LPCTSTR szTargetFile,HANDLE &hSourceFile,HANDLE &hTargetFile)
{
	hSourceFile = CreateFile(szSourceFile,GENERIC_READ,FILE_SHARE_READ | FILE_SHARE_WRITE,
		                      NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if( INVALID_HANDLE_VALUE == hSourceFile )
	{
		CString strOpenSourceFailed;
		strOpenSourceFailed.Format(IDS_OPEN_SOURCE_FAILED,szSourceFile);
#ifndef _YG_COMMAND_LINE
		AfxMessageBox((LPCTSTR)strOpenSourceFailed,MB_OK|MB_ICONINFORMATION,NULL);
#else
		cout << (LPCTSTR)strOpenSourceFailed << endl;
#endif
		return FALSE;
	}

	hTargetFile = CreateFile(szTargetFile,
		                     GENERIC_READ|GENERIC_WRITE,
							 FILE_SHARE_READ | FILE_SHARE_WRITE,
							 NULL,
							 CREATE_ALWAYS,
							 FILE_ATTRIBUTE_NORMAL,
							 NULL);

	if( INVALID_HANDLE_VALUE == hTargetFile )
	{
		CloseHandle(hSourceFile);
		CString strOpenTargetFailed;
		strOpenTargetFailed.Format(IDS_OPEN_TARGET_FAILED,szTargetFile);
#ifndef _YG_COMMAND_LINE
		AfxMessageBox((LPCTSTR)strOpenTargetFailed,MB_OK|MB_ICONINFORMATION,NULL);
#else
		cout << (LPCTSTR)strOpenTargetFailed << endl;
#endif
		return FALSE;
	}
	return TRUE;
}

ULONG CheckValidSource(LPCTSTR szSourceFile,LPCTSTR szPassword)
{
	FILE_ENCRYPT_HEAD FileEncryptHead;
	ULONG				ulImageVersion;

	// check source file
	DWORD dwFileAttribute;
	dwFileAttribute = GetFileAttributes(szSourceFile);
	
	if( 
		( -1 == dwFileAttribute ) ||  // if the path is not valid
		( 0 != ( FILE_ATTRIBUTE_DIRECTORY & dwFileAttribute ) ) // if it is a directory
	  )
	{
		TRACE("\nInvalid source file:%s",szSourceFile);
		CString strOpenSourceFailed;
		strOpenSourceFailed.Format(IDS_OPEN_SOURCE_FAILED,szSourceFile);
#ifndef _YG_COMMAND_LINE
		AfxMessageBox((LPCTSTR)strOpenSourceFailed,MB_OK|MB_ICONINFORMATION,NULL);
#else
		cout << (LPCTSTR)strOpenSourceFailed << endl;
#endif
		return 0;
	}

	HANDLE hImageFile;
	hImageFile = CreateFile(szSourceFile,GENERIC_READ,
							FILE_SHARE_READ | FILE_SHARE_WRITE,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL);

	if (hImageFile == INVALID_HANDLE_VALUE) return 0;

	ulImageVersion = GetFileEncryptHead(hImageFile,szPassword,FileEncryptHead);
	
	CloseHandle(hImageFile);

	return	ulImageVersion;
}

// 2004.09.15 added begin

// this function is used to get the FileCount
// and one file name in the image if the FileCount is 1

BOOL GetImageInfo(DECRYPT_INFO *pDecryInfo,DWORD &dwFileCount,CString &strOneFileName)
{
	dwFileCount = 0;
	strOneFileName.Empty();

	HANDLE hImageFile;
	hImageFile = CreateFile(pDecryInfo->szImageFile,
							GENERIC_READ,
							FILE_SHARE_READ | FILE_SHARE_WRITE,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL);

	if( INVALID_HANDLE_VALUE == hImageFile )
	{
		TRACE("\nCan not open image file.");
		return FALSE;
	}

	FILE_ENCRYPT_HEAD FileEncryptHead;
	ULONG				ulImageVersion;

    ulImageVersion = GetFileEncryptHead(hImageFile,pDecryInfo->szPassword,FileEncryptHead);

	__int64 linTargetFilePointer;
	switch (ulImageVersion)
	{
	case 100:
		if (!CheckPassword((UCHAR *)&((FILE_ENCRYPT_HEAD_100 *)&FileEncryptHead)->szPassword,pDecryInfo->szPassword))
		{
			CloseHandle(hImageFile);
			return FALSE;
		}
		if( FALSE == YGSetFilePointer(
										hImageFile,
										((FILE_ENCRYPT_HEAD_100 *)&FileEncryptHead)->FileInfoPointer.QuadPart, 
										FILE_BEGIN,
										linTargetFilePointer
									  ) )

		{
			TRACE("\nYGSetFilePointer error in GetImageInfo");
			CloseHandle(hImageFile);
			return FALSE;
		}
		break;
	case CURRENT_FILE_VERSION:
		if (!CheckPassword((UCHAR *)&FileEncryptHead.szPassword,pDecryInfo->szPassword))
		{
			CloseHandle(hImageFile);
			return FALSE;
		}
		if( FALSE == YGSetFilePointer(
										hImageFile,
										FileEncryptHead.FileInfoPointer.QuadPart, 
										FILE_BEGIN,
										linTargetFilePointer
									  ) )

		{
			TRACE("\nYGSetFilePointer error in GetImageInfo");
			CloseHandle(hImageFile);
			return FALSE;
		}
		break;
	default:
		TRACE("\nYGSetFilePointer error in GetImageInfo");
		CloseHandle(hImageFile);
		return FALSE;
	}

	ARRAY_NODE ArrayNode;
	DWORD		dwReadBytes;
	if( FALSE == ReadFile(hImageFile,&ArrayNode,sizeof(ArrayNode),&dwReadBytes,NULL) )
	{
		TRACE("\nRead ArrayNode error in GetImageInfo.");
		CloseHandle(hImageFile);
		return FALSE;
	}

	dwFileCount = ArrayNode.dwFileCount;

	if( ArrayNode.dwFileCount > ArrayNode.dwPathCount )
	{
		TRACE("\nError ArrayNode, dwFileCount should be less or equal dwPathCount.");
	}

	ARRAY_DATA ArrayData;
	DWORD dwItemIndex;
	for(dwItemIndex=1; dwItemIndex<=ArrayNode.dwPathCount; dwItemIndex++)
	{
		if( FALSE == ReadFile(hImageFile,&ArrayData,sizeof(ArrayData),&dwReadBytes,NULL) )
		{
			TRACE("\nRead ArrayData error in GetImageInfo.");
			CloseHandle(hImageFile);
			return FALSE;
		}

		if( FALSE == YGAESDecryptData((unsigned char*)pDecryInfo->szPassword,strlen(pDecryInfo->szPassword),(unsigned char*)(&ArrayData),(unsigned char*)(&ArrayData),sizeof(ArrayData)) )
		{
			  TRACE("\nYGAESEncryptData error in WriteTargetTail."); 
			  return FALSE;
		}

		if( 0 == (FILE_ATTRIBUTE_DIRECTORY & ArrayData.dwFileAttribute) )
		{
			// find the item
			strOneFileName = ArrayData.szFileName;
			CloseHandle(hImageFile);
			return TRUE;
		}
	}

	CloseHandle(hImageFile);
    return TRUE;
}

// 2004.09.15 added end