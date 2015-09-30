//======================================================================
//
// YGFSMON.h - include file for VxD YGFSMON
//
// Copyright (C) 1996-1999 Mark Russinovich and Bryce Cogswell
//
//======================================================================

//----------------------------------------------------------------------
//                           D E F I N E S 
//----------------------------------------------------------------------
#define YGFSMON_Major		1
#define YGFSMON_Minor		0

// number of hash buckets
#define NUMHASH		0x100
#define HASHOBJECT(_filenumber)	((ULONG)_filenumber)%NUMHASH

//
// Entries in VMM Win32 service table
//
#define VMMWIN32QUERYINFOKEY    (0x1E * 2 )

//
// Maximum path length
//
#define PROCESSLEN      9
#define MAXDRIVELETTER  26

#ifndef REG_DWORD
#define REG_DWORD       0x0004
#endif

#ifndef ENUMH_MAPCACHEBLOCK
#define ENUMH_MAPCACHEBLOCK 5
#endif

#if DEBUG
#define DBGPRINT		_Debug_Printf_Service
#else
void VXDINLINE _cdecl
DBGPRINT(char *pszfmt, ...)
{
}
#endif

#define 	CONVERTCHAR(pData,i,uChar) \
			_lmemcpy(&pData[i+2],&pData[i],_lstrlen(&pData[i])+1); \
			pData[i] = 'Y';	\
			pData[i+1] = uChar;	\
			pData[i+2] = 'G';	\
			i += 2;

//
// This designates the operation for which
// pathname conversion takes place
//
typedef enum {
    CONVERT_STANDARD,
    CONVERT_RENAME_SOURCE,
    CONVERT_RENAME_TARGET,
    CONVERT_FINDOPEN,
} CONVERT_TYPE;

//----------------------------------------------------------------------
//                        S T R U C T U R E S 
//----------------------------------------------------------------------
typedef struct  tagHASHOBJ
{
	ULONG  ulHashIndex;
	struct tagHASHOBJ *pNext;
	ULONG  ulDataSize;
	UCHAR  pData[];
}HASHOBJ,*PHASHOBJ;

typedef struct	tagHASH_HANDLE
{
    ULONG	ulHashSize;
    ULONG	ulFlag;
	VMM_SEMAPHORE	HashMutex;
	PHASHOBJ  pList[];   
} HASH_HANDLE,*PHASH_HANDLE;

typedef struct
{
	DWORD dwType;
	DWORD dwNameLen;
	DWORD dwDataLen;
}REGDATAHEAD,*PREGDATAHEAD;

typedef struct
{
	PVOID    hHandle;
	int      nDrive;
}FILEHANDLESTRU,*PFILEHANDLESTRU;

typedef struct
{
    pIFSFunc FuncRead;
    pIFSFunc FuncWrite;
    hndlmisc hndFunc;
}OPENFUNCSTRU,*POPENFUNCSTRU;

typedef struct {
    // embedded ioreq stucture
    ioreq            ifsir;
    // the structure isn't really defined this way, but
    // we take advantage of the layout for our purposes
    struct hndlfunc  *ifs_hndl;
    ULONG            reserved[10];
} ifsreq, *pifsreq;

typedef struct
{
	HANDLE				 hFileInfo;
	BOOL                 bProtect;
	unsigned int         unAttr;
	union
	{
	    WORD		     FileName[];
		CHAR             RegName[];
	};
}FSHASHINFO, *PFSHASHINFO;

typedef struct
{
    void			*pVolumeHandle;
	POPENFUNCSTRU   pOpenFunc;
	POPENFUNCSTRU   pFindFunc;
    volfunc			*pvolfuncs;
} FSDVOLINFO;

PVOID VXDINLINE
SelectorMapFlat(HVM VMHandle,DWORD dwCurProc,DWORD dwFlag)
{
	PVOID pSelector;
    _asm push [dwFlag]
    _asm push [dwCurProc]
    _asm push [VMHandle]
    VMMCall(_SelectorMapFlat)
    _asm add   esp,0ch
    _asm mov [pSelector],eax
	return pSelector;
}

PVOID VXDINLINE
GetCurrentProcessHandle(VOID)
{
	PVOID pProc;
    VxDCall(VWIN32_GetCurrentProcessHandle)
    _asm mov [pProc],eax
	return pProc;
}

VOID VXDINLINE
strcat(PCHAR pStr1,PCHAR pStr2)
{
	_lstrcpyn(&pStr1[_lstrlen(pStr1)],pStr2,_lstrlen(pStr2)+1);
}

VOID VXDINLINE
memset(UCHAR *pMem1,UCHAR ucVal,int nSize)
{
	int i;
	for (i = 0;i < nSize;i++)
	{
		pMem1[i] = ucVal;
	}
}

int VXDINLINE
stricmp(char *pStr1,char *pStr2)
{
	int nResult;
    _asm push [pStr1]
    _asm push [pStr2]
    VMMCall(_lstrcmpi)
    _asm add   esp,08h
    _asm mov [nResult],eax
	return nResult;
}
