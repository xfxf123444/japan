/*==================================================================
Volume.h
==================================================================*/

#ifdef UNICODE
#define GetVolumePathName  GetVolumePathNameW
#else
#define GetVolumePathName  GetVolumePathNameA
#endif // !UNICODE

BOOL GetVolumePathName(LPCTSTR lpszFileName,
					   LPTSTR lpszVolumePathName,
					   DWORD cchBufferLength);