#=================================================================================
#
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
#    module\version\sys\OS_PLATFORM
#=================================================================================

#########################################
# Only one can be 1, other must be zero
#########################################

OS_PLATFORM_BIOS		= 0
OS_PLATFORM_DOS			= 0
OS_PLATFORM_WIN9X		= 1
OS_PLATFORM_WINNT		= 0

TARGET_NAME		= Crypto
TARGET_TYPE		= lib

#########################################
# Create output directories
#########################################

!MESSAGE ------------------------------------------------------------

SPE_OUTDIR = .\obj

!IF $(OS_PLATFORM_BIOS)
COM_OUTDIR = .\obj
!ELSE IF $(OS_PLATFORM_DOS)
COM_OUTDIR = .\obj
!ELSE IF $(OS_PLATFORM_WIN9X)
COM_OUTDIR = .\obj
!ELSE IF $(OS_PLATFORM_WINNT)
COM_OUTDIR = .\obj
!ELSE
!ERROR Unkown OS platform for build!!!
!ENDIF

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

!MESSAGE ------------------------------------------------------------

##############################################
# Here add your source file for the project
##############################################

COM_SOURCES =\
..\Crypto.cpp ..\des.cpp ..\misc.cpp ..\nn.cpp ..\rsa.cpp ..\md5c.cpp ..\rijndael.cpp

##############################################
# Here comes the obj files definitions
##############################################

!IF $(OS_PLATFORM_BIOS)
COM_OBJS = $(COM_SOURCES:..\=.\obj\)
!ELSE IF $(OS_PLATFORM_DOS)
COM_OBJS = $(COM_SOURCES:..\=.\obj\)
!ELSE IF $(OS_PLATFORM_WIN9X)
COM_OBJS = $(COM_SOURCES:..\=.\obj\)
!ELSE IF $(OS_PLATFORM_WINNT)
COM_OBJS = $(COM_SOURCES:..\=.\obj\)
!ENDIF

SPE_OBJS = $(SPE_SOURCES:.\=.\obj\)

COM_OBJS = $(COM_OBJS:.cpp=.obj)
SPE_OBJS = $(SPE_OBJS:.cpp=.obj)
COM_OBJS = $(COM_OBJS:.asm=.obj)
SPE_OBJS = $(SPE_OBJS:.asm=.obj)

###########################################
# Add additional objs
###########################################

##############################################
# Here comes the libaries definitions
##############################################

