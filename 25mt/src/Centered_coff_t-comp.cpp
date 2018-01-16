//$Id: Centered_coff_t-comp.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "coff_t.h"

prevariable smoother(const dvariable &x, const double scale);
void bad_cell_type(const char* function, int boundary);

extern double sscale;

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void Centered_coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::comp(par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, year_month& date, dvar_matrix& u, dvar_matrix& v, dvar_matrix& sigma)
{
	dvar_matrix& acoff = this->getVariableA();
	dvar_matrix& bcoff = this->getVariableRawB();
	dvar_matrix& ccoff = this->getVariableC();
	dvar_matrix& dcoff = this->getVariableD();
	dvar_matrix& ecoff = this->getVariableE();
	dvar_matrix& fcoff = this->getVariableF();
	dvar_matrix& ybet_c = this->getVariableYBet();
	dvar_matrix& ygam_c = this->getVariableYGam();
	double dx    = param.get_deltax_eq();
	double dy    = param.get_deltay();
	double dxdx  = dx * dx;
	double dydy  = dy * dy;
	double twodxdx = 2.0*dxdx;
	double twodydy = 2.0*dydy;
	int m = param.get_m();
	int n = param.get_n();
	ivector jlb1(0,m+1);
	ivector jub1(0,m+1);
	for (int i = 1; i <= m; i++) {
		jlb1(i) = this->jlb(i)-1;
		jub1(i) = this->jub(i)+1;
	}
	jlb1(0) = 0;
	jlb1(m+1) = 0;
	jub1(0) = n+1;
	jub1(m+1) = n+1;
	if (param.m_ipar[34]<=0) {
		cerr << "param.ipar[34] must be greater than 0" << endl;
		exit(1);
	}
	sscale    = (double)param.m_ipar[34]/100.;

	const long nstep = this->get_k() > 1 ? this->get_k() : 1;// time steps per month
	const double rdt2 = 2*nstep;  // 2/delta_t

	bound boundary;

	for (int i = 1; i <= m; i++) {
		const int jmin = this->jlb(i);
		const int jmax = this->jub(i);
		for (int j = jmin; j <= jmax; j++) {
			boundary = param.get_cell_type(i,j);
			dvariable bsigy = 0.0;
			dvariable fsigy = 0.0;
			dvariable vm = 0.0;
			dvariable vv = 0.0;
			dvariable vp = 0.0;
			if (j >= this->jlb(i)) {
				bsigy = (sigma[i][j] + sigma[i][j-1]) / twodydy;
				vm = 0.5*v(i,j-1)/dy;
			}
			if (j <= this->jub(i)) {
				fsigy = (sigma[i][j] + sigma[i][j+1]) / twodydy;
				vp = 0.5*v(i,j+1)/dy;
			}
			// y direction
			dcoff(i,j) = atype(vm, bsigy, boundary.edgey());
			ecoff(i,j) = btype(vv, bsigy, fsigy, rdt2, boundary.edgey());
			fcoff(i,j) = ctype(vp, fsigy, boundary.edgey());
		}
	}
	for (int j = 1; j <= n;j++) {
		const int imin = this->ilb(j);
		const int imax = this->iub(j);
		for (int i = imin; i <= imax; i++) {
			boundary = param.get_cell_type(i,j);
			dvariable bsigx = 0.0;
			dvariable fsigx = 0.0;
			dvariable um = 0.0;
			dvariable uu = 0.0;
			dvariable up = 0.0;
			if (jlb1(i-1) <= j && j <= jub1(i-1)) {
				bsigx = (sigma[i][j]+sigma[i-1][j]) / twodxdx;
				um = 0.5*u(i-1,j)/dx;
			}
			if (jlb1(i+1) <= j && j <= jub1(i+1)) {
				fsigx = (sigma[i][j]+sigma[i+1][j]) / twodxdx;
				up = 0.5*u(i+1,j)/dx;
			}
			// x direction
			acoff(j, i) = atype(um, bsigx, boundary.edgex());
			bcoff(j, i) = btype(uu, bsigx, fsigx, rdt2, boundary.edgex());
			ccoff(j, i) = ctype(up, fsigx, boundary.edgex());
		}
	}
	for (int i = 1; i <= m; i++) {
		const int jmin = ecoff(i).indexmin();
		const int jmax = ecoff(i).indexmax();
		ygam_c(i,jmin) = 0.0;
		ybet_c(i,jmin) = 1.0/ecoff(i,jmin);
		for (int j = jmin + 1; j <= jmax; j++) {
			ygam_c(i,j) = fcoff(i,j-1) * ybet_c(i,j-1);
			ybet_c(i,j) = 1.0 /( ecoff(i,j) - dcoff(i,j) * ygam_c(i,j));
		}
	}
}
template void Centered_coff_t<d3_array,dmatrix,dvector,double>::comp(par_t<d3_array,dmatrix,dvector,double>& param, year_month& date, dvar_matrix& u, dvar_matrix& v, dvar_matrix& sigma);
template void Centered_coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::comp(par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param, year_month& date, dvar_matrix& u, dvar_matrix& v, dvar_matrix& sigma);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
prevariable Centered_coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::atype(const dvariable &um, const dvariable &bsig, const int boundary)
{
	dvariable acoff = 0;
	switch (boundary) {
	case NONE:
	case RIGHT_CLOSED:
		acoff = - bsig - smoother(um,sscale);
		break;
	case RIGHT_OPEN:
		acoff = - 2.0 * smoother(um,sscale);
		break;
	case LEFT_OPEN:
	case LEFT_CLOSED:
	case ISLAND:
	case CHANNEL:
		acoff = 0;
		break;
	default:
		bad_cell_type("atype", boundary);
	}
	return acoff;
}
template prevariable Centered_coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::atype(const dvariable &um, const dvariable &bsig, const int boundary);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
prevariable Centered_coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::btype(const dvariable &uu, const dvariable &bsig, const dvariable &fsig, const double dt, const int boundary)
{
	dvariable bcoff = 0;
	switch (boundary) {
	case LEFT_OPEN:
		bcoff = dt - 2.0 * smoother(uu,sscale);
		break;
	case RIGHT_OPEN:
		bcoff = dt + 2.0 * smoother(uu,sscale);
		break;
	case NONE:
		bcoff = dt + (bsig + fsig);
		break;
	case LEFT_CLOSED:
		bcoff = dt + fsig + smoother(uu,sscale);
		break;
	case RIGHT_CLOSED:
		bcoff = dt + bsig - smoother(uu,sscale);
		break;
	case ISLAND:
	case CHANNEL:
		bcoff = dt;
		break;
	default:
		bad_cell_type("btype", boundary);
	}
	return bcoff;
}
template prevariable Centered_coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::btype(const dvariable &uu, const dvariable &bsig, const dvariable &fsig, const double dt, const int boundary);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
prevariable Centered_coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::ctype(const dvariable &up, const dvariable &fsig, const int boundary)
{
	dvariable ccoff = 0;
	switch (boundary) {
	case NONE:
	case LEFT_CLOSED:
		ccoff =  - fsig + smoother(up,sscale);
		break;
	case LEFT_OPEN:
		ccoff = 2.0 * smoother(up,sscale);
		break;
	case RIGHT_OPEN:
	case RIGHT_CLOSED:
	case ISLAND:
	case CHANNEL:
		ccoff = 0;
		break;
	default:
		bad_cell_type("ctype", boundary);
	}
	return ccoff;
}
template prevariable Centered_coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::ctype(const dvariable &up, const dvariable &fsig, const int boundary);
