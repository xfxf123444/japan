#-------------------------------------------------------------------------------
#   Build.mak
#   Copyright: YuGuang Science and Technology co., Ltd.
#   HuiBin Li - 2/3/1999                                                          
#
#   Don't modify code in this file.It's used by all vxd builder.If any one want
#   to use this makefile, modify the file name tools.mak under the same directory.  
#-------------------------------------------------------------------------------
!include module.mak  

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
CCOPT		= -I$(DDK_INC32) -I$(SDK_INC32) -I$(VC_INC32) -Zdp -Gs -c -DIS_32 -Zl -Zp1\
			  -DBIOS=0 -DDOS=1 -DWIN9X=2 -DWIN2000=3 -DOS_PLATFORM=WIN9X\
			  -DDRIVER=0 -DAPPLICATION=1 -DMODULE_TYPE=DRIVER $(DDEBUG)
LFLAGS	    = -machine:i386 -debug:notmapped,full -debugtype:cv -subsystem:console kernel32.lib

AFLAGS	    = -I$(DDK_INC32) -coff -DBLD_COFF -DIS_32 -W2 -Zd -c -Cx -DMASM6 $(DDEBUG)

MSVC	    = $(MSVC42)\BIN\CL
ASM         = $(MASM)\bin\ml
LINK	    = $(MSVC42)\BIN\LINK.EXE
MAPSYM	    = $(MSVC42)\BIN\mapsym.exe
RC			= $(MSVC42)\BIN\rc
LIB			= $(MSVC42)\BIN\lib.exe	

all: $(SPE_OUTDIR)\$(TARGET_NAME).LIB

#.c.obj:
#	$(MSVC) $(CVXDFLAGS) -Fo$@ $<

#.asm.obj:
#	$(ASM) $(AFLAGS) -Fo$@ $<

#$(TARGET_DIR)\$(TARGET_NAME).LIB: $(OBJS)
#	LIB /OUT:$(TARGET_DIR)\$(TARGET_NAME).LIB /NOD $(OBJS)

$(SPE_OUTDIR)\$(TARGET_NAME).LIB:$(COM_OBJS) $(SPE_OBJS)
	LIB /OUT:$(SPE_OUTDIR)\$(TARGET_NAME).LIB /NOD $(COM_OBJS) $(SPE_OBJS)
	copy $(SPE_OUTDIR)\$(TARGET_NAME).LIB ..\..\export\9x\*.*

#clean:
#		-@del $(TARGET_DIR)\*.map
#                -@del $(TARGET_DIR)\*.lib
#                -@del $(TARGET_DIR)\*.exp
#                -@del $(TARGET_DIR)\*.sym
#		-@del *.obj
#		-@del ..\*.obj
		
######### Inference Rules ##########

{..\}.cpp{$(COM_OUTDIR)}.obj:
	$(CC) $(CCOPT) -Fo$@ $<

{.\}.cpp{$(SPE_OUTDIR)}.obj:
	$(CC) $(CCOPT) -Fo$@ $<

{..\}.asm{$(COM_OUTDIR)}.obj:
	$(ASMC) $(ASMCOPT) -Fo$@ $<

{.\}.asm{$(SPE_OUTDIR)}.obj:
	$(ASMC) $(ASMCOPT) -Fo$@ $<

##### Clean Directory #####
clean:
    -del $(COM_OUTDIR)\*.* /q
    -del $(SPE_OUTDIR)\*.* /q


