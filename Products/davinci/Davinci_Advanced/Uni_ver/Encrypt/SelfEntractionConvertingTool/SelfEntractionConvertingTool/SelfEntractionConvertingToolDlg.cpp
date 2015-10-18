
// SelfEntractionConvertingToolDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SelfEntractionConvertingTool.h"
#include "SelfEntractionConvertingToolDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSelfEntractionConvertingToolDlg ダイアログ


const  WCHAR  SELF_EXTRACTING_FILE_EXTENSION[12]= L"exe";
const  WCHAR  FILE_ENCRYPT_EXTENSION[12]= L"chy";
const  WCHAR  SELF_EXTRACTING_TEMP_EXTENSION[12]= L".xtfemp";

const  int   IMAGE_IDENTITY_SIZE=32;
const  int   IMAGE_RESERVED_SIZE=128;

const  WCHAR  IMAGE_IDENTITY[IMAGE_IDENTITY_SIZE]= L"FILE_ENCRYPT_IMAGE_FILE";
const WCHAR SELF_EXTRACTING_IDENTITY[IMAGE_IDENTITY_SIZE] = L"SELF_EXTRACTING_FILE_XF";

CSelfEntractionConvertingToolDlg::CSelfEntractionConvertingToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSelfEntractionConvertingToolDlg::IDD, pParent)
	, m_strTarget(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSelfEntractionConvertingToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_wndFilePath);
	DDX_Control(pDX, IDC_STATIC_STATUS, m_labelStatus);
	DDX_Text(pDX, IDC_EDIT1, m_strTarget);
}

BEGIN_MESSAGE_MAP(CSelfEntractionConvertingToolDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CSelfEntractionConvertingToolDlg::OnBnClickedButtonBrowse)
	ON_BN_CLICKED(ID_BUTTON_CONVERT, &CSelfEntractionConvertingToolDlg::OnBnClickedButtonConvert)
END_MESSAGE_MAP()


// CSelfEntractionConvertingToolDlg メッセージ ハンドラー

BOOL CSelfEntractionConvertingToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。
	m_labelStatus.ShowWindow(SW_HIDE);

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CSelfEntractionConvertingToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CSelfEntractionConvertingToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSelfEntractionConvertingToolDlg::OnBnClickedButtonBrowse()
{
	// TODO: Add your control notification handler code here
	CString strChyDescription;
	strChyDescription.LoadString(IDS_IMAGE_DESCRIPTION);
	CString strFilter;
	strFilter.Format(L"%s (*.%s)|*.%s||", strChyDescription, FILE_ENCRYPT_EXTENSION, FILE_ENCRYPT_EXTENSION);
	CString defaultExetension = FILE_ENCRYPT_EXTENSION;
	CFileDialog dlg (FALSE, defaultExetension, NULL, OFN_HIDEREADONLY, strFilter);
	if (IDOK == dlg.DoModal())
	{
		m_strTarget = dlg.GetPathName();
		CString suffix = m_strTarget.Mid(m_strTarget.ReverseFind(L'.') + 1);
		UpdateData(FALSE);
	}
}


void CSelfEntractionConvertingToolDlg::OnBnClickedButtonConvert()
{
	// TODO: Add your specialized code here and/or call the base class
	CString strText,strTitle;
	UpdateData(TRUE);
	if( 0 == m_strTarget.GetLength() )
	{
		strText.LoadString(IDS_EMPTY_TARGET);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
		return;
	}

	CString strFileEncryptSuffix;
	strFileEncryptSuffix.Empty();
	strFileEncryptSuffix = strFileEncryptSuffix + L"." + FILE_ENCRYPT_EXTENSION;

	if(0 != m_strTarget.Right(4).CompareNoCase(strFileEncryptSuffix))
	{
		strText.LoadString(IDS_INVALID_TARGET);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
		return;
	}

	m_labelStatus.ShowWindow(SW_SHOW);
	if (Convert(m_strTarget)) {
		strText.LoadString(IDS_SUCCESS);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(strText,strTitle,MB_OK);
	}
	else {
		strText.LoadString(IDS_FAILURE);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
	}
	m_labelStatus.ShowWindow(SW_HIDE);
}

BOOL CSelfEntractionConvertingToolDlg::Convert( const CString& sourcePath )
{

	// copy file
	TCHAR buf[MAX_PATH];
	memset(buf,0,sizeof(buf));
	GetModuleFileName(NULL,buf,MAX_PATH);
	//GetLongPathName(buf, buf, MAX_PATH);
	CString modulePath = buf;
	CString sourceExePath = modulePath.Left(modulePath.ReverseFind(L'\\')) + L"\\sefb.dll";
	CString targetPath = sourcePath.Left(sourcePath.GetLength() - 4) + L'.' + SELF_EXTRACTING_FILE_EXTENSION;

	BOOL operationResult = FALSE;
	HANDLE hTargetFile = 0;
	HANDLE hImage = 0;
	do {
		if (!CopyFile(sourceExePath, targetPath, FALSE)) {
			break;
		}
		SetFileAttributes(targetPath, FILE_ATTRIBUTE_NORMAL);
		hTargetFile = CreateFile(targetPath,GENERIC_WRITE, NULL, NULL,OPEN_EXISTING,FILE_ATTRIBUTE_HIDDEN,NULL);
		if (hTargetFile == INVALID_HANDLE_VALUE) {
			break;
		}
		hImage = CreateFile(sourcePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (hImage == INVALID_HANDLE_VALUE) {
			break;
		}
		LARGE_INTEGER address;
		LARGE_INTEGER size;
		DWORD dwTemp;
		size.LowPart = GetFileSize(hImage, &dwTemp);
		size.HighPart = dwTemp;
		address.LowPart = GetFileSize(hTargetFile, &dwTemp);
		address.HighPart = dwTemp;

		char chBuf[1024];
		ZeroMemory(chBuf, sizeof(chBuf));
		DWORD dwRead = 0;
		DWORD dwWrite = 0;
		LARGE_INTEGER remainToRead;
		remainToRead.QuadPart = size.QuadPart;
		LARGE_INTEGER totalRead;
		totalRead.QuadPart = 0;
		DWORD dwToRead = 0;
		if (remainToRead.QuadPart > sizeof(chBuf)) {
			dwToRead = sizeof(chBuf);
		}
		else {
			dwToRead = remainToRead.QuadPart;
		} 
		SetFilePointer(hImage, 0, 0, FILE_BEGIN);
		SetFilePointer(hTargetFile, 0, 0, FILE_END);
		while (dwToRead != 0
			&& ReadFile(hImage, chBuf, dwToRead, &dwRead, 0)
			&& dwToRead == dwRead){
				totalRead.QuadPart += dwRead;
				remainToRead.QuadPart -= dwRead;
				if (WriteFile(hTargetFile, chBuf, dwToRead, &dwWrite, 0)
					&& dwWrite == dwToRead) {
						if (remainToRead.QuadPart > sizeof(chBuf)) {
							dwToRead = sizeof(chBuf);
						}
						else {
							dwToRead = remainToRead.QuadPart;
						} 
						dwRead = dwWrite = 0;
						ZeroMemory(chBuf, sizeof(chBuf));
				}
				else {
					break;
				}
		}
		if (dwToRead == 0) {
			if (WriteFile(hTargetFile, SELF_EXTRACTING_IDENTITY, sizeof(SELF_EXTRACTING_IDENTITY), &dwTemp, 0)
				&& WriteFile(hTargetFile, &address.QuadPart, sizeof(LARGE_INTEGER), &dwTemp, 0)
				&& WriteFile(hTargetFile, &size.QuadPart, sizeof(LARGE_INTEGER), &dwTemp, 0)
				&& WriteFile(hTargetFile, SELF_EXTRACTING_IDENTITY, sizeof(SELF_EXTRACTING_IDENTITY), &dwTemp, 0)) {
					operationResult = TRUE;
			}
			else {
				operationResult = FALSE;
			}
		}
	} while (0);
	CloseHandle(hTargetFile);
	CloseHandle(hImage);
	if (!operationResult) {
		DeleteFile(targetPath);
	}
	return operationResult;
}
