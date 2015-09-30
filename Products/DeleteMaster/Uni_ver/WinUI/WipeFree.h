#if !defined(AFX_WIPEFREE_H__BD4D6B9F_23FF_49A4_A0DE_4911B7267BD1__INCLUDED_)
#define AFX_WIPEFREE_H__BD4D6B9F_23FF_49A4_A0DE_4911B7267BD1__INCLUDED_

#include "StdAfx.h"

/////////////////////////////////////////////////////////////////////////////
// CWipeFree dialog

class CWipeFree : public CPropertyPage
{
	DECLARE_DYNCREATE(CWipeFree)

// Construction
public:
	CWipeFree();
	~CWipeFree();

// Dialog Data
	//{{AFX_DATA(CWipeFree)
	enum { IDD = IDD_DM_WIPE_FREE_SPACE };
	CButton	m_BeepOff;
	CButton	m_CheckShredder;
	CComboBoxEx	m_SelMothed;
	CComboBoxEx	m_WipeDiskList;
	CButton	m_ButtonClear;
	CButton	m_ButtonWipe;
	CStatic	m_LabelMothed;
	CListCtrl	m_WipeParList;
	//}}AFX_DATA
	TCHAR		m_szDesktop[MAX_PATH];
	TCHAR		m_szLink[MAX_PATH];
	TCHAR		m_szCurDir[MAX_PATH];
	TCHAR		m_szCurFile[MAX_PATH];
	HRESULT		CreateIt(LPCTSTR pszShortcutFile, LPTSTR pszLink);

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWipeFree)
	public:
	virtual void OnOK();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWipeFree)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeWipeDiskCombo();
	afx_msg void OnClickWipeParList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDmWipe();
	afx_msg void OnWipeClear();
	afx_msg void OnCheckShredder();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIPEFREE_H__BD4D6B9F_23FF_49A4_A0DE_4911B7267BD1__INCLUDED_)
