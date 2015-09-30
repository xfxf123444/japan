#include "stdafx.h"
#include "AM01.h"
#include <shlobj.h>
#include "function.h"
#include "BackupState.h"
#include "ShutdownDlg.h"

extern WCHAR g_szPath[MAX_PATH];
BOOL g_bExecuting;

//-- Function for AMSetting begin
BOOL GetOsVersion()
{
	OSVERSIONINFO osvi;
	BOOL bOsVersionEx = FALSE;

	ZeroMemory(&osvi,sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx((OSVERSIONINFO *)&osvi);

	if (osvi.dwMajorVersion == 6)
		return TRUE;
	else
		return FALSE;
}

/*===============================================================
 *
 * Function Name: NewSelectFile();
 * Purpose:       Choose a file to save.
 * note:          NewSelectFile and SelectFile is similar, only
 *                Different in L"Open" and "Save AS" dialog type
 *                SelectFile() is put in AM01DLL
 *
 *===============================================================*/
BOOL NewSelectFile( WCHAR *szFileExt,WCHAR *szFileType,WCHAR *szFile )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	BOOL bResult = FALSE;
	CString strFilter;
	strFilter.Format(L"%s (*.%s)|*.%s||",szFileType,szFileExt,szFileExt);
	CFileDialog dlg (FALSE, szFileExt, NULL, OFN_HIDEREADONLY, strFilter);

	// if user selected a file then open it as a database
	if (IDOK == dlg.DoModal())
	{
		wcsncpy(szFile,dlg.GetPathName(),MAX_PATH-1);
		bResult = TRUE;
	}
	return bResult;
}

//-- Function for AMSetting end



/*====================================================
 *  
 *  Function Name: GetMyDocPath()
 *  Purpose: Read the path of L"My Document" from Reg
 *
 *===================================================*/
BOOL GetMyDocPath( WCHAR *szMyDocPath )
{

	HKEY hKEY;

	WCHAR szMyDocReg[MAX_PATH];

	long ret0;

	wcsncpy(szMyDocReg,L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",MAX_PATH-1);

	ret0=(::RegOpenKeyEx(HKEY_CURRENT_USER,szMyDocReg,0,KEY_READ,&hKEY));
	
	if(ERROR_SUCCESS != ret0)
	{
		TRACE(L"\nError read MyDoc reg.");
		return FALSE;
	}
	
	WCHAR szRegValue[MAX_PATH];
	memset(szRegValue,0,sizeof(szRegValue));
	
	DWORD dwDataType=REG_SZ;
	
	DWORD dwDataSize=MAX_PATH * sizeof(WCHAR);

	CString strName;
	
	strName = L"Personal";

	long ret2=::RegQueryValueEx(hKEY,(LPCTSTR)strName,NULL,&dwDataType,(LPBYTE)szRegValue,&dwDataSize);
	
	if(ERROR_SUCCESS != ret2 )
	{
		TRACE(L"\nRead MyDoc reg error.");
		return FALSE;
	}
	else
	{

		wcsncpy(szMyDocPath,(WCHAR *)szRegValue,MAX_PATH-1);
		return TRUE;
	}
}



/*=======================================================
 *
 * Function Name: RefreshSettingInfo()
 * Purpose:       Init column in ListCtrl.
 *                Since there are two ListCtrl, I take the 
 *                common part to one function. 
 *
 *=======================================================*/

BOOL InitSettingControl(CListCtrl *pSettingList,CImageList *pImageList)
{
	CString		csHeading;
	LV_COLUMN	Column;
	CRect		ListRect;

	(*pImageList).Create(IDB_STATEICONS,16,0,RGB(255,255,255));
	(*pSettingList).SetImageList(pImageList,LVSIL_STATE);

	(*pSettingList).GetClientRect(ListRect);

	Column.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
	Column.fmt = LVCFMT_LEFT;
	Column.cx  = ListRect.right-ListRect.left;
	csHeading.LoadString(IDS_SETTING_TITLE);
	Column.pszText = csHeading.GetBuffer(0);
	Column.cchTextMax = 0;		// ignored
	Column.iSubItem = 0;		
	(*pSettingList).InsertColumn(0,&Column);
	return TRUE;
}

/*===============================================================
 *
 * Function Name: RefreshSettingInfo()
 * Purpose:       Insert items in ListCtrl.
 *                Since there are two ListCtrl, I take the 
 *                common part to one function. 
 * 
 *===============================================================*/

BOOL RefreshSettingInfo(CStringArray *pSettingNameArray, CListCtrl *pSettingList)
{
	// SettingIndexArray is an extra array.
	CDWordArray SettingIndexArray;

	if (pSettingNameArray->GetSize()) pSettingNameArray->RemoveAll();

	if( FALSE == GetSettingArray(pSettingNameArray) )
	{
		TRACE(L"\nGetSettingArray error in RefreshSettingInfo.");
	}

	int i,nTotal,k;
	
	(*pSettingList).DeleteAllItems();
	
	nTotal = pSettingNameArray->GetSize();
	
	for(i=0; i < nTotal; i++)
	{
		k = (*pSettingList).InsertItem(i,pSettingNameArray->ElementAt(i));
   		(*pSettingList).SetItem(k,0,LVIF_STATE,L"",0,INDEXTOSTATEIMAGEMASK(2),LVIS_STATEIMAGEMASK,0);
	}
	return TRUE;
}

BOOL GetSelectedSetting( CListCtrl *pListCtrl,WCHAR *szSettingName )
{
	szSettingName[0]=L'\0';
	
	int nTotal,i;
		
	nTotal = (*pListCtrl).GetItemCount();

	if( 0 == nTotal )
	{
		return FALSE;
	}

	for (i = 0;i < nTotal;i ++)
	{
		if ( (*pListCtrl).GetItemState(i,LVIS_SELECTED))
		{
			wcsncpy(szSettingName, (LPCTSTR)( (*pListCtrl).GetItemText(i,0) ),MAX_SETTING_NAME_LEN);
			return TRUE;
			break;
		}
	}
	
	// no setting selected
	return FALSE;
}

BOOL ExecuteOneSetting(LPCTSTR szSettingName)
{
	BOOL bNewTarget;
	bNewTarget = FALSE;
	
	WCHAR szNewTarget[MAX_PATH];
	szNewTarget[0]=L'\0';

	return ( ExecuteOneSettingPro(szSettingName,bNewTarget,szNewTarget) );
}

// this function is for L"SnapShot"

BOOL ExecuteOneSettingPro(LPCTSTR szSettingName,BOOL bNewTarget, LPCTSTR szNewTargetName)
{
	if (!g_bExecuting)
	{
		CBackupState BackupStateDlg;
		g_bExecuting = TRUE;
		// memset(&BackupStateDlg.m_Setting,0,sizeof(BackupStateDlg.m_Setting));

		if (GetSettingInfo(szSettingName,&BackupStateDlg.m_Setting))
		{
			if(bNewTarget)
			{
				wcsncpy(BackupStateDlg.m_Setting.DataInfo.szTargetImage,szNewTargetName,MAX_PATH-1);
			}
			BackupStateDlg.m_Setting.DataInfo.BackupOption.bCmdLine = FALSE;
			BackupStateDlg.DoModal();
		}
		else
		{
			g_bExecuting = FALSE;

			TRACE(L"\nInitSettingInfo error in ExecuteOneSettingPro.");

			return FALSE;
		}
			
		g_bExecuting = FALSE;
	}
	else AfxMessageBox(IDS_BACKUP_RUNING,MB_OK | MB_ICONINFORMATION);

	return TRUE;
}

BOOL ExecuteSettingNewBase( WCHAR *szSettingName )
{
	AM01_PROG_SETTING SettingInfo;
	
	if( FALSE == GetSettingInfo(szSettingName,&SettingInfo) )
	{
		TRACE(L"\nInitSettingInfo error in ExecuteSettingNewBase.");
		return FALSE;
	}

	// the following is the backup a new file part
	BOOL bNewTarget;
	bNewTarget = TRUE;

	WCHAR szNewTargetName[MAX_PATH];
	szNewTargetName[0]=L'\0';
	
	if( FALSE == GetNextFileName(SettingInfo.DataInfo.szTargetImage,szNewTargetName,1) )
	{
		TRACE(L"\nGetNextFileName error in ExecuteSettingNewBase.");
		return FALSE;
	}

	if( FALSE == ExecuteOneSettingPro(szSettingName,bNewTarget,szNewTargetName) )
	{
		TRACE(L"\nExecuteOneSettingPro error in ExecuteSettingNewBase.");
		return FALSE;
	}

	if( FALSE == CreateDirFile(szNewTargetName) )
	{
		TRACE(L"\nCreateDirFile error in ExecuteSettingNewBase.");
	}

	return TRUE;

}

void FileSettingToProgSetting(AM01_FILE_SETTING *pFileSetting,AM01_PROG_SETTING *pProgSetting)
{
	pProgSetting->DataInfo = pFileSetting->DataInfo;
}

void ProgSettingToFileSetting(AM01_PROG_SETTING *pProgSetting, AM01_FILE_SETTING *pFileSetting)
{
	pFileSetting->DataInfo = pProgSetting->DataInfo;
}

void GetWeekDayString(ULONG nDay,CString &strText)
{
	switch (nDay)
	{
	case 0:
		strText.LoadString(IDS_SUNDAY);
		break;
	case 1:
		strText.LoadString(IDS_MONDAY);
		break;
	case 2:
		strText.LoadString(IDS_TUESDAY);
		break;
	case 3:
		strText.LoadString(IDS_WEDNESDAY);
		break;
	case 4:
		strText.LoadString(IDS_THURSDAY);
		break;
	case 5:
		strText.LoadString(IDS_FRIDAY);
		break;
	case 6:
		strText.LoadString(IDS_SATURDAY);
		break;
	default:
		strText.LoadString(IDS_UNKNOW);
		break;
	}
	return;
}

BOOL AddOneSetting(AM01_PROG_SETTING *pProgSetting)
{
	// SettingInfo, pass by value
	// will not influence the value in the main function

    // while add one setting, make the member of bEnable to TRUE
	__int64 lFilePointer;
	CStringArray SelectionArray;

	// store the content of SettingDataInfo.pcaSource
	// to SelectionArray

	SelectionArray.Append(pProgSetting->pcaSource);

	AM01_FILE_SETTING SettingDataInfo;

	SettingDataInfo.bEnable = TRUE;
	SettingDataInfo.DataInfo = pProgSetting->DataInfo;
	SettingDataInfo.dwNumofSource = SelectionArray.GetSize();
	
	BOOL bSettingNameExist;
	bSettingNameExist = FALSE;

	BOOL bError;
	bError = FALSE;

	if(FALSE == IsSettingNameExist(SettingDataInfo.DataInfo.szSettingName,&bSettingNameExist))
	{
		TRACE(L"\nIsSettingNameExist function error in AddOneSetting function.");
		return FALSE;
	}

    if( bSettingNameExist )
	{
	   return FALSE;
    }

	CString strProgSettingDataPath;

	if ( FALSE == GetSettingFileName(strProgSettingDataPath) )
	{
	   TRACE(L"\nError get setting file name in function AddOneSetting.");
	   return FALSE;
	}

	if( -1 != GetFileAttributes(strProgSettingDataPath) )
	{
		// here to check the file head of the SettingFile
		if( FALSE == IsFileBackupSettingFile((LPCTSTR)strProgSettingDataPath) )
		{
			TRACE(L"\nIsFileBackupSettingFile error in AddOneSetting, failed to AddOneSetting.");
			return FALSE;
		}
	}
	else
	{
		if( FALSE == RecordSettingHead( (LPCTSTR)strProgSettingDataPath ) )
		{
			TRACE(L"\nRecordSettingHead error in AddOneSetting, failed to AddOneSetting.");
			return FALSE;
		}
	}


    HANDLE hProgSettingDataFile;
	hProgSettingDataFile=NULL;
	   
	hProgSettingDataFile=CreateFile((LPCTSTR)strProgSettingDataPath,
		                             GENERIC_READ|GENERIC_WRITE,
								     FILE_SHARE_READ,NULL,
								     OPEN_EXISTING,
								     FILE_ATTRIBUTE_NORMAL,
								     NULL);
	
	if( INVALID_HANDLE_VALUE == hProgSettingDataFile)
	{
		TRACE(L"\nError open setting data file while adding one setting.");
		return FALSE;
	}
	
	DWORD dwWrittenBytes;
	
	DWORD dwReadBytes;

	SETTING_FILE_HEAD SettingFileHead;

	YGSetFilePointer(hProgSettingDataFile,0,FILE_BEGIN,lFilePointer);
	if( FALSE == ReadFile(hProgSettingDataFile,&SettingFileHead,sizeof(SettingFileHead),&dwReadBytes,NULL) )
	{
		TRACE(L"\nReadFile error in AddOneSetting.");
	}
	SettingFileHead.dwSettingCount = SettingFileHead.dwSettingCount + 1;

	YGSetFilePointer(hProgSettingDataFile,0,FILE_BEGIN,lFilePointer);
	WriteFile(hProgSettingDataFile,&SettingFileHead,sizeof(SettingFileHead),&dwWrittenBytes,NULL);

	YGSetFilePointer(hProgSettingDataFile,0,FILE_END,lFilePointer);

	// this value is assigned before
	SettingDataInfo.dwNumofSource = SelectionArray.GetSize();

	// Before writing to file, encrypt the password

	if(SettingDataInfo.DataInfo.BackupOption.bEncrypt)
	{
	   EncryptPassword((WCHAR *)(SettingDataInfo.DataInfo.BackupOption.pchPassword),PASSWORD_SIZE);
	}
	else
	{
	   memset(SettingDataInfo.DataInfo.BackupOption.pchPassword,0,PASSWORD_SIZE*sizeof(WCHAR));
	}

	WriteFile(hProgSettingDataFile,&SettingDataInfo,sizeof(SettingDataInfo),&dwWrittenBytes,NULL);

	WCHAR szOneSourceItem[MAX_PATH];

	int nSourceIndex;

	for(nSourceIndex=0; nSourceIndex < SelectionArray.GetSize(); nSourceIndex++)
	{
	    memset(szOneSourceItem,0,sizeof(szOneSourceItem));
		wcsncpy(szOneSourceItem,(LPCTSTR)(SelectionArray.GetAt(nSourceIndex)),MAX_PATH);
		if( FALSE == WriteFile(hProgSettingDataFile,szOneSourceItem,sizeof(szOneSourceItem),&dwWrittenBytes,NULL) )
		{
			bError = TRUE;
		}
	}

	CloseHandle(hProgSettingDataFile);
	hProgSettingDataFile=NULL;

	return TRUE;
}

BOOL IsSettingNameExist(LPCTSTR SettingName,BOOL *pbSettingNameExist)
{
	*pbSettingNameExist = FALSE;

	CStringArray SettingNameArray;

	CString strProgSettingDataPath;
	GetSettingFileName(strProgSettingDataPath);

	if( -1 == GetFileAttributes( (LPCTSTR)strProgSettingDataPath ) )
	{
	    // the seeting file is not exist
		// then the setting name is not exist
		*pbSettingNameExist = FALSE;
		return TRUE;
	}

	if( FALSE == GetSettingArray(&SettingNameArray) )
	{
		TRACE(L"\nGetSettingIndexInfo error in bSettingName.");
		return FALSE;
	}
	long i;
	for( i=0; i<SettingNameArray.GetSize(); i++)
	{
		if( 0 == SettingNameArray.ElementAt(i).CompareNoCase(SettingName) )
		{
			break;
		}
	}
	
	if(i<SettingNameArray.GetSize())
	{
		*pbSettingNameExist = TRUE;
	}
	else
	{
		*pbSettingNameExist = FALSE;
	}

	return TRUE;
}

/*==================================================================
 *
 * Function Name: GetSettingFileName()
 * Purpose:       get the setting data file and index file name
 *                Judge it exist or not
 * 
 * note:          if both not found return TRUE;
 *                if one found, the other not found return FALSE;
 *                if one found as directory return FALSE;
 *
 *=================================================================*/

BOOL GetSettingFileName(CString &strProgSettingDataPath)
{
	WCHAR    szSettingFile[MAX_PATH];

	if( FALSE == GetApplicationDataPath(szSettingFile) )
	{
		TRACE(L"\nGet Setting Dir error in GetSettingFileName.");
		return FALSE;
	}
	
	wcscat(szSettingFile,L"\\BackupMaster");
	CreateDirectory(szSettingFile,NULL);
	
	if (GetFileAttributes(szSettingFile) == -1) 
	{
		TRACE(L"\nGet Setting Dir error in GetSettingFileName.");
		return FALSE;
	}

	wcscat(szSettingFile,L"\\");
	wcscat(szSettingFile,SETTING_DATA_FILE);
	strProgSettingDataPath = szSettingFile;

	return TRUE;
}

BOOL IsFileBackupSettingFile(LPCTSTR szSettingFileName)
{
	// the setting file should be exist

	DWORD dwSettingDataAttribute;

	dwSettingDataAttribute = GetFileAttributes( LPCTSTR(szSettingFileName ) );

	if( -1 == dwSettingDataAttribute )
	{
		return FALSE;
	}

	HANDLE hSettingFile;
	hSettingFile = CreateFile( szSettingFileName,GENERIC_READ,FILE_SHARE_READ,NULL,
		                       OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if( INVALID_HANDLE_VALUE == hSettingFile )
	{
		TRACE(L"\nCreateFile error in IsUSBBackupSettingFile.");
		return FALSE;
	}

	SETTING_FILE_HEAD SettingFileHead;
	DWORD dwReadBytes;
	if( FALSE == ReadFile(hSettingFile,&SettingFileHead,sizeof(SettingFileHead),&dwReadBytes,NULL) )
	{
		TRACE(L"\nReadFile error in IsFileBackupSettingFile.");
		CloseHandle(hSettingFile);
		return FALSE;
	}

	if( 0 != wcscmp(SettingFileHead.szSettingIdentity,SETTING_FILE_IDENTITY) )
	{
		TRACE(L"\nThis is not an FileBackup setting file.");
		CloseHandle(hSettingFile);
		return FALSE;
	}
	else
	{
		TRACE(L"\nThis is an USBBackup setting file.");
	}

	CloseHandle(hSettingFile);

	return TRUE;
}

BOOL RecordSettingHead(LPCTSTR szSettingFile)
{
	HANDLE hSettingFile;
	hSettingFile = CreateFile(szSettingFile,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,
		                       NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

	if( INVALID_HANDLE_VALUE == hSettingFile )
	{
		TRACE(L"\nCreateFile error in RecordSettingHead.");
		return FALSE;
	}

	SETTING_FILE_HEAD SettingFileHead;
	memset(&SettingFileHead,0,sizeof(SettingFileHead));

	SettingFileHead.dwSettingCount = 0;
	wcsncpy(SettingFileHead.szSettingIdentity,SETTING_FILE_IDENTITY,SETTING_FILE_IDENTITY_SIZE);

	DWORD dwWrittenBytes;
	SettingFileHead.dwReserved=0x49494949;
	if( FALSE == WriteFile(hSettingFile,&SettingFileHead,sizeof(SettingFileHead),&dwWrittenBytes,NULL) )
	{
	   TRACE(L"\nWriteFile error in RecordSettingHead.");
	   CloseHandle(hSettingFile);
	   return FALSE;
	}
	CloseHandle(hSettingFile);
	return TRUE;
}

BOOL GetSettingArray(CStringArray *pSettingNameArray)
{
	__int64 lFilePointer;
	if (pSettingNameArray->GetSize()) pSettingNameArray->RemoveAll();

	CString strProgSettingDataPath;
	
	if( FALSE == GetSettingFileName(strProgSettingDataPath) )
	{
		TRACE(L"\nGetSettingFileName error in GetSettingArray.");
	}

	if( -1 == GetFileAttributes((LPCTSTR)strProgSettingDataPath) )
	{
		TRACE(L"\nSetting data file %s is not exist, GetSettingArray error.",(LPCTSTR)strProgSettingDataPath);
		return FALSE;
	}

	if( FALSE == IsFileBackupSettingFile((LPCTSTR)strProgSettingDataPath) )
	{
		TRACE(L"\n%s is not a USBBackup setting file.",(LPCTSTR)strProgSettingDataPath );
		SYSTEMTIME t;
		GetSystemTime(&t);
		WCHAR szBackupPath[MAX_PATH];
		ZeroMemory(szBackupPath, MAX_PATH * sizeof(WCHAR));
		swprintf(szBackupPath, L"%s_%d%d%d%d%d%d", strProgSettingDataPath, t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
		MoveFile(strProgSettingDataPath, szBackupPath);
		return FALSE;
	}

	HANDLE hSettingDataFile;
	hSettingDataFile = CreateFile( (LPCTSTR)strProgSettingDataPath,GENERIC_READ,FILE_SHARE_READ,
		                            NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if( INVALID_HANDLE_VALUE == hSettingDataFile )
	{
		TRACE(L"\nOpen setting data file error in GetSettingArray.");
		return FALSE;
	}

	SETTING_FILE_HEAD SettingFileHead;
	DWORD dwReadBytes;

	// we check the return code in the first ReadFile
	if( FALSE == ReadFile(hSettingDataFile,&SettingFileHead,sizeof(SettingFileHead),
		                  &dwReadBytes,NULL) )
	{
		TRACE(L"\nReadFile error in GetSettingArray.");
		CloseHandle(hSettingDataFile);
		return FALSE;
	}

	DWORD dwSettingIndex;

	AM01_FILE_SETTING SettingDataInfo;

	WCHAR szOneSourceItem[MAX_PATH];
	memset(szOneSourceItem,0,sizeof(szOneSourceItem));

	for( dwSettingIndex=0; dwSettingIndex < SettingFileHead.dwSettingCount; dwSettingIndex++ )
	{
		ReadFile(hSettingDataFile,&SettingDataInfo,sizeof(SettingDataInfo),&dwReadBytes,NULL);
		
		if(SettingDataInfo.bEnable)
		{
		   pSettingNameArray->Add(CString(SettingDataInfo.DataInfo.szSettingName));
		}

		// skip the selection array area
		YGSetFilePointer(hSettingDataFile,(SettingDataInfo.dwNumofSource)*sizeof(szOneSourceItem),FILE_CURRENT,lFilePointer);

	}

	CloseHandle(hSettingDataFile);
	hSettingDataFile=NULL;

	return TRUE;
}

BOOL GetSettingInfo(LPCTSTR szSettingName, AM01_PROG_SETTING *pProgSetting)
{
	__int64 lFilePointer;
	if (pProgSetting->pcaSource.GetSize()) pProgSetting->pcaSource.RemoveAll();	

	CString strProgSettingDataPath;
	
	if( FALSE == GetSettingFileName(strProgSettingDataPath) )
	{
		TRACE(L"\nGetSettingFileName error in GetSettingArray.");
	}

	if( FALSE == IsFileBackupSettingFile((LPCTSTR)strProgSettingDataPath) )
	{
		TRACE(L"\n%s is not a USBBackup setting file.",(LPCTSTR)strProgSettingDataPath );
		return FALSE;
	}

	HANDLE hSettingDataFile;
	hSettingDataFile = CreateFile( (LPCTSTR)strProgSettingDataPath,GENERIC_READ,FILE_SHARE_READ,
		                            NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if( INVALID_HANDLE_VALUE == hSettingDataFile )
	{
		TRACE(L"\nOpen setting data file error in GetSettingArray.");
		return FALSE;
	}

	SETTING_FILE_HEAD SettingFileHead;
	DWORD dwReadBytes;

	// we check the return code in the first ReadFile
	if( FALSE == ReadFile(hSettingDataFile,&SettingFileHead,sizeof(SettingFileHead),&dwReadBytes,NULL) )
	{
		TRACE(L"\nReadFile error in GetSettingArray.");
		CloseHandle(hSettingDataFile);
		return FALSE;
	}

	DWORD dwSettingIndex;

	AM01_FILE_SETTING OneSettingDataInfo;

	WCHAR szOneSourceItem[MAX_PATH];
	memset(szOneSourceItem,0,sizeof(szOneSourceItem));

	int  nSourceIndex;

	for( dwSettingIndex=0; dwSettingIndex < SettingFileHead.dwSettingCount ; dwSettingIndex++ )
	{
		ReadFile(hSettingDataFile,&OneSettingDataInfo,sizeof(OneSettingDataInfo),&dwReadBytes,NULL);
		// after read one setting, decrypt it

		if(OneSettingDataInfo.bEnable)
		{
			if( 0 == wcscmp(OneSettingDataInfo.DataInfo.szSettingName,szSettingName) )
			{
				// find one
				pProgSetting->DataInfo = OneSettingDataInfo.DataInfo;
				for(nSourceIndex=0; nSourceIndex<(int)(OneSettingDataInfo.dwNumofSource); nSourceIndex++ )
				{
					ReadFile(hSettingDataFile,szOneSourceItem,sizeof(szOneSourceItem),&dwReadBytes,NULL);
					pProgSetting->pcaSource.Add(szOneSourceItem);
				}
				
				YGSetFilePointer(hSettingDataFile,-((int)(OneSettingDataInfo.dwNumofSource))*sizeof(szOneSourceItem),FILE_CURRENT,lFilePointer);
				
				break;
			}
		}
		YGSetFilePointer(hSettingDataFile,(OneSettingDataInfo.dwNumofSource )*sizeof(szOneSourceItem),FILE_CURRENT,lFilePointer);
	}

	CloseHandle(hSettingDataFile);
	hSettingDataFile=NULL;

	return TRUE;
}

/*===================================================================
 *
 *  Function Name: ChangeSettingName()
 *  Purpose:       change the name of one setting
 *                 
 *  comment:       this fucntion is modified from GetSettingIndexInfo()
 *  
 *===================================================================*/

 // wait for continue, 2003.09.19  

BOOL ChangeSettingName(CString strOldSettingName, CString strNewSettingName)
{
	__int64 lFilePointer;
	CString strSettingName;
	strSettingName = strOldSettingName;
	
	CString strProgSettingDataPath;

	if( FALSE == GetSettingFileName(strProgSettingDataPath) )
	{
		TRACE(L"\nGetSettingFileName error in DeleteOneSetting.");
	}

	if( FALSE == IsFileBackupSettingFile((LPCTSTR)strProgSettingDataPath) )
	{
		TRACE(L"\n%s is not a FileBackup setting file.",(LPCTSTR)strProgSettingDataPath );
		return FALSE;
	}

	HANDLE hSettingDataFile;
	hSettingDataFile = CreateFile( (LPCTSTR)strProgSettingDataPath,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,
		                            NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if( INVALID_HANDLE_VALUE == hSettingDataFile )
	{
		TRACE(L"\nOpen setting data file error in GetSettingArray.");
		return FALSE;
	}

	SETTING_FILE_HEAD SettingFileHead;
	DWORD dwReadBytes;

	// we check the return code in the first ReadFile
	if( FALSE == ReadFile(hSettingDataFile,&SettingFileHead,sizeof(SettingFileHead),
		                  &dwReadBytes,NULL) )
	{
		TRACE(L"\nReadFile error in DeleteOneSetting.");
		CloseHandle(hSettingDataFile);
		return FALSE;
	}

	DWORD dwSettingIndex;

	AM01_FILE_SETTING SettingDataInfo;

	WCHAR szOneSourceItem[MAX_PATH];
	memset(szOneSourceItem,0,sizeof(szOneSourceItem));

	DWORD dwWrittenBytes;

	long nOffset;
	nOffset = 0;
	nOffset -= sizeof(SettingDataInfo);

	for( dwSettingIndex=0; dwSettingIndex < SettingFileHead.dwSettingCount; dwSettingIndex++ )
	{
		ReadFile(hSettingDataFile,&SettingDataInfo,sizeof(SettingDataInfo),&dwReadBytes,NULL);
		
		if( 
			( 0 == strSettingName.Compare(SettingDataInfo.DataInfo.szSettingName) ) &&
			( SettingDataInfo.bEnable )
		  )
		{
			memset(SettingDataInfo.DataInfo.szSettingName,0,sizeof(SettingDataInfo.DataInfo.szSettingName));
			
			wcsncpy(SettingDataInfo.DataInfo.szSettingName,(LPCTSTR)strNewSettingName,MAX_SETTING_NAME_LEN);

			YGSetFilePointer(hSettingDataFile,nOffset,FILE_CURRENT,lFilePointer);
			if( FALSE == WriteFile(hSettingDataFile,&SettingDataInfo,sizeof(SettingDataInfo),&dwWrittenBytes,NULL) )
			{
				TRACE(L"\nWriteFile error in DeleteOneSetting.");
			}
		}

		// skip the data area of selection array
		YGSetFilePointer(hSettingDataFile,(SettingDataInfo.dwNumofSource)*sizeof(szOneSourceItem),FILE_CURRENT,lFilePointer);
	}

	CloseHandle(hSettingDataFile);
	hSettingDataFile=NULL;

	return TRUE;	
}

BOOL DeleteOneSetting(LPCTSTR szSettingName)
{
	__int64 lFilePointer;
	CString strSettingName;
	strSettingName = szSettingName;
	
	CString strProgSettingDataPath;

	if( FALSE == GetSettingFileName(strProgSettingDataPath) )
	{
		TRACE(L"\nGetSettingFileName error in DeleteOneSetting.");
	}

	if( FALSE == IsFileBackupSettingFile((LPCTSTR)strProgSettingDataPath) )
	{
		TRACE(L"\n%s is not a FileBackup setting file.",(LPCTSTR)strProgSettingDataPath );
		return FALSE;
	}

	HANDLE hSettingDataFile;
	hSettingDataFile = CreateFile( (LPCTSTR)strProgSettingDataPath,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,
		                            NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if( INVALID_HANDLE_VALUE == hSettingDataFile )
	{
		TRACE(L"\nOpen setting data file error in GetSettingArray.");
		return FALSE;
	}

	SETTING_FILE_HEAD SettingFileHead;
	DWORD dwReadBytes;

	// we check the return code in the first ReadFile
	if( FALSE == ReadFile(hSettingDataFile,&SettingFileHead,sizeof(SettingFileHead),
		                  &dwReadBytes,NULL) )
	{
		TRACE(L"\nReadFile error in DeleteOneSetting.");
		CloseHandle(hSettingDataFile);
		return FALSE;
	}

	DWORD dwSettingIndex;

	AM01_FILE_SETTING SettingDataInfo;

	WCHAR szOneSourceItem[MAX_PATH];
	memset(szOneSourceItem,0,sizeof(szOneSourceItem));

	DWORD dwWrittenBytes;

	long nOffset;
	nOffset=0;
	nOffset -= sizeof(SettingDataInfo);

	for( dwSettingIndex=0; dwSettingIndex < SettingFileHead.dwSettingCount; dwSettingIndex++ )
	{
		ReadFile(hSettingDataFile,&SettingDataInfo,sizeof(SettingDataInfo),&dwReadBytes,NULL);
		
		if( 
			( 0 == strSettingName.Compare(SettingDataInfo.DataInfo.szSettingName) ) &&
			( SettingDataInfo.bEnable )
		  )
		{
			SettingDataInfo.bEnable = FALSE;
			YGSetFilePointer(hSettingDataFile,nOffset,FILE_CURRENT,lFilePointer);
			if( FALSE == WriteFile(hSettingDataFile,&SettingDataInfo,sizeof(SettingDataInfo),&dwWrittenBytes,NULL) )
			{
				TRACE(L"\nWriteFile error in DeleteOneSetting.");
			}
		}

		// skip the data area of selection array
		YGSetFilePointer(hSettingDataFile,(SettingDataInfo.dwNumofSource)*sizeof(szOneSourceItem),FILE_CURRENT,lFilePointer);
	}

	CloseHandle(hSettingDataFile);
	hSettingDataFile=NULL;

	return TRUE;	
}

BOOL GetScheduleFileName( WCHAR *szScheduleFileName )
{
	if( FALSE == GetApplicationDataPath(szScheduleFileName) )
	{
		TRACE(L"\nGet Setting Dir error in GetScheduleFileName.");
		return FALSE;
	}
	
	wcscat(szScheduleFileName,L"\\BackupMaster");
	CreateDirectory(szScheduleFileName,NULL);
	
	if (GetFileAttributes(szScheduleFileName) == -1) 
	{
		TRACE(L"\nGet Setting Dir error in GetScheduleFileName.");
		return FALSE;
	}
	
	wcscat(szScheduleFileName,L"\\");
	wcscat(szScheduleFileName,AM_SCHEDULE_FILE);

	return TRUE;
}


BOOL CreateScheduleFile(LPCTSTR szScheduleFileName)
{
	// if the schedule file is not exist
	// then create a new file
	
	HANDLE hScheduleFile;

	hScheduleFile = CreateFile(szScheduleFileName,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,
			        NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

	if( INVALID_HANDLE_VALUE == hScheduleFile )
	{
		TRACE(L"\nCan not create schedule file.");
		return FALSE;
	}

	SCHEDULE_HEAD ScheduleHead;
	
	memset(&ScheduleHead,0,sizeof(ScheduleHead));
	ScheduleHead.dwDataSize = sizeof(SCHEDULE_DATA);
	wcsncpy(ScheduleHead.szScheduleIdentity,AM_SCHEDULE_IDENTITY,IMAGE_IDENTITY_SIZE);
	ScheduleHead.dwSettingNumber = 0; // Add one setting number

	DWORD dwWrittenBytes;
	
	WriteFile(hScheduleFile,&ScheduleHead,sizeof(ScheduleHead),&dwWrittenBytes,NULL);
	if( dwWrittenBytes != sizeof(ScheduleHead) )
	{
		TRACE(L"\nWrite schedule file error.");
		CloseHandle(hScheduleFile);
		return FALSE;
	}

	CloseHandle(hScheduleFile);
	return TRUE;
}

BOOL RecordSchedule(CSCHEDULE_ARRAY *pScheduleArray)
{
	__int64 lFilePointer;
	WCHAR szScheduleFileName[MAX_PATH];
	if( FALSE == GetScheduleFileName(szScheduleFileName) )
	{
		TRACE(L"\nGet schedule file name error.");
		return FALSE;
	}

	DWORD dwFileAttribute;
	dwFileAttribute = 0;
	dwFileAttribute = GetFileAttributes(szScheduleFileName) ;

	// if this is a directory return FALSE;
	if (
		 (-1 != dwFileAttribute ) &&
		 (0  != (dwFileAttribute & FILE_ATTRIBUTE_DIRECTORY) )
	   )
	{
		// if the path name is a directory
		TRACE(L"\nThe path name is a directory, can not create the schedule file.");
		return FALSE;
	}

	// if no this file, create one
	if( -1 == dwFileAttribute )
	{
		if( FALSE == CreateScheduleFile(szScheduleFileName) )
		{
			TRACE(L"\nError create schedule file.");
			return FALSE;
		}
	}

	HANDLE hScheduleFile;
	hScheduleFile = CreateFile( 
		                       (LPCTSTR)szScheduleFileName,
							   GENERIC_READ|GENERIC_WRITE,
							   FILE_SHARE_READ,
							   NULL,
							   OPEN_EXISTING,
							   FILE_ATTRIBUTE_NORMAL,
							   NULL
							);

	if( INVALID_HANDLE_VALUE == hScheduleFile )
	{
		TRACE(L"\nOpen setting file error in RecordSetting.");
		return FALSE;
	}

	DWORD dwReadBytes;

	SCHEDULE_HEAD ScheduleHead;
	
	if( FALSE == ReadFile(hScheduleFile,&ScheduleHead,sizeof(ScheduleHead),&dwReadBytes,NULL) )
	{
		TRACE(L"\nRead Setting file head in ReadSetting.");
		CloseHandle(hScheduleFile);
		return FALSE;
	}

	YGSetFilePointer(hScheduleFile,0,FILE_BEGIN,lFilePointer);

	ScheduleHead.dwSettingNumber = pScheduleArray->GetSize();

	DWORD dwWrittenBytes;
	if( FALSE == WriteFile(hScheduleFile,&ScheduleHead,sizeof(ScheduleHead),&dwWrittenBytes,NULL) )
	{
		TRACE(L"\nWriteFile error in RecordingSetting.");
		CloseHandle(hScheduleFile);
		return FALSE;
	}

	int nItemIndex;
	for(nItemIndex=0; nItemIndex<pScheduleArray->GetSize(); nItemIndex++)
	{
		if( FALSE == WriteFile(hScheduleFile,&pScheduleArray->GetAt(nItemIndex),sizeof(SCHEDULE_DATA),&dwWrittenBytes,NULL) )
		{
			TRACE(L"\nWrite ScheduleItem error in RecordSetting.");
			CloseHandle(hScheduleFile);
			return FALSE;
		}
	}

	CloseHandle(hScheduleFile);
	return TRUE;
}

/*=============================================================================
 *
 *  Function Name: ReadScheduleInfo()
 *  Purpose:       Read Schedule Info from the task file.
 *  Return value:  return FALSE if can not find the task information
 *                 return TRUE if the setting have schedule information
 *
 *============================================================================*/

BOOL ReadScheduleInfo(LPCTSTR szSettingName,SCHEDULE_DATA *pScheduleReadData)
{
	WCHAR szScheduleFileName[MAX_PATH];
	memset(pScheduleReadData,0,sizeof(SCHEDULE_DATA));
	
	if( FALSE == GetScheduleFileName(szScheduleFileName) )
	{
		TRACE(L"\nGetScheduleFileName error while ReadScheduleInfo");
		return FALSE;
	}

	HANDLE hScheduleFile;

	hScheduleFile = CreateFile(szScheduleFileName,GENERIC_READ,FILE_SHARE_READ,
			        NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if( INVALID_HANDLE_VALUE == hScheduleFile )
	{
		TRACE(L"\nCan not open schedule file.");
		return FALSE;
	}

	SCHEDULE_HEAD ScheduleFileHead;
	SCHEDULE_DATA ScheduleFileData;

	memset(&ScheduleFileHead,0,sizeof(ScheduleFileHead));
	memset(&ScheduleFileData,0,sizeof(ScheduleFileData));

	DWORD dwReadBytes;

	ReadFile(hScheduleFile,&ScheduleFileHead,sizeof(ScheduleFileHead),&dwReadBytes,NULL);

	if( 0 != wcscmp(ScheduleFileHead.szScheduleIdentity,AM_SCHEDULE_IDENTITY) )
	{
		TRACE(L"\nNot AM01 Schedule file.");
		CloseHandle(hScheduleFile);
		return FALSE;
	}

	DWORD  dwSettingIndex;

	BOOL bSettingNameFound;
	bSettingNameFound = FALSE;

	for(dwSettingIndex=0; dwSettingIndex<ScheduleFileHead.dwSettingNumber; dwSettingIndex++)
	{
		ReadFile(hScheduleFile,&ScheduleFileData,sizeof(ScheduleFileData),&dwReadBytes,NULL);

		if(ScheduleFileData.nScheduleType == NORMAL_SCHEDULE &&
			!wcsicmp(szSettingName,ScheduleFileData.szSettingName) )
		{
			(*pScheduleReadData) = ScheduleFileData;
			bSettingNameFound = TRUE;
			break;
		}
	}

	CloseHandle(hScheduleFile);

	if( FALSE == bSettingNameFound )
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

/*===========================================================
 *  
 *  Function Name: GetScheduleArray()
 *  Purpose:       Load ScheduleArray to memory
 *                 ScheduleArray will be changed if schedule
 *                 is modified
 *
 *===========================================================*/

BOOL GetScheduleArray(CSCHEDULE_ARRAY *pScheduleArray)
{
	WCHAR szScheduleFileName[MAX_PATH];
    HANDLE hScheduleFile;

	if (pScheduleArray->GetSize()) pScheduleArray->RemoveAll();
	GetScheduleFileName(szScheduleFileName);
	hScheduleFile = CreateFile(szScheduleFileName,GENERIC_READ,FILE_SHARE_READ,
			        NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if( INVALID_HANDLE_VALUE == hScheduleFile )
	{
		TRACE(L"\nCan not open schedule file.");
		return FALSE;
	}

	SCHEDULE_HEAD ScheduleFileHead;
	SCHEDULE_DATA ScheduleFileData;

	memset(&ScheduleFileHead,0,sizeof(ScheduleFileHead));
	memset(&ScheduleFileData,0,sizeof(ScheduleFileData));

	DWORD dwReadBytes;

	ReadFile(hScheduleFile,&ScheduleFileHead,sizeof(ScheduleFileHead),&dwReadBytes,NULL);

	DWORD  dwSettingIndex;

	for(dwSettingIndex=0; dwSettingIndex<ScheduleFileHead.dwSettingNumber; dwSettingIndex++)
	{
		ReadFile(hScheduleFile,&ScheduleFileData,sizeof(ScheduleFileData),&dwReadBytes,NULL);

		// now ScheduleFileData.bHaveSchedule is not used
		// in the future it may be used

		pScheduleArray->Add(ScheduleFileData);
	}
	CloseHandle(hScheduleFile);
	return TRUE;
}
