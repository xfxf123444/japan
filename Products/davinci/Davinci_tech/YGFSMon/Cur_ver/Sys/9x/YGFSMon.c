//======================================================================
//
// YGFSMon.c - main module for VxD YGFSMON
//
// SysInternals - www.sysinternals.com
// Copyright (C) 1996-2000 Mark Russinovich and Bryce Cogswell
//
//======================================================================
#define WANTVXDWRAPS
#include <basedef.h>
#include <vmm.h>
#include <vmmreg.h>
#include <vxdwraps.h>
#include <ifs.h>
#include <ifsmgr.h>
#include <vwin32.h>
#include <winerror.h>
#include  "YGFSMon.h"

//----------------------------------------------------------------------
//                     G L O B A L   D A T A 
//----------------------------------------------------------------------

//
// FileMon defines
//
ppIFSFileHookFunc       PrevIFSHookProc;
FSDVOLINFO              FsVolTable[MAXDRIVELETTER];
//Regmon defines

HANDLE   		        hRegHashTable;
HANDLE   				hFSHashTable;
PDWORD                  VMMWin32ServiceTable;
BOOL                    g_bRecord = FALSE;

LONG (*OldVMMRegOpenKey)(VMMHKEY, PCHAR, PVMMHKEY );
LONG (*OldVMMRegCloseKey)(VMMHKEY );
LONG (*OldVMMRegFlushKey)(VMMHKEY );
LONG (*OldVMMRegCreateKey)(VMMHKEY, PCHAR, PVMMHKEY );
LONG (*OldVMMRegDeleteKey)(VMMHKEY, PCHAR );
LONG (*OldVMMRegDeleteValue)(VMMHKEY, PCHAR );
LONG (*OldVMMRegEnumKey)(VMMHKEY, DWORD, PCHAR, DWORD );
LONG (*OldVMMRegEnumValue)(VMMHKEY, DWORD, PCHAR, PDWORD, PDWORD, PDWORD,
                         PBYTE, PDWORD );
LONG (*OldVMMRegQueryInfoKey)(VMMHKEY, PCHAR, PDWORD, DWORD, PDWORD, PDWORD,
                            PDWORD, PDWORD, PDWORD, PDWORD, PDWORD, 
                            _FILETIME *);
LONG (__stdcall *OldVMMWin32RegQueryInfoKey)(CLIENT_STRUCT*, DWORD, 
                                           VMMHKEY, PDWORD, PDWORD, PDWORD, 
                                           PDWORD, PDWORD );
LONG (*OldVMMRegQueryValue)( VMMHKEY, PCHAR, PCHAR, PLONG );
LONG (*OldVMMRegQueryValueEx)(VMMHKEY, PCHAR, PDWORD, PDWORD, PBYTE, PDWORD );
LONG (*OldVMMRegSetValue)( VMMHKEY, PCHAR, DWORD, PCHAR, DWORD );
LONG (*OldVMMRegSetValueEx)(VMMHKEY, PCHAR, DWORD, DWORD, PBYTE, DWORD );
LONG (*OldVMMRegRemapPreDefKey)(VMMHKEY, VMMHKEY );
LONG (*OldVMMRegQueryMultipleValues)(VMMHKEY, PVALENT, DWORD, PCHAR, PDWORD );
LONG (*OldVMMRegCreateDynKey)( PCHAR, PVOID, PVOID, PVOID, DWORD, PVMMHKEY);

BOOL CreateDirAnsi(char *szDir)
{  
   int      j = 0,nResult;
   char     cTemp;
   WORD     wAttrib; 

   do 
   {
	   j++;
	   nResult = 1;
	   if((szDir[j] == '\\' || szDir[j] == '\0') && j > 2)		              //get first the node of the full directory to dire[]
	   {
	   		cTemp = szDir[j];
		   szDir[j] = '\0';
		   if (!R0_GetFileAttributes(szDir,&wAttrib))
		   {
			   if (wAttrib & FILE_ATTRIBUTE_DIRECTORY)
				   nResult = ERROR_ALREADY_EXISTS;
		   }
		   else nResult = CreateDirectory(szDir);
		   szDir[j] = cTemp;
	   }
   }   while (szDir[j] != '\0');

   if (nResult && nResult != ERROR_ALREADY_EXISTS) return FALSE;
   return TRUE;
}

BOOL YGFSMonConvertRegPath(char *szPath,BOOL bDir)
{
	int i;
	
	for (i = 0;szPath[i];i ++)
	{
		if (!bDir && szPath[i] == '\\')
		{
			CONVERTCHAR(szPath,i,'A');
		}
		else
		{
			switch (szPath[i])
			{
			case '/':
				CONVERTCHAR(szPath,i,'B');
				break;
			case ':':
				CONVERTCHAR(szPath,i,'C');
				break;
			case '*':
				CONVERTCHAR(szPath,i,'D');
				break;
			case '?':
				CONVERTCHAR(szPath,i,'E');
				break;
			case '"':
				CONVERTCHAR(szPath,i,'F');
				break;
			case '<':
				CONVERTCHAR(szPath,i,'G');
				break;
			case '>':
				CONVERTCHAR(szPath,i,'H');
				break;
			case '|':
				CONVERTCHAR(szPath,i,'I');
				break;
			}
		}
	}
	return TRUE;
}

BOOL YGFSMonSaveRegData(UCHAR ucDrive,PCHAR pRegPath,PCHAR pValue,DWORD dwType,UCHAR *pData,DWORD dwDataLen)
{
	char szPath[MAX_PATH],szFile[MAX_PATH];
	DWORD hFile,dwAction;
	REGDATAHEAD RegHead;

	RegHead.dwType = dwType;
	RegHead.dwDataLen = dwDataLen;
    _lstrcpyn(szPath, "C:\\YGReg\\",_lstrlen("C:\\YGReg\\")+1);
	szPath[0] = ucDrive;
	YGFSMonConvertRegPath(pRegPath,TRUE);
	strcat(szPath,pRegPath);
	if (CreateDirAnsi(szPath))
	{
		strcat(szPath,"\\");
		_lstrcpyn(szFile,(pValue&&pValue[0])?pValue:"(default)",_lstrlen((pValue&&pValue[0])?pValue:"(default)")+1);
		YGFSMonConvertRegPath(szFile,FALSE);
		strcat(szPath,szFile);
		if (!R0_OpenCreateFile(FALSE, ACCESS_READWRITE | SHARE_COMPATIBILITY,
							   0,ACTION_OPENALWAYS, 0, szPath, &hFile,&dwAction))
		{
			R0_WriteFile(FALSE,hFile,sizeof(REGDATAHEAD),0,(UCHAR *)&RegHead,&dwAction);
			R0_WriteFile(FALSE,hFile,dwDataLen,sizeof(REGDATAHEAD),pData,&dwAction);
			R0_CloseFile(hFile);
		}
		else _asm int 3
	}
	else _asm int 3
	return TRUE;
}

BOOL YGFSMonDeleteRegData(UCHAR ucDrive,PCHAR pRegPath,PCHAR pValue)
{
	char szPath[MAX_PATH],szFile[MAX_PATH];
    _lstrcpyn(szPath, "C:\\YGReg\\",_lstrlen("C:\\YGReg\\")+1);
	szPath[0] = ucDrive;
	YGFSMonConvertRegPath(pRegPath,TRUE);
	strcat(szPath,pRegPath);
	strcat(szPath,"\\");
	_lstrcpyn(szFile,(pValue&&pValue[0])?pValue:"(default)",_lstrlen((pValue&&pValue[0])?pValue:"(default)")+1);
	YGFSMonConvertRegPath(szFile,FALSE);
	strcat(szPath,szFile);
	R0_DeleteFile(szPath,0);
	return TRUE;
}

BOOL YGFSMonSaveRegKey(UCHAR ucDrive,PCHAR pRegPath)
{
	char szPath[MAX_PATH];
	return TRUE;
    _lstrcpyn(szPath, "C:\\YGReg\\",_lstrlen("C:\\YGReg\\")+1);
	szPath[0] = ucDrive;
	YGFSMonConvertRegPath(pRegPath,TRUE);
	strcat(szPath,pRegPath);
	return CreateDirAnsi(szPath);
}

BOOL DeleteDirectory(char *szPath,_WIN32_FIND_DATA *pFindData)
{
    DWORD	     hFind;
    int 	     nPathLen;

    nPathLen = _lstrlen(szPath);
    _lstrcpyn(&szPath[nPathLen],"\\*.*",6);
    if (!R0_FindFirstFile(0x37,szPath,pFindData,&hFind))
    {
		do
		{
			if (stricmp((PCHAR)pFindData->cFileName,".") &&
				stricmp((PCHAR)pFindData->cFileName,".."))
			{
				_lstrcpyn(&szPath[nPathLen+1],(PCHAR)pFindData->cFileName,MAX_PATH-nPathLen);
				if (pFindData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					DeleteDirectory(szPath,pFindData);
					RemoveDirectory(szPath);
				}
				else	R0_DeleteFile(szPath,(WORD)pFindData->dwFileAttributes);
			}
		}while (!R0_FindNextFile(hFind,pFindData));
		R0_FindClose(hFind);
    }
    szPath[nPathLen] = '\0';
	RemoveDirectory(szPath);
    return TRUE;
}

BOOL YGFSMonDeleteRegKey(UCHAR ucDrive,PCHAR pRegPath)
{
	_WIN32_FIND_DATA FindData;
	char szPath[MAX_PATH];
    _lstrcpyn(szPath, "C:\\YGReg\\",_lstrlen("C:\\YGReg\\")+1);
	szPath[0] = ucDrive;
	YGFSMonConvertRegPath(pRegPath,TRUE);
	strcat(szPath,pRegPath);
	return DeleteDirectory(szPath,&FindData);
}

void YGFSMonRedirStart()
{
	g_bRecord = TRUE;
}

void YGFSMonRedirStop()
{
	g_bRecord = FALSE;
}

//----------------------------------------------------------------------
//
// YGFSMonConvertParsedPath
// 
// Converts a parsed unicode path to ANSI.
//
//----------------------------------------------------------------------
VOID
YGFSMonConvertParsedPath(
    int drive,
    path_t ppath,
    PCHAR fullpathname 
    )
{
    int  i = 0;
    unsigned int  result;

    if( drive != 0xFF ) {

        //
        // Its a volume-based path
        // 
        fullpathname[0] = drive+'A'-1;
        fullpathname[1] = ':';
        i = 2;
    }
    fullpathname[i] = 0;
    result = UniToBCSPath( &fullpathname[i], ppath->pp_elements, 
                  MAX_PATH-1, BCS_WANSI);
    fullpathname[ i + result] = 0;
}

HANDLE CreateHashList(ULONG ulHashTblSize,ULONG ulFlag)
{
	PHASH_HANDLE  pHashHead;
    pHashHead = (PHASH_HANDLE)_HeapAllocate(sizeof(HASH_HANDLE)+ulHashTblSize*sizeof(PHASHOBJ),ulFlag);
    if (pHashHead)
	{
		memset((UCHAR *)pHashHead,0,sizeof(HASH_HANDLE)+ulHashTblSize*sizeof(PHASHOBJ));
		pHashHead->ulHashSize = ulHashTblSize;
		pHashHead->ulFlag = ulFlag;
	    pHashHead->HashMutex  = Create_Semaphore(1);
	}
    return (HANDLE)pHashHead;
}

//----------------------------------------------------------------------
//
// YGFSMonHashCleanup
//
// Called when we are unloading to free any memory that we have 
// in our possession.
//
//----------------------------------------------------------------------
VOID 
YGFSMonHashCleanup(
    HANDLE hHashTable
    )
{
	ULONG i;
	PHASHOBJ pObject;
	if (!hHashTable) return;
	for (i = 0;i<((PHASH_HANDLE)hHashTable)->ulHashSize;i++)
	{
		while (((PHASH_HANDLE)hHashTable)->pList[i])
		{
			pObject = ((PHASH_HANDLE)hHashTable)->pList[i];
			if (((PFSHASHINFO)pObject->pData)->hFileInfo) _HeapFree(((PFSHASHINFO)pObject->pData)->hFileInfo,0);
			((PHASH_HANDLE)hHashTable)->pList[i] = pObject->pNext;
			_HeapFree(pObject,0);
		}
	}
	Destroy_Semaphore(((PHASH_HANDLE)hHashTable)->HashMutex);
	_HeapFree(hHashTable,0);
    return;
}

//----------------------------------------------------------------------
//
// YGFSMonLogHash
//
// Logs the key and associated fullpath in the hash table.
//
//----------------------------------------------------------------------
PHASHOBJ
YGFSMonLogHash( 
	HANDLE        hHashTable,
	ULONG         ulHashIndex,
	UCHAR         *pData,
    ULONG         ulDataLen
    )
{
	PHASHOBJ  pObject = NULL;
	if (hHashTable)
	{
		pObject = (PHASHOBJ)_HeapAllocate(sizeof(HASHOBJ)+ulDataLen,0);
		if (pObject)
		{
			pObject->ulHashIndex = ulHashIndex;
			pObject->ulDataSize = ulDataLen;
			_lmemcpy(pObject->pData,pData,ulDataLen);
			Wait_Semaphore(((PHASH_HANDLE)hHashTable)->HashMutex, BLOCK_SVC_INTS );
			pObject->pNext = ((PHASH_HANDLE)hHashTable)->pList[HASHOBJECT(pObject->ulHashIndex)];
			((PHASH_HANDLE)hHashTable)->pList[HASHOBJECT(pObject->ulHashIndex)] = pObject;
			Signal_Semaphore(((PHASH_HANDLE)hHashTable)->HashMutex);
		}
	}
	return pObject;
}

//----------------------------------------------------------------------
//
// YGFSMonGetLog
//
// Logs the key and associated fullpath in the hash table.
//
//----------------------------------------------------------------------
PHASHOBJ
YGFSMonGetLog( 
	HANDLE        hHashTable,
    ULONG         ulHashIndex
    )
{
	PHASHOBJ pObject;
	if (!hHashTable) return NULL;
    Wait_Semaphore(((PHASH_HANDLE)hHashTable)->HashMutex, BLOCK_SVC_INTS );
	pObject = ((PHASH_HANDLE)hHashTable)->pList[HASHOBJECT(ulHashIndex)];
	while (pObject && pObject->ulHashIndex != ulHashIndex) pObject = pObject->pNext;
    Signal_Semaphore(((PHASH_HANDLE)hHashTable)->HashMutex);
	return pObject;
}


//----------------------------------------------------------------------
//
// YGFSMonFreeHashEntry
//
// When we see a file close, we can free the string we had associated
// with the fileobject being closed since we know it won't be used
// again.
//
//----------------------------------------------------------------------
VOID 
YGFSMonFreeHashEntry( 
	HANDLE        hHashTable,
    ULONG         ulHashIndex
    )
{
	PHASHOBJ  hashEntry,prevEntry;
	if (!hHashTable) return;

    Wait_Semaphore(((PHASH_HANDLE)hHashTable)->HashMutex, BLOCK_SVC_INTS );

    //
    // Look-up the entry.
    //
    hashEntry = ((PHASH_HANDLE)hHashTable)->pList[HASHOBJECT(ulHashIndex)];
    prevEntry = NULL;

    while( hashEntry && 
           hashEntry->ulHashIndex != ulHashIndex) {

        prevEntry = hashEntry;
        hashEntry = hashEntry->pNext;
    }

    //  
    // If we fall of the hash list without finding what we're looking
    // for, just return.
    //
    if( hashEntry )
	{
		//
		// Got it! Remove it from the list
		//
		if( prevEntry ) {

			prevEntry->pNext = hashEntry->pNext;
		} else {

			((PHASH_HANDLE)hHashTable)->pList[HASHOBJECT(ulHashIndex)] = hashEntry->pNext;
		}

		if (((PFSHASHINFO)hashEntry->pData)->hFileInfo)
			_HeapFree(((PFSHASHINFO)hashEntry->pData)->hFileInfo,0);
		_HeapFree( hashEntry, 0 );
	}
    Signal_Semaphore( ((PHASH_HANDLE)hHashTable)->HashMutex );
}

VOID ClearDriveInfo(int nDrive,pioreq pir)
{
	if (FsVolTable[nDrive].pVolumeHandle && FsVolTable[nDrive].pVolumeHandle == pir->ir_rh)
	{
		if (FsVolTable[nDrive].pvolfuncs)
			_HeapFree(FsVolTable[nDrive].pvolfuncs,0);

		if (FsVolTable[nDrive].pOpenFunc)
			_HeapFree(FsVolTable[nDrive].pOpenFunc,0);

		if (FsVolTable[nDrive].pFindFunc)
			_HeapFree(FsVolTable[nDrive].pFindFunc,0);

		memset((UCHAR *)&FsVolTable[nDrive],0,sizeof(FSDVOLINFO));
	}
	return;
}

VOID StoreDriveInfo(int Drive,pioreq pir)
{
	if (!FsVolTable[Drive].pvolfuncs)
		FsVolTable[Drive].pvolfuncs = (volfunc	*)_HeapAllocate(sizeof(volfunc),0);
	if (FsVolTable[Drive].pvolfuncs)
	{
		FsVolTable[Drive].pVolumeHandle = pir->ir_rh;
		*FsVolTable[Drive].pvolfuncs = *pir->ir_vfunc;
	}
    return;
}

BOOL IsProtectDirAndFile(ParsedPath *pPath)
{
	char    szDir[MAX_PATH];
	PathElement *pNextEl,*pCurEl;
    WORD    wSize,wOrigLen;

	pCurEl = pPath->pp_elements;
	pNextEl = IFSNextElement(pCurEl);
	wOrigLen = pNextEl->pe_length;
	pNextEl->pe_length = 0;
	wSize = UniToBCS(szDir, pCurEl->pe_unichars,pCurEl->pe_length, MAX_PATH-1, BCS_WANSI);
	pNextEl->pe_length = wOrigLen;
	szDir[wSize] = 0;
	if (!stricmp(szDir,"_RESTORE") || !stricmp(szDir,"RECYCLED")) return TRUE;

	pCurEl = IFSLastElement(pPath);
	wSize = UniToBCS(szDir, pCurEl->pe_unichars,pCurEl->pe_length, MAX_PATH-1, BCS_WANSI);
	szDir[wSize] = 0;
	if (!stricmp(szDir,"User.dat") || 
		!stricmp(szDir,"System.dat") || 
		!stricmp(szDir,"Win386.swp") ||
		!stricmp(szDir,"classes.dat")) return TRUE;
	return FALSE;
}

BOOL  CreateDir(int nDrive,ParsedPath *pPath)
{
    int	    i,nResult;
	char    szDir[MAX_PATH];
	PathElement *pNextEl,*pCurEl;
    WORD    wFileAttributes,wSize,wOrigLen,wPos = 4;

	szDir[0] = nDrive+'A'-1;
	szDir[1] = ':';
	i = 2;
	pCurEl = pPath->pp_elements;
	while(wPos < pPath->pp_prefixLength)
	{
		nResult = 1;
		wPos += pCurEl->pe_length;
		szDir[i] = '\\';
		i++;
		pNextEl = IFSNextElement(pCurEl);
		wOrigLen = pNextEl->pe_length;
		pNextEl->pe_length = 0;
		wSize = UniToBCS(&szDir[i], pCurEl->pe_unichars,pCurEl->pe_length, MAX_PATH-1, BCS_WANSI);
		szDir[i+wSize] = 0;
		pNextEl->pe_length = wOrigLen;
		i += wSize;
		if (!R0_GetFileAttributes(szDir,&wFileAttributes))
		{
			if (FILE_ATTRIBUTE_DIRECTORY & wFileAttributes)
			{
				nResult = ERROR_ALREADY_EXISTS;
			}
		}
		else nResult = CreateDirectory(szDir);
		pCurEl = pNextEl;
	}
    if (nResult && nResult != ERROR_ALREADY_EXISTS) return FALSE;

    return TRUE;
}


//----------------------------------------------------------------------
//
// wstrlen
//
// Determine the length of a wide-character string.
//
//----------------------------------------------------------------------
int 
wstrlen( 
    unsigned short *unistring 
    )
{
    int i = 0;
    int len = 0;
  
    while( unistring[i++] != 0 ) len+=2;
    return len;
}


//----------------------------------------------------------------------
//
// FilmonGetProcess
//
// Retrieves the process name.
//
//----------------------------------------------------------------------
PCHAR
YGFSMonGetProcess( 
    PCHAR ProcessName 
    )
{
    PVOID       CurProc;
    PVOID       ring3proc;
    char        *name;
    ULONG       i;

    //
    // Get the ring0 process pointer.
    //
    CurProc = (PVOID)GetCurrentProcessHandle();
  
    _lstrcpyn( ProcessName, "???",_lstrlen("???")+1);
    //
    // Now, map the ring3 PCB 
    //
    ring3proc = (PVOID) SelectorMapFlat( Get_Sys_VM_Handle(), 
                                         (DWORD) (*(PDWORD) ((char *) CurProc + 0x38)) | 0x7, 0 );

    if( ring3proc != (PVOID) -1 ) 
	{
        //
        // copy out the process name (max 8 characters)
        //
        name = ((char *)ring3proc) + 0xF2;
        if( name[0] >= 'A' && name[0] <= 'z' ) {

            _lstrcpyn( ProcessName, name,PROCESSLEN );
            ProcessName[PROCESSLEN-1] = 0;
        }
    }
    return ProcessName;
}

//----------------------------------------------------------------------
//                 P A T H   P A R S I N G
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//
// YGFSMonConvertUnparsedPath
//
// Converts an unparsed unicode path to ANSI. This is only used for
// UNC paths except for the special cases of renames and findopens.
//
//----------------------------------------------------------------------
VOID
YGFSMonConvertUnparsedPath(
    string_t pUniStr,
    PCHAR fullpathname
    )
{
    unsigned int  result;

    result = UniToBCS( fullpathname, pUniStr, wstrlen( pUniStr ),
              MAX_PATH, BCS_WANSI);
    fullpathname[ result ] = 0;
}


//----------------------------------------------------------------------
//
// YGFSMonConvertMixedPath
//
// This converts a mix of unparsed and parsed paths to ANSI. The 
// unparsed path is used for server/share, whereas the parsed
// path is used for the directory/file. Only UNC rename and findopen 
// use this.
// 
//----------------------------------------------------------------------
VOID
YGFSMonConvertMixedPath(
    pioreq pir,
    path_t ppath,
    PCHAR fullpathname
    )
{
    int     i, slashes;
    unsigned int  result;

    result = UniToBCS( fullpathname, pir->ir_upath, wstrlen( pir->ir_upath ), MAX_PATH-1,
              BCS_WANSI);
    fullpathname[ result ] = 0;

    slashes = 0;
    for( i = 0; i < result; i++ ) {
            
        //
        // We find the 4th slash: \\Server\share\...
        //
        if( fullpathname[i] == '\\' && ++slashes == 4 ) break;
    }
    if( slashes == 4 ) {

        YGFSMonConvertParsedPath( 0xFF, ppath, &fullpathname[i]);
    }
}


//----------------------------------------------------------------------
//
// YGFSMonConvertPath
//
// Converts a unicode path name to ANSI.
//
//----------------------------------------------------------------------
PCHAR 
YGFSMonConvertPath( 
    CONVERT_TYPE converttype,
    int drive, 
    pioreq pir,
    PCHAR fullpathname 
    )
{
    if( drive != 0xFF ) {

        //
        // Its a volume-based path
        // 
        YGFSMonConvertParsedPath( drive, pir->ir_ppath, fullpathname );

    } else {
        
        //
        // Its a UNC path. The parsed path doesn't include the
        // server/share, so we get that from the unparsed path.
        //
        switch( converttype ) {
        case CONVERT_STANDARD:
            YGFSMonConvertUnparsedPath(pir->ir_upath, fullpathname );
            break;

        case CONVERT_FINDOPEN:
        case CONVERT_RENAME_SOURCE:
            YGFSMonConvertMixedPath( pir, pir->ir_ppath, fullpathname );
            break;

        case CONVERT_RENAME_TARGET:
            YGFSMonConvertMixedPath( pir, pir->ir_ppath2, fullpathname );
            break;
        }
    }
    return fullpathname;
}

//----------------------------------------------------------------------
//
// YGRegMonGetFullName
//
// Returns the full pathname of a key, if we can obtain one, else
// returns a handle.
//
//----------------------------------------------------------------------
BOOL YGRegMonGetFullName( VMMHKEY hKey, PCHAR lpszSubKey,PFSHASHINFO pRegInfo)
{
	PHASHOBJ            pObject;
    CHAR                tmpkey[16];

    //
    // See if we find the key in the hash table
    //
    pRegInfo->RegName[0] = 0;
	pObject = YGFSMonGetLog(hRegHashTable,(ULONG)hKey);

    if( pObject ) {

        _lmemcpy( pRegInfo, pObject->pData,pObject->ulDataSize );

    } else {

        //
        // Okay, make a name
        //
        switch( hKey ) {

        case HKEY_CLASSES_ROOT:
            _lstrcpyn(pRegInfo->RegName, "HKCR",_lstrlen("HKCR")+1);
            break;

        case HKEY_CURRENT_USER:
            _lstrcpyn(pRegInfo->RegName, "HKCU",_lstrlen("HKCU")+1);
            break;

        case HKEY_LOCAL_MACHINE:
            _lstrcpyn(pRegInfo->RegName, "HKLM",_lstrlen("HKLM")+1);
            break;

        case HKEY_USERS:
            _lstrcpyn(pRegInfo->RegName, "HKU",_lstrlen("HKU")+1);
            break;

        case HKEY_CURRENT_CONFIG:
            _lstrcpyn(pRegInfo->RegName, "HKCC",_lstrlen("HKCC")+1);
            break;

        case HKEY_DYN_DATA:
            _lstrcpyn(pRegInfo->RegName, "HKDD",_lstrlen("HKDD")+1);
            break;

        default:

            //
            // We will only get here if key was created before we loaded
            //
			return FALSE;
            _Sprintf( tmpkey, "0x%X", hKey );					
            _lstrcpyn(pRegInfo->RegName, tmpkey,_lstrlen(tmpkey)+1 );
            break;
        }
    }

    //
    // Append subkey and value, if they are there
    //
    if( lpszSubKey ) {

        if( lpszSubKey[0] ) {

            _lstrcpyn(&pRegInfo->RegName[_lstrlen(pRegInfo->RegName)], "\\" ,_lstrlen("\\")+1);
            _lstrcpyn(&pRegInfo->RegName[_lstrlen(pRegInfo->RegName)], lpszSubKey,MAX_PATH );
        }
    }
	return TRUE;
}

//----------------------------------------------------------------------
//
// YGFSMonGetFullPath
//
// Returns the full pathname of a file, if we can obtain one, else
// returns a handle.
//
//----------------------------------------------------------------------
PHASHOBJ
YGFSMonGetFullPath(  
    fh_t filenumber, 
	char *szFullName,
    int Drive, 
    int ResType, 
    int CodePage, 
    pioreq pir 
    )
{
	PHASHOBJ        pObject = NULL;
    pIFSFunc        enumFunc;
    ifsreq          ifsr;
	PFSHASHINFO     pFileInfo;
    int             retval;

    //
    // See if we find the key in the hash table.
    //
	szFullName[0] = 0;
	if (FsVolTable[Drive].pVolumeHandle)
	{
		pObject = YGFSMonGetLog(hFSHashTable,(ULONG)filenumber);

		if( pObject ) {
	        YGFSMonConvertParsedPath(Drive,(ParsedPath *)&((PFSHASHINFO)pObject->pData)->FileName, szFullName);
		} 
		else {

			//
			// File name isn't in the table, so ask the
			// underlying file system
			//
			_Sprintf( szFullName, "0x%X", filenumber );

			pFileInfo = (PFSHASHINFO)IFSMgr_GetHeap(sizeof(FSHASHINFO) + MAX_PATH * sizeof(USHORT) + sizeof( path_t));
			if( pFileInfo ) {
            
				//
				// Send a query file name request
				//
	            _lmemcpy( &ifsr, pir, sizeof( ifsreq ));
				ifsr.ifsir.ir_flags = ENUMH_GETFILENAME;
				ifsr.ifsir.ir_ppath = (ParsedPath *)pFileInfo->FileName;
				enumFunc = ifsr.ifs_hndl->hf_misc->hm_func[HM_ENUMHANDLE];

				retval = (*PrevIFSHookProc)(enumFunc, IFSFN_ENUMHANDLE, 
											Drive, ResType, CodePage, 
											&ifsr.ifsir);

				if( retval == ERROR_SUCCESS ) {
                
					YGFSMonConvertParsedPath( Drive,(ParsedPath *)&pFileInfo->FileName, szFullName );
					pObject = YGFSMonLogHash(hFSHashTable,(ULONG)filenumber,(UCHAR *)pFileInfo,sizeof(FSHASHINFO)+pFileInfo->FileName[0]+2);
				}
				IFSMgr_RetHeap( (void *)pFileInfo);
			} 
		}
	}
    return pObject;
}


//----------------------------------------------------------------------
//                     H O O K   R O U T I N E
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// REGISTRY HOOKS
//
// All these hooks do essentially the same thing: dump API-specific
// info into the data buffer, call the original handler, and then
// dump any return information.
//----------------------------------------------------------------------
LONG YGRegMonVMMRegOpenKey( VMMHKEY hKey, PCHAR lpszSubKey, PVMMHKEY phkResult) {
    LONG     retval;
	UCHAR    RegInfo[sizeof(FSHASHINFO)+MAX_PATH] = {0};
    CHAR     process[PROCESSLEN];

//    DBGPRINT("RegOpenKey: <=\n");
	YGFSMonGetProcess(process);
    retval = OldVMMRegOpenKey( hKey, lpszSubKey, phkResult );
    if( retval == ERROR_SUCCESS && YGRegMonGetFullName( hKey, lpszSubKey,(PFSHASHINFO)RegInfo ))
	{
//	    DBGPRINT("%s RegOpenKey: => %s\n", process,((PFSHASHINFO)RegInfo)->RegName);
		YGFSMonFreeHashEntry(hRegHashTable,*phkResult);
		YGFSMonLogHash(hRegHashTable,*phkResult,RegInfo,sizeof(FSHASHINFO)+_lstrlen(((PFSHASHINFO)RegInfo)->RegName)+1);
    }  
//    DBGPRINT("RegOpenKey: => %x\n",retval);
    return retval;
}

LONG YGRegMonVMMRegCreateKey( VMMHKEY hKey, PCHAR lpszSubKey, PVMMHKEY phkResult) {
    LONG      retval;
	UCHAR    RegInfo[sizeof(FSHASHINFO)+MAX_PATH] = {0};
    CHAR      process[PROCESSLEN];

//    DBGPRINT("RegCreatKey: <=\n");
	YGFSMonGetProcess(process);
    retval = OldVMMRegCreateKey( hKey, lpszSubKey, phkResult );
    if( retval == ERROR_SUCCESS && YGRegMonGetFullName( hKey, lpszSubKey,(PFSHASHINFO)RegInfo)) 
	{
	    DBGPRINT("%s RegCreatKey: => %s\n", process,((PFSHASHINFO)RegInfo)->RegName);
		YGFSMonFreeHashEntry(hRegHashTable,*phkResult);
		YGFSMonLogHash(hRegHashTable,*phkResult,RegInfo,sizeof(FSHASHINFO)+_lstrlen(((PFSHASHINFO)RegInfo)->RegName)+1);
		if (g_bRecord) YGFSMonSaveRegKey('D',((PFSHASHINFO)RegInfo)->RegName);
    }
//    DBGPRINT("RegCreatKey: => %x\n",retval);
    return retval;
}

LONG YGRegMonVMMRegDeleteKey( VMMHKEY hKey, PCHAR lpszSubKey ) {
    LONG      retval;
	UCHAR    RegInfo[sizeof(FSHASHINFO)+MAX_PATH] = {0};
    CHAR      process[PROCESSLEN];

//	DBGPRINT("RegDeleteKey: <=\n");
	YGFSMonGetProcess(process);
    retval = OldVMMRegDeleteKey( hKey, lpszSubKey );
    if( retval == ERROR_SUCCESS && YGRegMonGetFullName( hKey, lpszSubKey,(PFSHASHINFO)RegInfo)) 
	{
		DBGPRINT("%s RegDeleteKey: => %s\n",process, ((PFSHASHINFO)RegInfo)->RegName);
		if (g_bRecord) YGFSMonDeleteRegKey('D',((PFSHASHINFO)RegInfo)->RegName);
	}
//	DBGPRINT("RegDeleteKey: => %x\n",retval);
    return retval;
}

LONG YGRegMonVMMRegDeleteValue( VMMHKEY hKey, PCHAR lpszSubKey ) {
    LONG      retval;
	UCHAR    RegInfo[sizeof(FSHASHINFO)+MAX_PATH] = {0};
    CHAR      process[PROCESSLEN];

//    DBGPRINT("RegDeleteValue: <=\n");
	YGFSMonGetProcess(process);
    retval = OldVMMRegDeleteValue( hKey, lpszSubKey );

    if( retval == ERROR_SUCCESS && YGRegMonGetFullName( hKey, lpszSubKey,(PFSHASHINFO)RegInfo)) 
	{
	    DBGPRINT("%s RegDeleteValue: => %s\n", process,((PFSHASHINFO)RegInfo)->RegName);
		if (g_bRecord) YGFSMonDeleteRegData('D',((PFSHASHINFO)RegInfo)->RegName,lpszSubKey);
	}
//    DBGPRINT("RegDeleteValue: => %x\n",retval);
    return retval;
}

LONG YGRegMonVMMRegCloseKey(VMMHKEY hKey) {
    LONG      retval;

//    DBGPRINT("RegCloseKey: <=\n");
    retval = OldVMMRegCloseKey( hKey );
	YGFSMonFreeHashEntry(hRegHashTable,hKey);

//    DBGPRINT("RegCloseKey: => %x\n",retval);

    return retval;
}

LONG YGRegMonVMMRegFlushKey( VMMHKEY hKey ) {
    LONG      retval;

//    DBGPRINT("RegFlushKey: <=\n");
    retval = OldVMMRegFlushKey( hKey );
//    DBGPRINT("RegFlushKey: => %x\n",retval);
    return retval;
}

LONG YGRegMonVMMRegEnumKey(VMMHKEY hKey, DWORD iSubKey, PCHAR lpszName, DWORD cchName) {
    LONG      retval;
  
//    DBGPRINT("RegEnumKey: <=\n");
    retval = OldVMMRegEnumKey( hKey, iSubKey, lpszName, cchName );
//    DBGPRINT("RegEnumKey: => %x\n",retval);

    return retval;
}

LONG YGRegMonVMMRegEnumValue(VMMHKEY hKey, DWORD iValue, PCHAR lpszValue, 
                      PDWORD lpcchValue, PDWORD lpdwReserved, 
                      PDWORD lpdwType, PBYTE lpbData, PDWORD lpcbData) {
    LONG      retval;

//    DBGPRINT("RegEnumValue: <=\n");
    retval = OldVMMRegEnumValue( hKey, iValue, lpszValue, lpcchValue,
                               lpdwReserved, lpdwType, lpbData, lpcbData );
//    DBGPRINT("RegEnumValue: => %x\n",retval);
    return retval;
}

LONG YGRegMonVMMRegQueryInfoKey( VMMHKEY hKey, PCHAR lpszClass, PDWORD lpcchClass,  
                          DWORD lpdwReserved, PDWORD lpcSubKeys, 
                          PDWORD lpcchMaxSubKey, PDWORD  pcchMaxClass, 
                          PDWORD lpcValues, PDWORD lpcchMaxValueName, 
                          PDWORD lpcbMaxValueData, 
                          PDWORD lpcbSecurityDescriptor, 
                          _FILETIME *lpftLastWriteTime) {
    LONG      retval;

//    DBGPRINT("RegQueryInfoKey: <=\n");
    retval = OldVMMRegQueryInfoKey( hKey, lpszClass, lpcchClass, lpdwReserved, 
                                  lpcSubKeys, lpcchMaxSubKey, pcchMaxClass,
                                  lpcValues, lpcchMaxValueName,
                                  lpcbMaxValueData,
                                  lpcbSecurityDescriptor,
                                  lpftLastWriteTime );
//    DBGPRINT("RegQueryInfoKey: => %x\n",retval);
    return retval;
}

LONG YGRegMonVMMRegQueryValue( VMMHKEY hKey, PCHAR lpszSubKey, PCHAR lpszValue, 
                        PLONG pcbValue ) {
    LONG      retval;

//    DBGPRINT("RegQueryValue: <=\n");
    retval = OldVMMRegQueryValue( hKey, lpszSubKey, lpszValue, pcbValue );
//    DBGPRINT("RegQueryValue: => %x\n",retval);
    return retval;
}

LONG YGRegMonVMMRegQueryValueEx( VMMHKEY hKey, PCHAR lpszValueName, 
                          PDWORD lpdwReserved, PDWORD lpdwType, 
                          PBYTE lpbData, PDWORD lpcbData ) {
    LONG      retval;
  
//    DBGPRINT("RegQueryValueEx: <=\n");
    retval = OldVMMRegQueryValueEx( hKey, lpszValueName, lpdwReserved, 
                                  lpdwType, lpbData, lpcbData );
//    DBGPRINT("RegQueryValueEx: => %x\n",retval);

    return retval;
}

LONG YGRegMonVMMRegSetValue( VMMHKEY hKey, PCHAR lpszSubKey, DWORD fdwType, 
                      PCHAR lpszData, DWORD cbData ) {
    LONG      retval;
	UCHAR    RegInfo[sizeof(FSHASHINFO)+MAX_PATH] = {0};
    CHAR      process[PROCESSLEN];
  
//    DBGPRINT("RegSetValue: <=\n");
	YGFSMonGetProcess(process);
    retval = OldVMMRegSetValue( hKey, lpszSubKey, fdwType, lpszData, cbData );
    if( retval == ERROR_SUCCESS && YGRegMonGetFullName( hKey, lpszSubKey,(PFSHASHINFO)RegInfo)) 
	{
	    DBGPRINT("%s RegSetValue: => %s\n", process,((PFSHASHINFO)RegInfo)->RegName);
		if (g_bRecord) YGFSMonSaveRegData('D',((PFSHASHINFO)RegInfo)->RegName, "(Default)",fdwType, lpszData, cbData);
	}
//    DBGPRINT("RegSetValue: => %x\n",retval);
    return retval;
}

LONG YGRegMonVMMRegSetValueEx( VMMHKEY hKey, PCHAR lpszValueName, 
                        DWORD lpdwReserved, DWORD fdwType, 
                        PBYTE lpbData, DWORD lpcbData ) {
    LONG      retval;
	UCHAR     RegInfo[sizeof(FSHASHINFO)+MAX_PATH] = {0};
    CHAR      process[PROCESSLEN];
  
//    DBGPRINT("RegSetValueEx: <=\n");
	YGFSMonGetProcess(process);
    retval = OldVMMRegSetValueEx( hKey, lpszValueName, lpdwReserved, 
                                fdwType, lpbData, lpcbData );
    if( retval == ERROR_SUCCESS && YGRegMonGetFullName( hKey, NULL,(PFSHASHINFO)RegInfo)) 
	{
	    DBGPRINT("%s RegSetValueEx: => %s\n", process,((PFSHASHINFO)RegInfo)->RegName);
		if (g_bRecord) YGFSMonSaveRegData('D',((PFSHASHINFO)RegInfo)->RegName,lpszValueName,fdwType, lpbData, lpcbData);
	}
//    DBGPRINT("RegSetValueEx: => %x\n",retval);
    return retval;
}

LONG YGRegMonVMMRegRemapPreDefKey( VMMHKEY hKey, VMMHKEY hRootKey ) {
    LONG      retval;
	UCHAR    RegInfo[sizeof(FSHASHINFO)+MAX_PATH] = {0};
    CHAR      process[PROCESSLEN];

//    DBGPRINT("RegRemapPreDefKey: <=\n");
    YGRegMonGetFullName( hKey, NULL,(PFSHASHINFO)RegInfo);
	YGFSMonGetProcess(process);
    retval = OldVMMRegRemapPreDefKey( hKey, hRootKey );
    if( retval == ERROR_SUCCESS ) 
	    DBGPRINT("%s RegRemapPreDefKey: => %s\n", process,((PFSHASHINFO)RegInfo)->RegName);
//    DBGPRINT("RegRemapPreDefKey: => %x\n",retval);

    return retval;
}

LONG YGRegMonVMMRegQueryMultipleValues( VMMHKEY hKey, PVALENT pValent, 
                                 DWORD dwNumVals, PCHAR pValueBuf, 
                                 PDWORD pTotSize ) {
    LONG      retval;
	UCHAR    RegInfo[sizeof(FSHASHINFO)+MAX_PATH] = {0};
    CHAR      process[PROCESSLEN];

//	DBGPRINT("QueryMultipleValues: <=\n");
    YGRegMonGetFullName( hKey, NULL, (PFSHASHINFO)RegInfo);
	YGFSMonGetProcess(process);
    retval = OldVMMRegQueryMultipleValues( hKey, pValent,
                                         dwNumVals, pValueBuf, pTotSize );

    if( retval == ERROR_SUCCESS ) 
		DBGPRINT("%s QueryMultipleValues: => %s\n", process,((PFSHASHINFO)RegInfo)->RegName);
//	DBGPRINT("QueryMultipleValues: => %x\n",retval);
    return retval;
}

LONG YGRegMonVMMRegCreateDynKey( PCHAR szName, PVOID KeyContext, 
                          PVOID pInfo, PVOID pValList, 
                          DWORD dwNumVals, PVMMHKEY pKeyHandle ) {
    LONG      retval;
	UCHAR    RegInfo[sizeof(FSHASHINFO)+MAX_PATH] = {0};
    CHAR      process[PROCESSLEN];

//    DBGPRINT("RegCreateDynKey: <=\n");
    _Sprintf(((PFSHASHINFO)RegInfo)->RegName, "DYNDAT\\%s", szName );
	YGFSMonGetProcess(process);
    retval = OldVMMRegCreateDynKey( szName, KeyContext, pInfo, pValList,
                                  dwNumVals, pKeyHandle );

    if( retval == ERROR_SUCCESS ) {

	    DBGPRINT("%s RegCreateDynKey: => %s, %x\n",process,((PFSHASHINFO)RegInfo)->RegName,*pKeyHandle);
		YGFSMonLogHash(hRegHashTable,*pKeyHandle,RegInfo,sizeof(FSHASHINFO)+_lstrlen(((PFSHASHINFO)RegInfo)->RegName)+1);
		if (g_bRecord) YGFSMonSaveRegKey('D',((PFSHASHINFO)RegInfo)->RegName);
    }
//    DBGPRINT("RegCreateDynKey: => %x\n",retval);
    return retval;
}

LONG __stdcall YGRegMonVMMWin32RegQueryInfoKey(CLIENT_STRUCT *pClientRegs, DWORD Dummy2, 
                                         VMMHKEY hKey, 
                                         PDWORD lpcSubKeys, 
                                         PDWORD lpcchMaxSubKey, 
                                         PDWORD lpcValues, PDWORD lpcchMaxValueName, 
                                         PDWORD lpcbMaxValueData )
{
    LONG      retval;
	UCHAR    RegInfo[sizeof(FSHASHINFO)+MAX_PATH] = {0};
    CHAR      process[PROCESSLEN];

    //
    // NOTE: this special hook is needed because Win95 has a bug where
    // the Win32 call to RegQueryInfoKey gets routed to VMM's Win32 
    // service table, but the service table handler does *not* call the
    // VMM RegQueryInfoKey entry point. Therefore, we have to hook the
    // VMM Win32 service as a special case.
    //
	YGFSMonGetProcess(process);
    YGRegMonGetFullName( hKey, NULL,(PFSHASHINFO)RegInfo);

    //
    // The return code is Logd in the client registers
    //
    retval = OldVMMWin32RegQueryInfoKey( pClientRegs, Dummy2, 
                              hKey, lpcSubKeys, lpcchMaxSubKey, 
                              lpcValues, lpcchMaxValueName,
                              lpcbMaxValueData );
    if( retval == ERROR_SUCCESS ) 
	{
	    DBGPRINT("%s Win32RegQueryInfoKey: => %s\n",process,((PFSHASHINFO)RegInfo)->RegName);
	    retval = pClientRegs->CRS.Client_EAX;
	}

    return retval;
}

BOOL YGFSMonRestoreFile(pioreq pir,int nDrive,char *szFileName,PFSHASHINFO pHashInfo,int ResType,int CodePage)
{
    pIFSFunc   FuncRead;
	UCHAR      *pData;
	ioreq      ifsreq;
	WORD       wAttr;
	DWORD      dwAction,dwPos,hFile;
	int        retval;

	if (!g_bRecord || !FsVolTable[4].pVolumeHandle) return FALSE;
	szFileName[0] = 'D';
//	if (!R0_GetFileAttributes(szFileName,&wAttr)) return TRUE;
	pHashInfo->bProtect = IsProtectDirAndFile((ParsedPath *)(pHashInfo->FileName));
	if (pHashInfo->bProtect) return FALSE;

	if (CreateDir(4,(ParsedPath *)(pHashInfo->FileName)))
	{
		if (!R0_OpenCreateFile(FALSE, ACCESS_READWRITE | SHARE_COMPATIBILITY,
							   0,ACTION_OPENALWAYS, 0, szFileName, &hFile,&dwAction))
		{
			pData = IFSMgr_GetHeap(64*1024);
			if(pData) 
			{
				ifsreq = *pir;
				ifsreq.ir_data = pData;
				ifsreq.ir_length = 64*1024;
				ifsreq.ir_pos = 0;
				dwPos = 0;
				ifsreq.ir_options = R0_NO_CACHE;
				FuncRead = FsVolTable[nDrive].pOpenFunc->FuncRead;

				do 
				{
					retval = (*PrevIFSHookProc)(FuncRead, IFSFN_READ, nDrive, ResType, CodePage,&ifsreq);
					R0_WriteFile(FALSE,hFile,ifsreq.ir_length,dwPos,pData,&dwAction);
					dwPos = ifsreq.ir_pos;
				}while (!retval && ifsreq.ir_length);

				IFSMgr_RetHeap( (void *)pData);
			}
			R0_CloseFile(hFile);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL YGFSMonParseWrite(pioreq pir,int nDrive,char *szFileName,PFSHASHINFO pFileInfo,int ResType,int CodePage)
{
	WORD       wStart;
	ioreq      ifsreq;
	path_t     uniFullName;
	hndlfunc   hFunc;

	if (!g_bRecord || !FsVolTable[nDrive].pVolumeHandle) return FALSE;
	uniFullName = IFSMgr_GetHeap( MAX_PATH * sizeof(USHORT) + sizeof( path_t));
	if( uniFullName ) 
	{
		hFunc.hf_misc = (hndlmisc *)_HeapAllocate(sizeof(hndlmisc),0);
		if( !hFunc.hf_misc )
		{
			IFSMgr_RetHeap( (void *) uniFullName );
			return FALSE;
		}
	}

	ifsreq = *pir;
	ifsreq.ir_flags = ACCESS_READWRITE;
	ifsreq.ir_options = ACTION_OPENALWAYS;
	ifsreq.ir_hfunc = &hFunc;
	ifsreq.ir_attr = 0;
	ifsreq.ir_ppath = (ParsedPath *)&pFileInfo->FileName;
	ifsreq.ir_upath = (WORD *)uniFullName;
	_lmemcpy((BYTE *)ifsreq.ir_upath,(BYTE *)&pFileInfo->FileName,((WORD *)&pFileInfo->FileName)[0] + 2);
	ifsreq.ir_upath[1]	= ':';
	ifsreq.ir_upath[0]	= 'D';
	ifsreq.ir_data = NULL;
	ifsreq.ir_pos = 0;
	ifsreq.ir_fh = NULL;
	wStart = 4;
	while (wStart < ((WORD *)&pFileInfo->FileName)[0])
	{
		ifsreq.ir_upath[wStart/2] = '\\';
		wStart += ((WORD *)&pFileInfo->FileName)[wStart/2];
	}
	ifsreq.ir_uFName =	&((WORD *)ifsreq.ir_upath)[((WORD *)&pFileInfo->FileName)[1]/2+1];

	YGFSMonParseNameBasedFunc(IFSFN_OPEN,szFileName,&ifsreq,ResType,CodePage,&pFileInfo->hFileInfo);

	_HeapFree(hFunc.hf_misc,0);
	IFSMgr_RetHeap( (void *) uniFullName );

	return TRUE;
}

int	YGFSMonParseNameBasedFunc(int fn,char *szFile,pioreq pir,int ResType,int CodePage,PVOID *hOpenHandle)
{
	int nResult = 1;
	WORD       wAttr;
    pIFSFunc   FuncName;

	if (!g_bRecord || !FsVolTable[4].pVolumeHandle) return nResult;
	if ((DWORD)pir->ir_ppath == 0xFFFFFBBB || IFSIsRoot(pir->ir_ppath)) return nResult;

	szFile[0] = 'D';
	if (!R0_GetFileAttributes(szFile,&wAttr))
	{
		FuncName = FsVolTable[4].pvolfuncs->vfn_func[fn-31];
		pir->ir_rh = FsVolTable[4].pVolumeHandle;
		if (pir->ir_upath[0] != 0x2e && pir->ir_upath != pir->ir_uFName) pir->ir_upath[0] = 'D';
		nResult = (*PrevIFSHookProc)(FuncName, fn, 
									4, ResType, CodePage, 
									pir);
		if (!nResult)
		{
			if (fn == IFSFN_OPEN)
			{
				*hOpenHandle = _HeapAllocate(sizeof(FILEHANDLESTRU),0);
				if (*hOpenHandle)
				{
					((PFILEHANDLESTRU)*hOpenHandle)->hHandle = pir->ir_fh;
					((PFILEHANDLESTRU)*hOpenHandle)->nDrive = 4;
				}
				if (!FsVolTable[4].pOpenFunc)
					FsVolTable[4].pOpenFunc = (POPENFUNCSTRU)_HeapAllocate(sizeof(OPENFUNCSTRU),0);
				if (FsVolTable[4].pOpenFunc)
				{
					FsVolTable[4].pOpenFunc->FuncRead = pir->ir_hfunc->hf_read;
					FsVolTable[4].pOpenFunc->FuncWrite = pir->ir_hfunc->hf_write;
					FsVolTable[4].pOpenFunc->hndFunc = *pir->ir_hfunc->hf_misc;
				}
			}
		}
	}
	return nResult;
}

int	YGFSMonMapHandleBasedFunc(HANDLE hOpenHandle,int fn,pioreq pir,int ResType,int CodePage)
{
	int nResult = 1;
    pIFSFunc   FuncHandle;

	if (!g_bRecord || !hOpenHandle || !FsVolTable[4].pVolumeHandle) return nResult;

	pir->ir_rh = FsVolTable[((PFILEHANDLESTRU)hOpenHandle)->nDrive].pVolumeHandle;
	pir->ir_fh = ((PFILEHANDLESTRU)hOpenHandle)->hHandle;
	if (fn == IFSFN_READ) FuncHandle = FsVolTable[((PFILEHANDLESTRU)hOpenHandle)->nDrive].pOpenFunc->FuncRead;
	else
	{
	     if (fn == IFSFN_WRITE) FuncHandle = FsVolTable[((PFILEHANDLESTRU)hOpenHandle)->nDrive].pOpenFunc->FuncWrite;
	     else FuncHandle = FsVolTable[((PFILEHANDLESTRU)hOpenHandle)->nDrive].pOpenFunc->hndFunc.hm_func[fn-10];
	}

	nResult = (*PrevIFSHookProc)(FuncHandle, fn, 
								4, ResType, CodePage, 
								pir);
	return nResult;
}

//----------------------------------------------------------------------
//
// YGFSMonHookProc
//
// All (most) IFS functions come through this routine for us to look
// at.
//
//----------------------------------------------------------------------
#pragma optimize("", off)
int 
_cdecl 
YGFSMonHookProc(
    pIFSFunc pfn, 
    int fn, 
    int Drive, 
    int ResType,
    int CodePage,
    pioreq pir
    )
{
    int                retval = 1;
	UCHAR              FileInfo[sizeof(FSHASHINFO)+MAX_PATH*sizeof(WORD)] = {0};
	PHASHOBJ           pObject = NULL;
    char               processname[64];
    char               szFileName[MAX_PATH];
    char               drivestring[4];
    ioreq              origir;
    struct srch_entry  *search;
    unsigned int  result;
    int                i, j;

    // 
    // Inititlize default data.
    //
	szFileName[0] = 0;
    //
    // Save original iorequest because some entries get modified.
    //
    origir = *pir;

    //
    // Get the current process name.
    //
    YGFSMonGetProcess( processname );

    retval = (*PrevIFSHookProc)(pfn, fn, Drive, ResType, CodePage, pir);

	if (Drive != 3 && fn != IFSFN_DISCONNECT && fn != IFSFN_CONNECT && fn != IFSFN_FLUSH)  return retval;

    if (fn < 2 || (fn >= 10 && fn <= 17)) //for function with handle of open
        pObject = YGFSMonGetFullPath( pir->ir_fh, szFileName, Drive, ResType, CodePage, pir );
    //
    // Now extract parameters based on the function type.
    //
    switch( fn ) 
	{
    case IFSFN_DISCONNECT:
        DBGPRINT( "%s\tDisConnect\t%c", processname,Drive+'A'-1);
		if (!retval && !pir->ir_error)	 ClearDriveInfo(Drive,pir);
		break;
    case IFSFN_CONNECT: 		       //deal with mount
        DBGPRINT( "%s\tConnect\t%c", processname,Drive+'A'-1);
		if (!retval && !pir->ir_error)
		{
			switch (pir->ir_flags)
			{
			case IR_FSD_MOUNT:
			case IR_FSD_MOUNT_CHILD:
				 StoreDriveInfo(Drive,pir);
				 break;
			case IR_FSD_MAP_DRIVE:
			case IR_FSD_UNMAP_DRIVE:
				 break;
			case IR_FSD_UNLOAD:
			case 1:
//			case IR_FSD_DISMOUNT:
				 ClearDriveInfo(Drive,pir);
				 break;
			}
		}
		break;
    case IFSFN_OPEN:
        YGFSMonConvertPath( CONVERT_STANDARD, Drive, pir, szFileName );
        DBGPRINT( "%s\tOpen\t%s", processname,szFileName);
        if( retval == ERROR_SUCCESS ) 
		{
			if (!FsVolTable[Drive].pOpenFunc)
				FsVolTable[Drive].pOpenFunc = (POPENFUNCSTRU)_HeapAllocate(sizeof(OPENFUNCSTRU),0);
			if (FsVolTable[Drive].pOpenFunc)
			{
				FsVolTable[Drive].pOpenFunc->FuncRead = pir->ir_hfunc->hf_read;
				FsVolTable[Drive].pOpenFunc->FuncWrite = pir->ir_hfunc->hf_write;
				FsVolTable[Drive].pOpenFunc->hndFunc = *pir->ir_hfunc->hf_misc;
			}
			_lmemcpy((BYTE *)&((PFSHASHINFO)FileInfo)->FileName,(BYTE *)pir->ir_ppath,((WORD *)pir->ir_ppath)[0]+2);
			YGFSMonParseNameBasedFunc(fn,szFileName,&origir,ResType, CodePage,&((PFSHASHINFO)FileInfo)->hFileInfo);
			YGFSMonLogHash(hFSHashTable,(ULONG)pir->ir_fh,FileInfo,sizeof(FSHASHINFO)+((PFSHASHINFO)FileInfo)->FileName[0]+2);
		}
        break;

    case IFSFN_WRITE:
		if (!((PFSHASHINFO)pObject->pData)->hFileInfo && !((PFSHASHINFO)pObject->pData)->bProtect)
		{
			if (YGFSMonRestoreFile(&origir,Drive,szFileName,(PFSHASHINFO)&pObject->pData,ResType,CodePage))
				YGFSMonParseWrite(&origir,4,szFileName,(PFSHASHINFO)&pObject->pData,ResType,CodePage);
		}
		YGFSMonMapHandleBasedFunc(((PFSHASHINFO)pObject->pData)->hFileInfo,fn,&origir,ResType,CodePage);

    case IFSFN_READ:
        DBGPRINT( "%s\t%s\t%s", 
                   processname, fn == IFSFN_READ? "Read" : "Write", szFileName);
        break;

    case IFSFN_CLOSE:
		YGFSMonMapHandleBasedFunc(((PFSHASHINFO)pObject->pData)->hFileInfo,fn,&origir,ResType,CodePage);
        DBGPRINT( "%s\tClose\t%s", processname,szFileName);
        if( origir.ir_flags == CLOSE_FINAL )
			YGFSMonFreeHashEntry(hFSHashTable,(ULONG)origir.ir_fh);
        break;

    case IFSFN_DIR:
        YGFSMonConvertPath( CONVERT_STANDARD, Drive, pir, szFileName);
        if( retval == ERROR_SUCCESS ) 
		{
	        DBGPRINT( "%s\tDirectory\t%s", processname, szFileName);
			YGFSMonParseNameBasedFunc(fn,szFileName,&origir,ResType, CodePage,NULL);
		}
        break;

    case IFSFN_SEEK:
		YGFSMonMapHandleBasedFunc(((PFSHASHINFO)pObject->pData)->hFileInfo,fn,&origir,ResType,CodePage);
        DBGPRINT( "%s\tSeek\t%s", processname, szFileName);
        break;

    case IFSFN_COMMIT:
		YGFSMonMapHandleBasedFunc(((PFSHASHINFO)pObject->pData)->hFileInfo,fn,&origir,ResType,CodePage);
        DBGPRINT( "%s\tCommit\t%s", processname, szFileName);
        break;

    case IFSFN_FILELOCKS:
		YGFSMonMapHandleBasedFunc(((PFSHASHINFO)pObject->pData)->hFileInfo,fn,&origir,ResType,CodePage);
        DBGPRINT( "%s\t%s\t%s", 
                   processname, origir.ir_flags == LOCK_REGION ? "Lock" : "Unlock",
                   szFileName);
        break;

    case IFSFN_FINDOPEN:

        break;

    case IFSFN_FINDNEXT:
        break;

    case IFSFN_FINDCLOSE:
        break;

    case IFSFN_FILEATTRIB:
        YGFSMonConvertPath( CONVERT_STANDARD, Drive, pir, szFileName);
        if( retval == ERROR_SUCCESS ) 
		{
	        DBGPRINT( "%s\tAttributes\t%s", processname, szFileName);
			YGFSMonParseNameBasedFunc(fn,szFileName,&origir,ResType, CodePage,NULL);
		}
        break;

    case IFSFN_FILETIMES:
		YGFSMonMapHandleBasedFunc(((PFSHASHINFO)pObject->pData)->hFileInfo,fn,&origir,ResType,CodePage);
        DBGPRINT( "%s\tAttributes\t%s", processname, szFileName);
        break;

    case IFSFN_FLUSH:
		if (!retval && !pir->ir_error && pir->ir_options == VOL_REMOUNT)
			StoreDriveInfo(Drive,pir);
        DBGPRINT( "%s\tFlushVolume",processname);
        break;

    case IFSFN_DELETE:
        YGFSMonConvertPath( CONVERT_STANDARD, Drive, pir,szFileName);
        YGFSMonFreeHashEntry(hFSHashTable,(ULONG)pir->ir_fh );
		if (!retval)
		{
			YGFSMonParseNameBasedFunc(fn,szFileName,&origir,ResType, CodePage,NULL);
	        DBGPRINT( "%s\tDelete\t%s",processname, szFileName);
		}
        break;

    case IFSFN_SEARCH:
        if( pir->ir_flags == SEARCH_FIRST ) 
            YGFSMonConvertPath( CONVERT_STANDARD, Drive, pir, szFileName);
        else
            _Sprintf(szFileName, "SearchNext" );
        DBGPRINT( "%s\tSearch\t%s", processname, szFileName);    
        break;
    
    case IFSFN_GETDISKINFO:
        DBGPRINT( "%s\tGetDiskInfo\t%d",processname, Drive);
        break;

    case IFSFN_RENAME:
        YGFSMonConvertPath( CONVERT_RENAME_SOURCE, Drive, pir,szFileName);
		if (!retval)
		{
			YGFSMonParseNameBasedFunc(fn,szFileName,&origir,ResType, CodePage,NULL);
	        DBGPRINT( "%s\tRename\t%s",processname,szFileName);		 
		}
        break;
    case IFSFN_IOCTL16DRIVE:
        DBGPRINT( "%s\tIoctl\t%d",processname, Drive);
        break;
	case IFSFN_PIPEREQUEST:
		break;
	case IFSFN_HANDLEINFO:
		break;
	case IFSFN_ENUMHANDLE:
        DBGPRINT( "%s\tENUMHANDLE\t%s", processname, szFileName);
		break;
	default:
        DBGPRINT( "%s\t%d", processname,fn);
		break;
    }
    DBGPRINT("==>%d\n", retval );
    return retval;
}
#pragma optimize("", on)

BOOL  OnCriticalInit()
{
    PVMMDDB vmmDDB;
    int i;
    //
    // Initialize the locks.
    //
	memset((UCHAR *)&FsVolTable[0],0,sizeof(FSDVOLINFO)*MAXDRIVELETTER);
	hRegHashTable = CreateHashList(NUMHASH,0);
	hFSHashTable = CreateHashList(NUMHASH,0);
	HookRegServices();
    vmmDDB = Get_DDB( VMM_DEVICE_ID, "" );
    VMMWin32ServiceTable = (PDWORD) vmmDDB->DDB_Win32_Service_Table;
	if (VMMWin32ServiceTable)
	{
		OldVMMWin32RegQueryInfoKey = (PVOID) VMMWin32ServiceTable[ VMMWIN32QUERYINFOKEY ];
		if (OldVMMWin32RegQueryInfoKey)
			VMMWin32ServiceTable[ VMMWIN32QUERYINFOKEY ] = (DWORD) YGRegMonVMMWin32RegQueryInfoKey;
	}
	return TRUE;
}

BOOL  OnCriticalExit()
{
	int  i;
    //
    // Free all memory.
    //
	for (i = 0;i < 26;i++)	ClearDriveInfo(i,FsVolTable[i].pVolumeHandle);
	UnHookRegServices();
    YGFSMonHashCleanup(hFSHashTable);
    YGFSMonHashCleanup(hRegHashTable);
	return TRUE;
}

//----------------------------------------------------------------------
//
// OnDeviceInit
//
//
//----------------------------------------------------------------------
BOOL 
OnDeviceInit(
    VOID
    )
{
    //
    // Hook IFS functions.
    //
    PrevIFSHookProc = IFSMgr_InstallFileSystemApiHook(YGFSMonHookProc);
    return TRUE;
}

//----------------------------------------------------------------------
//
// OnSysDynamicDeviceExit
//
// Dynamic exit. Unhook everything.
//
//----------------------------------------------------------------------
BOOL 
OnDeviceReboot(
    VOID
    )
{
    //
    // Unhook IFS functions.
    //
    IFSMgr_RemoveFileSystemApiHook(YGFSMonHookProc);

    return TRUE;
}

