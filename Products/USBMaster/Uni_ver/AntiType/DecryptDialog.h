#if !defined(AFX_DECRYPTDIALOG_H__8CC3E1DC_0EAF_42CD_A644_AB8F09B83021__INCLUDED_)
#define AFX_DECRYPTDIALOG_H__8CC3E1DC_0EAF_42CD_A644_AB8F09B83021__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DecryptDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDecryptDialog dialog

extern CString driverName;
extern BOOL g_bAbort;

class CDecryptDialog : public CDialog
{
// Construction
public:
	void UndoDecrypt(PBYTE pKey,int keyLength,PBYTE pBuf,int decryptType,DWORD alreadyDecryptSectors);
	CDecryptDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDecryptDialog)
	enum { IDD = IDD_DECRYPT_DIALOG };
	CProgressCtrl	m_decryptProgress;
	CString	m_decryptPassword;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDecryptDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDecryptDialog)
	virtual void OnCancel();
	afx_msg void OnStartDecrypt();
	virtual BOOL OnInitDialog();
	afx_msg void OnAbortDecrypt();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DECRYPTDIALOG_H__8CC3E1DC_0EAF_42CD_A644_AB8F09B83021__INCLUDED_)
