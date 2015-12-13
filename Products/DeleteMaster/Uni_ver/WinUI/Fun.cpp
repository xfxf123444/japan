#include "stdafx.h"
#include "DeleteMaster.h"
#include "Fun.h"
#include <WinError.h>

extern	CImageList			g_imgList_BOX,g_imgList_LIST;
extern  YG_PARTITION_INFO*	g_pFixDiskInfo;
extern  BYTE				g_btSecBuf[BYTEINSEC];
extern	int					g_nDelMothed;
extern  int					g_nWipeMothed;
CString						g_ModuleNumber;
CString						g_FirmWare;
CString						g_SerNumber;
CString						g_Capacity;
#define MAX_DELETE_TRY 10
#define DFP_GET_VERSION 0x00074080 
#define DFP_SEND_DRIVE_COMMAND 0x0007c084 
#define DFP_RECEIVE_DRIVE_DATA 0x0007c088 
BOOL GetLogfontFromResources( LPTSTR resourceName, LOGFONT *logfont )
{
	HRSRC		hrsrc;
	HGLOBAL		hglobal;
	LPVOID		rdata;

	hrsrc = FindResource(NULL, resourceName, RT_RCDATA);
	if (!hrsrc)
		return FALSE;
	hglobal = LoadResource(NULL, hrsrc);
	if (!hglobal)
		return FALSE;
	rdata = LockResource(hglobal);
	if (!rdata)
		return FALSE;
	memcpy(logfont, rdata, sizeof(LOGFONT));
	return TRUE;
}

void AddFixDisk(CComboBoxEx* pBox,BOOL bAddFloppy)
{
	COMBOBOXEXITEM		cbItem;
	BIOS_DRIVE_PARAM	DriveParam;
	CString				csText;
	TCHAR				szFloppy[32];
	BYTE				i,btAddItem = 0;
	DWORD				dwTotolDisk;
	DWORD				dwDiskNum;
	DEVICEPARAMS		dp;
	int					nType;
	DWORD				dwResult;

	cbItem.mask			  = CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_TEXT;
	cbItem.iItem		  = 0;
	cbItem.cchTextMax	  = MAX_PATH;
	cbItem.iImage		  = 0;
	cbItem.iSelectedImage = 0;

	_tcscpy(szFloppy,_T("A:\\"));
	memset (&dp, 0, sizeof(dp));
	if (bAddFloppy)
	{
		for(i=0;i<26;i++)
		{
			nType = GetDriveType(szFloppy);
			if(nType == DRIVE_REMOVABLE)
			{
				dwResult = GetFloppyFormFactor(i+1);
				if(dwResult == 350)
				{
					csText.Format(IDS_FD_LIST_FORMAT,szFloppy[0]);
					cbItem.pszText	= csText.GetBuffer (0);
					cbItem.iItem	= btAddItem;
					pBox->InsertItem(&cbItem);
					pBox->SetItemData(cbItem.iItem,szFloppy[0]);
					btAddItem++;
				}
			}
			szFloppy[0]++;
		}
	}

	dwDiskNum = GetHardDiskNum();
	for(i = 1; i <= dwDiskNum ; i ++)
	{
		if(!GetDriveParam(DISK_BASE+(i-1),&DriveParam)) {
			continue;
		}
		btAddItem ++ ;
		dwTotolDisk = DWORD(DriveParam.dwSectors/2048);
		csText.Format(IDS_DISK_LIST_FORMAT,i,dwTotolDisk);
		cbItem.pszText	= csText.GetBuffer (0);
		cbItem.iItem	= btAddItem-1;
		pBox->InsertItem(&cbItem);
		pBox->SetItemData(cbItem.iItem,i-1);
	}
}

void AddDelMothed(CComboBoxEx* pBox)
{
	COMBOBOXEXITEM		cbItem;
	BYTE				i;
	CString				csMothed;

	cbItem.mask			  = CBEIF_TEXT;
	cbItem.iItem		  = 0;
	cbItem.cchTextMax	  = MAX_PATH;
	cbItem.iImage		  = 0;
	cbItem.iSelectedImage = 0;

	for(i = 0; i < 8 ; i ++)
	{
		switch(i)
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
		cbItem.pszText		  = csMothed.GetBuffer(0);
		cbItem.iItem  = i;
		pBox->InsertItem(&cbItem);
		pBox->SetItemData(cbItem.iItem,i);
	}
	pBox->SetCurSel(3);
	g_nDelMothed = pBox->GetItemData(3);
	g_nWipeMothed = pBox->GetItemData(3);
}

void InitList(HWND hWnd, CRect rCreate)
{
	CRect	ListRect;
	int		nWidth;
	CString cstr;

	CListCtrl*	pWnd =(CListCtrl*) CListCtrl::FromHandle(hWnd);

	pWnd->SetExtendedStyle(LVS_EX_FULLROWSELECT);
	::SetWindowLong(pWnd->m_hWnd,GWL_EXSTYLE,WS_EX_CLIENTEDGE);
	pWnd->SetWindowPos((CWnd*)HWND_TOP,rCreate.left,rCreate.top,rCreate.Width()-1,rCreate.Height()-1,SWP_SHOWWINDOW);

	pWnd->GetClientRect(ListRect);
	nWidth = ListRect.right-ListRect.left;

	cstr.LoadString (IDS_VOLUME_LABEL);
	pWnd->InsertColumn(0,cstr,LVCFMT_CENTER,nWidth/5);
	cstr.LoadString (IDS_TYPE);
	pWnd->InsertColumn(1,cstr,LVCFMT_CENTER,nWidth/8);
	cstr.LoadString (IDS_SIZE);
	pWnd->InsertColumn(2,cstr,LVCFMT_CENTER,nWidth/8);
	cstr.LoadString (IDS_USED);
	pWnd->InsertColumn(3,cstr,LVCFMT_CENTER,nWidth/8);
	cstr.LoadString (IDS_UNUSED);
	pWnd->InsertColumn(4,cstr,LVCFMT_CENTER,nWidth/8);
	cstr.LoadString (IDS_STATUS);
	pWnd->InsertColumn(5,cstr,LVCFMT_CENTER,nWidth/8);
	cstr.LoadString (IDS_ORI_LOG);
	pWnd->InsertColumn(6,cstr,LVCFMT_CENTER,nWidth/6);

	pWnd->SetImageList(&g_imgList_LIST,LVSIL_SMALL   );

	return;
}

void ResizeListColumn(HWND hWnd)
{
	CRect	ListRect;
	int		nWidth;

	CListCtrl*	pWnd =(CListCtrl*) CListCtrl::FromHandle(hWnd);

	pWnd->GetClientRect(ListRect);
	nWidth = ListRect.right-ListRect.left;

	pWnd->SetColumnWidth(0,nWidth/5);
	pWnd->SetColumnWidth(1,nWidth/8);
	pWnd->SetColumnWidth(2,nWidth/8);
	pWnd->SetColumnWidth(3,nWidth/8);
	pWnd->SetColumnWidth(4,nWidth/8);
	pWnd->SetColumnWidth(5,nWidth/8);
	pWnd->SetColumnWidth(6,nWidth/6);
	return;
}

BOOL GetVolumeSpace( TCHAR chDrive,DWORD *pdwCount,DWORD *pdwUsed )
{
	BOOL	bResult;
    DWORD	dwSectPerClust,dwBytesPerSect,dwFreeClusters,dwTotalClusters;
    P_GDFSE pGetDiskFreeSpaceEx = NULL;
    unsigned __int64 i64Count,i64Free;
	TCHAR szDrive[]= _T("*:\\");
    
	szDrive[0] = chDrive;
    pGetDiskFreeSpaceEx = (P_GDFSE)GetProcAddress (GetModuleHandle (_T("kernel32.dll")),
                                                   "GetDiskFreeSpaceEx");
    if (pGetDiskFreeSpaceEx)
    {
        bResult = pGetDiskFreeSpaceEx (szDrive, NULL,
                                      (PULARGE_INTEGER)&i64Count,
                                      (PULARGE_INTEGER)&i64Free);
		if(pdwCount != NULL)
			*pdwCount = (DWORD)(i64Count / BYTEINSEC);
		if(pdwUsed != NULL)
			*pdwUsed = (DWORD)(i64Count / BYTEINSEC) - (DWORD)(i64Free / BYTEINSEC);
    }
    else
    {
         bResult = GetDiskFreeSpace (szDrive, 
                                     &dwSectPerClust,
                                     &dwBytesPerSect, 
                                     &dwFreeClusters,
                                     &dwTotalClusters);
         if (bResult)
         {
			if(pdwCount != NULL)
				*pdwCount = dwTotalClusters * dwSectPerClust;
			if(pdwUsed != NULL)
				*pdwUsed = dwTotalClusters * dwSectPerClust - (dwFreeClusters)* dwSectPerClust;
		 }
     }
     return bResult;
}

void SortParInfo(PARINFOONHARDDISKEX *ParInfo)
{
	int		m,n;
	PARINFOONHARDDISKEX DiskIfTemp;

	//sort Primary item
	for(m=0;m<ParInfo->wNumOfPri;m++)
	{
		//if(ParInfo->pePriParInfo[m].SystemFlag == 0)
		//	break;
		for(n=m;n<ParInfo->wNumOfPri;n++)
		{
			//if(ParInfo->pePriParInfo[n].SystemFlag == 0)
			//	break;
			if(ParInfo->pePriParInfo[m].StartSector
				> ParInfo->pePriParInfo[n].StartSector)
			{
				DiskIfTemp.pePriParInfo[m]   = ParInfo->pePriParInfo[m];
				ParInfo->pePriParInfo[m]	 = ParInfo->pePriParInfo[n];
				ParInfo->pePriParInfo[n]	 = DiskIfTemp.pePriParInfo[m];
			}
		}
	}
	//sort logical partition
	for(m=0;m<ParInfo->wNumOfLogic;m++)
	{
		//if(ParInfo->peLogParInfo[m].peCurParInfo.SystemFlag == 0)
		//	break;
		for(n=m;n<ParInfo->wNumOfLogic;n++)
		{
			//if(ParInfo->peLogParInfo[n].peCurParInfo.SystemFlag == 0)
			//	break;
			if(ParInfo->peLogParInfo[m].peCurParInfo.StartSector
				> ParInfo->peLogParInfo[n].peCurParInfo.StartSector)
			{
				DiskIfTemp.peLogParInfo[m]	= ParInfo->peLogParInfo[m];
				ParInfo->peLogParInfo[m]	= ParInfo->peLogParInfo[n];
				ParInfo->peLogParInfo[n]	= DiskIfTemp.peLogParInfo[m];
			}
					
		}
	}
}

BOOL GetFixDiskInfo(int nDisk)
{
	PARINFOONHARDDISKEX info;
	WORD				j,n;
	YG_PARTITION_INFO	*pNew, *pEnd;
	BIOS_DRIVE_PARAM	DriveParam;
	DWORD				dwPartitionTop;
	DWORD				dwLogPartitionTop;
	PARTITION_INFO		pInfo;

	if(!GetDriveParam(nDisk,&DriveParam)) {
		return FALSE;
	}
	if(!GetPartitionInfoEx(nDisk, &info)) {
		return FALSE;
	}
	SortParInfo(&info);
	//Get all partition items
	j = 0;
	dwPartitionTop = DISK_MIN_SECTOR;
	//while(info.pePriParInfo[j].SystemFlag != 0x00 && j < info.wNumOfPri)
	while(j < info.wNumOfPri)
	{
		if(info.pePriParInfo[j].StartSector > dwPartitionTop)
		{
			if(info.pePriParInfo[j].StartSector - dwPartitionTop >= (DriveParam.dwHeads * DriveParam.dwSecPerTrack) - DISK_MIN_SECTOR)
			{
				pNew = (YG_PARTITION_INFO*)malloc(sizeof(YG_PARTITION_INFO));
				memset(pNew,0,sizeof(YG_PARTITION_INFO));
				pNew->PartitionStyle = PARTITION_STYLE_MBR;
				pNew->BootFlag		 = 0;
				pNew->bLogic		 = FALSE;
				pNew->btDiskNum		 = (BYTE)nDisk-DISK_BASE;
				pNew->dwPartSize	 = info.pePriParInfo[j].StartSector - dwPartitionTop;
				pNew->dwStartSector  = dwPartitionTop;
				pNew->dwSystemFlag	 = 0;
				pNew->pNext			 = NULL;
				pNew->dwUsedSize	 = 0;
				pNew->DriveLetter	 = _T('*');
				if(g_pFixDiskInfo == NULL) g_pFixDiskInfo = pNew;
				else
				{
					pEnd = g_pFixDiskInfo;
					while(pEnd->pNext != NULL) pEnd = pEnd->pNext;
					pEnd->pNext = pNew;
				}
			}
			dwPartitionTop = info.pePriParInfo[j].StartSector;
		}
		dwPartitionTop += info.pePriParInfo[j].SectorsInPartition; 
		if(info.pePriParInfo[j].SystemFlag == 0x05 || info.pePriParInfo[j].SystemFlag == 0x0f)
		{
			n = 0;
			dwLogPartitionTop = info.pePriParInfo[j].StartSector;
			//while(info.peLogParInfo[n].peCurParInfo.SystemFlag != 0x00 && n< info.wNumOfLogic)
			while (n < info.wNumOfLogic)
			{
				if(info.peLogParInfo[n].peCurParInfo.StartSector > dwLogPartitionTop)
				{
					pNew = (YG_PARTITION_INFO*)malloc(sizeof(YG_PARTITION_INFO));
					memset(pNew,0,sizeof(YG_PARTITION_INFO));
					pNew->BootFlag		= 0;
					pNew->bLogic		= TRUE;
					pNew->btDiskNum		= (BYTE)nDisk-DISK_BASE;
					pNew->dwPartSize	= info.peLogParInfo[n].peCurParInfo.StartSector - 
											dwLogPartitionTop;
					pNew->dwStartSector = dwLogPartitionTop;
					pNew->dwSystemFlag  = 0;
					pNew->pNext			= NULL;
					pNew->dwUsedSize	= 0;
					pNew->DriveLetter	= _T('*');
					if(g_pFixDiskInfo == NULL) g_pFixDiskInfo = pNew;
					else
					{
						pEnd = g_pFixDiskInfo;
						while(pEnd->pNext != NULL) pEnd = pEnd->pNext;
						pEnd->pNext = pNew;
					}

					pNew = (YG_PARTITION_INFO*)malloc(sizeof(YG_PARTITION_INFO));
					memset(pNew,0,sizeof(YG_PARTITION_INFO));
					pNew->BootFlag		= info.peLogParInfo[n].peCurParInfo.BootFlag;
					pNew->bLogic		= TRUE;
					pNew->btDiskNum		= (BYTE)nDisk-DISK_BASE;
					pNew->dwPartSize	= info.peLogParInfo[n].peCurParInfo.SectorsInPartition;
					pNew->dwStartSector = info.peLogParInfo[n].peCurParInfo.StartSector;
					pNew->dwSystemFlag	= info.peLogParInfo[n].peCurParInfo.SystemFlag;
					pNew->pNext			= NULL;
					pNew->DriveLetter	= RetrieveDriveLttr((BYTE)nDisk,LOGICAL,
															info.peLogParInfo[n].peCurParInfo.StartSector);
					if((BYTE)pNew->DriveLetter == 0xFF)
					{
						pNew->DriveLetter = _T('*');
					}
					else
						pNew->DriveLetter += _T('A') - 1;
					if(pNew->DriveLetter != _T('*'))
					{
						if(GetVolumeSpace(pNew->DriveLetter, NULL, &pNew->dwUsedSize)) {
							//pNew->dwUsedSize = pNew->dwPartSize - pNew->dwUsedSize;
						}
						if(!PartitionInfoOfDriveLetter(pNew->DriveLetter-0x40,&pInfo)) {
							memset(&pInfo,0,sizeof(PARTITION_INFO));
						}
						memcpy(pNew->szLabel,pInfo.szLabelName ,MAX_LABELNAME);
						memcpy(pNew->szOsLabel,pInfo.szOsLabel,MAX_LABELNAME);
					}
					if(g_pFixDiskInfo == NULL) g_pFixDiskInfo = pNew;
					else
					{
						pEnd = g_pFixDiskInfo;
						while(pEnd->pNext != NULL) pEnd = pEnd->pNext;
						pEnd->pNext = pNew;
						//pEnd = pEnd->pNext;
					}
						
					dwLogPartitionTop = info.peLogParInfo[n].peCurParInfo.StartSector;
				}
				else
				{
					pNew = (YG_PARTITION_INFO*)malloc(sizeof(YG_PARTITION_INFO));
					memset(pNew,0,sizeof(YG_PARTITION_INFO));
					pNew->BootFlag		= info.peLogParInfo[n].peCurParInfo.BootFlag;
					pNew->bLogic		= TRUE;
					pNew->btDiskNum		= (BYTE)nDisk-DISK_BASE;
					pNew->dwPartSize	= info.peLogParInfo[n].peCurParInfo.SectorsInPartition;
					pNew->dwStartSector = info.peLogParInfo[n].peCurParInfo.StartSector;
					pNew->dwSystemFlag	= info.peLogParInfo[n].peCurParInfo.SystemFlag;
					pNew->pNext = NULL;
					pNew->DriveLetter	= RetrieveDriveLttr((BYTE)nDisk,LOGICAL,
															info.peLogParInfo[n].peCurParInfo.StartSector);
					if((BYTE)pNew->DriveLetter == 0xff)
					{
						pNew->DriveLetter = _T('*');
					}
					else
						pNew->DriveLetter += _T('A') - 1;

					if(pNew->DriveLetter != _T('*'))
					{
						if(GetVolumeSpace(pNew->DriveLetter, NULL, &pNew->dwUsedSize)) {
							//							pNew->dwUsedSize = pNew->dwPartSize - pNew->dwUsedSize;
						}
						if(!PartitionInfoOfDriveLetter(pNew->DriveLetter-0x40,&pInfo)) {
							memset(&pInfo,0,sizeof(PARTITION_INFO));
						}
						memcpy(pNew->szLabel,pInfo.szLabelName ,MAX_LABELNAME);
						memcpy(pNew->szOsLabel,pInfo.szOsLabel,MAX_LABELNAME);
					}
					if(g_pFixDiskInfo == NULL) g_pFixDiskInfo = pNew;
					else
					{
						pEnd = g_pFixDiskInfo;
						while(pEnd->pNext != NULL) pEnd = pEnd->pNext;
						pEnd->pNext = pNew;
						//pEnd = pEnd->pNext;
					}
				}
				dwLogPartitionTop += info.peLogParInfo[n].peCurParInfo.SectorsInPartition+DISK_MIN_SECTOR; 
				n++;
			}
			if(dwLogPartitionTop != (info.pePriParInfo[j].SectorsInPartition+
				info.pePriParInfo[j].StartSector))
			{
				pNew = (YG_PARTITION_INFO*)malloc(sizeof(YG_PARTITION_INFO));
				memset(pNew,0,sizeof(YG_PARTITION_INFO));
				pNew->BootFlag		= 0;
				pNew->bLogic		= TRUE;
				pNew->btDiskNum		= (BYTE)nDisk-DISK_BASE;
				pNew->dwPartSize	= info.pePriParInfo[j].SectorsInPartition+
									  info.pePriParInfo[j].StartSector-dwLogPartitionTop;
				pNew->dwStartSector = dwLogPartitionTop;
				pNew->dwSystemFlag	= 0;
				pNew->pNext			= NULL;
				pNew->dwUsedSize	= 0;
				pNew->DriveLetter	= _T('*');
				if(g_pFixDiskInfo == NULL) g_pFixDiskInfo = pNew;
				else
				{
					pEnd = g_pFixDiskInfo;
					while(pEnd->pNext != NULL) pEnd = pEnd->pNext;
					pEnd->pNext = pNew;
					//pEnd = pEnd->pNext;
				}
			}
		}
		else
		{
			pNew = (YG_PARTITION_INFO*)malloc(sizeof(YG_PARTITION_INFO));
			memset(pNew,0,sizeof(YG_PARTITION_INFO));
			pNew->BootFlag		= info.pePriParInfo[j].BootFlag;
			pNew->bLogic		= FALSE;
			pNew->btDiskNum		= (BYTE)nDisk-DISK_BASE;
			pNew->dwPartSize	= info.pePriParInfo[j].SectorsInPartition;
			pNew->dwStartSector = info.pePriParInfo[j].StartSector;
			pNew->dwSystemFlag	= info.pePriParInfo[j].SystemFlag;
			pNew->pNext			= NULL;
			pNew->DriveLetter	= RetrieveDriveLttr((BYTE)nDisk,PRIMARY,
													info.pePriParInfo[j].StartSector);
			if((BYTE)pNew->DriveLetter == 0xFF)
			{
				pNew->DriveLetter = _T('*');
			}
			else
				pNew->DriveLetter += _T('A') - 1;
			if(pNew->DriveLetter != _T('*'))
			{
				if(GetVolumeSpace(pNew->DriveLetter, NULL, &pNew->dwUsedSize)) {
					//					pNew->dwUsedSize = pNew->dwPartSize - pNew->dwUsedSize;
				}
				if(!PartitionInfoOfDriveLetter(pNew->DriveLetter-0x40,&pInfo)) {
					memset(&pInfo,0,sizeof(PARTITION_INFO));
				}
				memcpy(pNew->szLabel,pInfo.szLabelName ,MAX_LABELNAME);
				memcpy(pNew->szOsLabel,pInfo.szOsLabel,MAX_LABELNAME);
			}
			if(g_pFixDiskInfo == NULL) g_pFixDiskInfo = pNew;
			else
			{
				pEnd = g_pFixDiskInfo;
				while(pEnd->pNext != NULL) pEnd = pEnd->pNext;
				pEnd->pNext = pNew;
				//pEnd = pEnd->pNext;
			}
		}
		j++;
	}
	if(dwPartitionTop < DriveParam.dwSectors)
	{
		pNew = (YG_PARTITION_INFO*)malloc(sizeof(YG_PARTITION_INFO));
		memset(pNew,0,sizeof(YG_PARTITION_INFO));
		pNew->BootFlag		= 0;
		pNew->bLogic		= FALSE;
		pNew->btDiskNum		= (BYTE)nDisk-DISK_BASE;
		pNew->dwPartSize	= (DWORD)DriveParam.dwSectors-dwPartitionTop;
		pNew->dwStartSector = dwPartitionTop;
		pNew->dwSystemFlag	= 0;
		pNew->pNext			= NULL;
		pNew->dwUsedSize	= 0;
		pNew->DriveLetter	= _T('*');
		if(g_pFixDiskInfo == NULL) g_pFixDiskInfo = pNew;
		else
		{
			pEnd = g_pFixDiskInfo;
			while(pEnd->pNext != NULL) pEnd = pEnd->pNext;
				pEnd->pNext = pNew;
			//pEnd = pEnd->pNext;
		}
	}
	return TRUE;
}

BOOL   FreePartList( PYG_PARTITION_INFO  pPartInfo)
{
	PYG_PARTITION_INFO  pCur,pNext ;
	
    if ( NULL == pPartInfo)
	{
		return TRUE;
	}
    pCur = pPartInfo ;

	while(pCur)
	{
		pNext = pCur->pNext ;
		free(pCur);
		pCur = pNext;
	}
	return TRUE ;
}

void AddList(CListCtrl* pList, int nSelDisk)
{
	int					i = 0;
	CString				strParLetter;
	TCHAR				szType[20],
						szStatus[10];
	CString				csSize;
	TCHAR				szPriLog[8];
	CString				cstr;
	YG_PARTITION_INFO	*pEnd;

	FreePartList(g_pFixDiskInfo);
	g_pFixDiskInfo = NULL;
	GetFixDiskInfo(nSelDisk);
	pEnd = g_pFixDiskInfo;
	while(pEnd != NULL)
	{
		if(!pEnd->dwSystemFlag)
		{
			pEnd = pEnd->pNext;
			continue;
		}
		else
		{
			if(pEnd->dwSystemFlag == 0x05 || pEnd->dwSystemFlag == 0x0f)
			{
				pEnd = pEnd->pNext ;
				continue;
			}
		}

		if(pEnd->DriveLetter == _T('*'))
		{
			strParLetter = _T("*");
			strParLetter += _T(":");
		}
		else
		{
			strParLetter = pEnd->DriveLetter;
			strParLetter += _T(":");
			TCHAR temp[MAX_PATH];
			ZeroMemory(temp, MAX_PATH * sizeof(TCHAR));
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pEnd->szLabel, strlen(pEnd->szLabel), temp, MAX_PATH);
			strParLetter += temp;
		}
		switch(pEnd->dwSystemFlag)
		{
		case X_HIDE_FAT32:
		case X_HIDE_FAT32_OUT:
			cstr.LoadString(IDS_HIDE_FAT32);
			_tcscpy(szType,cstr);
			break;
		case X_FAT32: 
		case X_FAT32_OUT:
			cstr.LoadString(IDS_FAT32);
			_tcscpy(szType,cstr);
			break;
		case X_FAT16:
		case X_FAT16_1:
		case X_FAT16_4:
		case X_FAT16_E:
			cstr.LoadString (IDS_FAT);
			_tcscpy(szType,cstr);
			break;
		case X_HIDE_FAT16:
		case X_HIDE_FAT16_1:
		case X_HIDE_FAT16_4:
		case X_HIDE_FAT16_E:
			cstr.LoadString (IDS_HIDE_FAT);
			_tcscpy(szType,cstr);
			break;
		case X_NTFS:
			cstr.LoadString (IDS_NTFS);
			_tcscpy(szType,cstr);
			break;
		case X_NTFS_HIDE:
			cstr.LoadString (IDS_HIDE_NTFS);
			_tcscpy(szType,cstr);
			break;
		case X_LINX_EXT:
			cstr.LoadString (IDS_LUX_EXT2);
			_tcscpy(szType,cstr);
			break;
		case X_LINX_SWAP:
			cstr.LoadString (IDS_LUX_SWP);
			_tcscpy(szType,cstr);
			break;
		case 0x00:
			cstr.LoadString (IDS_FREE);
			_tcscpy(szType,cstr);
			break;
		default:
			//_tcscpy(szType, "Unknow");
			_stprintf(szType,_T("Sys ID (%X)H"),pEnd->dwSystemFlag);
			break;
		}

		if(pEnd->BootFlag || pEnd->GUIDType == PARTITION_SYSTEM_GUID)
		{
			cstr.LoadString (IDS_LIST_ACTIVE);
			_tcscpy(szStatus, cstr);
		}
		else
		{
			cstr.LoadString (IDS_LIST_INACTIVE);
			_tcscpy(szStatus, cstr);
		}

		if(pEnd->bLogic)
		{
			cstr.LoadString (IDS_LIST_LOG);
			_tcscpy(szPriLog, cstr);
		}
		else
		{
			cstr.LoadString (IDS_LIST_PRI);
			_tcscpy(szPriLog, cstr);
		}

		pList->InsertItem(i, strParLetter, 0);//volume
		pList->SetItemData(i, i);
		pList->SetItemText(i, 1, szType);//type

		csSize.Format(_T("%u"), pEnd->dwPartSize / 2048);
		pList->SetItemText(i, 2, csSize);//count size

		if( pEnd->dwUsedSize )
		{
			csSize.Format(_T("%u"), pEnd->dwUsedSize / 2048);
			pList->SetItemText(i, 3, csSize);//used size

			csSize.Format(_T("%u"), (pEnd->dwPartSize - pEnd->dwUsedSize) / 2048);
			pList->SetItemText(i, 4, csSize);//free size
		}
		else
		{
			cstr.LoadString (IDS_UNKNOW);
			pList->SetItemText(i, 3, cstr);
			pList->SetItemText(i, 4, cstr);
		}
		
		pList->SetItemData(i,(DWORD)pEnd);
		pList->SetItemText(i, 5, szStatus);//status
		pList->SetItemText(i, 6, szPriLog);//other

		i ++;
		pEnd = pEnd->pNext;
  }
	return;
}

BOOL IsSystemDisk()
{
	TCHAR				szSysDir[MAX_PATH];
	YG_PARTITION_INFO	*pCurParInfo;

	pCurParInfo = g_pFixDiskInfo;
	GetSystemDirectory(szSysDir,MAX_PATH);
	while(pCurParInfo != NULL)
	{
		if(pCurParInfo->DriveLetter == szSysDir[0]) return TRUE;
		pCurParInfo = pCurParInfo->pNext ;
	}
	return FALSE;
}

BOOL ShowSectorInfo(CWnd* pWnd)
{
	//int			i;
	//int			nBufOffset = 0;
	//char		szitem[80];
	//CPaintDC	dc(pWnd);

 //	for(i=0;i<1;i++)
 //	{
	//	sprintf(szitem, SECTORINFO, nBufOffset,
	//			g_btSecBuf[nBufOffset],g_btSecBuf[nBufOffset+1],
 //				g_btSecBuf[nBufOffset+2],g_btSecBuf[nBufOffset+3],
 //				g_btSecBuf[nBufOffset+4],g_btSecBuf[nBufOffset+5],
 //				g_btSecBuf[nBufOffset+6],g_btSecBuf[nBufOffset+7],
 //				g_btSecBuf[nBufOffset+8],g_btSecBuf[nBufOffset+9],
 //				g_btSecBuf[nBufOffset+10],g_btSecBuf[nBufOffset+11],
 //				g_btSecBuf[nBufOffset+12],g_btSecBuf[nBufOffset+13],
 //				g_btSecBuf[nBufOffset+14],g_btSecBuf[nBufOffset+15]);
 //		/*for(j=0;j<16;j++)
 //		{
 //			szch = g_btSecBuf[nBufOffset+j];
 //			if(	
 //				
 //				szch == 0x0a || 
 //				szch == 0x0d
 //				
	//		  )
 //			{
 //				//szch = szch;
 //				szch = 0x00;
 //			}
 //			if(szch == 0) szch = 0x2e;
 //			_stprintf(szitem,"%s%c",szitem,szch);
 //		}
	//	::MessageBox(NULL,szitem,"",MB_OK);*/
 //		dc.TextOut (0,i*12,_"Sector information"));
 //		nBufOffset += 16;
 //	}
	//return		TRUE;
	return false;
}

BOOL InitDelBuf(BYTE *pBuf,BYTE btFir,BYTE btSec,BYTE btThr,BOOL bRand,BOOL bOneByte)
{
	BYTE *pOneBuf;
	int	 i,nRemain;

	if(bRand)
	{
		srand( (unsigned)time( NULL ) );
   		btFir = (BYTE)rand();
	}
	if(bOneByte)
	{
		memset(pBuf,btFir,DATA_BUFFER_SIZE);
	}
	else
	{
		pOneBuf = (BYTE*)malloc(BYTEINSEC);
		if(!pOneBuf) return FALSE;
		for(i=0;i<BYTEINSEC;i++)
		{
			nRemain = i % 3;
			switch(nRemain)
			{
			case 0:
				*(pOneBuf+i) = btFir;
				break;
			case 1:
				*(pOneBuf+i) = btSec;
				break;
			case 2:
				*(pOneBuf+i) = btThr;
				break;
			}
		}
		for(i=0;i<100;i++)
		{
			memcpy(pBuf+BYTEINSEC*i,pOneBuf,BYTEINSEC);
		}
		free(pOneBuf);
	}
	return TRUE;
}

DWORD GetFloppyFormFactor(int iDrive)
{
    HANDLE	hDevice;
    TCHAR	tsz[32];
    DWORD	dwResult = 0;

    //if ((int)GetVersion() < 0)
    //{   // Windows 95

    //    hDevice = CreateFileA("\\\\.\\VWIN32", 0, 0, 0, 0,
    //                    FILE_FLAG_DELETE_ON_CLOSE, 0);
    //    if (hDevice != INVALID_HANDLE_VALUE)
    //    {
    //         DWORD          cb;
    //         DIOC_REGISTERS reg;
    //         DOSDPB         dpb;

    //         dpb.specialFunc = 0;  // return default type; do not hit disk

    //         reg.reg_EBX   = iDrive;       // BL = drive number (1-based)
    //         reg.reg_EDX   = (DWORD)&dpb;  // DS:EDX -> DPB
    //         reg.reg_ECX   = 0x0860;       // CX = Get DPB
    //         reg.reg_EAX   = 0x440D;       // AX = Ioctl
    //         reg.reg_Flags = 0x01;   // assume failure

    //         // Make sure both DeviceIoControl and Int 21h succeeded.
    //         if (DeviceIoControl (hDevice, VWIN32_DIOC_DOS_IOCTL, &reg,
    //                              sizeof(reg), &reg, sizeof(reg),
    //                              &cb, 0)
    //             && !(reg.reg_Flags & 0x01))
    //         {
    //            switch (dpb.devType)
    //            {
    //            case 2: // 3.5  720K floppy
    //            case 7: // 3.5  1.44MB floppy
    //            case 9: // 3.5  2.88MB floppy
    //               dwResult = 350; break;
				//case 5: // hard drive
				//	dwResult = 1;
				//	break;
    //            default: // other
    //               break;
    //            }
    //         }
    //         CloseHandle(hDevice);
    //      }
    //  }
    //  else
    //  {
         //On Windows NT, use the technique described in the Knowledge
         //Base article Q115828 and in the "FLOPPY" SDK sample.

		 _stprintf(tsz, TEXT("\\\\.\\%c:"), TEXT('@') + iDrive);
		 //_tcscpy(tsz,"\\\\.\\PhysicalDrive0");
         hDevice = CreateFile(tsz, 0, FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
         if (hDevice != INVALID_HANDLE_VALUE)
         {
            DISK_GEOMETRY Geom[20];
            DWORD cb;

            if (DeviceIoControl (hDevice, IOCTL_STORAGE_GET_MEDIA_TYPES, 0, 0,
                                 Geom, sizeof(Geom), &cb, 0)
                && cb > 0)
            {
               switch (Geom[0].MediaType)
               {
               case F3_1Pt44_512: // 3.5 1.44MB floppy
               case F3_2Pt88_512: // 3.5 2.88MB floppy
               case F3_20Pt8_512: // 3.5 20.8MB floppy
               case F3_720_512:   // 3.5 720K   floppy
                  dwResult = 350;
                  break;
               default:
				  break;	
               }
            }
			//int	nErr = GetLastError();
            CloseHandle(hDevice);
         }
//    }
	return dwResult;
}

BOOL GetFDParam(BYTE btFDLetter,BIOS_DRIVE_PARAM *pParam)
{
	HANDLE				hDevice;
	BOOL				bResult = FALSE;
	TCHAR				szDriveName[MAX_PATH];
	DISK_GEOMETRY		dgDrive;
	DWORD				dwBytesReturned = 0;

	_stprintf(szDriveName,_T("\\\\.\\%c:"),btFDLetter);
	//open handle in rw mode
	hDevice = CreateFile(szDriveName,GENERIC_READ|GENERIC_WRITE,
						FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,OPEN_EXISTING,	0, NULL);
	if( hDevice != INVALID_HANDLE_VALUE )
	{
		bResult = DeviceIoControl( hDevice,
								IOCTL_DISK_GET_DRIVE_GEOMETRY,
								NULL,0,
								&dgDrive,sizeof(DISK_GEOMETRY),
								&dwBytesReturned,
								NULL);
		if(bResult)
		{
			pParam->wInfoSize = sizeof(BIOS_DRIVE_PARAM);
			pParam->dwHeads = dgDrive.TracksPerCylinder;
			pParam->dwCylinders = (DWORD)dgDrive.Cylinders.QuadPart;
			pParam->dwSecPerTrack = dgDrive.SectorsPerTrack;
			pParam->SectorsSize = (WORD)dgDrive.BytesPerSector;
			pParam->dwSectors = dgDrive.Cylinders.QuadPart
										* dgDrive.TracksPerCylinder
										* dgDrive.SectorsPerTrack;
		}
		//close handle
		CloseHandle(hDevice);
	}
	return bResult;
}

BOOL WriteFDSector( DWORD					dwStartSec,
					WORD					wSectors,
					PBYTE					pBuf,
					BYTE					btUnit,
					PBIOS_DRIVE_PARAM		pDriveParam)
{
	BOOL					bResult = FALSE;
	DWORD					dwBytesWriten = 0;
	DWORD					dwBytesToWrite;
	HANDLE					hDrv;
	LARGE_INTEGER			lnStartBytes;
	TCHAR					szDriveName[MAX_PATH];

	dwBytesToWrite = wSectors * pDriveParam->SectorsSize;
	lnStartBytes.QuadPart = (__int64)dwStartSec * pDriveParam->SectorsSize;

	if(lnStartBytes.QuadPart >= 0)
	{
		_stprintf(szDriveName,_T("\\\\.\\%c:"),btUnit);
		hDrv = CreateFile(szDriveName,GENERIC_READ|GENERIC_WRITE,
						FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,OPEN_EXISTING,	0, NULL);
		ASSERT( hDrv != INVALID_HANDLE_VALUE);
		if(hDrv != INVALID_HANDLE_VALUE)
		{
			SetFilePointer(hDrv,lnStartBytes.LowPart,&lnStartBytes.HighPart,FILE_BEGIN);
			bResult = WriteFile(hDrv,pBuf,dwBytesToWrite,&dwBytesWriten,NULL);
			bResult = bResult ? dwBytesToWrite == dwBytesWriten : FALSE;
			dwBytesWriten = GetLastError();
			//close handle
			CloseHandle(hDrv);
		}
	}
	return bResult;
}

BOOL ReadFDSector( DWORD					dwStartSec,
					WORD					wSectors,
					PBYTE					pBuf,
					BYTE					btUnit,
					PBIOS_DRIVE_PARAM		pDriveParam)
{
	BOOL					bResult = FALSE;
	DWORD					dwBytesWriten = 0;
	DWORD					dwBytesToWrite;
	HANDLE					hDrv;
	LARGE_INTEGER			lnStartBytes;
	char					szDriveName[MAX_PATH];

	dwBytesToWrite = wSectors * pDriveParam->SectorsSize;
	lnStartBytes.QuadPart = (__int64)dwStartSec * pDriveParam->SectorsSize;

	if(lnStartBytes.QuadPart >= 0)
	{
		sprintf(szDriveName,"\\\\.\\%c:",btUnit);
		hDrv = CreateFileA(szDriveName,GENERIC_READ|GENERIC_WRITE,
						FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,OPEN_EXISTING,	0, NULL);
		ASSERT( hDrv != INVALID_HANDLE_VALUE);
		if(hDrv != INVALID_HANDLE_VALUE)
		{
			SetFilePointer(hDrv,lnStartBytes.LowPart,&lnStartBytes.HighPart,FILE_BEGIN);
			bResult = ReadFile(hDrv,pBuf,dwBytesToWrite,&dwBytesWriten,NULL);
			bResult = bResult ? dwBytesToWrite == dwBytesWriten : FALSE;
			dwBytesWriten = GetLastError();
			//close handle
			CloseHandle(hDrv);
		}
	}
	return bResult;
}

BOOL	GetRemovableDiskSize(int nDisk, LPTSTR pszDrv,DWORD *pdwSize)
{
	PARINFOONHARDDISKEX info;
	if(!GetPartitionInfoEx(nDisk, &info)) return FALSE;
	*pdwSize = info.pePriParInfo[0].SectorsInPartition;
	if(pszDrv)
	{
		*pszDrv = RetrieveDriveLttr(nDisk,PRIMARY,info.pePriParInfo[0].StartSector);
		*pszDrv += _T('A') - 1;
	}
	return TRUE;
}

BOOL HasMedia(TCHAR szDrive)
{
	TCHAR	szCurrentDir[MAX_PATH];
	TCHAR	szFDrive[32];

	_tcscpy(szFDrive,_T("A:\\"));
	szFDrive[0] = szDrive;
	GetCurrentDirectory(MAX_PATH,szCurrentDir);

	if(SetCurrentDirectory(szFDrive))
	{
		SetCurrentDirectory(szCurrentDir);
		return TRUE;
	}
	return FALSE;
}

BOOL  CreateTmpFileInSou( TCHAR szDriveLetter,int *nFileNum,BOOL *bHaveFile,__int64 *nTotalFileSize)
{
	DWORD             dwTotalSecs, dwFreeSecs,dwUsedSecs;
    TCHAR              TempFileName[MAX_PATH] ;
	HANDLE			  hTempFile ;
	__int64			  nHiFileSize = 0;
	int				  i,nNumTempFile;
	DWORD			  dwFileSize;
	LARGE_INTEGER	  lnFileSize;
	CString			  csComment;
    
	GetVolumeSpace(szDriveLetter ,&dwTotalSecs,&dwUsedSecs); // Get free sector of the partition
	dwFreeSecs = dwTotalSecs - dwUsedSecs;
	*nFileNum	= 0;
	
	BOOL usingUserPath = FALSE;
	CString userPath = GetUserPath();
	if (szDriveLetter == _T('c') || szDriveLetter == _T('C')) {
		usingUserPath = TRUE;
	}
	if ( 0!= dwFreeSecs)
	{		
		nHiFileSize  = (__int64)dwFreeSecs;
		nHiFileSize  = nHiFileSize * SECTORSIZE;
		nNumTempFile = (int)((nHiFileSize+MAX_FILE_SIZE) / MAX_FILE_SIZE);
		*nFileNum = nNumTempFile;
		dwFreeSecs = 0;
		for(i=0;i<nNumTempFile;i++)
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
			if (i)
			{
				_stprintf(TempFileName,_T("%s%d.dat"),TempFileName,i);
			}
			else
			{
				_stprintf(TempFileName,_T("%s.dat"),TempFileName);
			}
			//::MessageBox(NULL,TempFileName,"Will create free space temp file.",MB_OK);
			hTempFile = CreateFile( TempFileName,
				              		GENERIC_READ | GENERIC_WRITE ,
				                    0 , 
				                 	NULL ,
				                    CREATE_ALWAYS,
				                    FILE_ATTRIBUTE_NORMAL|FILE_FLAG_NO_BUFFERING,
				                 	NULL) ;
			if ( INVALID_HANDLE_VALUE == hTempFile)
			{
				DeleteTempFile(szDriveLetter,nNumTempFile);
				return FALSE ;
			}

			if(i==nNumTempFile-1)
			{
				GetVolumeSpace(szDriveLetter , &dwTotalSecs,&dwUsedSecs); // Get free sector of the partition
				dwFreeSecs = dwTotalSecs - dwUsedSecs;
				lnFileSize.QuadPart  = dwFreeSecs * SECTORSIZE ;
			}
			else
			{
				lnFileSize.QuadPart = MAX_FILE_SIZE;
			}
			//lnFileSize.QuadPart = dwOneFileSecs* SECTORSIZE;
			dwFileSize = ::SetFilePointer (hTempFile, lnFileSize.LowPart, &lnFileSize.HighPart, FILE_BEGIN) ; 
				
			if(!SetEndOfFile( hTempFile))
			{
				lnFileSize.QuadPart -= MIN_FILE_SECTORS * SECTORSIZE ;
				dwFileSize = SetFilePointer( hTempFile ,
			           			lnFileSize.LowPart,
			            		NULL ,
								FILE_BEGIN );
				while(!SetEndOfFile( hTempFile))
				{
					lnFileSize.QuadPart -= MIN_FILE_SECTORS * SECTORSIZE ;
					dwFileSize = SetFilePointer( hTempFile ,
			           				lnFileSize.LowPart,
			            			NULL ,
									FILE_BEGIN );
				}
				*nTotalFileSize += lnFileSize.QuadPart;
				FlushFileBuffers(hTempFile);
			}
			else
				*nTotalFileSize += dwFileSize;
			CloseHandle( hTempFile);
		}
		*bHaveFile  = TRUE;
    }
	else
	{
		*bHaveFile =  FALSE;
	}

	return TRUE ;
}

void	DeleteTempFile(TCHAR szDriveLetter,int nFileNum)
{
	int			nFiles;
    TCHAR        TempFileName[MAX_PATH] ;
	BOOL usingUserPath = FALSE;
	CString userPath = GetUserPath();
	if (szDriveLetter == _T('c') || szDriveLetter == _T('C')) {
		usingUserPath = TRUE;
	}
	for(nFiles=0;nFiles < nFileNum;nFiles++)
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
		
		BOOL ret = false;
		DWORD tryCount = 0;
		while (!ret && tryCount < MAX_DELETE_TRY)
		{
			ret = DeleteFile(TempFileName);
			if (GetLastError() == ERROR_SHARING_VIOLATION)
			{
				++tryCount;
				Sleep(200);
			}
		}
	}
}

VOID ChangeByteOrder(LPTSTR szString, USHORT uscStrSize) 
{ 
	USHORT i; 
	TCHAR temp; 

	for (i = 0; i < uscStrSize; i+=2) 
	{ 
		temp = szString[i]; 
		szString[i] = szString[i+1]; 
		szString[i+1] = temp; 
	} 
} 

VOID HDInfont(BYTE btDisk,HWND hHandle)
{ 
	TCHAR		hd[80]; 
	PIDSECTOR	phdinfo; 
	TCHAR		s[MAX_PATH];
	TCHAR		szMessage[128];
	BOOL		bResult;
	HANDLE		h; 
	DWORD		i; 
	YG_GETVERSIONINPARAMS		vers; 
	YG_SENDCMDINPARAMS			in; 
	YG_SENDCMDOUTPARAMS		out; 
	g_ModuleNumber = _T("");
	g_FirmWare = _T("");
	g_SerNumber = _T("");
	g_Capacity = _T("");
	static BYTE lasterrordist = -1;

	ZeroMemory(&vers,sizeof(vers)); 
	//Sstart in NT/Win2000 
	_stprintf(hd, _T("\\\\.\\PhysicalDrive%d"),btDisk);
	h=CreateFile(hd,GENERIC_READ|GENERIC_WRITE, 
		FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_EXISTING,0,0); 
	if(!h)
	{
		if (btDisk != lasterrordist)
		{
			::MessageBox(hHandle,_T("Can not open device."),_T("Error"),MB_OK);
			lasterrordist = btDisk;
		}
		return;
	}

	memset(&vers,0,sizeof(vers));
	bResult = DeviceIoControl(h,DFP_GET_VERSION,0,0,&vers,sizeof(vers),&i,0);
	if (!bResult)
	{ 
		if (btDisk != lasterrordist)
		{
			::MessageBox(hHandle,_T("DFP get version fail."),_T("Error"),MB_OK);
			CloseHandle(h); 
			lasterrordist = btDisk;
		}

		return;
	} 
	//If IDE identify command not supported, fails 
	if (!(vers.fCapabilities&1))
	{ 
		if (btDisk != lasterrordist)
		{
			::MessageBox(hHandle,_T("IDE identify command not supported."),_T("Error"),MB_OK);
			CloseHandle(h); 
			lasterrordist = btDisk;
		}

		return; 
	}
	//Identify the IDE drives 
	ZeroMemory(&in,sizeof(in)); 
	ZeroMemory(&out,sizeof(out)); 
	if (btDisk&1)
	{ 
		in.irDriveRegs.bDriveHeadReg=0xb0; 
	}
	else
	{ 
		in.irDriveRegs.bDriveHeadReg=0xa0; 
	} 
	if (vers.fCapabilities&(16>>btDisk))
	{ 
		//We don't detect a ATAPI device.
		if (btDisk != lasterrordist)
		{
			_stprintf(szMessage,_T("HardDisk - %d is a ATAPI device, it can't be detected"),btDisk);
			::MessageBox(hHandle,szMessage,_T("Error"),MB_OK);
			CloseHandle(h); 
			lasterrordist = btDisk;
		}

		return ;
	}
	else
	{ 
		in.irDriveRegs.bCommandReg=0xec; 
	} 
	in.bDriveNumber=btDisk; 
	in.irDriveRegs.bSectorCountReg=1; 
	in.irDriveRegs.bSectorNumberReg=1; 
	in.cBufferSize=512; 
	if (!DeviceIoControl(h,DFP_RECEIVE_DRIVE_DATA,&in,sizeof(in),&out,sizeof(out),&i,0))
	{ 
		if (btDisk != lasterrordist)
		{
			::MessageBox(hHandle,_T("DeviceIoControl failed:DFP_RECEIVE_DRIVE_DATA"),_T("Error"),MB_OK);
			CloseHandle(h); 
			lasterrordist = btDisk;
		}

		return; 
	} 
	phdinfo=(PIDSECTOR)out.bBuffer;
	ZeroMemory(s, MAX_PATH * sizeof(TCHAR));
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, phdinfo->sModelNumber, 40, s, MAX_PATH);
	ChangeByteOrder(s,40); 
	g_ModuleNumber= s;
	ZeroMemory(s, MAX_PATH * sizeof(TCHAR));
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, phdinfo->sFirmwareRev, 8, s, MAX_PATH);
	ChangeByteOrder(s,8); 
	g_FirmWare = s;
	ZeroMemory(s, MAX_PATH * sizeof(TCHAR));
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, phdinfo->sSerialNumber, 20, s, MAX_PATH);
	ChangeByteOrder(s,20); 
	g_SerNumber = s;
	g_Capacity.Format(_T("%dM"),phdinfo->ulTotalAddressableSectors/2/1024);
	CloseHandle(h); 
} 

VOID HDInfo9x(BYTE btDisk,HWND hHandle)
{ 
	PIDSECTOR	phdinfo; 
	TCHAR		s[MAX_PATH];
	TCHAR		szMessage[128];
	HANDLE		h; 
	DWORD		i; 
	YG_GETVERSIONINPARAMS		vers; 
	YG_SENDCMDINPARAMS			in; 
	YG_SENDCMDOUTPARAMS		out; 
	g_ModuleNumber = _T("");
	g_FirmWare = _T("");
	g_SerNumber = _T("");
	g_Capacity = _T("");

	ZeroMemory(&vers,sizeof(vers)); 
	//Start in 95/98/Me 
	h=CreateFile(_T("\\\\.\\Smartvsd"),0,0,0,CREATE_NEW,0,0); 
	if (!h)
	{ 
		::MessageBox(hHandle,_T("OPen smartvsd failed."),_T("Error"),MB_OK);
		return; 
	} 

	if (!DeviceIoControl(h,DFP_GET_VERSION,0,0,&vers,sizeof(vers),&i,0))
	{ 
		::MessageBox(hHandle,_T("DeviceIoControl failed:DFP_GET_VERSION"),_T("Error"),MB_OK);
		CloseHandle(h); 
		return; 
	} 
	//If IDE identify command not supported, fails 
	if (!(vers.fCapabilities&1))
	{ 
		::MessageBox(hHandle,_T("IDE identify command not supported."),_T("Error"),MB_OK);
		CloseHandle(h); 
		return; 
	} 
	//Display IDE drive number detected 
	//DetectIDE(vers.bIDEDeviceMap); 
	//Identify the IDE drives 
	ZeroMemory(&in,sizeof(in)); 
	ZeroMemory(&out,sizeof(out)); 
	if (btDisk&1)
	{ 
		in.irDriveRegs.bDriveHeadReg=0xb0; 
	}
	else
	{ 
		in.irDriveRegs.bDriveHeadReg=0xa0; 
	} 
	if (vers.fCapabilities&(16>>btDisk))
	{ 
	//We don't detect a ATAPI device. 
		_stprintf(szMessage,_T("HardDisk - %d is a ATAPI device, it can't be detected"),btDisk);
		::MessageBox(hHandle,szMessage,_T("Error"),MB_OK);
		CloseHandle(h); 
		return ;
	}
	else
	{ 
		in.irDriveRegs.bCommandReg=0xec; 
	} 
	in.bDriveNumber=btDisk; 
	in.irDriveRegs.bSectorCountReg=1; 
	in.irDriveRegs.bSectorNumberReg=1; 
	in.cBufferSize=512; 
	if (!DeviceIoControl(h,DFP_RECEIVE_DRIVE_DATA,&in,sizeof(in),&out,sizeof(out),&i,0))
	{ 
		::MessageBox(hHandle,_T("DeviceIoControl failed:DFP_RECEIVE_DRIVE_DATA"),_T("Error"),MB_OK);
		CloseHandle(h); 
		return; 
	} 
	phdinfo=(PIDSECTOR)out.bBuffer;
	ZeroMemory(s, MAX_PATH * sizeof(TCHAR));
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, phdinfo->sModelNumber, 40, s, MAX_PATH);
	ChangeByteOrder(s,40); 
	g_ModuleNumber= s;
	ZeroMemory(s, MAX_PATH * sizeof(TCHAR));
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, phdinfo->sFirmwareRev, 8, s, MAX_PATH);
	ChangeByteOrder(s,8); 
	g_FirmWare = s;
	ZeroMemory(s, MAX_PATH * sizeof(TCHAR));
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, phdinfo->sSerialNumber, 20, s, MAX_PATH);
	ChangeByteOrder(s,20); 
	g_SerNumber = s;
	g_Capacity.Format(_T("%dM"),phdinfo->ulTotalAddressableSectors/2/1024);
	CloseHandle(h); 
}

TCHAR * strrchrpro(TCHAR* szSource,TCHAR chChar)
{
	CString strSource;
	strSource=szSource;
	int nIndex;
	nIndex = strSource.ReverseFind(chChar);
	if ( -1 == nIndex )
	{
		// not found;
		return NULL;
	}
	// return a pointer
	return (szSource+nIndex);
}

CString GetUserPath()
{
	TCHAR path[MAX_PATH];
	memset(path,0,sizeof(path));
	SHGetSpecialFolderPath(NULL,path,CSIDL_PERSONAL,FALSE);
	GetLongPathName(path, path, MAX_PATH);
	return path;
}
