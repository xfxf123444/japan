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

DEFINE_GUID(CLSID_ShellExtension,0x510202d2L, 0xba05, 0x4e09, 0x89, 0x11, 0x31, 0xe0, 0x19, 0x99, 0x5a, 0xf0 );

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
	WCHAR		m_szCommand[MAX_PATH*MAX_PATH];
	WCHAR		m_szInstallPath[MAX_PATH];
	int			m_nSelected;

protected:
	ULONG        m_cRef;
	LPDATAOBJECT m_pDataObj;

	STDMETHODIMP DoDelCommand(HWND hParent, 
	                        LPCSTR pszWorkingDir, 
	                        LPCSTR pszCmd,
                            LPCSTR pszParam, 
                            int iShowCmd);

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

};
typedef CShellExt *LPCSHELLEXT;

#endif // _SHELLEXT_H
