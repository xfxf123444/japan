// EncryptDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AntiType.h"
#include "AntiTypeDlg.h"
#include "EncryptDialog.h"
#include <windows.h>
#include <stdlib.h>
#include <string.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEncryptDialog dialog

extern HWND    g_handle;

ULONG YGNSMCheckSum(UCHAR *pBuffer,ULONG ulSize)
{
	ULONG i,k=0;
	UCHAR ucCheckSum[4] = {0,0,0,0};
	for (i = 0;i < ulSize;i ++)
	{
		if (k >= 4) k = 0;
		ucCheckSum[k] += pBuffer[i];
		k++;
	}
	return *((ULONG *)&ucCheckSum[0]);
}

BOOL YGEncryptBootSector(UCHAR *pBuf,PBYTE pKey,int nKeyLen)
{
	WORD wEncyptMark = 'YG';

	YGAESEncryptData(pKey,nKeyLen,pBuf,pBuf,256);

	for (int k = 128;k <= 255;k++)
	{
		((WORD *)pBuf)[k] ^= wEncyptMark;
	}

	return TRUE;
}

BOOL YGDecryptBootSector(UCHAR *pBuf,PBYTE pKey,int nKeyLen)
{
	WORD wDecryptMark = 'YG';

	YGAESDecryptData(pKey,nKeyLen,pBuf,pBuf,256);

	for (int k = 128;k <= 255;k++)
	{
		((WORD *)pBuf)[k] ^= wDecryptMark;
	}

	return TRUE;
}

UINT DoEncrypt(LPVOID pParam)
{
	UCHAR	DESKey[32];
	int index;
	CString tempStr;
	//PBYTE tempPointer;
	long leftSectors;
	BYTE checkSum;
	BYTE encryptMark;
	BYTE encryptType = 255;
	BYTE keyLen = 255;
	DWORD tempStartLogicalSector;
	DWORD EncryptedSectorNum = 0;
 
	DWORD dwResult;
	BOOL bResult;
    HANDLE hVolume;

	PBIOS_DRIVE_PARAM pDriveParam;
    PPARTITION_INFO_2000 pParInfo;

	WORD wSectors = 256;
	PBYTE pBuf;

	CEncryptDialog *dlg = (CEncryptDialog *)pParam;
	tempStr = driverName.Mid(0,1);
	char *directory = (LPSTR)(LPCTSTR)tempStr;
	NormalKeyToDESKey(dlg->m_encryptPassword.GetBuffer(0),DESKey);

	pBuf = (PBYTE)malloc(256 * 512);
	pDriveParam = (PBIOS_DRIVE_PARAM)malloc(sizeof(BIOS_DRIVE_PARAM));
	pParInfo = (PPARTITION_INFO_2000)malloc(sizeof(PARTITION_INFO_2000));

	bResult = PartitionInfoByDriveLetter(*directory,pParInfo);

	if (bResult)
	{
		GetDriveParam(pParInfo->nHardDiskNum,pDriveParam);
		tempStartLogicalSector = pParInfo->pi.StartingOffset.QuadPart/pDriveParam->SectorsSize;
		int nTotalNum = (int)((pParInfo->pi.PartitionLength.QuadPart/512 - 1)/256);
		DWORD dwTotalSectors = pParInfo->pi.PartitionLength.QuadPart/512 - 1 - (pParInfo->pi.PartitionLength.QuadPart/512 - 1) % 256;
		index = dlg->m_encryptType.GetCurSel();
		
		tempStr.Format("\\\\.\\%c:",*directory);
		
		checkSum = (BYTE)YGNSMCheckSum((UCHAR *)dlg->m_encryptPassword.GetBuffer(0),dlg->m_encryptPassword.GetLength());
		checkSum = checkSum >> 1;
		encryptMark = 128;
		
		encryptType = (encryptType >> 5) & (index + 1);
		encryptType = encryptType << 5;
		keyLen = (keyLen >> 3) & (dlg->m_encryptPassword.GetLength());
		
		ReadSector(tempStartLogicalSector,1,pBuf,pParInfo->nHardDiskNum,pDriveParam);
		((PYGCRYDISK_BOOTSECTOR)pBuf)->uMark = 'Y';
		YGEncryptBootSector(pBuf,(UCHAR *)dlg->m_encryptPassword.GetBuffer(0),dlg->m_encryptPassword.GetLength());
		((PYGCRYDISK_BOOTSECTOR)pBuf)->uCheckSumAndEncryptMark = checkSum | encryptMark;
		((PYGCRYDISK_BOOTSECTOR)pBuf)->uEncrytTypeAndKeyLen = encryptType | keyLen;
		WriteSector(tempStartLogicalSector,1,pBuf,pParInfo->nHardDiskNum,pDriveParam);
		
		tempStartLogicalSector++;
		int nPercent;
		
		for (int i=1; i <= nTotalNum; i++)
		{
			ReadSector(tempStartLogicalSector,wSectors,pBuf,
				            pParInfo->nHardDiskNum,pDriveParam);
			
			switch(index)
			{
			case 0: YGAESEncryptData((UCHAR *)dlg->m_encryptPassword.GetBuffer(0),dlg->m_encryptPassword.GetLength(),pBuf,pBuf,256 * 512);break;
			case 1: YGDESEncryCode(DESKey,pBuf,pBuf,256 * 512,YGDESCRYPTO);break;
			case 2: YGDESEncryCode(DESKey,pBuf,pBuf,256 * 512,YGDESEDE2);break;
			case 3: YGDESEncryCode(DESKey,pBuf,pBuf,256 * 512,YGDESEDE3);break;
			case 4: YGDESEncryCode(DESKey,pBuf,pBuf,256 * 512,YGDESXEX3);break;
			}
			
			WriteSector(tempStartLogicalSector,wSectors,pBuf,
				pParInfo->nHardDiskNum,pDriveParam);
			EncryptedSectorNum += 256;
			nPercent = (int)(EncryptedSectorNum * 100/dwTotalSectors);
			dlg->m_encryptProgress.SetPos(nPercent);
			tempStartLogicalSector += 256;
			
			if (g_bAbort == TRUE)
			{
				dlg->UndoEncrypt((UCHAR *)dlg->m_encryptPassword.GetBuffer(0),dlg->m_encryptPassword.GetLength(),pBuf,index + 1,EncryptedSectorNum);
				
				free(pBuf);
				free(pDriveParam);
				free(pParInfo);
				
				(dlg->GetDlgItem(IDC_START_ENCRYPT))->EnableWindow(TRUE);
				(dlg->GetDlgItem(IDCANCEL))->EnableWindow(TRUE);
				(dlg->GetDlgItem(IDC_ABORT_ENCRYPT))->EnableWindow(FALSE);
				
				g_bAbort = FALSE;
				DWORD ExitCode = 0;
				HANDLE handle = GetCurrentThread();
				//DWORD currentThreadId = GetCurrentThreadId();
				GetExitCodeThread(handle,&ExitCode);
				AfxEndThread(ExitCode,TRUE);
			}
		}
		
		if (((pParInfo->pi.PartitionLength.QuadPart/512 - 1) % 256) != 0)
		{
			leftSectors = (pParInfo->pi.PartitionLength.QuadPart/512 - 1) % 256;
			ReadSector(tempStartLogicalSector,leftSectors,
				pBuf,pParInfo->nHardDiskNum,pDriveParam);
			
			switch(index)
			{
			case 0: YGAESEncryptData((UCHAR *)dlg->m_encryptPassword.GetBuffer(0),dlg->m_encryptPassword.GetLength(),pBuf,pBuf,leftSectors * 512);break;
			case 1: YGDESEncryCode(DESKey,pBuf,pBuf,leftSectors * 512,YGDESCRYPTO);break;
			case 2: YGDESEncryCode(DESKey,pBuf,pBuf,leftSectors * 512,YGDESEDE2);break;
			case 3: YGDESEncryCode(DESKey,pBuf,pBuf,leftSectors * 512,YGDESEDE3);break;
			case 4: YGDESEncryCode(DESKey,pBuf,pBuf,leftSectors * 512,YGDESXEX3);break;
			}
			
			WriteSector(tempStartLogicalSector,leftSectors,
				pBuf,pParInfo->nHardDiskNum,pDriveParam);
		}	
		
		//::WritePrivateProfileString("EncryptInfo","Password",strName,szPath);
		
		//	::SendMessage(HWND_BROADCAST,WM_MY_MESSAGE,NULL,NULL);
		hVolume = CreateFile(tempStr,GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_WRITE | FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		DeviceIoControl(hVolume,FSCTL_LOCK_VOLUME,NULL,0,NULL,0,&dwResult,NULL);
		DeviceIoControl(hVolume,FSCTL_DISMOUNT_VOLUME,NULL,0,NULL,0,&dwResult,NULL);
		DeviceIoControl(hVolume,FSCTL_UNLOCK_VOLUME,NULL,0,NULL,0,&dwResult,NULL);
		CloseHandle(hVolume);
		
		//	HWND handle = ::FindWindowEx(NULL,NULL,NULL,"AntiType");
		// 	::SendMessage(handle,WM_MY_MESSAGE,NULL,NULL);
		//	SendMessage((dlg->GetParent()->m_hWnd),WM_MY_MESSAGE,NULL,NULL);
		
		//	HWND handle = ::FindWindow(NULL,"AntiType");
		//	::SendMessage(handle,WM_MY_MESSAGE,NULL,NULL);
		//	::SendMessage(HWND_BROADCAST,WM_MY_MESSAGE,NULL,NULL);
		//	CAntiTypeDlg *dlg1 = new CAntiTypeDlg();
		//	::SendMessage(dlg1->m_hWnd,WM_MY_MESSAGE,NULL,NULL);
		//	delete(dlg);
	}

	free(pParInfo);
	free(pDriveParam);
	free(pBuf);	

	AfxMessageBox(IDS_ENCRYPT_COMPLETE,MB_ICONINFORMATION);
	SendMessage(g_handle,WM_MY_MESSAGE,NULL,NULL);

	(dlg->GetDlgItem(IDC_START_ENCRYPT))->EnableWindow(TRUE);
	(dlg->GetDlgItem(IDCANCEL))->EnableWindow(TRUE);
	(dlg->GetDlgItem(IDC_ABORT_ENCRYPT))->EnableWindow(FALSE);

	return 0;
}

CEncryptDialog::CEncryptDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CEncryptDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEncryptDialog)
	m_encryptPassword = _T("");
	m_confirmPassword = _T("");
	//}}AFX_DATA_INIT
}


void CEncryptDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEncryptDialog)
	DDX_Control(pDX, IDC_SELECT_ENCRYPT_ALGORITHM, m_encryptType);
	DDX_Control(pDX, IDC_ENCRYPT_PROGRESS, m_encryptProgress);
	DDX_Text(pDX, IDC_ENCRYPT_PASSWORD, m_encryptPassword);
	DDX_Text(pDX, IDC_CONFIRM_PASSWORD, m_confirmPassword);
	//}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(CEncryptDialog, CDialog)
	//{{AFX_MSG_MAP(CEncryptDialog)
	ON_BN_CLICKED(IDC_START_ENCRYPT, OnStartEncrypt)
	ON_BN_CLICKED(IDC_ABORT_ENCRYPT, OnAbortEncrypt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEncryptDialog message handlers


void CEncryptDialog::OnCancel() 
{
	// TODO: Add extra cleanup here
    
	CDialog::OnCancel();
}


void CEncryptDialog::OnStartEncrypt() 
{
	// TODO: Add your control notification handler code here
    
	CButton *pb;
	UpdateData(TRUE);
	//m_encryptPassword.Mid(0,24);
	//m_confirmPassword.Mid(0,24);

	if(m_encryptPassword.GetLength() == 0 ||
		m_confirmPassword.GetLength() == 0)
	{
		AfxMessageBox(IDS_ANY_PASSWORD_CAN_NOT_BE_NULL,MB_ICONSTOP);
	}
	else
	{
		if(m_encryptPassword.GetLength() == 1 ||
			m_confirmPassword.GetLength() == 1)
		{
			AfxMessageBox(IDS_PASSWORD_TOO_SHORT,MB_ICONSTOP);
			m_encryptPassword = "";
			m_confirmPassword = "";
			UpdateData(FALSE);
		}
		else
		{
			if(m_encryptPassword.CompareNoCase(m_confirmPassword) != 0)
			{
				AfxMessageBox(IDS_TWO_PASSWORDS_ARE_NOT_SUITED,MB_ICONSTOP);
			}
			else
			{
				pb = (CButton *)GetDlgItem(IDC_ABORT_ENCRYPT);
				pb->EnableWindow(TRUE);
				pb = (CButton *)GetDlgItem(IDC_START_ENCRYPT);
				pb->EnableWindow(FALSE);
				pb = (CButton *)GetDlgItem(IDCANCEL);
				pb->EnableWindow(FALSE);

				AfxBeginThread((AFX_THREADPROC)DoEncrypt,this);
			}
		}
	}
}

BOOL CEncryptDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
//	if (g_handle == INVALID_HANDLE_VALUE)
//	{
//		MessageBox("error",NULL,MB_OK);
//	}
//	m_handle = g_handle;
	CButton *pb;
	
	// TODO: Add extra initialization here
	
	//CButton *button = (CButton *)GetDlgItem(IDC_START_ENCRYPT);
    //button->EnableWindow(FALSE);

	pb = (CButton *)GetDlgItem(IDC_ABORT_ENCRYPT);
	pb->EnableWindow(FALSE);

	//((CComboBox *)GetDlgItem(IDC_SELECT_ENCRYPT_ALGORITHM))->SetCurSel(0);
	m_encryptType.SetCurSel(0);
	m_encryptProgress.SetPos(0);
	m_encryptProgress.SetRange(0,100);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEncryptDialog::OnAbortEncrypt() 
{
	// TODO: Add your control notification handler code here
	
	if(AfxMessageBox(IDS_ABORT_ENCRYPT,MB_YESNO | MB_ICONQUESTION,0) == IDYES)
	{
		g_bAbort = TRUE;
		CButton *pb;
		pb = (CButton *)GetDlgItem(IDC_ABORT_ENCRYPT);
		pb->EnableWindow(FALSE);
	}
}

void CEncryptDialog::UndoEncrypt(PBYTE pKey, int keyLength, PBYTE pBuf, int encryptType, DWORD alreadyEncryptSectors)
{
	UCHAR	DESKey[32];
	CString tempStr;
	int nPercent;
	DWORD tempStartLogicalSector;
	BOOL bResult;
	
	//PBYTE tempPointer;

	PBIOS_DRIVE_PARAM pDriveParam;
    PPARTITION_INFO_2000 pParInfo;

  	NormalKeyToDESKey((char *)pKey,DESKey);

	tempStr = driverName.Mid(0,1);
	char *directory = (LPSTR)(LPCTSTR)tempStr;

	pDriveParam = (PBIOS_DRIVE_PARAM)malloc(sizeof(BIOS_DRIVE_PARAM));
	pParInfo = (PPARTITION_INFO_2000)malloc(sizeof(PARTITION_INFO_2000));

	bResult = PartitionInfoByDriveLetter(*directory,pParInfo);

	if (bResult)
	{
		GetDriveParam(pParInfo->nHardDiskNum,pDriveParam);
		tempStartLogicalSector = pParInfo->pi.StartingOffset.QuadPart/pDriveParam->SectorsSize;
		ReadSector(tempStartLogicalSector,1,pBuf,pParInfo->nHardDiskNum,pDriveParam);
		YGDecryptBootSector(pBuf,pKey,keyLength);
		((PYGCRYDISK_BOOTSECTOR)pBuf)->uEncrytTypeAndKeyLen = 85;
		((PYGCRYDISK_BOOTSECTOR)pBuf)->uCheckSumAndEncryptMark = 170;
		WriteSector(tempStartLogicalSector,1,pBuf,pParInfo->nHardDiskNum,pDriveParam);
		
		tempStartLogicalSector++;
		int nTotalNum = alreadyEncryptSectors/256;
		DWORD dwEecryptSectors = alreadyEncryptSectors;
		DWORD dwTotalSector = (pParInfo->pi.PartitionLength.QuadPart/512) - 1 - (pParInfo->pi.PartitionLength.QuadPart/512 - 1) % 256;
		
		for(int i=1; i <= nTotalNum; i++)
		{
			ReadSector(tempStartLogicalSector,256,pBuf,
				pParInfo->nHardDiskNum,pDriveParam);
			
			switch(encryptType)
			{
			case 1: YGAESDecryptData(pKey,keyLength,pBuf,pBuf,256 * 512);break;
			case 2: YGDESDecryCode(DESKey,pBuf,pBuf,256 * 512,YGDESCRYPTO);break;
			case 3: YGDESDecryCode(DESKey,pBuf,pBuf,256 * 512,YGDESEDE2);break;
			case 4: YGDESDecryCode(DESKey,pBuf,pBuf,256 * 512,YGDESEDE3);break;
			case 5: YGDESDecryCode(DESKey,pBuf,pBuf,256 * 512,YGDESXEX3);break;
			}
			
			WriteSector(tempStartLogicalSector,256,pBuf,
				pParInfo->nHardDiskNum,pDriveParam);
			dwEecryptSectors -= 256;
			nPercent = (int)(dwEecryptSectors * 100/dwTotalSector);
			m_encryptProgress.SetPos(nPercent);
			tempStartLogicalSector += 256;
		}
		
		AfxMessageBox(IDS_ABORT_COMPLETE,MB_OK);
	}

	free(pDriveParam);
	free(pParInfo);

}
