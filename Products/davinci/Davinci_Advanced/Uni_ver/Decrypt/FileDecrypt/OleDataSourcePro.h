// OleDataSourcePro.h: interface for the COleDataSourcePro class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OLEDATASOURCEPRO_H__7AD9BE1A_C1DD_4430_9CC8_8FCA3A033BDC__INCLUDED_)
#define AFX_OLEDATASOURCEPRO_H__7AD9BE1A_C1DD_4430_9CC8_8FCA3A033BDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class COleDataSourcePro : public COleDataSource  
{
public:
	BOOL OnRenderGlobalData(LPFORMATETC lpFormatEtc, HGLOBAL *phGlobal);
	COleDataSourcePro();
	virtual ~COleDataSourcePro();

};

#endif // !defined(AFX_OLEDATASOURCEPRO_H__7AD9BE1A_C1DD_4430_9CC8_8FCA3A033BDC__INCLUDED_)
