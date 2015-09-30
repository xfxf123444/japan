#include "MailSetting.h"
#include "AMFunction.h"

/*=======================================
 *
 * to get the Outlook Express store reg
 *
 *=======================================*/
BOOL GetLastUserID(WCHAR *szLastUserID)
{

	// TODO: Add your control notification handler code here
	CString strPath;
	CString strName;

	strPath = L"Identities";
	strName = L"Last User ID";

	HKEY hKEY;

	long ret0;

	ret0=(::RegOpenKeyEx(HKEY_CURRENT_USER,(LPCTSTR)strPath,0,KEY_READ,&hKEY));
	
	if(ret0!=ERROR_SUCCESS)
	{
		// AfxMessageBox(L"error, can not open hKEY");
		return FALSE;
	}

	WCHAR szRegValue[MAX_PATH];
	
	DWORD type_2=REG_SZ;
	
	DWORD cbData_2=MAX_PATH;

	long ret2=::RegQueryValueEx(hKEY,(LPCTSTR)strName,NULL,&type_2,(LPBYTE)szRegValue,&cbData_2);
	
	if(ret2!=ERROR_SUCCESS)
	{
		// AfxMessageBox(L"Can not query reg");
		return FALSE;
	}

	wcsncpy(szLastUserID,(WCHAR *)szRegValue,MAX_PATH-1);

	return TRUE;

}


// for get the store_path
BOOL GetOEReg(WCHAR *szOEReg)
{
    CString strPath;

	WCHAR szLastUserID[MAX_PATH];

	if(GetLastUserID(szLastUserID) == FALSE)
	{
		return FALSE;
	}

	strPath.Empty();
/////////////////////////////////////////////////////////////////////////////////////////////////////////
    DWORD dwPlatformVersion;
	DWORD dwMajorVersion;
	

//	dwPlatformVersion=GetPlatformVersion();
    //dwPlatformVersion=2
    OSVERSIONINFOEX osvi;

    // Try calling GetVersionEx using the OSVERSIONINFOEX structure.
    // If that fails, try using the OSVERSIONINFO structure.

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    if(GetVersionEx ((OSVERSIONINFO *) &osvi))
    {
		dwPlatformVersion=osvi.dwPlatformId;
		if(dwPlatformVersion==2)
		{
			dwMajorVersion=osvi.dwMajorVersion;
			switch(dwMajorVersion)
			{
			case 5: { strPath = strPath + L"Identities" + L"\\" + szLastUserID + L"\\Software\\Microsoft\\Outlook Express"; break;}
			case 6: 
				{
					 strPath = strPath + L"Software" + L"\\" + L"Microsoft" + L"\\Windows Mail";
				     wcsncpy(szOEReg,(LPCTSTR)strPath,MAX_PATH-1);
					 return TRUE;
				}
			}

		}
		else 
			return 0;
		
	}
       
    else
	{
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
        if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
		{
           return 0;
		}
		else
		{
			dwPlatformVersion=osvi.dwPlatformId;
		    if(dwPlatformVersion==2)
			{

			   dwMajorVersion=osvi.dwMajorVersion;
			   switch(dwMajorVersion)
			   {
			   case 5: { strPath = strPath + L"Identities" + L"\\" + szLastUserID + L"\\Software\\Microsoft\\Outlook Express"; break;}
			        case 6: 
						{
							strPath = strPath + L"Software" + L"\\" + L"Microsoft" + L"\\Windows Mail";
							wcsncpy(szOEReg,(LPCTSTR)strPath,MAX_PATH-1);
							return TRUE;

						}
			   }

			}
		    else 

			    return 0;

		}		
	}

////////////////////////////////////////////////////////////////////////////////


	HKEY hKEY;
	
	long ret0=(::RegOpenKeyEx(HKEY_CURRENT_USER,(LPCTSTR)strPath,0,KEY_READ,&hKEY));
	
	if(ret0!=ERROR_SUCCESS)
	{
		return FALSE;
	}

   long nIdx;
   WCHAR szValue[MAX_PATH];

   DWORD dwValueSize;
   dwValueSize=sizeof(szValue);

   long ret;

   FILETIME ftLastWriteTime;

   nIdx = 0;

    // szValue have OE Version
	// ret = RegEnumKey(hKEY, nIdx, szValue, sizeof(szValue) );
	
	ret=RegEnumKeyEx(hKEY,nIdx,szValue,&dwValueSize,NULL,NULL,NULL,&ftLastWriteTime);

    if( 0 ==  ret)
	{
		strPath = strPath + L"\\" + szValue;
		wcsncpy(szOEReg,(LPCTSTR)strPath,MAX_PATH-1);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
     
}

BOOL GetOEStoreDir(WCHAR *szOEStoreDir)
{

	HKEY hKEY;

	WCHAR szOEReg[MAX_PATH];

	long ret0;

	if(GetOEReg(szOEReg) == FALSE)
	{
		// perhaps it is old version of OE;
		// try this path
		// wcsncpy(szOEReg,L"Software\\Microsoft\\Outlook Express",MAX_PATH-1);
		return FALSE;
	}

	ret0=(::RegOpenKeyEx(HKEY_CURRENT_USER,szOEReg,0,KEY_READ,&hKEY));
//szOEReg=Identities\ID\Software\Microsoft\Outlook Express\5.0	
	if(ERROR_SUCCESS != ret0)
	{
		return FALSE;
	}

	
	WCHAR szRegValue[MAX_PATH];
	
	DWORD type_2=REG_EXPAND_SZ;
	
	DWORD cbData_2=MAX_PATH;

	CString strName;

	strName = OE_REG_STORE_ROOT;

	long ret2;
    ret2=::RegQueryValueEx(hKEY,(LPCTSTR)strName,NULL,&type_2,(LPBYTE)szRegValue,&cbData_2);
	
	if(ERROR_SUCCESS != ret2 )
	{
		return FALSE;
	}
	else
	{
	   ExpandEnvironmentStrings( (WCHAR *)szRegValue, szOEStoreDir, MAX_PATH-1 );

		// trim right '\\'
		// if( '\\' == szOEStoreDir[wcslen(szOEStoreDir)-1] )
		// {
			 // szOEStoreDir[wcslen(szOEStoreDir)-1] = 0;
		// }

		TrimRightChar(szOEStoreDir,L'\\',MAX_PATH);

		// wcsncpy(szOEStoreDir,(char *)szRegValue,MAX_PATH-1);
	
        RegCloseKey(hKEY);

		return TRUE;
	}

}

BOOL NewGetOutlookExpressDataPath(WCHAR *szDataPath, WCHAR *szAddressPath)
{
	OSVERSIONINFOEX osvi;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	WCHAR szAppDataPath[MAX_PATH];
	memset(szAppDataPath,0,sizeof(szAppDataPath));
	

    if( !GetVersionEx ((OSVERSIONINFO *) &osvi))
    {
        // If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.
        osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
        if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
           return FALSE;
    }
	
	DWORD dwPlatformVer = osvi.dwPlatformId;
	DWORD dwMajorVersion=osvi.dwMajorVersion;


	// BOOL    bResult = FALSE;

	BOOL    bResult = TRUE;

    IMalloc *pm;
    LPITEMIDLIST pidl;

//	char szPath[MAX_PATH];

    if (!FAILED(CoInitialize(NULL)))
	{
//		HMODULE hmod = LoadLibrary(_T(L"shell32.dll"));  
		
//		pfnSHGetFolderPath ptr = (pfnSHGetFolderPath)GetProcAddress(hmod ,"SHGetFolderPath");  
		
//		if(ptr) 
//		{
//			if (SUCCEEDED(ptr(NULL,CSIDL_APPDATA,NULL,0,szPath)))
		    if (SUCCEEDED(SHGetSpecialFolderLocation(NULL,CSIDL_APPDATA,&pidl)))
			{

				SHGetPathFromIDList(pidl,szAppDataPath);
				//szAppDataPath=C:\Users\Administrator\Applicatution
				SHGetMalloc(&pm);
				if (pm)
				{
					pm->Free(pidl);
					pm->Release();
				}
				switch (dwPlatformVer)
				{
				case VER_PLATFORM_WIN32_NT:
					
					
					*strrchrpro(szAppDataPath,L'\\') = L'\0';
					
					wcsncpy(szAddressPath,szAppDataPath,MAX_PATH-1);
					
					// wcscat(szDataPath,L"\\Local Settings\\Application Data\\Identities");
					
					// get Address Path with the old way
					if(dwMajorVersion==5)
					{
						wcscat(szAddressPath,L"\\Application Data\\Microsoft\\Address Book");
						
					}
					else if(dwMajorVersion==6)
					{
						//	*strrchrpro(szAddressPath,'\\') = '\0';
						strrchrcut(szAddressPath);
						
					}					
					//szAddressPath=C:\Documents and Settings\Administrator\Application Data\Microsoft\Address Book
					if(GetFileAttributes(szAppDataPath) == -1)
					{
						bResult = FALSE;
					}
					
					if(GetFileAttributes(szAddressPath) == -1)
					{
						bResult = FALSE;
					}
					
					break;
				case VER_PLATFORM_WIN32_WINDOWS:
					
					wcsncpy(szAddressPath,szAppDataPath,MAX_PATH-1);
					
					// wcscat(szDataPath,L"\\Identities");
					wcscat(szAddressPath,L"\\Microsoft\\Address Book");
					
					break;
				}
			}
			
		}
		
		CoUninitialize();

	BOOL bBakKey;
	BOOL bExpand;

	bBakKey = FALSE;
	bExpand = TRUE;

	if( GetOEStoreDir(szDataPath) == FALSE)
	{
		bResult = FALSE;
	}

	if(GetFileAttributes(szDataPath) == -1)
	{
		 bResult = FALSE;
	}

	if(GetFileAttributes(szAddressPath) == -1)
	{
		 bResult = FALSE;
	}

	return bResult;
    
		
		
}