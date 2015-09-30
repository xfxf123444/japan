
// TestDeleteLibraryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestDeleteLibrary.h"
#include "TestDeleteLibraryDlg.h"
#include "afxdialogex.h"
#include "DeleteLibrary.h"
#include <string>
#include <shlwapi.h>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestDeleteLibraryDlg dialog



CTestDeleteLibraryDlg::CTestDeleteLibraryDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTestDeleteLibraryDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestDeleteLibraryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCEDITBROWSE_FOLDER_PATH, m_editBrowseFolder);
	DDX_Control(pDX, IDC_MFCEDITBROWSE_FILE_PATH, m_editBrowseFile);
}

BEGIN_MESSAGE_MAP(CTestDeleteLibraryDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CHECK_EXCLUDED_PATH, &CTestDeleteLibraryDlg::OnBnClickedButtonCheckExcludedPath)
	ON_BN_CLICKED(IDC_BUTTON_SECURE_DELETE_RECYCLE_BIN, &CTestDeleteLibraryDlg::OnBnClickedButtonSecureDeleteRecycleBin)
	ON_BN_CLICKED(IDC_BUTTON_SECURE_DELETE, &CTestDeleteLibraryDlg::OnBnClickedButtonSecureDelete)
	ON_BN_CLICKED(IDC_RADIO_Folder, &CTestDeleteLibraryDlg::OnBnClickedRadioFolder)
	ON_BN_CLICKED(IDC_RADIO_File, &CTestDeleteLibraryDlg::OnBnClickedRadioFile)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CTestDeleteLibraryDlg message handlers

BOOL CTestDeleteLibraryDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_isExecuting = false;
	m_pDeleteLibrary = new DeleteLibrary;
	m_pDeleteLibrary->SetDeleteMethod(DEL_METHOD_GUTMANN);
	
	m_logPath = GetModulePath();
	m_logPath.Append(_T("\\dm.log"));
	m_pDeleteLibrary->SetLogPath(m_logPath);

	m_editBrowseFolder.EnableFolderBrowseButton();
	m_editBrowseFile.EnableFileBrowseButton();
	((CButton*)GetDlgItem(IDC_RADIO_Folder))->SetCheck(BST_CHECKED);
	m_type = DT_Folder;
	GetDlgItem(IDC_MFCEDITBROWSE_FOLDER_PATH)->EnableWindow(TRUE);
	GetDlgItem(IDC_MFCEDITBROWSE_FILE_PATH)->EnableWindow(FALSE);

	GetDlgItem(IDC_EDIT_TOTAL_SIZE)->SetWindowText(_T("0"));
	GetDlgItem(IDC_EDIT_PROCESS_SIZE)->SetWindowText(_T("0"));

	SetTimer(100, 100, 0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestDeleteLibraryDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestDeleteLibraryDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestDeleteLibraryDlg::OnBnClickedButtonCheckExcludedPath()
{
	CString path;
	if (m_type == DT_File)
	{
		GetDlgItem(IDC_MFCEDITBROWSE_FILE_PATH)->GetWindowText(path);
	}
	else
	{
		GetDlgItem(IDC_MFCEDITBROWSE_FOLDER_PATH)->GetWindowText(path);
	}

	if (!path.IsEmpty() && GetFileAttributes(path) != -1)
	{
		bool flag = !DeleteLibrary::CheckIsNormalPath(path);
		CString s;
		s.Format(_T("check excluded of %s : %s"), path, flag ? _T("true") : _T("false"));
		MessageBox(s);
	}
	else 
	{
		MessageBox(_T("Select a correct path first."));
	}
}

void CTestDeleteLibraryDlg::OnBnClickedButtonSecureDeleteRecycleBin()
{
	if (m_isExecuting)
	{
		MessageBox(_T("Deleting is in progress."));
	}

	if (m_pDeleteLibrary->StartDeletingRecycledBin() == DEL_RESULT_OK)
	{
		m_isExecuting = true;
		m_deleteRecycledBin = true;
	}
	else
	{
		m_isExecuting = false;
		MessageBox(_T("Operation cannot be executed"));
	}
}

const TCHAR* CTestDeleteLibraryDlg::GetModulePath()
{
	static bool bInit = true;
	static TCHAR s_path[MAX_PATH];
	if (bInit)
	{
		bInit = false;
		memset(s_path,0,sizeof(s_path));
		GetModuleFileName(NULL,s_path,MAX_PATH);
		GetLongPathName(s_path, s_path, MAX_PATH);
#ifdef UNICODE
		wstring strWorkingDir = s_path;
		strWorkingDir = strWorkingDir.substr(0, strWorkingDir.find_last_of(L'\\'));
#else
		string strWorkingDir = s_path;
		strWorkingDir = strWorkingDir.substr(0, strWorkingDir.find_last_of('\\'));
#endif
		memset(s_path,0,sizeof(s_path));
		_tcscpy(s_path, strWorkingDir.c_str());
	}
	return s_path;
}

void CTestDeleteLibraryDlg::OnBnClickedButtonSecureDelete()
{
	if (m_isExecuting)
	{
		MessageBox(_T("Deleting is in progress."));
	}

	CString path;
	if (m_type == DT_File)
	{
		GetDlgItem(IDC_MFCEDITBROWSE_FILE_PATH)->GetWindowText(path);
	}
	else
	{
		GetDlgItem(IDC_MFCEDITBROWSE_FOLDER_PATH)->GetWindowText(path);
	}

	DWORD attri = GetFileAttributes(path);
	if (!path.IsEmpty() && attri != -1)
	{
		if (m_pDeleteLibrary->StartDeletingTask(path, 1) == DEL_RESULT_OK)
		{
			m_isExecuting = true;
			m_deleteRecycledBin = false;
		}
		else
		{
			m_isExecuting = false;
			MessageBox(_T("Operation cannot be executed"));
		}

// 		// multi paths for example
// 		int pathCount = 3;
// 		TCHAR* pathArray = new TCHAR[MAX_PATH * pathCount];
// 		ZeroMemory(pathArray, sizeof(TCHAR) * MAX_PATH * pathCount);
// 		for (int i = 0; i < pathCount; ++i)
// 		{
// 			_tcscpy(pathArray + i * MAX_PATH, path);
// 		}
// 		m_pDeleteLibrary->StartDeletingTask(pathArray, pathCount);
// 		delete[] pathArray;
	}
	else 
	{
		MessageBox(_T("Select a correct path first."));
	}
}


void CTestDeleteLibraryDlg::OnBnClickedRadioFolder()
{
	m_type = DT_Folder;
	GetDlgItem(IDC_MFCEDITBROWSE_FOLDER_PATH)->EnableWindow(TRUE);
	GetDlgItem(IDC_MFCEDITBROWSE_FILE_PATH)->EnableWindow(FALSE);
}


void CTestDeleteLibraryDlg::OnBnClickedRadioFile()
{
	m_type = DT_File;
	GetDlgItem(IDC_MFCEDITBROWSE_FOLDER_PATH)->EnableWindow(FALSE);
	GetDlgItem(IDC_MFCEDITBROWSE_FILE_PATH)->EnableWindow(TRUE);
}


void CTestDeleteLibraryDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (m_isExecuting)
	{
		ULARGE_INTEGER totalSize = m_pDeleteLibrary->GetTotalSizeOfProcessing();
		ULARGE_INTEGER processSize = m_pDeleteLibrary->GetSizeOfProcessed();
		CString temp;
		temp.Format(_T("%u,%u"), totalSize.HighPart, totalSize.LowPart);
		GetDlgItem(IDC_EDIT_TOTAL_SIZE)->SetWindowText(temp);
		temp.Format(_T("%u,%u"), processSize.HighPart, processSize.LowPart);
		GetDlgItem(IDC_EDIT_PROCESS_SIZE)->SetWindowText(temp);
		if (!m_pDeleteLibrary->IsDeletingInProgress())
		{
			m_isExecuting = false;
			if (m_deleteRecycledBin)
			{
				SHEmptyRecycleBin(NULL,NULL,SHERB_NOCONFIRMATION|SHERB_NOPROGRESSUI|SHERB_NOSOUND);
			}
			MessageBox(_T("Deletion finished."));
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}
