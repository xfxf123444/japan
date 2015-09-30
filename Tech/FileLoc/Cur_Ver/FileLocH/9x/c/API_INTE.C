//
//  File Locator's VxD interface
//
//  AUTHOR:
//	ZHOU LONG   (ZL)
//
//  Date:
//	11-Jan-2000
//

//
//  return  FileLocH's current version number,if zero ,FileLocH is not loaded
//

DWORD	GetVersion(VOID);

//
//  For getting approciate positions of a file, the user must open this file
//  through the API first to get a handle.If fail,return NULL( zero ).
//

HANDLE	OpenHandle(PSZ	pszFileName);


//
//  Closing an open handle( The handle must have been previously returned by
//  the OpenHandle service.
//  If hLocater is an invalid handle,the function fails and return FALSE, otherwise
//  return TRUE.
//

BOOL	CloseHandle(HANDLE  hLocater);


//
//  This function gets the pair about the physical position on hard drive vs.
//  logical position in file( identified by hLocater ).
//
//  hLocater : Identifies the file to be located.The handle must have been
//	       previously returned by the OpenHandle service.
//  dwLogicSectorNum
//	     : Logical sector number in file.( I assume the file > 512 * 2 ^ 32 )
//  pqwPhysSectorNum
//	     : Physical sector number on hard drive about the approciate logical
//	       sector number in file.
//
//

BOOL	LogicSector2PhysSector(HANDLE hLocater,DWORD dwLogicSectorNum,
			       PQWORD pqwPhysSectorNum)



//
//  following, I demonstrate how use these function to locate a file (For examples,
//  c:\config.sys).
//

    //
    //	first, I must check wheather the FileLOcH.VxD is loaded
    //
    if(!GetVersion())
	return;

    //
    //	Open c:\config.sys use IFSMGR interface
    //
    int nRet = R0_OpenCreateFile(FALSE,ACCESS_READONLY | SHARE_COMPATIBILITY,
		    A_NORMAL,ACTION_OPENEXISTING,0,"c:\config.sys",
		    &hSysFile,&dwAction);
    if(nRet)
	return; //  open fail

    //
    //	Get file size of c:\config.sys
    //
    nRet = R0_GetFileSize(hSysFile,&dwFileSize);
    R0_CloseFile(hSysFile);

    if(nRet)
    {
	return;
    }

    HANDLE  hLoc = OpenHandle("c:\config.sys");
    if(NULL == hLoc)
	return;


    dwSectors = (dwFileSize + 512 - 1) / 512;

    for(
	dwCount = 0;
	dwCount < dwSectors;
	dwCount ++
       )
    {
	if(LogicSector2PhysSector(hLoc,dwCount,pqwPhysSectorNum))
	{
	    ...

	}
	else
	{
	    CloseHandle(hLoc);
	    return;
	}

    }	//  End For Loop

    CloseHandle(hLoc);
