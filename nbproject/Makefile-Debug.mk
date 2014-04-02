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
	${OBJECTDIR}/bpred.o \
	${OBJECTDIR}/cache.o \
	${OBJECTDIR}/dlite.o \
	${OBJECTDIR}/eio.o \
	${OBJECTDIR}/endian.o \
	${OBJECTDIR}/eval.o \
	${OBJECTDIR}/eventq.o \
	${OBJECTDIR}/loader.o \
	${OBJECTDIR}/machine.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/memory.o \
	${OBJECTDIR}/misc.o \
	${OBJECTDIR}/options.o \
	${OBJECTDIR}/ptrace.o \
	${OBJECTDIR}/range.o \
	${OBJECTDIR}/regs.o \
	${OBJECTDIR}/resource.o \
	${OBJECTDIR}/sim-outorder.o \
	${OBJECTDIR}/stats.o \
	${OBJECTDIR}/symbol.o \
	${OBJECTDIR}/syscall.o \
	${OBJECTDIR}/sysprobe.o \
	${OBJECTDIR}/target-pisa/loader.o \
	${OBJECTDIR}/target-pisa/pisa.o \
	${OBJECTDIR}/target-pisa/symbol.o \
	${OBJECTDIR}/target-pisa/syscall.o


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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/simplescalar

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/simplescalar: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/simplescalar ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/bpred.o: bpred.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/bpred.o bpred.c

${OBJECTDIR}/cache.o: cache.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/cache.o cache.c

${OBJECTDIR}/dlite.o: dlite.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/dlite.o dlite.c

${OBJECTDIR}/eio.o: eio.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/eio.o eio.c

${OBJECTDIR}/endian.o: endian.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/endian.o endian.c

${OBJECTDIR}/eval.o: eval.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/eval.o eval.c

${OBJECTDIR}/eventq.o: eventq.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/eventq.o eventq.c

${OBJECTDIR}/loader.o: loader.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/loader.o loader.c

${OBJECTDIR}/machine.o: machine.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/machine.o machine.c

${OBJECTDIR}/main.o: main.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.c

${OBJECTDIR}/memory.o: memory.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/memory.o memory.c

${OBJECTDIR}/misc.o: misc.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/misc.o misc.c

${OBJECTDIR}/options.o: options.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/options.o options.c

${OBJECTDIR}/ptrace.o: ptrace.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ptrace.o ptrace.c

${OBJECTDIR}/range.o: range.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/range.o range.c

${OBJECTDIR}/regs.o: regs.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/regs.o regs.c

${OBJECTDIR}/resource.o: resource.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resource.o resource.c

${OBJECTDIR}/sim-outorder.o: sim-outorder.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/sim-outorder.o sim-outorder.c

${OBJECTDIR}/stats.o: stats.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/stats.o stats.c

${OBJECTDIR}/symbol.o: symbol.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/symbol.o symbol.c

${OBJECTDIR}/syscall.o: syscall.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/syscall.o syscall.c

${OBJECTDIR}/sysprobe.o: sysprobe.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/sysprobe.o sysprobe.c

${OBJECTDIR}/target-pisa/loader.o: target-pisa/loader.c 
	${MKDIR} -p ${OBJECTDIR}/target-pisa
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/target-pisa/loader.o target-pisa/loader.c

${OBJECTDIR}/target-pisa/pisa.o: target-pisa/pisa.c 
	${MKDIR} -p ${OBJECTDIR}/target-pisa
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/target-pisa/pisa.o target-pisa/pisa.c

${OBJECTDIR}/target-pisa/symbol.o: target-pisa/symbol.c 
	${MKDIR} -p ${OBJECTDIR}/target-pisa
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/target-pisa/symbol.o target-pisa/symbol.c

${OBJECTDIR}/target-pisa/syscall.o: target-pisa/syscall.c 
	${MKDIR} -p ${OBJECTDIR}/target-pisa
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/target-pisa/syscall.o target-pisa/syscall.c

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
