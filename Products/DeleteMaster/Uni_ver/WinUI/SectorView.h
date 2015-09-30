#if !defined(AFX_SECTORVIEW_H__5A348CF6_B4C9_4BD2_BAE1_0FB57146E3FE__INCLUDED_)
#define AFX_SECTORVIEW_H__5A348CF6_B4C9_4BD2_BAE1_0FB57146E3FE__INCLUDED_

#include "StdAfx.h"

/////////////////////////////////////////////////////////////////////////////
// CSectorView dialog

class CSectorView : public CDialog
{
// Construction
public:
	CSectorView(CWnd* pParent = NULL);   // standard constructor

	DiskViewInfo		m_SecInfo;
	BIOS_DRIVE_PARAM	m_DriveParam;

	void				Showchs();
// Dialog Data
	//{{AFX_DATA(CSectorView)
	enum { IDD = IDD_VIEW_SECTOR };
	CButton	m_PrevBut;
	CButton	m_NextBut;
	CButton	m_SectorNumBut;
	CString	m_szSectorCHS;
	CString	m_szSectorNum;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSectorView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSectorView)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	virtual void OnOK();
	afx_msg void OnSectorNext();
	afx_msg void OnSectorPrev();
	afx_msg void OnSetSectorNum();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SECTORVIEW_H__5A348CF6_B4C9_4BD2_BAE1_0FB57146E3FE__INCLUDED_)
