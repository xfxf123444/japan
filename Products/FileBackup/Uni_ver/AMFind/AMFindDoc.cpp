// AMFindDoc.cpp : implementation of the CAMFindDoc class
//

#include "stdafx.h"
#include "AMFind.h"

#include "AMFindDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAMFindDoc

IMPLEMENT_DYNCREATE(CAMFindDoc, CDocument)

BEGIN_MESSAGE_MAP(CAMFindDoc, CDocument)
	//{{AFX_MSG_MAP(CAMFindDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAMFindDoc construction/destruction

CAMFindDoc::CAMFindDoc()
{
	// TODO: add one-time construction code here

}

CAMFindDoc::~CAMFindDoc()
{
}

BOOL CAMFindDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CAMFindDoc serialization

void CAMFindDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAMFindDoc diagnostics

#ifdef _DEBUG
void CAMFindDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAMFindDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAMFindDoc commands

BOOL CAMFindDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	// TODO: Add your specialized creation code here
	
	return TRUE;
}
