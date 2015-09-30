
The following files are included from LSI Logic...

NTASPI.EXE
  ASPI driver installation kit for Windows NT.

W2KASPI.EXE
  ASPI driver installation kit for Windows 2000.

*****************************************************************************

ATAPI DEVICES

To enable support for ATAPI devices with the above ASPI drivers, you must
modify the following registry entry on your system...

  HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\AspiXNT\Parameters

  If ExcludeMiniports="" then ATAPI devices are enabled.
  If ExcludeMiniports="ATAPI", then ATAPI devices are disabled.

*****************************************************************************

LICENSE AGREEMENT

Our license agreement with LSI Logic Corporation only allows you to
distribute these drivers in complete kit form. You can not distribute
any of the individual files within the kit.

*****************************************************************************
