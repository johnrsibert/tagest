#$Id: Makefile 3125 2013-11-19 02:44:01Z jsibert $
#
.SUFFIXES: .tpl .o
.PRECIOUS: %.cpp %.o
TPL=tagest.tpl
XTPL=xtagest.tpl


#LOCAL_CFLAGS=-I. -I../src -I$(OBJ_DIR) $(CFLAGS)
#LOCAL_LFLAGS=-L.. -lmovemod $(LFLAGS)
#OBJECTS=$(RTAGEST_OBJECTS) $(NNTAGEST_OBJECTS) $(filter-out tagest14.o, rfgcomp.o $(LIBMOVEMOD_OBJECTS))
LOCAL_CFLAGS= -I$(ADMB_HOME)/contrib/include -I../src -I. -I$(OBJ_DIR) $(CFLAGS)
LOCAL_LFLAGS=$(LFLAGS) -L.. -lmovemod $(LFLAGS) 
#OBJECTS=$(RTAGEST_OBJECTS) $(NNTAGEST_OBJECTS) $(filter-out tagest14.o, rfgcomp.o)
OBJECTS=$(RTAGEST_OBJECTS) $(NNTAGEST_OBJECTS)

all: tagest$(EXE)

#tagest: $(TPL:%.tpl=build/%.o) build/par_t_nn-reset.o build/par_t_reg-reset.o build/par_t_reg-computeObjectiveFunctionValue.o build/par_t_nn-computeObjectiveFunctionValue.o build/nn_fgcomp2.o build/rfgcomp2.o build/computeLikelihood.o $(filter-out ../build/par_t_reg-reset2.o ../build/par_t_nn-reset.o ../build/nn_fgcomp2.o ../build/rfgcomp2.o, $(OBJECTS:%.o=../build/%.o))

tagest$(EXE): $(TPL:%.tpl=$(OBJ_DIR)/%.o) $(OBJECTS:%.o=../$(OBJ_DIR)/%.o)
	@echo ------- LOCAL_LFLAGS = $(LOCAL_LFLAGS)
	@echo ------- Link tpl/$@; $(LL) -o $@ $^ $(LOCAL_LFLAGS)

xtagest$(EXE): $(XTPL:%.tpl=$(OBJ_DIR)/%.o) $(OBJECTS:%.o=../$(OBJ_DIR)/%.o)
	@echo ------- LOCAL_LFLAGS = $(LOCAL_LFLAGS)
	@echo ------- Link tpl/$@; $(LL) -o $@ $^ $(LOCAL_LFLAGS)

#tagest$(EXE): $(OBJ_DIR)/tagest.o $(RTAGEST_OBJECTS) $(NNTAGEST_OBJECTS)
#	@echo ------- LOCAL LFLAGS = $(LOCAL_LFLAGS)
#	@echo ------- Link tpl/$@; $(LL) -o $@ $^ $(LOCAL_LFLAGS)

$(OBJ_DIR)/%.o:%.tpl
	@test -d $(OBJ_DIR) || mkdir -v $(OBJ_DIR);cp $< $(OBJ_DIR); cd $(OBJ_DIR); $(ADMB_HOME)/bin/tpl2cpp $(*F)
	@echo LOCAL_CFLAGS = $(LOCAL_CFLAGS)
	@echo ------- Compile tpl/$@; $(CC) -c -o $@ $(LOCAL_CFLAGS) $(OBJ_DIR)/$(*F).cpp

$(OBJ_DIR)/%.o : %.cpp
	@test -d $(OBJ_DIR) || mkdir -v $(OBJ_DIR);echo ------- Compile $@; $(CC) -c $(LOCAL_CFLAGS) -o $@ $<

clean:
	@rm -rvf $(OBJ_DIR)
	@rm -vf tagest$(EXE)
	@rm -vf tagest-dev
	@rm -vf tagest.log
	@rm -vf tagest.par
	@rm -vf tagest.bar
	@rm -vf tagest.rep
	@rm -vf tagest.std
	@rm -vf tagest.eva
	@rm -vf tagest.cor
	@rm -vf tagest.htp
	@rm -vf tagest.cpp
