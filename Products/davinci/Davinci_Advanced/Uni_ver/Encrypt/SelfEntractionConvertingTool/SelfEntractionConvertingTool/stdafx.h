
// stdafx.h : �˜ʤΥ����ƥ� ���󥯥�`�� �ե�����Υ��󥯥�`�� �ե����롢�ޤ���
// ���ջ������य�����Ĥ��ޤ�������ʤ����ץ������Ȍ��äΥ��󥯥�`�� �ե�����
// ��ӛ�����ޤ���

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Windows �إå��`����ʹ�ä���Ƥ��ʤ����֤���⤷�ޤ���
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // һ���� CString ���󥹥ȥ饯���`����ʾ�ĤǤ���

// һ��Ĥǟoҕ���Ƥⰲȫ�� MFC �ξ����å��`����һ���ηǱ�ʾ�������ޤ���
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC �Υ�������Ә˜ʥ���ݩ`�ͥ��
#include <afxext.h>         // MFC �Β�������


#include <afxdisp.h>        // MFC ���`�ȥ�`����� ���饹



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 ����� ����ȥ�`�� ���ݩ`��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows ����� ����ȥ�`�� ���ݩ`��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC �ˤ������ܥ�ȥ���ȥ�`�� �Щ`�Υ��ݩ`��









#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


