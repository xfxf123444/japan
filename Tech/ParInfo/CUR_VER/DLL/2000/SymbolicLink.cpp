/*
SymbolicLink.cpp
Author WQ
[C]CopyRight Yu Guang
*/

#include "SymbolicLink.h"

NTSTATUS
NtOpenSymbolicLinkObject(HANDLE					*phObject,
						 DWORD					dwAccess,
						 OBJECT_ATTRIBUTES		*pObjectAttributes
						 )
{
	HINSTANCE	hNtDll = LoadLibrary("ntdll.dll");
	FARPROC		FarProc;	
	NTSTATUS	Status;
	if( hNtDll != INVALID_HANDLE_VALUE )
	{
		FarProc = GetProcAddress(hNtDll,"NtOpenSymbolicLinkObject");
		if ( FarProc != NULL )
		{
			_asm push pObjectAttributes;
			_asm push dwAccess;
			_asm push phObject;
			_asm call FarProc;
			_asm mov  Status,eax;
			return Status;
		}
	}
	//return -1 indicate fail
	return -1;
}

NTSTATUS
NtQuerySymbolicLinkObject(HANDLE				hObject,
						  UNICODE_STRING		*pUnicodeString,
						  DWORD					*pdwReserved
						  )
{
	HINSTANCE	hNtDll = LoadLibrary("ntdll.dll");
	FARPROC		FarProc;
	NTSTATUS	Status;
	if( hNtDll != INVALID_HANDLE_VALUE )
	{
		FarProc = GetProcAddress(hNtDll,"NtQuerySymbolicLinkObject");
		if ( FarProc != NULL )
		{
			_asm push pdwReserved;
			_asm push pUnicodeString;
			_asm push hObject;
			_asm call FarProc;
			_asm mov  Status,eax;
			return Status;
		}
	}
	//return -1 indicate fail
	return -1;
}

void NtClose(HANDLE handle)
{
	HINSTANCE	hNtDll = LoadLibrary("ntdll.dll");
	FARPROC		FarProc;	
	if( hNtDll != INVALID_HANDLE_VALUE )
	{
		FarProc = GetProcAddress(hNtDll,"NtClose");
		if ( FarProc != NULL )
		{
			_asm push handle;
			_asm call FarProc;
		}
		return;
	}
	CloseHandle(handle);
}

BOOL UnicodeToAnsi(UNICODE_STRING		*pUnicode,
				   char					*pAnsiStr)
{
	int			n;

	for (n = 0;n < pUnicode->Length;n++)
	{
		pAnsiStr[n] = (char)pUnicode->Buffer[n];
	}
	pAnsiStr[n] = '\0';
	return TRUE;
}

BOOL AnsiToUnicode(UNICODE_STRING		*pUnicode,
				   char					*pAnsiStr)
{
	char		*pch;
	int			n;

	pUnicode->Length = strlen(pAnsiStr) * 2;
	if( pUnicode->MaximumLength < pUnicode->Length )
		return FALSE;
	for (	pch = pAnsiStr, n = 0;
			n < pUnicode->Length ;
			pch++,n+=2
		)
	{
		pUnicode->Buffer[n>>1] = *pch;
	}
	return TRUE;
}

NTSTATUS
QuerySymbolicLink(UNICODE_STRING		*pusLinkName,
				  UNICODE_STRING		*pusLinkContent)
{
	OBJECT_ATTRIBUTES		oa;
	HANDLE					hLink;
	NTSTATUS				status;

	//init object attributes
	InitializeObjectAttributes( &oa,
								pusLinkName,
								OBJ_CASE_INSENSITIVE,
								(HANDLE) NULL,
								(PSECURITY_DESCRIPTOR) NULL ); 
	//open symbolic link
	status = NtOpenSymbolicLinkObject(	&hLink,
										SYMBOLIC_LINK_ALL_ACCESS, 
										&oa );
	if( NT_SUCCESS( status ))
	{
		status = NtQuerySymbolicLinkObject( hLink, 
											pusLinkContent, 
											NULL );
		NtClose(hLink);
	}
	return status;
}
