// KeyGenerDlg.h : header file
//

#if !defined(AFX_KEYGENERDLG_H__2208F58C_B16B_456C_B6D2_919F930BD694__INCLUDED_)
#define AFX_KEYGENERDLG_H__2208F58C_B16B_456C_B6D2_919F930BD694__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CKeyGenerDlg dialog

class CKeyGenerDlg : public CDialog
{
// Construction
public:
	BOOL OnSelChangeRadio();
	CKeyGenerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CKeyGenerDlg)
	enum { IDD = IDD_KEYGENER_DIALOG };
	CEdit	m_KeyStart;
	CStatic	m_ProductKey;
	CProgressCtrl	m_Progress;
	CButton	m_RefButton;
	DWORD	m_dwProduct;
	CString	m_szCompany;
	int		m_nAppendToFile;
	DWORD	m_dwKeyNumber;
	DWORD	m_dwKeyStart;
	CString	m_LogFile;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKeyGenerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CKeyGenerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnCalculate();
	afx_msg void OnRefer();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnKillfocusFileName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEYGENERDLG_H__2208F58C_B16B_456C_B6D2_919F930BD694__INCLUDED_)
