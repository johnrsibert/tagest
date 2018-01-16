#include "par_treg.h"

extern ofstream clogf;

#define TRACE(object) clogf << "line " << __LINE__ << ", file "\
      << __FILE__ << ", " << #object " = " << object << endl;
 
void par_t_reg::sigma_comp(dmatrix& sigma, year_month& ym)
{
  int season = get_season(ym);

  //clogf << "sigma_comp for " << ym << "; season = " << season << endl;
  gridpartype_vector& ug = usergrid[season];

  for (int i=1; i<=m; i++)
  {
    int j1 = jlb(i);
    int jn = jub(i);
    for (int j=j1; j<=jn; j++)
    {
      int k = gridmap[i][j];
      sigma[i][j] = ug[k].sigma;
    }//forj
//    sigma[i][j1-1] = sigma[i][j1];    // <-- and
//    sigma[i][jn+1] = sigma[i][jn];    // <--
  }//fori
}//sigma_comp(dmatrix&,year_month&)
