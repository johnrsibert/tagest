/**
 * $Id: TestCPPUNIT.cpp 1164 2006-07-24 20:29:27Z johnoel $
 */
#include "TestTagestAdmodel.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(TestTagestAdmodel);

//Begin:------
void TestTagestAdmodel::testSimpleAdmodel() {
	gradient_structure gradientStructure;
	MinimizerTemplate minimizerTemplate;
	SimpleAdmodel simpleAdmodel(-1, 1);
	minimizerTemplate.minimize(simpleAdmodel);
}
void TestTagestAdmodel::testSimpleAdmodel2() {
	gradient_structure gradientStructure;
	MinimizerTemplate minimizerTemplate;
	SimpleAdmodel simpleAdmodel(3, 5);
	minimizerTemplate.minimize(simpleAdmodel);
}
//End:------
//Begin:------
void TestTagestAdmodel::testPar_tSingleton() {
	gradient_structure gradientStructure;
	CPPUNIT_ASSERT(par_t::instance == 0);

	par_t::createInstance("skj10");
	CPPUNIT_ASSERT(par_t::instance != 0);

	par_t::deleteInstance();
	CPPUNIT_ASSERT(par_t::instance == 0);
}
void TestTagestAdmodel::testCoff_tSingleton() {
	gradient_structure gradientStructure(350000);
	CPPUNIT_ASSERT(coff_t::instance == 0);

	par_t_reg param("skj10");
	coff_t::createInstance(param);
	CPPUNIT_ASSERT(coff_t::instance != 0);

	coff_t::deleteInstance();
	CPPUNIT_ASSERT(coff_t::instance == 0);
}
void TestTagestAdmodel::testRecaptypeVectorSingleton() {
	CPPUNIT_ASSERT(recaptype_vector::instance == 0);

	par_t_reg param("skj10");
	int nrec = -1;
	recaptype_vector::createInstance("skj10", param, nrec);
	CPPUNIT_ASSERT(recaptype_vector::instance != 0);
	CPPUNIT_ASSERT(nrec > 0);

	recaptype_vector::deleteInstance();
	CPPUNIT_ASSERT(recaptype_vector::instance == 0);
}
void TestTagestAdmodel::testIndexedRegionalFisheryRecordSingleton() {
	CPPUNIT_ASSERT(indexed_regional_fishery_record::instance == 0);

	indexed_regional_fishery_record::createInstance("skj10");
	CPPUNIT_ASSERT(indexed_regional_fishery_record::instance != 0);

	indexed_regional_fishery_record::deleteInstance();
	CPPUNIT_ASSERT(indexed_regional_fishery_record::instance == 0);
}
//End:------
//Begin:------
void TestTagestAdmodel::testModelParameters() {
	char *argv[10] = {"tagest\0"};
	model_parameters mp(350000, 1, argv);
	mp.final_calcs();
	CPPUNIT_ASSERT(par_t::instance == 0);
	CPPUNIT_ASSERT(coff_t::instance == 0);
	CPPUNIT_ASSERT(indexed_regional_fishery_record::instance == 0);
	CPPUNIT_ASSERT(recaptype_vector::instance == 0);
}
void TestTagestAdmodel::testModelParametersLocalCalcsParameterSection() {
	char *argv[10] = {"tagest\0"};
	model_parameters mp(350000, 1, argv);
	par_t_reg &param = (par_t_reg&)*par_t::instance;
	testCheckParametersEqual(mp, param);
	mp.final_calcs();
}
//End:------
//Begin:------
void TestTagestAdmodel::testModelParametersDataSection() {
	char *argv[10] = {"tagest\0"};
	model_parameters mp(350000, 1, argv);
	CPPUNIT_ASSERT(mp.toStringDataSection() == "\"filename\":\"skj10\"");
	mp.final_calcs();
}
void TestTagestAdmodel::testModelParametersParameterSection() {
	char *argv[10] = {"tagest\0"};
	model_parameters mp(350000, 1, argv);
	CPPUNIT_ASSERT(mp.toStringParameterSection() == "\"\":\"\"");
	mp.final_calcs();
}
//End:------
//Begin:------
void TestTagestAdmodel::testModelParametersReadData() {
	char *argv[10] = {"tagest\0"};
	CPPUNIT_ASSERT(par_t::instance == 0);
	CPPUNIT_ASSERT(coff_t::instance == 0);
	CPPUNIT_ASSERT(indexed_regional_fishery_record::instance == 0);
	CPPUNIT_ASSERT(recaptype_vector::instance == 0);
	model_parameters mp(350000, 1, argv);
	CPPUNIT_ASSERT(mp.readData() > 0);
	CPPUNIT_ASSERT(par_t::instance != 0);
	CPPUNIT_ASSERT(coff_t::instance != 0);
	CPPUNIT_ASSERT(indexed_regional_fishery_record::instance != 0);
	CPPUNIT_ASSERT(recaptype_vector::instance != 0);
	mp.final_calcs();
	CPPUNIT_ASSERT(par_t::instance == 0);
	CPPUNIT_ASSERT(coff_t::instance == 0);
	CPPUNIT_ASSERT(indexed_regional_fishery_record::instance == 0);
	CPPUNIT_ASSERT(recaptype_vector::instance == 0);
}
//End:------
//Begin:------
void TestTagestAdmodel::testModelParametersReset() {
	char *argv[10] = {"tagest\0"};
	model_parameters mp(0, 1, argv);
	par_t_reg &param = (par_t_reg&)*par_t::instance;
/*
	const double value = -1234567890.0987654321;
	param.set_mort(value);
	param.set_special_mort(value);
	for (int fleet = 1; fleet  <= param.nfleet; fleet ++) {
		param.set_q(fleet, value);
		param.set_report_rates(fleet, value);
		param.nb_par(fleet) = value;
	}
	for (int season = 1; season <= param.get_nseason(); season++) {
		for (int grid = 1; grid <= param.get_ngrid(); grid++) {
			param.set_usergrid_u(season, grid, value);
			param.set_usergrid_v(season, grid, value);
			param.set_usergrid_sigma(season, grid, value);
		}
	}
*/
	testCheckParametersEqual(mp, param);
	slave_gradcalc();
	mp.final_calcs();
}
void TestTagestAdmodel::testUvComp() {
	char *argv[10] = {"tagest\0"};
	CPPUNIT_ASSERT(par_t::instance == 0);
	model_parameters mp(350000, 1, argv);
	CPPUNIT_ASSERT(par_t::instance != 0);

	par_t_reg &param = (par_t_reg&)*par_t::instance;

	const int m = param.get_m();
	const int n = param.get_n();
	ivector jlb1(0,m+1);
	ivector jub1(0,m+1);
	for (int i = 1; i <= m; i++) {
		jlb1(i) = param.get_jlb(i) - 1;
		jub1(i) = param.get_jub(i) + 1;
	}
	jlb1(0) = 0;
	jlb1(m+1) = 0;
	jub1(0) = n+1;
	jub1(m+1) = n+1;
	dmatrix U(0, m+1, jlb1, jub1);
	dmatrix V(0, m+1, jlb1, jub1);
	dvar_matrix variableU(0, m+1, jlb1, jub1);
	dvar_matrix variableV(0, m+1, jlb1, jub1);

	for (int season = 1; season <= param.get_nseason(); season++) {
		CPPUNIT_ASSERT(isValuesEqual(param.get_usergrid_u(season, 0), 0));
		CPPUNIT_ASSERT(isValuesEqual(param.get_usergrid_v(season, 0), 0));
		CPPUNIT_ASSERT(isValuesEqual(param.get_usergrid_sigma(season, 0), 0));
	}

	const year_month &startDate = param.get_start_date();
	const int nrelease = param.get_nrelease();
	const int nmonth = param.get_nmonth();
	const year_month finalDate = param.get_tr_date(nrelease) + nmonth;
        for (year_month ym = startDate; ym <= finalDate; ym++) {
		dvar_matrix uSeasonGrid;
		uSeasonGrid.allocate(mp.getParameterU());
		uSeasonGrid.initialize();
		uSeasonGrid = mp.getUSeasonGrid();
		dvar_matrix vSeasonGrid;
		vSeasonGrid.allocate(mp.getParameterV());
		vSeasonGrid.initialize();
		vSeasonGrid = mp.getVSeasonGrid();
		for (int season = 1; season <= param.get_nseason(); season++) {
			for (int grid = 1; grid <= param.get_ngrid(); grid++) {
				CPPUNIT_ASSERT(isValuesEqual(param.get_usergrid_u(season, grid), value(uSeasonGrid(season, grid))));
				CPPUNIT_ASSERT(isValuesEqual(param.get_usergrid_v(season, grid), value(vSeasonGrid(season, grid))));
			}
		}
		const int season = param.get_season(ym);
		U.initialize();
		V.initialize();
		param.uvcomp(U, V, season);
		variableU.initialize();
		variableV.initialize();
		param.uvcomp(uSeasonGrid, vSeasonGrid, variableU, variableV, season);
		for (int i = 0; i <= m + 1; i++) {
			const int jmin = jlb1(i);
			const int jmax = jub1(i);
			for (int j = jmin; j <= jmax; j++) {
				const double valueV = value(variableV(i, j));
				const double expectedV = V(i,j);
				CPPUNIT_ASSERT(expectedV - 1e-5 <= valueV && valueV <= expectedV + 1e-5);
				const double valueU = value(variableU(i, j));
				const double expectedU = U(i,j);
				CPPUNIT_ASSERT(expectedU - 1e-5 <= valueU && valueU <= expectedU + 1e-5);
			}
		}
	}
	slave_gradcalc();

	mp.final_calcs();
}
void TestTagestAdmodel::testUvinterpolate() {
	char *argv[10] = {"tagest\0"};
	CPPUNIT_ASSERT(par_t::instance == 0);
	model_parameters mp(350000, 1, argv);
	CPPUNIT_ASSERT(par_t::instance != 0);

	//mp.readData();
	par_t_reg &param = (par_t_reg&)*par_t::instance;

	const int m = param.get_m();
	const int n = param.get_n();
	ivector jlb1(0,m+1);
	ivector jub1(0,m+1);
	for (int i = 1; i <= m; i++) {
		jlb1(i) = param.get_jlb(i) - 1;
		jub1(i) = param.get_jub(i) + 1;
	}
	jlb1(0) = 0;
	jlb1(m+1) = 0;
	jub1(0) = n+1;
	jub1(m+1) = n+1;
	dmatrix U(0, m+1, jlb1, jub1);
	dmatrix V(0, m+1, jlb1, jub1);
	dvar_matrix variableU(0, m+1, jlb1, jub1);
	dvar_matrix variableV(0, m+1, jlb1, jub1);

	const year_month &startDate = param.get_start_date();
	const int nrelease = param.get_nrelease();
	const int nmonth = param.get_nmonth();
	const year_month finalDate = param.get_tr_date(nrelease) + nmonth;
        for (year_month ym = startDate; ym <= finalDate; ym++) {
		U.initialize();
		V.initialize();
		variableU.initialize();
		variableV.initialize();

		dvar_matrix uSeasonGrid(mp.getParameterU());
		dvar_matrix vSeasonGrid(mp.getParameterV());
		param.uvinterpolate(U, V, ym);
		const int season = param.get_season(ym);
		param.uvinterpolate(uSeasonGrid, vSeasonGrid, variableU, variableV, season);
		for (int i = 0; i <= m + 1; i++) {
			const int jmin = jlb1(i);
			const int jmax = jub1(i);
			for (int j = jmin; j <= jmax; j++) {
				const double valueU = value(variableU(i, j));
				const double expectedU = U(i,j);
				CPPUNIT_ASSERT(expectedU - 1e-5 <= valueU && valueU <= expectedU + 1e-5);
				const double valueV = value(variableV(i, j));
				const double expectedV = V(i,j);
				CPPUNIT_ASSERT(expectedV - 1e-5 <= valueV && valueV <= expectedV + 1e-5);
			}
		}
	}
	slave_gradcalc();
	mp.final_calcs();
}
void TestTagestAdmodel::testSigmaComp() {
	void rowcol_average(dmatrix& x, const int npoint);
	void rowcol_average(dvar_matrix& x, const int npoint);

	char *argv[10] = {"tagest\0"};
	CPPUNIT_ASSERT(par_t::instance == 0);
	model_parameters mp(350000, 1, argv);
	CPPUNIT_ASSERT(par_t::instance != 0);

	par_t_reg &param = (par_t_reg&)*par_t::instance;

	const int m = param.get_m();
	const int n = param.get_n();
	ivector jlb1(0,m+1);
	ivector jub1(0,m+1);
	for (int i = 1; i <= m; i++) {
		jlb1(i) = param.get_jlb(i) - 1;
		jub1(i) = param.get_jub(i) + 1;
	}
	jlb1(0) = 0;
	jlb1(m+1) = 0;
	jub1(0) = n+1;
	jub1(m+1) = n+1;
	dmatrix sigma(0, m+1, jlb1, jub1);
	dvar_matrix variableSigma(0, m+1, jlb1, jub1);

	const year_month &startDate = param.get_start_date();
	const int nrelease = param.get_nrelease();
	const int nmonth = param.get_nmonth();
	const year_month finalDate = param.get_tr_date(nrelease) + nmonth;
        for (year_month ym = startDate; ym <= finalDate; ym++) {
		dvar_matrix sigmaSeasonGrid(mp.getSigmaSeasonGrid());
		for (int season = 1; season <= param.get_nseason(); season++) {
			for (int grid = 1; grid <= param.get_ngrid(); grid++) {
				CPPUNIT_ASSERT(isValuesEqual(param.get_usergrid_sigma(season, grid), value(sigmaSeasonGrid(season, grid))));
			}
		}
		sigma.initialize();
		param.sigma_comp(sigma, ym);
		variableSigma.initialize();
		const int season = param.get_season(ym);
		param.sigma_comp(sigmaSeasonGrid, variableSigma, season);
		for (int i = 0; i <= m + 1; i++) {
			const int jmin = jlb1(i);
			const int jmax = jub1(i);
			for (int j = jmin; j <= jmax; j++) {
				const double valueSigma = value(variableSigma(i, j));
				const double expectedSigma = sigma(i,j);
				CPPUNIT_ASSERT(expectedSigma - 1e-5 <= valueSigma && valueSigma <= expectedSigma + 1e-5);
			}
		}
		rowcol_average(sigma, param.m_ipar(36));
		rowcol_average(variableSigma, param.m_ipar(36));
		for (int i = 0; i <= m + 1; i++) {
			const int jmin = jlb1(i);
			const int jmax = jub1(i);
			for (int j = jmin; j <= jmax; j++) {
				const double valueSigma = value(variableSigma(i, j));
				const double expectedSigma = sigma(i,j);
				CPPUNIT_ASSERT(expectedSigma - 1e-5 <= valueSigma && valueSigma <= expectedSigma + 1e-5);
			}
		}
	}
	mp.final_calcs();
}
void TestTagestAdmodel::testUvsComp() {
	char *argv[10] = {"tagest\0"};
	CPPUNIT_ASSERT(par_t::instance == 0);
	model_parameters mp(350000, 1, argv);
	CPPUNIT_ASSERT(par_t::instance != 0);

	par_t_reg &param = (par_t_reg&)*par_t::instance;

	const int m = param.get_m();
	const int n = param.get_n();
	ivector jlb1(0,m+1);
	ivector jub1(0,m+1);
	for (int i = 1; i <= m; i++) {
		jlb1(i) = param.get_jlb(i) - 1;
		jub1(i) = param.get_jub(i) + 1;
	}
	jlb1(0) = 0;
	jlb1(m+1) = 0;
	jub1(0) = n+1;
	jub1(m+1) = n+1;
	dmatrix U(0, m+1, jlb1, jub1);
	dmatrix V(0, m+1, jlb1, jub1);
	dmatrix sigma(0, m+1, jlb1, jub1);
	dvar_matrix variableU(0, m+1, jlb1, jub1);
	dvar_matrix variableV(0, m+1, jlb1, jub1);
	dvar_matrix variableSigma(0, m+1, jlb1, jub1);

	const year_month &startDate = param.get_start_date();
	const int nrelease = param.get_nrelease();
	const int nmonth = param.get_nmonth();
	const year_month finalDate = param.get_tr_date(nrelease) + nmonth;
        for (year_month ym = startDate; ym <= finalDate; ym++) {
		for (int i = 0; i <= m + 1; i++) {
			const int jmin = jlb1(i);
			const int jmax = jub1(i);
			for (int j = jmin; j <= jmax; j++) {
				U(i, j) = 0;
				V(i, j) = 0;
				sigma(i, j) = 0;
				variableU(i, j) = 0;
				variableV(i, j) = 0;
				variableSigma(i, j) = 0;
			}
		}

		const int season = param.get_season(ym);
		dvar_matrix uSeasonGrid(mp.getParameterU());
		dvar_matrix vSeasonGrid(mp.getParameterV());
		dvar_matrix sigmaSeasonGrid(mp.getParameterSigma());
		param.uvs_comp(uSeasonGrid, vSeasonGrid, sigmaSeasonGrid, variableU, variableV, variableSigma, season);
		param.uvs_comp(U, V, sigma, ym);

		for (int i = 0; i <= m + 1; i++) {
			const int jmin = jlb1(i);
			const int jmax = jub1(i);
			for (int j = jmin; j <= jmax; j++) {
				const double valueU = value(variableU(i, j));
				const double expectedU = U(i,j);
				CPPUNIT_ASSERT(expectedU - 1e-5 <= valueU && valueU <= expectedU + 1e-5);

				const double valueV = value(variableV(i, j));
				const double expectedV = V(i,j);
				CPPUNIT_ASSERT(expectedV - 1e-5 <= valueV && valueV <= expectedV + 1e-5);

				const double valueSigma = value(variableSigma(i, j));
				const double expectedSigma = sigma(i,j);
				CPPUNIT_ASSERT(expectedSigma - 1e-5 <= valueSigma && valueSigma <= expectedSigma + 1e-5);
			}
		}
	}
	mp.final_calcs();
}
void TestTagestAdmodel::testParamVelpen() {
	void velpen(par_t_reg& param, double& temp, year_month& date);

	char *argv[10] = {"tagest\0"};
	model_parameters mp(350000, 1, argv);

	CPPUNIT_ASSERT(par_t::instance != 0);
	par_t_reg &param = *(par_t_reg*)par_t::instance;
	const year_month &startDate = param.get_start_date();
	const int nrelease = param.get_nrelease();
	const int nmonth = param.get_nmonth();
	const year_month finalDate = param.get_tr_date(nrelease) + nmonth;
        for (year_month ym = startDate; ym <= finalDate; ym++) {
		const int season = param.get_season(ym);
		dvar_matrix uSeasonGrid(mp.getParameterU());
		dvar_matrix vSeasonGrid(mp.getParameterV());
		const double v = value(param.addTotalUVPenalty(uSeasonGrid, vSeasonGrid, season));
		CPPUNIT_ASSERT(v <= 0);

		double expected = 0;
		velpen(param, expected, ym);
		CPPUNIT_ASSERT(expected <= 0);

		CPPUNIT_ASSERT(expected - 1e-5 <= v && v <= expected + 1e-5);
	}
	mp.final_calcs();
}
void TestTagestAdmodel::testParamSigpen() {
	void sigpen(par_t_reg& param, double& temp, year_month& date);

	char *argv[10] = {"tagest\0"};
	model_parameters mp(350000, 1, argv);

	CPPUNIT_ASSERT(par_t::instance != 0);
	par_t_reg &param = *(par_t_reg*)par_t::instance;
	const year_month &startDate = param.get_start_date();
	const int nrelease = param.get_nrelease();
	const int nmonth = param.get_nmonth();
	const year_month finalDate = param.get_tr_date(nrelease) + nmonth;
        for (year_month ym = startDate; ym <= finalDate; ym++) {
		const int season = param.get_season(ym);
		dvar_matrix sigmaSeasonGrid(mp.getParameterSigma());
		const double v = value(param.addTotalSigmaPenalty(sigmaSeasonGrid, season));
		CPPUNIT_ASSERT(-1.0 <= v && v <= 1.0);

		double expected = 0;
		sigpen(param, expected, ym);
		CPPUNIT_ASSERT(-1.0 <= expected && expected <= 1.0);

		CPPUNIT_ASSERT(expected - 1e-5 <= v && v <= expected + 1e-5);
	}
	mp.final_calcs();
}
void TestTagestAdmodel::testCoffComp() {
	char *argv[10] = {"tagest\0"};
	CPPUNIT_ASSERT(par_t::instance == 0);
	model_parameters mp(350000, 1, argv);
	CPPUNIT_ASSERT(par_t::instance != 0);

	mp.readData();

	par_t_reg &param = (par_t_reg&)*par_t::instance;
	coff_t &coff= (coff_t&)*coff_t::instance;

	const int m = param.get_m();
	const int n = param.get_n();
	const ivector &ilb = param.get_ilb();
	const ivector &iub = param.get_iub();
	const ivector &jlb = param.get_jlb();
	const ivector &jub = param.get_jub();

	ivector jlb1(0,m+1);
	ivector jub1(0,m+1);
	for (int i = 1; i <= m; i++) {
		jlb1(i) = param.get_jlb(i) - 1;
		jub1(i) = param.get_jub(i) + 1;
	}
	jlb1(0) = 0;
	jlb1(m+1) = 0;
	jub1(0) = n+1;
	jub1(m+1) = n+1;
	dmatrix u(0, m+1, jlb1, jub1);
	dmatrix v(0, m+1, jlb1, jub1);
	dmatrix sigma(0, m+1, jlb1, jub1);
	dvar_matrix variableU(0, m+1, jlb1, jub1);
	dvar_matrix variableV(0, m+1, jlb1, jub1);
	dvar_matrix variableSigma(0, m+1, jlb1, jub1);

	const year_month &startDate = param.get_start_date();
	const int nrelease = param.get_nrelease();
	const int nmonth = param.get_nmonth();
	const year_month finalDate = param.get_tr_date(nrelease) + nmonth;
        for (year_month ym = startDate; ym <= finalDate; ym++) {
		const int season = param.get_season(ym);
		param.uvs_comp(u, v, sigma, ym);
		dvar_matrix uSeasonGrid(mp.getParameterU());
		dvar_matrix vSeasonGrid(mp.getParameterV());
		dvar_matrix sigmaSeasonGrid(mp.getParameterSigma());
		param.uvs_comp(uSeasonGrid, vSeasonGrid, sigmaSeasonGrid, variableU, variableV, variableSigma, season);
		coff.comp(param, ym, 0, u, v, sigma);
		coff.comp(param, ym, variableU, variableV, variableSigma);

		dmatrix &a = coff.get_a();
		dmatrix &raw_b = coff.get_raw_b();
		dmatrix &c = coff.get_c();
		dmatrix &d = coff.get_d();
		dmatrix &e = coff.get_e();
		dmatrix &f = coff.get_f();
		dmatrix &ybet = coff.get_ybet();
		dmatrix &ygam = coff.get_ygam();
		dvar_matrix &variableA = coff.getVariableA();
		dvar_matrix &variableRawB = coff.getVariableRawB();
		dvar_matrix &variableC = coff.getVariableC();
		dvar_matrix &variableD = coff.getVariableD();
		dvar_matrix &variableE = coff.getVariableE();
		dvar_matrix &variableF = coff.getVariableF();
		dvar_matrix &variableYBet = coff.getVariableYBet();
		dvar_matrix &variableYGam = coff.getVariableYGam();
		for (int j = 1; j <= n; j++) {
			const int imin = ilb(j);
			const int imax = iub(j);
			for (int i = imin; i <= imax; i++) {
				CPPUNIT_ASSERT(isValuesEqual(a(j,i), value(variableA(j, i))));
				CPPUNIT_ASSERT(isValuesEqual(raw_b(j,i), value(variableRawB(j, i))));
				CPPUNIT_ASSERT(isValuesEqual(c(j,i), value(variableC(j, i))));
			}
		}
		for (int i = 1; i <= m; i++) {
			const int jmin = jlb(i);
			const int jmax = jub(i);
			for (int j = jmin; j <= jmax; j++) {
				CPPUNIT_ASSERT(isValuesEqual(d(i,j), value(variableD(i, j))));
				CPPUNIT_ASSERT(isValuesEqual(e(i,j), value(variableE(i, j))));
				CPPUNIT_ASSERT(isValuesEqual(f(i,j), value(variableF(i, j))));
				CPPUNIT_ASSERT(isValuesEqual(ybet(i,j), value(variableYBet(i, j))));
				CPPUNIT_ASSERT(isValuesEqual(ygam(i,j), value(variableYGam(i, j))));
			}
		}
	}
	mp.final_calcs();
}
void TestTagestAdmodel::testGetEffortArray() {
	void get_effort_array(par_t& param, indexed_regional_fishery_record& irfr, d3_array& effort, year_month& date, ivector& effort_occured);
	char *argv[10] = {"tagest\0"};
	model_parameters mp(350000, 1, argv);

	CPPUNIT_ASSERT(indexed_regional_fishery_record::instance == 0);
	mp.readData();

	CPPUNIT_ASSERT(indexed_regional_fishery_record::instance != 0);
	indexed_regional_fishery_record &irfr = *indexed_regional_fishery_record::instance;


	CPPUNIT_ASSERT(par_t::instance != 0);
	par_t_reg &param = *(par_t_reg*)par_t::instance;
	const int nfleet = param.get_nfleet();
	const int m = param.get_m();
	const ivector &jlb  = param.get_jlb();
	const ivector &jub  = param.get_jub();

        imatrix jlbm(1,nfleet,1,m);
        imatrix jubm(1,nfleet,1,m);
        ivector ilbv(1,nfleet);
        ivector iubv(1,nfleet);
        for (int s = 1; s <= nfleet; s++) {
                ilbv(s) = 1;
                iubv(s) = m;
                for (int i = 1; i <= m; i++) {
                        jlbm(s,i) = jlb(i);
                        jubm(s,i) = jub(i);
                }
        }
        d3_array effort(1, nfleet, ilbv, iubv, jlbm, jubm);
        ivector effort_occured(1, nfleet);

	const year_month &startDate = param.get_start_date();
	const int nrelease = param.get_nrelease();
	const int nmonth = param.get_nmonth();
	const year_month finalDate = param.get_tr_date(nrelease) + nmonth;
        for (year_month ym = startDate; ym <= finalDate; ym++) {
		get_effort_array(param, irfr, effort, ym, effort_occured);
	}
	mp.final_calcs();
}
void TestTagestAdmodel::testFishMortComp() {
	void get_effort_array(par_t& param, indexed_regional_fishery_record& irfr, d3_array& effort, year_month& date, ivector& effort_occured);

	char *argv[10] = {"tagest\0"};
	model_parameters mp(350000, 1, argv);

	CPPUNIT_ASSERT(indexed_regional_fishery_record::instance == 0);
	mp.readData();

	CPPUNIT_ASSERT(indexed_regional_fishery_record::instance != 0);
	indexed_regional_fishery_record &irfr = *indexed_regional_fishery_record::instance;


	CPPUNIT_ASSERT(par_t::instance != 0);
	par_t_reg &param = (par_t_reg&)*par_t::instance;
	const int nfleet = param.get_nfleet();
	const int m = param.get_m();
	const ivector &jlb  = param.get_jlb();
	const ivector &jub  = param.get_jub();

        imatrix jlbm(1,nfleet,1,m);
        imatrix jubm(1,nfleet,1,m);
        ivector ilbv(1,nfleet);
        ivector iubv(1,nfleet);
        for (int s = 1; s <= nfleet; s++) {
                ilbv(s) = 1;
                iubv(s) = m;
                for (int i = 1; i <= m; i++) {
                        jlbm(s,i) = jlb(i);
                        jubm(s,i) = jub(i);
                }
        }
        d3_array effort(1, nfleet, ilbv, iubv, jlbm, jubm);
        d3_array fish_mort(1, nfleet, ilbv, iubv, jlbm, jubm);
        dvar3_array variableFishMort(1, nfleet, ilbv, iubv, jlbm, jubm);
        ivector effort_occured(1, nfleet);

	const year_month &startDate = param.get_start_date();
	const int nrelease = param.get_nrelease();
	const int nmonth = param.get_nmonth();
	const year_month finalDate = param.get_tr_date(nrelease) + nmonth;
        for (year_month ym = startDate; ym <= finalDate; ym++) {
		get_effort_array(param, irfr, effort, ym, effort_occured);
		dvar_vector q(mp.getParameterQ());
		param.fish_mort_comp(variableFishMort, effort, q);
		param.fish_mort_comp(fish_mort, effort, ym);
		for (int fleet = 1; fleet <= nfleet; fleet++) {
			for (int i = 1; i <= m; i++) {
				const int jmin = jlb(i);
				const int jmax = jub(i);
				for (int j = jmin; j <= jmax; j++) {
					const double expected = fish_mort(fleet, i, j);
					const double v = value(variableFishMort(fleet, i, j));
					CPPUNIT_ASSERT(expected - 1e-5 <= v && v <= expected + 1e-5);
				}
			}
		}
	}
	mp.final_calcs();
}
void TestTagestAdmodel::testTotalMortComp() {
	void get_effort_array(par_t& param, indexed_regional_fishery_record& irfr, d3_array& effort, year_month& date, ivector& effort_occured);

	char *argv[10] = {"tagest\0"};
	model_parameters mp(350000, 1, argv);

	CPPUNIT_ASSERT(indexed_regional_fishery_record::instance == 0);
	mp.readData();

	CPPUNIT_ASSERT(indexed_regional_fishery_record::instance != 0);
	indexed_regional_fishery_record &irfr = *indexed_regional_fishery_record::instance;


	CPPUNIT_ASSERT(par_t::instance != 0);
	par_t_reg &param = (par_t_reg&)*par_t::instance;
	const int nfleet = param.get_nfleet();
	const int m = param.get_m();
	const ivector &jlb  = param.get_jlb();
	const ivector &jub  = param.get_jub();

        imatrix jlbm(1,nfleet,1,m);
        imatrix jubm(1,nfleet,1,m);
        ivector ilbv(1,nfleet);
        ivector iubv(1,nfleet);
        for (int s = 1; s <= nfleet; s++) {
                ilbv(s) = 1;
                iubv(s) = m;
                for (int i = 1; i <= m; i++) {
                        jlbm(s,i) = jlb(i);
                        jubm(s,i) = jub(i);
                }
        }
        d3_array effort(1, nfleet, ilbv, iubv, jlbm, jubm);
        d3_array fish_mort(1, nfleet, ilbv, iubv, jlbm, jubm);
        dmatrix total_mort(1, m, jlb, jub);
        dvar3_array variableFishMort(1, nfleet, ilbv, iubv, jlbm, jubm);
        dvar_matrix variableTotalMort(1, m, jlb, jub);
        ivector effort_occured(1, nfleet);

	const year_month &startDate = param.get_start_date();
	const int nrelease = param.get_nrelease();
	const int nmonth = param.get_nmonth();
	const year_month finalDate = param.get_tr_date(nrelease) + nmonth;
        for (year_month ym = startDate; ym <= finalDate; ym++) {
		get_effort_array(param, irfr, effort, ym, effort_occured);
		dvar_vector q(mp.getParameterQ());
		dvariable mort = mp.getParameterMort();
		dvariable special_mort = mp.getParameterSpecialMort();
		param.fish_mort_comp(variableFishMort, effort, q);
		param.total_mort_comp(variableFishMort, variableTotalMort, mort, special_mort);

		param.fish_mort_comp(fish_mort, effort, ym);
		param.total_mort_comp(fish_mort, total_mort);
		for (int i = 1; i <= m; i++) {
			const int jmin = jlb(i);
			const int jmax = jub(i);
			for (int j = jmin; j <= jmax; j++) {
				const double expected = total_mort(i, j);
				const double v = value(variableTotalMort(i, j));
				CPPUNIT_ASSERT(expected - 1e-5 <= v && v <= expected + 1e-5);
			}
		}
	}
	mp.final_calcs();
}
void TestTagestAdmodel::testCoffBAdjust() {
	void get_effort_array(par_t& param, indexed_regional_fishery_record& irfr, d3_array& effort, year_month& date, ivector& effort_occured);

	char *argv[10] = {"tagest\0"};
	CPPUNIT_ASSERT(par_t::instance == 0);
	model_parameters mp(350000, 1, argv);
	CPPUNIT_ASSERT(par_t::instance != 0);

	mp.readData();

	par_t_reg &param = (par_t_reg&)*par_t::instance;
	const int nfleet = param.get_nfleet();
	const int m = param.get_m();
	const int n = param.get_n();
	const ivector &ilb = param.get_ilb();
	const ivector &iub = param.get_iub();
	const ivector &jlb = param.get_jlb();
	const ivector &jub = param.get_jub();

	ivector jlb1(0,m+1);
	ivector jub1(0,m+1);
	for (int i = 1; i <= m; i++) {
		jlb1(i) = param.get_jlb(i) - 1;
		jub1(i) = param.get_jub(i) + 1;
	}
	jlb1(0) = 0;
	jlb1(m+1) = 0;
	jub1(0) = n+1;
	jub1(m+1) = n+1;
	dmatrix u(0, m+1, jlb1, jub1);
	dmatrix v(0, m+1, jlb1, jub1);
	dmatrix sigma(0, m+1, jlb1, jub1);
	dvar_matrix variableU(0, m+1, jlb1, jub1);
	dvar_matrix variableV(0, m+1, jlb1, jub1);
	dvar_matrix variableSigma(0, m+1, jlb1, jub1);

        imatrix jlbm(1,nfleet,1,m);
        imatrix jubm(1,nfleet,1,m);
        ivector ilbv(1,nfleet);
        ivector iubv(1,nfleet);
        for (int s = 1; s <= nfleet; s++) {
                ilbv(s) = 1;
                iubv(s) = m;
                for (int i = 1; i <= m; i++) {
                        jlbm(s,i) = jlb(i);
                        jubm(s,i) = jub(i);
                }
        }
        d3_array effort(1, nfleet, ilbv, iubv, jlbm, jubm);
        d3_array fish_mort(1, nfleet, ilbv, iubv, jlbm, jubm);
        dmatrix total_mort(1, m, jlb, jub);
        dvar3_array variableFishMort(1, nfleet, ilbv, iubv, jlbm, jubm);
        dvar_matrix variableTotalMort(1, m, jlb, jub);
        ivector effort_occured(1, nfleet);

	CPPUNIT_ASSERT(coff_t::instance != 0);
	coff_t &coff= (coff_t&)*coff_t::instance;
	CPPUNIT_ASSERT(indexed_regional_fishery_record::instance != 0);
	indexed_regional_fishery_record &irfr = *indexed_regional_fishery_record::instance;

	const year_month &startDate = param.get_start_date();
	const int nrelease = param.get_nrelease();
	const int nmonth = param.get_nmonth();
	const year_month finalDate = param.get_tr_date(nrelease) + nmonth;
        for (year_month ym = startDate; ym <= finalDate; ym++) {
		const int season = param.get_season(ym);
		param.uvs_comp(u, v, sigma, ym);
		dvar_matrix uSeasonGrid(mp.getParameterU());
		dvar_matrix vSeasonGrid(mp.getParameterV());
		dvar_matrix sigmaSeasonGrid(mp.getParameterSigma());
		param.uvs_comp(uSeasonGrid, vSeasonGrid, sigmaSeasonGrid, variableU, variableV, variableSigma, season);
		coff.clean();
		coff.comp(param, ym, 0, u, v, sigma);
		coff.comp(param, ym, variableU, variableV, variableSigma);

		dmatrix &raw_b = coff.get_raw_b();
		dvar_matrix &variableRawB = coff.getVariableRawB();

		get_effort_array(param, irfr, effort, ym, effort_occured);
		fish_mort.initialize();
		param.fish_mort_comp(fish_mort, effort, ym);
		total_mort.initialize();
		param.total_mort_comp(fish_mort, total_mort);

		dvar_vector q(mp.getParameterQ());
		dvariable mort = mp.getParameterMort();
		dvariable special_mort = mp.getParameterSpecialMort();
		variableFishMort.initialize();
		param.fish_mort_comp(variableFishMort, effort, q);
		variableTotalMort.initialize();
		param.total_mort_comp(variableFishMort, variableTotalMort, mort, special_mort);

		dmatrix &b = coff.get_b();
		dvar_matrix &variableB = coff.getVariableB();
		b.initialize();
		variableB.initialize();
		coff.b_adjust(param, total_mort);
		coff.b_adjust(param, variableTotalMort);
		CPPUNIT_ASSERT(isValuesEqual(sum(total_mort), sum(value(variableTotalMort))));
		for (int j = 1; j <= n; j++) {
			const int imin = ilb(j);
			const int imax = iub(j);
			for (int i = imin; i <= imax; i++) {
				const double expected = raw_b(j, i);
				const double v = value(variableRawB(j, i));
				CPPUNIT_ASSERT(expected - 1e-5 <= v && v <= expected + 1e-5);
				CPPUNIT_ASSERT(isValuesEqual(raw_b(j,i), value(variableRawB(j, i))));
				CPPUNIT_ASSERT(isValuesEqual(b(j,i), value(variableB(j, i))));
			}
		}
	}
	mp.final_calcs();
}
void TestTagestAdmodel::testCoffAdi() {
	void get_effort_array(par_t& param, indexed_regional_fishery_record& irfr, d3_array& effort, year_month& date, ivector& effort_occured);
	extern int saveinterflag;
	saveinterflag = 0;
	char *argv[10] = {"tagest\0"};
	CPPUNIT_ASSERT(par_t::instance == 0);
	model_parameters mp(450000, 1, argv);
	CPPUNIT_ASSERT(par_t::instance != 0);

	mp.readData();

	par_t_reg &param = (par_t_reg&)*par_t::instance;
	const int nfleet = param.get_nfleet();
	const int m = param.get_m();
	const int n = param.get_n();
	const ivector &ilb = param.get_ilb();
	const ivector &iub = param.get_iub();
	const ivector &jlb = param.get_jlb();
	const ivector &jub = param.get_jub();

	dmatrix release(1, m, jlb, jub);
	dvar_matrix variableRelease(1, m, jlb, jub);
	dvar_matrix dvVariableRelease(1, m, jlb, jub);
	release.initialize();
	variableRelease.initialize();
	dvVariableRelease.initialize();

	ivector jlb1(0,m+1);
	ivector jub1(0,m+1);
	for (int i = 1; i <= m; i++) {
		jlb1(i) = param.get_jlb(i) - 1;
		jub1(i) = param.get_jub(i) + 1;
	}
	jlb1(0) = 0;
	jlb1(m+1) = 0;
	jub1(0) = n+1;
	jub1(m+1) = n+1;
	dmatrix u(0, m+1, jlb1, jub1);
	dmatrix v(0, m+1, jlb1, jub1);
	dmatrix sigma(0, m+1, jlb1, jub1);
	dvar_matrix variableU(0, m+1, jlb1, jub1);
	dvar_matrix variableV(0, m+1, jlb1, jub1);
	dvar_matrix variableSigma(0, m+1, jlb1, jub1);

        imatrix jlbm(1,nfleet,1,m);
        imatrix jubm(1,nfleet,1,m);
        ivector ilbv(1,nfleet);
        ivector iubv(1,nfleet);
        for (int s = 1; s <= nfleet; s++) {
                ilbv(s) = 1;
                iubv(s) = m;
                for (int i = 1; i <= m; i++) {
                        jlbm(s,i) = jlb(i);
                        jubm(s,i) = jub(i);
                }
        }
        d3_array effort(1, nfleet, ilbv, iubv, jlbm, jubm);
        d3_array fish_mort(1, nfleet, ilbv, iubv, jlbm, jubm);
        dmatrix total_mort(1, m, jlb, jub);
        dvar3_array variableFishMort(1, nfleet, ilbv, iubv, jlbm, jubm);
        dvar_matrix variableTotalMort(1, m, jlb, jub);
        ivector effort_occured(1, nfleet);

	CPPUNIT_ASSERT(coff_t::instance != 0);
	coff_t &coff= (coff_t&)*coff_t::instance;
	CPPUNIT_ASSERT(indexed_regional_fishery_record::instance != 0);
	indexed_regional_fishery_record &irfr = *indexed_regional_fishery_record::instance;

	int r = 1;
	const year_month &startDate = param.get_start_date();
	const int nrelease = param.get_nrelease(); 
	const int nmonth = param.get_nmonth();
	const year_month finalDate = param.get_tr_date(nrelease) + nmonth;
        for (year_month ym = startDate; ym <= finalDate; ym++) {
		const int season = param.get_season(ym);
		param.uvs_comp(u, v, sigma, ym);
		dvar_matrix uSeasonGrid(mp.getParameterU());
		dvar_matrix vSeasonGrid(mp.getParameterV());
		dvar_matrix sigmaSeasonGrid(mp.getParameterSigma());
		param.uvs_comp(uSeasonGrid, vSeasonGrid, sigmaSeasonGrid, variableU, variableV, variableSigma, season);
		coff.clean();
		coff.comp(param, ym, 0, u, v, sigma);
		coff.comp(param, ym, variableU, variableV, variableSigma);

		dmatrix &raw_b = coff.get_raw_b();
		dvar_matrix &variableRawB = coff.getVariableRawB();

		get_effort_array(param, irfr, effort, ym, effort_occured);
		fish_mort.initialize();
		param.fish_mort_comp(fish_mort, effort, ym);
		total_mort.initialize();
		param.total_mort_comp(fish_mort, total_mort);

		dvar_vector q(mp.getParameterQ());
		dvariable mort = mp.getParameterMort();
		dvariable special_mort = mp.getParameterSpecialMort();
		variableFishMort.initialize();
		param.fish_mort_comp(variableFishMort, effort, q);
		variableTotalMort.initialize();
		param.total_mort_comp(variableFishMort, variableTotalMort, mort, special_mort);

		dmatrix &b = coff.get_b();
		dvar_matrix &variableB = coff.getVariableB();
		b.initialize();
		variableB.initialize();
		coff.b_adjust(param, total_mort);
		coff.b_adjust(param, variableTotalMort);

		while (r < nrelease && ym == param.get_tr_date(r)) {
			const int i = param.get_tr_i(r);
			const int j = param.get_tr_j(r);
			double total = param.get_tr_tags(r);
			CPPUNIT_ASSERT(total > 0);
			release(i,j) += total;
			variableRelease(i,j) += total;
			dvVariableRelease(i,j) += total;
			r++;
		}

		coff.adi(release, 0);
		coff.adi(variableRelease, param);
		//coff.adi(dvVariableRelease, param, true);
		for (int i = 1; i <= m; i++) {
			const int jmin = jlb(i);
			const int jmax = jub(i);
			for (int j = jmin; j <= jmax; j++) {
				CPPUNIT_ASSERT(isValuesEqual(release(i,j), value(variableRelease(i,j))));
				CPPUNIT_ASSERT(isValuesEqual(release(i,j), value(dvVariableRelease(i,j))));
			}
		}
	}
	mp.final_calcs();
}
void TestTagestAdmodel::testCoffDfAdiWrapper() {
	void get_effort_array(par_t& param, indexed_regional_fishery_record& irfr, d3_array& effort, year_month& date, ivector& effort_occured);
	void adi(const par_t &param, coff_t& coff, dvar_matrix &releases, intersavetype *savefile);
	extern int saveinterflag;
	extern setsaveinterflag interflag;
	extern intersavetype *isp;

	saveinterflag = 1;
	interflag.setsaveinter(1);
	if ((isp->savefile = open(isp->savefilename, O_RDWR | O_CREAT | O_TRUNC , 0660)) < 0) {
		cerr <<"Cannot open " << isp->savefilename <<" for output.\n";
		CPPUNIT_ASSERT(false);
	}
	char *argv[10] = {"tagest\0"};
	CPPUNIT_ASSERT(par_t::instance == 0);
	model_parameters mp(600000, 1, argv);
	CPPUNIT_ASSERT(par_t::instance != 0);

	mp.readData();

	par_t_reg &param = (par_t_reg&)*par_t::instance;
	const int nfleet = param.get_nfleet();
	const int m = param.get_m();
	const int n = param.get_n();
	const ivector &ilb = param.get_ilb();
	const ivector &iub = param.get_iub();
	const ivector &jlb = param.get_jlb();
	const ivector &jub = param.get_jub();

	dmatrix release(1, m, jlb, jub);
	dmatrix dfrelease(1, m, jlb, jub);
	dvar_matrix variableRelease(1, m, jlb, jub);
	dvar_matrix dvVariableRelease(1, m, jlb, jub);
	dmatrix u2;
	dmatrix dfu2;
	u2.allocate(release);
	dfu2.allocate(release);
	dfu2.initialize();
	release.initialize();
	dfrelease.initialize();
	variableRelease.initialize();
	dvVariableRelease.initialize();

	ivector jlb1(0,m+1);
	ivector jub1(0,m+1);
	for (int i = 1; i <= m; i++) {
		jlb1(i) = param.get_jlb(i) - 1;
		jub1(i) = param.get_jub(i) + 1;
	}
	jlb1(0) = 0;
	jlb1(m+1) = 0;
	jub1(0) = n+1;
	jub1(m+1) = n+1;
	dmatrix u(0, m+1, jlb1, jub1);
	dmatrix v(0, m+1, jlb1, jub1);
	dmatrix sigma(0, m+1, jlb1, jub1);
	dvar_matrix variableU(0, m+1, jlb1, jub1);
	dvar_matrix variableV(0, m+1, jlb1, jub1);
	dvar_matrix variableSigma(0, m+1, jlb1, jub1);

        imatrix jlbm(1,nfleet,1,m);
        imatrix jubm(1,nfleet,1,m);
        ivector ilbv(1,nfleet);
        ivector iubv(1,nfleet);
        for (int s = 1; s <= nfleet; s++) {
                ilbv(s) = 1;
                iubv(s) = m;
                for (int i = 1; i <= m; i++) {
                        jlbm(s,i) = jlb(i);
                        jubm(s,i) = jub(i);
                }
        }
        d3_array effort(1, nfleet, ilbv, iubv, jlbm, jubm);
        d3_array fish_mort(1, nfleet, ilbv, iubv, jlbm, jubm);
        dmatrix total_mort(1, m, jlb, jub);
        dvar3_array variableFishMort(1, nfleet, ilbv, iubv, jlbm, jubm);
        dvar_matrix variableTotalMort(1, m, jlb, jub);
        ivector effort_occured(1, nfleet);

	CPPUNIT_ASSERT(coff_t::instance != 0);
	coff_t &coff= (coff_t&)*coff_t::instance;
	CPPUNIT_ASSERT(indexed_regional_fishery_record::instance != 0);
	indexed_regional_fishery_record &irfr = *indexed_regional_fishery_record::instance;

	Upwind_coff_t dfcoff(param, param.m_ipar(6));

	int r = 1;
	const year_month &startDate = param.get_start_date();
	const int nrelease = param.get_nrelease(); 
	const int nmonth = param.get_nmonth();
	const year_month finalDate = param.get_tr_date(nrelease) + nmonth;
        for (year_month ym = startDate; ym <= finalDate; ym++) {
		interflag.isp_reset();

		const int season = param.get_season(ym);
		param.uvs_comp(u, v, sigma, ym);
		dvar_matrix uSeasonGrid(mp.getParameterU());
		dvar_matrix vSeasonGrid(mp.getParameterV());
		dvar_matrix sigmaSeasonGrid(mp.getParameterSigma());
		param.uvs_comp(uSeasonGrid, vSeasonGrid, sigmaSeasonGrid, variableU, variableV, variableSigma, season);
		coff.clean();
		coff.comp(param, ym, isp, u, v, sigma);
		coff.comp(param, ym, variableU, variableV, variableSigma);

		dmatrix &raw_b = coff.get_raw_b();
		dvar_matrix &variableRawB = coff.getVariableRawB();

		get_effort_array(param, irfr, effort, ym, effort_occured);
		fish_mort.initialize();
		param.fish_mort_comp(fish_mort, effort, ym);
		total_mort.initialize();
		param.total_mort_comp(fish_mort, total_mort);

		dvar_vector q(mp.getParameterQ());
		dvariable mort = mp.getParameterMort();
		dvariable special_mort = mp.getParameterSpecialMort();
		variableFishMort.initialize();
		param.fish_mort_comp(variableFishMort, effort, q);
		variableTotalMort.initialize();
		param.total_mort_comp(variableFishMort, variableTotalMort, mort, special_mort);

		dmatrix &b = coff.get_b();
		dvar_matrix &variableB = coff.getVariableB();
		b.initialize();
		variableB.initialize();
		coff.b_adjust(param, total_mort);
		coff.b_adjust(param, variableTotalMort);

		while (r < nrelease && ym == param.get_tr_date(r)) {
			const int i = param.get_tr_i(r);
			const int j = param.get_tr_j(r);
			double total = param.get_tr_tags(r);
			CPPUNIT_ASSERT(total > 0);
			release(i,j) += total;
			variableRelease(i,j) += total;
			dvVariableRelease(i,j) += total;
			r++;
		}

		const double sumRelease = sum(release);

		coff.adi(release, isp);
		//coff.adi(variableRelease, param);
		//adi(param, coff, dvVariableRelease, isp);
		interflag.iclose(isp);
		for (int i = 1; i <= m; i++) {
			const int jmin = jlb(i);
			const int jmax = jub(i);
			for (int j = jmin; j <= jmax; j++) {
				CPPUNIT_ASSERT(isValuesEqual(release(i,j), value(variableRelease(i,j))));
				CPPUNIT_ASSERT(isValuesEqual(release(i,j), value(dvVariableRelease(i,j))));
			}
		}
		//slave_gradcalc();
/*
		u2.initialize();
		dfcoff.clean();
		dfcoff.dfadi(coff, u2, dfu2, isp);
		CPPUNIT_ASSERT(isValuesEqual(sumRelease, sum(u2)));
		dmatrix &dfaExpected = dfcoff.get_a();
		dmatrix &dfbExpected = dfcoff.get_b();
		dmatrix &dfcExpected = dfcoff.get_c();
		dmatrix &dfdExpected = dfcoff.get_d();
		dmatrix &dfeExpected = dfcoff.get_e();
		dmatrix &dffExpected = dfcoff.get_f();
		CPPUNIT_ASSERT(isValuesEqual(sum(dfaExpected), 0));
		CPPUNIT_ASSERT(isValuesEqual(sum(dfbExpected), 0));
		CPPUNIT_ASSERT(isValuesEqual(sum(dfcExpected), 0));
		CPPUNIT_ASSERT(isValuesEqual(sum(dfdExpected), 0));
		CPPUNIT_ASSERT(isValuesEqual(sum(dfeExpected), 0));
		CPPUNIT_ASSERT(isValuesEqual(sum(dffExpected), 0));
		for (int j = 1; j <= n; j++) {
			const int imin = ilb(j);
			const int imax = iub(j);
			for (int i = imin; i <= imax; i++) {
				//cout << dfaExpected(j,i) << ' ';
			}
			//cout << endl;
		}
		for (int i = 1; i <= m; i++) {
			const int jmin = jlb(i);
			const int jmax = jub(i);
			for (int j = jmin; j <= jmax; j++) {
				//cout << dfu2(i,j) << ' ';
				//CPPUNIT_ASSERT(isValuesEqual(sumRelease, dfu2(i,j)));
			}
			//cout << endl;
		}
*/
	}
	mp.final_calcs();
	close(isp->savefile);
	interflag.setsaveinter(0);
}
void TestTagestAdmodel::testNegativeTagPenalty() {
	int negative_tag_penalty(dmatrix& tags, par_t& param, double& pen);
	dvariable negative_tag_penalty(const dvar_matrix &tags, const par_t& param);
	void get_effort_array(par_t& param, indexed_regional_fishery_record& irfr, d3_array& effort, year_month& date, ivector& effort_occured);
	extern int saveinterflag;

	saveinterflag = 0;
	char *argv[10] = {"tagest\0"};
	CPPUNIT_ASSERT(par_t::instance == 0);
	model_parameters mp(350000, 1, argv);
	CPPUNIT_ASSERT(par_t::instance != 0);

	mp.readData();

	par_t_reg &param = (par_t_reg&)*par_t::instance;
	const int nfleet = param.get_nfleet();
	const int m = param.get_m();
	const int n = param.get_n();
	const ivector &ilb = param.get_ilb();
	const ivector &iub = param.get_iub();
	const ivector &jlb = param.get_jlb();
	const ivector &jub = param.get_jub();

	dmatrix release(1, m, jlb, jub);
	dvar_matrix variableRelease(1, m, jlb, jub);
	release.initialize();
	variableRelease.initialize();

	ivector jlb1(0,m+1);
	ivector jub1(0,m+1);
	for (int i = 1; i <= m; i++) {
		jlb1(i) = param.get_jlb(i) - 1;
		jub1(i) = param.get_jub(i) + 1;
	}
	jlb1(0) = 0;
	jlb1(m+1) = 0;
	jub1(0) = n+1;
	jub1(m+1) = n+1;
	dmatrix u(0, m+1, jlb1, jub1);
	dmatrix v(0, m+1, jlb1, jub1);
	dmatrix sigma(0, m+1, jlb1, jub1);
	dvar_matrix variableU(0, m+1, jlb1, jub1);
	dvar_matrix variableV(0, m+1, jlb1, jub1);
	dvar_matrix variableSigma(0, m+1, jlb1, jub1);

        imatrix jlbm(1,nfleet,1,m);
        imatrix jubm(1,nfleet,1,m);
        ivector ilbv(1,nfleet);
        ivector iubv(1,nfleet);
        for (int s = 1; s <= nfleet; s++) {
                ilbv(s) = 1;
                iubv(s) = m;
                for (int i = 1; i <= m; i++) {
                        jlbm(s,i) = jlb(i);
                        jubm(s,i) = jub(i);
                }
        }
        d3_array effort(1, nfleet, ilbv, iubv, jlbm, jubm);
        d3_array fish_mort(1, nfleet, ilbv, iubv, jlbm, jubm);
        dmatrix total_mort(1, m, jlb, jub);
        dvar3_array variableFishMort(1, nfleet, ilbv, iubv, jlbm, jubm);
        dvar_matrix variableTotalMort(1, m, jlb, jub);
        ivector effort_occured(1, nfleet);

	CPPUNIT_ASSERT(coff_t::instance != 0);
	coff_t &coff= (coff_t&)*coff_t::instance;
	CPPUNIT_ASSERT(indexed_regional_fishery_record::instance != 0);
	indexed_regional_fishery_record &irfr = *indexed_regional_fishery_record::instance;

	int r = 1;
	const year_month &startDate = param.get_start_date();
	const int nrelease = param.get_nrelease(); 
	const int nmonth = param.get_nmonth();
	const year_month finalDate = param.get_tr_date(nrelease) + nmonth;
        for (year_month ym = startDate; ym <= finalDate; ym++) {
		const int season = param.get_season(ym);
		param.uvs_comp(u, v, sigma, ym);
		dvar_matrix uSeasonGrid(mp.getParameterU());
		dvar_matrix vSeasonGrid(mp.getParameterV());
		dvar_matrix sigmaSeasonGrid(mp.getParameterSigma());
		param.uvs_comp(uSeasonGrid, vSeasonGrid, sigmaSeasonGrid, variableU, variableV, variableSigma, season);
		coff.clean();
		coff.comp(param, ym, 0, u, v, sigma);
		coff.comp(param, ym, variableU, variableV, variableSigma);

		dmatrix &raw_b = coff.get_raw_b();
		dvar_matrix &variableRawB = coff.getVariableRawB();

		get_effort_array(param, irfr, effort, ym, effort_occured);
		fish_mort.initialize();
		param.fish_mort_comp(fish_mort, effort, ym);
		total_mort.initialize();
		param.total_mort_comp(fish_mort, total_mort);

		dvar_vector q(mp.getParameterQ());
		dvariable mort = mp.getParameterMort();
		dvariable special_mort = mp.getParameterSpecialMort();
		variableFishMort.initialize();
		param.fish_mort_comp(variableFishMort, effort, q);
		variableTotalMort.initialize();
		param.total_mort_comp(variableFishMort, variableTotalMort, mort, special_mort);

		dmatrix &b = coff.get_b();
		dvar_matrix &variableB = coff.getVariableB();
		b.initialize();
		variableB.initialize();
		coff.b_adjust(param, total_mort);
		coff.b_adjust(param, variableTotalMort);

		while (r < nrelease && ym == param.get_tr_date(r)) {
			const int i = param.get_tr_i(r);
			const int j = param.get_tr_j(r);
			double total = param.get_tr_tags(r);
			CPPUNIT_ASSERT(total > 0);
			release(i,j) += total;
			variableRelease(i,j) += total;
			r++;
		}

		coff.adi(release, 0);
		coff.adi(variableRelease, param);
		release *= -1;
		variableRelease *= -1;
		double penalty = 0;
		negative_tag_penalty(release, param, penalty);
		dvariable variablePenalty = negative_tag_penalty(release, param);

		CPPUNIT_ASSERT(isValuesEqual(penalty, value(variablePenalty)));
	}
	mp.final_calcs();
}
void TestTagestAdmodel::testTotalTagPenalty() {
	void total_tag_penalty(dmatrix& tags, par_t& param, double& pen);
	dvariable total_tag_penalty(const dvar_matrix &tags, const par_t &param);
	void get_effort_array(par_t& param, indexed_regional_fishery_record& irfr, d3_array& effort, year_month& date, ivector& effort_occured);
	extern int saveinterflag;

	saveinterflag = 0;
	char *argv[10] = {"tagest\0"};
	CPPUNIT_ASSERT(par_t::instance == 0);
	model_parameters mp(350000, 1, argv);
	CPPUNIT_ASSERT(par_t::instance != 0);

	mp.readData();

	par_t_reg &param = (par_t_reg&)*par_t::instance;
	const int nfleet = param.get_nfleet();
	const int m = param.get_m();
	const int n = param.get_n();
	const ivector &ilb = param.get_ilb();
	const ivector &iub = param.get_iub();
	const ivector &jlb = param.get_jlb();
	const ivector &jub = param.get_jub();

	dmatrix release(1, m, jlb, jub);
	dvar_matrix variableRelease(1, m, jlb, jub);
	release.initialize();
	variableRelease.initialize();

	ivector jlb1(0,m+1);
	ivector jub1(0,m+1);
	for (int i = 1; i <= m; i++) {
		jlb1(i) = param.get_jlb(i) - 1;
		jub1(i) = param.get_jub(i) + 1;
	}
	jlb1(0) = 0;
	jlb1(m+1) = 0;
	jub1(0) = n+1;
	jub1(m+1) = n+1;
	dmatrix u(0, m+1, jlb1, jub1);
	dmatrix v(0, m+1, jlb1, jub1);
	dmatrix sigma(0, m+1, jlb1, jub1);
	dvar_matrix variableU(0, m+1, jlb1, jub1);
	dvar_matrix variableV(0, m+1, jlb1, jub1);
	dvar_matrix variableSigma(0, m+1, jlb1, jub1);

        imatrix jlbm(1,nfleet,1,m);
        imatrix jubm(1,nfleet,1,m);
        ivector ilbv(1,nfleet);
        ivector iubv(1,nfleet);
        for (int s = 1; s <= nfleet; s++) {
                ilbv(s) = 1;
                iubv(s) = m;
                for (int i = 1; i <= m; i++) {
                        jlbm(s,i) = jlb(i);
                        jubm(s,i) = jub(i);
                }
        }
        d3_array effort(1, nfleet, ilbv, iubv, jlbm, jubm);
        d3_array fish_mort(1, nfleet, ilbv, iubv, jlbm, jubm);
        dmatrix total_mort(1, m, jlb, jub);
        dvar3_array variableFishMort(1, nfleet, ilbv, iubv, jlbm, jubm);
        dvar_matrix variableTotalMort(1, m, jlb, jub);
        ivector effort_occured(1, nfleet);

	CPPUNIT_ASSERT(coff_t::instance != 0);
	coff_t &coff= (coff_t&)*coff_t::instance;
	CPPUNIT_ASSERT(indexed_regional_fishery_record::instance != 0);
	indexed_regional_fishery_record &irfr = *indexed_regional_fishery_record::instance;

	int r = 1;
	const year_month &startDate = param.get_start_date();
	const int nrelease = param.get_nrelease(); 
	const int nmonth = param.get_nmonth();
	const year_month finalDate = param.get_tr_date(nrelease) + nmonth;
        for (year_month ym = startDate; ym <= finalDate; ym++) {
		const int season = param.get_season(ym);
		param.uvs_comp(u, v, sigma, ym);
		dvar_matrix uSeasonGrid(mp.getParameterU());
		dvar_matrix vSeasonGrid(mp.getParameterV());
		dvar_matrix sigmaSeasonGrid(mp.getParameterSigma());
		param.uvs_comp(uSeasonGrid, vSeasonGrid, sigmaSeasonGrid, variableU, variableV, variableSigma, season);
		coff.clean();
		coff.comp(param, ym, 0, u, v, sigma);
		coff.comp(param, ym, variableU, variableV, variableSigma);

		dmatrix &raw_b = coff.get_raw_b();
		dvar_matrix &variableRawB = coff.getVariableRawB();

		get_effort_array(param, irfr, effort, ym, effort_occured);
		fish_mort.initialize();
		param.fish_mort_comp(fish_mort, effort, ym);
		total_mort.initialize();
		param.total_mort_comp(fish_mort, total_mort);

		dvar_vector q(mp.getParameterQ());
		dvariable mort = mp.getParameterMort();
		dvariable special_mort = mp.getParameterSpecialMort();
		variableFishMort.initialize();
		param.fish_mort_comp(variableFishMort, effort, q);
		variableTotalMort.initialize();
		param.total_mort_comp(variableFishMort, variableTotalMort, mort, special_mort);

		dmatrix &b = coff.get_b();
		dvar_matrix &variableB = coff.getVariableB();
		b.initialize();
		variableB.initialize();
		coff.b_adjust(param, total_mort);
		coff.b_adjust(param, variableTotalMort);

		while (r < nrelease && ym == param.get_tr_date(r)) {
			const int i = param.get_tr_i(r);
			const int j = param.get_tr_j(r);
			double total = param.get_tr_tags(r);
			CPPUNIT_ASSERT(total > 0);
			release(i,j) += total;
			variableRelease(i,j) += total;
			r++;
		}

		coff.adi(release, 0);
		coff.adi(variableRelease, param);
		release *= -1;
		variableRelease *= -1;
		double penalty = 0;
		total_tag_penalty(release, param, penalty);
		dvariable variablePenalty = total_tag_penalty(release, param);

		CPPUNIT_ASSERT(isValuesEqual(penalty, value(variablePenalty)));
	}
	mp.final_calcs();
}
void TestTagestAdmodel::testPoisson() {
	void poisson_like(double& like, dmatrix& pred_tags, year_month& date, int cohort, recaptype_vector& recaps, int nrec, par_t& param, dmatrix& z, ivector& effort_occured, d3_array& fmort);
	void poisson_like(dvariable &likelihood, const dvar_matrix& pred_tags, const year_month& date, const int cohort, const recaptype_vector& recaps, const int nrec, const par_t& param, const dvar_matrix &z, const ivector &effort_occured, const dvar3_array &fmort, dvar_vector &report_rates);
	void get_effort_array(par_t& param, indexed_regional_fishery_record& irfr, d3_array& effort, year_month& date, ivector& effort_occured);
	extern int saveinterflag;

	saveinterflag = 0;
	char *argv[10] = {"tagest\0"};
	CPPUNIT_ASSERT(par_t::instance == 0);
	model_parameters mp(550000, 1, argv);
	CPPUNIT_ASSERT(par_t::instance != 0);

	mp.readData();
	CPPUNIT_ASSERT(recaptype_vector::instance != 0);
	recaptype_vector &recaps = *recaptype_vector::instance;

	par_t_reg &param = (par_t_reg&)*par_t::instance;
	const int nfleet = param.get_nfleet();
	const int m = param.get_m();
	const int n = param.get_n();
	const ivector &ilb = param.get_ilb();
	const ivector &iub = param.get_iub();
	const ivector &jlb = param.get_jlb();
	const ivector &jub = param.get_jub();

	dmatrix release(1, m, jlb, jub);
	dvar_matrix variableRelease(1, m, jlb, jub);
	release.initialize();
	variableRelease.initialize();

	ivector jlb1(0,m+1);
	ivector jub1(0,m+1);
	for (int i = 1; i <= m; i++) {
		jlb1(i) = param.get_jlb(i) - 1;
		jub1(i) = param.get_jub(i) + 1;
	}
	jlb1(0) = 0;
	jlb1(m+1) = 0;
	jub1(0) = n+1;
	jub1(m+1) = n+1;
	dmatrix u(0, m+1, jlb1, jub1);
	dmatrix v(0, m+1, jlb1, jub1);
	dmatrix sigma(0, m+1, jlb1, jub1);
	dvar_matrix variableU(0, m+1, jlb1, jub1);
	dvar_matrix variableV(0, m+1, jlb1, jub1);
	dvar_matrix variableSigma(0, m+1, jlb1, jub1);

        imatrix jlbm(1,nfleet,1,m);
        imatrix jubm(1,nfleet,1,m);
        ivector ilbv(1,nfleet);
        ivector iubv(1,nfleet);
        for (int s = 1; s <= nfleet; s++) {
                ilbv(s) = 1;
                iubv(s) = m;
                for (int i = 1; i <= m; i++) {
                        jlbm(s,i) = jlb(i);
                        jubm(s,i) = jub(i);
                }
        }
        d3_array effort(1, nfleet, ilbv, iubv, jlbm, jubm);
        d3_array fish_mort(1, nfleet, ilbv, iubv, jlbm, jubm);
        dmatrix total_mort(1, m, jlb, jub);
        dvar3_array variableFishMort(1, nfleet, ilbv, iubv, jlbm, jubm);
        dvar_matrix variableTotalMort(1, m, jlb, jub);
        ivector effort_occured(1, nfleet);

	CPPUNIT_ASSERT(coff_t::instance != 0);
	coff_t &coff= (coff_t&)*coff_t::instance;
	CPPUNIT_ASSERT(indexed_regional_fishery_record::instance != 0);
	indexed_regional_fishery_record &irfr = *indexed_regional_fishery_record::instance;

	int r = 1;
	const year_month &startDate = param.get_start_date();
	const int nrelease = param.get_nrelease(); 
	const int nmonth = param.get_nmonth();
	const year_month finalDate = param.get_tr_date(nrelease) + nmonth;
        for (year_month ym = startDate; ym <= finalDate; ym++) {
		const int season = param.get_season(ym);
		param.uvs_comp(u, v, sigma, ym);
		dvar_matrix uSeasonGrid(mp.getParameterU());
		dvar_matrix vSeasonGrid(mp.getParameterV());
		dvar_matrix sigmaSeasonGrid(mp.getParameterSigma());
		param.uvs_comp(uSeasonGrid, vSeasonGrid, sigmaSeasonGrid, variableU, variableV, variableSigma, season);
		coff.clean();
		coff.comp(param, ym, 0, u, v, sigma);
		coff.comp(param, ym, variableU, variableV, variableSigma);

		dmatrix &raw_b = coff.get_raw_b();
		dvar_matrix &variableRawB = coff.getVariableRawB();

		get_effort_array(param, irfr, effort, ym, effort_occured);
		fish_mort.initialize();
		param.fish_mort_comp(fish_mort, effort, ym);
		total_mort.initialize();
		param.total_mort_comp(fish_mort, total_mort);

		variableFishMort.initialize();
		dvar_vector q(mp.getParameterQ());
		dvariable mort = mp.getParameterMort();
		dvariable special_mort = mp.getParameterSpecialMort();
		param.fish_mort_comp(variableFishMort, effort, q);
		variableTotalMort.initialize();
		param.total_mort_comp(variableFishMort, variableTotalMort, mort, special_mort);

		dmatrix &b = coff.get_b();
		dvar_matrix &variableB = coff.getVariableB();
		b.initialize();
		variableB.initialize();
		coff.b_adjust(param, total_mort);
		coff.b_adjust(param, variableTotalMort);

		while (r < nrelease && ym == param.get_tr_date(r)) {
			const int i = param.get_tr_i(r);
			const int j = param.get_tr_j(r);
			double total = param.get_tr_tags(r);
			CPPUNIT_ASSERT(total > 0);
			release(i,j) += total;
			variableRelease(i,j) += total;
			r++;
		}

		coff.adi(release, 0);
		coff.adi(variableRelease, param);
		double likelihood = 0;
		poisson_like(likelihood, release, ym, 0, recaps, 4211, param, total_mort, effort_occured, fish_mort);
		dvar_vector report_rates(mp.getParameterReportRates());
		dvariable variableLikelihood = 0;
		poisson_like(variableLikelihood, release, ym, 0, recaps, 4211, param, total_mort, effort_occured, fish_mort, report_rates);
		CPPUNIT_ASSERT(isValuesEqual(likelihood, value(variableLikelihood)));
	}
	mp.final_calcs();
}
void TestTagestAdmodel::testModelParametersComputeObjectiveFunctionValue() {
	char *argv[10] = {"tagest\0"};
	model_parameters mp(550000, 1, argv);
	mp.preliminary_calculations();

	double expectedLikelihood = 0; {
		indexed_regional_fishery_record &irfr = *indexed_regional_fishery_record::instance;
		par_t_reg &param = (par_t_reg&)*par_t::instance;
		recaptype_vector &recaps = *recaptype_vector::instance;
		coff_t &coff= (coff_t&)*coff_t::instance;
		Upwind_coff_t dfcoff(param, param.m_ipar[6]);
		coff.clean();
		dfcoff.clean();
		const int totalVariables = param.nvarcalc();
		adstring_array names(1,totalVariables);
		dvector x(1, totalVariables);
		x.initialize();
		param.xinit(x, names);
		dvector g(1, totalVariables);
		g.initialize();
		extern setsaveinterflag interflag;
		interflag.setsaveinter(1);
		param.fgcomp(expectedLikelihood, x, g, totalVariables, recaps, 4211, coff, dfcoff, irfr);
		interflag.setsaveinter(0);  // free buffer
	} 
	const double likelihood = value(mp.computeObjectiveFunctionValue());
	mp.final_calcs();
	CPPUNIT_ASSERT(isValuesEqual(likelihood, expectedLikelihood));
}
//End:------
//Begin:------
void TestTagestAdmodel::testModelParametersComputations() {
	char *argv[10] = {"tagest\0"};
	model_parameters mp(350000, 1, argv);
	mp.iprint=10;
	mp.preliminary_calculations();
	mp.computations(1, argv);
	CPPUNIT_ASSERT(par_t::instance == 0);
	CPPUNIT_ASSERT(coff_t::instance == 0);
	CPPUNIT_ASSERT(indexed_regional_fishery_record::instance == 0);
	CPPUNIT_ASSERT(recaptype_vector::instance == 0);
	mp.final_calcs();
}
int tagest(int argc,char * argv[]);
void TestTagestAdmodel::testTagest() {
	char *argv[10] = {"tagest\0"};
	tagest(1, argv);
}
//End:------
//Begin:------
void TestTagestAdmodel::testCheckParametersEqual(model_parameters &mp, const par_t_reg &param) {
	const double mort = mp.getParameterMort();
	CPPUNIT_ASSERT(mort - 1e-5 <= param.mort && param.mort <= mort + 1e-5);

	const double special_mort = mp.getParameterSpecialMort();
	CPPUNIT_ASSERT(special_mort - 1e-5 <= param.special_mort && param.special_mort <= special_mort + 1e-5);

	const dvector q = mp.getParameterQ();
	const dvector report_rates  = mp.getParameterReportRates();
	const dvector nb_par = mp.getParameterNbPar();
	for (int i = 1; i <= param.nfleet; i++) {
		CPPUNIT_ASSERT(q(i) - 1e-5 <= param.q(i) && param.q(i) <= q(i) + 1e-5);
		CPPUNIT_ASSERT(report_rates(i) - 1e-5 <= param.report_rates(i) && param.report_rates(i) <= report_rates(i) + 1e-5);
		CPPUNIT_ASSERT(nb_par(i) - 1e-5 <= param.nb_par(i) && param.nb_par(i) <= nb_par(i) + 1e-5);
	}
	dmatrix uu = mp.getParameterU();
	dmatrix vv = mp.getParameterV();
	dmatrix sigma = mp.getParameterSigma();
	for (int season = 1; season <= param.get_nseason(); season++) {
		for (int grid = 1; grid <= param.get_ngrid(); grid++) {
			const double u = uu(season, grid);
			const double uparam = param.get_usergrid_u(season, grid);
			CPPUNIT_ASSERT(u - 1e-5 <=  uparam && uparam <= u + 1e-5);

			const double v = vv(season, grid);
			const double vparam = param.get_usergrid_v(season, grid);
			CPPUNIT_ASSERT(v - 1e-5 <= vparam && vparam <= v + 1e-5);

			const double s = sigma(season, grid);
			const double sparam = param.get_usergrid_sigma(season, grid);
			CPPUNIT_ASSERT(s - 1e-5 <= sparam && sparam <= s + 1e-5);
		}
	}
	mp.final_calcs();
}
bool TestTagestAdmodel::isValuesEqual(const double value, const double expected) {
	return expected - 1e-5 <= value && value <= expected + 1e-5;
}
//End:------
