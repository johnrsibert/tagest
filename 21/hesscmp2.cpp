#include "precomp.h"
#include "fish_rec.h"
#include <prnstrem.h>
#include "trace.h"

extern ofstream clogf;
ostream& setfixed(ostream& _s);
ostream& setscientific(ostream& _s);
int extra_nvarcalc(par_t& param);
void extra_fgcomp(dvector& x, dvector& g, int nvar, par_t& param);
int file_exists(const char* name);
void make_ext(const char c, const int n, char * ext);
int last_ext_number(const char* root, const char letter);
int mqrr_check(const adstring& name);


// estimate the matrix of second derivatives
//void function_minimizer::hess_routine()
//#pragma option -v
#ifdef __ZTC__
const char& adstring::operator()(const int i) const
{
  if (i < 1 || i > shape->size())
  {
    cerr << "Index out of bounds in adstring::operator () (const int)\n"
    << "Index value was " << i << " The size of this adstring is "
    << shape->size() << "\n";
    exit(1);
  }
  return (s[i]);
}
#endif

//void hess_comp(par_t& param, int nvar_model, int nvar_extra, dvector& x,
//      recaptype_vector& recaps,
//      int nrec, boundstype& bds, int npoint,
//      indexed_regional_fishery_record& irfr)
void hess_comp(par_t_reg& param, int nvar_model, dvector& x,
      recaptype_vector& recaps, int nrec, int npoint,
      indexed_regional_fishery_record& irfr)
{
  if ( (npoint != 2) ) //&& (npoint !=4) )
  {
    cerr << "One only does 2 or 4 point approximations to Hesssian" << endl;
    cerr << "You specified " << npoint << endl;
    exit(1);
  }

  char ext[5];
  int ext_no =  param.m_ipar[8];
  make_ext('h', ext_no, ext);

  adstring fullname = param.pathname.getPathRoot()+ext;
  if (file_exists(fullname))
    return;

  Upwind_coff_t coff(param, param.m_ipar[6]);
  coff.clean();
  Upwind_coff_t dfcoff(param, param.m_ipar[6]);
  dfcoff.clean();

  //TTRACE(nvar_model,nvar_extra);
  int nvar = nvar_model;//+nvar_extra;

  double f = 0;
  double delta=1.e-5;
  dvector g1(1,nvar);
  g1.initialize();
  dvector g2(1,nvar);
  g2.initialize();
  //dvector gbest(1,nvar);
  dvector hess(1,nvar);
  dvector hess1(1,nvar);
  dvector hess2(1,nvar);
  double eps=.1;

  //gbest.fill_seqadd(1.e+50,0.);
  ofstream ofs(fullname);
  ofs << "  " << nvar << endl;

   param.fgcomp(f, x, g1, nvar_model, recaps, nrec, coff, dfcoff, irfr);

  for (int i=1;i<=nvar;i++)
  {
      cout << "Estimating (" << npoint << " point) row " << i << " out of " << nvar
           << " for hessian";// << endl;

      double f=0.0;
      double xsave=x(i);

      if (npoint == 2)
      {
        x(i)=xsave+delta;
        cout << " upper";
        param.fgcomp(f, x, g2, nvar_model, recaps, nrec, coff, dfcoff, irfr);
        hess1=(g2-g1)/delta;

        x(i)=xsave+eps*delta;
        cout << ", lower";
        param.fgcomp(f, x, g2, nvar_model, recaps, nrec, coff, dfcoff, irfr);
        hess2=(g2-g1)/(eps*delta);

        x(i)=xsave;
         cout << endl;
        hess = (eps*hess1 - hess2)/(eps-1.0);
      }
      /*
      if (npoint == 4)
      {
        x(i)=xsave+delta;
        fgcomp(f, x, g1, nvar, param, recaps, nrec, bds, irfr);

        x(i)=xsave-delta;
        fgcomp(f, x, g2, nvar, param, recaps, nrec, bds, irfr);
        x(i)=xsave;

        hess1=(g1-g2)/(2.*delta);

        x(i)=xsave+eps*delta;
        fgcomp(f, x, g1, nvar, param, recaps, nrec, bds, irfr);

        x(i)=xsave-eps*delta;
        fgcomp(f, x, g2, nvar, param, recaps, nrec, bds, irfr);
        x(i)=xsave;

        double eps2=eps*eps;
        hess2=(g1-g2)/(2.*eps*delta);
        hess=(eps2*hess1-hess2) /(eps2-1.);
      }
      */
      ofs << setscientific << setprecision(13) << hess  << endl;
  }
}


// symmetrize and invert the hessian
void hess_inv(par_t& param, int nvar, dvector& x,
              dmatrix& hess, const adstring_array& x_names)
{
  char ext[5];
  int ext_no =  param.m_ipar[8];
  make_ext('h', ext_no, ext);

  adstring hess_name = param.pathname.getPathRoot()+ext;
  cout  << "Attempting to read hessian from " << hess_name << endl;
  clogf << "Attempting to read hessian from " << hess_name << endl;

  ifstream ifs(hess_name);
  if (!ifs)
  {
    cerr << "Unable to open file " << hess_name << endl;
    exit(1);
  }

  int file_nvar;
  ifs  >> file_nvar;
  cout << file_nvar << " variables in file" << endl;
  if (nvar !=file_nvar)
  {
    cerr << "Number of active variables in file " << hess_name
         << " is wrong" << endl;
    cerr << "Found " << file_nvar << "; should have " << nvar << endl;
    exit(1);
  }

  cout << "reading hessian" << endl;
  for (int i=1;i<=nvar;i++)
  {
    ifs >> hess(i);
    if (!ifs)
    {
      cerr << "Error reading line " << i  << " of the hessian"
           << " in routine hess_inv()" << endl;
      exit(1);
    }
  }

  cout << "symmetrizing  hessian" << endl;
  for (int i=1;i<=nvar;i++)
  {
    for (int j=1;j<i;j++)
    {
      double tmp=(hess(i,j)+hess(j,i))/2.;
      hess(i,j)=tmp;
      hess(j,i)=tmp;
    }
  }

  cout << "inverting hessian ... ";
  hess=inv(hess);
  cout << "done" << endl;

  cout << "computing eigenvalues ... ";
  dvector egval = eigenvalues(hess);
  cout << "done" << endl;
  TRACE(egval)

  int lb = egval.indexmin();
  int ub = egval.indexmax();
  TTRACE(lb,ub)
  TRACE(nvar)

  clogf << "\neigenvalues of inverse hessian" << endl;
  clogf << "number   eigenvalue      proportion" << endl;
  
  double ev_sum = sum(egval);
  double sum = 0.0;
  for (int i = ub; i >= lb; i--)
  {
    sum += egval(i);
    double prop = sum/ev_sum;
    clogf << setw(4) << (ub-i+lb) 
          << "  " << setscientific << setprecision(7) << egval(i) 
          << "      " << setfixed << setprecision(4) << prop << endl;
  }
}


int file_exists(const char* name)
{
  //ifstream f(name);
  ifstream f(name,ios::nocreate);
  if (f)
    return(1);
  else
    return(0);
}

int mqrr_check(const adstring& name)
{
   if ( (name(1) == 'M') || (name(1) == 'q') || (name(1) == 'r'))
      return(1);
   else
     return(0);
}
