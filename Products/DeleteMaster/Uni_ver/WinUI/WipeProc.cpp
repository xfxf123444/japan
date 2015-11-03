// WipeProc.cpp : implementation file
//

#include "stdafx.h"
#include "DeleteMaster.h"
#include "WipeProc.h"
#include "Fun.h"

extern	int						g_nWipeMothed;
extern  BOOL                    g_bBeepOff;
/////////////////////////////////////////////////////////////////////////////
// CWipeProc dialog


CWipeProc::CWipeProc(CWnd* pParent /*=NULL*/)
	: CDialog(CWipeProc::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWipeProc)
	m_szTar = _T("");
	m_WipeMothed = _T("");
	m_szPercent = _T("");
	m_csElasped = _T("");
	m_csRemain = _T("");
	//}}AFX_DATA_INIT
}


void CWipeProc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWipeProc)
	DDX_Control(pDX, IDCANCEL, m_CancelButton);
	DDX_Control(pDX, IDC_COMPLETE_PECNET, m_WipePresent);
	DDX_Control(pDX, IDC_SAVE, m_SaveButton);
	DDX_Control(pDX, IDC_NO_SAVE, m_NoSaveButton);
	DDX_Control(pDX, IDC_WIPE_PROGRESS, m_WipeProgress);
	DDX_Text(pDX, DC_WIPE_PROGRESS_TAR, m_szTar);
	DDX_Text(pDX, IDC_WIPE_PROGRESS_MOTHED, m_WipeMothed);
	DDX_Text(pDX, IDC_COMPLETE_PECNET, m_szPercent);
	DDX_Text(pDX, IDC_TIME_ELASPED, m_csElasped);
	DDX_Text(pDX, IDC_TIME_REMAINING, m_csRemain);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWipeProc, CDialog)
	//{{AFX_MSG_MAP(CWipeProc)
	ON_BN_CLICKED(IDC_NO_SAVE, OnNoSave)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWipeProc message handlers

BOOL CWipeProc::OnInitDialog() 
{
	CString		csMothed;
	DWORD		threadID;

	CDialog::OnInitDialog();
	
	m_WipeProgress.SetRange(0,100);
	m_WipeProgress.SetPos(0);
	
	m_nTotalFileSize = 0;
	m_nFileNum = 0;
	m_nTotalWipeSize = 0;
	m_nWipedSize	 = 0;
	m_nPresent		 = 0;
	m_dwElaspedTime  = 0;
	m_bCancelProc	 = FALSE; 
	m_NoSaveButton.ShowWindow(SW_HIDE);
	m_SaveButton.ShowWindow(SW_HIDE);

	m_szTar.Format (IDS_WIPE_FREE,m_DriveLetter);	
	switch(g_nWipeMothed)
	{
	case 0:
		csMothed.LoadString(IDS_00_MOTHED);
		break;
	case 1:
		csMothed.LoadString(IDS_FF_MOTHED);
		break;
	case 2:
		csMothed.LoadString(IDS_RANDOM_MOTHED);
		break;
	case 3:
		csMothed.LoadString(IDS_NSA_MOTHED);
		break;
	case 4:
		csMothed.LoadString(IDS_OLD_NSA_MOTHED);
		break;
	case 5:
		csMothed.LoadString(IDS_DOD_STD_MOTHED);
		break;
	case 6:
		csMothed.LoadString(IDS_NATO_MOTHED);
		break;
	case 7:
		csMothed.LoadString(IDS_GUTMANN_MOTHED);
		break;
	}
	m_WipeMothed.Format(IDS_DEL_PROGRESS_MOTHED,csMothed);
	UpdateData(FALSE);

	SetTimer(IDT_TIMER1,1000,NULL);
	m_Thread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)CWipeProc::ThreadProc ,
							(LPVOID)this,0,&threadID);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

DWORD WINAPI CWipeProc::ThreadProc(CWipeProc* dlg)
{
	DWORD ret = dlg->ProgressProc();
	return ret;
}

DWORD CWipeProc::ProgressProc()
{
	WipeContent();
	return 0;
}

BOOL CWipeProc::WipeContent()
{
//	int							nFileNum;
	BOOL						bHaveFile;
	struct tm					*newtime;
	time_t						ltime; 

	time(&ltime );
	newtime = localtime( &ltime );
	memcpy(&m_StartTime,newtime,sizeof(tm));

	CreateTmpFileInSou( m_DriveLetter,&m_nFileNum,&bHaveFile, &m_nTotalFileSize);
	if(WipePartition(m_nFileNum))
	{
		m_CancelButton.ShowWindow(SW_HIDE);
		m_NoSaveButton.ShowWindow(SW_SHOW);
		m_SaveButton.ShowWindow(SW_SHOW);
		m_WipeMothed.LoadString (IDS_DEL_SUCCESS);
		DeleteTempFile(m_DriveLetter,m_nFileNum);
		Sleep(100);
	}
	else
	{
		//DeleteTempFile(m_DriveLetter,nFileNum);
		//if (!g_bBeepOff)
		//{
		//	Beep(1500,500);
		//	Beep(1500,500);
		//	Beep(1500,500);
		//	Beep(1500,500);
		//	Beep(1500,500);
		//}
		//CDialog::OnCancel ();
		//return FALSE;
	}

	time(&ltime );
	newtime = localtime( &ltime );
	memcpy(&m_EndTime,newtime,sizeof(tm));
	return TRUE;
}

BOOL CWipeProc::WipeFiles( TCHAR szDriveLetter,int nFileNum,BYTE *pBuf )
{
	BOOL		bResult;
	HANDLE		hFile;
	int			nFiles;
	DWORD		dwFileSizeLo,dwFileSizeHigh;
	DWORD		i,j,k;
	DWORD		dwWrited;
    TCHAR        TempFileName[MAX_PATH] ;

	BOOL usingUserPath = FALSE;
	CString userPath = GetUserPath();
	if (szDriveLetter == _T('c') || szDriveLetter == _T('C')) {
		usingUserPath = TRUE;
	}
	for(nFiles=0;nFiles<nFileNum;nFiles++)
	{
		if (!usingUserPath) {
			TempFileName[0] = szDriveLetter ;
			TempFileName[1] = TempFileName[2]= STRING_END_CHAR ;
			_tcscat( TempFileName,_T(":\\"));
		}
		else {
			memset(TempFileName, 0, sizeof(TempFileName));
			_tcscat(TempFileName, userPath);
			_tcscat(TempFileName, _T("\\"));
		}
		_tcscat( TempFileName,TEMP_DATA_FILE_NAME ) ;
		if (nFiles)
		{
			_stprintf(TempFileName,_T("%s%d.dat"),TempFileName,nFiles);
		}
		else
		{
			_stprintf(TempFileName,_T("%s.dat"),TempFileName);
		}

		hFile =	CreateFile( TempFileName ,GENERIC_READ|GENERIC_WRITE,
							NULL,NULL,OPEN_EXISTING ,
							FILE_ATTRIBUTE_NORMAL ,NULL) ;
		if ( INVALID_HANDLE_VALUE == hFile )        
		{
			return FALSE;
		}
		dwFileSizeLo = GetFileSize(hFile,&dwFileSizeHigh);
		i = dwFileSizeLo / DATA_BUFFER_SIZE + dwFileSizeHigh * 0x4000;
		j = dwFileSizeLo % DATA_BUFFER_SIZE; 

		for(k=0;k<i;k++)
		{
			if(m_bCancelProc)
			{
				CloseHandle(hFile);
				return FALSE;
			}
			bResult = WriteFile(hFile,pBuf,DATA_BUFFER_SIZE,&dwWrited,NULL);
			m_nWipedSize += DATA_BUFFER_SIZE;
		}
		if(j)
		{
			if(m_bCancelProc)
			{
				CloseHandle(hFile);
				return FALSE;
			}
			bResult = WriteFile(hFile,pBuf,j,&dwWrited,NULL);
			m_nWipedSize += j;
		}
		CloseHandle(hFile);
	}
	return TRUE;
}

BOOL CWipeProc::WipePartition(int nFileNum)
{
	BOOL				bResult = TRUE;
	BYTE				*pBuf;
	int					i;

	pBuf = NULL;
	pBuf = (BYTE *) malloc(DATA_BUFFER_SIZE);
	if(!pBuf)
	{
		return FALSE;
	}
	switch(g_nWipeMothed)
	{
	case DELETE_WITH_ZERO:
		m_nTotalWipeSize = m_nTotalFileSize;
		InitDelBuf(pBuf,0,0,0,FALSE,TRUE);
		bResult = WipeFiles(m_DriveLetter,nFileNum,pBuf);
		break;
	case DELETE_WITH_FF:
		m_nTotalWipeSize = m_nTotalFileSize;
		InitDelBuf(pBuf,0XFF,0,0,FALSE,TRUE);
		bResult = WipeFiles(m_DriveLetter,nFileNum,pBuf);
		break;
	case DELETE_WITH_RANDOM:
		m_nTotalWipeSize = m_nTotalFileSize;
		InitDelBuf(pBuf,0,0,0,TRUE,TRUE);
		bResult = WipeFiles(m_DriveLetter,nFileNum,pBuf);
		break;
	case DELETE_NSA_MOTHED:
		m_nTotalWipeSize = m_nTotalFileSize*3;
		for(i=0;i<3;i++)
		{
			switch(i)
			{
			case 0:
			case 1:
				InitDelBuf(pBuf,0,0,0,TRUE,TRUE);
				break;
			case 2:
				InitDelBuf(pBuf,0,0,0,FALSE,TRUE);
				break;
			}
			if(!WipeFiles(m_DriveLetter,nFileNum,pBuf))
			{
				bResult = FALSE;
				break;
			}
		}
		break;
	case DELETE_OLD_NSA_MOTHED:
		m_nTotalWipeSize = m_nTotalFileSize*4;
		for(i=0;i<4;i++)
		{
			switch(i)
			{
			case 0:
			case 2:
				InitDelBuf(pBuf,0,0,0,FALSE,TRUE);
				break;
			case 1:
			case 3:
				InitDelBuf(pBuf,0XFF,0,0,FALSE,TRUE);
				break;
			}
			if(!WipeFiles(m_DriveLetter,nFileNum,pBuf))
			{
				bResult = FALSE;
				break;
			}
		}
		break;
	case DELETE_DOD_STD_MOTHED:
		m_nTotalWipeSize = m_nTotalFileSize*3;
		for(i=0;i<3;i++)
		{
			switch(i)
			{
			case 0:
				InitDelBuf(pBuf,0,0,0,FALSE,TRUE);
				break;
			case 1:
				InitDelBuf(pBuf,0XFF,0,0,FALSE,TRUE);
				break;
			case 2:
				InitDelBuf(pBuf,0X35,0XCA,0X97,FALSE,FALSE);
				break;
			}
			if(!WipeFiles(m_DriveLetter,nFileNum,pBuf))
			{
				bResult = FALSE;
				break;
			}
		}
		break;
	case DELETE_NATO_MOTHED:
		m_nTotalWipeSize = m_nTotalFileSize*7;
		for(i=0;i<7;i++)
		{
			switch(i)
			{
			case 0:
			case 2:
			case 4:
				InitDelBuf(pBuf,0,0,0,FALSE,TRUE);
				break;
			case 1:
			case 3:
			case 5:
				InitDelBuf(pBuf,0XFF,0,0,FALSE,TRUE);
				break;
			case 6:
				InitDelBuf(pBuf,0X35,0XCA,0X97,FALSE,FALSE);
				break;
			}
			if(!WipeFiles(m_DriveLetter,nFileNum,pBuf))
			{
				bResult = FALSE;
				break;
			}
		}
		break;
	case DELETE_GUTMANN_MOTHED:
		m_nTotalWipeSize = m_nTotalFileSize*35;
		for(i=0;i<35;i++)
		{
			switch(i)
			{
			case 0:
			case 1:
			case 2:
			case 3:
			case 31:
			case 32:
			case 33:
			case 34:
				InitDelBuf(pBuf,0,0,0,TRUE,TRUE);
				break;
			case 4:
				InitDelBuf(pBuf,0X55,0,0,FALSE,TRUE);
				break;
			case 5:
				InitDelBuf(pBuf,0XAA,0,0,FALSE,TRUE);
				break;
			case 6:
			case 25:
				InitDelBuf(pBuf,0X92,0X49,0X24,FALSE,FALSE);
				break;
			case 7:
			case 26:
				InitDelBuf(pBuf,0X49,0X24,0X92,FALSE,FALSE);
				break;
			case 8:
			case 27:
				InitDelBuf(pBuf,0X24,0X92,0X49,FALSE,FALSE);
				break;
			case 9:
				InitDelBuf(pBuf,0,0,0,FALSE,TRUE);
				break;
			case 10:
				InitDelBuf(pBuf,0X11,0,0,FALSE,TRUE);
				break;
			case 11:
				InitDelBuf(pBuf,0X22,0,0,FALSE,TRUE);
				break;
			case 12:
				InitDelBuf(pBuf,0X33,0,0,FALSE,TRUE);
				break;
			case 13:
				InitDelBuf(pBuf,0X44,0,0,FALSE,TRUE);
				break;
			case 14:
				InitDelBuf(pBuf,0X55,0,0,FALSE,TRUE);
				break;
			case 15:
				InitDelBuf(pBuf,0X66,0,0,FALSE,TRUE);
				break;
			case 16:
				InitDelBuf(pBuf,0x77,0,0,FALSE,TRUE);
				break;
			case 17:
				InitDelBuf(pBuf,0X88,0,0,FALSE,TRUE);
				break;
			case 18:
				InitDelBuf(pBuf,0X99,0,0,FALSE,TRUE);
				break;
			case 19:
				InitDelBuf(pBuf,0XAA,0,0,FALSE,TRUE);
				break;
			case 20:
				InitDelBuf(pBuf,0XBB,0,0,FALSE,TRUE);
				break;
			case 21:
				InitDelBuf(pBuf,0XCC,0,0,FALSE,TRUE);
				break;
			case 22:
				InitDelBuf(pBuf,0XDD,0,0,FALSE,TRUE);
				break;
			case 23:
				InitDelBuf(pBuf,0XEE,0,0,FALSE,TRUE);
				break;
			case 24:
				InitDelBuf(pBuf,0XFF,0,0,FALSE,TRUE);
				break;
			case 28:
				InitDelBuf(pBuf,0X6D,0XB6,0XDB,FALSE,FALSE);
				break;
			case 29:
				InitDelBuf(pBuf,0XB6,0XDB,0X6D,FALSE,FALSE);
				break;
			case 30:
				InitDelBuf(pBuf,0XDB,0X6D,0XB6,FALSE,FALSE);
				break;
			}
			if(!WipeFiles(m_DriveLetter,nFileNum,pBuf))
			{
				bResult = FALSE;
				break;
			}
		}
		break;
	}
	free(pBuf);
	return bResult;
}


void CWipeProc::OnCancel() 
{
	m_bCancelProc = TRUE;
	Sleep(100);
	DeleteTempFile(m_DriveLetter,m_nFileNum);
	if (!g_bBeepOff)
	{
		Beep(1500,500);
		Beep(1500,500);
		Beep(1500,500);
		Beep(1500,500);
		Beep(1500,500);
	}
	
	KillTimer(IDT_TIMER1);
	
	CDialog::OnCancel ();
}

void CWipeProc::OnNoSave() 
{

	UpdateData(TRUE);
	KillTimer(IDT_TIMER1);
	CDialog::OnOK();
}

void CWipeProc::OnSave() 
{

	UpdateData(TRUE);
	CString		csFile;
	CString		csFiter(_T("Text files (*.txt)|*.txt|All Files (*.*)|*.*||"));
	CFileDialog dlg(FALSE, _T("txt"), _T(""), OFN_FILEMUSTEXIST|OFN_HIDEREADONLY, csFiter);

	if( IDCANCEL == dlg.DoModal()) return ;
	else
	{
		do
		{
			csFile = dlg.GetPathName();
			//SaveReport(csFile.GetBuffer (0));
		}
		while(!SaveReport(csFile.GetBuffer (0)));
		CDialog::OnOK();
	}
}

BOOL CWipeProc::SaveReport(LPCTSTR lpFileName)
{
	HANDLE				hFile;
	CStringA				csInfo;
	DWORD				dwWrite;
	WORD				wData = 0x0a0d;
	CStringA				csMothed,csMonth;
	char				szFile[MAX_PATH];
	char				cDrive;
	ZeroMemory(szFile, MAX_PATH);
	WideCharToMultiByte(CP_ACP,0,lpFileName,_tcslen(lpFileName), szFile, MAX_PATH, NULL, NULL);
	hFile =	CreateFileA( szFile ,GENERIC_READ|GENERIC_WRITE,
						NULL,NULL,OPEN_ALWAYS ,
						FILE_ATTRIBUTE_NORMAL ,NULL) ;
	if ( INVALID_HANDLE_VALUE == hFile )        
	{
		return FALSE;
	}
	SetFilePointer(hFile,0,NULL,FILE_END);
	//Write wipe information line
	wctomb(&cDrive, m_DriveLetter);
	csInfo.Format (IDS_WIPE_PARTITION_REPORT,cDrive);
	WriteFile(hFile,csInfo.GetBuffer (0),csInfo.GetLength (),&dwWrite,NULL);
	WriteFile(hFile,&wData,sizeof(wData),&dwWrite,NULL);
	//Write monted line
	switch(g_nWipeMothed)
	{
	case 0:
		csMothed.LoadString (IDS_00_MOTHED);
		break;
	case 1:
		csMothed.LoadString (IDS_FF_MOTHED);
		break;
	case 2:
		csMothed.LoadString (IDS_RANDOM_MOTHED);
		break;
	case 3:
		csMothed.LoadString (IDS_NSA_MOTHED);
		break;
	case 4:
		csMothed.LoadString (IDS_OLD_NSA_MOTHED);
		break;
	case 5:
		csMothed.LoadString (IDS_DOD_STD_MOTHED);
		break;
	case 6:
		csMothed.LoadString (IDS_NATO_MOTHED);
		break;
	case 7:
		csMothed.LoadString (IDS_GUTMANN_MOTHED);
		break;
	}
	csInfo.Format(IDS_USED_MOTHED,csMothed);
	WriteFile(hFile,csInfo.GetBuffer (0),csInfo.GetLength (),&dwWrite,NULL);
	WriteFile(hFile,&wData,sizeof(wData),&dwWrite,NULL);
	//Write time stamp
	switch(m_StartTime.tm_mon)
	{
	case 0:
		csMonth.LoadString (IDS_JAN);
		break;
	case 1:
		csMonth.LoadString (IDS_FEB);
		break;
	case 2:
		csMonth.LoadString (IDS_MAR);
		break;
	case 3:
		csMonth.LoadString (IDS_APR);
		break;
	case 4:
		csMonth.LoadString (IDS_MAY);
		break;
	case 5:
		csMonth.LoadString (IDS_JUN);
		break;
	case 6:
		csMonth.LoadString (IDS_JUL);
		break;
	case 7:
		csMonth.LoadString (IDS_AUG);
		break;
	case 8:
		csMonth.LoadString (IDS_SEP);
		break;
	case 9:
		csMonth.LoadString (IDS_OCT);
		break;
	case 10:
		csMonth.LoadString (IDS_NOV);
		break;
	case 11:
		csMonth.LoadString (IDS_DEC);
		break;
	}
	csInfo.Format(IDS_TIME_STAMP,csMonth,(int)(m_StartTime.tm_mday),(int)(m_StartTime.tm_hour),
					(int)(m_StartTime.tm_min),(int)(m_StartTime.tm_sec),
					(int)(m_StartTime.tm_year+1900),
					csMonth,(int)(m_EndTime.tm_mday),(int)(m_EndTime.tm_hour),
					(int)(m_EndTime.tm_min),(int)(m_EndTime.tm_sec),
					(int)(m_EndTime.tm_year+1900));
	WriteFile(hFile,csInfo.GetBuffer (0),csInfo.GetLength (),&dwWrite,NULL);
	WriteFile(hFile,&wData,sizeof(wData),&dwWrite,NULL);
	WriteFile(hFile,&wData,sizeof(wData),&dwWrite,NULL);
	CloseHandle(hFile);
	return TRUE;
}

void CWipeProc::OnTimer(UINT nIDEvent) 
{
	float	fRemain = 0;
	DWORD	dwTimeRemain;

	if(m_nPresent < 100)
	{
		m_dwElaspedTime ++;
		if(m_nTotalWipeSize)
		{
			m_nPresent = (int)(m_nWipedSize*100/m_nTotalWipeSize);
		}
		m_WipeProgress.SetPos(m_nPresent);
		m_szPercent.Format (IDS_DEL_PERSENT,m_nPresent);
		m_WipePresent.SetWindowText (m_szPercent);

		m_csElasped.Format(IDS_TIME_INFO,m_dwElaspedTime /3600,m_dwElaspedTime%3600/60,m_dwElaspedTime%60);
	
		if(m_nTotalWipeSize)
		{
			fRemain  = (float(m_nTotalWipeSize-m_nWipedSize))/m_nTotalWipeSize;
		}
		dwTimeRemain = (DWORD)(fRemain*m_dwElaspedTime);
		m_csRemain.Format(IDS_TIME_INFO,dwTimeRemain /3600,dwTimeRemain%3600/60,dwTimeRemain%60);
	

		if(m_nPresent == 100 && !g_bBeepOff)
		{
			Beep(2000,500);
			Beep(2000,500);
			Beep(2000,500);
		}
		UpdateData(FALSE);
	}
	
	CDialog::OnTimer(nIDEvent);
}
