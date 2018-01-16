#include "precomp.h"
#undef TRACE
#define TRACE(object) clogf << "line " << __LINE__ << ", file "\
      << __FILE__ << ", " << #object " = " << object << endl;

/* John: I think we should remove boundary references.  P. */

extern ofstream clogf;

void dfuvcomp(par_t& param, par_t_reg& dfparam, dmatrix& dfu, dmatrix& dfv,
              int season)
{
  //clogf << "    dfuvcomp for season = " << season << endl;

  gridpartype_vector dfug = dfparam.get_usergrid(season);

  double rdx = 30.0/param.get_deltax_eq();
  double rdy = 30.0/param.get_deltay();
  int _m = param.get_m();
  ivector& jlb = param.get_jlb();
  ivector& jub = param.get_jub();
  for (int i=_m; i>=1; i--)
  {
    int j1 = jlb(i);
    int jn = jub(i);
    for (int j=jn; j>=j1; j--)
    {
      int k = param.get_gridmap(i, j);

      //v[i][j] = ug[k].v*rdy;
      dfug[k].v += rdy*dfv[i][j];
      dfv[i][j] = 0.0;


      //u[i][j] = ug[k].u*rdx;
      dfug[k].u += rdx*dfu[i][j];
      dfu[i][j] = 0.0;
    }
  }
}
