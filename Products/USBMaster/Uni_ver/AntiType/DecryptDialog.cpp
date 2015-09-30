// DecryptDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AntiType.h"
#include "AntiTypeDlg.h"
#include "DecryptDialog.h"
#include "EncryptDialog.h"
#include <stdlib.h>
#include <string.h>
#include <shlwapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDecryptDialog dialog

UINT DoDecrypt(LPVOID pParam)
{
	UCHAR	DESKey[32];
	int index;
	int nPercent;
	CString tempStr;
	BOOL bResult;
	long leftSectors;
	DWORD tempStartLogicalSector;
	DWORD dwEncryptedSectorNum = 0;

	PBIOS_DRIVE_PARAM pDriveParam;
    PPARTITION_INFO_2000 pParInfo;

	WORD wSectors = 256;
	PBYTE pBuf;

	DismountDrive();
	CDecryptDialog *dlg = (CDecryptDialog *)pParam;
	tempStr = driverName.Mid(0,1);
	char *directory = (LPSTR)(LPCTSTR)tempStr;
  	NormalKeyToDESKey(dlg->m_decryptPassword.GetBuffer(0),DESKey);

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
		
		(dlg->GetDlgItem(IDC_START_DECRYPT))->EnableWindow(FALSE);
		(dlg->GetDlgItem(IDCANCEL))->EnableWindow(FALSE);
		(dlg->GetDlgItem(IDC_ABORT_DECRYPT))->EnableWindow(TRUE);
		
		ReadSector(tempStartLogicalSector,1,pBuf,pParInfo->nHardDiskNum,pDriveParam);
		index = (((PYGCRYDISK_BOOTSECTOR)pBuf)->uEncrytTypeAndKeyLen) >> 5;
		YGDecryptBootSector(pBuf,(UCHAR *)dlg->m_decryptPassword.GetBuffer(0),dlg->m_decryptPassword.GetLength());
		((PYGCRYDISK_BOOTSECTOR)pBuf)->uEncrytTypeAndKeyLen = 85;
		((PYGCRYDISK_BOOTSECTOR)pBuf)->uCheckSumAndEncryptMark = 170;
		WriteSector(tempStartLogicalSector,1,pBuf,
				        pParInfo->nHardDiskNum,pDriveParam);
		
		tempStartLogicalSector++;
		
		for(int i=1; i <= nTotalNum; i++)
		{
			ReadSector(tempStartLogicalSector,wSectors,pBuf,
				            pParInfo->nHardDiskNum,pDriveParam);
			
			switch(index)
			{
			case 1: YGAESDecryptData((UCHAR *)dlg->m_decryptPassword.GetBuffer(0),dlg->m_decryptPassword.GetLength(),pBuf,pBuf,256 * 512);break;
			case 2: YGDESDecryCode(DESKey,pBuf,pBuf,256 * 512,YGDESCRYPTO);break;
			case 3: YGDESDecryCode(DESKey,pBuf,pBuf,256 * 512,YGDESEDE2);break;
			case 4: YGDESDecryCode(DESKey,pBuf,pBuf,256 * 512,YGDESEDE3);break;
			case 5: YGDESDecryCode(DESKey,pBuf,pBuf,256 * 512,YGDESXEX3);break;
			}
			
			WriteSector(tempStartLogicalSector,wSectors,pBuf,
				pParInfo->nHardDiskNum,pDriveParam);
			dwEncryptedSectorNum += 256;
			nPercent = (int)(dwEncryptedSectorNum * 100/dwTotalSectors);
			dlg->m_decryptProgress.SetPos(nPercent);
			tempStartLogicalSector += 256;
			
			if(g_bAbort == TRUE)
			{
				dlg->UndoDecrypt((UCHAR *)dlg->m_decryptPassword.GetBuffer(0),dlg->m_decryptPassword.GetLength(),pBuf,index - 1,dwEncryptedSectorNum);
				
				free(pBuf);
				free(pDriveParam);
				free(pParInfo);
				
				(dlg->GetDlgItem(IDC_START_DECRYPT))->EnableWindow(TRUE);
				(dlg->GetDlgItem(IDCANCEL))->EnableWindow(TRUE);
				(dlg->GetDlgItem(IDC_ABORT_DECRYPT))->EnableWindow(FALSE);
				
				g_bAbort = FALSE;
				DWORD ExitCode = 0;
				HANDLE handle = GetCurrentThread();
				DWORD currentThreadId = GetCurrentThreadId();
				GetExitCodeThread(handle,&ExitCode);
				AfxEndThread(ExitCode,TRUE);
			}
		}
		
		if(((pParInfo->pi.PartitionLength.QuadPart/512 - 1) % 256) != 0)
		{
            leftSectors = (pParInfo->pi.PartitionLength.QuadPart/512 - 1) % 256;
			ReadSector(tempStartLogicalSector,leftSectors,pBuf,
				            pParInfo->nHardDiskNum,pDriveParam);
			switch(index)
			{
			case 1: YGAESDecryptData((UCHAR *)dlg->m_decryptPassword.GetBuffer(0),dlg->m_decryptPassword.GetLength(),pBuf,pBuf,leftSectors * 512);break;
			case 2: YGDESDecryCode(DESKey,pBuf,pBuf,leftSectors * 512,YGDESCRYPTO);break;
			case 3: YGDESDecryCode(DESKey,pBuf,pBuf,leftSectors * 512,YGDESEDE2);break;
			case 4: YGDESDecryCode(DESKey,pBuf,pBuf,leftSectors * 512,YGDESEDE3);break;
			case 5: YGDESDecryCode(DESKey,pBuf,pBuf,leftSectors * 512,YGDESXEX3);break;
			}
			
			WriteSector(tempStartLogicalSector,leftSectors,pBuf,
				pParInfo->nHardDiskNum,pDriveParam);
		}
		
		char szPath[MAX_PATH];
		char fileName[] = "\\encrypt.dat";
		char encryptInfoDirectory[] = "\\USBMaster";
		DWORD len = MAX_PATH;
		
		SHGetSpecialFolderPath(0,szPath,CSIDL_APPDATA,0);
		strcat(szPath,encryptInfoDirectory);
		//CreateDirectory(szPath,NULL);
		strcat(szPath,fileName);
		
		if(PathFileExists(szPath))
		{
			DeleteFile(szPath);
		}
		
		AfxMessageBox(IDS_DECRYPT_COMPLETE,MB_ICONINFORMATION);
		
		HWND handle = ::FindWindow(NULL,"USBMaster");
		SendMessage(handle,WM_MY_MESSAGE,NULL,NULL);
		CloseHandle(handle);
		
		(dlg->GetDlgItem(IDC_START_DECRYPT))->EnableWindow(TRUE);
		(dlg->GetDlgItem(IDCANCEL))->EnableWindow(TRUE);
		(dlg->GetDlgItem(IDC_ABORT_DECRYPT))->EnableWindow(FALSE);

	}

	free(pBuf);
	free(pDriveParam);
	free(pParInfo);

	return 0;
}

CDecryptDialog::CDecryptDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CDecryptDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDecryptDialog)
	m_decryptPassword = _T("");
	//}}AFX_DATA_INIT
}


void CDecryptDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDecryptDialog)
	DDX_Control(pDX, IDC_DECRYPT_PROGRESS, m_decryptProgress);
	DDX_Text(pDX, IDC_DECRYPT_PASSWORD, m_decryptPassword);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDecryptDialog, CDialog)
	//{{AFX_MSG_MAP(CDecryptDialog)
	ON_BN_CLICKED(IDC_START_DECRYPT, OnStartDecrypt)
	ON_BN_CLICKED(IDC_ABORT_DECRYPT, OnAbortDecrypt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDecryptDialog message handlers

void CDecryptDialog::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}


void CDecryptDialog::OnStartDecrypt() 
{
	// TODO: Add your control notification handler code here
    
	UpdateData(TRUE);
	//m_decryptPassword.Mid(0,24);

	int encryptType;
	BYTE checkMark;
	BYTE bit;
	CString tempStr;
//	PBYTE tempPointer;
	PBYTE pBuf;
	DWORD tempStartLogicalSector;
	BOOL bResult;

	PBIOS_DRIVE_PARAM pDriveParam;
    PPARTITION_INFO_2000 pParInfo;

	tempStr = driverName.Mid(0,1);
	char *directory = (LPSTR)(LPCTSTR)tempStr;
    unsigned char *pKey = (unsigned char *)(m_decryptPassword.GetBuffer(0));
	m_decryptPassword.ReleaseBuffer();

	pBuf = (PBYTE)malloc(512);
	pDriveParam = (PBIOS_DRIVE_PARAM)malloc(sizeof(BIOS_DRIVE_PARAM));
	pParInfo = (PPARTITION_INFO_2000)malloc(sizeof(PARTITION_INFO_2000));

	bResult = PartitionInfoByDriveLetter(*directory,pParInfo);
	
	if (bResult)
	{
		GetDriveParam(pParInfo->nHardDiskNum,pDriveParam);
		tempStartLogicalSector = pParInfo->pi.StartingOffset.QuadPart/pDriveParam->SectorsSize;
		ReadSector(tempStartLogicalSector,1,pBuf,
			pParInfo->nHardDiskNum,pDriveParam);
		
		BYTE checkSum = (BYTE)YGNSMCheckSum(pKey,m_decryptPassword.GetLength());
		checkSum = checkSum >> 1;
		bit = 224;
		encryptType = ((((PYGCRYDISK_BOOTSECTOR)pBuf)->uEncrytTypeAndKeyLen) & bit) >> 5;
		bit = 127;
		BYTE ch = (((PYGCRYDISK_BOOTSECTOR)pBuf)->uCheckSumAndEncryptMark) & bit;
		
		YGDecryptBootSector(pBuf,pKey,m_decryptPassword.GetLength());
		
		checkMark = ((PYGCRYDISK_BOOTSECTOR)pBuf)->uMark;
		
		if(m_decryptPassword.GetLength() == 0)
		{
			AfxMessageBox(IDS_PASSWORD_CAN_NOT_BE_NULL,MB_ICONSTOP);
		}
		else
		{
			if((ch != checkSum) || (checkMark != 'Y'))
			{
				AfxMessageBox(IDS_PASSWORD_IS_INCORRECT,MB_ICONSTOP);
			}
			else
			{
				AfxBeginThread(DoDecrypt,this);
			}
		}
	}

	free(pBuf);
	free(pDriveParam);
	free(pParInfo);
}

BOOL CDecryptDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	m_decryptProgress.SetPos(0);
	m_decryptProgress.SetRange(0,100);

	CButton *pb;
	pb = (CButton *)GetDlgItem(IDC_ABORT_DECRYPT);
	pb->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDecryptDialog::OnAbortDecrypt() 
{
	// TODO: Add your control notification handler code here

	if(AfxMessageBox(IDS_ABORT_DECRYPT,MB_YESNO|MB_ICONQUESTION,0) == IDYES)
	{
		g_bAbort = TRUE;
		CButton *pb;
		pb = (CButton *)GetDlgItem(IDC_ABORT_DECRYPT);
		pb->EnableWindow(FALSE);
	}
	
}

void CDecryptDialog::UndoDecrypt(PBYTE pKey, int keyLength, PBYTE pBuf, int decryptType, DWORD alreadyDecryptSectors)
{
	UCHAR	DESKey[32];
	CString tempStr;
	DWORD tempStartLogicalSector;
	BYTE bit;
	BYTE keyLen;
	int nPercent;
	int nTotalNum;
	BOOL bResult;

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
		((PYGCRYDISK_BOOTSECTOR)pBuf)->uMark = 'Y';
		YGEncryptBootSector(pBuf,pKey,keyLength);
		BYTE checkSum = (BYTE)YGNSMCheckSum(pKey,keyLength);
		checkSum = checkSum >> 1;
		bit = 128;
		((PYGCRYDISK_BOOTSECTOR)pBuf)->uCheckSumAndEncryptMark = checkSum | bit;
		keyLen = 255;
		keyLen  = (keyLen >> 3) & keyLength;
		bit = 224;
		bit = ((bit >> 5) & decryptType) << 5;
		((PYGCRYDISK_BOOTSECTOR)pBuf)->uEncrytTypeAndKeyLen = bit | keyLen;
		WriteSector(tempStartLogicalSector,1,pBuf,pParInfo->nHardDiskNum,pDriveParam);
		tempStartLogicalSector++;
		nTotalNum = alreadyDecryptSectors/256;
		DWORD dwDecryptSector = alreadyDecryptSectors;
		DWORD dwTotalSector = pParInfo->pi.PartitionLength.QuadPart/512 - 1 - (pParInfo->pi.PartitionLength.QuadPart/512 - 1) % 256;
		
		for(int i=1; i <= nTotalNum ; i++)
		{
			ReadSector(tempStartLogicalSector,256,pBuf,
				pParInfo->nHardDiskNum,pDriveParam);
			
			switch(decryptType)
			{
			case 0: YGAESEncryptData(pKey,keyLength,pBuf,pBuf,256 * 512);break;
			case 1: YGDESEncryCode(DESKey,pBuf,pBuf,256 * 512,YGDESCRYPTO);break;
			case 2: YGDESEncryCode(DESKey,pBuf,pBuf,256 * 512,YGDESEDE2);break;
			case 3: YGDESEncryCode(DESKey,pBuf,pBuf,256 * 512,YGDESEDE3);break;
			case 4: YGDESEncryCode(DESKey,pBuf,pBuf,256 * 512,YGDESXEX3);break;
			}
			
			WriteSector(tempStartLogicalSector,256,pBuf,
				pParInfo->nHardDiskNum,pDriveParam);
			dwDecryptSector -= 256;
			nPercent = (int)(dwDecryptSector * 100/dwTotalSector);
			m_decryptProgress.SetPos(nPercent);
			tempStartLogicalSector += 256;
		}
		
//		BYTE checkSum = (BYTE)YGNSMCheckSum(pKey,keyLength);
//		ReadSector(pParInfo->pi.StartingOffset.QuadPart,1,pBuf,
//			pParInfo->nHardDiskNum,pDriveParam);
//		
//		checkSum = checkSum >> 1;
//		bit = 128;
//		((PYGCRYDISK_BOOTSECTOR)pBuf)->uCheckSumAndEncryptMark = checkSum | bit;
//		keyLen = 255;
//		keyLen  = (keyLen >> 3) & keyLength;
//		bit = 224;
//		bit = ((bit >> 5) & decryptType) << 5;
//		((PYGCRYDISK_BOOTSECTOR)pBuf)->uEncrytTypeAndKeyLen = bit | keyLen;
//		
//		WriteSector(pParInfo->pi.StartingOffset.QuadPart,1,pBuf,
//				        pParInfo->nHardDiskNum,pDriveParam);
		
		AfxMessageBox(IDS_ABORT_COMPLETE,MB_OK);
	}

	free(pDriveParam);
	free(pParInfo);
}
