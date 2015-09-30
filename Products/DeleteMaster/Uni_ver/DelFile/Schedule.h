#if !defined(AFX_SCHEDULE_H__0F275809_856F_419D_8993_D516AC24CF53__INCLUDED_)
#define AFX_SCHEDULE_H__0F275809_856F_419D_8993_D516AC24CF53__INCLUDED_

#include "StdAfx.h"
#include "Fun.h"	// Added by ClassView

/////////////////////////////////////////////////////////////////////////////
// CSchedule dialog

class CSchedule : public CDialog
{
// Construction
public:
	SCHEDULE_ITEM m_ScheduleInfo;
	BOOL m_bModify;
	CSchedule(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSchedule)
	enum { IDD = IDD_SCHEDULE };
	CDateTimeCtrl	m_ExecuteTime;
	CComboBox	m_Week;
	CComboBox	m_Month;
	CComboBoxEx	m_DeleteMethod;
	BOOL	m_bCheckLog;
	CTime	m_Time;
	CString	m_FilePath;
	int		m_nTimeType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSchedule)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSchedule)
	virtual BOOL OnInitDialog();
	afx_msg void OnRefer();
	virtual void OnOK();
	afx_msg void OnRadioDayOfWeek();
	afx_msg void OnRadioDayOfMonth();
	afx_msg void OnRadioDeleteFile();
	afx_msg void OnRadioDeleteRecycle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCHEDULE_H__0F275809_856F_419D_8993_D516AC24CF53__INCLUDED_)
