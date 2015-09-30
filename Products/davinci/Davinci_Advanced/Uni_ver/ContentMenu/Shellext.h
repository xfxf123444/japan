//
// The class ID of this Shell extension class.
//
// class id:  87b9bd00-c65c-11cd-a259-00dd010e8c28
//
//
// NOTE!!!  If you use this shell extension as a starting point, 
//          you MUST change the GUID below.  Simply run UUIDGEN.EXE
//          to generate a new GUID.
//
                                  
#ifndef _SHELLEXT_H
#define _SHELLEXT_H

#include "ShellExtConst.h"

// DEFINE_GUID(CLSID_ShellExtension, 0x87b9bd00L, 0xc65c, 0x11cd, 0xa2, 0x59, 0x00, 0xdd, 0x01, 0x0e, 0x8c, 0x28 );
// DEFINE_GUID(CLSID_ShellExtension,0xd1d0c6b8L, 0xabd2, 0x4ec8, 0x9a, 0xef, 0xba, 0x3e, 0x0e, 0x51, 0x61, 0x26 );

// this class factory object creates context menu handlers for Windows 95 shell
class CShellExtClassFactory : public IClassFactory
{
protected:
	ULONG	m_cRef;

public:
	CShellExtClassFactory();
	~CShellExtClassFactory();

	//IUnknown members
	STDMETHODIMP			QueryInterface(REFIID, LPVOID FAR *);
	STDMETHODIMP_(ULONG)	AddRef();
	STDMETHODIMP_(ULONG)	Release();

	//IClassFactory members
	STDMETHODIMP		CreateInstance(LPUNKNOWN, REFIID, LPVOID FAR *);
	STDMETHODIMP		LockServer(BOOL);

};
typedef CShellExtClassFactory *LPCSHELLEXTCLASSFACTORY;

// this is the actual OLE Shell context menu handler
class CShellExt : public IContextMenu, 
                         IShellExtInit
{
public:
	TCHAR		m_szCommand[MAX_PATH*MAX_PATH];
	TCHAR		m_szInstallPath[MAX_PATH];
	int			m_nSelected;

protected:
	ULONG        m_cRef;
	LPDATAOBJECT m_pDataObj;

	STDMETHODIMP DoFileEncrypt( /*HWND hParent, 
								LPCTSTR pszWorkingDir, 
								LPCTSTR pszCmd,
								LPCTSTR pszParam, 
								int iShowCmd*/);

	STDMETHODIMP DoFileDecrypt( /*HWND hParent, 
								LPCTSTR pszWorkingDir, 
								LPCTSTR pszCmd,
								LPCTSTR pszParam, 
								int iShowCmd*/);

public:
	CShellExt();
	~CShellExt();

	//IUnknown members
	STDMETHODIMP			QueryInterface(REFIID, LPVOID FAR *);
	STDMETHODIMP_(ULONG)	AddRef();
	STDMETHODIMP_(ULONG)	Release();

	//IShell members
	STDMETHODIMP			QueryContextMenu(HMENU hMenu,
	                                         UINT indexMenu, 
	                                         UINT idCmdFirst,
                                             UINT idCmdLast, 
                                             UINT uFlags);

	STDMETHODIMP			InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi);

	STDMETHODIMP			GetCommandString(UINT_PTR idCmd, 
	                                         UINT uFlags, 
	                                         UINT FAR *reserved, 
                                             LPSTR pszName, 
                                             UINT cchMax);

	//IShellExtInit methods
	STDMETHODIMP		    Initialize(LPCITEMIDLIST pIDFolder, 
	                                   LPDATAOBJECT pDataObj, 
	                                   HKEY hKeyID);

	BOOL					GatherAppData();
private:
	BOOL m_bPathOverflow;
	BOOL m_bAppendEncryptMenu;
	BOOL m_bAppendDecryptMenu;

};
typedef CShellExt *LPCSHELLEXT;

#endif // _SHELLEXT_H
