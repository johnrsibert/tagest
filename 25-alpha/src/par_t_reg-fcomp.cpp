//$Id: par_t_reg-fcomp.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "par_t_reg.h"
#include "coff_t.h"
/*
void least_squares_like(dvariable& like, dvar_matrix& pred_tags, year_month& date, int cohort, recaptype_vector& recaps, int nrec, par_t& param, dvar_matrix z, ivector& effort_occured, dvar3_array& fmort);
*/
/*
void get_effort_array(par_t<d3_array,dmatrix,dvector,double>& param, indexed_regional_fishery_record& irfr, d3_array& effort, year_month& date, ivector& effort_occured);
void poisson_like(dvariable &likelihood, const dvar_matrix& pred_tags, const year_month& date, const int cohort, const recaptype_vector& recaps, const int nrec, const par_t<d3_array,dmatrix,dvector,double>& param, const dvar_matrix &z, const ivector &effort_occured, const dvar3_array &fmort, dvar_vector &report_rates);
dvariable negative_tag_penalty(const dvar_matrix &tags, const par_t<d3_array,dmatrix,dvector,double>& param);
dvariable total_tag_penalty(const dvar_matrix &tags, const par_t<d3_array,dmatrix,dvector,double>& param);

extern ofstream clogf;
extern int _global_report_flag;
*/
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
dvariable par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::fcomp(dvar_vector q, dvariable mort, dvariable special_mort, dvar_vector report_rates, dvar_vector nb_par, dvar_matrix seasonGridU, dvar_matrix seasonGridV, dvar_matrix seasonGridSigma)
{
	dvariable plike = 0.0;
/*
	m_ipar[5]++;

	//reset(parameters);
	indexed_regional_fishery_record &irfr = *indexed_regional_fishery_record::instance;
	recaptype_vector &recaps = *recaptype_vector::instance;
	coff_t &coff = *coff_t::instance;

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
	dvar3_array fish_mort(1, nfleet, ilbv, iubv, jlbm, jubm);
	dvar_matrix tot_mort(1,m, jlb, jub);
	dvar_matrix release(1,m, jlb, jub);
	ivector effort_occured(1, nfleet); 

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
	dvar_matrix u(0, m+1, jlb1, jub1);
	dvar_matrix v(0, m+1, jlb1, jub1);
	dvar_matrix sigma(0, m+1, jlb1, jub1);

	effort.initialize();
	tot_mort.initialize();
	release.initialize();
	effort_occured.initialize();
	u.initialize();
	v.initialize();
	sigma.initialize();

	total_uv_penalty = 0;
	total_sigma_penalty = 0;
	total_negative_tag_penalty = 0;
	total_total_tag_penalty = 0 ;
	total_bounds_penalty = 0;

	//cout << gradient_structure::totalbytes() << endl;

	const int ncohort = nrelease;

	int pool_tags = m_ipar[13];
	int cohort;
	if (pool_tags) {
		cohort = 0;
	}

	int last_rn = 0;

	int current_season = 0;

	const year_month local_start_date = start_date;
	const year_month final_date = get_tr_date(nrelease) + nmonth;
	for (year_month date = local_start_date; date <= final_date; date++) {
		if (pool_tags) {
			while (last_rn < ncohort && date == get_tr_date(last_rn+1)) {
				last_rn++;
				const int i = get_tr_i(last_rn);
				const int j = get_tr_j(last_rn);
				release(i,j) += get_tr_tags(last_rn);
			}
		}

		void b_adjust2(coff_t &coff, const dvar_matrix &tot_mort, par_t_reg &param, year_month &date);
		b_adjust2(coff, tot_mort, *this, date);

		const int season =  get_season(date);
		if (current_season != season) {
			uvs_comp(seasonGridU, seasonGridV, seasonGridSigma, u, v, sigma, season);
			{
				dvariable penalty = addTotalUVPenalty(seasonGridU, seasonGridV, season);
				//total_uv_penalty += value(penalty);
				plike -= penalty;
			}
			{
				dvariable penalty = addTotalSigmaPenalty(seasonGridSigma, season);
				//total_sigma_penalty += value(penalty);
				plike -= penalty;
			}
			//coff.comp(*this, date, u, v, sigma);
			void comp(coff_t &coff, dvar_matrix &u, dvar_matrix &v, dvar_matrix &sigma, par_t &param, year_month &date);
			comp(coff, u, v, sigma, *this, date);
			current_season = season;
		}
		get_effort_array(*this, irfr, effort, date, effort_occured);
		fish_mort_comp(fish_mort, effort, q);
		total_mort_comp(fish_mort, tot_mort, mort, special_mort);
		//coff.b_adjust(*this, tot_mort);
		void b_adjust(coff_t &coff, const dvar_matrix &tot_mort, par_t_reg &param);
		b_adjust(coff, tot_mort, *this);
		const int nrec = 4211;
		if (m_ipar[11] == 2) {
			dvariable likelihood = 0.0;
			poisson_like(likelihood, release, date, cohort, recaps, nrec, *this, tot_mort, effort_occured, fish_mort, report_rates);
			plike -= likelihood;
		} else if (m_ipar[11] == 3) {
			dvariable likelihood = 0.0;
			//least_squares_like(likelihood, release, date, cohort, recaps, nrec, *this, tot_mort, effort_occured, fish_mort);
			plike += 1e-1*likelihood;
		} else {
			cerr << "Unknown likelihood function specified" << "; m_ipar[11] = " << m_ipar[11]  << endl;
			exit(1);
		}
		{
			dvariable penalty = negative_tag_penalty(release, *this);
			//total_negative_tag_penalty += value(penalty);
			plike += penalty;
		}
		{
			dvariable penalty = total_tag_penalty(release, *this);
			//total_total_tag_penalty += value(penalty);
			plike += penalty;
		}
if (date < final_date) {
		//coff.adi(release, *this);
		//void checkAdiDerivatives(coff_t &coff, const dvar_matrix &releases);
		//checkAdiDerivatives(coff, release);
		void adi(coff_t &coff, dvar_matrix &release, par_t_reg &param);
		adi(coff, release, *this);
}
	}
*/
	const int nvar = nvarcalc();
	dvar_vector x(1, nvar);
	dvector g(1, nvar);
	
	coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable> &coff = *coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::instance;
	coff_t<d3_array,dmatrix,dvector,double> &dfcoff = *coff_t<d3_array,dmatrix,dvector,double>::dfinstance;
	indexed_regional_fishery_record &irfr = *indexed_regional_fishery_record::instance;
	recaptype_vector &recaps = *recaptype_vector::instance;
	const int nrec = recaps.getTotalRecords();
	fgcomp2(plike, x, g, nvar, recaps, nrec, coff, dfcoff, irfr);

	return plike;
}
template dvariable par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>::fcomp(dvar_vector q, dvariable mort, dvariable special_mort, dvar_vector report_rates, dvar_vector nb_par, dvar_matrix seasonGridU, dvar_matrix seasonGridV, dvar_matrix seasonGridSigma);
