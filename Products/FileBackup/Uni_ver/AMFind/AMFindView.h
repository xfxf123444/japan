// AMFindView.h : interface of the CAMFindView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_AMFINDVIEW_H__9D00495C_6913_4900_BDFB_CB27821E94C2__INCLUDED_)
#define AFX_AMFINDVIEW_H__9D00495C_6913_4900_BDFB_CB27821E94C2__INCLUDED_

#include "StdAfx.h"
class CAMFindDoc;

class CAMFindView : public CListView
{
protected: // create from serialization only
	CAMFindView();
	DECLARE_DYNCREATE(CAMFindView)
	CImageList *m_pImageList;
// Attributes
public:
	CAMFindDoc* GetDocument();

// Operations
public:
	void OnRestoreTo();
	void OnRestore();
	void OnMyFileOpen();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAMFindView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAMFindView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CAMFindView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in AMFindView.cpp
inline CAMFindDoc* CAMFindView::GetDocument()
   { return (CAMFindDoc*)m_pDocument; }
#endif

////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AMFINDVIEW_H__9D00495C_6913_4900_BDFB_CB27821E94C2__INCLUDED_)
