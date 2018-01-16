/**
 * $Id$
 */

#include <stdexcept>
using std::runtime_error;

#include <cppunit/extensions/HelperMacros.h>
#include <par_t_reg.h>
#include <par_t_nn.h>
#include <admodel.h>
#include <tagest.htp>

int tagest(int argc,char * argv[]);

class TestTagestNeuralNet: public CppUnit::TestFixture
{
public:
TestTagestNeuralNet() {;}
TestTagestNeuralNet(const TestTagestNeuralNet&);
virtual ~TestTagestNeuralNet() {;}

public:
CPPUNIT_TEST_SUITE(TestTagestNeuralNet);
CPPUNIT_TEST(testVariable);
CPPUNIT_TEST(testConstructorDestructor);
CPPUNIT_TEST(testVariableConstructorDestructor);
CPPUNIT_TEST(testCreateInstance);
CPPUNIT_TEST(testVariableCreateInstance);
CPPUNIT_TEST(testVariableResetDiffHwts);
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
	par_t<d3_array,dmatrix,dvector,double>::deleteInstance();

	gradient_structure gradientStructure;
	par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::deleteInstance();
}
public:
void testVariable() {
	gradient_structure gradientStructure;
	dvariable d = 0;
}
void testConstructorDestructor() {
	par_t_nn<d3_array,dmatrix,dvector,double> param(filename);
	CPPUNIT_ASSERT(param.isRegional() == false && param.isNeuralNet() == true);
}
void testVariableConstructorDestructor() {
	gradient_structure gradientStructure;
	par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	CPPUNIT_ASSERT(param.isRegional() == false && param.isNeuralNet() == true);
}
void testCreateInstance() {
	typedef par_t<d3_array,dmatrix,dvector,double> NeuralNet;

	NeuralNet *param = NeuralNet::createInstance(filename);
	CPPUNIT_ASSERT(NeuralNet::instance != 0 && param->isRegional() == false && param->isNeuralNet() == true);

	NeuralNet::deleteInstance();
	CPPUNIT_ASSERT(NeuralNet::instance == 0);
}
void testVariableCreateInstance() {
	gradient_structure gradientStructure;
	typedef par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable> VariableNeuralNet;

	VariableNeuralNet *param = VariableNeuralNet::createInstance(filename);
	CPPUNIT_ASSERT(VariableNeuralNet::instance != 0 && param->isRegional() == false && param->isNeuralNet() == true);

	VariableNeuralNet::deleteInstance();
	CPPUNIT_ASSERT(VariableNeuralNet::instance == 0);
}
void testVariableResetDiffHwts() {
	gradient_structure gradientStructure;
	typedef par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable> VariableNeuralNet;

	VariableNeuralNet param(filename);
	for (int n = 1; n <= param.num_nn; n++) {
		if (param.nn(n).nn_ipar(5)) {
			dvar3_array weights = param.nn(n).get_weights();
			dvar_matrix diff_hwts = param.nn(n).get_diff_hwts();
			weights.initialize();
			diff_hwts.initialize();
		}
	}
	for (int n = 1; n <= param.num_nn; n++) {
		if (param.nn(n).nn_ipar(5)) {
			dvar3_array weights = param.nn(n).get_weights();
			dvar_matrix diff_hwts = param.nn(n).get_diff_hwts();
			const ivector vnrow = param.nn(n).get_nrow();
			const ivector vncol = param.nn(n).get_ncol();
			for (int i = 1; i < param.num_levels(n); i++) {
				if (i < param.num_levels(n) - 1) {
					for (int j = 1; j <= vnrow(i); j++) {
						for (int k = 1;  k <= vncol(i); k++) {
							CPPUNIT_ASSERT(weights(i,j,k) == 0);
						}
					}
				} else {
					for (int j = 1; j <= vnrow(i); j++) {
						for (int k = 1; k <= vncol(i); k++) {
							CPPUNIT_ASSERT(diff_hwts(j,k) == 0);
							diff_hwts(j,k) = 1;
						}
					}
				}
			}
		}
	}
	param.reset_diff_hwts(param.nn);
	for (int n = 1; n <= param.num_nn; n++) {
		if (param.nn(n).nn_ipar(5)) {
			dvar3_array weights = param.nn(n).get_weights();
			dvar_matrix diff_hwts = param.nn(n).get_diff_hwts();
			const ivector vnrow = param.nn(n).get_nrow();
			const ivector vncol = param.nn(n).get_ncol();
			for (int i = 1; i < param.num_levels(n); i++) {
				if (i < param.num_levels(n) - 1) {
					for (int j = 1; j <= vnrow(i); j++) {
						for (int k = 1;  k <= vncol(i); k++) {
							//CPPUNIT_ASSERT(weights(i,j,k) > 0);
						}
					}
				} else {
					for (int j = 1; j <= vnrow(i); j++) {
						for (int k = 1; k <= vncol(i); k++) {
							CPPUNIT_ASSERT(diff_hwts(j,k) > 0);
						}
					}
				}
			}
		}
	}
}
private:
char filename[9];
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestTagestNeuralNet);
