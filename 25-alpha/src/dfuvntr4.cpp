//$Id: dfuvntr4.cpp 3156 2014-03-14 01:01:14Z eunjung $
#include "par_t_reg.h"

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void dfuvcomp(par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, par_t_reg<d3_array,dmatrix,dvector,double>& dfparam, dmatrix& dfu, dmatrix& dfv, int season);

extern ofstream clogf;

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void dfuvinterpolate(dmatrix& dfu, dmatrix& dfv, par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, par_t_reg<d3_array,dmatrix,dvector,double>& dfparam, year_month& date)
{
  //clogf << "  dfuvinterpolate for month = " << month << "; season = " << season << endl;
  int npoint = int(param.m_ipar[35]);
  if (npoint <= 1)
  {
    dfuvcomp(param, dfparam, dfu, dfv, param.get_season(date));
    return;
  }

  int _m = param.get_m();
  int _n = param.get_n();
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

  dmatrix dfuu(1,_m,1,_n);
  dmatrix dfvv(1,_m,1,_n);
  dfuu.initialize();
  dfvv.initialize();

  int i, j; //, k;

  bound boundary;

  imatrix& cell_type=param.get_cell_type();
  ivector& ilb = param.get_ilb();
  ivector& iub = param.get_iub();
  ivector& jlb = param.get_jlb();
  ivector& jub = param.get_jub();

  double dfysum = 0.0;

  for (i=_m; i >= 1; i--)
  {
    int jl = jlb(i);
    int ju = jub(i);
    int jadd = ju+np2; //jl+np2-2;
    int jsub = 2-np2+ju; //jl-np2;
    for (j = ju; j >= jl; j--)
    {
      //TTRACE(i,j)
      boundary = cell_type[i][j];
      jadd--;
      jsub--;

      if (jsub > jl)
      {
	     //ysum -= vv[i][jsub];
	     dfvv[i][jsub] -= dfysum;
      }

      //TTRACE(ilb(j),iub(j))
      if (i >= ilb(j) && i <= iub(j))
      {
//      v[i][j] = ysum * rnpoint;
	     dfysum += rnpoint*dfv[i][j];
	     dfv[i][j] = 0.0;
      }
      //TTRACE(jadd,ju)
      if (jadd <= ju)
      {
        //ysum += vv(i,jadd);
	     dfvv(i,jadd) += dfysum;
      }
    }
    //ysum = vv(i,jl);
    dfvv(i,jl) += dfysum;
    dfysum = 0;
  }
  

  double dfxsum = 0.0;
  for (j=_n; j >= 1; j--)
  {
    int il = ilb(j);
    int iu = iub(j);
    int iadd = np2+iu; //il+np2-2;
    int isub = 2-np2+iu; //il-np2;
    for (i=iu; i >= il; i--)
    {
      boundary = cell_type[i][j];
      iadd--;
      isub--;

      if (isub >= il)
      {
	     //xsum -= uu[isub][j];
	     dfuu(isub,j) -= dfxsum;
      	//clogf << "dfxsum: i = " << i <<", subtracted element " << i-np2+1
        //      << " = " << isub << endl;
      }

      if (j >= jlb(i) && j <= jub(i))
      {
     	  //u[i][j] = xsum * rnpoint;
        dfxsum += rnpoint*dfu[i][j];
        dfu[i][j] = 0.0;
      }
      if (iadd <= iu)
      {
	     //xsum += uu(iadd,j);
	     dfuu(iadd,j) += dfxsum;
	     //clogf << "dfxsum: i = " << i <<", added element " << i+np2-1
        //      << " = " << iadd << endl;
      }
    }
    //xsum = uu(il,j);
    dfuu(il,j) += dfxsum;
    dfxsum = 0.0;
  }

  // do moving average

//  uvcomp(param, uu, vv, season);
    dfuvcomp(param, dfparam, dfuu, dfvv, param.get_season(date));
// expand regions
}
template void dfuvinterpolate(dmatrix& dfu, dmatrix& dfv, par_t_reg<d3_array,dmatrix,dvector,double>& param, par_t_reg<d3_array,dmatrix,dvector,double>& dfparam, year_month& date);
template void dfuvinterpolate(dmatrix& dfu, dmatrix& dfv, par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param, par_t_reg<d3_array,dmatrix,dvector,double>& dfparam, year_month& date);
