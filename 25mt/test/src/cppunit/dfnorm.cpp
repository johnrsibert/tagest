/**
 * $Id: dfnorm.cpp 2737 2010-08-05 22:14:06Z johnoel $
 */
#include <fvar.hpp>

void dfnorm(const dmatrix &x, dmatrix &dfx)
{
        double sum(0);
        for (int i = x.indexmin(); i <= x.indexmax(); i++) {
                const dvector &xi = x(i);
                const int jmin = xi.indexmin();
                const int jmax = xi.indexmax();
                for (int j = jmin; j <= jmax; j++) {
                        const double xij = xi(j);
                        sum += xij * xij;
                }
        }
        //double y = sqrt(sum);
                                                                                                                                                                                                    
        double dfsum = /*0.5*/pow(sum, -0.5);
        for (int i = x.indexmin(); i <= x.indexmax(); i++) {
                const dvector &xi = x(i);
                const int jmin = xi.indexmin();
                const int jmax = xi.indexmax();
                for (int j = jmin; j <= jmax; j++) {
                        const double xij = xi(j);
                        //sum += xij * xij;
                        dfx(i,j) += /*2.0*/dfsum * xij;
                }
        }
}
