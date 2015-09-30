// OleDropSourcePro.cpp: implementation of the COleDropSourcePro class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "OleDropSourcePro.h"
#include "..\DecryptFunction\DecryptFunction.h"
#include "DecryptStatus.h"
#include "EraseDirectory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CString g_strDraggedItem;
extern DECRYPT_INFO g_DecryptInfo;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COleDropSourcePro::COleDropSourcePro()
{
	
}

COleDropSourcePro::~COleDropSourcePro()
{
	
}

SCODE COleDropSourcePro::QueryContinueDrag(BOOL bEscapePressed, DWORD dwKeyState)
{
	ASSERT_VALID(this);
	
	// TRACE(L"\nQueryContinueDrag.");
	
	// check escape key or right button -- and cancel
	if (bEscapePressed || (dwKeyState & m_dwButtonCancel) != 0)
	{
		m_bDragStarted = FALSE; // avoid unecessary cursor setting
		return DRAGDROP_S_CANCEL;
	}
	
	// check left-button up to end drag/drop and do the drop
	if ((dwKeyState & m_dwButtonDrop) == 0)
	{
		
		TRACE(L"\nDrag finished ?.");
		
		if(m_bDragStarted)
		{
			
			TRACE(L"\nDrag and drop finished, the dragged item is %s.",(LPCTSTR)g_strDraggedItem);
			
			// 2004.09.10 added begin
			DECRYPT_INFO DecryptInfo;
			memcpy(&DecryptInfo,&g_DecryptInfo,sizeof(DECRYPT_INFO));
			
			DecryptInfo.nRestoreType = DECRYPT_PART;
			DecryptInfo.nTargetType = DECRYPT_NEW_PLACE;
			wcsncpy(DecryptInfo.szSelectedPath,(LPCTSTR)g_strDraggedItem,MAX_PATH-1);
			GetTempPath(MAX_PATH-1,DecryptInfo.szTargetPath);
			
			// 2004.09.15 added begin
			// this can delete the duplicated file in the temp directory
			CString strSelectedPath;
			strSelectedPath = DecryptInfo.szSelectedPath;
			strSelectedPath.TrimRight(L'\\');
			int nIndex;
			nIndex = strSelectedPath.ReverseFind(L'\\');
			strSelectedPath = strSelectedPath.Mid(nIndex+1);
			
			CString strTempTarget;
			strTempTarget = DecryptInfo.szTargetPath;
			strTempTarget.TrimRight(L'\\');
			strTempTarget = strTempTarget + L"\\" +strSelectedPath;
			
			DWORD dwFileAttribute;
			dwFileAttribute = GetFileAttributes((LPCTSTR)strTempTarget);
			if( -1 != dwFileAttribute )
			{
				SetFileAttributes((LPCTSTR)strTempTarget,FILE_ATTRIBUTE_NORMAL);
				if( 0 == ( dwFileAttribute & FILE_ATTRIBUTE_DIRECTORY ) )
				{
					if( FALSE == DeleteFile((LPCTSTR)strTempTarget) )
					{
						TRACE(L"\nDeleteFile %s error",(LPCTSTR)strTempTarget);
					}
					else
					{
						// TRACE(L"\nDeleteFile %s succeed",(LPCTSTR)strTempTarget);
					}
				}
				else
				{
					CEraseDirectory OneEraseDirectory;
					if( FALSE == OneEraseDirectory.EraseDirectory((LPCTSTR)strTempTarget) )
					{
						TRACE(L"Erase dir %s error in OnBeginDrag",(LPCTSTR)strTempTarget);
						
						// try this
						if( FALSE == MoveFile((LPCTSTR)strTempTarget,(LPCTSTR)(strTempTarget + L"_temp")) )
						{
							TRACE(L"\nRename dir %s error in OnBeginDrag",(LPCTSTR)strTempTarget );
						}
					}
				}
			}
			
			// 2004.09.15 added end
			
			CDecryptStatus DecryptStatusDlg;
			DecryptStatusDlg.m_DecryptInfo = DecryptInfo;

			// If we decrypt to the temp dir, hide the prompt information
			DecryptStatusDlg.m_bConcisePrompt = TRUE;
			DecryptStatusDlg.DoModal();
			
			// 2004.09.10 added end
			
			return DRAGDROP_S_DROP;
		}
		else
		{
			TRACE(L"\nDrag and drop canceled.");
			return DRAGDROP_S_CANCEL;
		}
	}
	
	// otherwise, keep polling...
	return S_OK;
}
