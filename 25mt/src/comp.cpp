//$Id: comp.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "par_t_reg.h"
#include "coff_t.h"

void dfcomp()
{
        dvar_matrix_position positionU = restore_dvar_matrix_position();
	dmatrix u = restore_dvar_matrix_value(positionU);
        dvar_matrix_position positionV = restore_dvar_matrix_position();
	dmatrix v = restore_dvar_matrix_value(positionV);
        dvar_matrix_position positionSigma = restore_dvar_matrix_position();
	dmatrix sigma = restore_dvar_matrix_value(positionSigma);

        dvar_matrix_position positionA = restore_dvar_matrix_position();
        dvar_matrix_position positionRawB = restore_dvar_matrix_position();
        dvar_matrix_position positionC = restore_dvar_matrix_position();
        dvar_matrix_position positionD = restore_dvar_matrix_position();
	//dmatrix d = restore_dvar_matrix_value(positionD);
        dvar_matrix_position positionE = restore_dvar_matrix_position();
	//dmatrix e = restore_dvar_matrix_value(positionE);
        dvar_matrix_position positionF = restore_dvar_matrix_position();
	//dmatrix f = restore_dvar_matrix_value(positionF);
        dvar_matrix_position positionYBet = restore_dvar_matrix_position();
	//dmatrix ybet = restore_dvar_matrix_value(positionYBet);
        dvar_matrix_position positionYGam = restore_dvar_matrix_position();
	//dmatrix ygam = restore_dvar_matrix_value(positionYGam);

	const int year = restore_int_value();
	const int month = restore_int_value();

	year_month date(year, month);

	dmatrix dfu = restore_dvar_matrix_derivatives(positionU);
	dmatrix dfv = restore_dvar_matrix_derivatives(positionV);
	dmatrix dfsigma = restore_dvar_matrix_derivatives(positionSigma);

	dmatrix dfa = restore_dvar_matrix_derivatives(positionA);
	dmatrix dfraw_b = restore_dvar_matrix_derivatives(positionRawB);
	dmatrix dfc = restore_dvar_matrix_derivatives(positionC);
	dmatrix dfd = restore_dvar_matrix_derivatives(positionD);
	dmatrix dfe = restore_dvar_matrix_derivatives(positionE);
	dmatrix dff = restore_dvar_matrix_derivatives(positionF);
	dmatrix dfybet = restore_dvar_matrix_derivatives(positionYBet);
	dmatrix dfygam = restore_dvar_matrix_derivatives(positionYGam);

	par_t_reg<d3_array,dmatrix,dvector,double> &param = (par_t_reg<d3_array,dmatrix,dvector,double>&)*par_t<d3_array,dmatrix,dvector,double>::instance;
	coff_t<d3_array,dmatrix,dvector,double> &coff = *coff_t<d3_array,dmatrix,dvector,double>::instance;
	coff.dfcomp(coff, param,
		u, v, sigma,
		date, 
		dfu, dfv, dfsigma,
		0,
		coff.get_d(),
		coff.get_e(),
		coff.get_f(),
		coff.get_ybet(),
		coff.get_ygam(),
		dfa,
		dfraw_b,
		dfc,
		dfd,
		dfe,
		dff,
		dfybet,
		dfygam);

	dfu.save_dmatrix_derivatives(positionU);
	dfv.save_dmatrix_derivatives(positionV);
	dfsigma.save_dmatrix_derivatives(positionSigma);
	dfa.save_dmatrix_derivatives(positionA);
	dfraw_b.save_dmatrix_derivatives(positionRawB);
	dfc.save_dmatrix_derivatives(positionC);
	dfd.save_dmatrix_derivatives(positionD);
	dfe.save_dmatrix_derivatives(positionE);
	dff.save_dmatrix_derivatives(positionF);
	dfybet.save_dmatrix_derivatives(positionYBet);
	dfygam.save_dmatrix_derivatives(positionYGam);
}
void comp(coff_t<d3_array,dmatrix,dvector,double> &coff, dvar_matrix &u, dvar_matrix &v, dvar_matrix &sigma, par_t<d3_array,dmatrix,dvector,double> &param, year_month &date)
{
	dvar_matrix &a = coff.getVariableA();
	dvar_matrix &raw_b = coff.getVariableRawB();
	dvar_matrix &c = coff.getVariableC();
	dvar_matrix &d = coff.getVariableD();
	dvar_matrix &e = coff.getVariableE();
	dvar_matrix &f = coff.getVariableF();
	dvar_matrix &ygam = coff.getVariableYGam();
	dvar_matrix &ybet = coff.getVariableYBet();

	dmatrix u2;
	u2.allocate(u);
	u2 = value(u);
	dmatrix v2;
	v2.allocate(v);
	v2 = value(v);
	dmatrix sigma2;
	sigma2.allocate(sigma);
	sigma2 = value(sigma);

	coff.comp(param, u2, v2, sigma2);

	a = coff.get_a();
	raw_b = coff.get_raw_b();
	c = coff.get_c();
	d = coff.get_d();
	e = coff.get_e();
	f = coff.get_f();
	ygam = coff.get_ygam();
	ybet = coff.get_ybet();
                                                                                                                                                                                                    
	save_int_value(date.get_month_value());
	save_int_value(date.get_year());

	//ygam.save_dvar_matrix_value();
	ygam.save_dvar_matrix_position();
	//ybet.save_dvar_matrix_value();
	ybet.save_dvar_matrix_position();
	//f.save_dvar_matrix_value();
	f.save_dvar_matrix_position();
	//e.save_dvar_matrix_value();
	e.save_dvar_matrix_position();
	//d.save_dvar_matrix_value();
	d.save_dvar_matrix_position();
	c.save_dvar_matrix_position();
	raw_b.save_dvar_matrix_position();
	a.save_dvar_matrix_position();
	sigma.save_dvar_matrix_value();
	sigma.save_dvar_matrix_position();
	v.save_dvar_matrix_value();
	v.save_dvar_matrix_position();
	u.save_dvar_matrix_value();
	u.save_dvar_matrix_position();

	gradient_structure::GRAD_STACK1->set_gradient_stack(dfcomp);
}
