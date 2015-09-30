// WZStep4.cpp : implementation file
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "AMRestor.h"

// required by CoCreateGuid
#include <objbase.h>

#include "WZStep4.h"
#include "RestoreState.h"

// #include "Function.h"
#include "../AM01Expt/AM01Expt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern AM_RESTORE_INFO g_RestoreInfo;
int g_SelectedItem;
RESTORE_STATE g_RestoreState;

CDATA_ARRAY g_ImageArray;

SEGMENT_INFO g_SegmentInfo;

CString g_strTextCancel;

BOOL LoadSegment(
				  LPCTSTR		szImageFile, // this should be the last file
				  CPATH_ARRAY	&PathArray,
				  DWORD			dwSegmentNumber,
				  DWORD			dwSegmentSize
				);


BOOL bPathInSegment(
					 LPCTSTR		SourceFilePath,
					 CPATH_ARRAY	&PathArray,
					 DWORD          dwBeginOffset,
					 DWORD			*pdwPathOffset
				   );

BOOL NewGetPathID(
				   LPCTSTR szImageFile,
				   LPCTSTR szOnePathString,
				   DWORD *pdwPathID
				 );

BOOL GetPathCount(LPCTSTR szImageFile, DWORD *pdwPathCount);

//-- new one added begin

/////////////////////////////////////////////////////////////////////////////
// CWZStep4 property page

IMPLEMENT_DYNCREATE(CWZStep4, CPropertyPageEx)

CWZStep4::CWZStep4() : CPropertyPageEx(CWZStep4::IDD, 0, IDS_RESTORE_FINISH_TITLE, IDS_RESTORE_FINISH_SUBTITLE)
{
	//{{AFX_DATA_INIT(CWZStep4)
	//}}AFX_DATA_INIT
//	m_psp.dwFlags |= PSP_HIDEHEADER;
}

CWZStep4::~CWZStep4()
{
}

void CWZStep4::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWZStep4)
	DDX_Control(pDX, IDC_TREE1, m_DirTree);
	DDX_Control(pDX, IDC_LIST1, m_TimeStamp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWZStep4, CPropertyPageEx)
	//{{AFX_MSG_MAP(CWZStep4)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, OnClickList1)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnSelchangedTree1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWZStep4 message handlers

BOOL CWZStep4::OnSetActive()
{
	CPropertySheet* pSheet = (CPropertySheet*)GetParent();
	ASSERT_KINDOF(CPropertySheet, pSheet);
	
	// pSheet->SetWizardButtons(PSWIZB_NEXT | PSWIZB_FINISH);
	
	//-- for debug
	// pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT | PSWIZB_FINISH);
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
	//-- end for debug

	//-- for debug temp comment
	InitDialogItem();
	//-- end for debug temp comment

	//-- this is third time added begin
	
	// pSheet->SetFinishText("Restore");

	
	(pSheet->GetDlgItem(IDCANCEL))->GetWindowText(g_strTextCancel);
	
	CString strRestore;
	strRestore.LoadString(IDS_RESTORE);
	(pSheet->GetDlgItem(ID_WIZFINISH))->SetWindowText(strRestore);

	CString strClose;
	strClose.LoadString(IDS_CLOSE);
    
	(pSheet->GetDlgItem(IDCANCEL))->SetWindowText(strClose);

	(pSheet->GetDlgItem(IDHELP))->ShowWindow(FALSE);

	//-- this is third time added end
	
	return CPropertyPageEx::OnSetActive();
}


BOOL CWZStep4::OnInitDialog() 
{

	CString		csLine;
	CString		csHeading;
	LV_COLUMN	Column;
	CRect		ListRect;

	CPropertyPageEx::OnInitDialog();

	//-- load temp dir information begin
	// do not know MAX_PATH-1 or not, just use  MAX_PATH -1 for safe
	GetTempPath(MAX_PATH-1,g_RestoreInfo.szTempDir);
	CString strTempDir;
	strTempDir=g_RestoreInfo.szTempDir;
	strTempDir.TrimRight(L"\\");
	wcsncpy(g_RestoreInfo.szTempDir,(LPCTSTR)strTempDir,MAX_PATH-1);
	//-- load temp dir information end

    //-- the get more file will be placed in OnInitDialog begin
	
	// GetMoreFile(g_RestoreInfo.szImageFile);
	
	WCHAR szMainImageName[MAX_PATH];
	memset(szMainImageName,0,sizeof(szMainImageName));
	if( FALSE == GetLongMainName(g_RestoreInfo.szImageFile,szMainImageName) )
	{
		TRACE(L"\nGetLongMainName error in OnInitDialog.");
	}
	else
	{
		GetSeperateFile(g_RestoreInfo.szImageFile, szMainImageName, g_RestoreInfo.szTempDir);
	}
	
	//-- the get more file will be placed in OnInitDialog end

	//-- create image will be placed in oninitdialog begin
	
	// here we just use m_ImageList
	// we needn't use "new" to create a ImageList

	m_ImageList.Create(IDB_DRIVER, 16, 1, RGB (255, 0, 255));
	CBitmap FileBitmap;
	FileBitmap.LoadBitmap(IDB_FILE);
	m_ImageList.Add(&FileBitmap,RGB(255,0,255));
	//-- create image will be placed in oninitdialog end


	m_TimeStamp.GetClientRect(ListRect);

	Column.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
	Column.fmt = LVCFMT_LEFT;
	Column.cx  = ListRect.right-ListRect.left;
	// if (g_nRestoreType)	Column.cx /= 2;

	if(g_RestoreInfo.nRestoreType==RESTORE_PART)
	{
		Column.cx/=2;
	}

	csHeading.LoadString(IDS_TIME_STAMP);
	Column.pszText = csHeading.GetBuffer(0);
	Column.cchTextMax = 0;		// ignored
	Column.iSubItem = 0;		

	//-- for debug comment it
	// m_TimeStamp.InsertColumn(0,&Column);
	//--end for debug comment it
	
	switch (g_RestoreInfo.nRestoreType)
	{
	case RESTORE_ENTIRE:  // 0

	    // total restore, do not show the information of the tree
		m_DirTree.EnableWindow(FALSE);
		break;
	case RESTORE_PART: // 1

		// partial restore, show the information of the tree

		m_DirTree.EnableWindow(TRUE);
		csHeading.LoadString(IDS_STATE);
		Column.pszText = csHeading.GetBuffer(0);
		
		//-- for debug comment it
		// m_TimeStamp.InsertColumn(1,&Column);
		//-- end for debug comment it
		
		break;
	}
	// TODO: Add extra initialization here

	// now init the segment information

	DWORD dwSegmentSize;

	dwSegmentSize=SEGMENT_SIZE;
	
	g_SegmentInfo.dwCurrentOffset  = 0;
	g_SegmentInfo.dwCurrentSegment = 0;
    LoadSegment( g_RestoreInfo.szImageFile, g_SegmentInfo.PathArray,
		         g_SegmentInfo.dwCurrentSegment, dwSegmentSize );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CWZStep4::OnWizardFinish() 
{

	if(GetUIInfo()==FALSE)
	{
		return FALSE;
	}

	// this is a global function.
	// in fact it can be in the UI.
	int nTotalFileCount;

	nTotalFileCount=0;

	SetRestoreInfo(g_RestoreInfo);

	GetRestoreFileCount(g_RestoreInfo);

	//-- for debug temp comment begin
	// GetRestoreFileCount( (m_DirTree.m_StringFromFileArray),	(m_DirTree.m_dwFileAttributeArray), &nTotalFileCount );
	//-- for debug temp comment end

	CRestoreState  RestoreDlg;

	RestoreDlg.DoModal();

	return(FALSE);

	// return CPropertyPageEx::OnWizardFinish();
}


void CWZStep4::LoadTimeStamp(LPCTSTR szImageFile)
{
	__int64 lFilePointer;
	// DWORD dwTimeStamp;

	m_TimeStamp.DeleteAllItems();

	HANDLE hImageFile;
	DWORD dwReadBytes;
	hImageFile=CreateFile(szImageFile,GENERIC_READ,FILE_SHARE_READ,
		                  NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    
	if(hImageFile==INVALID_HANDLE_VALUE)
	{
		TRACE(L" open image file error.");
		return;
	}

	IMAGE_HEAD ImageHead;
	ReadFile(hImageFile,&ImageHead,sizeof(ImageHead),
		     &dwReadBytes,NULL);

	TOTAL_INDEX_NODE TotalIndexNode;

	YGSetFilePointer(hImageFile,ImageHead.TotalIndexPointer.QuadPart,FILE_BEGIN,lFilePointer);
	ReadFile(hImageFile,&TotalIndexNode,sizeof(TotalIndexNode),&dwReadBytes,NULL);

	YGSetFilePointer(hImageFile,ImageHead.StampPointer.QuadPart,FILE_BEGIN,lFilePointer);

	YGSetFilePointer(hImageFile,sizeof(STAMP_NODE),FILE_CURRENT,lFilePointer);

	STAMP_DATA StampData;
	SYSTEMTIME OneSystemTime;
		
	for(DWORD i=0; i<TotalIndexNode.dwTimeStampCount; i++)
	{
	  ReadFile(hImageFile,&StampData,sizeof(StampData),&dwReadBytes,NULL);
	  
	  CString strShowTime;
	  
	  OneSystemTime=StampData.StampTime;

	  strShowTime.Format(L"%.4d-%.2d-%.2d %.2d:%.2d:%.2d",
		                  OneSystemTime.wYear,
						  OneSystemTime.wMonth,
						  OneSystemTime.wDay,
						  OneSystemTime.wHour,
						  OneSystemTime.wMinute,
						  OneSystemTime.wSecond);

	  m_TimeStamp.InsertItem(i,strShowTime);
	}
	CloseHandle(hImageFile);
}


void CWZStep4::OnClickList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here

    DWORD dwPos = ::GetMessagePos();

    CPoint point ((int) LOWORD (dwPos), (int) HIWORD (dwPos));

    m_TimeStamp.ScreenToClient (&point);

    int nIndex;
    if ((nIndex = m_TimeStamp.HitTest (point)) != -1) 
	{
        CString string = m_TimeStamp.GetItemText (nIndex, 0);
        // TRACE (_T ("Number %d, %s was clicked\n"), nIndex,string);
		g_SelectedItem=nIndex;
    }

	*pResult = 0;
}


CString CWZStep4::GetItemPath()
{
    CString OnePathString;
	
	HTREEITEM hItem = m_DirTree.GetSelectedItem();
	OnePathString=m_DirTree.GetFullPath(hItem);

	OnePathString.TrimRight(L"\\");
	OnePathString=OnePathString+L"\\";

	return OnePathString;
}


void CWZStep4::ShowItemInfo(DWORD dwPathID)
{
    // if only selected a driver letter, dwPathID may be zero
	__int64 lFilePointer;
    CString TotalIndexFilePath;
    CString ImageFileName;
	ImageFileName=g_RestoreInfo.szImageFile;

	CString strTempDir;
	strTempDir=g_RestoreInfo.szTempDir;

    // GetImageFileName(g_RestoreInfo.szImageFile,ImageFileName.GetBuffer(MAX_PATH));
	GetLongMainName(g_RestoreInfo.szImageFile,ImageFileName.GetBuffer(MAX_PATH));
	ImageFileName.ReleaseBuffer();

	TotalIndexFilePath=strTempDir+L"\\"+ ImageFileName + AM_INDEX_SUFFIX;

    WIN32_FIND_DATA findFileData;

    HANDLE hFind = FindFirstFile((LPCTSTR)TotalIndexFilePath,
		                            &findFileData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		//-- for debug
		TRACE(L"Can't find total index file.\n");
		//-- end for debug
		return ;
	}

     HANDLE hTotalIndexFile;
     hTotalIndexFile=CreateFile((LPCTSTR)TotalIndexFilePath,GENERIC_READ,
	                            FILE_SHARE_READ,NULL,
							    OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,
							    NULL);

     DWORD dwReadBytes;

	 TOTAL_INDEX_NODE TotalIndexNode;
	 YGSetFilePointer(hTotalIndexFile,0,FILE_BEGIN,lFilePointer);
	 ReadFile(hTotalIndexFile,&TotalIndexNode,sizeof(TotalIndexNode),
		      &dwReadBytes,NULL);

	 INDEX_DATA IndexData;

     DWORD dwFileOffset;
     // DWORD dwTimeStampCount;

     CString strShowSize;
     CString strShowTime;
     CString strChangedOrNot;
     
	 if(dwPathID>0)
	 {
	   for(DWORD dwCurrentStamp=1; dwCurrentStamp<=TotalIndexNode.dwTimeStampCount; dwCurrentStamp++)
	   {
	     dwFileOffset=GetTotalIndexOffset(TotalIndexNode.dwTotalPathCount,dwCurrentStamp,dwPathID);
		 YGSetFilePointer(hTotalIndexFile,dwFileOffset,FILE_BEGIN,lFilePointer);
		 ReadFile(hTotalIndexFile,&IndexData,sizeof(IndexData),
			      &dwReadBytes,NULL);

		 if( IndexData.wStatusFlag != FILE_NOT_EXIST)
		 {
           // just show the file size that less then 2 GB.
		   // wait for improve
		   //-- for debug
		   strShowSize.Format(L"%d",IndexData.FileInfo.nFileSizeLow);
		   //-- end for debug

           FILETIME ftOneFileTime;
           ftOneFileTime = IndexData.FileInfo.ftLastWriteTime;
  
           FILETIME ftOneLocalFileTime;
           FileTimeToLocalFileTime(&ftOneFileTime,&ftOneLocalFileTime);

           SYSTEMTIME OneSysTime;
           FileTimeToSystemTime(&ftOneLocalFileTime,&OneSysTime);

           strShowTime.Format(L"%.4d-%.2d-%.2d %.2d:%.2d:%.2d",OneSysTime.wYear,OneSysTime.wMonth,OneSysTime.wDay,OneSysTime.wHour,OneSysTime.wMinute,OneSysTime.wSecond);

		 }

		 switch(IndexData.wStatusFlag)
		  {
	       case FILE_NEW:
 		     strChangedOrNot=L"Original";
		      break;
	       case FILE_NO_CHANGE:
  		      strChangedOrNot=L"Same";
              strShowSize.Empty(); 
              strShowTime.Empty();
		      break;
	       case FILE_CHANGE:
              strChangedOrNot=L"Changed";
		      break;
	       case DIRECTORY_NEW:
		      strChangedOrNot=L"Original";
              strShowSize.Empty(); // for directory, don't show size and time
              strShowTime.Empty();
		      break;
	       case DIRECTORY_EXIST:
		      strChangedOrNot=L"Exist";
              strShowSize.Empty();
              strShowTime.Empty();
		      break;
	       case FILE_NOT_EXIST:
		      strChangedOrNot=L"Not Exist";
              strShowSize.Empty();
              strShowTime.Empty();
		      break;
	       case RM_FILE_NEW:
 		      strChangedOrNot=L"RM Original";
		      break;

		   default:
		    ;
	   }
       
	   m_TimeStamp.SetItemText(dwCurrentStamp-1,1,strChangedOrNot);
       m_TimeStamp.SetItemText(dwCurrentStamp-1,2,strShowSize);
	   m_TimeStamp.SetItemText(dwCurrentStamp-1,3,strShowTime);

	   } // end for
	 }// end if dwPathID>p
	 else if(dwPathID==0)
	 {
	   for(DWORD dwCurrentStamp=1; dwCurrentStamp<=TotalIndexNode.dwTimeStampCount; dwCurrentStamp++)
	   {
	     strChangedOrNot.Empty();
         strShowSize.Empty();
         strShowTime.Empty();
	     m_TimeStamp.SetItemText(dwCurrentStamp-1,1,strChangedOrNot);
         m_TimeStamp.SetItemText(dwCurrentStamp-1,2,strShowSize);
	     m_TimeStamp.SetItemText(dwCurrentStamp-1,3,strShowTime);
	   } // end for
	 }

     CloseHandle(hTotalIndexFile);
}


int CWZStep4::GetSelectedItem()
{
   int nItem;

   POSITION pos = m_TimeStamp.GetFirstSelectedItemPosition();
   if (pos == NULL)
   {
       nItem=-1;
   }
   else
   {
     while (pos)
	 {
      nItem = m_TimeStamp.GetNextSelectedItem(pos);
	 }
   }

   return nItem;
}

void CWZStep4::SetEntireRestoreColumn()
{
    ClearAllColumn();
	CString strTimeStamp;
	strTimeStamp.LoadString(IDS_TIME_STAMP);
	// m_TimeStamp.InsertColumn(0,"Time Stamp",LVCFMT_LEFT,m_TimeStamp.GetStringWidth("Time Stamp    ") );
	m_TimeStamp.InsertColumn(0,strTimeStamp,LVCFMT_LEFT,m_TimeStamp.GetStringWidth(strTimeStamp + L"    ") );
}

void CWZStep4::SetPartRestoreColumn()
{
	ClearAllColumn();

	CString strTimeStamp;
	strTimeStamp.LoadString(IDS_TIME_STAMP);
	m_TimeStamp.InsertColumn(0,strTimeStamp,LVCFMT_LEFT,m_TimeStamp.GetStringWidth(strTimeStamp + L"    ") );

	// m_TimeStamp.InsertColumn(0,"Time Stamp",LVCFMT_LEFT,m_TimeStamp.GetStringWidth("Time Stamp   ") );

	CString strState;
	strState.LoadString(IDS_STATE);
	m_TimeStamp.InsertColumn(1,strState,LVCFMT_LEFT,m_TimeStamp.GetStringWidth(strState + L"    ") );
	
	// m_TimeStamp.InsertColumn(1,"State",LVCFMT_LEFT,m_TimeStamp.GetStringWidth("State   ") );
	
	CString strSize;
	strSize.LoadString(IDS_SIZE);
	m_TimeStamp.InsertColumn(2,strSize,LVCFMT_LEFT,m_TimeStamp.GetStringWidth( strSize + L"   ") );

	CString strModifyTime;
	strModifyTime.LoadString(IDS_MODIFY_TIME);
	m_TimeStamp.InsertColumn(3,strModifyTime,LVCFMT_LEFT,m_TimeStamp.GetStringWidth( strModifyTime + L"   ") );
}

void CWZStep4::LoadTree()
{
	// TODO: Add your control notification handler code here

	//-- create image will be placed in oninitdialog begin
	// m_ImageList.Create(IDB_DRIVER, 16, 1, RGB (255, 0, 255));
	// CBitmap FileBitmap;
	// FileBitmap.LoadBitmap(IDB_FILE);
	// m_ImageList.Add(&FileBitmap,RGB(255,0,255));
	//-- create image will be placed in oninitdialog end

	m_DirTree.SetImageList(&m_ImageList,TVSIL_NORMAL);
    m_DirTree.LoadTreeFromFile();
    if (m_DirTree.m_ExpandedNodeArray.GetSize()) m_DirTree.m_ExpandedNodeArray.RemoveAll();
}


void CWZStep4::InitDialogItem()
{
	m_DirTree.DeleteAllItems();
    ClearAllColumn();

	//-- for debug begin
	//-- now the getmorefile will be placed in initdialog
	// GetMoreFile(g_RestoreInfo.szImageFile);
	//-- for debug end

	GetSourceArray(g_RestoreInfo.szLastImageFile,g_ImageArray);

	if( RESTORE_ENTIRE == g_RestoreInfo.nRestoreType )
	{
		m_DirTree.EnableWindow(FALSE);
		SetEntireRestoreColumn();
		LoadTimeStamp(g_RestoreInfo.szImageFile);
	}
	else if( RESTORE_PART == g_RestoreInfo.nRestoreType)
	{
		m_DirTree.EnableWindow(TRUE);
		LoadTree();
		SetPartRestoreColumn();
		LoadTimeStamp(g_RestoreInfo.szImageFile);
	}
}

void CWZStep4::ClearFile()
{
  // TODO: Add your control notification handler code here
  CString strImageFilePath;
  CString strTargetDirectory;
  CString strTargetFileName;

  strImageFilePath=g_RestoreInfo.szImageFile;
  strTargetDirectory=g_RestoreInfo.szTempDir;
  // GetImageFileName(g_RestoreInfo.szImageFile,strTargetFileName.GetBuffer(MAX_PATH));
  GetLongMainName(g_RestoreInfo.szImageFile,strTargetFileName.GetBuffer(MAX_PATH));
  strTargetFileName.ReleaseBuffer();

  CString strTotalIndexFilePath;
  strTotalIndexFilePath=strTargetDirectory + L"\\"+ strTargetFileName + AM_INDEX_SUFFIX;

  CString strArrayFilePath;
  strArrayFilePath=strTargetDirectory + L"\\"+ strTargetFileName + AM_ARRAY_SUFFIX;

  DeleteFile( LPCTSTR(strTotalIndexFilePath) ); // TotalIndex file
  DeleteFile( LPCTSTR(strArrayFilePath) ); // array file
}

void CWZStep4::ClearAllColumn()
{
    m_TimeStamp.DeleteAllItems();
	for(int i=0; i<4; i++)
	{
	 int nReturnVal=m_TimeStamp.DeleteColumn(0);
	 if( nReturnVal==0 )
	 {
		 break;
	 }
	 else
	 {
 	    // MessageBox("You delete column");
	 }
	}
}

void CWZStep4::ShowStampInfo()
{
	// TODO: Add your control notification handler code here

	CString OnePathString;

	OnePathString=GetItemPath();

	//-- for debug
	// MessageBox("The PathString is:" + OnePathString);
	//-- end debug

	DWORD dwPathID;

	// dwPathID=GetPathID(OnePathString);

	//-- for debug
	DWORD dwFileAttribute=0x10;
	//-- end for debug

	//-- for debug temp comment begin
	// GetPathID((LPCTSTR)OnePathString,dwFileAttribute,(m_DirTree.m_StringFromFileArray),(m_DirTree.m_dwFileAttributeArray), &dwPathID);
	//-- for debug temp comment end

	NewGetPathID( g_RestoreInfo.szImageFile , (LPCTSTR)OnePathString, &dwPathID);

    //-- for debug
	// CString strResult;
	// strResult.Format("%d",dwPathID);
	// MessageBox("The PathID is: " + strResult);
	//-- end debug

	ShowItemInfo(dwPathID);
	
}


void CWZStep4::OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult) 
{  
	
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
    
    ShowStampInfo();
	
	*pResult = 0;
}



void CWZStep4::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class

#if 1
	ClearFile();
#endif

   CPropertyPageEx::PostNcDestroy();
}


IMPLEMENT_DYNCREATE(CDialogThread, CWinThread)
BOOL CDialogThread::InitInstance() 
{ 
	CRestoreState RestoreState;
	m_pMainWnd = &RestoreState;
	RestoreState.DoModal();
	return FALSE;
}



BOOL CWZStep4::GetUIInfo()
{

	if(GetSelectedItem()<0)
	{
		// MessageBox("Please Selected a TimeStamp");
		AfxMessageBox(IDS_NOSELECT_TIMESTAMP,MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}

	// fill the parameter for RestoreFile(RestoreInfo)
	g_RestoreInfo.dwTimeStamp = GetSelectedItem()+1;

	if(g_RestoreInfo.nRestoreType==RESTORE_PART)
	{
	    CString strSelectedPath;
		strSelectedPath=GetItemPath();
		if(strSelectedPath.GetLength()==0)
		{
		   // MessageBox("Please Select a Item to Restore");
		   AfxMessageBox(IDS_NO_ITEM_SELECTED,MB_OK|MB_ICONINFORMATION);
		   return FALSE;
		}
		wcsncpy(g_RestoreInfo.szSelectedPath , (LPCTSTR)strSelectedPath,MAX_PATH-1);
	}
	else
	{
	   wcsncpy(g_RestoreInfo.szSelectedPath,L"\0",MAX_PATH-1);
	}
	return TRUE;
}

BOOL LoadSegment(
				  LPCTSTR szImageFile, // this should be the last file
				  CPATH_ARRAY &PathArray,
				  DWORD dwSegmentNumber,
				  DWORD dwSegmentSize
				)
{
	// Certainly we should insure that 
	// need the RestoreInfo.szImageFile;
 	__int64 lFilePointer;
	// this is needed, since we often refresh PathArray
	if (PathArray.GetSize()) PathArray.RemoveAll();

	HANDLE hDataFile;
	hDataFile=CreateFile(szImageFile,
                        GENERIC_READ,FILE_SHARE_READ,NULL,
                        OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	// if(GetLastError()!=0)
	if(hDataFile == INVALID_HANDLE_VALUE)
	{
	   // if open data file error
	   // return
	   //-- for debug
	   TRACE(L"Open image file error.\n");
	   //-- end for debug
	   return FALSE;
	}
   
	DWORD dwReadBytes;

	// read image head
	IMAGE_HEAD ImageHead;
	YGSetFilePointer(hDataFile,0,FILE_BEGIN,lFilePointer);
	ReadFile(hDataFile,&ImageHead,sizeof(ImageHead),
	        &dwReadBytes,NULL);

   // read total index node
	TOTAL_INDEX_NODE TotalIndexNode;
	YGSetFilePointer(hDataFile,ImageHead.TotalIndexPointer.QuadPart,FILE_BEGIN,lFilePointer);
	ReadFile(hDataFile,&TotalIndexNode,sizeof(TotalIndexNode),
	        &dwReadBytes,NULL);

	if ( dwSegmentNumber * dwSegmentSize > TotalIndexNode.dwTotalPathCount )
	{
	   CloseHandle(hDataFile);
	   return FALSE;
	}

	DWORD dwMaxOffset;

	dwMaxOffset = TotalIndexNode.dwTotalPathCount - dwSegmentNumber * dwSegmentSize ;
   
	if( dwMaxOffset > dwSegmentSize )
	{
	   dwMaxOffset = dwSegmentSize;
	}

	// skip array node
	YGSetFilePointer(hDataFile,ImageHead.ArrayPointer.QuadPart,FILE_BEGIN,lFilePointer);
   
	YGSetFilePointer(hDataFile,sizeof(ARRAY_NODE),FILE_CURRENT,lFilePointer);
   
	YGSetFilePointer(hDataFile,sizeof(ARRAY_DATA)*dwSegmentNumber*dwSegmentSize,FILE_CURRENT,lFilePointer);

	ARRAY_DATA ArrayData;
   
	for(DWORD dwPathNumber=0; dwPathNumber<dwMaxOffset ; dwPathNumber++)
	{
	   ReadFile(hDataFile,&ArrayData,sizeof(ArrayData),&dwReadBytes,NULL);
	   PathArray.Add(ArrayData);
	}
   
	CloseHandle(hDataFile);

	return TRUE;

}


BOOL bPathInSegment(
					 LPCTSTR		SourceFilePath,
					 CPATH_ARRAY	&PathArray,
					 DWORD			dwBeginOffset,
					 DWORD			*pdwPathOffset
				   )
{

	CString FilePathString;
	FilePathString=SourceFilePath;

	// BOOL bReturnValue;

	// before compare, insure the path is ended with "\\"
	// if(FilePathString.Right(1)!="\\")
	// {
	     // FilePathString=FilePathString+"\\";
	// }

	BOOL bFound;
	bFound=FALSE;

	if(dwBeginOffset >= (DWORD)( PathArray.GetSize()) )
	{
		dwBeginOffset=0;
	}
	
	DWORD i;
	for(i=dwBeginOffset; i < (DWORD) (PathArray.GetSize()); i++)
	{
		if ( FilePathString.CompareNoCase( LPCTSTR (PathArray[i].szFilePath ) ) == 0 )
		{
			bFound=TRUE;
			break;
		}
	}

	if(bFound==FALSE)
	{
		for(i=0 ; i< dwBeginOffset; i++)
		{

			if ( FilePathString.CompareNoCase( LPCTSTR (PathArray[i].szFilePath ) ) == 0 )
			{
				bFound=TRUE;
				break;
			}
		}
	}


	if(bFound==TRUE)
	{
	    (*pdwPathOffset) = i;
    }
	else
	{
		(*pdwPathOffset) = 0;
	}

	return bFound;
}


BOOL NewGetPathID(LPCTSTR szImageFile, LPCTSTR szOnePathString, DWORD *pdwPathID)
{
	//-- for debug
	// AfxMessageBox("The PathString is:" + CString(OnePathString) ,MB_OK|MB_ICONINFORMATION,NULL	);
	//-- end debug

	CString OnePathString;
	
	OnePathString=szOnePathString;
	
	OnePathString.TrimRight(L"\\");

	BOOL bPathFound;

	bPathFound=FALSE;

	DWORD dwSegmentNumber;
	dwSegmentNumber=0;

	DWORD dwSegmentSize;
	dwSegmentSize=SEGMENT_SIZE;

	DWORD dwPathIndex;

	// first found in the current segment
	// if not found, then found in other segment

	DWORD dwBeginOffset;
	dwBeginOffset=0;
	bPathFound = bPathInSegment(
								 (LPCTSTR)OnePathString,
								 g_SegmentInfo.PathArray,
								 dwBeginOffset,
								 &dwPathIndex
							   );

	if(bPathFound==TRUE)
	{
		CString strPathIndex;

		strPathIndex.Format(L"Segment: %d, PathOffset: %d, PathIndex: %d",
							 g_SegmentInfo.dwCurrentSegment,
							 g_SegmentInfo.dwCurrentOffset,
							 g_SegmentInfo.dwCurrentSegment * dwSegmentSize + dwPathIndex);

		g_SegmentInfo.dwCurrentOffset = dwPathIndex;
		
		// AfxMessageBox( "Found in current segment, place " + strPathIndex ,MB_OK|MB_ICONINFORMATION,NULL);

		(*pdwPathID)=g_SegmentInfo.dwCurrentSegment * dwSegmentSize + dwPathIndex + 1;
		
	}


	DWORD dwCurrentSegment;
	dwCurrentSegment=g_SegmentInfo.dwCurrentSegment;

     if(bPathFound == FALSE)
	 {
        dwSegmentNumber = dwCurrentSegment + 1;

		do
		{
			 LoadSegment(   szImageFile, 
							g_SegmentInfo.PathArray,
							dwSegmentNumber,
							dwSegmentSize );

			 g_SegmentInfo.dwCurrentSegment = dwSegmentNumber;

			 DWORD dwBeginOffset;
			 dwBeginOffset=0;
			 bPathFound = bPathInSegment(
										   (LPCTSTR)OnePathString,
										   g_SegmentInfo.PathArray,
										   dwBeginOffset,
										   &dwPathIndex
										);

			 if(bPathFound == TRUE)
			 {

				CString strPathIndex;

				strPathIndex.Format(L"Segment: %d, PathOffset: %d, PathIndex: %d",
									 g_SegmentInfo.dwCurrentSegment,
									 g_SegmentInfo.dwCurrentOffset,
									 g_SegmentInfo.dwCurrentSegment * dwSegmentSize + dwPathIndex);

				g_SegmentInfo.dwCurrentSegment = dwSegmentNumber;

				g_SegmentInfo.dwCurrentOffset = dwPathIndex;
				
				// AfxMessageBox( "Found in place " + strPathIndex,MB_OK|MB_ICONINFORMATION,NULL);

				(*pdwPathID)=g_SegmentInfo.dwCurrentSegment * dwSegmentSize + dwPathIndex + 1;
		
				break;

			 }
			
			 dwSegmentNumber++;

		}while(g_SegmentInfo.PathArray.GetSize()>0);
	}


	if(bPathFound == FALSE)
	{

	  for( dwSegmentNumber = 0;
		   dwSegmentNumber < dwCurrentSegment;
		   dwSegmentNumber ++ )
	  {

		   LoadSegment(g_RestoreInfo.szImageFile, g_SegmentInfo.PathArray,
					   dwSegmentNumber,dwSegmentSize);
		   
		   g_SegmentInfo.dwCurrentSegment = dwSegmentNumber;

		   DWORD dwBeginOffset;
		   dwBeginOffset=0;
		   bPathFound = bPathInSegment(
										 (LPCTSTR)OnePathString,
										 g_SegmentInfo.PathArray,
										 dwBeginOffset,
										 &dwPathIndex
									   );

			if(bPathFound == TRUE)
			{

				CString strPathIndex;

				strPathIndex.Format(L"Segment: %d, PathOffset: %d, PathIndex: %d",
									 g_SegmentInfo.dwCurrentSegment,
									 g_SegmentInfo.dwCurrentOffset,
									 g_SegmentInfo.dwCurrentSegment * dwSegmentSize + dwPathIndex);

				g_SegmentInfo.dwCurrentSegment = dwSegmentNumber;

				g_SegmentInfo.dwCurrentOffset = dwPathIndex;
				
				// AfxMessageBox( "Found in place " + strPathIndex,MB_OK|MB_ICONINFORMATION,NULL);

				(*pdwPathID)=g_SegmentInfo.dwCurrentSegment * dwSegmentSize + dwPathIndex + 1;

				break;
			}
	  }
	}


	if(bPathFound==TRUE)
	{
	   return TRUE;
	}
	else
	{
		(*pdwPathID)=0;
		return FALSE;
	}

}


BOOL GetPathCount(LPCTSTR szImageFile, DWORD *pdwPathCount)
{
	__int64 lFilePointer;
	   HANDLE hDataFile;

	   hDataFile=CreateFile(szImageFile,
							GENERIC_READ,
							FILE_SHARE_READ,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL);

	   if(hDataFile == INVALID_HANDLE_VALUE)
	   {
		   // if open data file error
		   // return

		   //-- for debug
		   TRACE(L"Open image file error.\n");
		   //-- end for debug

		   (*pdwPathCount)=0;

		   return FALSE;
	   }
   
	   DWORD dwReadBytes;

	   // read image head
	   IMAGE_HEAD ImageHead;
	   YGSetFilePointer(hDataFile,0,FILE_BEGIN,lFilePointer);
	   ReadFile(hDataFile,&ImageHead,sizeof(ImageHead),
				&dwReadBytes,NULL);

	   // read total index node
	   TOTAL_INDEX_NODE TotalIndexNode;
	   YGSetFilePointer(hDataFile,ImageHead.TotalIndexPointer.QuadPart,FILE_BEGIN,lFilePointer);
	   ReadFile(hDataFile,&TotalIndexNode,sizeof(TotalIndexNode),
				&dwReadBytes,NULL);

	   (*pdwPathCount)=TotalIndexNode.dwTotalPathCount;

	   CloseHandle(hDataFile);

	   return TRUE;
}

LRESULT CWZStep4::OnWizardBack() 
{
	// TODO: Add your specialized code here and/or call the base class

	CPropertySheet* pSheet = (CPropertySheet*)GetParent();
	ASSERT_KINDOF(CPropertySheet, pSheet);

	(pSheet->GetDlgItem(IDCANCEL))->SetWindowText(g_strTextCancel);
	
	return CPropertyPageEx::OnWizardBack();
}

void CWZStep4::OnCancel() 
{
	// TODO: Add your specialized code here and/or call the base class
	FreeAMDataArray(g_ImageArray);
	if (g_SegmentInfo.PathArray.GetSize()) g_SegmentInfo.PathArray.RemoveAll();

	CPropertyPageEx::OnCancel();
}
