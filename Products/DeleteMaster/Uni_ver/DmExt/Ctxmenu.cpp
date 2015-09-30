// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993-1996  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE:   ctxmenu.cpp
//
//  PURPOSE:   Implements the IContextMenu member functions necessary to support
//             the context menu portioins of this shell extension.  Context menu
//             shell extensions are called when the user right clicks on a file
//             (of the type registered for the shell extension--see SHELLEXT.REG
//             for details on the registry entries.  In this sample, the relevant
//             files are of type .GAK) in the Explorer, or selects the File menu 
//             item.
//

#include "priv.h"
#include "shellext.h"
#include "Resource.h"
#include <stdio.h>

BOOL GetInstallPath(WCHAR *szInstallPath);

extern HINSTANCE g_hmodThisDll;
//
//  FUNCTION: CShellExt::QueryContextMenu(HMENU, UINT, UINT, UINT, UINT)
//
//  PURPOSE: Called by the shell just before the context menu is displayed.
//           This is where you add your specific menu items.
//
//  PARAMETERS:
//    hMenu      - Handle to the context menu
//    indexMenu  - Index of where to begin inserting menu items
//    idCmdFirst - Lowest value for new menu ID's
//    idCmtLast  - Highest value for new menu ID's
//    uFlags     - Specifies the context of the menu event
//
//  RETURN VALUE:
//
//
//  COMMENTS:
//

STDMETHODIMP CShellExt::QueryContextMenu(HMENU hMenu,
                                         UINT indexMenu,
                                         UINT idCmdFirst,
                                         UINT idCmdLast,
                                         UINT uFlags)
{
    UINT	idCmd = idCmdFirst;
    WCHAR	szMenuText[MAX_PATH];
    BOOL	bAppendItems=TRUE;

    if ((uFlags & 0x000F) == CMF_NORMAL)  //Check == here, since CMF_NORMAL=0
    {
        LoadString(g_hmodThisDll,IDS_DEL_MENU,szMenuText,MAX_PATH);
    }
    else
        if (uFlags & CMF_VERBSONLY)
        {
	        LoadString(g_hmodThisDll,IDS_DEL_MENU,szMenuText,MAX_PATH);
        }
    else
        if (uFlags & CMF_EXPLORE)
        {
	        LoadString(g_hmodThisDll,IDS_DEL_MENU,szMenuText,MAX_PATH);
        }
    else
        if (uFlags & CMF_DEFAULTONLY)
        {
            bAppendItems = FALSE;
        }
    else
        {
            bAppendItems = FALSE;
        }

    if (bAppendItems)
    {
        InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);
        
        InsertMenu(hMenu,
                   indexMenu++,
                   MF_STRING|MF_BYPOSITION,
                   idCmd++,
                   szMenuText);
        
        InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

        return ResultFromShort(idCmd-idCmdFirst); //Must return number of menu
												  //items we added.
   }

   return NOERROR;
}

//
//  FUNCTION: CShellExt::InvokeCommand(LPCMINVOKECOMMANDINFO)
//
//  PURPOSE: Called by the shell after the user has selected on of the
//           menu items that was added in QueryContextMenu().
//
//  PARAMETERS:
//    lpcmi - Pointer to an CMINVOKECOMMANDINFO structure
//
//  RETURN VALUE:
//
//
//  COMMENTS:
//
STDMETHODIMP CShellExt::InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi)
{
	HRESULT hr = E_INVALIDARG;

	if (!HIWORD(lpcmi->lpVerb))
    {
        UINT idCmd = LOWORD(lpcmi->lpVerb);

        switch (idCmd)
        {
            case 0:
				hr = DoDelCommand(lpcmi->hwnd,
								lpcmi->lpDirectory,
								lpcmi->lpVerb,
								lpcmi->lpParameters,
								lpcmi->nShow);
				break;
		}
    }
    return hr;
}


STDMETHODIMP CShellExt::GetCommandString(UINT_PTR idCmd,
                                         UINT uFlags,
                                         UINT FAR *reserved,
                                         LPSTR pszName,
                                         UINT cchMax)
{
	//lstrcpy(pszName, "New menu item number 1");
    return NOERROR;
}


STDMETHODIMP CShellExt::DoDelCommand(HWND hParent,
                                   LPCSTR pszWorkingDir,
                                   LPCSTR pszCmd,
                                   LPCSTR pszParam,
                                   int iShowCmd)
{
	WCHAR					szCommand[MAX_PATH*(MAX_PATH+1)];

	GetInstallPath(szCommand);
	wcscat(szCommand,L"\\DelFile.exe");
	ShellExecute(GetFocus(),L"open",szCommand,m_szCommand,NULL,SW_SHOW);
    return NOERROR;
}

BOOL GetInstallPath(WCHAR *szInstallPath)
{
	WCHAR    szKey[MAX_PATH];
	DWORD	dwType,dwSize;
	HKEY	hKey;
 	WCHAR	RetuValue[MAX_PATH];
	dwType = REG_SZ;

    LoadString(g_hmodThisDll,IDS_PRODUCT_KEY,szKey,MAX_PATH);
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,szKey,0,KEY_QUERY_VALUE,&hKey) != ERROR_SUCCESS) return FALSE;

	dwSize = MAX_PATH * sizeof(WCHAR);
	if(RegQueryValueEx(hKey,L"DFInstallPath",NULL,&dwType,(LPBYTE)RetuValue,&dwSize) ==ERROR_SUCCESS) 
		wcscpy(szInstallPath,RetuValue);
	else wcscpy(szInstallPath,L"UnKnown");

	RegCloseKey(hKey);

	return TRUE;
}

