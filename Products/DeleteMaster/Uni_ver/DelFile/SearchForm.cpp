// SearchForm.cpp : implementation file
//

#include "stdafx.h"
#include "DelFile.h"
#include "SearchForm.h"
#include "Fun.h"

BOOL CheckSearchMatch(WIN32_FIND_DATAW *pFileInfo,PDMFINDSTRUCT pPara,BOOL bCheckName);
BOOL GetFileTypeDesc(WCHAR *szFileExt,WCHAR *szDesc);
BOOL SearchInDirectory(PDMFINDSTRUCT pFindParam,BOOL bFaintlyFind);

extern CListCtrl *g_pListView;

CString g_strCurPath;
/////////////////////////////////////////////////////////////////////////////
// CSearchForm

CSearchForm::CSearchForm(CWnd* pParent /*=NULL*/)
	: CDialog(CSearchForm::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSearchForm)
	m_CheckDate = FALSE;
	m_CheckSize = FALSE;
	m_CheckType = FALSE;
	m_DateStart = 0;
	m_DateEnd = 0;
	m_SearchName = _T("");
	m_FileSize = 0;
	//}}AFX_DATA_INIT
	m_bSearch = TRUE;
}

void CSearchForm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSearchForm)
	DDX_Control(pDX, IDC_COMBO_PATH, m_SearchPath);
	DDX_Control(pDX, IDC_SPIN_SIZE, m_SpinSize);
	DDX_Control(pDX, IDC_COMBO_SIZE_TYPE, m_SizeType);
	DDX_Control(pDX, IDC_COMBO_FILE_TYPE, m_FileType);
	DDX_Control(pDX, IDC_COMBO_DATE_TYPE, m_DateType);
	DDX_Check(pDX, IDC_CHECK_DATE, m_CheckDate);
	DDX_Check(pDX, IDC_CHECK_SIZE, m_CheckSize);
	DDX_Check(pDX, IDC_CHECK_TYPE, m_CheckType);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_START, m_DateStart);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_END, m_DateEnd);
	DDX_Text(pDX, IDC_FILE_TOSEARCH, m_SearchName);
	DDX_Text(pDX, IDC_FILE_SIZE, m_FileSize);
	DDV_MinMaxDWord(pDX, m_FileSize, 0, 999999);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSearchForm, CDialog)
	//{{AFX_MSG_MAP(CSearchForm)
	ON_BN_CLICKED(IDC_CHECK_DATE, OnCheckDate)
	ON_BN_CLICKED(IDC_CHECK_TYPE, OnCheckType)
	ON_BN_CLICKED(IDC_CHECK_SIZE, OnCheckSize)
	ON_BN_CLICKED(IDC_SEARCH, OnSearch)
	//}}AFX_MSG_MAP
	ON_COMMAND(IDC_SEARCH,OnSearch)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchForm message handlers

void CSearchForm::OnCheckDate() 
{
	InitControl();
	// TODO: Add your control notification handler code here
}

void CSearchForm::OnCheckType() 
{
	InitControl();
	// TODO: Add your control notification handler code here
}

void CSearchForm::OnCheckSize() 
{
	InitControl();
	// TODO: Add your control notification handler code here
	
}

void CSearchForm::OnInitialUpdate() 
{
	CString  strTemp;
	int      nItem;

	// TODO: Add your specialized code here and/or call the base class
	strTemp.LoadString(IDS_CREATE_TIME);
	nItem = m_DateType.AddString(strTemp);
	m_DateType.SetItemData(nItem,CREATE_TIME);

	strTemp.LoadString(IDS_LAST_MODIFY);
	nItem = m_DateType.SetCurSel(m_DateType.AddString(strTemp));
	m_DateType.SetItemData(nItem,LAST_MODIFY_TIME);

	strTemp.LoadString(IDS_LAST_ACCESS);
	nItem = m_DateType.AddString(strTemp);
	m_DateType.SetItemData(nItem,LAST_ACCESS_TIME);

	strTemp.LoadString(IDS_MIN);
	nItem = m_SizeType.SetCurSel(m_SizeType.AddString(strTemp));
	m_SizeType.SetItemData(nItem,MINSIZE);

	strTemp.LoadString(IDS_MAX);
	nItem = m_SizeType.AddString(strTemp);
	m_SizeType.SetItemData(nItem,MAXSIZE);

	InitFileType();

	m_DateEnd = CTime::GetCurrentTime();
	m_DateStart = m_DateEnd - CTimeSpan(365,0,0,0);

	m_SpinSize.SetRange(0,999999);
	m_SpinSize.SetBuddy((CWnd*)GetDlgItem(IDC_FILE_SIZE));
	m_SpinSize.SetPos(0);

	UpdateData(FALSE);
	EnableControl(TRUE);
	InitControl();
}

void CSearchForm::InitControl()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_DATETIMEPICKER_END)->EnableWindow(m_CheckDate);
	GetDlgItem(IDC_DATETIMEPICKER_START)->EnableWindow(m_CheckDate);
	GetDlgItem(IDC_COMBO_DATE_TYPE)->EnableWindow(m_CheckDate);
	GetDlgItem(IDC_SPIN_SIZE)->EnableWindow(m_CheckSize);
	GetDlgItem(IDC_COMBO_SIZE_TYPE)->EnableWindow(m_CheckSize);
	GetDlgItem(IDC_FILE_SIZE)->EnableWindow(m_CheckSize);
	GetDlgItem(IDC_COMBO_FILE_TYPE)->EnableWindow(m_CheckType);
	UpdateData(FALSE);
}

void CSearchForm::InitFileType()
{
	DWORD		dwIndex = 0,dwSize;
	WCHAR		szKeyName[MAX_PATH];
	WCHAR		szDescName[MAX_PATH];
	FILETIME    KeyTime;
	CString strTemp;

	strTemp.LoadString(IDS_ALL_TYPE);
	m_FileType.SetCurSel(m_FileType.AddString(strTemp));
	
	dwSize = MAX_PATH;
	while (ERROR_SUCCESS==
			RegEnumKeyEx(HKEY_CLASSES_ROOT,dwIndex,szKeyName,&dwSize,
							NULL,NULL,NULL,&KeyTime))
	{
		if (szKeyName[0] == L'.')
		{
			if (GetFileTypeDesc(szKeyName,szDescName))
			{
				strTemp.Format(L"(%s) %s",szKeyName,szDescName);
				m_FileType.AddString(strTemp);
			}
		}
		dwIndex ++;
		dwSize = MAX_PATH;
	}
	return;
}

void CSearchForm::OnSearch() 
{
	CString      strTemp;
	int          i;
	DWORD        dwThreadID;

	if (m_bSearch)
	{
		UpdateData(TRUE);
		i = m_SearchPath.GetCurSel();
		m_SearchPath.GetLBText(i,m_strSearchPath);
		if (CheckConflict()) return;

		g_pListView->DeleteAllItems();
		
		memset(&m_FindParam,0,sizeof(DMFINDSTRUCT));
		wcscpy(m_FindParam.szSearchPath,m_strSearchPath);
		wcscpy(m_FindParam.szNameMatch,m_SearchName);
		m_FindParam.bCheckDate = m_CheckDate;
		m_FindParam.dwDateType = m_DateType.GetItemData(m_DateType.GetCurSel());
		m_FindParam.TimeStart = m_DateStart;
		m_FindParam.TimeEnd = m_DateEnd;

		m_FindParam.bCheckSize = m_CheckSize;
		m_FindParam.dwSizeType = m_SizeType.GetItemData(m_SizeType.GetCurSel());
		m_FindParam.dwSize = m_FileSize*1024;

		m_FindParam.bCheckType = m_CheckType;
		m_FileType.GetLBText(m_FileType.GetCurSel(),strTemp);
		wcscpy(m_FindParam.szFileExt,&(strTemp.GetBuffer(0))[1]);

		i = 0;
		while (TRUE)
		{
			if (m_FindParam.szFileExt[i] == L'\0' || m_FindParam.szFileExt[i] == L')')
			{
				m_FindParam.szFileExt[i] = L'\0';
				break;
			}
			i ++;
		}
		
		m_FindParam.hParWnd = m_hWnd;
		strTemp.LoadString(IDS_CANCEL_SEARCH);
		GetDlgItem(IDC_SEARCH)->SetWindowText(strTemp);
		EnableControl(FALSE);
		m_bSearch = FALSE;
		m_hThreadHandle = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)CSearchForm::ThreadProc,(LPVOID)&m_FindParam,0,&dwThreadID);
	}
	else
	{
		TerminateThread(m_hThreadHandle,0);
		g_pListView->SortItems(FileListCompare,(DWORD)TRUE);
		strTemp.LoadString(IDS_SEARCH);
		GetDlgItem(IDC_SEARCH)->SetWindowText(strTemp);
		EnableControl(TRUE);
		InitControl();
		m_bSearch = TRUE;
	}
	// TODO: Add your control notification handler code here
	
}

BOOL CSearchForm::CheckConflict()
{
	if (m_SearchName == _T(""))
	{
		AfxMessageBox(IDS_EMPTY_NAME,MB_OK | MB_ICONINFORMATION);
		return TRUE;
	}

	if (m_CheckDate)
	{
		if (m_DateEnd < m_DateStart)
		{
			AfxMessageBox(IDS_DATE_ERROR,MB_OK | MB_ICONINFORMATION);
			return TRUE;
		}
	}

	if (GetFileAttributes(m_strSearchPath) == -1)
	{
		AfxMessageBox(IDS_INVALID_PATH,MB_OK | MB_ICONINFORMATION);
		return TRUE;
	}
	return FALSE;
}

void CSearchForm::EnableControl(BOOL bEnable)
{
	GetDlgItem(IDC_FILE_TOSEARCH)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_DATE)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_TYPE)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_SIZE)->EnableWindow(bEnable);
	GetDlgItem(IDC_DATETIMEPICKER_END)->EnableWindow(bEnable);
	GetDlgItem(IDC_DATETIMEPICKER_START)->EnableWindow(bEnable);
	GetDlgItem(IDC_COMBO_DATE_TYPE)->EnableWindow(bEnable);
	GetDlgItem(IDC_SPIN_SIZE)->EnableWindow(bEnable);
	GetDlgItem(IDC_COMBO_SIZE_TYPE)->EnableWindow(bEnable);
	GetDlgItem(IDC_FILE_SIZE)->EnableWindow(bEnable);
	GetDlgItem(IDC_COMBO_FILE_TYPE)->EnableWindow(bEnable);
}

DWORD WINAPI CSearchForm::ThreadProc(PDMFINDSTRUCT pPara)
{
	WCHAR *p;
	BOOL bFaintlyFind = FALSE;

	p = wcschr(pPara->szNameMatch,L'?');
	if (p) bFaintlyFind = TRUE;
	p = wcschr(pPara->szNameMatch,L'*');
	if (p) bFaintlyFind = TRUE;
	SearchInDirectory(pPara,bFaintlyFind);
	g_pListView->SortItems(FileListCompare,(DWORD)TRUE);

	::PostMessage(pPara->hParWnd,WM_COMMAND,IDC_SEARCH,0);
	return 0;
}

BOOL SearchInDirectory(PDMFINDSTRUCT pFindParam,BOOL bFaintlyFind)
{
	CString strText;
	CTime   FileTime;
	HANDLE hFind;
	WIN32_FIND_DATA FileInfo = {0};
	int    nLen,nItem;

	nLen = wcslen(pFindParam->szSearchPath);

	if (bFaintlyFind)
	{
		wcscat(pFindParam->szSearchPath,L"\\");
		wcscat(pFindParam->szSearchPath,pFindParam->szNameMatch);

		hFind = FindFirstFile(pFindParam->szSearchPath,&FileInfo);
		pFindParam->szSearchPath[nLen] = L'\0';

		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (wcscmp(FileInfo.cFileName,L".") &&
					wcscmp(FileInfo.cFileName,L".."))
				{
					if (CheckSearchMatch(&FileInfo,pFindParam,FALSE))
					{
						if (FileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
						{
							nItem = g_pListView->InsertItem(0,FileInfo.cFileName,2);
							g_pListView->SetItemData(nItem,g_pListView->GetItemCount()+0x10000);
						}
						else
						{
							nItem = g_pListView->InsertItem(0,FileInfo.cFileName,10);
							g_pListView->SetItemData(nItem,g_pListView->GetItemCount());
						}
						g_pListView->SetItemText(nItem,1,pFindParam->szSearchPath);
						FileTime = CTime(FileInfo.ftLastWriteTime);
						strText.Format(L"%d-%d-%d %d:%d",FileTime.GetYear(),FileTime.GetMonth(),FileTime.GetDay(),FileTime.GetHour(),FileTime.GetMinute());
						g_pListView->SetItemText(nItem,2,strText);
					}
				}
			}
			while (FindNextFile(hFind,&FileInfo));
			FindClose(hFind);
		}
	}

	wcscat(pFindParam->szSearchPath,L"\\*.*");

	hFind = FindFirstFile(pFindParam->szSearchPath,&FileInfo);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (wcscmp(FileInfo.cFileName,L".") &&
				wcscmp(FileInfo.cFileName,L".."))
			{
				pFindParam->szSearchPath[nLen] = L'\0';
				if (!bFaintlyFind)
				{
					if (CheckSearchMatch(&FileInfo,pFindParam,TRUE))
					{
						if (FileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
						{
							nItem = g_pListView->InsertItem(0,FileInfo.cFileName,2);
							g_pListView->SetItemData(nItem,g_pListView->GetItemCount()+0x10000);
						}
						else
						{
							nItem = g_pListView->InsertItem(0,FileInfo.cFileName,10);
							g_pListView->SetItemData(nItem,g_pListView->GetItemCount());
						}
						g_pListView->SetItemText(nItem,1,pFindParam->szSearchPath);
						FileTime = CTime(FileInfo.ftLastWriteTime);
						strText.Format(L"%d-%d-%d %d:%d",FileTime.GetYear(),FileTime.GetMonth(),FileTime.GetDay(),FileTime.GetHour(),FileTime.GetMinute());
						g_pListView->SetItemText(nItem,2,strText);
					}
				}
				if (FileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					pFindParam->szSearchPath[nLen] = L'\\';
					wcscpy(&pFindParam->szSearchPath[nLen+1],FileInfo.cFileName);
					SearchInDirectory(pFindParam,bFaintlyFind);
				}
			}
		}
		while (FindNextFile(hFind,&FileInfo));
		FindClose(hFind);
	}
	pFindParam->szSearchPath[nLen] = L'\0';

	return TRUE;
}

BOOL CheckSearchMatch(WIN32_FIND_DATA *pFileInfo,PDMFINDSTRUCT pPara,BOOL bCheckName)
{
	CString strName1,strName2;
	WCHAR *p;

	if (bCheckName)
	{
		strName1 = pFileInfo->cFileName;
		strName2 = pPara->szNameMatch;
		strName1.MakeUpper();
		strName2.MakeUpper();
		if (strName1.Find(strName2) == -1) return FALSE;
	}

	if (pPara->bCheckDate)
	{
		CTime CheckTime;
		switch (pPara->dwDateType)
		{
		case LAST_MODIFY_TIME:
			CheckTime = CTime(pFileInfo->ftLastWriteTime);
			break;
		case CREATE_TIME:
			CheckTime = CTime(pFileInfo->ftCreationTime);
			break;
		case LAST_ACCESS_TIME:
			CheckTime = CTime(pFileInfo->ftLastAccessTime);
			break;
		default:
			return FALSE;
			break;
		}
		if (CheckTime < pPara->TimeStart ||	CheckTime > pPara->TimeEnd)
			return FALSE;
	}

	if (pPara->bCheckType)
	{
		p = wcsrchr(pFileInfo->cFileName,L'.');
		if (p)
		{
			if (wcsicmp(p,pPara->szFileExt)) return FALSE;
		}
		else return FALSE;
	}

	if (pPara->bCheckSize)
	{
		switch(pPara->dwSizeType)
		{
		case MINSIZE:
			if (pFileInfo->nFileSizeLow < pPara->dwSize) return FALSE;
			break;
		case MAXSIZE:
			if (pFileInfo->nFileSizeLow > pPara->dwSize) return FALSE;
			break;
		default:
			return FALSE;
		}
	}

	return TRUE;
}

LRESULT CSearchForm::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch(message)  
	{  
	case  WM_REFRESH_ITEM: 
		m_SearchName = _T("");
		g_pListView->DeleteAllItems();
		InitPathComobo();
		break;  
	}  
	
	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL GetFileTypeDesc(WCHAR *szFileExt,WCHAR *szDesc)
{
	HKEY hKey,hDescKey;
	WCHAR	szDescKey[MAX_PATH];
	DWORD dwValSize;
	BOOL  bResult = FALSE;
	if (ERROR_SUCCESS == RegOpenKey(HKEY_CLASSES_ROOT,szFileExt,&hKey))
	{
		dwValSize = MAX_PATH;
		if (RegQueryValueEx(hKey,L"",NULL,NULL,(BYTE *)szDescKey,&dwValSize) == ERROR_SUCCESS)
		{
			if (ERROR_SUCCESS == RegOpenKey(HKEY_CLASSES_ROOT,szDescKey,&hDescKey))
			{
				dwValSize = MAX_PATH;
				if (RegQueryValueEx(hDescKey,L"",NULL,NULL,(BYTE *)szDesc,&dwValSize) == ERROR_SUCCESS)
					bResult = dwValSize;
				RegCloseKey(hDescKey);
			}
		}
		RegCloseKey(hKey);
	}
	return bResult;
}

BOOL CSearchForm::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	OnInitialUpdate();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSearchForm::InitPathComobo()
{
	DWORD       dwLen;
	DWORD		i;
	BOOL        bInit = FALSE;
	int			nType,nItem;
	WCHAR		*p;
	WCHAR		szLetter[4];
	WCHAR		szDrivesLetter[MAX_PATH];

	m_SearchPath.ResetContent();
	dwLen = GetLogicalDriveStrings(0,NULL);
	::GetLogicalDriveStrings(dwLen,szDrivesLetter);
	_wcsupr( szDrivesLetter );
	p = szDrivesLetter;

	dwLen = dwLen/4;
	for(i = 0;i<dwLen;i++)
	{
		wcscpy(szLetter,p);
		nType = GetLetterType(szLetter);
		szLetter[2] = 0;
		if (nType < 7)
		{
			nItem = m_SearchPath.InsertString(0,szLetter);
			if (!wcsicmp(szLetter,g_strCurPath))
			{
				m_SearchPath.SetCurSel(nItem);
				bInit = TRUE;
			}
		}
		p += 4;
	}
	if (!bInit)
	{
		nItem = m_SearchPath.InsertString(0,g_strCurPath);
		m_SearchPath.SetCurSel(nItem);
	}
}

void CSearchForm::OnOK() 
{
	return;	
}

