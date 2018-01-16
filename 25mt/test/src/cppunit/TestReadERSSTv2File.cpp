/**
 * $Id$
 */

#include <stdexcept>
using std::runtime_error;

#include <cppunit/extensions/HelperMacros.h>

int readERSSTv2File(const char* filename);
int readERSSTv2File(const char* filename,
                    const int beginYear,
                    const int beginMonth,
                    const int beginLongitude,
                    const int beginLatitude,
                    const int endYear,
                    const int endMonth,
                    const int endLongitude,
                    const int endLatitude);

class TestReadERSSTv2File
	:public CppUnit::TestFixture
{
public:
TestReadERSSTv2File() {;}
TestReadERSSTv2File(const TestReadERSSTv2File&);
virtual ~TestReadERSSTv2File() {;}

public:
CPPUNIT_TEST_SUITE(TestReadERSSTv2File);
/*
CPPUNIT_TEST(testExpectedTotalRecords);
CPPUNIT_TEST(testNullFilename);
CPPUNIT_TEST(testFileNotFound);
//CPPUNIT_TEST(testFileFound19701979);
//CPPUNIT_TEST(testFileFound19801989);
CPPUNIT_TEST(testInvalidFilenameBackPortion);
CPPUNIT_TEST(testInvalidFilenameFrontPortion);
CPPUNIT_TEST(testInvalidFilenameBeginYear);
CPPUNIT_TEST(testInvalidFilenameEndYear);
CPPUNIT_TEST(testBeginYearLessThanFullBeginYear);
CPPUNIT_TEST(testBeginYearGreaterThanFullEndYear);
CPPUNIT_TEST(testEndYearLessThanBeginYear);
//CPPUNIT_TEST(testValidFilename);
CPPUNIT_TEST(testIsMonthBetween1And12);
CPPUNIT_TEST(testSameYearBeginMonthGreaterThanEndMonth);
CPPUNIT_TEST(testGetAllTotalRecords);
CPPUNIT_TEST(testGetAllTotalRecordsInTheSameYear);
CPPUNIT_TEST(testGetTotalRecordsInTheSameYear);
CPPUNIT_TEST(testGetTotalRecordsMultipleYears);
CPPUNIT_TEST(testBeginLongitudeGreaterThanEndLongitude);
CPPUNIT_TEST(testBeginLongitudeOutOfBounds);
CPPUNIT_TEST(testBeginLongitudeBounds);
CPPUNIT_TEST(testEndLongitudeBounds);
CPPUNIT_TEST(testEndLongitudeOutOfBounds);
CPPUNIT_TEST(testGetAllLatitudeBetweenBeginLongitudeAndEndLongitudeSameMonthEven);
CPPUNIT_TEST(testGetAllLatitudeBetweenBeginLongitudeAndEndLongitudeSameMonthOdd);
CPPUNIT_TEST(testGetAllLatitudeBetweenBeginLongitudeAndEndLongitude3MonthsOdd);
CPPUNIT_TEST(testGetAllLatitudeBetweenBeginLongitudeAndEndLongitude1YearOdd);
CPPUNIT_TEST(testBeginLatitudeGreaterThanEndLatitude);
CPPUNIT_TEST(testBeginLatitudeNotInBounds);
CPPUNIT_TEST(testEndLatitudeNotInBounds);
CPPUNIT_TEST(testAllValid3Months);
CPPUNIT_TEST(testAllValid1Years);
CPPUNIT_TEST(testAllValid2Years);
CPPUNIT_TEST(testAllValidNYears);
*/
CPPUNIT_TEST(testAllValidSingleMonthOdd);
CPPUNIT_TEST(testAllValidSingleMonthEven);
//CPPUNIT_TEST(testModulo);
CPPUNIT_TEST_SUITE_END();
public:
static const int pointsZonal = 180;
static const int pointsMeridional = 89;
static const int totalRecords = 10 * 12 * 180;
void setUp() {
}
void tearDown() {
}
private:
int expectedTotalRecords(const int beginYear, const int beginMonth, const int endYear, const int endMonth)
{
	int totalRecords = 0;
	int month = beginMonth;
        for (int year = beginYear; year <= endYear; year++) {
                for (; month <= (year == endYear ? endMonth : 12); month++) {
			totalRecords += pointsZonal;
		}
		month = 1;
	}
	return totalRecords;
}
int expectedTotalPoints(const int beginYear,
			const int beginMonth,
			const int beginLongitude,
			const int beginLatitude,
			const int endYear,
			const int endMonth,
			const int endLongitude,
			const int endLatitude)
{
	return expectedTotalRecords(beginYear, beginMonth, endYear, endMonth) * (endLongitude - beginLongitude) * (endLatitude - beginLatitude);
}
public:
void testExpectedTotalRecords() {
	CPPUNIT_ASSERT(expectedTotalRecords(1980, 1, 1989, 12) == totalRecords);
	CPPUNIT_ASSERT(expectedTotalRecords(1980, 1, 1984, 12) == (5 * 12 * 180));
	CPPUNIT_ASSERT(expectedTotalRecords(1980, 1, 1980, 1) == pointsZonal);
	CPPUNIT_ASSERT(expectedTotalRecords(1980, 5, 1980, 7) == (pointsZonal * 3));
	CPPUNIT_ASSERT(expectedTotalRecords(1980, 1, 1980, 2) == (pointsZonal * 2));
	CPPUNIT_ASSERT(expectedTotalRecords(1980, 11, 1980, 12) == (pointsZonal * 2));
	CPPUNIT_ASSERT(expectedTotalRecords(1983, 5, 1985, 8) == 5040);
	CPPUNIT_ASSERT(expectedTotalRecords(1983, 5, 1986, 8) == 7200);
}
void testNullFilename() {
	const int value = readERSSTv2File(0);
	CPPUNIT_ASSERT(value == -1);
}
void testFileNotFound() {
	const int value = readERSSTv2File("ThisFileDoesNotExist");
	CPPUNIT_ASSERT(value == -1);
}
void testFileFound19701979() {
	const int value = readERSSTv2File("../../data/ersst.v2.1970.1979.asc");
	CPPUNIT_ASSERT(value == 0);
}
void testFileFound19801989() {
	const int value = readERSSTv2File("../../data/ersst.v2.1980.1989.asc");
	CPPUNIT_ASSERT(value == 0);
}
void testInvalidFilenameBackPortion() {
	const int value = readERSSTv2File("../../data/u_0176-1290.txt");
	CPPUNIT_ASSERT(value == -2);
}
void testInvalidFilenameFrontPortion() {
	const int value = readERSSTv2File("../../data/ersst.v2.serrv.1980.1989.asc");
	CPPUNIT_ASSERT(value == -2);
}
void testInvalidFilenameBeginYear() {
	const int value = readERSSTv2File("../../data/ersst.v2.1x80.1989.asc");
	CPPUNIT_ASSERT(value == -3);
}
void testInvalidFilenameEndYear() {
	const int value = readERSSTv2File("../../data/ersst.v2.1980.19x9.asc");
	CPPUNIT_ASSERT(value == -3);
}
void testValidFilename() {
	const int value = readERSSTv2File("../../data/ersst.v2.1980.1989.asc");
	CPPUNIT_ASSERT(value > 0);
}
void testBeginYearLessThanFullBeginYear() {
        const int beginMonth= 0;
        const int beginLongitude = 0;
        const int beginLatitude = 0;
        const int endYear = 0;
        const int endMonth= 0;
        const int endLongitude = 0;
        const int endLatitude = 0;
	const int value = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
					  1979,
					  beginMonth,
					  beginLongitude,
					  beginLatitude,
					  endYear,
					  endMonth,
					  endLongitude,
					  endLatitude);
	CPPUNIT_ASSERT(value == -4);
}
void testBeginYearGreaterThanFullEndYear() {
        const int beginMonth= 0;
        const int beginLongitude = 0;
        const int beginLatitude = 0;
        const int endYear = 0;
        const int endMonth= 0;
        const int endLongitude = 0;
        const int endLatitude = 0;
	const int value = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
					  1990,
					  beginMonth,
					  beginLongitude,
					  beginLatitude,
					  endYear,
					  endMonth,
					  endLongitude,
					  endLatitude);
	CPPUNIT_ASSERT(value == -4);
}
void testEndYearLessThanBeginYear() {
        const int beginMonth= 0;
        const int beginLongitude = 0;
        const int beginLatitude = 0;
        const int endMonth= 0;
        const int endLongitude = 0;
        const int endLatitude = 0;
	const int value = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
					  1981,
					  beginMonth,
					  beginLongitude,
					  beginLatitude,
					  1980,
					  endMonth,
					  endLongitude,
					  endLatitude);
	CPPUNIT_ASSERT(value == -4);
}
void testIsMonthBetween1And12() {
        const int beginLongitude = 0;
        const int beginLatitude = 0;
        const int endMonth= 0;
        const int endLongitude = 359;
        const int endLatitude = 0;

	for (int month = 0; month > -12 ; month--) {
		const int value = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
						  1981,
						  month,
						  beginLongitude,
						  beginLatitude,
						  1981,
						  1,
						  endLongitude,
						  endLatitude);
		CPPUNIT_ASSERT(value == -4);
		const int value2 = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
						  1981,
						  1,
						  beginLongitude,
						  beginLatitude,
						  1981,
						  month,
						  endLongitude,
						  endLatitude);
		CPPUNIT_ASSERT(value2 == -4);
	}
	for (int month = 1; month <= 12; month++) {
		const int value = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
						  1981,
						  month,
						  beginLongitude,
						  beginLatitude,
						  1981,
						  12,
						  endLongitude,
						  endLatitude);
		CPPUNIT_ASSERT(value == expectedTotalRecords(1981, month, 1981, 12));
		const int value2 = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
						  1981,
						  1,
						  beginLongitude,
						  beginLatitude,
						  1981,
						  month,
						  endLongitude,
						  endLatitude);
		CPPUNIT_ASSERT(value2 == expectedTotalRecords(1981, 1, 1981, month));
	}
	for (int month = 13; month <= 24 ; month++) {
		const int value = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
						  1981,
						  month,
						  beginLongitude,
						  beginLatitude,
						  1981,
						  endMonth,
						  endLongitude,
						  endLatitude);
		CPPUNIT_ASSERT(value == -4);
		const int value2 = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
						  1981,
						  1,
						  beginLongitude,
						  beginLatitude,
						  1981,
						  month,
						  endLongitude,
						  endLatitude);
		CPPUNIT_ASSERT(value2 == -4);
	}
}
void testSameYearBeginMonthGreaterThanEndMonth() {
        const int beginLongitude = 0;
        const int beginLatitude = 0;
        const int endLongitude = 0;
        const int endLatitude = 0;
	const int value = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
					  1980,
					  12,
					  beginLongitude,
					  beginLatitude,
					  1980,
					  1,
					  endLongitude,
					  endLatitude);
	CPPUNIT_ASSERT(value == -4);
}
void testGetAllTotalRecords() {
        const int beginLongitude = 0;
        const int beginLatitude = 0;
        const int endLongitude = 359;
        const int endLatitude = 0;
	const int value = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
					  1980,
					  1,
					  beginLongitude,
					  beginLatitude,
					  1989,
					  12,
					  endLongitude,
					  endLatitude);
	CPPUNIT_ASSERT(value == totalRecords);
}
void testGetAllTotalRecordsInTheSameYear() {
        const int beginLongitude = 0;
        const int beginLatitude = 0;
        const int endLongitude = 359;
        const int endLatitude = 0;
	const int value = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
					  1985,
					  1,
					  beginLongitude,
					  beginLatitude,
					  1985,
					  12,
					  endLongitude,
					  endLatitude);
	CPPUNIT_ASSERT(value == expectedTotalRecords(1985, 1, 1985, 12));
}
void testGetTotalRecordsInTheSameYear() {
        const int beginLongitude = 0;
        const int beginLatitude = 0;
        const int endLongitude = 359;
        const int endLatitude = 0;
	const int value = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
					  1985,
					  5,
					  beginLongitude,
					  beginLatitude,
					  1985,
					  8,
					  endLongitude,
					  endLatitude);
	CPPUNIT_ASSERT(value == expectedTotalRecords(1985, 5, 1985, 8));
}
void testGetTotalRecordsMultipleYears()
{
        const int beginLongitude = 0;
        const int beginLatitude = 0;
        const int endLongitude = 359;
        const int endLatitude = 0;
	const int value = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
					  1983,
					  5,
					  beginLongitude,
					  beginLatitude,
					  1985,
					  8,
					  endLongitude,
					  endLatitude);
	CPPUNIT_ASSERT(value == expectedTotalRecords(1983, 5, 1985, 8));
}
void testBeginLongitudeGreaterThanEndLongitude()
{
        const int beginLatitude = 0;
        const int endLatitude = 0;
	const int value = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
					  1983,
					  5,
					  100,
					  beginLatitude,
					  1985,
					  8,
					  10,
					  endLatitude);
	CPPUNIT_ASSERT(value == -5);
}
void testBeginLongitudeOutOfBounds()
{
        const int beginLatitude = 0;
        const int endLatitude = 0;
	const int value = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
					  1983,
					  5,
					  -1,
					  beginLatitude,
					  1985,
					  8,
					  10,
					  endLatitude);
	CPPUNIT_ASSERT(value == -5);
	const int value2 = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
					  1983,
					  5,
					  360,
					  beginLatitude,
					  1985,
					  8,
					  10,
					  endLatitude);
	CPPUNIT_ASSERT(value2 == -5);
}
void testBeginLongitudeBounds()
{
        const int beginLatitude = 0;
        const int endLongitude = 359;
        const int endLatitude = 0;
	const int value = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
					  1983,
					  5,
					  0,
					  beginLatitude,
					  1985,
					  8,
					  endLongitude,
					  endLatitude);
	CPPUNIT_ASSERT(value == expectedTotalRecords(1983, 5, 1985, 8));
	const int value2 = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
					  1983,
					  5,
					  0,
					  beginLatitude,
					  1985,
					  8,
					  359,
					  endLatitude);
	CPPUNIT_ASSERT(value2 == expectedTotalRecords(1983, 5, 1985, 8));
}
void testEndLongitudeBounds()
{
        const int beginLongitude = 0;
        const int beginLatitude = 0;
        const int endLatitude = 0;
	const int value = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
					  1983,
					  5,
					  beginLongitude,
					  beginLatitude,
					  1985,
					  8,
					  359,
					  endLatitude);
	CPPUNIT_ASSERT(value == expectedTotalRecords(1983, 5, 1985, 8));
	const int value2 = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
					  1983,
					  5,
					  beginLongitude,
					  beginLatitude,
					  1985,
					  8,
					  359,
					  endLatitude);
	CPPUNIT_ASSERT(value2 == expectedTotalRecords(1983, 5, 1985, 8));
}
void testEndLongitudeOutOfBounds()
{
        const int beginLatitude = 0;
        const int endLatitude = 0;
	const int value = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
					  1983,
					  5,
					  10,
					  beginLatitude,
					  1985,
					  8,
					  -1,
					  endLatitude);
	CPPUNIT_ASSERT(value == -5);
	const int value2 = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
					  1983,
					  5,
					  10,
					  beginLatitude,
					  1985,
					  8,
					  360,
					  endLatitude);
	CPPUNIT_ASSERT(value2 == -5);
}
void testGetAllLatitudeBetweenBeginLongitudeAndEndLongitudeSameMonthEven()
{
	const int value = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
					  1985,
					  5,
					  10,
					  0,
					  1985,
					  5,
					  20,
					  0);
	CPPUNIT_ASSERT(value == 6);
}
void testGetAllLatitudeBetweenBeginLongitudeAndEndLongitudeSameMonthOdd()
{
	const int value = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
					  1985,
					  5,
					  11,
					  0,
					  1985,
					  5,
					  21,
					  0);
	CPPUNIT_ASSERT(value == 6);
}
void testGetAllLatitudeBetweenBeginLongitudeAndEndLongitude3MonthsOdd()
{
	const int value = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
					  1985,
					  5,
					  11,
					  0,
					  1985,
					  9,
					  21,
					  0);
	CPPUNIT_ASSERT(value == (6 * 5));
}
void testGetAllLatitudeBetweenBeginLongitudeAndEndLongitude1YearOdd()
{
	const int value = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
					  1984,
					  5,
					  11,
					  0,
					  1985,
					  5,
					  21,
					  0);
	CPPUNIT_ASSERT(value == (6 * 13));
}
void testBeginLatitudeGreaterThanEndLatitude()
{
	const int value = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
					  1984,
					  5,
					  11,
					  20,
					  1985,
					  5,
					  21,
					  10);
	CPPUNIT_ASSERT(value == -6);
}
void testBeginLatitudeNotInBounds()
{
	const int value = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
					  1984,
					  5,
					  11,
					  -91,
					  1985,
					  5,
					  21,
					  10);
	CPPUNIT_ASSERT(value == -6);
	const int value2 = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
					  1984,
					  5,
					  11,
					  91,
					  1985,
					  5,
					  21,
					  10);
	CPPUNIT_ASSERT(value2 == -6);
}
void testEndLatitudeNotInBounds()
{
	const int value = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
					  1984,
					  5,
					  11,
					  -90,
					  1985,
					  5,
					  21,
					  -91);
	CPPUNIT_ASSERT(value == -6);
	const int value2 = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
					  1984,
					  5,
					  11,
					  90,
					  1985,
					  5,
					  21,
					  91);
	CPPUNIT_ASSERT(value2 == -6);
}
void testAllValid3Months()
{
	const int value = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
					  1985,
					  5,
					  10,
					  10,
					  1985,
					  7,
					  16,
					  16);
	CPPUNIT_ASSERT(value == (16 * 3));
}
void testAllValid1Years()
{
	const int value = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
					  1980,
					  5,
					  10,
					  10,
					  1981,
					  4,
					  16,
					  16);
	CPPUNIT_ASSERT(value == (16 * 12));
}
void testAllValid2Years()
{
	const int value = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
					  1980,
					  5,
					  10,
					  10,
					  1982,
					  4,
					  16,
					  16);
	CPPUNIT_ASSERT(value == (16 * 24));
}
void testAllValidNYears()
{
	for (int year = 1981; year < 1990; year++) { 
		const int value = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
						  1980,
						  5,
						  10,
						  10,
						  year,
						  4,
						  16,
						  16);
		const int expectedTotalPoints = (year - 1980) * 12  * 16;
		CPPUNIT_ASSERT(value == expectedTotalPoints);
	}
}
void testAllValidSingleMonthEven()
{
	const int value = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
					  1985,
					  5,
					  10,
					  10,
					  1985,
					  5,
					  16,
					  16);
	CPPUNIT_ASSERT(value == 16);
}
void testAllValidSingleMonthOdd()
{
	const int value = readERSSTv2File("../../data/ersst.v2.1980.1989.asc",
					  1985,
					  5,
					  11,
					  11,
					  1985,
					  5,
					  17,
					  17);
	CPPUNIT_ASSERT(value == 16);
}
void testModulo()
{
	CPPUNIT_ASSERT((11 % 2) == 1);
	CPPUNIT_ASSERT((10 % 2) == 0);
	CPPUNIT_ASSERT((-11 % 2) == -1);
	CPPUNIT_ASSERT((-10 % 2) == 0);

	int latitude = -88;
	for (int j = 1; j <= 89; j++) {
		std::cout << j << ' ' << latitude << std::endl;
		latitude += 2;
	} 
}
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestReadERSSTv2File);
