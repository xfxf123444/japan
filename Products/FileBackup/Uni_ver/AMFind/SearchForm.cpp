// SearchForm.cpp : implementation file
//

#include "stdafx.h"
#include "AMFind.h"
#include "AMFindDoc.h"
#include "Mainfrm.h"
#include "AMFindView.h"
#include "SearchForm.h"

#include "..\AM01STRUCT\AM01Struct.h"
#include "..\AM01Expt\AM01Expt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern WCHAR g_szImgFile[MAX_PATH];
WCHAR g_szTempDir[MAX_PATH];
CSearchForm *g_pForm;

BOOL CheckSearchMatch(WCHAR *szFile,INDEX_DATA *pIndexData,PAMFINDSTRUCT pPara);

/////////////////////////////////////////////////////////////////////////////
// CSearchForm

IMPLEMENT_DYNCREATE(CSearchForm, CFormView)

CSearchForm::CSearchForm()
	: CFormView(CSearchForm::IDD)
{
	//{{AFX_DATA_INIT(CSearchForm)
	m_CheckDate = FALSE;
	m_CheckSize = FALSE;
	m_CheckType = FALSE;
//	m_ContainChar = _T("");
	m_DateStart = 0;
	m_DateEnd = 0;
	m_SearchName = _T("");
	m_FileSize = 0;
	//}}AFX_DATA_INIT
	m_bSearch = TRUE;
}

CSearchForm::~CSearchForm()
{
}

void CSearchForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSearchForm)
	DDX_Control(pDX, IDC_SPIN_SIZE, m_SpinSize);
	DDX_Control(pDX, IDC_COMBO_SIZE_TYPE, m_SizeType);
	DDX_Control(pDX, IDC_COMBO_TIMESTAMP, m_TimeStamp);
	DDX_Control(pDX, IDC_COMBO_FILE_TYPE, m_FileType);
	DDX_Control(pDX, IDC_COMBO_DATE_TYPE, m_DateType);
	DDX_Check(pDX, IDC_CHECK_DATE, m_CheckDate);
	DDX_Check(pDX, IDC_CHECK_SIZE, m_CheckSize);
	DDX_Check(pDX, IDC_CHECK_TYPE, m_CheckType);
//	DDX_Text(pDX, IDC_CONTAIN_CHAR, m_ContainChar);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_END, m_DateEnd);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_START, m_DateStart);
	DDX_Text(pDX, IDC_FILE_TOSEARCH, m_SearchName);
	DDX_Text(pDX, IDC_FILE_SIZE, m_FileSize);
	DDV_MinMaxDWord(pDX, m_FileSize, 0, 999999);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSearchForm, CFormView)
	//{{AFX_MSG_MAP(CSearchForm)
	ON_BN_CLICKED(IDC_CHECK_DATE, OnCheckDate)
	ON_BN_CLICKED(IDC_CHECK_TYPE, OnCheckType)
	ON_BN_CLICKED(IDC_CHECK_SIZE, OnCheckSize)
	ON_BN_CLICKED(IDC_SEARCH, OnSearch)
	//}}AFX_MSG_MAP
	ON_COMMAND(IDC_SEARCH,OnSearch)
	ON_COMMAND(ID_FILE_OPEN, OnMyFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchForm diagnostics

#ifdef _DEBUG
void CSearchForm::AssertValid() const
{
	CFormView::AssertValid();
}

void CSearchForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CAMFindDoc* CSearchForm::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAMFindDoc)));
	return (CAMFindDoc*)m_pDocument;
}
#endif //_DEBUG

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
	CFormView::OnInitialUpdate();
	g_pForm = this;
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
	WCHAR		szKeyName[MAX_PATH],szDescName[MAX_PATH];
	FILETIME    KeyTime;
	CString strTemp;

	strTemp.LoadString(IDS_ALL_TYPE);
	m_FileType.SetCurSel(m_FileType.AddString(strTemp));
	
	while (TRUE)
	{
		dwSize = MAX_PATH;
		if(ERROR_SUCCESS==
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
		}
		else break;
	}
	return;
}

void CSearchForm::OnMyFileOpen()
{
	CAMFindDoc* pDocument;
	if (m_bSearch)
	{
		if (SelectFile(L"AMG",L"AM01 Image File",g_szImgFile))
		{
			pDocument = GetDocument();
			if (pDocument) pDocument->SetTitle(g_szImgFile);

			DWORD dwErrorCode;

			if( CheckLastImage(g_szImgFile,&dwErrorCode)!=TRUE )
			{
				return;
			}

			IMAGE_HEAD ImageHead;

			if( FALSE == GetImageHead(g_szImgFile,&ImageHead) )
			{
				TRACE(L"\nGetImageHead error in OnMyFileOpen.");
				// AfxMessageBox(IDS_OPEN_IMAGE_FAIL,MB_OK|MB_ICONINFORMATION,NULL);
				return;
			}

			if( ImageHead.bEncrypt )
			{
				if(FALSE == CheckImagePassword(g_szImgFile))
				{
					return;
				}
			}

			CString strTempDir;
            GetTempPath(MAX_PATH-1,m_szTempDir);
			strTempDir=m_szTempDir;
			strTempDir.TrimRight(L"\\");
			wcsncpy(m_szTempDir,(LPCTSTR)strTempDir,MAX_PATH-1);
			wcsncpy(g_szTempDir,(LPCTSTR)strTempDir,MAX_PATH-1);

			WCHAR szMainImageName[MAX_PATH];
			memset(szMainImageName,0,sizeof(szMainImageName));
			if( FALSE == GetLongMainName(g_szImgFile,szMainImageName) )
			{
				TRACE(L"\nGetLongMainName error in OnMyFileOpen.");
			}
			else
			{
			    GetSeperateFile(g_szImgFile,szMainImageName,(LPCTSTR)strTempDir);
			}

			EnableControl(TRUE);
			FreshTimeStamp(g_szImgFile);
		}
	}
}

void CSearchForm::OnSearch() 
{
	CString      strTemp;
	int          i;
	DWORD        dwThreadID;
	CMainFrame *pFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	CAMFindView *pView = static_cast<CAMFindView*>(pFrame->GetRightPane());

	if (g_szImgFile[0] == L'\0')
	{
		PostMessage(WM_COMMAND,ID_FILE_OPEN,0);
		return;
	}
	if (m_bSearch)
	{
		UpdateData(TRUE);
		if (CheckConflict()) return;

		CListCtrl& listView = pView->GetListCtrl();
		listView.DeleteAllItems();
		
		memset(&m_FindParam,0,sizeof(AMFINDSTRUCT));
		m_FindParam.dwTimeStamp = m_TimeStamp.GetItemData(m_TimeStamp.GetCurSel());
//		wcscpy(AMFindParam.szContainChar,m_ContainChar);
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
		m_FindParam.plistView = &listView;
		pFrame->m_wndStatusBar.SetWindowText(L"Searching . . .");
		strTemp.LoadString(IDS_CANCEL_SEARCH);
		GetDlgItem(IDC_SEARCH)->SetWindowText(strTemp);
		EnableControl(FALSE);
		m_bSearch = FALSE;
		m_hThreadHandle = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)CSearchForm::ThreadProc,(LPVOID)&m_FindParam,0,&dwThreadID);
	}
	else
	{
		TerminateThread(m_hThreadHandle,0);
		strTemp.LoadString(IDS_SEARCH);
		GetDlgItem(IDC_SEARCH)->SetWindowText(strTemp);
		pFrame->m_wndStatusBar.SetWindowText(L"Ready");
		EnableControl(TRUE);
		m_bSearch = TRUE;
	}
	// TODO: Add your control notification handler code here
	
}


BOOL CSearchForm::CheckConflict()
{
	if (m_CheckDate)
	{
		if (m_DateEnd < m_DateStart)
		{
			AfxMessageBox(IDS_DATE_ERROR,MB_OK | MB_ICONINFORMATION);
			return TRUE;
		}
	}
	if (m_TimeStamp.GetCurSel() == CB_ERR)
	{
		AfxMessageBox(IDS_NO_TIMESTAMP,MB_OK | MB_ICONINFORMATION);
		return TRUE;
	}
	if (m_SearchName.Find(L'*') != -1 || m_SearchName.Find(L'?') != -1)
	{
		AfxMessageBox(IDS_NAME_VALID,MB_OK | MB_ICONINFORMATION);
		return TRUE;
	}
	return FALSE;
}

void CSearchForm::EnableControl(BOOL bEnable)
{
	GetDlgItem(IDC_FILE_TOSEARCH)->EnableWindow(bEnable);
	GetDlgItem(IDC_SEARCH)->EnableWindow(bEnable);
	GetDlgItem(IDC_COMBO_RANGE)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_DATE)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_TYPE)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_SIZE)->EnableWindow(bEnable);
}

BOOL CSearchForm::FreshTimeStamp(WCHAR *szImageFile)
{
	m_TimeStamp.ResetContent();

	LoadTimeStamp(szImageFile);

	return TRUE;
}

DWORD WINAPI CSearchForm::ThreadProc(PAMFINDSTRUCT pPara)
{
	__int64 lFilePointer;
	pPara->plistView->DeleteAllItems();

	// get the temp TotalIndex file path
	CString strTotalIndexFilePath;
	CString strImageFileName;
	// GetImageFileName(g_szImgFile,strImageFileName.GetBuffer(MAX_PATH));
	GetLongMainName(g_szImgFile,strImageFileName.GetBuffer(MAX_PATH));
	strImageFileName.ReleaseBuffer();

	CString strTempDir;
	strTempDir=g_szTempDir;
	
	strTotalIndexFilePath= strTempDir +L"\\"+ strImageFileName + AM_INDEX_SUFFIX;

	CString strArrayFilePath;
	strArrayFilePath=strTempDir + L"\\" + strImageFileName + AM_ARRAY_SUFFIX;


    // open total index file (for temp use)
    HANDLE hTotalIndexFile;
    hTotalIndexFile=CreateFile( LPCTSTR(strTotalIndexFilePath), GENERIC_READ,
	                            FILE_SHARE_READ,NULL,
							    OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,
							    NULL);

	if(hTotalIndexFile==INVALID_HANDLE_VALUE)
	{
		TRACE(L"Can not open temp total Index file.\n");
		return FALSE;
	}

	TOTAL_INDEX_NODE TotalIndexNode;
	DWORD dwReadBytes;
	ReadFile(hTotalIndexFile,&TotalIndexNode,sizeof(TotalIndexNode),
		     &dwReadBytes,NULL);
	
	// reset the file pointer to file begin
	YGSetFilePointer(hTotalIndexFile,0,FILE_BEGIN,lFilePointer);

	// open array file (for temp use)
    HANDLE hArrayFile;
    hArrayFile=CreateFile( LPCTSTR(strArrayFilePath), GENERIC_READ,
	                       FILE_SHARE_READ,NULL,
					       OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,
						   NULL);

	if(hArrayFile==INVALID_HANDLE_VALUE)
	{
		TRACE(L"Can not open temp array file.\n");
		return FALSE;
	}

	YGSetFilePointer(hArrayFile,sizeof(ARRAY_NODE),FILE_BEGIN,lFilePointer);
	ARRAY_DATA ArrayData;

	DWORD dwCurrentPathID;
	INDEX_DATA IndexData;
	int   nItem;

	CString OnePathString;
	WCHAR    *p,szPath[MAX_PATH];
    FILETIME ftOneLocalFileTime;
    SYSTEMTIME OneSysTime;

	for(dwCurrentPathID=1; dwCurrentPathID<=TotalIndexNode.dwTotalPathCount ; dwCurrentPathID++)
	{
		ReadFile(hArrayFile,&ArrayData,sizeof(ArrayData),&dwReadBytes,NULL);

		OnePathString=ArrayData.szFilePath;
		OnePathString.TrimRight(L"\\");
	
		GetIndexData(hTotalIndexFile,dwCurrentPathID,pPara->dwTimeStamp,IndexData);

	    INDEX_DATA DataIndexData;
		
		GetDataFilePointer(hTotalIndexFile,TotalIndexNode.dwTotalPathCount,
		                   dwCurrentPathID,pPara->dwTimeStamp,&DataIndexData);

		if(IndexData.dwPathID>0)
		{
			if (!(IndexData.FileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				 && DataIndexData.wStatusFlag != RM_FILE_NEW )
			{
				wcscpy(szPath,OnePathString.GetBuffer(0));

				p = strrchrpro(szPath,L'\\');
				
				if (p)
				{
					*p = L'\0';
					p++;
						
					if (CheckSearchMatch(p,&IndexData,pPara))
					{
						nItem = pPara->plistView->InsertItem(0,p,0);
						pPara->plistView->SetItemText(nItem,1,szPath);

						swprintf(szPath,L"%dKB",(IndexData.FileInfo.nFileSizeLow+1023)/1024);
						pPara->plistView->SetItemText(nItem,2,szPath);

					    FileTimeToLocalFileTime(&IndexData.FileInfo.ftLastWriteTime,&ftOneLocalFileTime);
					    FileTimeToSystemTime(&ftOneLocalFileTime,&OneSysTime);
					    swprintf(szPath,L"%.4d-%.2d-%.2d %.2d:%.2d:%.2d",OneSysTime.wYear,OneSysTime.wMonth,OneSysTime.wDay,OneSysTime.wHour,OneSysTime.wMinute,OneSysTime.wSecond);
						pPara->plistView->SetItemText(nItem,3,szPath);
						pPara->plistView->SetItemData(nItem,IndexData.dwPathID);
				   		pPara->plistView->SetItem(nItem,0,LVIF_STATE,L"",0,INDEXTOSTATEIMAGEMASK(1),LVIS_STATEIMAGEMASK,0);
					}
				}
			}
		}
	}
	
	CloseHandle(hTotalIndexFile);
	CloseHandle(hArrayFile);

	::PostMessage(pPara->hParWnd,WM_COMMAND,IDC_SEARCH,0);
	return 0;
}

void CSearchForm::LoadTimeStamp(LPCTSTR szImageFile)
{
	__int64 lFilePointer;
	HANDLE hImageFile;
	DWORD dwReadBytes;
	int k;
	hImageFile=CreateFile(szImageFile,GENERIC_READ,FILE_SHARE_READ,
		                  NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    
	if(hImageFile == INVALID_HANDLE_VALUE)
	{
		TRACE(L" open image file error.");
		return;
	}

	IMAGE_HEAD ImageHead;
	ReadFile(hImageFile,&ImageHead,sizeof(ImageHead),
		     &dwReadBytes,NULL);

	TOTAL_INDEX_NODE TotalIndexNode;

	YGSetFilePointer(hImageFile,ImageHead.TotalIndexPointer.QuadPart,FILE_BEGIN,lFilePointer);
	ReadFile(hImageFile,&TotalIndexNode,sizeof(TotalIndexNode),&dwReadBytes,NULL);

	YGSetFilePointer(hImageFile,ImageHead.StampPointer.QuadPart,FILE_BEGIN,lFilePointer);

	YGSetFilePointer(hImageFile,sizeof(STAMP_NODE),FILE_CURRENT,lFilePointer);

	STAMP_DATA StampData;
	SYSTEMTIME OneSystemTime;
		
	for(DWORD i=0; i<TotalIndexNode.dwTimeStampCount; i++)
	{
	  ReadFile(hImageFile,&StampData,sizeof(StampData),&dwReadBytes,NULL);
	  
	  CString strShowTime;
	  
	  OneSystemTime=StampData.StampTime;

	  strShowTime.Format(L"%.4d-%.2d-%.2d %.2d:%.2d:%.2d",
		                  OneSystemTime.wYear,
						  OneSystemTime.wMonth,
						  OneSystemTime.wDay,
						  OneSystemTime.wHour,
						  OneSystemTime.wMinute,
						  OneSystemTime.wSecond);

	  k=m_TimeStamp.AddString(strShowTime);
	  m_TimeStamp.SetItemData(k,i+1);
	}
    m_TimeStamp.SetCurSel(0);

	CloseHandle(hImageFile);
}

BOOL CheckSearchMatch(WCHAR *szFile,INDEX_DATA *pIndexData,PAMFINDSTRUCT pPara)
{
	CString strTemp,strFind;
	WCHAR    *p;
	strTemp = szFile;
	if (wcslen(pPara->szNameMatch))
	{
		if (strTemp.Find(pPara->szNameMatch) == -1) return FALSE;
	}

	if (pPara->bCheckDate)
	{
		CTime CheckTime;
		switch (pPara->dwDateType)
		{
		case LAST_MODIFY_TIME:
			CheckTime = CTime(pIndexData->FileInfo.ftLastWriteTime);
			break;
		case CREATE_TIME:
			CheckTime = CTime(pIndexData->FileInfo.ftCreationTime);
			break;
		case LAST_ACCESS_TIME:
			CheckTime = CTime(pIndexData->FileInfo.ftLastAccessTime);
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
		p = strrchrpro(szFile,L'.');
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
			if (pIndexData->FileInfo.nFileSizeLow < pPara->dwSize) return FALSE;
			break;
		case MAXSIZE:
			if (pIndexData->FileInfo.nFileSizeLow > pPara->dwSize) return FALSE;
			break;
		default:
			return FALSE;
		}
	}
	return TRUE;
}
