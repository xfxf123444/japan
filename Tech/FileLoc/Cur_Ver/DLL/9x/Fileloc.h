/******************************************************************************
*	FileLoc.H   header file
*
*	AUTHOR	:   ZL
*
*	Copyright 1999 - Shanghai YuGuang Science & Technology Co. Ltd .
*	All rights reserved.
*
*	Date:	02-Dec-1999
*
******************************************************************************/


#ifndef EXPORT
#define EXPORT	__declspec(dllexport)
#endif


#ifndef	MAX_CAPACITY_SUPPORTED_BY_OLD_INT13
#define	MAX_CAPACITY_SUPPORTED_BY_OLD_INT13		63 * 1024 *	256
#endif	

#define	MAX_DRIVE_MAP	10		

#ifndef	DIOC_REG
#define	DIOC_REG

//  In vwin32.h
// DIOCRegs
// Structure with i386 registers for making DOS_IOCTLS
// vwin32 DIOC handler interprets lpvInBuffer , lpvOutBuffer to be this struc.
// and does the int 21
// reg_flags is valid only for lpvOutBuffer->reg_Flags
typedef struct DIOCRegs	{
	DWORD	reg_EBX;
	DWORD	reg_EDX;
	DWORD	reg_ECX;
	DWORD	reg_EAX;
	DWORD	reg_EDI;
	DWORD	reg_ESI;
	DWORD	reg_Flags;		
	} DIOC_REGISTERS;


#endif	//	DIOC_REG	


#define	VWIN32_DIOC_DOS_IOCTL	1

	
