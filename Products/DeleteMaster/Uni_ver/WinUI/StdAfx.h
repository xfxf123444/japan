// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__B1BDED61_4C0D_4CC6_B84C_39C7F76620A7__INCLUDED_)
#define AFX_STDAFX_H__B1BDED61_4C0D_4CC6_B84C_39C7F76620A7__INCLUDED_

#ifndef WINVER				
#define WINVER 0x0501
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501	
#endif						

#ifndef _WIN32_WINDOWS		
#define _WIN32_WINDOWS 0x0410
#endif

#ifndef _WIN32_IE			
#define _WIN32_IE 0x0600
#endif
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <tchar.h>

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#define SECTORINFO "%.4X:  %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X - %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X "

#define	DMMainWinClass	_T("mainclass")
#define DMSecWinClass	_T("SecInfoClass")
#define bkRGB			RGB(192,220,192)

#define	MAIN_CLIENT_WIDTH		680
#define MAIN_CLIENT_HEIGHT		385

#define SB_WIDTH				95
#define SB_HEIGHT				28

#define MAIN_LOGO_AREA			0
#define MAIN_QUIT_AREA			30
#define PAGE_COUNT				3
#define TAB_IMAGE_WEDITH		64

#define IMG_LIST_SELECT			1

//#define MBR_ITEM				4
#define DISK_MIN_SECTOR			0x3f
#define DISK_BASE				0x80
#define REMOVEABLE_DISK			0x0004
#define	BYTEINSEC				512

#define BOOT_FLAG				0X80
#define DATA_BUFFER_SIZE 		51200
#define DATA_BUFFER_SECTOR 		100

#define DELETE_WITH_ZERO		0
#define DELETE_WITH_FF			1
#define DELETE_WITH_RANDOM		2
#define DELETE_NSA_MOTHED		3
#define DELETE_OLD_NSA_MOTHED	4
#define DELETE_DOD_STD_MOTHED	5
#define DELETE_NATO_MOTHED		6
#define DELETE_GUTMANN_MOTHED	7

#define IDT_TIMER1				100

#pragma warning(disable: 4996)

#endif // !defined(AFX_STDAFX_H__B1BDED61_4C0D_4CC6_B84C_39C7F76620A7__INCLUDED_)
