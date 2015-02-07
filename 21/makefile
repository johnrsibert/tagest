.SUFFIXES: .cpp .obj
.PHONY: ALL CLEAN BIN RULES
CC = g++
LL = g++

ifeq ($(OSTYPE),cygwin)
# AD_DIR =/usr/local/admb-cygwin-gcc331
  AD_DIR =/usr/local/admodel
  JAVA_DIR = $(JAVA_HOME)
  JNI_GRAPHICS_DIR = $(JNIGRAPHICS_PATH)
  CFLAGS = -Wno-deprecated -fpermissive -c -O -I. -I$(AD_DIR)/include -I$(JNI_GRAPHICS_DIR) -I$(JAVA_DIR)/include -I$(JAVA_DIR)/include/win32 -I$(JAVA_DIR)/include/linux -D__GNUDOS__ -DOPT_LIB -D__REGIONAL__
  JNI_LINK = -L$(JNI_GRAPHICS_DIR) -L$(JAVA_HOME)/jre/lib/i386/client -ljnigraphics3
  OBJ_DIR = cygwin
  EXE = .exe
else
  AD_DIR = /usr/local/admodel
  JAVA_DIR = $(JAVA_HOME)/jre/lib/i386/client
  JNI_GRAPHICS_DIR = $(HOME)/jnigraphics3.2002-06-12
  #JNI_GRAPHICS_DIR = /usr/local/jnigraphics
  CFLAGS = -Wno-deprecated -fpermissive -c -O -I. -I$(AD_DIR)/include -I$(JNI_GRAPHICS_DIR)/include -I$(JAVA_DIR)/include -I$(JAVA_DIR)/include/linux -D__GNUDOS__ -DOPT_LIB -D__REGIONAL__
  EFLAGS = -E -Wno-deprecated -fpermissive -I. -I$(AD_DIR)/include -I$(JNI_GRAPHICS_DIR)/include -I$(JAVA_DIR)/include -I$(JAVA_DIR)/include/linux -D__GNUDOS__ -DOPT_LIB -D__REGIONAL__
  JNI_LINK = -L$(JNI_GRAPHICS_DIR)/lib -L$(JAVA_HOME)/jre/lib/i386/client -ljnigraphics3 -ljvm
  OBJ_DIR = gnu_obj
  EXE =
endif

#CFLAGS = -Wno-deprecated -fpermissive -c -O -I. -I/usr/include/g++-3 -I$(AD_DIR)/include -I$(JNI_GRAPHICS_DIR)/include -I$(JAVA_DIR)/include -I$(JAVA_DIR)/include/win32 -I$(JAVA_DIR)/include/linux -D__GNUDOS__ -DOPT_LIB -D__REGIONAL__

BIN_DIR = $(HOME)/bin

vpath %.obj $(OBJ_DIR)
vpath %.a $(OBJ_DIR)
#VPATH = $(OBJ_DIR)

#PFLAGS = -pg


RULES:
	@echo CURDIR = $(CURDIR)
	@echo PWD = $(PWD)
	@echo OSTYPE = $(OSTYPE)
	@echo TERM = $(TERM)
	@echo BIN_DIR = $(BIN_DIR)
	@echo OBJ_DIR = $(OBJ_DIR)
	@echo AD_DIR = $(AD_DIR)
	@echo JNI_GRAPHICS_DIR = $(JNI_GRAPHICS_DIR)
	@echo JAVA_DIR = $(JAVA_DIR)
	@echo JNI_LINK = $(JNI_LINK)
	@echo CFLAGS = $(CFLAGS)
	@echo EXE = $(EXE)

ALL: \
	libmovemod.a \
	rtagest21 \
	makemap \
	rtagmove \
	fit_tab \
	rtac \
	covar21 \
	gis_files \


BIN:
	mv -fv rtagest21 $(BIN_DIR)
	mv -fv makemap $(BIN_DIR)
	mv -fv rtagmove $(BIN_DIR)
	mv -fv fit_tab $(BIN_DIR)
	mv -fv rtac $(BIN_DIR)
	mv -fv covar21 $(BIN_DIR)
	mv -fv gis_files $(BIN_DIR)


CLEAN:
	rm -f $(OBJ_DIR)/*.obj
	rm -f $(OBJ_DIR)/*.a
	rm -f *.a
	rm -f *.so
	rm -f *.obj
  

include objects.lst

libmovemod.a: $(LIBMOVEMOD_OBJECTS)
	ar -rc $(OBJ_DIR)/libmovemod.a $^

TEST_OBJECTS = liketest.obj

makepar: libmovemod.a $(MAKEPAR_OBJECTS)
	$(LL) $^ -L$(OBJ_DIR) -L$(AD_DIR)/lib  -lmovemod -lado -ladt -o makepar 

setw: setw.obj
	$(LL) $^ -L. -L$(AD_DIR)/lib  -lmovemod -lado -lor -o $@

liketest: liketest.obj
	$(LL) $^ -L. -L$(AD_DIR)/lib  -lmovemod -lads -lor -o $@

walk: libmovemod.a $(WORMWALK_OBJECTS) 
	$(LL) $^ -L$(OBJ_DIR) -L$(AD_DIR)/lib $(JNI_LINK) -lmovemod -lado -ladt -o $@
	mv $@$(EXE) $(BIN_DIR)
#$(LL) $^ -L$(OBJ_DIR) -L$(AD_DIR)/lib  -lmovemod -lado -ladt -o $@

medianest: libmovemod.a $(RTAGEST_OBJECTS) medianlk.obj
	$(LL) $^ -L. -L$(AD_DIR)/lib  -lmovemod -lado -lor -o medianest 

test_est: libmovemod.a $(TEST_EST_OBJECTS)
	$(LL) $^ -L. -L$(AD_DIR)/lib  -lmovemod -lado -lor -o test_est 

trimest: libmovemod.a $(TRIMEST_OBJECTS)
	$(LL) $^ -L. -L$(AD_DIR)/lib  -lmovemod -lado -lor -o trimest 

makemap: libmovemod.a $(MAKEMAP_OBJECTS)
	$(LL) $^ -L. -L$(AD_DIR)/lib  -lmovemod -lado -lor -o makemap 

rtagest21: libmovemod.a $(RTAGEST_OBJECTS)
	$(LL) $^ -L$(OBJ_DIR) -L$(AD_DIR)/lib  -lmovemod -lado -ladt -o rtagest21 

covar21: libmovemod.a $(COVAR_OBJECTS)
	$(LL) $^ -L. -L$(AD_DIR)/lib  -lmovemod -lado -lor -o covar21 

fit_tab: $(FIT_TAB_OBJECTS) 
	$(LL) $^ -L. -L$(AD_DIR)/lib -lmovemod -lado -lor -o $@

rtac: $(RTAC_OBJECTS) 
	$(LL) $^ -L. -L$(AD_DIR)/lib -lmovemod -lado -lor -o $@

gis_files: gis_files.obj
	$(LL) $^ -L. -L$(AD_DIR)/lib -lmovemod -lado -lor -o $@

rtagmove: libmovemod.a $(EXE_RTAGMOVE_OBJECTS)
	$(LL)  $^ -L$(OBJ_DIR) -L$(AD_DIR)/lib $(JNI_LINK) -lmovemod -lado -ladt -o $@
#$(LL)  $^ -L$(OBJ_DIR) -L$(AD_DIR)/lib -L$(JNI_GRAPHICS_DIR) -L$(JAVA_HOME)/jre/lib/i386/client -lmovemod -lado -ladt -ljnigraphics3 -o $@
#$(LL)  $^ -L$(OBJ_DIR) -L$(AD_DIR)/lib  -L$(JNI_GRAPHICS_DIR)/lib -L$(JAVA_HOME)/jre/lib/i386/client -lmovemod -lado -ladt -ljnigraphics3 -ljvm -o $@
	mv $@$(EXE) $(BIN_DIR)

t.s: t.cpp
	cpp $(EFLAGS) $< -o t.s

t: t.obj ran1.obj
	$(LL)  $^ -L$(OBJ_DIR) -L$(AD_DIR)/lib $(JNI_LINK) -lado -ladt -o $@

tag_effort: libmovemod.a $(TAG_EFFORT_OBJECTS)
	$(LL)  $^ -L. -L$(AD_DIR)/lib  -L$(JNI_GRAPHICS_DIR)/linux/lib -lmovemod -lado -lor -ljniGraphics -o tag_effort

exe_tagmove15.obj: tagmove15.cpp 
	$(CC) $(CFLAGS) -D__MAKE_EXECUTABLE__ tagmove15.cpp -o $(OBJ_DIR)/exe_tagmove15.obj

banner.obj: banner.cpp 
	$(CC) $(CFLAGS) -D__PWD__='"$(PWD)"' banner.cpp -o $(OBJ_DIR)/banner.obj


###############

.cpp.obj:
	$(CC) $(CFLAGS) $< -o $(OBJ_DIR)/$@
#$(CC) $(CFLAGS) $*.cpp -o $(OBJ_DIR)/$*.obj



	
