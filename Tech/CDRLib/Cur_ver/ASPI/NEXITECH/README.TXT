
The following files are included from NexiTech...


**************************************************************************

WNASPI9X.DLL
WNASPI9X.VXD

These are the ASPI drivers for Windows 95/98/Me. The file WNASPI9X.DLL must
be copied to the WINDOWS\SYSTEM directory and the file WNASPI9X.VXD must be
copied to the WINDOWS\SYSTEM\IOSUBSYS directory.

**************************************************************************

WNASPINT.DLL

This is the ASPI driver for Windows NT/2000/XP. The file must be copied to
the WINDOWS\SYSTEM32 directory. Users can only access devices from an
Administrator account.

**************************************************************************

ASPIKIT.EXE

This program will install the ASPI drivers into the proper directories on
any Windows 95/98/Me/NT/2000/XP system.

**************************************************************************

SCSIACC.EXE

This service allows all Windows NT/2000 users from non-Administrator
accounts the ability to access devices through the ASPI drivers. The NT
designers tightened up security for Windows NT SP4 and beyond (which
includes Windows 2K) and this created problems for accessing ASPI devices.

Once the ASPI installation is complete (which needs to be done from an
Administrator account since the files are being copied to the SYSTEM32
directory), an Administrator needs to go to the SYSTEM32 directory and
do the following:

  1) ScsiAcc -install

  2) net start ScsiAcc

  3) Reboot the system

Before un-installing the ASPI drivers, an Administrator needs to go to the
SYSTEM32 directory and do the following:

  ScsiAcc -remove

The ScsiAcc service should be used ONLY if it is absolutely necessary that
users with non-Administrator accounts be able to access devices through the
ASPI drivers.If the users are all running from Admin accounts, then this
service is not necessary, and should not be used.

**************************************************************************

