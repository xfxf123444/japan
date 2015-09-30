/*++

Copyright (c) 2003-2003  YuGuang Corporation

Module Name:

    YGFSMon.c

Abstract:

    This module contains the code that implements the general purpose sample
    file system filter driver.

Environment:

    Kernel mode

--*/

#ifdef WINXP
#include "ntifs.h"
#else
#include "ntddk.h"
#endif
#include <WDMSec.h>
#include "stdarg.h"
#include "stdio.h"
#include "..\Export\YGFSMon.h"
#include "YGFSMon.h"
//
//  Enable these warnings in the code.
//

#pragma warning(error:4100)   // Unreferenced formal parameter
#pragma warning(error:4101)   // Unreferenced formal parameter

/////////////////////////////////////////////////////////////////////////////
//
//                      Global variables
//
/////////////////////////////////////////////////////////////////////////////
// {24C214D9-A006-4cb2-8109-7F895926BB07}
static const GUID GUID_DEVCLASS_YGFSMON = 
{ 0x24c214d9, 0xa006, 0x4cb2, { 0x81, 0x9, 0x7f, 0x89, 0x59, 0x26, 0xbb, 0x7 } };


PDRIVER_OBJECT g_YGFSMonDriverObject = NULL;
PDEVICE_OBJECT g_YGFSMonControlDeviceObject = NULL;
BOOLEAN             bStartWork = FALSE;
FSDVOLINFO          FsVolTable[MAXDRIVELETTER];
ULONG               ProcessNameOffset;
PHASHOBJ	        HashTable[NUMHASH];
ERESOURCE			HashMutex;
PFSPATHINFO         ExcludeProcessList = NULL;
PFSPATHINFO         IncludePathList = NULL;
PFSPATHINFO         ExcludePathList = NULL;
PFSPATHINFO		    MonitorFileList = NULL;
ERESOURCE			ResourceMutex;
PMODIFYFILELIST     ModifyFileList = NULL;
ERESOURCE			StoreMutex;
WCHAR               PathPreFix[4] = {'\\','?','?','\\'};
//KSEMAPHORE			StoreSemaphore;

/////////////////////////////////////////////////////////////////////////////
//
//  Assign text sections for each routine.
//
/////////////////////////////////////////////////////////////////////////////

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, DriverUnload)
#pragma alloc_text(PAGE, YGFSMonFsNotification)
#pragma alloc_text(PAGE, YGFSMonFsControl)
#pragma alloc_text(PAGE, YGFSMonFsControlMountVolume)
#pragma alloc_text(PAGE, YGFSMonFsControlLoadFileSystem)
#pragma alloc_text(PAGE, YGFSMonFastIoCheckIfPossible)
#pragma alloc_text(PAGE, YGFSMonFastIoRead)
#pragma alloc_text(PAGE, YGFSMonFastIoWrite)
#pragma alloc_text(PAGE, YGFSMonFastIoQueryBasicInfo)
#pragma alloc_text(PAGE, YGFSMonFastIoQueryStandardInfo)
#pragma alloc_text(PAGE, YGFSMonFastIoLock)
#pragma alloc_text(PAGE, YGFSMonFastIoUnlockSingle)
#pragma alloc_text(PAGE, YGFSMonFastIoUnlockAll)
#pragma alloc_text(PAGE, YGFSMonFastIoUnlockAllByKey)
#pragma alloc_text(PAGE, YGFSMonFastIoDeviceControl)
#pragma alloc_text(PAGE, YGFSMonFastIoDetachDevice)
#pragma alloc_text(PAGE, YGFSMonFastIoQueryNetworkOpenInfo)
#pragma alloc_text(PAGE, YGFSMonFastIoMdlRead)
#pragma alloc_text(PAGE, YGFSMonFastIoPrepareMdlWrite)
#pragma alloc_text(PAGE, YGFSMonFastIoMdlWriteComplete)
#pragma alloc_text(PAGE, YGFSMonFastIoReadCompressed)
#pragma alloc_text(PAGE, YGFSMonFastIoWriteCompressed)
#pragma alloc_text(PAGE, YGFSMonFastIoQueryOpen)
#ifdef WINXP
#pragma alloc_text(PAGE, YGFSMonPreFsFilterPassThrough)
#pragma alloc_text(PAGE, YGFSMonPostFsFilterPassThrough)
#endif
#pragma alloc_text(PAGE, YGFSMonAttachToFileSystemDevice)
#pragma alloc_text(PAGE, YGFSMonDetachFromFileSystemDevice)
#pragma alloc_text(PAGE, YGFSMonEnumerateFileSystemVolumes)
#pragma alloc_text(PAGE, YGFSMonAttachToMountedDevice)
#endif

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
    )
{
	PFSPATHINFO pPathInfo;
	PMODIFYFILELIST pModifyInfo;
	ULONG i;

	PHASHOBJ pObject;
	ExAcquireResourceSharedLite(&HashMutex,TRUE);
	for (i = 0;i<NUMHASH;i++)
	{
		while (HashTable[i])
		{
			pObject = HashTable[i];
			HashTable[i] = pObject->pNext;
			ExFreePool(pObject);
		}
	}
	memset((UCHAR *)HashTable,0,NUMHASH*sizeof(PHASHOBJ));
	ExReleaseResourceLite(&HashMutex);

	ExAcquireResourceSharedLite(&ResourceMutex,TRUE);
	while (ExcludeProcessList)
	{
		pPathInfo = ExcludeProcessList;
		ExcludeProcessList = pPathInfo->pNext;
		if (pPathInfo->pDestPath)	ExFreePool(pPathInfo->pDestPath);
		if (pPathInfo->pMonitorPath)	ExFreePool(pPathInfo->pMonitorPath);
		ExFreePool(pPathInfo);
	}
	while (IncludePathList)
	{
		pPathInfo = IncludePathList;
		IncludePathList = pPathInfo->pNext;
		if (pPathInfo->pDestPath)	ExFreePool(pPathInfo->pDestPath);
		if (pPathInfo->pMonitorPath)	ExFreePool(pPathInfo->pMonitorPath);
		ExFreePool(pPathInfo);
	}
	while (ExcludePathList)
	{
		pPathInfo = ExcludePathList;
		ExcludePathList = pPathInfo->pNext;
		if (pPathInfo->pDestPath)	ExFreePool(pPathInfo->pDestPath);
		if (pPathInfo->pMonitorPath)	ExFreePool(pPathInfo->pMonitorPath);
		ExFreePool(pPathInfo);
	}
	while (MonitorFileList)
	{
		pPathInfo = MonitorFileList;
		MonitorFileList = pPathInfo->pNext;
		if (pPathInfo->pDestPath)	ExFreePool(pPathInfo->pDestPath);
		if (pPathInfo->pMonitorPath)	ExFreePool(pPathInfo->pMonitorPath);
		ExFreePool(pPathInfo);
	}
	while (ModifyFileList)
	{
		pModifyInfo = ModifyFileList;
		ModifyFileList = pModifyInfo->pNext;
		ExFreePool(pModifyInfo);
	}

	ExReleaseResourceLite(&ResourceMutex);
    return;
}

//----------------------------------------------------------------------
//
// MatchWithPattern
//
// Converts strings to upper-case before calling core comparison routine.
//
//----------------------------------------------------------------------

BOOLEAN MatchWithPattern( PCHAR Pattern, PCHAR Name,BOOLEAN bDirMatch)
{
    int        nPatternLen,nNameLen;
	UCHAR      ucTemp;
	STRING     ntPattern,ntName;
    BOOLEAN    isMatch = FALSE;

	nPatternLen = strlen(Pattern);
	nNameLen = strlen(Name);
	if (nPatternLen > nNameLen) return FALSE;

	nNameLen = nPatternLen;
	ucTemp = Name[nNameLen];
	Name[nNameLen] = 0;

	RtlInitString(&ntPattern,Pattern);
	RtlInitString(&ntName,Name);

	if (!RtlCompareString(&ntName,&ntPattern,TRUE))
	{
		Name[nNameLen] = ucTemp;
		if (bDirMatch)
			isMatch = Name[nNameLen] == 0 || Name[nNameLen] == '\\' || Name[nNameLen-1] == '\\';
		else  isMatch = (Name[nNameLen] == 0);
	}
	Name[nNameLen] = ucTemp;
    return isMatch;
}

/*BOOLEAN MatchWithPattern( PCHAR Pattern, PCHAR Name,BOOLEAN bDirMatch)
{
    int        nPatternLen,nNameLen;
	UCHAR      ucTemp;
	STRING     ntPattern,ntName;
    BOOLEAN    isMatch = FALSE;

	nPatternLen = strlen(Pattern);
	nNameLen = strlen(Name);
	if (nPatternLen > nNameLen)
	{
		ucTemp = Pattern[nNameLen];
		Pattern[nNameLen] = 0;
	}
	else
	{
		nNameLen = nPatternLen;
		ucTemp = Name[nNameLen];
		Name[nNameLen] = 0;
	}

	RtlInitString(&ntPattern,Pattern);
	RtlInitString(&ntName,Name);

	if (!RtlCompareString(&ntName,&ntPattern,TRUE))
	{
		if (nNameLen == nPatternLen) Name[nNameLen] = ucTemp;
		else Pattern[nNameLen] = ucTemp;
		if (bDirMatch)
			isMatch = ((Pattern[nNameLen] == Name[nNameLen]) || 
					(!Pattern[nNameLen] && (Name[nNameLen] == '\\' || Name[nNameLen-1] == '\\')));
		else  isMatch = (Pattern[nNameLen] == Name[nNameLen]);
	}
	else
	{
		if (nNameLen == nPatternLen) Name[nNameLen] = ucTemp;
		else Pattern[nNameLen] = ucTemp;
	}
    return isMatch;
}
*/
ULONG GetMonitorFileInfo(UCHAR *pPath,PCONVERT_BUF pBuff)
{
	PFSPATHINFO         pPathInfo;
	ULONG				ulMatchType = MATCH_NONE;

	ExAcquireResourceSharedLite(&ResourceMutex,TRUE);
    for(pPathInfo = ExcludePathList;pPathInfo;pPathInfo = pPathInfo->pNext ) {

        if( MatchWithPattern(pPathInfo->pMonitorPath, pPath,TRUE)) {

			ExReleaseResourceLite(&ResourceMutex);
            return MATCH_NONE;
        }
    }

    for(pPathInfo = IncludePathList;pPathInfo;pPathInfo = pPathInfo->pNext ) 
	{
        if( MatchWithPattern(pPathInfo->pMonitorPath, pPath,TRUE))
		{
			ulMatchType = MATCH_INCLUDE_PATH;
			pBuff->ulCompressLevel = pPathInfo->ulCompressLevel;
			pBuff->usNodeType = pPathInfo->usNodeType;
			pBuff->usModifyType = pPathInfo->usModifyType;
			pBuff->bCheckPasswordAlways = pPathInfo->bCheckPasswordAlways;
			pBuff->bErrorLimit = pPathInfo->bErrorLimit;
			pBuff->ulMaxErrLimit = pPathInfo->ulMaxErrLimit;
			strcpy(pBuff->szMonitorFile,pPathInfo->pMonitorPath);
			strcpy(pBuff->szDestFile,pPathInfo->pDestPath);
			memcpy(pBuff->szPassword,pPathInfo->szPassword,60);
			break;
		}
    }
	ExReleaseResourceLite(&ResourceMutex);
	return ulMatchType;
}

ULONG MatchWithPathFilter(PCONVERT_BUF pMatchInfo)
{
	PFSPATHINFO         pPathInfo;
	ULONG				ulMatchType = MATCH_NONE;

	ExAcquireResourceSharedLite(&ResourceMutex,TRUE);
    for(pPathInfo = ExcludePathList;pPathInfo;pPathInfo = pPathInfo->pNext ) {

        if( MatchWithPattern(pPathInfo->pMonitorPath,pMatchInfo->szMonitorFile,TRUE)) {

			ExReleaseResourceLite(&ResourceMutex);
            return MATCH_NONE;
        }
    }

    for(pPathInfo = IncludePathList;pPathInfo;pPathInfo = pPathInfo->pNext ) 
	{
        if( MatchWithPattern(pPathInfo->pMonitorPath,pMatchInfo->szMonitorFile,TRUE))
		{
			ulMatchType = MATCH_INCLUDE_PATH;
			pMatchInfo->ulCompressLevel = pPathInfo->ulCompressLevel;
			pMatchInfo->usNodeType = pPathInfo->usNodeType;
			pMatchInfo->usModifyType = pPathInfo->usModifyType;
			pMatchInfo->bCheckPasswordAlways = pPathInfo->bCheckPasswordAlways;
			pMatchInfo->bErrorLimit = pPathInfo->bErrorLimit;
			pMatchInfo->ulMaxErrLimit = pPathInfo->ulMaxErrLimit;
			strcpy(pMatchInfo->szDestFile,pMatchInfo->szMonitorFile);
			strcat(pMatchInfo->szDestFile,".chy");
			memcpy(pMatchInfo->szPassword,pPathInfo->szPassword,60);
			break;
		}
    }
	
	if (!ulMatchType)
	{
		for(pPathInfo = MonitorFileList;pPathInfo;pPathInfo = pPathInfo->pNext ) 
		{
			if( MatchWithPattern(pPathInfo->pMonitorPath,pMatchInfo->szMonitorFile,FALSE))
			{
				ulMatchType = MATCH_MONITOR_FILE;
				pMatchInfo->ulCompressLevel = pPathInfo->ulCompressLevel;
				pMatchInfo->usNodeType = pPathInfo->usNodeType;
				pMatchInfo->usModifyType = pPathInfo->usModifyType;
				pMatchInfo->bCheckPasswordAlways = pPathInfo->bCheckPasswordAlways;
				pMatchInfo->bErrorLimit = pPathInfo->bErrorLimit;
				pMatchInfo->ulMaxErrLimit = pPathInfo->ulMaxErrLimit;
				strcpy(pMatchInfo->szDestFile,pPathInfo->pDestPath);
				memcpy(pMatchInfo->szPassword,pPathInfo->szPassword,60);
				break;
			}
		}
	}
	ExReleaseResourceLite(&ResourceMutex);
	return ulMatchType;
}

BOOLEAN YGFSMonAddExcludeProcess(PCHAR  pProcess)
{
	PFSPATHINFO pPathInfo;

	pPathInfo = ExAllocatePool(NonPagedPool, sizeof(FSPATHINFO));
	if( !pPathInfo ) return FALSE;
	memset(pPathInfo,0,sizeof(FSPATHINFO));
	pPathInfo->pMonitorPath = ExAllocatePool(NonPagedPool,NT_PROCNAMELEN+1);
	if (!pPathInfo->pMonitorPath)
	{
		ExFreePool(pPathInfo);
		return FALSE;
	}

	strncpy(pPathInfo->pMonitorPath,pProcess,NT_PROCNAMELEN);
	pPathInfo->pMonitorPath[NT_PROCNAMELEN - 1] = 0;

	ExAcquireResourceSharedLite(&ResourceMutex,TRUE);
	pPathInfo->pNext = ExcludeProcessList;
	ExcludeProcessList = pPathInfo;
	ExReleaseResourceLite(&ResourceMutex);
	return TRUE;
}

BOOLEAN YGFSMonRemoveExcludeProcess(PCHAR  pProcess)
{
	PFSPATHINFO pCur,pPrev = NULL;
	int         nNameLen;

	nNameLen = strlen(pProcess);
	if (nNameLen >= NT_PROCNAMELEN) nNameLen = NT_PROCNAMELEN - 1;
	pProcess[nNameLen] = 0;

	ExAcquireResourceSharedLite(&ResourceMutex,TRUE);
	for (pCur = ExcludeProcessList;pCur;pCur = pCur->pNext)
	{
		if(MatchWithPattern(pCur->pMonitorPath,pProcess,FALSE))
		{
			if (pPrev) pPrev->pNext = pCur->pNext;
			else ExcludeProcessList = pCur->pNext;
			ExFreePool(pCur);
			break;
		}
		pPrev = pCur;
	}
	ExReleaseResourceLite(&ResourceMutex);
	return TRUE;
}

BOOLEAN YGFSMonAddExcludePath(PCHAR  pPath)
{
	PFSPATHINFO pPathInfo;
	int nPathLen;

	nPathLen = strlen(pPath);
	if (!nPathLen || nPathLen >= MAXPATHLEN) return FALSE;

	pPathInfo = ExAllocatePool(NonPagedPool, sizeof(FSPATHINFO));
	if( !pPathInfo ) return FALSE;
	memset(pPathInfo,0,sizeof(FSPATHINFO));

	pPathInfo->pMonitorPath = ExAllocatePool(NonPagedPool,nPathLen+1);
	if (!pPathInfo->pMonitorPath)
	{
		ExFreePool(pPathInfo);
		return FALSE;
	}

	strcpy(pPathInfo->pMonitorPath,pPath);

	ExAcquireResourceSharedLite(&ResourceMutex,TRUE);
	pPathInfo->pNext = ExcludePathList;
	ExcludePathList = pPathInfo;
	ExReleaseResourceLite(&ResourceMutex);
	return TRUE;
}

BOOLEAN YGFSMonRemoveExcludePath(PCHAR  pPath)
{
	PFSPATHINFO pCur,pPrev = NULL;
	int         nPathLen;

	nPathLen = strlen(pPath);
	if (!nPathLen || nPathLen >= MAXPATHLEN) return FALSE;

	ExAcquireResourceSharedLite(&ResourceMutex,TRUE);
	for (pCur = ExcludePathList;pCur;pCur = pCur->pNext)
	{
		if(MatchWithPattern(pCur->pMonitorPath,pPath,FALSE))
		{
			if (pPrev) pPrev->pNext = pCur->pNext;
			else ExcludePathList = pCur->pNext;
			if (pCur->pMonitorPath) ExFreePool(pCur->pMonitorPath);
			if (pCur->pDestPath) ExFreePool(pCur->pDestPath);
			ExFreePool(pCur);
			break;
		}
		pPrev = pCur;
	}
	ExReleaseResourceLite(&ResourceMutex);
	return TRUE;
}

BOOLEAN CheckExistMonitor(PCONVERT_BUF pMonitorItem)
{
	int nPathLen;
	PFSPATHINFO pPathInfo,pPrev = NULL;
	BOOLEAN		bResult = FALSE;

	ExAcquireResourceSharedLite(&ResourceMutex,TRUE);
	switch (pMonitorItem->usNodeType)
	{
	case CONVERT_NODETYPE_AUTO_FILE:
		for (pPathInfo = MonitorFileList;pPathInfo;pPathInfo = pPathInfo->pNext)
		{
			if(MatchWithPattern(pPathInfo->pMonitorPath,pMonitorItem->szMonitorFile,FALSE))
			{
				if(!MatchWithPattern(pPathInfo->pDestPath,pMonitorItem->szDestFile,FALSE))
				{
					ExFreePool(pPathInfo->pDestPath);
					pPathInfo->pDestPath = NULL;
					nPathLen = strlen(pMonitorItem->szDestFile);
					pPathInfo->pDestPath = ExAllocatePool(NonPagedPool,nPathLen+1);
					strcpy(pPathInfo->pDestPath,pMonitorItem->szDestFile);
				}
				bResult = TRUE;
				break;
			}
			pPrev = pPathInfo;
		}
		break;
	case CONVERT_NODETYPE_FILE:
		for (pPathInfo = MonitorFileList;pPathInfo;pPathInfo = pPathInfo->pNext)
		{
			if(MatchWithPattern(pPathInfo->pMonitorPath,pMonitorItem->szMonitorFile,FALSE))
			{
				bResult = TRUE;
				break;
			}
		}
		break;
	case CONVERT_NODETYPE_DIRECTORY:
		for (pPathInfo = IncludePathList;pPathInfo;pPathInfo = pPathInfo->pNext)
		{
			if(MatchWithPattern(pPathInfo->pMonitorPath,pMonitorItem->szMonitorFile,TRUE))
			{
				bResult = TRUE;
				break;
			}
		}
		break;
	default:
		break;
	}
	ExReleaseResourceLite(&ResourceMutex);
	return bResult;
}

BOOLEAN YGFSMonAddMonitorItem(PCONVERT_BUF  pMonitorItem)
{
	PFSPATHINFO pPathInfo;
	int nPathLen;

	nPathLen = strlen(pMonitorItem->szMonitorFile);
	if (!nPathLen || nPathLen >= MAXPATHLEN) return FALSE;

	if (CheckExistMonitor(pMonitorItem)) return FALSE;

	pPathInfo = ExAllocatePool(NonPagedPool, sizeof(FSPATHINFO));
	if( !pPathInfo ) return FALSE;
	memset(pPathInfo,0,sizeof(FSPATHINFO));

	pPathInfo->pMonitorPath = ExAllocatePool(NonPagedPool,nPathLen+1);
	if (!pPathInfo->pMonitorPath)
	{
		ExFreePool(pPathInfo);
		return FALSE;
	}
	pPathInfo->usNodeType = pMonitorItem->usNodeType;
	strcpy(pPathInfo->pMonitorPath,pMonitorItem->szMonitorFile);
	memcpy(pPathInfo->szPassword,pMonitorItem->szPassword,60);
	pPathInfo->ulCompressLevel = pMonitorItem->ulCompressLevel;
	pPathInfo->ulCompressLevel = pMonitorItem->ulCompressLevel;
	pPathInfo->bCheckPasswordAlways = pMonitorItem->bCheckPasswordAlways;
	pPathInfo->bErrorLimit = pMonitorItem->bErrorLimit;
	pPathInfo->ulMaxErrLimit = pMonitorItem->ulMaxErrLimit;

	nPathLen = strlen(pMonitorItem->szDestFile);
	pPathInfo->pDestPath = ExAllocatePool(NonPagedPool,nPathLen+1);
	if (!pPathInfo->pDestPath)
	{
		ExFreePool(pPathInfo->pMonitorPath);
		ExFreePool(pPathInfo);
		return FALSE;
	}
	strcpy(pPathInfo->pDestPath,pMonitorItem->szDestFile);

	ExAcquireResourceSharedLite(&ResourceMutex,TRUE);
	switch (pMonitorItem->usNodeType)
	{
	case CONVERT_NODETYPE_AUTO_FILE:
	case CONVERT_NODETYPE_FILE:
		pPathInfo->pNext = MonitorFileList;
		MonitorFileList = pPathInfo;
		break;
	case CONVERT_NODETYPE_DIRECTORY:
		pPathInfo->pNext = IncludePathList;
		IncludePathList = pPathInfo;
		break;
	default:
		break;
	}
	ExReleaseResourceLite(&ResourceMutex);
	return TRUE;
}

BOOLEAN YGFSMonRemoveMonitorItem(PCONVERT_BUF  pMonitorItem)
{
	PFSPATHINFO pCur,pPrev = NULL;
	int         nPathLen;

	nPathLen = strlen(pMonitorItem->szMonitorFile);
	if (!nPathLen || nPathLen >= MAXPATHLEN) return FALSE;

	ExAcquireResourceSharedLite(&ResourceMutex,TRUE);

	if (pMonitorItem->usNodeType == CONVERT_NODETYPE_FILE ||
		pMonitorItem->usNodeType == CONVERT_NODETYPE_AUTO_FILE)
		pCur = MonitorFileList;
	else pCur = IncludePathList; 

	while (pCur)
	{
		if(MatchWithPattern(pCur->pMonitorPath,pMonitorItem->szMonitorFile,FALSE))
		{
			if (pPrev) pPrev->pNext = pCur->pNext;
			else
			{
				if (pMonitorItem->usNodeType == CONVERT_NODETYPE_FILE ||
					pMonitorItem->usNodeType == CONVERT_NODETYPE_AUTO_FILE)
					MonitorFileList = pCur->pNext;
				else IncludePathList = pCur->pNext;
			}
			if (pCur->pDestPath) ExFreePool(pCur->pDestPath);
			if (pCur->pMonitorPath) ExFreePool(pCur->pMonitorPath);
			ExFreePool(pCur);
			break;
		}
		pPrev = pCur;
		pCur = pCur->pNext;
	}
	ExReleaseResourceLite(&ResourceMutex);
	return TRUE;
}

VOID YGFSMonAddConvertRecord(PHASHOBJ  hashEntry)
{
	PMODIFYFILELIST convertBuf = NULL,pModifyNode;

	convertBuf = ExAllocatePool(NonPagedPool, sizeof(MODIFYFILELIST));
	if( !convertBuf ) return;

	memcpy(&convertBuf->ConvertInfo,&hashEntry->MatchInfo,sizeof(CONVERT_BUF));
	convertBuf->pNext = NULL;

	ExAcquireResourceSharedLite(&StoreMutex,TRUE);

	if (ModifyFileList)
	{
		for (pModifyNode = ModifyFileList;pModifyNode;pModifyNode = pModifyNode->pNext)
		{
			if (MatchWithPattern(pModifyNode->ConvertInfo.szMonitorFile,convertBuf->ConvertInfo.szMonitorFile,FALSE))
			{
				if (convertBuf->ConvertInfo.usModifyType == FILE_OPEN_MODIFYED)
					pModifyNode->ConvertInfo.usModifyType = FILE_OPEN_MODIFYED;
				ExFreePool(convertBuf);
				convertBuf = NULL;
				break;
			}
			if (!pModifyNode->pNext) 
			{
				pModifyNode->pNext = convertBuf;
				break;
			}
		}
	}
	else ModifyFileList = convertBuf;
	ExReleaseResourceLite(&StoreMutex);
}

BOOLEAN	YGFSMonSetWriteFlag(ULONG ulHashIndex,BOOLEAN bRename)
{
	BOOLEAN  bResult = FALSE;
	PHASHOBJ  hashEntry;

	ExAcquireResourceSharedLite(&HashMutex,TRUE);

    //
    // Look-up the entry.
    //
    hashEntry = HashTable[HASHOBJECT(ulHashIndex)];
    while(hashEntry)
	{
        if (hashEntry->ulHashIndex == ulHashIndex)
		{
			if (bRename && hashEntry->MatchInfo.usModifyType == FILE_OPEN_RENAME)
				hashEntry->MatchInfo.usModifyType = FILE_OPEN_MODIFYED;

			if (!bRename && hashEntry->MatchInfo.usModifyType == FILE_OPEN_NORMAL)
				hashEntry->MatchInfo.usModifyType = FILE_OPEN_MODIFYED;

			bResult = TRUE;
			break;
		}
        hashEntry = hashEntry->pNext;
	}
	ExReleaseResourceLite(&HashMutex);
	return bResult;
}

PHASHOBJ YGFSMonLogHash(ULONG fileObject,ULONG ulModifyType,PCONVERT_BUF pMatchInfo)
{
    PHASHOBJ            newEntry;

    // 
    // Now that we have a name associated with the file object, put the
    // association in a hash table
    //

    newEntry = ExAllocatePool(NonPagedPool, sizeof(HASHOBJ));

    //
    // If no memory for a new entry, oh well.
    //
    if( newEntry ) 
	{
	    newEntry->ulHashIndex = fileObject;
		newEntry->ulRefCount = 1;
		memcpy(&newEntry->MatchInfo,pMatchInfo,sizeof(CONVERT_BUF));
		newEntry->MatchInfo.usModifyType = (USHORT)ulModifyType;
		ExAcquireResourceSharedLite(&HashMutex,TRUE);
        newEntry->pNext = HashTable[ HASHOBJECT(fileObject) ];
        HashTable[ HASHOBJECT(fileObject) ] = newEntry;	
		ExReleaseResourceLite(&HashMutex);
    }
	return newEntry;
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
    ULONG         ulHashIndex
    )
{
	PHASHOBJ  hashEntry,prevEntry;

	ExAcquireResourceSharedLite(&HashMutex,TRUE);

    //
    // Look-up the entry.
    //
    hashEntry = HashTable[HASHOBJECT(ulHashIndex)];
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

			HashTable[HASHOBJECT(ulHashIndex)] = hashEntry->pNext;
		}
		if (hashEntry->MatchInfo.usModifyType == FILE_OPEN_MODIFYED ||
			hashEntry->MatchInfo.usNodeType == CONVERT_NODETYPE_AUTO_FILE)
			YGFSMonAddConvertRecord(hashEntry);
		ExFreePool( hashEntry);
	}
	ExReleaseResourceLite(&HashMutex);
}

//----------------------------------------------------------------------
//
// YGFSMonGetOldestRecord
//
// Traverse the list of allocated buffers to find the last one, which
// will be the oldest (as we want to return the oldest data to the GUI
// first).
//
//----------------------------------------------------------------------
BOOLEAN YGFSMonGetOldestRecord(PCONVERT_BUF pModifyFile)
{
    PMODIFYFILELIST  ptr;

	pModifyFile->usNodeType = CONVERT_NODETYPE_NONE;

	ExAcquireResourceSharedLite(&StoreMutex,TRUE);
	ptr = ModifyFileList;
	if (ptr) 
		ModifyFileList = ptr->pNext;
	ExReleaseResourceLite(&StoreMutex);
	if (ptr)
	{
		memcpy(pModifyFile,&ptr->ConvertInfo,sizeof(CONVERT_BUF));
		ExFreePool(ptr);
	}
    return TRUE;
}

NTSTATUS
YGFSMonQueryCompletion (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PKEVENT SynchronizingEvent
    )
/*++

Routine Description:

    This routine does the cleanup necessary once the query request completed
    by the file system.
    
Arguments:

    DeviceObject - This will be NULL since we originated this
        Irp.

    Irp - The io request structure containing the information
        about the current state of our file name query.

    SynchronizingEvent - The event to signal to notify the 
        originator of this request that the operation is
        complete.

Return Value:

    Returns STATUS_MORE_PROCESSING_REQUIRED so that IO Manager
    will not try to free the Irp again.

--*/
{

    UNREFERENCED_PARAMETER( DeviceObject );
    
    //
    //  Make sure that the Irp status is copied over to the user's
    //  IO_STATUS_BLOCK so that the originator of this irp will know
    //  the final status of this operation.
    //

    ASSERT( NULL != Irp->UserIosb );
    *Irp->UserIosb = Irp->IoStatus;

    //
    //  Signal SynchronizingEvent so that the originator of this
    //  Irp know that the operation is completed.
    //

    KeSetEvent( SynchronizingEvent, IO_NO_INCREMENT, FALSE );

    //
    //  We are now done, so clean up the irp that we allocated.
    //

    IoFreeIrp( Irp );

    return STATUS_MORE_PROCESSING_REQUIRED;
}


NTSTATUS
YGFSMonQueryInformationFile (
	IN PDEVICE_OBJECT NextDeviceObject,
	IN PFILE_OBJECT FileObject,
	OUT PVOID FileInformation,
	IN ULONG Length,
	IN FILE_INFORMATION_CLASS FileInformationClass,
	OUT PULONG LengthReturned OPTIONAL
	)

/*++

Routine Description:

    This routine returns the requested information about a specified file.
    The information returned is determined by the FileInformationClass that
    is specified, and it is placed into the caller's FileInformation buffer.

Arguments:

    NextDeviceObject - Supplies the device object where this IO should start
        in the device stack.

    FileObject - Supplies the file object about which the requested
        information should be returned.

    FileInformation - Supplies a buffer to receive the requested information
        returned about the file.  This must be a buffer allocated from kernel
        space.

    Length - Supplies the length, in bytes, of the FileInformation buffer.

    FileInformationClass - Specifies the type of information which should be
        returned about the file.

    LengthReturned - the number of bytes returned if the operation was 
        successful.

Return Value:

    The status returned is the final completion status of the operation.

--*/

{
    PIRP irp = NULL;
    PIO_STACK_LOCATION irpSp = NULL;
    IO_STATUS_BLOCK ioStatusBlock;
    KEVENT event;
    NTSTATUS status;

    PAGED_CODE();
    
    //
    //  In DBG builds, make sure that we have valid parameters before we do 
    //  any work here.
    //

    ASSERT( NULL != NextDeviceObject );
    ASSERT( NULL != FileObject );
    ASSERT( NULL != FileInformation );
    
    //
    //  The parameters look ok, so setup the Irp.
    //

    KeInitializeEvent( &event, NotificationEvent, FALSE );
    ioStatusBlock.Status = STATUS_SUCCESS;
    ioStatusBlock.Information = 0;

    irp = IoAllocateIrp( NextDeviceObject->StackSize, FALSE );
    
    if (irp == NULL) {

        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    //  Set our current thread as the thread for this
    //  irp so that the IO Manager always knows which
    //  thread to return to if it needs to get back into
    //  the context of the thread that originated this
    //  irp.
    //
    
    irp->Tail.Overlay.Thread = PsGetCurrentThread();

    //
    //  Set that this irp originated from the kernel so that
    //  the IO Manager knows that the buffers do not
    //  need to be probed.
    //
    
    irp->RequestorMode = KernelMode;

    //
    //  Initialize the UserIosb and UserEvent in the 
    irp->UserIosb = &ioStatusBlock;
    irp->UserEvent = NULL;

    //
    //  Set the IRP_SYNCHRONOUS_API to denote that this
    //  is a synchronous IO request.
    //

    irp->Flags = IRP_SYNCHRONOUS_API;

    irpSp = IoGetNextIrpStackLocation( irp );

    irpSp->MajorFunction = IRP_MJ_QUERY_INFORMATION;
    irpSp->FileObject = FileObject;

    //
    //  Setup the parameters for IRP_MJ_QUERY_INFORMATION.  These
    //  were supplied by the caller of this routine.
    //  The buffer we want to be filled in should be placed in
    //  the system buffer.
    //

    irp->AssociatedIrp.SystemBuffer = FileInformation;

    irpSp->Parameters.QueryFile.Length = Length;
    irpSp->Parameters.QueryFile.FileInformationClass = FileInformationClass;

    //
    //  Set up the completion routine so that we know when our
    //  request for the file name is completed.  At that time,
    //  we can free the irp.
    //
    
    IoSetCompletionRoutine( irp, 
                            YGFSMonQueryCompletion, 
                            &event, 
                            TRUE, 
                            TRUE, 
                            TRUE );

    status = IoCallDriver( NextDeviceObject, irp );

    if (STATUS_PENDING == status) {

        KeWaitForSingleObject( &event, 
                               Executive, 
                               KernelMode,
                               FALSE,
                               NULL );
     }

    //
    //  Verify the completion has actually been run
    //

    ASSERT(KeReadStateEvent(&event) || !NT_SUCCESS(ioStatusBlock.Status));


    if (ARGUMENT_PRESENT(LengthReturned)) {

        *LengthReturned = (ULONG) ioStatusBlock.Information;
    }

    return ioStatusBlock.Status;
}

NTSTATUS YGFSMonParseReName(IN PYGFS_HOOK_EXTENSION  hookExt,IN PIRP Irp,IN PFILE_OBJECT FileObject,PCHAR name,PCONVERT_BUF  pMatchInfo)
{
	ULONG ulMatchType,i = 0;
    UNICODE_STRING nameString;
	PFILE_RENAME_INFORMATION pFileInfo = Irp->AssociatedIrp.SystemBuffer;
	WCHAR           *pPathName;
    ANSI_STRING     AnsiName;

	if (YGFSMonGetProcess(name) && pFileInfo->FileNameLength >= 4)
	{
		pPathName = (WCHAR*)ExAllocatePool(NonPagedPool, pFileInfo->FileNameLength+sizeof(WCHAR));
		if (pPathName)
		{
			memcpy(pPathName,pFileInfo->FileName,pFileInfo->FileNameLength);
			pPathName[pFileInfo->FileNameLength/2] = 0;
			RtlInitUnicodeString(&nameString,pPathName);
			RtlUnicodeStringToAnsiString( &AnsiName, &nameString, TRUE );    
			if (!memcmp(pFileInfo->FileName,PathPreFix,4*sizeof(WCHAR)))
				i = 4;
			strncpy(pMatchInfo->szMonitorFile,&AnsiName.Buffer[i],AnsiName.Length-i);
			pMatchInfo->szMonitorFile[AnsiName.Length-i] = 0;
			RtlFreeAnsiString( &AnsiName );
			ExFreePool(pPathName);
			ulMatchType = MatchWithPathFilter(pMatchInfo);
			if (ulMatchType)
			{
				KdPrint(("%s\tIRP_MJ_SET_INFORMATION\t%x:Directory:%s\n", name,(ULONG)FileObject,pMatchInfo->szMonitorFile));
				YGFSMonLogHash((ULONG)FileObject,FILE_OPEN_RENAME,pMatchInfo);
			}
		}
	}
	IoCopyCurrentIrpStackLocationToNext(Irp);
    //
    // Grab the time stamp and store it in the current stack location. This
    // is legal since the stack location is ours, and we're done looking at 
    // the parameters. This makes it easy to pass this to the completion routine. The
    // DiskPerf example in the NT DDK uses this trick.
    //
    IoSetCompletionRoutine( Irp, YGFSMonHookDone,NULL, TRUE, TRUE, TRUE );

    //
    // Return the results of the call to the caller
    //
    return IoCallDriver( hookExt->FileSystem, Irp );
}
/////////////////////////////////////////////////////////////////////////////
//
//                      Functions
//
/////////////////////////////////////////////////////////////////////////////

NTSTATUS
DriverEntry (
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )

/*++

Routine Description:

    This is the initialization routine for the SFILTER file system filter
    driver.  This routine creates the device object that represents this
    driver in the system and registers it for watching all file systems that
    register or unregister themselves as active file systems.

Arguments:

    DriverObject - Pointer to driver object created by the system.

Return Value:

    The function value is the final status from the initialization operation.

--*/

{
    PFAST_IO_DISPATCH fastIoDispatch;
    UNICODE_STRING nameString;
    UNICODE_STRING linkString;
    UNICODE_STRING		sddlString;
#ifdef WINXP
    FS_FILTER_CALLBACKS YGFSMonCallbacks;
#endif
    PDEVICE_OBJECT rawDeviceObject;
    PFILE_OBJECT FileObject;
    NTSTATUS status;
    ULONG i;

    UNREFERENCED_PARAMETER(RegistryPath);
    //
    //  Get Registry values
    //

    //
    //  Save our Driver Object, set our UNLOAD routine
    //

    g_YGFSMonDriverObject = DriverObject;
    g_YGFSMonDriverObject->DriverUnload = DriverUnload;
//_asm int 3
    //
    //  Create the Control Device Object (CDO).  This object represents this 
    //  driver.  Note that it does not have a device extension.
    //
#ifndef WINXP
    RtlInitUnicodeString( &nameString, L"\\Device\\YGFSMon" );
#else
    RtlInitUnicodeString( &nameString, L"\\FileSystem\\Filters\\YGFSMon" );
#endif
    (void) RtlInitUnicodeString( &sddlString, L"D:P(A;;GA;;;SY)(A;;GA;;;BA)(A;;GA;;;WD)(A;;GA;;;RC)");
    status = IoCreateDeviceSecure(
                DriverObject,
                0,                      //has not device extension
                &nameString,
                FILE_DEVICE_YGFSMON,
                FILE_DEVICE_SECURE_OPEN,
                (BOOLEAN) FALSE,
                &sddlString,
                (LPCGUID)&GUID_DEVCLASS_YGFSMON,
                &g_YGFSMonControlDeviceObject
                );
/*    status = IoCreateDevice(
                DriverObject,
                0,                      //has not device extension
                &nameString,
                FILE_DEVICE_YGFSMON,
                FILE_DEVICE_SECURE_OPEN,
                FALSE,
                &g_YGFSMonControlDeviceObject );*/

    if (!NT_SUCCESS( status )) {

        KdPrint(( "DriverEntry: Error creating control device object, status=%08x\n", status ));
        return status;
    }

    RtlInitUnicodeString( &linkString, YGFSMON_DOSDEVICE_NAME );
    status = IoCreateSymbolicLink( &linkString, &nameString );

    if (!NT_SUCCESS(status)) {

        IoDeleteSymbolicLink( &linkString );
        status = IoCreateSymbolicLink( &linkString, &nameString );

        if (!NT_SUCCESS(status)) {

            KdPrint(("DriverEntry: IoCreateSymbolicLink failed\n") );
            IoDeleteDevice(g_YGFSMonControlDeviceObject);
            return status;
        }
    }
    //
    //  Initialize the driver object with this device driver's entry points.
    //

    for (i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++) {

        DriverObject->MajorFunction[i] = YGFSMonDispatch;
    }

    fastIoDispatch = ExAllocatePoolWithTag( NonPagedPool, sizeof( FAST_IO_DISPATCH ), YGFS_POOL_TAG );
    if (!fastIoDispatch) {

        IoDeleteDevice( g_YGFSMonControlDeviceObject );
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory( fastIoDispatch, sizeof( FAST_IO_DISPATCH ) );

    fastIoDispatch->SizeOfFastIoDispatch = sizeof( FAST_IO_DISPATCH );
    fastIoDispatch->FastIoCheckIfPossible = YGFSMonFastIoCheckIfPossible;
    fastIoDispatch->FastIoRead = YGFSMonFastIoRead;
    fastIoDispatch->FastIoWrite = YGFSMonFastIoWrite;
    fastIoDispatch->FastIoQueryBasicInfo = YGFSMonFastIoQueryBasicInfo;
    fastIoDispatch->FastIoQueryStandardInfo = YGFSMonFastIoQueryStandardInfo;
    fastIoDispatch->FastIoLock = YGFSMonFastIoLock;
    fastIoDispatch->FastIoUnlockSingle = YGFSMonFastIoUnlockSingle;
    fastIoDispatch->FastIoUnlockAll = YGFSMonFastIoUnlockAll;
    fastIoDispatch->FastIoUnlockAllByKey = YGFSMonFastIoUnlockAllByKey;
    fastIoDispatch->FastIoDeviceControl = YGFSMonFastIoDeviceControl;
    fastIoDispatch->FastIoDetachDevice = YGFSMonFastIoDetachDevice;
    fastIoDispatch->FastIoQueryNetworkOpenInfo = YGFSMonFastIoQueryNetworkOpenInfo;
    fastIoDispatch->MdlRead = YGFSMonFastIoMdlRead;
    fastIoDispatch->MdlReadComplete = YGFSMonFastIoMdlReadComplete;
    fastIoDispatch->PrepareMdlWrite = YGFSMonFastIoPrepareMdlWrite;
    fastIoDispatch->MdlWriteComplete = YGFSMonFastIoMdlWriteComplete;
    fastIoDispatch->FastIoReadCompressed = YGFSMonFastIoReadCompressed;
    fastIoDispatch->FastIoWriteCompressed = YGFSMonFastIoWriteCompressed;
    fastIoDispatch->MdlReadCompleteCompressed = YGFSMonFastIoMdlReadCompleteCompressed;
    fastIoDispatch->MdlWriteCompleteCompressed = YGFSMonFastIoMdlWriteCompleteCompressed;
    fastIoDispatch->FastIoQueryOpen = YGFSMonFastIoQueryOpen;

    DriverObject->FastIoDispatch = fastIoDispatch;

#ifdef WINXP
    YGFSMonCallbacks.SizeOfFsFilterCallbacks = sizeof( FS_FILTER_CALLBACKS );
    YGFSMonCallbacks.PreAcquireForSectionSynchronization = YGFSMonPreFsFilterPassThrough;
    YGFSMonCallbacks.PostAcquireForSectionSynchronization = YGFSMonPostFsFilterPassThrough;
    YGFSMonCallbacks.PreReleaseForSectionSynchronization = YGFSMonPreFsFilterPassThrough;
    YGFSMonCallbacks.PostReleaseForSectionSynchronization = YGFSMonPostFsFilterPassThrough;
    YGFSMonCallbacks.PreAcquireForCcFlush = YGFSMonPreFsFilterPassThrough;
    YGFSMonCallbacks.PostAcquireForCcFlush = YGFSMonPostFsFilterPassThrough;
    YGFSMonCallbacks.PreReleaseForCcFlush = YGFSMonPreFsFilterPassThrough;
    YGFSMonCallbacks.PostReleaseForCcFlush = YGFSMonPostFsFilterPassThrough;
    YGFSMonCallbacks.PreAcquireForModifiedPageWriter = YGFSMonPreFsFilterPassThrough;
    YGFSMonCallbacks.PostAcquireForModifiedPageWriter = YGFSMonPostFsFilterPassThrough;
    YGFSMonCallbacks.PreReleaseForModifiedPageWriter = YGFSMonPreFsFilterPassThrough;
    YGFSMonCallbacks.PostReleaseForModifiedPageWriter = YGFSMonPostFsFilterPassThrough;

    status = FsRtlRegisterFileSystemFilterCallbacks( DriverObject, &YGFSMonCallbacks );

    if (!NT_SUCCESS( status )) {
        
        DriverObject->FastIoDispatch = NULL;
        ExFreePool( fastIoDispatch );
        IoDeleteDevice( g_YGFSMonControlDeviceObject );
        return status;
    }
#endif
    //
    //  Register this driver for watching file systems coming and going.  This
    //  enumerates all existing file systems as well as new file systems as they
    //  come and go.
    //

    status = IoRegisterFsRegistrationChange( DriverObject, YGFSMonFsNotification );
    if (!NT_SUCCESS( status )) {

        KdPrint(( "DriverEntry: Error registering FS change notification, status=%08x\n", status ));

        DriverObject->FastIoDispatch = NULL;
        ExFreePool( fastIoDispatch );
        IoDeleteDevice( g_YGFSMonControlDeviceObject );
        return status;
    }


    RtlInitUnicodeString( &nameString, L"\\Device\\RawDisk" );

    status = IoGetDeviceObjectPointer(
                    &nameString,
                    FILE_READ_ATTRIBUTES,
                    &FileObject,
                    &rawDeviceObject );

    if (NT_SUCCESS( status )) {

        YGFSMonFsNotification( rawDeviceObject, TRUE );
        ObDereferenceObject( FileObject );
    }
#ifndef WINXP
    RtlInitUnicodeString( &nameString, L"\\Fat" );

    status = IoGetDeviceObjectPointer(
                    &nameString,
                    FILE_READ_ATTRIBUTES,
                    &FileObject,
                    &rawDeviceObject );

    if (NT_SUCCESS( status )) {

        YGFSMonFsNotification( rawDeviceObject, TRUE );
        ObDereferenceObject( FileObject );
    }

    RtlInitUnicodeString( &nameString, L"\\Ntfs" );

    status = IoGetDeviceObjectPointer(
                    &nameString,
                    FILE_READ_ATTRIBUTES,
                    &FileObject,
                    &rawDeviceObject );

    if (NT_SUCCESS( status )) {

        YGFSMonFsNotification( rawDeviceObject, TRUE );
        ObDereferenceObject( FileObject );
    }
#endif

    //
    //  Clear the initializing flag on the control device object since we
    //  have now successfully initialized everything.
    //

    ClearFlag( g_YGFSMonControlDeviceObject->Flags, DO_DEVICE_INITIALIZING );

    ProcessNameOffset = YGFSMonGetProcessNameOffset();

	memset((UCHAR *)FsVolTable,0,sizeof(FSDVOLINFO)*MAXDRIVELETTER);

	memset((UCHAR *)HashTable,0,NUMHASH*sizeof(PHASHOBJ));
    ExInitializeResourceLite( &HashMutex);
    ExInitializeResourceLite( &ResourceMutex);
    ExInitializeResourceLite( &StoreMutex);
	YGFSMonAddExcludeProcess("System");
//    KeInitializeSemaphore(&StoreSemaphore, 0, MAXLONG);

    //
    // Register for shutdown notification
    //
    IoRegisterShutdownNotification( g_YGFSMonControlDeviceObject );
    return STATUS_SUCCESS;
}

#ifndef WINXP
NTSTATUS
  IoEnumerateDeviceObjectList(
    IN PDRIVER_OBJECT  DriverObject,
    IN PDEVICE_OBJECT  *DeviceObjectList,
    IN ULONG  DeviceObjectListSize,
    OUT PULONG  ActualNumberDeviceObjects
    )
{
	NTSTATUS status = STATUS_BUFFER_TOO_SMALL;
	PDEVICE_OBJECT  pDeviceObj = DriverObject->DeviceObject;
	*ActualNumberDeviceObjects = 0;
	if (!DeviceObjectList)
	{
		while (pDeviceObj)
		{
			(*ActualNumberDeviceObjects) ++;
			pDeviceObj = pDeviceObj->NextDevice;
			if (DriverObject->DeviceObject == pDeviceObj) break;
		}
	}
	else
	{
		while (DeviceObjectListSize && pDeviceObj)
		{
			DeviceObjectListSize -= sizeof(PDEVICE_OBJECT);
			DeviceObjectList[*ActualNumberDeviceObjects] = pDeviceObj;
			(*ActualNumberDeviceObjects) ++;
			pDeviceObj = pDeviceObj->NextDevice;
		}
		if (!pDeviceObj) status = STATUS_SUCCESS;
	}
	return status;
}
#endif

VOID
DriverUnload (
    IN PDRIVER_OBJECT DriverObject
    )

/*++

Routine Description:

    This routine is called when a driver can be unloaded.  This performs all of
    the necessary cleanup for unloading the driver from memory.  Note that an
    error can NOT be returned from this routine.
    
    When a request is made to unload a driver the IO System will cache that
    information and not actually call this routine until the following states
    have occurred:
    - All device objects which belong to this filter are at the top of their
      respective attachment chains.
    - All handle counts for all device objects which belong to this filter have
      gone to zero.

    WARNING: Microsoft does not officially support the unloading of File
             System Filter Drivers.  This is an example of how to unload
             your driver if you would like to use it during development.
             This should not be made available in production code.

Arguments:

    DriverObject - Driver object for this module

Return Value:

    None.

--*/

{
    PYGFS_HOOK_EXTENSION devExt;
    PFAST_IO_DISPATCH fastIoDispatch;
    NTSTATUS status=STATUS_SUCCESS;
    ULONG numDevices;
    UNICODE_STRING linkString;
    ULONG i;
    LARGE_INTEGER interval;

#   define DEVOBJ_LIST_SIZE 64
    PDEVICE_OBJECT devList[DEVOBJ_LIST_SIZE];

    //
    //  Log we are unloading
    //

    KdPrint(("DriverUnload:                        Unloading driver\n"));

    //
    //  Don't get anymore file system change notifications
    //

    IoUnregisterFsRegistrationChange( DriverObject, YGFSMonFsNotification );
    //
    // Delete the symbolic link for our GUI device
    //
    RtlInitUnicodeString( &linkString, YGFSMON_DOSDEVICE_NAME );
    IoDeleteSymbolicLink( &linkString);
    //
    //  This is the loop that will go through all of the devices we are attached
    //  to and detach from them.  Since we don't know how many there are and
    //  we don't want to allocate memory (because we can't return an error)
    //  we will free them in chunks using a local array on the stack.
    //

    for (;;) {

        //
        //  Get what device objects we can for this driver.  Quit if there
        //  are not any more.
        //

        status = IoEnumerateDeviceObjectList(
                        DriverObject,
                        devList,
                        sizeof(devList),
                        &numDevices);

        if (numDevices <= 0)  {

            break;
        }

        numDevices = min( numDevices, DEVOBJ_LIST_SIZE );

        //
        //  First go through the list and detach each of the devices.
        //  Our control device object does not have a DeviceExtension and
        //  is not attached to anything so don't detach it.
        //

        for (i=0; i < numDevices; i++) {

            devExt = devList[i]->DeviceExtension;
            if (NULL != devExt) {

                IoDetachDevice( devExt->FileSystem );
            }
        }

        //
        //  The IO Manager does not currently add a reference count to a device
        //  object for each outstanding IRP.  This means there is no way to
        //  know if there are any outstanding IRPs on the given device.
        //  We are going to wait for a reasonable amount of time for pending
        //  irps to complete.  
        //
        //  WARNING: This does not work 100% of the time and the driver may be
        //           unloaded before all IRPs are completed.  This can easily
        //           occur under stress situations and if a long lived IRP is
        //           pending (like oplocks and directory change notifications).
        //           The system will fault when this Irp actually completes.
        //           This is a sample of how to do this during testing.  This
        //           is not recommended for production code.
        //

        interval.QuadPart = -5 * (10 * 1000 * 1000);      //delay 5 seconds
        KeDelayExecutionThread( KernelMode, FALSE, &interval );

        //
        //  Now go back through the list and delete the device objects.
        //

        for (i=0; i < numDevices; i++) {

            //
            //  See if this is our control device object.  If not then cleanup
            //  the device extension.  If so then clear the global pointer
            //  that references it.
            //

            if (NULL != devList[i]->DeviceExtension) {

                YGFSMonCleanupMountedDevice( devList[i] );

            } else {

                ASSERT(devList[i] == g_YGFSMonControlDeviceObject);
                g_YGFSMonControlDeviceObject = NULL;
            }

            //
            //  Delete the device object, remove reference counts added by
            //  IoEnumerateDeviceObjectList.  Note that the delete does
            //  not actually occur until the reference count goes to zero.
            //

            IoDeleteDevice( devList[i] );
#ifdef WINXP
            ObDereferenceObject( devList[i] );
#endif
        }
    }

    //
    //  Free our FastIO table
    //

    fastIoDispatch = DriverObject->FastIoDispatch;
    DriverObject->FastIoDispatch = NULL;
    ExFreePool( fastIoDispatch );
    YGFSMonHashCleanup();
//    KeReleaseSemaphore(&StoreSemaphore,0,1,FALSE );
	ExDeleteResourceLite(&HashMutex);
	ExDeleteResourceLite(&ResourceMutex);
	ExDeleteResourceLite(&StoreMutex);
}


VOID
YGFSMonFsNotification (
    IN PDEVICE_OBJECT DeviceObject,
    IN BOOLEAN FsActive
    )

/*++

Routine Description:

    This routine is invoked whenever a file system has either registered or
    unregistered itself as an active file system.

    For the former case, this routine creates a device object and attaches it
    to the specified file system's device object.  This allows this driver
    to filter all requests to that file system.  Specifically we are looking
    for MOUNT requests so we can attach to newly mounted volumes.

    For the latter case, this file system's device object is located,
    detached, and deleted.  This removes this file system as a filter for
    the specified file system.

Arguments:

    DeviceObject - Pointer to the file system's device object.

    FsActive - Boolean indicating whether the file system has registered
        (TRUE) or unregistered (FALSE) itself as an active file system.

Return Value:

    None.

--*/

{
    PAGED_CODE();


    //
    //  Handle attaching/detaching from the given file system.
    //

    if (FsActive) {

        YGFSMonAttachToFileSystemDevice( DeviceObject);

    } else {

        YGFSMonDetachFromFileSystemDevice( DeviceObject );
    }
}


/////////////////////////////////////////////////////////////////////////////
//
//                  IRP Handling Routines
//
/////////////////////////////////////////////////////////////////////////////


NTSTATUS
YGFSMonDispatch(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine is the main dispatch routine for the general purpose file
    system driver.  It simply passes requests onto the next driver in the
    stack, which is presumably a disk file system.

Arguments:

    DeviceObject - Pointer to the device object for this driver.

    Irp - Pointer to the request packet representing the I/O request.

Return Value:

    The function value is the status of the operation.

Note:

    A note to file system filter implementers:  
        This routine actually "passes" through the request by taking this
        driver out of the IRP stack.  If the driver would like to pass the
        I/O request through, but then also see the result, then rather than
        taking itself out of the loop it could keep itself in by copying the
        caller's parameters to the next stack location and then set its own
        completion routine.  

        Hence, instead of calling:
    
            IoSkipCurrentIrpStackLocation( Irp );

        You could instead call:

            IoCopyCurrentIrpStackLocationToNext( Irp );
            IoSetCompletionRoutine( Irp, NULL, NULL, FALSE, FALSE, FALSE );


        This example actually NULLs out the caller's I/O completion routine, but
        this driver could set its own completion routine so that it would be
        notified when the request was completed (see YGFSMonCreate for an example of
        this).

--*/

{
    ASSERT(KeGetCurrentIrql() <= APC_LEVEL);

    //
    //  If this is for our control device object, fail the operation
    //

    if (g_YGFSMonControlDeviceObject == DeviceObject) {

		return YGFSMonDeviceRoutine(DeviceObject, Irp);
    }


    return( YGFSMonHookRoutine( DeviceObject, Irp ));
}

//----------------------------------------------------------------------
//
// YGFSMonDeviceRoutine
//
// In this routine we handle requests to our own device. The only 
// requests we care about handling explicitely are IOCTL commands that
// we will get from the GUI. We also expect to get Create and Close 
// commands when the GUI opens and closes communications with us.
//
//----------------------------------------------------------------------
NTSTATUS YGFSMonDeviceRoutine( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp )
{
    PIO_STACK_LOCATION  irpStack;
    PVOID               inputBuffer;
    PVOID               outputBuffer;
    ULONG               inputBufferLength;
    ULONG               outputBufferLength;
    ULONG               ioControlCode;
    NTSTATUS status = STATUS_SUCCESS;

    //
    // Go ahead and set the request up as successful
    //
    Irp->IoStatus.Information = 0;

    //
    // Get a pointer to the current location in the Irp. This is where
    // the function codes and parameters are located.
    //
    irpStack = IoGetCurrentIrpStackLocation (Irp);

    //
    // Get the pointer to the input/output buffer and its length
    //
    inputBuffer     = Irp->AssociatedIrp.SystemBuffer;
    inputBufferLength   = irpStack->Parameters.DeviceIoControl.InputBufferLength;
    outputBuffer    = Irp->AssociatedIrp.SystemBuffer;
    outputBufferLength  = irpStack->Parameters.DeviceIoControl.OutputBufferLength;
    ioControlCode   = irpStack->Parameters.DeviceIoControl.IoControlCode;

    switch (irpStack->MajorFunction) {
	case IRP_MJ_CREATE:

        DbgPrint(("YGFSMon: IRP_MJ_CREATE\n"));
        status = STATUS_SUCCESS;
        break;

    case IRP_MJ_CLOSE:

        DbgPrint(("YGFSMon: IRP_MJ_CLOSE\n"));
        status = STATUS_SUCCESS;

        break;

    case IRP_MJ_DEVICE_CONTROL:

        DbgPrint (("YGFSMon: IRP_MJ_DEVICE_CONTROL\n"));

        //
        // Get output buffer if its passed as an MDL
        //
        if( Irp->MdlAddress ) {

            outputBuffer = MmGetSystemAddressForMdl( Irp->MdlAddress );
        }

        //
        // Its a request from the GUI. Simply call our fast handler.
        //
        if (YGFSMonFastIoDeviceControl( irpStack->FileObject, TRUE,
                                    inputBuffer, inputBufferLength, 
                                    outputBuffer, outputBufferLength,
                                    ioControlCode, &Irp->IoStatus, DeviceObject ))
			status = STATUS_SUCCESS;
		else status = STATUS_INVALID_PARAMETER;
        break;
	case IRP_MJ_CLEANUP:
        
        //
        //  This is the cleanup that we will see when all references to a handle
	    //  opened to filespy's control device object are cleaned up.  We don't
        //  have to do anything here since we wait until the actual IRP_MJ_CLOSE
        //  to clean up the name cache.  Just complete the IRP successfully.
        //

        status = STATUS_SUCCESS;

        break;
	default:
        status = STATUS_INVALID_DEVICE_REQUEST;
		break;
    }

    Irp->IoStatus.Status = status;
    IoCompleteRequest( Irp, IO_NO_INCREMENT );
    return Irp->IoStatus.Status;   
}

//----------------------------------------------------------------------
//
// YGFSMonHookRoutine
//
// This routine is the main hook routine where we figure out what
// calls are being sent to the file system.
//
//----------------------------------------------------------------------
NTSTATUS YGFSMonHookRoutine( PDEVICE_OBJECT HookDevice, IN PIRP Irp )
{
    PIO_STACK_LOCATION  currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
    PFILE_OBJECT        FileObject;
    PYGFS_HOOK_EXTENSION     hookExt;
    CHAR                name[PROCNAMELEN] = {0};
	CONVERT_BUF			MatchInfo = {0};

	if (currentIrpStack->MajorFunction == IRP_MJ_FILE_SYSTEM_CONTROL)
		return YGFSMonFsControl(HookDevice,Irp);

    //
    // Extract the file object from the IRP
    //
    FileObject    = currentIrpStack->FileObject;

    //
    // Point at the device extension, which contains information on which
    // file system this IRP is headed for
    //
    hookExt = HookDevice->DeviceExtension;


	if (currentIrpStack->MajorFunction == IRP_MJ_SET_INFORMATION &&
		currentIrpStack->Parameters.SetFile.FileInformationClass == FileRenameInformation)
		return YGFSMonParseReName(hookExt,Irp,FileObject,name,&MatchInfo);

    //
    // Only log it if it passes the filter
    //
	if(YGFSMonGetProcess(name) && YGFSMonGetFullPath(FileObject,hookExt,&MatchInfo))
	{
        //
        // Determine what function we're dealing with
        //
        switch( currentIrpStack->MajorFunction ) {

        case IRP_MJ_CREATE:
            KdPrint(( "%s\tIRP_MJ_CREATE\t%x:%s\n", name,(ULONG)FileObject,MatchInfo.szMonitorFile));
            break;

        case IRP_MJ_CREATE_NAMED_PIPE:

            KdPrint(("%s\tIRP_MJ_CREATE_NAMED_PIPE\t%x:%s\n", name,(ULONG)FileObject,MatchInfo.szMonitorFile));
            break;

        case IRP_MJ_CREATE_MAILSLOT:

            KdPrint(("%s\tIRP_MJ_CREATE_MAILSLOT\t%x:%s\n", name,(ULONG)FileObject,MatchInfo.szMonitorFile));
            break;

        case IRP_MJ_READ:

            KdPrint(("%s\tIRP_MJ_READ%c\t%x:%s\tOffset: %d Length: %d\n", 
                         name, 
                         (Irp->Flags & IRP_PAGING_IO) || (Irp->Flags & IRP_SYNCHRONOUS_PAGING_IO) ? '*' : ' ',
                         (ULONG)FileObject,MatchInfo.szMonitorFile, 
                         currentIrpStack->Parameters.Read.ByteOffset.LowPart,
                         currentIrpStack->Parameters.Read.Length ));
            break;

        case IRP_MJ_WRITE:

            KdPrint(("%s\tIRP_MJ_WRITE%c\t%x:%s\tOffset: %d Length: %d\n", 
                         name, 
                         (Irp->Flags & IRP_PAGING_IO) || (Irp->Flags & IRP_SYNCHRONOUS_PAGING_IO) ? '*' : ' ',
                         (ULONG)FileObject,MatchInfo.szMonitorFile, 
                         currentIrpStack->Parameters.Write.ByteOffset.LowPart,
                         currentIrpStack->Parameters.Write.Length ));
			YGFSMonSetWriteFlag((ULONG)FileObject,FALSE);
            break;

        case IRP_MJ_CLOSE:

            KdPrint(("%s\tIRP_MJ_CLOSE\t%x:%s\t\n", name,(ULONG)FileObject,MatchInfo.szMonitorFile));

            //
            // This fileobject/name association can be discarded now.
            //
            YGFSMonFreeHashEntry((ULONG)FileObject );
            break;

        case IRP_MJ_FLUSH_BUFFERS:

            KdPrint(("%s\tIRP_MJ_FLUSH\t%x:%s\t\n", name,(ULONG)FileObject,MatchInfo.szMonitorFile));
            break;

        case IRP_MJ_QUERY_INFORMATION:
 
            KdPrint(("%s\tIRP_MJ_QUERY_INFORMATION\t%x:%s\n", name,(ULONG)FileObject,MatchInfo.szMonitorFile));
            break;

        case IRP_MJ_SET_INFORMATION:

            KdPrint(("%s\tIRP_MJ_SET_INFORMATION\t%x:%s\n", name,(ULONG)FileObject,MatchInfo.szMonitorFile));
            break;

        case IRP_MJ_QUERY_EA:

            KdPrint(("%s\tIRP_MJ_QUERY_EA\t%x:%s\t\n", name,(ULONG)FileObject,MatchInfo.szMonitorFile));
            break;

        case IRP_MJ_SET_EA:

            KdPrint(("%s\tIRP_MJ_SET_EA\t%x:%s\t\n", name,(ULONG)FileObject,MatchInfo.szMonitorFile));
            break;

        case IRP_MJ_QUERY_VOLUME_INFORMATION:

            KdPrint(("%s\tIRP_MJ_QUERY_VOLUME_INFORMATION\t%x:%s\t%x\n", 
                         name,(ULONG)FileObject,MatchInfo.szMonitorFile,currentIrpStack->Parameters.QueryVolume.FsInformationClass));
            break;

        case IRP_MJ_SET_VOLUME_INFORMATION:

            KdPrint(("%s\tIRP_MJ_SET_VOLUME_INFORMATION\t%x:%s\t\n", name,(ULONG)FileObject,MatchInfo.szMonitorFile));
            break;

        case IRP_MJ_DIRECTORY_CONTROL:

            switch( currentIrpStack->MinorFunction ) {
            case IRP_MN_NOTIFY_CHANGE_DIRECTORY:
                KdPrint(("%s\tIRP_MJ_DIRECTORY_CONTROL\t%x:%s\tChange Notify\n", name,(ULONG)FileObject,MatchInfo.szMonitorFile));
                break;
            case IRP_MN_QUERY_DIRECTORY:
                KdPrint(("%s\tIRP_MJ_DIRECTORY_CONTROL\t%x:%s\n", name,(ULONG)FileObject,MatchInfo.szMonitorFile));
                break; 
            default:
                KdPrint(("%s\tIRP_MJ_DIRECTORY_CONTROL\t%x:%s\t\n", name,(ULONG)FileObject,MatchInfo.szMonitorFile));
                break;
            }
            break;

        case IRP_MJ_FILE_SYSTEM_CONTROL:

            KdPrint(("%s\t%x\t%x:%s\n", 
                         name, currentIrpStack->Parameters.DeviceIoControl.IoControlCode,
						 (ULONG)FileObject,MatchInfo.szMonitorFile));
            break;

        case IRP_MJ_SHUTDOWN:

            KdPrint(("%s\tIRP_MJ_SHUTDOWN\t\t\n", name ));
            break;

        case IRP_MJ_LOCK_CONTROL:

            KdPrint(("%s\tIRP_MJ_LOCK_CONTROL\t%x:%s\tOffset: %d Length: %d\n",
                         name,(ULONG)FileObject,MatchInfo.szMonitorFile,
                         ((PLOCK_CONTROL)&currentIrpStack->Parameters)->ByteOffset.LowPart,
                         ((PLOCK_CONTROL)&currentIrpStack->Parameters)->Length ?
                         ((PLOCK_CONTROL)&currentIrpStack->Parameters)->Length->LowPart : 0 ));
            break;

        case IRP_MJ_CLEANUP:

            KdPrint(("%s\tIRP_MJ_CLEANUP\t%x:%s\t\n", name,(ULONG)FileObject,MatchInfo.szMonitorFile));
            YGFSMonFreeHashEntry((ULONG)FileObject );
            break;

        case IRP_MJ_DEVICE_CONTROL:
 
            KdPrint(("%s\tIRP_MJ_DEVICE_CONTROL\t%x:%s\tIOCTL: 0x%X\n", name, 
                        (ULONG)FileObject, MatchInfo.szMonitorFile, currentIrpStack->Parameters.DeviceIoControl.IoControlCode ));
			break;
        case IRP_MJ_QUERY_SECURITY:

            KdPrint(("%s\tIRP_MJ_QUERY_SECURITY\t%x:%s\t\n", 
                         name,(ULONG)FileObject,MatchInfo.szMonitorFile));
            break;

        case IRP_MJ_SET_SECURITY:

            KdPrint(("%s\tIRP_MJ_SET_SECURITY\t%x:%s\t\n", name,(ULONG)FileObject,MatchInfo.szMonitorFile));
            break;

        case IRP_MJ_PNP_POWER:
            
            KdPrint(("%s\tIRP_MJ_PNP_POWER\t%x:%s\t\n",name,(ULONG)FileObject,MatchInfo.szMonitorFile));
            break;

        default:

            KdPrint(("%s\t*UNKNOWN* 0x%X\t\t\n", name, currentIrpStack->MajorFunction ));
            break;
        }
    } else {

        //
        // Do name processing for the sake of keeping the hash table current
        //
        switch( currentIrpStack->MajorFunction ) {

		case IRP_MJ_CREATE:
		case IRP_MJ_CREATE_NAMED_PIPE:
		case IRP_MJ_CREATE_MAILSLOT:

            //
            // Clear any existing fileobject/name association stored in the
            // hash table
            //
            YGFSMonFreeHashEntry((ULONG)FileObject );
            break;
 
        case IRP_MJ_CLOSE:

            //
            // This fileobject/name association can be discarded now.
            //
            YGFSMonFreeHashEntry((ULONG)FileObject );
            break;
        }        
    }

	IoCopyCurrentIrpStackLocationToNext(Irp);
    //
    // Grab the time stamp and store it in the current stack location. This
    // is legal since the stack location is ours, and we're done looking at 
    // the parameters. This makes it easy to pass this to the completion routine. The
    // DiskPerf example in the NT DDK uses this trick.
    //
    IoSetCompletionRoutine( Irp, YGFSMonHookDone,NULL, TRUE, TRUE, TRUE );

    //
    // Return the results of the call to the caller
    //
    return IoCallDriver( hookExt->FileSystem, Irp );
}

//----------------------------------------------------------------------
// 
// YGFSMonHookDone
//
// Gets control after a filesystem operation has completed so that
// we can get return status information about it.
//
//----------------------------------------------------------------------
NTSTATUS YGFSMonHookDone( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp,
                          IN PVOID Context )
{
    PIO_STACK_LOCATION   IrpSp;
    PYGFS_HOOK_EXTENSION hookExt = DeviceObject->DeviceExtension;

    UNREFERENCED_PARAMETER( Context );

    //
    // A request completed - look at the result 
    //
    IrpSp = IoGetCurrentIrpStackLocation( Irp );    

    //
    // If this was an error on a create call, we have to free the hash entry we
    // created for this name
    //
	switch (IrpSp->MajorFunction)
	{
	case IRP_MJ_CREATE:
	case IRP_MJ_CREATE_NAMED_PIPE:
	case IRP_MJ_CREATE_MAILSLOT:
        if (!NT_SUCCESS( Irp->IoStatus.Status ))
	        YGFSMonFreeHashEntry((ULONG)IrpSp->FileObject );
		break;
	case IRP_MJ_SET_INFORMATION:
		if (IrpSp->Parameters.SetFile.FileInformationClass == FileRenameInformation)
		{
			if (NT_SUCCESS( Irp->IoStatus.Status ))
				YGFSMonSetWriteFlag((ULONG)IrpSp->FileObject,TRUE);
			YGFSMonFreeHashEntry((ULONG)IrpSp->FileObject );
		}
		break;
	case IRP_MJ_DIRECTORY_CONTROL:
        break;
	}
    //
    // Now we have to mark Irp as pending if necessary
    //
    if( Irp->PendingReturned ) {

        IoMarkIrpPending( Irp );
    }
    return Irp->IoStatus.Status;
}


NTSTATUS
YGFSMonFsControl (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine is invoked whenever an I/O Request Packet (IRP) w/a major
    function code of IRP_MJ_FILE_SYSTEM_CONTROL is encountered.  For most
    IRPs of this type, the packet is simply passed through.  However, for
    some requests, special processing is required.

Arguments:

    DeviceObject - Pointer to the device object for this driver.

    Irp - Pointer to the request packet representing the I/O request.

Return Value:

    The function value is the status of the operation.

--*/

{
    PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation( Irp );

    PAGED_CODE();
    ASSERT(KeGetCurrentIrql() <= APC_LEVEL);

    //
    //  If this is for our control device object, fail the operation
    //

    if (g_YGFSMonControlDeviceObject == DeviceObject) {

        Irp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
        Irp->IoStatus.Information = 0;

        IoCompleteRequest( Irp, IO_NO_INCREMENT );

        return STATUS_INVALID_DEVICE_REQUEST;
    }

    //
    //  Process the minor function code.
    //

    switch (irpSp->MinorFunction) 
	{
    case IRP_MN_MOUNT_VOLUME:
        return YGFSMonFsControlMountVolume( DeviceObject, Irp );
    case IRP_MN_LOAD_FILE_SYSTEM:
        return YGFSMonFsControlLoadFileSystem( DeviceObject, Irp );
	case IRP_MN_USER_FS_REQUEST:
        switch ((ULONG)irpSp->Parameters.Others.Argument3) 
		{		
		case FSCTL_DISMOUNT_VOLUME:
           {
                PYGFS_HOOK_EXTENSION devExt = DeviceObject->DeviceExtension;

                KdPrint(("YGFSMonFsControl:                         Dismounting volume") );
                break;
            }
        }
		break;
    }        

    //
    //  Pass all other file system control requests through.
    //

    IoSkipCurrentIrpStackLocation( Irp );
    return IoCallDriver( ((PYGFS_HOOK_EXTENSION)DeviceObject->DeviceExtension)->FileSystem, Irp );
}


NTSTATUS
YGFSMonFsControlCompletion (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    )

/*++

Routine Description:

    This routine is invoked for the completion of an FsControl request.  It
    signals an event used to re-sync back to the dispatch routine.

Arguments:

    DeviceObject - Pointer to this driver's device object that was attached to
            the file system device object

    Irp - Pointer to the IRP that was just completed.

    Context - Pointer to the event to signal

--*/

{
    UNREFERENCED_PARAMETER( DeviceObject );
    UNREFERENCED_PARAMETER( Irp );

    KeSetEvent((PKEVENT)Context, IO_NO_INCREMENT, FALSE);

    return STATUS_MORE_PROCESSING_REQUIRED;
}


NTSTATUS
YGFSMonFsControlMountVolume (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This processes a MOUNT VOLUME request

Arguments:

    DeviceObject - Pointer to the device object for this driver.

    Irp - Pointer to the request packet representing the I/O request.

Return Value:

    The status of the operation.

--*/

{
    PYGFS_HOOK_EXTENSION devExt = DeviceObject->DeviceExtension;
    PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation( Irp );
    PDEVICE_OBJECT newDeviceObject;
    PYGFS_HOOK_EXTENSION newDevExt;
    PDEVICE_OBJECT attachedDeviceObject;
    PVPB vpb;
    KEVENT waitEvent;
    NTSTATUS status;

    PAGED_CODE();

    //
    //  This is a mount request.  Create a device object that can be
    //  attached to the file system's volume device object if this request
    //  is successful.  We allocate this memory now since we can not return
    //  an error in the completion routine.  
    //
    //  Since the device object we are going to attach to has not yet been
    //  created (it is created by the base file system) we are going to use
    //  the type of the file system control device object.  We are assuming
    //  that the file system control device object will have the same type
    //  as the volume device objects associated with it.
    //

    status = IoCreateDevice(
                g_YGFSMonDriverObject,
                sizeof( YGFS_HOOK_EXTENSION ),
                NULL,
                DeviceObject->DeviceType,
                0,
                FALSE,
                &newDeviceObject );

    if (!NT_SUCCESS( status )) {

        //
        //  If we can not attach to the volume, then don't allow the volume
        //  to be mounted.
        //

        KdPrint(( "YGFSMonFsControlMountVolume: Error creating volume device object, status=%08x\n", status ));

        Irp->IoStatus.Information = 0;
        Irp->IoStatus.Status = status;
        IoCompleteRequest( Irp, IO_NO_INCREMENT );

        return status;
    }

    //
    //  We need to save the RealDevice object pointed to by the vpb
    //  parameter because this vpb may be changed by the underlying
    //  file system.  Both FAT and CDFS may change the VPB address if
    //  the volume being mounted is one they recognize from a previous
    //  mount.
    //

    newDevExt = newDeviceObject->DeviceExtension;
	RtlInitEmptyUnicodeString( &newDevExt->DeviceName,
							   newDevExt->DeviceNameBuffer,
							   sizeof(newDevExt->DeviceNameBuffer) );

	newDevExt->DiskSystem = irpSp->Parameters.MountVolume.Vpb->RealDevice;
	YGFSMonGetObjectName(newDevExt->DiskSystem,&newDevExt->DeviceName);
    //
    //  Initialize our completion routine
    //

    KeInitializeEvent( &waitEvent, NotificationEvent, FALSE );

    IoCopyCurrentIrpStackLocationToNext( Irp );

    IoSetCompletionRoutine( Irp,
                            YGFSMonFsControlCompletion,
                            &waitEvent,          //context parameter
                            TRUE,
                            TRUE,
                            TRUE );

    //
    //  Call the driver
    //

    status = IoCallDriver( devExt->FileSystem, Irp );

    //
    //  Wait for the completion routine to be called.  
    //  Note:  Once we get to this point we can no longer fail this operation.
    //

	if (STATUS_PENDING == status) {

		NTSTATUS localStatus = KeWaitForSingleObject(&waitEvent, Executive, KernelMode, FALSE, NULL);
	    ASSERT(STATUS_SUCCESS == localStatus);
	}

    //
    //  Verify the IoCompleteRequest was called
    //

    ASSERT(KeReadStateEvent(&waitEvent) ||
           !NT_SUCCESS(Irp->IoStatus.Status));

    //
    //  Get the correct VPB from the real device object saved in our
    //  device extension.  We do this because the VPB in the IRP stack
    //  may not be the correct VPB when we get here.  The underlying
    //  file system may change VPBs if it detects a volume it has
    //  mounted previously.
    //

    vpb = newDevExt->DiskSystem->Vpb;

    //
    //  Display a message when we detect that the VPB for the given
    //  device object has changed.
    //

    if (vpb != irpSp->Parameters.MountVolume.Vpb) {

        KdPrint(("YGFSMonFsControlMountVolume:              VPB in IRP stack changed   %p \"%wZ\" IRPVPB=%p VPB=%p\n",
                       vpb->DeviceObject,
					   newDevExt->DeviceName,
                       irpSp->Parameters.MountVolume.Vpb,
                       vpb) );
    }

    //
    //  If the operation succeeded and we are not already attached,
    //  attach to the device object.  Note: we can be already attached if
    //  the underlying file system revived a previous mount.
    //

    if (NT_SUCCESS( Irp->IoStatus.Status ) && 
        !YGFSMonIsAttachedToDevice( vpb->DeviceObject, &attachedDeviceObject )) {

        //
        //  Attach to the new mounted volume.  The file system device
        //  object that was just mounted is pointed to by the VPB.
        //

		newDevExt->LogicalDrive = YGFSMonGetDriverLetter(newDevExt->DiskSystem);
		newDevExt->DeviceType = newDevExt->DiskSystem->DeviceType;
        status = YGFSMonAttachToMountedDevice( vpb->DeviceObject,
		                                      newDeviceObject );

        if (!NT_SUCCESS( status )) { 

            //
            //  The attachment failed, cleanup.  Since we are in the
            //  post-mount phase, we can not fail this operation.
            //

            YGFSMonCleanupMountedDevice( newDeviceObject );
            IoDeleteDevice( newDeviceObject );
        }

    } else {

        //
        //  The mount request failed, handle it.
        //

        if (!NT_SUCCESS( Irp->IoStatus.Status )) {

            KdPrint(("YGFSMonFsControlMountVolume:              Mount volume failure for   %p \"%wZ\", status=%08x\n", 
                           DeviceObject,
						   newDevExt->DeviceName,
                           Irp->IoStatus.Status) );

        } else {

            KdPrint(("YGFSMonFsControlMountVolume               Mount volume failure for   %p \"%wZ\", already attached\n", 
                           ((attachedDeviceObject != NULL) ?
                                ((PYGFS_HOOK_EXTENSION)attachedDeviceObject->DeviceExtension)->FileSystem :
                                NULL),
							newDevExt->DeviceName));
        }

        //
        //  Cleanup and delete the device object we created
        //

        YGFSMonCleanupMountedDevice( newDeviceObject );
        IoDeleteDevice( newDeviceObject );
    }

    //
    //  Complete the request.  
    //  NOTE:  We must save the status before completing because after
    //         completing the IRP we can not longer access it (it might be
    //         freed).
    //

    status = Irp->IoStatus.Status;

    IoCompleteRequest( Irp, IO_NO_INCREMENT );

    return status;
}


NTSTATUS
YGFSMonFsControlLoadFileSystem (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine is invoked whenever an I/O Request Packet (IRP) w/a major
    function code of IRP_MJ_FILE_SYSTEM_CONTROL is encountered.  For most
    IRPs of this type, the packet is simply passed through.  However, for
    some requests, special processing is required.

Arguments:

    DeviceObject - Pointer to the device object for this driver.

    Irp - Pointer to the request packet representing the I/O request.

Return Value:

    The function value is the status of the operation.

--*/

{
    PYGFS_HOOK_EXTENSION devExt = DeviceObject->DeviceExtension;
    KEVENT waitEvent;
    NTSTATUS status;

    PAGED_CODE();

    //
    //  This is a "load file system" request being sent to a file system
    //  recognizer device object.  This IRP_MN code is only sent to 
    //  file system recognizers.
    //
    //  NOTE:  Since we no longer are attaching to the standard Microsoft file
    //         system recognizers we will normally never execute this code.
    //         However, there might be 3rd party file systems which have their
    //         own recognizer which may still trigger this IRP.
    //

    KdPrint(("YGFSMonFscontrolLoadFileSystem:           Loading File System, Detaching from \"%wZ\" \n",
		    devExt->DeviceName) );

    //
    //  Set a completion routine so we can delete the device object when
    //  the load is complete.
    //

    KeInitializeEvent( &waitEvent, NotificationEvent, FALSE );

    IoCopyCurrentIrpStackLocationToNext( Irp );

    IoSetCompletionRoutine(
        Irp,
        YGFSMonFsControlCompletion,
        &waitEvent,
        TRUE,
        TRUE,
        TRUE );

    //
    //  Detach from the file system recognizer device object.
    //

    IoDetachDevice( devExt->FileSystem );

    //
    //  Call the driver
    //

    status = IoCallDriver( devExt->FileSystem, Irp );

    //
    //  Wait for the completion routine to be called
    //

	if (STATUS_PENDING == status) {

		NTSTATUS localStatus = KeWaitForSingleObject(&waitEvent, Executive, KernelMode, FALSE, NULL);
	    ASSERT(STATUS_SUCCESS == localStatus);
	}

    //
    //  Verify the IoCompleteRequest was called
    //

    ASSERT(KeReadStateEvent(&waitEvent) ||
           !NT_SUCCESS(Irp->IoStatus.Status));

    //
    //  Display the name if requested
    //

    KdPrint(("YGFSMonFsControlLoadFileSystem:           Detaching from recognizer  %p \"%wZ\", status=%08x\n", 
                   DeviceObject,
				   devExt->DeviceName,
                   Irp->IoStatus.Status) );

    //
    //  Check status of the operation
    //

    if (!NT_SUCCESS( Irp->IoStatus.Status ) && 
        (Irp->IoStatus.Status != STATUS_IMAGE_ALREADY_LOADED)) {

        //
        //  The load was not successful.  Simply reattach to the recognizer
        //  driver in case it ever figures out how to get the driver loaded
        //  on a subsequent call.  There is not a lot we can do if this
        //  reattach fails.
        //

        devExt->FileSystem = IoAttachDeviceToDeviceStack( DeviceObject, 
                                         devExt->FileSystem);

        ASSERT(devExt->FileSystem != NULL);

    } else {

        //
        //  The load was successful, delete the Device object
        //

        YGFSMonCleanupMountedDevice( DeviceObject );
        IoDeleteDevice( DeviceObject );
    }

    //
    //  Continue processing the operation
    //

    status = Irp->IoStatus.Status;

    IoCompleteRequest( Irp, IO_NO_INCREMENT );

    return status;
}


/////////////////////////////////////////////////////////////////////////////
//
//                      FastIO Handling routines
//
/////////////////////////////////////////////////////////////////////////////

BOOLEAN
YGFSMonFastIoCheckIfPossible (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    IN BOOLEAN Wait,
    IN ULONG LockKey,
    IN BOOLEAN CheckForReadOperation,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for checking to see
    whether fast I/O is possible for this file.

    This function simply invokes the file system's corresponding routine, or
    returns FALSE if the file system does not implement the function.

Arguments:

    FileObject - Pointer to the file object to be operated on.

    FileOffset - Byte offset in the file for the operation.

    Length - Length of the operation to be performed.

    Wait - Indicates whether or not the caller is willing to wait if the
        appropriate locks, etc. cannot be acquired

    LockKey - Provides the caller's key for file locks.

    CheckForReadOperation - Indicates whether the caller is checking for a
        read (TRUE) or a write operation.

    IoStatus - Pointer to a variable to receive the I/O status of the
        operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.

--*/

{
    BOOLEAN         retval = FALSE;
    PDEVICE_OBJECT nextDeviceObject;
    PFAST_IO_DISPATCH fastIoDispatch;
    CHAR            name[PROCNAMELEN];
	CONVERT_BUF		MatchInfo = {0};

    PAGED_CODE();
    ASSERT(KeGetCurrentIrql() <= APC_LEVEL);

    if (DeviceObject->DeviceExtension) {

        ASSERT(IS_MY_DEVICE_OBJECT( DeviceObject ));

        //
        //  Pass through logic for this type of Fast I/O
        //

        nextDeviceObject = ((PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension)->FileSystem;
        ASSERT(nextDeviceObject);

        fastIoDispatch = nextDeviceObject->DriverObject->FastIoDispatch;

        if (VALID_FAST_IO_DISPATCH_HANDLER( fastIoDispatch, FastIoCheckIfPossible )) {

            retval = (fastIoDispatch->FastIoCheckIfPossible)(
                        FileObject,
                        FileOffset,
                        Length,
                        Wait,
                        LockKey,
                        CheckForReadOperation,
                        IoStatus,
                        nextDeviceObject );
			if(YGFSMonGetProcess(name) && YGFSMonGetFullPath(FileObject,(PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension,&MatchInfo))
			{
				KdPrint(("%s\tFASTIO_CHECK_IF_POSSIBLE\t%x:%s\t%s Offset: %d Length: %d\t%s\n", 
							 name,(ULONG)FileObject,MatchInfo.szMonitorFile, 
							 CheckForReadOperation ? "Read:" : "Write:",
							 FileOffset->LowPart, Length, 
							 retval?"SUCCESS":"FAILURE" )); 
			}
        }
    }
    return retval;
}


BOOLEAN
YGFSMonFastIoRead (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    IN BOOLEAN Wait,
    IN ULONG LockKey,
    OUT PVOID Buffer,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for reading from a
    file.

    This function simply invokes the file system's corresponding routine, or
    returns FALSE if the file system does not implement the function.

Arguments:

    FileObject - Pointer to the file object to be read.

    FileOffset - Byte offset in the file of the read.

    Length - Length of the read operation to be performed.

    Wait - Indicates whether or not the caller is willing to wait if the
        appropriate locks, etc. cannot be acquired

    LockKey - Provides the caller's key for file locks.

    Buffer - Pointer to the caller's buffer to receive the data read.

    IoStatus - Pointer to a variable to receive the I/O status of the
        operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.

--*/

{
    BOOLEAN             retval = FALSE;
    PDEVICE_OBJECT nextDeviceObject;
    PFAST_IO_DISPATCH fastIoDispatch;
    CHAR                name[PROCNAMELEN];
	CONVERT_BUF			MatchInfo = {0};

    PAGED_CODE();
    ASSERT(KeGetCurrentIrql() <= APC_LEVEL);

    if (DeviceObject->DeviceExtension) {

        ASSERT(IS_MY_DEVICE_OBJECT( DeviceObject ));

        //
        //  Pass through logic for this type of Fast I/O
        //

        nextDeviceObject = ((PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension)->FileSystem;
        ASSERT(nextDeviceObject);

        fastIoDispatch = nextDeviceObject->DriverObject->FastIoDispatch;

        if (VALID_FAST_IO_DISPATCH_HANDLER( fastIoDispatch, FastIoRead )) {

            retval = (fastIoDispatch->FastIoRead)(
                        FileObject,
                        FileOffset,
                        Length,
                        Wait,
                        LockKey,
                        Buffer,
                        IoStatus,
                        nextDeviceObject );
			if(YGFSMonGetProcess(name) && YGFSMonGetFullPath(FileObject,(PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension,&MatchInfo))
			{
				KdPrint(("%s\tFASTIO_READ\t%x:%s\tOffset: %d Length: %ld\t%x\n", 
							 name,(ULONG)FileObject,MatchInfo.szMonitorFile, FileOffset->LowPart, Length, retval));
			}
        }
    }
    return retval;
}


BOOLEAN
YGFSMonFastIoWrite (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    IN BOOLEAN Wait,
    IN ULONG LockKey,
    IN PVOID Buffer,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for writing to a
    file.

    This function simply invokes the file system's corresponding routine, or
    returns FALSE if the file system does not implement the function.

Arguments:

    FileObject - Pointer to the file object to be written.

    FileOffset - Byte offset in the file of the write operation.

    Length - Length of the write operation to be performed.

    Wait - Indicates whether or not the caller is willing to wait if the
        appropriate locks, etc. cannot be acquired

    LockKey - Provides the caller's key for file locks.

    Buffer - Pointer to the caller's buffer that contains the data to be
        written.

    IoStatus - Pointer to a variable to receive the I/O status of the
        operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.

--*/

{
    BOOLEAN             retval = FALSE;
    PDEVICE_OBJECT nextDeviceObject;
    PFAST_IO_DISPATCH fastIoDispatch;
    CHAR                name[PROCNAMELEN];
	CONVERT_BUF			MatchInfo = {0};

    PAGED_CODE();
    ASSERT(KeGetCurrentIrql() <= APC_LEVEL);

    if (DeviceObject->DeviceExtension) {

        ASSERT(IS_MY_DEVICE_OBJECT( DeviceObject ));

        //
        //  Pass through logic for this type of Fast I/O
        //

        nextDeviceObject = ((PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension)->FileSystem;
        ASSERT(nextDeviceObject);

        fastIoDispatch = nextDeviceObject->DriverObject->FastIoDispatch;

        if (VALID_FAST_IO_DISPATCH_HANDLER( fastIoDispatch, FastIoWrite )) {

            retval = (fastIoDispatch->FastIoWrite)(
                        FileObject,
                        FileOffset,
                        Length,
                        Wait,
                        LockKey,
                        Buffer,
                        IoStatus,
                        nextDeviceObject );
			if(YGFSMonGetProcess(name) && YGFSMonGetFullPath(FileObject,(PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension,&MatchInfo))
			{
				KdPrint(( "%s\tFASTIO_WRITE\t%x:%s\tOffset: %d Length: %d\t%x\n", 
							 name,(ULONG)FileObject,MatchInfo.szMonitorFile, FileOffset->LowPart, Length, retval)); 
				YGFSMonSetWriteFlag((ULONG)FileObject,FALSE);
			}
        }
    }
    return retval;
}


BOOLEAN
YGFSMonFastIoQueryBasicInfo (
    IN PFILE_OBJECT FileObject,
    IN BOOLEAN Wait,
    OUT PFILE_BASIC_INFORMATION Buffer,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for querying basic
    information about the file.

    This function simply invokes the file system's corresponding routine, or
    returns FALSE if the file system does not implement the function.

Arguments:

    FileObject - Pointer to the file object to be queried.

    Wait - Indicates whether or not the caller is willing to wait if the
        appropriate locks, etc. cannot be acquired

    Buffer - Pointer to the caller's buffer to receive the information about
        the file.

    IoStatus - Pointer to a variable to receive the I/O status of the
        operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.

--*/

{
    BOOLEAN             retval = FALSE;
    PDEVICE_OBJECT nextDeviceObject;
    PFAST_IO_DISPATCH fastIoDispatch;
    CHAR                name[PROCNAMELEN];
	CONVERT_BUF			MatchInfo = {0};

    PAGED_CODE();
    ASSERT(KeGetCurrentIrql() <= APC_LEVEL);

    if (DeviceObject->DeviceExtension) {

        ASSERT(IS_MY_DEVICE_OBJECT( DeviceObject ));

        //
        //  Pass through logic for this type of Fast I/O
        //

        nextDeviceObject = ((PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension)->FileSystem;
        ASSERT(nextDeviceObject);

        fastIoDispatch = nextDeviceObject->DriverObject->FastIoDispatch;

        if (VALID_FAST_IO_DISPATCH_HANDLER( fastIoDispatch, FastIoQueryBasicInfo )) {

            retval = (fastIoDispatch->FastIoQueryBasicInfo)(
                        FileObject,
                        Wait,
                        Buffer,
                        IoStatus,
                        nextDeviceObject );
			if(YGFSMonGetProcess(name) && YGFSMonGetFullPath(FileObject,(PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension,&MatchInfo))
			{
				KdPrint(( "%s\tFASTIO_QUERY_BASIC_INFO\t%x:%s\t\t%s\n", 
						 name,(ULONG)FileObject,MatchInfo.szMonitorFile, retval?"SUCCESS":"FAILURE" ));
			}
        }
    }
    return retval;
}


BOOLEAN
YGFSMonFastIoQueryStandardInfo (
    IN PFILE_OBJECT FileObject,
    IN BOOLEAN Wait,
    OUT PFILE_STANDARD_INFORMATION Buffer,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for querying standard
    information about the file.

    This function simply invokes the file system's corresponding routine, or
    returns FALSE if the file system does not implement the function.

Arguments:

    FileObject - Pointer to the file object to be queried.

    Wait - Indicates whether or not the caller is willing to wait if the
        appropriate locks, etc. cannot be acquired

    Buffer - Pointer to the caller's buffer to receive the information about
        the file.

    IoStatus - Pointer to a variable to receive the I/O status of the
        operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.

--*/

{
    BOOLEAN             retval = FALSE;
    PDEVICE_OBJECT nextDeviceObject;
    PFAST_IO_DISPATCH fastIoDispatch;
    CHAR                name[PROCNAMELEN];
	CONVERT_BUF			MatchInfo = {0};

    PAGED_CODE();
    ASSERT(KeGetCurrentIrql() <= APC_LEVEL);

    if (DeviceObject->DeviceExtension) {

        ASSERT(IS_MY_DEVICE_OBJECT( DeviceObject ));

        //
        //  Pass through logic for this type of Fast I/O
        //

        nextDeviceObject = ((PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension)->FileSystem;
        ASSERT(nextDeviceObject);

        fastIoDispatch = nextDeviceObject->DriverObject->FastIoDispatch;

        if (VALID_FAST_IO_DISPATCH_HANDLER( fastIoDispatch, FastIoQueryStandardInfo )) {

            retval = (fastIoDispatch->FastIoQueryStandardInfo)(
                        FileObject,
                        Wait,
                        Buffer,
                        IoStatus,
                        nextDeviceObject );
			if(YGFSMonGetProcess(name) && YGFSMonGetFullPath(FileObject,(PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension,&MatchInfo))
			{
				KdPrint(( "%s\tFASTIO_QUERY_STANDARD_INFO\t%x:%s\t\t%x\n", 
							 name,(ULONG)FileObject,MatchInfo.szMonitorFile, retval));
			}
        }
    }
    return retval;
}


BOOLEAN
YGFSMonFastIoLock (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN PLARGE_INTEGER Length,
    PEPROCESS ProcessId,
    ULONG Key,
    BOOLEAN FailImmediately,
    BOOLEAN ExclusiveLock,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for locking a byte
    range within a file.

    This function simply invokes the file system's corresponding routine, or
    returns FALSE if the file system does not implement the function.

Arguments:

    FileObject - Pointer to the file object to be locked.

    FileOffset - Starting byte offset from the base of the file to be locked.

    Length - Length of the byte range to be locked.

    ProcessId - ID of the process requesting the file lock.

    Key - Lock key to associate with the file lock.

    FailImmediately - Indicates whether or not the lock request is to fail
        if it cannot be immediately be granted.

    ExclusiveLock - Indicates whether the lock to be taken is exclusive (TRUE)
        or shared.

    IoStatus - Pointer to a variable to receive the I/O status of the
        operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.

--*/

{
    BOOLEAN             retval = FALSE;
    PDEVICE_OBJECT nextDeviceObject;
    PFAST_IO_DISPATCH fastIoDispatch;
    CHAR                name[PROCNAMELEN];
	CONVERT_BUF			MatchInfo = {0};

    PAGED_CODE();
    ASSERT(KeGetCurrentIrql() <= APC_LEVEL);

    if (DeviceObject->DeviceExtension) {

        ASSERT(IS_MY_DEVICE_OBJECT( DeviceObject ));

        //
        //  Pass through logic for this type of Fast I/O
        //

        nextDeviceObject = ((PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension)->FileSystem;
        ASSERT(nextDeviceObject);

        fastIoDispatch = nextDeviceObject->DriverObject->FastIoDispatch;

        if (VALID_FAST_IO_DISPATCH_HANDLER( fastIoDispatch, FastIoLock )) {

            retval = (fastIoDispatch->FastIoLock)(
                        FileObject,
                        FileOffset,
                        Length,
                        ProcessId,
                        Key,
                        FailImmediately,
                        ExclusiveLock,
                        IoStatus,
                        nextDeviceObject );
			if(YGFSMonGetProcess(name) && YGFSMonGetFullPath(FileObject,(PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension,&MatchInfo))
			{
				KdPrint(( "%s\tFASTIO_LOCK\t%x:%s\tExcl: %s Offset: %d Length: %d\t%x\n", 
							 name,(ULONG)FileObject,MatchInfo.szMonitorFile,
							 ExclusiveLock ? "Yes":"No", FileOffset ? FileOffset->LowPart : 0,
							 Length ? Length->LowPart : 0, retval));
			}
        }
    }
    return retval;
}


BOOLEAN
YGFSMonFastIoUnlockSingle (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN PLARGE_INTEGER Length,
    PEPROCESS ProcessId,
    ULONG Key,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for unlocking a byte
    range within a file.

    This function simply invokes the file system's corresponding routine, or
    returns FALSE if the file system does not implement the function.

Arguments:

    FileObject - Pointer to the file object to be unlocked.

    FileOffset - Starting byte offset from the base of the file to be
        unlocked.

    Length - Length of the byte range to be unlocked.

    ProcessId - ID of the process requesting the unlock operation.

    Key - Lock key associated with the file lock.

    IoStatus - Pointer to a variable to receive the I/O status of the
        operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.

--*/

{
    BOOLEAN             retval = FALSE;
    PDEVICE_OBJECT nextDeviceObject;
    PFAST_IO_DISPATCH fastIoDispatch;
    CHAR                name[PROCNAMELEN];
	CONVERT_BUF			MatchInfo = {0};

    PAGED_CODE();
    ASSERT(KeGetCurrentIrql() <= APC_LEVEL);

    if (DeviceObject->DeviceExtension) {

        ASSERT(IS_MY_DEVICE_OBJECT( DeviceObject ));

        //
        //  Pass through logic for this type of Fast I/O
        //

        nextDeviceObject = ((PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension)->FileSystem;
        ASSERT(nextDeviceObject);

        fastIoDispatch = nextDeviceObject->DriverObject->FastIoDispatch;

        if (VALID_FAST_IO_DISPATCH_HANDLER( fastIoDispatch, FastIoUnlockSingle )) {

            retval = (fastIoDispatch->FastIoUnlockSingle)(
                        FileObject,
                        FileOffset,
                        Length,
                        ProcessId,
                        Key,
                        IoStatus,
                        nextDeviceObject );
			if(YGFSMonGetProcess(name) && YGFSMonGetFullPath(FileObject,(PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension,&MatchInfo))
			{
				KdPrint(( "%s\tFASTIO_UNLOCK\t%x:%s\tOffset: %d Length: %d\t%x\n", 
							 name,(ULONG)FileObject,MatchInfo.szMonitorFile, 
							 FileOffset? FileOffset->LowPart : 0, Length ? Length->LowPart : 0, retval));
			}
        }
    }
    return retval;
}


BOOLEAN
YGFSMonFastIoUnlockAll (
    IN PFILE_OBJECT FileObject,
    PEPROCESS ProcessId,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for unlocking all
    locks within a file.

    This function simply invokes the file system's corresponding routine, or
    returns FALSE if the file system does not implement the function.

Arguments:

    FileObject - Pointer to the file object to be unlocked.

    ProcessId - ID of the process requesting the unlock operation.

    IoStatus - Pointer to a variable to receive the I/O status of the
        operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.

--*/

{
    BOOLEAN             retval = FALSE;
    PDEVICE_OBJECT nextDeviceObject;
    PFAST_IO_DISPATCH fastIoDispatch;
    CHAR                name[PROCNAMELEN];
	CONVERT_BUF			MatchInfo = {0};

    PAGED_CODE();
    ASSERT(KeGetCurrentIrql() <= APC_LEVEL);

    if (DeviceObject->DeviceExtension) {

        ASSERT(IS_MY_DEVICE_OBJECT( DeviceObject ));

        //
        //  Pass through logic for this type of Fast I/O
        //

        nextDeviceObject = ((PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension)->FileSystem;

        if (nextDeviceObject) {

            fastIoDispatch = nextDeviceObject->DriverObject->FastIoDispatch;

            if (VALID_FAST_IO_DISPATCH_HANDLER( fastIoDispatch, FastIoUnlockAll )) {

                retval = (fastIoDispatch->FastIoUnlockAll)(
                            FileObject,
                            ProcessId,
                            IoStatus,
                            nextDeviceObject );
				if(YGFSMonGetProcess(name) && YGFSMonGetFullPath(FileObject,(PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension,&MatchInfo))
				{
					KdPrint(( "%s\tFASTIO_UNLOCK_ALL\t%x:%s\t\t%x\n", 
								 name,(ULONG)FileObject,MatchInfo.szMonitorFile, retval));
				}
            }
        }
    }
    return retval;
}


BOOLEAN
YGFSMonFastIoUnlockAllByKey (
    IN PFILE_OBJECT FileObject,
    PVOID ProcessId,
    ULONG Key,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for unlocking all
    locks within a file based on a specified key.

    This function simply invokes the file system's corresponding routine, or
    returns FALSE if the file system does not implement the function.

Arguments:

    FileObject - Pointer to the file object to be unlocked.

    ProcessId - ID of the process requesting the unlock operation.

    Key - Lock key associated with the locks on the file to be released.

    IoStatus - Pointer to a variable to receive the I/O status of the
        operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.

--*/

{
    BOOLEAN             retval = FALSE;
    PDEVICE_OBJECT nextDeviceObject;
    PFAST_IO_DISPATCH fastIoDispatch;
    CHAR                name[PROCNAMELEN];
	CONVERT_BUF			MatchInfo = {0};

    PAGED_CODE();
    ASSERT(KeGetCurrentIrql() <= APC_LEVEL);

    if (DeviceObject->DeviceExtension) {

        ASSERT(IS_MY_DEVICE_OBJECT( DeviceObject ));

        //
        //  Pass through logic for this type of Fast I/O
        //

        nextDeviceObject = ((PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension)->FileSystem;
        ASSERT(nextDeviceObject);

        fastIoDispatch = nextDeviceObject->DriverObject->FastIoDispatch;

        if (VALID_FAST_IO_DISPATCH_HANDLER( fastIoDispatch, FastIoUnlockAllByKey )) {

            retval = (fastIoDispatch->FastIoUnlockAllByKey)(
                        FileObject,
                        ProcessId,
                        Key,
                        IoStatus,
                        nextDeviceObject );
			if(YGFSMonGetProcess(name) && YGFSMonGetFullPath(FileObject,(PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension,&MatchInfo))
			{
				KdPrint(( "%s\tFASTIO_UNLOCK_ALL_BY_KEY\t%x:%s\t\t%x\n", 
							 name,(ULONG)FileObject,MatchInfo.szMonitorFile, retval));
			}
        }
    }
    return retval;
}

BOOLEAN
YGFSMonFastIoDeviceControl (
    IN PFILE_OBJECT FileObject,
    IN BOOLEAN Wait,
    IN PVOID InputBuffer OPTIONAL,
    IN ULONG InputBufferLength,
    OUT PVOID OutputBuffer OPTIONAL,
    IN ULONG OutputBufferLength,
    IN ULONG IoControlCode,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for device I/O control
    operations on a file.

    This function simply invokes the file system's corresponding routine, or
    returns FALSE if the file system does not implement the function.

Arguments:

    FileObject - Pointer to the file object representing the device to be
        serviced.

    Wait - Indicates whether or not the caller is willing to wait if the
        appropriate locks, etc. cannot be acquired

    InputBuffer - Optional pointer to a buffer to be passed into the driver.

    InputBufferLength - Length of the optional InputBuffer, if one was
        specified.

    OutputBuffer - Optional pointer to a buffer to receive data from the
        driver.

    OutputBufferLength - Length of the optional OutputBuffer, if one was
        specified.

    IoControlCode - I/O control code indicating the operation to be performed
        on the device.

    IoStatus - Pointer to a variable to receive the I/O status of the
        operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.

--*/

{
    BOOLEAN             retval = FALSE;
	ULONG               i,k;
    PDEVICE_OBJECT nextDeviceObject;
    PFAST_IO_DISPATCH fastIoDispatch;
    CHAR                name[PROCNAMELEN];
	CONVERT_BUF			MatchInfo = {0};

    PAGED_CODE();
    ASSERT(KeGetCurrentIrql() <= APC_LEVEL);

    if ( DeviceObject == g_YGFSMonControlDeviceObject )  
	{

        IoStatus->Status      = STATUS_SUCCESS; // Assume success
        IoStatus->Information = 0;      // Assume nothing returned

        switch ( IoControlCode ) 
		{
        case IOCTL_YGFSMON_GET_VERSION_NT:
            if ( OutputBufferLength < sizeof(ULONG) ||
                 OutputBuffer == NULL ) 
			{
                IoStatus->Status = STATUS_INVALID_PARAMETER;
                break;
            }
            
            *(ULONG *)OutputBuffer = YGFSMON_VERSION_MAJOR*0x10000+YGFSMON_VERSION_MINOR;
            IoStatus->Information = sizeof(ULONG);
            break;
        case IOCTL_YGFSMON_START_WORK_NT:
            if ( InputBufferLength || OutputBufferLength) 
			{
                IoStatus->Status = STATUS_INVALID_PARAMETER;
                break;
            }
			bStartWork = TRUE;
            break;
        case IOCTL_YGFSMON_STOP_WORK_NT:
            if ( InputBufferLength || OutputBufferLength) 
			{
                IoStatus->Status = STATUS_INVALID_PARAMETER;
                break;
            }
			bStartWork = FALSE;
            break;
        case IOCTL_YGFSMON_CLEAR_STATE_NT:
            if ( InputBufferLength || OutputBufferLength) 
			{
                IoStatus->Status = STATUS_INVALID_PARAMETER;
                break;
            }
			YGFSMonHashCleanup();
			YGFSMonAddExcludeProcess("System");
            break;
        case IOCTL_YGFSMON_ADD_EXCLUDE_PROCESS_NT:
            if (InputBuffer == NULL ) 
			{
                IoStatus->Status = STATUS_INVALID_PARAMETER;
                break;
            }
			YGFSMonAddExcludeProcess((PCHAR)InputBuffer);
            break;
        case IOCTL_YGFSMON_REMOVE_EXCLUDE_PROCESS_NT:
            if (InputBuffer == NULL ) 
			{
                IoStatus->Status = STATUS_INVALID_PARAMETER;
                break;
            }
			YGFSMonRemoveExcludeProcess((PCHAR)InputBuffer);
            break;
        case IOCTL_YGFSMON_ADD_EXCLUDE_PATH_NT:
            if (InputBuffer == NULL ) 
			{
                IoStatus->Status = STATUS_INVALID_PARAMETER;
                break;
            }
			YGFSMonAddExcludePath((PCHAR)InputBuffer);
            break;
        case IOCTL_YGFSMON_REMOVE_EXCLUDE_PATH_NT:
            if (InputBuffer == NULL ) 
			{
                IoStatus->Status = STATUS_INVALID_PARAMETER;
                break;
            }
			YGFSMonRemoveExcludePath((PCHAR)InputBuffer);
            break;
        case IOCTL_YGFSMON_ADD_MONITOR_ITEM_NT:
            if (InputBufferLength != sizeof(CONVERT_BUF) || InputBuffer == NULL ) 
			{
                IoStatus->Status = STATUS_INVALID_PARAMETER;
                break;
            }
			YGFSMonAddMonitorItem((PCONVERT_BUF)InputBuffer);
            break;
        case IOCTL_YGFSMON_REMOVE_MONITOR_ITEM_NT:
            if (InputBufferLength != sizeof(CONVERT_BUF) || InputBuffer == NULL ) 
			{
                IoStatus->Status = STATUS_INVALID_PARAMETER;
                break;
            }
			YGFSMonRemoveMonitorItem((PCONVERT_BUF)InputBuffer);
            break;
		case IOCTL_YGFSMON_GET_MODIFY_FILE_NT:
            if (OutputBufferLength != sizeof(CONVERT_BUF) || OutputBuffer == NULL ) 
			{
                IoStatus->Status = STATUS_INVALID_PARAMETER;
                break;
            }
			YGFSMonGetOldestRecord((PCONVERT_BUF)OutputBuffer);
	        IoStatus->Information = sizeof(CONVERT_BUF);
            break;
		case IOCTL_YGFSMON_GET_MONITOR_FILE_INFO_NT:
            if (OutputBufferLength != sizeof(CONVERT_BUF) || OutputBuffer == NULL ) 
			{
                IoStatus->Status = STATUS_INVALID_PARAMETER;
                break;
            }
            if (InputBufferLength != sizeof(CONVERT_BUF) || InputBuffer == NULL ) 
			{
                IoStatus->Status = STATUS_INVALID_PARAMETER;
                break;
            }
			if (GetMonitorFileInfo(((PCONVERT_BUF)InputBuffer)->szMonitorFile,(PCONVERT_BUF)OutputBuffer) == MATCH_INCLUDE_PATH)
		        IoStatus->Information = sizeof(CONVERT_BUF);
			else IoStatus->Status = STATUS_INVALID_PARAMETER;
            break;
		case IOCTL_YGFSMON_GET_MONITOR_DRIVE_NT:
            if (OutputBufferLength < 26 || OutputBuffer == NULL ) 
			{
                IoStatus->Status = STATUS_INVALID_PARAMETER;
                break;
            }
			k = 0;
			for (i = 0;i < 26;i++)
			{
				if (FsVolTable[i].FileSystem)
				{
					((PCHAR)OutputBuffer)[k] = (UCHAR)i+'A';
					k++;
				}
			}
			((PCHAR)OutputBuffer)[k] = 0;
	        IoStatus->Information = OutputBufferLength;
            break;
        default:

            //
            // Unknown control
            // 
            DbgPrint (("YGFSMon: unknown IRP_MJ_DEVICE_CONTROL\n"));
            IoStatus->Status = STATUS_INVALID_DEVICE_REQUEST;
            break;
        }
        retval = TRUE;
	}
	else
	{
	    if (DeviceObject->DeviceExtension) {

			   //
			//  Pass through logic for this type of Fast I/O
			//

			nextDeviceObject = ((PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension)->FileSystem;
			ASSERT(nextDeviceObject);

			fastIoDispatch = nextDeviceObject->DriverObject->FastIoDispatch;

			if (VALID_FAST_IO_DISPATCH_HANDLER( fastIoDispatch, FastIoDeviceControl )) {

				retval = (fastIoDispatch->FastIoDeviceControl)(
							FileObject,
							Wait,
							InputBuffer,
							InputBufferLength,
							OutputBuffer,
							OutputBufferLength,
							IoControlCode,
							IoStatus,
							nextDeviceObject );
				if(YGFSMonGetProcess(name) && YGFSMonGetFullPath(FileObject,(PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension,&MatchInfo))
				{
					 KdPrint(("%s\tFASTIO_DEVICE_CONTROL\t%x:%s\tIOCTL: 0x%X\t%x\n", 
								 name,(ULONG)FileObject,MatchInfo.szMonitorFile,IoControlCode, retval));
				}
            }
        }
    }
    return retval;
}


VOID
YGFSMonFastIoDetachDevice (
    IN PDEVICE_OBJECT SourceDevice,
    IN PDEVICE_OBJECT TargetDevice
    )

/*++

Routine Description:

    This routine is invoked on the fast path to detach from a device that
    is being deleted.  This occurs when this driver has attached to a file
    system volume device object, and then, for some reason, the file system
    decides to delete that device (it is being dismounted, it was dismounted
    at some point in the past and its last reference has just gone away, etc.)

Arguments:

    SourceDevice - Pointer to my device object, which is attached
        to the file system's volume device object.

    TargetDevice - Pointer to the file system's volume device object.

Return Value:

    None

--*/

{
    CHAR                name[PROCNAMELEN];
    PYGFS_HOOK_EXTENSION devExt;

    PAGED_CODE();
    ASSERT(KeGetCurrentIrql() <= APC_LEVEL);

    ASSERT(IS_MY_DEVICE_OBJECT( SourceDevice ));

    devExt = SourceDevice->DeviceExtension;

    //
    //  Display name information
    //

    KdPrint(("YGFSMonFastIoDetachDevice:                Detaching from volume      %p \"%wZ\"\n",
                   TargetDevice,
				   devExt->DeviceName) );

    //
    //  Detach from the file system's volume device object.
    //

    YGFSMonCleanupMountedDevice( SourceDevice );
    if( YGFSMonGetProcess( name ) ) {

        KdPrint(("%s\tFASTIO_DETACH_DEVICE\t\t\tOK\n", name));
    }
    IoDetachDevice( TargetDevice );
    IoDeleteDevice( SourceDevice );
}


BOOLEAN
YGFSMonFastIoQueryNetworkOpenInfo (
    IN PFILE_OBJECT FileObject,
    IN BOOLEAN Wait,
    OUT PFILE_NETWORK_OPEN_INFORMATION Buffer,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for querying network
    information about a file.

    This function simply invokes the file system's corresponding routine, or
    returns FALSE if the file system does not implement the function.

Arguments:

    FileObject - Pointer to the file object to be queried.

    Wait - Indicates whether or not the caller can handle the file system
        having to wait and tie up the current thread.

    Buffer - Pointer to a buffer to receive the network information about the
        file.

    IoStatus - Pointer to a variable to receive the final status of the query
        operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.

--*/

{
    BOOLEAN             retval = FALSE;
    PDEVICE_OBJECT nextDeviceObject;
    PFAST_IO_DISPATCH fastIoDispatch;
    CHAR                name[PROCNAMELEN];
	CONVERT_BUF			MatchInfo = {0};

	PYGFS_HOOK_EXTENSION hookExt = DeviceObject->DeviceExtension;
    PAGED_CODE();
    ASSERT(KeGetCurrentIrql() <= APC_LEVEL);

    if (DeviceObject->DeviceExtension) {

        ASSERT(IS_MY_DEVICE_OBJECT( DeviceObject ));

        //
        //  Pass through logic for this type of Fast I/O
        //

        nextDeviceObject = hookExt->FileSystem;
        ASSERT(nextDeviceObject);

        fastIoDispatch = nextDeviceObject->DriverObject->FastIoDispatch;

        if (VALID_FAST_IO_DISPATCH_HANDLER( fastIoDispatch, FastIoQueryNetworkOpenInfo )) {

            retval = (fastIoDispatch->FastIoQueryNetworkOpenInfo)(
                        FileObject,
                        Wait,
                        Buffer,
                        IoStatus,
                        nextDeviceObject );
			if(YGFSMonGetProcess(name) && YGFSMonGetFullPath(FileObject,hookExt,&MatchInfo))
			{
				KdPrint(( "%s\tFASTIO_QUERY_NETWORK_OPEN_INFO\t%x:%s\t\t%x\n", 
							 name,(ULONG)FileObject,MatchInfo.szMonitorFile, retval));
			}
        }
    }
    return retval;
}


BOOLEAN
YGFSMonFastIoMdlRead (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    IN ULONG LockKey,
    OUT PMDL *MdlChain,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for reading a file
    using MDLs as buffers.

    This function simply invokes the file system's corresponding routine, or
    returns FALSE if the file system does not implement the function.

Arguments:

    FileObject - Pointer to the file object that is to be read.

    FileOffset - Supplies the offset into the file to begin the read operation.

    Length - Specifies the number of bytes to be read from the file.

    LockKey - The key to be used in byte range lock checks.

    MdlChain - A pointer to a variable to be filled in w/a pointer to the MDL
        chain built to describe the data read.

    IoStatus - Variable to receive the final status of the read operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.

--*/

{
    BOOLEAN             retval = FALSE;
    PDEVICE_OBJECT nextDeviceObject;
    PFAST_IO_DISPATCH fastIoDispatch;
    CHAR                name[PROCNAMELEN];
	CONVERT_BUF			MatchInfo = {0};

    PAGED_CODE();
    ASSERT(KeGetCurrentIrql() <= APC_LEVEL);

    if (DeviceObject->DeviceExtension) {

        ASSERT(IS_MY_DEVICE_OBJECT( DeviceObject ));

        //
        //  Pass through logic for this type of Fast I/O
        //

        nextDeviceObject = ((PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension)->FileSystem;
        ASSERT(nextDeviceObject);

        fastIoDispatch = nextDeviceObject->DriverObject->FastIoDispatch;

        if (VALID_FAST_IO_DISPATCH_HANDLER( fastIoDispatch, MdlRead )) {

            retval = (fastIoDispatch->MdlRead)(
                        FileObject,
                        FileOffset,
                        Length,
                        LockKey,
                        MdlChain,
                        IoStatus,
                        nextDeviceObject );
			if(YGFSMonGetProcess(name) && YGFSMonGetFullPath(FileObject,(PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension,&MatchInfo))
			{
				KdPrint(( "%s\tFASTIO_MDL_READ\t%x:%s\tOffset: %d Length: %d\t%x\n", 
							 name,(ULONG)FileObject,MatchInfo.szMonitorFile,FileOffset->LowPart, Length, retval));
			}
        }
    }
    return retval;
}


BOOLEAN
YGFSMonFastIoMdlReadComplete (
    IN PFILE_OBJECT FileObject,
    IN PMDL MdlChain,
    IN PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for completing an
    MDL read operation.

    This function simply invokes the file system's corresponding routine, if
    it has one.  It should be the case that this routine is invoked only if
    the MdlRead function is supported by the underlying file system, and
    therefore this function will also be supported, but this is not assumed
    by this driver.

Arguments:

    FileObject - Pointer to the file object to complete the MDL read upon.

    MdlChain - Pointer to the MDL chain used to perform the read operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE, depending on whether or not it is
    possible to invoke this function on the fast I/O path.

--*/

{
    BOOLEAN             retval = FALSE;
    PDEVICE_OBJECT nextDeviceObject;
    PFAST_IO_DISPATCH fastIoDispatch;
    CHAR                name[PROCNAMELEN];
	CONVERT_BUF			MatchInfo = {0};

    if (DeviceObject->DeviceExtension) {

        ASSERT(IS_MY_DEVICE_OBJECT( DeviceObject ));

        //
        //  Pass through logic for this type of Fast I/O
        //

        nextDeviceObject = ((PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension)->FileSystem;
        ASSERT(nextDeviceObject);

        fastIoDispatch = nextDeviceObject->DriverObject->FastIoDispatch;

        if (VALID_FAST_IO_DISPATCH_HANDLER( fastIoDispatch, MdlReadComplete )) {

            retval = (fastIoDispatch->MdlReadComplete)(
                        FileObject,
                        MdlChain,
                        nextDeviceObject );
			if(YGFSMonGetProcess(name) && YGFSMonGetFullPath(FileObject,(PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension,&MatchInfo))
			{
				KdPrint(( "%s\tFASTIO_MDL_READ_COMPLETE\t%x:%s\n", name,(ULONG)FileObject,MatchInfo.szMonitorFile));
			}
        }
    }
    return retval;
}


BOOLEAN
YGFSMonFastIoPrepareMdlWrite (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    IN ULONG LockKey,
    OUT PMDL *MdlChain,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for preparing for an
    MDL write operation.

    This function simply invokes the file system's corresponding routine, or
    returns FALSE if the file system does not implement the function.

Arguments:

    FileObject - Pointer to the file object that will be written.

    FileOffset - Supplies the offset into the file to begin the write operation.

    Length - Specifies the number of bytes to be write to the file.

    LockKey - The key to be used in byte range lock checks.

    MdlChain - A pointer to a variable to be filled in w/a pointer to the MDL
        chain built to describe the data written.

    IoStatus - Variable to receive the final status of the write operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.

--*/

{
    BOOLEAN             retval = FALSE;
    PDEVICE_OBJECT nextDeviceObject;
    PFAST_IO_DISPATCH fastIoDispatch;
    CHAR                name[PROCNAMELEN];
	CONVERT_BUF			MatchInfo = {0};

    PAGED_CODE();
    ASSERT(KeGetCurrentIrql() <= APC_LEVEL);

    if (DeviceObject->DeviceExtension) {

        ASSERT(IS_MY_DEVICE_OBJECT( DeviceObject ));

        //
        //  Pass through logic for this type of Fast I/O
        //

        nextDeviceObject = ((PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension)->FileSystem;
        ASSERT(nextDeviceObject);

        fastIoDispatch = nextDeviceObject->DriverObject->FastIoDispatch;

        if (VALID_FAST_IO_DISPATCH_HANDLER( fastIoDispatch, PrepareMdlWrite )) {

            retval = (fastIoDispatch->PrepareMdlWrite)(
                        FileObject,
                        FileOffset,
                        Length,
                        LockKey,
                        MdlChain,
                        IoStatus,
                        nextDeviceObject );
			if(YGFSMonGetProcess(name) && YGFSMonGetFullPath(FileObject,(PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension,&MatchInfo))
			{
				KdPrint(( "%s\tFASTIO_PREPARE_MDL_WRITE\t%x:%s\tOffset: %d Length: %d\t%x\n", 
							 name,(ULONG)FileObject,MatchInfo.szMonitorFile, FileOffset->LowPart, Length, retval));
			}
        }
    }
    return retval;
}


BOOLEAN
YGFSMonFastIoMdlWriteComplete (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN PMDL MdlChain,
    IN PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for completing an
    MDL write operation.

    This function simply invokes the file system's corresponding routine, if
    it has one.  It should be the case that this routine is invoked only if
    the PrepareMdlWrite function is supported by the underlying file system,
    and therefore this function will also be supported, but this is not
    assumed by this driver.

Arguments:

    FileObject - Pointer to the file object to complete the MDL write upon.

    FileOffset - Supplies the file offset at which the write took place.

    MdlChain - Pointer to the MDL chain used to perform the write operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE, depending on whether or not it is
    possible to invoke this function on the fast I/O path.

--*/

{
    BOOLEAN             retval = FALSE;
    PDEVICE_OBJECT nextDeviceObject;
    PFAST_IO_DISPATCH fastIoDispatch;
    CHAR                name[PROCNAMELEN];
	CONVERT_BUF			MatchInfo = {0};

    PAGED_CODE();
    ASSERT(KeGetCurrentIrql() <= APC_LEVEL);

    if (DeviceObject->DeviceExtension) {

        ASSERT(IS_MY_DEVICE_OBJECT( DeviceObject ));

        //
        //  Pass through logic for this type of Fast I/O
        //

        nextDeviceObject = ((PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension)->FileSystem;
        ASSERT(nextDeviceObject);

        fastIoDispatch = nextDeviceObject->DriverObject->FastIoDispatch;

        if (VALID_FAST_IO_DISPATCH_HANDLER( fastIoDispatch, MdlWriteComplete )) {

            retval = (fastIoDispatch->MdlWriteComplete)(
                        FileObject,
                        FileOffset,
                        MdlChain,
                        nextDeviceObject );
			if(YGFSMonGetProcess(name) && YGFSMonGetFullPath(FileObject,(PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension,&MatchInfo))
			{
				KdPrint(( "%s\tFASTIO_MDL_WRITE_COMPLETE\t%x:%s\tOffset: %d\tOK\n", 
							 name, (ULONG)FileObject,MatchInfo.szMonitorFile, FileOffset->LowPart ));
				YGFSMonSetWriteFlag((ULONG)FileObject,FALSE);
			}
        }
    }
    return retval;
}


/*********************************************************************************
        UNIMPLEMENTED FAST IO ROUTINES
        
        The following four Fast IO routines are for compression on the wire
        which is not yet implemented in NT.  
        
        NOTE:  It is highly recommended that you include these routines (which
               do a pass-through call) so your filter will not need to be
               modified in the future when this functionality is implemented in
               the OS.
        
        FastIoReadCompressed, FastIoWriteCompressed, 
        FastIoMdlReadCompleteCompressed, FastIoMdlWriteCompleteCompressed
**********************************************************************************/


BOOLEAN
YGFSMonFastIoReadCompressed (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    IN ULONG LockKey,
    OUT PVOID Buffer,
    OUT PMDL *MdlChain,
    OUT PIO_STATUS_BLOCK IoStatus,
    OUT struct _COMPRESSED_DATA_INFO *CompressedDataInfo,
    IN ULONG CompressedDataInfoLength,
    IN PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for reading compressed
    data from a file.

    This function simply invokes the file system's corresponding routine, or
    returns FALSE if the file system does not implement the function.

Arguments:

    FileObject - Pointer to the file object that will be read.

    FileOffset - Supplies the offset into the file to begin the read operation.

    Length - Specifies the number of bytes to be read from the file.

    LockKey - The key to be used in byte range lock checks.

    Buffer - Pointer to a buffer to receive the compressed data read.

    MdlChain - A pointer to a variable to be filled in w/a pointer to the MDL
        chain built to describe the data read.

    IoStatus - Variable to receive the final status of the read operation.

    CompressedDataInfo - A buffer to receive the description of the compressed
        data.

    CompressedDataInfoLength - Specifies the size of the buffer described by
        the CompressedDataInfo parameter.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.

--*/

{
    BOOLEAN             retval = FALSE;
    PDEVICE_OBJECT nextDeviceObject;
    PFAST_IO_DISPATCH fastIoDispatch;
    CHAR                name[PROCNAMELEN];
	CONVERT_BUF			MatchInfo = {0};

    PAGED_CODE();
    ASSERT(KeGetCurrentIrql() <= APC_LEVEL);

    if (DeviceObject->DeviceExtension) {

        ASSERT(IS_MY_DEVICE_OBJECT( DeviceObject ));

        //
        //  Pass through logic for this type of Fast I/O
        //

        nextDeviceObject = ((PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension)->FileSystem;
        ASSERT(nextDeviceObject);

        fastIoDispatch = nextDeviceObject->DriverObject->FastIoDispatch;

        if (VALID_FAST_IO_DISPATCH_HANDLER( fastIoDispatch, FastIoReadCompressed )) {

            retval = (fastIoDispatch->FastIoReadCompressed)(
                        FileObject,
                        FileOffset,
                        Length,
                        LockKey,
                        Buffer,
                        MdlChain,
                        IoStatus,
                        CompressedDataInfo,
                        CompressedDataInfoLength,
                        nextDeviceObject );
			if(YGFSMonGetProcess(name) && YGFSMonGetFullPath(FileObject,(PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension,&MatchInfo))
			{
				KdPrint(( "%s\tFASTIO_READ_COMPRESSED\t%x:%s\tOffset: %d Length: %d\t%x\n", 
							 name, (ULONG)FileObject,MatchInfo.szMonitorFile, FileOffset->LowPart, Length, retval));
			}
        }
    }
    return retval;
}


BOOLEAN
YGFSMonFastIoWriteCompressed (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    IN ULONG LockKey,
    IN PVOID Buffer,
    OUT PMDL *MdlChain,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN struct _COMPRESSED_DATA_INFO *CompressedDataInfo,
    IN ULONG CompressedDataInfoLength,
    IN PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for writing compressed
    data to a file.

    This function simply invokes the file system's corresponding routine, or
    returns FALSE if the file system does not implement the function.

Arguments:

    FileObject - Pointer to the file object that will be written.

    FileOffset - Supplies the offset into the file to begin the write operation.

    Length - Specifies the number of bytes to be write to the file.

    LockKey - The key to be used in byte range lock checks.

    Buffer - Pointer to the buffer containing the data to be written.

    MdlChain - A pointer to a variable to be filled in w/a pointer to the MDL
        chain built to describe the data written.

    IoStatus - Variable to receive the final status of the write operation.

    CompressedDataInfo - A buffer to containing the description of the
        compressed data.

    CompressedDataInfoLength - Specifies the size of the buffer described by
        the CompressedDataInfo parameter.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.

--*/

{
    BOOLEAN             retval = FALSE;
    PDEVICE_OBJECT nextDeviceObject;
    PFAST_IO_DISPATCH fastIoDispatch;
    CHAR                name[PROCNAMELEN];
	CONVERT_BUF			MatchInfo = {0};

    PAGED_CODE();
    ASSERT(KeGetCurrentIrql() <= APC_LEVEL);

    if (DeviceObject->DeviceExtension) {

        ASSERT(IS_MY_DEVICE_OBJECT( DeviceObject ));

        //
        //  Pass through logic for this type of Fast I/O
        //

        nextDeviceObject = ((PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension)->FileSystem;
        ASSERT(nextDeviceObject);

        fastIoDispatch = nextDeviceObject->DriverObject->FastIoDispatch;

        if (VALID_FAST_IO_DISPATCH_HANDLER( fastIoDispatch, FastIoWriteCompressed )) {

            retval = (fastIoDispatch->FastIoWriteCompressed)(
                        FileObject,
                        FileOffset,
                        Length,
                        LockKey,
                        Buffer,
                        MdlChain,
                        IoStatus,
                        CompressedDataInfo,
                        CompressedDataInfoLength,
                        nextDeviceObject );
			if(YGFSMonGetProcess(name) && YGFSMonGetFullPath(FileObject,(PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension,&MatchInfo))
			{
				KdPrint(( "%s\tFASTIO_WRITE_COMPRESSED\t%x:%s\tOffset: %d Length: %d\t%x\n", 
							 name,(ULONG)FileObject, MatchInfo.szMonitorFile, FileOffset->LowPart, Length, retval));
				YGFSMonSetWriteFlag((ULONG)FileObject,FALSE);
			}
        }
    }
    return retval;
}


BOOLEAN
YGFSMonFastIoMdlReadCompleteCompressed (
    IN PFILE_OBJECT FileObject,
    IN PMDL MdlChain,
    IN PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for completing an
    MDL read compressed operation.

    This function simply invokes the file system's corresponding routine, if
    it has one.  It should be the case that this routine is invoked only if
    the read compressed function is supported by the underlying file system,
    and therefore this function will also be supported, but this is not assumed
    by this driver.

Arguments:

    FileObject - Pointer to the file object to complete the compressed read
        upon.

    MdlChain - Pointer to the MDL chain used to perform the read operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE, depending on whether or not it is
    possible to invoke this function on the fast I/O path.

--*/

{
    BOOLEAN             retval = FALSE;
    PDEVICE_OBJECT nextDeviceObject;
    PFAST_IO_DISPATCH fastIoDispatch;
    CHAR                name[PROCNAMELEN];
	CONVERT_BUF			MatchInfo = {0};

    if (DeviceObject->DeviceExtension) {

        ASSERT(IS_MY_DEVICE_OBJECT( DeviceObject ));

        //
        //  Pass through logic for this type of Fast I/O
        //

        nextDeviceObject = ((PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension)->FileSystem;
        ASSERT(nextDeviceObject);

        fastIoDispatch = nextDeviceObject->DriverObject->FastIoDispatch;

        if (VALID_FAST_IO_DISPATCH_HANDLER( fastIoDispatch, MdlReadCompleteCompressed )) {

            retval = (fastIoDispatch->MdlReadCompleteCompressed)(
                        FileObject,
                        MdlChain,
                        nextDeviceObject );
			if(YGFSMonGetProcess(name) && YGFSMonGetFullPath(FileObject,(PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension,&MatchInfo))
			{
				KdPrint(("%s\tFASTIO_MDL_READ_COMPLETE_COMPRESSED\t%x:%s\n", 
							 name, (ULONG)FileObject,MatchInfo.szMonitorFile));
			}
        }
    }
    return retval;
}


BOOLEAN
YGFSMonFastIoMdlWriteCompleteCompressed (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN PMDL MdlChain,
    IN PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for completing a
    write compressed operation.

    This function simply invokes the file system's corresponding routine, if
    it has one.  It should be the case that this routine is invoked only if
    the write compressed function is supported by the underlying file system,
    and therefore this function will also be supported, but this is not assumed
    by this driver.

Arguments:

    FileObject - Pointer to the file object to complete the compressed write
        upon.

    FileOffset - Supplies the file offset at which the file write operation
        began.

    MdlChain - Pointer to the MDL chain used to perform the write operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE, depending on whether or not it is
    possible to invoke this function on the fast I/O path.

--*/

{
    BOOLEAN             retval = FALSE;
    PDEVICE_OBJECT nextDeviceObject;
    PFAST_IO_DISPATCH fastIoDispatch;
    CHAR                name[PROCNAMELEN];
	CONVERT_BUF			MatchInfo = {0};
			
    if (DeviceObject->DeviceExtension) {

        ASSERT(IS_MY_DEVICE_OBJECT( DeviceObject ));

        //
        //  Pass through logic for this type of Fast I/O
        //

        nextDeviceObject = ((PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension)->FileSystem;
        ASSERT(nextDeviceObject);

        fastIoDispatch = nextDeviceObject->DriverObject->FastIoDispatch;

        if (VALID_FAST_IO_DISPATCH_HANDLER( fastIoDispatch, MdlWriteCompleteCompressed )) {

            retval = (fastIoDispatch->MdlWriteCompleteCompressed)(
                        FileObject,
                        FileOffset,
                        MdlChain,
                        nextDeviceObject );
			if(YGFSMonGetProcess(name) && YGFSMonGetFullPath(FileObject,(PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension,&MatchInfo))
			{
				KdPrint(("%s\tFASTIO_MDL_WRITE_COMPLETE_COMPRESSED\t%x:%s\tOffset: %d\n", 
							 name,(ULONG)FileObject, MatchInfo.szMonitorFile, FileOffset->LowPart));
				YGFSMonSetWriteFlag((ULONG)FileObject,FALSE);
			}
        }
    }
    return retval;
}


BOOLEAN
YGFSMonFastIoQueryOpen (
    IN PIRP Irp,
    OUT PFILE_NETWORK_OPEN_INFORMATION NetworkInformation,
    IN PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for opening a file
    and returning network information for it.

    This function simply invokes the file system's corresponding routine, or
    returns FALSE if the file system does not implement the function.

Arguments:

    Irp - Pointer to a create IRP that represents this open operation.  It is
        to be used by the file system for common open/create code, but not
        actually completed.

    NetworkInformation - A buffer to receive the information required by the
        network about the file being opened.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.

--*/

{
    PDEVICE_OBJECT nextDeviceObject;
    PFAST_IO_DISPATCH fastIoDispatch;
    PFILE_OBJECT        fileobject;
    BOOLEAN retval = FALSE;
    CHAR                name[PROCNAMELEN];
	CONVERT_BUF			MatchInfo = {0};
    PIO_STACK_LOCATION  irpSp;

    PAGED_CODE();
    ASSERT(KeGetCurrentIrql() <= APC_LEVEL);

    if (DeviceObject->DeviceExtension) {

        ASSERT(IS_MY_DEVICE_OBJECT( DeviceObject ));

        //
        //  Pass through logic for this type of Fast I/O
        //

        nextDeviceObject = ((PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension)->FileSystem;
        ASSERT(nextDeviceObject);

        fastIoDispatch = nextDeviceObject->DriverObject->FastIoDispatch;

        if (VALID_FAST_IO_DISPATCH_HANDLER( fastIoDispatch, FastIoQueryOpen )) {

            irpSp = IoGetCurrentIrpStackLocation( Irp );
			fileobject      = irpSp->FileObject;

            irpSp->DeviceObject = nextDeviceObject;

            retval = (fastIoDispatch->FastIoQueryOpen)(
                        Irp,
                        NetworkInformation,
                        nextDeviceObject );

            if (!retval) {

                irpSp->DeviceObject = DeviceObject;
            }
			if(YGFSMonGetProcess(name) && YGFSMonGetFullPath(fileobject,(PYGFS_HOOK_EXTENSION) DeviceObject->DeviceExtension,&MatchInfo))
			{
				KdPrint(( "%s\tFASTIO_QUERY_OPEN\t%x:%s\t\t%x\n", 
							 name, (ULONG)fileobject,MatchInfo.szMonitorFile, retval));
			}
        }
    }
    return retval;
}


#ifdef WINXP
/////////////////////////////////////////////////////////////////////////////
//
//                  FYGFSMon callback handling routines
//
/////////////////////////////////////////////////////////////////////////////

NTSTATUS
YGFSMonPreFsFilterPassThrough (
    IN PFS_FILTER_CALLBACK_DATA Data,
    OUT PVOID *CompletionContext
    )
/*++

Routine Description:

    This routine is the FS Filter pre-operation "pass through" routine.

Arguments:

    Data - The FS_FILTER_CALLBACK_DATA structure containing the information
        about this operation.
        
    CompletionContext - A context set by this operation that will be passed
        to the corresponding YGFSMonPostFsFilterOperation call.
        
Return Value:

    Returns STATUS_SUCCESS if the operation can continue or an appropriate
    error code if the operation should fail.

--*/
{
    PAGED_CODE();
    ASSERT(KeGetCurrentIrql() <= APC_LEVEL);

    UNREFERENCED_PARAMETER( Data );
    UNREFERENCED_PARAMETER( CompletionContext );

    return STATUS_SUCCESS;
}

VOID
YGFSMonPostFsFilterPassThrough (
    IN PFS_FILTER_CALLBACK_DATA Data,
    IN NTSTATUS OperationStatus,
    IN PVOID CompletionContext
    )
/*++

Routine Description:

    This routine is the FS Filter post-operation "pass through" routine.

Arguments:

    Data - The FS_FILTER_CALLBACK_DATA structure containing the information
        about this operation.
        
    OperationStatus - The status of this operation.        
    
    CompletionContext - A context that was set in the pre-operation 
        callback by this driver.
        
Return Value:

    None.
    
--*/
{
    ASSERT(KeGetCurrentIrql() <= APC_LEVEL);

    UNREFERENCED_PARAMETER( Data );
    UNREFERENCED_PARAMETER( OperationStatus );
    UNREFERENCED_PARAMETER( CompletionContext );

}
#endif

/////////////////////////////////////////////////////////////////////////////
//
//                  Support routines
//
/////////////////////////////////////////////////////////////////////////////


NTSTATUS
YGFSMonAttachToFileSystemDevice (
    IN PDEVICE_OBJECT DeviceObject
    )
/*++

Routine Description:

    This will attach to the given file system device object.  We attach to
    these devices so we will know when new volumes are mounted.

Arguments:

    DeviceObject - The device to attach to

    Name - An already initialized unicode string used to retrieve names.
           This is passed in to reduce the number of strings buffers on
           the stack.

Return Value:

    Status of the operation

--*/
{
    PDEVICE_OBJECT newDeviceObject;
    PYGFS_HOOK_EXTENSION devExt;
    NTSTATUS status;
    UNICODE_STRING fsrecName;
    UNICODE_STRING tempName;
    WCHAR tempNameBuffer[MAX_DEVNAME_LENGTH];

    PAGED_CODE();

    //
    //  See if this is a file system type we care about.  If not, return.
    //

    if (!IS_DESIRED_DEVICE_TYPE(DeviceObject->DeviceType)) {

        return STATUS_SUCCESS;
    }

    RtlInitEmptyUnicodeString( &tempName,
                               tempNameBuffer,
                               sizeof(tempNameBuffer) );
    RtlInitUnicodeString( &fsrecName, L"\\FileSystem\\Fs_Rec" );
    YGFSMonGetObjectName( DeviceObject, &tempName );
    YGFSMonGetObjectName( DeviceObject->DriverObject, &tempName );

    if (RtlCompareUnicodeString( &tempName, &fsrecName, TRUE ) == 0) {

        return STATUS_SUCCESS;
    }

    //
    //  We want to attach to this file system.  Create a new device object we
    //  can attach with.
    //

    status = IoCreateDevice( g_YGFSMonDriverObject,
                             sizeof( YGFS_HOOK_EXTENSION ),
                             NULL,
                             DeviceObject->DeviceType,
                             0,
                             FALSE,
                             &newDeviceObject );

    if (!NT_SUCCESS( status )) {

        return status;
    }

    //
    //  Propagate flags from Device Object we are trying to attach to.
    //  Note that we do this before the actual attachment to make sure
    //  the flags are properly set once we are attached (since an IRP
    //  can come in immediately after attachment but before the flags would
    //  be set).
    //

    if ( FlagOn( DeviceObject->Flags, DO_BUFFERED_IO )) {

        SetFlag( newDeviceObject->Flags, DO_BUFFERED_IO );
    }

    if ( FlagOn( DeviceObject->Flags, DO_DIRECT_IO )) {

        SetFlag( newDeviceObject->Flags, DO_DIRECT_IO );
    }

    //
    //  Do the attachment
    //

    devExt = newDeviceObject->DeviceExtension;
	if (DeviceObject->DeviceType == FILE_DEVICE_NETWORK_FILE_SYSTEM) devExt->LogicalDrive = '#';
	devExt->DeviceType = DeviceObject->DeviceType;
    RtlInitEmptyUnicodeString( &devExt->DeviceName,
                               devExt->DeviceNameBuffer,
                               sizeof(devExt->DeviceNameBuffer) );
	devExt->DiskSystem = DeviceObject;
    YGFSMonGetObjectName( devExt->DiskSystem,&devExt->DeviceName);

    devExt->FileSystem = IoAttachDeviceToDeviceStack( newDeviceObject, 
                                          devExt->DiskSystem);

    if (!devExt->FileSystem) {

        goto ErrorCleanupDevice;
    }

    //
    //  Mark we are done initializing
    //

    ClearFlag( newDeviceObject->Flags, DO_DEVICE_INITIALIZING );

    //
    //  Display who we have attached to
    //

    KdPrint(("YGFSMonAttachToFileSystemDevice:          Attaching to file system   %p \"%wZ\" \n",
                   DeviceObject,&devExt->DeviceName) );

    //
    //  Enumerate all the mounted devices that currently
    //  exist for this file system and attach to them.
    //

    status = YGFSMonEnumerateFileSystemVolumes( DeviceObject);

    if (!NT_SUCCESS( status )) {

        goto ErrorCleanupAttachment;
    }

    return STATUS_SUCCESS;

    /////////////////////////////////////////////////////////////////////
    //                  Cleanup error handling
    /////////////////////////////////////////////////////////////////////

    ErrorCleanupAttachment:
        IoDetachDevice( devExt->FileSystem );

    ErrorCleanupDevice:
        YGFSMonCleanupMountedDevice( newDeviceObject );
        IoDeleteDevice( newDeviceObject );

    return status;
}


VOID
YGFSMonDetachFromFileSystemDevice (
    IN PDEVICE_OBJECT DeviceObject
    )
/*++

Routine Description:

    Given a base file system device object, this will scan up the attachment
    chain looking for our attached device object.  If found it will detach
    us from the chain.

Arguments:

    DeviceObject - The file system device to detach from.

Return Value:

--*/ 
{
    PDEVICE_OBJECT ourAttachedDevice;
    PYGFS_HOOK_EXTENSION devExt;

    PAGED_CODE();

    //
    //  Skip the base file system device object (since it can't be us)
    //

    ourAttachedDevice = DeviceObject->AttachedDevice;

    while (NULL != ourAttachedDevice) {

        if (IS_MY_DEVICE_OBJECT( ourAttachedDevice )) {

            devExt = ourAttachedDevice->DeviceExtension;

            //
            //  Display who we detached from
            //

            KdPrint(("YGFSMonDetachFromFileSystemDevice:        Detaching from file system %p \"%wZ\"\n",
                           devExt->FileSystem,
						   devExt->DeviceName) );

            //
            //  Detach us from the object just below us
            //  Cleanup and delete the object
            //

            YGFSMonCleanupMountedDevice( ourAttachedDevice );
            IoDetachDevice( DeviceObject );
            IoDeleteDevice( ourAttachedDevice );

            return;
        }

        //
        //  Look at the next device up in the attachment chain
        //

        DeviceObject = ourAttachedDevice;
        ourAttachedDevice = ourAttachedDevice->AttachedDevice;
    }
}

NTSTATUS
YGFSMonEnumerateFileSystemVolumes (
    IN PDEVICE_OBJECT FSDeviceObject
    ) 
/*++

Routine Description:

    Enumerate all the mounted devices that currently exist for the given file
    system and attach to them.  We do this because this filter could be loaded
    at any time and there might already be mounted volumes for this file system.

Arguments:

    FSDeviceObject - The device object for the file system we want to enumerate

    Name - An already initialized unicode string used to retrieve names
           This is passed in to reduce the number of strings buffers on
           the stack.

Return Value:

    The status of the operation

--*/
{
    PDEVICE_OBJECT newDeviceObject;
    PYGFS_HOOK_EXTENSION newDevExt;
    PDEVICE_OBJECT *devList;
    NTSTATUS status;
    ULONG numDevices;
    ULONG i;

    PAGED_CODE();

    //
    //  Find out how big of an array we need to allocate for the
    //  mounted device list.
    //

    status = IoEnumerateDeviceObjectList(
                    FSDeviceObject->DriverObject,
                    NULL,
                    0,
                    &numDevices);

    //
    //  We only need to get this list of there are devices.  If we
    //  don't get an error there are no devices so go on.
    //

    if (!NT_SUCCESS( status )) {

        //
        //  Allocate memory for the list of known devices
        //

        numDevices += 8;        //grab a few extra slots

        devList = ExAllocatePoolWithTag( NonPagedPool, 
                                         (numDevices * sizeof(PDEVICE_OBJECT)), 
                                         YGFS_POOL_TAG );
        if (NULL == devList) {

            return STATUS_INSUFFICIENT_RESOURCES;
        }

        //
        //  Now get the list of devices.  If we get an error again
        //  something is wrong, so just fail.
        //

        status = IoEnumerateDeviceObjectList(
                        FSDeviceObject->DriverObject,
                        devList,
                        (numDevices * sizeof(PDEVICE_OBJECT)),
                        &numDevices);

        if (!NT_SUCCESS( status ))  {

            ExFreePool( devList );
            return status;
        }

        //
        //  Walk the given list of devices and attach to them if we should.
        //

        for (i=0; i < numDevices; i++) {

            //
            //  Do not attach if:
            //      - This is the control device object (the one passed in)
            //      - We are already attached to it
            //

            if ((devList[i] != FSDeviceObject) && IS_DESIRED_DEVICE_TYPE(devList[i]->DeviceType) &&
                !YGFSMonIsAttachedToDevice( devList[i], NULL )) {


                //
                //  Allocate a new device object to attach with
                //

                status = IoCreateDevice( g_YGFSMonDriverObject,
                                         sizeof( YGFS_HOOK_EXTENSION ),
                                         NULL,
                                         devList[i]->DeviceType,
                                         0,
                                         FALSE,
                                         &newDeviceObject );

                if (NT_SUCCESS( status )) {

                    //
                    //  Set disk device object
                    //

                    newDevExt = newDeviceObject->DeviceExtension;
                    
                    //
                    //  Attach to volume.
                    //
					newDevExt->LogicalDrive = YGFSMonGetDriverLetter(devList[i]);
					newDevExt->DeviceType = devList[i]->DeviceType;
					RtlInitEmptyUnicodeString( &newDevExt->DeviceName,
											   newDevExt->DeviceNameBuffer,
											   sizeof(newDevExt->DeviceNameBuffer) );
					newDevExt->DiskSystem = devList[i];
					YGFSMonGetObjectName(newDevExt->DiskSystem,&newDevExt->DeviceName);
                    status = YGFSMonAttachToMountedDevice( newDevExt->DiskSystem, 
		                                                  newDeviceObject );
                    if (!NT_SUCCESS( status )) { 

                        //
                        //  The attachment failed, cleanup.  Note that
                        //  we continue processing so we will cleanup
                        //  the reference counts and try to attach to
                        //  the rest of the volumes.
                        //

                        YGFSMonCleanupMountedDevice( newDeviceObject );
                        IoDeleteDevice( newDeviceObject );
                    }
                } 
            }
#ifdef WINXP
            //
            //  Dereference the object (reference added by 
            //  IoEnumerateDeviceObjectList)
            //

            ObDereferenceObject( devList[i] );
#endif
        }

        //
        //  We are going to ignore any errors received while mounting.  We
        //  simply won't be attached to those volumes if we get an error
        //

        status = STATUS_SUCCESS;

        //
        //  Free the memory we allocated for the list
        //

        ExFreePool( devList );
    }

    return status;
}


NTSTATUS
YGFSMonAttachToMountedDevice (
    IN PDEVICE_OBJECT DeviceObject,
    IN PDEVICE_OBJECT YGFSMonDeviceObject
    )
/*++

Routine Description:

    This will attach to a DeviceObject that represents a mounted volume.

Arguments:

    DeviceObject - The device to attach to

    YGFSMonDeviceObject - Our device object we are going to attach

Return Value:

    Status of the operation

--*/
{        
    PYGFS_HOOK_EXTENSION newDevExt = YGFSMonDeviceObject->DeviceExtension;

    PAGED_CODE();

    //
    //  Propagate flags from Device Object we are trying to attach to.
    //  Note that we do this before the actual attachment to make sure
    //  the flags are properly set once we are attached (since an IRP
    //  can come in immediately after attachment but before the flags would
    //  be set).
    //

    if (FlagOn( DeviceObject->Flags, DO_BUFFERED_IO )) {

        SetFlag( YGFSMonDeviceObject->Flags, DO_BUFFERED_IO );
    }

    if (FlagOn( DeviceObject->Flags, DO_DIRECT_IO )) {

        SetFlag( YGFSMonDeviceObject->Flags, DO_DIRECT_IO );
    }

    //
    //  Attach our device object to the given device object
    //  The only reason this can fail is if someone is trying to dismount
    //  this volume while we are attaching to it.
    //

    newDevExt->FileSystem = IoAttachDeviceToDeviceStack( YGFSMonDeviceObject, 
                                              DeviceObject);
    if (newDevExt->FileSystem) {

		if (newDevExt->LogicalDrive >= 'A' && newDevExt->LogicalDrive <= 'Z')
		{
			FsVolTable[newDevExt->LogicalDrive-'A'].DiskDevice = newDevExt->DiskSystem;
			FsVolTable[newDevExt->LogicalDrive-'A'].YGFSMonDevice = YGFSMonDeviceObject;
			FsVolTable[newDevExt->LogicalDrive-'A'].FileSystem = newDevExt->FileSystem;
		}

        //
        //  Finished all initialization of the new device object,  so clear the
        //  initializing flag now.
        //

        ClearFlag( YGFSMonDeviceObject->Flags, DO_DEVICE_INITIALIZING );

        //
        //  Display the name
        //

        KdPrint(("YGFSMonAttachToMountedDevice:             Attaching to volume        %p \"%wZ\"\n", 
                       newDevExt->FileSystem,
					   newDevExt->DeviceName) );
    }

    return STATUS_SUCCESS;
}

VOID
YGFSMonCleanupMountedDevice (
    IN PDEVICE_OBJECT DeviceObject
    )
/*++

Routine Description:

    This cleans up any allocated memory in the device extension.

Arguments:

    DeviceObject - The device we are cleaning up

Return Value:

--*/
{        
	PYGFS_HOOK_EXTENSION hookExt;
	hookExt = (PYGFS_HOOK_EXTENSION)DeviceObject->DeviceExtension;
	if (hookExt->LogicalDrive >= 'A' && hookExt->LogicalDrive <= 'Z')
	{
		FsVolTable[hookExt->LogicalDrive-'A'].DiskDevice = NULL;
		FsVolTable[hookExt->LogicalDrive-'A'].YGFSMonDevice = NULL;
		FsVolTable[hookExt->LogicalDrive-'A'].FileSystem = NULL;
	}
}


VOID
YGFSMonGetObjectName (
    IN PVOID Object,
    IN OUT PUNICODE_STRING Name
    )
/*++

Routine Description:

    This routine will return the name of the given object.
    If a name can not be found an empty string will be returned.

Arguments:

    Object - The object whose name we want

    Name - A unicode string that is already initialized with a buffer that
           receives the name of the object.

Return Value:

    None

--*/
{
    NTSTATUS status;
    CHAR nibuf[512];        //buffer that receives NAME information and name
    POBJECT_NAME_INFORMATION nameInfo = (POBJECT_NAME_INFORMATION)nibuf;
    ULONG retLength;

    status = ObQueryNameString( Object, nameInfo, sizeof(nibuf), &retLength);

    Name->Length = 0;
    if (NT_SUCCESS( status )) {

        RtlCopyUnicodeString( Name, &nameInfo->Name );
    }
}

BOOLEAN
YGFSMonIsAttachedToDevice (
    PDEVICE_OBJECT DeviceObject,
    PDEVICE_OBJECT *AttachedDeviceObject OPTIONAL
    )
/*++

Routine Description:

    This walks down the attachment chain looking for a device object that
    belongs to this driver.

Arguments:

    DeviceObject - The device chain we want to look through

Return Value:

    TRUE if we are attached, FALSE if not

--*/
{
    PDEVICE_OBJECT currentDevObj;
    PDEVICE_OBJECT nextDevObj;

#ifdef WINXP 
    currentDevObj = IoGetAttachedDeviceReference( DeviceObject );
#else
    currentDevObj = DeviceObject->AttachedDevice;
#endif
    //
    //  CurrentDevObj has the top of the attachment chain.  Scan
    //  down the list to find our device object.

    while (currentDevObj)
	{
    
        if (IS_MY_DEVICE_OBJECT( currentDevObj )) {

            //
            //  We have found that we are already attached.  If we are
            //  returning the device object we are attached to then leave the
            //  reference on it.  If not then remove the reference.
            //

            if (ARGUMENT_PRESENT(AttachedDeviceObject)) {

                *AttachedDeviceObject = currentDevObj;
            }            
#ifdef WINXP
            ObDereferenceObject( currentDevObj );
#endif
            return TRUE;
        }

        //
        //  Get the next attached object.  This puts a reference on 
        //  the device object.
        //

#ifdef WINXP
        nextDevObj = IoGetLowerDeviceObject( currentDevObj );
#endif
        nextDevObj = currentDevObj->AttachedDevice;

        //
        //  Dereference our current device object, before
        //  moving to the next one.
        //

#ifdef WINXP
        ObDereferenceObject( currentDevObj );
#endif
        currentDevObj = nextDevObj;
        
    }
    
    if (ARGUMENT_PRESENT(AttachedDeviceObject)) {

        *AttachedDeviceObject = NULL;
    }

    return FALSE;
}    

UCHAR YGFSMonGetDriverLetter(PDEVICE_OBJECT pFileSystem)
{
    OBJECT_ATTRIBUTES   objectAttributes;
    UNICODE_STRING      fileNameUnicodeString;
    WCHAR               filename[] = L"\\DosDevices\\A:";
    NTSTATUS            ntStatus;
	ULONG               ulBuff;
	HANDLE				hLinkHandle;
    UCHAR               Drive = 0;
    UNICODE_STRING tempName;
    WCHAR tempNameBuffer[MAX_DEVNAME_LENGTH];
    UNICODE_STRING DriverName;
    WCHAR DriverNameBuffer[MAX_DEVNAME_LENGTH];
    
	RtlInitEmptyUnicodeString( &DriverName,
							   DriverNameBuffer,
							   sizeof(DriverNameBuffer) );
    YGFSMonGetObjectName(pFileSystem, &DriverName );

	if (!DriverName.Length) return '*';

	for (Drive = 0;Drive < 26;Drive++)
	{
        filename[12] = 'A'+Drive;

        //
        // We have to figure out what device to hook - first open the volume's 
        // root directory
        //
        RtlInitUnicodeString( &fileNameUnicodeString, filename );
        InitializeObjectAttributes( &objectAttributes, &fileNameUnicodeString, 
                                    OBJ_CASE_INSENSITIVE, NULL, NULL );
		ntStatus = ZwOpenSymbolicLinkObject(&hLinkHandle,GENERIC_READ,&objectAttributes);
		if( NT_SUCCESS( ntStatus ) ) 
		{
			RtlInitEmptyUnicodeString( &tempName,
									   tempNameBuffer,
									   sizeof(tempNameBuffer) );
			ulBuff = sizeof(tempNameBuffer);
			ntStatus = ZwQuerySymbolicLinkObject(hLinkHandle,&tempName,&ulBuff);
			ZwClose(hLinkHandle);
			if( NT_SUCCESS( ntStatus ) ) 
			{
				if (RtlCompareUnicodeString( &tempName,&DriverName, TRUE ) == 0) 
				{
					return (UCHAR)('A'+Drive);
				}
			}
		}
	}
	return '*';
}

//----------------------------------------------------------------------
//
// YGFSMonGetFullPath
//
// Takes a fileobject and filename and returns a canonical path,
// nicely formatted, in MatchFile.
//
//----------------------------------------------------------------------
PHASHOBJ YGFSMonGetFullPath( PFILE_OBJECT fileObject, PYGFS_HOOK_EXTENSION hookExt, 
	                         PCONVERT_BUF	pMatchInfo)
{
    ULONG               ulMatchType,ulPathPointer = 0;
    PFILE_OBJECT        relatedFileObject;
    PHASHOBJ            hashEntry;
    ANSI_STRING         componentName;
    //
    // Only do this if a GUI is active and filtering is on
    //
    if(!hookExt)  return NULL;

    //
    // First, lookup the object in the hash table to see if a name 
    // has already been generated for it
    //
	ExAcquireResourceSharedLite(&HashMutex,TRUE);

    hashEntry = HashTable[HASHOBJECT(fileObject)];
    while( hashEntry && hashEntry->ulHashIndex != (ULONG)fileObject )  {

        hashEntry = hashEntry->pNext;
    }

    //
    // Did we find an entry?
    //
    if( hashEntry ) {

        //
        // Yes, so get the name from the entry.
        //
        strcpy( pMatchInfo->szMonitorFile, hashEntry->MatchInfo.szMonitorFile);
		ExReleaseResourceLite(&HashMutex);
        return hashEntry;
    }
	ExReleaseResourceLite(&HashMutex);

    //
    // We didn't find the name in the hash table, so we have to attempt
    // to construct it from the file objects.  Note that we won't always
    // be able to successfully do this, because the file system may
    // deallocate the name in the file object at its discretion. 
    //

    //
    // Is it DASD (Volume) I/O?
    //
    if( !fileObject || !fileObject->FileName.Length || fileObject->FileName.Length > 2*MAXPATHLEN ) {

        sprintf(pMatchInfo->szMonitorFile, "%C: DASD", hookExt->LogicalDrive );
        return NULL;
    }
    
    //
    // Add the drive letter first at the front of the name
    //
    sprintf(pMatchInfo->szMonitorFile, "%C:", hookExt->LogicalDrive );

    ulPathPointer = 2;
	relatedFileObject = fileObject->RelatedFileObject;
    
	//
	// Only look at related file object if this is a relative name
	//
	if( fileObject->FileName.Buffer[0] != L'\\' )  
	{
		while( relatedFileObject ) 
		{
            RtlUnicodeStringToAnsiString( &componentName, 
                                          &relatedFileObject->FileName, TRUE );

			if (ulPathPointer + componentName.Length >= MAXPATHLEN)
			{
	            RtlFreeAnsiString( &componentName );
				return NULL;
			}
			strncpy(&pMatchInfo->szMonitorFile[ulPathPointer],componentName.Buffer,componentName.Length);
			ulPathPointer += componentName.Length;
		
            RtlFreeAnsiString( &componentName );
			relatedFileObject = relatedFileObject->RelatedFileObject;
		}
	}
	else
	{
		RtlUnicodeStringToAnsiString( &componentName, &fileObject->FileName, TRUE );    

		if (ulPathPointer + componentName.Length >= MAXPATHLEN)
		{
			RtlFreeAnsiString( &componentName );
			return NULL;
		}
		strncpy(&pMatchInfo->szMonitorFile[ulPathPointer],componentName.Buffer,componentName.Length);
		ulPathPointer += componentName.Length;
		RtlFreeAnsiString( &componentName );
	}
	pMatchInfo->szMonitorFile[ulPathPointer] = 0;

    //
    // If its a network drive, take off the first drive letter. This is because
    // network drives share a common device objects and the names are fully formed to 
    // specify the network mounted drive letter already
    //
    if( ulPathPointer > 5 && pMatchInfo->szMonitorFile[4] == ':' && pMatchInfo->szMonitorFile[5] == '\\') 
	{
        strcpy( pMatchInfo->szMonitorFile,pMatchInfo->szMonitorFile + 3 );
    }
	ulMatchType = MatchWithPathFilter(pMatchInfo);
	if (!ulMatchType) return NULL;
	return YGFSMonLogHash((ULONG)fileObject,FILE_OPEN_NORMAL,pMatchInfo);
}


//----------------------------------------------------------------------
//
// YGFSMonGetProcessNameOffset
//
// In an effort to remain version-independent, rather than using a
// hard-coded into the KPEB (Kernel Process Environment Block), we
// scan the KPEB looking for the name, which should match that
// of the system process. This is because we are in the system process'
// context in DriverEntry, where this is called.
//
//----------------------------------------------------------------------
ULONG YGFSMonGetProcessNameOffset()
{
    PEPROCESS       curproc;
    int             i;

    curproc = PsGetCurrentProcess();

    //
    // Scan for 12KB, hoping the KPEB never grows that big!
    //
    for( i = 0; i < 3*PAGE_SIZE; i++ ) {
     
        if( !strncmp( SYSNAME, (PCHAR) curproc + i, strlen(SYSNAME) )) {

            return i;
        }
    }

    //
    // Name not found - oh, well
    //
    return 0;
}

//----------------------------------------------------------------------
//
// YGFSMonGetProcess
//
// Uses undocumented data structure offsets to obtain the name of the
// currently executing process.
//
//----------------------------------------------------------------------
PCHAR YGFSMonGetProcess( PCHAR Name )
{
    PEPROCESS       curproc;
	PFSPATHINFO     pPathInfo;
    char            *nameptr;

	if (!bStartWork) return FALSE;
    //
    // We only do this if we determined the process name offset
    //
    if( ProcessNameOffset ) {
      
        //
        // Get a pointer to the current process block
        //
        curproc = PsGetCurrentProcess();

        //
        // Dig into it to extract the name 
        //
        nameptr   = (PCHAR) curproc + ProcessNameOffset;
		strncpy(Name,nameptr,NT_PROCNAMELEN);
        Name[NT_PROCNAMELEN] = 0;
    } else {

        strcpy( Name, "???" );
    }

    //
    // Apply process name filters
    //
	ExAcquireResourceSharedLite(&ResourceMutex,TRUE);
    for(pPathInfo = ExcludeProcessList;pPathInfo;pPathInfo = pPathInfo->pNext ) {

        if( MatchWithPattern(pPathInfo->pMonitorPath, Name,FALSE)) {

			ExReleaseResourceLite(&ResourceMutex);
            return NULL;
        }
    }

	ExReleaseResourceLite(&ResourceMutex);

    return Name;
}
