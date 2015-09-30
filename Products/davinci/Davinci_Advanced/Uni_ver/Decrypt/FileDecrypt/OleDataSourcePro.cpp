// OleDataSourcePro.cpp: implementation of the COleDataSourcePro class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "filedecrypt.h"
#include "OleDataSourcePro.h"
#include "..\DecryptFunction\DecryptFunction.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CString  g_strDraggedItem;
extern DECRYPT_INFO g_DecryptInfo;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COleDataSourcePro::COleDataSourcePro()
{
	
}

COleDataSourcePro::~COleDataSourcePro()
{
	
}

BOOL COleDataSourcePro::OnRenderGlobalData(LPFORMATETC lpFormatEtc, HGLOBAL *phGlobal)
{
	TRACE(L"\nOnRenderGlobalData.");
	TRACE(L"HGLOBAL=%d",*phGlobal);
	TRACE(L"lpFormatEtc->cfFormat=%d,lpFormatEtc->tymed=%d",lpFormatEtc->cfFormat,lpFormatEtc->tymed);
	
	TRACE(L"\nlpFormatEtc->dwAspect=%d",lpFormatEtc->dwAspect);
	TRACE(L"\nlpFormatEtc->lindex =%d",lpFormatEtc->lindex);
	TRACE(L"\nlpFormatEtc->ptd=%d",lpFormatEtc->ptd);
	
	if( NULL == *phGlobal )
	{
		TRACE(L"\nAllocate a new global memory.");
		CString strDraggedItem;
		
		strDraggedItem = g_strDraggedItem;
		
		// 2004.09.10 second added begin
		// tbd
		COleDataSourcePro datasrc;
		HGLOBAL        hgDrop;
		DROPFILES*     pDrop;
		CStringList    lsDraggedFiles;
		POSITION       pos;
		UINT           uBuffSize = 0;
		TCHAR*         pszBuff;
		FORMATETC      etc = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
		
		
		// For selected item in the list, put the filename into lsDraggedFiles.
		
		// the file will be decrypted in a fixed directory first
		CString strModifiedPath;
		WCHAR szTempDir[MAX_PATH];
		GetTempPath(MAX_PATH,szTempDir);
		CString strTempDir;
		strTempDir = szTempDir;
		strTempDir.TrimRight(L'\\');
		
		if( strDraggedItem.GetLength() <= 3 )
		{
			strModifiedPath = strTempDir + L"\\" + L"Disk" + strDraggedItem.Left(1);
		}
		else
		{
			int nIndex;
			nIndex = strDraggedItem.ReverseFind(L'\\');
			strDraggedItem = strDraggedItem.Mid(nIndex+1);
			strModifiedPath = strTempDir + L"\\" + strDraggedItem;
		}
		
		lsDraggedFiles.AddTail (strModifiedPath);
		
		// Calculate the # of chars required to hold this string.
		
		uBuffSize += lstrlen ( strModifiedPath ) + 1;
		
		// Add 1 extra for the final null char, and the size of the DROPFILES struct.
		
		uBuffSize = sizeof(DROPFILES) + sizeof(TCHAR) * (uBuffSize + 1);
		
		// Allocate memory from the heap for the DROPFILES struct.
		
		hgDrop = GlobalAlloc ( GHND | GMEM_SHARE, uBuffSize );
		
		if ( NULL == hgDrop )
			return FALSE;
		
		pDrop = (DROPFILES*) GlobalLock ( hgDrop );
		
		if ( NULL == pDrop )
		{
			GlobalFree ( hgDrop );
			return FALSE;
		}
		
		// Fill in the DROPFILES struct.
		
		pDrop->pFiles = sizeof(DROPFILES);
		
#ifdef _UNICODE
		// If we're compiling for Unicode, set the Unicode flag in the struct to
		// indicate it contains Unicode strings.
		
		pDrop->fWide = TRUE;
#endif;
		
		// Copy all the filenames into memory after the end of the DROPFILES struct.
		
		pos = lsDraggedFiles.GetHeadPosition();
		pszBuff = (TCHAR*) (LPBYTE(pDrop) + sizeof(DROPFILES));
		
		while ( NULL != pos )
		{
			lstrcpy ( pszBuff, (LPCTSTR) lsDraggedFiles.GetNext ( pos ) );
			pszBuff = 1 + _tcschr ( pszBuff, L'\0' );
		}
		
		GlobalUnlock ( hgDrop );
		
		*phGlobal = hgDrop;
		
		// 2004.09.15 added begin

		DECRYPT_INFO DecryptInfo;
		memset(&DecryptInfo,0,sizeof(DecryptInfo));
		
		DecryptInfo.nRestoreType = DECRYPT_PART;
		DecryptInfo.nTargetType = DECRYPT_NEW_PLACE;
		wcsncpy(DecryptInfo.szImageFile,g_DecryptInfo.szImageFile,MAX_PATH-1);
		wcsncpy(DecryptInfo.szPassword,g_DecryptInfo.szPassword,PASSWORD_SIZE-1);
		wcsncpy(DecryptInfo.szSelectedPath,(LPCTSTR)g_strDraggedItem,MAX_PATH-1);
		GetTempPath(MAX_PATH-1,DecryptInfo.szTargetPath);
		
		// 2004.09.15 added end
		
		CacheGlobalData(CF_HDROP,hgDrop,lpFormatEtc);
	}
	else
	{
		TRACE(L"\nUsing existing global memeory.");
		CacheGlobalData(CF_HDROP,*phGlobal,lpFormatEtc);
	}
	return FALSE;
}


