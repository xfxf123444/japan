#if !defined(AFX_DELPROC_H__96B2E89E_EF04_4879_9339_C557059F95C5__INCLUDED_)
#define AFX_DELPROC_H__96B2E89E_EF04_4879_9339_C557059F95C5__INCLUDED_

#include "StdAfx.h"
#include <vector>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CDelProc dialog

class CDelProc : public CDialog
{
// Construction
public:
	CDelProc(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDelProc)
	enum { IDD = IDD_DELETE_PROGRESS };
	CButton	m_CancelButton;
	CButton	m_SaveButton;
	CButton	m_NoSaveButton;
	CStatic	m_DelPresent;
	CProgressCtrl	m_DelProgress;
	CString	m_szPercent;
	CString	m_szTar;
	CString	m_szDelMothed;
	CString	m_csElasped;
	CString	m_csRemain;
	//}}AFX_DATA
	BOOL		m_bCancelProc;
	BOOL		m_bDisk;
	BOOL		m_bActive;
	BOOL		m_bLogc;
	int			m_nDisk;
	TCHAR		m_DriveLetter;
	DWORD		m_dwMinSec,m_dwMaxSec;
	struct tm	m_StartTime;
	struct tm	m_EndTime;
	DWORD		m_dwElaspedTime;
	int			m_nPresent;
	BOOL		m_bShowVerifyMsg;
	TCHAR        m_szLogFile[MAX_PATH];
	HANDLE		m_Thread;
	vector<HANDLE> m_volumeHandleSet;
	vector<TCHAR> m_driveLetterSet;

	BOOL DeleteSectors();
	BOOL DeleteContent();
	BOOL WriteSecsWithBuf(DWORD dwStartSec,DWORD dwSize,BYTE *pBuf,BOOL bVerify);
	BOOL SaveReport(LPCTSTR lpFileName);
	BOOL SaveVerifyInfo(LPCTSTR lpFileName,DWORD dwStartSec,DWORD dwSectors,BOOL bWriteFail);
	static	DWORD WINAPI ThreadProc(CDelProc* dlg);
	virtual DWORD ProgressProc();
	BOOL LockDiskOrVolume();
	BOOL LockVolume(TCHAR driveLetter);
	void UnlockDiskOrVolume();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDelProc)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDelProc)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnNoSave();
	afx_msg void OnSave();
	afx_msg void OnTimer(UINT nIDEvent);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DELPROC_H__96B2E89E_EF04_4879_9339_C557059F95C5__INCLUDED_)
