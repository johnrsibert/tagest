/**
 * $Id$
 *
 * ToCheck:
 * What if file does not exist?
 * What if file is corrupt or does not have correct structure?
 * Can i read then write dym file which is identical to the original?
 * What if area is outside the dym box?
 */

#include <iostream>
using std::cout;
using std::endl;
#include <stdexcept>
using std::runtime_error;

#include <cppunit/extensions/HelperMacros.h>

#include "DymData.h"
#include "longtude.h"
#include "latitude.h"

string getHeader(ifstream& ifs);

int computeTimeId(const int year, const int month);

int readDYMv2File(const char* filename,
                  const int year,
                  const int month,
		  dmatrix& data
);

class TestDymData: public CppUnit::TestFixture
{
public:
TestDymData() {;}
TestDymData(const TestDymData&);
virtual ~TestDymData() {;}

public:
CPPUNIT_TEST_SUITE(TestDymData);
CPPUNIT_TEST(testSizeOfTypes);
CPPUNIT_TEST(testLongitudeValues);
CPPUNIT_TEST(testLatitudeValues);
CPPUNIT_TEST(testDefaultConstructor);
CPPUNIT_TEST(testStaticGetData);
CPPUNIT_TEST(testReadDYMv2FileTimeSeries);
CPPUNIT_TEST(testEmptyFile);
CPPUNIT_TEST(testMissingFile);
CPPUNIT_TEST(testHeader);
CPPUNIT_TEST(testTimeId);
CPPUNIT_TEST_SUITE_END();
public:
void setUp() 
{
}
void tearDown() 
{
}
public:
/**
 * Make sure the sizes are known.
 */
void testSizeOfTypes() 
{
	CPPUNIT_ASSERT(sizeof(char) == 1);
	CPPUNIT_ASSERT(sizeof(int) == 4);
	CPPUNIT_ASSERT(sizeof(float) == 4);
	CPPUNIT_ASSERT(sizeof(double) == 8);
	//CPPUNIT_ASSERT(sizeof(long) == 8);
	//CPPUNIT_ASSERT(sizeof(long) == 4);
}
/**
 * Make sure the direction is eastward.
 */
void testLongitudeValues() 
{
	Longitude lng179W("179W");
	CPPUNIT_ASSERT(lng179W.value() == 181);
	Longitude lng180W("180W");
	CPPUNIT_ASSERT(lng180W.value() == 180);
	Longitude lng181W("181W");
	CPPUNIT_ASSERT(lng181W.value() == 179);

	Longitude lng179E("179E");
	CPPUNIT_ASSERT(lng179E.value() == 179);
	Longitude lng180E("180E");
	CPPUNIT_ASSERT(lng180E.value() == 180);
	Longitude lng181E("181E");
	CPPUNIT_ASSERT(lng181E.value() == 181);
}
/**
 * Make sure the get expected latitude values.
 */
void testLatitudeValues() 
{
	Latitude lat20N("20N");
	CPPUNIT_ASSERT(lat20N.value() == 20);
	Latitude lat00N("00N");
	CPPUNIT_ASSERT(lat00N.value() == 0);
	Latitude lat00S("00S");
	CPPUNIT_ASSERT(lat00S.value() == 0);
	Latitude lat20S("20S");
	CPPUNIT_ASSERT(lat20S.value() == -20);
}
/**
 * Can i do a simple instance?
 */
void testDefaultConstructor() 
{
	DymData dymData;
}
/**
 * Can i get data?
 */
void testStaticGetData() 
{
	const adstring file;
	const int year = 0;
	const int month = 0;
	const double swlongitude = 0;
	const double swlatitude = 0;
	const int dx = 0;
	const int dy = 0;
	dmatrix data;
	DymData::getData(file, 
                         year, month, 
                         swlongitude, dx,
                         swlatitude, dy,
                         data);
}
void testReadDYMv2FileTimeSeries()
{
	//CPPUNIT_ASSERT(readDYMv2File("../../../data/mclPptot0-400m.dym") == 0);
	dmatrix data;
	CPPUNIT_ASSERT(readDYMv2File("../../../data/Young_Habitat.dym", 1988, 1, data) == 0);
	CPPUNIT_ASSERT(sum(data) > 0);
}
void testEmptyFile()
{
	dmatrix data;
	CPPUNIT_ASSERT(readDYMv2File("", 1988, 1, data) == -1);
}
void testMissingFile()
{
	dmatrix data;
	CPPUNIT_ASSERT(readDYMv2File("MissingFile", 1988, 1, data) == -1);
}
void testHeader()
{
	std::ifstream ifs("../../../data/Young_Habitat.dym", ios::binary|ios::in);
	string header = getHeader(ifs);
	ifs.close();
	CPPUNIT_ASSERT(header.compare("DYM2") == 0);
}
void testTimeId()
{
	//1998.04 1998.12 1998.21 1998.29 1998.38 1998.46 1998.54 1998.62 1998.71 1998.79 1998.88 1998.96
	CPPUNIT_ASSERT(computeTimeId(1988, -1) == -1);
	CPPUNIT_ASSERT(computeTimeId(1988, 0) == -1);
	CPPUNIT_ASSERT(computeTimeId(1988, 1) == 198804);
	CPPUNIT_ASSERT(computeTimeId(1988, 2) == 198812);
	CPPUNIT_ASSERT(computeTimeId(1988, 3) == 198821);
	CPPUNIT_ASSERT(computeTimeId(1988, 4) == 198829);
	CPPUNIT_ASSERT(computeTimeId(1988, 5) == 198838);
	CPPUNIT_ASSERT(computeTimeId(1988, 6) == 198846);
	CPPUNIT_ASSERT(computeTimeId(1988, 7) == 198854);
	CPPUNIT_ASSERT(computeTimeId(1988, 8) == 198862);
	CPPUNIT_ASSERT(computeTimeId(1988, 9) == 198871);
	CPPUNIT_ASSERT(computeTimeId(1988, 10) == 198879);
	CPPUNIT_ASSERT(computeTimeId(1988, 11) == 198888);
	CPPUNIT_ASSERT(computeTimeId(1988, 12) == 198896);
	CPPUNIT_ASSERT(computeTimeId(1989, 13) == -1);
}
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestDymData);
