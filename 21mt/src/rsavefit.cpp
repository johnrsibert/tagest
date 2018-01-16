#include "par_treg.h"
#include "trace.h"

adstring Model_area::par_file_version = "#v20r\0";

int a_move_corner(const int i, const int edge);
void make_ext(const char c, const int n, char * ext);
int last_ext_number(const char* root, const char letter);

const extern  double min_nb_par;
const extern  double max_nb_par;
extern ofstream clogf;

void par_t_reg::savefit(const double _like, const int _nvar, const double gmax,
                        const char* _ext)
{
  char fullname[20];
  strcpy(fullname, pathname.getRoot());
  if (!_ext)
  {
    char ext[5];
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
  par_t::savefit(pf, fullname);
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
       << "# k active             u                        v                     D\n";
    for (int k=1; k <= ngrid; k++)
    {
      pf << setw(3) << k
         << setw(7) << stratum_active[s][k]
         << setw(22) << setprecision(16)  << usergrid[s][k].u << " " 
         << setw(22) << setprecision(16)  << usergrid[s][k].v << " "
         << setw(23) << setprecision(16)  << usergrid[s][k].sigma
         << endl;
    }
  }

  if ( (m_ipar[11] == 4) || (m_ipar[11] == 5) || 
       (m_ipar[11] ==14) || (m_ipar[11] ==15) ||
       (m_ipar[11] ==24) || (m_ipar[11] ==25) )
  {
    pf << "#" << endl;
    pf << "# min_nb_par = " << min_nb_par << endl;
    pf << "# max_nb_par = " << max_nb_par << endl;
    pf << "# negative binomial parameters:" << endl;
    if ( (m_ipar[11] ==24) || (m_ipar[11] ==25) )
    {
       for (int f = 1; f <= nfleet; f++)
       {
          pf << "# " << (char*)fleet_names[f] << endl;
          pf  << setw(23) << setprecision(16)  << nb_par(f) << endl;
       }
    }
    else
      pf  << setw(23) << setprecision(16)  << nb_par << endl;
  }
 
  pf << "#\n# number of variables:\n" << _nvar << endl;
  pf << "# objective function value:\n" << setprecision(12) << _like <<endl;
  pf << "# maximum gradient magnitude:\n" << setprecision(12) << gmax <<endl;
  
  cout  << "Parameters saved to " << fullname << endl;
  clogf << "Parameters saved to " << fullname << endl;
}//End savefit
