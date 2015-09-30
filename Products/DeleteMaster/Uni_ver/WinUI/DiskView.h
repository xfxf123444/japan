#if !defined(AFX_DISKVIEW_H__3341F64E_149E_4317_8F8A_E217CEBF09A6__INCLUDED_)
#define AFX_DISKVIEW_H__3341F64E_149E_4317_8F8A_E217CEBF09A6__INCLUDED_

#include "StdAfx.h"

/////////////////////////////////////////////////////////////////////////////
// CDiskView dialog

class CDiskView : public CPropertyPage
{
	DECLARE_DYNCREATE(CDiskView)

// Construction
public:
	CDiskView();
	~CDiskView();

// Dialog Data
	//{{AFX_DATA(CDiskView)
	enum { IDD = IDD_DM_DISK_VIEW };
	CListCtrl	m_ViewParList;
	CButton	m_CheckEntireDisk;
	CComboBoxEx	m_ViewDiskList;
	CButton	m_ButtonView;
	CButton	m_ButtonClear;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDiskView)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDiskView)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangeViewDiskCombo();
	afx_msg void OnViewClear();
	afx_msg void OnViewEntirDisk();
	afx_msg void OnClickDmViewParList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDmView();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISKVIEW_H__3341F64E_149E_4317_8F8A_E217CEBF09A6__INCLUDED_)
