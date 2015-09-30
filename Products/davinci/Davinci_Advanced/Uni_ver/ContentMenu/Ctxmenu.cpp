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
#include "ShellExtConst.h"

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

	if( m_bPathOverflow )
	{
		// if too much path, do not show the menu
        
		// NO_ERROR and FALSE are both zero, but the meaning is not the same
		return FALSE; 
	}

    UINT idCmd = idCmdFirst;
    TCHAR szEncryptMenu[MAX_PATH];
    TCHAR szDecryptMenu[MAX_PATH];
    
	m_bAppendEncryptMenu = FALSE;
	m_bAppendDecryptMenu = FALSE;

    if ((uFlags & 0x000F) == CMF_NORMAL)  //Check == here, since CMF_NORMAL=0
    {
        LoadString(g_hmodThisDll,IDS_ENCRYPT_MENU,szEncryptMenu,MAX_PATH);
        LoadString(g_hmodThisDll,IDS_DECRYPT_MENU,szDecryptMenu,MAX_PATH);
    }
    else if (uFlags & CMF_VERBSONLY)
	{
        LoadString(g_hmodThisDll,IDS_ENCRYPT_MENU,szEncryptMenu,MAX_PATH);
        LoadString(g_hmodThisDll,IDS_DECRYPT_MENU,szDecryptMenu,MAX_PATH);
	}
	else if (uFlags & CMF_EXPLORE)
	{
        LoadString(g_hmodThisDll,IDS_ENCRYPT_MENU,szEncryptMenu,MAX_PATH);
        LoadString(g_hmodThisDll,IDS_DECRYPT_MENU,szDecryptMenu,MAX_PATH);
	}
	else if (uFlags & CMF_DEFAULTONLY)
	{
		return FALSE;
	}
	else
	{
		return FALSE;
	}

	// 2004.04.28 added begin

	TCHAR szFileName[MAX_PATH];
	memset(szFileName,0,sizeof(szFileName));
	_tcsncpy(szFileName,m_szCommand,MAX_PATH - 1);

	TCHAR *pChar;
	pChar = _tcsstr(szFileName,_T("*"));
	if( NULL == pChar)
	{
		// Can not find a asterisk in the command line
        return FALSE;
	}
	else
	{
		(*pChar) = _T('\0');
	}

	TCHAR *pNextAsterisk;
	pNextAsterisk = _tcsstr(pChar + 1,_T("*"));
	
	if( NULL != pNextAsterisk )
	{
		// find another asterisk
	    // this program will not support multi selection
		return FALSE;
	}

	pChar = pChar - _tcslen(FILE_ENCRYPT_EXTENSION); // ".chy"

	if( 0 == _tcsnicmp(pChar,FILE_ENCRYPT_EXTENSION,_tcslen(FILE_ENCRYPT_EXTENSION) ) )
	{
		m_bAppendEncryptMenu = FALSE;
		m_bAppendDecryptMenu = TRUE;

		DWORD dwFileAttribute;
		dwFileAttribute = GetFileAttributes(szFileName);
		if( -1 == dwFileAttribute )
		{
			return FALSE;
		}
		if( dwFileAttribute & FILE_ATTRIBUTE_DIRECTORY )
		{
			// this is a directory
			return FALSE;
		}
	}
	else
	{
		m_bAppendEncryptMenu = TRUE;
		m_bAppendDecryptMenu = FALSE;
	}

	// 2004.04.28 added end

    if ( m_bAppendEncryptMenu || m_bAppendDecryptMenu )
    {
        InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

		if( m_bAppendEncryptMenu)
		{
			InsertMenu(hMenu,
					   indexMenu++,
					   MF_STRING|MF_BYPOSITION,
					   idCmd++,
					   szEncryptMenu);
		}
		
		if( m_bAppendDecryptMenu )
		{
			InsertMenu(hMenu,
					   indexMenu++,
					   MF_STRING|MF_BYPOSITION,
					   idCmd++,
					   szDecryptMenu);
		}

        InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

        return ResultFromShort(idCmd-idCmdFirst); //Must return number of menu
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
				if(m_bAppendEncryptMenu)
				{
					hr = DoFileEncrypt( /*lpcmi->hwnd,
										lpcmi->lpDirectory,
										lpcmi->lpVerb,
										lpcmi->lpParameters,
										lpcmi->nShow*/);
				}
				else if(m_bAppendDecryptMenu)
				{
					hr = DoFileDecrypt( /*lpcmi->hwnd,
										lpcmi->lpDirectory,
										lpcmi->lpVerb,
										lpcmi->lpParameters,
										lpcmi->nShow*/);
				}
				else
				{
					// some thing is wrong
					// if the function is invoked
					// one of m_bAppendEncryptMenu or m_bAppendDecryptMenu must be true
				}
				break;
			default:
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


STDMETHODIMP CShellExt::DoFileEncrypt( /*HWND hParent,
									   LPCTSTR pszWorkingDir,
									   LPCTSTR pszCmd,
									   LPCTSTR pszParam,
									   int iShowCmd*/)
{
	TCHAR szCommand[MAX_PATH*(MAX_PATH+1)];

	_tcscpy(szCommand,m_szInstallPath);

	_tcscat(szCommand,_T("\\"));
	_tcscat(szCommand,FILE_ENCRYPT_PROGRAM);
	//TCHAR szPath[MAX_PATH];
	//ZeroMemory(szPath, sizeof(szPath));
	//_tcscpy(szPath, szCommand);
	_tcscat(szCommand,_T(" "));
	_tcscat(szCommand,m_szCommand);
	//WinExec(szCommand,SW_SHOW);
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

	return NOERROR;
}

STDMETHODIMP CShellExt::DoFileDecrypt( /*HWND hParent,
									   LPCSTR pszWorkingDir,
									   LPCSTR pszCmd,
									   LPCSTR pszParam,
									   int iShowCmd*/)
{
	TCHAR szCommand[MAX_PATH*(MAX_PATH+1)];

	_tcscpy(szCommand,m_szInstallPath);

	_tcscat(szCommand,_T("\\"));
	_tcscat(szCommand,FILE_DECRYPT_PROGRAM);
	//TCHAR szPath[MAX_PATH];
	//ZeroMemory(szPath, sizeof(szPath));
	//_tcscpy(szPath, szCommand);
	_tcscat(szCommand,_T(" "));
	_tcscat(szCommand,m_szCommand);
	//WinExec(szCommand,SW_SHOW);
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

    return NOERROR;
}
