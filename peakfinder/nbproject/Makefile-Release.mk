#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/InputImages/complex.o \
	${OBJECTDIR}/InputImages/fourier.o \
	${OBJECTDIR}/PointLL.o \
	${OBJECTDIR}/RegionLL.o \
	${OBJECTDIR}/auxFunc.o \
	${OBJECTDIR}/binary.o \
	${OBJECTDIR}/contrast.o \
	${OBJECTDIR}/mask.o \
	${OBJECTDIR}/maxTreshHold.o \
	${OBJECTDIR}/peakfinder.o \
	${OBJECTDIR}/readTiff.o \
	${OBJECTDIR}/tiffFile.o \
	${OBJECTDIR}/writeTiff.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/peakfinder

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/peakfinder: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/peakfinder ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/InputImages/complex.o: InputImages/complex.c 
	${MKDIR} -p ${OBJECTDIR}/InputImages
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/InputImages/complex.o InputImages/complex.c

${OBJECTDIR}/InputImages/fourier.o: InputImages/fourier.c 
	${MKDIR} -p ${OBJECTDIR}/InputImages
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/InputImages/fourier.o InputImages/fourier.c

${OBJECTDIR}/PointLL.o: PointLL.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PointLL.o PointLL.c

${OBJECTDIR}/RegionLL.o: RegionLL.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/RegionLL.o RegionLL.c

${OBJECTDIR}/auxFunc.o: auxFunc.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/auxFunc.o auxFunc.c

${OBJECTDIR}/binary.o: binary.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/binary.o binary.c

${OBJECTDIR}/contrast.o: contrast.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/contrast.o contrast.c

${OBJECTDIR}/mask.o: mask.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/mask.o mask.c

${OBJECTDIR}/maxTreshHold.o: maxTreshHold.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/maxTreshHold.o maxTreshHold.c

${OBJECTDIR}/peakfinder.o: peakfinder.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/peakfinder.o peakfinder.c

${OBJECTDIR}/readTiff.o: readTiff.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/readTiff.o readTiff.c

${OBJECTDIR}/tiffFile.o: tiffFile.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/tiffFile.o tiffFile.c

${OBJECTDIR}/writeTiff.o: writeTiff.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/writeTiff.o writeTiff.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/peakfinder

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
