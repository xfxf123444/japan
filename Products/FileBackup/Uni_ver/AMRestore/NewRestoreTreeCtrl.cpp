// NewRestoreTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "AMRestor.h"
#include "NewRestoreTreeCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern AM_RESTORE_INFO g_RestoreInfo;

//-- function from the other file begin

BOOL LoadSegment(
				  LPCTSTR		szImageFile, // this should be the last file
				  CPATH_ARRAY	&PathArray,
				  DWORD			dwSegmentNumber,
				  DWORD			dwSegmentSize
				);

//-- function from the other file end

void AddPathToWorkingArray(
							CString			ExpandPath,
							CString			OnePath,
							DWORD			dwFileAttribute,
							CStringArray	&WorkingStringArray,
							BOOL			*pbHaveSubItem
					      );

/////////////////////////////////////////////////////////////////////////////
// CNewRestoreTreeCtrl

CNewRestoreTreeCtrl::CNewRestoreTreeCtrl()
{
	if (m_TreeSegment.PathArray.GetSize()) m_TreeSegment.PathArray.RemoveAll();
}

CNewRestoreTreeCtrl::~CNewRestoreTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CNewRestoreTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CNewRestoreTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpanded)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewRestoreTreeCtrl message handlers


void CNewRestoreTreeCtrl::LoadTreeFromFile()
{

    DeleteAllItems();


	CString ElementString;

	if (m_WorkStringArray.GetSize()) m_WorkStringArray.RemoveAll();

	DWORD dwSegmentNumber;
	DWORD dwSegmentSize;
	
	dwSegmentNumber = 0;
	dwSegmentSize=SEGMENT_SIZE;

	do
	{
		 LoadSegment(   g_RestoreInfo.szImageFile, 
						m_TreeSegment.PathArray,
						dwSegmentNumber,
						dwSegmentSize );

		 m_TreeSegment.dwCurrentSegment = dwSegmentNumber;

		 for(int i = 0; i < m_TreeSegment.PathArray.GetSize(); i++)
		 {

			   ElementString = m_TreeSegment.PathArray[i].szFilePath;
			   ElementString = ElementString.Left(2);

			   if(FoundInWorkStringArray(ElementString)==FALSE)
			   {
				   m_WorkStringArray.Add(ElementString);
				   HTREEITEM hParent=InsertItem(ElementString,0,0,TVI_ROOT);
				   
				   InsertItem(L"TempItem",6,6,hParent);
			   }
		 }

		 dwSegmentNumber++;

	} while(m_TreeSegment.PathArray.GetSize()>0);

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
	if (m_WorkStringArray.GetSize()) m_WorkStringArray.RemoveAll();
	
	if ( nCode == TVE_EXPAND )
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

	// Displaying the Path in the TreeCtrl
    //-- for debug

    // CString ActionString;
    // DWORD BeginTime;
    // DWORD FinishTime;
    // DWORD MiddleTime;

    // ActionString="Fill Work String Array";
  
    // MessageBox("Begin "+ActionString);
  
    // TRACE("\n%s\n","Begin "+ActionString);
  
    // BeginTime=GetTickCount();

    // -- end debug

	// FillWorkStringArray(g_RestoreInfo.szImageFile,hParent);

	NewFillWorkStringArray(g_RestoreInfo.szImageFile,hParent);

    //-- for debug

    // FinishTime=GetTickCount();
    // MiddleTime=FinishTime-BeginTime;

	// CString strOutPut;

	// strOutPut.Format("%d",MiddleTime);
  
    // TRACE("\n%s\n",ActionString+" used "+ strOutPut + " milliseconds");

    //-- end debug

    
	//-- for debug

    // ActionString="Insert Item";
 
    //MessageBox("Begin "+ActionString);
  
    // TRACE("\n%s\n","Begin "+ActionString);
  
    // BeginTime=GetTickCount();

    //-- end debug

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

     //-- for debug

     // FinishTime=GetTickCount();
     // MiddleTime=FinishTime-BeginTime;

	 // CString strUsedTime;
	 
	 // strUsedTime.Format("%d",MiddleTime);
    
     // TRACE("\n%s\n",ActionString+" used "+ strUsedTime + " millisecond");
     //-- end debug

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

	
	CString strPath;

	hExpandedItem=pNMTreeView->itemNew.hItem;

	if ( pNMTreeView->itemNew.state & TVIS_EXPANDED )
	{
		//-- for debug

         // CTime WorkTime;
         CString ActionString;
         // CTime BeginTime;
         // CTime FinishTime;
         // CTimeSpan MiddleTime;

         DWORD BeginTime;
         DWORD FinishTime;
         DWORD MiddleTime;

         ActionString=L"Expand a item";
         
		 //MessageBox(L"Begin "+ActionString);
		 
		 TRACE(L"\n%s\n","Begin "+ActionString);
         
		 BeginTime=GetTickCount();

         //-- end debug
         
		 ExpandItem(hExpandedItem, TVE_EXPAND );

         //-- for debug

         FinishTime=GetTickCount();

         MiddleTime=FinishTime-BeginTime;
  
         // MessageBox(ActionString+" used "+IntToString(MiddleTime)+ " seconds");
		 
		 CString strUsedTime;
		 strUsedTime.Format(L"%d",MiddleTime);
		 TRACE(L"\n%s\n",ActionString+L" used "+ strUsedTime + L" milliseconds");
         
		 //-- end debug

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
						//-- for debug temp comment begin
						// if(bHaveSubDirNew(OnePath)==TRUE)
						//-- for debug temp comment end
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


void CNewRestoreTreeCtrl::NewFillWorkStringArray(LPCTSTR szImageFile, HTREEITEM hParent)
{

	CString PathString,ElementString;

	PathString=GetFullPath(hParent)+L"\\";
	long PathStringLength;
	PathStringLength=PathString.GetLength();

	if (m_WorkStringArray.GetSize()) m_WorkStringArray.RemoveAll();

	BOOL bHaveSubItem;
	
	// in fact, bHaveSubItem is not used.

	bHaveSubItem=FALSE;

	DWORD dwSegmentNumber;
	DWORD dwSegmentSize;
	
	dwSegmentNumber = 0;
	dwSegmentSize=SEGMENT_SIZE;

	do
	{
		 LoadSegment(
			          szImageFile, 
					  m_TreeSegment.PathArray,
					  dwSegmentNumber,
					  dwSegmentSize
					);

		 m_TreeSegment.dwCurrentSegment = dwSegmentNumber;

		 for(int i = 0; i < m_TreeSegment.PathArray.GetSize(); i++)
		 {
			  ElementString = m_TreeSegment.PathArray[i].szFilePath;


			  ElementString=ElementString + L"\\";

			  DWORD dwFileAttribute;
			  dwFileAttribute=m_TreeSegment.PathArray[i].dwFileAttribute;
		      
			  AddPathToWorkingArray(PathString,ElementString,dwFileAttribute,m_WorkStringArray,&bHaveSubItem);
		 }

		 dwSegmentNumber++;

	} while( m_TreeSegment.PathArray.GetSize()>0 );
}
