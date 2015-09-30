// PARTYPE.C
// Copyright (C) 2000 by YG Corp.
// All rights reserved
// Author:  ZL
// Date:    18-Feb-2000

//  define all kinds of partition type

#define PARTYPE_PRIMARY_FAT12	    0x01
#define PARTYPE_PRIMARY_FAT16	    0x04
#define PARTYPE_EXTENDED_05	    0x05
#define PARTYPE_BIGDOS_FAT	    0x06
#define PARTYPE_NTFS		    0x07
#define PARTYPE_PRIMARY_FAT32	    0x0B
#define PARTYPE_EXTENDED_FAT32	    0x0C
#define PARTYPE_EXTENDED_FAT16	    0x0E
#define PARTYPE_LOGICAL_FAT16	    0x0F


#define PARTYPE_UNKNOWN 	    0x00



//  define  FS type string

#define FAT16_STRING		    "FAT16   "	//  8 bytes + NULL byte
#define FAT32_STRING		    "FAT32   "



BOOL	IsExtendedPartition(BYTE btFlag);