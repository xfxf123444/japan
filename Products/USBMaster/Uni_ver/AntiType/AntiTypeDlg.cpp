// AntiTypeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AntiType.h"
#include "AntiTypeDlg.h"
#include "EncryptDialog.h"
#include "DecryptDialog.h"
#include "AvailableDriverDialog.h"
#include "ChangePasswordDialog.h"
#include <windows.h>
#include <winioctl.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <stdio.h>
#include <direct.h>
#include <dbt.h>
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

CString driverName;
CString mirrorDriverName;
HWND g_handle;
static int sDriveNum = 0;
static BOOL bFirstMark = FALSE;
static int nInitNum = 0;

const int PREVENT_POPUP_TIMER_ID = 101;
const int SCHEDULE_TIMER_ID  = 102;

CImageList		g_imgList_LIST;
BOOL    g_bAbort = FALSE;
ORIGINALDRIVER_MIRRORDRIVER usedDrivers[4];
BYTE btRemovedDriver[27] = {0};

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
// CAntiTypeDlg dialog

CAntiTypeDlg::CAntiTypeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAntiTypeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAntiTypeDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAntiTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAntiTypeDlg)
	DDX_Control(pDX, IDC_REMOVABLE_DRIVER_LIST, m_removableDriverList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAntiTypeDlg, CDialog)
	//{{AFX_MSG_MAP(CAntiTypeDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ENCRYPT, OnEncrypt)
	ON_BN_CLICKED(IDC_DECRYPT, OnDecrypt)
	ON_BN_CLICKED(IDC_BUTTON_DISMOUNT_ALL, OnButtonDismountAll)
	ON_BN_CLICKED(IDC_BUTTON_MOUNT, OnButtonMount)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_PASSWORD, OnButtonChangePassword)
	ON_COMMAND(ID_ICON_SHOW_WINDOW, OnIconShowWindow)
	ON_COMMAND(ID_ICON_EXIT, OnIconExit)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_WM_DEVICECHANGE()
	ON_MESSAGE(WM_MY_MESSAGE,OnMyMessage)
	ON_MESSAGE(WM_NOTIFY_ICON,OnNotifyIcon)
	ON_MESSAGE(WM_DEVICE_ARRIVED,OnDeviceArrived)
	ON_MESSAGE(WM_DEVICE_REMOVED,OnDeviceRemoved)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAntiTypeDlg message handlers

void SaveDriverInfo(PDRIVE_MAP pDriveMap)
{
	for (int j = 0;j < 4;j++)
	{
		if (usedDrivers[j].btOriginalDriver < 'A' || usedDrivers[j].btOriginalDriver > 'Z')
		{
			usedDrivers[j].btOriginalDriver = pDriveMap->original_drive;
			usedDrivers[j].btMirrorDriver = pDriveMap->mirror_drive;
			break;
		}
		else
		{
			continue;
		}
	}
}

void GetRemovedDriver(ULONG unitMark)
{
	for (char i = 0;i < 26;i++)
	{
		if (unitMark & 0x1)
		{
			btRemovedDriver[i] = i + 'A';
		}

		unitMark = unitMark >> 1;
	}
}

TCHAR FirstDriveFromMark(ULONG unitMark)
{
	char  i = 0;
	for (i = 0;i < 26;i++)
	{
		if (unitMark & 0x1)
		{
			break;
		}

		unitMark = unitMark >> 1;
	}

	return i + 'A';
}

void InitialDriverInfo()
{
	for (int i = 0;i < 4;i++)
	{
		usedDrivers[i].btOriginalDriver = '0';
		usedDrivers[i].btMirrorDriver = '0';
	}
}

void DismountDrive()
{
	BYTE tchar;

	for(int i = 0;i < 4;i++)
	{
		tchar = GetMPDriveInfo(i);
		
		if(tchar)
			DiscardVirtualDrive(tchar);
	}
}

BOOL EnstimateDriver(char chDriver)
{
	PBIOS_DRIVE_PARAM pDriveParam;
    PPARTITION_INFO_2000 pParInfo;
	PBYTE pBuf;
	BOOL bResult;
	int nKeyLen;
	int nEncryptType;
	BYTE bit = 128;

	pBuf = (PBYTE)malloc(512);
	pDriveParam = (PBIOS_DRIVE_PARAM)malloc(sizeof(BIOS_DRIVE_PARAM));
	pParInfo = (PPARTITION_INFO_2000)malloc(sizeof(PARTITION_INFO_2000));	

	bResult = PartitionInfoByDriveLetter(chDriver,pParInfo);

	if (bResult)
	{
		GetDriveParam(pParInfo->nHardDiskNum,pDriveParam);
		DWORD dwStartSector = pParInfo->pi.StartingOffset.QuadPart/pDriveParam->SectorsSize;
		ReadSector(dwStartSector,1,pBuf,
			pParInfo->nHardDiskNum,pDriveParam);
		BYTE btTemp = 31;
		nEncryptType = (((PYGCRYDISK_BOOTSECTOR)pBuf)->uEncrytTypeAndKeyLen) >> 5;
		nKeyLen = (((PYGCRYDISK_BOOTSECTOR)pBuf)->uEncrytTypeAndKeyLen) & btTemp;
					   
		if ((((PYGCRYDISK_BOOTSECTOR)pBuf)->uCheckSumAndEncryptMark & bit) &&
			(nEncryptType >= 1 && nEncryptType <= 5) &&
			(nKeyLen > 1 && nKeyLen <= 31) &&
			(((PYGCRYDISK_BOOTSECTOR)pBuf)->uCheckSumAndEncryptMark != 170) &&
			(((PYGCRYDISK_BOOTSECTOR)pBuf)->uEncrytTypeAndKeyLen != 85))
		{
			free(pBuf);
			free(pDriveParam);
			free(pParInfo);
			return TRUE;
		}
	}
	
	free(pBuf);
	free(pDriveParam);
	free(pParInfo);
	
	return FALSE;
}

UINT DeviceArrival(PTHREAD_PARAM pThreadParam)
{
//	CAntiTypeDlg *dlg = (CAntiTypeDlg *)pParam;
	int encryptType;
	BOOL bResult;
	CString temp;
	PBIOS_DRIVE_PARAM pDriveParam;
	PPARTITION_INFO_2000 pParInfo;
	PDRIVE_MAP pDriveMap;
//	PTHREAD_PARAM pThreadParam;

	PBYTE pBuf;
	BYTE checkSum;
	BYTE keyBuffer[33];
	PBYTE buffer;
	BYTE bit;
	char key[MAX_PATH];

	DWORD len = MAX_PATH;
	GetUserName(key,&len);
//	tempStr = driverName.Mid(0,1);
//	char *directory = (LPSTR)(LPCTSTR)tempStr;
//	GetPrivateProfileString("EncryptInfo","Password",NULL,password.GetBuffer(MAX_PATH),
//			MAX_PATH,szPath);

	temp = key;
	PBYTE tempKey = (PBYTE)(temp.GetBuffer(0));
	temp.ReleaseBuffer();
	pDriveMap = (PDRIVE_MAP)malloc(sizeof(DRIVE_MAP));

	FILE *fp;
	buffer = (PBYTE)malloc(sizeof(SAVE_INFO));
	ZeroMemory(buffer,sizeof(SAVE_INFO));
	fp = fopen(pThreadParam->szSaveInfoPath,"r+b");
	fread(buffer,sizeof(char),sizeof(SAVE_INFO),fp);
	int keyLen =((PSAVE_INFO)buffer)->keyLen;
	pDriveMap->mirror_drive =((PSAVE_INFO)buffer)->defaultName;
	pDriveMap->original_drive = pThreadParam->szOriginalDriver;
	for (int j = 0;j < 32;j++)
	{
		keyBuffer[j] = buffer[j];
	}

//	keyBuffer[32] = '\0';

	YGAESDecryptData(tempKey,temp.GetLength(),keyBuffer,keyBuffer,32);
	keyBuffer[keyLen] = '\0';
	free(buffer);
	fclose(fp);
					
	pBuf = (PBYTE)malloc(512);
	pDriveParam = (PBIOS_DRIVE_PARAM)malloc(sizeof(BIOS_DRIVE_PARAM));
	pParInfo = (PPARTITION_INFO_2000)malloc(sizeof(PARTITION_INFO_2000));
	bit = 127;
	
	bResult = PartitionInfoByDriveLetter(pThreadParam->szOriginalDriver,pParInfo);

	if (bResult)
	{
		GetDriveParam(pParInfo->nHardDiskNum,pDriveParam);
		DWORD dwStartSector = pParInfo->pi.StartingOffset.QuadPart/pDriveParam->SectorsSize;
		ReadSector(dwStartSector,1,pBuf,
		pParInfo->nHardDiskNum,pDriveParam);
		encryptType = (((PYGCRYDISK_BOOTSECTOR)pBuf)->uEncrytTypeAndKeyLen) >> 5;
		BYTE ch = (((PYGCRYDISK_BOOTSECTOR)pBuf)->uCheckSumAndEncryptMark) & bit;
		checkSum = (BYTE)YGNSMCheckSum(keyBuffer,keyLen);
		checkSum = checkSum >> 1;

		YGDecryptBootSector(pBuf,keyBuffer,keyLen);

		if(((PYGCRYDISK_BOOTSECTOR)pBuf)->uMark != 'Y' || checkSum != ch)
		{
			CAvailableDriverDialog dlg;
			dlg.DoModal();
		}
		else
		{
			if (pDriveMap->mirror_drive >= 'A' && pDriveMap->mirror_drive <= 'Z')
			{
				int k = 0;
				int nCount = 26;
				ULONG uDriveMark = _getdrives();
				BYTE btUsedDriver;

				while (nCount--)
				{
					if (uDriveMark & 1)
					{
						btUsedDriver = 'A' + k;

						if (btUsedDriver == pDriveMap->mirror_drive)
						{
							AfxMessageBox(IDS_DEFAULT_DRIVER_BE_USED,MB_ICONEXCLAMATION);
							CAvailableDriverDialog dlg;
							dlg.DoModal();
							break;
						}
						else
						{
							k++;
							uDriveMark >>= 1;
							continue;
						}
					}

					k++;
					uDriveMark >>= 1;
				}	

				if (nCount == -1)
				{
					if (MakeCryDisk(pDriveMap,keyBuffer,keyLen,encryptType))
					{
						bFirstMark = TRUE;
						SaveDriverInfo(pDriveMap);
					}
				}
			}
			else
			{
				free(pBuf);
				free(pDriveParam);
				free(pParInfo);
				free(pDriveMap);
				return 0;
			}
		}
	}

	free(pBuf);
	free(pDriveParam);
	free(pParInfo);
	free(pDriveMap);
	return 1;
}

BOOL GetDisksProperty(HANDLE hDevice, PSTORAGE_DEVICE_DESCRIPTOR pDevDesc)
{
	STORAGE_PROPERTY_QUERY	Query;	// input param for query
	DWORD dwOutBytes;				// IOCTL output length
	BOOL bResult;					// IOCTL return val

	// specify the query type
	Query.PropertyId = StorageDeviceProperty;
	Query.QueryType = PropertyStandardQuery;

	// Query using IOCTL_STORAGE_QUERY_PROPERTY 
	bResult = ::DeviceIoControl(hDevice,			// device handle
			IOCTL_STORAGE_QUERY_PROPERTY,			// info of device property
			&Query, sizeof(STORAGE_PROPERTY_QUERY),	// input data buffer
			pDevDesc, pDevDesc->Size,				// output data buffer
			&dwOutBytes,							// out's length
			(LPOVERLAPPED)NULL);					

	return bResult;
}

BOOL CAntiTypeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	g_handle = this->m_hWnd;
	nInitNum++;

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
	
	// TODO: Add extra initialization here

	if (nInitNum == 1)
	{
		InitialDriverInfo();
	}

//	CAntiTypeDlg *dlg = new CAntiTypeDlg();
//	::SendMessage(dlg->m_hWnd,WM_MY_MESSAGE,NULL,NULL);

//	char szPath[MAX_PATH];
//	char fileName[] = "\\encrypt.bat";
//	char encryptInfoDirectory[] = "\\USBMaster";
//	DWORD len = MAX_PATH;
//	char key[MAX_PATH];
//	BYTE buffer[32];
//	BYTE pKey[] = "123456";
//	CString strName;
//	CString temp;
//	CString password;
//	GetUserName(key,&len);
//	temp = key;
//	PBYTE tempKey1 = (PBYTE)(temp.GetBuffer(temp.GetLength() + 1));
//	temp.ReleaseBuffer();
//	_mbscpy(buffer,pKey);
//	YGAESEncryptData(tempKey1,temp.GetLength(),buffer,buffer,32);
//	YGAESDecryptData(tempKey,temp.GetLength(),buffer,buffer,32);
//	temp = buffer;
//	strName = temp.Mid(0,6);
//	strName = buffer;
//	CString defaultDriver = "Y";
//    SHGetSpecialFolderPath(0,szPath,CSIDL_APPDATA,0);
//	strcat(szPath,encryptInfoDirectory);
//	CreateDirectory(szPath,NULL);
//	strcat(szPath,fileName);
//	
//	if(PathFileExists(szPath))
//	{
//		DeleteFile(szPath);
//	}

	//strName = dlg->m_encryptPassword;
 
	//::WritePrivateProfileString("EncryptInfo","Password",strName,szPath);
//	GetPrivateProfileString("EncryptInfo","Password",NULL,password.GetBuffer(MAX_PATH),
//					MAX_PATH,szPath);
//	PBYTE ppKey = (PBYTE)password.GetBuffer(password.GetLength() + 1);
//	password.ReleaseBuffer();
//	_mbscpy(buffer,ppKey);
//	PBYTE tempKey2 = (PBYTE)(temp.GetBuffer(temp.GetLength() + 1));
//	temp.ReleaseBuffer();
//	YGAESDecryptData(tempKey2,temp.GetLength(),buffer,buffer,32);
//	temp = buffer;
//	strName = temp.Mid(0,6);
	//::WritePrivateProfileString("EncryptInfo","Password",strName,szPath);
	//::WritePrivateProfileString("EncryptInfo","DefaultDriver",defaultDriver,szPath);*/

//	#pragma   pack(push)   
//    #pragma   pack   (1) 
//
//	char szPath[MAX_PATH];
//	char fileName[] = "\\encrypt.dat";
//	char encryptInfoDirectory[] = "\\USBMaster";
//	DWORD len = MAX_PATH;
//	char key[MAX_PATH];
//	CString temp;
//	BYTE pKey[] = "123456";
//	//BYTE buffer[33];
//
//	GetUserName(key,&len);
//	temp = key;
//	PBYTE tempKey = (PBYTE)(temp.GetBuffer(temp.GetLength() + 1));
//	temp.ReleaseBuffer();
//	_mbscpy(saveInfo.password,pKey);
//	saveInfo.defaultName = 'Y';
//
//	SHGetSpecialFolderPath(0,szPath,CSIDL_APPDATA,0);
//	strcat(szPath,encryptInfoDirectory);
//	CreateDirectory(szPath,NULL);
//	strcat(szPath,fileName);
//	
//	if(PathFileExists(szPath))
//	{
//		DeleteFile(szPath);
//	}
//	
//	FILE *fp;
//	fp = fopen(szPath,"w+b");
//	YGAESEncryptData(tempKey,temp.GetLength(),saveInfo.password,saveInfo.password,32);
//	fwrite(&saveInfo,33,1,fp);
//	fclose(fp);

//	FILE *fp2;
//	fp2 = fopen(szPath,"r+b");
//	fread(buffer,sizeof(char),32,fp2);
//	YGAESDecryptData(tempKey,temp.GetLength(),buffer,buffer,32);
//	//_mbscpy(saveInfo.password,buffer);
//	//int k = fwrite((PBYTE)&saveInfo,33,1,fp2);
//	fclose(fp2);

//	FILE *fp1;
//	fp1 = fopen(szPath,"wb");
//	_mbscpy(buffer,saveInfo.password);
//	YGAESDecryptData(tempKey,temp.GetLength(),buffer,buffer,32);
//	_mbscpy(saveInfo.password,buffer);
//	fwrite(&saveInfo,33,1,fp1);
//	fclose(fp1);

	//#pragma   pack(pop)
//	unsigned short n = sizeof(int);

//	char szPath[MAX_PATH];
//	char fileName[] = "\\encrypt.dat";
//	char encryptInfoDirectory[] = "\\USBMaster";
//
//	SHGetSpecialFolderPath(0,szPath,CSIDL_APPDATA,0);
//	strcat(szPath,encryptInfoDirectory);
//	strcat(szPath,fileName);
//
//	FILE *fp;
//	fp = fopen(szPath,"r+b");
//	fread(&saveInfo,34,1,fp);
//	int keylen = saveInfo.keyLen;
//	fclose(fp);
//	int g = sizeof(UCHAR);

    CRect listRect;
	CImageList img;
	CString str;
	int nWidth;
	static int initDlgNumber = 1;
//	CAntiTypeDlg *pAntiTypeDlg;
//	CListCtrl *pListCtrl;

//  if(g_mountOrDismount == TRUE)
//  SetDlgItemText(IDC_BUTTON_MOUNT,"Dismount");
	LONG listStyle;

	listStyle = GetWindowLong(m_removableDriverList.m_hWnd,GWL_STYLE);
	listStyle |= LVS_SHOWSELALWAYS;
	SetWindowLong(m_removableDriverList.m_hWnd,GWL_STYLE,listStyle);

    if (initDlgNumber == 1)
	{
	    m_removableDriverList.SetImageList(&img,LVSIL_SMALL);
	    m_removableDriverList.GetClientRect(&listRect);
	    nWidth = listRect.right - listRect.left;

	    str.LoadString(IDS_REMOVABLE_DRIVE);
	    m_removableDriverList.InsertColumn(0,str,LVCFMT_LEFT,nWidth/4);
	    str.LoadString(IDS_REMOVABLE_DRIVE_SIZE);
	    m_removableDriverList.InsertColumn(1,str,LVCFMT_LEFT,nWidth/4);
	    str.LoadString(IDS_REMOVABLE_DRIVE_TYPE);
	    m_removableDriverList.InsertColumn(2,str,LVCFMT_LEFT,nWidth/4);
	    str.LoadString(IDS_REMOVABLE_DRIVE_STATUS);
	    m_removableDriverList.InsertColumn(3,str,LVCFMT_LEFT,nWidth/4);

	    m_removableDriverList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	}

//	pListCtrl->Create(WS_CHILD|WS_VISIBLE|WS_BORDER|LVS_SHOWSELALWAYS|LVS_SINGLESEL,
//		listRect,pAntiTypeDlg,IDC_REMOVABLE_DRIVER_LIST);
//	m_removableDriverList.Create(LVS_SHOWSELALWAYS|LVS_SINGLESEL,listRect,pAntiTypeDlg,IDC_REMOVABLE_DRIVER_LIST);
	m_removableDriverList.DeleteAllItems();

//	HWND handle = ::FindWindow("CAntiTypeDlg",NULL);
//	::SendMessage(handle,WM_MY_MESSAGE,NULL,NULL);
//	CAntiTypeDlg dlg1;
//	::SendMessage(dlg1.m_hWnd,WM_MY_MESSAGE,NULL,NULL);

	int DSLength = GetLogicalDriveStrings(0,NULL);
	char *Dstr = (char *)malloc(DSLength+1);
	GetLogicalDriveStrings(DSLength,(LPTSTR)Dstr);

	int driveType;
	int si = 0;
    int itemIndex = 0;
	int nKeyLen;
	int nEncryptType;
	int totalSpace;
	BOOL bResult;
	BYTE bit = 128;

	PBYTE pBuf;
	PBYTE tempPointer;

	CString driveVolume;
	CString partType;
	CString strDrive;

	PPARTITION_INFO_2000 pParInfo;
	PBIOS_DRIVE_PARAM pDriveParam;
	PSTORAGE_DEVICE_DESCRIPTOR pDevDesc;

	pBuf = (PBYTE)malloc(512);
	pParInfo = (PPARTITION_INFO_2000)malloc(sizeof(PARTITION_INFO_2000));
	pDriveParam = (PBIOS_DRIVE_PARAM)malloc(sizeof(BIOS_DRIVE_PARAM));

	for (int i = 0;i < DSLength/4;i++)
	{
		char  directory[3] = {Dstr[si],':'};
		driveType = GetDriveType(Dstr+i*4);

		if (directory[0] == 'A' || directory[0] == 'a')
		{
			si += 4;
			continue;
		}
        
		if (driveType == DRIVE_REMOVABLE)
		{
			tempPointer = pBuf;
			bResult = PartitionInfoByDriveLetter(directory[0],pParInfo);

			if (bResult)
			{
			   switch((int)(pParInfo->pi.PartitionType))
			   {
			   case 4: 
			   case 6:
			   case 14:
			   case 20:
			   case 1:
			   case 17:
			   case 11:
			   case 12:
			   case 18: partType = "FAT";break;
			   case 7: partType = "NTFS";break;
			   case 131: partType = "Linux";break;
			   case 130: partType = "LinuxSwp";break;
			   default :partType = "Unknown";break;
			   }

			   m_removableDriverList.InsertItem(itemIndex,directory);
			   GetDriveParam(pParInfo->nHardDiskNum,pDriveParam);
			   totalSpace = (DWORD)(pParInfo->pi.PartitionLength.QuadPart/(1024*1024)) & 0xFFFFFFFF;
			   driveVolume.Format("%d",totalSpace);
			   driveVolume = driveVolume + " MB";
			   m_removableDriverList.SetItemText(itemIndex,1,(LPCTSTR)driveVolume);
			   m_removableDriverList.SetItemText(itemIndex,2,(LPCTSTR)partType);
			   
			   DWORD dwStartSector = pParInfo->pi.StartingOffset.QuadPart/pDriveParam->SectorsSize;
			   ReadSector(dwStartSector,1,pBuf,pParInfo->nHardDiskNum,pDriveParam);
			   BYTE btTemp = 31;
			   nEncryptType = (((PYGCRYDISK_BOOTSECTOR)pBuf)->uEncrytTypeAndKeyLen) >> 5;
			   nKeyLen = (((PYGCRYDISK_BOOTSECTOR)pBuf)->uEncrytTypeAndKeyLen) & btTemp;
			   
			   if ((((PYGCRYDISK_BOOTSECTOR)pBuf)->uCheckSumAndEncryptMark & bit) &&
				   (nEncryptType >= 1 && nEncryptType <= 5) &&
				   (nKeyLen > 1 && nKeyLen <= 31) &&
				   (((PYGCRYDISK_BOOTSECTOR)pBuf)->uCheckSumAndEncryptMark != 170) &&
				   (((PYGCRYDISK_BOOTSECTOR)pBuf)->uEncrytTypeAndKeyLen != 85))
			   {
				   m_removableDriverList.SetItemText(itemIndex,3,"Encrypted !");
			   }
			   else
			   {
				   m_removableDriverList.SetItemText(itemIndex,3,"Not Encrypt !");
			   }

			   itemIndex++;
			}	
		}

		if (driveType == DRIVE_FIXED)
		{
			strDrive.Format("\\\\.\\%c:",directory[0]);
			HANDLE hDevice = CreateFile(strDrive, GENERIC_READ,
							FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

			if (hDevice != INVALID_HANDLE_VALUE)
			{
				pDevDesc = (PSTORAGE_DEVICE_DESCRIPTOR)malloc( sizeof(STORAGE_DEVICE_DESCRIPTOR));
				pDevDesc->Size = sizeof(STORAGE_DEVICE_DESCRIPTOR);

				if (GetDisksProperty(hDevice, pDevDesc))
				{
					if (pDevDesc->BusType == BusTypeUsb)
					{
			            tempPointer = pBuf;
						bResult = PartitionInfoByDriveLetter(directory[0],pParInfo);						
						if (bResult)
						{
							GetDriveParam(pParInfo->nHardDiskNum,pDriveParam);
							m_removableDriverList.InsertItem(itemIndex,directory);
							totalSpace = (DWORD)(pParInfo->pi.PartitionLength.QuadPart/(1024*1024)) & 0xFFFFFFFF;
							driveVolume.Format("%d",totalSpace);
							driveVolume = driveVolume + " MB";
							m_removableDriverList.SetItemText(itemIndex,1,(LPCTSTR)driveVolume);
 
							switch((int)(pParInfo->pi.PartitionType))
							{
							case 4: 
							case 6:
							case 14:
							case 20:
							case 1:
							case 17:
							case 11:
							case 12:
							case 18: partType = "FAT";break;
							case 7: partType = "NTFS";break;
							case 131: partType = "Linux";break;
							case 130: partType = "LinuxSwp";break;
							default :partType = "Unknown";break;
							}

							m_removableDriverList.SetItemText(itemIndex,2,(LPCTSTR)partType);
							DWORD dwStartSector = pParInfo->pi.StartingOffset.QuadPart/pDriveParam->SectorsSize;
							ReadSector(dwStartSector,1,pBuf,(BYTE)(pParInfo->nHardDiskNum),pDriveParam);
							BYTE btTemp = 31;
							nEncryptType = (((PYGCRYDISK_BOOTSECTOR)pBuf)->uEncrytTypeAndKeyLen) >> 5;
							nKeyLen = (((PYGCRYDISK_BOOTSECTOR)pBuf)->uEncrytTypeAndKeyLen) & btTemp;

							if ((((PYGCRYDISK_BOOTSECTOR)pBuf)->uCheckSumAndEncryptMark & bit) &&
								(nEncryptType >= 1 && nEncryptType <= 5) &&
								(nKeyLen > 1 && nKeyLen <= 31) &&
								(((PYGCRYDISK_BOOTSECTOR)pBuf)->uCheckSumAndEncryptMark != 170) &&
								(((PYGCRYDISK_BOOTSECTOR)pBuf)->uEncrytTypeAndKeyLen != 85))
							{
								m_removableDriverList.SetItemText(itemIndex,3,"Encrypted !");
							}
							else
							{
								m_removableDriverList.SetItemText(itemIndex,3,"Not Encrypt !");
							}
							
							itemIndex++;
						}
					}
				}

				free(pDevDesc);
			    CloseHandle(hDevice);
			}
		}

		si += 4;
	}
    
	free(pParInfo);
	free(pBuf);
	free(pDriveParam);
	free(Dstr);
	initDlgNumber++;
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAntiTypeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if ((nID & 0xFFF0)==SC_CLOSE)
	{
		AnimateWindow(/*GetSafeHwnd(),*/1000,AW_HIDE|AW_BLEND);
        KillTimer(0);
		ShowWindow(SW_HIDE);
	}
	else if ((nID & 0xFFF0)==SC_MINIMIZE)
	{
		AnimateWindow(/*GetSafeHwnd(),*/1000,AW_HIDE|AW_BLEND);
        KillTimer(0);
		ShowWindow(SW_HIDE);
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAntiTypeDlg::OnPaint() 
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
HCURSOR CAntiTypeDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CAntiTypeDlg::OnEncrypt() 
{
	// TODO: Add your control notification handler code here
    int itemCount;

    itemCount = m_removableDriverList.GetItemCount();
	int i = 0;
	for(; i < itemCount; i++)
	{
		if(m_removableDriverList.GetItemState(i,LVIS_SELECTED) == LVIS_SELECTED)
		{
			driverName = m_removableDriverList.GetItemText(i,0);
			break;
		}
	}

	if(i == itemCount)
	{
		AfxMessageBox(IDS_SELECT_DRIVER_FIRSTLY,MB_ICONEXCLAMATION);
	}
	else
	{
		//m_removableDriverList.SetItemState(i,LVIS_SELECTED,LVIS_SELECTED);
		//m_removableDriverList.ModifyStyle(0,LVS_SHOWSELALWAYS);
		//CEncryptDialog *dlg=new CEncryptDialog;
	    //dlg->Create();
	    //dlg->ShowWindow(SW_SHOW);
		if(m_removableDriverList.GetItemText(i,3).Compare("Encrypted !") == 0)
		{
			AfxMessageBox(IDS_DRIVER_ENCRYPTED,MB_ICONSTOP);
		}
        else
		{
            CEncryptDialog dlg;
		    dlg.DoModal();
			CAntiTypeDlg::UpdateDriverList();
		}	

	}
	
}

void CAntiTypeDlg::OnDecrypt() 
{
	// TODO: Add your control notification handler code here

    //CDecryptDialog *dlg = new CDecryptDialog;
	//dlg->Create();
	//dlg->ShowWindow(SW_SHOW);

	int itemCount;
    itemCount = m_removableDriverList.GetItemCount();
	int i = 0;
	for(; i < itemCount; i++)
	{
		if(m_removableDriverList.GetItemState(i,LVIS_SELECTED) == LVIS_SELECTED)
		{
			driverName = m_removableDriverList.GetItemText(i,0);
			break;
		}
	}

	if(i == itemCount)
	{
		AfxMessageBox(IDS_SELECT_DRIVER_FIRSTLY,MB_ICONEXCLAMATION);
	}
	else
	{
		if(m_removableDriverList.GetItemText(i,3).Compare("Not Encrypt !") == 0)
		{
			AfxMessageBox(IDS_DRIVER_NOT_ENCRYPTED,MB_ICONSTOP);
		}
		else
		{
            CDecryptDialog dlg;
	        dlg.DoModal();
			CAntiTypeDlg::UpdateDriverList();
		}
		
	}

}

void CAntiTypeDlg::OnButtonDismountAll() 
{
	// TODO: Add your control notification handler code here

	BYTE tchar;

	for(int i = 0;i < 4;i++)
	{
		tchar = GetMPDriveInfo(i);
		
		if(tchar)
			DiscardVirtualDrive(tchar);
	}

	bFirstMark = FALSE;
	InitialDriverInfo();
}

void CAntiTypeDlg::OnButtonMount() 
{
	// TODO: Add your control notification handler code here
	
	int itemCount;
	int count;
	CString temp;

    itemCount = m_removableDriverList.GetItemCount();
	int i = 0;
	for(; i < itemCount; i++)
	{
		if(m_removableDriverList.GetItemState(i,LVIS_SELECTED) == LVIS_SELECTED)
		{
			driverName = m_removableDriverList.GetItemText(i,0);
			break;
		}
	}

	if (i == itemCount)
	{
		AfxMessageBox(IDS_SELECT_DRIVER_FIRSTLY,MB_ICONEXCLAMATION);
	}
	else
	{
		temp = m_removableDriverList.GetItemText(i,3);
		if (!temp.CompareNoCase("Not Encrypt !"))
		{
			AfxMessageBox(IDS_DRIVER_NOT_ENCRYPTED,MB_ICONEXCLAMATION);
		}
		else
		{
			count = GetCount();
			if(count >= 3)
			{
				AfxMessageBox(IDS_INPUT_EXCEED_THREE_TIMES,MB_ICONSTOP);
			}
			else
			{
				CAvailableDriverDialog availableDriverDlg;
				availableDriverDlg.DoModal();
 			}
		}
	}
}

BOOL CAntiTypeDlg::OnDeviceChange(UINT wParam, DWORD_PTR lParam)
{
	char btRemovedChar;
	PDEV_BROADCAST_VOLUME pdbv = NULL;
	int nDriveType;
	char chTemp[4] = "C:\\";
	CString strDrive;
	
	switch(wParam)
	{
	case DBT_DEVICEREMOVECOMPLETE:
		pdbv = (PDEV_BROADCAST_VOLUME)lParam;
		GetRemovedDriver(pdbv->dbcv_unitmask);
		PostMessage(WM_DEVICE_REMOVED,wParam,lParam);
		break;
	case DBT_DEVICEARRIVAL:
		pdbv = (PDEV_BROADCAST_VOLUME)lParam;
		btRemovedChar = FirstDriveFromMark(pdbv->dbcv_unitmask);
		chTemp[0] = btRemovedChar;
		nDriveType = GetDriveType(chTemp);
		
		if (nDriveType == DRIVE_REMOVABLE)
		{
			PostMessage(WM_DEVICE_ARRIVED,wParam,(LPARAM)btRemovedChar);
			break;
		}
		if (nDriveType == DRIVE_FIXED)
		{
			PSTORAGE_DEVICE_DESCRIPTOR pDevDesc;
			strDrive.Format("\\\\.\\%c:",chTemp[0]);
			HANDLE hDevice = CreateFile(strDrive, GENERIC_READ,
				FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
			
			if (hDevice != INVALID_HANDLE_VALUE)
			{
				pDevDesc = (PSTORAGE_DEVICE_DESCRIPTOR)malloc( sizeof(STORAGE_DEVICE_DESCRIPTOR));
				pDevDesc->Size = sizeof(STORAGE_DEVICE_DESCRIPTOR);
				
				if (GetDisksProperty(hDevice, pDevDesc))
				{
					if (pDevDesc->BusType == BusTypeUsb)
					{
						PostMessage(WM_DEVICE_ARRIVED,wParam,(LPARAM)btRemovedChar);
						break;
					}
				}

				CloseHandle(hDevice);
			}
		}
	}
	return TRUE;
}

LRESULT CAntiTypeDlg::OnDeviceArrived(WPARAM wParam, LPARAM lParam)
{
//	CString tempStr;
//	CString driverState;

	CAntiTypeDlg::UpdateDriverList();

	if (bFirstMark)
	{
		return FALSE;
	}
	
//	sDriveNum++;
//	driverName = m_removableDriverList.GetItemText((sDriveNum - 1),0);
//	driverState = m_removableDriverList.GetItemText((sDriveNum - 1),3);
//	tempStr = driverName.Mid(0,1);
//	char *pc = (LPSTR)(LPCTSTR)tempStr;
//	
//	if (((char)lParam) != *pc)
//	{
//		return FALSE;
//	}
	
 	if (EnstimateDriver((char)lParam))
	{
		char szPath[MAX_PATH];
		char fileName[] = "\\encrypt.dat";
		char encryptInfoDirectory[] = "\\USBMaster";
		char chDriverName[4] = "C:\\";
		
		SHGetSpecialFolderPath(0,szPath,CSIDL_APPDATA,0);
		strcat(szPath,encryptInfoDirectory);
		strcat(szPath,fileName);
		
		if(!PathFileExists(szPath))
		{
			chDriverName[0] = (char)lParam;
			driverName = chDriverName;
			CAvailableDriverDialog dlg;
			dlg.DoModal();
		}
		else
		{
			PTHREAD_PARAM pThp = (PTHREAD_PARAM)malloc(sizeof(THREAD_PARAM));
//			tempStr = driverName.Mid(0,1);
//			char *directory = (LPSTR)(LPCTSTR)tempStr;
			strcpy(pThp->szSaveInfoPath,szPath);
			pThp->szOriginalDriver = (char)lParam;
			DeviceArrival(pThp);
		}
	}
	
	return 1;	
}

LRESULT CAntiTypeDlg::OnDeviceRemoved(WPARAM wParam, LPARAM lParam)
{
	int i,j;
	
	if (sDriveNum > 0)
	{
		sDriveNum--;
	}
	
	for (i = 0;i < 4;i++)
	{
		for (j = 0;j < 26;j++)
		{
			if (btRemovedDriver[j] == usedDrivers[i].btOriginalDriver)
			{
				bFirstMark = FALSE;
				usedDrivers[i].btOriginalDriver = 0;
				btRemovedDriver[j] = 0;
				DiscardVirtualDrive(usedDrivers[i].btMirrorDriver);
				break;
			}
		}
	}
	
	
	CAntiTypeDlg::UpdateDriverList();
	
	return 1;
}

LRESULT CAntiTypeDlg::OnNotifyIcon(WPARAM wParam,LPARAM lParam)
{
    CMenu       menu;     
	CMenu		SubMenu;
	POINT		pt ;
	CString		csTitle;

	switch (lParam)
	{
	case WM_LBUTTONDBLCLK:
		PostMessage(WM_COMMAND,ID_ICON_SHOW_WINDOW,0);
		//ShowWindow(SW_SHOW);
		break;
	case WM_RBUTTONDOWN:
	{
		VERIFY(menu.LoadMenu(IDR_TRAY_MENU));
		CMenu* pPopup = menu.GetSubMenu ( 0 ) ;
		ASSERT(pPopup != NULL);
		if(IsWindowVisible())
			pPopup->CheckMenuItem(0,MF_BYPOSITION|MF_CHECKED);
		GetCursorPos ( &pt ) ;
		SetForegroundWindow ();
		pPopup->TrackPopupMenu ( TPM_LEFTALIGN | TPM_RIGHTBUTTON,
									pt.x, pt.y, AfxGetMainWnd() );
        break;
	}
	case WM_MOUSEMOVE:
		csTitle.LoadString (IDS_MOUSEMOVE_ON_TRY_ICON);
		m_pTray->SetState(csTitle);
	default: 
		break;
    }
	return 1;
}


LRESULT CAntiTypeDlg::OnMyMessage(WPARAM wParam, LPARAM lParam)
{
	CAntiTypeDlg::UpdateDriverList();

	return 1;
}

void CAntiTypeDlg::OnButtonChangePassword() 
{
	// TODO: Add your control notification handler code here
	
	int itemCount;

    itemCount = m_removableDriverList.GetItemCount();
	int i = 0;
	for(; i < itemCount; i++)
	{
		if(m_removableDriverList.GetItemState(i,LVIS_SELECTED) == LVIS_SELECTED)
		{
			driverName = m_removableDriverList.GetItemText(i,0);
			break;
		}
	}

	if(i == itemCount)
	{
		AfxMessageBox(IDS_SELECT_DRIVER_FIRSTLY,MB_ICONEXCLAMATION);
	}
	else
	{
		//m_removableDriverList.SetItemState(i,LVIS_SELECTED,LVIS_SELECTED);
		//m_removableDriverList.ModifyStyle(0,LVS_SHOWSELALWAYS);
		//CEncryptDialog *dlg=new CEncryptDialog;
	    //dlg->Create();
	    //dlg->ShowWindow(SW_SHOW);
		if(m_removableDriverList.GetItemText(i,3).Compare("Not Encrypt !") == 0)
		{
			AfxMessageBox(IDS_DRIVER_NOT_ENCRYPTED,MB_ICONSTOP);
		}
        else
		{
            CChangePasswordDialog dlg;
		    dlg.DoModal();
		}	

	}
}

void CAntiTypeDlg::OnIconShowWindow() 
{
	// TODO: Add your command handler code here
	ShowWindow(SW_SHOW);
	UpdateDriverList();
}

void CAntiTypeDlg::OnIconExit() 
{
	// TODO: Add your command handler code here
	g_imgList_LIST.DeleteImageList();
//	PostMessage(WM_CLOSE,0,0);
	CAntiTypeDlg::OnButtonDismountAll();
	CDialog::OnCancel();
}

int CAntiTypeDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here

	m_pTray = new CSystemTray (this, WM_NOTIFY_ICON, "AntiType");

	SetTimer(PREVENT_POPUP_TIMER_ID,1000,NULL);
	SetTimer(SCHEDULE_TIMER_ID,60000,NULL);
	
	return 0;
}

void CAntiTypeDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here

	if (m_pTray)
	{
		m_pTray->Close();
		delete m_pTray;
	}

	KillTimer(PREVENT_POPUP_TIMER_ID);
	KillTimer(SCHEDULE_TIMER_ID);
	
}

void CAntiTypeDlg::OnCancel() 
{
	// TODO: Add extra cleanup here

	AnimateWindow(/*GetSafeHwnd(),*/1000,AW_HIDE|AW_BLEND);
    KillTimer(0);
	ShowWindow(SW_HIDE);
}

BOOL CAntiTypeDlg::UpdateDriverList()
{
	m_removableDriverList.DeleteAllItems();
	int DSLength = GetLogicalDriveStrings(0,NULL);
	char *Dstr = (char *)malloc(DSLength+1);
	GetLogicalDriveStrings(DSLength,(LPTSTR)Dstr);

	int driveType;
	int si = 0;
    int itemIndex = 0;
	int nKeyLen;
	int nEncryptType;
	int totalSpace;
	BOOL bResult;
	BYTE bit = 128;

	PBYTE pBuf;
	PBYTE tempPointer;

	CString driveVolume;
	CString partType;
	CString strDrive;

	PPARTITION_INFO_2000 pParInfo;
	PBIOS_DRIVE_PARAM pDriveParam;
	PSTORAGE_DEVICE_DESCRIPTOR pDevDesc;

	pBuf = (PBYTE)malloc(512);
	pParInfo = (PPARTITION_INFO_2000)malloc(sizeof(PARTITION_INFO_2000));
	pDriveParam = (PBIOS_DRIVE_PARAM)malloc(sizeof(BIOS_DRIVE_PARAM));

	for (int i = 0;i < DSLength/4;i++)
	{
		char  directory[3] = {Dstr[si],':'};
		driveType = GetDriveType(Dstr+i*4);

		if (directory[0] == 'A' || directory[0] == 'a')
		{
			si += 4;
			continue;
		}
        
		if (driveType == DRIVE_REMOVABLE)
		{
			tempPointer = pBuf;
			bResult = PartitionInfoByDriveLetter(directory[0],pParInfo);

			if (bResult)
			{
			   switch((int)(pParInfo->pi.PartitionType))
			   {
			   case 4: 
			   case 6:
			   case 14:
			   case 20:
			   case 1:
			   case 17:
			   case 11:
			   case 12:
			   case 18: partType = "FAT";break;
			   case 7: partType = "NTFS";break;
			   case 131: partType = "Linux";break;
			   case 130: partType = "LinuxSwp";break;
			   default :partType = "Unknown";break;
			   }

			   m_removableDriverList.InsertItem(itemIndex,directory);
			   GetDriveParam(pParInfo->nHardDiskNum,pDriveParam);
			   totalSpace = (DWORD)(pParInfo->pi.PartitionLength.QuadPart/(1024*1024)) & 0xFFFFFFFF;
			   driveVolume.Format("%d",totalSpace);
			   driveVolume = driveVolume + " MB";
			   m_removableDriverList.SetItemText(itemIndex,1,(LPCTSTR)driveVolume);
			   m_removableDriverList.SetItemText(itemIndex,2,(LPCTSTR)partType);
			   
			   DWORD dwStartSector = pParInfo->pi.StartingOffset.QuadPart/pDriveParam->SectorsSize;
			   ReadSector(dwStartSector,1,pBuf,pParInfo->nHardDiskNum,pDriveParam);
			   BYTE btTemp = 31;
			   nEncryptType = (((PYGCRYDISK_BOOTSECTOR)pBuf)->uEncrytTypeAndKeyLen) >> 5;
			   nKeyLen = (((PYGCRYDISK_BOOTSECTOR)pBuf)->uEncrytTypeAndKeyLen) & btTemp;
			   
			   if ((((PYGCRYDISK_BOOTSECTOR)pBuf)->uCheckSumAndEncryptMark & bit) &&
				   (nEncryptType >= 1 && nEncryptType <= 5) &&
				   (nKeyLen > 1 && nKeyLen <= 31) &&
				   ((PYGCRYDISK_BOOTSECTOR)pBuf)->uCheckSumAndEncryptMark != 170 &&
				   (((PYGCRYDISK_BOOTSECTOR)pBuf)->uEncrytTypeAndKeyLen != 85))
			   {
				   m_removableDriverList.SetItemText(itemIndex,3,"Encrypted !");
			   }
			   else
			   {
				   m_removableDriverList.SetItemText(itemIndex,3,"Not Encrypt !");
			   }
			   
			   itemIndex++;
			}
		}

		if (driveType == DRIVE_FIXED)
		{
			strDrive.Format("\\\\.\\%c:",directory[0]);
			HANDLE hDevice = CreateFile(strDrive, GENERIC_READ,
							FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

			if (hDevice != INVALID_HANDLE_VALUE)
			{
				pDevDesc = (PSTORAGE_DEVICE_DESCRIPTOR)malloc( sizeof(STORAGE_DEVICE_DESCRIPTOR));
				pDevDesc->Size = sizeof(STORAGE_DEVICE_DESCRIPTOR);

				if (GetDisksProperty(hDevice, pDevDesc))
				{
					if (pDevDesc->BusType == BusTypeUsb)
					{
			            tempPointer = pBuf;
						bResult = PartitionInfoByDriveLetter(directory[0],pParInfo);						
						if (bResult)
						{
							GetDriveParam(pParInfo->nHardDiskNum,pDriveParam);
							m_removableDriverList.InsertItem(itemIndex,directory);
							totalSpace = (DWORD)(pParInfo->pi.PartitionLength.QuadPart/(1024*1024)) & 0xFFFFFFFF;
							driveVolume.Format("%d",totalSpace);
							driveVolume = driveVolume + " MB";
							m_removableDriverList.SetItemText(itemIndex,1,(LPCTSTR)driveVolume);
 
							switch((int)(pParInfo->pi.PartitionType))
							{
							case 4: 
							case 6:
							case 14:
							case 20:
							case 1:
							case 17:
							case 11:
							case 12:
							case 18: partType = "FAT";break;
							case 7: partType = "NTFS";break;
							case 131: partType = "Linux";break;
							case 130: partType = "LinuxSwp";break;
							default :partType = "Unknown";break;
							}

							m_removableDriverList.SetItemText(itemIndex,2,(LPCTSTR)partType);
							DWORD dwStartSector = pParInfo->pi.StartingOffset.QuadPart/pDriveParam->SectorsSize;
							ReadSector(dwStartSector,1,pBuf,pParInfo->nHardDiskNum,pDriveParam);
							BYTE btTemp = 31;
							nEncryptType = (((PYGCRYDISK_BOOTSECTOR)pBuf)->uEncrytTypeAndKeyLen) >> 5;
							nKeyLen = (((PYGCRYDISK_BOOTSECTOR)pBuf)->uEncrytTypeAndKeyLen) & btTemp;							 

							if ((((PYGCRYDISK_BOOTSECTOR)pBuf)->uCheckSumAndEncryptMark & bit) &&
								(nEncryptType >= 1 && nEncryptType <= 5) &&
								(nKeyLen > 1 && nKeyLen <= 31) &&
								(((PYGCRYDISK_BOOTSECTOR)pBuf)->uCheckSumAndEncryptMark != 170) &&
								(((PYGCRYDISK_BOOTSECTOR)pBuf)->uEncrytTypeAndKeyLen != 85))
							{
								m_removableDriverList.SetItemText(itemIndex,3,"Encrypted !");
							}
							else
							{
								m_removableDriverList.SetItemText(itemIndex,3,"Not Encrypt !");
							}

							itemIndex++;
						}
					}
				}

				free(pDevDesc);
			    CloseHandle(hDevice);
			}
		}

		si += 4;
	}
    
	free(pBuf);
	free(pParInfo);
	free(pDriveParam);
	free(Dstr);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


