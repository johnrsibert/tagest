.SUFFIXES: .cpp .o

#HEADERS=TestTagestAdmodel.h MinimizerTemplate.h AdmodelTemplate.h SimpleAdmodel.h
SRCS=TestMain.cpp $(HEADERS:%.h=%.cpp)

##############################################################
CPPUNIT_HOME=/usr/local/cppunit
LOG4CXX_HOME=/usr/local/log4cxx-svn
APR_HOME=/usr/local/apr
APRUTIL_HOME=/usr/local/apr-util
DEBUG=-pg -g
BINPATH=.
##############################################################

LOCAL_CFLAGS=-I../tpl/build -Isrc -I../src -I$(CPPUNIT_HOME)/include -I$(LOG4CXX_HOME)/include -I$(APR_HOME)/include -I$(APRUTIL_HOME)/include $(CFLAGS)
LOCAL_LFLAGS_ONLY=$(CPPUNIT_HOME)/lib/libcppunit.a $(APRUTIL_HOME)/lib/libaprutil-1.a $(APR_HOME)/lib/libapr-1.a -ldl -lpthread -lxml2 -lexpat
LOCAL_LFLAGS=$(LOCAL_LFLAGS_ONLY) $(LFLAGS)

OBJECTS=$(RTAGEST_OBJECTS) $(NNTAGEST_OBJECTS) $(filter-out tagest14.o, $(LIBMOVEMOD_OBJECTS))
TestHeaders = \
TestHeader-bound.h.cpp \
TestHeader-ch_type.h.cpp \
TestHeader-coff_t.h.cpp \
TestHeader-dbmatrix.h.cpp \
TestHeader-ers12.h.cpp \
TestHeader-etopo5.h.cpp \
TestHeader-etopo_area.h.cpp \
TestHeader-factoral.h.cpp \
TestHeader-fish_rec.h.cpp \
TestHeader-generic.h.cpp \
TestHeader-geo_cord.h.cpp \
TestHeader-habitat.h.cpp \
TestHeader-intersav.h.cpp \
TestHeader-ipar.h.cpp \
TestHeader-latitude.h.cpp \
TestHeader-linbcg.h.cpp \
TestHeader-longlat.h.cpp \
TestHeader-longtude.h.cpp \
TestHeader-modlarea.h.cpp \
TestHeader-neural_net.h.cpp \
TestHeader-neural_net_vector.h.cpp \
TestHeader-par_t.h.cpp \
TestHeader-par_t_nn.h.cpp \
TestHeader-par_t_reg.h.cpp \
TestHeader-PathFileParser.h.cpp \
TestHeader-pattdisp.h.cpp \
TestHeader-policy.h.cpp \
TestHeader-prnstrem.h.cpp \
TestHeader-prn_t.h.cpp \
TestHeader-recap_t.h.cpp \
TestHeader-record3.h.cpp \
TestHeader-shift.h.cpp \
TestHeader-trace.h.cpp \
TestHeader-wrap_num.h.cpp \
TestHeader-xypair.h.cpp \
TestHeader-yr_mon2.h.cpp
#TestHeader-tag_pack3.h.cpp
#TestHeader-preproc3.h.cpp

#all: testHeaders test
all: test

#$(BINPATH)/testReadERSSTv2File
#$(BINPATH)/testDymData
#$(BINPATH)/testTpl
#$(BINPATH)/testTagestNeuralNet
#$(BINPATH)/testTagestRegional
#$(BINPATH)/testReadSeaSurfaceTemperatureFile
test:\
$(BINPATH)/testMinimizerTemplates \
$(BINPATH)/binaryHabitatDataToXml \
$(BINPATH)/testReadSouthernOscillationIndexFile

../build/%.o :

build/%.o : src/%.cpp
	@test -d build || mkdir -v build;echo ------- Compile test/$@; $(CC) -o$@ $(DEBUG) -c $(LOCAL_CFLAGS) $(filter %.cpp, $^)

build/tagest.o : ../tpl/build/tagest.cpp
	@echo ------- Compile test/$@; $(CC) -o$@ $(DEBUG) -c -Dmain=tagest $(LOCAL_CFLAGS) $(filter %.cpp, $^)

testHeaders: $(patsubst %.cpp,build/%.o, $(TestHeaders))
	@echo "------- Compiled all TestHeaders"

$(BINPATH)/testMinimizerTemplates: $(patsubst %.cpp,build/%.o, TestMain.cpp MinimizerTemplate.cpp TestMinimizerTemplates.cpp)
	@echo "------- Link test/$@";\
	$(LL) -o$@ $^ $(LOCAL_LFLAGS)

$(BINPATH)/testTpl: $(patsubst %.cpp,build/%.o, TestMain.cpp TestTpl.cpp) build/tagest.o ../tpl/build/par_t_reg-computeObjectiveFunctionValue.o ../tpl/build/par_t_nn-computeObjectiveFunctionValue.o
	@echo "------- Link test/$@";\
	$(LL) -o$@ $^ $(OBJECTS:%.o=../build/%.o) $(LOCAL_LFLAGS)

$(BINPATH)/testTagestRegional: $(patsubst %.cpp,build/%.o, TestMain.cpp TestTagest.cpp TestTagestRegionalReset.cpp TestTagestGradients.cpp) build/tagest.o ../tpl/build/par_t_reg-computeObjectiveFunctionValue.o ../tpl/build/par_t_nn-computeObjectiveFunctionValue.o
	@echo "------- Link test/$@";\
	$(LL) -o$@ $^ $(OBJECTS:%.o=../build/%.o) $(LOCAL_LFLAGS)

$(BINPATH)/testTagestNeuralNet: $(patsubst %.cpp,build/%.o, TestMain.cpp TestTagest.cpp TestTagestNeuralNet.cpp TestTagestNeuralNetReset.cpp TestTagestGradients.cpp) build/tagest.o ../tpl/build/par_t_reg-computeObjectiveFunctionValue.o ../tpl/build/par_t_nn-computeObjectiveFunctionValue.o
	@echo "------- Link test/$@";\
	$(LL) -o$@ $^ $(OBJECTS:%.o=../build/%.o) $(LOCAL_LFLAGS)

$(BINPATH)/testDymData: $(patsubst %.cpp,build/%.o, TestMain.cpp TestTemplate.cpp TestDymData.cpp) ../build/DymData.o ../build/longtude.o ../build/latitude.o ../build/longlat.o ../build/wrap_num.o
	@echo "------- Link test/$@";\
	$(LL) -o$@ $^ $(LOCAL_LFLAGS_ONLY) -L/usr/local/admb/lib -ldf1b2o -ladmod -ladt -lado -ldf1b2o -ladmod -ladt -lado

$(BINPATH)/binaryHabitatDataToXml: $(patsubst %.cpp,build/%.o, binaryHabitatDataToXml.cpp) 
	@echo "------- Link test/$@";\
	$(LL) -o$@ $^ $(LOCAL_LFLAGS_ONLY) -L/usr/local/admb/lib -ldf1b2o -ladmod -ladt -lado -ldf1b2o -ladmod -ladt -lado

$(BINPATH)/testReadSouthernOscillationIndexFile: $(patsubst %.cpp,build/%.o, TestMain.cpp TestReadSouthernOscillationIndexFile.cpp) 
	@echo "------- Link test/$@";\
	$(LL) -o$@ $^ ../build/linear_scale.o ../build/dminmax.o ../build/readSouthernOscillationIndexFile.o $(LOCAL_LFLAGS_ONLY) -L/usr/local/admb/lib -ldf1b2o -ladmod -ladt -lado -ldf1b2o -ladmod -ladt -lado

$(BINPATH)/testReadSeaSurfaceTemperatureFile: $(patsubst %.cpp,build/%.o, TestMain.cpp TestReadSeaSurfaceTemperatureFile.cpp sstGroupByLongitudeLatitude.cpp) build/tagest.o ../tpl/build/par_t_reg-computeObjectiveFunctionValue.o ../tpl/build/par_t_nn-computeObjectiveFunctionValue.o
	@echo "------- Link test/$@";\
	$(LL) -o$@ $^ $(OBJECTS:%.o=../build/%.o) $(LOCAL_LFLAGS)

$(BINPATH)/testReadERSSTv2File: $(patsubst %.cpp,build/%.o, TestMain.cpp TestReadERSSTv2File.cpp) ../build/readERSSTv2File.o
	@echo "------- Link test/$@";\
	$(LL) -o$@ $^ $(LOCAL_LFLAGS_ONLY) -L/usr/local/admb/lib -ldf1b2o -ladmod -ladt -lado -ldf1b2o -ladmod -ladt -lado

#dev:\
checkTestDymData

#checkTestTagest
#checkTestReadSeaSurfaceTemperatureFile 
#checkTestElevation
#checkTestReadSouthernOscillationIndexFile
#checkTestMinimizerTemplates

#checkTestReadERSSTv2File

check:\
checkTestMakeIfr \
checkTestMakePar \
checkTestDistance \
checkTestMissingFileSignature \
checkTestAverageFishingMortality \
checkTestTagestTplMultipleOutputFiles \
checkTestNoVariablesToEstimate
	@rm -f fmin.log
	@rm -f gmon.out

check2:\
checkTestMakeNeuralNetHabitatData

checkTestMakeNeuralNetHabitatData:
	@cd movemod/habdata/;PATH=../../..:../../bin:$(PATH) testMakeNeuralNetHabitatData.sh

checkTestAverageFishingMortality:
	@cd movemod/rttp/grid16/testAverageFishingMortality/;../../../../bin/testAverageFishingMortality.sh

checkTestTagestTplMultipleOutputFiles:
	@cd movemod/rttp/grid16/testTagestTplMultipleOutputFiles/;../../../../bin/testTagestTplMultipleOutputFiles.sh

checkTestReadSeaSurfaceTemperatureFile:
	@printf "Checking readSeaSurfaceTemperatureFile... ";cd movemod/ssap/grid10/3-upwind-sst2/;../../../../testReadSeaSurfaceTemperatureFile > /dev/null

checkTestReadERSSTv2File:
	@printf "Checking testReadERSSTv2File... ";cd movemod/ssap/grid10/;../../../testReadERSSTv2File

checkTestMakeIfr:
	@cd movemod/ssap/grid10/;../../../bin/testMakeIfr.sh
	@cd movemod/ssap/grid16/;../../../bin/testMakeIfr.sh
	@cd movemod/rttp/grid10/;../../../bin/testMakeIfr.sh
	@cd movemod/rttp/grid16/;../../../bin/testMakeIfr.sh

checkTestMakePar:
	@cd movemod/ssap/grid10/;../../../bin/testMakePar.sh
	@cd movemod/ssap/grid16/;../../../bin/testMakePar.sh
	@cd movemod/rttp/grid10/;../../../bin/testMakePar.sh
	@cd movemod/rttp/grid16/;../../../bin/testMakePar.sh

checkTestElevation:
	@cd movemod/ssap/grid10/;../../../bin/testElevation.sh

checkTestDistance:
	@cd movemod/ssap/grid10/;../../../bin/testDistance.sh

checkTestDymData:
	@printf "Checking testDymData... "; cd movemod/ssap/grid10/test;../../../../testDymData

checkTestMinimizerTemplates:
	@printf "Checking testMinimizerTemplates... "; ./testMinimizerTemplates > /dev/null

checkTestTagest:
	@cd movemod/ssap/grid10/fit_p0b-centered;printf "Checking testTagestRegional in \"`pwd`\"... ";../../../../testTagestRegional > /dev/null
	@cd movemod/ssap/grid10/fit_p0b-upwind;printf "Checking testTagestRegional in \"`pwd`\"... ";../../../../testTagestRegional > /dev/null
	@cd movemod/ssap/grid10/3-centered;printf "Checking testTagestNeuralNet in \"`pwd`\"... ";../../../../testTagestNeuralNet > /dev/null
	@cd movemod/ssap/grid10/3-upwind;printf "Checking testTagestNeuralNet in \"`pwd`\"...  ";../../../../testTagestNeuralNet > /dev/null

checkTestMissingFileSignature:
	@cd movemod/ssap/grid10/missingFileSignature/;../../../../bin/testMissingFileSignature.sh ../../../../../rtagest25
	@cd movemod/ssap/grid10/missingFileSignature/;../../../../bin/testMissingFileSignature.sh ../../../../../rtagest25b
	@cd movemod/ssap/grid10/missingFileSignature/;../../../../bin/testMissingFileSignature.sh ../../../../../ntagest25
	@cd movemod/ssap/grid10/missingFileSignature/;../../../../bin/testMissingFileSignature.sh ../../../../../ntagest25b

checkTestNoVariablesToEstimate:
	@cd movemod/ssap/grid10/00NU-000-0000/;../../../../bin/testNoVariablesToEstimate.sh ../../../../../rtagest25
	@cd movemod/ssap/grid10/00NU-000-0000/;../../../../bin/testNoVariablesToEstimate.sh ../../../../../rtagest25b
	@cd movemod/ssap/grid10/00NU-000-0000/;../../../../bin/testNoVariablesToEstimate.sh ../../../../../ntagest25
	@cd movemod/ssap/grid10/00NU-000-0000/;../../../../bin/testNoVariablesToEstimate.sh ../../../../../ntagest25b
	@cd movemod/ssap/grid10/00NC-000-0000/;../../../../bin/testNoVariablesToEstimate.sh ../../../../../rtagest25
	@cd movemod/ssap/grid10/00NC-000-0000/;../../../../bin/testNoVariablesToEstimate.sh ../../../../../rtagest25b
	@cd movemod/ssap/grid10/00NC-000-0000/;../../../../bin/testNoVariablesToEstimate.sh ../../../../../ntagest25
	@cd movemod/ssap/grid10/00NC-000-0000/;../../../../bin/testNoVariablesToEstimate.sh ../../../../../ntagest25b

checkTestReadSouthernOscillationIndexFile:
	@printf "Checking readSouthernOscillationIndexFile... ";cd movemod/ssap/grid10/test;../../../../testReadSouthernOscillationIndexFile > /dev/null

run: runTagestOriginal runTagestModified runTagestTpl runObsTac

runTagestOriginal: runTagestOriginal00 runTagestOriginal01 runTagestOriginal02 runTagestOriginal03 runTagestOriginal04 runTagestOriginal05

runTagestOriginal00:
	@cd movemod/ssap/grid10/fit_p0b-upwind;../../../../bin/runTagestOriginal.sh

runTagestOriginal01:
	@cd movemod/ssap/grid10/3-centered;../../../../bin/runTagestOriginal.sh

runTagestOriginal02:
	@cd movemod/ssap/grid10/fit_p0b-centered;../../../../bin/runTagestOriginal.sh

runTagestOriginal03:
	@cd movemod/ssap/grid10/3-upwind;../../../../bin/runTagestOriginal.sh

runTagestOriginal04:
	@cd movemod/ssap/grid10/3-centered-soi2;../../../../bin/runTagestOriginal.sh

runTagestOriginal05:
	@cd movemod/ssap/grid10/3-upwind-soi2;../../../../bin/runTagestOriginal.sh

runTagestModified: runTagestModified00 runTagestModified01 runTagestModified02 runTagestModified03 runTagestModified04 runTagestModified05

runTagestModified00:
	@cd movemod/ssap/grid10/fit_p0b-upwind;../../../../bin/runTagestModified.sh

runTagestModified01:
	@cd movemod/ssap/grid10/3-centered;../../../../bin/runTagestModified.sh

runTagestModified02:
	@cd movemod/ssap/grid10/fit_p0b-centered;../../../../bin/runTagestModified.sh

runTagestModified03:
	@cd movemod/ssap/grid10/3-upwind;../../../../bin/runTagestModified.sh

runTagestModified04:
	@cd movemod/ssap/grid10/3-centered-soi2;../../../../bin/runTagestModified.sh

runTagestModified05:
	@cd movemod/ssap/grid10/3-upwind-soi2;../../../../bin/runTagestModified.sh

runTagestTpl: runTagestTpl00 runTagestTpl01 runTagestTpl02 runTagestTpl03 runTagestTpl04 runTagestTpl05

runTagestTpl00:
	@cd movemod/ssap/grid10/fit_p0b-upwind;../../../../bin/runTagestTpl.sh

runTagestTpl01:
	@cd movemod/ssap/grid10/3-centered;../../../../bin/runTagestTpl.sh

runTagestTpl02:
	@cd movemod/ssap/grid10/fit_p0b-centered;../../../../bin/runTagestTpl.sh

runTagestTpl03:
	@cd movemod/ssap/grid10/3-upwind;../../../../bin/runTagestTpl.sh

runTagestTpl04:
	@cd movemod/ssap/grid10/3-centered-soi2;../../../../bin/runTagestTpl.sh

runTagestTpl05:
	@cd movemod/ssap/grid10/3-upwind-soi2;../../../../bin/runTagestTpl.sh

runTagestTplDev: runTagestTplDev00 runTagestTplDev01 runTagestTplDev02 runTagestTplDev03 runTagestTplDev04 runTagestTplDev05

runTagestTplDev00:
	@cd movemod/ssap/grid10/fit_p0b-upwind;../../../../bin/runTagestTplDev.sh

runTagestTplDev01:
	@cd movemod/ssap/grid10/3-centered;../../../../bin/runTagestTplDev.sh

runTagestTplDev02:
	@cd movemod/ssap/grid10/fit_p0b-centered;../../../../bin/runTagestTplDev.sh

runTagestTplDev03:
	@cd movemod/ssap/grid10/3-upwind;../../../../bin/runTagestTplDev.sh

runTagestTplDev04:
	@cd movemod/ssap/grid10/3-centered-soi2;../../../../bin/runTagestTplDev.sh

runTagestTplDev05:
	@cd movemod/ssap/grid10/3-upwind-soi2;../../../../bin/runTagestTplDev.sh

runObsTac:
	@cd movemod/ssap/grid10/1;../../../../bin/$@.sh

cpd.log:
	@/usr/local/pmd/bin/cpd.sh ../src > $@

clean:
	@rm -vrf build
	@rm -vf testTagest
	@rm -vf checkTest*
	@rm -vf test*
	@rm -vf gmon.out
	@rm -vf binaryHabitatDataToXml
	@rm -vf fmin.log
	@rm -vf movemod/nbt/worm-test/walksim.tag
	@rm -vf movemod/nbt/worm-test/gmon.out
	@rm -vf movemod/nbt/worm-test/wormhead.dat
	@rm -vf movemod/nbt/worm-test/walk.log
	@rm -vf movemod/ssap/grid10/grid10.dst
	@rm -vf movemod/ssap/grid10/grid10.ifr
	@rm -vf movemod/ssap/grid10/gmon.out
	@rm -vf movemod/ssap/grid10/grid10.zdf
	@rm -vf movemod/ssap/grid10/grid10.p00
	@rm -vf movemod/ssap/grid10/grid10.zdt
	@rm -vf movemod/ssap/grid10/grid10.tag
	@rm -vf movemod/ssap/grid10/3-upwind-sst2/gmon.out
	@rm -vf movemod/ssap/grid10/3-upwind/gmon.out
	@rm -vf movemod/ssap/grid10/3-upwind/variance
	@rm -vf movemod/ssap/grid10/3-upwind/fmin.log
	@rm -vf movemod/ssap/grid10/3-upwind/eigv.rpt
	@rm -vf movemod/ssap/grid10/test/gmon.out
	@rm -vf movemod/ssap/grid10/fit_p0b-upwind/gmon.out
	@rm -vf movemod/ssap/grid10/fit_p0b-upwind/variance
	@rm -vf movemod/ssap/grid10/fit_p0b-upwind/fmin.log
	@rm -vf movemod/ssap/grid10/fit_p0b-upwind/eigv.rpt
	@rm -vf movemod/ssap/grid10/3-centered/gmon.out
	@rm -vf movemod/ssap/grid10/3-centered/variance
	@rm -vf movemod/ssap/grid10/3-centered/fmin.log
	@rm -vf movemod/ssap/grid10/3-centered/eigv.rpt
	@rm -vf movemod/ssap/grid10/fit_p0b-centered/gmon.out
	@rm -vf movemod/ssap/grid10/fit_p0b-centered/variance
	@rm -vf movemod/ssap/grid10/fit_p0b-centered/fmin.log
	@rm -vf movemod/ssap/grid10/fit_p0b-centered/eigv.rpt
	@rm -vf movemod/ssap/grid16/grid16.p00
	@rm -vf movemod/ssap/grid16/grid16.tag
	@rm -vf movemod/ssap/grid16/grid16.ifr
	@rm -vf movemod/rttp/grid10/skj10.p00
	@rm -vf movemod/rttp/grid10/skj10.tag
	@rm -vf movemod/rttp/grid10/skj10.ifr
	@rm -vf movemod/rttp/grid16/skj16.p00
	@rm -vf movemod/rttp/grid16/skj16.tag
	@rm -vf movemod/rttp/grid16/skj16.ifr
	@rm -vf movemod/ssap/grid10/3-centered-soi2/skj10.p01
	@rm -vf movemod/ssap/grid10/3-centered-soi2/tagestModified.log
	@rm -vf movemod/ssap/grid10/3-centered-soi2/tagest.par
	@rm -vf movemod/ssap/grid10/3-centered-soi2/tagestOriginal.log
	@rm -vf movemod/ssap/grid10/3-upwind/skj10.p01
	@rm -vf movemod/ssap/grid10/3-upwind/tagestModified.log
	@rm -vf movemod/ssap/grid10/3-upwind/tagest.par
	@rm -vf movemod/ssap/grid10/3-upwind/tagestOriginal.log
	@rm -vf movemod/ssap/grid10/3-upwind-soi2/skj10.p01
	@rm -vf movemod/ssap/grid10/3-upwind-soi2/tagestModified.log
	@rm -vf movemod/ssap/grid10/3-upwind-soi2/tagest.par
	@rm -vf movemod/ssap/grid10/3-upwind-soi2/tagestOriginal.log
	@rm -vf movemod/ssap/grid10/fit_p0b-upwind/skj10.p01
	@rm -vf movemod/ssap/grid10/fit_p0b-upwind/tagestModified.log
	@rm -vf movemod/ssap/grid10/fit_p0b-upwind/tagest.par
	@rm -vf movemod/ssap/grid10/fit_p0b-upwind/tagestOriginal.log
	@rm -vf movemod/ssap/grid10/3-centered/skj10.p01
	@rm -vf movemod/ssap/grid10/3-centered/tagestModified.log
	@rm -vf movemod/ssap/grid10/3-centered/tagest.par
	@rm -vf movemod/ssap/grid10/3-centered/tagestOriginal.log
	@rm -vf movemod/ssap/grid10/1/fit02_P1_ota.csv
	@rm -vf movemod/ssap/grid10/1/tac.log
	@rm -vf movemod/ssap/grid10/fit_p0b-centered/skj10.p01
	@rm -vf movemod/ssap/grid10/fit_p0b-centered/tagest.par
	@rm -vf bin/runObsTac.sh.log
	@rm -vf movemod/ssap/grid10/testElevation.log
	@rm -vf movemod/ssap/grid10/elevation.log
	@rm -vf movemod/ssap/grid10/3-centered-soi2/skj10.ppp
	@rm -vf movemod/ssap/grid10/3-centered-soi2/skj10.tmp
	@rm -vf movemod/ssap/grid10/3-centered-soi2/tagestTpl.log
	@rm -vf movemod/ssap/grid10/3-centered-soi2/variance
	@rm -vf movemod/ssap/grid10/3-centered-soi2/tagest.bar
	@rm -vf movemod/ssap/grid10/3-centered-soi2/eigv.rpt
	@rm -vf movemod/ssap/grid10/3-centered-soi2/gmon.out
	@rm -vf movemod/ssap/grid10/3-centered-soi2/tagest.log
	@rm -vf movemod/ssap/grid10/3-centered-soi2/fmin.log
	@rm -vf movemod/ssap/grid10/3-centered-soi2/tagest.rep
	@rm -vf movemod/ssap/grid10/3-upwind/skj10.ppp
	@rm -vf movemod/ssap/grid10/3-upwind/tagest.log
	@rm -vf movemod/ssap/grid10/3-upwind/tagestTpl.log
	@rm -vf movemod/ssap/grid10/3-upwind/skj10.tmp
	@rm -vf movemod/ssap/grid10/3-upwind/tagest.bar
	@rm -vf movemod/ssap/grid10/3-upwind/tagest.rep
	@rm -vf movemod/ssap/grid10/3-upwind-soi2/skj10.ppp
	@rm -vf movemod/ssap/grid10/3-upwind-soi2/tagestTpl.log
	@rm -vf movemod/ssap/grid10/3-upwind-soi2/variance
	@rm -vf movemod/ssap/grid10/3-upwind-soi2/tagest.bar
	@rm -vf movemod/ssap/grid10/3-upwind-soi2/eigv.rpt
	@rm -vf movemod/ssap/grid10/3-upwind-soi2/gmon.out
	@rm -vf movemod/ssap/grid10/3-upwind-soi2/tagest.log
	@rm -vf movemod/ssap/grid10/3-upwind-soi2/fmin.log
	@rm -vf movemod/ssap/grid10/3-upwind-soi2/tagest.rep
	@rm -vf movemod/ssap/grid10/fit_p0b-upwind/skj10.ppp
	@rm -vf movemod/ssap/grid10/fit_p0b-upwind/tagest.log
	@rm -vf movemod/ssap/grid10/fit_p0b-upwind/tagestTpl.log
	@rm -vf movemod/ssap/grid10/fit_p0b-upwind/tagest.bar
	@rm -vf movemod/ssap/grid10/fit_p0b-upwind/tagest.rep
	@rm -vf movemod/ssap/grid10/3-centered/skj10.ppp
	@rm -vf movemod/ssap/grid10/3-centered/tagest.log
	@rm -vf movemod/ssap/grid10/3-centered/tagestTpl.log
	@rm -vf movemod/ssap/grid10/3-centered/tagest.bar
	@rm -vf movemod/ssap/grid10/3-centered/tagest.rep
	@rm -vf movemod/ssap/grid10/fit_p0b-centered/skj10.ppp
	@rm -vf movemod/ssap/grid10/fit_p0b-centered/tagest.log
	@rm -vf movemod/ssap/grid10/fit_p0b-centered/tagestTpl.log
	@rm -vf movemod/ssap/grid10/fit_p0b-centered/tagestModified.log
	@rm -vf movemod/ssap/grid10/fit_p0b-centered/tagest.bar
	@rm -vf movemod/ssap/grid10/fit_p0b-centered/tagest.rep
	@rm -vf movemod/ssap/grid10/fit_p0b-upwind/admodel.hes
	@rm -vf movemod/ssap/grid10/1/admodel.hes
	@rm -vf movemod/ssap/grid10/1/skj10.ppp
	@rm -vf movemod/ssap/grid10/1/skj10.p01
	@rm -vf movemod/ssap/grid10/1/skj10.p02
	@rm -vf movemod/ssap/grid10/1/variance
	@rm -vf movemod/ssap/grid10/1/skj10.p03
	@rm -vf movemod/ssap/grid10/1/skj10.p04
	@rm -vf movemod/ssap/grid10/1/admodel.cov
	@rm -vf movemod/ssap/grid10/1/skj10.p05
	@rm -vf movemod/ssap/grid10/1/skj10.p06
	@rm -vf movemod/ssap/grid10/1/skj10.p07
	@rm -vf movemod/ssap/grid10/1/eigv.rpt
	@rm -vf movemod/ssap/grid10/1/parsave.tmp
	@rm -vf movemod/ssap/grid10/1/gmon.out
	@rm -vf movemod/ssap/grid10/1/out
	@rm -vf movemod/ssap/grid10/1/tagest.log
	@rm -vf movemod/ssap/grid10/1/fmin.log
	@rm -vf movemod/ssap/grid10/1/admodel.dep
	@rm -vf movemod/rttp/grid16/test/skj16.par
	@rm -vf movemod/rttp/grid16/test/admodel.hes
	@rm -vf movemod/rttp/grid16/test/gmon.out
	@rm -vf movemod/rttp/grid16/test/tagest.log
	@rm -vf movemod/rttp/grid16/test/variance
	@rm -vf movemod/rttp/grid16/test/fmin.log
	@rm -vf movemod/rttp/grid16/test/file_nam.tmp
	@rm -vf movemod/rttp/grid16/test/admodel.cov
	@rm -vf movemod/rttp/grid16/test/admodel.dep
	@rm -vf movemod/rttp/grid16/test/skj16.ppp
	@rm -vf movemod/rttp/grid16/test/skj16.p01
	@rm -vf movemod/rttp/grid16/test/eigv.rpt
	@rm -vf movemod/rttp/grid16/test/skj16.p02
