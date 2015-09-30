// KeyGenerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "KeyGener.h"
#include "KeyGenerDlg.h"
#include "..\export\SCApi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKeyGenerDlg dialog

CKeyGenerDlg::CKeyGenerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKeyGenerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKeyGenerDlg)
	m_dwProduct = 1;
	m_szCompany = _T("");
	m_nAppendToFile = 0;
	m_dwKeyNumber = 1;
	m_dwKeyStart = 1;
	m_LogFile = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKeyGenerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKeyGenerDlg)
	DDX_Control(pDX, IDC_KEY_START, m_KeyStart);
	DDX_Control(pDX, IDC_PRODUCT_KEY, m_ProductKey);
	DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
	DDX_Control(pDX, IDC_REFER, m_RefButton);
	DDX_Text(pDX, IDC_PRODUCT, m_dwProduct);
	DDV_MinMaxDWord(pDX, m_dwProduct, 1, 999);
	DDX_Text(pDX, IDC_COMPANY, m_szCompany);
	DDV_MaxChars(pDX, m_szCompany, 4);
	DDX_Radio(pDX, IDC_RADIO1, m_nAppendToFile);
	DDX_Text(pDX, IDC_KEY_NUMBER, m_dwKeyNumber);
	DDV_MinMaxDWord(pDX, m_dwKeyNumber, 1, 999999);
	DDX_Text(pDX, IDC_KEY_START, m_dwKeyStart);
	DDV_MinMaxDWord(pDX, m_dwKeyStart, 1, 999999);
	DDX_Text(pDX, IDC_FILE_NAME, m_LogFile);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CKeyGenerDlg, CDialog)
	//{{AFX_MSG_MAP(CKeyGenerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CALCULATE, OnCalculate)
	ON_BN_CLICKED(IDC_REFER, OnRefer)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_EN_KILLFOCUS(IDC_FILE_NAME, OnKillfocusFileName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKeyGenerDlg message handlers

BOOL CKeyGenerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	m_Progress.SetRange(0,100);
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CKeyGenerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CKeyGenerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CKeyGenerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CKeyGenerDlg::OnCalculate() 
{
	char      szText[MAX_PATH];
	SYSTEMTIME CurTime;
	WORD      wEnter = 0x0A0D;
	YGCSSTRU  YGCSInfo;
	HANDLE    hFile;
	DWORD     i,dwWrited;
	char      szSerial[30];
	if (!UpdateData(TRUE)) return;
	if (m_szCompany.GetLength() != 4)
	{
		AfxMessageBox("Company name length not match!");
		return;
	}
	m_Progress.SetPos(0);

	hFile = CreateFile(m_LogFile.GetBuffer(0),GENERIC_READ | GENERIC_WRITE, 
                       FILE_SHARE_READ, NULL,OPEN_ALWAYS,0,NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		if (!m_nAppendToFile || GetFileSize(hFile,NULL) == 0)
		{
			strcpy(szText,"YuGuang License Code File.");
			WriteFile(hFile,szText,strlen(szText),&dwWrited,NULL);
			WriteFile(hFile,&wEnter,2,&dwWrited,NULL);
		}
		else
		{
			SetFilePointer(hFile,0,0,FILE_END);
			WriteFile(hFile,&wEnter,2,&dwWrited,NULL);
		}

		WriteFile(hFile,&wEnter,2,&dwWrited,NULL);
		GetLocalTime(&CurTime);
		sprintf(szText,"License Number:%d--%d,Start Time:%d//%d//%d  %d:%d:%d",
						m_dwKeyStart,m_dwKeyStart+m_dwKeyNumber-1,
						CurTime.wMonth,CurTime.wDay,CurTime.wYear,
						CurTime.wHour,CurTime.wMinute,CurTime.wSecond);
		WriteFile(hFile,szText,strlen(szText),&dwWrited,NULL);
		m_szCompany.MakeUpper();
		strcpy(YGCSInfo.szComp,m_szCompany);
		for (i = 0;i < m_dwKeyNumber;i++)
		{
			WriteFile(hFile,&wEnter,2,&dwWrited,NULL);
			YGCSInfo.dwProduct = m_dwProduct;
			YGCSInfo.dwNumber = m_dwKeyStart+i;
			if (YGSCGenerateSerial(szSerial,&YGCSInfo))
			{
				WriteFile(hFile,&szSerial,strlen(szSerial),&dwWrited,NULL);
				m_ProductKey.SetWindowText(szSerial);
				m_Progress.SetPos(i*100/m_dwKeyNumber);
			}
		}
		WriteFile(hFile,&wEnter,2,&dwWrited,NULL);
		GetLocalTime(&CurTime);
		sprintf(szText,"License Number:%d--%d, End Time:%d//%d//%d  %d:%d:%d",
						m_dwKeyStart,m_dwKeyStart+m_dwKeyNumber-1,
						CurTime.wMonth,CurTime.wDay,CurTime.wYear,
						CurTime.wHour,CurTime.wMinute,CurTime.wSecond);
		WriteFile(hFile,szText,strlen(szText),&dwWrited,NULL);
		SetEndOfFile(hFile);
		CloseHandle(hFile);
	}
	m_ProductKey.SetWindowText("");
	m_Progress.SetPos(0);
	// TODO: Add your control notification handler code here

}


void CKeyGenerDlg::OnRefer() 
{
	char szFile[MAX_PATH];
	if (SelectFile("CSV","YG License Code File",szFile))
	{
		m_LogFile = szFile;
		UpdateData(FALSE);
		OnSelChangeRadio();
	}
	// TODO: Add your control notification handler code here
	
}

void CKeyGenerDlg::OnRadio1() 
{
	m_nAppendToFile = 0;
	OnSelChangeRadio();
	// TODO: Add your control notification handler code here
	
}

void CKeyGenerDlg::OnRadio2() 
{
	m_nAppendToFile = 1;
	OnSelChangeRadio();
	// TODO: Add your control notification handler code here
	
}

BOOL CKeyGenerDlg::OnSelChangeRadio()
{
	HANDLE hFile;
	BOOL   bResult = FALSE;
	WORD      wEnter = 0x0A0D;
	DWORD  dwReaded,dwSize,i;
	char   szText[MAX_PATH],szSign[MAX_PATH];

	UpdateData(TRUE);
	if (m_nAppendToFile)
	{
		m_RefButton.EnableWindow(TRUE);
		m_KeyStart.EnableWindow(FALSE);
		if (GetFileAttributes(m_LogFile.GetBuffer(0)) != -1)
		{
			hFile = CreateFile(m_LogFile.GetBuffer(0),GENERIC_READ | GENERIC_WRITE, 
							   FILE_SHARE_READ, NULL,OPEN_ALWAYS,0,NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				strcpy(szText,"YuGuang License Code File.");
				dwSize = GetFileSize(hFile,NULL);
				if (dwSize > strlen(szText))
				{
					ReadFile(hFile,szSign,strlen(szText),&dwReaded,NULL);
					szSign[strlen(szText)] = '\0';
					if (!stricmp(szText,szSign))
					{
						if (dwSize > MAX_PATH)
						{
							SetFilePointer(hFile,dwSize-MAX_PATH,NULL,FILE_BEGIN);
							dwSize = MAX_PATH;
						}
						ReadFile(hFile,szSign,dwSize,&dwReaded,NULL);
						strcpy(szText,"License Number:");
						for (i = MAX_PATH-2;i > 0;i--)
						{
							if (*((WORD*)&szSign[i]) == wEnter)
							{
								if (!memcmp(&szSign[i+2],szText,strlen(szText)))
								{
									i += strlen(szText);
									for (;i < MAX_PATH;i++)
									{
										if (szSign[i] == '-' && szSign[i+1] == '-')
										{
											i += 2;
											memcpy(szText,&szSign[i],MAX_PATH-i);
											for (i = 0;i < 7;i ++)
											{
												if (szText[i] == ',')
												{
													szText[i] = '\0';
													break;
												}
											}
											m_dwKeyStart = atoi(szText)+1;
											bResult = TRUE;
											break;
										}
									}
									break;
								}
							}
						}
					}
				}
				CloseHandle(hFile);
				if (!bResult) 
				{
					AfxMessageBox("Can not get key information from file!");
					m_LogFile = "";
				}
			}
		}
	}
	else
	{
		m_RefButton.EnableWindow(FALSE);
		m_KeyStart.EnableWindow(TRUE);
		bResult = TRUE;
	}
	UpdateData(FALSE);
	return bResult;

}

void CKeyGenerDlg::OnKillfocusFileName() 
{
	OnSelChangeRadio();
	// TODO: Add your control notification handler code here
	
}
