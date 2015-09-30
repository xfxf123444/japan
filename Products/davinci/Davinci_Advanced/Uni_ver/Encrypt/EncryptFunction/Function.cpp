#include "Function.h"
#include "EncryptFunction.h"

#ifdef _FILE_ENCRYPT
#include "..\FileEncrypt\resource.h"
#endif

#ifdef _SHELL_ENCRYPT
#include "..\ShellEncrypt\resource.h"
#endif

#ifdef _YG_COMMAND_LINE
#include <IOSTREAM.H>
#include "..\..\FED\resource.h"
#endif

#include "..\..\EncryptToolStruct\EncryptToolStruct.h"
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


/*===============================================================
 *
 * Function Name: SelectFilePro();
 * Purpose:       Choose a file to save
 *===============================================================*/
BOOL SelectFilePro(LPCTSTR szFileExt,LPCTSTR szFileType,char *szFile)
{

	BOOL bResult = FALSE;
	CString strFilter;
	strFilter.Format("%s (*.%s)|*.%s||",szFileType,szFileExt,szFileExt);
	CFileDialog dlg (FALSE, szFileExt, NULL, OFN_HIDEREADONLY, strFilter);

	// if user selected a file then open it as a database
	if (IDOK == dlg.DoModal())
	{
		strncpy(szFile,dlg.GetPathName(),MAX_PATH-1);
		bResult = TRUE;
	}
	return bResult;
}

BOOL GetTargetFileName(LPCTSTR szSourceFile,CString &strTargetFile)
{
	// get target file name

	CString strSourceFile;
	strSourceFile = szSourceFile;

	if("." == strSourceFile.Right(4).Left(1) )
	{
		// normal suffix, replace the suffix with ".chy"
		strTargetFile = strSourceFile.Left(strSourceFile.GetLength()-4) + "." + FILE_ENCRYPT_EXTENSION;
	}
	else
	{
		// not normal suffix, just append ".chy" to the original file name
		strTargetFile = strSourceFile + "." + FILE_ENCRYPT_EXTENSION;
	}
	return TRUE;
}

BOOL GetBackupFileHandle(LPCTSTR szSourceFile,LPCTSTR szTargetFile,HANDLE &hSourceFile,HANDLE &hTargetFile)
{
	
	SetFileAttributes(szTargetFile,FILE_ATTRIBUTE_NORMAL);
	
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


BOOL CheckPlacement(LPCTSTR szSourceDir, LPCTSTR szTargetImage)
{
	CString strSourceDir;
	CString strTargetImage;
	strSourceDir = szSourceDir;
	strTargetImage = szTargetImage;

	strSourceDir.TrimRight("\\");
	strSourceDir = strSourceDir + "\\";
	
	if( 0 == strTargetImage.Left(strSourceDir.GetLength()).CompareNoCase((LPCTSTR)strSourceDir) )
	{
		return FALSE;
	}
	return TRUE;
}


void AddDelMethod(CComboBoxEx* pBox)
{
	COMBOBOXEXITEM		cbItem;
	BYTE				i;
	CString				csMethod;

	cbItem.mask			  = CBEIF_TEXT;
	cbItem.iItem		  = 0;
	cbItem.cchTextMax	  = MAX_PATH;
	cbItem.iImage		  = 0;
	cbItem.iSelectedImage = 0;

	for(i = 0; i < 8 ; i ++)
	{
		switch(i)
		{
		case 0:
			csMethod.LoadString (IDS_00_METHOD);
			break;
		case 1:
			csMethod.LoadString (IDS_FF_METHOD);
			break;
		case 2:
			csMethod.LoadString (IDS_RANDOM_METHOD);
			break;
		case 3:
			csMethod.LoadString (IDS_NSA_METHOD);
			break;
		case 4:
			csMethod.LoadString (IDS_OLD_NSA_METHOD);
			break;
		case 5:
			csMethod.LoadString (IDS_DOD_STD_METHOD);
			break;
		case 6:
			csMethod.LoadString (IDS_NATO_METHOD);
			break;
		case 7:
			csMethod.LoadString (IDS_GUTMANN_METHOD);
			break;
		}

		char szMethod[MAX_PATH];
		memset(szMethod,0,sizeof(szMethod));
		strncpy(szMethod,(LPCTSTR)csMethod,MAX_PATH-1);
		cbItem.pszText = szMethod;
		cbItem.iItem  = i;
		pBox->InsertItem(&cbItem);
		pBox->SetItemData(cbItem.iItem,i);
	}
	pBox->SetCurSel(3);
}

