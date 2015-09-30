
                      CDROM and DVD Recording Devices
                         Software Development Kit

             Copyright (c) 1995-2007 by Golden Hawk Technology


                       VERSION 4.0D - January 2007


WHAT'S NEW
----------

- Support for many new DVD recorder models.



PREVIOUS CHANGES
----------------

- Support for image files larger than 4Gb.

- Support for DVD Dual-Layer discs (8.5Gb).

- Full support for UNICODE and MBCS filenames. A separate Unicode version
  of the SDK is now included.

- Support for ISO9660 Level 2 filenames (maximum 31 characters).

- Support for new character translation tables, including ISO9660, DOS,
  and ASCII (previously, only DOS was supported).

- Support for DOS high-ASCII characters.

- Filename sorting options.

- DVD-R and DVD-RW discs are supported.

- DVD+R and DVD+RW discs are supported.

- UDF and UDF BRIDGE image file formats are supported.

- Borland C++ Builder is supported.

Automatic MP3 file decoding is now supported by the "Full" version of the
library (32-bit Intel platform only). The following directories have been
added to the distribution CDROM...

  \LIB\32BIT\I386MP3\MICROSFT\MT
    Multithreaded and linking to MFC in a static library.

  \LIB\32BIT\I386MP3\MICROSFT\MTDDL
    Multithreaded and linking to MFC in a shared DLL.

  You must link your application against all three files in the directory
  (CD32xxxx.LIB, XAUDIO.LIB, and XANALYZE.LIB). Please refer to the file
  EASYDAO.CPP (in the SOURCE directory) for an example on how to record
  MP3 files to an audio disc.


DIRECTORY CONTENTS
------------------

LIB      - Contains the 16-bit and 32-bit versions of the library.
             The 16-bit version can only be used to build DOS console
             applications for old versions of MS-DOS. It supports SCSI
             CDROM devices only and 16-bit ASPI drivers are required.

             The 32-bit version can only be used to build applications for
             Windows 95/98/NT/2K (this includes 32-bit console apps as well).
             Libraries compiled with both Microsoft Visual C++ and Borland
             C++ Builder are included in separate subdirectories.

ASPI     - Contains ASPI drivers for Windows 95/98/NT/2K.

DOCS     - Contains the documentation for the class library (in Microsoft
           Word and Adobe PDF formats).

GOLDHAWK - Contains the latest version of Golden Hawk Technology's CDROM
           recording software (for both DOS and Windows). This directory
           also includes the file KEYS.TXT, which contains the instructions
           for unlocking the software.

INCLUDE  - Contains all of the class library header files. The easiest
           way to access all library functions is to include "CDRLIB.H"
           in your application.

SOURCE   - Contains the source code for all of Golden Hawk Technology's
           DOS recording utilities. These modules demonstrate how to use
           just about every major function call in the library. By learning
           what the DOS programs do and by studying the source code, you
           will find that it is very easy to add CDROM recording support to
           your application.


LIBRARY NAMING CONVENTION
-------------------------

          Lite/Full (L/F)
          |
          | Static/Dynamic (S/D)
          | |
          | | ASCII/Unicode (A/U)
          | | |
Filename: XXXXXNN.LIB
           | | ||
           | | 16bit/32bit (16/32)
           | |          
           | Singlethreaded/Multithreaded (S/M) 
           |
           Release/Debug (R/D)
   

LRSMA32.LIB    32bit - Lite/Release/Static/Multithreaded/ASCII
LRDMA32.LIB    32bit - Lite/Release/Dynamic/Multithreaded/ASCII
LDSMA32.LIB    32bit - Lite/Debug/Static/Multithreaded/ASCII
LDDMA32.LIB    32bit - Lite/Debug/Dynamic/Multithreaded/ASCII

LRSMU32.LIB    32bit - Lite/Release/Static/Multithreaded/Unicode
LRDMU32.LIB    32bit - Lite/Release/Dynamic/Multithreaded/Unicode
LDSMU32.LIB    32bit - Lite/Debug/Static/Multithreaded/Unicode
LDDMU32.LIB    32bit - Lite/Debug/Dynamic/Multithreaded/Unicode

FRSMA32.LIB    32bit - Full/Release/Static/Multithreaded/ASCII
FRDMA32.LIB    32bit - Full/Release/Dynamic/Multithreaded/ASCII
FDSMA32.LIB    32bit - Full/Debug/Dynamic/Multithreaded/ASCII
FDDMA32.LIB    32bit - Full/Debug/DDL/Multithreaded/ASCII

FRSMU32.LIB    32bit - Full/Release/Static/Multithreaded/Unicode
FRDMU32.LIB    32bit - Full/Release/Dynamic/Multithreaded/Unicode
FDSMU32.LIB    32bit - Full/Debug/Dynamic/Multithreaded/Unicode
FDDMU32.LIB    32bit - Full/Debug/DDL/Multithreaded/Unicode

