#-------------------------------------------------------------------------------
#   Build.mak
#   Copyright: YuGuang Science and Technology co., Ltd.
#   HuiBin Li - 2/3/1999                                                          
#
#   Don't modify code in this file.It's used by all vxd builder.If any one want
#   to use this makefile, modify the file name tools.mak under the same directory.  
#-------------------------------------------------------------------------------
!include tools.mak  

!ifdef DEBUGBUILD
DDEBUG	 =-DDEBUG -DDEBLEVEL=1
!else
DDEBUG	 =-DDEBLEVEL=0
!endif

DDK_INC16   = $(DDK)\inc16
DDK_INC32   = $(DDK)\inc32
DDK_LIB     = $(DDK)\lib

SDK_INC32   = $(SDK)\include
VC_INC32    = $(MSVC42)\include

CFLAGS	    = -DWIN32 -DCON -Di386 -D_X86_ -D_NTWIN -W3 -Gs -D_DEBUG -Zi -Zp1
CVXDFLAGS   = -I$(DDK_INC32) -I$(SDK_INC32) -I$(VC_INC32) -Zdp -Gs -c -DIS_32 -Zl -Zp1 $(DDEBUG)
LFLAGS	    = -machine:i386 -debug:notmapped,full -debugtype:cv -subsystem:console kernel32.lib

AFLAGS	    = -I$(DDK_INC32) -coff -DBLD_COFF -DIS_32 -W2 -Zd -c -Cx -DMASM6 $(DDEBUG)

MSVC	    = $(MSVC42)\BIN\CL
ASM         = $(MASM)\bin\ml
LINK	    = $(MSVC42)\BIN\LINK.EXE
MAPSYM	    = $(MSVC42)\BIN\mapsym.exe
RC	    = $(MSVC42)\BIN\rc

all: $(TARGET_NAME).VXD

.c.obj:
	$(MSVC) $(CVXDFLAGS) -Fo$@ $<

.asm.obj:
	$(ASM) $(AFLAGS) -Fo$@ $<

$(TARGET_NAME).VXD: $(OBJS)
	echo >NUL @<<$(TARGET_NAME).crf
-MACHINE:i386 -DEBUG:NONE -PDB:NONE 
-DEF:$(TARGET_NAME).def -OUT:$(TARGET_NAME).vxd -MAP:$(TARGET_NAME).map
-VXD $(DDK_LIB)\vxdwraps.clb $(OBJS)
<<
        $(LINK) @$(TARGET_NAME).crf
	$(MAPSYM) -o $(TARGET_NAME).SYM $(TARGET_NAME).MAP

clean:
		-@del $(TARGET_DIR)\*.map
                -@del $(TARGET_DIR)\*.lib
                -@del $(TARGET_DIR)\*.exp
                -@del $(TARGET_DIR)\*.sym
