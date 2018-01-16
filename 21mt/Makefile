.SUFFIXES: .cxx .cpp
.PHONY: ALL CLEAN
#CC = /usr/local/gcc-3.4.4/bin/g++
CC = g++
GCC = gcc
LL = $(CC)
#ADMB_HOME = /usr/local/admb-gcc344/
JAVA_DIR = /usr/local/java
AKIRA_DIR = /usr/local/jnigraphics
PT_DIR = /home/kawakawa/jsibert/movemod/21mt/threads/POSIX_package/PThreads
OBJ_DIR = build
VPATH=src

#profileing for gprof
PFLAGS = -pg

#use -gstabs+ for gdb in order to get retain the symbol table after optimizations
#CFLAGS =  -gstabs+ -pg -O3 -pthread -fpermissive -Wno-deprecated -Isrc -I$(ADMB_HOME)/include -I$(AKIRA_DIR)/include -I$(JAVA_DIR)/include  -I$(JAVA_DIR)/include/linux -D__GNUDOS__  -DOPT_LIB -D__REGIONAL__

CFLAGS =  -pg -O3 -pthread -fpermissive -Wno-deprecated -Isrc -I$(ADMB_HOME)/include -I$(AKIRA_DIR)/include -I$(JAVA_DIR)/include  -I$(JAVA_DIR)/include/linux -D__GNUDOS__  -DOPT_LIB -D__REGIONAL__

#cFLAGS =  -O3 -pthread -fstack-check -Isrc -I$(ADMB_HOME)/include -I$(AKIRA_DIR)/include -I$(JAVA_DIR)/include  -I$(JAVA_DIR)/include/linux -D__GNUDOS__  -DOPT_LIB -D__REGIONAL__

#LFLAGS=-L$(ADMB_HOME)/lib -gstabs+ -pg -lpthread -ldf1b2o -ladmod -ladt -lado -ldf1b2o -ladmod -ladt -lado
LFLAGS=-L$(ADMB_HOME)/lib -pg -lpthread -ldf1b2o -ladmod -ladt -lado -ldf1b2o -ladmod -ladt -lado

include objects.lst

default: build rtagest

clean:
	@rm -vf build/*.o
	@rm -vf libmovemod.a
	@rm -vf rtagest
                                                                                                                                                                                                    
build:
	@test -d build || mkdir -v build

rtagest: $(RTAGEST_OBJECTS:%.o=build/%.o) libmovemod.a
	$(LL) -o $@ $^ $(LFLAGS)

rtagest_mt: $(RTAGEST_OBJECTS:%.o=build/%.o) libmovemod.a
	$(LL) -o $@ $^ -pg $(LFLAGS)

libmovemod.a: $(LIBMOVEMOD_OBJECTS:%.o=build/%.o)
	ar -rc libmovemod.a $^

test: libmovemod.a $(TEST_OBJECTS) test.o
	$(LL) $^ -L. -L$(ADMB_HOME)/lib  -lmovemod -ladot -lor -o test 

medianest: libmovemod.a $(RTAGEST_OBJECTS) medianlk.o
	$(LL) $^ -L. -L$(ADMB_HOME)/lib  -lmovemod -ladot -lor -o medianest 

test_est: libmovemod.a $(TEST_EST_OBJECTS)
	$(LL) $^ -L. -L$(ADMB_HOME)/lib  -lmovemod -ladot -lor -o test_est 

trimest: libmovemod.a $(TRIMEST_OBJECTS)
	$(LL) $^ -L. -L$(ADMB_HOME)/lib  -lmovemod -ladot -lor -o trimest 

fit_tab: $(FIT_TAB_OBJECTS) 
	$(LL) $^ -L. -L$(ADMB_HOME)/lib -lmovemod -lado -lor -o $@

rtac: $(RTAC_OBJECTS) 
	$(LL) $^ -L. -L$(ADMB_HOME)/lib -lmovemod -lado -lor -o $@

rtagmove: $(EXE_RTAGMOVE_OBJECTS) 
	$(LL)  $^ -L. -L$(ADMB_HOME)/lib  -L$(AKIRA_DIR)/linux/lib -lmovemod -lado -lor -ljniGraphics -o rtagmove

tag_effort: libmovemod.a $(TAG_EFFORT_OBJECTS)
	$(LL)  $^ -L. -L$(ADMB_HOME)/lib  -L$(AKIRA_DIR)/linux/lib -lmovemod -lado -lor -ljniGraphics -o tag_effort

librtagmove.so: $(RTAGMOVE_OBJECTS) libmovemod.a
	g++  -shared -fpic $^ -L. -L$(ADMB_HOME)/lib  -L$(AKIRA_DIR)/linux/lib -lmovemod -ladot -lor -ljniGraphics -o librtagmove.so

libpattdisp.so: $(PATTDISP_OBJECTS) libmovemod.a
	g++  -shared -fpic $^ -L. -L$(ADMB_HOME)/lib  -L$(AKIRA_DIR)/linux/lib -lmovemod -lado -lor -ljniGraphics -o libpattdisp.so

librpreproc.so: $(PREPROC_OBJECTS) libmovemod.a
	g++  -shared -fpic $^ -L. -L$(ADMB_HOME)/lib  -L$(AKIRA_DIR)/linux/lib -lmovemod -lado -lor -ljniGraphics -o librpreproc.so

libMakePar.so: $(MAKEPAR_OBJECTS) $(PREPROC_OBJECTS) libmovemod.a
	g++  -shared -fpic $^ -L. -L$(ADMB_HOME)/lib  -L$(AKIRA_DIR)/linux/lib -lmovemod -lado -lor -ljniGraphics -o libMakePar.so

libMakeIfr.so: $(MAKEIFR_OBJECTS) $(PREPROC_OBJECTS) libmovemod.a
	g++  -shared -fpic $^ -L. -L$(ADMB_HOME)/lib  -L$(AKIRA_DIR)/linux/lib -lmovemod -lado -lor -ljniGraphics -o libMakeIfr.so

exe_tagmove15.o: tagmove15.cpp 
	$(CC) $(CFLAGS) -D__MAKE_EXECUTABLE__ tagmove15.cpp -o $(OBJ_DIR)/exe_tagmove15.o

$(OBJ_DIR)/thread_extensions.o: $(PT_DIR)/thread_extensions.c 
	$(GCC) -c $(cFLAGS) $< -o $@

##################################
build/%.o : %.cpp
	@echo ------- Compiling $@
	$(CC) -c $(CFLAGS) -o $@ $<

