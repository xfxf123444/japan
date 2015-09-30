#if !defined(AFX_DELPAR_H__E480A03E_427C_4BBD_B2EF_7462A4E6DC48__INCLUDED_)
#define AFX_DELPAR_H__E480A03E_427C_4BBD_B2EF_7462A4E6DC48__INCLUDED_

#include "StdAfx.h"

/////////////////////////////////////////////////////////////////////////////
// CDelPar dialog

class CDelPar : public CPropertyPage
{
	DECLARE_DYNCREATE(CDelPar)

// Construction
public:
	CDelPar();
	~CDelPar();

// Dialog Data
	//{{AFX_DATA(CDelPar)
	enum { IDD = IDD_DM_DELETE_PARTITION };
	CButton	m_BeepOff;
	CButton	m_ButtonDiskInfo;
	CStatic	m_LabelMothed;
	CComboBoxEx	m_SelMothed;
	CButton	m_ButtonClear;
	CButton	m_ButtonDelete;
	CListCtrl	m_DelParList;
	CButton	m_CheckEntirDisk;
	CComboBoxEx	m_DelDiskList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDelPar)
	public:
	virtual void OnOK();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDelPar)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangeDeleteDiskCombo();
	afx_msg void OnClickDmDeleteParList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDmDelete();
	afx_msg void OnDeleteClear();
	afx_msg void OnDeleteEntirDisk();
	afx_msg void OnDmDiskInfo();
	afx_msg void OnDmOptions();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DELPAR_H__E480A03E_427C_4BBD_B2EF_7462A4E6DC48__INCLUDED_)
