/**
 * $Id$
 */

#include <stdexcept>
using std::runtime_error;

#include <cppunit/extensions/HelperMacros.h>

class TestTemplate
	:public CppUnit::TestFixture
{
public:
TestTemplate() {;}
TestTemplate(const TestTemplate&);
virtual ~TestTemplate() {;}

public:
CPPUNIT_TEST_SUITE(TestTemplate);
CPPUNIT_TEST(testAssert);
CPPUNIT_TEST_EXCEPTION(testException, runtime_error);
CPPUNIT_TEST_SUITE_END();
public:
void setUp() {
}
void tearDown() {
}
public://sanity checks
void testAssert() {
	CPPUNIT_ASSERT(true);
}
void testException() {
	throw runtime_error("test");
}
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestTemplate);
