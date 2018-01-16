#include "par_treg.h"
extern ofstream clogf;


void par_t_reg::uvinterpolate(dmatrix& u, dmatrix& v, year_month& ym)
{

  int season = get_season(ym);
  //int month = get_month(ym);

  int npoint = m_ipar[35];
  if (npoint <= 1)
  {
    uvcomp(u, v, season);
    //u = usergrid[1][1].u;
    return;
  }
  int _m = get_m();
  int _n = get_n();
  if((npoint > _m) || (npoint > _n))
  {
    cerr << "You can't do a " << npoint <<" moving average interpolation";
    cerr << "  on a " << _m << "x" << _n << " grid." << endl;
    clogf << "You can't do a " << npoint <<" moving average interpolation";
    clogf << "  on a " << _m << "x" << _n << " grid." << endl;
    exit(1);
  }

  int np2 = npoint/2;
  if (2*np2 == npoint)
  {
    cerr << "The number of points in the moving average interpolation must be odd."
	 << endl;
    cerr << "You have " << npoint << endl;
    clogf << "The number of points in the moving average interpolation must be odd."
	  << endl;
    clogf << "You have " << npoint << endl;
    exit(1);
  }
  np2 ++;

  double rnpoint  = 1.0/(double)npoint;

  dmatrix uu(1,_m,1,_n);
  dmatrix vv(1,_m,1,_n);

  int i, j;

  bound boundary;

  // expand regions
  uvcomp(uu, vv, season);

  imatrix& _cell_type=get_cell_type();
  ivector& _ilb = get_ilb();
  ivector& _iub = get_iub();
  ivector& _jlb = get_jlb();
  ivector& _jub = get_jub();

  // do moving average
  double xsum;

  for (j=1; j <= _n; j++)
  {
    int il = _ilb(j);
    int iu = _iub(j);
    int iadd = il+np2-2; //np2-1;
    int isub = il-np2; //1-np2;
    xsum = uu(il,j);
    for (i=il; i <= iu; i++)
    {
      boundary = _cell_type[i][j];
      iadd ++;
      isub ++;

      if (iadd <= iu)
      {
	      HERE
	      xsum += uu(iadd,j);
      }

      if (j >= _jlb(i) && j <= _jub(i))
      {
        u[i][j] = xsum * rnpoint;
      }
      if (isub >= il)
      {
      	xsum -= uu[isub][j];
      }
    }
  }

  double ysum;
  for (i=1; i <= _m; i++)
  {
    int jl = _jlb(i);
    int ju = _jub(i);
    int jadd = jl+np2-2; //np2-1;
    int jsub = jl-np2; //1-np2;
    ysum = vv(i,jl);
    for (j=jl; j <= ju; j++)
    {
      boundary = _cell_type[i][j];
      jadd ++;
      jsub ++;

      if (jadd <= ju)
      {
      	ysum += vv(i,jadd);
      }

      if (i >= _ilb(j) && i <= _iub(j))
      {
        v[i][j] =  ysum * rnpoint;
      }
      if (jsub > jl)
      {
	      ysum -= vv[i][jsub];
      }
    }
  }

  for (i=1; i<=_m; i++)
  {
    int j1 = _jlb(i);
    int jn = _jub(i);
    u[i][j1-1] = u[i][j1];
    u[i][jn+1] = u[i][jn];
    v[i][j1-1] = v[i][j1];
    v[i][jn+1] = v[i][jn];
  }
}

