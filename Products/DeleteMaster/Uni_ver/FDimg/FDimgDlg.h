// FDimgDlg.h : header file
//

#if !defined(AFX_FDIMGDLG_H__A2131C73_0DBC_4B77_B6DB_25C426B62F5A__INCLUDED_)
#define AFX_FDIMGDLG_H__A2131C73_0DBC_4B77_B6DB_25C426B62F5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CFDimgDlg dialog

class CFDimgDlg : public CDialog
{
// Construction
public:
	CFDimgDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CFDimgDlg)
	enum { IDD = IDD_FDIMG_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFDimgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CFDimgDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FDIMGDLG_H__A2131C73_0DBC_4B77_B6DB_25C426B62F5A__INCLUDED_)
