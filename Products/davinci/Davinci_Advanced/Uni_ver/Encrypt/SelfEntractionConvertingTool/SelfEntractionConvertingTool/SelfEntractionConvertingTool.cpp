
// SelfEntractionConvertingTool.cpp : ���ץꥱ�`�����Υ��饹�������x���ޤ���
//

#include "stdafx.h"
#include "SelfEntractionConvertingTool.h"
#include "SelfEntractionConvertingToolDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSelfEntractionConvertingToolApp

BEGIN_MESSAGE_MAP(CSelfEntractionConvertingToolApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CSelfEntractionConvertingToolApp ���󥹥ȥ饯�����

CSelfEntractionConvertingToolApp::CSelfEntractionConvertingToolApp()
{
	// �����ӥޥͩ`����`�򥵥ݩ`�Ȥ��ޤ�
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: ����λ�ä˘��B�å��`�ɤ�׷�Ӥ��Ƥ���������
	// ������ InitInstance �Ф���Ҫ�ʳ��ڻ��I��򤹤٤�ӛ�����Ƥ���������
}


// Ψһ�� CSelfEntractionConvertingToolApp ���֥������ȤǤ���

CSelfEntractionConvertingToolApp theApp;


// CSelfEntractionConvertingToolApp ���ڻ�

BOOL CSelfEntractionConvertingToolApp::InitInstance()
{
	// ���ץꥱ�`����� �ޥ˥ե����Ȥ� visual ����������Є��ˤ��뤿��ˡ�
	// ComCtl32.dll Version 6 �Խ���ʹ�ä�ָ��������Ϥϡ�
	// Windows XP �� InitCommonControlsEx() ����Ҫ�Ǥ�������ʤ���С�������ɥ����ɤϤ��٤�ʧ�����ޤ���
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ���ץꥱ�`������ʹ�ä��뤹�٤ƤΥ���� ����ȥ�`�� ���饹�򺬤��ˤϡ�
	// ������O�����ޤ���
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// ���������˥����� �ĥ�` �ӥ�`�ޤ��ϥ����� �ꥹ�� �ӥ�` ����ȥ�`�뤬
	// ���ޤ�Ƥ�����Ϥ˥����� �ޥͩ`����`�����ɤ��ޤ���
	CShellManager *pShellManager = new CShellManager;

	// �˜ʳ��ڻ�
	// �����ΙC�ܤ�ʹ�鷺����K�Ĥʌg�п��ܥե������
	// ��������sС���������Ϥϡ����¤��鲻Ҫ�ʳ��ڻ�
	// ��`������������Ƥ���������
	// �O������{����Ƥ���쥸���ȥ� ���`�������ޤ���
	// TODO: �������ޤ��ϽM�����ʤɤ��m�Ф������Ф�
	// ���������Ф������Ƥ���������
	SetRegistryKey(_T("���ץꥱ�`����� �������`�ɤ����ɤ��줿��`���� ���ץꥱ�`�����"));

	CSelfEntractionConvertingToolDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: ���������� <OK> �������줿�r�Υ��`�ɤ�
		//  ӛ�����Ƥ���������
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ���������� <����󥻥�> �������줿�r�Υ��`�ɤ�
		//  ӛ�����Ƥ���������
	}

	// �Ϥ����ɤ��줿������ �ޥͩ`����`���������ޤ���
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// �����������]�����ޤ��������ץꥱ�`�����Υ�å��`�� �ݥ�פ��_ʼ���ʤ���
	//  ���ץꥱ�`������K�ˤ��뤿��� FALSE �򷵤��Ƥ���������
	return FALSE;
}

