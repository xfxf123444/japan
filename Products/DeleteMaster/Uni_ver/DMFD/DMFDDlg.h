// DMFDDlg.h : header file
//

#if !defined(AFX_DMFDDLG_H__1FCC55AE_81FB_49D1_84CF_33D1F91B21BA__INCLUDED_)
#define AFX_DMFDDLG_H__1FCC55AE_81FB_49D1_84CF_33D1F91B21BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDMFDDlg dialog

class CDMFDDlg : public CDialog
{
// Construction
public:
	CDMFDDlg(CWnd* pParent = NULL);	// standard constructor

	char				m_szFloppy[3];

	DWORD GetFloppyFormFactor(int iDrive);
	BOOL  HasFloppy();
// Dialog Data
	//{{AFX_DATA(CDMFDDlg)
	enum { IDD = IDD_DMFD_DIALOG };
	CString	m_MainInfo;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDMFDDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDMFDDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnContinue();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DMFDDLG_H__1FCC55AE_81FB_49D1_84CF_33D1F91B21BA__INCLUDED_)
