// ShellDecryptDlg.cpp : implementation file
//

#include "stdafx.h"
#include "filedecrypt.h"
#include "ShellDecryptDlg.h"
#include "DecryptStatus.h"
#include "..\..\DavinciFunction\DavinciFunction.h"
#include "..\..\DavinciFunction\deletemethoddlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DECRYPT_INFO g_DecryptInfo;
extern int nMaxInputNumber;
extern BOOL g_bDecryptSucceed;
int nCurrentInputNumber;

/////////////////////////////////////////////////////////////////////////////
// CShellDecryptDlg dialog


CShellDecryptDlg::CShellDecryptDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShellDecryptDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShellDecryptDlg)
	m_strPassword = _T("");
	m_bSecureDeleteSource = FALSE;
	//}}AFX_DATA_INIT
}


void CShellDecryptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShellDecryptDlg)
	DDX_Text(pDX, IDC_PASSWORD, m_strPassword);
	DDX_Check(pDX, IDC_SECURE_DELETE_SOURCE, m_bSecureDeleteSource);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShellDecryptDlg, CDialog)
	//{{AFX_MSG_MAP(CShellDecryptDlg)
	ON_BN_CLICKED(IDC_BUTTON_DECRYPT, OnButtonDecrypt)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnButtonCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShellDecryptDlg message handlers

void CShellDecryptDlg::OnButtonDecrypt() 
{
	// TODO: Add your control notification handler code here
	CString strText,strTitle;
	UpdateData(TRUE);
	CDecryptStatus DecryptStatusDlg;

	memset(g_DecryptInfo.szImageFile,0,sizeof(g_DecryptInfo.szImageFile));
	wcsncpy(g_DecryptInfo.szImageFile,m_strImageFile,MAX_PATH-1);

	if( 0 == m_strPassword.GetLength() )
	{
		strText.LoadString(IDS_EMPTY_PASSWORD);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
		return;
	}

	if( m_strPassword.GetLength() > PASSWORD_SIZE-1 )
	{
		strText.LoadString(IDS_PASSWORD_TOO_LONG);
		strTitle.LoadString(IDS_APP_NAME);
		MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
		return;
	}

/*	if (!YGNSMGetAutoDecryptInfo(&g_DecryptInfo))
	{
		AfxMessageBox(IDS_INVALID_IMAGE_FILE);
		return;
	}
*/
	CString strTargetDir;
	int nIndex;
	wcsncpy(g_DecryptInfo.szPassword,(LPCTSTR)m_strPassword,PASSWORD_SIZE-1);
	nIndex = m_strImageFile.ReverseFind(L'\\');
	strTargetDir = m_strImageFile.Left(nIndex);

	memset(g_DecryptInfo.szTargetPath,0,sizeof(g_DecryptInfo.szTargetPath));
	wcsncpy(g_DecryptInfo.szTargetPath,(LPCTSTR)strTargetDir,MAX_PATH-1);

	DWORD dwImageFileCount;

	CString strOneFileName;

	if( FALSE == GetImageInfo(&g_DecryptInfo,dwImageFileCount,strOneFileName) )
	{
//		strText.LoadString(IDS_GET_IMAGEINFO_FAIL);
//		strTitle.LoadString(IDS_APP_NAME);
//		MessageBox(strText,strTitle,MB_OK | MB_ICONWARNING);
		return;
	}

	// 2004.09.15 modify begin

	if( 1 != dwImageFileCount) // more than one file, or no file, only directory
	{
		CStringArray SelectionArray;
		if( FALSE == ReadSelectionArray(g_DecryptInfo.szImageFile,g_DecryptInfo.szPassword,SelectionArray) )
		{
			TRACE(L"\nReadSelectionArray error.");
			return;
		}

		if( 1 == SelectionArray.GetSize() )
		{
			// contain more than one file
			// but only select only one item
			// this item must be a directory
			g_DecryptInfo.nRestoreType = DECRYPT_PART;
			g_DecryptInfo.nTargetType = DECRYPT_NEW_PLACE;
			memset(g_DecryptInfo.szSelectedPath,0,sizeof(g_DecryptInfo.szSelectedPath));
			wcsncpy(g_DecryptInfo.szSelectedPath,(LPCTSTR)(SelectionArray[0]),MAX_PATH-1);
		}
		else
		{
			// if selection is more than one item

			CString strCommonLeftDir;
			
			if( FALSE == GetCommonLeftDir(SelectionArray,strCommonLeftDir) )
			{
				TRACE(L"\nGetCommonLeftDir error in OnButtonDecrypt.");
			}
			else
			{
				TRACE(L"\nThe common left dir is:%s",(LPCTSTR)strCommonLeftDir);
			}

			if( 0 == strCommonLeftDir.GetLength() )
			{
				// more than one item, and not in the same disk
				g_DecryptInfo.nRestoreType = DECRYPT_ENTIRE;
				g_DecryptInfo.nTargetType = DECRYPT_NEW_PLACE;
				// since we are decrypt all the file to a new place
				// the szSelectedPath is not needed.
				memset(g_DecryptInfo.szSelectedPath,0,sizeof(g_DecryptInfo.szSelectedPath));
			}
			else
			{
				// more than one item, but in the same disk
				strCommonLeftDir.TrimRight(L'\\');
				g_DecryptInfo.nRestoreType = DECRYPT_PART;
				g_DecryptInfo.nTargetType = DECRYPT_NEW_PLACE;
				memset(g_DecryptInfo.szSelectedPath,0,sizeof(g_DecryptInfo.szSelectedPath));
				wcsncpy(g_DecryptInfo.szSelectedPath,(LPCTSTR)(strCommonLeftDir),MAX_PATH-1);
			}
		}
	}
	else
	{
		// if it contains only one file
		// if this image file only contain one file
		// right click it will decrypt the file to the same directory as the original image file
		g_DecryptInfo.nRestoreType = DECRYPT_PART;
		g_DecryptInfo.nTargetType = DECRYPT_NEW_PLACE;
		memset(g_DecryptInfo.szSelectedPath,0,sizeof(g_DecryptInfo.szSelectedPath));
		wcsncpy(g_DecryptInfo.szSelectedPath,(LPCTSTR)strOneFileName,MAX_PATH-1);
		if (m_bOpen)
		{
			if (GetTempDir((WCHAR *)&g_DecryptInfo.szTargetPath))
			{
				ARRAY_DATA ArrayData;
				CONVERT_BUF DecryptFileInfo = {0};
				wcscpy(ArrayData.szFileName,strOneFileName);
				AdjustOnePathString(ArrayData,g_DecryptInfo);
				SetFileAttributes(ArrayData.szFileName,FILE_ATTRIBUTE_NORMAL);
				DeleteFile(ArrayData.szFileName);
				DecryptStatusDlg.DoModal();

				if (GetFileAttributes(ArrayData.szFileName) != -1)
				{
					ZeroMemory(DecryptFileInfo.szMonitorFile, sizeof(DecryptFileInfo.szMonitorFile));
					ZeroMemory(DecryptFileInfo.szDestFile, sizeof(DecryptFileInfo.szDestFile));
					WideCharToMultiByte(CP_ACP, 0, ArrayData.szFileName, -1,
						DecryptFileInfo.szMonitorFile, MAX_PATH, 0, 0);
					WideCharToMultiByte(CP_ACP, 0, g_DecryptInfo.szImageFile, -1,
						DecryptFileInfo.szDestFile, MAX_PATH, 0, 0);
					//wcscpy(DecryptFileInfo.szMonitorFile,ArrayData.szFileName);
					//wcscpy(DecryptFileInfo.szDestFile,g_DecryptInfo.szImageFile);
					DecryptFileInfo.usNodeType = CONVERT_NODETYPE_AUTO_FILE;
					DecryptFileInfo.ulCompressLevel = g_DecryptInfo.ulCompressLevel;
					ZeroMemory(DecryptFileInfo.szPassword, PASSWORD_SIZE);
					WideCharToMultiByte(CP_ACP, 0, g_DecryptInfo.szPassword, PASSWORD_SIZE,
						DecryptFileInfo.szPassword, PASSWORD_SIZE, 0, 0);
					// memcpy(DecryptFileInfo.szPassword,g_DecryptInfo.szPassword,PASSWORD_SIZE*sizeof(WCHAR));
					DecryptFileInfo.bErrorLimit = g_DecryptInfo.EncryptOption.bOptionOn && g_DecryptInfo.EncryptOption.bErrorLimit;
					DecryptFileInfo.ulMaxErrLimit = g_DecryptInfo.EncryptOption.ulErrorLimit;

					YGNSMRemoveMonitorFile(&DecryptFileInfo);
					if ((ULONG)ShellExecute(NULL,L"open",ArrayData.szFileName,NULL,NULL,SW_SHOWNORMAL) > 32)
					{
#ifdef NO_DRIVER_MODE
						BOOL bWriteSuc = TRUE;
						DWORD cnt = 0;
						CString strInfoPath;
						if (GetTempInfoFilePath(strInfoPath))
						{
							do 
							{
								HANDLE hFile = INVALID_HANDLE_VALUE;
								DWORD dwTemp;
								if (GetFileAttributes(strInfoPath) != -1)
								{
									hFile = CreateFile( strInfoPath,GENERIC_READ | GENERIC_WRITE,NULL,NULL,
										OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN,NULL);
								}
								else
								{
									hFile = CreateFile( strInfoPath,GENERIC_READ | GENERIC_WRITE,NULL,NULL,
										CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN,NULL);
								}
								if (hFile != INVALID_HANDLE_VALUE)
								{
									SetFilePointer(hFile, 0, 0, FILE_END);
									DWORD temp = 0;
									FILETIME ft;
									HANDLE hFileTarget = CreateFile( ArrayData.szFileName,
										GENERIC_READ,
										FILE_SHARE_READ,
										NULL,
										OPEN_EXISTING,
										FILE_ATTRIBUTE_NORMAL,
										NULL);
									if (hFileTarget != INVALID_HANDLE_VALUE)
									{
										ft = GetLastModifyTime(hFileTarget);
										CloseHandle(hFileTarget);
									}
									else
									{
										SYSTEMTIME st;
										GetSystemTime(&st);
										SystemTimeToFileTime(&st, &ft);
									}
									DecryptFileInfo.ulCompressLevel = ft.dwHighDateTime;
									DecryptFileInfo.ulMaxErrLimit = ft.dwLowDateTime;
									if (WriteFile(hFile, &DecryptFileInfo, sizeof(CONVERT_BUF), &temp, 0)
										&& temp == sizeof(CONVERT_BUF))
									{
										bWriteSuc = TRUE;
									}
									else
									{
										bWriteSuc = FALSE;
									}
									CloseHandle(hFile);
								}
								else
								{
									bWriteSuc = FALSE;
								}
								if (!bWriteSuc)
								{
									Sleep(10);
								}
								++cnt;
							} while(!bWriteSuc && cnt < 100);	
						}
#else
						YGNSMAddMonitorFile(&DecryptFileInfo);
#endif
					}
					
					ShowWindow(SW_HIDE);
					EndDialog(TRUE);
					return;
				}
			}
		}
	}

	// 2004.09.15 modify end
	
	ShowWindow(SW_HIDE);

//	CDecryptStatus DecryptStatusDlg;
	DecryptStatusDlg.DoModal();

	if (m_bSecureDeleteSource)
	{
		if (g_bDecryptSucceed)
		{
			CStringArray strSourceArray;
			CString strSourceFile = g_DecryptInfo.szImageFile;
			strSourceArray.Add(strSourceFile);
			CDeleteMethodDlg DeleteMethodDlg;
			DeleteMethodDlg.m_SourceArray.RemoveAll();
			DeleteMethodDlg.m_SourceArray.Append(strSourceArray);
			DeleteMethodDlg.DoModal();
		}
	}

	EndDialog(TRUE);
}

void CShellDecryptDlg::OnButtonCancel() 
{
	// TODO: Add your control notification handler code here
	EndDialog(FALSE);
}

void CShellDecryptDlg::OnOK()
{
	// overwrite CDialog::OnOK()
}

