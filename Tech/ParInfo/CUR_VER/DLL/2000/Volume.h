/*
Volume.h
the windows new volume API from ddk;
*/

#ifndef VOLUME_H
#define VOLUME_H


#ifdef __cplusplus
extern "C" {
#endif

WINBASEAPI
HANDLE
WINAPI
FindFirstVolumeA(
    LPSTR lpszVolumeName,
    DWORD cchBufferLength
    );
WINBASEAPI
HANDLE
WINAPI
FindFirstVolumeW(
    LPWSTR lpszVolumeName,
    DWORD cchBufferLength
    );

#ifdef UNICODE
#define FindFirstVolume FindFirstVolumeW
#else
#define FindFirstVolume FindFirstVolumeA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
FindNextVolumeA(
    HANDLE hFindVolume,
    LPSTR lpszVolumeName,
    DWORD cchBufferLength
    );
WINBASEAPI
BOOL
WINAPI
FindNextVolumeW(
    HANDLE hFindVolume,
    LPWSTR lpszVolumeName,
    DWORD cchBufferLength
    );

#ifdef UNICODE
#define FindNextVolume FindNextVolumeW
#else
#define FindNextVolume FindNextVolumeA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
FindVolumeClose(
    HANDLE hFindVolume
    );

WINBASEAPI
BOOL
WINAPI
SetVolumeMountPointA(
    LPCSTR lpszVolumeMountPoint,
    LPCSTR lpszVolumeName
    );
WINBASEAPI
BOOL
WINAPI
SetVolumeMountPointW(
    LPCWSTR lpszVolumeMountPoint,
    LPCWSTR lpszVolumeName
    );
#ifdef UNICODE
#define SetVolumeMountPoint SetVolumeMountPointW
#else
#define SetVolumeMountPoint SetVolumeMountPointA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
DeleteVolumeMountPointA(
    LPCSTR lpszVolumeMountPoint
    );
WINBASEAPI
BOOL
WINAPI
DeleteVolumeMountPointW(
    LPCWSTR lpszVolumeMountPoint
    );

#ifdef UNICODE
#define DeleteVolumeMountPoint DeleteVolumeMountPointW
#else
#define DeleteVolumeMountPoint DeleteVolumeMountPointA
#endif // !UNICODE

#ifdef __cplusplus
}
#endif

#endif//VOLUME_H