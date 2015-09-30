#ifndef AM_RESTORE_DLL_H
#define AM_RESTORE_DLL_H

#include "stdafx.h"
#include "..\AM01Struct\AM01Struct.h"

BOOL bIsFileSelected(LPCTSTR szOnePath,AM_RESTORE_INFO RestoreInfo);

BOOL GetRestoreFileCount(AM_RESTORE_INFO RestoreInfo);

BOOL RestoreFile(AM_RESTORE_INFO RestoreInfo,CDATA_ARRAY &ImageArray);

BOOL SetRestoreState(RESTORE_STATE RestoreState);

BOOL RestoreOnePath( LPCTSTR OnePathString,
					 IMAGE_HEAD *pImageHead,
					 DWORD dwCurrentPathID,
				     DWORD dwTimeStamp,
					 CDATA_ARRAY &ImageArray,
				     CURRENT_RESTORE_INFO *pCurrentInfo,
					 HANDLE hTotalIndexFile,
					 BOOL bNewPlace);

BOOL RestoreFindFile(
					    LPCTSTR szImgFile,
					    LPCTSTR szTempDir,
				        DWORD dwPathID,
				        DWORD dwTimeStampNumber,
				        LPCTSTR szTargetFile
					  );

BOOL CheckOutlookExpressDir(AM_RESTORE_INFO RestoreInfo, ADJUST_MAIL_INFO &AdjustMailInfo);

BOOL GetRestoreState(RESTORE_STATE *pRestoreState);

BOOL SetRestoreInfo(AM_RESTORE_INFO RestoreInfo);

BOOL RestoreOneFile(
					 LPCTSTR               szOnePathString,
					 IMAGE_HEAD			   *pImageHead,
				     HANDLE	               hTotalIndexFile,
				     DWORD                 dwTotalPathCount, 
				     DWORD                 dwPathID, 
				     DWORD                 dwTimeStampNumber, 
				     CURRENT_RESTORE_INFO  *pCurrentInfo,
				     const CDATA_ARRAY     &ImageArray
			       );

BOOL bPathExist(LPCTSTR OnePath);

BOOL CreateOneDir(LPCTSTR szOneDir);

BOOL PrepareOnePath(LPCTSTR szOnePath);

BOOL RestoreOneDir( LPCTSTR	szDirPath, INDEX_DATA IndexData);

BOOL AdjustOnePathString(CString &OnePathString, AM_RESTORE_INFO RestoreInfo, ADJUST_MAIL_INFO AdjustMailInfo);

BOOL CheckFreeSpace( LPCTSTR DriveLetter ,  unsigned __int64 * pqwFreeSpace );

BOOL GetMoreSeperateFile(LPCTSTR szLastImage, LPCTSTR TargetDirectory, LPCTSTR TargetFileName,BOOL bResetImage);









#endif
