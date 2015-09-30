// DMFDDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DMFD.h"
#include "DMFDDlg.h"
#include "MakeFD.h"

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
// CDMFDDlg dialog

CDMFDDlg::CDMFDDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDMFDDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDMFDDlg)
	m_MainInfo = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDMFDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDMFDDlg)
	DDX_Text(pDX, IDC_MAIN_FD_INFORMATION, m_MainInfo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDMFDDlg, CDialog)
	//{{AFX_MSG_MAP(CDMFDDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CONTINUE, OnContinue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDMFDDlg message handlers

BOOL CDMFDDlg::OnInitDialog()
{
	int					i;
	DEVICEPARAMS		dp;
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
	
	int  nType;
	strcpy(m_szFloppy,"A:\\");
	memset (&dp, 0, sizeof(dp));
	for(i=0;i<26;i++)
	{
		nType = GetDriveType(m_szFloppy);
		if(nType == DRIVE_REMOVABLE)
		{
			if(GetFloppyFormFactor(i+1) == 350) 
				break;
		}
		m_szFloppy[0]++;
	}
	m_MainInfo.Format (IDS_MAIN_INFORMATION,m_szFloppy);
	UpdateData(FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDMFDDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CDMFDDlg::OnPaint() 
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
HCURSOR CDMFDDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

DWORD CDMFDDlg::GetFloppyFormFactor(int iDrive)
{
    HANDLE	hDevice;
    TCHAR	tsz[8];
    DWORD	dwResult = 0;
    DWORD	cb;

    if ((int)GetVersion() < 0)
    {   // Windows 95

        hDevice = CreateFileA("\\\\.\\VWIN32", 0, 0, 0, 0,
                        FILE_FLAG_DELETE_ON_CLOSE, 0);
        if (hDevice != INVALID_HANDLE_VALUE)
        {
             DWORD          cb;
             DIOC_REGISTERS reg;
             DOSDPB         dpb;

             dpb.specialFunc = 0;  // return default type; do not hit disk

             reg.reg_EBX   = iDrive;       // BL = drive number (1-based)
             reg.reg_EDX   = (DWORD)&dpb;  // DS:EDX -> DPB
             reg.reg_ECX   = 0x0860;       // CX = Get DPB
             reg.reg_EAX   = 0x440D;       // AX = Ioctl
             reg.reg_Flags = 0x01;   // assume failure

             // Make sure both DeviceIoControl and Int 21h succeeded.
             if (DeviceIoControl (hDevice, VWIN32_DIOC_DOS_IOCTL, &reg,
                                  sizeof(reg), &reg, sizeof(reg),
                                  &cb, 0)
                 && !(reg.reg_Flags & 0x01))
             {
                switch (dpb.devType)
                {
                case 2: // 3.5  720K floppy
                case 7: // 3.5  1.44MB floppy
                case 9: // 3.5  2.88MB floppy
                   dwResult = 350; break;
                default: // other
                   break;
                }
             }
             CloseHandle(hDevice);
          }
      }
      else
      {
         //On Windows NT, use the technique described in the Knowledge
         //Base article Q115828 and in the "FLOPPY" SDK sample.

		 wsprintf(tsz, TEXT("\\\\.\\%c:"), TEXT('@') + iDrive);
         hDevice = CreateFile(tsz, 0, FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
         if (hDevice != INVALID_HANDLE_VALUE)
         {
            DISK_GEOMETRY Geom[20];

			memset(Geom,0,20*sizeof(DISK_GEOMETRY));

            if (DeviceIoControl (hDevice, IOCTL_DISK_GET_MEDIA_TYPES, 0, 0,
                                 Geom, sizeof(Geom), &cb, 0)
                && cb > 0)
            {
               switch (Geom[0].MediaType)
               {
               case F3_1Pt44_512: // 3.5 1.44MB floppy
               case F3_2Pt88_512: // 3.5 2.88MB floppy
               case F3_20Pt8_512: // 3.5 20.8MB floppy
               case F3_720_512:   // 3.5 720K   floppy
			   case F3_1Pt2_512:  // 3.5" 1.2Mb 512 bytes/sector
			   case F3_1Pt23_1024:// 3.5" 1.23Mb 1024 bytes/sector
                 dwResult = 350;
                  break;
               default:
				  break;	
               }
            }
            CloseHandle(hDevice);
         }
    }
	return dwResult;
}

void CDMFDDlg::OnContinue() 
{
	CString		szCaption,szError;
	char		szDrv[3];
	CMakeFD		MakeFDDlg;

	szCaption.LoadString (IDS_DEL_MASTER_ERROR);

	if(!HasFloppy())
	{
		strcpy(szDrv,"A:");
		szDrv[0] = m_szFloppy[0];
		szError.Format (IDS_INSERT_FD,szDrv);
		MessageBox(szError,szCaption,MB_OK|MB_ICONSTOP);
	}
	else
	{
		strcpy(MakeFDDlg.m_szFloppy ,m_szFloppy);
		if(MakeFDDlg.DoModal () == IDOK)
		{
			CDialog::OnOK();
		}
	}
}

BOOL CDMFDDlg::HasFloppy()
{
	char	szCurrentDir[MAX_PATH];

	GetCurrentDirectory(MAX_PATH,szCurrentDir);

	if(SetCurrentDirectory(m_szFloppy))
	{
		SetCurrentDirectory(szCurrentDir);
		return TRUE;
	}
	int nErr = GetLastError();
	return FALSE;
}
