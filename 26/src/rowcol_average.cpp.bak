//$Id: rowcol_average.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include <fvar.hpp>
extern ofstream clogf;

void rowcol_average(dvar_matrix& x, const int npoint)
{
	if (npoint <= 1) {
		return; //(x);
	}
	dvar_matrix y;
	y.allocate(x);
	y.initialize();

	const int np2 = npoint/2;
	if (2*np2 == npoint) {
		cerr << "The number of points in the moving average interpolation must be odd." << endl;
		cerr << "You have " << npoint << endl;
		clogf << "The number of points in the moving average interpolation must be odd." << endl;
		clogf << "You have " << npoint << endl;
		exit(1);
	}
	const int imin = x.rowmin();
	const int imax = x.rowmax();
	for (int i = imin; i <= imax; i++) {
		const int jmin = x(i).indexmin();
		const int jmax = x(i).indexmax();
		for (int j = jmin; j <= jmax; j++) {
			dvariable sum = 0;
			double den = 0;

			int kmin = j - np2;
			int kmax = j + np2;
			for (int k = kmin; k <= kmax; k++) {
				if (jmin <= k && k <= jmax) {
					sum += x(i,k);
					den += 1.0;
				}
			}
			kmin = i - np2;
			kmax = i + np2;
			for (int k = kmin; k <= kmax; k++) {
				if (imin <= k && k <= imax) {
					if (x(k).indexmin() <= j && j <= x(k).indexmax() && k != i) {
						sum += x(k,j);
						den += 1.0;
					}
				}
			}
			y(i,j) = den > 1 ? sum/den : x(i, j);
		}
	}
	x = y;
}
