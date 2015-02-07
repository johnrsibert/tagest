#include "par_treg.h"

extern ofstream clogf;

#undef TRACE
//#define TRACE
#define TRACE(object) clogf << "line " << __LINE__ << ", file "\
      << __FILE__ << ", " << #object " = " << object << endl;

// expands regional u and v movement parameters into arrays of u and v
// scales from miles per day to miles per month
// scales by dx and dy

 
/* John: the ADI scheme leads to a tri-diagonal set of equations BECAUSE the
 *       the boundary conditions allow the fictitious end values
 *       to be expressed in terms of interior values; i.e. there is no need
 *       to "set" boundary values here, either for closed or open boundaries
 *       ... they have already been set in the code in "atype", "btype" and
 *       "ctype"
 */


void par_t_reg::uvcomp(dmatrix& u, dmatrix& v, int season)
{
  //clogf << "    uvcomp for season = " << season << endl;
  gridpartype_vector& ug = usergrid[season];
  double rdx = 30.0/get_deltax_eq();
  double rdy = 30.0/get_deltay();
  for (int i=1; i <= m; i++)
  {
    int j1 = jlb(i);
    int jn = jub(i);
    for (int j = j1; j <= jn; j++)
    {
      int k = gridmap(i,j);
      u[i][j] = ug[k].u*rdx;
      v[i][j] = ug[k].v*rdy;
    }//forj
  }//fori
}//uvcomp(dmatrix&,dmatrix&,int)
