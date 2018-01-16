/**
 * $Id: TagestFacade.cpp 2737 2010-08-05 22:14:06Z johnoel $
 */
#include "TagestFacade.h"

//LoggerPtr TagestFacade::logger(Logger::getLogger("tagest.log"));

void TagestFacade::init()
{
	//follow code in tagest main from beginning till xinit
	cifstream fnt("file_nam.tmp");
	if (!fnt) {
		cerr << "Cannot open file_nam.tmp in main()" << endl;
		exit(1);
	}
	char filename[9];
	fnt >> filename;
	fnt.close();

	//allocate par_t
	make_par_t(adstring(filename));
	if (_param->m_ipar[14]) {
		cerr << "Omit release month option (flag 14) not currently supported." << endl;
		exit(1);
	}

	_param->m_ipar[5] = -1;
	
	//allocate coff_t
	make_coff_t();

	//read data
	nrec = 5000;
	_recaps.reset(new recaptype_vector(1, nrec));
	_recaps->set_cohort_pooling(_param->m_ipar[13]);
	readrecaps(*_param, filename, *_recaps, nrec);
	if (nrec <=0) {
		cerr << "No recapture data found." << endl;
		exit(1);
	}
	//_param->savefit(0.0, 0, 0.0, ".ppp");
	if (_param->m_ipar[13] == 2) {
		_param->collapse_cohorts(*_recaps, nrec);
	}

	_irfr.reset(new indexed_regional_fishery_record(filename));
	if (_param->m_ipar[43]) {
		adstring phdo("phdo");
		int s_phdo = _irfr->set_effort_to_recaps(phdo, *_recaps, nrec, *_param);
		cout << s_phdo << " effort records substituted for " << phdo << endl;
		clogf << s_phdo << " effort records substituted for " << phdo << endl;
	}
	if (_param->m_ipar[44]) {
		adstring iddo("iddo");
		int s_iddo = _irfr->set_effort_to_recaps(iddo, *_recaps, nrec, *_param);
		cout << s_iddo << " effort records substituted for " << iddo << endl;
		clogf << s_iddo << " effort records substituted for " << iddo << endl;
	}
	_param->set_fishing_effort_normalization(*_irfr);
	if (_irfr->is_normalized())
		clogf << "Fishing effort is normalized." << endl;
	else
		clogf << "Fishing effort not normalized." << endl;

	interflag.setsaveinter(1);  /* if 1 psi saving is activated */
	cout << "finished setsaveinterflag(1)" << endl;

	const int nfleet = _param->get_nfleet();
	const int m = _param->get_m();
	const int n = _param->get_n();
	const ivector &jlb = _param->get_jlb();
	const ivector &jub = _param->get_jub();

	tot_mort.allocate(1,m, jlb, jub);
	dftot_mort.allocate(1,m, jlb, jub);
	release.allocate(1,m, jlb, jub);
	dfrelease.allocate(1, m, jlb, jub);

	effort.allocate(1, nfleet);
	fish_mort.allocate(1, nfleet);
	dffish_mort.allocate(1, nfleet);
	for (int f = 1; f <= nfleet; f++) {
		effort(f).allocate(1, m, jlb, jub);
		fish_mort(f).allocate(1, m, jlb, jub);
		dffish_mort(f).allocate(1, m, jlb, jub);
	}

	//Use a macro for cleaner code
	ivector jlb1(0,m+1);
	ivector jub1(0,m+1);
	for (int i = 1; i <= m; i++) {
		jlb1(i) = jlb(i) - 1;
		jub1(i) = jub(i) + 1;
	}
	jlb1(0) = 0;
	jlb1(m+1) = 0;
	jub1(0) = n+1;
	jub1(m+1) = n+1;

	u.allocate(0, m+1, jlb1, jub1);
	v.allocate(0, m+1, jlb1, jub1);
	sigma.allocate(0, m+1, jlb1, jub1);

	dfu.allocate(0, m+1, jlb1, jub1);
	dfv.allocate(0, m+1, jlb1, jub1);
	dfsigma.allocate(0, m+1, jlb1, jub1);
}
void TagestFacade::make_par_t(const adstring filename)
{
	//read signature par file
	ifstream ifs(adstring(filename) + ".par");
	char signature[50];
	ifs.getline(signature, 50, '\n');
	ifs.close();

	if (adstring(signature) == "#v25n") {
		_param.reset(new par_t_nn(filename));

		//LOG_INFO(logger, "creating par_t_nn for file signature\"" << signature << "\" from file \"" << filename << "\"");
	} else if (adstring(signature) == "#v20r") {
		_param.reset(new par_t_reg(filename));

		//LOG_INFO(logger, "creating par_t_reg for file signature\"" << signature << "\" from file \"" << filename << "\"");
	} else {
		//LOG_ERROR(logger, "unknown file signature\"" << signature << "\" from file \"" << filename << "\"");
	}
}
void TagestFacade::make_coff_t()
{
	_coff.reset(create_coff_t(*_param, _param->m_ipar[6], _param->m_ipar[49]));
	_dfcoff.reset(create_coff_t(*_param, _param->m_ipar[6], _param->m_ipar[49]));
}
void TagestFacade::xinit(dvector& x)
{
	const int nvar = _param->nvarcalc();
	adstring_array x_names(1,nvar);
	x.allocate(1, nvar);
	_param->xinit(x,x_names);

	//LOG_DEBUG(logger, "\nxinit:\n" << "nvar = " << nvar << "\nx_names:\n" << x_names << "\n");
}
double TagestFacade::run(const dvector& x, dvector& g)
{
	double plike(0);

	clock_t time1 = clock();

	if ( ( isp->savefile = open(isp->savefilename, O_RDWR | O_CREAT | O_TRUNC , 0660) ) < 0) {
		cerr <<"Cannot open " << isp->savefilename <<" for output.\n";
		exit(1);
	}

	par_t_nn &param = (par_t_nn&)*_param;
	param.m_ipar[5]++;

	const int nfleet = param.get_nfleet();
	ivector effort_occured(1, nfleet); effort_occured.initialize();

	param.total_uv_penalty = 0;
	param.total_sigma_penalty = 0;
	param.total_negative_tag_penalty = 0;
	param.total_total_tag_penalty = 0 ;
	param.total_bounds_penalty = 0;
	param.total_wts_penalty = 0.0;
	param.total_wts_switch_penalty = 0.0;

	param.set_fpen(0);
	param.reset(x);
        plike -= param.get_fpen();
	param.total_bounds_penalty += param.get_fpen();

	par_t_nn dfparam((par_t_nn&)*_param);

	param.reset_diff_hwts(param.nn);
	double wts_weight = double(param.m_ipar[79])*1.0e-9;
	if (param.m_ipar[78] > 0) {
		weight_penalty(param.nn, wts_weight, param.total_wts_penalty, param.m_ipar[78]);
		plike -= param.total_wts_penalty;
	}
	double switch_wt = double(param.m_ipar[82])*1.0e-9;
	if (param.m_ipar[81] == 1) {
		weight_switch_penalty(param.nn,switch_wt, param.total_wts_switch_penalty);
		plike -=  param.total_wts_switch_penalty;
	}

	double dfplike = 1.0;

	coff_t &coff = *_coff;
	coff_t &dfcoff = *_dfcoff;

	int last_rn = 0;
	const year_month save_start_date = param.start_date;

	const int pool_tags = param.m_ipar[13];
	const int ncohort = param.nrelease;
	const int c2 = pool_tags == 2 ? ncohort : 1;
	for (int cc = 1; cc <= c2; cc++) {
		effort.initialize();
		fish_mort.initialize();
		dffish_mort.initialize();
		tot_mort.initialize();
		dftot_mort.initialize();

		u.initialize();
		v.initialize();
		sigma.initialize();
		dfu.initialize();
		dfv.initialize();
		dfsigma.initialize();

		release.initialize();
		dfrelease.initialize();

		coff.clean();
		dfcoff.clean();

		int cohort = 0;
		year_month local_start_date = param.start_date;
		year_month final_date = param.get_tr_date(param.nrelease) + param.nmonth;
		if (pool_tags == 2) {
			cohort = param.get_tr_cohort(cc);
			//clogf << "\nReleasing tags for cohort " << cohort << endl;
			local_start_date = param.get_tr_date(cc);
			final_date = local_start_date + param.nmonth;
			param.start_date = local_start_date;
			while (last_rn < ncohort && cohort == param.get_tr_cohort(last_rn+1)) {
				last_rn++;
				const int i = param.get_tr_i(last_rn);
				const int j = param.get_tr_j(last_rn);
				release(i,j) += param.get_tr_tags(last_rn);
			}
			cc = last_rn;
		}

		interflag.isp_reset();
		//clogf << "---------- starting forward loop ----------" << endl;
		for (year_month date = local_start_date; date <= final_date; date++) {
      			if (pool_tags == 1) {
				get_releases(param, release, date, last_rn, ncohort);
      			}

			param.uvs_comp(u, v, sigma, date);

			if (param.m_ipar[33] == 1) {
				double uv_penalty = 0.0;
				param.velpen2(u, v, uv_penalty);
				plike -= uv_penalty;
				param.total_uv_penalty += uv_penalty;
			}

			if (param.m_ipar[41] == 1) {
				double sigma_penalty = 0.0;
				param.sigpen2(sigma, sigma_penalty);
				plike -= sigma_penalty;
				param.total_sigma_penalty += sigma_penalty;
			}

			coff.comp(param, date, isp, u, v, sigma);

			get_effort_array(param, *_irfr, effort, date, effort_occured);
			param.fish_mort_comp(fish_mort, effort, date);
			param.total_mort_comp(fish_mort, tot_mort);

			coff.b_adjust(param, tot_mort);

			likelihoodComp(plike, param, release, cohort, date, nrec, nfleet, effort_occured);

			double neg_tag_pen = 0;
			negative_tag_penalty(release, param, neg_tag_pen);
			param.total_negative_tag_penalty += neg_tag_pen;
			plike -= neg_tag_pen;

			double tot_tag_pen = 0;
			total_tag_penalty(release, param, tot_tag_pen);
			param.total_total_tag_penalty += tot_tag_pen;
			plike -= tot_tag_pen;

			coff.adi(release, isp);
		}
		interflag.iclose(isp);

		//clogf << "\n---------- starting reverse loop ----------" << endl;
		for (year_month date = final_date; date >= local_start_date;) {
			dfcoff.dfadi(coff, release, dfrelease, isp);

			double dftot_tag_pen = 0.0;
			//plike -= tot_tag_pen;
			dftot_tag_pen -= dfplike;
			//total_tag_penalty(release, param, tot_tag_pen);
			df_total_tag_penalty(release, dfrelease, param, dftot_tag_pen);
			//double tot_tag_pen = 0;
			dftot_tag_pen = 0.0;

			double dfneg_tag_pen = 0.0;
			//plike -= neg_tag_pen;
			dfneg_tag_pen -= dfplike;
			//negative_tag_penalty(release, *this, neg_tag_pen, negatives);
			df_negative_tag_penalty(release, dfrelease, param, dfneg_tag_pen);
			//double neg_tag_pen = 0;
			dfneg_tag_pen = 0.0;

			dflikelihoodComp(dfplike, param, dfparam, release, dfrelease, cohort, date, nrec, nfleet, effort_occured);

			//coff.b_adjust(*this, tot_mort);
			dfcoff.dfb_adjust(coff, param, dfparam, dftot_mort);

			//total_mort_comp(fish_mort, tot_mort);
			dftotal_mort_comp(param, dfparam, fish_mort, dffish_mort, dftot_mort);

			//fish_mort_comp(fish_mort, effort, date);
			dfparam.df_fish_mort_comp(dfparam, dffish_mort, effort, date);

			//coff.comp(*this, date, isp, u, v, sigma);
			dfcoff.dfcomp(coff, param, dfparam, u, v, sigma, date, dfu, dfv, dfsigma, isp);

			if (param.m_ipar[41] == 1) {
				double dfsigma_penalty = 0.0;
				//plike -= sigma_penalty;
				dfsigma_penalty -= dfplike;
				//sigpen(sigma, sig_weight, sigma_penalty);
				dfparam.dfsigpen2(sigma, dfsigma, dfsigma_penalty);
				//double sigma_penalty = 0.0;
				dfsigma_penalty = 0.0;
			}

			if (param.m_ipar[33] == 1) {
				double dfuv_penalty = 0.0;
				//plike -= uv_penalty;
				dfuv_penalty -= dfplike;
				//velpen(u, v, uv_xweight, uv_yweight, uv_penalty);
				dfparam.dfvelpen2(u, v, dfu, dfv, dfuv_penalty);
				//double uv_penalty = 0.0;
				dfuv_penalty = 0.0;
			}

			//uvs_comp(u, v, sigma, date);
			dfparam.dfuvs_comp(param, dfu, dfv, dfsigma, date);

			date--;
			// recompute variables needed for adjoint computations
			get_effort_array(param, *_irfr, effort, date, effort_occured);
			param.fish_mort_comp(fish_mort, effort, date);
			param.total_mort_comp(fish_mort, tot_mort);
			coff.b_adjust(param, tot_mort);
		}
	}
	param.start_date = save_start_date;

	if (param.m_ipar[81] == 1) {
		double dftotal_wts_switch_penalty = 0.0;
		//plike -=  total_wts_switch_penalty;
		dftotal_wts_switch_penalty -= dfplike;
		//weight_switch_penalty(nn,switch_wt, total_wts_switch_penalty);
		dfweight_switch_penalty(param.nn, dfparam.nn, switch_wt, dftotal_wts_switch_penalty);
		//total_wts_switch_penalty = 0.0;
		dftotal_wts_switch_penalty = 0.0;
	}
	if (param.m_ipar[78] > 0) {
		double dftotal_wts_penalty = 0.0;
		//plike -= total_wts_penalty;
		dftotal_wts_penalty -= dfplike;
		//weight_penalty(param.nn, wts_weight, total_wts_penalty, param.m_ipar[78]);
		dfweight_penalty(param.nn, dfparam.nn, wts_weight, dftotal_wts_penalty, param.m_ipar[78]);
		//total_wts_penalty  = 0.0;
		dftotal_wts_penalty = 0.0;
	}
	dfparam.dfreset_diff_hwts(param.nn, dfparam.nn);

	dfparam.dfreset(g, x);
	reverse(plike, g, g.indexmax());

	close(isp->savefile);

	if (_global_report_flag) {
		resid_comp(release, release, release, 1);
		uvs_freq_comp(release, release, release, param.get_gridmap(), 1);
	}

	time_t time_sec;
	time(&time_sec);
	clock_t time2 = clock();
	double elapsed_time = (double)(time2-time1)/CLOCKS_PER_SEC;

	//LOG_INFO(logger, "Objective function value = " << setprecision(8) << plike << " elapsed time = " << setprecision(2) << elapsed_time << " seconds." << setprecision(5));

	return plike;
}
void TagestFacade::likelihoodComp(double &likelihood, par_t_nn &param, dmatrix &release, const int cohort, year_month &date, const int nrec, const int nfleet, ivector &effort_occured)
{
	double plike1 = 0.0;
	switch (param.m_ipar[11]) {
	case 2:
		if (param.m_ipar[27] == 0 || param.m_ipar[27] == 1) {
			poisson_like(plike1, release, date, cohort, *_recaps, nrec, param, tot_mort, effort_occured, fish_mort);
		} else {
			cerr << "Unknown option (" << param.m_ipar[27] << ") passed to fgcomp in " << __FILE__ << endl;
			exit(1);
		}
		break;
	case 3:
		cerr << "switch[11] = 3; " << "least squares likelihood not implemented" << endl;
		exit(1);
		break;
	case 4:
	case 5:
		negative_binomial_like(plike1, release, date, cohort, *_recaps, nrec, param, tot_mort, effort_occured, fish_mort,param.nb_par,1,param.m_ipar[18]);
		break;
	case 14:
	case 15:
		negative_binomial_like(plike1, release, date, cohort, *_recaps, nrec, param, tot_mort, effort_occured, fish_mort,param.nb_par,2,param.m_ipar[18]);
		break;
	case 24:
	case 25:
		negative_binomial_like(plike1, release, date, cohort, *_recaps, nrec, param, tot_mort, effort_occured, fish_mort,param.nb_par,nfleet,param.m_ipar[18]);
		break;
	case 6:
		exponential_like(plike1, release, date, cohort, *_recaps, nrec, param, tot_mort, effort_occured, fish_mort);
		break;
	default:
		cerr << "Unknown likelihood function specified" << "; param.m_ipar[11] = " << param.m_ipar[11]  << endl;
		exit(1);
	
	}

        likelihood += plike1;
}
void TagestFacade::dflikelihoodComp(double &dfplike, par_t_nn &param, par_t_nn&dfparam, dmatrix &release, dmatrix &dfrelease, const int cohort, year_month &date, const int nrec, const int nfleet, ivector &effort_occured)
{
	switch(param.m_ipar[11]) {
	case 2:
		if (param.m_ipar[27] == 0 || param.m_ipar[27] == 1) {
			df_poisson_like(dfplike, release, dfrelease, date, cohort, *_recaps, nrec, param, dfparam, tot_mort, dftot_mort, effort_occured, fish_mort, dffish_mort);
		}
		break;
	case 4:
	case 5:
		df_nb_like(dfplike, release, dfrelease, date, cohort, *_recaps, nrec, param, dfparam, tot_mort, dftot_mort, effort_occured, fish_mort, dffish_mort, param.nb_par, dfparam.nb_par, 1,param.m_ipar[18]);
		break;
	case 14:
	case 15:
		df_nb_like(dfplike, release, dfrelease, date, cohort, *_recaps, nrec, param, dfparam, tot_mort, dftot_mort, effort_occured, fish_mort, dffish_mort, param.nb_par, dfparam.nb_par, 2,param.m_ipar[18]);
		break;
	case 24:
	case 25:
		df_nb_like(dfplike, release, dfrelease, date, cohort, *_recaps, nrec, param, dfparam, tot_mort, dftot_mort, effort_occured, fish_mort, dffish_mort, param.nb_par, dfparam.nb_par, nfleet,param.m_ipar[18]);
		break;
	case 6:
		df_exp_like(dfplike, release, dfrelease, date, cohort, *_recaps, nrec, param, dfparam, tot_mort, dftot_mort, effort_occured, fish_mort, dffish_mort);
		break;
	default:
		cerr << "Unknown likelihood function specified" <<  "; param.m_ipar[11] = " << param.m_ipar[11]  << endl << "This can't happen!" << endl;
		exit(1);
	}
}
void TagestFacade::get_releases(const par_t_nn &param, dmatrix &release, const year_month &date, int &last_rn, const int ncohort)
{
	while(last_rn < ncohort && date == param.get_tr_date(last_rn+1)) {
		last_rn++;
		const int i = param.get_tr_i(last_rn);
		const int j = param.get_tr_j(last_rn);
		release(i,j) += param.get_tr_tags(last_rn);
	}
}
