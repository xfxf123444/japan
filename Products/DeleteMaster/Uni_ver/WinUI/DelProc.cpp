// DelProc.cpp : implementation file
//

#include "stdafx.h"
#include "DeleteMaster.h"
#include "DelProc.h"
#include "DiskInfo.h"
#include "Fun.h"

extern	int					g_nDelMothed;
extern  __int64				g_nTotalDelSize;
extern	__int64				g_nDeletedSize;
extern	YG_PARTITION_INFO*	g_pTargetParInfo ;
extern  YG_PARTITION_INFO*	g_pFixDiskInfo;
extern  CString				g_ModuleNumber;
extern  CString				g_FirmWare;
extern  CString				g_SerNumber;
extern  CString				g_Capacity;
extern  BOOL				g_bVerify;
extern  BOOL				g_bFormat;
extern  BOOL                g_bBeepOff;
extern	DWORD				g_dwMaxTry;

/////////////////////////////////////////////////////////////////////////////
// CDelProc dialog


CDelProc::CDelProc(CWnd* pParent /*=NULL*/)
	: CDialog(CDelProc::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDelProc)
	m_szPercent = _T("");
	m_szTar = _T("");
	m_szDelMothed = _T("");
	m_csElasped = _T("");
	m_csRemain = _T("");
	//}}AFX_DATA_INIT
}


void CDelProc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDelProc)
	DDX_Control(pDX, IDCANCEL, m_CancelButton);
	DDX_Control(pDX, IDC_SAVE, m_SaveButton);
	DDX_Control(pDX, IDC_NO_SAVE, m_NoSaveButton);
	DDX_Control(pDX, IDC_COMPLETE_PECNET, m_DelPresent);
	DDX_Control(pDX, IDC_DELETE_PROGRESS, m_DelProgress);
	DDX_Text(pDX, IDC_COMPLETE_PECNET, m_szPercent);
	DDX_Text(pDX, IDC_DEL_PROGRESS_TAR, m_szTar);
	DDX_Text(pDX, IDC_DEL_PROGRESS_MOTHED, m_szDelMothed);
	DDX_Text(pDX, IDC_TIME_ELASPED, m_csElasped);
	DDX_Text(pDX, IDC_TIME_REMAINING, m_csRemain);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDelProc, CDialog)
	//{{AFX_MSG_MAP(CDelProc)
	ON_BN_CLICKED(IDC_NO_SAVE, OnNoSave)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDelProc message handlers

BOOL CDelProc::OnInitDialog() 
{
	TCHAR              *p;
	DWORD				threadID;
	CString				csMothed;
	CDialog::OnInitDialog();

	GetModuleFileName(NULL,m_szLogFile,MAX_PATH);
	p = strrchrpro(m_szLogFile,_T('\\'));
	_tcscpy(p,_T("\\verify.log"));

	m_DelProgress.SetRange (0,100);
	m_DelProgress.SetPos(0);

	m_nPresent		 = 0;
	m_dwElaspedTime  = 0;
	m_bCancelProc	 = FALSE; 
	m_bShowVerifyMsg = TRUE;
	m_NoSaveButton.ShowWindow(SW_HIDE);
	m_SaveButton.ShowWindow(SW_HIDE);

	if(m_bDisk)
	{
		if(m_nDisk >= _T('A'))
		{
			if(HIWORD(m_nDisk))
				m_szTar.Format(IDS_CONFIRM_RD,m_nDisk,m_dwMinSec,m_dwMaxSec);
			else
				m_szTar.Format(IDS_CONFIRM_FD,m_nDisk,m_dwMinSec,m_dwMaxSec);
		}
		else
			m_szTar.Format(IDS_CONFIRM_DISK,m_nDisk+1,m_dwMinSec,m_dwMaxSec);
	}
	else
		m_szTar.Format(IDS_CONFIRM_PARTITION,m_DriveLetter,m_nDisk+1,
						m_dwMinSec,m_dwMaxSec);

	switch(g_nDelMothed)
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
	m_szDelMothed.Format(IDS_DEL_PROGRESS_MOTHED,csMothed);
	UpdateData(FALSE);

	SetTimer(IDT_TIMER1,1000,NULL);

	m_Thread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)CDelProc::ThreadProc ,
							(LPVOID)this,0,&threadID);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

DWORD WINAPI CDelProc::ThreadProc(CDelProc* dlg)
{
	DWORD ret = dlg->ProgressProc();
	return ret;
}

DWORD CDelProc::ProgressProc()
{
	DeleteContent();
	return 0;
}

BOOL CDelProc::DeleteContent()
{
	PARTITION_ENTRY				PartitionInfo;
	//CREATE_PAR_FLAG				cfg;
	//DWORD						dwFlag;
	//BYTE						btLabel[0x0b];
	//int							nErr;
	BOOL						bResult = TRUE;
	struct tm					*newtime;
	time_t						ltime; 
	CString						cstr,csCaption;

	csCaption.LoadString (IDS_DM_ERROR);
	
	time(&ltime );
	newtime = localtime( &ltime );
	memcpy(&m_StartTime,newtime,sizeof(tm));
	
	memset(&PartitionInfo,0,sizeof(PARTITION_ENTRY));
//	if(!m_bDisk)
//	{
//		cfg.Extended = 1;
//		cfg.Active	 = m_bActive;
//		cstr.LoadString (IDS_DELETE_DELETE_PAR_FAIL);
//
//		bResult = DeleteSectors();
//		if(bResult)
//		{
//			if(!DeletePartition(m_dwMinSec,m_nDisk+DISK_BASE,cfg,&nErr))
//			{
//				MessageBox(cstr,csCaption,MB_OK|MB_ICONSTOP);
//			}
//			PartitionInfo.StartSector = m_dwMinSec - DISK_MIN_SECTOR;
//			PartitionInfo.SectorsInPartition = g_pTargetParInfo->dwPartSize + DISK_MIN_SECTOR ;
//			PartitionInfo.SystemFlag = (BYTE)g_pTargetParInfo->dwSystemFlag ;
//			if(PartitionInfo.SystemFlag > 0x20)
//			{
//				PartitionInfo.SystemFlag = 0x0b;
//			}
//			dwFlag = g_pTargetParInfo->bLogic ?LOGICAL : PRIMARY;
//			memcpy(&btLabel,g_pTargetParInfo->szLabel ,0x0b);
//			if(g_bFormat)
//			{
//				CreatePartition(&PartitionInfo,m_nDisk+DISK_BASE,dwFlag,
//					TRUE,btLabel,m_hWnd,&nErr);
//			}
//		}
//	}
//	else
//	{
		bResult = DeleteSectors();
    //}
	
	if(bResult)
	{
		m_CancelButton.ShowWindow(SW_HIDE);
		m_NoSaveButton.ShowWindow(SW_SHOW);
		m_SaveButton.ShowWindow(SW_SHOW);
		m_szDelMothed.LoadString (IDS_DEL_SUCCESS);
		Sleep(100);
		//UpdateData(FALSE);
	}
	time(&ltime );
	newtime = localtime( &ltime );
	memcpy(&m_EndTime,newtime,sizeof(tm));

	return bResult;
}

BOOL CDelProc::DeleteSectors()
{
	BYTE				*pBuf;
	DWORD				dwSize;
	int					i;
	BOOL				bVerify = FALSE;

	pBuf = NULL;
	pBuf = (BYTE *) malloc(DATA_BUFFER_SIZE);
	if(!pBuf)
	{
		return FALSE;
	}
	dwSize = m_dwMaxSec - m_dwMinSec;
	g_nDeletedSize  = 0;
	switch(g_nDelMothed)
	{
	case DELETE_WITH_ZERO:
		g_nTotalDelSize = dwSize;
		InitDelBuf(pBuf,0,0,0,FALSE,TRUE);
		bVerify = g_bVerify;
		WriteSecsWithBuf(m_dwMinSec,dwSize,pBuf,bVerify);
		break;
	case DELETE_WITH_FF:
		g_nTotalDelSize = dwSize;
		InitDelBuf(pBuf,0XFF,0,0,FALSE,TRUE);
		bVerify = g_bVerify;
		WriteSecsWithBuf(m_dwMinSec,dwSize,pBuf,bVerify);
		break;
	case DELETE_WITH_RANDOM:
		g_nTotalDelSize = dwSize;
		InitDelBuf(pBuf,0,0,0,TRUE,TRUE);
		bVerify = g_bVerify;
		WriteSecsWithBuf(m_dwMinSec,dwSize,pBuf,bVerify);
		break;
	case DELETE_NSA_MOTHED:
		g_nTotalDelSize = (__int64)dwSize*3;
		for(i=0;i<3;i++)
		{
			switch(i)
			{
			case 0:
			case 1:
				InitDelBuf(pBuf,0,0,0,TRUE,TRUE);
				break;
			case 2:
				g_bVerify = TRUE;
				InitDelBuf(pBuf,0,0,0,FALSE,TRUE);
				break;
			}
			if(!WriteSecsWithBuf(m_dwMinSec,dwSize,pBuf,bVerify))
				break;
		}
		break;
	case DELETE_OLD_NSA_MOTHED:
		g_nTotalDelSize = (__int64)dwSize*4;
		for(i=0;i<4;i++)
		{
			switch(i)
			{
			case 0:
			case 2:
				InitDelBuf(pBuf,0,0,0,FALSE,TRUE);
				break;
			case 1:
				InitDelBuf(pBuf,0XFF,0,0,FALSE,TRUE);
				break;
			case 3:
				g_bVerify = TRUE;
				InitDelBuf(pBuf,0XFF,0,0,FALSE,TRUE);
				break;
			}
			if(!WriteSecsWithBuf(m_dwMinSec,dwSize,pBuf,bVerify))
				break;
		}
		break;
	case DELETE_DOD_STD_MOTHED:
		g_nTotalDelSize = (__int64)dwSize*3;
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
				g_bVerify = TRUE;
				InitDelBuf(pBuf,0X35,0XCA,0X97,FALSE,FALSE);
				break;
			}
			if(!WriteSecsWithBuf(m_dwMinSec,dwSize,pBuf,bVerify))
				break;
		}
		break;
	case DELETE_NATO_MOTHED:
		g_nTotalDelSize = (__int64)dwSize*7;
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
				g_bVerify = TRUE;
				InitDelBuf(pBuf,0X35,0XCA,0X97,FALSE,FALSE);
				break;
			}
			if(!WriteSecsWithBuf(m_dwMinSec,dwSize,pBuf,bVerify))
				break;
		}
		break;
	case DELETE_GUTMANN_MOTHED:
		g_nTotalDelSize = (__int64)dwSize*35;
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
				InitDelBuf(pBuf,0,0,0,TRUE,TRUE);
				break;
			case 34:
				g_bVerify = TRUE;
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
			if(!WriteSecsWithBuf(m_dwMinSec,dwSize,pBuf,bVerify))
				break;
		}
		break;
	}
	
	free(pBuf);
	return TRUE;
}

BOOL CDelProc::WriteSecsWithBuf(DWORD dwStartSec,DWORD dwSize,BYTE *pBuf,BOOL bVerify)
{
	DWORD				i,j,k,nTry;
	BOOL				bResult;
	CString				cstr,csCaption;
	BYTE				*pCompBuf;

	pCompBuf = NULL;
	csCaption.LoadString (IDS_DM_ERROR);

	BIOS_DRIVE_PARAM	DriveParam;

	if(m_nDisk >= _T('A'))
	{
		if(!GetFDParam(m_nDisk,&DriveParam)) return FALSE;
		if(HIWORD(m_nDisk))
		{
			if(!GetRemovableDiskSize(DISK_BASE+HIWORD(m_nDisk), NULL,(DWORD*)(&DriveParam.dwSectors)))
				return FALSE;
		}
	}
	else
	{
		if(!GetDriveParam(m_nDisk+DISK_BASE,&DriveParam)) return FALSE;
	}

	if(bVerify)
	{
		pCompBuf = (BYTE *) malloc(DATA_BUFFER_SIZE);
		if(!pCompBuf)
			return FALSE;
	}

	i = dwSize / DATA_BUFFER_SECTOR;
    j = dwSize % DATA_BUFFER_SECTOR;
	for (k=0; k<i ;k++)
	{
		nTry = 0;
TRY_LABEL_1:
		if(m_bCancelProc)
		{
			if(pCompBuf) free(pCompBuf);
			return FALSE;
		}
		if(m_nDisk >= _T('A'))
		{
			bResult = WriteFDSector(dwStartSec+k*DATA_BUFFER_SECTOR,DATA_BUFFER_SECTOR,pBuf,m_nDisk,&DriveParam);
			if(bVerify && bResult)
			{
				ReadFDSector(dwStartSec+k*DATA_BUFFER_SECTOR,DATA_BUFFER_SECTOR,pCompBuf,m_nDisk,&DriveParam);
				if(memcmp(pBuf,pCompBuf,DATA_BUFFER_SIZE))
				{
					if(m_bShowVerifyMsg)
					{
						cstr.Format(IDS_VERIFY_FAIL_WRAN_MSG,m_szLogFile);
						MessageBox (cstr,csCaption,MB_OK);
						m_bShowVerifyMsg = FALSE;
					}
					SaveVerifyInfo(m_szLogFile,dwStartSec+k*DATA_BUFFER_SECTOR,DATA_BUFFER_SECTOR,FALSE);
				}
			}
		}
		else
		{
			bResult = WriteSector(dwStartSec+k*DATA_BUFFER_SECTOR,DATA_BUFFER_SECTOR,pBuf,m_nDisk+DISK_BASE,&DriveParam);
			if(bVerify && bResult)
			{
				ReadSector(dwStartSec+k*DATA_BUFFER_SECTOR,DATA_BUFFER_SECTOR,pCompBuf,m_nDisk+DISK_BASE,&DriveParam);
				if(memcmp(pBuf,pCompBuf,DATA_BUFFER_SIZE))
				{
					if(m_bShowVerifyMsg)
					{
						cstr.Format(IDS_VERIFY_FAIL_WRAN_MSG,m_szLogFile);
						MessageBox (cstr,csCaption,MB_OK);
						m_bShowVerifyMsg = FALSE;
					}
					SaveVerifyInfo(m_szLogFile,dwStartSec+k*DATA_BUFFER_SECTOR,DATA_BUFFER_SECTOR,FALSE);
				}
			}
		}
		if(!bResult)
		{
			if (g_dwMaxTry == 0)
			{
				cstr.Format (IDS_WRITE_SEC_FAIL,dwStartSec+k*DATA_BUFFER_SECTOR);
				if(IDNO == MessageBox (cstr,csCaption,MB_YESNO))
					m_bCancelProc = TRUE;
			}
			if (nTry < g_dwMaxTry)	
			{
				nTry ++;
				SaveVerifyInfo(m_szLogFile,dwStartSec+k*DATA_BUFFER_SECTOR,DATA_BUFFER_SECTOR,TRUE);
				goto TRY_LABEL_1;
			}
		}
		g_nDeletedSize += DATA_BUFFER_SECTOR;
	}
	if (j)
	{
		nTry = 0;
TRY_LABEL_2:
		if(m_bCancelProc)
		{
			if(pCompBuf) free(pCompBuf);
			return FALSE;
		}

		if(m_nDisk >= 'A')
		{
			bResult = WriteFDSector(dwStartSec+i*DATA_BUFFER_SECTOR,(WORD)j,pBuf,m_nDisk,&DriveParam);
			if(bVerify && bResult)
			{
				ReadFDSector(dwStartSec+i*DATA_BUFFER_SECTOR,(WORD)j,pCompBuf,m_nDisk,&DriveParam);
				if(memcmp(pBuf,pCompBuf,j*SECTORSIZE))
				{
					if(m_bShowVerifyMsg)
					{
						cstr.Format(IDS_VERIFY_FAIL_WRAN_MSG,m_szLogFile);
						MessageBox (cstr,csCaption,MB_OK);
						m_bShowVerifyMsg = FALSE;
					}
					SaveVerifyInfo(m_szLogFile,dwStartSec+i*DATA_BUFFER_SECTOR,j,FALSE);
				}
			}
		}
		else
		{
			bResult = WriteSector(dwStartSec+i*DATA_BUFFER_SECTOR,(WORD)j,pBuf,m_nDisk+DISK_BASE,&DriveParam);
			if(bVerify && bResult)
			{
				ReadSector(dwStartSec+i*DATA_BUFFER_SECTOR,(WORD)j,pCompBuf,m_nDisk+DISK_BASE,&DriveParam);
				if(memcmp(pBuf,pCompBuf,j*SECTORSIZE))
				{
					if(m_bShowVerifyMsg)
					{
						cstr.Format(IDS_VERIFY_FAIL_WRAN_MSG,m_szLogFile);
						MessageBox (cstr,csCaption,MB_OK);
						m_bShowVerifyMsg = FALSE;
					}
					SaveVerifyInfo(m_szLogFile,dwStartSec+i*DATA_BUFFER_SECTOR,j,FALSE);
				}
			}
		}
		if(!bResult)
		{
			if (g_dwMaxTry == 0)
			{
				cstr.Format (IDS_WRITE_SEC_FAIL,dwStartSec+k*DATA_BUFFER_SECTOR);
				if(IDNO == MessageBox (cstr,csCaption,MB_YESNO))
					m_bCancelProc = TRUE;
			}
			if (nTry < g_dwMaxTry)
			{
				nTry ++;
				SaveVerifyInfo(m_szLogFile,dwStartSec+i*DATA_BUFFER_SECTOR,j,TRUE);
				goto TRY_LABEL_2;
			}
		}
		g_nDeletedSize += DATA_BUFFER_SECTOR;
	}
	if(pCompBuf) free(pCompBuf);
	return TRUE;
}

void CDelProc::OnCancel() 
{
	m_bCancelProc = TRUE;
	Sleep(2000);
	Beep(1500,500);
	Beep(1500,500);
	Beep(1500,500);
	Beep(1500,500);
	Beep(1500,500);
	KillTimer(IDT_TIMER1);
	CDialog::OnCancel ();
}

void CDelProc::OnNoSave() 
{
	UpdateData(TRUE);
	KillTimer(IDT_TIMER1);
	CDialog::OnOK();
}

void CDelProc::OnSave() 
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

BOOL CDelProc::SaveReport(LPCTSTR lpFileName)
{
	BOOL				bShowDiskInfo = FALSE;
	HANDLE				hFile;
	CStringA				csInfo;
	DWORD				dwWrite;
	WORD				wData = 0x0a0d;
	BIOS_DRIVE_PARAM	DriveParam;
	CStringA				csMothed,csMonth;
	BYTE				btDisk;
	char				szFile[MAX_PATH];
	char				cDrive;
	ZeroMemory(szFile, MAX_PATH);
	WideCharToMultiByte(CP_ACP,0,lpFileName,_tcslen(lpFileName), szFile, MAX_PATH, NULL, NULL);
	hFile =	CreateFileA( szFile ,GENERIC_READ|GENERIC_WRITE,
						NULL,NULL,OPEN_ALWAYS ,
						FILE_ATTRIBUTE_NORMAL ,NULL) ;
	wctomb(&cDrive, m_DriveLetter);
	if ( INVALID_HANDLE_VALUE == hFile )        
	{
		return FALSE;
	}
	SetFilePointer(hFile,0,NULL,FILE_END);
	//Write delete information line
	if(m_bDisk)
	{
		//GetDriveParam(DISK_BASE+g_pFixDiskInfo->btDiskNum,&DriveParam);
		if(cDrive >= 'A')
		{
			GetFDParam(cDrive,&DriveParam);
			csInfo.Format (IDS_DELETE_FD_REPORT,cDrive,
							DriveParam.dwCylinders,DriveParam.dwHeads,DriveParam.dwSecPerTrack);
		}
		else
		{
			bShowDiskInfo = TRUE;
			btDisk		  = g_pFixDiskInfo->btDiskNum;
			GetDriveParam(DISK_BASE+g_pFixDiskInfo->btDiskNum,&DriveParam);
			csInfo.Format (IDS_DELETE_DISK_REPORT,(int)(g_pFixDiskInfo->btDiskNum +1),
							DriveParam.dwCylinders,DriveParam.dwHeads,DriveParam.dwSecPerTrack);
		}
	}
	else
	{
		bShowDiskInfo = TRUE;
		btDisk		  = g_pTargetParInfo->btDiskNum; 
		csInfo.Format (IDS_DELETE_PARTITION_REPORT,g_pTargetParInfo->DriveLetter,
						(int)(g_pTargetParInfo->btDiskNum +1),m_dwMinSec,m_dwMaxSec);
	}
	WriteFile(hFile,csInfo.GetBuffer (0),csInfo.GetLength (),&dwWrite,NULL);
	WriteFile(hFile,&wData,sizeof(wData),&dwWrite,NULL);
	if(bShowDiskInfo)
	{
		HDInfont(btDisk,m_hWnd);
		csInfo = "----------Disk Information----------";
		WriteFile(hFile,csInfo.GetBuffer (0),csInfo.GetLength (),&dwWrite,NULL);
		WriteFile(hFile,&wData,sizeof(wData),&dwWrite,NULL);

		csInfo.Format(IDS_DISKINFO_MODULE,g_ModuleNumber );
		WriteFile(hFile,csInfo.GetBuffer (0),csInfo.GetLength (),&dwWrite,NULL);
		WriteFile(hFile,&wData,sizeof(wData),&dwWrite,NULL);

		csInfo.Format(IDS_DISKINFO_FIRMWARE_VER,g_FirmWare);
		WriteFile(hFile,csInfo.GetBuffer (0),csInfo.GetLength (),&dwWrite,NULL);
		WriteFile(hFile,&wData,sizeof(wData),&dwWrite,NULL);
	
		csInfo.Format(IDS_DISKINFO_SERIAL_NUM,g_SerNumber);
		WriteFile(hFile,csInfo.GetBuffer (0),csInfo.GetLength (),&dwWrite,NULL);
		WriteFile(hFile,&wData,sizeof(wData),&dwWrite,NULL);

		csInfo.Format(IDS_DISKINFO_CAPACITY,g_Capacity);
		WriteFile(hFile,csInfo.GetBuffer (0),csInfo.GetLength (),&dwWrite,NULL);
		WriteFile(hFile,&wData,sizeof(wData),&dwWrite,NULL);
	}
	//Write monted line
	switch(g_nDelMothed)
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

void CDelProc::OnTimer(UINT nIDEvent) 
{
	float	fRemain;
	DWORD	dwTimeRemain;

	if(m_nPresent < 100)
	{
		m_dwElaspedTime ++;
		m_nPresent = (int)(g_nDeletedSize*100/g_nTotalDelSize);
		m_DelProgress.SetPos(m_nPresent);
		m_szPercent.Format (IDS_DEL_PERSENT,m_nPresent);
		m_DelPresent.SetWindowText (m_szPercent);

		m_csElasped.Format(IDS_TIME_INFO,m_dwElaspedTime /3600,m_dwElaspedTime%3600/60,m_dwElaspedTime%60);
	
		fRemain  = (float(g_nTotalDelSize-g_nDeletedSize))/g_nDeletedSize;
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

BOOL CDelProc::SaveVerifyInfo(LPCTSTR lpFileName,DWORD dwStartSec,DWORD dwSectors,BOOL bWriteFail)
{
	BOOL				bShowDiskInfo = FALSE;
	HANDLE				hFile;
	CStringA				csInfo;
	DWORD				dwWrite;
	WORD				wData = 0x0a0d;
	BIOS_DRIVE_PARAM	DriveParam;
	CStringA				csMothed,csMonth;
	BYTE				btDisk;
	char				szFile[MAX_PATH];
	char				cDrive;
	ZeroMemory(szFile, MAX_PATH);
	WideCharToMultiByte(CP_ACP,0,lpFileName,_tcslen(lpFileName), szFile, MAX_PATH, NULL, NULL);
	hFile =	CreateFileA( szFile ,GENERIC_READ|GENERIC_WRITE,
		NULL,NULL,OPEN_ALWAYS ,
		FILE_ATTRIBUTE_NORMAL ,NULL) ;
	wctomb(&cDrive, m_DriveLetter);

	if ( INVALID_HANDLE_VALUE == hFile )        
	{
		return FALSE;
	}
	SetFilePointer(hFile,0,NULL,FILE_END);
	//Write delete information line
	if(m_bDisk)
	{
		//GetDriveParam(DISK_BASE+g_pFixDiskInfo->btDiskNum,&DriveParam);
		if(cDrive >= 'A')
		{
			GetFDParam(cDrive,&DriveParam);
			csInfo.Format (IDS_DELETE_FD_REPORT,cDrive,
							DriveParam.dwCylinders,DriveParam.dwHeads,DriveParam.dwSecPerTrack);
		}
		else
		{
			bShowDiskInfo = TRUE;
			btDisk		  = g_pFixDiskInfo->btDiskNum;
			GetDriveParam(DISK_BASE+g_pFixDiskInfo->btDiskNum,&DriveParam);
			csInfo.Format (IDS_DELETE_DISK_REPORT,(int)(g_pFixDiskInfo->btDiskNum +1),
							DriveParam.dwCylinders,DriveParam.dwHeads,DriveParam.dwSecPerTrack);
		}
	}
	else
	{
		bShowDiskInfo = TRUE;
		btDisk		  = g_pTargetParInfo->btDiskNum; 
		csInfo.Format (IDS_DELETE_PARTITION_REPORT,g_pTargetParInfo->DriveLetter,
						(int)(g_pTargetParInfo->btDiskNum +1),m_dwMinSec,m_dwMaxSec);
	}
	WriteFile(hFile,csInfo.GetBuffer (0),csInfo.GetLength (),&dwWrite,NULL);
	WriteFile(hFile,&wData,sizeof(wData),&dwWrite,NULL);
	if(bShowDiskInfo)
	{
		HDInfont(btDisk,m_hWnd);
		csInfo = "----------Disk Information----------";
		WriteFile(hFile,csInfo.GetBuffer (0),csInfo.GetLength (),&dwWrite,NULL);
		WriteFile(hFile,&wData,sizeof(wData),&dwWrite,NULL);

		csInfo.Format(IDS_DISKINFO_MODULE,g_ModuleNumber );
		WriteFile(hFile,csInfo.GetBuffer (0),csInfo.GetLength (),&dwWrite,NULL);
		WriteFile(hFile,&wData,sizeof(wData),&dwWrite,NULL);

		csInfo.Format(IDS_DISKINFO_FIRMWARE_VER,g_FirmWare);
		WriteFile(hFile,csInfo.GetBuffer (0),csInfo.GetLength (),&dwWrite,NULL);
		WriteFile(hFile,&wData,sizeof(wData),&dwWrite,NULL);
	
		csInfo.Format(IDS_DISKINFO_SERIAL_NUM,g_SerNumber);
		WriteFile(hFile,csInfo.GetBuffer (0),csInfo.GetLength (),&dwWrite,NULL);
		WriteFile(hFile,&wData,sizeof(wData),&dwWrite,NULL);

		csInfo.Format(IDS_DISKINFO_CAPACITY,g_Capacity);
		WriteFile(hFile,csInfo.GetBuffer (0),csInfo.GetLength (),&dwWrite,NULL);
		WriteFile(hFile,&wData,sizeof(wData),&dwWrite,NULL);
	}
	//Write monted line
	switch(g_nDelMothed)
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

	if (bWriteFail)
		csInfo.Format (IDS_WRITE_LOG,dwStartSec,dwSectors);
	else csInfo.Format (IDS_VERIFY_LOG,dwStartSec,dwSectors);
	WriteFile(hFile,csInfo.GetBuffer (0),csInfo.GetLength (),&dwWrite,NULL);
	WriteFile(hFile,&wData,sizeof(wData),&dwWrite,NULL);
	CloseHandle(hFile);
	return TRUE;
}

BOOL CDelProc::LockDiskOrVolume()
{
	m_volumeHandleSet.clear();
	BOOL bRes = FALSE;
	if (m_bDisk) {
		for (vector<TCHAR>::iterator it = m_driveLetterSet.begin(); it != m_driveLetterSet.end(); ++it){
			TCHAR driveLetter = *it;
			bRes = LockVolume(driveLetter);
			if (!bRes) {
				break;
			}
		}
	}
	else {
		bRes = LockVolume(m_DriveLetter);
	}

	if (!bRes) {
		UnlockDiskOrVolume();
	}
	return bRes;
}

void CDelProc::UnlockDiskOrVolume()
{
	for (vector<HANDLE>::iterator it = m_volumeHandleSet.begin();
		it != m_volumeHandleSet.end();
		++it){
			CloseHandle(*it);
	}
	m_volumeHandleSet.clear();
}

BOOL CDelProc::LockVolume(TCHAR driveLetter)
{
	CString path;
	BOOL bRes;
	path.Format(_T("\\\\.\\%c:"),  driveLetter);
	HANDLE hVolume = CreateFile( path, GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
	if (hVolume != INVALID_HANDLE_VALUE) {
		DWORD dwReturned;
		bRes = DeviceIoControl( hVolume, FSCTL_LOCK_VOLUME, 0, 0, 0, 0, &dwReturned, 0 );
		if (bRes) {
			bRes = DeviceIoControl( hVolume, FSCTL_DISMOUNT_VOLUME, 0, 0, 0, 0, &dwReturned, 0 );
		}
		if (bRes) {
			m_volumeHandleSet.push_back(hVolume);
		}
	}

	return bRes;
}
