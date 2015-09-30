#if !defined(AFX_NEWRESTORETREECTRL_H__4D0E968B_0A01_4237_AF8F_E2E3734FDCA5__INCLUDED_)
#define AFX_NEWRESTORETREECTRL_H__4D0E968B_0A01_4237_AF8F_E2E3734FDCA5__INCLUDED_

#include "..\..\EncryptToolStruct\EncryptToolStruct.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewRestoreTreeCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewRestoreTreeCtrl window

class CNewRestoreTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CNewRestoreTreeCtrl();

	CStringArray m_WorkStringArray;

	BOOL LoadTreeFromFile(LPCTSTR szImageFile);

	CString GetLeftOnePart(CString OneString);
	
	void AddSelectedItem(HTREEITEM hItem);

	BOOL HaveShortPath(HTREEITEM hItem);

	CString GetFullPath( HTREEITEM hItem );

    BOOL FoundInWorkStringArray(CString OneString);

    BOOL HaveExpanded(HTREEITEM hItem);

    void DisplayPath(HTREEITEM hParent);

    void ExpandItem(HTREEITEM hItem, UINT nCode);

    void AddPathToWorkingArray(
		                        CString			ExpandPath,
								CString			OnePath,
								DWORD			dwFileAttribute,
								CStringArray	&WorkingArray,
								BOOL			*pbHaveSubItem
							  );
    
	void NewFillWorkStringArray(LPCTSTR szImageFile,HTREEITEM hParent);

	BOOL LoadSegment(
					  LPCTSTR		szImageFile, // this should be the last file
					  CPATH_ARRAY	&PathArray,
					  DWORD			dwSegmentNumber,
					  DWORD			dwSegmentSize
					);

// Attributes
public:

	CStringArray m_SelectedNodeArray;
	CStringArray m_ExpandedNodeArray;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewRestoreTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNewRestoreTreeCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CNewRestoreTreeCtrl)
	afx_msg void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	SEGMENT_INFO m_TreeSegment;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWRESTORETREECTRL_H__4D0E968B_0A01_4237_AF8F_E2E3734FDCA5__INCLUDED_)
