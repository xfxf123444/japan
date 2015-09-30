// ChangePasswordDialog.cpp : implementation file
//

#include "stdafx.h"
#include "antitype.h"
#include "ChangePasswordDialog.h"
#include "AvailableDriverDialog.h"
#include "EncryptDialog.h"
#include "AntiTypeDlg.h"
#include <windows.h>
#include <shlwapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChangePasswordDialog dialog

CChangePasswordDialog::CChangePasswordDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CChangePasswordDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChangePasswordDialog)
	m_originalPassword = _T("");
	m_newPassword = _T("");
	m_confirmChangedPassword = _T("");
	//}}AFX_DATA_INIT
}


void CChangePasswordDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChangePasswordDialog)
	DDX_Control(pDX, IDC_CHANGE_ENCRYPT_ALGORITHM, m_changeEncryptAlgorithm);
	DDX_Control(pDX, IDC_PROGRESS_CHANGE_PASSWORD, m_changePasswordProgress);
	DDX_Text(pDX, IDC_ORIGINAL_PASSWORD, m_originalPassword);
	DDX_Text(pDX, IDC_NEW_PASSWORD, m_newPassword);
	DDX_Text(pDX, IDC_CONFIRM_CHANGED_PASSWORD, m_confirmChangedPassword);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChangePasswordDialog, CDialog)
	//{{AFX_MSG_MAP(CChangePasswordDialog)
	ON_BN_CLICKED(IDC_BUTTON_START_CHANGE, OnButtonStartChange)
	ON_BN_CLICKED(IDC_BUTTON_ABORT_CHANGE_PASSWORD, OnButtonAbortChangePassword)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangePasswordDialog message handlers

UINT ChangePassword(LPVOID pParam)
{
	UCHAR	DESKeyNew[32],DESKeyOld[32];
	int newEncryptType;
	int originalEncryptType;
	int nTotalNum;
	int nPercent;
	BYTE checkSum;
	BYTE uNewEncyptType;
	BYTE uKeyLen;
	BYTE bit;
	CString tempStr;
	long leftSectors;
	DWORD dwStartSector;
	DWORD dwModifiedSectorNum = 0;
	DWORD dwTempStartLogicalSector;
	BOOL bResult;

	PBIOS_DRIVE_PARAM pDriveParam;
    PPARTITION_INFO_2000 pParInfo;

	WORD wSectors = 256;
	PBYTE pBuf;

	DismountDrive();
	CChangePasswordDialog *dlg = (CChangePasswordDialog *)pParam;
	tempStr = driverName.Mid(0,1);
	char *directory = (LPSTR)(LPCTSTR)tempStr;
	NormalKeyToDESKey(dlg->m_newPassword.GetBuffer(0),DESKeyNew);
	NormalKeyToDESKey(dlg->m_originalPassword.GetBuffer(0),DESKeyOld);

	pBuf = (PBYTE)malloc(256 * 512);
	pDriveParam = (PBIOS_DRIVE_PARAM)malloc(sizeof(BIOS_DRIVE_PARAM));
	pParInfo = (PPARTITION_INFO_2000)malloc(sizeof(PARTITION_INFO_2000));

	bResult = PartitionInfoByDriveLetter(*directory,pParInfo);

	if (bResult)
	{
		GetDriveParam(pParInfo->nHardDiskNum,pDriveParam);
		dwStartSector = pParInfo->pi.StartingOffset.QuadPart/pDriveParam->SectorsSize;
		dwTempStartLogicalSector = dwStartSector;
		ReadSector(dwStartSector,1,pBuf,pParInfo->nHardDiskNum,pDriveParam);
		originalEncryptType = (((PYGCRYDISK_BOOTSECTOR)pBuf)->uEncrytTypeAndKeyLen) >> 5;
		newEncryptType = dlg->m_changeEncryptAlgorithm.GetCurSel() + 1;
		checkSum = (BYTE)YGNSMCheckSum((UCHAR *)dlg->m_newPassword.GetBuffer(0),dlg->m_newPassword.GetLength());
		
		uNewEncyptType = 224;
		uNewEncyptType = ((uNewEncyptType >> 5) & newEncryptType) << 5;
		uKeyLen = 31;
		uKeyLen = uKeyLen & (dlg->m_newPassword.GetLength());
		YGDecryptBootSector(pBuf,(UCHAR *)dlg->m_originalPassword.GetBuffer(0),dlg->m_originalPassword.GetLength());
		YGEncryptBootSector(pBuf,(UCHAR *)dlg->m_newPassword.GetBuffer(0),dlg->m_newPassword.GetLength());
		((PYGCRYDISK_BOOTSECTOR)pBuf)->uEncrytTypeAndKeyLen = uKeyLen | uNewEncyptType;
		bit = 128;
		((PYGCRYDISK_BOOTSECTOR)pBuf)->uCheckSumAndEncryptMark = (checkSum >> 1) | bit;
		
		WriteSector(dwStartSector,1,pBuf,pParInfo->nHardDiskNum,pDriveParam);

		nTotalNum = (int)((pParInfo->pi.PartitionLength.QuadPart/512 - 1)/256);
		DWORD dwTotalSectors = pParInfo->pi.PartitionLength.QuadPart/512 - 1 - (pParInfo->pi.PartitionLength.QuadPart/512 - 1) % 256;
		
		dwTempStartLogicalSector++;

		for (int i = 1;i <= nTotalNum;i++)
		{
			ReadSector(dwTempStartLogicalSector,wSectors,pBuf,
				pParInfo->nHardDiskNum,pDriveParam);
			
			switch(originalEncryptType)
			{
			case 1: YGAESDecryptData((UCHAR *)dlg->m_originalPassword.GetBuffer(0),dlg->m_originalPassword.GetLength(),pBuf,pBuf,256 * 512);break;
			case 2: YGDESDecryCode(DESKeyOld,pBuf,pBuf,256 * 512,YGDESCRYPTO);break;
			case 3: YGDESDecryCode(DESKeyOld,pBuf,pBuf,256 * 512,YGDESEDE2);break;
			case 4: YGDESDecryCode(DESKeyOld,pBuf,pBuf,256 * 512,YGDESEDE3);break;
			case 5: YGDESDecryCode(DESKeyOld,pBuf,pBuf,256 * 512,YGDESXEX3);break;
			}
			
			switch(newEncryptType)
			{
			case 1: YGAESEncryptData((UCHAR *)dlg->m_newPassword.GetBuffer(0),dlg->m_newPassword.GetLength(),pBuf,pBuf,256 * 512);break;
			case 2: YGDESEncryCode(DESKeyNew,pBuf,pBuf,256 * 512,YGDESCRYPTO);break;
			case 3: YGDESEncryCode(DESKeyNew,pBuf,pBuf,256 * 512,YGDESEDE2);break;
			case 4: YGDESEncryCode(DESKeyNew,pBuf,pBuf,256 * 512,YGDESEDE3);break;
			case 5: YGDESEncryCode(DESKeyNew,pBuf,pBuf,256 * 512,YGDESXEX3);break;
			}
			
			WriteSector(dwTempStartLogicalSector,wSectors,pBuf,
				pParInfo->nHardDiskNum,pDriveParam);
			dwModifiedSectorNum += 256;
			nPercent = (int)(dwModifiedSectorNum * 100/dwTotalSectors);
			dlg->m_changePasswordProgress.SetPos(nPercent);
			dwTempStartLogicalSector += 256;
			
			if (g_bAbort == TRUE)
			{
				dlg->UndoChangePassword((UCHAR *)dlg->m_originalPassword.GetBuffer(0),(UCHAR *)dlg->m_newPassword.GetBuffer(0),dlg->m_originalPassword.GetLength(),
					dlg->m_newPassword.GetLength(),pBuf,originalEncryptType,newEncryptType,dwModifiedSectorNum);
				
				free(pBuf);
				free(pDriveParam);
				free(pParInfo);
				
				(dlg->GetDlgItem(IDC_BUTTON_START_CHANGE))->EnableWindow(TRUE);
				(dlg->GetDlgItem(IDCANCEL))->EnableWindow(TRUE);
				(dlg->GetDlgItem(IDC_ABORT_ENCRYPT))->EnableWindow(FALSE);
				
				g_bAbort = FALSE;
				DWORD ExitCode = 0;
				HANDLE handle = GetCurrentThread();
				GetExitCodeThread(handle,&ExitCode);
				AfxEndThread(ExitCode,TRUE);
			}
		}
		
		if ((pParInfo->pi.PartitionLength.QuadPart/512 - 1) % 256 != 0)
		{
			leftSectors = (pParInfo->pi.PartitionLength.QuadPart/512 - 1) % 256;
			ReadSector(dwTempStartLogicalSector,(USHORT)leftSectors,pBuf,
				pParInfo->nHardDiskNum,pDriveParam);
			
			switch(originalEncryptType)
			{
			case 1: YGAESDecryptData((UCHAR *)dlg->m_originalPassword.GetBuffer(0),dlg->m_originalPassword.GetLength(),pBuf,pBuf,leftSectors * 512);break;
			case 2: YGDESDecryCode(DESKeyOld,pBuf,pBuf,leftSectors * 512,YGDESCRYPTO);break;
			case 3: YGDESDecryCode(DESKeyOld,pBuf,pBuf,leftSectors * 512,YGDESEDE2);break;
			case 4: YGDESDecryCode(DESKeyOld,pBuf,pBuf,leftSectors * 512,YGDESEDE3);break;
			case 5: YGDESDecryCode(DESKeyOld,pBuf,pBuf,leftSectors * 512,YGDESXEX3);break;
			}
			
			switch(newEncryptType)
			{
			case 1: YGAESEncryptData((UCHAR *)dlg->m_newPassword.GetBuffer(0),dlg->m_newPassword.GetLength(),pBuf,pBuf,leftSectors * 512);break;
			case 2: YGDESEncryCode(DESKeyNew,pBuf,pBuf,leftSectors * 512,YGDESCRYPTO);break;
			case 3: YGDESEncryCode(DESKeyNew,pBuf,pBuf,leftSectors * 512,YGDESEDE2);break;
			case 4: YGDESEncryCode(DESKeyNew,pBuf,pBuf,leftSectors * 512,YGDESEDE3);break;
			case 5: YGDESEncryCode(DESKeyNew,pBuf,pBuf,leftSectors * 512,YGDESXEX3);break;
			}
			
			WriteSector(dwTempStartLogicalSector,(USHORT)leftSectors,pBuf,
				pParInfo->nHardDiskNum,pDriveParam);
		}
		
		char szPath[MAX_PATH];
		char fileName[] = "\\encrypt.dat";
		char encryptInfoDirectory[] = "\\USBMaster";
		DWORD len = MAX_PATH;
		char key[MAX_PATH];
		CString strTemp;
		
		SHGetSpecialFolderPath(0,szPath,CSIDL_APPDATA,0);
		strcat(szPath,encryptInfoDirectory);
		strcat(szPath,fileName);
		GetUserName(key,&len);

		strTemp = key;
		PBYTE tempKey = (PBYTE)(strTemp.GetBuffer(0));
		strTemp.ReleaseBuffer();
		
		if (PathFileExists(szPath))
		{
			FILE *fp;
			PBYTE buffer;
			buffer = (PBYTE)malloc(sizeof(SAVE_INFO));
			ZeroMemory(buffer,sizeof(SAVE_INFO));
			fp = fopen(szPath,"r+b");
			fread(buffer,sizeof(BYTE),sizeof(SAVE_INFO),fp);
			
			YGAESDecryptData(tempKey,strTemp.GetLength(),buffer,buffer,32);
			YGAESEncryptData(tempKey,strTemp.GetLength(),buffer,buffer,32);
			
			((PSAVE_INFO)buffer)->keyLen = dlg->m_newPassword.GetLength();
			fseek(fp,0,SEEK_SET);
			fwrite(buffer,sizeof(BYTE),sizeof(SAVE_INFO),fp);
			free(buffer);
			fclose(fp);	
		}

		free(pBuf);
		free(pDriveParam);
		free(pParInfo);
		
		HWND handle = ::FindWindow(NULL,"USBMaster");
		::SendMessage(handle,WM_MY_MESSAGE,NULL,NULL);
		
		(dlg->GetDlgItem(IDC_BUTTON_START_CHANGE))->EnableWindow(TRUE);
		(dlg->GetDlgItem(IDCANCEL))->EnableWindow(TRUE);
		(dlg->GetDlgItem(IDC_BUTTON_ABORT_CHANGE_PASSWORD))->EnableWindow(FALSE);
		
		AfxMessageBox(IDS_CHANGE_PASSWORD_COMPLETE,MB_ICONINFORMATION);
	}
	
	return 0;
}

BOOL CChangePasswordDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	CButton *pb;

	pb = (CButton *)GetDlgItem(IDC_BUTTON_ABORT_CHANGE_PASSWORD);
	pb->EnableWindow(FALSE);

	m_changeEncryptAlgorithm.SetCurSel(0);
	m_changePasswordProgress.SetPos(0);
	m_changePasswordProgress.SetRange(0,100);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChangePasswordDialog::OnButtonStartChange() 
{
	// TODO: Add your control notification handler code here

	CButton *pb;
	UpdateData(TRUE);

	int originalEncryptType;
	BYTE checkMark;
	BYTE checkSum;
	BYTE bit;
	CString tempStr;
	PBYTE pBuf;
	DWORD dwStartSector;
	BOOL bResult;
    
	PBIOS_DRIVE_PARAM pDriveParam;
    PPARTITION_INFO_2000 pParInfo;

	tempStr = driverName.Mid(0,1);
	char *directory = (LPSTR)(LPCTSTR)tempStr;
    unsigned char *pKey = (unsigned char *)(m_originalPassword.GetBuffer(0));
	m_originalPassword.ReleaseBuffer();

	pBuf = (PBYTE)malloc(512);
	pDriveParam = (PBIOS_DRIVE_PARAM)malloc(sizeof(BIOS_DRIVE_PARAM));
	pParInfo = (PPARTITION_INFO_2000)malloc(sizeof(PARTITION_INFO_2000));

	bResult = PartitionInfoByDriveLetter(*directory,pParInfo);

	if (bResult)
	{
		GetDriveParam(pParInfo->nHardDiskNum,pDriveParam);
		dwStartSector = pParInfo->pi.StartingOffset.QuadPart/pDriveParam->SectorsSize;
		ReadSector(dwStartSector,1,pBuf,pParInfo->nHardDiskNum,pDriveParam);
		
		originalEncryptType = (((PYGCRYDISK_BOOTSECTOR)pBuf)->uEncrytTypeAndKeyLen) >> 5;
		checkSum = (BYTE)YGNSMCheckSum(pKey,m_originalPassword.GetLength());
		bit = 127;
		bit = (((PYGCRYDISK_BOOTSECTOR)pBuf)->uCheckSumAndEncryptMark) & bit;
		
		YGDecryptBootSector(pBuf,pKey,m_originalPassword.GetLength());
		
		checkMark = ((PYGCRYDISK_BOOTSECTOR)pBuf)->uMark;
		
		if (m_originalPassword.GetLength() ==0 ||
			m_newPassword.GetLength() == 0 ||
			m_confirmChangedPassword.GetLength() == 0)
		{
			AfxMessageBox(IDS_ANY_PASSWORD_CAN_NOT_BE_NULL,MB_ICONWARNING);
		}
		else
		{
			if((bit != (checkSum >> 1)) || (checkMark != 'Y'))
			{
				AfxMessageBox(IDS_ORIGINAL_PASSWORD_IS_INCORRECT,MB_ICONSTOP);
			}
			else
			{
				if(m_newPassword.CompareNoCase(m_confirmChangedPassword) != 0)
				{
					AfxMessageBox(IDS_TWO_PASSWORDS_ARE_NOT_SUITED,MB_ICONSTOP);
				}
				else
				{
					pb = (CButton *)GetDlgItem(IDC_BUTTON_START_CHANGE);
					pb->EnableWindow(FALSE);
					pb = (CButton *)GetDlgItem(IDCANCEL);
					pb->EnableWindow(FALSE);
					pb = (CButton *)GetDlgItem(IDC_BUTTON_ABORT_CHANGE_PASSWORD);
					pb->EnableWindow(TRUE);
					
					AfxBeginThread((AFX_THREADPROC)ChangePassword,this);
				}
			}
		}
	}
	
	free(pBuf);
	free(pDriveParam);
	free(pParInfo);
}

void CChangePasswordDialog::UndoChangePassword(PBYTE pOriginalPassword, PBYTE pNewPassword, 
											   int originalPasswordLen, int newPasswordLen, 
											   PBYTE pBuf, int originalEncryptType, int newEncryptType,
											   DWORD alreadyEncryptSectors)
{
	UCHAR	DESKeyNew[32],DESKeyOld[32];
	CString tempStr;
	int nPercent;
	DWORD dwTempStartLogicalSector;
	DWORD dwModifiedSectorNum;
	BYTE bit;
	BYTE uOriginalKeyLen;
	BYTE uOriginalEncryptType;
	BOOL bResult;

	PBIOS_DRIVE_PARAM pDriveParam;
    PPARTITION_INFO_2000 pParInfo;

	tempStr = driverName.Mid(0,1);
	char *directory = (LPSTR)(LPCTSTR)tempStr;

  	NormalKeyToDESKey((char *)pOriginalPassword,DESKeyOld);
  	NormalKeyToDESKey((char *)pNewPassword,DESKeyNew);

	pDriveParam = (PBIOS_DRIVE_PARAM)malloc(sizeof(BIOS_DRIVE_PARAM));
	pParInfo = (PPARTITION_INFO_2000)malloc(sizeof(PARTITION_INFO_2000));

	bResult = PartitionInfoByDriveLetter(*directory,pParInfo);
	
	if (bResult)
	{
		GetDriveParam(pParInfo->nHardDiskNum,pDriveParam);
		dwTempStartLogicalSector = pParInfo->pi.StartingOffset.QuadPart/pDriveParam->SectorsSize;
		BYTE checkSum = (BYTE)YGNSMCheckSum(pOriginalPassword,m_originalPassword.GetLength());
		
		ReadSector(dwTempStartLogicalSector,1,pBuf,pParInfo->nHardDiskNum,pDriveParam);
		bit = 128;
		YGDecryptBootSector(pBuf,pOriginalPassword,originalPasswordLen);
		YGEncryptBootSector(pBuf,pNewPassword,newPasswordLen);
		
		((PYGCRYDISK_BOOTSECTOR)pBuf)->uCheckSumAndEncryptMark = bit | (checkSum >> 1);
		uOriginalKeyLen = 31;
		uOriginalKeyLen = uOriginalKeyLen & originalPasswordLen;
		uOriginalEncryptType = 224;
		uOriginalEncryptType = ((uOriginalEncryptType >> 5) & originalEncryptType) << 5;
		((PYGCRYDISK_BOOTSECTOR)pBuf)->uEncrytTypeAndKeyLen = uOriginalEncryptType | uOriginalKeyLen;
		
		WriteSector(dwTempStartLogicalSector,1,pBuf,
				        pParInfo->nHardDiskNum,pDriveParam);
		int nTotalNum = alreadyEncryptSectors/256;
		dwModifiedSectorNum = alreadyEncryptSectors;
		DWORD dwTotalSector = pParInfo->pi.PartitionLength.QuadPart/512 - 1 - (pParInfo->pi.PartitionLength.QuadPart/512 - 1) % 256;

		dwTempStartLogicalSector++;
		
		for(int i=1; i <= nTotalNum; i++)
		{
			ReadSector(dwTempStartLogicalSector,256,pBuf,
				pParInfo->nHardDiskNum,pDriveParam);
			
			switch(newEncryptType)
			{
			case 1: YGAESDecryptData(pNewPassword,newPasswordLen,pBuf,pBuf,256 * 512);break;
			case 2: YGDESDecryCode(DESKeyNew,pBuf,pBuf,256 * 512,YGDESCRYPTO);break;
			case 3: YGDESDecryCode(DESKeyNew,pBuf,pBuf,256 * 512,YGDESEDE2);break;
			case 4: YGDESDecryCode(DESKeyNew,pBuf,pBuf,256 * 512,YGDESEDE3);break;
			case 5: YGDESDecryCode(DESKeyNew,pBuf,pBuf,256 * 512,YGDESXEX3);break;
			}
			
			switch(originalEncryptType)
			{
			case 1: YGAESEncryptData(pOriginalPassword,originalPasswordLen,pBuf,pBuf,256 * 512);break;
			case 2: YGDESEncryCode(DESKeyOld,pBuf,pBuf,256 * 512,YGDESCRYPTO);break;
			case 3: YGDESEncryCode(DESKeyOld,pBuf,pBuf,256 * 512,YGDESEDE2);break;
			case 4: YGDESEncryCode(DESKeyOld,pBuf,pBuf,256 * 512,YGDESEDE3);break;
			case 5: YGDESEncryCode(DESKeyOld,pBuf,pBuf,256 * 512,YGDESXEX3);break;
			}
			
			WriteSector(dwTempStartLogicalSector,256,pBuf,
				pParInfo->nHardDiskNum,pDriveParam);
			dwModifiedSectorNum -= 256;
			nPercent = (int)(dwModifiedSectorNum * 100/dwTotalSector);
			m_changePasswordProgress.SetPos(nPercent);
			dwTempStartLogicalSector += 256;
		}
		
		AfxMessageBox(IDS_ABORT_COMPLETE,MB_OK);
	}

	free(pDriveParam);
	free(pParInfo);
}

void CChangePasswordDialog::OnButtonAbortChangePassword() 
{
	// TODO: Add your control notification handler code here

	if(AfxMessageBox(IDS_ABORT_CHANGE_PASSWORD,MB_YESNO | MB_ICONQUESTION,0) == IDYES)
	{
		g_bAbort = TRUE;
		CButton *pb = (CButton *)GetDlgItem(IDC_BUTTON_ABORT_CHANGE_PASSWORD);
		pb->EnableWindow(FALSE);
	}
	
}
