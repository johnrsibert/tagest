/**
 * $Id$
 */

#include <stdexcept>
using std::runtime_error;

#include <cppunit/extensions/HelperMacros.h>
#include "MinimizerTemplate.h"
#include "Example.h"
#include "ExampleFunnel.h"
#include "SimpleAdmodel.h"

class TestMinimizerTemplates
	:public CppUnit::TestFixture
{
public:
TestMinimizerTemplates() {;}
virtual ~TestMinimizerTemplates() {;}

public:
CPPUNIT_TEST_SUITE(TestMinimizerTemplates);
CPPUNIT_TEST(testDeallocateEmptyDVector);
CPPUNIT_TEST(testExample);
CPPUNIT_TEST(testExample2);
CPPUNIT_TEST(testExample3);
CPPUNIT_TEST(testSimpleModel);
CPPUNIT_TEST(testExampleFunnel);
CPPUNIT_TEST(testExampleFunnelInitialStatistics);
CPPUNIT_TEST_SUITE_END();
public:
void setUp() {
}
void tearDown() {
}
public:
void testDeallocateEmptyDVector() {
	dvector dv;
	dv.deallocate();
	for (int i = dv.indexmin(); i <= dv.indexmax(); i++) {
		CPPUNIT_ASSERT(false);
	}
}
void testExample() {
	gradient_structure gs;
	Example example;
	MinimizerTemplate minimizer;
	CPPUNIT_ASSERT(minimizer.minimize(example) == double(-100));
	CPPUNIT_ASSERT((-5 - 1e-6) < example.get_x() && example.get_x() < (-5 + 1e-6));
	CPPUNIT_ASSERT(minimizer.get_gradients()(1) != 0);
}
void testExample2() {
	gradient_structure gs;
	Example example;
	MinimizerTemplate minimizer;
	CPPUNIT_ASSERT(minimizer.minimize(example) == double(-100));
	CPPUNIT_ASSERT((-5 - 1e-6) < example.get_x() && example.get_x() < (-5 + 1e-6));
	CPPUNIT_ASSERT(minimizer.get_gradients()(1) != 0);
}
void testExample3() {
	gradient_structure gs;
	Example example;
	MinimizerTemplate minimizer;
	CPPUNIT_ASSERT(minimizer.minimize(example) == double(-100));
	CPPUNIT_ASSERT((-5 - 1e-6) < example.get_x() && example.get_x() < (-5 + 1e-6));
	CPPUNIT_ASSERT(minimizer.get_gradients()(1) != 0);
}
void testSimpleModel() {
	gradient_structure gs;
	SimpleAdmodel simple(2, 5);
	MinimizerTemplate minimizer;
	minimizer.minimize(simple);

	//WARNING: after call to gradcalc in MinimizerTemplate the dvariable class members [a and b] in SimpleAdmodel
	//         are set to 0.  [_a and _b] are doubles that save the values of the independantVariables.
	//CPPUNIT_ASSERT(simple.a == simple.get_a());
	//CPPUNIT_ASSERT(simple.b == simple.get_b());
	CPPUNIT_ASSERT(gradient_structure::totalbytes() == 0);
}
void testExampleFunnel() {
	gradient_structure gs;
	ExampleFunnel exampleFunnel;
	MinimizerTemplate minimizer;
	CPPUNIT_ASSERT(minimizer.minimize(exampleFunnel) == double(-100));
	CPPUNIT_ASSERT((-5 - 1e-6) < exampleFunnel.get_x() && exampleFunnel.get_x() < (-5 + 1e-6));
	CPPUNIT_ASSERT(minimizer.get_gradients()(1) != 0);
}
void testExampleFunnelInitialStatistics() {
	gradient_structure gs;
	ExampleFunnel exampleFunnel;

        double objectiveFunctionValue = 0;
                                                                                                                                                                                                     
        const int totalVariables = exampleFunnel.getTotalVariables();
                                                                                                                                                                                                     
        independent_variables independantVariables(1, totalVariables);
        exampleFunnel.initialize(independantVariables);
                                                                                                                                                                                                     
	dvector gradients(1,totalVariables);
        gradients.initialize();
                                                                                                                                                                                                     
        fmm functionMinimizer(totalVariables);
        CPPUNIT_ASSERT(functionMinimizer.ireturn >= 0);
	{
                functionMinimizer.fmin(objectiveFunctionValue, independantVariables, gradients);
                cout << "ireturn: " << functionMinimizer.ireturn << endl; 
                CPPUNIT_ASSERT(functionMinimizer.ireturn > 0);
		objectiveFunctionValue = exampleFunnel.computeObjectiveFunctionValue(independantVariables);
		gradcalc(totalVariables, gradients);
                functionMinimizer.fmin(objectiveFunctionValue, independantVariables, gradients);
                exampleFunnel.output();
        }
}
};
CPPUNIT_TEST_SUITE_REGISTRATION(TestMinimizerTemplates);
