#ifndef YG_SHELL_EXT_CONST_H
#define YG_SHELL_EXT_CONST_H

// note: the key here should be same as what install shield wrote
#include <tchar.h>

#ifdef DAVINCI_ADVANCED_VERSION
const TCHAR FILE_ENCRYPT_INSTALL_KEY[MAX_PATH]=_T("SOFTWARE\\FRONTLINE\\AngoukaMaster\\2.00");
#endif

#ifdef DAVINCI_BASIC_VERSION
const TCHAR FILE_ENCRYPT_INSTALL_KEY[MAX_PATH]=_T("SOFTWARE\\FRONTLINE\\SimpleAngouka\\2.00");
#endif

// note: the file name here should be the same as the name is install shield
const TCHAR FILE_ENCRYPT_PROGRAM[MAX_PATH]=_T("ShellEncrypt.exe");
const TCHAR FILE_DECRYPT_PROGRAM[MAX_PATH]=_T("ShellDecrypt.exe");

const TCHAR FILE_ENCRYPT_EXTENSION[MAX_PATH]=_T(".chy");

// note: the guid shoud be the same as what is written in InstallShield
// the GUID in install shield: {F3E55AC4-7D39-46b2-B17A-1C1EC8C92050}
// DEFINE_GUID(CLSID_ShellExtension,0xd1d0c6b8L, 0xabd2, 0x4ec8, 0x9a, 0xef, 0xba, 0x3e, 0x0e, 0x51, 0x61, 0x26 );
// DEFINE_GUID(CLSID_ShellExtension,0xF3E55AC4L, 0x7D39, 0x46b2, 0xb1, 0x7a, 0x1c, 0x1e, 0xc8, 0xc9, 0x20, 0x50 );

// {6EB9CFA7-B6C9-49f5-A3F0-986255E60F27}
DEFINE_GUID(CLSID_ShellExtension,0x6eb9cfa7, 0xb6c9, 0x49f5, 0xa3, 0xf0, 0x98, 0x62, 0x55, 0xe6, 0xf, 0x27);

#endif
