// AMMakeCDR.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "AM01.h"
#include "AMMakeCDR.h"

// required by CoCreateGuid
#include <objbase.h>

#include "../AM01Expt/AM01Expt.h"

// Function prototypes.
HANDLE           g_hAbortEvent;
CStatic         *g_pStateMsg; 
CButton         *g_CancelButton;
CProgressCtrl	*g_pProgress;
BOOL             g_bAbort;
HWND             g_hWnd;  

DWORD            g_dwOneCDRSize;
DWORD            g_dwMaxCDFileSize;

DWORD            g_dwSpaceLeft = 3000;
UBYTE            g_wRecordSpeed = SPEED_MAX;
extern           char g_szPath[MAX_PATH];
int g_DiscType;

// FREE_SPACE_NEED is the extra free space of disk for buffer file
#define FREE_SPASCE_NEED 1024*1024*10;

// 5 M ,different CD may have differnt free space
const int	CD_DIFFERENT_FREE_SPACE = 1024 * 1024 * 5 ; 

// left some extra free space to avoid write fail

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//-- for seperate file begin
// BOOL MakeMultiCD(CDWriter *cdwriterP,CString csLastFile, DWORD dwMaxFileSize);

BOOL NewMakeMultiCD(CDWriter *cdwriterP,CString csLastFile, DWORD dwMaxFileSize);

//-- for seperate file end


/////////////////////////////////////////////////////////////////////////////
// CAMMakeCDR dialog


IMPLEMENT_DYNCREATE(CAMMakeCDR, CPropertyPage)

CAMMakeCDR::CAMMakeCDR() : CPropertyPage(CAMMakeCDR::IDD)
{
	//{{AFX_DATA_INIT(CAMMakeCDR)
	m_csFileName = _T("");
	//}}AFX_DATA_INIT
}

CAMMakeCDR::~CAMMakeCDR()
{
}

void CAMMakeCDR::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAMMakeCDR)
	DDX_Control(pDX, IDCANCEL, m_CancleButton);
	DDX_Control(pDX, IDC_BUTTON_FILE, m_RefFile);
	DDX_Control(pDX, IDC_BUTTON_CREATE, m_CreateCD);
	DDX_Control(pDX, IDC_STATE, m_StateMsg);
	DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
	DDX_Text(pDX, IDC_IMAGE_FILE_NAME, m_csFileName);
	DDV_MaxChars(pDX, m_csFileName, 260);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAMMakeCDR, CPropertyPage)
	//{{AFX_MSG_MAP(CAMMakeCDR)
	ON_BN_CLICKED(IDC_BUTTON_CREATE, OnCreateCD)
	ON_BN_CLICKED(IDC_BUTTON_FILE, OnOpenImgFile)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CAMMakeCDR::OnCreateCD() 
{
	UpdateData(TRUE);

	DWORD dwErrorCode;
	dwErrorCode=0;

	if( CheckLastImage(LPCTSTR(m_csFileName),&dwErrorCode) != TRUE)
	{
		if(dwErrorCode==NOT_LAST_IMAGE)
		{
		    // AfxMessageBox(L"Not not the last image of this backup set.",MB_OK|MB_ICONINFORMATION,NULL);
			AfxMessageBox(IDS_NOT_LAST_IMAGE,MB_OK|MB_ICONINFORMATION,NULL);
		}
		return;
	}

	m_RefFile.EnableWindow(FALSE);
	m_CreateCD.EnableWindow(FALSE);
	if (m_csFileName != "" )
	{
		m_Progress.SetRange(0,100);
		g_bAbort = FALSE;
		File2CD(m_cdWriterP,m_csFileName,&m_Progress,&m_StateMsg);
	}
	else DisplayMessage(m_hWnd,IDS_SELECT_FILE,IDS_TITLE,MB_OK | MB_ICONINFORMATION);
	m_RefFile.EnableWindow(TRUE);
	m_CreateCD.EnableWindow(TRUE);
}

void CAMMakeCDR::OnOpenImgFile() 
{
	char szFile[MAX_PATH];
	if (SelectFile(L"AMG","AM01 File Image",szFile))
	{
		m_csFileName = szFile;
		UpdateData(FALSE);
	}	
}

void Exception(MSGVEC *msgvecP)
{
	CString cstr;
	CString cstr1;
	CString cstr2;
	char *msgtextP;
	
	// If this is the L"User Abort" exception, then just ignore it.

	if (msgvecP->code == E_UserAbort)
	{
		g_bAbort = TRUE;
		return;
	}

	if (msgvecP->code == E_CommandFailed && 
		msgvecP->subcode == E_DeviceNotReady)
	{
		DisplayMessage(g_hWnd,IDS_DISK_NOTREADY,IDS_ERROR,MB_OK | MB_ICONERROR);
		return;
	}
	// Process the primary exception message.

	msgtextP = ::GetMessageText(msgvecP->code);
  
	cstr1.Format(msgtextP,
		msgvecP->param[0], msgvecP->param[1], msgvecP->param[2],
		msgvecP->param[3], msgvecP->param[4], msgvecP->param[5],
		msgvecP->param[6], msgvecP->param[7]);

	// Process the secondary exception message.

	if (msgvecP->subcode)
	{
		msgtextP = ::GetMessageText(msgvecP->subcode);

  		cstr2.Format(msgtextP,
			msgvecP->subparam[0], msgvecP->subparam[1], msgvecP->subparam[2],
			msgvecP->subparam[3], msgvecP->subparam[4], msgvecP->subparam[5],
			msgvecP->subparam[6], msgvecP->subparam[7]);
	}
	// Display the exception in a message box.
	cstr = cstr1 + L"\n" + cstr2;
	AfxMessageBox(cstr, MB_ICONSTOP);
	return;
}

BOOL EventCallback(void *param, CDREVENT *event)
{
	CString  strMessage;
	static DWORD dwWrite=0;
	static DWORD dwTotal = 100;
	GetSendMessage(g_hWnd);
	switch(event->code)
	{
		case EVENT_BUILD_CDFS_START :
			dwWrite = 0;
			dwTotal=event->param[0];
			g_pProgress->SetPos(0);
			strMessage.LoadString(IDS_BUILD_ISO9660);
			g_pStateMsg->SetWindowText(strMessage);
			break;
		case EVENT_BUILD_CDFS_PROGRESS:
			dwWrite+=event->param[0];
			g_pProgress->SetPos(dwWrite*100/dwTotal);
			break;
		case EVENT_BUILD_CDFS_END :
			g_pProgress->SetPos(0);
			g_pStateMsg->SetWindowText("");
			break;
		case EVENT_CLOSE_SESSION_START:
			g_pProgress->SetPos(100);
			strMessage.LoadString(IDS_CLOSE_SESSION);
			g_pStateMsg->SetWindowText(strMessage);
			g_CancelButton->EnableWindow(FALSE);
			break;
		case EVENT_CLOSE_SESSION_END:
			g_pStateMsg->SetWindowText("");
			g_pProgress->SetPos(0);
			g_CancelButton->EnableWindow(TRUE);
			break;
		case EVENT_ERASE_DISC_START:
			break;
		case EVENT_ERASE_DISC_END:
			break;
		case EVENT_RECORD_GAP_START:
			dwWrite = 0;
			dwTotal=event->param[1];
			strMessage.LoadString(IDS_RECORD_GAP);
			g_pStateMsg->SetWindowText(strMessage);
			break;
		case EVENT_RECORD_GAP_PROGRESS:
			dwWrite+=event->param[1];
			g_pProgress->SetPos(dwWrite*100/dwTotal);
			break;
		case EVENT_RECORD_GAP_END:
			g_pStateMsg->SetWindowText("");
			g_pProgress->SetPos(0);
			break;

		case EVENT_RECORD_TRACK_START:
			dwWrite = 0;
			dwTotal=event->param[1];
			strMessage.LoadString(IDS_RECORD_TRACK);
			g_pStateMsg->SetWindowText(strMessage);
			break;
		case EVENT_RECORD_TRACK_PROGRESS:
			dwWrite+=event->param[1];
			g_pProgress->SetPos(dwWrite*100/dwTotal);
			break;
		case EVENT_RECORD_TRACK_END:
			g_pStateMsg->SetWindowText("");
			g_pProgress->SetPos(0);
			break;
		case EVENT_RECORD_USERDATA_START:
			strMessage.LoadString(IDS_RECORD_USERDATA);
			g_pStateMsg->SetWindowText(strMessage);
			break;
		case EVENT_RECORD_USERDATA_END:
			g_pStateMsg->SetWindowText("");
			break;
		default:
			break;
	}
	return TRUE;
}


BOOL CDWriterReady(CDWriter *cdwriterP,SLONG *base_lba)
{
	
	UBYTE  wSpeed; 
    int status;
    int write_status;
    ULONG freespace;
	DWORD dwCylinderSize;

	if (cdwriterP == NULL) return FALSE;
    // Make sure that the device is ready.
    cdwriterP->LoadDisc (TRUE);
    cdwriterP->GetDiscType (&g_DiscType);
	switch (g_DiscType)
    {
	case DISCTYPE_CDROM:
	case DISCTYPE_CDR:
	case DISCTYPE_CDRW:
	case DISCTYPE_CDROM_DD:
	case DISCTYPE_CDR_DD:
	case DISCTYPE_CDRW_DD:
	case DISCTYPE_DVDROM:
	case DISCTYPE_DVDR:
	case DISCTYPE_DVDR_DL:
	case DISCTYPE_DVDRW:
	case DISCTYPE_DVDPLUSR:
	case DISCTYPE_DVDPLUSR_DL:
	case DISCTYPE_DVDPLUSRW:
	case DISCTYPE_DVDRAM:
	// Get the write status of a disc.
        cdwriterP->GetDiscWriteStatus (&write_status);
		if (write_status == DISC_STATUS_WRITABLE || write_status == DISC_STATUS_EMPTY)
		{
			// Get the amount of freespace (in sectors) remaining on the disc.
			status = cdwriterP->GetDiscFreespace(&freespace);
			if (status == E_Success)
			{
				if (*base_lba < 0) *base_lba=0;
				dwCylinderSize = SECTORS_PER_TRACK*HEADS*SECTOR_SIZE;
							
				__int64 qwOneCDRSize;
				
				qwOneCDRSize = (((__int64)freespace*CDSCT-BOOTABLE_CD_RESERVED_SYS_SPACE)/dwCylinderSize)*dwCylinderSize-BOOTABLE_IMG_RESERVED_SYS_SPACE;
						
				if( qwOneCDRSize > AM_MAX_FILE_SIZE)
				{
				   g_dwOneCDRSize = AM_MAX_FILE_SIZE;
				}
				else
				{
				   g_dwOneCDRSize =  (DWORD)qwOneCDRSize;
				}

							
				wSpeed = cdwriterP->GetMaxWriteSpeed();
				if (g_wRecordSpeed > wSpeed)	g_wRecordSpeed = wSpeed;			
				return TRUE;
			}
			else DisplayMessage(g_hWnd,IDS_NOT_WRITEABLE,IDS_ERROR,MB_OK | MB_ICONERROR);
		}
		else DisplayMessage(g_hWnd,IDS_NOT_WRITEABLE,IDS_ERROR,MB_OK | MB_ICONERROR);
		break;
    default:
		DisplayMessage(g_hWnd,IDS_NO_CDR,IDS_ERROR,MB_OK | MB_ICONERROR);
		break;
	}
	return FALSE;
}


BOOL MakeCD(CDWriter *cdwriterP,CString csFile)
{
	SLONG     base_lba;
	BOOL      bResult = FALSE;
	int       desc_count = 0;
	char      szExeName[MAX_PATH];
	
	// char      szFile[MAX_PATH];

	if (!CDWriterReady(cdwriterP,&base_lba)) return FALSE;

	DIRPATHDESC pathdesc_vec[4];
	memset(&pathdesc_vec,0,sizeof(DIRPATHDESC)*4);

	pathdesc_vec[0].pathnameP = csFile.GetBuffer(0);
	// pathdesc_vec[0].fullpath_flag = TRUE;
	desc_count ++;

	wcscpy(szExeName,g_szPath);

	wcscat(szExeName,L"\\FileRestore.exe");
	
	pathdesc_vec[1].pathnameP = szExeName;
	desc_count ++;

	char szDllFileName[MAX_PATH];
	wcscpy(szDllFileName,g_szPath);
	wcscat(szDllFileName,L"\\AM01DLL.dll");
	pathdesc_vec[2].pathnameP = szDllFileName;
	desc_count ++;

	char szZlibDll[MAX_PATH];
	wcscpy(szZlibDll,g_szPath);
	wcscat(szZlibDll,L"\\zlib.dll");
	pathdesc_vec[3].pathnameP = szZlibDll;
	desc_count ++;

	CDFSOPTIONS cdfs_options;
   	memset(&cdfs_options,0, sizeof(CDFSOPTIONS));

	cdfs_options.filesystem_type = CDFS_TYPE_ISO9660;
	cdfs_options.file_date_option = CDFS_FILEDATE_ORIGINAL;

	RECORDOPTIONS recordopt;
	memset(&recordopt,0, sizeof(RECORDOPTIONS));
	recordopt.speed = g_wRecordSpeed;
	recordopt.test_flag = FALSE;
	recordopt.postgap_flag = TRUE;
	recordopt.close_session_flag = TRUE;
	recordopt.multisession_flag = FALSE;
	recordopt.beep_flag = FALSE;
	recordopt.eject_flag = TRUE;
	recordopt.log_flag = TRUE;
	ResetEvent(g_hAbortEvent);
		
	cdwriterP->RecordDirectoryPaths(pathdesc_vec, desc_count,NULL, &cdfs_options, &recordopt, FALSE);
	bResult = TRUE;
	return bResult;
}

int DisplayMessage(HWND hWnd,DWORD dwMessage,DWORD dwTitle,DWORD dwType)
{
	CString strMessage,strTitle;
	strMessage.LoadString(dwMessage);
	strTitle.LoadString(dwTitle);
	return MessageBox(hWnd,strMessage,strTitle,dwType);
}

BOOL File2CD(CDWriter *cdwriterP,CString csFile,CProgressCtrl *pProgress,CStatic *pStateMsg)
{
	char     szFile[MAX_PATH];
	BOOL  bResult = FALSE;
	SLONG    base_lba;
	WIN32_FIND_DATA FindData;
	HANDLE		hFind;

	auto_ptr<DirectoryTree> dirtreeP;
	
	// Set the memory allocation failure handler.

	g_dwSpaceLeft = 2000;
	// Enable exception handling.

	try
	{
		// Register the event callback function.
		if (CDWriterReady(cdwriterP,&base_lba))
		{
			// this is a line to get g_dwCDRSize;

			//-- for debug temp comment begin
			g_dwMaxCDFileSize = g_dwOneCDRSize - CD_DIFFERENT_FREE_SPACE - CD_DIFFERENT_FREE_SPACE;
			//-- for debug temp comment end

			//-- for debug begin
			// g_dwMaxCDFileSize = 26 * 1024 * 1024 ;
			//-- for debug end

			g_pProgress = pProgress;
			g_pStateMsg = pStateMsg;
			hFind = FindFirstFile(csFile.GetBuffer(0),&FindData);
			if(hFind != INVALID_HANDLE_VALUE)
			{
				FindClose(hFind);
				wcscpy(szFile,csFile);
				if (
					 ( FindData.nFileSizeLow >  g_dwMaxCDFileSize)
					 ||  ( bMultiFile(szFile) == TRUE)
				   )
				{
					// AfxMessageBox(L"File too big",MB_OK | MB_ICONINFORMATION);
					// bResult = FALSE;
					// now csFile should be the last file name.
					CString csLastFile;
					
					csLastFile=csFile;

					bResult = NewMakeMultiCD(cdwriterP,csLastFile,g_dwMaxCDFileSize);
				}
				else
				{
					bResult = MakeCD(cdwriterP,csFile);
				}
			}
			else DisplayMessage(g_hWnd,IDS_FILE_NOTFOUND,IDS_ERROR,MB_OK | MB_ICONERROR);
		}
	}
	catch (MSGVEC *msgvecP)						
	{												
		Exception(msgvecP);		
		delete msgvecP;		
	}
	return bResult;
}


BOOL GetSepName(CString csFile,char *szFile)
{
	char      *p;
	HANDLE    hFind;
	WIN32_FIND_DATA FindInfo;
	BOOL      bResult = TRUE;
	__int64   inFileSize,inUsedSize,inFreeSize = 0;
	
	hFind = FindFirstFile(csFile,&FindInfo);
	if (hFind == INVALID_HANDLE_VALUE) return FALSE;
	FindClose(hFind);
	inFileSize = FindInfo.nFileSizeHigh*0x100000000+FindInfo.nFileSizeLow;

	// if(inFileSize>g_dwCDRSize)	inFileSize=g_dwCDRSize;
	if(inFileSize > g_dwMaxCDFileSize) inFileSize=g_dwMaxCDFileSize;
	
	inFileSize += FREE_SPASCE_NEED;
	
	wcscpy(szFile,g_szPath);
	
	wcscat(szFile,strrchrpro(csFile.GetBuffer(0),'\\'));

    if (!GetVolumeFreeSpace(szFile[0],&inUsedSize,&inFreeSize)) return FALSE;

	if(inFreeSize < inFileSize)
	{
		wcscpy(szFile,csFile);
		if ((szFile[0] >= 'a' && szFile[0] <= 'z') || (szFile[0] >= 'A' && szFile[0] <= 'Z'))
		{
		    if (!GetVolumeFreeSpace(szFile[0],&inUsedSize,&inFreeSize)) return FALSE;
		}
		bResult = inFreeSize > inFileSize;
	}
	
    p = strrchrpro(szFile,'.');
	
	wcscpy(p,L".ISO");
	return bResult;
}


void GetSendMessage(HWND hWnd)
{
	MSG   msg;
	while (PeekMessage(&msg,hWnd,0,0,PM_REMOVE))
	{
		switch (msg.message)
		{
		case WM_COMMAND:
		case WM_DRAWITEM:
		case WM_ERASEBKGND:
		case WM_PAINT:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_NCDESTROY:
        case WM_NCLBUTTONDOWN:
		case WM_NCLBUTTONUP:
			::SendMessage(msg.hwnd,msg.message,msg.wParam,msg.lParam);
			break;
		default:
			break;
		}
	}
}

BOOL CAMMakeCDR::OnInitDialog() 
{
	int    x = 3;
	CPropertyPage::OnInitDialog();
	g_pStateMsg = &m_StateMsg; 
	g_pProgress = &m_Progress;
	g_hWnd = m_hWnd;  
	g_CancelButton = &m_CancleButton;
    m_cdWriterP=NULL;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CAMMakeCDR::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	ASPIAdapter::ShutdownManager();
	CPropertyPage::OnClose();
}

void CAMMakeCDR::OnCancel() 
{
	// TODO: Add extra cleanup here
	SetEvent(g_hAbortEvent);
	g_bAbort = TRUE;	
	CPropertyPage::OnCancel();
}

BOOL GetVolumeFreeSpace(char cDrive,__int64 *pi64Count,__int64 *pFreeBytes)
{
	BOOL  fResult=FALSE;
	char  szDrive[10];
    DWORD dwSectPerClust,dwBytesPerSect,dwFreeClusters,dwTotalClusters;
    P_GDFSE pGetDiskFreeSpaceEx = NULL;
    unsigned __int64 i64FreeBytesToCaller;//,i64TotalBytes;
    
	wcscpy(szDrive,L"C:\\");
	szDrive[0] = cDrive;

    pGetDiskFreeSpaceEx = (P_GDFSE)GetProcAddress (GetModuleHandle (L"kernel32.dll"),
                                                   L"GetDiskFreeSpaceExA");
    if (pGetDiskFreeSpaceEx)
    {
        fResult = pGetDiskFreeSpaceEx (szDrive,
                                      (PULARGE_INTEGER)&i64FreeBytesToCaller,
                                      (PULARGE_INTEGER)pi64Count,
                                      (PULARGE_INTEGER)pFreeBytes);
    }
    else
    {
         fResult = GetDiskFreeSpace (szDrive, 
                                     &dwSectPerClust,
                                     &dwBytesPerSect, 
                                     &dwFreeClusters,
                                     &dwTotalClusters);
         if (fResult)
         {
            /* force 64-bit math */ 
            *pi64Count = (__int64)dwTotalClusters * dwSectPerClust *
                              dwBytesPerSect;
            *pFreeBytes = (__int64)dwFreeClusters * dwSectPerClust *
                              dwBytesPerSect;
		 }
     }
     return fResult;
}

void CAMMakeCDR::DisableControl()
{

	m_CancleButton.EnableWindow(FALSE);
	m_RefFile.EnableWindow(FALSE);
	m_CreateCD.EnableWindow(FALSE);
	m_StateMsg.EnableWindow(FALSE);
	m_Progress.EnableWindow(FALSE);
	GetDlgItem(IDC_IMAGE_FILE_NAME)->EnableWindow(FALSE);

}

//-- for seperate file begin



//-- for seperate file end

BOOL NewMakeMultiCD(CDWriter *cdwriterP,CString csLastFile, DWORD dwMaxFileSize)
{
	// g_cdwriterP=cdwriterP;

	CDATA_ARRAY SourceArray;
	CDATA_ARRAY TargetArray;

	GetSourceArray((LPCTSTR)csLastFile,SourceArray);

    if(SourceArray.GetUpperBound()<1)
	{
		return FALSE;
	}

	//-- count the total file size begin
	//-- this can judge if the buffer size is enough

	unsigned __int64 qwTotalFileSize;
	qwTotalFileSize = 0;
	int nFileNumber;

	for(nFileNumber=1; nFileNumber<=SourceArray.GetUpperBound(); nFileNumber ++)
	{
		qwTotalFileSize = qwTotalFileSize + SourceArray[nFileNumber].qwFileSize.QuadPart ;
	}

	char szLastImage[MAX_PATH];
	memset(szLastImage,0,MAX_PATH);
	wcsncpy(szLastImage,SourceArray[SourceArray.GetUpperBound()].szFileName,MAX_PATH-1);
	FreeAMDataArray(SourceArray);

	TABLE_DATA EmptyData;
	memset(&EmptyData,0,sizeof(EmptyData));
	TargetArray.Add(EmptyData);

	CString csBufferDir;
    int nIndex;
	nIndex=csLastFile.ReverseFind('\\');
	csBufferDir=csLastFile.Left(nIndex);

	// if there is not enough buffer size in target dir
	// the return
	HANDLE hFind;
	WIN32_FIND_DATA FindData;
	hFind = FindFirstFile((LPCTSTR)csLastFile,&FindData);

	ULARGE_INTEGER qwFileSize;
	qwFileSize.HighPart=FindData.nFileSizeHigh;
	qwFileSize.LowPart=FindData.nFileSizeLow;

	FindClose(hFind);

	DWORD dwSectorsPerCluster;
    DWORD dwBytesPerSector;
	DWORD dwNumberOfFreeClusters;
	DWORD dwTotalNumberOfClusters;

	CString csRootPath;
	csRootPath=csBufferDir.Left(2)+L"\\";
	
	GetDiskFreeSpace( (LPCTSTR)csRootPath,
					  &dwSectorsPerCluster,
					  &dwBytesPerSector,
					  &dwNumberOfFreeClusters,
					  &dwTotalNumberOfClusters );

	//-- for debug
	int nErrorCode;
	nErrorCode=GetLastError();
	//-- end for debug

	ULARGE_INTEGER qwFreeSpace;
	qwFreeSpace.QuadPart = dwNumberOfFreeClusters;
	qwFreeSpace.QuadPart = qwFreeSpace.QuadPart * dwSectorsPerCluster * dwBytesPerSector;
	
	if ( qwFreeSpace.QuadPart < qwTotalFileSize )
	{
	   if( qwFreeSpace.LowPart < dwMaxFileSize)
	   {
		AfxMessageBox(IDS_NOT_ENOUGH_SPACE,MB_OK|MB_ICONINFORMATION,NULL);
		return FALSE;
	   }
	}


	TABLE_DATA TargetData;
	CString strLastFile;
	// BOOL bReturnValue;

	DWORD dwLastTarget;

	// here we calculate the image size and get the Target Number
	GetLastTargetNumber(szLastImage,dwMaxFileSize,&dwLastTarget);

	for( DWORD dwTargetNumber=1; dwTargetNumber<=dwLastTarget; dwTargetNumber++ )
	{

	   GetOneFile(szLastImage,  (LPCTSTR)csBufferDir,  dwMaxFileSize,  dwTargetNumber,  &TargetData);

	   if( dwTargetNumber < dwLastTarget)
	   {
			strLastFile = TargetData.szFileName ;

			if( (dwTargetNumber > 1) && (dwTargetNumber < dwLastTarget) )
			{
				AfxMessageBox(IDS_NEXT_CD,MB_OK|MB_ICONINFORMATION);
			}
			
			if ( NewMakeCD(cdwriterP,strLastFile) == FALSE )
			{
			   return FALSE;
			}

			DeleteFile( (LPCTSTR)strLastFile );
	   }

	   TargetArray.Add(TargetData);
	}

	BOOL bNewFile;
	char szNewTarget[MAX_PATH];
	bNewFile=FALSE;
	memset(szNewTarget,0,MAX_PATH);

    // last target file will come from TargetArray
	// if the tail file is a new file, szNewTarget will be useful.
	NewRecordTail ( szLastImage, (LPCTSTR)csBufferDir, TargetArray, dwMaxFileSize, &bNewFile, szNewTarget);

	CString csFile;

	csFile = TargetArray[TargetArray.GetUpperBound()].szFileName;

	if(bNewFile == FALSE)
	{
		//-- here to make the pre last CD

		if( TargetArray.GetUpperBound() > 1 )
		{
			AfxMessageBox(IDS_NEXT_CD,MB_OK|MB_ICONINFORMATION);
		}
		
		if( NewMakeCD(cdwriterP,csFile) == FALSE )
		{
		    DeleteFile( (LPCTSTR)csFile );
			return FALSE;
		}

		DeleteFile( (LPCTSTR)csFile );

		//-- here to make the pre last CD end
	}
	else
	{

		if( TargetArray.GetUpperBound() > 1 )
		{
		    // ask user to put a new cd to CD-RW
			AfxMessageBox(IDS_NEXT_CD,MB_OK|MB_ICONINFORMATION);

		}

		if( NewMakeCD(cdwriterP,csFile) == FALSE )
		{
		    DeleteFile( (LPCTSTR)csFile );
			return FALSE;
		}

		DeleteFile( (LPCTSTR)csFile );

		csFile=szNewTarget;

		AfxMessageBox(IDS_NEXT_CD,MB_OK|MB_ICONINFORMATION);

		if( NewMakeCD(cdwriterP,csFile) == FALSE )
		{
		    DeleteFile( (LPCTSTR)csFile );
			return FALSE;
		}

		DeleteFile( (LPCTSTR)csFile );

	}

	return TRUE;
}


BOOL NewMakeCD(CDWriter *cdwriterP,CString csFile)
{

    BOOL bReturnValue;
	
	int nUserOption;

	//*
	SLONG base_lba;
	
	bReturnValue = CDWriterReady(cdwriterP,&base_lba);

	while(bReturnValue!=TRUE)
	{
		// nUserOption = AfxMessageBox(L"Record to CD-ROM Error",MB_RETRYCANCEL|MB_ICONINFORMATION,NULL);
		nUserOption = AfxMessageBox(IDS_RECORD_CDR_ERROR,MB_RETRYCANCEL|MB_ICONINFORMATION,NULL);
		if(nUserOption == IDCANCEL)
		{
			break;
		}
		// bReturnValue=MakeCD(cdwriterP,csFile);
		bReturnValue = CDWriterReady(cdwriterP,&base_lba);
	}

	if(nUserOption == IDCANCEL)
	{
	   return FALSE;
	}


	if( g_dwOneCDRSize <  g_dwMaxCDFileSize )
	{
		AfxMessageBox(L"Not enough free space in the target CD",MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}

    //*/

	bReturnValue = MakeCD(cdwriterP,csFile);

	while(bReturnValue!=TRUE)
	{
		// nUserOption = AfxMessageBox(L"Record to CD-ROM Error",MB_RETRYCANCEL|MB_ICONINFORMATION,NULL);
		nUserOption = AfxMessageBox(IDS_RECORD_CDR_ERROR,MB_RETRYCANCEL|MB_ICONINFORMATION,NULL);
		if(nUserOption == IDCANCEL)
		{
			break;
		}
		bReturnValue=MakeCD(cdwriterP,csFile);
	}

	if(nUserOption == IDCANCEL)
	{
	   return FALSE;
	}


	return TRUE;

}

BOOL CAMMakeCDR::OnSetActive() 
{
	if (!m_cdWriterP)
	{
		EXCEPTION_HANDLER_START
		EventRegisterCallback (ConsoleEventCallback);
		g_hAbortEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		RegisterAbortEvent(g_hAbortEvent);
		// Startup the ASPI manager.
		ASPIAdapter::StartupManager (FALSE, FALSE, TRUE);
		m_cdWriterP = (CDWriter *)ASPIAdapter::FindDeviceObject (ASPI_M_DEVTYPE_WORM);
		if (!m_cdWriterP) DisableControl();
		EXCEPTION_HANDLER_EXIT
	}
	
	return CPropertyPage::OnSetActive();
}
