/**
 * $Id$
 */

#include <iostream>
#include <stdexcept>
using std::runtime_error;

#include <cppunit/extensions/HelperMacros.h>

#include <fvar.hpp>

int readSouthernOscillationIndexFile(const adstring& soiFile, const int beginYear, const int endYear, dmatrix& soidata);
int readSouthernOscillationIndexFile(const int beginYear, const int endYear, dmatrix& soidata);
void linear_input_scale(dmatrix& t);

class TestTemplate
	:public CppUnit::TestFixture
{
public:
TestTemplate() {;}
TestTemplate(const TestTemplate&);
virtual ~TestTemplate() {;}

public:
CPPUNIT_TEST_SUITE(TestTemplate);
CPPUNIT_TEST(testEmptySoiDatafile);
CPPUNIT_TEST(testFileNotFound);
CPPUNIT_TEST(testReadWithEmptyMatrix);
CPPUNIT_TEST(testReadWithNonEmptyMatrix);
CPPUNIT_TEST(testIncorrectSoiData);
CPPUNIT_TEST(testLinearInputScale);
CPPUNIT_TEST(testBeginYearLargerThanEndYear);
CPPUNIT_TEST(testBeginYearSmallerThanMinimumYear);
CPPUNIT_TEST(testEndYearGreaterThanMaximumYear);
CPPUNIT_TEST(testSubset);
CPPUNIT_TEST_SUITE_END();
public:
void setUp() {
}
void tearDown() {
}
public:
void testEmptySoiDatafile() {
        const int beginYear = 1951;
        const int endYear = 2006;
	dmatrix soidata;
	const int b = readSouthernOscillationIndexFile("", beginYear, endYear, soidata);
	CPPUNIT_ASSERT(soidata.rowmin() > soidata.rowmax());
	CPPUNIT_ASSERT(b == -1);
}
void testFileNotFound() {
        const int beginYear = 1951;
        const int endYear = 2006;
	dmatrix soidata;
	const adstring soiDatafile("ThisShouldNotExistFileName");
	const int b = readSouthernOscillationIndexFile(soiDatafile, beginYear, endYear, soidata);
	CPPUNIT_ASSERT(b == -2);
	CPPUNIT_ASSERT(soidata.rowmin() > soidata.rowmax());
}
void testReadWithEmptyMatrix() {
        const int beginYear = 1951;
        const int endYear = 2006;
	dmatrix soidata;
	const int b = readSouthernOscillationIndexFile(beginYear, endYear, soidata);
	CPPUNIT_ASSERT(b == true);
	CPPUNIT_ASSERT(soidata.rowmin() == 1951);
	CPPUNIT_ASSERT(soidata.rowmax() == 2006);
	CPPUNIT_ASSERT(soidata.colmin() == 1);
	CPPUNIT_ASSERT(soidata.colmax() == 12);
}
void testReadWithNonEmptyMatrix() {
        const int beginYear = 1951;
        const int endYear = 2006;
	dmatrix soidata(1, 100, 1, 100);
	const int b = readSouthernOscillationIndexFile(beginYear, endYear, soidata);
	CPPUNIT_ASSERT(b == true);
	CPPUNIT_ASSERT(soidata.rowmin() == 1951);
	CPPUNIT_ASSERT(soidata.rowmax() == 2006);
	CPPUNIT_ASSERT(soidata.colmin() == 1);
	CPPUNIT_ASSERT(soidata.colmax() == 12);
}
void testIncorrectSoiData() {
        const int beginYear = 1951;
        const int endYear = 2006;
	dmatrix soidata;
	const adstring soiDatafile("../../../data/habitat.inf");
	const int b = readSouthernOscillationIndexFile(soiDatafile, beginYear, endYear, soidata);
	CPPUNIT_ASSERT(b == -3);
}
void testLinearInputScale() {
        const int beginYear = 1951;
        const int endYear = 2006;
	dmatrix soidata;
	readSouthernOscillationIndexFile(beginYear, endYear, soidata);
	linear_input_scale(soidata);
	for (int i = soidata.rowmin(); i <= soidata.rowmax(); i++) {
		for (int j = soidata.colmin(); j <= soidata.colmax(); j++) {
			const bool b = -1 <= soidata(i, j) && soidata(i, j) <= 1;
			CPPUNIT_ASSERT(b);
		}
	}
}
void testBeginYearLargerThanEndYear()
{
	dmatrix soidata;
	const int b = readSouthernOscillationIndexFile(1996, 1995, soidata);
	CPPUNIT_ASSERT(b == -4);
}
void testBeginYearSmallerThanMinimumYear()
{
	dmatrix soidata;
	const int b = readSouthernOscillationIndexFile(1, 1995, soidata);
	CPPUNIT_ASSERT(b == -4);
}
void testEndYearGreaterThanMaximumYear()
{
	dmatrix soidata;
	const int b = readSouthernOscillationIndexFile(1995, 3000, soidata);
	CPPUNIT_ASSERT(b == -4);
}
void testSubset()
{
	dmatrix soidata;
	const int b = readSouthernOscillationIndexFile(1995, 2000, soidata);
	CPPUNIT_ASSERT(b == true);
	CPPUNIT_ASSERT(soidata.rowmin() == 1995);
	CPPUNIT_ASSERT(soidata.rowmax() == 2000);
	CPPUNIT_ASSERT(soidata.colmin() == 1);
	CPPUNIT_ASSERT(soidata.colmax() == 12);
}
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestTemplate);
