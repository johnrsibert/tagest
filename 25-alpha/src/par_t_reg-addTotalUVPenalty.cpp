//$Id: par_t_reg-addTotalUVPenalty.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "par_t_reg.h"

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
dvariable par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::addTotalUVPenalty(dvar_matrix &uSeasonGrid, dvar_matrix &vSeasonGrid, const int season)
{
	dvariable penalty = 0;
	if (m_ipar[33] > 0) {
		const dvar_vector &useason = uSeasonGrid(season);
		const dvar_vector &vseason = vSeasonGrid(season);

		const double xpen = double(m_ipar[31])*1.0e-9;
		dvar_matrix dxu(1, m, 1, n);
		dvar_matrix dxv(1, m, 1, n);
		dxu.initialize();
		dxv.initialize();

		// calculate differences in x direction
		for (int j = 1; j <= n; j++) {
			for (int i = 1; i <= m - 1; i++) {
				const int k1 = get_gridmap(i, j);
				const int k2 = get_gridmap(i + 1, j);
				if (k1 > 0 && k2 > 0) {
					dxu(i, j) = (useason[k2] - useason[k1]) * 30.0;
					dxv(i, j) = (vseason[k2] - vseason[k1]) * 30.0;
				}
			}
		}
		for (int j = 1; j <= n; j++) {
			for (int i = 1; i <= m - 2; i++) {
				dvariable d2xu = dxu[i+1][j] - dxu[i][j];
				penalty -= d2xu*d2xu*xpen;

				dvariable d2xv = dxv[i+1][j] - dxv[i][j];
				penalty -= d2xv*d2xv*xpen;
			}
		}

		const double ypen = double(m_ipar[32])*1.0e-9;
		dvar_matrix dyu(1, m, 1, n);
		dvar_matrix dyv(1, m, 1, n);
		dyu.initialize();
		dyv.initialize();

		// calculate differences in y direction
		for (int j = 1; j <= n - 1; j++) {
			for (int i = 1; i <= m; i++) {
				const int k3 = get_gridmap(i, j);
				const int k4 = get_gridmap(i, j + 1);
				if (k3 > 0 && k4 > 0) {
					dyu(i,j) = (useason[k4] - useason[k3]) * 30.0;
					dyv(i,j) = (vseason[k4] - vseason[k3]) * 30.0;
				}
			}
		}
		for (int j = 1; j <= n - 2; j++) {
			for (int i = 1; i <= m; i++) {
				dvariable d2yu = dyu[i][j+1] - dyu[i][j];
				penalty -= d2yu*d2yu*ypen;

				dvariable d2yv = dyv[i][j+1] - dyv[i][j];
				penalty -= d2yv*d2yv*ypen;
			}
		}
	}
	return penalty;
}
