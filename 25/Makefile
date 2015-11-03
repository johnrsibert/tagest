#$Id: Makefile 3205 2014-08-19 00:50:00Z eunjung $
# to make execuables with ADMB "safe" library, simply type make
# to make excutables with ADMB "optimized" library, type make OPT=TRUE (yes, upper case).
# the safe version also sets the debugger flag
#
.SUFFIXES: .cpp .o
.PHONY: all clean rules dox tpl test check dev

SRC_DIRS=src
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

CC=g++
#CC=clang++

# this makes the "safe" version by default
# to make "the optimized" version, type make OPT=TRUE
ifeq ($(OPT),TRUE)
  CC_OPT = -O3 -DOPT_LIB
  LFLAGS= $(ADMB_HOME)/lib/libadmb-contribo.a
else
  CC_OPT = -ggdb -O0
# CC_OPT = -g -gdwarf-2
  LFLAGS= $(ADMB_HOME)/lib/libadmb-contrib.a
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

  CFLAGS = -m64 -Wall $(CC_OPT)  -Isrc -I$(ADMB_HOME)/include -I$(ADMB_HOME)/contrib/include -I$(JNIGRAPHICS_PATH) -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/$(OS) -D__GNUDOS__ -Dlinux -DUSE_LAPLACE -UOLD_DO_KLUDGE 
  TAGMOVE_LFLAGS=-L$(JNIGRAPHICS_PATH) -L$(JAVA_HOME)/jre/lib/$(UNAME_M)/server -ljnigraphics3 -ljvm
 
  BIN_DIR = .
  EXE =
  LL=$(CC)

vpath %.cpp $(SRC_DIRS)
ifdef OSTYPE
OBJ_DIR = $(OSTYPE)
else
OBJ_DIR = build
endif




###############################
export CC
export LL
export CC_OPT
export MPICF
export MPILF
export CFLAGS
export LFLAGS
export ADMB_HOME 
export JAVA_HOME
export OBJ_DIR
export EXE
export JNIGRAPHICS_PATH
export LIBMOVEMOD_OBJECTS
export NNTAGEST_OBJECTS
export RTAGEST_OBJECTS
###############################

default: all

all:\
init \
libmovemod.a \
rsimspawn$(EXE) \
makeifr$(EXE) \
makepar$(EXE) \
distance$(EXE) \
elevation$(EXE) \
rtagmove$(EXE) \
obstac$(EXE) \
walk$(EXE) \
makemap$(EXE) \
landmask2gmt$(EXE) \
rhalflife$(EXE) \
tpl

neural_net: \
nn_makedata$(EXE) \
readERSSTv2File$(EXE) \
ntagest$(EXE) \
ntagmove$(EXE)

HEADERS = \
bound.h \
ch_type.h \
coff_t.h \
dbmatrix.h \
DymData.h \
ers12.h \
etopo5.h \
etopo_area.h \
factoral.h \
fish_rec.h \
generic.h \
geo_cord.h \
habitat.h \
intersav.h \
ipar.h \
latitude.h \
linbcg.h \
longlat.h \
longtude.h \
modlarea.h \
neural_net.h \
neural_net_vector.h \
par_t.h \
par_t_nn.h \
par_t_reg.h \
PathFileParser.h \
pattdisp.h \
policy.h \
preproc3.h \
prnstrem.h \
prn_t.h \
recap_t.h \
record3.h \
shift.h \
tag_pack3.h \
trace.h \
wrap_num.h \
xypair.h \
yr_mon2.h 

include objects.lst

init:
	@test -d $(OBJ_DIR) || mkdir -pv $(OBJ_DIR)
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
	@echo LL = $(LL)
	@echo CFLAGS = $(CFLAGS)
	@echo LFLAGS = $(LFLAGS)
	@echo EXE = $(EXE)
	@echo OBJ_DIR = $(OBJ_DIR)

todo:\
fit_tab \
covar21 \
ncovar24n2 \
gis_files

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
banner.obj: banner.cpp src/banner-info.cpp

svn:
	@rm -vf nohup.out;make clean;cd $(OBJ_DIR);svn checkout svn+ssh://maguro/movemod/25

nohup:
	@rm -vf nohup.out; nohup make clean all check run&

check: test
	@make -C test check

dev: readERSSTv2File test
	@make -C test dev

verify:
	@make -C test run

test: tpl
	@make -C test all

tpl: libmovemod.a
	@make -j 1 -B -C tpl all

xtagest: libmovemod.a
	@make -B -C tpl xtagest

dox:
	@make -C docs/API

clean:
	@rm -vrf $(OBJ_DIR)
	@rm -fv src/banner-info.cpp
	@rm -vf libmovemod.a
	@rm -vf rsimspawn$(EXE)
	@rm -vf makeifr$(EXE)
	@rm -vf makepar$(EXE)
	@rm -vf elevation$(EXE)
	@rm -vf nn_makedata$(EXE)
	@rm -vf ntagest25$(EXE)
	@rm -vf rtagest25$(EXE)
	@rm -vf ntagmove25$(EXE)
	@rm -vf rtagmove25$(EXE)
	@rm -vf ntagest25b$(EXE)
	@rm -vf rtagest25b$(EXE)
	@rm -vf ntagmove25b$(EXE)
	@rm -vf rtagmove25b$(EXE)
	@rm -vf readERSSTv2File$(EXE)
	@rm -vf makemap$(EXE)
	@rm -fv rtagmove$(EXE)
	@rm -fv landmask2gmt$(EXE)
	@rm -vf gmon.out
	@rm -vf distance$(EXE)
	@rm -vf obstac$(EXE)
	@rm -vf walk$(EXE)
	@make -C tpl clean
	@make -C docs/API clean

astyle:
	@astyle --style=ansi --indent=tab=5 --brackets=break --indent-switches --indent-cases --pad=oper --max-instatement-indent=80 
	@md5sum build/* ntag* rtag* > checksums.md5
	@md5sum --check checksums.md5

$(OBJ_DIR)/DymData.o: src/DymData.h

libmovemod.a: src/banner-info.cpp $(LIBMOVEMOD_OBJECTS:%.o=$(OBJ_DIR)/%.o) src/DymData.h
	@echo ------- Create $@; ar -rc $@ $(filter-out src/banner-info.cpp src/DymData.h, $^)

makeifr$(EXE): $(MAKEIFR_OBJECTS:%.o=$(OBJ_DIR)/%.o) $(RTAGEST_OBJECTS:%.o=$(OBJ_DIR)/%.o) libmovemod.a $(OBJ_DIR)/par_t_reg-computeObjectiveFunctionValue.o $(OBJ_DIR)/par_t_nn-computeObjectiveFunctionValue.o
	@echo ------- Link $@; $(LL) -o $@ $^ $(LFLAGS)

rsimspawn$(EXE): $(RSIMSPAWN_OBJECTS:%.o=$(OBJ_DIR)/%.o) $(RTAGEST_OBJECTS:%.o=$(OBJ_DIR)/%.o) libmovemod.a $(OBJ_DIR)/par_t_reg-computeObjectiveFunctionValue.o $(OBJ_DIR)/par_t_nn-computeObjectiveFunctionValue.o
	@echo ------- Link $@; $(LL) -o $@ $^ $(LFLAGS)

rseasspawn$(EXE): $(RSEASSPAWN_OBJECTS:%.o=$(OBJ_DIR)/%.o) $(RTAGEST_OBJECTS:%.o=$(OBJ_DIR)/%.o) libmovemod.a $(OBJ_DIR)/par_t_reg-computeObjectiveFunctionValue.o $(OBJ_DIR)/par_t_nn-computeObjectiveFunctionValue.o
	@echo ------- Link $@; $(LL) -o $@ $^ $(LFLAGS)

eezmap: $(EEZMAP_OBJECTS:%.o=$(OBJ_DIR)/%.o)  libmovemod.a 
	@echo ------- Link $@; $(LL) -o $@ $^ $(LFLAGS)

makemap: $(MAKEMAP_OBJECTS:%.o=$(OBJ_DIR)/%.o)  libmovemod.a 
	@echo ------- Link $@; $(LL) -o $@ $^ $(LFLAGS)

landmask2gmt: $(LANDMASK2GMT_OBJECTS:%.o=$(OBJ_DIR)/%.o)  libmovemod.a 
	@echo ------- Link $@; $(LL) -o $@ $^ $(LFLAGS)

prn2poly: $(PRN2POLY_OBJECTS:%.o=$(OBJ_DIR)/%.o)  libmovemod.a 
	@echo ------- Link $@; $(LL) -o $@ $^ $(LFLAGS)

par2gmt: $(PAR2GMT_OBJECTS:%.o=$(OBJ_DIR)/%.o) $(RTAGEST_OBJECTS:%.o=$(OBJ_DIR)/%.o) libmovemod.a $(OBJ_DIR)/par_t_reg-computeObjectiveFunctionValue.o $(OBJ_DIR)/par_t_nn-computeObjectiveFunctionValue.o
	@echo ------- Link $@; $(LL) -o $@ $^ $(LFLAGS)

fadregion: $(FADREGION_OBJECTS:%.o=$(OBJ_DIR)/%.o) $(RTAGEST_OBJECTS:%.o=$(OBJ_DIR)/%.o) libmovemod.a $(OBJ_DIR)/par_t_reg-computeObjectiveFunctionValue.o $(OBJ_DIR)/par_t_nn-computeObjectiveFunctionValue.o
	@echo ------- Link $@; $(LL) -o $@ $^ $(LFLAGS)

makepar$(EXE): $(MAKEPAR_OBJECTS:%.o=$(OBJ_DIR)/%.o) $(RTAGEST_OBJECTS:%.o=$(OBJ_DIR)/%.o) libmovemod.a $(OBJ_DIR)/par_t_reg-computeObjectiveFunctionValue.o $(OBJ_DIR)/par_t_nn-computeObjectiveFunctionValue.o
	@echo ------- Link $@; $(LL) -o $@ $^ $(LFLAGS)

#tagest$(EXE): rtagest$(EXE) ntagest$(EXE) libmovemod.a
rtagest$(EXE): rtagest25$(EXE) 
	@ln -sf rtagest25$(EXE) rtagest$(EXE)

ntagest$(EXE): ntagest25$(EXE) 
	@ln -sf ntagest25$(EXE) ntagest$(EXE)

rtagest25$(EXE): $(RTAGEST_OBJECTS:%.o=$(OBJ_DIR)/%.o) $(NNTAGEST_OBJECTS:%.o=$(OBJ_DIR)/%.o) $(OBJ_DIR)/tagest14.o $(OBJ_DIR)/tagest_main.o libmovemod.a $(OBJ_DIR)/par_t_reg-computeObjectiveFunctionValue.o $(OBJ_DIR)/par_t_nn-computeObjectiveFunctionValue.o
	@echo ------- Link $@; $(LL) -o $@ $^ $(LFLAGS)
rtagest25b: $(RTAGEST_OBJECTS:%.o=$(OBJ_DIR)/%.o) $(NNTAGEST_OBJECTS:%.o=$(OBJ_DIR)/%.o) $(OBJ_DIR)/tagest15.o $(OBJ_DIR)/tagest15_main.o libmovemod.a $(OBJ_DIR)/par_t_reg-computeObjectiveFunctionValue.o $(OBJ_DIR)/par_t_nn-computeObjectiveFunctionValue.o
	@echo ------- Link $@; $(LL) -o $@ $^ $(LFLAGS)
ntagest25$(EXE): $(NNTAGEST_OBJECTS:%.o=$(OBJ_DIR)/%.o) $(RTAGEST_OBJECTS:%.o=$(OBJ_DIR)/%.o) $(OBJ_DIR)/tagest14.o $(OBJ_DIR)/tagest_main.o libmovemod.a $(OBJ_DIR)/par_t_reg-computeObjectiveFunctionValue.o $(OBJ_DIR)/par_t_nn-computeObjectiveFunctionValue.o
	@echo ------- Link $@; $(LL) -o $@ $^ $(LFLAGS)
ntagest25b: $(NNTAGEST_OBJECTS:%.o=$(OBJ_DIR)/%.o) $(RTAGEST_OBJECTS:%.o=$(OBJ_DIR)/%.o) $(OBJ_DIR)/tagest15.o $(OBJ_DIR)/tagest15_main.o libmovemod.a $(OBJ_DIR)/par_t_reg-computeObjectiveFunctionValue.o $(OBJ_DIR)/par_t_nn-computeObjectiveFunctionValue.o
	@echo ------- Link $@; $(LL) -o $@ $^ $(LFLAGS)

#tagmove$(EXE): rtagmove$(EXE) ntagmove$(EXE)
rtagmove$(EXE): rtagmove25$(EXE)
	@ln -sf rtagmove25$(EXE) rtagmove$(EXE)

#ntagmove$(EXE): ntagmove25$(EXE)
rtagmove25$(EXE): $(RTAGMOVE_OBJECTS:%.o=$(OBJ_DIR)/%.o) $(RTAGEST_OBJECTS:%.o=$(OBJ_DIR)/%.o) libmovemod.a $(OBJ_DIR)/par_t_reg-computeObjectiveFunctionValue.o $(OBJ_DIR)/par_t_nn-computeObjectiveFunctionValue.o
	@echo ------- Link $@; $(LL)  -o $@ $(filter-out $(OBJ_DIR)/rtagest14.o, $^) $(TAGMOVE_LFLAGS) $(LFLAGS)

ntagmove25: $(NTAGMOVE_OBJECTS:%.o=$(OBJ_DIR)/%.o) $(NNTAGEST_OBJECTS:%.o=$(OBJ_DIR)/%.o) libmovemod.a $(OBJ_DIR)/par_t_reg-computeObjectiveFunctionValue.o $(OBJ_DIR)/par_t_nn-computeObjectiveFunctionValue.o
	@echo ------- Link $@; $(LL)  -o $@ $(filter-out $(OBJ_DIR)/ntagest14.o, $^) $(TAGMOVE_LFLAGS) $(LFLAGS)

#rhalflife: rhalflife25
rhalflife: $(HALFLIFE_OBJECTS:%.o=$(OBJ_DIR)/%.o) $(RTAGEST_OBJECTS:%.o=$(OBJ_DIR)/%.o) libmovemod.a $(OBJ_DIR)/par_t_reg-computeObjectiveFunctionValue.o $(OBJ_DIR)/par_t_nn-computeObjectiveFunctionValue.o
	@echo ------- Link $@; $(LL)  -o $@ $(filter-out $(OBJ_DIR)/rtagest14.o, $^) $(TAGMOVE_LFLAGS) $(LFLAGS)

nn_makedata: $(NN_MAKEDATA_OBJECTS:%.o=$(OBJ_DIR)/%.o) libmovemod.a
	@echo ------- Link $@; $(LL) -o $@ $^ $(LFLAGS)

$(OBJ_DIR)/rtagmove15.o: tagmove15.cpp
	@echo ------- Compiling $@; $(CC) -c $(CFLAGS) -D__REGIONAL__ -o $@ $<

$(OBJ_DIR)/ntagmove15.o: tagmove15.cpp
	@echo ------- Compiling $@; $(CC) -c $(CFLAGS) -D__NEURAL_NET__ -o $@ $<

$(OBJ_DIR)/rhalfmain.o: halfmain.cpp
	@echo ------- Compiling $@; $(CC) -c $(CFLAGS) -D__REGIONAL__ -o $@ $<

elevation$(EXE): $(OBJ_DIR)/mpar.o $(ELEVATION_OBJECTS:%.o=$(OBJ_DIR)/%.o) libmovemod.a
	@echo ------- Link $@; $(LL) -o $@ $^ $(LFLAGS)

distance$(EXE): $(DISTANCE_OBJECTS:%.o=$(OBJ_DIR)/%.o) libmovemod.a
	@echo ------- Link $@; $(LL) -o $@ $^ $(LFLAGS)

prnstream-test: 
	$(CC) src/prnstrem-test.cpp src/prnstrem.cpp $(CFLAGS) -o $@ $(LFLAGS)

readERSSTv2File$(EXE): $(OBJ_DIR)/readERSSTv2File.o $(OBJ_DIR)/mainReadERSSTv2File.o
	@echo ------- Link $@; $(LL) -o $@ $^ $(LFLAGS)

obstac: $(RTAGEST_OBJECTS:%.o=$(OBJ_DIR)/%.o) $(NNTAGEST_OBJECTS:%.o=$(OBJ_DIR)/%.o) $(RTAC_OBJECTS:%.o=$(OBJ_DIR)/%.o) libmovemod.a $(OBJ_DIR)/par_t_reg-computeObjectiveFunctionValue.o $(OBJ_DIR)/par_t_nn-computeObjectiveFunctionValue.o
	@echo ------- Link $@; $(LL) -o $@ $^ $(LFLAGS)

ttac: $(RTAGEST_OBJECTS:%.o=$(OBJ_DIR)/%.o) $(NNTAGEST_OBJECTS:%.o=$(OBJ_DIR)/%.o) $(TTAC_OBJECTS:%.o=$(OBJ_DIR)/%.o) libmovemod.a $(OBJ_DIR)/par_t_reg-computeObjectiveFunctionValue.o $(OBJ_DIR)/par_t_nn-computeObjectiveFunctionValue.o
	@echo ------- Link $@; $(LL) -o $@ $^ $(LFLAGS)

walk: $(RTAGEST_OBJECTS:%.o=$(OBJ_DIR)/%.o) $(WORMWALK_OBJECTS:%.o=$(OBJ_DIR)/%.o) libmovemod.a $(OBJ_DIR)/par_t_reg-computeObjectiveFunctionValue.o $(OBJ_DIR)/par_t_nn-computeObjectiveFunctionValue.o
	@echo ------- Link $@; $(LL) -o $@ $^ $(TAGMOVE_LFLAGS) $(LFLAGS)

$(OBJ_DIR)/%.o : %.cpp
	@test -d $(OBJ_DIR) || mkdir -pv $(OBJ_DIR)
	$(CC) -c $(CFLAGS) -o $@ $<
#	@echo ------- Compile $@; $(CC) -c $(CFLAGS) -o $@ $<

##############################
#
#covar21: libmovemod.a $(COVAR_OBJECTS)
#	$(LL) -o $@ $^ -L. -L$(ADMB_HOME)/lib  -lmovemod -lado -adt
#
#ncovar24n2: libmovemod.a $(NCOVAR_OBJECTS)
#	$(LL) -o $@ $^ -L. -L$(ADMB_HOME)/lib  -lmovemod -adt -lads
#
#nseedbump25: $(NSEEDBUMP_OBJECTS)
#	$(LL) -o $@ $^ -L. -L$(ADMB_HOME)/lib  -lmovemod -ado -ladt
#
#nsimspawn: nsimspawn.o ext_numb.o
#	$(LL) -o $@ $^ -L. -L$(ADMB_HOME)/lib  -lmovemod -adt -lado
#
#fit_tab: $(FIT_TAB_OBJECTS)
#	$(LL) -o $@ $^ -L. -L$(ADMB_HOME)/lib -lmovemod -lado -adt
#
#
#gis_files: gis_files.o
#	$(LL) -o $@ $^ -L. -L$(ADMB_HOME)/lib -lmovemod -lado -adt
#
#tag_effort: libmovemod.a $(TAG_EFFORT_OBJECTS)
#	$(LL)  -o $@ $^ -L. -L$(ADMB_HOME)/lib  -L$(AKIRA_DIR)/linux/lib -lmovemod -lado -adt -ljniGraphics
#
#build/ncovar.o: covar.cpp
#	@echo ------- Compiling $@
#	$(CC) -c $(CFLAGS) -D__NEURAL_NET__ -o $@ $<
#
#build/nseedbump.o:
#	@echo ------- Compiling $@
#	$(CC) -c $(CFLAGS) -D__NEURAL_NET__ -o $@ $<
#
#setw: setw.o
#	$(LL) $@ $^ -L. -L$(ADMB_HOME)/lib  -lmovemod -lado -adt
#
#liketest: liketest.o
#	$(LL) $@ $^ -L. -L$(ADMB_HOME)/lib  -lmovemod -lads -adt
#
#medianest: libmovemod.a $(RTAGEST_OBJECTS) medianlk.o
#	$(LL) -o $@ $^ -L. -L$(ADMB_HOME)/lib  -lmovemod -lado -adt
#
#test_est: libmovemod.a $(TEST_EST_OBJECTS)
#	$(LL) -o $@ $^ -L. -L$(ADMB_HOME)/lib  -lmovemod -lado -adt
#
#trimest: libmovemod.a $(TRIMEST_OBJECTS)
#	$(LL) -o $@ $^ -L. -L$(ADMB_HOME)/lib  -lmovemod -lado -adt
#
