#-------------------------------------------------------------------------------
#   TOOLS.MAK
#   Copyright: YuGuang Science and Technology co., Ltd.
#   HuiBin Li - 2/3/1999
#
#   Add you source file in this file.Only need to tell the Vxd file name and 
#   OBJ file name.
#   for example: You file include an ASM file named test.asm and a C file named
#                shell.c, you want generate test.vxd,the three file all at current
#                directory. You shoult write like this:
#                     
#                TARGET_NAME = test
#                OBJS        = test.obj shell.obj    
#-------------------------------------------------------------------------------


TARGET_NAME  = .\result\FileLocH

OBJS	     = .\c\HighLoc.OBJ .\asm\FileLocH.OBJ
