//$Id: reverse.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include <fvar.hpp>

template <typename DOUBLE>
void reverse(DOUBLE& like, dvector& g, const int nvar)
{
  like *= -1;// Reverse the sign.
  for (int i=1; i <= nvar; i++)
      g[i] *= -1;
}
template void reverse(double& like, dvector& g, const int nvar);
template void reverse(dvariable& like, dvector& g, const int nvar);
