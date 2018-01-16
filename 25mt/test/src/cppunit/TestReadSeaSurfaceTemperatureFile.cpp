/**
 * $Id$
 */

#include <stdexcept>
using std::runtime_error;
#include <string>
using std::string;

#include <cppunit/extensions/HelperMacros.h>
#include "par_t_nn.h"

int sstGroupByLongitudeLatitude(const char* file,
                                const int beginLongitude,
                                const int endLongitude,
                                const int beginLatitude,
                                const int endLatitude);

int sstGroupByLongitudeLatitude(const char* file);

int readSeaSurfaceTemperatureFile(const adstring sstfile, const int year, const int month, const int originLongitude, const int originLatitude, dmatrix& sst);

class TestReadSeaSurfaceTemperatureFile
	:public CppUnit::TestFixture
{
public:
TestReadSeaSurfaceTemperatureFile() {;}
TestReadSeaSurfaceTemperatureFile(const TestReadSeaSurfaceTemperatureFile&);
virtual ~TestReadSeaSurfaceTemperatureFile() {;}

public:
CPPUNIT_TEST_SUITE(TestReadSeaSurfaceTemperatureFile);
CPPUNIT_TEST(testNullFile);
CPPUNIT_TEST(testMissingFile);
CPPUNIT_TEST(testOpenedFile);
CPPUNIT_TEST(testBeginLongitudeGreaterThanEndLongitude);
CPPUNIT_TEST(testBeginLatitudeGreaterThanEndLatitude);
CPPUNIT_TEST(testBeginLongitudeLessThanZero);
CPPUNIT_TEST(testBeginLongitudeGreaterThan360);
CPPUNIT_TEST(testEndLongitudeLessThanZero);
CPPUNIT_TEST(testEndLongitudeGreaterThan360);
CPPUNIT_TEST(testBeginLatitudeLessThanMinus90);
CPPUNIT_TEST(testBeginLatitudeGreaterThan90);
CPPUNIT_TEST(testEndLatitudeLessThanMinus90);
CPPUNIT_TEST(testEndLatitudeGreaterThan90);
CPPUNIT_TEST(testReadSeaSurfaceTemperatureFile);
CPPUNIT_TEST(testCreateNeuralNetInstance);
CPPUNIT_TEST(testChoose_nn_input_layer_display);
CPPUNIT_TEST(testSstFlagIsOn);
CPPUNIT_TEST(testReadSeaSurfaceTemperatureFileNotFound);
//CPPUNIT_TEST(testSstUvs_compRead);
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
public://sanity checks
void testNullFile() {
	int v = sstGroupByLongitudeLatitude(0,
					    0,
					    0,
					    0,
					    0);
	CPPUNIT_ASSERT(v == -1);
}
void testMissingFile() {
	int v = sstGroupByLongitudeLatitude("MissingFile",
					    0,
					    0,
					    0,
					    0);
	CPPUNIT_ASSERT(v == -2);
}
void testOpenedFile() {
	int v = sstGroupByLongitudeLatitude("../../../data/sst-test.txt",
					    150,
					    179,
					    0,
					    19);
	CPPUNIT_ASSERT(v == 8400);
}
void testBeginLongitudeGreaterThanEndLongitude() {
	int v = sstGroupByLongitudeLatitude("../../../data/sst-test.txt",
					    20,
					    10,
					    0,
					    0);
	CPPUNIT_ASSERT(v == -3);
}
void testBeginLatitudeGreaterThanEndLatitude() {
	int v = sstGroupByLongitudeLatitude("../../../data/sst-test.txt",
					    10,
					    20,
					    20,
					    10);
	CPPUNIT_ASSERT(v == -3);
}
void testBeginLongitudeLessThanZero() {
	int v = sstGroupByLongitudeLatitude("../../../data/sst-test.txt",
					    -10,
					    20,
					    10,
					    20);
	CPPUNIT_ASSERT(v == -3);
}
void testBeginLongitudeGreaterThan360() {
	int v = sstGroupByLongitudeLatitude("../../../data/sst-test.txt",
					    400,
					    420,
					    10,
					    20);
	CPPUNIT_ASSERT(v == -3);
}
void testEndLongitudeLessThanZero() {
	int v = sstGroupByLongitudeLatitude("../../../data/sst-test.txt",
					    2,
					    -1,
					    10,
					    20);
	CPPUNIT_ASSERT(v == -3);
}
void testEndLongitudeGreaterThan360() {
	int v = sstGroupByLongitudeLatitude("../../../data/sst-test.txt",
					    20,
					    400,
					    10,
					    20);
	CPPUNIT_ASSERT(v == -3);
}
void testBeginLatitudeLessThanMinus90() {
	int v = sstGroupByLongitudeLatitude("../../../data/sst-test.txt",
					    10,
					    20,
					    -100,
					    20);
	CPPUNIT_ASSERT(v == -3);
}
void testBeginLatitudeGreaterThan90() {
	int v = sstGroupByLongitudeLatitude("../../../data/sst-test.txt",
					    10,
					    20,
					    100,
					    200);
	CPPUNIT_ASSERT(v == -3);
}
void testEndLatitudeLessThanMinus90() {
	int v = sstGroupByLongitudeLatitude("../../../data/sst-test.txt",
					    10,
					    20,
					    -90,
					    -91);
	CPPUNIT_ASSERT(v == -3);
}
void testEndLatitudeGreaterThan90() {
	int v = sstGroupByLongitudeLatitude("../../../data/sst-test.txt",
					    10,
					    20,
					    80,
					    91);
	CPPUNIT_ASSERT(v == -3);
}
void testReadSeaSurfaceTemperatureFileNotFound() {
	dmatrix sst;
	const int v = readSeaSurfaceTemperatureFile("FileDoesNotExist", 0, 0, 0, 0, sst);
	CPPUNIT_ASSERT(v == -1);
}
void testReadSeaSurfaceTemperatureFile() {
	//path<-get.sst.from.server(150,180,0,20,c(2003,1,13),c(2004,2,17))
	dmatrix sst(1, 30, 1, 20);

	for (int year = 2003; year < 2004; year++) {
		for (int month = 1; month <= 12; month++) {
			sst.initialize();
			const int totalReadRecords = readSeaSurfaceTemperatureFile("../../../data/sst-test.txt",
										    year,
										    month,
										    150,
										    0,
										    sst);
			CPPUNIT_ASSERT(totalReadRecords == 8400);
			const double value = sum(sst);
			CPPUNIT_ASSERT(value != 0);
		}
	}
}
void testCreateNeuralNetInstance() {
	par_t_nn<d3_array,dmatrix,dvector,double> param(filename);
	CPPUNIT_ASSERT(param.isRegional() == false && param.isNeuralNet() == true);
}
void testChoose_nn_input_layer_display() {
	imatrix input_controls(1, 4, 1, 5);
	input_controls.initialize();
	CPPUNIT_ASSERT(input_controls.rowmin() == 1);
	const int num_nn = input_controls.rowmax();
	CPPUNIT_ASSERT(num_nn == 4);
	const int n1 = input_controls.colmin();
	CPPUNIT_ASSERT(n1 == 1);
	const int n2 = input_controls.colmax();
	CPPUNIT_ASSERT(n2 == 5);
	input_controls(1, 1) = 1;
	input_controls(2, 2) = 2;
	input_controls(3, 3) = 3;
	input_controls(4, 5) = 4;
	//1 0 0 0 0
	//0 2 0 0 0
	//0 0 3 0 0
	//0 0 0 0 4
	ivector display_node(n1,n2); display_node.initialize();
	for (int t = n1; t <= n2; t++) {
		for (int s = 1; s <= num_nn; s++) {
			display_node(t) = input_controls(s,t);
			if (display_node(t) > 0) break;
		}
	}
	CPPUNIT_ASSERT(display_node(1) == 1);
	CPPUNIT_ASSERT(display_node(2) == 2);
	CPPUNIT_ASSERT(display_node(3) == 3);
	CPPUNIT_ASSERT(display_node(4) == 0);
	CPPUNIT_ASSERT(display_node(5) == 4);
	ivector choose_nn_input_layer_display(imatrix &input_controls);
	ivector display_nodes2 = choose_nn_input_layer_display(input_controls);
	CPPUNIT_ASSERT(display_node.indexmin() == display_nodes2.indexmin());
	CPPUNIT_ASSERT(display_node.indexmax() == display_nodes2.indexmax());
	CPPUNIT_ASSERT(display_node(1) == display_nodes2(1));
	CPPUNIT_ASSERT(display_node(2) == display_nodes2(2));
	CPPUNIT_ASSERT(display_node(3) == display_nodes2(3));
	CPPUNIT_ASSERT(display_node(4) == display_nodes2(4));
	CPPUNIT_ASSERT(display_node(5) == display_nodes2(5));
}
void testSstFlagIsOn() {
	par_t_nn<d3_array,dmatrix,dvector,double> param(filename);
	ivector choose_nn_input_layer_display(imatrix &input_controls);
	ivector display_nodes = choose_nn_input_layer_display(param.input_controls);
	CPPUNIT_ASSERT(display_nodes(13) == 2);
}
void testSstUvs_compRead() {
	par_t_nn<d3_array,dmatrix,dvector,double> param(filename);
	const int m = param.get_m();
	const int n = param.get_n();
	dmatrix u(1, m, 1, n); u.initialize();
	dmatrix v(1, m, 1, n); v.initialize();
	dmatrix sigma(1, m, 1, n); sigma.initialize();
	param.sst.initialize();
	double value = sum(param.sst);
	CPPUNIT_ASSERT(value == 0);
	param.uvs_comp(u, v, sigma, param.start_date);
	value = sum(param.sst);
	CPPUNIT_ASSERT(value != 0);
}
private:
char filename[9];
};
CPPUNIT_TEST_SUITE_REGISTRATION(TestReadSeaSurfaceTemperatureFile);
