// MainDlg.h : header file
//
#if !defined(AFX_MAINDLG_H__CAF2EC4E_D834_483D_A2DA_FA329A409898__INCLUDED_)
#define AFX_MAINDLG_H__CAF2EC4E_D834_483D_A2DA_FA329A409898__INCLUDED_

#include "StdAfx.h"

/////////////////////////////////////////////////////////////////////////////
// CMainDlg dialog
#include "SearchForm.h"
#include "FolderForm.h"
#include "ScheduleEdit.h"

class CMainDlg : public CDialog
{
// Construction
public:
	BOOL m_bVerified;
	int m_nMinDlgHigth;
	int m_nMinDlgWidth;
	BOOL m_bRectInitlized;
	BOOL m_bIsWin9x;
	void SetWindowVersion();
	void EnableMenu(int nPos,BOOL bEnable);
	BOOL m_bAsc;
	CImageList m_ImageList;
	CToolBar m_wndToolBar;
	CFolderForm*     m_pStep1;
	CSearchForm*     m_pStep2;
	CScheduleEdit*   m_pStep3;

	CRect m_rectPage;

	void ShowPage(UINT nPos);
	void AddPage(CDialog *pDialog, UINT nID);

	UINT m_nPageCount;
	UINT m_nCurrentPage;
	CMainDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMainDlg)
	enum { IDD = IDD_MAIN_DIALOG };
	CListCtrl	m_FileList;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	void SetStatusBar();
	void InitPage();
	void CalcRect();
	typedef struct PAGELINK{
		UINT nNum;
		CDialog* pDialog;
		struct PAGELINK* Next;
	};
	PAGELINK* m_pPageLink;

	// Generated message map functions
	//{{AFX_MSG(CMainDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnSearchView();
	afx_msg void OnFolderView();
	afx_msg void OnExit();
	afx_msg void OnSecDelete();
	afx_msg void OnScheduleEdit();
	afx_msg void OnClickFileList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnColumnclickFileList(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINDLG_H__CAF2EC4E_D834_483D_A2DA_FA329A409898__INCLUDED_)
