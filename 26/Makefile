#$Id: Makefile 3153 2014-03-08 01:17:28Z johnoel $
# extensivly hacked version of
# A Generic Makefile for ADMB programs that also includes additonal libraries.
# Developed for Mac OSx using the clang++ compiler
# Author: Steve Martell & John Sibert

# establish the C++ compiler (on Mac OSX use clang++)
CC=gcc
CXX=g++
#CC=clang
#CXX=clang++
# and linker
LL = $(CC)
LD = $(CXX)
# Remove macro
RM=rm -fv

# identify some extra file name suffixes
.SUFFIXES: .tpl .cpp .o 

# tell make not to delete these intermediate targets
.PRECIOUS: %.c %.cpp %.o

# make some special PHONY targets
.PHONY: all help rules clean dox


SRC_DIRS=./src
INCLUDE_SRC_DIRS=-Isrc

ifndef JAVA_HOME
  JAVA_HOME=/usr/local/java
endif
ifndef ADMB_HOME
  ADMB_HOME=/usr/local/admb
endif
ifndef JNIGRAPHICS_PATH
  JNIGRAPHICS_PATH=/usr/local/jnigraphics
endif

# set up ADMB flags and appropriate libraries
# make the "safe" version by default
# to make "the optimized" version, type  `make OPT=TRUE
ifeq ($(OPT),TRUE)
  CC_OPT = -O3 -DOPT_LIB
  LFLAGS= $(ADMB_HOME)/lib/libadmbo.a $(ADMB_HOME)/contrib/lib/libcontribo.a
else
  CC_OPT = -g
  LFLAGS= $(ADMB_HOME)/lib/libadmb.a $(ADMB_HOME)/contrib/lib/libcontrib.a
  OPT = FALSE
endif


  UNAME_S=$(shell uname -s)
  ifeq ($(UNAME_S),Darwin)
    OS=darwin
  else
    OS=linux
  endif

  UNAME_M=$(shell uname -m)
  ifeq ($(UNAME_M),x86_64)
    UNAME_M=amd64
  else
    UNAME_M=i386
  endif

  CFLAGS = -m64 -Wall $(CC_OPT)  -Isrc -I$(ADMB_HOME)/include -I$(JNIGRAPHICS_PATH) -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/$(OS) -D__GNUDOS__ -Dlinux -DUSE_LAPLACE -UOLD_DO_KLUDGE 
  CXXFLAGS=$(CFLAGS)
  TAGMOVE_LFLAGS=-L$(JNIGRAPHICS_PATH) -L$(JAVA_HOME)/jre/lib/$(UNAME_M)/server -ljnigraphics3 -ljvm
 
  BIN_DIR = .
  EXE =
  LL=$(CC)

vpath %.cpp $(SRC_DIRS)
OBJ_DIR = obj
vpath %.o $(OBJ_DIR)




default: all

all:\
libmovemod.a \
tagmove$(EXE)

include headers.lst
include objects.lst

rules:
	@echo OPT = $(OPT)
	@echo CC_OPT = $(CC_OPT)
	@echo MPI = $(MPI)
	@echo MPICF = $(MPICF)
	@echo MPILF = $(MPILF)
	@echo CURDIR = $(CURDIR)
	@echo PWD = $(PWD)
	@echo SRC_DIRS=$(SRC_DIRS)
	@echo OSTYPE = $(OSTYPE)
	@echo OS     = $(OS)
	@echo TERM = $(TERM)
	@echo BIN_DIR = $(BIN_DIR)
	@echo ADMB_HOME = $(ADMB_HOME)
	@echo ADMB_VERSION = $(ADMB_VERSION)
	@echo JNIGRAPHICS_PATH = $(JNIGRAPHICS_PATH)
	@echo JAVA_HOME = $(JAVA_HOME)
	@echo LD_LIBRARY_PATH = $(LD_LIBRARY_PATH)
	@echo CC = $(CC)
	$(CC) --version
	@echo CXX = $(CXX)
	$(CXX) --version
	@echo LL = $(LL)
	@echo CFLAGS = $(CFLAGS)
	@echo CXXFLAGS = $(CXXFLAGS)
	@echo LFLAGS = $(LFLAGS)
	@echo EXE = $(EXE)
	@echo OBJ_DIR = $(OBJ_DIR)

clean:
	@rm -vrf $(OBJ_DIR)
	@rm -fv src/banner-info.cpp
	@rm -vf libmovemod.a
	@rm -vf tagmove$(EXE)
	
REV = `svnversion .`
ADMB_VERSION=`cat $(ADMB_HOME)/VERSION`


#BANNER-INFO:
# it may be necessary to export OSTYPE and HOSTNAME in .bashrc
src/banner-info.cpp: Makefile
	@echo "adstring host(\"$(HOSTNAME)\");" > src/banner-info.cpp
	@echo "adstring machine(\"$(MACHTYPE)\");" >> src/banner-info.cpp
	@echo "adstring os(\"$(OSTYPE)\");" >> src/banner-info.cpp
	@echo "adstring wd(\"$(PWD)\");" >> src/banner-info.cpp
	@echo "adstring rev(\"$(REV)\");" >> src/banner-info.cpp
	@echo "adstring admb_version(\"$(ADMB_VERSION)\");" >> src/banner-info.cpp

libmovemod.a:src/banner-info.cpp $(LIBMOVEMOD_OBJECTS:%.o=$(OBJ_DIR)/%.o) 
	@echo ------- Create $@;
	ar -rc $@ $^

tagmove: $(OBJ_DIR)/tagmove15.o libmovemod.a
	@echo ------- Link $@; $(LD) -v  -o $@ $^ $(TAGMOVE_LFLAGS) $(LFLAGS)

# Build the cpp file from the tpl
%.cpp : %.tpl tpl2cpp
	./tpl2cpp $*


$(OBJ_DIR)/%.o : %.cpp
	@test -d $(OBJ_DIR) || mkdir -pv $(OBJ_DIR)
	$(CXX) -c  $(CFLAGS) -o $@ $<
