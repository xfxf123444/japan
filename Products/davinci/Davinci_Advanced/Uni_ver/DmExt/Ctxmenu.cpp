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
  	HRESULT hr;
    UINT	idCmd = idCmdFirst;
	TCHAR	szTarPath[MAX_PATH];
    TCHAR	szMenuText[MAX_PATH];
	TCHAR	szShredder[MAX_PATH];
    BOOL	bAppendItems=TRUE;

    if ((uFlags & 0x000F) == CMF_NORMAL)  //Check == here, since CMF_NORMAL=0
    {
        LoadString(g_hmodThisDll,IDS_DEL_MENU,szMenuText,MAX_PATH);
		LoadString(g_hmodThisDll,IDS_DEL_SHREDDER,szShredder,MAX_PATH);
    }
    else
        if (uFlags & CMF_VERBSONLY)
        {
	        LoadString(g_hmodThisDll,IDS_DEL_MENU,szMenuText,MAX_PATH);
			LoadString(g_hmodThisDll,IDS_DEL_SHREDDER,szShredder,MAX_PATH);
        }
    else
        if (uFlags & CMF_EXPLORE)
        {
	        LoadString(g_hmodThisDll,IDS_DEL_MENU,szMenuText,MAX_PATH);
			LoadString(g_hmodThisDll,IDS_DEL_SHREDDER,szShredder,MAX_PATH);
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
        
		hr = SHGetSpecialFolderPath(NULL,szTarPath ,CSIDL_COMMON_DESKTOPDIRECTORY , FALSE);
		_tcscat(szTarPath,_T("\\Shredder*"));

		if(!_tcsicmp(szTarPath,m_szCommand))
		{
			InsertMenu(hMenu,
					   indexMenu++,
					   MF_STRING|MF_BYPOSITION,
					   idCmd++,
					   szShredder);
		}
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
				hr = DoDelCommand(/*lpcmi->hwnd,
								lpcmi->lpDirectory,
								lpcmi->lpVerb,
								lpcmi->lpParameters,
								lpcmi->nShow*/);
				break;
			case 1:
				hr = DoDelShredder(/*lpcmi->hwnd,
								lpcmi->lpDirectory,
								lpcmi->lpVerb,
								lpcmi->lpParameters,
								lpcmi->nShow*/);
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


STDMETHODIMP CShellExt::DoDelCommand(/*HWND hParent,
                                   LPCSTR pszWorkingDir,
                                   LPCSTR pszCmd,
                                   LPCSTR pszParam,
                                   int iShowCmd*/)
{
	TCHAR					szCommand[MAX_PATH*(MAX_PATH+1)];

	_tcscpy(szCommand,m_szInstallPath);
	_tcscat(szCommand,_T("\\DelFile.exe "));
	_tcscat(szCommand,m_szCommand);

	//TCHAR szPath[MAX_PATH];
	//ZeroMemory(szPath, MAX_PATH * sizeof(TCHAR));
	//_tcscpy(szPath,m_szInstallPath);
	//_tcscat(szPath,_T("\\DelFile.exe"));

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );
	CreateProcess( NULL,   // No module name (use command line)
		szCommand,      // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi );          // Pointer to PROCESS_INFORMATION structure

	//WinExec(szCommand,SW_SHOW);
	// CreateProcess(szPath, szCommand, 0, 0, 0, 0, 0, 0, 0, 0);
    return NOERROR;
}

STDMETHODIMP CShellExt::DoDelShredder(/*HWND hParent,
                                   LPCSTR pszWorkingDir,
                                   LPCSTR pszCmd,
                                   LPCSTR pszParam,
                                   int iShowCmd*/)
{
	TCHAR					szCommand[MAX_PATH*(MAX_PATH+1)];

	_tcscpy(szCommand,m_szInstallPath);
	_tcscat(szCommand,_T("\\DelFile.exe *"));
	_tcscat(szCommand,m_szCommand);

	//TCHAR szPath[MAX_PATH];
	//ZeroMemory(szPath, MAX_PATH * sizeof(TCHAR));
	//_tcscpy(szPath,m_szInstallPath);
	//_tcscat(szPath,_T("\\DelFile.exe"));

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );
	CreateProcess( NULL,   // No module name (use command line)
		szCommand,      // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi );          // Pointer to PROCESS_INFORMATION structure
	//WinExec(szCommand,SW_SHOW);
    return NOERROR;
}
