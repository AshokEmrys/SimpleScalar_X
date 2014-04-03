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
	${OBJECTDIR}/src/bpred.o \
	${OBJECTDIR}/src/cache.o \
	${OBJECTDIR}/src/dlite.o \
	${OBJECTDIR}/src/eio.o \
	${OBJECTDIR}/src/endian.o \
	${OBJECTDIR}/src/eval.o \
	${OBJECTDIR}/src/loader.o \
	${OBJECTDIR}/src/machine.o \
	${OBJECTDIR}/src/main.o \
	${OBJECTDIR}/src/memory.o \
	${OBJECTDIR}/src/misc.o \
	${OBJECTDIR}/src/options.o \
	${OBJECTDIR}/src/ptrace.o \
	${OBJECTDIR}/src/range.o \
	${OBJECTDIR}/src/regs.o \
	${OBJECTDIR}/src/resource.o \
	${OBJECTDIR}/src/sim-outorder.o \
	${OBJECTDIR}/src/stats.o \
	${OBJECTDIR}/src/symbol.o \
	${OBJECTDIR}/src/syscall.o


# C Compiler Flags
CFLAGS=-DBYTES_LITTLE_ENDIAN -DWORDS_LITTLE_ENDIAN -DFAST_SRL -DFAST_SRA -DGZIP_PATH="/bin/gzip" -DDEBUG

# CC Compiler Flags
CCFLAGS=-DBYTES_LITTLE_ENDIAN -DWORDS_LITTLE_ENDIAN -DFAST_SRL -DFAST_SRA -DGZIP_PATH="/bin/gzip" -DDEBUG
CXXFLAGS=-DBYTES_LITTLE_ENDIAN -DWORDS_LITTLE_ENDIAN -DFAST_SRL -DFAST_SRA -DGZIP_PATH="/bin/gzip" -DDEBUG

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=src/libexo/libexo.a -lm

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/simplescalar

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/simplescalar: src/libexo/libexo.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/simplescalar: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/simplescalar ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/src/bpred.o: src/bpred.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -w -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/bpred.o src/bpred.c

${OBJECTDIR}/src/cache.o: src/cache.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -w -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cache.o src/cache.cpp

${OBJECTDIR}/src/dlite.o: src/dlite.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -w -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/dlite.o src/dlite.c

${OBJECTDIR}/src/eio.o: src/eio.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -w -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/eio.o src/eio.c

${OBJECTDIR}/src/endian.o: src/endian.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -w -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/endian.o src/endian.c

${OBJECTDIR}/src/eval.o: src/eval.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -w -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/eval.o src/eval.c

${OBJECTDIR}/src/loader.o: src/loader.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -w -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/loader.o src/loader.c

${OBJECTDIR}/src/machine.o: src/machine.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -w -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/machine.o src/machine.c

${OBJECTDIR}/src/main.o: src/main.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -w -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/main.o src/main.cpp

${OBJECTDIR}/src/memory.o: src/memory.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -w -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/memory.o src/memory.c

${OBJECTDIR}/src/misc.o: src/misc.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -w -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/misc.o src/misc.c

${OBJECTDIR}/src/options.o: src/options.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -w -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/options.o src/options.c

${OBJECTDIR}/src/ptrace.o: src/ptrace.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -w -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ptrace.o src/ptrace.c

${OBJECTDIR}/src/range.o: src/range.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -w -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/range.o src/range.c

${OBJECTDIR}/src/regs.o: src/regs.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -w -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/regs.o src/regs.c

${OBJECTDIR}/src/resource.o: src/resource.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -w -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/resource.o src/resource.c

${OBJECTDIR}/src/sim-outorder.o: src/sim-outorder.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -w -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/sim-outorder.o src/sim-outorder.cpp

${OBJECTDIR}/src/stats.o: src/stats.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -w -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/stats.o src/stats.c

${OBJECTDIR}/src/symbol.o: src/symbol.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -w -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/symbol.o src/symbol.c

${OBJECTDIR}/src/syscall.o: src/syscall.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -w -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/syscall.o src/syscall.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/simplescalar

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
