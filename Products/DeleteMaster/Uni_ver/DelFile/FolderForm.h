#if !defined(AFX_FOLDERFORM_H__0E01F2A2_857A_11D4_98D1_5254AB10B215__INCLUDED_)
#define AFX_FOLDERFORM_H__0E01F2A2_857A_11D4_98D1_5254AB10B215__INCLUDED_

#include "StdAfx.h"

/////////////////////////////////////////////////////////////////////////////
// CFolderForm dialog

class CFolderForm : public CDialog
{
// Construction
public:
	void UpdateFolderView();
	BOOL m_bInitlizing;
	void InitTreeView();
	CFolderForm(CWnd* pParent = NULL);   // standard constructor
	CImageList  m_ImageList;
	BOOL	m_bIsSec;
	int		m_nMaxChars;
	
	BOOL    CreateFolder(CString csDir, CTreeCtrl* pTree, HTREEITEM hPar, BOOL bIsSecond = FALSE);
	CString ConvertHTREEToPath(HTREEITEM hTree,CTreeCtrl* pTree);
	BOOL	HasFloppy();
// Dialog Data
	//{{AFX_DATA(CFolderForm)
	enum { IDD = IDD_FOLDER_FORM };
	CTreeCtrl	m_Tree;
	CString	m_csName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFolderForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFolderForm)
	virtual BOOL OnInitDialog();
	afx_msg void OnItemexpandingTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FOLDERFORM_H__0E01F2A2_857A_11D4_98D1_5254AB10B215__INCLUDED_)
