// MakeFD.cpp : implementation file
//

#include "stdafx.h"
#include "DMFD.h"
#include "MakeFD.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMakeFD dialog


CMakeFD::CMakeFD(CWnd* pParent /*=NULL*/)
	: CDialog(CMakeFD::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMakeFD)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMakeFD::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMakeFD)
	DDX_Control(pDX, IDC_MAKE_STATUS, m_MakeStatus);
	DDX_Control(pDX, IDOK, m_OkBut);
	DDX_Control(pDX, IDCANCEL, m_CancelBut);
	DDX_Control(pDX, IDC_MAKE_PROGRESS, m_MakeProgress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMakeFD, CDialog)
	//{{AFX_MSG_MAP(CMakeFD)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMakeFD message handlers

void CMakeFD::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	m_bContinue = FALSE;
	//CDialog::OnCancel();
}

BOOL CMakeFD::OnInitDialog() 
{
	DWORD		threadID;

	CDialog::OnInitDialog();
	
	m_bContinue = TRUE;

	m_MakeProgress.SetRange (0,100);
	m_MakeProgress.SetPos(0);

	m_OkBut.ShowWindow (SW_HIDE);
	UpdateData(FALSE);
	
	m_Thread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)CMakeFD::ThreadProc ,
							(LPVOID)this,0,&threadID);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

DWORD WINAPI CMakeFD::ThreadProc(CMakeFD* dlg)
{
	DWORD ret = dlg->ProgressProc();
	return ret;
}

DWORD CMakeFD::ProgressProc()
{
	MakeFloppyDisk();
	return 0;
}

BOOL CMakeFD::MakeFloppyDisk()
{
	BYTE			*pBuf;
	BYTE			btDisk;
	HANDLE			hFile;
	DWORD			dwReadWrite;
	DWORD			dwTotalSize;
	DWORD			dwSize;
	float			fpresent;
	int				nPresent;
	int				i;
	char			*p;
	char			szCurrentDir[MAX_PATH];
	CString			csStatus;
	BOOL			bResult = TRUE;

	pBuf		= NULL;
	dwSize		= 0;
	dwTotalSize = RW_SIZE*RW_TIMES;
	GetModuleFileName(NULL,szCurrentDir,MAX_PATH);

	p = strrchr(szCurrentDir,'\\');
	*p= 0;
	p = strrchr(szCurrentDir,'\\');
	strcpy(p,"\\ErDisk.bin");

	hFile = CreateFile(szCurrentDir, 
						GENERIC_READ, 
						FILE_SHARE_READ,
						NULL,
						OPEN_EXISTING, 
						//NULL,
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if (hFile == INVALID_HANDLE_VALUE) 
	{
		m_OkBut.ShowWindow (SW_SHOW);
		m_CancelBut.ShowWindow (SW_HIDE);
		m_MakeProgress.ShowWindow (SW_HIDE);
		return FALSE;
	}
	else
	{
		pBuf = (BYTE*)malloc(RW_SIZE);
		if(!pBuf)
		{
			CloseHandle(hFile);
			m_OkBut.ShowWindow (SW_SHOW);
			m_CancelBut.ShowWindow (SW_HIDE);
			m_MakeProgress.ShowWindow (SW_HIDE);
			CloseHandle(hFile);
			return FALSE;
		}
	}
	btDisk = m_szFloppy[0] - 'A';
#ifdef WIN_9X
	BIOS_DRIVE_PARAM	DriveParam;

	if(!Init_PartitionInfo())
	{
		if(pBuf) free(pBuf);
		m_OkBut.ShowWindow (SW_SHOW);
		m_CancelBut.ShowWindow (SW_HIDE);
		CloseHandle(hFile);
		m_MakeProgress.ShowWindow (SW_HIDE);
		return FALSE;
	}

	if(!GetDriveParam(btDisk,&DriveParam))
	{
		if(pBuf) free(pBuf);
		m_OkBut.ShowWindow (SW_SHOW);
		m_CancelBut.ShowWindow (SW_HIDE);
		m_MakeProgress.ShowWindow (SW_HIDE);
		CloseHandle(hFile);
		return FALSE;
	}
	for(i=0;i<RW_TIMES;i++)
	{
		if(!m_bContinue)
		{
			bResult = FALSE;
			break;
		}
		ReadFile(hFile,pBuf,RW_SIZE,&dwReadWrite,NULL);
		WriteSector(i*(RW_SIZE/512),(WORD)(RW_SIZE/512),pBuf,btDisk,&DriveParam);
		dwSize += RW_SIZE;
		fpresent = float(dwSize)/float(dwTotalSize);
		nPresent = (int)(fpresent*100);
		m_MakeProgress.SetPos(nPresent);
	}
	Free_PartitionInfo();
#else
    HANDLE		hDevice;
    TCHAR		tsz[8];

	wsprintf(tsz, TEXT("\\\\.\\%c:"), TEXT('A') + btDisk);
    hDevice = CreateFile(tsz, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hDevice != INVALID_HANDLE_VALUE)
    {
		dwReadWrite = 0;
		for(i=0;i<RW_TIMES;i++)
		{
			if(!m_bContinue)
			{
				bResult = FALSE;
				break;
			}
			if(!ReadFile(hFile,pBuf,RW_SIZE,&dwReadWrite,NULL))
			{
				dwReadWrite = GetLastError();
				break;
			}
			if(!m_bContinue)
			{
				bResult = FALSE;
				break;
			}
			if(!WriteFile(hDevice,pBuf,RW_SIZE,&dwReadWrite,NULL))
			{
				dwReadWrite = GetLastError();
				break;
			}
			dwSize += RW_SIZE;
			fpresent = float(dwSize)/float(dwTotalSize);
			nPresent = (int)(fpresent*100);
			m_MakeProgress.SetPos(nPresent);
		}
	}
	CloseHandle(hDevice);
#endif
	CloseHandle(hFile);
	if(pBuf) free(pBuf);

	m_OkBut.ShowWindow (SW_SHOW);
	m_CancelBut.ShowWindow (SW_HIDE);
	m_MakeProgress.ShowWindow (SW_HIDE);
	if(bResult)
		csStatus.LoadString (IDS_STATUS_SUCCESS);
	m_MakeStatus.SetWindowText (csStatus);
	//UpdateData(FALSE);
	return TRUE;
}
