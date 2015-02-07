#include "precomp.h"

void reverse(double& like, dvector& g, int nvar)
{
  like *= -1;// Reverse the sign.
  for (int i=1; i <= nvar; i++)
      g[i] *= -1;
}
