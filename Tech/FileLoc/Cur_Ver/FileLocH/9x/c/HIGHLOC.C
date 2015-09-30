//
//  HighLoc.C MODULE
//
//  AUTHOR:
//	ZHOU LONG   (ZL)
//
//  Date:
//	22-Nov-1999
//

#define     WANTVXDWRAPS
#include    <basedef.h>
#include    <vmm.h>
#include    <vxdwraps.h>
#include    <debug.h>
#include    <ifs.h>
#include    <vwin32.h>
#include    <winerror.h>



#include    "ifsmgr.h"
#include    "..\..\..\..\..\BlkMover\Cur_Ver\sys\9x\c\share.h"
#include    "..\..\..\filelocl\9x\lowloc.h"
#include    "HighLoc.h"


#pragma     VxD_LOCKED_DATA_SEG
VMMLIST     handle_list;

char	    SignString[] = "Yg File Locater.";	//  16 bytes

char	    SignSector[SECTOR_SIZE];

LOC_INFO   LocInfo[SECTORS_OF_CACHE];

#ifdef	DEBUG

#define GARBAGE_CHAR	0xCC

#endif

#ifdef	DEBUG
QWORD		    ticksOpen;
QWORD		    ticksOpenFile;
QWORD		    ticksClose;
QWORD		    ticksCloseFile;
QWORD		    ticksLog2Phys;
QWORD		    ticksGetAttr;
QWORD		    ticksSetAttr;
QWORD		    ticksRead;
QWORD		    ticksWrite;
QWORD		    ticksGetSize;
QWORD		    ticksSetSize;
QWORD		    ticksVolFlush;
#endif

#pragma     VxD_LOCKED_CODE_SEG

DWORD	(_stdcall *HighLoc_W32_PROC[])(DWORD,DWORD,LPDIOC) = {
		HighLoc_GetVersion,
		HighLoc_OpenHandle,
		HighLoc_CloseHandle,
		HighLoc_LogicSector2PhysSector,
		HighLoc_LogicSector2PhysSectorFast,
		HighLoc_Flush,
		HighLoc_OpenBuffer,
		HighLoc_CloseBuffer,
		HighLoc_CopyData,
		HighLoc_EnableDisable,
		HighLoc_SetPhysSectors
    };

#define MAX_HighLoc_W32_API    (sizeof(HighLoc_W32_PROC)/sizeof(DWORD))


DWORD	_stdcall HighLoc_W32_DeviceIoControl(DWORD dwService,
					     DWORD dwDDB,
					     DWORD hDevice,
					     LPDIOC lpDIOCParms)
{
    DWORD   dwRetVal;

    if(DIOC_OPEN == dwService)
    {
	dwRetVal = 0;
    }
    else if(DIOC_CLOSEHANDLE == dwService)
    {
	dwRetVal = HighLoc_Cleanup();
    }
    else if(dwService > MAX_HighLoc_W32_API)
    {
	dwRetVal = ERROR_NOT_SUPPORTED;
    }
    else
    {
	dwRetVal = (HighLoc_W32_PROC[dwService-1])(dwDDB,hDevice,lpDIOCParms);
    }
    return  dwRetVal;
}   //	HighLoc_W32_DeviceIoControl


DWORD	HighLoc_Cleanup(void)
{
    return  VXD_SUCCESS;
}   //	HighLoc_Cleanup


DWORD	_stdcall HighLoc_GetVersion(DWORD dwDDB,DWORD hDevice,LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pRegInOut;

    pRegInOut = (DIOC_REGISTERS *)lpDIOCParms->lpvInBuffer;
    pRegInOut->reg_EAX = GetVersion();
    return  NO_ERROR;
}   //	HighLoc_GetVersion


DWORD	GetVersion(VOID)
{
    return  (HighLoc_Major_Ver << 16) | HighLoc_Minor_Ver;
}   //	GetVersion


DWORD	_stdcall HighLoc_OpenBuffer(DWORD dwDDB,DWORD hDevice,LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pRegInOut;
    DWORD	    nCount;

    pRegInOut = (DIOC_REGISTERS *)lpDIOCParms->lpvInBuffer;
    nCount = (DWORD)pRegInOut->reg_ECX;
    pRegInOut->reg_EAX = (DWORD)_HeapAllocate(nCount*sizeof(QWORD),HEAPSWAP);
    ASSERT(pRegInOut->reg_EAX);
    if (pRegInOut->reg_EAX)
	memset((PVOID)pRegInOut->reg_EAX,0,nCount*sizeof(QWORD));
    return  NO_ERROR;
}   /*	HighLoc_Open  */

DWORD	_stdcall HighLoc_CloseBuffer(DWORD dwDDB,DWORD hDevice,LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pRegInOut;
    PVOID	    p;

    pRegInOut = (DIOC_REGISTERS *)lpDIOCParms->lpvInBuffer;
    p = (PVOID)pRegInOut->reg_EAX;
    pRegInOut->reg_EAX = _HeapFree(p,0);
    ASSERT(pRegInOut->reg_EAX);
    return  NO_ERROR;
}   /*	HighLoc_Close  */

DWORD	_stdcall HighLoc_CopyData(DWORD dwDDB,DWORD hDevice,LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pRegInOut;
    PBYTE	    pSrc;
    PBYTE	    pTar;
    DWORD	    nCount;


    pRegInOut = (DIOC_REGISTERS *)lpDIOCParms->lpvInBuffer;
    pSrc = (PBYTE)pRegInOut->reg_ESI;
    pTar = (PBYTE)pRegInOut->reg_EDI;
    nCount = (DWORD)pRegInOut->reg_ECX;
    memcpy(pTar,pSrc,nCount*sizeof(QWORD));
    memset(pSrc,0,nCount*sizeof(QWORD));
    pRegInOut->reg_EAX = 1;
    return  NO_ERROR;
}   /*	HighLoc_CopyData */

DWORD	_stdcall HighLoc_OpenHandle(DWORD dwDDB,DWORD hDevice,LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pRegInOut;

    pRegInOut = (DIOC_REGISTERS *)lpDIOCParms->lpvInBuffer;
    ASSERT(pRegInOut->reg_EAX < 0x80000000 && pRegInOut->reg_EAX > 0x400000);
    pRegInOut->reg_EAX = (DWORD)OpenHandle((PSZ)pRegInOut->reg_EAX);
    return  NO_ERROR;
}   /*	HighLoc_OpenHandle  */


DWORD	_stdcall HighLoc_CloseHandle(DWORD dwDDB,DWORD hDevice,LPDIOC lpDIOCParms)
{
    DIOC_REGISTERS  *pRegInOut;
    pRegInOut = (DIOC_REGISTERS *)lpDIOCParms->lpvInBuffer;
    ASSERT(pRegInOut->reg_EAX > 0xc0000000);
    pRegInOut->reg_EAX = (DWORD)CloseHandle((HANDLE)pRegInOut->reg_EAX);
    return  NO_ERROR;
}   /*	HighLoc_CloseHandle */


DWORD	_stdcall HighLoc_LogicSector2PhysSector(DWORD dwDDB,DWORD hDevice,LPDIOC lpDIOCParms)
{
    HANDLE  hHandle;
    PQWORD  pPhysicalSec;
    DWORD   dwLogicalSec;
    BOOL    bResult;
    int     nNumSectors;

    DIOC_REGISTERS  *pRegInOut;
    pRegInOut = (DIOC_REGISTERS *)lpDIOCParms->lpvInBuffer;

    hHandle = (HANDLE)pRegInOut->reg_EAX;
    pPhysicalSec = (PQWORD)pRegInOut->reg_EBX;
    dwLogicalSec = (DWORD)pRegInOut->reg_EDX;
    nNumSectors = (int)pRegInOut->reg_ECX;

    ASSERT(pRegInOut->reg_EAX > 0xc0000000);
    ASSERT(pRegInOut->reg_EBX > 0x400000 && pRegInOut->reg_EBX < 0x80000000);
    bResult = (DWORD)LogicSector2PhysSectorEx(hHandle,dwLogicalSec,pPhysicalSec,nNumSectors);
    pRegInOut->reg_EAX = bResult;
    return  NO_ERROR;
}   /*	HighLoc_LogicSector2PhysSector	*/

DWORD	_stdcall HighLoc_LogicSector2PhysSectorFast(DWORD dwDDB,DWORD hDevice,LPDIOC lpDIOCParms)
{
    HANDLE  hHandle;
    PQWORD  pPhysicalSec;
    DWORD   dwLogicalSec;
    BOOL    bResult;
    int     nNumSectors;
    DWORD   dwArrIndex;

    DIOC_REGISTERS  *pRegInOut;
    pRegInOut = (DIOC_REGISTERS *)lpDIOCParms->lpvInBuffer;

    hHandle = (HANDLE)pRegInOut->reg_EAX;
    pPhysicalSec = (PQWORD)pRegInOut->reg_EBX;
    dwLogicalSec = (DWORD)pRegInOut->reg_EDX;
    nNumSectors = (int)pRegInOut->reg_ECX;
    dwArrIndex	= (DWORD)pRegInOut->reg_ESI;

    ASSERT(pRegInOut->reg_EAX > 0xc0000000);
    bResult = (DWORD)LogicSector2PhysSectorFast(hHandle,dwLogicalSec,pPhysicalSec,dwArrIndex,nNumSectors);
    pRegInOut->reg_EAX = bResult;
    return  NO_ERROR;
}   /*	HighLoc_LogicSector2PhysSectorFast  */

DWORD	_stdcall HighLoc_Flush(DWORD dwDDB,DWORD hDevice,LPDIOC lpDIOCParms)
{
    DWORD   dwHandle;
    BOOL    bResult;

    DIOC_REGISTERS  *pRegInOut;
    pRegInOut = (DIOC_REGISTERS *)lpDIOCParms->lpvInBuffer;

    dwHandle = (DWORD)pRegInOut->reg_EBX;
    bResult = _VolFlush(dwHandle,VOL_DISCARD_CACHE);
    ASSERT(!bResult);
    pRegInOut->reg_EAX = !bResult;
    return  NO_ERROR;
}

DWORD	_stdcall HighLoc_EnableDisable(DWORD dwDDB,DWORD hDevice,LPDIOC lpDIOCParms)
{
    DWORD   dwHandle;
    BOOL    bResult;

    DIOC_REGISTERS  *pRegInOut;
    pRegInOut = (DIOC_REGISTERS *)lpDIOCParms->lpvInBuffer;

    WorkingOrResting((BOOL)pRegInOut->reg_EAX);
    pRegInOut->reg_EAX = TRUE;
    return  NO_ERROR;
}

DWORD	_stdcall HighLoc_SetPhysSectors(DWORD dwDDB,DWORD hDevice,LPDIOC lpDIOCParms)
{
    DWORD   dwHandle;
    BOOL    bResult;
    PQWORD  pPhysSectorArr;
    DWORD   dwIndex;
    DWORD   dwSectors;
    DWORD   dwValue;


    DIOC_REGISTERS  *pRegInOut;
    pRegInOut = (DIOC_REGISTERS *)lpDIOCParms->lpvInBuffer;

    dwValue = pRegInOut->reg_EAX;
    pPhysSectorArr = (PQWORD)pRegInOut->reg_EBX;
    dwIndex = (DWORD)pRegInOut->reg_ESI;
    dwSectors = (DWORD)pRegInOut->reg_ECX;
    memset(&pPhysSectorArr[dwIndex],dwValue,dwSectors*sizeof(QWORD));
    pRegInOut->reg_EAX = TRUE;
    return  NO_ERROR;
}

BOOL	OnSysDynamicDeviceInit()
{
    int     nCount;
    BOOL    bSucc;

    if(!CanWeWork())
	return	FALSE;	//  FILELOCL vsd is not working , so we must exit.

    //	temporarily disable FILELOCL to monitor disk io
    WorkingOrResting(FALSE);

    handle_list = List_Create(LF_SWAP | LF_ALLOC_ERROR,sizeof(FILE_LOC_HANDLE));
    if(!handle_list)
	return	FALSE;

    //pLocInfo = (PLOC_INFO)_HeapAllocate(sizeof(LOC_INFO) * SECTORS_OF_CACHE,HEAPSWAP);
    //if(NULL == pLocInfo)
    //{
    //	  List_Destroy(handle_list);
    //	  return  FALSE;
    //}

    for(
	nCount = 0;
	nCount < sizeof(SignSector) / sizeof(SignString);
	nCount++
       )
    {
	memcpy(SignSector + (nCount * sizeof(SignString)),SignString,sizeof(SignString));
    }

    return  TRUE;
}   /*	OnSysDynamicDeviceInit	*/


BOOL	OnSysDynamicDeviceExit()
{
    PFILE_LOC_HANDLE	hHandle;

    #ifdef  DEBUG
    int     nDiskIoRet;
    ULONG   ulCheck;
    #endif

    //	disable FILELOCL to monitor disk io
    WorkingOrResting(FALSE);
    /*
    for(
	hHandle = (PFILE_LOC_HANDLE)List_Get_First(handle_list);
	hHandle;
	hHandle = (PFILE_LOC_HANDLE)List_Get_Next(handle_list,hHandle)
       )
    {
	if(hHandle->wAttr)
	{
	    //
	    //	restore original file attribute
	    //

	    #ifdef  DEBUG

	    nDiskIoRet = R0_SetFileAttributes(hHandle->pszFileName,hHandle->wAttr);
	    ASSERT(!nDiskIoRet);

	    #else

	    R0_SetFileAttributes(hHandle->pszFileName,hHandle->wAttr);

	    #endif
	}

	#ifdef	DEBUG

	ulCheck = _HeapFree(hHandle->pszFileName,0);
	ASSERT(ulCheck);
	nDiskIoRet = R0_CloseFile(hHandle->dwhSys);
	ASSERT(!nDiskIoRet);
	if(hHandle->lMap)
	{
	    List_Destroy(hHandle->lMap);
	}
	memset(hHandle,GARBAGE_CHAR,sizeof(FILE_LOC_HANDLE));

	#else

	_HeapFree(hHandle->pszFileName,0);
	R0_CloseFile(hHandle->dwhSys);
	if(hHandle->lMap)
	{
	    List_Destroy(hHandle->lMap);
	}

	#endif
    }	//  For Loop
    */
    if (handle_list)
	List_Destroy(handle_list);
    #ifdef  DEBUG
    handle_list = 0;
    #endif
    return  TRUE;
}   /*	OnSysDynamicDeviceExit	*/

HANDLE	OpenHandle(PSZ	pszFileName)
{
    PFILE_LOC_HANDLE	hHandle;
    WORD		wAttri;
    DWORD		dwFileHandle;
    DWORD		dwAction;
    DWORD		dwFileSize = 0; //  must init = 0
    int 		nDiskIoRet;
    DWORD		dwExtendSize;
    DWORD		dwWritten;

    #ifdef  DEBUG
    ULONG		ulCheck;
    PDWORD		pdwVol;
    WORD		wCheckAttri;
    #endif

    #ifdef  DEBUG
    QWORD		tick_1,tick_2;
    QWORD		tick_3,tick_4;
    QWORD		tick_5,tick_6;
    QWORD		tick_7,tick_8;
    QWORD		tick_9,tick_10;
    QWORD		tick_11,tick_12;
    #endif

    #ifdef  DEBUG
    GetCurTime(&tick_1);
    #endif

    hHandle = (PFILE_LOC_HANDLE)NULL;
    wAttri = 0;
    dwFileHandle = 0;

    #ifdef  DEBUG

    Debug_Printf("Locating %s \r\n",pszFileName);

    //	pszFileName must like "c:\lowloc.c" (complete path)
    pdwVol = (PDWORD)pszFileName;
    ASSERT(HIBYTE(LOWORD(*pdwVol)) == ':');

    #endif

    #ifdef  DEBUG
    GetCurTime(&tick_3);
    #endif

    nDiskIoRet = R0_GetFileAttributes(pszFileName,&wAttri);

    #ifdef  DEBUG
    GetCurTime(&tick_4);
    CalcTick(tick_3,tick_4,&ticksGetAttr);
    #endif

    if (nDiskIoRet)
	return	(HANDLE)NULL;

    if ((wAttri & _A_RDONLY) || (wAttri & _A_SYSTEM) || (wAttri & _A_HIDDEN))
    {
	#ifdef	DEBUG
	GetCurTime(&tick_5);
	#endif
	nDiskIoRet = R0_SetFileAttributes(pszFileName,_A_NORMAL);
	#ifdef	DEBUG
	GetCurTime(&tick_6);
	CalcTick(tick_5,tick_6,&ticksSetAttr);
	#endif
	ASSERT(!nDiskIoRet);
	if(nDiskIoRet)
	    return  (HANDLE)NULL;
    }

    /*
     *	We open the file using OPEN_FLAGS_NO_CACHE flag,make that reads
     *	and writes on the file should not be cached, all operations will be
     *	directly done to the disk.
     *
     */
    #ifdef  DEBUG
    GetCurTime(&tick_7);
    #endif
    nDiskIoRet = R0_OpenCreateFile(FALSE,ACCESS_READWRITE | SHARE_COMPATIBILITY,
		    _A_NORMAL,ACTION_OPENEXISTING,OPEN_FLAGS_NO_CACHE,
		    pszFileName,&dwFileHandle,&dwAction);
    #ifdef  DEBUG
    GetCurTime(&tick_8);
    CalcTick(tick_7,tick_8,&ticksOpenFile);
    #endif

    if (nDiskIoRet == 0)
    {
	#ifdef	DEBUG
	GetCurTime(&tick_9);
	#endif

	nDiskIoRet = R0_GetFileSize(dwFileHandle,&dwFileSize);

	#ifdef	DEBUG
	GetCurTime(&tick_10);
	CalcTick(tick_9,tick_10,&ticksGetSize);
	#endif

	if (nDiskIoRet == 0)
	{
	    hHandle = (PFILE_LOC_HANDLE)List_Allocate(handle_list);
	    if (hHandle)
	    {
		memset(hHandle,0,sizeof(FILE_LOC_HANDLE));
		hHandle->wFileNameLen = strlen(pszFileName) + 1;
		hHandle->pszFileName = (PSZ)_HeapAllocate(hHandle->wFileNameLen,HEAPSWAP);
		if (hHandle->pszFileName)
		{
		    strcpy(hHandle->pszFileName,pszFileName);
		    hHandle->dwhSys = dwFileHandle;
		    hHandle->wAttr = wAttri;
		    hHandle->dwVol =(DWORD)((*pszFileName | 0x20) - 0x61) & 0xFF;
		    hHandle->dwFileSize = dwFileSize;
		    hHandle->lMap = List_Create(LF_SWAP | LF_ALLOC_ERROR,sizeof(LOGIC2PHYS));
		    if (hHandle->lMap)
		    {
			List_Attach_Tail(handle_list,hHandle);
			#ifdef	DEBUG
			GetCurTime(&tick_2);
			CalcTick(tick_1,tick_2,&ticksOpen);
			#endif
			dwExtendSize = dwFileSize % SECTOR_SIZE;
			if (0 != dwExtendSize)
			{
			    //
			    //	change file size ( write 0 byte )
			    //
			    //

			    #ifdef  DEBUG
			    GetCurTime(&tick_11);
			    #endif
			    nDiskIoRet = R0_WriteFile(FALSE,dwFileHandle,0,
						     dwFileSize + SECTOR_SIZE - dwExtendSize,(PBYTE)&dwWritten,&dwWritten);
			    #ifdef  DEBUG
			    GetCurTime(&tick_12);
			    CalcTick(tick_11,tick_12,&ticksSetSize);
			    #endif
			    if (nDiskIoRet == 0)
			    {
				return hHandle;
			    }
			}
			else
			    return hHandle;
			List_Remove(handle_list,hHandle);
		    }
		    _HeapFree(hHandle->pszFileName,0);
		}
		List_Deallocate(handle_list,hHandle);
	    }
	}
	R0_CloseFile(dwFileHandle);
    }
    return NULL;
}   /*	OpenHandle  */



BOOL CloseHandle(HANDLE hLocater)
{
    PFILE_LOC_HANDLE	hHandle;
    DWORD		dwWritten;

    #ifdef  DEBUG
    int 		nDiskIoRet;
    ULONG		ulCheck;
    #endif

    #ifdef  DEBUG
    QWORD		tick_1,tick_2;
    QWORD		tick_3,tick_4;
    QWORD		tick_5,tick_6;
    QWORD		tick_7,tick_8;
    #endif


    #ifdef  DEBUG
    GetCurTime(&tick_1);
    #endif
    hHandle = (PFILE_LOC_HANDLE)hLocater;

    #ifdef  DEBUG

    ASSERT((DWORD)hHandle > 0xC0000000 && (DWORD)hHandle < 0xD0000000);

    #endif

    if(hHandle->wAttr)
    {
	#ifdef	DEBUG
	GetCurTime(&tick_3);
	#endif

	#ifdef	DEBUG

	nDiskIoRet = R0_SetFileAttributes(hHandle->pszFileName,
			hHandle->wAttr);
	ASSERT(!nDiskIoRet);

	#else

	R0_SetFileAttributes(hHandle->pszFileName,hHandle->wAttr);

	#endif
	#ifdef	DEBUG
	GetCurTime(&tick_4);
	CalcTick(tick_3,tick_4,&ticksSetAttr);
	#endif
    }

    if (0 != hHandle->dwFileSize % SECTOR_SIZE)
    {
	#ifdef	DEBUG
	GetCurTime(&tick_5);
	#endif
	#ifdef	DEBUG

	nDiskIoRet = R0_WriteFile(FALSE,hHandle->dwhSys,0,hHandle->dwFileSize,(PBYTE)&dwWritten,&dwWritten);
	ASSERT(!nDiskIoRet);

	#else

	R0_WriteFile(FALSE,hHandle->dwhSys,0,hHandle->dwFileSize,&dwWritten,&dwWritten);

	#endif
	#ifdef	DEBUG
	GetCurTime(&tick_6);
	CalcTick(tick_5,tick_6,&ticksSetSize);
	#endif
    }

    #ifdef  DEBUG
    GetCurTime(&tick_7);
    #endif

    #ifdef  DEBUG
    if (hHandle->dwhSys)
    {
	nDiskIoRet = R0_CloseFile(hHandle->dwhSys);
	ASSERT(!nDiskIoRet);
    }
    if (hHandle->pszFileName)
    {
	ulCheck = _HeapFree(hHandle->pszFileName,0);
	ASSERT(ulCheck);
    }
    if (hHandle->lMap)
	List_Destroy(hHandle->lMap);

    List_Remove(handle_list,hHandle);
    memset(hHandle,GARBAGE_CHAR,sizeof(FILE_LOC_HANDLE));
    List_Deallocate(handle_list,hHandle);

    #else
    if (hHandle->dwhSys)
	R0_CloseFile(hHandle->dwhSys);

    if (hHandle->pszFileName)
	_HeapFree(hHandle->pszFileName,0);
    if (hHandle->lMap)
	List_Destroy(hHandle->lMap);
    List_Remove(handle_list,hHandle);
    List_Deallocate(handle_list,hHandle);

    #endif
    #ifdef  DEBUG
    GetCurTime(&tick_8);
    CalcTick(tick_7,tick_8,&ticksCloseFile);
    #endif

    #ifdef  DEBUG
    GetCurTime(&tick_2);
    CalcTick(tick_1,tick_2,&ticksClose);
    #endif

    return  TRUE;
}   /*	CloseHandle */



BOOL	LogicSector2PhysSector(HANDLE hLocater,DWORD dwLogicSectorNum,
			       PQWORD pqwPhysSectorNum)
{
    return LogicSector2PhysSectorEx(hLocater,dwLogicSectorNum,pqwPhysSectorNum,1);
}   /*	LogicSector2PhysSector	*/

BOOL	LogicSector2PhysSectorEx(HANDLE hLocater,DWORD dwLogicSectorNum,
				 PQWORD pqwPhysSectorNum, int nSectors)
{
    PFILE_LOC_HANDLE	hHandle;
    int 		nDiskIoRet;
    DWORD		dwFileHandle;
    DWORD		dwWrittenRead;
    DWORD		dwTotalSectors;
    DWORD		dwActualSectors;
    DWORD		dwSectorsLeft;
    DWORD		dwLogicalSec;
    int 		nCount;
    int 		nIndex;
    QWORD		PhysSecArr[SECTORS_OF_CACHE];

    #ifdef  DEBUG
    ULONG		ulCheck;
    PQWORD		pqwCheck;
    QWORD		tick_1,tick_2;
    QWORD		tick_3,tick_4;
    QWORD		tick_5,tick_6;
    QWORD		tick_7,tick_8;
    #endif

    #ifdef  DEBUG
    GetCurTime(&tick_1);
    #endif
    hHandle = (PFILE_LOC_HANDLE)hLocater;
    dwLogicalSec = dwLogicSectorNum;
    dwSectorsLeft = nSectors;
    nIndex = 0;
    do
    {
	if (dwSectorsLeft > SECTORS_OF_CACHE)
	    dwActualSectors = SECTORS_OF_CACHE;
	else
	    dwActualSectors = dwSectorsLeft;
	if (dwActualSectors)
	{
	    for (nCount = 0; nCount < dwActualSectors; nCount++)
	    {
		LocInfo[nCount].pqwPhysSector = &PhysSecArr[nCount];
		LocInfo[nCount].dwLogicSecNum = dwLogicSectorNum+nCount;
		LocInfo[nCount].dwReserve = 0;
		memcpy(LocInfo[nCount].LocSign,SignSector + SIGN_HEAD_LEN,SIGN_LEN);
	    }

	    /*
	     *	Now we hope FILELOCL to work (monitor disk io to locate)
	     *
	     */
	    WorkingOrResting(TRUE);

	    #ifdef  DEBUG
	    GetCurTime(&tick_5);
	    #endif
	    nDiskIoRet = R0_WriteFile(FALSE,hHandle->dwhSys,
			    SECTOR_SIZE*dwActualSectors,
			    dwLogicalSec*SECTOR_SIZE,(PBYTE)LocInfo,
			    &dwWrittenRead);

	    #ifdef  DEBUG
	    GetCurTime(&tick_6);
	    CalcTick(tick_5,tick_6,&ticksWrite);
	    #endif

	    ASSERT(!nDiskIoRet && SECTOR_SIZE * dwActualSectors == dwWrittenRead);
	    if(nDiskIoRet || SECTOR_SIZE * dwActualSectors != dwWrittenRead)
		return	FALSE;

	    #ifdef  DEBUG
	    GetCurTime(&tick_3);
	    #endif
	    #ifdef  DEBUG
	    memset(LocInfo,0,dwActualSectors*sizeof(LOC_INFO));
	    ulCheck = _VolFlush(hHandle->dwVol,VOL_DISCARD_CACHE);
	    ASSERT(!ulCheck);

	    #else

	    _VolFlush(hHandle->dwVol,VOL_DISCARD_CACHE);

	    #endif

	    #ifdef  DEBUG
	    GetCurTime(&tick_4);
	    CalcTick(tick_3,tick_4,&ticksVolFlush);
	    #endif

	    /*
	     *	temporarily disable FILELOCL to monitor disk io
	     *
	     */
	    WorkingOrResting(FALSE);

	    ASSERT(!nDiskIoRet && SECTOR_SIZE * dwActualSectors == dwWrittenRead);
	    if(nDiskIoRet || SECTOR_SIZE * dwActualSectors != dwWrittenRead)
		return	FALSE;
	    memcpy(&pqwPhysSectorNum[nIndex],PhysSecArr,dwActualSectors*sizeof(QWORD));
	    nIndex += dwActualSectors;
	    dwLogicalSec += dwActualSectors;
	    dwSectorsLeft -= dwActualSectors;
	}
    } while (dwSectorsLeft > 0);
    #ifdef  DEBUG
    GetCurTime(&tick_2);
    CalcTick(tick_1,tick_2,&ticksLog2Phys);
    #endif
    return  TRUE;
}   /*	LogicSector2PhysSectorEx  */

BOOL	LogicSector2PhysSectorFast(HANDLE hLocater,DWORD dwLogicSector,
				 PQWORD pqwPhysSectorArr, DWORD dwArrIndex, int nSectors)
{
    PFILE_LOC_HANDLE	hHandle;
    int 		nDiskIoRet;
    DWORD		dwFileHandle;
    DWORD		dwWrittenRead;
    DWORD		dwTotalSectors;
    DWORD		dwActualSectors;
    DWORD		dwSectorsLeft;
    DWORD		dwLogicalSec;
    int 		nCount;
    int 		nIndex;

    #ifdef  DEBUG
    ULONG		ulCheck;
    PQWORD		pqwCheck;
    QWORD		tick_1,tick_2;
    QWORD		tick_3,tick_4;
    QWORD		tick_5,tick_6;
    QWORD		tick_7,tick_8;
    #endif

    #ifdef  DEBUG
    GetCurTime(&tick_1);
    #endif
    hHandle = (PFILE_LOC_HANDLE)hLocater;
    dwLogicalSec = dwLogicSector;
    dwSectorsLeft = nSectors;
    nIndex = 0;
    do
    {
	if (dwSectorsLeft > SECTORS_OF_CACHE)
	    dwActualSectors = SECTORS_OF_CACHE;
	else
	    dwActualSectors = dwSectorsLeft;
	if (dwActualSectors)
	{
	    for (nCount = 0; nCount < dwActualSectors; nCount++)
	    {
		LocInfo[nCount].pqwPhysSector = &pqwPhysSectorArr[dwArrIndex+nIndex+nCount];
		LocInfo[nCount].dwLogicSecNum = dwLogicSector+nCount;
		LocInfo[nCount].dwReserve = 0;
		memcpy(LocInfo[nCount].LocSign,SignSector + SIGN_HEAD_LEN,SIGN_LEN);
	    }

	    /*
	     *	Now we hope FILELOCL to work (monitor disk io to locate)
	     *
	     */

	    #ifdef  DEBUG
	    GetCurTime(&tick_5);
	    #endif
	    nDiskIoRet = R0_WriteFile(FALSE,hHandle->dwhSys,
			    SECTOR_SIZE*dwActualSectors,
			    dwLogicalSec*SECTOR_SIZE,(PBYTE)LocInfo,
			    &dwWrittenRead);
	    #ifdef  DEBUG
	    GetCurTime(&tick_6);
	    CalcTick(tick_5,tick_6,&ticksWrite);
	    #endif

	    ASSERT(!nDiskIoRet && SECTOR_SIZE * dwActualSectors == dwWrittenRead);
	    if(nDiskIoRet || SECTOR_SIZE * dwActualSectors != dwWrittenRead)
		return	FALSE;

	    /*
	     *	temporarily disable FILELOCL to monitor disk io
	     *
	     */

	    ASSERT(!nDiskIoRet && SECTOR_SIZE * dwActualSectors == dwWrittenRead);
	    if(nDiskIoRet || SECTOR_SIZE * dwActualSectors != dwWrittenRead)
		return	FALSE;
	    nIndex += dwActualSectors;
	    dwLogicalSec += dwActualSectors;
	    dwSectorsLeft -= dwActualSectors;
	}
    } while (dwSectorsLeft > 0);
    #ifdef  DEBUG
    GetCurTime(&tick_2);
    CalcTick(tick_1,tick_2,&ticksLog2Phys);
    #endif
    return  TRUE;
}   /*	LogicSector2PhysSectorFast  */



BOOL	IsEmptyList(VMMLIST list)
{
    PVOID   pv;

    ASSERT(list);

    pv = (PVOID)List_Get_First(list);
    if(pv)
	return	FALSE;
    else
	return	TRUE;
}   //	IsEmptyList


PLOGIC2PHYS SearchNode(VMMLIST list,DWORD dwLogicNum)
{
    PLOGIC2PHYS pMap;

    for(
	pMap = (PLOGIC2PHYS)List_Get_First(list);
	pMap;
	pMap = (PLOGIC2PHYS)List_Get_Next(list,pMap)
       )
    {
	if(pMap->dwLogicNum == dwLogicNum)
	    return  pMap;
    }
    return  (PLOGIC2PHYS)NULL;
}   //	SearchNode


BOOL	FreeCacheList(PFILE_LOC_HANDLE pHandle)
{
    ASSERT(pHandle);
    ASSERT(pHandle->lMap);

    if(IsEmptyList(pHandle->lMap))
	return	TRUE;

    //	If not empty list
    List_Destroy(pHandle->lMap);

    pHandle->lMap = List_Create(LF_SWAP | LF_ALLOC_ERROR,sizeof(LOGIC2PHYS));
    if(NULL == pHandle->lMap)
    {
	return	FALSE;
    }

    return  TRUE;
}   //	FreeCacheList

#ifdef	DEBUG
VOID	CalcTick(QWORD	tick_1, QWORD  tick_2, PQWORD ticks)
{
    ASSERT(tick_2.qword_hi == tick_1.qword_hi);
    ticks->qword_lo += tick_2.qword_lo - tick_1.qword_lo;
}   //	CalcTick

#endif
