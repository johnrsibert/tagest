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

class TestTagestRegionalReset: public CppUnit::TestFixture
{
public:
TestTagestRegionalReset() {;}
TestTagestRegionalReset(const TestTagestRegionalReset&);
virtual ~TestTagestRegionalReset() {;}

public:
CPPUNIT_TEST_SUITE(TestTagestRegionalReset);
CPPUNIT_TEST(testVariableResetOnQ);
CPPUNIT_TEST(testVariableResetOffQ);
CPPUNIT_TEST(testVariableResetOnMort);
CPPUNIT_TEST(testVariableResetOffMort);
CPPUNIT_TEST(testVariableResetOnSpecialMort);
CPPUNIT_TEST(testVariableResetOffSpecialMort);
CPPUNIT_TEST(testVariableResetOnReportRates);
CPPUNIT_TEST(testVariableResetOffReportRates);
CPPUNIT_TEST(testVariableResetOnUserGridU);
CPPUNIT_TEST(testVariableResetOffUserGridU);
CPPUNIT_TEST(testVariableResetOnUserGridV);
CPPUNIT_TEST(testVariableResetOffUserGridV);
CPPUNIT_TEST(testVariableResetOnUserGridSigma);
CPPUNIT_TEST(testVariableResetOffUserGridSigma);
CPPUNIT_TEST(testVariableResetOnUniformUserGridSigma);
CPPUNIT_TEST(testVariableResetOffUniformUserGridSigma);
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
	par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	int value = __LINE__;
	param.q.initialize();
	dvector q(param.q.indexmin(), param.q.indexmax());
	for (int i = param.q.indexmin(); i <= param.q.indexmax(); i++) {
		CPPUNIT_ASSERT(param.q(i) == 0);
		q(i) = value;
		value += __LINE__;
	}
	param.m_ipar(25) = 2;
	param.reset(q,
		    param.mort,
		    param.special_mort,
		    param.report_rates,
		    param.nb_par,
		    param.usergridU,
		    param.usergridV,
		    param.usergridSigma);
	for (int i = param.q.indexmin(); i <= param.q.indexmax(); i++) {
		CPPUNIT_ASSERT(param.q(i) == q(i));
	}
}
void testVariableResetOffQ() {
	gradient_structure gradientStructure;
	par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	int value = __LINE__;
	param.q.initialize();
	dvector q(param.q.indexmin(), param.q.indexmax());
	for (int i = param.q.indexmin(); i <= param.q.indexmax(); i++) {
		CPPUNIT_ASSERT(param.q(i) == 0);
		q(i) = value;
		value += __LINE__;
	}
	param.m_ipar(25) = 0;
	param.reset(q,
		    param.mort,
		    param.special_mort,
		    param.report_rates,
		    param.nb_par,
		    param.usergridU,
		    param.usergridV,
		    param.usergridSigma);
	for (int i = param.q.indexmin(); i <= param.q.indexmax(); i++) {
		CPPUNIT_ASSERT(param.q(i) == 0);
	}
}
void testVariableResetOnMort() {
	gradient_structure gradientStructure;
	par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	param.mort = 0;
	dvariable mort = __LINE__;
	param.m_ipar(22) = 1;
	param.reset(param.q,
		    mort,
		    param.special_mort,
		    param.report_rates,
		    param.nb_par,
		    param.usergridU,
		    param.usergridV,
		    param.usergridSigma);
	CPPUNIT_ASSERT(param.mort == mort);
}
void testVariableResetOffMort() {
	gradient_structure gradientStructure;
	par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	param.mort = 0;
	dvariable mort = __LINE__;
	param.m_ipar(22) = 0;
	param.reset(param.q,
		    mort,
		    param.special_mort,
		    param.report_rates,
		    param.nb_par,
		    param.usergridU,
		    param.usergridV,
		    param.usergridSigma);
	CPPUNIT_ASSERT(param.mort == 0);
}
void testVariableResetOnSpecialMort() {
	gradient_structure gradientStructure;
	par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	param.special_mort = 0;
	dvariable special_mort = __LINE__;
	param.m_ipar(22) = 1;
	param.m_ipar(19) = 1;
	param.reset(param.q,
		    param.mort,
		    special_mort,
		    param.report_rates,
		    param.nb_par,
		    param.usergridU,
		    param.usergridV,
		    param.usergridSigma);
	CPPUNIT_ASSERT(param.special_mort == special_mort);
}
void testVariableResetOffSpecialMort() {
	gradient_structure gradientStructure;
	par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	param.special_mort = 0;
	dvariable special_mort = __LINE__;
	param.m_ipar(22) = 0;
	param.reset(param.q,
		    param.mort,
		    special_mort,
		    param.report_rates,
		    param.nb_par,
		    param.usergridU,
		    param.usergridV,
		    param.usergridSigma);
	CPPUNIT_ASSERT(param.special_mort == 0);
}
void testVariableResetOnReportRates() {
	gradient_structure gradientStructure;
	par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	int value = __LINE__;
	param.report_rates.initialize();
	dvector report_rates(param.report_rates.indexmin(), param.report_rates.indexmax());
	for (int i = param.report_rates.indexmin(); i <= param.report_rates.indexmax(); i++) {
		CPPUNIT_ASSERT(param.report_rates(i) == 0);
		report_rates(i) = value;
		value += __LINE__;
	}
	param.m_ipar(28) = 1;
	param.reset(param.q,
		    param.mort,
		    param.special_mort,
		    report_rates,
		    param.nb_par,
		    param.usergridU,
		    param.usergridV,
		    param.usergridSigma);
	for (int i = param.report_rates.indexmin(); i <= param.report_rates.indexmax(); i++) {
		CPPUNIT_ASSERT(param.report_rates(i) == report_rates(i));
	}
}
void testVariableResetOffReportRates() {
	gradient_structure gradientStructure;
	par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	int value = __LINE__;
	param.report_rates.initialize();
	dvector report_rates(param.report_rates.indexmin(), param.report_rates.indexmax());
	for (int i = param.report_rates.indexmin(); i <= param.report_rates.indexmax(); i++) {
		CPPUNIT_ASSERT(param.report_rates(i) == 0);
		report_rates(i) = value;
		value += __LINE__;
	}
	param.m_ipar(28) = 0;
	param.reset(param.q,
		    param.mort,
		    param.special_mort,
		    report_rates,
		    param.nb_par,
		    param.usergridU,
		    param.usergridV,
		    param.usergridSigma);
	for (int i = param.report_rates.indexmin(); i <= param.report_rates.indexmax(); i++) {
		CPPUNIT_ASSERT(param.report_rates(i) == 0);
	}
}
void testVariableResetOnUserGridU() {
	gradient_structure gradientStructure;
	par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	param.m_ipar(23) = 1;
	int value = __LINE__;
	param.usergridU.initialize();
	const int nseason = param.get_nseason();
	const int ngrid = param.get_ngrid();
	dvar_matrix usergridU(1, nseason, 0, ngrid);
	for (int season = 1; season <= nseason; season++) {
		for (int grid = 1; grid <= ngrid; grid++) {
			param.stratum_active(season,grid) = 1;
			CPPUNIT_ASSERT(param.usergridU(season,grid) == 0);
			usergridU(season,grid) = value;
			value += __LINE__;
		}
	}
	param.reset(param.q,
		    param.mort,
		    param.special_mort,
		    param.report_rates,
		    param.nb_par,
		    usergridU,
		    param.usergridV,
		    param.usergridSigma);
	for (int season = 1; season <= nseason; season++) {
		for (int grid = 1; grid <= ngrid; grid++) {
			CPPUNIT_ASSERT(param.usergridU(season,grid) == usergridU(season,grid));
		}
	}
}
void testVariableResetOffUserGridU() {
	gradient_structure gradientStructure;
	par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	param.m_ipar(23) = 0;
	int value = __LINE__;
	param.usergridU.initialize();
	const int nseason = param.get_nseason();
	const int ngrid = param.get_ngrid();
	dvar_matrix usergridU(1, nseason, 0, ngrid);
	for (int season = 1; season <= nseason; season++) {
		for (int grid = 1; grid <= ngrid; grid++) {
			param.stratum_active(season,grid) = 1;
			CPPUNIT_ASSERT(param.usergridU(season,grid) == 0);
			usergridU(season,grid) = value;
			value += __LINE__;
		}
	}
	param.reset(param.q,
		    param.mort,
		    param.special_mort,
		    param.report_rates,
		    param.nb_par,
		    usergridU,
		    param.usergridV,
		    param.usergridSigma);
	for (int season = 1; season <= nseason; season++) {
		for (int grid = 1; grid <= ngrid; grid++) {
			CPPUNIT_ASSERT(param.usergridU(season,grid) == 0);
		}
	}
}
void testVariableResetOnUserGridV() {
	gradient_structure gradientStructure;
	par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	param.m_ipar(23) = 1;
	int value = __LINE__;
	param.usergridV.initialize();
	const int nseason = param.get_nseason();
	const int ngrid = param.get_ngrid();
	dvar_matrix usergridV(1, nseason, 0, ngrid);
	for (int season = 1; season <= nseason; season++) {
		for (int grid = 1; grid <= ngrid; grid++) {
			param.stratum_active(season,grid) = 1;
			CPPUNIT_ASSERT(param.usergridV(season,grid) == 0);
			usergridV(season,grid) = value;
			value += __LINE__;
		}
	}
	param.reset(param.q,
		    param.mort,
		    param.special_mort,
		    param.report_rates,
		    param.nb_par,
		    param.usergridU,
		    usergridV,
		    param.usergridSigma);
	for (int season = 1; season <= nseason; season++) {
		for (int grid = 1; grid <= ngrid; grid++) {
			CPPUNIT_ASSERT(param.usergridV(season,grid) == usergridV(season,grid));
		}
	}
}
void testVariableResetOffUserGridV() {
	gradient_structure gradientStructure;
	par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	param.m_ipar(23) = 0;
	int value = __LINE__;
	param.usergridV.initialize();
	const int nseason = param.get_nseason();
	const int ngrid = param.get_ngrid();
	dvar_matrix usergridV(1, nseason, 0, ngrid);
	for (int season = 1; season <= nseason; season++) {
		for (int grid = 1; grid <= ngrid; grid++) {
			param.stratum_active(season,grid) = 1;
			CPPUNIT_ASSERT(param.usergridV(season,grid) == 0);
			usergridV(season,grid) = value;
			value += __LINE__;
		}
	}
	param.reset(param.q,
		    param.mort,
		    param.special_mort,
		    param.report_rates,
		    param.nb_par,
		    param.usergridU,
		    usergridV,
		    param.usergridSigma);
	for (int season = 1; season <= nseason; season++) {
		for (int grid = 1; grid <= ngrid; grid++) {
			CPPUNIT_ASSERT(param.usergridV(season,grid) == 0);
		}
	}
}
void testVariableResetOnUserGridSigma() {
	gradient_structure gradientStructure;
	par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	param.m_ipar(21) = 2;
	int value = __LINE__;
	param.usergridSigma.initialize();
	const int nseason = param.get_nseason();
	const int ngrid = param.get_ngrid();
	dvar_matrix usergridSigma(1, nseason, 0, ngrid);
	for (int season = 1; season <= nseason; season++) {
		for (int grid = 1; grid <= ngrid; grid++) {
			param.stratum_active(season,grid) = 1;
			CPPUNIT_ASSERT(param.usergridSigma(season,grid) == 0);
			usergridSigma(season,grid) = value;
			value += __LINE__;
		}
	}
	param.reset(param.q,
		    param.mort,
		    param.special_mort,
		    param.report_rates,
		    param.nb_par,
		    param.usergridU,
		    param.usergridV,
		    usergridSigma);
	for (int season = 1; season <= nseason; season++) {
		for (int grid = 1; grid <= ngrid; grid++) {
			CPPUNIT_ASSERT(param.usergridSigma(season,grid) == usergridSigma(season,grid));
		}
	}
}
void testVariableResetOffUserGridSigma() {
	gradient_structure gradientStructure;
	par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	param.m_ipar(21) = 0;
	int value = __LINE__;
	param.usergridSigma.initialize();
	const int nseason = param.get_nseason();
	const int ngrid = param.get_ngrid();
	dvar_matrix usergridSigma(1, nseason, 0, ngrid);
	for (int season = 1; season <= nseason; season++) {
		for (int grid = 1; grid <= ngrid; grid++) {
			param.stratum_active(season,grid) = 1;
			CPPUNIT_ASSERT(param.usergridSigma(season,grid) == 0);
			usergridSigma(season,grid) = value;
			value += __LINE__;
		}
	}
	param.reset(param.q,
		    param.mort,
		    param.special_mort,
		    param.report_rates,
		    param.nb_par,
		    param.usergridU,
		    param.usergridV,
		    usergridSigma);
	for (int season = 1; season <= nseason; season++) {
		for (int grid = 1; grid <= ngrid; grid++) {
			CPPUNIT_ASSERT(param.usergridSigma(season,grid) == 0);
		}
	}
}
void testVariableResetOnUniformUserGridSigma() {
	gradient_structure gradientStructure;
	par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	param.m_ipar(21) = 1;
	int value = __LINE__;
	param.usergridSigma.initialize();
	const int nseason = param.get_nseason();
	const int ngrid = param.get_ngrid();
	dvar_matrix usergridSigma(1, nseason, 0, ngrid);
	for (int season = 1; season <= nseason; season++) {
		for (int grid = 1; grid <= ngrid; grid++) {
			param.stratum_active(season,grid) = 1;
			CPPUNIT_ASSERT(param.usergridSigma(season,grid) == 0);
			usergridSigma(season,grid) = value;
			value += __LINE__;
		}
	}
	param.reset(param.q,
		    param.mort,
		    param.special_mort,
		    param.report_rates,
		    param.nb_par,
		    param.usergridU,
		    param.usergridV,
		    usergridSigma);
	for (int season = 1; season <= 1; season++) {
		for (int grid = 1; grid <= 1; grid++) {
			CPPUNIT_ASSERT(param.usergridSigma(season,grid) == usergridSigma(season,grid));
		}
	}
}
void testVariableResetOffUniformUserGridSigma() {
	gradient_structure gradientStructure;
	par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable> param(filename);
	param.m_ipar(21) = 0;
	int value = __LINE__;
	param.usergridSigma.initialize();
	const int nseason = param.get_nseason();
	const int ngrid = param.get_ngrid();
	dvar_matrix usergridSigma(1, nseason, 0, ngrid);
	for (int season = 1; season <= nseason; season++) {
		for (int grid = 1; grid <= ngrid; grid++) {
			param.stratum_active(season,grid) = 1;
			CPPUNIT_ASSERT(param.usergridSigma(season,grid) == 0);
			usergridSigma(season,grid) = value;
			value += __LINE__;
		}
	}
	param.reset(param.q,
		    param.mort,
		    param.special_mort,
		    param.report_rates,
		    param.nb_par,
		    param.usergridU,
		    param.usergridV,
		    usergridSigma);
	for (int season = 1; season <= 1; season++) {
		for (int grid = 1; grid <= 1; grid++) {
			CPPUNIT_ASSERT(param.usergridSigma(season,grid) == 0);
		}
	}
}
private:
char filename[9];
};
CPPUNIT_TEST_SUITE_REGISTRATION(TestTagestRegionalReset);
