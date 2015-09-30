// AMFindDoc.h : interface of the CAMFindDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_AMFINDDOC_H__F5A4C5FD_E41C_4407_9596_578C3859C69D__INCLUDED_)
#define AFX_AMFINDDOC_H__F5A4C5FD_E41C_4407_9596_578C3859C69D__INCLUDED_

#include "StdAfx.h"


class CAMFindDoc : public CDocument
{
protected: // create from serialization only
	CAMFindDoc();
	DECLARE_DYNCREATE(CAMFindDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAMFindDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAMFindDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CAMFindDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AMFINDDOC_H__F5A4C5FD_E41C_4407_9596_578C3859C69D__INCLUDED_)
