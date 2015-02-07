//$Id: uvcomp.cpp 3163 2014-04-26 01:58:02Z eunjung $
#include "par_t_reg.h"
#include "trace.h"
#include "math.h"

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
int mynan(const dvariable & x)
{
   double y = value(x);
   return (isnan(y));
}

int mynan(const double &y)
{
   return (isnan(y));
}

void par_t_reg::uvcomp(dvar_matrix & u, dvar_matrix & v, 
                       const int season, const int t)
{
   dvar_vector & ugU = usergridU[season];
   dvar_vector & ugV = usergridV[season];
   //TTRACE(ugU,ugV)
   const double rdx = 30.0 / get_deltax_eq();
   const double rdy = 30.0 / get_deltay();
   for (int i = 1; i <= m; i++)
   {
      const int j1 = jlb(i);
      const int jn = jub(i);
      for (int j = j1; j <= jn; j++)
      {
	 if (m_ipar(83) == 1 && m_ipar(84)==1 
             && fadmap(t,i,j)!=0.0 && gx_fadmap(t,i,j)!=0.0 && gy_fadmap(t,i,j)!=0.0)
	 {
	    //const int grid =gridmap(i,j);
	    //if (1 <= grid && grid <= ngrid) {
	    const int k = gridmap(i, j);
	    //dvariable d_u = ugU[k]*rdx;
	    //dvariable d_v = ugV[k]*rdy;
	    if (gx_fadmap(t,i, j) < 0.0)
	    {
	        TRACE(t) TTRACE(i, j) TRACE(gx_fadmap(t,i, j))
            }
	    // u(i, j) = (ugU[k] + (mean_size / (100000 * 1.852)) * 86400 * sus_spd * gx_fadmap(i, j)) * rdx;
	    //v(i, j) = (ugV[k] + (mean_size / (100000 * 1.852)) * 86400 * sus_spd * gy_fadmap(i, j)) * rdy;
	    u(i, j) = (ugU[k] + (chi * gx_fadmap(t, i, j)/max_g)) * rdx;
	    v(i, j) = (ugV[k] + (chi * gy_fadmap(t, i, j)/max_g)) * rdy;
            //if(fabs(v(i,j))>9)
            //{
            //  v(i,j)=0;
            //} 
	    //u(i, j) = (chi * gx_fadmap(t, i, j)/max_g) * rdx;
	    //v(i, j) = (chi * gy_fadmap(t, i, j)/max_g) * rdy;
            if (mynan(chi))
	    {
	       cout << "i,j: " << i << "," << j << endl;
	       cout << "k: " << k << endl;
	       cout << "gx_fadmap(t,i,j): " << gx_fadmap(t, i, j) << endl;
	       cout << "gy_fadmap(t,i,j): " << gy_fadmap(t, i, j) << endl;
	       cout << "chi: " << chi << endl;
	       cout << "U: " << u(i, j) << endl;
	       cout << "V: " << v(i, j) << endl;
	       exit(1);
	    }
	    //}
	    //TTRACE(u,v)
	    //TTRACE(sus_spd,sus_spd)
	 } else
	 {
	    const int k = gridmap(i, j);
	    u(i, j) = ugU[k] * rdx;
	    v(i, j) = ugV[k] * rdy;
	 }
      }
   }
   //cout  << "u: " << u << endl;
   //cout  << "v: " << v << endl;
   //cout << "max(v):" << max(value(v)) << endl; 
   //cout << "min(v):" << min(value(v)) << endl;
}

void par_t_reg::uvcomp(dvar_matrix & uSeasonGrid, dvar_matrix & vSeasonGrid,
		    dvar_matrix & u, dvar_matrix & v, const int season)
{
   const double rdx = 30.0 / get_deltax_eq();
   const double rdy = 30.0 / get_deltay();
   for (int i = 1; i <= m; i++)
   {
      const int jmin = jlb(i);
      const int jmax = jub(i);
      for (int j = jmin; j <= jmax; j++)
      {
	 const int grid = gridmap(i, j);
	 if (1 <= grid && grid <= ngrid)
	 {
	    u(i, j) = uSeasonGrid(season, grid) * rdx;
	    v(i, j) = vSeasonGrid(season, grid) * rdy;
	 }
      }
   }
}
