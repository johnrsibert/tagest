//$Id: rsavefit.cpp 3129 2014-02-03 22:10:56Z eunjung $
#include "par_t_reg.h"
#include "trace.h"

int a_move_corner(const int i, const int edge);
void make_ext(const char c, const int n, char * ext);
int last_ext_number(const char* root, const char letter);

extern ofstream clogf;

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::savefit(const double _like, const int _nvar, const double gmax, const char* _ext)
{
  char fullname[80];
  strcpy(fullname, pathname.getRoot());
  if (!_ext)
  {
    char ext[10];
    int ext_no = last_ext_number(pathname.getRoot(), 'p') + 1;
    m_ipar[8] = ext_no;
    make_ext('p', ext_no, ext);
    strcat(fullname, ext);
  }
  else
  {
    strcat(fullname, _ext);
  }

  ofstream pf(fullname);
  if (!pf)
  {
    cerr << "Error: Cannot open file " << fullname
         << " in par_t_reg::savefit. " << endl;
    exit(1);
  }
  par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::savefit(pf, fullname);
  pf << "#\n#  REGIONAL PARAMETERS \n#" << endl;
  if (_nvar > 0)
  {
    pf << "# total velocity curvature penalty = "
       << setw(25) << setprecision(17) << total_uv_penalty
       << "\n# total sigma curvature penalty = "
       << setw(25) << setprecision(17) << total_sigma_penalty
       << "\n# total negative tags penalty = "
       << setw(25) << setprecision(17) << total_negative_tag_penalty
       << "\n# total total tags penalty = "
       << setw(25) << setprecision(17) << total_total_tag_penalty
       << "\n# total bounds penalty = "
       << setw(25) << setprecision(17) << total_bounds_penalty
    //pf << "# bounds penalty = " << param.likecont[0] << endl;
       << "\n#" << endl;
  }
  pf << "# ngrid   nseason\n"
     << "  " << ngrid << "  " << nseason << endl;

  pf << "#\n"
     << "#           seasonal user grid parameters:\n";
  for (int s=1; s <= nseason; s++)
  {
    pf << "#\n#  season " << s
       << "\n#  starting month\n"
       << setw(11) << startmonth[s]
       << "\n#\n"
//     << "# k             u                        v                sigma\n";
       << "# k active             u(Nmi/day)               v(Nmi/day)               D(Nmi^2/month)\n";
    for (int k=1; k <= ngrid; k++)
    {
      pf << setw(3) << k
         << setw(7) << stratum_active[s][k]
         << setw(22) << setprecision(16)  << usergridU[s][k] << " "
         << setw(22) << setprecision(16)  << usergridV[s][k] << " "
         << setw(23) << setprecision(16)  << usergridSigma[s][k]
         << endl;
    }
  }

  if (m_ipar(83) == 1)
  {
     if (m_ipar(84) == 1)
     {
       //pf  << "#\n# mean size of fish (cm)\n" 
       //    << setw(11) << mean_size << endl;
       pf  << "#\n# chi\n" 
           << setw(11) << chi << endl;
     }

     if(m_ipar(85) == 1)
     {
       pf << "#\n# region   active    min_D          log(slope)           inflection\n";
       //pf << "#\n# region   active    log(slope)           inflection\n";
       for (int g = 1; g <= ngrid; g++)
       {
            pf<< setw(8) << g
              << setw(7) << fadmap_stratum_active_d(g)
              << setw(15) << setprecision(5) << min_D(g) << " " 
              << setw(12) << setprecision(5) << logslope(g) << " "
              << setw(16) << setprecision(5) << inflection(g) 
              << endl;
       }
     }
     if(m_ipar(85) == 2)
     {
       pf  << "#\n# log(gamma)\n" 
           << setw(11) << loggamma << endl;
     }
  }

  pf << "#\n# number of variables:\n" << _nvar << endl;
  pf << "# objective function value:\n" << setprecision(12) << _like <<endl;
  pf << "# maximum gradient magnitude:\n" << setprecision(12) << gmax <<endl;

  cout  << "Parameters saved to " << fullname << endl;
  clogf << "Parameters saved to " << fullname << endl;
}
template void par_t_reg<d3_array,dmatrix,dvector,double>::savefit(const double _like, const int _nvar, const double gmax, const char* _ext);
template void par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>::savefit(const double _like, const int _nvar, const double gmax, const char* _ext);
