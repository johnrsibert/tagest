/**
 * $Id$
 */

#include <string>
using std::string;
#include <stdexcept>
using std::runtime_error;

#include <cppunit/extensions/HelperMacros.h>

#include "par_t.h"
#include "recap_t.h"

class TestTagest
: public CppUnit::TestFixture
{
public:
TestTagest() {;}
TestTagest(const TestTagest&);
virtual ~TestTagest() {;}

public:
CPPUNIT_TEST_SUITE(TestTagest);
CPPUNIT_TEST(testOrderingCollapseCohorts);
CPPUNIT_TEST(testGetParFileVersion);
CPPUNIT_TEST(testPathFileParser);
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
void testOrderingCollapseCohorts() {
	typedef par_t<d3_array,dmatrix,dvector,double> Param;
	Param::createInstance(filename.c_str());
	CPPUNIT_ASSERT(Param::instance != 0);

	Param& param = *Param::instance;
	for (int release = 1; release < param.nrelease; release++) {
		CPPUNIT_ASSERT(param.get_tr_cohort(release) < param.get_tr_cohort(release + 1));
	}
	recaptype_vector* recaps = recaptype_vector::createInstance(filename.c_str(), param);
	CPPUNIT_ASSERT(recaptype_vector::instance != 0);

	param.collapse_cohorts(*recaps, recaps->getTotalRecords());
	for (int release = 1; release < param.nrelease; release++) {
		if (param.get_tr_cohort(release) > param.get_tr_cohort(release + 1)) {
			CPPUNIT_ASSERT(param.get_tr_cohort(release) < param.get_tr_cohort(release + 1));
		}
	}

	Param::deleteInstance();
	CPPUNIT_ASSERT(Param::instance == 0);

	recaptype_vector::deleteInstance();
	CPPUNIT_ASSERT(recaptype_vector::instance == 0);
}
void testGetParFileVersion() {
	typedef par_t<d3_array,dmatrix,dvector,double> Param;
	Param::createInstance(filename.c_str());
	CPPUNIT_ASSERT(Param::instance != 0);

	std::string parfile = filename + ".par";
        cifstream cifs(parfile.c_str());
	CPPUNIT_ASSERT(cifs.is_open() == true);
        adstring signature = cifs.signature();
	CPPUNIT_ASSERT(signature.length() > 0);
        cifs.close();
	Param& param = *Param::instance;
	CPPUNIT_ASSERT(param.getParFileVersion() == signature);

	Param::deleteInstance();
	CPPUNIT_ASSERT(Param::instance == 0);
}
void testPathFileParser() {
	adstring input("/home/maguro/johnoel/25/test/benchmark/v20r/ssap/6/skj10.par");
	PathFileParser parser(input);
	CPPUNIT_ASSERT(input(1, 51) == parser.getPath());
	CPPUNIT_ASSERT(input(52, 56) == parser.getRoot());
	CPPUNIT_ASSERT(input(1, 56) == parser.getPathRoot());
}
private:
string filename;
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestTagest);
