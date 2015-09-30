// OleDropSourcePro.h: interface for the COleDropSourcePro class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OLEDROPSOURCEPRO_H__56BDE3A1_7B75_4567_8308_5F888D0BE79C__INCLUDED_)
#define AFX_OLEDROPSOURCEPRO_H__56BDE3A1_7B75_4567_8308_5F888D0BE79C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class COleDropSourcePro : public COleDropSource  
{
public:
	SCODE QueryContinueDrag(BOOL bEscapePressed, DWORD dwKeyState);
	COleDropSourcePro();
	virtual ~COleDropSourcePro();

};

#endif // !defined(AFX_OLEDROPSOURCEPRO_H__56BDE3A1_7B75_4567_8308_5F888D0BE79C__INCLUDED_)
