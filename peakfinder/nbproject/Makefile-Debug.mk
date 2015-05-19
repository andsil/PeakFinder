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
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Auxiliary/auxFunc.o \
	${OBJECTDIR}/Auxiliary/complex.o \
	${OBJECTDIR}/ImageFilters/clahe.o \
	${OBJECTDIR}/ImageFilters/contrast.o \
	${OBJECTDIR}/ImageFilters/filters.o \
	${OBJECTDIR}/ImageFilters/fourier.o \
	${OBJECTDIR}/ImageFilters/mask.o \
	${OBJECTDIR}/ImageFilters/transformations.o \
	${OBJECTDIR}/ImageProcessing/PointLL.o \
	${OBJECTDIR}/ImageProcessing/RegionLL.o \
	${OBJECTDIR}/ImageProcessing/maxTreshHold.o \
	${OBJECTDIR}/Main/main.o \
	${OBJECTDIR}/Test/peakfinder.o \
	${OBJECTDIR}/TiffImage/readTiff.o \
	${OBJECTDIR}/TiffImage/tiffFile.o \
	${OBJECTDIR}/TiffImage/writeTiff.o


# C Compiler Flags
CFLAGS=-O3 -ftree-vectorize -lm -ltiff -fopenmp

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
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/peakfinder ${OBJECTFILES} ${LDLIBSOPTIONS} -lm -ltiff -lfftw3

${OBJECTDIR}/Auxiliary/auxFunc.o: Auxiliary/auxFunc.c 
	${MKDIR} -p ${OBJECTDIR}/Auxiliary
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Auxiliary/auxFunc.o Auxiliary/auxFunc.c

${OBJECTDIR}/Auxiliary/complex.o: Auxiliary/complex.c 
	${MKDIR} -p ${OBJECTDIR}/Auxiliary
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Auxiliary/complex.o Auxiliary/complex.c

${OBJECTDIR}/ImageFilters/clahe.o: ImageFilters/clahe.c 
	${MKDIR} -p ${OBJECTDIR}/ImageFilters
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ImageFilters/clahe.o ImageFilters/clahe.c

${OBJECTDIR}/ImageFilters/contrast.o: ImageFilters/contrast.c 
	${MKDIR} -p ${OBJECTDIR}/ImageFilters
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ImageFilters/contrast.o ImageFilters/contrast.c

${OBJECTDIR}/ImageFilters/filters.o: ImageFilters/filters.c 
	${MKDIR} -p ${OBJECTDIR}/ImageFilters
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ImageFilters/filters.o ImageFilters/filters.c

${OBJECTDIR}/ImageFilters/fourier.o: ImageFilters/fourier.c 
	${MKDIR} -p ${OBJECTDIR}/ImageFilters
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ImageFilters/fourier.o ImageFilters/fourier.c

${OBJECTDIR}/ImageFilters/mask.o: ImageFilters/mask.c 
	${MKDIR} -p ${OBJECTDIR}/ImageFilters
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ImageFilters/mask.o ImageFilters/mask.c

${OBJECTDIR}/ImageFilters/transformations.o: ImageFilters/transformations.c 
	${MKDIR} -p ${OBJECTDIR}/ImageFilters
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ImageFilters/transformations.o ImageFilters/transformations.c

${OBJECTDIR}/ImageProcessing/PointLL.o: ImageProcessing/PointLL.c 
	${MKDIR} -p ${OBJECTDIR}/ImageProcessing
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ImageProcessing/PointLL.o ImageProcessing/PointLL.c

${OBJECTDIR}/ImageProcessing/RegionLL.o: ImageProcessing/RegionLL.c 
	${MKDIR} -p ${OBJECTDIR}/ImageProcessing
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ImageProcessing/RegionLL.o ImageProcessing/RegionLL.c

${OBJECTDIR}/ImageProcessing/maxTreshHold.o: ImageProcessing/maxTreshHold.c 
	${MKDIR} -p ${OBJECTDIR}/ImageProcessing
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ImageProcessing/maxTreshHold.o ImageProcessing/maxTreshHold.c

${OBJECTDIR}/Main/main.o: Main/main.c 
	${MKDIR} -p ${OBJECTDIR}/Main
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Main/main.o Main/main.c

${OBJECTDIR}/Test/peakfinder.o: Test/peakfinder.c 
	${MKDIR} -p ${OBJECTDIR}/Test
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Test/peakfinder.o Test/peakfinder.c

${OBJECTDIR}/TiffImage/readTiff.o: TiffImage/readTiff.c 
	${MKDIR} -p ${OBJECTDIR}/TiffImage
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TiffImage/readTiff.o TiffImage/readTiff.c

${OBJECTDIR}/TiffImage/tiffFile.o: TiffImage/tiffFile.c 
	${MKDIR} -p ${OBJECTDIR}/TiffImage
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TiffImage/tiffFile.o TiffImage/tiffFile.c

${OBJECTDIR}/TiffImage/writeTiff.o: TiffImage/writeTiff.c 
	${MKDIR} -p ${OBJECTDIR}/TiffImage
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TiffImage/writeTiff.o TiffImage/writeTiff.c

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
