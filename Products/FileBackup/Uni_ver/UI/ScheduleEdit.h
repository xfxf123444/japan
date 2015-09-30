#if !defined(AFX_SCHEDULEEDIT_H__495B56F6_4819_492C_9DAB_9FDF58B69F18__INCLUDED_)
#define AFX_SCHEDULEEDIT_H__495B56F6_4819_492C_9DAB_9FDF58B69F18__INCLUDED_

#include "stdafx.h"
#include "ScheduleWizard.h"
#include "SchedulePage1.h"
#include "SchedulePage2.h"
#include "SchedulePage3.h"

/////////////////////////////////////////////////////////////////////////////
// CScheduleEdit dialog

class CScheduleEdit : public CDialog
{
// Construction
public:
	CString m_strSetting;
	CScheduleEdit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CScheduleEdit)
	enum { IDD = IDD_SCHEDULE_EDIT };
	CListCtrl	m_ScheduleList;
	//}}AFX_DATA

	CScheduleWizard * m_pScheduleWizard;
    CSchedulePage1*   m_pSchedulePage1;
    CSchedulePage2*   m_pSchedulePage2;
    CSchedulePage3*   m_pSchedulePage3;

	CImageList			m_ImgList;
	
	void InitList();
	void AddList();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScheduleEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScheduleEdit)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddSchedule();
	afx_msg void OnDeleteSchedule();
	afx_msg void OnButtonClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCHEDULEEDIT_H__495B56F6_4819_492C_9DAB_9FDF58B69F18__INCLUDED_)
