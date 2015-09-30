#if !defined(AFX_ENCRYPTDIALOG_H__932BBA44_7663_48A4_B5EA_33B2FFE8CA63__INCLUDED_)
#define AFX_ENCRYPTDIALOG_H__932BBA44_7663_48A4_B5EA_33B2FFE8CA63__INCLUDED_
#define WM_MY_MESSAGE (WM_USER + 101)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EncryptDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEncryptDialog dialog

//extern CString driverName;
extern BOOL g_bAbort;
//extern BYTE key[32];
//extern UINT EncryptType;
//extern UINT keyLen;
ULONG YGNSMCheckSum(UCHAR *pBuffer,ULONG ulSize);
BOOL YGEncryptBootSector(UCHAR *pBuf,PBYTE pKey,int nKeyLen);
BOOL YGDecryptBootSector(UCHAR *pBuf,PBYTE pKey,int nKeyLen);

class CEncryptDialog : public CDialog
{
// Construction
public:
	HWND m_handle;
	void UndoEncrypt(PBYTE pKey,int keyLength,PBYTE pBuf,int encryptType,DWORD alreadyEncryptSectors);
	CEncryptDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEncryptDialog)
	enum { IDD = IDD_ENCRYPT_DIALOG };
	CComboBox	m_encryptType;
	CProgressCtrl	m_encryptProgress;
	CString	m_encryptPassword;
	CString	m_confirmPassword;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEncryptDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEncryptDialog)
	virtual void OnCancel();
	afx_msg void OnStartEncrypt();
	virtual BOOL OnInitDialog();
	afx_msg void OnAbortEncrypt();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENCRYPTDIALOG_H__932BBA44_7663_48A4_B5EA_33B2FFE8CA63__INCLUDED_)
