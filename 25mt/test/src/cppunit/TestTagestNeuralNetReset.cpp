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

class TestTagestNeuralNetReset: public CppUnit::TestFixture
{
public:
TestTagestNeuralNetReset() {;}
TestTagestNeuralNetReset(const TestTagestNeuralNetReset&);
virtual ~TestTagestNeuralNetReset() {;}

public:
CPPUNIT_TEST_SUITE(TestTagestNeuralNetReset);
CPPUNIT_TEST(testVariableResetOnQ);
CPPUNIT_TEST(testVariableResetOffQ);
CPPUNIT_TEST(testVariableResetOnMort);
CPPUNIT_TEST(testVariableResetOffMort);
CPPUNIT_TEST(testVariableResetOnSpecialMort);
CPPUNIT_TEST(testVariableResetOffSpecialMort);
CPPUNIT_TEST(testVariableResetOnReportRates);
CPPUNIT_TEST(testVariableResetOffReportRates);
CPPUNIT_TEST(testVariableResetOnSlopeU);
CPPUNIT_TEST(testVariableResetOffSlopeU);
CPPUNIT_TEST(testVariableResetOnOffsetU);
CPPUNIT_TEST(testVariableResetOffOffsetU);
CPPUNIT_TEST(testVariableResetOnSlopeV);
CPPUNIT_TEST(testVariableResetOffSlopeV);
CPPUNIT_TEST(testVariableResetOnOffsetV);
CPPUNIT_TEST(testVariableResetOffOffsetV);
CPPUNIT_TEST(testVariableResetOnSlopeSigma);
CPPUNIT_TEST(testVariableResetOffSlopeSigma);
CPPUNIT_TEST(testVariableResetOnOffsetSigma);
CPPUNIT_TEST(testVariableResetOffOffsetSigma);
CPPUNIT_TEST(testVariableResetOnUniformSigma);
CPPUNIT_TEST(testVariableResetOffUniformSigma);
CPPUNIT_TEST(testVariableResetWeights);
CPPUNIT_TEST(testVariableResetDiffHwts);
CPPUNIT_TEST(testVariableResetBias);
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
void testVariableResetOnQ() {
	gradient_structure gradientStructure;
	par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	for (int n = 1; n <= param.num_nn; n++) {
		param.nn(n).nn_ipar(5) = 0;//do not set weights and diff_hwts
		param.nn(n).nn_ipar(6) = 0;//do not set bias
	}
	int value = __LINE__;
	param.q.initialize();
	dvector q(param.q.indexmin(), param.q.indexmax());
	for (int i = param.q.indexmin(); i <= param.q.indexmax(); i++) {
		CPPUNIT_ASSERT(param.q(i) == 0);
		q(i) = value;
		value += __LINE__;
	}
	dvar_vector weights, diff_hwts, bias;
	param.m_ipar(25) = 2;
	param.reset(q,
		    param.mort,
		    param.special_mort,
		    param.report_rates,
		    param.u_slope,
		    param.u_offset,
		    param.v_slope,
		    param.v_offset,
		    param.sig_slope,
		    param.sig_offset,
		    param.uniform_sigma,
		    weights,
		    diff_hwts,
		    bias);
	for (int i = param.q.indexmin(); i <= param.q.indexmax(); i++) {
		CPPUNIT_ASSERT(param.q(i) == q(i));
	}
}
void testVariableResetOffQ() {
	gradient_structure gradientStructure;
	par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	for (int n = 1; n <= param.num_nn; n++) {
		param.nn(n).nn_ipar(5) = 0;//do not set weights and diff_hwts
		param.nn(n).nn_ipar(6) = 0;//do not set bias
	}
	int value = __LINE__;
	param.q.initialize();
	dvector q(param.q.indexmin(), param.q.indexmax());
	for (int i = param.q.indexmin(); i <= param.q.indexmax(); i++) {
		CPPUNIT_ASSERT(param.q(i) == 0);
		q(i) = value;
		value += __LINE__;
	}
	dvar_vector weights, diff_hwts, bias;
	param.m_ipar(25) = 0;
	param.reset(q,
		    param.mort,
		    param.special_mort,
		    param.report_rates,
		    param.u_slope,
		    param.u_offset,
		    param.v_slope,
		    param.v_offset,
		    param.sig_slope,
		    param.sig_offset,
		    param.uniform_sigma,
		    weights,
		    diff_hwts,
		    bias);
	for (int i = param.q.indexmin(); i <= param.q.indexmax(); i++) {
		CPPUNIT_ASSERT(param.q(i) == 0);
	}
}
void testVariableResetOnMort() {
	gradient_structure gradientStructure;
	par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	for (int n = 1; n <= param.num_nn; n++) {
		param.nn(n).nn_ipar(5) = 0;//do not set weights and diff_hwts
		param.nn(n).nn_ipar(6) = 0;//do not set bias
	}
	param.mort = 0;
	dvariable mort = __LINE__;
	dvar_vector weights, diff_hwts, bias;
	param.m_ipar(22) = 1;
	param.reset(param.q,
		    mort,
		    param.special_mort,
		    param.report_rates,
		    param.u_slope,
		    param.u_offset,
		    param.v_slope,
		    param.v_offset,
		    param.sig_slope,
		    param.sig_offset,
		    param.uniform_sigma,
		    weights,
		    diff_hwts,
		    bias);
	CPPUNIT_ASSERT(param.mort == mort);
}
void testVariableResetOffMort() {
	gradient_structure gradientStructure;
	par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	for (int n = 1; n <= param.num_nn; n++) {
		param.nn(n).nn_ipar(5) = 0;//do not set weights and diff_hwts
		param.nn(n).nn_ipar(6) = 0;//do not set bias
	}
	param.mort = 0;
	dvariable mort = __LINE__;
	dvar_vector weights, diff_hwts, bias;
	param.m_ipar(22) = 0;
	param.reset(param.q,
		    mort,
		    param.special_mort,
		    param.report_rates,
		    param.u_slope,
		    param.u_offset,
		    param.v_slope,
		    param.v_offset,
		    param.sig_slope,
		    param.sig_offset,
		    param.uniform_sigma,
		    weights,
		    diff_hwts,
		    bias);
	CPPUNIT_ASSERT(param.mort == 0);
}
void testVariableResetOnSpecialMort() {
	gradient_structure gradientStructure;
	par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	for (int n = 1; n <= param.num_nn; n++) {
		param.nn(n).nn_ipar(5) = 0;//do not set weights and diff_hwts
		param.nn(n).nn_ipar(6) = 0;//do not set bias
	}
	param.special_mort = 0;
	dvariable special_mort = __LINE__;
	dvar_vector weights, diff_hwts, bias;
	param.m_ipar(22) = 1;
	param.m_ipar(19) = 1;
	param.reset(param.q,
		    param.mort,
		    special_mort,
		    param.report_rates,
		    param.u_slope,
		    param.u_offset,
		    param.v_slope,
		    param.v_offset,
		    param.sig_slope,
		    param.sig_offset,
		    param.uniform_sigma,
		    weights,
		    diff_hwts,
		    bias);
	CPPUNIT_ASSERT(param.special_mort == special_mort);
}
void testVariableResetOffSpecialMort() {
	gradient_structure gradientStructure;
	par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	for (int n = 1; n <= param.num_nn; n++) {
		param.nn(n).nn_ipar(5) = 0;//do not set weights and diff_hwts
		param.nn(n).nn_ipar(6) = 0;//do not set bias
	}
	param.special_mort = 0;
	dvariable special_mort = __LINE__;
	dvar_vector weights, diff_hwts, bias;
	param.m_ipar(22) = 0;
	param.reset(param.q,
		    param.mort,
		    special_mort,
		    param.report_rates,
		    param.u_slope,
		    param.u_offset,
		    param.v_slope,
		    param.v_offset,
		    param.sig_slope,
		    param.sig_offset,
		    param.uniform_sigma,
		    weights,
		    diff_hwts,
		    bias);
	CPPUNIT_ASSERT(param.special_mort == 0);
}
void testVariableResetOnReportRates() {
	gradient_structure gradientStructure;
	par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	for (int n = 1; n <= param.num_nn; n++) {
		param.nn(n).nn_ipar(5) = 0;//do not set weights and diff_hwts
		param.nn(n).nn_ipar(6) = 0;//do not set bias
	}
	int value = __LINE__;
	param.report_rates.initialize();
	dvector report_rates(param.report_rates.indexmin(), param.report_rates.indexmax());
	for (int i = param.report_rates.indexmin(); i <= param.report_rates.indexmax(); i++) {
		CPPUNIT_ASSERT(param.report_rates(i) == 0);
		report_rates(i) = value;
		value += __LINE__;
	}
	dvar_vector weights, diff_hwts, bias;
	param.m_ipar(28) = 1;
	param.reset(param.q,
		    param.mort,
		    param.special_mort,
		    report_rates,
		    param.u_slope,
		    param.u_offset,
		    param.v_slope,
		    param.v_offset,
		    param.sig_slope,
		    param.sig_offset,
		    param.uniform_sigma,
		    weights,
		    diff_hwts,
		    bias);
	for (int i = param.report_rates.indexmin(); i <= param.report_rates.indexmax(); i++) {
		CPPUNIT_ASSERT(param.report_rates(i) == report_rates(i));
	}
}
void testVariableResetOffReportRates() {
	gradient_structure gradientStructure;
	par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	for (int n = 1; n <= param.num_nn; n++) {
		param.nn(n).nn_ipar(5) = 0;//do not set weights and diff_hwts
		param.nn(n).nn_ipar(6) = 0;//do not set bias
	}
	int value = __LINE__;
	param.report_rates.initialize();
	dvector report_rates(param.report_rates.indexmin(), param.report_rates.indexmax());
	for (int i = param.report_rates.indexmin(); i <= param.report_rates.indexmax(); i++) {
		CPPUNIT_ASSERT(param.report_rates(i) == 0);
		report_rates(i) = value;
		value += __LINE__;
	}
	dvar_vector weights, diff_hwts, bias;
	param.m_ipar(28) = 0;
	param.reset(param.q,
		    param.mort,
		    param.special_mort,
		    report_rates,
		    param.u_slope,
		    param.u_offset,
		    param.v_slope,
		    param.v_offset,
		    param.sig_slope,
		    param.sig_offset,
		    param.uniform_sigma,
		    weights,
		    diff_hwts,
		    bias);
	for (int i = param.report_rates.indexmin(); i <= param.report_rates.indexmax(); i++) {
		CPPUNIT_ASSERT(param.report_rates(i) == 0);
	}
}
void testVariableResetOnSlopeU() {
	gradient_structure gradientStructure;
	par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	for (int n = 1; n <= param.num_nn; n++) {
		param.nn(n).nn_ipar(5) = 0;//do not set weights and diff_hwts
		param.nn(n).nn_ipar(6) = 0;//do not set bias
	}
	param.u_slope = 0;
	dvariable u_slope= __LINE__;
	dvar_vector weights, diff_hwts, bias;
	param.m_ipar(71) = 1;
	param.reset(param.q,
		    param.mort,
		    param.special_mort,
		    param.report_rates,
		    u_slope,
		    param.u_offset,
		    param.v_slope,
		    param.v_offset,
		    param.sig_slope,
		    param.sig_offset,
		    param.uniform_sigma,
		    weights,
		    diff_hwts,
		    bias);
	CPPUNIT_ASSERT(param.u_slope == u_slope);
}
void testVariableResetOffSlopeU() {
	gradient_structure gradientStructure;
	par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	for (int n = 1; n <= param.num_nn; n++) {
		param.nn(n).nn_ipar(5) = 0;//do not set weights and diff_hwts
		param.nn(n).nn_ipar(6) = 0;//do not set bias
	}
	param.u_slope = 0;
	dvariable u_slope= __LINE__;
	dvar_vector weights, diff_hwts, bias;
	param.m_ipar(71) = 0;
	param.reset(param.q,
		    param.mort,
		    param.special_mort,
		    param.report_rates,
		    u_slope,
		    param.u_offset,
		    param.v_slope,
		    param.v_offset,
		    param.sig_slope,
		    param.sig_offset,
		    param.uniform_sigma,
		    weights,
		    diff_hwts,
		    bias);
	CPPUNIT_ASSERT(param.u_slope == 0);
}
void testVariableResetOnOffsetU() {
	gradient_structure gradientStructure;
	par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	for (int n = 1; n <= param.num_nn; n++) {
		param.nn(n).nn_ipar(5) = 0;//do not set weights and diff_hwts
		param.nn(n).nn_ipar(6) = 0;//do not set bias
	}
	param.u_offset = 0;
	dvariable u_offset = __LINE__;
	dvar_vector weights, diff_hwts, bias;
	param.m_ipar(72) = 1;
	param.reset(param.q,
		    param.mort,
		    param.special_mort,
		    param.report_rates,
		    param.u_slope,
		    u_offset,
		    param.v_slope,
		    param.v_offset,
		    param.sig_slope,
		    param.sig_offset,
		    param.uniform_sigma,
		    weights,
		    diff_hwts,
		    bias);
	CPPUNIT_ASSERT(param.u_offset == u_offset);
}
void testVariableResetOffOffsetU() {
	gradient_structure gradientStructure;
	par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	for (int n = 1; n <= param.num_nn; n++) {
		param.nn(n).nn_ipar(5) = 0;//do not set weights and diff_hwts
		param.nn(n).nn_ipar(6) = 0;//do not set bias
	}
	param.u_offset = 0;
	dvariable u_offset = __LINE__;
	dvar_vector weights, diff_hwts, bias;
	param.m_ipar(72) = 0;
	param.reset(param.q,
		    param.mort,
		    param.special_mort,
		    param.report_rates,
		    param.u_slope,
		    u_offset,
		    param.v_slope,
		    param.v_offset,
		    param.sig_slope,
		    param.sig_offset,
		    param.uniform_sigma,
		    weights,
		    diff_hwts,
		    bias);
	CPPUNIT_ASSERT(param.u_offset == 0);
}
void testVariableResetOnSlopeV() {
	gradient_structure gradientStructure;
	par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	for (int n = 1; n <= param.num_nn; n++) {
		param.nn(n).nn_ipar(5) = 0;//do not set weights and diff_hwts
		param.nn(n).nn_ipar(6) = 0;//do not set bias
	}
	param.v_slope = 0;
	dvariable v_slope= __LINE__;
	dvar_vector weights, diff_hwts, bias;
	param.m_ipar(73) = 1;
	param.reset(param.q,
		    param.mort,
		    param.special_mort,
		    param.report_rates,
		    param.u_slope,
		    param.u_offset,
		    v_slope,
		    param.v_offset,
		    param.sig_slope,
		    param.sig_offset,
		    param.uniform_sigma,
		    weights,
		    diff_hwts,
		    bias);
	CPPUNIT_ASSERT(param.v_slope == v_slope);
}
void testVariableResetOffSlopeV() {
	gradient_structure gradientStructure;
	par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	for (int n = 1; n <= param.num_nn; n++) {
		param.nn(n).nn_ipar(5) = 0;//do not set weights and diff_hwts
		param.nn(n).nn_ipar(6) = 0;//do not set bias
	}
	param.v_slope = 0;
	dvariable v_slope= __LINE__;
	dvar_vector weights, diff_hwts, bias;
	param.m_ipar(73) = 0;
	param.reset(param.q,
		    param.mort,
		    param.special_mort,
		    param.report_rates,
		    param.u_slope,
		    param.u_offset,
		    v_slope,
		    param.v_offset,
		    param.sig_slope,
		    param.sig_offset,
		    param.uniform_sigma,
		    weights,
		    diff_hwts,
		    bias);
	CPPUNIT_ASSERT(param.v_slope == 0);
}
void testVariableResetOnOffsetV() {
	gradient_structure gradientStructure;
	par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	for (int n = 1; n <= param.num_nn; n++) {
		param.nn(n).nn_ipar(5) = 0;//do not set weights and diff_hwts
		param.nn(n).nn_ipar(6) = 0;//do not set bias
	}
	param.v_offset = 0;
	dvariable v_offset = __LINE__;
	dvar_vector weights, diff_hwts, bias;
	param.m_ipar(74) = 1;
	param.reset(param.q,
		    param.mort,
		    param.special_mort,
		    param.report_rates,
		    param.u_slope,
		    param.u_offset,
		    param.v_slope,
		    v_offset,
		    param.sig_slope,
		    param.sig_offset,
		    param.uniform_sigma,
		    weights,
		    diff_hwts,
		    bias);
	CPPUNIT_ASSERT(param.v_offset == v_offset);
}
void testVariableResetOffOffsetV() {
	gradient_structure gradientStructure;
	par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	for (int n = 1; n <= param.num_nn; n++) {
		param.nn(n).nn_ipar(5) = 0;//do not set weights and diff_hwts
		param.nn(n).nn_ipar(6) = 0;//do not set bias
	}
	param.v_offset = 0;
	dvariable v_offset = __LINE__;
	dvar_vector weights, diff_hwts, bias;
	param.m_ipar(74) = 0;
	param.reset(param.q,
		    param.mort,
		    param.special_mort,
		    param.report_rates,
		    param.u_slope,
		    param.u_offset,
		    param.v_slope,
		    v_offset,
		    param.sig_slope,
		    param.sig_offset,
		    param.uniform_sigma,
		    weights,
		    diff_hwts,
		    bias);
	CPPUNIT_ASSERT(param.v_offset == 0);
}
void testVariableResetOnSlopeSigma() {
	gradient_structure gradientStructure;
	par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	for (int n = 1; n <= param.num_nn; n++) {
		param.nn(n).nn_ipar(5) = 0;//do not set weights and diff_hwts
		param.nn(n).nn_ipar(6) = 0;//do not set bias
	}
	param.sig_slope = 0;
	dvariable sig_slope = __LINE__;
	dvar_vector weights, diff_hwts, bias;
	param.m_ipar(21) = 0;
	param.m_ipar(75) = 1; 
	param.reset(param.q,
		    param.mort,
		    param.special_mort,
		    param.report_rates,
		    param.u_slope,
		    param.u_offset,
		    param.v_slope,
		    param.v_offset,
		    sig_slope,
		    param.sig_offset,
		    param.uniform_sigma,
		    weights,
		    diff_hwts,
		    bias);
	CPPUNIT_ASSERT(param.sig_slope == sig_slope);
}
void testVariableResetOffSlopeSigma() {
	gradient_structure gradientStructure;
	par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	for (int n = 1; n <= param.num_nn; n++) {
		param.nn(n).nn_ipar(5) = 0;//do not set weights and diff_hwts
		param.nn(n).nn_ipar(6) = 0;//do not set bias
	}
	param.sig_slope = 0;
	dvariable sig_slope = __LINE__;
	dvar_vector weights, diff_hwts, bias;
	param.m_ipar(21) = 0;
	param.m_ipar(75) = 0; 
	param.reset(param.q,
		    param.mort,
		    param.special_mort,
		    param.report_rates,
		    param.u_slope,
		    param.u_offset,
		    param.v_slope,
		    param.v_offset,
		    sig_slope,
		    param.sig_offset,
		    param.uniform_sigma,
		    weights,
		    diff_hwts,
		    bias);
	CPPUNIT_ASSERT(param.sig_slope == 0);
}
void testVariableResetOnOffsetSigma() {
	gradient_structure gradientStructure;
	par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	for (int n = 1; n <= param.num_nn; n++) {
		param.nn(n).nn_ipar(5) = 0;//do not set weights and diff_hwts
		param.nn(n).nn_ipar(6) = 0;//do not set bias
	}
	param.sig_offset = 0;
	dvariable sig_offset = __LINE__;
	dvar_vector weights, diff_hwts, bias;
	param.m_ipar(21) = 0;
	param.m_ipar(76) = 1; 
	param.reset(param.q,
		    param.mort,
		    param.special_mort,
		    param.report_rates,
		    param.u_slope,
		    param.u_offset,
		    param.v_slope,
		    param.v_offset,
		    param.sig_slope,
		    sig_offset,
		    param.uniform_sigma,
		    weights,
		    diff_hwts,
		    bias);
	CPPUNIT_ASSERT(param.sig_offset == sig_offset);
}
void testVariableResetOffOffsetSigma() {
	gradient_structure gradientStructure;
	par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	for (int n = 1; n <= param.num_nn; n++) {
		param.nn(n).nn_ipar(5) = 0;//do not set weights and diff_hwts
		param.nn(n).nn_ipar(6) = 0;//do not set bias
	}
	param.sig_offset = 0;
	dvariable sig_offset = __LINE__;
	dvar_vector weights, diff_hwts, bias;
	param.m_ipar(21) = 0;
	param.m_ipar(76) = 0; 
	param.reset(param.q,
		    param.mort,
		    param.special_mort,
		    param.report_rates,
		    param.u_slope,
		    param.u_offset,
		    param.v_slope,
		    param.v_offset,
		    param.sig_slope,
		    sig_offset,
		    param.uniform_sigma,
		    weights,
		    diff_hwts,
		    bias);
	CPPUNIT_ASSERT(param.sig_offset == 0);
}
void testVariableResetOnUniformSigma() {
	gradient_structure gradientStructure;
	par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	for (int n = 1; n <= param.num_nn; n++) {
		param.nn(n).nn_ipar(5) = 0;//do not set weights and diff_hwts
		param.nn(n).nn_ipar(6) = 0;//do not set bias
	}
	param.uniform_sigma = 0;
	dvariable uniform_sigma = __LINE__;
	dvar_vector weights, diff_hwts, bias;
	param.m_ipar(21) = 1;
	param.reset(param.q,
		    param.mort,
		    param.special_mort,
		    param.report_rates,
		    param.u_slope,
		    param.u_offset,
		    param.v_slope,
		    param.v_offset,
		    param.sig_slope,
		    param.sig_offset,
		    uniform_sigma,
		    weights,
		    diff_hwts,
		    bias);
	CPPUNIT_ASSERT(param.uniform_sigma == uniform_sigma);
}
void testVariableResetOffUniformSigma() {
	gradient_structure gradientStructure;
	par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	for (int n = 1; n <= param.num_nn; n++) {
		param.nn(n).nn_ipar(5) = 0;//do not set weights and diff_hwts
		param.nn(n).nn_ipar(6) = 0;//do not set bias
	}
	param.uniform_sigma = 0;
	dvariable uniform_sigma = __LINE__;
	dvar_vector weights, diff_hwts, bias;
	param.m_ipar(21) = 0;
	param.reset(param.q,
		    param.mort,
		    param.special_mort,
		    param.report_rates,
		    param.u_slope,
		    param.u_offset,
		    param.v_slope,
		    param.v_offset,
		    param.sig_slope,
		    param.sig_offset,
		    uniform_sigma,
		    weights,
		    diff_hwts,
		    bias);
	CPPUNIT_ASSERT(param.uniform_sigma == 0);
}
void testVariableResetWeights() {
	gradient_structure gradientStructure;
	par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	for (int n = 1; n <= param.num_nn; n++) {
		param.nn(n).nn_ipar(5) = 1;//set weights and diff_hwts
		param.nn(n).nn_ipar(6) = 0;//do not set bias
	}
	int sizeWeights = 0;
	int sizeDiffHwts = 0;
	for (int n = 1; n <= param.num_nn; n++) {
		if (param.nn(n).nn_ipar(5) == 1) {
			ivector nrow = param.nn(n).get_nrow();
			ivector ncol = param.nn(n).get_ncol();
			const int num_levels =  param.num_levels(n) - 1;
			for (int level = 1; level < num_levels; level++) {
				sizeWeights += nrow(level) * ncol(level);
			}
			sizeDiffHwts += nrow(num_levels) * ncol(num_levels);

			param.nn(n).get_weights().initialize();
		}
	}
	dvar_vector weights(1, sizeWeights); weights.initialize();
	int value = __LINE__;
	for (int i = weights.indexmin(); i <= weights.indexmax(); i++) {
		weights(i) = value;
		value += __LINE__;
	}
	dvar_vector diff_hwts(1, sizeDiffHwts); diff_hwts.initialize();
	dvar_vector bias;
	param.reset(param.q,
		    param.mort,
		    param.special_mort,
		    param.report_rates,
		    param.u_slope,
		    param.u_offset,
		    param.v_slope,
		    param.v_offset,
		    param.sig_slope,
		    param.sig_offset,
		    param.uniform_sigma,
		    weights,
		    diff_hwts,
		    bias);
	int index = weights.indexmin();
	for (int s = 1; s <= param.num_nn; s++) {
		if (param.nn(s).nn_ipar(5) == 1) {
			ivector nrow = param.nn(s).get_nrow();
			ivector ncol = param.nn(s).get_ncol();
			dvar3_array w = param.nn(s).get_weights();
			for (int level = 1; level < param.num_levels(s) - 1; level++) {
				for (int i = 1; i <= nrow(level); i++) {
					for (int j = 1;  j <= ncol(level); j++) {
						CPPUNIT_ASSERT(w(level,i,j) == weights(index));
						index++;
					}
				}
			}
			dvar_matrix d = param.nn(s).get_diff_hwts();
			for (int i = 1; i <= nrow(param.num_levels(s) - 1); i++) {
				for (int j = 1; j <= ncol(param.num_levels(s) - 1); j++) {
					CPPUNIT_ASSERT(d(i,j) == 0);
				}
			}
		}
	}
}
void testVariableResetDiffHwts() {
	gradient_structure gradientStructure;
	par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	for (int n = 1; n <= param.num_nn; n++) {
		param.nn(n).nn_ipar(5) = 1;//set weights and diff_hwts
		param.nn(n).nn_ipar(6) = 0;//do not set bias
	}
	int sizeWeights = 0;
	int sizeDiffHwts = 0;
	for (int n = 1; n <= param.num_nn; n++) {
		if (param.nn(n).nn_ipar(5) == 1) {
			ivector nrow = param.nn(n).get_nrow();
			ivector ncol = param.nn(n).get_ncol();
			const int num_levels =  param.num_levels(n) - 1;
			for (int level = 1; level < num_levels; level++) {
				sizeWeights += nrow(level) * ncol(level);
			}
			sizeDiffHwts += nrow(num_levels) * ncol(num_levels);

			param.nn(n).get_weights().initialize();
		}
	}
	dvar_vector weights(1, sizeWeights); weights.initialize();
	dvar_vector diff_hwts(1, sizeDiffHwts); diff_hwts.initialize();
	int value = __LINE__;
	for (int i = diff_hwts.indexmin(); i <= diff_hwts.indexmax(); i++) {
		diff_hwts(i) = value;
		value += __LINE__;
	}
	dvar_vector bias;
	param.reset(param.q,
		    param.mort,
		    param.special_mort,
		    param.report_rates,
		    param.u_slope,
		    param.u_offset,
		    param.v_slope,
		    param.v_offset,
		    param.sig_slope,
		    param.sig_offset,
		    param.uniform_sigma,
		    weights,
		    diff_hwts,
		    bias);
	int index = diff_hwts.indexmin();
	for (int s = 1; s <= param.num_nn; s++) {
		if (param.nn(s).nn_ipar(5) == 1) {
			ivector nrow = param.nn(s).get_nrow();
			ivector ncol = param.nn(s).get_ncol();
			dvar3_array w = param.nn(s).get_weights();
			for (int level = 1; level < param.num_levels(s) - 1; level++) {
				for (int i = 1; i <= nrow(level); i++) {
					for (int j = 1;  j <= ncol(level); j++) {
						CPPUNIT_ASSERT(weights(index) == 0);
					}
				}
			}
			dvar_matrix d = param.nn(s).get_diff_hwts();
			for (int i = 1; i <= nrow(param.num_levels(s) - 1); i++) {
				for (int j = 1; j <= ncol(param.num_levels(s) - 1); j++) {
					CPPUNIT_ASSERT(diff_hwts(index) == d(i,j));
					index++;
				}
			}
		}
	}
}
void testVariableResetBias() {
	gradient_structure gradientStructure;
	par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	for (int n = 1; n <= param.num_nn; n++) {
		param.nn(n).nn_ipar(5) = 0;//do not set weights and diff_hwts
		param.nn(n).nn_ipar(6) = 1;//set bias
	}
	int sizeBias = 0;
	for (int n = 1; n <= param.num_nn; n++) { //biases
		if (param.nn(n).nn_ipar(6)) {
			ivector nrow = param.nn(n).get_nrow();
			for (int level = 1; level < param.num_levels(n); level++) {
				sizeBias += nrow(level);
			}
			param.nn(n).get_bias().initialize();
		}
	}
	dvar_vector weights, diff_hwts;
	dvar_vector bias(1, sizeBias); bias.initialize();
	int value = __LINE__;
	for (int i = bias.indexmin(); i <= bias.indexmax(); i++) {
		bias(i) = value;
		value += __LINE__;
	}
	param.reset(param.q,
		    param.mort,
		    param.special_mort,
		    param.report_rates,
		    param.u_slope,
		    param.u_offset,
		    param.v_slope,
		    param.v_offset,
		    param.sig_slope,
		    param.sig_offset,
		    param.uniform_sigma,
		    weights,
		    diff_hwts,
		    bias);
	int index = bias.indexmin();
	for (int s = 1; s <= param.num_nn; s++) { //biases
		if (param.nn(s).nn_ipar(6)) {
			ivector nrow = param.nn(s).get_nrow();
			dvar_matrix b = param.nn(s).get_bias();
			for (int level = 1; level < param.num_levels(s); level++) {
				for (int i = 1; i <= nrow(level); i++) {
					CPPUNIT_ASSERT(bias(index) == b(level, i));
					index++;
				}
			}
		}
	}
}
private:
char filename[9];
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestTagestNeuralNetReset);
