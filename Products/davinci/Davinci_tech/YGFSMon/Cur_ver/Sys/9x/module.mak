#=================================================================================
# Standard Module definition file ( module.std )
# 
# Wei Qi
# 2002-4-26 14:37
# 
# Source Strcture:
# 	(1) module\version\sys\, 		here reside the os independent code
# 	(2) module\version\sys\BIOS,	here reside the BIOS specified code
# 	(3) module\version\sys\WIN9X, 	here reside the WIN9X specified code
# 	(4) module\version\sys\WIN2000,	here reside the WIN200 specified code
# 	(5) etc.
# Output Structure:
# 	(1) module\version\sys\obj\BIOS
# 	(2) module\version\sys\obj\WIN9X
# 	(3) module\version\sys\obj\WIN2000
# 	(4) module\version\sys\BIOS\obj
# 	(5) module\version\sys\WIN9X\obj
# 	(6) module\version\sys\WIN2000\obj
# Make file is reside in dir:
#    module\version\sys\OS_PLATFORM = { DOS,BIOS,WIN9X,WIN2000 }
#=================================================================================

#########################################
# Only one can be 1, other must be zero
#########################################

OS_PLATFORM =WIN9X

TARGET_NAME     = YGFSMon
TARGET_TYPE	= VXD

#########################################
# Create output directories
#########################################

!MESSAGE \
------------------------------------------------------------

SPE_OUTDIR = .\obj
COM_OUTDIR = ..\obj\$(OS_PLATFORM)

!IF EXIST($(COM_OUTDIR))
!MESSAGE Set $(COM_OUTDIR) for common output.
!ELSE
!IF [MD $(COM_OUTDIR)]
!ERROR Fail to create $(COM_OUTDIR) for common output.
!ELSE
!MESSAGE Succeed to create $(COM_OUTDIR) for common output.
!ENDIF
!ENDIF

!IF EXIST($(SPE_OUTDIR))
!MESSAGE Set $(SPE_OUTDIR) for specific output.
!ELSE
!IF [MD $(SPE_OUTDIR)]
!ERROR Fail to create $(SPE_OUTDIR) for specific output.
!ELSE
!MESSAGE Succeed to create $(SPE_OUTDIR) for specific output.
!ENDIF
!ENDIF

!MESSAGE \
------------------------------------------------------------

##############################################
# Here add your source file for the project
##############################################

SPE_SOURCES =\
.\SYSCTRL.asm .\Shell.c .\YGFSMon.c 


##############################################
# Here comes the obj files definitions
##############################################

!IF "$(OS_PLATFORM)" == "BIOS"
COM_OBJS = $(COM_SOURCES:..\=..\obj\BIOS\)
!ELSEIF  "$(OS_PLATFORM)" == "DOS"
COM_OBJS = $(COM_SOURCES:..\=..\obj\DOS\)
!ELSEIF  "$(OS_PLATFORM)" == "WIN9X"
COM_OBJS = $(COM_SOURCES:..\=..\obj\WIN9X\)
!ELSEIF  "$(OS_PLATFORM)" == "WIN2000"
COM_OBJS = $(COM_SOURCES:..\=..\obj\WIN2000\)
!ELSE
!ERROR Dose not know which OS Platform to build!
!ENDIF

SPE_OBJS = $(SPE_SOURCES:.\=.\obj\)
COM_OBJS = $(COM_OBJS:.c=.obj)
SPE_OBJS = $(SPE_OBJS:.c=.obj)
COM_OBJS = $(COM_OBJS:.asm=.obj)
SPE_OBJS = $(SPE_OBJS:.asm=.obj)

###########################################
# Add additional objs
###########################################

#COM_OBJS = $(COM_OBJS)

##############################################
# Here comes the libaries definitions
##############################################
