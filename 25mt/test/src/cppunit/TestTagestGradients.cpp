/**
 * $Id$
 */

#include <iostream>
using std::cerr;
using std::endl;
#include <fstream>
using std::ifstream;
#include <string>
using std::string;
#include <stdexcept>
using std::runtime_error;

#include <cppunit/extensions/HelperMacros.h>

#include "par_t.h"

class TestTagestGradients
: public CppUnit::TestFixture
{
public:
TestTagestGradients() {;}
TestTagestGradients(const TestTagestGradients&);
virtual ~TestTagestGradients() {;}

public:
CPPUNIT_TEST_SUITE(TestTagestGradients);
CPPUNIT_TEST(testFilename);
CPPUNIT_TEST(testConstructionDestruction);
CPPUNIT_TEST_SUITE_END();
public:
void setUp() {
        ifstream ifs("file_nam.tmp");
        if (!ifs) {
                cerr << "Cannot open file_nam.tmp in main()" << endl;
                return;
        }
        ifs >> filename;
        ifs.close();
}
void tearDown() {
}
public:
void testFilename() {
	CPPUNIT_ASSERT(filename.length() > 0);
}
void testConstructionDestruction() {
	typedef par_t<d3_array,dmatrix,dvector,double> param;
	param::createInstance(filename.c_str());
	CPPUNIT_ASSERT(param::instance != 0);
	param::deleteInstance();
	CPPUNIT_ASSERT(param::instance == 0);

	gradient_structure gs;
	typedef par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable> vparam;
	vparam::createInstance(filename.c_str());
	CPPUNIT_ASSERT(vparam::instance != 0);
	vparam::deleteInstance();
	CPPUNIT_ASSERT(vparam::instance == 0);
}
private:
string filename;
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestTagestGradients);
