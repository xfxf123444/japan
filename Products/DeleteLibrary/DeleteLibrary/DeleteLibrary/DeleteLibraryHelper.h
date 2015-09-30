#ifndef   DELETE_LIBRARY_HELPER_INCLUDED    
#define   DELETE_LIBRARY_HELPER_INCLUDED 

#include "stdafx.h"
#include <atlstr.h>
#include <vector>

class DeleteLibraryHelper
{
public:
	// get file and folder path
	static bool GetFilePath( const TCHAR* path, 
		std::vector<CString>& pathSet, 
		TCHAR* pFileSuffix = 0,
		bool bIterate = true, 
		ULARGE_INTEGER* pSize = 0,
		bool bIncludingSubDir = false);
	static bool GetFileSizeInRecycled( ULARGE_INTEGER* pSize);

	// get special folder path
	static const TCHAR* GetRootPath();
	static const TCHAR* GetWindowsPath();
	static const TCHAR* GetProgramPath();
	static const TCHAR* GetProgramX86Path();
	static const TCHAR* GetAppDataPath();
	static const TCHAR* GetLocalAppDataPath();
	static const TCHAR* GetDesktopPath();
	static const TCHAR* GetPersonalPath();
	static const TCHAR* GetDocumentPath();
	static const TCHAR* GetModulePath();
	static const TCHAR* GetTempPath();
	static bool IsNormalPath(const TCHAR* path);
	static bool CheckEqualOrSubDir(LPCTSTR szFirstDir,LPCTSTR szSecondDir,bool &bEqualDir, bool &bSubDir);

private:
	static bool __GetFilePath(const TCHAR* path, 
		std::vector<CString>& pathSet,
		TCHAR* pFileSuffix,
		bool bIterate, 
		ULARGE_INTEGER* pSize,
		bool bIncludingSubDir);
	static bool __GetFileSizeInRecycled( TCHAR *pszDir, ULARGE_INTEGER* pSize);
};

#endif