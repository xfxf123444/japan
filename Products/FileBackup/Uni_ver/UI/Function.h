#ifndef AM01_FUNCTION_HEAD_FILE_H
#define AM01_FUNCTION_HEAD_FILE_H

#include "stdafx.h"
#include "../AM01Struct/AM01Struct.h"
#include "../AM01Expt/AM01Expt.h"

const WCHAR OUTLOOK_MASTER_KEY[MAX_PATH] = L"9207f3e0a3b11019908b08002b2a56c2";
const WCHAR OUTLOOK_ACCOUNT_REG_9X[MAX_PATH] = L"Software\\Microsoft\\Windows Messaging Subsystem\\Profiles";
const WCHAR OUTLOOK_ACCOUNT_REG[MAX_PATH] = L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Windows Messaging Subsystem\\Profiles";

BOOL GetOsVersion();

BOOL NewSelectFile(WCHAR *szFileExt,WCHAR *szFileType,WCHAR *szFile);

BOOL GetMyDocPath(WCHAR *szMyDocPath);

BOOL InitSettingControl(CListCtrl *pSettingList,CImageList *pImageList);
BOOL RefreshSettingInfo(CStringArray *pSettingNameArray, CListCtrl *pSettingList);
BOOL GetSelectedSetting(CListCtrl *pListCtrl,WCHAR *szSettingName);

BOOL ExecuteOneSetting(LPCTSTR szSettingName);
BOOL ExecuteOneSettingPro(LPCTSTR szSettingName,BOOL bNewTargetName, LPCTSTR szNewTargetName);
BOOL ExecuteSettingNewBase(WCHAR *szSettingName);

void FileSettingToProgSetting(AM01_FILE_SETTING *pFileSetting,AM01_PROG_SETTING *pProgSetting);
void ProgSettingToFileSetting(AM01_PROG_SETTING *pProgSetting, AM01_FILE_SETTING *pFileSetting);
void GetWeekDayString(ULONG nDay,CString &strWeekDay);

BOOL GetSettingFileName(CString &strProgSettingDataPath);
BOOL IsFileBackupSettingFile(LPCTSTR szSettingFileName);
BOOL AddOneSetting(AM01_PROG_SETTING *pProgSetting);
BOOL DeleteOneSetting(LPCTSTR szSettingName);
BOOL RecordSettingHead(LPCTSTR szSettingFile);
BOOL GetSettingArray(CStringArray *pSettingNameArray);
BOOL GetSettingInfo(LPCTSTR szSettingName, AM01_PROG_SETTING *pProgSetting);
BOOL IsSettingNameExist(LPCTSTR SettingName,BOOL *pbSettingNameExist);
BOOL ChangeSettingName(CString strOldSettingName, CString strNewSettingName);

BOOL GetScheduleFileName(WCHAR *szScheduleFileName);
BOOL CreateScheduleFile(LPCTSTR szScheduleFileName);
BOOL RecordSchedule(CSCHEDULE_ARRAY *pScheduleArray);
BOOL ReadScheduleInfo(LPCTSTR szSettingName,SCHEDULE_DATA *pScheduleReadData);
BOOL GetScheduleArray(CSCHEDULE_ARRAY *pScheduleArray);

#endif