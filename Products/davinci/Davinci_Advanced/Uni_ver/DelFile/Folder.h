#if !defined(AFX_FOLDER_H__0E01F2A2_857A_11D4_98D1_5254AB10B215__INCLUDED_)
#define AFX_FOLDER_H__0E01F2A2_857A_11D4_98D1_5254AB10B215__INCLUDED_

#include "StdAfx.h"

/////////////////////////////////////////////////////////////////////////////
// CFolder dialog

class CFolder : public CDialog
{
// Construction
public:
	BOOL m_bShowFile;
	CFolder(CWnd* pParent = NULL);   // standard constructor
	CString	m_csFileName;
	CImageList  m_ImageList;
	BOOL	m_bIsSec;
	int		m_nMaxChars;
	
	BOOL    CreateFolder(CString csDir, CTreeCtrl* pTree, HTREEITEM hPar, BOOL bIsSecond = FALSE);
	CString ConvertHTREEToPath(HTREEITEM hTree,CTreeCtrl* pTree);
// Dialog Data
	//{{AFX_DATA(CFolder)
	enum { IDD = IDD_GIVE_DIRECTORY };
	CTreeCtrl	m_Tree;
	CString	m_csName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFolder)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFolder)
	virtual BOOL OnInitDialog();
	afx_msg void OnItemexpandingTree(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FOLDER_H__0E01F2A2_857A_11D4_98D1_5254AB10B215__INCLUDED_)
