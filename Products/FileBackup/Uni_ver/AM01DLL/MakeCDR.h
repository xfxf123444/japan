#ifndef AM_MAKE_CDR_DLL_H
#define AM_MAKE_CDR_DLL_H

#include "stdafx.h"
#include "..\AM01Struct\AM01Struct.h"

BOOL GetLastTargetNumber(LPCTSTR szLastImage,
						 unsigned __int64 qwMaxFileSize,
						 DWORD *pdwLastTarget);

BOOL GetDataSize(CDATA_ARRAY &SourceArray, __int64 *pDataSize);

BOOL GetTotalOffset( CDATA_ARRAY &SourceArray,
					  DWORD dwSourceNumber,
					  __int64 SourceOffset,
					  __int64 *pTotalOffset);

BOOL  GetOneFile ( LPCTSTR		     szLastImage,
				   LPCTSTR			 szBufferDir,
				   unsigned __int64	 qwMaxFileSize,
				   DWORD			 dwTargetNumber,
				   TABLE_DATA		 *pTargetData
				 );

BOOL GetRequiredImage(char *szImageFile, DWORD dwFileNumber, CDATA_ARRAY &ImageArray);

BOOL NewGetNewTargetHandle( HANDLE &hCurrentTarget,
						    LPCTSTR szBufferDir,
						    LPCTSTR szLastFileName,
						    DWORD dwFileNumber,
						    char *szTargetName);

BOOL GetSubFilePointer(CDATA_ARRAY &FileArray, __int64 TotalOffset, DWORD *pdwFileNumber, LARGE_INTEGER *pSubFilePointer);

BOOL GetMaxTotalOffset(CDATA_ARRAY &SourceArary, __int64 *pMaxTotalOffset);

BOOL NewRecordTail(
				    LPCTSTR           szLastImage,
				    LPCTSTR           szBufferDir,
					CDATA_ARRAY       &TargetArray,
				    unsigned __int64  qwMaxFileSize,
					BOOL             *pbNewFile,
					char             *szNewTarget
				  );

BOOL CopyIndexFile(HANDLE hLastSource, HANDLE hLastTarget, LARGE_INTEGER TotalIndexPointer,CDATA_ARRAY &SourceArray, CDATA_ARRAY &TargetArray);

BOOL CopyLeftTail(HANDLE hLastSource, HANDLE hLastTarget, LARGE_INTEGER ArrayPointer, unsigned __int64 qwTailSize);

BOOL AdjustFileHead(const CDATA_ARRAY &TargetArray);

BOOL bMultiFile(LPCTSTR szLastImage);

#endif