/**
 * $Id$
 */

#include <stdexcept>
using std::runtime_error;

#include <cppunit/extensions/HelperMacros.h>
#include "par_t_reg.h"
#include "par_t_nn.h"
#include <admodel.h>
#include <tagest.htp>

int tagest(int argc,char * argv[]);

class TestTpl: public CppUnit::TestFixture
{
public:
	TestTpl() {;}
	TestTpl(const TestTpl&);
	virtual ~TestTpl() {;}

public:
	CPPUNIT_TEST_SUITE(TestTpl);
	CPPUNIT_TEST(test1);
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp() 
	{
	}
	void tearDown() 
	{
	}
public:
	void test1() 
	{
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestTpl);
