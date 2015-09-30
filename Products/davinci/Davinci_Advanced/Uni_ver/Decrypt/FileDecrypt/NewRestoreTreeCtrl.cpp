// NewRestoreTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "NewRestoreTreeCtrl.h"
#include "..\DecryptFunction\DecryptFunction.h"
#include "..\..\DavinciFunction\DavinciFunction.h"
#include "OleDropSourcePro.h"
#include "EraseDirectory.h"
#include "OleDataSourcePro.h"
#include "OleDropSourcePro.h"

UINT g_uCustomClipbrdFormat = RegisterClipboardFormat ( _T("MultiFiler_A812EB7F_889D_4076_96F0_B6FBE3EA9BDC") );

HGLOBAL  g_hgDrop;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString g_strDraggedItem;

extern DECRYPT_INFO g_DecryptInfo;

/////////////////////////////////////////////////////////////////////////////
// CNewRestoreTreeCtrl

CNewRestoreTreeCtrl::CNewRestoreTreeCtrl()
{
	m_TreeSegment.PathArray.RemoveAll();
}

CNewRestoreTreeCtrl::~CNewRestoreTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CNewRestoreTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CNewRestoreTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpanded)
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBegindrag)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewRestoreTreeCtrl message handlers


BOOL CNewRestoreTreeCtrl::LoadTreeFromFile(LPCTSTR szImageFile)
{

    DeleteAllItems();

	CString ElementString;

	m_WorkStringArray.RemoveAll();

	DWORD dwSegmentNumber;
	DWORD dwSegmentSize;
	
	dwSegmentNumber = 0;
	dwSegmentSize=SEGMENT_SIZE;

    int nArraySize;

	do
	{

		 if( FALSE == LoadSegment(  szImageFile,
									m_TreeSegment.PathArray,
									dwSegmentNumber,
									dwSegmentSize ) )
		 {
			 TRACE(L"\nLoadSegment error in LoadTreeFromFile.");
			 return FALSE;
		 }

		 m_TreeSegment.dwCurrentSegment = dwSegmentNumber;

		 nArraySize = m_TreeSegment.PathArray.GetSize();

		 for(int i = 0; i < nArraySize; i++)
		 {

			   // ElementString = m_TreeSegment.PathArray[i].szFilePath;
			   ElementString = m_TreeSegment.PathArray[i].szFileName;
			   ElementString = ElementString.Left(2);

			   if(FoundInWorkStringArray(ElementString)==FALSE)
			   {
				   m_WorkStringArray.Add(ElementString);
				   HTREEITEM hParent=InsertItem(ElementString,0,0,TVI_ROOT);
				   
				   InsertItem(L"TempItem",6,6,hParent);
			   }
		 }

		 dwSegmentNumber++;

	} while(nArraySize>0);

	return TRUE;
}


void CNewRestoreTreeCtrl::AddSelectedItem(HTREEITEM hItem)
{
  
  if(HaveShortPath(hItem)==FALSE)
	  m_SelectedNodeArray.Add(GetFullPath(hItem)+L"\\");
  
}


BOOL CNewRestoreTreeCtrl::HaveShortPath(HTREEITEM hItem)
{
  BOOL HaveShortPathOrNot;

  CString ItemPath;

  long i,ArraySize;

  int ElementLen;

  ItemPath=GetFullPath(hItem)+L"\\";

  HaveShortPathOrNot=FALSE;

  ArraySize=m_SelectedNodeArray.GetSize();

  for(i=0;i<ArraySize;i++)
  {
	  ElementLen=m_SelectedNodeArray.ElementAt(i).GetLength();

	  if(m_SelectedNodeArray.ElementAt(i)==ItemPath.Left(ElementLen))
	  {
		  HaveShortPathOrNot=TRUE;
		  break;
	  }
	  else
	  {
	  }
  }
  
  return HaveShortPathOrNot;
}


CString CNewRestoreTreeCtrl::GetFullPath(HTREEITEM hItem)
{
	//-- get the Full Path of the item
	//-- return value haven't "\\" in the end.

	CString strReturn;
	CString strTemp;
	HTREEITEM hParent = hItem;

	strReturn = L"";

	while ( hParent )
	{
		
		strTemp  = GetItemText( hParent );
		strTemp += L"\\";
		strReturn = strTemp + strReturn;
		hParent = GetParentItem( hParent );
	}
    
	strReturn.TrimRight( L'\\' );

    return strReturn;

}


BOOL CNewRestoreTreeCtrl::FoundInWorkStringArray(CString OneString)
{
	CString FoundString;
	
	FoundString=OneString;
	
	for(int i=0; i<m_WorkStringArray.GetSize(); i++)
	{
		if(FoundString==m_WorkStringArray.ElementAt(i))
		{
			return TRUE;
			break;
		}
	}
	return FALSE;
}


void CNewRestoreTreeCtrl::ExpandItem(HTREEITEM hItem, UINT nCode)
{	
	m_WorkStringArray.RemoveAll();
	
	if ( TVE_EXPAND == nCode )
	{
		if(HaveExpanded(hItem)==TRUE)
		{
		}
		else
		{
		  
		  HTREEITEM hChild = GetChildItem( hItem );

		  if(hChild!=NULL)
		  {
			 DeleteItem(hChild);
		  }
		
		  while ( hChild )
		  {

			DeleteItem( hChild );
			hChild = GetChildItem( hItem );
		  }
		
		  DisplayPath(hItem);

		}

	}
}


BOOL CNewRestoreTreeCtrl::HaveExpanded(HTREEITEM hItem)
{
  BOOL HaveExpandedOrNot;

  CString ItemPath;

  ItemPath=GetFullPath(hItem);

  long i,ArraySize;

  HaveExpandedOrNot=FALSE;

  ArraySize=m_ExpandedNodeArray.GetSize();

  for(i=0;i<ArraySize;i++)
  {
	  if(m_ExpandedNodeArray.ElementAt(i)==ItemPath)
	  {
		  HaveExpandedOrNot=TRUE;
		  break;
	  }
	  else
	  {
	  }
  }
  
  return HaveExpandedOrNot;
}


void CNewRestoreTreeCtrl::DisplayPath(HTREEITEM hParent)
{
	NewFillWorkStringArray(g_DecryptInfo.szImageFile,hParent);
    
	for(int i=0; i<m_WorkStringArray.GetSize(); i++)
	{
	   CString OnePartString;

	   OnePartString=m_WorkStringArray.ElementAt(i);

       if(OnePartString.Right(2)==L"**")
	   {
		   // one dir, have sub dir or sub files.
		   OnePartString.TrimRight(L"*");
		   OnePartString.TrimRight(L"*");
		   HTREEITEM hRecordItem=InsertItem(OnePartString,4,4,hParent);
           InsertItem(L"TempString",6,6,hRecordItem);
	   }
       else if(OnePartString.Right(1)==L"*")
	   {
		   OnePartString.TrimRight(L"*");
		   HTREEITEM hRecordItem=InsertItem(OnePartString,4,4,hParent);
	   }
	   else
	   {
		   InsertItem(OnePartString,6,6,hParent);
	   }

	}

}



CString CNewRestoreTreeCtrl::GetLeftOnePart(CString OneString)
{
	CString LeftOnePart;
	int i=0;


	i=OneString.Find(L"\\");
    LeftOnePart=OneString.Left(i);
	return LeftOnePart;
}


void CNewRestoreTreeCtrl::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	// TODO: Add your control notification handler code here

    HTREEITEM hExpandedItem;
    CString ExpandedPath;

	hExpandedItem=pNMTreeView->itemNew.hItem;
	if ( pNMTreeView->itemNew.state & TVIS_EXPANDED )
	{
		ExpandItem(hExpandedItem, TVE_EXPAND );
		ExpandedPath=GetFullPath(hExpandedItem);
		m_ExpandedNodeArray.Add(ExpandedPath);

	}

	*pResult = 0;
}



void CNewRestoreTreeCtrl::AddPathToWorkingArray(
													CString			ExpandPath,
													CString			OnePath,
													DWORD			dwFileAttribute,
													CStringArray	&WorkingStringArray,
													BOOL			*pbHaveSubItem
											   )
{

	int PathStringLength;

	(*pbHaveSubItem)=FALSE;

	PathStringLength=ExpandPath.GetLength();

	if ( OnePath.Left(PathStringLength) == ExpandPath )
	{
		//-- Mid is Zero Based, then it jumps over the "\"

		CString OnePartString,RightPartString;

		RightPartString=OnePath.Mid(PathStringLength);

		if(RightPartString.GetLength()>0)
		{
			OnePartString=GetLeftOnePart(RightPartString);

			RightPartString=RightPartString.Mid(OnePartString.GetLength()+1);

			if(RightPartString.GetLength()==0)
			{
				CString NextElementString;

				if( (  dwFileAttribute & FILE_ATTRIBUTE_DIRECTORY) !=0)
				{
					if(TRUE)
					{
						   // a dir, and have sub dir
						OnePartString=OnePartString+L"**";
					}
					else
					{
							// a dir, and have no sub dir
						OnePartString=OnePartString+L"*";
					}
				}
			  
				WorkingStringArray.Add(OnePartString);
			}
		}

		(*pbHaveSubItem)=TRUE;

	}
}


void CNewRestoreTreeCtrl::NewFillWorkStringArray(LPCTSTR szImageFile,HTREEITEM hParent)
{

	CString PathString,ElementString;

	PathString=GetFullPath(hParent)+L"\\";
	long PathStringLength;
	PathStringLength=PathString.GetLength();

	m_WorkStringArray.RemoveAll();

	BOOL bHaveSubItem;
	
	// in fact, bHaveSubItem is not used.

	bHaveSubItem=FALSE;

	DWORD dwSegmentNumber;
	DWORD dwSegmentSize;
	
	dwSegmentNumber = 0;
	dwSegmentSize=SEGMENT_SIZE;

	do
	{
		 if( FALSE == LoadSegment(
								  szImageFile, 
								  m_TreeSegment.PathArray,
								  dwSegmentNumber,
								  dwSegmentSize
								 ) )
		 {
			 TRACE(L"\nLoadSegment error in NewFillWorkStringArray.");
			 break;
		 }

		 m_TreeSegment.dwCurrentSegment = dwSegmentNumber;

		 for(int i = 0; i < m_TreeSegment.PathArray.GetSize(); i++)
		 {
			  // ElementString = m_TreeSegment.PathArray[i].szFilePath;
			  ElementString = m_TreeSegment.PathArray[i].szFileName;

			  ElementString=ElementString + L"\\";

			  DWORD dwFileAttribute;
			  dwFileAttribute=m_TreeSegment.PathArray[i].dwFileAttribute;
		      
			  AddPathToWorkingArray(PathString,ElementString,dwFileAttribute,m_WorkStringArray,&bHaveSubItem);
		 }

		 dwSegmentNumber++;

	} while( m_TreeSegment.PathArray.GetSize()>0 );
}


// 2004.08.26 huapage modify begin

BOOL CNewRestoreTreeCtrl::LoadSegment(
										  LPCTSTR szImageFile, // this should be the last file
										  CPATH_ARRAY &PathArray,
										  DWORD dwSegmentNumber,
										  DWORD dwSegmentSize
									 )
{
	// Certainly we should insure that 
	// need the RestoreInfo.szImageFile;

	// this is needed, since we often refresh PathArray
	PathArray.RemoveAll();

   HANDLE hDataFile;
   hDataFile=CreateFile(szImageFile,
                        GENERIC_READ,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,
                        OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

   if( INVALID_HANDLE_VALUE == hDataFile )
   {
	   // if open data file error
	   // return
	   //-- for debug
	   TRACE(L"Open image file error in LoadSegment.\n");
	   //-- end for debug
	   return FALSE;
   }
   
   DWORD dwReadBytes;

   FILE_ENCRYPT_HEAD FileEncryptHead;
   
   __int64 linTargetFilePointer;
   ULONG   ulImageVersion;

   ulImageVersion = GetFileEncryptHead(hDataFile,g_DecryptInfo.szPassword,FileEncryptHead);

   switch (ulImageVersion)
   {
   case 100:
	    if(FALSE == YGSetFilePointer( hDataFile,
									  ((FILE_ENCRYPT_HEAD_100 *)&FileEncryptHead)->FileInfoPointer.QuadPart,
									  FILE_BEGIN,
									  linTargetFilePointer
								     ))
		{
			TRACE(L"\n2. YGSetFilePointer error in LoadSegment.");
			CloseHandle(hDataFile);
			return FALSE;
		}
	    break;
	case 0x200:
   case CURRENT_FILE_VERSION:
	    if(FALSE == YGSetFilePointer( hDataFile,
									  FileEncryptHead.FileInfoPointer.QuadPart,
									  FILE_BEGIN,
									  linTargetFilePointer
								     ))
		{
			TRACE(L"\n2. YGSetFilePointer error in LoadSegment.");
			CloseHandle(hDataFile);
			return FALSE;
		}
	    break;
   default:
		CloseHandle(hDataFile);
		return FALSE;
		break;
   }

   ARRAY_NODE ArrayNode;

   if( FALSE == ReadFile(hDataFile,&ArrayNode,sizeof(ArrayNode),
	                     &dwReadBytes,NULL) )
   {
	   TRACE(L"\nReadFile error in LoadSegment.");
   }

   if ( dwSegmentNumber * dwSegmentSize > ArrayNode.dwPathCount )
   {
	   // finish reading the file
	   CloseHandle(hDataFile);
	   return TRUE;
   }

   DWORD dwMaxOffset;

   dwMaxOffset = ArrayNode.dwPathCount - dwSegmentNumber * dwSegmentSize ;
   
   if( dwMaxOffset > dwSegmentSize )
   {
	   dwMaxOffset = dwSegmentSize;
   }

   if( FALSE == YGSetFilePointer(hDataFile,(__int64)(sizeof(ARRAY_DATA))*dwSegmentNumber*dwSegmentSize,FILE_CURRENT,linTargetFilePointer) )
   {
	   TRACE(L"\n3. YGSetFilePointer error in LoadSegment.");
   }

   DWORD dwPathIndex;
   for(dwPathIndex=1; dwPathIndex<=dwMaxOffset; dwPathIndex++)
   {
	   ARRAY_DATA ArrayData;
	   if( FALSE == ReadFile(hDataFile,&ArrayData,sizeof(ArrayData),&dwReadBytes,NULL) )
	   {
		   TRACE(L"\nRead array data error in LoadSegment.");
		   break;
	   }

	   // decrypt array data begin
	   char chPassward[MAX_PATH];
	   ZeroMemory(chPassward, sizeof(chPassward));
	   WideCharToMultiByte(CP_ACP,0,g_DecryptInfo.szPassword,wcslen(g_DecryptInfo.szPassword), 
		   chPassward, MAX_PATH, NULL, NULL);
	   if( FALSE == YGAESDecryptData((unsigned char*)chPassward,strlen(chPassward),(unsigned char*)(&ArrayData),(unsigned char*)(&ArrayData),sizeof(ArrayData)) )
	   {
		   TRACE(L"\nYGAESDecryptData error in DecryptSelectionFile."); 
		   return FALSE;
	   }
	   // decrypt array data end

	   // decrypt array data begin
	   //if( FALSE == YGAESDecryptData((unsigned char*)g_DecryptInfo.szPassword,strlen(g_DecryptInfo.szPassword),(unsigned char*)(&ArrayData),(unsigned char*)(&ArrayData),sizeof(ArrayData)) )
	   //{
	 	 //  TRACE(L"\nYGAESDecryptData error in DecryptSelectionFile."); 
		  // return FALSE;
	   //}
	   // decrypt array data end

	   PathArray.Add(ArrayData);
   }
   
   CloseHandle(hDataFile);

   return TRUE;
}

// 2004.08.26 huapage modify end

void CNewRestoreTreeCtrl::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	HTREEITEM hTSelItem = pNMTreeView->itemNew.hItem;
	// Highlight selected item
	SelectItem(hTSelItem);
	Select(hTSelItem, TVGN_DROPHILITE);

	// 2004.09.10 added begin

	CString strDraggedItem;

	strDraggedItem = GetFullPath(hTSelItem);

	g_strDraggedItem = strDraggedItem;
	
	COleDataSourcePro   datasrc;

	FORMATETC           etc = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

	datasrc.DelayRenderData(CF_HDROP,&etc);

    // Add in our own custom data, so we know that the drag originated from our 
    // window.  CMyDropTarget::DragEnter() checks for this custom format, and
    // doesn't allow the drop if it's present.  This is how we prevent the user
    // from dragging and then dropping in our own window.
    // The data will just be a dummy bool.
	
	HGLOBAL hgBool;
	
    hgBool = GlobalAlloc ( GHND | GMEM_SHARE, sizeof(bool) );
	
    if ( NULL == hgBool )
	{
        return;
	}
	
    // Put the data in the data source.
	
    etc.cfFormat = g_uCustomClipbrdFormat;
    
    datasrc.CacheGlobalData ( g_uCustomClipbrdFormat, hgBool, &etc );
	
    // Start the drag 'n' drop!

	COleDropSourcePro DropSource;

	DROPEFFECT dwEffect = datasrc.DoDragDrop ( DROPEFFECT_COPY | DROPEFFECT_MOVE,NULL,(COleDropSource *) (&DropSource) );
	
    // If the DnD completed OK, we remove all of the dragged items from our
    // list.
	
    switch ( dwEffect )
	{
	case DROPEFFECT_COPY:
	case DROPEFFECT_MOVE:
        break;
	case DROPEFFECT_NONE:
        break;  // end case DROPEFFECT_NONE
	}   // end switch

	// 2004.09.10 second added end

	*pResult = 0;
}
