#ifndef AM_SQUEEZE_DLL_H
#define AM_SQUEEZE_DLL_H

#include "stdafx.h"
#include "..\AM01Struct\AM01Struct.h"

BOOL CopyOneFile( CURRENT_SQUEEZE_INFO   *pCurrentSqueezeInfo,    // here contain current source number and current handle, current target is always the last element of TargetArray
				  AM_FILE_HANDLE	     *pFileHandle,
				  BYTE                   *CopyString,      // compressed buffer
				  const CDATA_ARRAY      &SourceArray,     // Source Array, read only
				  CDATA_ARRAY            &TargetArray,
				  IMAGE_HEAD       *pImageHead
				 );   // Target Array, may be changed

BOOL CopyLastTimeStamp(LPCTSTR szSourceImage,LPCTSTR szTargetImage);

BOOL SqueezeImage(LPCTSTR szSourceImage, LPCTSTR szTargetImage);

void SetSqueezeCancel();

void GetSqueezeState(SQUEEZE_STATE *pSqueezeState);

BOOL AdjustMainName(LPCTSTR szSourceImage,LPCTSTR szTargetImage,WCHAR *szSourceMainName);

BOOL GetSqueezeFileHandle(LPCTSTR szSourceImage,LPCTSTR szSourceMainName,SQUEEZE_FILE_HANDLE &SqueezeFileHandle);

BOOL CloseSqueezeFileHandle(SQUEEZE_FILE_HANDLE &SqueezeFileHandle);

BOOL WriteSqueezeHead(AM_FILE_HANDLE *pFileHandle, IMAGE_HEAD *pOriginalHead,CDATA_ARRAY &TargetArray);

BOOL GetStampCount(HANDLE hImageFile, DWORD *pdwStampCount);

BOOL GetStampInfo(HANDLE hImageFile,DWORD dwStampNumber,STAMP_DATA &StampData);


BOOL  CancelSqueezeImage( SQUEEZE_FILE_HANDLE  &SqueezeFileHandle,
						  AM_FILE_HANDLE       *pFileHandle,
						  const CDATA_ARRAY    &TargetArray,
						  LPCTSTR              szSourceMainName,
						  LPCTSTR              szTargetMainName);

BOOL GetIndexData(HANDLE hTotalIndexFile, DWORD dwPathID, DWORD dwTimeStampNumber, INDEX_DATA &IndexData);

#endif