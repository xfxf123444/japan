//-- backup function begin

#ifndef   AM_BAK_EXPORT_H  // if have been included then skip all
#define   AM_BAK_EXPORT_H 

#include "..\AM01Struct\AM01Struct.h"

BOOL EXPORT CheckLastImage(LPCTSTR szLastImage, DWORD *pdwErrorCode);
BOOL BackupSelectionFile(SETTING_DATA_INFO SettingDataInfo,CStringArray &SelectionArray);
BOOL EXPORT DiffBackupSelectionFile(LPCTSTR ImageFilePath,PBACKUPOPTION pBackupOption);
BOOL EXPORT GetWorkState(AM01WORKSTATE *pWorkState);
BOOL EXPORT SetBakCancel();
BOOL EXPORT bMultiFile(LPCTSTR szLastImage);
BOOL EXPORT GetOneFile ( LPCTSTR		    szLastImage,
					     LPCTSTR			szBufferDir,
						 unsigned __int64	qwMaxFileSize,
						 DWORD			dwTargetNumber,
						 TABLE_DATA		*pTargetData
					    );
BOOL EXPORT NewRecordTail (  LPCTSTR           szLastImage,
							 LPCTSTR           szBufferDir,
							 CDATA_ARRAY       &TargetArray,
							 unsigned __int64  qwMaxFileSize ,
							 BOOL	           *pbNewFile,
							 WCHAR              *szNewTarget
						  );
BOOL EXPORT GetLastTargetNumber( LPCTSTR szLastImage,
								 unsigned __int64 qwMaxFileSize,
								 DWORD *pdwLastTarget);

//Fail Retry
BOOL EXPORT RecordFailRetrySchedule(PFAILRETRYSCHEDULE pScheduleItem);
BOOL EXPORT RemoveFailRetrySchedule(PFAILRETRYSCHEDULE pScheduleItem);
BOOL EXPORT ReadFailRetrySchedule(CFAILRETRYSCHEDULE &ScheduleItemArray);
BOOL EXPORT ExcuteFailRetrySchedule(PFAILRETRYSCHEDULE pScheduleItem);
BOOL EXPORT FreeFailRetrySchedule(CFAILRETRYSCHEDULE &ScheduleItemArray);

BOOL EXPORT GetApplicationDataPath(WCHAR *szPath);

BOOL EXPORT SearchLastImage(LPTSTR szImageFile, DWORD *pdwErrorCode);
#endif

//-- backup function end

#ifndef   AM01_EXPORT_H  // if have been included then skip all
#define   AM01_EXPORT_H 

//-- restore function begin

BOOL EXPORT RestoreFile(AM_RESTORE_INFO RestoreInfo,CDATA_ARRAY &ImageArray);

BOOL EXPORT GetSeperateFile(LPCTSTR szImageFile,LPCTSTR szMainImageName,LPCTSTR TargetDirectory);

BOOL EXPORT SelectFile(WCHAR *szFileExt,WCHAR *szFileType,WCHAR *szFile);

BOOL EXPORT	GetRestoreFileCount(AM_RESTORE_INFO RestoreInfo);

DWORD EXPORT GetTotalIndexOffset(DWORD dwMaxPathID, DWORD dwTimeStampCount, DWORD dwPathID);

BOOL EXPORT bPathExist(LPCTSTR OnePath);

BOOL EXPORT SelectFolder(HWND hWnd,WCHAR *ach);

BOOL EXPORT GetRestoreState(RESTORE_STATE *pRestoreState);

BOOL EXPORT SetRestoreState(RESTORE_STATE RestoreState);

BOOL EXPORT SetRestoreInfo(AM_RESTORE_INFO RestoreInfo);


//-- restore function end

//-- find function begin

BOOL EXPORT GetDataFilePointer( HANDLE hTotalIndexFile,
									DWORD dwMaxPathID,
									DWORD dwPathID, 
									DWORD dwTimeStampNumber, 
									INDEX_DATA *pIndexData);


BOOL EXPORT RestoreFindFile( LPCTSTR szImgFile,
		                           LPCTSTR szTempDir,
								   DWORD dwPathID,
								   DWORD dwTimeStampNumber,
								   LPCTSTR szTargetFile );

BOOL EXPORT GetIndexData(HANDLE hTotalIndexFile,
		                     DWORD dwPathID,
							 DWORD dwTimeStampNumber,
							 INDEX_DATA &IndexData);
//-- find function end

//-- function for segment begin
BOOL EXPORT bExistingFileNew( LPCTSTR SourceFilePath,
							  HANDLE  hArrayFile,
							  DWORD *pdwPathIndex );

BOOL EXPORT LoadFirstSegment(HANDLE hArrayFile);
//-- function for segment end

BOOL EXPORT SqueezeImage(LPCTSTR szSourceImage, LPCTSTR szTargetImage);
void EXPORT SetSqueezeCancel();
void EXPORT GetSqueezeState(SQUEEZE_STATE *pSqueezeState);

BOOL EXPORT GetLongMainName(LPCTSTR szImageFile,WCHAR *szLongMainName);

BOOL EXPORT NewGetOutlookExpressDataPath(WCHAR *szDataPath, WCHAR *szAddressPath);

BOOL EXPORT GetSelectionArrayPro(LPCTSTR szLastImage,CStringArray &SelectionArray);
BOOL EXPORT GetSelectionLink(LPCTSTR szLastImage,SELECTION_LINK **ppSelectionHead);
BOOL EXPORT FreeSelectionLink(SELECTION_LINK *pSelectionHead);
BOOL  EXPORT IsOneDirectory(LPCTSTR pstr);
BOOL EXPORT GetNextFileName(LPCTSTR szTargetFile, WCHAR *szDataFilePath,DWORD dwFileNumber);

WCHAR * EXPORT strrchrpro(WCHAR* szSource,int chFindChar);

BOOL EXPORT TrimRightChar(WCHAR *szOneString, const WCHAR chOneChar, const int nMaxCharCount);
BOOL EXPORT CreateOneDir(LPCTSTR szOneDir);
BOOL EXPORT GetRightPart(LPCTSTR szOnePath,WCHAR *szRightPart);

BOOL EXPORT	CreateDirFile(LPCTSTR szLastImage);

// 2004.05.20 added for encrypt begin
void EncryptPassword(WCHAR *pchPassword,int nPasswordSize);
void DecryptPassword(WCHAR *pchPassword,int nPasswordSize);
// 2004.05.20 added for encrypt end

// 2004.05.21 added begin

BOOL EXPORT	GetImageHead(const WCHAR *szLastImage,IMAGE_HEAD *pImageHead);

BOOL EXPORT	CheckImagePassword(LPCTSTR szImageFile);
BOOL EXPORT	GetSourceArray(LPCTSTR szLastFileName, CDATA_ARRAY &SourceArray);

BOOL EXPORT YGSetFilePointer(HANDLE hFile, __int64 linDistance, DWORD dwMoveMethod, __int64 &linTargetFilePointer);


BOOL FreeAMDataArray(CDATA_ARRAY &DataArray);
BOOL FreeAMStringArray(CStringArray &StringArray);

// 2004.05.21 added end

#endif
