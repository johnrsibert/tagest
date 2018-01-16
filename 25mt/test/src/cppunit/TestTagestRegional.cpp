/**
 * $Id$
 */

#include <iostream>
using std::cout;
using std::endl;
#include <stdexcept>
using std::runtime_error;

#include <cppunit/extensions/HelperMacros.h>
#include <par_t.h>
#include <admodel.h>
#include <tagest.htp>

int tagest(int argc,char * argv[]);

class TestTagestNeuralNet
	:public CppUnit::TestFixture
{
public:
TestTagestNeuralNet() {;}
TestTagestNeuralNet(const TestTagestNeuralNet&);
virtual ~TestTagestNeuralNet() {;}

public:
CPPUNIT_TEST_SUITE(TestTagestNeuralNet);
CPPUNIT_TEST(testCreateInstance);
CPPUNIT_TEST(testTplConstructor);
CPPUNIT_TEST(testTplPreliminaryCalculations);
CPPUNIT_TEST(testTplUserFunction);
//CPPUNIT_TEST(testTplRun);
CPPUNIT_TEST_SUITE_END();
public:
void setUp() {
	ifstream ifs("file_nam.tmp");
	if (!ifs) {
		cerr << "Cannot open file_nam.tmp in main()" << endl;
	} else {
		ifs >> filename;
		ifs.close();
	}
}
void tearDown() {
	par_t<d3_array,dmatrix,dvector,double>::deleteInstance();
}
public:
void testCreateInstance() {
	par_t<d3_array,dmatrix,dvector,double> *param = par_t<d3_array,dmatrix,dvector,double>::createInstance(filename);
	//CPPUNIT_ASSERT(par_t<d3_array,dmatrix,dvector,double>::instance != 0 && param->isRegional() == true && param->isNeuralNet() == false);

	par_t<d3_array,dmatrix,dvector,double>::deleteInstance();
	//CPPUNIT_ASSERT(par_t<d3_array,dmatrix,dvector,double>::instance == 0);
}
void testTplConstructor() {
	char *argv[10] = {"tagest\0"};
	model_parameters mp(350000, 1, argv);
	mp.final_calcs();
}
void testTplPreliminaryCalculations() {
	char *argv[10] = {"tagest\0"};
	model_parameters mp(350000, 1, argv);
	mp.preliminary_calculations();
	mp.final_calcs();
}
void testTplUserFunction() {
	char *argv[10] = {"tagest\0"};
	model_parameters mp(350000, 1, argv);
	mp.preliminary_calculations();
	mp.userfunction();
	mp.final_calcs();
}
void testTplRun() {
        char *argv[10] = {"tagest\0"};
        tagest(1, argv);
}
private:
char filename[9];
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestTagestNeuralNet);
