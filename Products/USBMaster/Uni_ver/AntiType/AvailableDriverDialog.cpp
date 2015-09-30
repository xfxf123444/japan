// AvailableDriverDialog.cpp : implementation file
//

#include "stdafx.h"
#include "antitype.h"
#include "AntiTypeDlg.h"
#include "AvailableDriverDialog.h"
#include "EncryptDialog.h"
#include <windows.h>
#include <shlwapi.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAvailableDriverDialog dialog

CAvailableDriverDialog::CAvailableDriverDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CAvailableDriverDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAvailableDriverDialog)
	m_mountCheckPassword = _T("");
	m_checkSetDefaultDriver = FALSE;
	//}}AFX_DATA_INIT
}


void CAvailableDriverDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAvailableDriverDialog)
	DDX_Control(pDX, IDC_AVAILABLE_DRIVER_LIST, m_availableDriverList);
	DDX_Text(pDX, IDC_EDIT1, m_mountCheckPassword);
	DDV_MaxChars(pDX, m_mountCheckPassword, 32);
	DDX_Check(pDX, IDC_CHECK_SET_DEFAULT_DRIVER, m_checkSetDefaultDriver);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAvailableDriverDialog, CDialog)
	//{{AFX_MSG_MAP(CAvailableDriverDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAvailableDriverDialog message handlers

extern ORIGINALDRIVER_MIRRORDRIVER usedDrivers[4];

void SetDefaultInfo(PBYTE pKey,int nKeyLen,PBYTE defaultDrive)
{
	char szPath[MAX_PATH];
	char fileName[] = "\\encrypt.dat";
	char encryptInfoDirectory[] = "\\USBMaster";
	DWORD len = MAX_PATH;
	char key[MAX_PATH];
	SAVE_INFO saveInfo;
	CString temp;

	GetUserName(key,&len);
	temp = key;
	PBYTE tempKey = (PBYTE)(temp.GetBuffer(0));
	temp.ReleaseBuffer();
 	_mbscpy(saveInfo.password,pKey);

    SHGetSpecialFolderPath(0,szPath,CSIDL_APPDATA,0);
	strcat(szPath,encryptInfoDirectory);
	CreateDirectory(szPath,NULL);
	strcat(szPath,fileName);
	
	if(PathFileExists(szPath))
	{
		DeleteFile(szPath);
	}

	FILE *fp;
	fp = fopen(szPath,"w+b");
	YGAESEncryptData(tempKey,temp.GetLength(),saveInfo.password,saveInfo.password,32);
	saveInfo.keyLen = nKeyLen;
	saveInfo.defaultName = *defaultDrive;
	fwrite(&saveInfo,sizeof(saveInfo),1,fp);
	fclose(fp);

//	FILE *fp;
//	PBYTE buffer;
//	buffer = (PBYTE)malloc(sizeof(SAVE_INFO));
//	ZeroMemory(buffer,sizeof(SAVE_INFO));
//	fp = fopen(szPath,"r+b");
//	fread(buffer,sizeof(BYTE),sizeof(SAVE_INFO),fp);
//	((PSAVE_INFO)buffer)->defaultName = *defaultDriver;
//	fseek(fp,0,SEEK_SET);
//	fwrite(buffer,sizeof(BYTE),sizeof(SAVE_INFO),fp);
//	free(buffer);
//	fclose(fp);
}

BOOL CAvailableDriverDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	CRect listRect;
	CImageList img;
	CString str;

	int nWidth;
	int index = 0;
	static int initDlgNumber = 1;
	char availableDriverLetter;
	char availableDriverName[3];
	LONG listStyle;

	listStyle = GetWindowLong(m_availableDriverList.m_hWnd,GWL_STYLE);
	listStyle |= LVS_SHOWSELALWAYS;
	SetWindowLong(m_availableDriverList.m_hWnd,GWL_STYLE,listStyle);
    
	if(initDlgNumber == 1)
	{
	    m_availableDriverList.SetImageList(&img,LVSIL_SMALL);
	    m_availableDriverList.GetClientRect(&listRect);

	    nWidth = listRect.right - listRect.left;
	
	    str.LoadString(IDS_AVAILABLE_DRIVE);
	    m_availableDriverList.InsertColumn(0,str,LVCFMT_LEFT,nWidth/2);
	    str.LoadString(IDS_AVAILABLE_DRIVE_TYPE);
	    m_availableDriverList.InsertColumn(1,str,LVCFMT_LEFT,nWidth/2);
    
	    m_availableDriverList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	}

	m_availableDriverList.DeleteAllItems();
	m_checkSetDefaultDriver = FALSE;
	UpdateData(FALSE);

	int i = 0;
	ULONG uDriveMark = _getdrives();
	//uDriveMark >>= 2;
	int count = 26;

	while (count--)
	{
		if (!(uDriveMark & 1))
		{
			availableDriverLetter = 'A' + i;
			availableDriverName[0] = availableDriverLetter;
			availableDriverName[1] = ':';
			availableDriverName[2] = '\0';

			if (availableDriverLetter == 'A' || availableDriverLetter == 'B')
			{
				i++;
				uDriveMark >>= 1;
				continue;
			}
			else
			{
				m_availableDriverList.InsertItem(index,availableDriverName);
				m_availableDriverList.SetItemText(index,1,"Available");
				index++;
			}
			
		}

		i++;
		uDriveMark >>= 1;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CAvailableDriverDialog::OnOK() 
{
	// TODO: Add extra validation here
	
	static int inputNumber = 0;
    int itemCount;
	int encryptType;
	BYTE checkMark;
    BYTE checkSum;
	BYTE bit;
	CString tempStr;
	PBYTE tempPointer;
	PBYTE pBuf;
	DWORD dwStartSector;
	BOOL bResult;

	PBIOS_DRIVE_PARAM pDriveParam;
    PPARTITION_INFO_2000 pParInfo;
	PDRIVE_MAP pdriveMap;

	pdriveMap = (PDRIVE_MAP)malloc(sizeof(DRIVE_MAP));

	itemCount = m_availableDriverList.GetItemCount();

    UpdateData(TRUE);

	tempStr = driverName.Mid(0,1);
	char *directory = (LPSTR)(LPCTSTR)tempStr;
    unsigned char *pKey = (unsigned char *)(m_mountCheckPassword.GetBuffer(0));
	m_mountCheckPassword.ReleaseBuffer();

	pBuf = (PBYTE)malloc(512);
	pDriveParam = (PBIOS_DRIVE_PARAM)malloc(sizeof(BIOS_DRIVE_PARAM));
	pParInfo = (PPARTITION_INFO_2000)malloc(sizeof(PARTITION_INFO_2000));

	bResult = PartitionInfoByDriveLetter(*directory,pParInfo);

	if (bResult)
	{
		GetDriveParam(pParInfo->nHardDiskNum,pDriveParam);
		dwStartSector = pParInfo->pi.StartingOffset.QuadPart/pDriveParam->SectorsSize;
		ReadSector(dwStartSector,1,pBuf,pParInfo->nHardDiskNum,pDriveParam);
		tempPointer = pBuf;
		encryptType = (((PYGCRYDISK_BOOTSECTOR)tempPointer)->uEncrytTypeAndKeyLen) >> 5;
		bit = 127;
		BYTE ch  = (((PYGCRYDISK_BOOTSECTOR)tempPointer)->uCheckSumAndEncryptMark) & bit;

		YGDecryptBootSector(pBuf,pKey,m_mountCheckPassword.GetLength());
		checkMark = ((PYGCRYDISK_BOOTSECTOR)pBuf)->uMark;

		if (m_mountCheckPassword.GetLength() == 0)
		{
			AfxMessageBox(IDS_INPUT_PASSWORD,MB_ICONEXCLAMATION);
		}
		else
		{
			if (inputNumber == 3)
			{
				AfxMessageBox(IDS_INPUT_EXCEED_THREE_TIMES,MB_ICONSTOP);
			}	
			else
			{
				checkSum = (BYTE)YGNSMCheckSum(pKey,m_mountCheckPassword.GetLength());
				checkSum = checkSum >> 1;
				tempStr = driverName.Mid(0,1);
				char *originalDirectory = (LPSTR)(LPCTSTR)tempStr;
				pdriveMap->original_drive = *originalDirectory;

				if (ch != checkSum || (checkMark != 'Y'))
				{
					inputNumber++;
					AddCount();
					AfxMessageBox(IDS_PASSWORD_IS_INCORRECT,MB_ICONSTOP);
				}
				else
				{
					int i = 0;
					for (i = 0;i < itemCount;i++)
					{
						if (m_availableDriverList.GetItemState(i,LVIS_SELECTED) == LVIS_SELECTED)
						{
							mirrorDriverName = m_availableDriverList.GetItemText(i,0);
							break;
						}
					}

					if (i == itemCount)
					{
						CString str;
						str.LoadString(IDS_SELECT_DRIVER_FIRSTLY);
						MessageBox(str,NULL,MB_ICONEXCLAMATION);
					}
					else
					{
						if(m_checkSetDefaultDriver == TRUE)
						{
							CString temp;
							temp = mirrorDriverName.Mid(0,1);
							PBYTE defaultDriver = (PBYTE)(temp.GetBuffer(temp.GetLength() + 1));
							temp.ReleaseBuffer();
						
							SetDefaultInfo(pKey,m_mountCheckPassword.GetLength(),defaultDriver);
						//::WritePrivateProfileString("EncryptInfo","Password",strName,szPath);
						//::WritePrivateProfileString("EncryptInfo","DefaultDriver",defaultDriver,szPath);
						}

						tempStr = mirrorDriverName.Mid(0,1);
						char *mirrorDirectory = (LPSTR)(LPCTSTR)tempStr;
						pdriveMap->mirror_drive = *mirrorDirectory;

						UINT EncryptType = encryptType;
						PBYTE	key = (PBYTE)malloc(32);
						_mbscpy(key,pKey);
						BOOL b  = MakeCryDisk(pdriveMap,key,m_mountCheckPassword.GetLength(),EncryptType);

						if (!b)
						{
							AfxMessageBox(IDS_MAKE_MIRROR_DRIVE_FAILED,MB_ICONSTOP);
							free(key);
							free(pBuf);
							free(pDriveParam);
							free(pParInfo);
							free(pdriveMap);
							CDialog::OnOK();
						}
						else
						{
							SaveDriverInfo(pdriveMap);
						}
						
						free(key);
						free(pBuf);
						free(pDriveParam);
						free(pParInfo);
						free(pdriveMap);
						CDialog::OnOK();
					}
				}
			}
		}
	}
	else
	{
		free(pBuf);
		free(pDriveParam);
		free(pParInfo);
		free(pdriveMap);
		
		CDialog::OnOK();
	}
}
