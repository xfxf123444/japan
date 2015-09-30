#-----------------------------------------------------
# Standard Make File ( makefile.std )
# 
# Wei Qi
# 2002-4-26 14:37
#-----------------------------------------------------

!INCLUDE module.mak

##### Tool Macros ########
ASMC	    = $(MASM)\BIN\ML
CC		    = $(MSVC152)\BIN\CL
LINK	    = $(MSVC152)\BIN\LINK
LIB			= $(MSVC152)\BIN\LIB

######## C7 Macro ########
C7	    = 1

###### Memory Model ######
MOD		= -AC

##### General Macros #####

!IF $(OS_PLATFORM_BIOS)
DEF	    = -DSTRICT -DOS_PLATFORM=BIOS
!ELSE IF $(OS_PLATFORM_DOS)
DEF	    = -DSTRICT -DOS_PLATFORM=DOS
!ELSE IF $(OS_PLATFORM_WIN9X)
DEF	    = -DSTRICT -DOS_PLATFORM=WIN9X
!ELSE IF $(OS_PLATFORM_WINNT)
DEF	    = -DSTRICT -DOS_PLATFORM=WIN2000
!ELSE
!ERROR Unkown OS platform for build!!!
!ENDIF

####### DEBUG Macro ######
DEBUG	= 1

##### Build Option Macros #####

!IF $(DEBUG) ##### Debug ######
ASMCOPT		= -c -Zm -Cx
CCOPT		= -nologo -c -Zl -Zp1 -Gs -Asw -Ot -W3 $(MOD) $(DEF)
LOPT		= /NOD /NOE /MAP:FULL
LIBOPT		= /NOLOGO /NOI /NOE
!ELSE   ####### Release #######
ASMCOPT		= -c -Zm -Cx
CCOPT		= -nologo -c -Zl -Zp1 -Gs -Asw -Ot -W3 $(MOD) $(DEF)
LOPT		= /NOD /NOE /MAP:FULL
LIBOPT		= /NOLOGO /NOI /NOE
!ENDIF

########## goal ##########

goal:$(SPE_OUTDIR)\$(TARGET_NAME).$(TARGET_TYPE)

######### build ##########

$(SPE_OUTDIR)\$(TARGET_NAME).$(TARGET_TYPE):$(COM_OBJS) $(SPE_OBJS)
    $(LIB) $(LIBOPT) $(SPE_OUTDIR)\$(TARGET_NAME).$(TARGET_TYPE) $(COM_OBJS:..\=-+..\) $(SPE_OBJS:.\=-+.\),,


######### Inference Rules ##########

{..\}.c{$(COM_OUTDIR)}.obj:
	$(CC) $(CCOPT) -Fo$@ $<

{.\}.c{$(SPE_OUTDIR)}.obj:
	$(CC) $(CCOPT) -Fo$@ $<

{..\}.asm{$(COM_OUTDIR)}.obj:
	$(ASMC) $(ASMCOPT) -Fo$@ $<

{.\}.asm{$(SPE_OUTDIR)}.obj:
	$(ASMC) $(ASMCOPT) -Fo$@ $<

######### Clean Directory ###########
clean:
    -del $(COM_OUTDIR)\*.* /q
    -del $(SPE_OUTDIR)\*.* /q


