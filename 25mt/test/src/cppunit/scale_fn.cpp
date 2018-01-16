/**
 * $Id: scale_fn.cpp 2737 2010-08-05 22:14:06Z johnoel $
 */
#include <cmath>

void myscale_fn(const double x, double &y, const double slope, const double offset, const double min, const double max) {
        const double mid = (max - min) * 0.5;
	const double arg = slope * (x - offset);
	y = min + mid * (1 + (2.0 / M_PI) * atan(arg));
}

void dfmyscale_fn(const double x, double &dfx, double &dfy, const double slope, const double offset, const double min, const double max) {
        const double mid = (max - min) * 0.5;
	const double arg = slope * (x - offset);

	double dfarg(0);
	//y = min + mid * (1 + (2.0 / M_PI) * atan(arg));
	dfarg += dfy * mid * (2.0 / M_PI) * (1.0 / (1.0 + arg * arg));
	dfy = 0;

	//const double arg = slope * (x - offset);
	dfx += dfarg * slope;
	dfarg = 0;
}
