#if !defined(AFX_SCHEDULEEDIT_H__495B56F6_4819_492C_9DAB_9FDF58B69F18__INCLUDED_)
#define AFX_SCHEDULEEDIT_H__495B56F6_4819_492C_9DAB_9FDF58B69F18__INCLUDED_

#include "StdAfx.h"
#include "Fun.h"

/////////////////////////////////////////////////////////////////////////////
// CScheduleEdit dialog

class CScheduleEdit : public CDialog
{
// Construction
public:
	ULONG m_ItemNum;
	void ModifyListItem(int nItem,PSCHEDULE_ITEM pScheduleInfo,BOOL bInsert);
	CScheduleEdit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CScheduleEdit)
	enum { IDD = IDD_SCHEDULE_EDIT };
	CButton	m_SetPassword;
	CButton	m_ModifySchedule;
	CButton	m_DeleteSchedule;
	CButton	m_AddSchedule;
	CListCtrl	m_ScheduleList;
	//}}AFX_DATA


	CImageList			m_ImgList;
	
	void InitList();
	void AddList();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScheduleEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScheduleEdit)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnAddSchedule();
	afx_msg void OnModifySchedule();
	afx_msg void OnDeleteSchedule();
	virtual void OnOK();
	afx_msg void OnSetPassword();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCHEDULEEDIT_H__495B56F6_4819_492C_9DAB_9FDF58B69F18__INCLUDED_)
