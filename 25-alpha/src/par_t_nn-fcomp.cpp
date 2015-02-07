//$Id: par_t_nn-fcomp.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "par_t_nn.h"
#include "coff_t.h"

/*
void resid_comp(const dmatrix& obs, const dmatrix& pred, const dmatrix& like,
                const int report);
void uvs_freq_comp(const dmatrix& unode, const dmatrix& vnode, const dmatrix& snode,
                 const imatrix& gridmap, const int report);
*/
void total_tag_penalty(dmatrix& tags, par_t<d3_array,dmatrix,dvector,double>& param, double& pen);
/*

void velpen(const dmatrix& u, const dmatrix& v,
            const double xweight, const double yweight,
            double& penalty);
void sigpen(const dmatrix& s, const double weight, double& penalty);

void weight_penalty(neural_net_vector& nn,
                    const double wts_weight, double& wts_penalty,
                    const int m_ipar78);
void hidden_wt_switch(neural_net_vector& nn,  double&  swpen);

void weight_switch_penalty(neural_net_vector& nn, const double switch_wt, double & wts_switch_penalty);

*/
void negative_tag_penalty(dmatrix& tags, par_t<d3_array,dmatrix,dvector,double>& param, double& pen);

void poisson_like(double& like, dmatrix& pred_tags, year_month& date,
       int cohort, recaptype_vector& recaps, int nrec, par_t<d3_array,dmatrix,dvector,double>& param,
       dmatrix& z, ivector& effort_occured, d3_array& fmort);

void negative_binomial_like(double& like, dmatrix& pred_tags, year_month& date, int cohort,
             recaptype_vector& recaps, int nrec, par_t<d3_array,dmatrix,dvector,double>& param, dmatrix& z,
             ivector& effort_occured, d3_array& fmort, dvector& a,
             const int naa, const int nb_scale);

void exponential_like(double& like, dmatrix& pred_tags, year_month& date, int cohort,
       recaptype_vector& recaps, int nrec, par_t<d3_array,dmatrix,dvector,double>& param, dmatrix& z,
       ivector& effort_occured, d3_array& fmort);

void get_effort_array(par_t<d3_array,dmatrix,dvector,double>& param, indexed_regional_fishery_record& irfr,
       d3_array& effort, year_month& date,
       ivector& effort_occured);

extern int _global_report_flag;
extern setsaveinterflag interflag;
extern intersavetype * isp;
extern ofstream clogf;

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void compute2(
dmatrix &release, year_month &date,
const int cohort, recaptype_vector& recaps, const int nrec, dmatrix &tot_mort,
ivector &effort_occured, d3_array &fish_mort, const int nfleet, double &plike,
par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE> &param) {
        double plike1 = 0.0;
        switch (param.m_ipar[11])
        {
          case 2:
		const int optionPoisson = param.m_ipar[27];
            if (optionPoisson == 0 || optionPoisson == 1) {
               poisson_like(plike1, release, date, cohort, recaps,
                nrec, param, tot_mort, effort_occured,
                fish_mort);
            } //end_if
            //
            //else if (param.ipar[27] == 2)
            //{
            //    poisson_like(plike1, psi_bar, date, cohort, recaps,
            //                 nrec, param, tot_mort, effort_occured,
            //                 fish_mort);
            //}
            else
            {
              cerr << "Error: Unknown option poisson m_ipar[27] = \"" << optionPoisson << "\" passed to fgcomp in " << __FILE__ << endl;
              exit(1);
            } //end_else
            break;

          case 3:
          //{
          //    least_square_like(plike1, release, date, cohort, recaps,
          //                      nrec, param, effort, fish_mort);
          //}
            cerr << "switch[11] = 3; "
                 << "least squares likelihood not implemented" << endl;
            exit(1);
            break;
          case 4:
          case 5:
            negative_binomial_like(plike1, release, date, cohort, recaps,
                nrec, param, tot_mort, effort_occured,
                fish_mort,param.nb_par,1,param.m_ipar[18]);
            break;
          case 14:
          case 15:
            negative_binomial_like(plike1, release, date, cohort, recaps,
                nrec, param, tot_mort, effort_occured,
                fish_mort,param.nb_par,2,param.m_ipar[18]);
            break;
          case 24:
          case 25:
            negative_binomial_like(plike1, release, date, cohort, recaps,
                nrec, param, tot_mort, effort_occured,
                fish_mort,param.nb_par,nfleet,param.m_ipar[18]);
            break;
          case 6:
            exponential_like(plike1, release, date, cohort, recaps,
                nrec, param, tot_mort, effort_occured,
                fish_mort);
            break;
          default:
            cerr << "Unknown likelihood function specified"
                 << "; param.m_ipar[11] = " << param.m_ipar[11]  << endl;
            exit(1);
        } // end switch


        plike += plike1;
      } //end_else

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void myfcomp2(const year_month &local_start_date, const year_month &final_date, 
		const bool pool_tags, int &last_rn, const int ncohort, dmatrix &release,
		dvar_matrix &u, dvar_matrix &v, dvar_matrix &sigma,
		double &plike, double &total_uv_penalty, double &total_sigma_penalty,
		coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE> &coff, indexed_regional_fishery_record& irfr, d3_array &effort, ivector &effort_occured, d3_array &fish_mort, dmatrix &tot_mort,
		const int cohort, recaptype_vector& recaps, const int nrec,
		const int nfleet,
		double &total_negative_tag_penalty,
		double &total_total_tag_penalty,
par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE> &param) {
	for (year_month date = local_start_date; date <= final_date; date++) {
		//cout << date << endl;
		if (pool_tags == 1) {
			while (last_rn < ncohort && date == param.get_tr_date(last_rn+1)) {
				last_rn++;
				const int i = param.get_tr_i(last_rn);
				const int j = param.get_tr_j(last_rn);
				release(i,j) += param.get_tr_tags(last_rn);
			}
		}
		//cout << sum(release) << endl;
		param.uvs_comp(u, v, sigma, date);
/*
		if (param.m_ipar[33] == 1) {
			const double uv_xweight = (const double)param.m_ipar[31]*1e-9;
			const double uv_yweight = (const double)param.m_ipar[32]*1e-9;
			double uv_penalty = 0.0;
			velpen(u, v, uv_xweight, uv_yweight, uv_penalty);
			plike -= uv_penalty;
			total_uv_penalty += uv_penalty;
		}
		if (param.m_ipar[41] == 1) {
			const double sig_weight = (const double)param.m_ipar[42]*1e-9;
			double sigma_penalty = 0.0;
			sigpen(sigma, sig_weight, sigma_penalty);
			plike -= sigma_penalty;
			total_sigma_penalty += sigma_penalty;
		}
		coff.comp(param, date, isp, u, v, sigma);
*/
		get_effort_array(param, irfr, effort, date, effort_occured);
		//cout << sum(effort) << endl;
		//cout << effort_occured << endl;
/*
		param.fish_mort_comp(fish_mort, effort, date);
		param.total_mort_comp(fish_mort, tot_mort);
		coff.b_adjust(param, tot_mort);
		compute2(release, date, cohort, recaps, nrec, tot_mort, effort_occured, fish_mort, nfleet, plike, param);

      		double neg_tag_pen = 0;
		negative_tag_penalty(release, param, neg_tag_pen);
		total_negative_tag_penalty += neg_tag_pen;
		plike -= neg_tag_pen;

		double tot_tag_pen = 0;
		total_tag_penalty(release, param, tot_tag_pen);
		total_total_tag_penalty += tot_tag_pen;
		plike -= tot_tag_pen;
		coff.adi(release);
*/
	}
}
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::reset(const dvar_vector& q,
						    const dvariable& mort,
						    const dvariable& special_mort,
						    const dvar_vector& report_rates,
						    const dvariable& u_slope,
						    const dvariable& u_offset,
						    const dvariable& v_slope,
						    const dvariable& v_offset,
						    const dvariable& sig_slope,
						    const dvariable& sig_offset,
						    const dvariable& uniform_sigma,
						    const dvar_vector& weights,
						    const dvar_vector& diff_hwts,
						    const dvar_vector& bias)
{
	if (m_ipar[24] > 0) {
		for (int i = 1; i <= num_afm; i++) {
			this->afm(i) = afm(i);
		}
	}
	if (m_ipar[22] == 1) {
		this->mort = mort;
		if (m_ipar[19] > 0) {
			this->special_mort = special_mort;
		}
	}
	if (m_ipar[25] == 1) {
		this->q(1) = q(1);
	} else if (m_ipar[25] == 2) {
		for (int f = 1; f <= get_nfleet(); f++) {
			this->q(f) = q(f);
		}
	}
	if (m_ipar[28] == 1) { //separate report_rates for each fleet
		for (int f = 1; f <= nfleet; f++) {
			this->report_rates(f) = report_rates(f);
		}
	}
	if (m_ipar[11] == 4 || m_ipar[11] == 14) {
		this->nb_par(1) = nb_par(1);
		if (m_ipar[11] == 14) {
			this->nb_par(2) = nb_par(2);
		}
	} else if (m_ipar[11] == 24) {
		for (int f = 1; f <= nfleet; f++) {
			this->nb_par(f) = nb_par(f);
		}
	}
	if (m_ipar[71] == 1) { // count slope & offset scaling parameters
		this->u_slope = u_slope;
	}
	if (m_ipar[72] == 1) {
		this->u_offset = u_offset;
	}
	if (m_ipar[73] == 1) {
		this->v_slope = v_slope;
	}
	if (m_ipar[74] == 1) {
		this->v_offset = v_offset;
	}
	if (m_ipar[21] == 1) {
		this->uniform_sigma = uniform_sigma;
	} else {
		if (m_ipar[75] == 1) {
			this->sig_slope = sig_slope;
		} 
		if (m_ipar[76] == 1) {
			this->sig_offset = sig_offset;
		}
	}
	int index = 1;
	int index2 = 1;
	for (int s = 1; s <= num_nn; s++) { //get the weights
		if (nn(s).nn_ipar(5) == 1) {
			D3_ARRAY w = nn(s).get_weights();
			MATRIX d = nn(s).get_diff_hwts();
			ivector vnrow = nn(s).get_nrow();
			ivector vncol = nn(s).get_ncol();
			for (int i = 1; i < num_levels(s); i++) {
				if (i < num_levels(s) - 1) {
					int index = 1;
					for (int j = 1; j <= vnrow(i); j++) {
						for (int k = 1;  k <= vncol(i); k++) {
							w(i,j,k) = weights(index);
							index++;
						}
					}
				} else {
					for (int j = 1; j <= vnrow(i); j++) {
						for (int k = 1; k <= vncol(i); k++) {
							d(j,k) = diff_hwts(index2);
							index2++;
						}
					}
				}
			}
		}
	}
	index = 1;
	for (int s = 1; s <= num_nn; s++) { //biases
		if (nn(s).nn_ipar(6)) {
			MATRIX b = nn(s).get_bias();
			ivector vnrow = nn(s).get_nrow();
			for (int i = 1; i < num_levels(s); i++) {
				for (int j = 1; j <= vnrow(i); j++) {
					b(i,j) = bias(index);
					index++;
				}
			}
		}
	}
}
template void par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>::reset(const dvar_vector &q, const dvariable &mort, const dvariable &special_mort, const dvar_vector &report_rates, const dvariable &u_slope, const dvariable &u_offset, const dvariable &v_slope, const dvariable &v_offset, const dvariable &sig_slope, const dvariable &sig_offset, const dvariable& uniform_sigma, const dvar_vector &weights, const dvar_vector &diff_hwts, const dvar_vector &bias);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
dvariable par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::fcomp(dvar_vector q, dvariable mort, dvariable special_mort, dvar_vector report_rates, dvariable u_slope, dvariable u_offset, dvariable v_slope, dvariable v_offset, dvariable sigma_slope, dvariable sigma_offset, dvariable uniform_sigma, dvar_vector weights, dvar_vector diff_hwts, dvar_vector bias) {
	dvariable plike = 0;
/*
	if (true) return plike;

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
	d3_array effort(1, nfleet, ilbv, iubv, jlbm, jubm); effort.initialize();
	d3_array fish_mort(1, nfleet, ilbv, iubv, jlbm, jubm); fish_mort.initialize();
	dmatrix tot_mort(1,m, jlb, jub); tot_mort.initialize();
	dmatrix release(1,m, jlb, jub); release.initialize();
	ivector effort_occured(1, nfleet); effort_occured.initialize();

	fpen = 0.0;
	total_uv_penalty = 0;
	total_sigma_penalty = 0;
	total_negative_tag_penalty = 0;
	total_total_tag_penalty = 0 ;
	total_bounds_penalty = 0;
	total_wts_penalty = 0.0;
	total_wts_switch_penalty = 0.0;

	reset_diff_hwts(nn); //put back the diff_hwts into the weights
	plike -= fpen;
	total_bounds_penalty += fpen; // msa this is not neccessary!
	if (m_ipar[78] > 0) {
		void weight_penalty(neural_net_vector& nn, const double wts_weight, double& wts_penalty, const int m_ipar78);

		const double wts_weight =(double)m_ipar[79]*1.0e-9;
		weight_penalty(nn, wts_weight, total_wts_penalty, m_ipar[78]);
		plike -= total_wts_penalty;
	}
	if (m_ipar[81] == 1) {
		void weight_switch_penalty(neural_net_vector& nn, const double switch_wt, double & wts_switch_penalty);

		const double switch_wt= (double) m_ipar[82]*1.0e-9;
		weight_switch_penalty(nn,switch_wt, total_wts_switch_penalty);
		plike -=  total_wts_switch_penalty;
	}
	int cohort = 0;
	int ncohort = nrelease;
	int pool_tags = m_ipar[13];
	int last_rn = 0;
	int c1 = 1;
	int c2 = -1;
	if (pool_tags == 1) {
		c2 = 1;
	} else if (pool_tags == 2) {
		c2 = ncohort;
	}
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
	u.initialize();
	v.initialize();
	sigma.initialize();

  year_month local_start_date = start_date;
  year_month save_start_date = start_date;
  year_month final_date = get_tr_date(nrelease) + nmonth;

    int total_months = final_date - start_date + 1;
	myfcomp2(local_start_date, final_date, pool_tags, last_rn, ncohort, release,
		u, v, sigma,
		plike, total_uv_penalty, total_sigma_penalty,
		*coff_t::instance, *indexed_regional_fishery_record::instance, effort, effort_occured, fish_mort, tot_mort,
		cohort, *recaptype_vector::instance, recaptype_vector::instance->getTotalRecords(),
		nfleet,
		total_negative_tag_penalty,
		total_total_tag_penalty,
		*this);
*/
        const int nvar = nvarcalc();
        dvar_vector x(1, nvar);
        dvector g(1, nvar);

        recaptype_vector &recaps = *recaptype_vector::instance;
        const int nrec = recaps.getTotalRecords();
        coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable> &coff = *coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::instance;
        coff_t<d3_array,dmatrix,dvector,double> &dfcoff = *coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::createDfInstance(*this);
        indexed_regional_fishery_record &irfr = *indexed_regional_fishery_record::instance;

        fgcomp2(plike, x, g, nvar, recaps, nrec, coff, dfcoff, irfr);

	return plike;
}
template dvariable par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>::fcomp(dvar_vector q, dvariable mort, dvariable special_mort, dvar_vector report_rates, dvariable u_slope, dvariable u_offset, dvariable v_slope, dvariable v_offset, dvariable sigma_slope, dvariable sigma_offset, dvariable uniform_sigma, dvar_vector weights, dvar_vector diff_hwts, dvar_vector bias);
