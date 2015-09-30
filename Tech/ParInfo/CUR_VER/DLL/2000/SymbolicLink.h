/*
SymbolicLink.h
Author WQ
[C]CopyRight Yu Guang
*/

#ifndef SYMBOLICLINK_H
#define SYMBOLICLINK_H

#include <windows.h>

//Copied from nt ddk for use
typedef LONG NTSTATUS;

#define SYMBOLIC_LINK_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | 0x1)

#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)

#define OBJ_CASE_INSENSITIVE    0x00000040L

#define OBJ_PERMANENT           0x00000010L

typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;        // Points to type SECURITY_DESCRIPTOR
    PVOID SecurityQualityOfService;  // Points to type SECURITY_QUALITY_OF_SERVICE
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;


#define InitializeObjectAttributes( p, n, a, r, s ) { \
    (p)->Length = sizeof( OBJECT_ATTRIBUTES );          \
    (p)->RootDirectory = r;                             \
    (p)->Attributes = a;                                \
    (p)->ObjectName = n;                                \
    (p)->SecurityDescriptor = s;                        \
    (p)->SecurityQualityOfService = NULL;               \
    }

NTSTATUS
NtOpenSymbolicLinkObject(HANDLE					*phObject,
						 DWORD					dwAccess,
						 OBJECT_ATTRIBUTES		*pObjectAttributes
						 );

NTSTATUS
NtQuerySymbolicLinkObject(HANDLE				hObject,
						  UNICODE_STRING		*pUnicodeString,
						  DWORD					*pdwReserved
						  );

void NtClose(HANDLE handle);

BOOL UnicodeToAnsi(UNICODE_STRING		*pUnicode,
				   char					*pAnsiStr);

BOOL AnsiToUnicode(UNICODE_STRING		*pUnicode,
				   char					*pAnsiStr);

NTSTATUS
QuerySymbolicLink(UNICODE_STRING		*pusLinkName,
				  UNICODE_STRING		*pusLinkContent);


#endif//SYMBOLICLINK_H