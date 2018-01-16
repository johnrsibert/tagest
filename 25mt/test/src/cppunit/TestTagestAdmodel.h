#ifndef __TestTagestAdmodel_h__
#define __TestTagestAdmodel_h__
/**
 * $Id: TestCPPUNIT.h 1164 2006-07-24 20:29:27Z johnoel $
 */

#include <stdexcept>
using std::runtime_error;

#include <cppunit/extensions/HelperMacros.h>

#include <admodel.h>
#include "MinimizerTemplate.h"
#include "SimpleAdmodel.h"
#include "tagest.htp"
#include "recap_t.h"
#include "modlarea.h"
#include "par_t.h"
#include "par_treg.h"
#include "coff_t.h"

class TestTagestAdmodel
	:public CppUnit::TestFixture
{
public:
TestTagestAdmodel() {;}
TestTagestAdmodel(const TestTagestAdmodel&);
virtual ~TestTagestAdmodel() {;}

public:
CPPUNIT_TEST_SUITE(TestTagestAdmodel);
CPPUNIT_TEST(testAssert);
CPPUNIT_TEST_EXCEPTION(testException, runtime_error);
CPPUNIT_TEST(testSimpleAdmodel);
CPPUNIT_TEST(testSimpleAdmodel2);
CPPUNIT_TEST(testPar_tSingleton);
CPPUNIT_TEST(testCoff_tSingleton);
CPPUNIT_TEST(testRecaptypeVectorSingleton);
CPPUNIT_TEST(testIndexedRegionalFisheryRecordSingleton);
CPPUNIT_TEST(testModelParameters);
CPPUNIT_TEST(testModelParametersLocalCalcsParameterSection);
CPPUNIT_TEST(testModelParametersReset);
CPPUNIT_TEST(testModelParametersDataSection);
CPPUNIT_TEST(testModelParametersParameterSection);
CPPUNIT_TEST(testModelParametersReadData);
CPPUNIT_TEST(testUvComp);
CPPUNIT_TEST(testUvComp);
CPPUNIT_TEST(testUvinterpolate);
CPPUNIT_TEST(testSigmaComp);
CPPUNIT_TEST(testUvsComp);
CPPUNIT_TEST(testParamVelpen);
CPPUNIT_TEST(testParamSigpen);
CPPUNIT_TEST(testCoffComp);
CPPUNIT_TEST(testGetEffortArray);
CPPUNIT_TEST(testFishMortComp);
CPPUNIT_TEST(testTotalMortComp);
CPPUNIT_TEST(testCoffBAdjust);
/*
CPPUNIT_TEST(testCoffAdi);
CPPUNIT_TEST(testCoffDfAdiWrapper);
CPPUNIT_TEST(testNegativeTagPenalty);
CPPUNIT_TEST(testTotalTagPenalty);
CPPUNIT_TEST(testPoisson);
CPPUNIT_TEST(testModelParametersComputeObjectiveFunctionValue);
*/
//CPPUNIT_TEST(testModelParametersComputations);
//CPPUNIT_TEST(testTagest);
CPPUNIT_TEST_SUITE_END();
public:
void setUp() {
}
public://SelfTest: Sanitiy Checks
void testAssert() {
	CPPUNIT_ASSERT(true);
}
void testException() {
	throw runtime_error("test");
}
void testSimpleAdmodel();
void testSimpleAdmodel2();

public://PreTest: model_parameters mp(arrmblsize,argc,argv);
void testPar_tSingleton();
void testCoff_tSingleton();
void testRecaptypeVectorSingleton();
void testIndexedRegionalFisheryRecordSingleton();

public://Test: model_parameters mp(arrmblsize,argc,argv);
void testModelParameters();
void testModelParametersLocalCalcsParameterSection();

public://PostTest: model_parameters mp(arrmblsize,argc,argv);
void testModelParametersDataSection();
void testModelParametersParameterSection();

public://Test: mp.preliminary_calculations();
void testModelParametersReadData();

public://Test: mp.computations(argc,argv);
void testModelParametersReset();
void testUvComp();
void testUvinterpolate();
void testSigmaComp();
void testUvsComp();
void testParamVelpen();
void testParamSigpen();
void testCoffComp();
void testParamAddTotalNegativeTagPenalty();
void testGetEffortArray();
void testFishMortComp();
void testTotalMortComp();
void testCoffBAdjust();
void testCoffAdi();
void testCoffDfAdiWrapper();
void testNegativeTagPenalty();
void testTotalTagPenalty();
void testPoisson();
void testModelParametersComputeObjectiveFunctionValue();
//PostComputationsTest: mp.computations(argc,argv);

public:
void testModelParametersComputations();//PreGoal: run test with the same calls like main in tagest.cpp
void testTagest();//Goal: run full application

public:
void tearDown() {
}
private:
void testCheckParametersEqual(model_parameters &mp, const par_t_reg &param);
bool isValuesEqual(const double value, const double expected);
void cleanup() {
	if (recaptype_vector::instance != 0) {
        	recaptype_vector::deleteInstance();
	}
	if (indexed_regional_fishery_record::instance != 0) {
	        indexed_regional_fishery_record::deleteInstance();
	}
	if (coff_t::instance != 0) {
        	coff_t::deleteInstance();
	}
	if (par_t::instance != 0) {
        	par_t::deleteInstance();
	}
}
};
#endif
