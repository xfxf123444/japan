// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993-1996  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE:   shexinit.cpp
//
//  PURPOSE:   Implements the IShellExtInit member function necessary to support
//             the context menu and property sheet portioins of this shell extension.  
//

#include "priv.h"
#include "shellext.h"

//
//  FUNCTION: CShellExt::Initialize(LPCITEMIDLIST, LPDATAOBJECT, HKEY)
//
//  PURPOSE: Called by the shell when initializing a context menu or property
//           sheet extension.
//
//  PARAMETERS:
//    pIDFolder - Specifies the parent folder
//    pDataObj  - Spefifies the set of items selected in that folder.
//    hRegKey   - Specifies the type of the focused item in the selection.
//
//  RETURN VALUE:
//
//    NOERROR in all cases.
//
//  COMMENTS:   Note that at the time this function is called, we don't know 
//              (or care) what type of shell extension is being initialized.  
//              It could be a context menu or a property sheet.
//
extern HINSTANCE g_hmodThisDll;

#define    FileDelInstallInfoKey   _T("SOFTWARE\\FRONTLINE\\AngoukaMaster\\2.00")

STDMETHODIMP CShellExt::Initialize(LPCITEMIDLIST pIDFolder,
                                   LPDATAOBJECT pDataObj,
                                   HKEY hRegKey)
{
	if (pDataObj == 0)
	{
		return E_INVALIDARG;
	}
	HRESULT hr = E_FAIL;
	FORMATETC	format={CF_HDROP,NULL,DVASPECT_CONTENT,-1,TYMED_HGLOBAL};
	STGMEDIUM	medium;
	int			i;
	TCHAR		szDirectory[MAX_PATH];

	TCHAR szAppData[MAX_PATH];

	/*hr = */SHGetSpecialFolderPath(NULL, szAppData,CSIDL_COMMON_DESKTOPDIRECTORY ,FALSE);
    // Initialize can be called more than once
    if (m_pDataObj)
    	m_pDataObj->Release();
    // duplicate the object pointer and registry handle
    if (pDataObj)
    {
    	m_pDataObj = pDataObj;
    	pDataObj->AddRef();
    }
	GatherAppData();

	if (SUCCEEDED(pDataObj->GetData(&format,&medium)))
	{
		HDROP hDrop = static_cast<HDROP>(GlobalLock(medium.hGlobal));
		if (hDrop)
		{
			m_nSelected=DragQueryFile((HDROP&)medium.hGlobal,0xFFFFFFFF,NULL,0);
			for(i=0;i<m_nSelected;i++)
			{
				DragQueryFile((HDROP&)medium.hGlobal,i,szDirectory,MAX_PATH);
				if(i==0)
				{
					_tcscpy(m_szCommand,szDirectory);
					_tcscat(m_szCommand,_T("*"));
				}
				else
				{
					_tcscat(m_szCommand,szDirectory);
					_tcscat(m_szCommand,_T("*"));
				}
				hr = S_OK;
			}
			GlobalUnlock(medium.hGlobal);
		}

	}

    return hr;
}

BOOL CShellExt::GatherAppData()
{
	DWORD	dwType,dwSize;
	HKEY	hKey;
 	TCHAR	RetuValue[MAX_PATH];
	dwType = REG_SZ;

	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,FileDelInstallInfoKey,0,KEY_QUERY_VALUE,&hKey) != ERROR_SUCCESS) return FALSE;

	dwSize = sizeof(RetuValue);
	if(RegQueryValueEx(hKey,_T("InstallPath"),NULL,&dwType,(LPBYTE)RetuValue,&dwSize) ==ERROR_SUCCESS) 
		_tcscpy(m_szInstallPath,RetuValue);
	else _tcscpy(m_szInstallPath,_T("UnKnown"));

	RegCloseKey(hKey);

	return TRUE;
}
