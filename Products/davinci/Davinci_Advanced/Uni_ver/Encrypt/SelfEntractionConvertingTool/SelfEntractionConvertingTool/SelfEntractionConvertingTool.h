
// SelfEntractionConvertingTool.h : PROJECT_NAME ���ץꥱ�`�����Υᥤ�� �إå��` �ե�����Ǥ���
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH �ˌ����Ƥ��Υե�����򥤥󥯥�`�ɤ���ǰ�� 'stdafx.h' �򥤥󥯥�`�ɤ��Ƥ�������"
#endif

#include "resource.h"		// �ᥤ�� ����ܥ�


// CSelfEntractionConvertingToolApp:
// ���Υ��饹�Όgװ�ˤĤ��Ƥϡ�SelfEntractionConvertingTool.cpp ����դ��Ƥ���������
//

class CSelfEntractionConvertingToolApp : public CWinApp
{
public:
	CSelfEntractionConvertingToolApp();

// ���`�Щ`�饤��
public:
	virtual BOOL InitInstance();

// �gװ

	DECLARE_MESSAGE_MAP()
};

extern CSelfEntractionConvertingToolApp theApp;