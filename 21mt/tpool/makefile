###########################################################
# An example source module to accompany...
#
# "Using POSIX Threads: Programming with Pthreads"
#     by Brad nichols, Dick Buttlar, Jackie Farrell
#     O'Reilly & Associates, Inc.
#
###########################################################
# tpool/Makefile
#
# makefile for the example programs for pthreads programming book
#

ADMB_HOME=/home/jsibert/admb/trunk/build/admb-10.1-linux-gcc4.4.3-64bit/admb-10.1-linux-gcc4.4.3-64bit

# GCC/Linux compile flags
# GNUCC = ${GCC_HOME}/bin/g++
GNUCC=g++
GNUCFLAGS = -g -ansi -Wstrict-prototypes
GNULDFLAGS = -static
GNULDFLAGS_T = ${GNULDFLAGS} -pthread

#####################################################
CC = ${GNUCC}
CFLAGS = ${GNUCFLAGS}
LDFLAGS = ${GNULDFLAGS}
LDFLAGS_T = ${GNULDFLAGS_T}
CFLAGSM =  -O3 -fpermissive -Wno-deprecated -I. -I$(ADMB_HOME)/include -D__GNUDOS__  -DOPT_LIB
LFLAGSM= -pthread -L$(ADMB_HOME)/lib -ldf1b2o -ladmod -ladt -lado -ldf1b2o -ladmod -ladt -lado

#all :  tptest2

tptest2: tptest2.o tpool.o
	@echo "ADMB_HOME   = "$(ADMB_HOME)
	@echo "GCC_HOME = "$(GCC_HOME)
	@echo "CC       = "$(CC)
	@echo "CFLAGSM  = "${CFLAGSM}
	@echo "LFLAGSM  = "${LFLAGSM}
	${CC} tptest2.o tpool.o ${LFLAGSM} -o tptest2 

tptest: tptest.o tpool.o
	${CC} ${CFLAGS} ${LDFLAGS_T} tptest.o tpool.o -o tptest 

tpooltest: tpooltest.o tpool.o
	${CC} ${CFLAGS} ${LDFLAGS_T} tpooltest.o tpool.o -o tpooltest 

bugtest: bugtest.o tpool.o
	${CC} ${CFLAGS}  ${LDFLAGS_T} bugtest.o tpool.o -o bugtest

.cpp.o:
	@echo "CFLAGSM  = "${CFLAGSM}
	$(CC) -c $(CFLAGSM) $< -o$*.o


#clean :
#	rm -fv *.o *~ *# core  \
#	tpooltest \
#	*.out
