//$Id: par_t_reg-addTotalSigmaPenalty.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "par_t_reg.h"

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
dvariable par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::addTotalSigmaPenalty(dvar_matrix &sigmaSeasonGrid, const int season)
{
	dvariable penalty = 0;
	if (m_ipar[41] > 0) {
		// convert sigma to a radius
		dvar_vector sigma(1,ngrid);
		sigma.initialize();
		const dvar_vector& sigmaseason = sigmaSeasonGrid(season);
		for (int grid = 1; grid <= ngrid; grid++) {
			sigma[grid] = sqrt(sigmaseason(grid)/M_PI);
		}
		dvar_matrix dxsigma(1,m, 1,n);
		dxsigma.initialize();
		// calculate differences in x direction
		for (int j = 1; j <= n; j++) {
			for (int i=1; i <= m - 1; i++) {
				const int k1 = get_gridmap(i, j);
				const int k2 = get_gridmap(i+1, j);
				if (k1 > 0 && k2 > 0) {
					dxsigma(i,j) = sigma[k2] - sigma[k1];
				}
			}
		}
		const double sigpen = double(m_ipar[42])*1.0e-9;
		for (int j=1; j <= n; j++) {
			for (int i=1; i <= m-2; i++) {
				dvariable d2xsigma = dxsigma[i+1][j] - dxsigma[i][j];
				penalty -= d2xsigma*d2xsigma*sigpen;
			}
		}
		dvar_matrix dysigma(1,m, 1,n);
		dysigma.initialize();
		// calculate differences in y direction
		for (int j = 1; j <= n - 1; j++) {
			for (int i = 1; i <= m; i++) {
				const int k3 = get_gridmap(i, j);
				const int k4 = get_gridmap(i, j + 1);
				if (k3 > 0 && k4 > 0) {
					dysigma(i,j) = sigma[k4] - sigma[k3];
				} 
			}
		}
		for (int j = 1; j <= n - 2; j++) {
			for (int i=1; i <= m; i++) {
				dvariable d2ysigma = dysigma[i][j + 1] - dysigma[i][j];
				penalty -= d2ysigma*d2ysigma*sigpen;
			}
		}
		if (fabs(value(penalty)) > 1.0) {
			cerr << "* * warning: sigma curvature penalty is getting large, " << value(penalty) << endl;
		}
	}
	return penalty;
}
