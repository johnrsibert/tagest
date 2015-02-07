//$Id: b_adjust.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "par_t_reg.h"
#include "coff_t.h"

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void get_effort_array(par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, indexed_regional_fishery_record& irfr, d3_array& effort, year_month& date, ivector& effort_occured);

void dfb_adjust()
{
        dvar_matrix_position positionZ = restore_dvar_matrix_position();

        dvar_matrix_position positionA = restore_dvar_matrix_position();
        //dmatrix a = restore_dvar_matrix_value(positionA);
        dvar_matrix_position positionRawB = restore_dvar_matrix_position();
        dvar_matrix_position positionB = restore_dvar_matrix_position();
        //dmatrix b = restore_dvar_matrix_value(positionB);
        dvar_matrix_position positionC = restore_dvar_matrix_position();
        //dmatrix c = restore_dvar_matrix_value(positionC);
        dvar_matrix_position positionXBet = restore_dvar_matrix_position();
        //dmatrix xbet = restore_dvar_matrix_value(positionXBet);
        dvar_matrix_position positionXGam= restore_dvar_matrix_position();
        //dmatrix xgam = restore_dvar_matrix_value(positionXGam);

	dmatrix dfz = restore_dvar_matrix_derivatives(positionZ);
	dmatrix dfa = restore_dvar_matrix_derivatives(positionA);
	dmatrix dfraw_b = restore_dvar_matrix_derivatives(positionRawB);
	dmatrix dfb = restore_dvar_matrix_derivatives(positionB);
	dmatrix dfc = restore_dvar_matrix_derivatives(positionC);
	dmatrix dfxbet = restore_dvar_matrix_derivatives(positionXBet);
	dmatrix dfxgam = restore_dvar_matrix_derivatives(positionXGam);

	coff_t<d3_array,dmatrix,dvector,double> &coff = *coff_t<d3_array,dmatrix,dvector,double>::instance;
	par_t_reg<d3_array,dmatrix,dvector,double> &param = (par_t_reg<d3_array,dmatrix,dvector,double>&)*par_t<d3_array,dmatrix,dvector,double>::instance;
	coff.dfb_adjust(
        param,
        dfz,
        coff.get_a(),
        coff.get_b(),
        coff.get_c(),
        coff.get_xbet(),
        coff.get_xgam(),
        dfa,
        dfraw_b,
        dfb,
        dfc,
        dfxbet,
        dfxgam);

	dfz.save_dmatrix_derivatives(positionZ);
	dfa.save_dmatrix_derivatives(positionA);
	dfraw_b.save_dmatrix_derivatives(positionRawB);
	dfb.save_dmatrix_derivatives(positionB);
	dfc.save_dmatrix_derivatives(positionC);
	dfxbet.save_dmatrix_derivatives(positionXBet);
	dfxgam.save_dmatrix_derivatives(positionXGam);
}
void b_adjust(coff_t<d3_array,dmatrix,dvector,double> &coff, const dvar_matrix &tot_mort, par_t_reg<d3_array,dmatrix,dvector,double> &param)
{
	dvar_matrix &a = coff.getVariableA();
	dvar_matrix &raw_b = coff.getVariableRawB();
	dvar_matrix &b = coff.getVariableB();
	dvar_matrix &c = coff.getVariableC();
	dvar_matrix &xbet = coff.getVariableXBet();
	dvar_matrix &xgam = coff.getVariableXGam();
		
	//coff.get_a() = value(a);
	//coff.get_raw_b() = value(raw_b);
	//coff.get_b() = value(b);
	//coff.get_c() = value(c);
	//coff.get_xbet() = value(xbet);
	//coff.get_xgam() = value(xgam);

	dmatrix z;
	z.allocate(tot_mort);
	z = value(tot_mort);
	coff.b_adjust(param, z);

	b = coff.get_b();
	xbet = coff.get_xbet();
	xgam = coff.get_xgam();

	//xgam.save_dvar_matrix_value();
	xgam.save_dvar_matrix_position();
	//xbet.save_dvar_matrix_value();
	xbet.save_dvar_matrix_position();
	//c.save_dvar_matrix_value();
	c.save_dvar_matrix_position();
	//b.save_dvar_matrix_value();
	b.save_dvar_matrix_position();
	raw_b.save_dvar_matrix_position();
	//a.save_dvar_matrix_value();
	a.save_dvar_matrix_position();
	tot_mort.save_dvar_matrix_position();

	gradient_structure::GRAD_STACK1->set_gradient_stack(dfb_adjust);
}
void dfb_adjust2()
{
	const int year = restore_int_value();
	const int month = restore_int_value();
	year_month date(year, month);
	date--;

	par_t_reg<d3_array,dmatrix,dvector,double> &param = (par_t_reg<d3_array,dmatrix,dvector,double>&)*par_t<d3_array,dmatrix,dvector,double>::instance;
	const int nfleet = param.get_nfleet();
	const int m = param.get_m();
	const ivector &jlb = param.get_jlb();
	const ivector &jub = param.get_jub();

	imatrix jlbm(1,nfleet,1,m);
	imatrix jubm(1,nfleet,1,m);
	ivector ilbv(1,nfleet);
	ivector iubv(1,nfleet);
	for (int fleet = 1; fleet <= nfleet; fleet++) {
		ilbv(fleet) = 1;
		iubv(fleet) = m;
		for (int i = 1; i <= m; i++) {
			jlbm(fleet,i) = jlb(i);
			jubm(fleet,i) = jub(i);
		}
	}
	d3_array effort(1, nfleet, ilbv, iubv, jlbm, jubm);
	d3_array fish_mort(1, nfleet, ilbv, iubv, jlbm, jubm);
	dmatrix tot_mort(1,m, jlb, jub);
	indexed_regional_fishery_record &irfr = *indexed_regional_fishery_record::instance;
	ivector effort_occured(1, param.get_nfleet());
	get_effort_array<d3_array,dmatrix,dvector,double>(param, irfr, effort, date, effort_occured);
	param.fish_mort_comp(fish_mort, effort, date);
	param.total_mort_comp(fish_mort, tot_mort);
	coff_t<d3_array,dmatrix,dvector,double> &coff = *coff_t<d3_array,dmatrix,dvector,double>::instance;
	coff.b_adjust(param, tot_mort);
}
void b_adjust2(coff_t<d3_array,dmatrix,dvector,double> &coff, const dvar_matrix &tot_mort, par_t_reg<d3_array,dmatrix,dvector,double> &param, year_month &date)
{
	save_int_value(date.get_month_value());
	save_int_value(date.get_year());
	gradient_structure::GRAD_STACK1->set_gradient_stack(dfb_adjust2);
}
